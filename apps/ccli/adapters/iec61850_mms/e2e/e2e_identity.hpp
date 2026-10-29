#pragma once

#include "ber_codec.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ccli::mms::e2e {

enum class IdentityError : std::uint8_t {
    none,
    unavailable,
    invalid_input,
    invalid_certificate,
    invalid_intermediate_chain,
    invalid_trust_anchor,
    invalid_crl,
    unsupported_version,
    negative_or_malformed_serial,
    forbidden_unique_id,
    association_identity_mismatch,
    key_usage_invalid,
    unsupported_public_key,
    ca_certificate_used_as_peer,
    crl_distribution_point_missing,
    revocation_unavailable,
    revoked_or_untrusted,
};

// DER inputs are borrowed for the duration of validate_peer_certificate().
// expected_identity_oid_value contains the OBJECT IDENTIFIER contents (without
// its DER tag and length), as derived from the peer's ACSE AP-title/AE-qualifier.
struct E2ePeerCertificatePath {
    ByteView expected_identity_oid_value{};
    std::vector<ByteView> intermediates;
    std::vector<ByteView> trust_anchors;
    std::vector<ByteView> issuer_crls;
};

// Validates the IEC 62351-4 Annex G leaf profile, association identity,
// certificate path, time validity and CRL coverage. It checks direct issuer
// CRL distribution-point/issuing-distribution-point scope before handing only
// applicable CRLs to Mbed TLS. Indirect, delta, relative-name and partial-reason
// CRLs fail closed. OCSP is not implemented. Bounds are finite because inputs
// are untrusted.
IdentityError validate_peer_certificate(ByteView leaf_der,
                                        const E2ePeerCertificatePath& path) noexcept;

} // namespace ccli::mms::e2e
