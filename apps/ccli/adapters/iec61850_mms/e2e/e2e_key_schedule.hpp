#pragma once

#include "e2e_crypto.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ccli::mms::e2e {

struct E2eKeyScheduleLengths {
    // In two-key mode these are the client's/server's complete directional
    // keys. In four-key mode they are the ICV keys.
    std::size_t client_directional_key{0};
    std::size_t server_directional_key{0};
    // Both are zero for integrity-only or authenticated-encryption mode.
    // Set both when encryption uses keys separate from the ICV keys.
    std::size_t client_content_encryption_key{0};
    std::size_t server_content_encryption_key{0};
};

struct E2eDirectionalKeyMaterial {
    std::vector<std::uint8_t> client_directional_key;
    std::vector<std::uint8_t> server_directional_key;
    std::vector<std::uint8_t> client_content_encryption_key;
    std::vector<std::uint8_t> server_content_encryption_key;

    E2eDirectionalKeyMaterial() = default;
    ~E2eDirectionalKeyMaterial();
    E2eDirectionalKeyMaterial(const E2eDirectionalKeyMaterial&) = delete;
    E2eDirectionalKeyMaterial& operator=(const E2eDirectionalKeyMaterial&) = delete;
    E2eDirectionalKeyMaterial(E2eDirectionalKeyMaterial&&) = delete;
    E2eDirectionalKeyMaterial& operator=(E2eDirectionalKeyMaterial&&) = delete;

    void clear() noexcept;
};

// data1/data2 are the exact BER tbs contents for HandshakeReq/HandshakeAcc:
// preserve their contents bytes and omit each SEQUENCE start tag and length.
// The formula's h0=0 is represented here as one zero octet; confirm this
// encoding against an independent IEC peer before freezing interoperability.
CryptoError derive_initial_transcript_salt_sha256(
    const E2eCryptoProvider& crypto, ByteView data1, ByteView data2,
    std::vector<std::uint8_t>& salt);

// data3 is the exact BER tbp contents of the key-update transfer, excluding
// its SEQUENCE start tag and length. The initial salt is retained across rekeys.
CryptoError derive_rekey_transcript_salt_sha256(
    const E2eCryptoProvider& crypto, ByteView data3, ByteView initial_salt,
    std::vector<std::uint8_t>& updated_salt);

// Implements IEC 62351-4 §13.3.1.3-4 key ordering with SHA-256 HKDF and an
// absent info value. Keys are emitted client first, server second; if separate
// content-encryption keys are selected, those follow client then server. In
// authenticated-encryption mode, use the two directional keys for the combined
// encryption/authentication algorithm. Sizes come from the negotiated profile.
CryptoError derive_directional_key_material_sha256(
    const E2eCryptoProvider& crypto, ByteView shared_secret_z,
    ByteView transcript_salt, const E2eKeyScheduleLengths& lengths,
    E2eDirectionalKeyMaterial& keys);

} // namespace ccli::mms::e2e
