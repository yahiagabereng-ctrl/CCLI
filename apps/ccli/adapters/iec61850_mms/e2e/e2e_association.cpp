#include "e2e_association.hpp"

#include "e2e_time.hpp"
#include "e2e_wire.hpp"

#include <cmath>
#include <cstring>
#include <vector>

namespace ccli::mms::e2e {
namespace {

constexpr std::uint8_t kSha256WithRsaOid[] = {
    0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b};
constexpr std::uint8_t kEcdsaWithSha256Oid[] = {
    0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02};

bool same_bytes(ByteView left, ByteView right) noexcept {
    return left.valid() && right.valid() && left.size == right.size &&
           (left.size == 0 || std::memcmp(left.data, right.data, left.size) == 0);
}

enum class ParsedSignatureAlgorithm : std::uint8_t {
    unsupported,
    rsa_pkcs1_v15_sha256,
    ecdsa_sha256,
};

ParsedSignatureAlgorithm parse_signature_algorithm(ByteView encoded) noexcept {
    if (wire::simple_algorithm_matches(encoded,
            {kSha256WithRsaOid, sizeof(kSha256WithRsaOid)}, true))
        return ParsedSignatureAlgorithm::rsa_pkcs1_v15_sha256;
    if (wire::simple_algorithm_matches(encoded,
            {kEcdsaWithSha256Oid, sizeof(kEcdsaWithSha256Oid)}, false))
        return ParsedSignatureAlgorithm::ecdsa_sha256;
    return ParsedSignatureAlgorithm::unsupported;
}

bool parse_certificate_path(ByteView encoded,
                            std::vector<BerStringView>& certificates) {
    certificates.clear();
    if (!encoded.valid() || encoded.empty()) {
        return false;
    }
    BerLimits limits{};
    limits.allow_indefinite = true;
    limits.max_encoded_bytes = 1024 * 1024;
    limits.max_elements = 256;
    BerReader outer(encoded, limits);
    BerTlvView sequence{};
    BerError error = BerError::none;
    if (!outer.next(sequence, error) || !outer.finished() ||
        sequence.tag.tag_class != BerTagClass::universal ||
        sequence.tag.number != 16 || !sequence.tag.constructed) {
        return false;
    }
    BerReader entries = outer.children(sequence);
    while (!entries.finished()) {
        BerTlvView certificate{};
        if (!entries.next(certificate, error) ||
            certificate.tag.tag_class != BerTagClass::universal ||
            certificate.tag.number != 4 || certificates.size() >= 32) {
            certificates.clear();
            return false;
        }
        auto value = wire::string_value(certificate, 4);
        if (value.empty()) { certificates.clear(); return false; }
        certificates.push_back(std::move(value));
    }
    return !certificates.empty();
}

// Extract only the structural fields needed for authentication. Typed decoding
// also validates version/defaults and other signed semantics, so it must follow
// this signature check rather than decide a semantic rejection first.
HandshakeValidationError verify_request_envelope(ByteView encoded,
                                                 const E2eCryptoProvider& crypto) {
    try {
        const auto outer = wire::fields(encoded, 0x30);
        if (outer.size() != 1) return HandshakeValidationError::malformed_association_request;
        const auto external = wire::fields(outer[0].encoded, 0x28);
        if (external.size() != 2 || !wire::tag(external[0], 2) || !wire::tag(external[1], 0xa0))
            return HandshakeValidationError::malformed_association_request;
        const auto request = wire::fields(external[1].value, 0x30);
        if (request.size() != 3 || request[0].tag.tag_class != BerTagClass::universal || request[0].tag.number != 12)
            return HandshakeValidationError::malformed_association_request;
        const auto tbs = wire::fields(request[1].encoded, 0x30);
        if (tbs.empty()) return HandshakeValidationError::malformed_association_request;
        const auto token = wire::fields(tbs[0].encoded, 0x30);
        const auto signature = wire::fields(request[2].encoded, 0x30);
        if (token.size() < 2 || signature.size() < 2)
            return HandshakeValidationError::malformed_association_request;
        const auto certificate_value = wire::string_value(token[1], 4);
        const auto signature_value = wire::string_value(signature[1], 4);
        const auto algorithm = parse_signature_algorithm(signature[0].encoded);
        const auto token_algorithm = parse_signature_algorithm(token[0].encoded);
        if (algorithm == ParsedSignatureAlgorithm::unsupported || token_algorithm == ParsedSignatureAlgorithm::unsupported)
            return HandshakeValidationError::unsupported_signature_algorithm;
        if (algorithm != token_algorithm) return HandshakeValidationError::token_signature_algorithm_mismatch;
        ByteView contents; E2eCodecError error{};
        if (!ber_authentication_contents(request[1].encoded, contents, error))
            return HandshakeValidationError::malformed_association_request;
        const auto result = crypto.verify_sha256(algorithm == ParsedSignatureAlgorithm::rsa_pkcs1_v15_sha256
            ? SignatureScheme::rsa_pkcs1_v15_sha256 : SignatureScheme::ecdsa_sha256,
            certificate_value, contents, signature_value);
        if (result == CryptoError::unavailable) return HandshakeValidationError::crypto_unavailable;
        return result == CryptoError::none ? HandshakeValidationError::none : HandshakeValidationError::signature_rejected;
    } catch (...) { return HandshakeValidationError::malformed_association_request; }
}

} // namespace

static HandshakeValidationError validate_handshake_request_impl(
    ByteView encoded_association_request,
    const HandshakeValidationPolicy& policy,
    const E2eCryptoProvider& crypto,
    ValidatedHandshakePeer& peer,
    IdentityError* identity_error,
    E2eCodecError* codec_error) {
    peer = {};
    if (identity_error) *identity_error = IdentityError::none;
    if (codec_error) *codec_error = E2eCodecError::none;
    if (!encoded_association_request.valid() || encoded_association_request.empty() ||
        !policy.expected_protected_protocol.valid() ||
        policy.expected_protected_protocol.empty() ||
        !policy.peer_certificate_path.expected_identity_oid_value.valid() ||
        policy.peer_certificate_path.expected_identity_oid_value.empty() ||
        policy.maximum_clock_skew.count() < 0) {
        return HandshakeValidationError::invalid_policy;
    }
    if (crypto.status() != CryptoError::none) {
        return HandshakeValidationError::crypto_unavailable;
    }
    const auto envelope_signature = verify_request_envelope(encoded_association_request, crypto);
    if (envelope_signature != HandshakeValidationError::none) return envelope_signature;

    OsiAssociationRequestView request{};
    E2eCodecError decode_error = E2eCodecError::none;
    BerLimits limits{};
    limits.max_encoded_bytes = 1024 * 1024;
    limits.max_depth = 24;
    limits.max_elements = 2048;
    if (!decode_osi_association_request(encoded_association_request, request,
                                        decode_error, limits)) {
        if (codec_error) *codec_error = decode_error;
        return HandshakeValidationError::malformed_association_request;
    }
    const auto signature_algorithm =
        parse_signature_algorithm(request.handshake.signature.algorithm_identifier);
    const auto token_signature_algorithm =
        parse_signature_algorithm(request.handshake.token1_fields.signature_algorithm);
    if (signature_algorithm == ParsedSignatureAlgorithm::unsupported ||
        token_signature_algorithm == ParsedSignatureAlgorithm::unsupported) {
        return HandshakeValidationError::unsupported_signature_algorithm;
    }
    if (signature_algorithm != token_signature_algorithm) {
        return HandshakeValidationError::token_signature_algorithm_mismatch;
    }

    E2ePeerCertificatePath certificate_path = policy.peer_certificate_path;
    std::vector<BerStringView> intermediates;
    const auto& token = request.handshake.token1_fields;
    if (token.has_certificate_path) {
        if (!parse_certificate_path(token.certificate_path, intermediates)) {
            return HandshakeValidationError::invalid_certificate_path_encoding;
        }
        certificate_path.intermediates.assign(intermediates.begin(), intermediates.end());
    } else {
        certificate_path.intermediates.clear();
    }

    // The TBS signature is checked before inspecting any signed semantic field.
    if (!same_bytes(request.handshake.protected_protocol,
                    policy.expected_protected_protocol)) {
        return HandshakeValidationError::protected_protocol_mismatch;
    }

    std::chrono::system_clock::time_point peer_time{};
    GeneralizedTimeError time_error = GeneralizedTimeError::none;
    if (!parse_generalized_time(token.timestamp, peer_time, time_error)) {
        return HandshakeValidationError::invalid_timestamp;
    }
    const long double peer_epoch = std::chrono::duration<long double>(
        peer_time.time_since_epoch()).count();
    const long double current_epoch = std::chrono::duration<long double>(
        policy.current_time.time_since_epoch()).count();
    const long double skew = peer_epoch - current_epoch;
    const long double max_skew =
        static_cast<long double>(policy.maximum_clock_skew.count());
    if (!std::isfinite(skew) || std::fabs(skew) > max_skew) {
        return HandshakeValidationError::timestamp_outside_window;
    }

    const IdentityError identity = validate_peer_certificate(
        token.public_key_certificate, certificate_path);
    if (identity_error) *identity_error = identity;
    if (identity != IdentityError::none) {
        return HandshakeValidationError::peer_identity_rejected;
    }

    peer.certificate_der = token.public_key_certificate;
    peer.association_id = token.association_id;
    peer.key_agreement_group = token.dh.group_id;
    peer.ephemeral_public_key = token.dh.public_key;
    return HandshakeValidationError::none;
}

HandshakeValidationError validate_handshake_request(
    ByteView encoded_association_request,
    const HandshakeValidationPolicy& policy,
    const E2eCryptoProvider& crypto,
    ValidatedHandshakePeer& peer,
    IdentityError* identity_error,
    E2eCodecError* codec_error) {
    peer = {};
    if (identity_error) *identity_error = IdentityError::none;
    if (codec_error) *codec_error = E2eCodecError::none;
    try {
        return validate_handshake_request_impl(encoded_association_request, policy,
                                               crypto, peer, identity_error,
                                               codec_error);
    } catch (...) {
        peer = {};
        if (identity_error) *identity_error = IdentityError::unavailable;
        return HandshakeValidationError::resource_limit_exceeded;
    }
}

} // namespace ccli::mms::e2e
