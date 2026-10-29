#pragma once

#include "e2e_codec.hpp"
#include "e2e_crypto.hpp"
#include "e2e_identity.hpp"

#include <chrono>
#include <cstdint>

namespace ccli::mms::e2e {

enum class HandshakeValidationError : std::uint8_t {
    none,
    invalid_policy,
    malformed_association_request,
    resource_limit_exceeded,
    protected_protocol_mismatch,
    unsupported_signature_algorithm,
    token_signature_algorithm_mismatch,
    invalid_timestamp,
    timestamp_outside_window,
    invalid_certificate_path_encoding,
    peer_identity_rejected,
    signature_rejected,
    crypto_unavailable,
};

struct HandshakeValidationPolicy {
    ByteView expected_protected_protocol{};
    E2ePeerCertificatePath peer_certificate_path{};
    std::chrono::system_clock::time_point current_time{};
    std::chrono::seconds maximum_clock_skew{std::chrono::minutes(10)};
};

// Borrowed slices refer to encoded_association_request and remain valid only
// while that buffer is alive. A successful result means the request's profile
// name, Annex G certificate path, timestamp and signature passed preflight;
// it does not mean that algorithm negotiation or the association is accepted.
struct ValidatedHandshakePeer {
    BerStringView certificate_der{};
    BerStringView association_id{};
    std::uint64_t key_agreement_group{0};
    BerStringView ephemeral_public_key{};
};

// Validate the cryptographic and identity preconditions of one native OSI
// Association Request. The caller supplies the AP-title/AE-derived identity
// OID, commissioned trust anchors and current CRLs in policy. No MMS data is
// dispatched and no association state is retained by this function.
HandshakeValidationError validate_handshake_request(
    ByteView encoded_association_request,
    const HandshakeValidationPolicy& policy,
    const E2eCryptoProvider& crypto,
    ValidatedHandshakePeer& peer,
    IdentityError* identity_error = nullptr,
    E2eCodecError* codec_error = nullptr);

} // namespace ccli::mms::e2e
