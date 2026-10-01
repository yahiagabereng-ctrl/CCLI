#pragma once

#include <cstdint>
#include <string>

namespace cci::hal {

struct TimePoint {
    std::int64_t epoch_ms{0};
};

TimePoint now();
std::string format_iso8601(const TimePoint& tp);

}  // namespace cci::hal
