#include "e2e_time.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <limits>

namespace ccli::mms::e2e {
namespace {

constexpr std::size_t kMaximumTimeValueLength = 64;

bool digit(std::uint8_t value) noexcept {
    return value >= static_cast<std::uint8_t>('0') &&
           value <= static_cast<std::uint8_t>('9');
}

bool parse_digits(ByteView value, std::size_t offset, std::size_t count,
                  int& result) noexcept {
    if (offset > value.size || count > value.size - offset) {
        return false;
    }
    int parsed = 0;
    for (std::size_t i = 0; i < count; ++i) {
        if (!digit(value.data[offset + i])) {
            return false;
        }
        parsed = parsed * 10 + (value.data[offset + i] - '0');
    }
    result = parsed;
    return true;
}

bool leap_year(int year) noexcept {
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

int days_in_month(int year, int month) noexcept {
    static constexpr int days[] = {31, 28, 31, 30, 31, 30,
                                   31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) {
        return 0;
    }
    return month == 2 && leap_year(year) ? 29 : days[month - 1];
}

// Proleptic Gregorian calendar conversion, with 1970-01-01 as day zero.
std::int64_t days_from_civil(int year, unsigned month, unsigned day) noexcept {
    year -= month <= 2;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned year_of_era = static_cast<unsigned>(year - era * 400);
    const unsigned adjusted_month = month > 2 ? month - 3 : month + 9;
    const unsigned day_of_year = (153 * adjusted_month + 2) / 5 + day - 1;
    const unsigned day_of_era = year_of_era * 365 + year_of_era / 4 - year_of_era / 100 + day_of_year;
    return static_cast<std::int64_t>(era) * 146097 +
           static_cast<std::int64_t>(day_of_era) - 719468;
}

bool parse_zone(ByteView value, std::size_t& content_end,
                int& offset_seconds) noexcept {
    offset_seconds = 0;
    if (content_end == 0) {
        return false;
    }
    const auto last = value.data[content_end - 1];
    if (last == 'Z') {
        --content_end;
        return true;
    }
    if (content_end < 5 ||
        (value.data[content_end - 5] != '+' && value.data[content_end - 5] != '-')) {
        return false; // Local time cannot be compared safely with UTC.
    }
    const std::size_t zone_start = content_end - 5;
    int hours = 0;
    int minutes = 0;
    if (!parse_digits(value, zone_start + 1, 2, hours) ||
        !parse_digits(value, zone_start + 3, 2, minutes) ||
        hours > 23 || minutes > 59) {
        return false;
    }
    const int sign = value.data[zone_start] == '+' ? 1 : -1;
    offset_seconds = sign * (hours * 3600 + minutes * 60);
    content_end = zone_start;
    return true;
}

bool split_fraction(ByteView value, std::size_t& content_end,
                    long double& fraction) noexcept {
    fraction = 0.0L;
    std::size_t dot = content_end;
    for (std::size_t i = 0; i < content_end; ++i) {
        if (value.data[i] == '.') {
            if (dot != content_end) {
                return false;
            }
            dot = i;
        }
    }
    if (dot == content_end) {
        return true;
    }
    if (dot + 1 == content_end) {
        return false;
    }
    long double place = 0.1L;
    for (std::size_t i = dot + 1; i < content_end; ++i) {
        if (!digit(value.data[i])) {
            return false;
        }
        fraction += static_cast<long double>(value.data[i] - '0') * place;
        place *= 0.1L;
    }
    content_end = dot;
    return true;
}

bool to_clock_duration(long double seconds,
                       std::chrono::system_clock::duration& result) noexcept {
    using Duration = std::chrono::system_clock::duration;
    using Rep = Duration::rep;
    const long double ticks = seconds * static_cast<long double>(Duration::period::den) /
                              static_cast<long double>(Duration::period::num);
    if (!std::isfinite(ticks) ||
        ticks < static_cast<long double>(std::numeric_limits<Rep>::lowest()) ||
        ticks > static_cast<long double>(std::numeric_limits<Rep>::max())) {
        return false;
    }
    result = Duration(static_cast<Rep>(ticks));
    return true;
}

} // namespace

bool parse_generalized_time(ByteView encoded_value,
                            std::chrono::system_clock::time_point& result,
                            GeneralizedTimeError& error) noexcept {
    result = {};
    error = GeneralizedTimeError::invalid_input;
    if (!encoded_value.valid() || encoded_value.empty() ||
        encoded_value.size > kMaximumTimeValueLength) {
        return false;
    }

    // XSD whitespace-collapse permits surrounding ASCII spaces. The ASN.1
    // contents are otherwise treated as ASCII GeneralizedTime characters.
    std::size_t begin = 0;
    std::size_t end = encoded_value.size;
    while (begin < end && encoded_value.data[begin] == ' ') ++begin;
    while (end > begin && encoded_value.data[end - 1] == ' ') --end;
    if (begin == end) {
        error = GeneralizedTimeError::invalid_format;
        return false;
    }
    ByteView value{encoded_value.data + begin, end - begin};

    int offset_seconds = 0;
    if (!parse_zone(value, end, offset_seconds)) {
        error = GeneralizedTimeError::invalid_format;
        return false;
    }
    long double fraction = 0.0L;
    if (!split_fraction(value, end, fraction)) {
        error = GeneralizedTimeError::invalid_format;
        return false;
    }

    if (end != 8 && end != 10 && end != 12 && end != 14) {
        error = GeneralizedTimeError::invalid_format;
        return false;
    }
    for (std::size_t i = 0; i < end; ++i) {
        if (!digit(value.data[i])) {
            error = GeneralizedTimeError::invalid_format;
            return false;
        }
    }

    int year = 0;
    int month = 1;
    int day = 1;
    int hour = 0;
    int minute = 0;
    int second = 0;
    if (!parse_digits(value, 0, 4, year) ||
        (end >= 10 && !parse_digits(value, 8, 2, hour)) ||
        (end >= 12 && !parse_digits(value, 10, 2, minute)) ||
        (end >= 14 && !parse_digits(value, 12, 2, second))) {
        error = GeneralizedTimeError::invalid_format;
        return false;
    }
    if (!parse_digits(value, 4, 2, month) || !parse_digits(value, 6, 2, day) ||
        year < 1 || month < 1 || month > 12 || day < 1 ||
        day > days_in_month(year, month) || hour > 23 || minute > 59 || second > 59) {
        error = GeneralizedTimeError::invalid_calendar_value;
        return false;
    }

    const std::int64_t days = days_from_civil(year, static_cast<unsigned>(month),
                                              static_cast<unsigned>(day));
    long double seconds = static_cast<long double>(days) * 86400.0L +
                          static_cast<long double>(hour * 3600 + minute * 60 + second);
    const int fraction_unit_seconds = end == 8 ? 86400 : end == 10 ? 3600 : end == 12 ? 60 : 1;
    seconds += fraction * static_cast<long double>(fraction_unit_seconds);
    // A positive local offset means local time is ahead of UTC.
    seconds -= static_cast<long double>(offset_seconds);

    std::chrono::system_clock::duration duration{};
    if (!to_clock_duration(seconds, duration)) {
        error = GeneralizedTimeError::out_of_range;
        return false;
    }
    result = std::chrono::system_clock::time_point(duration);
    error = GeneralizedTimeError::none;
    return true;
}

bool format_generalized_time_utc(std::chrono::system_clock::time_point value,
                                std::string& result) noexcept {
    result.clear();
    using namespace std::chrono;
    const auto whole_seconds = floor<seconds>(value.time_since_epoch());
    const auto fraction = value.time_since_epoch() - whole_seconds;
    const auto unix_seconds = whole_seconds.count();
    const std::time_t timestamp = static_cast<std::time_t>(unix_seconds);
    if (static_cast<std::int64_t>(timestamp) != unix_seconds) {
        return false;
    }
    std::tm utc{};
#if defined(_WIN32)
    if (gmtime_s(&utc, &timestamp) != 0) return false;
#else
    if (gmtime_r(&timestamp, &utc) == nullptr) return false;
#endif
    if (utc.tm_year + 1900 < 1 || utc.tm_year + 1900 > 9999) {
        return false;
    }

    std::array<char, 40> buffer{};
    const int base_length = std::snprintf(
        buffer.data(), buffer.size(), "%04d%02d%02d%02d%02d%02d",
        utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
        utc.tm_hour, utc.tm_min, utc.tm_sec);
    if (base_length != 14) {
        return false;
    }
    std::size_t length = static_cast<std::size_t>(base_length);
    auto nanos = duration_cast<nanoseconds>(fraction).count();
    if (nanos != 0) {
        if (nanos < 0 || nanos >= 1000000000) return false;
        char fractional[10]{};
        std::snprintf(fractional, sizeof(fractional), "%09lld",
                      static_cast<long long>(nanos));
        int fractional_length = 9;
        while (fractional_length > 0 && fractional[fractional_length - 1] == '0') {
            --fractional_length;
        }
        buffer[length++] = '.';
        for (int i = 0; i < fractional_length; ++i) {
            buffer[length++] = fractional[i];
        }
    }
    buffer[length++] = 'Z';
    try {
        result.assign(buffer.data(), length);
        return true;
    } catch (...) {
        result.clear();
        return false;
    }
}

} // namespace ccli::mms::e2e
