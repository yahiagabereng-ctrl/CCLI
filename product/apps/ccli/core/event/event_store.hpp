#pragma once

#include "event/event_ring.hpp"

#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <mutex>
#include <string>
#include <vector>

namespace cci::core {

/** O.14 rolling event log configuration (P7-01 / P7-03). */
struct EventLogConfig {
    bool        enabled{true};
    std::string path{"/var/lib/ccli/events.jsonl"};
    bool        syslog_enabled{false};
    std::string syslog_host{"127.0.0.1"};
    int         syslog_port{514};
};

enum class FirmwareBootKind { First, Unchanged, Updated };

struct FirmwareBootNote {
    FirmwareBootKind kind{FirmwareBootKind::First};
    std::string      detail;
};

/** In-memory ring (2048) with append-only persistence. No public erase. */
class EventStore {
public:
    static constexpr std::size_t kMaxEvents = EventRing::kMaxEvents;

    explicit EventStore(EventLogConfig cfg = {});

    void append(EventRecord ev);
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
    mutable std::mutex mu_;

    void load_from_file();
    void persist_line(const EventRecord& ev);
    void rewrite_file_from_ring();
    void apply_file_mode() const;
    void syslog_send(const EventRecord& ev) const;
};

std::string format_o14_timestamp(std::int64_t epoch_ms);

/** RFC 5424 syslog line (PRI = local0.notice or .warning). */
std::string format_rfc5424(const EventRecord& ev, const std::string& hostname,
                           const std::string& app_name);

std::string firmware_marker_path(const std::string& log_path);

}  // namespace cci::core
