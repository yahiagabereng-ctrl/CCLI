#include "event/event_ring.hpp"

namespace cci::core {

// Bound the buffer so a long-running daemon cannot grow memory without limit
// (IEC 62443-3-3 essential-function availability). Oldest events are dropped.
void EventRing::append(EventRecord ev) {
    if (ring_.size() >= kMaxEvents) {
        ring_.erase(ring_.begin());
    }
    ring_.push_back(std::move(ev));
}

std::vector<EventRecord> EventRing::recent(std::size_t max_count) const {
    if (ring_.size() <= max_count) {
        return ring_;
    }
    return std::vector<EventRecord>(ring_.end() - static_cast<std::ptrdiff_t>(max_count), ring_.end());
}

}  // namespace cci::core
