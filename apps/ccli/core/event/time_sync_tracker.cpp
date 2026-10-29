#include "event/time_sync_tracker.hpp"

namespace cci::core {

std::string TimeSyncEventTracker::update(const bool synchronized, const std::string& source) {
    if (!known_) {
        known_ = true;
        synchronized_ = synchronized;
        source_ = source;
        return synchronized ? "IEC62351-14:8.1" : "IEC62351-14:8.2";
    }

    const bool source_changed = source_ != source;
    source_ = source;
    if (synchronized_ == synchronized) {
        return synchronized && source_changed ? "IEC62351-14:8.1" : std::string{};
    }

    const bool was_synchronized = synchronized_;
    synchronized_ = synchronized;
    if (synchronized) return "IEC62351-14:8.1";
    return was_synchronized ? "IEC62351-14:8.3" : "IEC62351-14:8.2";
}

} // namespace cci::core
