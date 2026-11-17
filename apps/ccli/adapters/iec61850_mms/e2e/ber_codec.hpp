#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace ccli::mms::e2e {

struct ByteView {
    const std::uint8_t* data{nullptr};
    std::size_t size{0};

    bool empty() const noexcept { return size == 0; }
    bool valid() const noexcept { return size == 0 || data != nullptr; }
};

// Primitive values borrow input bytes. Fragmented BER string values retain
// their flattened contents when a decoded result is copied or moved.
struct BerStringView : ByteView {
    std::shared_ptr<const std::vector<std::uint8_t>> storage{};
    BerStringView() noexcept = default;
    BerStringView(ByteView value) noexcept : ByteView(value) {}
    BerStringView(const std::uint8_t* bytes, std::size_t count) noexcept : ByteView{bytes, count} {}
};

enum class BerTagClass : std::uint8_t {
    universal = 0,
    application = 1,
    context_specific = 2,
    private_use = 3,
};

struct BerTag {
    BerTagClass tag_class{BerTagClass::universal};
    bool constructed{false};
    std::uint32_t number{0};
};

enum class BerError : std::uint8_t {
    none,
    end_of_input,
    invalid_input,
    input_too_large,
    truncated_tag,
    invalid_tag,
    tag_too_large,
    truncated_length,
    invalid_length,
    length_overflow,
    value_truncated,
    indefinite_primitive,
    indefinite_not_allowed,
    missing_end_of_contents,
    unexpected_end_of_contents,
    nesting_limit,
    element_limit,
};

struct BerLimits {
    std::size_t max_depth{32};
    std::size_t max_elements{4096};
    std::size_t max_tag_octets{6};
    bool allow_indefinite{true};
    std::size_t max_encoded_bytes{1024 * 1024};
};

struct BerTlvView {
    BerTag tag{};
    ByteView encoded{}; // Exact received TLV bytes, including EOC for indefinite form.
    std::size_t header_size{0}; // Tag plus length octets; excludes EOC if indefinite.
    ByteView value{};   // Contents only; excludes the terminating EOC when present.
    bool indefinite{false};
};

// Bounded BER reader. It does not decode ASN.1 types or normalize bytes: callers
// can retain encoded/value slices for signature and integrity calculations.
class BerReader {
public:
    explicit BerReader(ByteView input, BerLimits limits = {}) noexcept;

    bool next(BerTlvView& tlv, BerError& error) noexcept;
    bool finished() const noexcept { return cursor_ == input_.size; }
    std::size_t offset() const noexcept { return cursor_; }

    BerReader children(const BerTlvView& parent) const noexcept;

private:
    ByteView input_{};
    BerLimits limits_{};
    std::size_t cursor_{0};
    std::size_t elements_seen_{0};
    bool input_valid_{true};
    bool input_too_large_{false};
};

// Decode an OCTET STRING, BIT STRING, UTF8String or GeneralizedTime, including
// IMPLICIT root tags. Constructed octet/character strings contain universal
// OCTET STRING segments; BIT STRING segments use universal tag 3 (X.690 8.6,
// 8.7, 8.23, 8.25). Original encoded authentication slices are never changed.
bool ber_string_contents(const BerTlvView& value, std::uint32_t universal_type,
                         BerStringView& result, BerError& error,
                         BerLimits limits = {}) noexcept;

} // namespace ccli::mms::e2e
