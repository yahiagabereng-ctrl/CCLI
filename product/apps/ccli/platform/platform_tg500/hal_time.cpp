#include "cci/hal/time.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace cci::hal {

TimePoint now() {
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return TimePoint{ms.count()};
}

std::string format_iso8601(const TimePoint& tp) {
    const std::time_t sec = static_cast<std::time_t>(tp.epoch_ms / 1000);
    std::tm tm_buf{};
#if defined(_WIN32)
    gmtime_s(&tm_buf, &sec);
#else
    gmtime_r(&sec, &tm_buf);
#endif
    std::ostringstream os;
    os << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%S") << 'Z';
    return os.str();
}

}  // namespace cci::hal
