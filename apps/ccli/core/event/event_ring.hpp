#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cci::core {

struct EventRecord {
    std::int64_t timestamp_ms{0};
    std::string type;
    std::string detail;
};

class EventRing {
public:
    static constexpr std::size_t kMaxEvents = 2048;

    void append(EventRecord ev);
    std::vector<EventRecord> recent(std::size_t max_count) const;

private:
    std::vector<EventRecord> ring_;
};

}  // namespace cci::core
