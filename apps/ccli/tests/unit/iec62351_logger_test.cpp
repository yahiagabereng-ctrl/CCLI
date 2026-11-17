#include "event/event_store.hpp"
#include "event/iec62351_event.hpp"
#include "event/syslog_sender.hpp"
#include "config/ccli_config.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <thread>

using namespace cci::core;
namespace fs = std::filesystem;
void require(bool value, const char* reason) { if (!value) throw std::runtime_error(reason); }

int main() {
    const auto dir = fs::temp_directory_path() / ("ccli-62351-14-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(dir);
    try {
        EventRecord tls{1001, "iec104", "TLS_SECURITY_EVENT severity=notice code=16 Established peer=192.0.2.2:6514"};
        tls.sequence = 42;
        const auto message = format_rfc5424(tls, "cci-01", "ccli");
        require(message == "<109>1 1970-01-01T00:00:01.001Z cci-01 ccli - IEC62351-14:1 "
            "[62351-14@41912 ID=\"IEC62351-3:1.1\" MNEMONIC=\"TLS_HS_SUCCESS\" "
            "Text=\"TLS session successfully established\" SqNum=\"42\" PeerInfo=\"192.0.2.2:6514\"]",
            "mandatory wire fields or UTC timestamp incorrect");
        require(frame_rfc5425(message) == std::to_string(message.size()) + " " + message, "RFC 5425 framing incorrect");
        EventRecord rbac{2000, "mms", R"({"rbac":true,"event":"ASSOCIATION_AUTH_OK","severity":"notice","subject":"CN=user","peer":"192.0.2.5:123","roles":[{"id":-1,"revision":1,"definition":"IEC62351-8"}]})"};
        require(prepare_iec_event(rbac, "cci", "mms") && rbac.role == "-1:1:IEC62351-8" &&
            rbac.user_id == "CN=user" && rbac.peer_info == "192.0.2.5:123", "conditional identity fields missing");
        EventRecord unicode{2000, "mms", R"({"event":"ASSOCIATION_AUTH_OK","subject":"\u00e8\ud83d\ude00"})"};
        require(prepare_iec_event(unicode, "cci", "mms") && unicode.user_id == "\xc3\xa8\xf0\x9f\x98\x80", "Unicode identity decoding failed");
        EventRecord json_tls{2000, "mms", R"({"event":"TLS_SECURITY_EVENT","object":"16","reason":"TLS established peer=192.0.2.1:6514"})"};
        require(prepare_iec_event(json_tls, "cci", "mms") && json_tls.peer_info == "192.0.2.1:6514", "JSON TLS peer extraction failed");
        rbac.parameters = {{"P.Name", "a\"]\\b"}, {"P.UTF8", "\xc3\xa8"}};
        const auto escaped = format_rfc5424(rbac, "cci", "mms");
        require(escaped.find("P.Name=\"a\\\"\\]\\\\b\"") != std::string::npos, "SD escaping permits injection");
        auto bad = rbac;
        bad.parameters.emplace_back("P.Name", "duplicate");
        require(format_rfc5424(bad, "cci", "mms").empty(), "duplicate SD parameters accepted");
        bad = rbac; bad.parameters = {{"P.Test", std::string("\xc0\xaf")}};
        require(format_rfc5424(bad, "cci", "mms").empty(), "invalid UTF-8 accepted");
        bad = rbac; bad.source = "cci\ninjected";
        require(format_rfc5424(bad, "cci", "mms").empty(), "header injection accepted");
        bad = rbac; bad.iec_id = "IEC62351-3:99.99";
        require(format_rfc5424(bad, "cci", "mms").empty(), "invented IEC ID accepted");
        require(format_rfc5424({0, "meter", "quality_stale"}, "cci", "ccli").empty(), "process data mislabeled as IEC security event");
        require(format_rfc5424({0, "iec104", "TLS_SECURITY_EVENT severity=alarm code=1004 revoked"}, "cci", "ccli").find("ID=\"IEC62351-3:1.8\"") != std::string::npos,
                "revoked session closure mapping incorrect");
        EventLogConfig cfg;
        cfg.path = (dir / "events.jsonl").string(); cfg.syslog_source = "cci";
        {
            EventStore store(cfg);
            require(store.append(rbac), "IEC append failed");
            require(store.recent(1)[0].sequence == 1, "sequence must start at one");
        }
        {
            EventStore reloaded(cfg);
            const auto restored = reloaded.recent(1)[0];
            require(restored.iec_id == "IEC62351-14:1.1" && restored.parameters == rbac.parameters &&
                restored.user_id == rbac.user_id && restored.sequence == 1, "IEC metadata lost across restart");
            require(reloaded.append(tls) && reloaded.recent(1)[0].sequence == 2, "sequence restarted or accepted caller sequence");
        }
        // Sequence rollover is defined, including across a device restart.
        {
            std::ofstream out(cfg.path);
            out << "{\"ts_ms\":1,\"type\":\"seed\",\"sequence\":4294967295}\n";
        }
        {
            EventStore store(cfg);
            require(store.append(tls) && store.recent(1)[0].sequence == 1, "sequence did not wrap to one");
        }
        cfg.path = (dir / "threads.jsonl").string();
        {
            EventStore store(cfg);
            std::thread a([&] { for (int i=0;i<20;++i) require(store.append(tls), "writer a failed"); });
            std::thread b([&] { for (int i=0;i<20;++i) require(store.append(tls), "writer b failed"); });
            a.join(); b.join();
            const auto records = store.recent(40);
            for (std::size_t i=0;i<records.size();++i) require(records[i].sequence == i + 1, "concurrent sequence order incorrect");
        }
        cfg.syslog_enabled = true;
        bool rejected = false;
        try { EventStore missing_credentials(cfg); } catch (const std::invalid_argument&) { rejected = true; }
        require(rejected, "incomplete TLS settings accepted");
        cfg.syslog_enabled = false;
        {
            EventStore store(cfg);
            bad.iec_id = "IEC62351-3:99.99";
            require(!store.append(bad) && store.recent(1)[0].iec_id.empty(), "malformed explicit metadata masquerades as an IEC record");
        }
        const auto yaml = dir / "logger.yaml";
        {
            std::ofstream out(yaml);
            out << "event_log:\n  syslog_enabled: true\n  syslog_port: 6514\n  syslog_tls_ca: ca.pem\n"
                   "  platform_audit_enabled: false\n"
                   "  syslog_tls_cert: cert.pem\n  syslog_tls_key: key.pem\n  syslog_tls_crl: ca.crl\n"
                   "  syslog_tls_peer_sha256: ABCD\n  syslog_source: cci\n  syslog_app_name: security\n"
                   "  syslog_revocation_check_s: 120\n  syslog_renegotiation_s: 600\n  syslog_resumption_s: 60\n";
        }
        const auto parsed = load_config(yaml.string());
        require(parsed.event_log.syslog_tls_ca == "ca.pem" && parsed.event_log.syslog_tls_crl == "ca.crl" &&
            parsed.event_log.syslog_tls_peer_sha256 == "ABCD" && parsed.event_log.syslog_tls_key == "key.pem" &&
            !parsed.event_log.platform_audit_enabled &&
            parsed.event_log.syslog_revocation_check_s == 120 && parsed.event_log.syslog_renegotiation_s == 600 &&
            parsed.event_log.syslog_resumption_s == 60 && parsed.event_log.syslog_app_name == "security", "TLS syslog YAML not loaded");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n'; fs::remove_all(dir); return 1;
    }
    fs::remove_all(dir);
    return 0;
}
