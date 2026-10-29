#include "e2e_identity.hpp"

#include <algorithm>
#include <array>

#if defined(CONFIG_MMS_SUPPORT_TLS)
#include "mbedtls/asn1.h"
#include "mbedtls/md.h"
#include "mbedtls/oid.h"
#include "mbedtls/pk.h"
#include "mbedtls/version.h"
#include "mbedtls/x509_crl.h"
#include "mbedtls/x509_crt.h"
#include "e2e_mbedtls_crl.hpp"
#endif

namespace ccli::mms::e2e {
namespace {

constexpr std::size_t kMaxCertificateBytes = 256u * 1024u;
constexpr std::size_t kMaxCertificatePathItems = 8;
constexpr std::size_t kMaxTrustAnchors = 4;
constexpr std::size_t kMaxCrls = 8;
constexpr std::size_t kMaxTotalDerBytes = 1024u * 1024u;
constexpr std::size_t kMaxX509Extensions = 64;
constexpr std::size_t kMaxDistributionPoints = 16;
constexpr std::uint8_t kObjectIdentifierAttributeOid[] = {0x55, 0x04, 0x6a};
constexpr std::uint8_t kCrlDistributionPointsOid[] = {0x55, 0x1d, 0x1f};
constexpr std::uint8_t kIssuingDistributionPointOid[] = {0x55, 0x1d, 0x1c};
constexpr std::uint8_t kDeltaCrlIndicatorOid[] = {0x55, 0x1d, 0x1b};
constexpr std::uint8_t kCertificateIssuerOid[] = {0x55, 0x1d, 0x1d};

bool valid(ByteView bytes) noexcept {
    return bytes.valid();
}

#if defined(CONFIG_MMS_SUPPORT_TLS)
struct DerTlv {
    std::uint8_t tag{0};
    ByteView encoded{};
    ByteView value{};
};

bool read_der_tlv(ByteView input, DerTlv& result, std::size_t& consumed) noexcept {
    if (!input.valid() || input.data == nullptr || input.size < 2 ||
        (input.data[0] & 0x1fu) == 0x1fu) {
        return false;
    }

    std::size_t header_size = 2;
    std::size_t value_size = 0;
    const std::uint8_t first_length = input.data[1];
    if ((first_length & 0x80u) == 0) {
        value_size = first_length;
    } else {
        const std::size_t length_octets = first_length & 0x7fu;
        if (length_octets == 0 || length_octets > sizeof(std::size_t) ||
            length_octets > input.size - 2 || input.data[2] == 0) {
            return false;
        }
        header_size += length_octets;
        for (std::size_t i = 0; i < length_octets; ++i) {
            if (value_size > (static_cast<std::size_t>(-1) >> 8)) {
                return false;
            }
            value_size = (value_size << 8) | input.data[2 + i];
        }
        if (value_size < 128) {
            return false;
        }
    }
    if (value_size > input.size - header_size) {
        return false;
    }

    consumed = header_size + value_size;
    result.tag = input.data[0];
    result.encoded = {input.data, consumed};
    result.value = {input.data + header_size, value_size};
    return true;
}

bool read_only_der_tlv(ByteView input, std::uint8_t expected_tag,
                       DerTlv& result) noexcept {
    std::size_t consumed = 0;
    return read_der_tlv(input, result, consumed) && consumed == input.size &&
           result.tag == expected_tag;
}

bool read_sequence(ByteView encoded, ByteView& contents) noexcept {
    DerTlv sequence;
    if (!read_only_der_tlv(encoded, 0x30, sequence)) {
        return false;
    }
    contents = sequence.value;
    return true;
}

bool same_bytes(ByteView lhs, ByteView rhs) noexcept {
    return lhs.valid() && rhs.valid() && lhs.size == rhs.size &&
           (lhs.size == 0 || std::equal(lhs.data, lhs.data + lhs.size, rhs.data));
}

struct ExtensionValue {
    bool present{false};
    bool critical{false};
    ByteView value{};
};

struct ExtensionTarget {
    const std::uint8_t* oid{nullptr};
    std::size_t oid_size{0};
    ExtensionValue* result{nullptr};
};

bool parse_extensions(ByteView encoded, ExtensionTarget* targets,
                      std::size_t target_count,
                      bool* unsupported_critical = nullptr) noexcept {
    if (unsupported_critical != nullptr) {
        *unsupported_critical = false;
    }
    if (encoded.size == 0) {
        return encoded.valid();
    }
    ByteView contents;
    if (!read_sequence(encoded, contents)) {
        return false;
    }

    std::size_t offset = 0;
    std::size_t extension_count = 0;
    while (offset < contents.size) {
        if (++extension_count > kMaxX509Extensions) {
            return false;
        }
        DerTlv extension;
        std::size_t consumed = 0;
        if (!read_der_tlv({contents.data + offset, contents.size - offset},
                          extension, consumed) || extension.tag != 0x30) {
            return false;
        }
        offset += consumed;

        std::size_t field_offset = 0;
        DerTlv oid;
        if (!read_der_tlv(extension.value, oid, consumed) || oid.tag != 0x06 ||
            oid.value.size == 0) {
            return false;
        }
        field_offset += consumed;

        bool critical = false;
        if (field_offset < extension.value.size &&
            extension.value.data[field_offset] == 0x01) {
            DerTlv boolean;
            if (!read_der_tlv({extension.value.data + field_offset,
                               extension.value.size - field_offset},
                              boolean, consumed) || boolean.tag != 0x01 ||
                boolean.value.size != 1 ||
                (boolean.value.data[0] != 0x00 && boolean.value.data[0] != 0xff)) {
                return false;
            }
            critical = boolean.value.data[0] != 0;
            field_offset += consumed;
        }

        DerTlv octets;
        if (!read_der_tlv({extension.value.data + field_offset,
                           extension.value.size - field_offset},
                          octets, consumed) || octets.tag != 0x04 ||
            field_offset + consumed != extension.value.size) {
            return false;
        }

        std::size_t matched_target = target_count;
        for (std::size_t i = 0; i < target_count; ++i) {
            if (targets[i].oid != nullptr &&
                oid.value.size == targets[i].oid_size &&
                std::equal(targets[i].oid, targets[i].oid + targets[i].oid_size,
                           oid.value.data)) {
                if (matched_target != target_count) {
                    return false;
                }
                matched_target = i;
            }
        }
        if (matched_target != target_count) {
            auto& target = *targets[matched_target].result;
            if (target.present) {
                return false;
            }
            target.present = true;
            target.critical = critical;
            target.value = octets.value;
        } else if (critical && unsupported_critical != nullptr) {
            *unsupported_critical = true;
        }
    }
    return true;
}

struct DistributionPointName {
    bool present{false};
    bool full_name{false};
    ByteView general_names{};
};

bool valid_general_names(ByteView names, std::size_t* count_out = nullptr) noexcept {
    std::size_t offset = 0;
    std::size_t count = 0;
    while (offset < names.size) {
        DerTlv name;
        std::size_t consumed = 0;
        if (!read_der_tlv({names.data + offset, names.size - offset}, name, consumed)) {
            return false;
        }
        const bool valid_tag = name.tag == 0xa0 || name.tag == 0x81 ||
                               name.tag == 0x82 || name.tag == 0xa3 ||
                               name.tag == 0xa4 || name.tag == 0xa5 ||
                               name.tag == 0x86 || name.tag == 0x87 ||
                               name.tag == 0x88;
        if (!valid_tag) {
            return false;
        }
        offset += consumed;
        ++count;
    }
    if (count_out != nullptr) {
        *count_out = count;
    }
    return count != 0;
}

bool parse_distribution_point_name(ByteView explicit_value,
                                   DistributionPointName& result) noexcept {
    DerTlv choice;
    if (!read_only_der_tlv(explicit_value, 0xa0, choice)) {
        return false;
    }
    DerTlv name_choice;
    if (!read_only_der_tlv(choice.value, 0xa0, name_choice)) {
        // nameRelativeToCRLIssuer (A1) needs DN construction and is rejected
        // conservatively; it cannot be used to prove matching CRL scope here.
        return false;
    }
    if (!valid_general_names(name_choice.value)) {
        return false;
    }
    result.present = true;
    result.full_name = true;
    result.general_names = name_choice.value;
    return true;
}

bool general_names_intersect(ByteView lhs, ByteView rhs) noexcept {
    if (!valid_general_names(lhs) || !valid_general_names(rhs)) {
        return false;
    }
    for (std::size_t lhs_offset = 0; lhs_offset < lhs.size;) {
        DerTlv lhs_name;
        std::size_t lhs_consumed = 0;
        if (!read_der_tlv({lhs.data + lhs_offset, lhs.size - lhs_offset},
                          lhs_name, lhs_consumed)) {
            return false;
        }
        for (std::size_t rhs_offset = 0; rhs_offset < rhs.size;) {
            DerTlv rhs_name;
            std::size_t rhs_consumed = 0;
            if (!read_der_tlv({rhs.data + rhs_offset, rhs.size - rhs_offset},
                              rhs_name, rhs_consumed)) {
                return false;
            }
            if (same_bytes(lhs_name.encoded, rhs_name.encoded)) {
                return true;
            }
            rhs_offset += rhs_consumed;
        }
        lhs_offset += lhs_consumed;
    }
    return false;
}

struct CrlScope {
    bool has_idp{false};
    bool only_user_certificates{false};
    bool only_ca_certificates{false};
    bool only_attribute_certificates{false};
    bool indirect_crl{false};
    bool partial_reasons{false};
    DistributionPointName name{};
};

bool parse_der_boolean(const DerTlv& value, bool& result) noexcept {
    if (value.value.size != 1 ||
        (value.value.data[0] != 0x00 && value.value.data[0] != 0xff)) {
        return false;
    }
    result = value.value.data[0] != 0;
    return true;
}

bool valid_bit_string_contents(ByteView value) noexcept {
    if (!value.valid() || value.size < 2 || value.data[0] > 7) {
        return false;
    }
    const std::uint8_t unused_bits = value.data[0];
    if (unused_bits != 0 &&
        (value.data[value.size - 1] &
         static_cast<std::uint8_t>((1u << unused_bits) - 1u)) != 0) {
        return false;
    }
    return true;
}

bool parse_crl_scope(const mbedtls_x509_crl& crl, CrlScope& scope) noexcept {
    ExtensionValue idp;
    ExtensionValue delta;
    ExtensionTarget targets[] = {
        {kIssuingDistributionPointOid, sizeof(kIssuingDistributionPointOid), &idp},
        {kDeltaCrlIndicatorOid, sizeof(kDeltaCrlIndicatorOid), &delta},
    };
    bool unsupported_critical = false;
    const ByteView extensions{crl.crl_ext.p, crl.crl_ext.len};
    if (!parse_extensions(extensions, targets, 2, &unsupported_critical) ||
        unsupported_critical || delta.present) {
        return false;
    }
    for (auto* entry = &crl.entry; entry != nullptr; entry = entry->next) {
        ExtensionValue certificate_issuer;
        ExtensionTarget entry_target{ kCertificateIssuerOid,
                                      sizeof(kCertificateIssuerOid),
                                      &certificate_issuer };
        bool unsupported_entry_critical = false;
        const ByteView entry_extensions{entry->entry_ext.p, entry->entry_ext.len};
        if (!parse_extensions(entry_extensions, &entry_target, 1,
                              &unsupported_entry_critical) ||
            certificate_issuer.present || unsupported_entry_critical) {
            return false;
        }
    }
    if (!idp.present) {
        return true;
    }
    if (!idp.critical) {
        return false;
    }

    ByteView contents;
    if (!read_sequence(idp.value, contents)) {
        return false;
    }
    scope.has_idp = true;
    unsigned last_field = 0;
    bool saw_field = false;
    std::size_t offset = 0;
    while (offset < contents.size) {
        DerTlv field;
        std::size_t consumed = 0;
        if (!read_der_tlv({contents.data + offset, contents.size - offset},
                          field, consumed)) {
            return false;
        }
        offset += consumed;
        unsigned field_number = 0;
        switch (field.tag) {
        case 0xa0: field_number = 0; break;
        case 0x81: field_number = 1; break;
        case 0x82: field_number = 2; break;
        case 0x83: field_number = 3; break;
        case 0x84: field_number = 4; break;
        case 0x85: field_number = 5; break;
        default: return false;
        }
        if (saw_field && field_number <= last_field) {
            return false;
        }
        saw_field = true;
        last_field = field_number;

        bool flag = false;
        switch (field.tag) {
        case 0xa0:
            if (!parse_distribution_point_name(field.encoded, scope.name)) {
                return false;
            }
            break;
        case 0x81:
            if (!parse_der_boolean(field, flag) || !flag) return false;
            scope.only_user_certificates = true;
            break;
        case 0x82:
            if (!parse_der_boolean(field, flag) || !flag) return false;
            scope.only_ca_certificates = true;
            break;
        case 0x83:
            if (!valid_bit_string_contents(field.value)) {
                return false;
            }
            scope.partial_reasons = true;
            break;
        case 0x84:
            if (!parse_der_boolean(field, flag) || !flag) return false;
            scope.indirect_crl = true;
            break;
        case 0x85:
            if (!parse_der_boolean(field, flag) || !flag) return false;
            scope.only_attribute_certificates = true;
            break;
        default: return false;
        }
    }
    return !(scope.only_user_certificates && scope.only_ca_certificates);
}

struct CertificateDistributionPoint {
    DistributionPointName name{};
    bool has_reasons{false};
    bool has_crl_issuer{false};
};

bool parse_certificate_distribution_point(ByteView encoded,
                                          CertificateDistributionPoint& result) noexcept {
    ByteView contents;
    if (!read_sequence(encoded, contents)) {
        return false;
    }
    unsigned last_field = 0;
    bool saw_field = false;
    std::size_t offset = 0;
    while (offset < contents.size) {
        DerTlv field;
        std::size_t consumed = 0;
        if (!read_der_tlv({contents.data + offset, contents.size - offset},
                          field, consumed)) {
            return false;
        }
        offset += consumed;
        unsigned field_number = 0;
        switch (field.tag) {
        case 0xa0: field_number = 0; break;
        case 0x81: field_number = 1; break;
        case 0xa2: field_number = 2; break;
        default: return false;
        }
        if (saw_field && field_number <= last_field) {
            return false;
        }
        saw_field = true;
        last_field = field_number;

        if (field.tag == 0xa0) {
            if (!parse_distribution_point_name(field.encoded, result.name)) {
                return false;
            }
        } else if (field.tag == 0x81) {
            if (!valid_bit_string_contents(field.value)) {
                return false;
            }
            result.has_reasons = true;
        } else {
            if (!valid_general_names(field.value)) {
                return false;
            }
            result.has_crl_issuer = true;
        }
    }
    return true;
}

bool certificate_distribution_points_cover(const mbedtls_x509_crt& certificate,
                                           bool is_leaf, const CrlScope& scope) noexcept {
    ExtensionValue distribution_points;
    ExtensionTarget target{ kCrlDistributionPointsOid,
                            sizeof(kCrlDistributionPointsOid),
                            &distribution_points };
    const ByteView extensions{certificate.v3_ext.p, certificate.v3_ext.len};
    if (!parse_extensions(extensions, &target, 1)) {
        return false;
    }
    if (!distribution_points.present) {
        // A supplied intermediate without CDP can only be covered by a full,
        // issuer-wide CRL. The Annex G leaf always requires a CDP.
        return !is_leaf && !scope.has_idp;
    }
    if (distribution_points.critical) {
        return false;
    }

    ByteView points;
    if (!read_sequence(distribution_points.value, points)) {
        return false;
    }
    std::size_t offset = 0;
    std::size_t point_count = 0;
    bool covered = false;
    while (offset < points.size) {
        if (++point_count > kMaxDistributionPoints) {
            return false;
        }
        DerTlv point;
        std::size_t consumed = 0;
        if (!read_der_tlv({points.data + offset, points.size - offset},
                          point, consumed) || point.tag != 0x30) {
            return false;
        }
        offset += consumed;

        CertificateDistributionPoint parsed;
        if (!parse_certificate_distribution_point(point.encoded, parsed)) {
            return false;
        }
        // Annex G's direct-issuer form requires a distributionPoint and omits
        // reasons and cRLIssuer. Indirect CRLs are deliberately unsupported.
        if (!parsed.name.present || parsed.has_reasons || parsed.has_crl_issuer) {
            if (is_leaf && parsed.has_reasons) {
                return false;
            }
            continue;
        }
        if (!scope.has_idp || !scope.name.present) {
            covered = true;
        } else if (parsed.name.full_name && scope.name.full_name &&
                   general_names_intersect(parsed.name.general_names,
                                           scope.name.general_names)) {
            covered = true;
        }
    }
    return point_count != 0 && covered;
}

struct ParsedCertificates {
    mbedtls_x509_crt value;
    ParsedCertificates() { mbedtls_x509_crt_init(&value); }
    ~ParsedCertificates() { mbedtls_x509_crt_free(&value); }
};

struct ParsedCrls {
    mbedtls_x509_crl value;
    ParsedCrls() { mbedtls_x509_crl_init(&value); }
    ~ParsedCrls() { mbedtls_x509_crl_free(&value); }
};

struct PeerExtensionState {
    bool has_noncritical_crl_distribution_points{false};
};

#if MBEDTLS_VERSION_MAJOR >= 3
int inspect_peer_extension(void* context, const mbedtls_x509_crt*,
                           const mbedtls_x509_buf* oid, int critical,
                           const unsigned char*, const unsigned char*) {
    auto* state = static_cast<PeerExtensionState*>(context);
    const bool is_crl_distribution_points =
        oid->tag == MBEDTLS_ASN1_OID && oid->len == sizeof(kCrlDistributionPointsOid) &&
        std::equal(kCrlDistributionPointsOid,
                   kCrlDistributionPointsOid + sizeof(kCrlDistributionPointsOid), oid->p);
    if (is_crl_distribution_points && critical == 0) {
        state->has_noncritical_crl_distribution_points = true;
    }
    // Unknown critical extensions are not safe to ignore. Mbed TLS skips
    // unknown non-critical extensions when this callback returns success.
    return critical != 0 ? MBEDTLS_ERR_X509_INVALID_EXTENSIONS : 0;
}
#else
bool peer_has_noncritical_crl_distribution_points(const mbedtls_x509_crt& certificate) noexcept {
    if (!certificate.v3_ext.p || certificate.v3_ext.len == 0) {
        return false;
    }
    unsigned char* cursor = certificate.v3_ext.p;
    const unsigned char* end = cursor + certificate.v3_ext.len;
    size_t sequence_size = 0;
    if (mbedtls_asn1_get_tag(&cursor, end, &sequence_size,
                             MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0 ||
        cursor + sequence_size != end) {
        return false;
    }
    const unsigned char* extensions_end = cursor + sequence_size;
    while (cursor < extensions_end) {
        size_t extension_size = 0;
        if (mbedtls_asn1_get_tag(&cursor, extensions_end, &extension_size,
                                 MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0) {
            return false;
        }
        const unsigned char* extension_end = cursor + extension_size;
        size_t oid_size = 0;
        if (mbedtls_asn1_get_tag(&cursor, extension_end, &oid_size,
                                 MBEDTLS_ASN1_OID) != 0) {
            return false;
        }
        const bool is_crl_distribution_points =
            oid_size == sizeof(kCrlDistributionPointsOid) &&
            std::equal(kCrlDistributionPointsOid,
                       kCrlDistributionPointsOid + sizeof(kCrlDistributionPointsOid), cursor);
        cursor += oid_size;
        int critical = 0;
        if (cursor < extension_end && *cursor == MBEDTLS_ASN1_BOOLEAN &&
            mbedtls_asn1_get_bool(&cursor, extension_end, &critical) != 0) {
            return false;
        }
        size_t value_size = 0;
        if (mbedtls_asn1_get_tag(&cursor, extension_end, &value_size,
                                 MBEDTLS_ASN1_OCTET_STRING) != 0) {
            return false;
        }
        cursor += value_size;
        if (cursor != extension_end) {
            return false;
        }
        if (is_crl_distribution_points) {
            return critical == 0;
        }
    }
    return false;
}
#endif

int parse_peer_certificate(mbedtls_x509_crt& certificate, ByteView der,
                           PeerExtensionState& extensions) {
#if MBEDTLS_VERSION_MAJOR >= 3
    return mbedtls_x509_crt_parse_der_with_ext_cb(
        &certificate, der.data, der.size, 1, inspect_peer_extension, &extensions);
#else
    const int result = mbedtls_x509_crt_parse_der(&certificate, der.data, der.size);
    if (result == 0) {
        extensions.has_noncritical_crl_distribution_points =
            peer_has_noncritical_crl_distribution_points(certificate);
    }
    return result;
#endif
}

bool equal(ByteView lhs, ByteView rhs) noexcept {
    return same_bytes(lhs, rhs);
}

bool within_bounds(const E2ePeerCertificatePath& path, ByteView leaf) noexcept {
    if (!valid(leaf) || leaf.empty() || leaf.size > kMaxCertificateBytes ||
        !valid(path.expected_identity_oid_value) ||
        path.expected_identity_oid_value.empty() ||
        path.expected_identity_oid_value.size > 128 ||
        path.intermediates.size() + 1 > kMaxCertificatePathItems ||
        path.trust_anchors.empty() || path.trust_anchors.size() > kMaxTrustAnchors ||
        path.issuer_crls.size() > kMaxCrls) {
        return false;
    }

    std::size_t total = leaf.size;
    const auto add = [&total](const std::vector<ByteView>& values,
                              std::size_t maximum_count) noexcept {
        if (values.size() > maximum_count) {
            return false;
        }
        for (const auto value : values) {
            if (!valid(value) || value.empty() || value.size > kMaxCertificateBytes ||
                total > kMaxTotalDerBytes - value.size) {
                return false;
            }
            total += value.size;
        }
        return true;
    };

    return total <= kMaxTotalDerBytes &&
           add(path.intermediates, kMaxCertificatePathItems - 1) &&
           add(path.trust_anchors, kMaxTrustAnchors) &&
           add(path.issuer_crls, kMaxCrls);
}

bool parse_certificates(ParsedCertificates& destination,
                        const std::vector<ByteView>& encoded) noexcept {
    for (const auto bytes : encoded) {
        if (mbedtls_x509_crt_parse_der(&destination.value, bytes.data, bytes.size) != 0) {
            return false;
        }
    }
    return true;
}

bool has_nonnegative_serial(const mbedtls_x509_crt& certificate) noexcept {
    const auto& serial = certificate.serial;
    if (!serial.p || serial.len == 0 || (serial.p[0] & 0x80u) != 0) {
        return false;
    }
    return serial.len == 1 || serial.p[0] != 0 || (serial.p[1] & 0x80u) != 0;
}

bool has_extension(const mbedtls_x509_crt& certificate, int extension) noexcept {
#if MBEDTLS_VERSION_MAJOR >= 3
    return mbedtls_x509_crt_has_ext_type(&certificate, extension) != 0;
#else
    return (certificate.ext_types & extension) != 0;
#endif
}

bool has_required_key_usage(const mbedtls_x509_crt& certificate) noexcept {
    if (!has_extension(certificate, MBEDTLS_X509_EXT_KEY_USAGE)) {
        return false;
    }
    constexpr unsigned int required = MBEDTLS_X509_KU_DIGITAL_SIGNATURE |
                                      MBEDTLS_X509_KU_KEY_AGREEMENT;
    return mbedtls_x509_crt_check_key_usage(&certificate, required) == 0;
}

bool supported_certificate_key(const mbedtls_x509_crt& certificate) noexcept {
    if (mbedtls_pk_can_do(&certificate.pk, MBEDTLS_PK_RSA)) {
        return mbedtls_pk_get_bitlen(&certificate.pk) >= 2048;
    }
    if (mbedtls_pk_can_do(&certificate.pk, MBEDTLS_PK_ECDSA)) {
        return mbedtls_pk_get_bitlen(&certificate.pk) >= 256;
    }
    return false;
}

bool matches_subject_identity(const mbedtls_x509_crt& certificate,
                              ByteView expected_oid_value) noexcept {
    const auto& name = certificate.subject;
    return name.next == nullptr &&
           name.oid.tag == MBEDTLS_ASN1_OID &&
           name.oid.len == sizeof(kObjectIdentifierAttributeOid) &&
           std::equal(kObjectIdentifierAttributeOid,
                      kObjectIdentifierAttributeOid + sizeof(kObjectIdentifierAttributeOid),
                      name.oid.p) &&
           name.val.tag == MBEDTLS_ASN1_OID &&
           name.val.len == expected_oid_value.size &&
           expected_oid_value.valid() &&
           std::equal(name.val.p, name.val.p + name.val.len,
                      expected_oid_value.data);
}

bool has_crl_for_issuer(const mbedtls_x509_crt& certificate,
                        const mbedtls_x509_crl& crl) noexcept {
    const ByteView issuer{certificate.issuer_raw.p, certificate.issuer_raw.len};
    return equal(issuer, {crl.issuer_raw.p, crl.issuer_raw.len});
}

struct CrlCandidate {
    ByteView encoded{};
    const mbedtls_x509_crl* parsed{nullptr};
    CrlScope scope{};
    bool scope_valid{false};
};

bool crl_covers_certificate(const mbedtls_x509_crt& certificate, bool is_leaf,
                            const CrlCandidate& crl) noexcept {
    if (!crl.scope_valid || crl.parsed == nullptr ||
        !has_crl_for_issuer(certificate, *crl.parsed) ||
        crl.scope.indirect_crl || crl.scope.only_attribute_certificates ||
        crl.scope.partial_reasons) {
        return false;
    }
    bool is_ca = false;
    if (has_extension(certificate, MBEDTLS_X509_EXT_BASIC_CONSTRAINTS)) {
#if MBEDTLS_VERSION_MAJOR >= 3
        is_ca = mbedtls_x509_crt_get_ca_istrue(&certificate) != 0;
#else
        is_ca = certificate.ca_istrue != 0;
#endif
    }
    if ((crl.scope.only_user_certificates && is_ca) ||
        (crl.scope.only_ca_certificates && !is_ca)) {
        return false;
    }
    return certificate_distribution_points_cover(certificate, is_leaf, crl.scope);
}

bool certificate_has_crl_coverage(const mbedtls_x509_crt& certificate, bool is_leaf,
                                 const std::array<CrlCandidate, kMaxCrls>& candidates,
                                 std::size_t candidate_count) noexcept {
    for (std::size_t i = 0; i < candidate_count; ++i) {
        if (crl_covers_certificate(certificate, is_leaf, candidates[i])) {
            return true;
        }
    }
    return false;
}

bool has_crl_coverage(const mbedtls_x509_crt& leaf,
                      const mbedtls_x509_crt& intermediates,
                      const std::array<CrlCandidate, kMaxCrls>& candidates,
                      std::size_t candidate_count) noexcept {
    if (!certificate_has_crl_coverage(leaf, true, candidates, candidate_count)) {
        return false;
    }
    // Mbed TLS uses an initialized, empty head node when the path has no
    // intermediates. Do not treat that sentinel as a certificate.
    if (intermediates.version != 0) {
        for (auto* certificate = &intermediates; certificate != nullptr;
             certificate = certificate->next) {
            if (!certificate_has_crl_coverage(*certificate, false, candidates,
                                              candidate_count)) {
                return false;
            }
        }
    }
    return true;
}

bool build_verification_crls(const mbedtls_x509_crt& leaf,
                             const mbedtls_x509_crt& intermediates,
                             const std::array<CrlCandidate, kMaxCrls>& candidates,
                             std::size_t candidate_count,
                             ParsedCrls& destination,
                             bool& coverage_available) noexcept {
    std::array<bool, kMaxCrls> include{};
    for (std::size_t i = 0; i < candidate_count; ++i) {
        const auto& candidate = candidates[i];
        if (!candidate.scope_valid || candidate.parsed == nullptr) {
            continue;
        }
        bool relevant = false;
        bool applies_to_all_matching_issuers = true;
        if (has_crl_for_issuer(leaf, *candidate.parsed)) {
            relevant = true;
            applies_to_all_matching_issuers =
                crl_covers_certificate(leaf, true, candidate);
        }
        if (intermediates.version != 0) {
            for (auto* certificate = &intermediates; certificate != nullptr;
                 certificate = certificate->next) {
                if (has_crl_for_issuer(*certificate, *candidate.parsed)) {
                    relevant = true;
                    applies_to_all_matching_issuers =
                        applies_to_all_matching_issuers &&
                        crl_covers_certificate(*certificate, false, candidate);
                }
            }
        }
        // Do not give Mbed TLS a same-issuer CRL whose scope does not cover
        // every certificate it could otherwise be selected for.
        include[i] = relevant && applies_to_all_matching_issuers;
    }

    const auto covered_by_selected_crl = [&candidates, &include, candidate_count](
                                             const mbedtls_x509_crt& certificate,
                                             bool is_leaf) noexcept {
        for (std::size_t i = 0; i < candidate_count; ++i) {
            if (include[i] && crl_covers_certificate(certificate, is_leaf,
                                                     candidates[i])) {
                return true;
            }
        }
        return false;
    };
    coverage_available = covered_by_selected_crl(leaf, true);
    if (intermediates.version != 0) {
        for (auto* certificate = &intermediates; certificate != nullptr;
             certificate = certificate->next) {
            coverage_available = coverage_available &&
                                 covered_by_selected_crl(*certificate, false);
        }
    }
    if (!coverage_available) {
        return true;
    }

    for (std::size_t i = 0; i < candidate_count; ++i) {
        if (include[i] &&
            parse_native_crl_der(&destination.value, candidates[i].encoded.data,
                                       candidates[i].encoded.size) != 0) {
            return false;
        }
    }
    return true;
}
#endif

} // namespace

IdentityError validate_peer_certificate(ByteView leaf_der,
                                        const E2ePeerCertificatePath& path) noexcept {
#if defined(CONFIG_MMS_SUPPORT_TLS) && defined(MBEDTLS_VERSION_MAJOR) && MBEDTLS_VERSION_MAJOR >= 3
    if (!within_bounds(path, leaf_der)) {
        return IdentityError::invalid_input;
    }

    ParsedCertificates leaf;
    ParsedCertificates intermediates;
    ParsedCertificates trust_anchors;
    ParsedCrls crls;
    PeerExtensionState peer_extensions;
    if (parse_peer_certificate(leaf.value, leaf_der, peer_extensions) != 0) {
        return IdentityError::invalid_certificate;
    }
    if (!parse_certificates(intermediates, path.intermediates)) {
        return IdentityError::invalid_intermediate_chain;
    }
    if (!parse_certificates(trust_anchors, path.trust_anchors)) {
        return IdentityError::invalid_trust_anchor;
    }
    std::array<CrlCandidate, kMaxCrls> crl_candidates{};
    std::size_t crl_candidate_count = 0;
    for (const auto encoded : path.issuer_crls) {
        if (parse_native_crl_der(&crls.value, encoded.data, encoded.size) != 0) {
            return IdentityError::invalid_crl;
        }
        crl_candidates[crl_candidate_count++].encoded = encoded;
    }
    auto* parsed_crl = crls.value.version == 0 ? crls.value.next : &crls.value;
    for (std::size_t i = 0; i < crl_candidate_count; ++i) {
        if (parsed_crl == nullptr) {
            return IdentityError::invalid_crl;
        }
        crl_candidates[i].parsed = parsed_crl;
        crl_candidates[i].scope_valid = parse_crl_scope(*parsed_crl,
                                                        crl_candidates[i].scope);
        parsed_crl = parsed_crl->next;
    }

    const auto& peer = leaf.value;
    // Mbed TLS exposes the parsed X.509 version as 1/2/3, while
    // MBEDTLS_X509_CRT_VERSION_3 names the encoded INTEGER value (2).
    if (peer.version != 3) {
        return IdentityError::unsupported_version;
    }
    if (!has_nonnegative_serial(peer)) {
        return IdentityError::negative_or_malformed_serial;
    }
    if (peer.issuer_id.len != 0 || peer.subject_id.len != 0) {
        return IdentityError::forbidden_unique_id;
    }
    if (!matches_subject_identity(peer, path.expected_identity_oid_value)) {
        return IdentityError::association_identity_mismatch;
    }
    if (!has_required_key_usage(peer)) {
        return IdentityError::key_usage_invalid;
    }
    if (!supported_certificate_key(peer)) {
        return IdentityError::unsupported_public_key;
    }
    bool peer_is_ca = false;
    if (has_extension(peer, MBEDTLS_X509_EXT_BASIC_CONSTRAINTS)) {
#if MBEDTLS_VERSION_MAJOR >= 3
        peer_is_ca = mbedtls_x509_crt_get_ca_istrue(&peer) != 0;
#else
        peer_is_ca = peer.ca_istrue != 0;
#endif
    }
    if (peer_is_ca) {
        return IdentityError::ca_certificate_used_as_peer;
    }

    if (!peer_extensions.has_noncritical_crl_distribution_points) {
        return IdentityError::crl_distribution_point_missing;
    }
    if (!has_crl_coverage(peer, intermediates.value, crl_candidates,
                          crl_candidate_count)) {
        return IdentityError::revocation_unavailable;
    }

    ParsedCrls verification_crls;
    bool applicable_crls_cover_path = false;
    if (!build_verification_crls(peer, intermediates.value, crl_candidates,
                                 crl_candidate_count, verification_crls,
                                 applicable_crls_cover_path)) {
        return IdentityError::invalid_crl;
    }
    if (!applicable_crls_cover_path) {
        return IdentityError::revocation_unavailable;
    }

    // Use Mbed TLS's path verifier, but exclude SHA-1 and weaker certificate
    // signatures and require at least 2048-bit RSA for all chain keys.
    auto profile = mbedtls_x509_crt_profile_default;
    profile.allowed_mds = MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256) |
                          MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384) |
                          MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA512);
#if defined(MBEDTLS_SHA3_C)
    profile.allowed_mds |= MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA3_256) |
                           MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA3_384) |
                           MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA3_512);
#endif
    profile.rsa_min_bitlen = 2048;

    if (intermediates.value.version != 0) {
        for (auto* certificate = &intermediates.value; certificate != nullptr;
             certificate = certificate->next) {
            if (!supported_certificate_key(*certificate)) {
                return IdentityError::unsupported_public_key;
            }
        }
    }
    for (auto* certificate = &trust_anchors.value; certificate != nullptr;
         certificate = certificate->next) {
        if (!supported_certificate_key(*certificate)) {
            return IdentityError::unsupported_public_key;
        }
    }

    std::uint32_t verification_flags = 0;
    const int verification_result = mbedtls_x509_crt_verify_with_profile(
        &leaf.value, &trust_anchors.value, &verification_crls.value, &profile, nullptr,
        &verification_flags, nullptr, nullptr);
    return verification_result == 0 && verification_flags == 0
               ? IdentityError::none
               : IdentityError::revoked_or_untrusted;
#else
    (void)leaf_der;
    (void)path;
    return IdentityError::unavailable;
#endif
}

} // namespace ccli::mms::e2e
