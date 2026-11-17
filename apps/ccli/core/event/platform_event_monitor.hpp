#pragma once

#include "event/event_ring.hpp"

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cci::core {

/** Parse only recognized platform authentication records. The original line is
 * never copied into the returned event. */
std::optional<EventRecord> parse_platform_auth_log_line(const std::string& line,
                                                        std::int64_t timestamp_ms);

/** In-memory OS-account snapshot. credential_fingerprint is never persisted or
 * emitted; it exists only to detect a shadow credential change between scans. */
struct PlatformAccountState {
    std::string credential_fingerprint;
    std::string attributes;
    bool credential_locked{false};
};
using PlatformAccountSnapshot = std::map<std::string, PlatformAccountState>;

std::vector<EventRecord> diff_platform_account_snapshots(
    const PlatformAccountSnapshot& previous, const PlatformAccountSnapshot& current,
    std::int64_t timestamp_ms);

/** Parse the updater's persistent `epoch|EVENT|detail` audit format.
 * Unknown updater event names remain local O.14 records without a claimed IEC ID. */
std::optional<EventRecord> parse_firmware_updater_audit_line(const std::string& line);

/** Durable, at-least-once importer for firmware-updater's fsynced audit log. */
class FirmwareUpdaterAuditImporter {
public:
    using Append = bool (*)(void*, EventRecord);
    FirmwareUpdaterAuditImporter(std::string audit_path, std::string cursor_path);
    ~FirmwareUpdaterAuditImporter();
    FirmwareUpdaterAuditImporter(const FirmwareUpdaterAuditImporter&) = delete;
    FirmwareUpdaterAuditImporter& operator=(const FirmwareUpdaterAuditImporter&) = delete;
    void poll(void* context, Append append, bool compact_consumed = false);
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

class PlatformEventMonitor {
public:
    using Append = bool (*)(void*, EventRecord);

    PlatformEventMonitor(void* context, Append append, std::string updater_audit_path,
                         std::string updater_cursor_path,
                         std::vector<std::string> credential_paths);
    ~PlatformEventMonitor();
    PlatformEventMonitor(const PlatformEventMonitor&) = delete;
    PlatformEventMonitor& operator=(const PlatformEventMonitor&) = delete;

private:
    struct Impl;
    Impl* impl_;
};

/** True only on the supported OpenWrt platform. */
bool platform_event_monitor_available();

} // namespace cci::core
