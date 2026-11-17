#pragma once

#include "ber_codec.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace ccli::mms::e2e {

struct MutableByteView {
    std::uint8_t* data{nullptr};
    std::size_t size{0};

    bool valid() const noexcept { return size == 0 || data != nullptr; }
};

enum class CryptoError : std::uint8_t {
    none,
    unavailable,
    invalid_input,
    unsupported_key,
    provider_failure,
    authentication_failed,
};

enum class SignatureScheme : std::uint8_t {
    rsa_pkcs1_v15_sha256,
    ecdsa_sha256,
};

enum class KeyAgreementGroup : std::uint8_t {
    modp_group14 = 14,
    ecdh_group23 = 23,
    ecdh_group28 = 28,
};
enum class KeyAgreementUse : std::uint8_t { initial, retained_server_rekey };

class E2eCryptoProvider;

// Owns one generated ephemeral private key. The private value stays inside
// Mbed TLS and is cleared when this object is destroyed. Public values use
// the raw OCTET STRING representation carried by the E2E ASN.1 DHKey.
class E2eEphemeralKey {
public:
    ~E2eEphemeralKey();
    E2eEphemeralKey(const E2eEphemeralKey&) = delete;
    E2eEphemeralKey& operator=(const E2eEphemeralKey&) = delete;
    E2eEphemeralKey(E2eEphemeralKey&&) = delete;
    E2eEphemeralKey& operator=(E2eEphemeralKey&&) = delete;

    KeyAgreementGroup group() const noexcept;
    ByteView public_key() const noexcept;
    // On failure, shared_secret is cleared. The caller must erase the returned
    // secret after deriving the protocol's directional keys.
    CryptoError derive_shared_secret(ByteView peer_public_key,
                                     std::vector<std::uint8_t>& shared_secret,
                                     KeyAgreementUse use = KeyAgreementUse::initial);

private:
    friend class E2eCryptoProvider;
    struct Impl;
    explicit E2eEphemeralKey(std::unique_ptr<Impl> impl);
    std::unique_ptr<Impl> impl_;
};

// Mbed TLS-backed primitives for the IEC 62351-4 adapter. This layer does not
// choose protocol algorithms, derive IEC session keys, parse profile OIDs, or
// decide which exact bytes a signature/ICV covers. Callers must pass the
// preserved codec slices specified by the selected profile.
class E2eCryptoProvider {
public:
    E2eCryptoProvider();
    ~E2eCryptoProvider();

    E2eCryptoProvider(const E2eCryptoProvider&) = delete;
    E2eCryptoProvider& operator=(const E2eCryptoProvider&) = delete;
    E2eCryptoProvider(E2eCryptoProvider&&) = delete;
    E2eCryptoProvider& operator=(E2eCryptoProvider&&) = delete;

    CryptoError status() const noexcept;
    CryptoError random(MutableByteView output);
    CryptoError sha256(ByteView input, std::array<std::uint8_t, 32>& digest) const noexcept;
    CryptoError hmac_sha256(ByteView key, ByteView input,
                            std::array<std::uint8_t, 32>& mac) const noexcept;
    // Accepts a protocol-selected prefix of the full SHA-256 HMAC. The caller
    // remains responsible for enforcing the negotiated ICV length.
    CryptoError verify_hmac_sha256(ByteView key, ByteView input,
                                   ByteView expected_mac) const noexcept;
    CryptoError hkdf_sha256(ByteView salt, ByteView input_key_material,
                            ByteView info, MutableByteView output) const noexcept;

    // GMAC is GCM with an empty plaintext and the authenticated bytes as AAD.
    // AES-128 and AES-256 keys are supported; Mbed TLS validates the requested
    // tag length against its supported GCM tag sizes.
    CryptoError aes_gmac(ByteView key, ByteView nonce, ByteView authenticated_data,
                         MutableByteView tag) const noexcept;
    CryptoError verify_aes_gmac(ByteView key, ByteView nonce,
                                ByteView authenticated_data,
                                ByteView tag) const noexcept;

    // These helpers apply SHA-256 to the supplied message and use the
    // explicitly selected scheme. They do not interpret Signature.algorithm
    // or enforce certificate policy.
    CryptoError sign_sha256(SignatureScheme scheme, ByteView private_key_der_or_pem,
                            ByteView password, ByteView message,
                            std::vector<std::uint8_t>& signature);
    CryptoError verify_sha256(SignatureScheme scheme, ByteView certificate_der,
                              ByteView message,
                              ByteView signature) const noexcept;

    // Generates an ephemeral local key for a supported IEC group. Group 14
    // uses a 2048-bit MODP prime; groups 23 and 28 use secp256r1 and
    // brainpoolP256r1 ECDH respectively. Peer points/values are validated.
    CryptoError generate_ephemeral_key(KeyAgreementGroup group,
                                      std::unique_ptr<E2eEphemeralKey>& key);

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace ccli::mms::e2e
