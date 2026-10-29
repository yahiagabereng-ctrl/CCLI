#include "e2e_codec.hpp"

#include <array>
#include <limits>

namespace ccli::mms::e2e {
namespace {

constexpr BerTagClass kUniversal = BerTagClass::universal;
constexpr BerTagClass kApplication = BerTagClass::application;
constexpr BerTagClass kContext = BerTagClass::context_specific;
constexpr std::uint32_t kInteger = 2;
constexpr std::uint32_t kBitString = 3;
constexpr std::uint32_t kOctetString = 4;
constexpr std::uint32_t kObjectIdentifier = 6;
constexpr std::uint32_t kSequence = 16;
constexpr std::uint32_t kUtf8String = 12;
constexpr std::uint32_t kGeneralizedTime = 24;

bool has_tag(const BerTlvView& tlv, BerTagClass tag_class,
             std::uint32_t number, bool constructed) noexcept {
    return tlv.tag.tag_class == tag_class && tlv.tag.number == number &&
           tlv.tag.constructed == constructed;
}

bool string_tag(const BerTlvView& value, std::uint32_t type) noexcept {
    return value.tag.tag_class == kUniversal && value.tag.number == type;
}

bool string_contents(const BerTlvView& value, std::uint32_t type,
                     BerStringView& result, E2eCodecError& error, BerLimits limits) noexcept {
    BerError reason{};
    if (ber_string_contents(value, type, result, reason, limits)) return true;
    error = reason == BerError::input_too_large || reason == BerError::nesting_limit ||
            reason == BerError::element_limit ? E2eCodecError::resource_limit_exceeded : E2eCodecError::invalid_field;
    return false;
}

bool decode_string(const BerTlvView& value, std::uint32_t type,
                   BerStringView& result, E2eCodecError& error,
                   BerLimits limits) noexcept {
    if (!string_tag(value, type)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    return string_contents(value, type, result, error, limits);
}

bool read_required(BerReader& reader, BerTlvView& tlv,
                   E2eCodecError& error) noexcept {
    BerError ber_error = BerError::none;
    if (!reader.next(tlv, ber_error)) {
        if (ber_error == BerError::end_of_input) {
            error = E2eCodecError::missing_field;
        } else if (ber_error == BerError::input_too_large ||
                   ber_error == BerError::nesting_limit ||
                   ber_error == BerError::element_limit) {
            error = E2eCodecError::resource_limit_exceeded;
        } else {
            error = E2eCodecError::malformed_ber;
        }
        return false;
    }
    return true;
}

bool read_single(ByteView encoded, BerLimits limits, BerTlvView& tlv,
                 E2eCodecError& error) noexcept {
    BerReader reader(encoded, limits);
    if (!read_required(reader, tlv, error)) {
        return false;
    }
    if (!reader.finished()) {
        error = E2eCodecError::trailing_data;
        return false;
    }
    return true;
}

bool next_child(BerReader& reader, BerTlvView& child,
                E2eCodecError& error) noexcept {
    if (reader.finished()) {
        error = E2eCodecError::missing_field;
        return false;
    }
    return read_required(reader, child, error);
}

bool valid_association_id(ByteView value) noexcept {
    if (!value.valid() || value.empty()) {
        return false;
    }

    std::size_t digits = 0;
    bool saw_colon = false;
    std::size_t second_digits = 0;
    for (std::size_t i = 0; i < value.size; ++i) {
        const std::uint8_t ch = value.data[i];
        if (ch >= '0' && ch <= '9') {
            if (saw_colon) {
                ++second_digits;
                if (second_digits > 3) {
                    return false;
                }
            } else {
                ++digits;
                if (digits > 3) {
                    return false;
                }
            }
        } else if (ch == ':' && !saw_colon && digits != 0 && i + 1 < value.size) {
            saw_colon = true;
        } else {
            return false;
        }
    }
    return digits != 0 && (!saw_colon || second_digits != 0);
}

bool valid_nmtoken_contents(ByteView value) noexcept {
    if (!value.valid() || value.empty()) {
        return false;
    }
    std::size_t offset = 0;
    while (offset < value.size) {
        const std::uint8_t first = value.data[offset++];
        std::uint32_t codepoint = 0;
        std::size_t continuation_count = 0;
        if (first <= 0x7f) {
            codepoint = first;
        } else if (first >= 0xc2 && first <= 0xdf) {
            codepoint = first & 0x1fu;
            continuation_count = 1;
        } else if (first >= 0xe0 && first <= 0xef) {
            codepoint = first & 0x0fu;
            continuation_count = 2;
        } else if (first >= 0xf0 && first <= 0xf4) {
            codepoint = first & 0x07u;
            continuation_count = 3;
        } else {
            return false;
        }
        if (continuation_count > value.size - offset) {
            return false;
        }
        for (std::size_t i = 0; i < continuation_count; ++i) {
            const std::uint8_t next = value.data[offset++];
            if ((next & 0xc0u) != 0x80u) {
                return false;
            }
            codepoint = (codepoint << 6) | (next & 0x3fu);
        }
        if ((continuation_count == 1 && codepoint < 0x80) ||
            (continuation_count == 2 && codepoint < 0x800) ||
            (continuation_count == 3 && codepoint < 0x10000) ||
            codepoint > 0x10fffdu ||
            (codepoint < 0x20) ||
            (codepoint >= 0xd800 && codepoint <= 0xdfff)) {
            return false;
        }
    }
    return true;
}

bool parse_nonnegative_integer(ByteView value, std::uint64_t& result) noexcept {
    if (!value.valid() || value.empty()) {
        return false;
    }

    // BER INTEGER is signed two's complement. These protocol fields are
    // non-negative and must use the minimum number of octets.
    if ((value.data[0] & 0x80) != 0) {
        return false;
    }
    std::size_t first = 0;
    if (value.size > 1 && value.data[0] == 0) {
        if ((value.data[1] & 0x80) == 0) {
            return false;
        }
        first = 1; // One leading sign octet is required for the high bit.
    }
    const std::size_t significant = value.size - first;
    if (significant > sizeof(std::uint64_t)) {
        return false;
    }
    std::uint64_t parsed = 0;
    for (std::size_t i = first; i < value.size; ++i) {
        parsed = (parsed << 8) | value.data[i];
    }
    result = parsed;
    return true;
}

bool valid_oid_contents(ByteView value) noexcept {
    if (!value.valid() || value.empty()) {
        return false;
    }

    bool first_octet_of_arc = true;
    for (std::size_t i = 0; i < value.size; ++i) {
        const std::uint8_t octet = value.data[i];
        if (first_octet_of_arc && octet == 0x80) {
            return false; // Non-minimal base-128 subidentifier.
        }
        first_octet_of_arc = false;
        if ((octet & 0x80) == 0) {
            first_octet_of_arc = true;
        }
    }
    return first_octet_of_arc; // Final subidentifier must be terminated.
}

bool validate_algorithm_identifier(ByteView encoded, BerLimits limits,
                                   E2eCodecError& error) noexcept {
    BerTlvView root{};
    if (!read_single(encoded, limits, root, error)) {
        return false;
    }
    if (!has_tag(root, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(root);
    BerTlvView oid{};
    if (!next_child(fields, oid, error)) {
        return false;
    }
    if (!has_tag(oid, kUniversal, kObjectIdentifier, false) ||
        !valid_oid_contents(oid.value)) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    while (!fields.finished()) {
        BerTlvView parameters{};
        if (!next_child(fields, parameters, error)) return false;
    }
    // Parameters and extension additions are opaque at this structural layer.
    // Known algorithm parameter policies are enforced by the crypto consumers.
    return true;
}

bool decode_boolean(ByteView value, bool& result) noexcept {
    if (!value.valid() || value.size != 1) {
        return false;
    }
    result = value.data[0] != 0;
    return true;
}

bool decode_v1(ByteView value) noexcept {
    // BIT STRING { v1 (0) } with only bit zero set: one data octet and
    // seven unused low-order bits. The DEFAULT value may be omitted entirely.
    if (!value.valid() || value.size < 2 || value.data[0] > 7 || value.data[1] != 0x80) return false;
    for (std::size_t i = 2; i < value.size; ++i) if (value.data[i]) return false;
    return true; // Named BIT STRING permits trailing zero bits in BER.
}

bool valid_bit_string(ByteView value) noexcept {
    if (!value.valid() || value.empty() || value.data[0] > 7) {
        return false;
    }
    if (value.size == 1) {
        return value.data[0] == 0;
    }
    const std::uint8_t unused = value.data[0];
    if (unused == 0) {
        return true;
    }
    const std::uint8_t mask = static_cast<std::uint8_t>((1u << unused) - 1u);
    return (value.data[value.size - 1] & mask) == 0;
}

bool bit_string_has(ByteView value, std::size_t bit_number) noexcept {
    if (!valid_bit_string(value)) {
        return false;
    }
    const std::size_t byte_index = 1 + bit_number / 8;
    if (byte_index >= value.size) {
        return false;
    }
    const std::size_t available_bits = (value.size - 1) * 8 - value.data[0];
    if (bit_number >= available_bits) {
        return false;
    }
    const std::uint8_t mask = static_cast<std::uint8_t>(0x80u >> (bit_number % 8));
    return (value.data[byte_index] & mask) != 0;
}

bool validate_cert_path(ByteView encoded, BerLimits limits,
                        E2eCodecError& error) noexcept {
    BerTlvView path{};
    if (!read_single(encoded, limits, path, error)) {
        return false;
    }
    if (!has_tag(path, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    BerReader certificates = BerReader(encoded, limits).children(path);
    if (certificates.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    while (!certificates.finished()) {
        BerTlvView certificate{};
        if (!read_required(certificates, certificate, error)) {
            return false;
        }
        BerStringView contents;
        if (!decode_string(certificate, kOctetString, contents, error, limits)) return false;
        if (contents.empty()) {
            error = E2eCodecError::invalid_field;
            return false;
        }
    }
    return true;
}

bool validate_algorithm_list(ByteView encoded_content, BerLimits limits,
                             E2eCodecError& error) noexcept {
    BerReader algorithms(encoded_content, limits);
    if (algorithms.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    while (!algorithms.finished()) {
        BerTlvView algorithm{};
        if (!read_required(algorithms, algorithm, error)) {
            return false;
        }
        if (!has_tag(algorithm, kUniversal, kSequence, true) ||
            !validate_algorithm_identifier(algorithm.encoded, limits, error)) {
            if (error == E2eCodecError::none) {
                error = E2eCodecError::wrong_tag;
            }
            return false;
        }
    }
    return true;
}

bool validate_encryption_mode(ByteView encoded, BerLimits limits,
                              EncryptionModeKind& mode,
                              E2eCodecError& error) noexcept {
    BerTlvView choice{};
    if (!read_single(encoded, limits, choice, error)) {
        return false;
    }
    if (has_tag(choice, kUniversal, kSequence, true)) {
        mode = EncryptionModeKind::authenticated_encryption;
        BerReader algorithms = BerReader(encoded, limits).children(choice);
        if (algorithms.finished()) {
            error = E2eCodecError::invalid_field;
            return false;
        }
        while (!algorithms.finished()) {
            BerTlvView algorithm{};
            if (!read_required(algorithms, algorithm, error)) {
                return false;
            }
            if (!has_tag(algorithm, kUniversal, kSequence, true) ||
                !validate_algorithm_identifier(algorithm.encoded, limits, error)) {
                if (error == E2eCodecError::none) {
                    error = E2eCodecError::wrong_tag;
                }
                return false;
            }
        }
        return true;
    }
    if (!has_tag(choice, kContext, 0, true)) {
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }

    mode = EncryptionModeKind::separate_encryption_and_icv;
    BerReader fields = BerReader(encoded, limits).children(choice);
    BerTlvView current{};
    bool has_icv_algorithms = false;
    bool has_encryption_algorithms = false;
    std::uint32_t previous_component = 0;
    while (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        if (current.tag.tag_class != kContext ||
            (current.tag.number != 0 && current.tag.number != 1) ||
            current.tag.constructed == false || current.tag.number < previous_component) {
            error = E2eCodecError::invalid_field;
            return false;
        }
        previous_component = current.tag.number;
        if (current.tag.number == 0) {
            if (has_encryption_algorithms) {
                error = E2eCodecError::invalid_field;
                return false;
            }
            if (!validate_algorithm_list(current.value, limits, error)) {
                return false;
            }
            has_encryption_algorithms = true;
        } else {
            if (has_icv_algorithms ||
                !validate_algorithm_list(current.value, limits, error)) {
                if (error == E2eCodecError::none) {
                    error = E2eCodecError::invalid_field;
                }
                return false;
            }
            has_icv_algorithms = true;
        }
    }
    if (!has_icv_algorithms) {
        error = E2eCodecError::missing_field;
        return false;
    }
    return true;
}

bool copy_view(const BerTlvView& tlv, BerStringView& value,
               E2eCodecError& error, BerLimits limits) noexcept {
    return string_contents(tlv, kOctetString, value, error, limits);
}

bool decode_signature(ByteView encoded, SignatureView& result,
                      E2eCodecError& error, BerLimits limits) noexcept {
    result = {};
    BerTlvView signature{};
    if (!read_single(encoded, limits, signature, error)) {
        return false;
    }
    if (!has_tag(signature, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(signature);
    BerTlvView algorithm{};
    if (!next_child(fields, algorithm, error)) {
        return false;
    }
    if (!has_tag(algorithm, kUniversal, kSequence, true) ||
        !validate_algorithm_identifier(algorithm.encoded, limits, error)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }

    BerTlvView value{};
    if (!next_child(fields, value, error)) {
        return false;
    }
    BerStringView signature_value;
    if (!decode_string(value, kOctetString, signature_value, error, limits)) return false;
    if (signature_value.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    while (!fields.finished()) {
        if (!read_required(fields, value, error)) {
            return false;
        }
        // Signature is extensible. Once algo/sign are consumed, future
        // additions may use a tag already present in the root.
    }

    result.encoded = signature.encoded;
    result.algorithm_identifier = algorithm.encoded;
    result.value = signature_value;
    return true;
}

bool get_signed_contents(const BerTlvView& tlv, ByteView& result,
                         E2eCodecError& error) noexcept {
    if (!tlv.encoded.valid() || tlv.encoded.empty() ||
        tlv.header_size > tlv.encoded.size) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    // For definite form this is exactly the value contents. For indefinite
    // form the encoded slice also includes the EOC bytes, as required by the
    // IEC signature-input convention.
    result = {tlv.encoded.data + tlv.header_size,
              tlv.encoded.size - tlv.header_size};
    return true;
}

bool append_raw(std::vector<std::uint8_t>& output, ByteView value) {
    if (!value.valid() || value.size > output.max_size() - output.size()) {
        return false;
    }
    if (value.size != 0) {
        output.insert(output.end(), value.data, value.data + value.size);
    }
    return true;
}

bool append_length(std::vector<std::uint8_t>& output, std::size_t length) {
    if (length < 128) {
        output.push_back(static_cast<std::uint8_t>(length));
        return true;
    }
    std::array<std::uint8_t, sizeof(std::size_t)> octets{};
    std::size_t count = 0;
    while (length != 0) {
        octets[count++] = static_cast<std::uint8_t>(length & 0xffu);
        length >>= 8;
    }
    if (count > 0x7e) {
        return false;
    }
    output.push_back(static_cast<std::uint8_t>(0x80u | count));
    while (count != 0) {
        output.push_back(octets[--count]);
    }
    return true;
}

bool append_tlv(std::vector<std::uint8_t>& output, std::uint8_t tag,
                ByteView value) {
    if (!value.valid() || output.size() > output.max_size() - 1 ||
        value.size > output.max_size() - output.size() - 1) {
        return false;
    }
    output.push_back(tag);
    if (!append_length(output, value.size)) {
        return false;
    }
    return append_raw(output, value);
}

bool append_unsigned_integer(std::vector<std::uint8_t>& output,
                             std::uint8_t tag, std::uint64_t value) {
    std::array<std::uint8_t, sizeof(value) + 1> octets{};
    std::size_t begin = octets.size();
    do {
        octets[--begin] = static_cast<std::uint8_t>(value & 0xffu);
        value >>= 8;
    } while (value != 0);
    if ((octets[begin] & 0x80u) != 0) {
        octets[--begin] = 0;
    }
    return append_tlv(output, tag,
                      {octets.data() + begin, octets.size() - begin});
}

bool read_expected_tag(ByteView encoded, BerTagClass tag_class,
                       std::uint32_t number, bool constructed,
                       BerLimits limits, E2eCodecError& error) noexcept {
    BerTlvView tlv{};
    if (!read_single(encoded, limits, tlv, error)) {
        return false;
    }
    if (!has_tag(tlv, tag_class, number, constructed)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    return true;
}

bool encode_osi_external(std::uint32_t indirect_reference, ByteView inner,
                         std::vector<std::uint8_t>& encoded,
                         E2eCodecError& error) {
    encoded.clear();
    error = E2eCodecError::none;
    if (!inner.valid() || inner.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    std::vector<std::uint8_t> external_content;
    if (!append_unsigned_integer(external_content, 0x02, indirect_reference)) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    std::vector<std::uint8_t> explicit_content;
    if (!append_raw(explicit_content, inner) ||
        !append_tlv(external_content, 0xa0,
                    {explicit_content.data(), explicit_content.size()})) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    std::vector<std::uint8_t> external;
    if (!append_tlv(external, 0x28,
                    {external_content.data(), external_content.size()}) ||
        !append_tlv(encoded, 0x30, {external.data(), external.size()})) {
        encoded.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    return true;
}

bool decode_osi_external(ByteView encoded, ByteView& full_encoding,
                         std::uint32_t& indirect_reference,
                         ByteView& inner_encoding, E2eCodecError& error,
                         BerLimits limits) noexcept {
    full_encoding = {};
    inner_encoding = {};
    error = E2eCodecError::none;
    BerTlvView root{};
    if (!read_single(encoded, limits, root, error)) {
        return false;
    }
    if (!has_tag(root, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    BerReader list = BerReader(encoded, limits).children(root);
    BerTlvView external{};
    if (!next_child(list, external, error)) {
        return false;
    }
    if (!has_tag(external, kUniversal, 8, true) || !list.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    BerReader fields = BerReader(encoded, limits).children(external);
    BerTlvView reference{};
    if (!next_child(fields, reference, error)) {
        return false;
    }
    if (!has_tag(reference, kUniversal, kInteger, false)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    std::uint64_t parsed_reference = 0;
    if (!parse_nonnegative_integer(reference.value, parsed_reference) ||
        parsed_reference > std::numeric_limits<std::uint32_t>::max()) {
        error = E2eCodecError::integer_out_of_range;
        return false;
    }
    BerTlvView explicit_value{};
    if (!next_child(fields, explicit_value, error)) {
        return false;
    }
    if (!has_tag(explicit_value, kContext, 0, true) || !fields.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    BerReader inner = BerReader(encoded, limits).children(explicit_value);
    BerTlvView value{};
    if (!next_child(inner, value, error)) {
        return false;
    }
    if (!inner.finished()) {
        error = E2eCodecError::trailing_data;
        return false;
    }
    full_encoding = root.encoded;
    indirect_reference = static_cast<std::uint32_t>(parsed_reference);
    inner_encoding = value.encoded;
    return true;
}

} // namespace

bool decode_data_transfer(ByteView encoded, DataTransferKind& kind,
                          ByteView& encoded_transfer, E2eCodecError& error,
                          BerLimits limits) noexcept {
    error = E2eCodecError::none;
    encoded_transfer = {};
    BerTlvView choice{};
    if (!read_single(encoded, limits, choice, error)) {
        return false;
    }
    if (choice.tag.tag_class != kContext || !choice.tag.constructed ||
        (choice.tag.number != 0 && choice.tag.number != 1)) {
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }

    kind = choice.tag.number == 0 ? DataTransferKind::clear
                                  : DataTransferKind::encrypted;
    // The alternatives are tagged SEQUENCE types under IMPLICIT TAGS. Their
    // context tag replaces UNIVERSAL SEQUENCE; there is no nested 0x30 TLV.
    encoded_transfer = choice.encoded;
    return true;
}

bool decode_handshake_request(ByteView encoded, HandshakeRequestView& result,
                              E2eCodecError& error,
                              BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView request{};
    if (!read_single(encoded, limits, request, error)) {
        return false;
    }
    if (!has_tag(request, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader request_fields = BerReader(encoded, limits).children(request);
    BerTlvView protocol{};
    if (!next_child(request_fields, protocol, error)) {
        return false;
    }
    if (!decode_string(protocol, kUtf8String, result.protected_protocol, error, limits)) return false;
    if (!valid_nmtoken_contents(result.protected_protocol)) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerTlvView tbs{};
    if (!next_child(request_fields, tbs, error)) {
        return false;
    }
    if (!has_tag(tbs, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerTlvView signature{};
    if (!next_child(request_fields, signature, error)) {
        return false;
    }
    if (!has_tag(signature, kUniversal, kSequence, true) ||
        !request_fields.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerReader tbs_fields = BerReader(encoded, limits).children(tbs);
    BerTlvView token1{};
    if (!next_child(tbs_fields, token1, error)) {
        return false;
    }
    if (!has_tag(token1, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    if (!decode_clear_token1(token1.encoded, result.token1_fields, error, limits)) {
        return false;
    }

    bool has_appl_data = false;
    ByteView appl_data{};
    if (!tbs_fields.finished()) {
        BerTlvView next{};
        if (!read_required(tbs_fields, next, error)) {
            return false;
        }
        // ApplData is the optional second root component. Later extensibility
        // additions are opaque and remain covered by the signed original TBS.
        // ApplData is supplied by the protected protocol's Virtual-API.
        // MMS uses MMSpdu CHOICE tags (e.g. A8 InitiateRequest), not a
        // synthetic universal SEQUENCE around the MMS PDU.
        has_appl_data = true;
        appl_data = next.encoded;
        while (!tbs_fields.finished()) {
            if (!read_required(tbs_fields, next, error)) {
                return false;
            }
        }
    }

    if (!decode_signature(signature.encoded, result.signature, error, limits)) {
        return false;
    }
    if (!get_signed_contents(tbs, result.signed_tbs_contents, error)) {
        return false;
    }
    result.encoded = request.encoded;
    result.protected_tbs = tbs.encoded;
    result.token1 = token1.encoded;
    result.has_application_data = has_appl_data;
    result.application_data = appl_data;
    return true;
}

bool decode_clear_token1(ByteView encoded, ClearToken1View& result,
                         E2eCodecError& error, BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView token{};
    if (!read_single(encoded, limits, token, error)) {
        return false;
    }
    if (!has_tag(token, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(token);
    BerTlvView current{};
    BerStringView logical;
    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!has_tag(current, kUniversal, kSequence, true) ||
        !validate_algorithm_identifier(current.encoded, limits, error)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    result.signature_algorithm = current.encoded;

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!decode_string(current, kOctetString, result.public_key_certificate, error, limits)) return false;
    if (result.public_key_certificate.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (has_tag(current, kUniversal, kSequence, true)) {
        if (!validate_cert_path(current.encoded, limits, error)) {
            return false;
        }
        result.has_certificate_path = true;
        result.certificate_path = current.encoded;
        if (!next_child(fields, current, error)) {
            return false;
        }
    }
    if (string_tag(current, kBitString)) {
        if (!string_contents(current, kBitString, logical, error, limits)) return false;
        if (!decode_v1(logical)) {
            error = E2eCodecError::unsupported_version;
            return false;
        }
        if (!next_child(fields, current, error)) {
            return false;
        }
    }
    if (!decode_string(current, kUtf8String, result.association_id, error, limits)) return false;
    if (!valid_association_id(result.association_id)) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerTlvView dh{};
    if (!next_child(fields, dh, error)) {
        return false;
    }
    if (!has_tag(dh, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    BerReader dh_fields = BerReader(encoded, limits).children(dh);
    BerTlvView group{};
    if (!next_child(dh_fields, group, error)) {
        return false;
    }
    if (!has_tag(group, kUniversal, kInteger, false) ||
        !parse_nonnegative_integer(group.value, result.dh.group_id)) {
        error = E2eCodecError::integer_out_of_range;
        return false;
    }
    BerTlvView public_key{};
    if (!next_child(dh_fields, public_key, error)) {
        return false;
    }
    BerStringView dh_public_key;
    if (!decode_string(public_key, kOctetString, dh_public_key, error, limits)) return false;
    if (dh_public_key.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    while (!dh_fields.finished()) {
        if (!read_required(dh_fields, public_key, error)) {
            return false;
        }
        // DiffieHellmanSet is extensible; retain unknown additions.
    }
    result.dh.encoded = dh.encoded;
    result.dh.public_key = dh_public_key;

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!has_tag(current, kUniversal, kSequence, true) ||
        !validate_algorithm_identifier(current.encoded, limits, error)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    result.hmac_algorithm = current.encoded;

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!decode_string(current, kGeneralizedTime, result.timestamp, error, limits)) return false;
    if (result.timestamp.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!validate_encryption_mode(current.encoded, limits,
                                  result.encryption_mode, error)) {
        return false;
    }
    result.encoded_encryption_mode = current.encoded;

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!decode_string(current, kBitString, logical, error, limits)) return false;
    if (!valid_bit_string(logical)) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    result.tls_protected = bit_string_has(logical, 0);
    result.application_protected = bit_string_has(logical, 1);
    result.vpn = bit_string_has(logical, 2);

    if (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        if (string_tag(current, kOctetString)) {
            result.has_attribute_certificate = true;
            if (!string_contents(current, kOctetString, result.attribute_certificate, error, limits)) return false;
        }
    }
    while (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        // Trailing values are extensibility additions after the known root.
    }

    result.encoded = token.encoded;
    return true;
}

bool decode_presentation_user_data(ByteView encoded,
                                   PresentationUserDataView& result,
                                   E2eCodecError& error,
                                   BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView user_data{};
    if (!read_single(encoded, limits, user_data, error)) {
        return false;
    }
    if (!has_tag(user_data, kApplication, 1, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader list = BerReader(encoded, limits).children(user_data);
    BerTlvView entry{};
    if (!next_child(list, entry, error)) {
        return false;
    }
    if (!has_tag(entry, kUniversal, kSequence, true) || !list.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(entry);
    BerTlvView context_id{};
    if (!next_child(fields, context_id, error)) {
        return false;
    }
    if (!has_tag(context_id, kUniversal, kInteger, false)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    std::uint64_t parsed_context = 0;
    if (!parse_nonnegative_integer(context_id.value, parsed_context) ||
        parsed_context == 0 || parsed_context > std::numeric_limits<std::uint32_t>::max()) {
        error = E2eCodecError::integer_out_of_range;
        return false;
    }

    BerTlvView explicit_data{};
    if (!next_child(fields, explicit_data, error)) {
        return false;
    }
    if (!has_tag(explicit_data, kContext, 0, true) || !fields.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerReader data_reader = BerReader(encoded, limits).children(explicit_data);
    BerTlvView choice{};
    if (!next_child(data_reader, choice, error)) {
        return false;
    }
    if (!data_reader.finished()) {
        error = E2eCodecError::trailing_data;
        return false;
    }
    if (!decode_data_transfer(choice.encoded, result.kind,
                              result.encoded_transfer, error, limits)) {
        return false;
    }

    result.encoded = user_data.encoded;
    result.presentation_context_id = static_cast<std::uint32_t>(parsed_context);
    return true;
}

bool decode_clear_token2(ByteView encoded, ClearToken2View& result,
                         E2eCodecError& error, BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView token{};
    if (!read_single(encoded, limits, token, error)) {
        return false;
    }
    if (!has_tag(token, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(token);
    BerTlvView current{};
    BerStringView logical;
    if (!next_child(fields, current, error)) {
        return false;
    }
    if (string_tag(current, kBitString)) {
        if (!string_contents(current, kBitString, logical, error, limits)) return false;
        if (!decode_v1(logical)) {
            error = E2eCodecError::unsupported_version;
            return false;
        }
        if (!next_child(fields, current, error)) {
            return false;
        }
    }

    if (!decode_string(current, kUtf8String, result.association_id, error, limits)) return false;
    if (!valid_association_id(result.association_id)) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!decode_string(current, kGeneralizedTime, result.timestamp, error, limits)) return false;
    if (result.timestamp.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!has_tag(current, kUniversal, kInteger, false) ||
        !parse_nonnegative_integer(current.value, result.sequence)) {
        error = E2eCodecError::integer_out_of_range;
        return false;
    }

    std::uint32_t next_optional_tag = 0;
    bool extension_fields_started = false;
    while (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        if (current.tag.tag_class == kContext && current.tag.number <= 3) {
            if (extension_fields_started || current.tag.number < next_optional_tag) {
                error = E2eCodecError::invalid_field;
                return false;
            }
            next_optional_tag = current.tag.number + 1;
            switch (current.tag.number) {
            case 0:
                if (!copy_view(current, result.initialization_vector, error, limits)) return false;
                if (result.initialization_vector.size != 16) {
                    error = E2eCodecError::invalid_field;
                    return false;
                }
                result.has_initialization_vector = true;
                break;
            case 1:
                if (!copy_view(current, result.rekey_public_key, error, limits)) return false;
                if (result.rekey_public_key.empty()) {
                    error = E2eCodecError::invalid_field;
                    return false;
                }
                result.has_rekey_public_key = true;
                break;
            case 2:
                if (current.tag.constructed || !decode_boolean(current.value, result.request_rekey)) {
                    error = E2eCodecError::invalid_field;
                    return false;
                }
                break;
            case 3:
                if (current.tag.constructed || !decode_boolean(current.value, result.changed_key)) {
                    error = E2eCodecError::invalid_field;
                    return false;
                }
                break;
            default:
                break;
            }
        } else {
            // The ASN.1 SEQUENCE is extensible. Preserve the parent TLV and
            // safely skip unknown trailing additions; root optional fields
            // cannot appear after an extension addition.
            // The root optionals have been parsed. A trailing value can be
            // an extension even when its tag matches a root type.
            extension_fields_started = true;
        }
    }

    result.encoded = token.encoded;
    return true;
}

bool decode_authenticator(ByteView encoded, AuthenticatorView& result,
                          E2eCodecError& error, BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView authenticator{};
    if (!read_single(encoded, limits, authenticator, error)) {
        return false;
    }
    if (!has_tag(authenticator, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(authenticator);
    BerTlvView current{};
    if (!next_child(fields, current, error)) {
        return false;
    }
    if (string_tag(current, kOctetString)) {
        result.has_nonce = true;
        if (!string_contents(current, kOctetString, result.nonce, error, limits)) return false;
        if (!next_child(fields, current, error)) {
            return false;
        }
    }

    if (!has_tag(current, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    result.algorithm_identifier = current.encoded;
    if (!validate_algorithm_identifier(result.algorithm_identifier, limits, error)) {
        return false;
    }

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!string_tag(current, kOctetString)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    if (!string_contents(current, kOctetString, result.icv, error, limits)) return false;
    while (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        // Authenticator has an extension marker; future additions are opaque.
    }

    result.encoded = authenticator.encoded;
    return true;
}

bool decode_clear_transfer(ByteView encoded, ClearTransferView& result,
                           E2eCodecError& error, BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView transfer{};
    if (!read_single(encoded, limits, transfer, error)) {
        return false;
    }
    if (!has_tag(transfer, kUniversal, kSequence, true) &&
        !has_tag(transfer, kContext, 0, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(transfer);
    BerTlvView tbp{};
    if (!next_child(fields, tbp, error)) {
        return false;
    }
    if (!has_tag(tbp, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    BerTlvView auth{};
    if (!next_child(fields, auth, error)) {
        return false;
    }
    if (!has_tag(auth, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    BerTlvView extension{};
    if (!fields.finished()) { error = E2eCodecError::invalid_field; return false; }

    BerReader tbp_fields = BerReader(encoded, limits).children(tbp);
    BerTlvView token{};
    if (!next_child(tbp_fields, token, error)) {
        return false;
    }
    if (!has_tag(token, kUniversal, kSequence, true) ||
        !decode_clear_token2(token.encoded, result.token, error, limits)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    if (result.token.has_initialization_vector) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerTlvView appl_data{};
    if (!next_child(tbp_fields, appl_data, error)) {
        return false;
    }
    while (!tbp_fields.finished()) {
        if (!read_required(tbp_fields, extension, error)) {
            return false;
        }
    }

    if (!decode_authenticator(auth.encoded, result.authenticator, error, limits)) {
        return false;
    }
    result.encoded = transfer.encoded;
    result.protected_tbp = tbp.encoded;
    if (!tbp.encoded.valid() || tbp.encoded.empty() ||
        tbp.header_size > tbp.encoded.size) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    result.icv_input = {tbp.encoded.data + tbp.header_size,
                        tbp.encoded.size - tbp.header_size};
    result.application_data = appl_data.encoded;
    return true;
}

bool ber_authentication_contents(ByteView encoded_tlv,
                                 ByteView& authentication_contents,
                                 E2eCodecError& error,
                                 BerLimits limits) noexcept {
    authentication_contents = {};
    error = E2eCodecError::none;
    BerTlvView tlv{};
    if (!read_single(encoded_tlv, limits, tlv, error)) {
        return false;
    }
    return get_signed_contents(tlv, authentication_contents, error);
}

bool decode_clear_token3(ByteView encoded, ClearToken3View& result,
                         E2eCodecError& error, BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView token{};
    if (!read_single(encoded, limits, token, error)) {
        return false;
    }
    if (!has_tag(token, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(token);
    BerTlvView current{};
    BerStringView logical;
    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!has_tag(current, kUniversal, kSequence, true) ||
        !validate_algorithm_identifier(current.encoded, limits, error)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    result.signature_algorithm = current.encoded;

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (string_tag(current, kBitString)) {
        if (!string_contents(current, kBitString, logical, error, limits)) return false;
        if (!decode_v1(logical)) {
            error = E2eCodecError::unsupported_version;
            return false;
        }
        if (!next_child(fields, current, error)) {
            return false;
        }
    }
    if (string_tag(current, kUtf8String)) {
        // ClearToken3 is also used in rejection messages. Preserve partial or
        // otherwise invalid IDs so the signed diagnostic can still be checked.
        if (!string_contents(current, kUtf8String, result.association_id, error, limits)) return false;
        result.has_association_id = true;
            if (!next_child(fields, current, error)) {
            return false;
        }
    }
    if (!decode_string(current, kGeneralizedTime, result.timestamp, error, limits)) return false;
    if (result.timestamp.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    if (!next_child(fields, current, error)) {
        return false;
    }
    if (!decode_string(current, kOctetString, result.public_key_certificate, error, limits)) return false;
    if (result.public_key_certificate.empty()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    if (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        if (has_tag(current, kUniversal, kSequence, true)) {
            if (!validate_cert_path(current.encoded, limits, error)) {
                return false;
            }
            result.has_certificate_path = true;
            result.certificate_path = current.encoded;
        }
    }
    while (!fields.finished()) {
        if (!read_required(fields, current, error)) {
            return false;
        }
        // ClearToken3 is extensible after certPath; unknown values stay opaque.
    }

    result.encoded = token.encoded;
    return true;
}

bool decode_signed_message(ByteView encoded, SignedMessageKind kind,
                           SignedMessageView& result, E2eCodecError& error,
                           BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView message{};
    if (!read_single(encoded, limits, message, error)) {
        return false;
    }
    BerTagClass expected_class = kUniversal;
    std::uint32_t expected_number = kSequence;
    switch (kind) {
    case SignedMessageKind::handshake_accept:
        expected_class = kContext;
        expected_number = 1;
        break;
    case SignedMessageKind::application_reject:
        expected_class = kContext;
        expected_number = 2;
        break;
    case SignedMessageKind::handshake_security_reject:
        expected_class = kContext;
        expected_number = 3;
        break;
    case SignedMessageKind::handshake_security_abort:
        expected_class = kContext;
        expected_number = 0;
        break;
    case SignedMessageKind::transfer_security_abort:
        expected_class = kContext;
        expected_number = 1;
        break;
    case SignedMessageKind::application_abort:
        expected_class = kContext;
        expected_number = 2;
        break;
    case SignedMessageKind::release_request:
    case SignedMessageKind::release_response:
        break;
    default:
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    if (!has_tag(message, expected_class, expected_number, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader message_fields = BerReader(encoded, limits).children(message);
    BerTlvView tbs{};
    if (!next_child(message_fields, tbs, error)) {
        return false;
    }
    if (!has_tag(tbs, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    BerTlvView signature{};
    if (!next_child(message_fields, signature, error)) {
        return false;
    }
    if (!has_tag(signature, kUniversal, kSequence, true) ||
        !message_fields.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerReader body = BerReader(encoded, limits).children(tbs);
    BerTlvView token{};
    if (!next_child(body, token, error)) {
        return false;
    }
    if (!has_tag(token, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    if (kind == SignedMessageKind::handshake_accept) {
        result.contains_clear_token1 = true;
        if (!decode_clear_token1(token.encoded, result.token1, error, limits)) {
            return false;
        }
    } else if (!decode_clear_token3(token.encoded, result.token3, error, limits)) {
        return false;
    }

    bool extension_fields_started = false;
    while (!body.finished()) {
        BerTlvView current{};
        if (!read_required(body, current, error)) {
            return false;
        }

        const bool message_has_appl_data =
            kind == SignedMessageKind::handshake_accept ||
            kind == SignedMessageKind::application_reject ||
            kind == SignedMessageKind::application_abort;
        const bool application_data_required =
            kind == SignedMessageKind::application_reject;
        const bool message_has_diagnostic =
            kind == SignedMessageKind::handshake_security_reject ||
            kind == SignedMessageKind::handshake_security_abort ||
            kind == SignedMessageKind::transfer_security_abort;

        if (message_has_appl_data &&
            !result.has_application_data && !extension_fields_started) {
            result.has_application_data = true;
            result.application_data = current.encoded;
            continue;
        }
        if (message_has_diagnostic &&
            has_tag(current, kUniversal, 10, false) &&
            !result.has_diagnostic && !extension_fields_started) {
            if (!parse_nonnegative_integer(current.value, result.diagnostic)) {
                error = E2eCodecError::integer_out_of_range;
                return false;
            }
            result.has_diagnostic = true;
            continue;
        }

        // The known root fields have been consumed. Preserve well-formed
        // future additions, including those with a familiar tag.
        extension_fields_started = true;
    }
    if (kind == SignedMessageKind::application_reject &&
        !result.has_application_data) {
        error = E2eCodecError::missing_field;
        return false;
    }

    if (!decode_signature(signature.encoded, result.signature, error, limits) ||
        !get_signed_contents(tbs, result.signed_tbs_contents, error)) {
        return false;
    }
    result.encoded = message.encoded;
    result.protected_tbs = tbs.encoded;
    return true;
}

bool decode_osi_association_request(ByteView encoded,
                                    OsiAssociationRequestView& result,
                                    E2eCodecError& error,
                                    BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView request{};
    if (!read_single(encoded, limits, request, error)) {
        return false;
    }
    if (!has_tag(request, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader list = BerReader(encoded, limits).children(request);
    BerTlvView external{};
    if (!next_child(list, external, error)) {
        return false;
    }
    if (!has_tag(external, kUniversal, 8, true) || !list.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(external);
    BerTlvView indirect_reference{};
    if (!next_child(fields, indirect_reference, error)) {
        return false;
    }
    if (!has_tag(indirect_reference, kUniversal, kInteger, false)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    std::uint64_t parsed_reference = 0;
    if (!parse_nonnegative_integer(indirect_reference.value, parsed_reference) ||
        parsed_reference > std::numeric_limits<std::uint32_t>::max()) {
        error = E2eCodecError::integer_out_of_range;
        return false;
    }

    BerTlvView single_asn1_type{};
    if (!next_child(fields, single_asn1_type, error)) {
        return false;
    }
    if (!has_tag(single_asn1_type, kContext, 0, true) || !fields.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    BerReader inner = BerReader(encoded, limits).children(single_asn1_type);
    BerTlvView handshake{};
    if (!next_child(inner, handshake, error)) {
        return false;
    }
    if (!inner.finished()) {
        error = E2eCodecError::trailing_data;
        return false;
    }
    if (!decode_handshake_request(handshake.encoded, result.handshake, error, limits)) {
        return false;
    }

    result.encoded = request.encoded;
    result.indirect_reference = static_cast<std::uint32_t>(parsed_reference);
    return true;
}

bool decode_osi_association_response(ByteView encoded,
                                     OsiAssociationResponseView& result,
                                     E2eCodecError& error,
                                     BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    BerTlvView response{};
    if (!read_single(encoded, limits, response, error)) {
        return false;
    }
    if (!has_tag(response, kUniversal, kSequence, true)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }

    BerReader list = BerReader(encoded, limits).children(response);
    BerTlvView external{};
    if (!next_child(list, external, error)) {
        return false;
    }
    if (!has_tag(external, kUniversal, 8, true) || !list.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    BerReader fields = BerReader(encoded, limits).children(external);
    BerTlvView indirect_reference{};
    if (!next_child(fields, indirect_reference, error)) {
        return false;
    }
    if (!has_tag(indirect_reference, kUniversal, kInteger, false)) {
        error = E2eCodecError::wrong_tag;
        return false;
    }
    std::uint64_t parsed_reference = 0;
    if (!parse_nonnegative_integer(indirect_reference.value, parsed_reference) ||
        parsed_reference > std::numeric_limits<std::uint32_t>::max()) {
        error = E2eCodecError::integer_out_of_range;
        return false;
    }

    BerTlvView single_asn1_type{};
    if (!next_child(fields, single_asn1_type, error)) {
        return false;
    }
    if (!has_tag(single_asn1_type, kContext, 0, true) || !fields.finished()) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    BerReader inner = BerReader(encoded, limits).children(single_asn1_type);
    BerTlvView choice{};
    if (!next_child(inner, choice, error)) {
        return false;
    }
    if (!inner.finished() || choice.tag.tag_class != kContext ||
        !choice.tag.constructed) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    switch (choice.tag.number) {
    case 1:
        result.choice = SignedMessageKind::handshake_accept;
        break;
    case 2:
        result.choice = SignedMessageKind::application_reject;
        break;
    case 3:
        result.choice = SignedMessageKind::handshake_security_reject;
        break;
    default:
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    if (!decode_signed_message(choice.encoded, result.choice,
                               result.message, error, limits)) {
        return false;
    }

    result.encoded = response.encoded;
    result.indirect_reference = static_cast<std::uint32_t>(parsed_reference);
    return true;
}

bool decode_osi_association_abort(ByteView encoded, OsiAbortView& result,
                                  E2eCodecError& error,
                                  BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    ByteView inner{};
    if (!decode_osi_external(encoded, result.encoded,
                             result.indirect_reference, inner,
                             error, limits)) {
        return false;
    }
    BerTlvView choice{};
    if (!read_single(inner, limits, choice, error) ||
        choice.tag.tag_class != kContext) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }
    if (choice.tag.number == 3) {
        if (choice.tag.constructed ||
            !parse_nonnegative_integer(choice.value, result.osi_diagnostic)) {
            error = E2eCodecError::invalid_field;
            return false;
        }
        result.has_osi_diagnostic = true;
        return true;
    }
    switch (choice.tag.number) {
    case 0:
        result.choice = SignedMessageKind::handshake_security_abort;
        break;
    case 1:
        result.choice = SignedMessageKind::transfer_security_abort;
        break;
    case 2:
        result.choice = SignedMessageKind::application_abort;
        break;
    default:
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    if (!choice.tag.constructed ||
        !decode_signed_message(inner, result.choice, result.message,
                               error, limits)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    return true;
}

bool decode_osi_release_request(ByteView encoded, OsiReleaseView& result,
                                E2eCodecError& error,
                                BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    ByteView inner{};
    if (!decode_osi_external(encoded, result.encoded,
                             result.indirect_reference, inner,
                             error, limits) ||
        !decode_signed_message(inner, SignedMessageKind::release_request,
                               result.message, error, limits)) {
        return false;
    }
    return true;
}

bool decode_osi_release_response(ByteView encoded, OsiReleaseView& result,
                                 E2eCodecError& error,
                                 BerLimits limits) noexcept {
    result = {};
    error = E2eCodecError::none;
    ByteView inner{};
    if (!decode_osi_external(encoded, result.encoded,
                             result.indirect_reference, inner,
                             error, limits) ||
        !decode_signed_message(inner, SignedMessageKind::release_response,
                               result.message, error, limits)) {
        return false;
    }
    return true;
}

bool encode_signature(ByteView algorithm_identifier, ByteView signature_value,
                      std::vector<std::uint8_t>& encoded,
                      E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    if (!validate_algorithm_identifier(algorithm_identifier, limits, error) ||
        !signature_value.valid() || signature_value.empty()) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }
    std::vector<std::uint8_t> content;
    if (!append_raw(content, algorithm_identifier) ||
        !append_tlv(content, 0x04, signature_value) ||
        !append_tlv(encoded, 0x30, {content.data(), content.size()})) {
        encoded.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    SignatureView parsed{};
    return decode_signature({encoded.data(), encoded.size()}, parsed, error, limits);
}

bool encode_handshake_request(ByteView protected_protocol, ByteView encoded_tbs,
                              ByteView signature_algorithm,
                              ByteView signature_value,
                              std::vector<std::uint8_t>& encoded,
                              E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    if (!valid_nmtoken_contents(protected_protocol) ||
        !read_expected_tag(encoded_tbs, kUniversal, kSequence, true,
                           limits, error)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }
    std::vector<std::uint8_t> signature;
    if (!encode_signature(signature_algorithm, signature_value, signature,
                          error, limits)) {
        return false;
    }
    std::vector<std::uint8_t> content;
    if (!append_tlv(content, 0x0c, protected_protocol) ||
        !append_raw(content, encoded_tbs) ||
        !append_raw(content, {signature.data(), signature.size()}) ||
        !append_tlv(encoded, 0x30, {content.data(), content.size()})) {
        encoded.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    HandshakeRequestView parsed{};
    if (!decode_handshake_request({encoded.data(), encoded.size()}, parsed,
                                  error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_signed_message(SignedMessageKind kind, ByteView encoded_tbs,
                           ByteView signature_algorithm,
                           ByteView signature_value,
                           std::vector<std::uint8_t>& encoded,
                           E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    if (!read_expected_tag(encoded_tbs, kUniversal, kSequence, true,
                           limits, error)) {
        return false;
    }
    std::vector<std::uint8_t> signature;
    if (!encode_signature(signature_algorithm, signature_value, signature,
                          error, limits)) {
        return false;
    }
    std::uint8_t outer_tag = 0x30;
    switch (kind) {
    case SignedMessageKind::handshake_accept:
        outer_tag = 0xa1;
        break;
    case SignedMessageKind::application_reject:
        outer_tag = 0xa2;
        break;
    case SignedMessageKind::handshake_security_reject:
        outer_tag = 0xa3;
        break;
    case SignedMessageKind::handshake_security_abort:
        outer_tag = 0xa0;
        break;
    case SignedMessageKind::transfer_security_abort:
        outer_tag = 0xa1;
        break;
    case SignedMessageKind::application_abort:
        outer_tag = 0xa2;
        break;
    case SignedMessageKind::release_request:
    case SignedMessageKind::release_response:
        break;
    default:
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    std::vector<std::uint8_t> content;
    if (!append_raw(content, encoded_tbs) ||
        !append_raw(content, {signature.data(), signature.size()}) ||
        !append_tlv(encoded, outer_tag, {content.data(), content.size()})) {
        encoded.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    SignedMessageView parsed{};
    if (!decode_signed_message({encoded.data(), encoded.size()}, kind,
                               parsed, error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_osi_association_request(std::uint32_t indirect_reference,
                                    ByteView encoded_handshake_request,
                                    std::vector<std::uint8_t>& encoded,
                                    E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    HandshakeRequestView parsed{};
    if (!decode_handshake_request(encoded_handshake_request, parsed, error, limits) ||
        !encode_osi_external(indirect_reference, encoded_handshake_request,
                             encoded, error)) {
        encoded.clear();
        return false;
    }
    OsiAssociationRequestView envelope{};
    if (!decode_osi_association_request({encoded.data(), encoded.size()}, envelope,
                                        error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_osi_association_response(std::uint32_t indirect_reference,
                                     ByteView encoded_handshake_response,
                                     std::vector<std::uint8_t>& encoded,
                                     E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    BerTlvView choice{};
    if (!read_single(encoded_handshake_response, limits, choice, error) ||
        choice.tag.tag_class != kContext || !choice.tag.constructed) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }
    SignedMessageKind kind{};
    switch (choice.tag.number) {
    case 1:
        kind = SignedMessageKind::handshake_accept;
        break;
    case 2:
        kind = SignedMessageKind::application_reject;
        break;
    case 3:
        kind = SignedMessageKind::handshake_security_reject;
        break;
    default:
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    SignedMessageView parsed{};
    if (!decode_signed_message(encoded_handshake_response, kind, parsed,
                               error, limits) ||
        !encode_osi_external(indirect_reference, encoded_handshake_response,
                             encoded, error)) {
        encoded.clear();
        return false;
    }
    OsiAssociationResponseView envelope{};
    if (!decode_osi_association_response({encoded.data(), encoded.size()},
                                         envelope, error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_osi_association_abort(std::uint32_t indirect_reference,
                                  ByteView encoded_abort_choice,
                                  std::vector<std::uint8_t>& encoded,
                                  E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    BerTlvView choice{};
    if (!read_single(encoded_abort_choice, limits, choice, error) ||
        choice.tag.tag_class != kContext) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }
    SignedMessageKind kind{};
    switch (choice.tag.number) {
    case 0:
        kind = SignedMessageKind::handshake_security_abort;
        break;
    case 1:
        kind = SignedMessageKind::transfer_security_abort;
        break;
    case 2:
        kind = SignedMessageKind::application_abort;
        break;
    case 3: {
        std::uint64_t diagnostic = 0;
        if (choice.tag.constructed ||
            !parse_nonnegative_integer(choice.value, diagnostic)) {
            error = E2eCodecError::invalid_field;
            return false;
        }
        if (!encode_osi_external(indirect_reference, encoded_abort_choice,
                                 encoded, error)) {
            return false;
        }
        OsiAbortView envelope{};
        if (!decode_osi_association_abort({encoded.data(), encoded.size()},
                                          envelope, error, limits)) {
            encoded.clear();
            return false;
        }
        return true;
    }
    default:
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    SignedMessageView parsed{};
    if (!choice.tag.constructed ||
        !decode_signed_message(encoded_abort_choice, kind, parsed, error, limits)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    if (!encode_osi_external(indirect_reference, encoded_abort_choice,
                             encoded, error)) {
        return false;
    }
    OsiAbortView envelope{};
    if (!decode_osi_association_abort({encoded.data(), encoded.size()},
                                      envelope, error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_osi_release_request(std::uint32_t indirect_reference,
                                ByteView encoded_release_request,
                                std::vector<std::uint8_t>& encoded,
                                E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    SignedMessageView parsed{};
    if (!decode_signed_message(encoded_release_request,
                               SignedMessageKind::release_request,
                               parsed, error, limits)) {
        encoded.clear();
        return false;
    }
    if (!encode_osi_external(indirect_reference, encoded_release_request,
                             encoded, error)) {
        return false;
    }
    OsiReleaseView envelope{};
    if (!decode_osi_release_request({encoded.data(), encoded.size()}, envelope,
                                   error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_osi_release_response(std::uint32_t indirect_reference,
                                 ByteView encoded_release_response,
                                 std::vector<std::uint8_t>& encoded,
                                 E2eCodecError& error, BerLimits limits) {
    encoded.clear();
    error = E2eCodecError::none;
    SignedMessageView parsed{};
    if (!decode_signed_message(encoded_release_response,
                               SignedMessageKind::release_response,
                               parsed, error, limits)) {
        encoded.clear();
        return false;
    }
    if (!encode_osi_external(indirect_reference, encoded_release_response,
                             encoded, error)) {
        return false;
    }
    OsiReleaseView envelope{};
    if (!decode_osi_release_response({encoded.data(), encoded.size()}, envelope,
                                    error, limits)) {
        encoded.clear();
        return false;
    }
    return true;
}

bool encode_clear_token2(const ClearToken2Fields& fields,
                         std::vector<std::uint8_t>& encoded,
                         E2eCodecError& error) {
    encoded.clear();
    error = E2eCodecError::none;
    if (!valid_association_id(fields.association_id) ||
        !fields.timestamp.valid() || fields.timestamp.empty() ||
        (fields.has_initialization_vector &&
         (!fields.initialization_vector.valid() || fields.initialization_vector.size != 16)) ||
        (fields.has_rekey_public_key &&
         (!fields.rekey_public_key.valid() || fields.rekey_public_key.empty()))) {
        error = E2eCodecError::invalid_field;
        return false;
    }

    std::vector<std::uint8_t> content;
    if (!append_tlv(content, 0x0c, fields.association_id) ||
        !append_tlv(content, 0x18, fields.timestamp) ||
        !append_unsigned_integer(content, 0x02, fields.sequence) ||
        (fields.has_initialization_vector &&
         !append_tlv(content, 0x80, fields.initialization_vector)) ||
        (fields.has_rekey_public_key &&
         !append_tlv(content, 0x81, fields.rekey_public_key))) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    const std::uint8_t true_value = 0xff;
    if ((fields.request_rekey && !append_tlv(content, 0x82, {&true_value, 1})) ||
        (fields.changed_key && !append_tlv(content, 0x83, {&true_value, 1})) ||
        !append_tlv(encoded, 0x30, {content.data(), content.size()})) {
        encoded.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    return true;
}

bool encode_authenticator(const AuthenticatorFields& fields,
                          std::vector<std::uint8_t>& encoded,
                          E2eCodecError& error) {
    encoded.clear();
    error = E2eCodecError::none;
    if ((fields.has_nonce && (!fields.nonce.valid() || fields.nonce.empty())) ||
        !fields.icv.valid() || fields.icv.empty() ||
        !validate_algorithm_identifier(fields.algorithm_identifier, {}, error)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }

    std::vector<std::uint8_t> content;
    if ((fields.has_nonce && !append_tlv(content, 0x04, fields.nonce)) ||
        !append_raw(content, fields.algorithm_identifier) ||
        !append_tlv(content, 0x04, fields.icv) ||
        !append_tlv(encoded, 0x30, {content.data(), content.size()})) {
        encoded.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    return true;
}

bool encode_tbp(ByteView encoded_token2, ByteView encoded_application_data,
                std::vector<std::uint8_t>& encoded_tbp,
                E2eCodecError& error, BerLimits limits) {
    encoded_tbp.clear();
    error = E2eCodecError::none;
    BerTlvView application_pdu{};
    if (!read_expected_tag(encoded_token2, kUniversal, kSequence, true,
                           limits, error) ||
        !read_single(encoded_application_data, limits, application_pdu, error)) {
        return false;
    }
    ClearToken2View token{};
    if (!decode_clear_token2(encoded_token2, token, error, limits) ||
        token.has_initialization_vector) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }

    std::vector<std::uint8_t> content;
    if (!append_raw(content, encoded_token2) ||
        !append_raw(content, encoded_application_data) ||
        !append_tlv(encoded_tbp, 0x30, {content.data(), content.size()})) {
        encoded_tbp.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    return true;
}

bool encode_clear_transfer(ByteView encoded_tbp, ByteView encoded_authenticator,
                           std::vector<std::uint8_t>& encoded_transfer,
                           E2eCodecError& error, BerLimits limits) {
    encoded_transfer.clear();
    error = E2eCodecError::none;
    if (!read_expected_tag(encoded_tbp, kUniversal, kSequence, true,
                           limits, error) ||
        !read_expected_tag(encoded_authenticator, kUniversal, kSequence, true,
                           limits, error)) {
        return false;
    }
    AuthenticatorView authenticator{};
    if (!decode_authenticator(encoded_authenticator, authenticator, error, limits)) {
        return false;
    }

    std::vector<std::uint8_t> content;
    if (!append_raw(content, encoded_tbp) ||
        !append_raw(content, encoded_authenticator) ||
        !append_tlv(encoded_transfer, 0x30, {content.data(), content.size()})) {
        encoded_transfer.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    ClearTransferView parsed{};
    if (!decode_clear_transfer({encoded_transfer.data(), encoded_transfer.size()},
                               parsed, error, limits)) {
        encoded_transfer.clear();
        return false;
    }
    return true;
}

bool encode_data_transfer_choice(DataTransferKind kind,
                                 ByteView encoded_transfer,
                                 std::vector<std::uint8_t>& encoded_choice,
                                 E2eCodecError& error, BerLimits limits) {
    encoded_choice.clear();
    error = E2eCodecError::none;
    if (kind != DataTransferKind::clear) {
        error = E2eCodecError::unsupported_transfer_choice;
        return false;
    }
    ClearTransferView transfer{};
    if (!decode_clear_transfer(encoded_transfer, transfer, error, limits)) {
        return false;
    }
    BerTlvView root{};
    if (!read_single(encoded_transfer, limits, root, error) ||
        !has_tag(root, kUniversal, kSequence, true)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::wrong_tag;
        }
        return false;
    }
    if (!append_tlv(encoded_choice, 0xa0, root.value)) {
        encoded_choice.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    return true;
}

bool encode_presentation_user_data(std::uint32_t presentation_context_id,
                                   ByteView encoded_data_transfer,
                                   std::vector<std::uint8_t>& encoded_user_data,
                                   E2eCodecError& error, BerLimits limits) {
    encoded_user_data.clear();
    error = E2eCodecError::none;
    if (presentation_context_id == 0) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    DataTransferKind kind{};
    ByteView transfer{};
    if (!decode_data_transfer(encoded_data_transfer, kind, transfer, error, limits) ||
        transfer.empty() ||
        (kind != DataTransferKind::clear && kind != DataTransferKind::encrypted)) {
        if (error == E2eCodecError::none) {
            error = E2eCodecError::invalid_field;
        }
        return false;
    }

    std::vector<std::uint8_t> entry_content;
    if (!append_unsigned_integer(entry_content, 0x02, presentation_context_id)) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    std::vector<std::uint8_t> explicit_content;
    if (!append_raw(explicit_content, encoded_data_transfer) ||
        !append_tlv(entry_content, 0xa0,
                    {explicit_content.data(), explicit_content.size()})) {
        error = E2eCodecError::invalid_field;
        return false;
    }
    std::vector<std::uint8_t> entry;
    if (!append_tlv(entry, 0x30, {entry_content.data(), entry_content.size()}) ||
        !append_tlv(encoded_user_data, 0x61, {entry.data(), entry.size()})) {
        encoded_user_data.clear();
        error = E2eCodecError::invalid_field;
        return false;
    }
    return true;
}

} // namespace ccli::mms::e2e
