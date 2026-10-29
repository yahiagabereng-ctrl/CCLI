#include "event/iec62351_event.hpp"
#include "event/event_store.hpp"
#include "event/platform_event_monitor.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <unistd.h>
#include <iostream>
#include <string>

namespace {
void check(bool ok, const char* what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << '\n';
        std::exit(1);
    }
}
}

int main() {
    using namespace cci::core;
    constexpr std::int64_t time = 1793200800000LL;

    const auto ssh_ok = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:00 2026 authpriv.info dropbear[123]: Password auth succeeded for 'root' from 192.168.0.112:53000",
        time);
    check(ssh_ok.has_value(), "recognize Dropbear successful login");
    check(ssh_ok->iec_id == "IEC62351-14:1.1" && ssh_ok->user_id == "root" &&
          ssh_ok->peer_info == "192.168.0.112:53000" && ssh_ok->app_name == "dropbear",
          "preserve SSH actor and peer fields");

    const auto ssh_fail = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:01 2026 authpriv.info dropbear[123]: Password auth failed for 'operator' from 192.168.0.112:53000; supplied=never-store-this",
        time + 1);
    check(ssh_fail && ssh_fail->iec_id == "IEC62351-14:1.3" && ssh_fail->user_id == "operator",
          "recognize Dropbear authentication failure");
    check(ssh_fail->detail.find("never-store-this") == std::string::npos,
          "never retain arbitrary syslog contents or password-like material");

    const auto ssh_logout = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:02 2026 authpriv.info dropbear[123]: Exit (root) from 192.168.0.112:53000: Exited normally",
        time + 2);
    check(ssh_logout && ssh_logout->iec_id == "IEC62351-14:1.8" && ssh_logout->user_id == "root",
          "recognize authenticated SSH logout");
    check(!parse_platform_auth_log_line(
        "Wed Oct 28 10:00:03 2026 authpriv.info dropbear[123]: Exit before auth from 192.168.0.112:53000",
        time + 3), "do not report unauthenticated socket closure as logout");

    const auto web_ok = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:04 2026 daemon.info rpcd[42]: user 'operator' authenticated successfully from 192.168.0.112",
        time + 4);
    check(web_ok && web_ok->iec_id == "IEC62351-14:1.1" && web_ok->parameters.size() == 1 &&
          web_ok->parameters.front().second == "web", "recognize explicit rpcd web login success");
    const auto web_fail = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:05 2026 daemon.info rpcd[42]: user 'operator' authentication failed from 192.168.0.112",
        time + 5);
    check(web_fail && web_fail->iec_id == "IEC62351-14:1.3", "recognize explicit rpcd web login failure");
    const auto luci_ok = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:06 2026 authpriv.info dispatcher.uc: luci: accepted login on /admin for root from 192.168.0.112",
        time + 6);
    check(luci_ok && luci_ok->iec_id == "IEC62351-14:1.1" && luci_ok->user_id == "root" &&
          luci_ok->peer_info == "192.168.0.112", "recognize LuCI's explicit accepted-login record");
    const auto luci_fail = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:07 2026 authpriv.warning dispatcher.uc: luci: failed login on /admin for root from 192.168.0.112",
        time + 7);
    check(luci_fail && luci_fail->iec_id == "IEC62351-14:1.3" && luci_fail->user_id == "root",
          "recognize LuCI's explicit failed-login record");
    check(!parse_platform_auth_log_line(
        "Wed Oct 28 10:00:06 2026 daemon.info uhttpd[45]: GET /admin/index.html 403",
        time + 6), "do not mistake an ordinary HTTP denial for a login event");
    const auto threshold = parse_platform_auth_log_line(
        "Wed Oct 28 10:00:08 2026 authpriv.info dropbear[123]: Exit before auth from 192.168.0.112: Max auth tries reached",
        time + 8);
    check(threshold && threshold->iec_id == "IEC62351-14:1.5", "recognize platform-reported login threshold");

    PlatformAccountSnapshot before;
    before["root"] = {"credential-a", "0:0:/bin/ash:g=wheel", false};
    before["olduser"] = {"credential-b", "1001:1001:/bin/ash", false};
    before["operator"] = {"credential-c", "1002:1002:/bin/ash", false};
    PlatformAccountSnapshot after;
    after["root"] = {"credential-a", "0:0:/bin/ash:g=wheel", false};
    after["operator"] = {"credential-new", "1002:1002:/bin/ash:g=wheel", false};
    after["newuser"] = {"credential-d", "1003:1003:/bin/ash", false};
    const auto changes = diff_platform_account_snapshots(before, after, time + 8);
    check(changes.size() == 4, "emit creation, deletion, password, and account/group-change records");
    bool created = false, deleted = false, password = false, attributes = false;
    for (auto event : changes) {
        check(prepare_iec_event(event, "localhost", event.app_name), "account event uses a valid catalogue definition");
        if (event.iec_id == "IEC62351-14:2.1" && event.user_id == "newuser") created = true;
        if (event.iec_id == "IEC62351-14:2.7" && event.user_id == "olduser") deleted = true;
        if (event.iec_id == "IEC62351-14:2.9" && event.user_id == "operator") password = true;
        if (event.iec_id == "IEC62351-14:6.10" && event.user_id == "operator") attributes = true;
    }
    check(created && deleted && password && attributes, "account lifecycle event IDs and target account mapping");

    PlatformAccountSnapshot locked = after;
    locked["operator"].credential_fingerprint = "credential-locked";
    locked["operator"].credential_locked = true;
    const auto lock_events = diff_platform_account_snapshots(after, locked, time + 9);
    check(lock_events.size() == 1 && lock_events.front().iec_id == "IEC62351-14:2.5",
          "map locked account state to account deactivation event");
    const auto read_ok = parse_firmware_updater_audit_line("1793200800|LOG_READ_SUCCESS|uid=0");
    const auto read_fail = parse_firmware_updater_audit_line("1793200800|LOG_READ_FAIL|uid=0");
    check(read_ok && read_ok->iec_id == "IEC62351-14:9.1" && read_ok->app_name == "event-reader",
          "map durable log-view success to IEC event");
    check(read_fail && read_fail->iec_id == "IEC62351-14:9.2", "map log-view failure");
    const auto test_dir = std::filesystem::temp_directory_path() /
        ("ccli-audit-import-" + std::to_string(getpid()));
    std::filesystem::create_directory(test_dir);
    const auto audit = (test_dir / "audit").string();
    const auto cursor = (test_dir / "cursor").string();
    struct Sink { bool fail{false}; std::vector<EventRecord> records; } sink;
    const auto save = +[](void* context, EventRecord event) -> bool {
        auto& out = *static_cast<Sink*>(context);
        if (out.fail) return false;
        out.records.push_back(std::move(event));
        return true;
    };
    { std::ofstream out(audit); out << "1793200800|UPDATE_SUCCEEDED|transaction=test\n"
                                   << "1793200801|LOG_READ_SUCCESS|uid=0"; }
    {
        FirmwareUpdaterAuditImporter importer(audit, cursor);
        importer.poll(&sink, save);
        check(sink.records.size() == 1, "defer partial audit row");
        importer.poll(&sink, save);
        check(sink.records.size() == 1, "cursor prevents repeat imports");
    }
    { std::ofstream out(audit, std::ios::app); out << "\n"; }
    {
        FirmwareUpdaterAuditImporter importer(audit, cursor);
        sink.fail = true;
        importer.poll(&sink, save);
        check(sink.records.size() == 1, "failed append retains cursor");
        sink.fail = false;
        importer.poll(&sink, save);
        check(sink.records.size() == 2 && sink.records.back().iec_id == "IEC62351-14:9.1",
              "retry persisted read request after importer restart");
        { std::ofstream out(audit, std::ios::app);
          out << std::string(70000, 'X') << "\n1793200802|UPDATE_FAILED|transaction=test\n"; }
        importer.poll(&sink, save);
        importer.poll(&sink, save);
        check(sink.records.size() == 3 && sink.records.back().iec_id == "IEC62351-14:6.2",
              "oversized row does not block following valid event");
    }
    const auto log_path = (test_dir / "events.jsonl").string();
    check(EventStore::audit_log_read(log_path, true), "persist log-view audit sidecar");
    check(EventStore::audit_log_read(log_path, false), "persist log-view failure sidecar");
    FirmwareUpdaterAuditImporter read_import(log_path + ".read-audit", log_path + ".read-cursor");
    sink.records.clear();
    read_import.poll(&sink, save, true);
    check(sink.records.size() == 2 && sink.records[0].iec_id == "IEC62351-14:9.1" &&
          sink.records[1].iec_id == "IEC62351-14:9.2", "read audit writer-to-importer integration");
    { std::ofstream out(log_path + ".read-audit", std::ios::app);
      out << std::string(70000, 'X') << "\n"; }
    read_import.poll(&sink, save, true);
    check(sink.records.size() == 2, "compact fully imported bounded read-audit sidecar");
    check(EventStore::audit_log_read(log_path, true), "append after sidecar compaction");
    read_import.poll(&sink, save, true);
    check(sink.records.size() == 3 && sink.records.back().iec_id == "IEC62351-14:9.1",
          "import read event after compaction without loss");
    check(ssh_fail && EventStore::audit_platform_auth(log_path, *ssh_fail),
          "persist recognized platform authentication event before EventStore import");
    const auto auth_sidecar = log_path + ".platform-auth-audit";
    std::string auth_contents;
    { std::ifstream in(auth_sidecar); auth_contents.assign(std::istreambuf_iterator<char>(in), {}); }
    check(auth_contents.find("never-store-this") == std::string::npos,
          "platform auth spool contains only normalized fields, not raw syslog text");
    FirmwareUpdaterAuditImporter auth_import(auth_sidecar, auth_sidecar + ".cursor");
    auth_import.poll(&sink, save, true);
    check(sink.records.size() == 4 && sink.records.back().iec_id == "IEC62351-14:1.3" &&
          sink.records.back().user_id == "operator" && sink.records.back().peer_info == "192.168.0.112:53000",
          "replay durable management authentication event with actor and peer fields");
    const auto guarded_audit = (test_dir / "guarded-audit").string();
    const auto guarded_cursor = (test_dir / "guarded-cursor").string();
    const auto cursor_target = (test_dir / "cursor-target").string();
    { std::ofstream out(guarded_audit); out << "1793200803|LOG_READ_SUCCESS|uid=0\n"; }
    { std::ofstream out(cursor_target); out << "untouched\n"; }
    std::filesystem::create_symlink(cursor_target, guarded_cursor);
    FirmwareUpdaterAuditImporter guarded_import(guarded_audit, guarded_cursor);
    const auto before_guarded = sink.records.size();
    guarded_import.poll(&sink, save, true);
    std::string target_contents;
    { std::ifstream in(cursor_target); std::getline(in, target_contents); }
    check(sink.records.size() == before_guarded && target_contents == "untouched" &&
          std::filesystem::is_symlink(guarded_cursor),
          "reject symlinked importer cursor without following or replacing it");
    std::filesystem::remove_all(test_dir);
    std::cout << "platform authentication and account event tests passed\n";
    return 0;
}
