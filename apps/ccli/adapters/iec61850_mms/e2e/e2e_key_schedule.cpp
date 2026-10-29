#include "e2e_key_schedule.hpp"

#include <array>
#include <limits>

namespace ccli::mms::e2e {
namespace {

constexpr std::size_t sha256_size = 32;
constexpr std::size_t max_transcript_component_size = 1024u * 1024u;

void secure_clear(std::vector<std::uint8_t>& bytes) noexcept {
    volatile std::uint8_t* data = bytes.data();
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        data[i] = 0;
    }
    bytes.clear();
}

struct SensitiveBytes {
    std::vector<std::uint8_t> value;
    ~SensitiveBytes() { secure_clear(value); }
};

CryptoError hash_concatenation_sha256(
    const E2eCryptoProvider& crypto, ByteView left, ByteView right,
    std::array<std::uint8_t, sha256_size>& digest) {
    if (!left.valid() || !right.valid() ||
        left.size > max_transcript_component_size ||
        right.size > max_transcript_component_size ||
        left.size > std::numeric_limits<std::size_t>::max() - right.size) {
        digest.fill(0);
        return CryptoError::invalid_input;
    }

    std::vector<std::uint8_t> input;
    input.reserve(left.size + right.size);
    if (left.size != 0) {
        input.insert(input.end(), left.data, left.data + left.size);
    }
    if (right.size != 0) {
        input.insert(input.end(), right.data, right.data + right.size);
    }
    // Build the input before clearing the result in case the views alias it.
    digest.fill(0);
    return crypto.sha256({input.data(), input.size()}, digest);
}

bool supported_key_size(std::size_t size) noexcept {
    return size == 16 || size == 32;
}

bool lengths_valid(const E2eKeyScheduleLengths& lengths,
                   std::size_t& total_size) noexcept {
    if (!supported_key_size(lengths.client_directional_key) ||
        lengths.client_directional_key != lengths.server_directional_key) {
        return false;
    }
    const bool has_client_content_key = lengths.client_content_encryption_key != 0;
    const bool has_server_content_key = lengths.server_content_encryption_key != 0;
    if (has_client_content_key != has_server_content_key ||
        (has_client_content_key &&
         (!supported_key_size(lengths.client_content_encryption_key) ||
          !supported_key_size(lengths.server_content_encryption_key)))) {
        return false;
    }

    total_size = lengths.client_directional_key + lengths.server_directional_key;
    if (has_client_content_key) {
        if (lengths.client_content_encryption_key >
                std::numeric_limits<std::size_t>::max() - total_size ||
            lengths.server_content_encryption_key >
                std::numeric_limits<std::size_t>::max() - total_size -
                    lengths.client_content_encryption_key) {
            return false;
        }
        total_size += lengths.client_content_encryption_key +
                      lengths.server_content_encryption_key;
    }
    return total_size <= 255u * sha256_size;
}

void assign_slice(std::vector<std::uint8_t>& destination,
                  const std::vector<std::uint8_t>& source,
                  std::size_t offset, std::size_t count) {
    destination.assign(source.begin() + static_cast<std::ptrdiff_t>(offset),
                       source.begin() + static_cast<std::ptrdiff_t>(offset + count));
}

} // namespace

E2eDirectionalKeyMaterial::~E2eDirectionalKeyMaterial() {
    clear();
}

void E2eDirectionalKeyMaterial::clear() noexcept {
    secure_clear(client_directional_key);
    secure_clear(server_directional_key);
    secure_clear(client_content_encryption_key);
    secure_clear(server_content_encryption_key);
}

CryptoError derive_initial_transcript_salt_sha256(
    const E2eCryptoProvider& crypto, ByteView data1, ByteView data2,
    std::vector<std::uint8_t>& salt) {
    if (!data1.valid() || !data2.valid() ||
        data1.size > max_transcript_component_size ||
        data2.size > max_transcript_component_size ||
        data1.size == std::numeric_limits<std::size_t>::max()) {
        secure_clear(salt);
        return CryptoError::invalid_input;
    }

    std::vector<std::uint8_t> data1_with_zero;
    data1_with_zero.reserve(data1.size + 1);
    if (data1.size != 0) {
        data1_with_zero.insert(data1_with_zero.end(), data1.data,
                               data1.data + data1.size);
    }
    data1_with_zero.push_back(0);

    std::array<std::uint8_t, sha256_size> hash1{};
    auto result = crypto.sha256({data1_with_zero.data(), data1_with_zero.size()}, hash1);
    if (result != CryptoError::none) {
        secure_clear(salt);
        return result;
    }
    std::array<std::uint8_t, sha256_size> hash2{};
    result = hash_concatenation_sha256(
        crypto, data2, {hash1.data(), hash1.size()}, hash2);
    if (result == CryptoError::none) {
        salt.assign(hash2.begin(), hash2.end());
    } else {
        secure_clear(salt);
    }
    hash1.fill(0);
    hash2.fill(0);
    return result;
}

CryptoError derive_rekey_transcript_salt_sha256(
    const E2eCryptoProvider& crypto, ByteView data3, ByteView initial_salt,
    std::vector<std::uint8_t>& updated_salt) {
    if (!data3.valid() || data3.size > max_transcript_component_size ||
        !initial_salt.valid() || initial_salt.size != sha256_size) {
        secure_clear(updated_salt);
        return CryptoError::invalid_input;
    }
    std::array<std::uint8_t, sha256_size> digest{};
    const auto result = hash_concatenation_sha256(
        crypto, data3, initial_salt, digest);
    if (result == CryptoError::none) {
        updated_salt.assign(digest.begin(), digest.end());
    } else {
        secure_clear(updated_salt);
    }
    digest.fill(0);
    return result;
}

CryptoError derive_directional_key_material_sha256(
    const E2eCryptoProvider& crypto, ByteView shared_secret_z,
    ByteView transcript_salt, const E2eKeyScheduleLengths& lengths,
    E2eDirectionalKeyMaterial& keys) {
    keys.clear();
    if (!shared_secret_z.valid() || shared_secret_z.empty() ||
        !transcript_salt.valid() || transcript_salt.size != sha256_size) {
        return CryptoError::invalid_input;
    }

    std::size_t total_size = 0;
    if (!lengths_valid(lengths, total_size)) {
        return CryptoError::invalid_input;
    }

    SensitiveBytes okm;
    okm.value.resize(total_size, 0);
    const auto result = crypto.hkdf_sha256(
        transcript_salt, shared_secret_z, {nullptr, 0},
        {okm.value.data(), okm.value.size()});
    if (result != CryptoError::none) {
        return result;
    }

    std::size_t offset = 0;
    assign_slice(keys.client_directional_key, okm.value, offset,
                 lengths.client_directional_key);
    offset += lengths.client_directional_key;
    assign_slice(keys.server_directional_key, okm.value, offset,
                 lengths.server_directional_key);
    offset += lengths.server_directional_key;
    if (lengths.client_content_encryption_key != 0) {
        assign_slice(keys.client_content_encryption_key, okm.value, offset,
                     lengths.client_content_encryption_key);
        offset += lengths.client_content_encryption_key;
        assign_slice(keys.server_content_encryption_key, okm.value, offset,
                     lengths.server_content_encryption_key);
    }
    return CryptoError::none;
}

} // namespace ccli::mms::e2e
