#pragma once

#include <string>

namespace cci::core {

/** Emits IEC 62351-14 time-source events only when synchronization state changes. */
class TimeSyncEventTracker {
public:
    /** Returns an IEC 62351-14 event ID, or an empty string when no event is due. */
    std::string update(bool synchronized, const std::string& source);

private:
    bool known_{false};
    bool synchronized_{false};
    std::string source_;
};

} // namespace cci::core
