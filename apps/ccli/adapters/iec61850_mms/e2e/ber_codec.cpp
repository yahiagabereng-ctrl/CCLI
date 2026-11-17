#include "ber_codec.hpp"

#include <limits>

namespace ccli::mms::e2e {
namespace {

struct ScannedTlv {
    BerTag tag{};
    std::size_t value_begin{0};
    std::size_t value_end{0};
    std::size_t encoded_end{0};
    bool indefinite{false};
};

bool parse_one(const std::uint8_t* data, std::size_t begin, std::size_t limit,
               const BerLimits& limits, std::size_t depth, std::size_t& element_count,
               ScannedTlv& result, BerError& error) noexcept {
    if (depth > limits.max_depth) {
        error = BerError::nesting_limit;
        return false;
    }
    if (element_count >= limits.max_elements) {
        error = BerError::element_limit;
        return false;
    }
    ++element_count;

    std::size_t cursor = begin;
    if (cursor >= limit) {
        error = BerError::truncated_tag;
        return false;
    }

    const std::uint8_t first_tag = data[cursor++];
    result.tag.tag_class = static_cast<BerTagClass>((first_tag >> 6) & 0x03);
    result.tag.constructed = (first_tag & 0x20) != 0;
    std::uint32_t tag_number = first_tag & 0x1f;
    if (tag_number == 0x1f) {
        std::size_t tag_octets = 1;
        bool first_subidentifier = true;
        tag_number = 0;
        for (;;) {
            if (cursor >= limit) {
                error = BerError::truncated_tag;
                return false;
            }
            if (tag_octets >= limits.max_tag_octets) {
                error = BerError::tag_too_large;
                return false;
            }
            const std::uint8_t octet = data[cursor++];
            ++tag_octets;
            const std::uint8_t payload = octet & 0x7f;
            if (first_subidentifier && payload == 0) {
                error = BerError::invalid_tag;
                return false;
            }
            first_subidentifier = false;
            if (tag_number > (std::numeric_limits<std::uint32_t>::max() >> 7)) {
                error = BerError::tag_too_large;
                return false;
            }
            tag_number = static_cast<std::uint32_t>((tag_number << 7) | payload);
            if ((octet & 0x80) == 0) {
                break;
            }
        }
        if (tag_number < 31) {
            error = BerError::invalid_tag;
            return false;
        }
    }
    result.tag.number = tag_number;
    if (result.tag.tag_class == BerTagClass::universal && tag_number == 0) {
        // Universal tag zero is reserved for the exact 00 00 terminator, which
        // is consumed by the enclosing indefinite-length value before recursing.
        error = BerError::unexpected_end_of_contents;
        return false;
    }

    if (cursor >= limit) {
        error = BerError::truncated_length;
        return false;
    }
    const std::uint8_t first_length = data[cursor++];
    std::size_t value_length = 0;
    const bool indefinite = first_length == 0x80;
    if (indefinite) {
        if (!result.tag.constructed) {
            error = BerError::indefinite_primitive;
            return false;
        }
        if (!limits.allow_indefinite) {
            error = BerError::indefinite_not_allowed;
            return false;
        }
    } else if ((first_length & 0x80) == 0) {
        value_length = first_length;
    } else {
        const std::size_t length_octets = first_length & 0x7f;
        if (length_octets == 0 || length_octets == 0x7f) {
            error = BerError::invalid_length;
            return false;
        }
        if (length_octets > sizeof(std::size_t)) {
            error = BerError::length_overflow;
            return false;
        }
        if (length_octets > limit - cursor) {
            error = BerError::truncated_length;
            return false;
        }
        for (std::size_t i = 0; i < length_octets; ++i) {
            const std::uint8_t octet = data[cursor++];
            if (value_length > (std::numeric_limits<std::size_t>::max() >> 8)) {
                error = BerError::length_overflow;
                return false;
            }
            value_length = (value_length << 8) | octet;
        }
    }

    result.value_begin = cursor;
    result.indefinite = indefinite;
    if (!indefinite) {
        if (value_length > limit - cursor) {
            error = BerError::value_truncated;
            return false;
        }
        const std::size_t value_end = cursor + value_length;
        if (result.tag.constructed) {
            while (cursor < value_end) {
                // End-of-contents is only valid as the terminator of an
                // indefinite-length constructed value.
                ScannedTlv child{};
                if (!parse_one(data, cursor, value_end, limits, depth + 1,
                               element_count, child, error)) {
                    return false;
                }
                if (child.tag.tag_class == BerTagClass::universal &&
                    !child.tag.constructed && child.tag.number == 0 &&
                    child.value_begin == child.value_end) {
                    error = BerError::unexpected_end_of_contents;
                    return false;
                }
                cursor = child.encoded_end;
            }
        }
        result.value_end = value_end;
        result.encoded_end = value_end;
        error = BerError::none;
        return true;
    }

    for (;;) {
        if (cursor >= limit) {
            error = BerError::missing_end_of_contents;
            return false;
        }
        if (limit - cursor >= 2 && data[cursor] == 0 && data[cursor + 1] == 0) {
            result.value_end = cursor;
            result.encoded_end = cursor + 2;
            error = BerError::none;
            return true;
        }
        ScannedTlv child{};
        if (!parse_one(data, cursor, limit, limits, depth + 1,
                       element_count, child, error)) {
            return false;
        }
        if (child.tag.tag_class == BerTagClass::universal &&
            !child.tag.constructed && child.tag.number == 0 &&
            child.value_begin == child.value_end) {
            error = BerError::unexpected_end_of_contents;
            return false;
        }
        cursor = child.encoded_end;
    }
}

} // namespace

BerReader::BerReader(ByteView input, BerLimits limits) noexcept
    : input_(input), limits_(limits), input_valid_(input.valid()),
      input_too_large_(input.size > limits.max_encoded_bytes) {}

bool BerReader::next(BerTlvView& tlv, BerError& error) noexcept {
    tlv = {};
    if (!input_valid_ || (input_.size != 0 && input_.data == nullptr)) {
        error = BerError::invalid_input;
        return false;
    }
    if (input_too_large_) {
        error = BerError::input_too_large;
        return false;
    }
    if (cursor_ == input_.size) {
        error = BerError::end_of_input;
        return false;
    }

    std::size_t element_count = elements_seen_;
    ScannedTlv scanned{};
    if (!parse_one(input_.data, cursor_, input_.size, limits_, 1,
                   element_count, scanned, error)) {
        return false;
    }

    tlv.tag = scanned.tag;
    tlv.encoded = {input_.data + cursor_, scanned.encoded_end - cursor_};
    tlv.header_size = scanned.value_begin - cursor_;
    tlv.value = {input_.data + scanned.value_begin,
                 scanned.value_end - scanned.value_begin};
    tlv.indefinite = scanned.indefinite;
    cursor_ = scanned.encoded_end;
    elements_seen_ = element_count;
    error = BerError::none;
    return true;
}

BerReader BerReader::children(const BerTlvView& parent) const noexcept {
    BerReader reader(parent.value, limits_);
    if (!parent.tag.constructed) {
        reader.input_valid_ = false;
    }
    return reader;
}

namespace {
bool collect_string(const BerTlvView& node, bool bits, BerLimits limits,
                    std::size_t depth, std::size_t& count,
                    std::vector<std::uint8_t>& bytes, unsigned& unused,
                    BerError& error) {
    if (depth > limits.max_depth) { error = BerError::nesting_limit; return false; }
    if (++count > limits.max_elements) { error = BerError::element_limit; return false; }
    if (!node.tag.constructed) {
        const auto value = node.value;
        const std::size_t prefix = bits ? 1 : 0;
        if (bits && (value.empty() || value.data[0] > 7 ||
                     (value.size == 1 && value.data[0] != 0))) {
            error = BerError::invalid_input; return false;
        }
        if (bits && unused) { error = BerError::invalid_input; return false; }
        if (value.size - prefix > limits.max_encoded_bytes - bytes.size()) {
            error = BerError::input_too_large; return false;
        }
        if (value.size > prefix)
            bytes.insert(bytes.end(), value.data + prefix, value.data + value.size);
        unused = bits ? value.data[0] : 0;
        return true;
    }
    BerReader children(node.value, limits); BerTlvView child;
    while (!children.finished()) {
        if (!children.next(child, error)) return false;
        if (child.tag.tag_class != BerTagClass::universal || child.tag.number != (bits ? 3U : 4U)) {
            error = BerError::invalid_tag; return false;
        }
        if (bits && unused) { error = BerError::invalid_input; return false; }
        if (!collect_string(child, bits, limits, depth + 1, count, bytes, unused, error)) return false;
    }
    return true;
}
}

bool ber_string_contents(const BerTlvView& value, std::uint32_t type,
                         BerStringView& result, BerError& error, BerLimits limits) noexcept {
    result = {}; error = BerError::none;
    if (!value.encoded.valid() || !value.value.valid() ||
        (type != 3 && type != 4 && type != 12 && type != 24)) {
        error = BerError::invalid_input; return false;
    }
    if (value.encoded.size > limits.max_encoded_bytes) { error = BerError::input_too_large; return false; }
    if (value.value.size > limits.max_encoded_bytes) { error = BerError::input_too_large; return false; }
    if (!limits.max_depth) { error = BerError::nesting_limit; return false; }
    if (!limits.max_elements) { error = BerError::element_limit; return false; }
    if (value.indefinite && !limits.allow_indefinite) { error = BerError::indefinite_not_allowed; return false; }
    const bool bits = type == 3;
    if (!value.tag.constructed) {
        if (bits && (value.value.empty() || value.value.data[0] > 7 ||
                     (value.value.size == 1 && value.value.data[0] != 0))) {
            error = BerError::invalid_input; return false;
        }
        // BER permits nonzero padding bits. Mask them in the logical value,
        // while leaving the original TLV and signed/MAC bytes untouched.
        if (!bits || value.value.size < 2 || !value.value.data[0] ||
            !(value.value.data[value.value.size - 1] & ((1U << value.value.data[0]) - 1))) {
            result = value.value; return true;
        }
    }
    try {
        auto contents = std::make_shared<std::vector<std::uint8_t>>();
        std::size_t count = 0; unsigned unused = 0;
        if (!collect_string(value, bits, limits, 1, count, *contents, unused, error)) return false;
        if (bits) {
            if (unused && !contents->empty()) contents->back() &= static_cast<std::uint8_t>(0xffU << unused);
            contents->insert(contents->begin(), static_cast<std::uint8_t>(unused));
            if (contents->size() > limits.max_encoded_bytes) { error = BerError::input_too_large; return false; }
        }
        result.storage = contents; result.data = contents->data(); result.size = contents->size();
        return true;
    } catch (...) { result = {}; error = BerError::input_too_large; return false; }
}

} // namespace ccli::mms::e2e
