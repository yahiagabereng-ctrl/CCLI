#include "event/event_store.hpp"
#include "event/o14_catalog.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <utility>

namespace {

bool write_file(const char* path, const char* body) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return false;
    }
    out << body;
    return out.good();
}

}  // namespace

int main() {
    const std::string path = "ccli-event-store-unit.jsonl";
    const std::string marker = cci::core::firmware_marker_path(path);
    std::remove(path.c_str());
    std::remove((path + ".1").c_str());
    std::remove(marker.c_str());

    if (cci::core::o14::category_for("mms", "wlim_on") !=
        std::string(cci::core::o14::kDsoCommand)) {
        std::fprintf(stderr, "category wlim failed\n");
        return 1;
    }
    if (cci::core::o14::category_for("mms", "auth_reject_mechanism") !=
        std::string(cci::core::o14::kAuth)) {
        std::fprintf(stderr, "category auth failed\n");
        return 1;
    }

    if (!cci::core::EventStore::run_wrap_test(path)) {
        std::fprintf(stderr, "wrap test failed\n");
        std::remove(path.c_str());
        return 1;
    }
    std::remove(path.c_str());
    std::remove((path + ".1").c_str());

    cci::core::EventLogConfig isolated;
    isolated.path = path;
    isolated.platform_audit_enabled = false;
    cci::core::EventStore store(isolated);
    store.append({1, "mms", "client_connect", ""});
    if (store.size() != 1) {
        std::fprintf(stderr, "size after append failed\n");
        return 1;
    }
    const auto rows = store.recent(1);
    if (rows.empty() || rows[0].category != cci::core::o14::kCommsDso) {
        std::fprintf(stderr, "category fill failed got=%s\n",
                     rows.empty() ? "(empty)" : rows[0].category.c_str());
        return 1;
    }

    const auto ts = cci::core::format_o14_timestamp(0);
    if (ts.size() != 19 || ts[4] != '/' || ts[7] != '/' || ts[10] != ' ') {
        std::fprintf(stderr, "timestamp format failed: %s\n", ts.c_str());
        return 1;
    }

    cci::core::EventRecord ev{1000, "mms", "auth_reject_empty_cert",
                              cci::core::o14::kAuth};
    const auto rfc = cci::core::format_rfc5424(ev, "dut", "ccli");
    if (rfc.find("<105>") != 0 || rfc.find("COMM_PEER_AUTHENTICATION_FAIL") == std::string::npos) {
        std::fprintf(stderr, "rfc5424 failed: %s\n", rfc.c_str());
        return 1;
    }

    for (const auto& level : {std::pair<const char*, int>{"alarm", 105}, {"warning", 108}, {"notice", 109}, {"error", 107}}) {
        const std::string detail = std::string("{\"rbac\":true,\"severity\":\"") + level.first + "\",\"event\":\"ACCESS_DECISION\"}";
        cci::core::EventRecord event{1000, "mms", detail, cci::core::o14::kSecurity};
        event.iec_id = std::string(level.first) == "alarm" ? "IEC62351-3:2.4" :
            std::string(level.first) == "warning" ? "IEC62351-3:2.6" :
            std::string(level.first) == "error" ? "IEC62351-14:4.2" : "IEC62351-3:1.1";
        if (cci::core::o14::category_for("mms", detail) != std::string(cci::core::o14::kSecurity) ||
            cci::core::format_rfc5424(event, "dut", "ccli").find("<" + std::to_string(level.second) + ">") != 0) {
            std::fprintf(stderr, "RBAC security category/severity failed\n");
            return 1;
        }
    }

    auto first = cci::core::EventStore::record_firmware_boot(path, "0.1.0-r35");
    if (first.kind != cci::core::FirmwareBootKind::First) {
        std::fprintf(stderr, "firmware first boot failed\n");
        return 1;
    }
    auto same = cci::core::EventStore::record_firmware_boot(path, "0.1.0-r35");
    if (same.kind != cci::core::FirmwareBootKind::Unchanged) {
        std::fprintf(stderr, "firmware unchanged failed\n");
        return 1;
    }
    auto upd = cci::core::EventStore::record_firmware_boot(path, "0.1.0-r36");
    if (upd.kind != cci::core::FirmwareBootKind::Updated ||
        upd.detail.find("0.1.0-r35->0.1.0-r36") == std::string::npos) {
        std::fprintf(stderr, "firmware update failed: %s\n", upd.detail.c_str());
        return 1;
    }

    if (!cci::core::EventStore::user_overwrite_forbidden()) {
        std::fprintf(stderr, "overwrite flag failed\n");
        return 1;
    }

    (void)write_file;
    {
        cci::core::EventLogConfig reader_cfg;
        reader_cfg.path = path;
        // Deliberately leave producer configuration enabled: read-only must
        // override it, including on the actual OpenWrt target.
        reader_cfg.platform_audit_enabled = true;
        reader_cfg.syslog_enabled = true;
        cci::core::EventStore reader(reader_cfg, true);
        const auto count = reader.size();
        if (reader.append({9999, "test", "must_not_write", ""}) || reader.size() != count)
            return 2;
        std::ostringstream output;
        if (cci::core::EventStore::dump_to_stream(output, path, 1, true) != 0 ||
            output.str().find("client_connect") == std::string::npos) return 3;
        std::ostringstream failed;
        failed.setstate(std::ios::badbit);
        if (cci::core::EventStore::dump_to_stream(failed, path, 1, true) == 0) return 4;
        if (std::filesystem::exists(path + ".platform-auth-audit") ||
            std::filesystem::exists(path + ".firmware-updater.cursor")) return 5;
    }
    std::remove(path.c_str());
    std::remove((path + ".1").c_str());
    std::remove(marker.c_str());
    return 0;
}
