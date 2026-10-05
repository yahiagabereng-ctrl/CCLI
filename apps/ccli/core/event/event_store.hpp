#pragma once

#include "event/event_ring.hpp"

#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace cci::core {

/** O.14 rolling event log configuration (P7-01). */
struct EventLogConfig {
    bool        enabled{true};
    std::string path{"/var/lib/ccli/events.jsonl"};
};

/** In-memory ring (2048) with optional append-only persistence. */
class EventStore {
public:
    static constexpr std::size_t kMaxEvents = EventRing::kMaxEvents;

    explicit EventStore(EventLogConfig cfg = {});

    void append(EventRecord ev);
    std::vector<EventRecord> recent(std::size_t max_count) const;
    std::size_t size() const;

    /** Underlying ring for services that hold EventRing&. */
    EventRing& ring() { return ring_; }
    const EventRing& ring() const { return ring_; }

    const EventLogConfig& config() const { return cfg_; }

    /** P7-01 wrap test: append kMaxEvents+1 synthetic rows; expect size()==kMaxEvents. */
    static bool run_wrap_test(const std::string& path);

    /** P7-02 dump: O.14 timestamp format yyyy/mm/dd hh:mm:ss (UTC). */
    static int dump_to_stream(std::ostream& out, const std::string& path, std::size_t max_count,
                              bool json_out);

private:
    EventRing      ring_;
    EventLogConfig cfg_;

    void load_from_file();
    void persist_line(const EventRecord& ev);
    void rewrite_file_from_ring();
};

std::string format_o14_timestamp(std::int64_t epoch_ms);

}  // namespace cci::core
