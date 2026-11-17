#pragma once

#include "event/event_ring.hpp"

#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <mutex>
#include <memory>
#include <string>
#include <vector>

namespace cci::core {
class SyslogSender;
class PlatformEventMonitor;

/** O.14 rolling event log configuration (P7-01 / P7-03). */
struct EventLogConfig {
    bool        enabled{true};
    std::string path{"/var/lib/ccli/events.jsonl"};
    bool        syslog_enabled{false};
    std::string syslog_host{"127.0.0.1"};
    int         syslog_port{6514};
    std::string syslog_source;
    std::string syslog_app_name{"ccli"};
    std::string syslog_tls_ca;
    std::string syslog_tls_cert;
    std::string syslog_tls_key;
    std::string syslog_tls_crl;
    /** RFC 5425 end-entity authorization: SHA-256 of the collector DER leaf. */
    std::string syslog_tls_peer_sha256;
    /** Import OpenWrt management authentication and account-database events. */
    bool        platform_audit_enabled{true};
    /** Canonical firmware-updater persistent audit path on TesPro/OpenWrt. */
    std::string platform_updater_audit_path{
        "/mnt/mmcblk0/firmware-updater/security-audit.log"};
    /** Files whose replacement should be recorded as a configuration change. */
    std::vector<std::string> platform_credential_paths;
    int syslog_revocation_check_s{3600};
    int syslog_renegotiation_s{43200};
    int syslog_resumption_s{3600};
};

enum class FirmwareBootKind { First, Unchanged, Updated };

struct FirmwareBootNote {
    FirmwareBootKind kind{FirmwareBootKind::First};
    std::string      detail;
};

/** Latest 2048 events, persisted in two append-only rotating segments.
 * Each segment holds at most 2048 lines; reads expose the latest 2048 events
 * across path + path.1. Rotation never rewrites retained records.
 */
class EventStore {
public:
    static constexpr std::size_t kMaxEvents = EventRing::kMaxEvents;

    /** Read-only instances load history without starting producers or changing files. */
    explicit EventStore(EventLogConfig cfg = {}, bool read_only = false);
    ~EventStore();

    /** False on persistence failure or malformed explicit IEC metadata.
     * Raw input remains locally available; remote delivery is asynchronous. */
    bool append(EventRecord ev);
    static bool audit_log_read(const std::string& path, bool success);
    /** Durable spool for recognized platform authentication events. */
    static bool audit_platform_auth(const std::string& path, const EventRecord& event);
    bool persistence_ok() const;
    /** Latched false after a forwarding fault; local append success is separate. */
    bool syslog_ok() const;
    std::vector<EventRecord> recent(std::size_t max_count) const;
    std::size_t size() const;

    EventRing& ring() { return ring_; }
    const EventRing& ring() const { return ring_; }

    const EventLogConfig& config() const { return cfg_; }

    /** P7-01 wrap test: append kMaxEvents+1 synthetic rows; expect size()==kMaxEvents. */
    static bool run_wrap_test(const std::string& path);

    /** P7-02 dump: O.14 timestamp format yyyy/mm/dd hh:mm:ss (UTC). Read-only. */
    static int dump_to_stream(std::ostream& out, const std::string& path, std::size_t max_count,
                              bool json_out);

    /**
     * P7-01: user overwrite is forbidden. Always returns false.
     * CLI `--event-clear` maps here.
     */
    static bool user_overwrite_forbidden() { return true; }

    /** Compare last boot version sidecar next to the jsonl path. */
    static FirmwareBootNote record_firmware_boot(const std::string& log_path,
                                                 const std::string& version);

private:
    EventRing      ring_;
    EventLogConfig cfg_;
    bool read_only_{false};
    mutable std::mutex mu_;
    std::size_t current_lines_{0};
    bool needs_separator_{false};
    bool persistence_ok_{true};
    bool syslog_ok_{true};
    std::uint32_t last_sequence_{0};
    std::unique_ptr<SyslogSender> sender_;
    std::unique_ptr<PlatformEventMonitor> platform_monitor_;

    void load_from_file();
    void load_segment(const std::string& path, bool current);
    bool persist_line(const EventRecord& ev);
    bool persistence_error(const char* operation);
    void syslog_failure(const std::string& reason);
    bool append_impl(EventRecord ev, bool forward);
};

std::string format_o14_timestamp(std::int64_t epoch_ms);

/** IEC 62351-14 RFC 5424 message, or empty for an unmapped/invalid event. */
std::string format_rfc5424(const EventRecord& ev, const std::string& hostname,
                           const std::string& app_name);

std::string firmware_marker_path(const std::string& log_path);

}  // namespace cci::core
