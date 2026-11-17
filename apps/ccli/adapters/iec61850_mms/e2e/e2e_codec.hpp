#pragma once

#include "ber_codec.hpp"

#include <cstdint>
#include <vector>

namespace ccli::mms::e2e {

enum class E2eCodecError : std::uint8_t {
    none,
    malformed_ber,
    trailing_data,
    wrong_tag,
    missing_field,
    invalid_field,
    unsupported_version,
    integer_out_of_range,
    unsupported_transfer_choice,
    resource_limit_exceeded,
};

enum class DataTransferKind : std::uint8_t {
    clear,
    encrypted,
};

// Encoded authentication slices and primitive fields borrow the input buffer.
// Constructed string fields retain their bounded flattened logical values.
// Keep input alive until all authentication/application processing completes.
struct ClearToken2View {
    ByteView encoded{};
    BerStringView association_id{};       // NMTOKEN/UTF8String contents
    BerStringView timestamp{};            // GeneralizedTime contents
    std::uint64_t sequence{0};
    bool has_initialization_vector{false};
    BerStringView initialization_vector{};
    bool has_rekey_public_key{false};
    BerStringView rekey_public_key{};
    bool request_rekey{false};
    bool changed_key{false};
};

struct AuthenticatorView {
    ByteView encoded{};
    bool has_nonce{false};
    BerStringView nonce{};
    ByteView algorithm_identifier{}; // Complete AlgorithmIdentifier SEQUENCE.
    BerStringView icv{};
};

struct ClearToken2Fields {
    BerStringView association_id{};
    BerStringView timestamp{};
    std::uint64_t sequence{0};
    bool has_initialization_vector{false};
    BerStringView initialization_vector{};
    bool has_rekey_public_key{false};
    BerStringView rekey_public_key{};
    bool request_rekey{false};
    bool changed_key{false};
};

struct AuthenticatorFields {
    bool has_nonce{false};
    BerStringView nonce{};
    ByteView algorithm_identifier{}; // Complete AlgorithmIdentifier SEQUENCE.
    BerStringView icv{};
};

struct ClearTransferView {
    ByteView encoded{};
    ByteView protected_tbp{};        // Exact received TBP TLV, including its BER header.
    ByteView icv_input{};            // TBP contents; indefinite form retains its EOC.
    ClearToken2View token{};
    ByteView application_data{};     // Exact received ApplData TLV.
    AuthenticatorView authenticator{};
};

struct PresentationUserDataView {
    ByteView encoded{};
    std::uint32_t presentation_context_id{0};
    DataTransferKind kind{DataTransferKind::clear};
    ByteView encoded_transfer{};     // Complete ClearTransfer or EncrTransfer TLV.
};

struct SignatureView {
    ByteView encoded{};
    ByteView algorithm_identifier{};
    BerStringView value{};
};

struct DiffieHellmanSetView {
    ByteView encoded{};
    std::uint64_t group_id{0};
    BerStringView public_key{};
};

enum class EncryptionModeKind : std::uint8_t {
    authenticated_encryption,
    separate_encryption_and_icv,
};

struct ClearToken1View {
    ByteView encoded{};
    ByteView signature_algorithm{};
    BerStringView public_key_certificate{};
    bool has_certificate_path{false};
    ByteView certificate_path{};
    BerStringView association_id{};
    DiffieHellmanSetView dh{};
    ByteView hmac_algorithm{};
    BerStringView timestamp{};
    EncryptionModeKind encryption_mode{EncryptionModeKind::separate_encryption_and_icv};
    ByteView encoded_encryption_mode{};
    bool tls_protected{false};
    bool application_protected{false};
    bool vpn{false};
    bool has_attribute_certificate{false};
    BerStringView attribute_certificate{};
};

struct ClearToken3View {
    ByteView encoded{};
    ByteView signature_algorithm{};
    bool has_association_id{false};
    BerStringView association_id{}; // Preserved verbatim; errors may carry partial IDs.
    BerStringView timestamp{};
    BerStringView public_key_certificate{};
    bool has_certificate_path{false};
    ByteView certificate_path{};
};

enum class SignedMessageKind : std::uint8_t {
    handshake_accept,
    application_reject,
    handshake_security_reject,
    handshake_security_abort,
    transfer_security_abort,
    application_abort,
    release_request,
    release_response,
};

struct SignedMessageView {
    ByteView encoded{};
    ByteView protected_tbs{};
    ByteView signed_tbs_contents{};
    bool contains_clear_token1{false};
    ClearToken1View token1{};
    ClearToken3View token3{};
    bool has_application_data{false};
    ByteView application_data{};
    bool has_diagnostic{false};
    std::uint64_t diagnostic{0};
    SignatureView signature{};
};

struct HandshakeRequestView {
    ByteView encoded{};
    BerStringView protected_protocol{};  // NMTOKEN contents; profile check is separate.
    ByteView protected_tbs{};       // Complete received TBS SEQUENCE TLV.
    ByteView signed_tbs_contents{}; // Excludes tag/length; retains EOC if indefinite.
    ByteView token1{};              // Complete ClearToken1 TLV for typed validation.
    ClearToken1View token1_fields{};
    bool has_application_data{false};
    ByteView application_data{};    // Complete ApplData TLV if present.
    SignatureView signature{};
};

struct OsiAssociationRequestView {
    ByteView encoded{};
    std::uint32_t indirect_reference{0};
    HandshakeRequestView handshake{};
};

struct OsiAssociationResponseView {
    ByteView encoded{};
    std::uint32_t indirect_reference{0};
    SignedMessageKind choice{SignedMessageKind::handshake_accept};
    SignedMessageView message{};
};

struct OsiAbortView {
    ByteView encoded{};
    std::uint32_t indirect_reference{0};
    bool has_osi_diagnostic{false};
    std::uint64_t osi_diagnostic{0};
    SignedMessageKind choice{SignedMessageKind::handshake_security_abort};
    SignedMessageView message{};
};

struct OsiReleaseView {
    ByteView encoded{};
    std::uint32_t indirect_reference{0};
    SignedMessageView message{};
};

// Decode the IEC 62351-4 OSI Presentation User-data wrapper. The E2E ASN.1
// declares exactly one presentation data value and one DataTransfer choice.
bool decode_presentation_user_data(ByteView encoded,
                                   PresentationUserDataView& result,
                                   E2eCodecError& error,
                                   BerLimits limits = {}) noexcept;

// Decode one DataTransfer CHOICE ([0] ClearTransfer or [1] EncrTransfer).
// Under IMPLICIT TAGS the choice tag replaces the transfer SEQUENCE tag, so
// the returned view is the complete context-tagged transfer TLV itself.
bool decode_data_transfer(ByteView encoded, DataTransferKind& kind,
                          ByteView& encoded_transfer, E2eCodecError& error,
                          BerLimits limits = {}) noexcept;

// Decode the signed HandshakeReq envelope while preserving the exact signed
// TBS contents. ClearToken1 semantics and signature verification are separate.
bool decode_handshake_request(ByteView encoded, HandshakeRequestView& result,
                              E2eCodecError& error,
                              BerLimits limits = {}) noexcept;

bool decode_clear_token1(ByteView encoded, ClearToken1View& result,
                         E2eCodecError& error,
                         BerLimits limits = {}) noexcept;
bool decode_clear_token3(ByteView encoded, ClearToken3View& result,
                         E2eCodecError& error,
                         BerLimits limits = {}) noexcept;
bool decode_signed_message(ByteView encoded, SignedMessageKind kind,
                           SignedMessageView& result, E2eCodecError& error,
                           BerLimits limits = {}) noexcept;
bool decode_osi_association_request(ByteView encoded,
                                    OsiAssociationRequestView& result,
                                    E2eCodecError& error,
                                    BerLimits limits = {}) noexcept;
bool decode_osi_association_response(ByteView encoded,
                                     OsiAssociationResponseView& result,
                                     E2eCodecError& error,
                                     BerLimits limits = {}) noexcept;
bool decode_osi_association_abort(ByteView encoded, OsiAbortView& result,
                                  E2eCodecError& error,
                                  BerLimits limits = {}) noexcept;
bool decode_osi_release_request(ByteView encoded, OsiReleaseView& result,
                                E2eCodecError& error,
                                BerLimits limits = {}) noexcept;
bool decode_osi_release_response(ByteView encoded, OsiReleaseView& result,
                                 E2eCodecError& error,
                                 BerLimits limits = {}) noexcept;

// Signed envelopes preserve the caller's encoded TBS bytes exactly. The
// caller signs the TBS contents (after tag/length, retaining EOC if indefinite)
// and supplies the resulting signature value and its complete algorithm ID.
bool encode_signature(ByteView algorithm_identifier, ByteView signature_value,
                      std::vector<std::uint8_t>& encoded,
                      E2eCodecError& error, BerLimits limits = {});
bool encode_handshake_request(ByteView protected_protocol, ByteView encoded_tbs,
                              ByteView signature_algorithm,
                              ByteView signature_value,
                              std::vector<std::uint8_t>& encoded,
                              E2eCodecError& error, BerLimits limits = {});
bool encode_signed_message(SignedMessageKind kind, ByteView encoded_tbs,
                           ByteView signature_algorithm,
                           ByteView signature_value,
                           std::vector<std::uint8_t>& encoded,
                           E2eCodecError& error, BerLimits limits = {});
bool encode_osi_association_request(std::uint32_t indirect_reference,
                                    ByteView encoded_handshake_request,
                                    std::vector<std::uint8_t>& encoded,
                                    E2eCodecError& error,
                                    BerLimits limits = {});
bool encode_osi_association_response(std::uint32_t indirect_reference,
                                     ByteView encoded_handshake_response,
                                     std::vector<std::uint8_t>& encoded,
                                     E2eCodecError& error,
                                     BerLimits limits = {});
bool encode_osi_association_abort(std::uint32_t indirect_reference,
                                  ByteView encoded_abort_choice,
                                  std::vector<std::uint8_t>& encoded,
                                  E2eCodecError& error,
                                  BerLimits limits = {});
bool encode_osi_release_request(std::uint32_t indirect_reference,
                                ByteView encoded_release_request,
                                std::vector<std::uint8_t>& encoded,
                                E2eCodecError& error,
                                BerLimits limits = {});
bool encode_osi_release_response(std::uint32_t indirect_reference,
                                 ByteView encoded_release_response,
                                 std::vector<std::uint8_t>& encoded,
                                 E2eCodecError& error,
                                 BerLimits limits = {});

// Definite-length BER encoders for the selected non-AE ClearTransfer path.
// They omit the DEFAULT version and FALSE flags. Authentication is computed by
// the caller over the returned TBP value contents before encode_clear_transfer.
bool encode_clear_token2(const ClearToken2Fields& fields,
                         std::vector<std::uint8_t>& encoded,
                         E2eCodecError& error);
bool encode_authenticator(const AuthenticatorFields& fields,
                          std::vector<std::uint8_t>& encoded,
                          E2eCodecError& error);
bool encode_tbp(ByteView encoded_token2, ByteView encoded_application_data,
                std::vector<std::uint8_t>& encoded_tbp,
                E2eCodecError& error, BerLimits limits = {});
bool encode_clear_transfer(ByteView encoded_tbp, ByteView encoded_authenticator,
                           std::vector<std::uint8_t>& encoded_transfer,
                           E2eCodecError& error, BerLimits limits = {});
bool encode_data_transfer_choice(DataTransferKind kind,
                                 ByteView encoded_transfer,
                                 std::vector<std::uint8_t>& encoded_choice,
                                 E2eCodecError& error, BerLimits limits = {});
bool encode_presentation_user_data(std::uint32_t presentation_context_id,
                                   ByteView encoded_data_transfer,
                                   std::vector<std::uint8_t>& encoded_user_data,
                                   E2eCodecError& error, BerLimits limits = {});

// Structural decoders for the integrity-only transfer path. They validate
// root fields and retain original encoded bytes; cryptographic verification,
// GeneralizedTime interpretation, and profile-specific algorithm checks are
// separate responsibilities.
bool decode_clear_token2(ByteView encoded, ClearToken2View& result,
                         E2eCodecError& error, BerLimits limits = {}) noexcept;
bool decode_authenticator(ByteView encoded, AuthenticatorView& result,
                          E2eCodecError& error, BerLimits limits = {}) noexcept;
bool decode_clear_transfer(ByteView encoded, ClearTransferView& result,
                           E2eCodecError& error, BerLimits limits = {}) noexcept;

// Return BER contents after the tag/length and before an enclosing EOC; for
// indefinite form the EOC is retained. This is the input convention used by
// IEC signatures and ClearTransfer ICVs.
bool ber_authentication_contents(ByteView encoded_tlv,
                                 ByteView& authentication_contents,
                                 E2eCodecError& error,
                                 BerLimits limits = {}) noexcept;

} // namespace ccli::mms::e2e
