#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>

namespace cci::core {

struct EventRecord {
    std::int64_t timestamp_ms{0};
    std::string type;
    std::string detail;
    /** Annex O.14 category; filled by EventStore if empty. */
    std::string category;
    /** IEC 62351-14 Table 2. Empty ID means an O.14-only operational record. */
    std::uint32_t sequence{0};
    std::string iec_id;
    std::string mnemonic;
    std::string severity;
    std::string text;
    std::string source;
    std::string app_name;
    std::string peer_info;
    std::string user_id;
    std::string role;
    std::vector<std::pair<std::string, std::string>> parameters;
};

class EventRing {
public:
    static constexpr std::size_t kMaxEvents = 2048;

    void append(EventRecord ev);
    std::size_t size() const { return ring_.size(); }
    std::vector<EventRecord> recent(std::size_t max_count) const;

private:
    std::vector<EventRecord> ring_;
};

}  // namespace cci::core
