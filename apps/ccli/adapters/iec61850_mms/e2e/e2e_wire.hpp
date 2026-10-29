#pragma once

#include "ber_codec.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace ccli::mms::e2e::wire {
using Bytes = std::vector<std::uint8_t>;
inline ByteView view(const Bytes& value) { return {value.data(), value.size()}; }
inline ByteView view(const std::string& value) {
    return {reinterpret_cast<const std::uint8_t*>(value.data()), value.size()};
}
inline bool equal(ByteView a, ByteView b) {
    return a.valid() && b.valid() && a.size == b.size &&
        (!a.size || std::memcmp(a.data, b.data, a.size) == 0);
}
inline void append(Bytes& destination, ByteView source) {
    if (!source.valid()) throw std::runtime_error("invalid byte view");
    if (source.size) destination.insert(destination.end(), source.data, source.data + source.size);
}
inline Bytes copy(ByteView source) { Bytes result; append(result, source); return result; }
inline Bytes tlv(std::uint8_t tag, ByteView contents) {
    Bytes result{tag};
    if (contents.size < 128) result.push_back(static_cast<std::uint8_t>(contents.size));
    else {
        Bytes length;
        for (auto size = contents.size; size; size >>= 8) length.insert(length.begin(), size & 255);
        result.push_back(0x80 | static_cast<std::uint8_t>(length.size()));
        append(result, view(length));
    }
    append(result, contents); return result;
}
inline Bytes join(std::initializer_list<ByteView> parts) {
    Bytes result; for (auto part : parts) append(result, part); return result;
}
inline Bytes integer(std::uint64_t value, std::uint8_t tag = 2) {
    Bytes result;
    do { result.insert(result.begin(), value & 255); value >>= 8; } while (value);
    if (result.front() & 128) result.insert(result.begin(), 0);
    return tlv(tag, view(result));
}
inline BerTlvView single(ByteView encoded) {
    BerReader reader(encoded); BerTlvView item; BerError error{};
    if (!reader.next(item, error) || !reader.finished()) throw std::runtime_error("invalid BER value");
    return item;
}
inline bool tag(const BerTlvView& item, std::uint8_t expected) {
    return item.encoded.size && item.encoded.data[0] == expected;
}
inline BerStringView string_value(const BerTlvView& item, unsigned type, bool implicit = false) {
    if (!implicit && (item.tag.tag_class != BerTagClass::universal || item.tag.number != type))
        throw std::runtime_error("unexpected BER string tag");
    BerStringView value; BerError error{};
    if (!ber_string_contents(item, type, value, error)) throw std::runtime_error("invalid BER string value");
    return value;
}
inline std::vector<BerTlvView> fields(ByteView encoded, std::uint8_t expected) {
    const auto outer = single(encoded);
    if (!tag(outer, expected)) throw std::runtime_error("unexpected BER tag");
    BerReader reader(outer.value); BerTlvView item; BerError error{};
    std::vector<BerTlvView> result;
    while (!reader.finished()) {
        if (!reader.next(item, error) || result.size() >= 128) throw std::runtime_error("invalid BER fields");
        result.push_back(item);
    }
    return result;
}
inline std::uint64_t number(ByteView contents) {
    if (contents.empty() || contents.size > 9 || (contents.data[0] & 128) ||
        (contents.size == 9 && contents.data[0]) ||
        (contents.size > 1 && contents.data[0] == 0 && !(contents.data[1] & 128)))
        throw std::runtime_error("invalid unsigned integer");
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < contents.size; ++i) value = (value << 8) | contents.data[i];
    return value;
}
// Shared policy for algorithms with absent parameters or optional RSA/HMAC
// NULL. A distinguishable context-specific addition starts the extension tail;
// later additions are opaque. Universal values at the parameters slot must
// satisfy the known algorithm policy, rather than silently becoming extensions.
inline bool simple_algorithm_matches(ByteView encoded, ByteView oid, bool allow_null) noexcept {
    try {
        if (encoded.size > 1024) return false;
        const auto values = fields(encoded, 0x30);
        if (values.empty() || !tag(values[0], 6) || !equal(values[0].value, oid)) return false;
        if (values.size() == 1) return true;
        if (values[1].tag.tag_class == BerTagClass::context_specific) return true;
        return allow_null && tag(values[1], 5) && values[1].value.empty();
    } catch (...) { return false; }
}
inline void oid_arc(Bytes& result, std::uint64_t value) {
    Bytes encoded{static_cast<std::uint8_t>(value & 127)};
    while ((value >>= 7)) encoded.insert(encoded.begin(), 128 | (value & 127));
    append(result, view(encoded));
}
inline Bytes oid_value(const std::string& text) {
    std::vector<std::uint64_t> arcs;
    std::size_t offset = 0;
    while (offset < text.size()) {
        const auto end = text.find('.', offset);
        const auto token = text.substr(offset, end == std::string::npos ? end : end - offset);
        if (token.empty() || token.size() > 10 || token.find_first_not_of("0123456789") != std::string::npos)
            throw std::runtime_error("invalid OID");
        arcs.push_back(std::stoull(token));
        if (arcs.back() > 0xffffffffULL || arcs.size() > 32) throw std::runtime_error("OID arc out of range");
        if (end == std::string::npos) break;
        offset = end + 1;
        if (offset == text.size()) throw std::runtime_error("invalid OID");
    }
    if (arcs.size() < 2 || arcs[0] > 2 || (arcs[0] < 2 && arcs[1] > 39)) throw std::runtime_error("invalid OID");
    Bytes result; oid_arc(result, arcs[0] * 40 + arcs[1]);
    for (std::size_t i = 2; i < arcs.size(); ++i) oid_arc(result, arcs[i]);
    return result;
}
inline std::vector<std::uint32_t> oid_arcs(ByteView value) {
    std::vector<std::uint32_t> result;
    std::uint64_t arc = 0; unsigned octets = 0;
    for (std::size_t i = 0; i < value.size; ++i) {
        const auto byte = value.data[i];
        if (++octets > 5 || (octets == 1 && byte == 0x80)) throw std::runtime_error("invalid OID arc");
        arc = (arc << 7) | (byte & 127);
        if (!(byte & 128)) {
            if (result.empty()) {
                const unsigned first = arc < 40 ? 0 : arc < 80 ? 1 : 2;
                result.push_back(first); arc -= first * 40;
            }
            if (arc > 0xffffffffULL) throw std::runtime_error("OID arc too large");
            result.push_back(static_cast<std::uint32_t>(arc));
            arc = 0; octets = 0;
        }
    }
    if (octets || result.size() < 2 || result.size() > 10) throw std::runtime_error("AP-title outside stack limits");
    return result;
}
inline Bytes algorithm(const std::string& oid) { const auto id = tlv(6, view(oid_value(oid))); return tlv(0x30, view(id)); }
inline void erase(Bytes& bytes) noexcept {
    volatile std::uint8_t* p = bytes.data();
    for (std::size_t i = 0; i < bytes.size(); ++i) p[i] = 0;
    bytes.clear();
}
} // namespace ccli::mms::e2e::wire
