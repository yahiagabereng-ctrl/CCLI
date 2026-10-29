#pragma once

#include "ber_codec.hpp"

#include <chrono>
#include <string>

namespace ccli::mms::e2e {

enum class GeneralizedTimeError : unsigned char {
    none,
    invalid_input,
    invalid_format,
    invalid_calendar_value,
    out_of_range,
};

// Parse the XSD GeneralizedTime profile used by the IEC 62351-4 component.
// The result is an absolute system-clock time; Z or an explicit numeric offset
// is required and offsets are normalized to UTC. Values with precision below
// seconds and decimal seconds are supported. Inputs are bounded to keep
// untrusted BER fields cheap to scan.
bool parse_generalized_time(ByteView encoded_value,
                            std::chrono::system_clock::time_point& result,
                            GeneralizedTimeError& error) noexcept;

// Format an E2E timestamp as UTC GeneralizedTime, including Z and a fractional
// second only when the supplied time point has a non-zero fractional second.
bool format_generalized_time_utc(std::chrono::system_clock::time_point value,
                                std::string& result) noexcept;

} // namespace ccli::mms::e2e
