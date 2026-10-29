#include "e2e_crypto.hpp"

#include <algorithm>
#include <mutex>
#include <utility>

#if defined(CONFIG_MMS_SUPPORT_TLS)
#include "mbedtls/bignum.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"
#include "mbedtls/gcm.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/version.h"
#include "mbedtls/x509_crt.h"
#endif

namespace ccli::mms::e2e {
namespace {

bool valid(ByteView bytes) noexcept {
    return bytes.valid();
}

const unsigned char* data_or_empty(ByteView bytes) noexcept {
    static const unsigned char empty = 0;
    return bytes.size == 0 ? &empty : bytes.data;
}

bool supported_gcm_tag_size(std::size_t size) noexcept {
    return size == 4 || size == 8 || (size >= 12 && size <= 16);
}

bool constant_time_equal(ByteView left, ByteView right) noexcept {
    if (!left.valid() || !right.valid() || left.size != right.size) {
        return false;
    }
    volatile std::uint8_t difference = 0;
    for (std::size_t i = 0; i < left.size; ++i) {
        difference = static_cast<std::uint8_t>(difference | (left.data[i] ^ right.data[i]));
    }
    return difference == 0;
}

void zero(MutableByteView bytes) noexcept {
    if (bytes.valid() && bytes.size != 0) {
#if defined(CONFIG_MMS_SUPPORT_TLS)
        mbedtls_platform_zeroize(bytes.data, bytes.size);
#else
        volatile std::uint8_t* output = bytes.data;
        for (std::size_t i = 0; i < bytes.size; ++i) {
            output[i] = 0;
        }
#endif
    }
}

#if defined(CONFIG_MMS_SUPPORT_TLS)
const mbedtls_md_info_t* sha256_info() noexcept {
    return mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
}

bool key_matches_scheme(const mbedtls_pk_context& key,
                        SignatureScheme scheme) noexcept {
    switch (scheme) {
    case SignatureScheme::rsa_pkcs1_v15_sha256:
        return mbedtls_pk_can_do(&key, MBEDTLS_PK_RSA) != 0 &&
               mbedtls_pk_get_bitlen(&key) >= 2048;
    case SignatureScheme::ecdsa_sha256:
        return mbedtls_pk_can_do(&key, MBEDTLS_PK_ECDSA) != 0 &&
               mbedtls_pk_get_bitlen(&key) >= 256;
    }
    return false;
}

struct ParsedPrivateKey {
    mbedtls_pk_context value;
    ParsedPrivateKey() { mbedtls_pk_init(&value); }
    ~ParsedPrivateKey() { mbedtls_pk_free(&value); }
};

struct SensitiveBuffer {
    std::vector<std::uint8_t> value;
    ~SensitiveBuffer() {
        if (!value.empty()) {
            mbedtls_platform_zeroize(value.data(), value.size());
        }
    }
};

struct ParsedCertificate {
    mbedtls_x509_crt value;
    ParsedCertificate() { mbedtls_x509_crt_init(&value); }
    ~ParsedCertificate() { mbedtls_x509_crt_free(&value); }
};

bool is_pem(ByteView key) noexcept {
    static constexpr char header[] = "-----BEGIN ";
    return key.size >= sizeof(header) - 1 &&
           std::equal(header, header + sizeof(header) - 1,
                      reinterpret_cast<const char*>(key.data));
}

constexpr char group14_prime_hex[] =
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
    "29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
    "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
    "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
    "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
    "83655D23DCA3AD961C62F356208552BB9ED529077096966D"
    "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
    "DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
    "15728E5A8AACAA68FFFFFFFFFFFFFFFF";

int set_group14(mbedtls_dhm_context& context) noexcept {
    mbedtls_mpi prime;
    mbedtls_mpi generator;
    mbedtls_mpi_init(&prime);
    mbedtls_mpi_init(&generator);
    int result = mbedtls_mpi_read_string(&prime, 16, group14_prime_hex);
    if (result == 0) {
        result = mbedtls_mpi_lset(&generator, 2);
    }
#if MBEDTLS_VERSION_MAJOR >= 3
    if (result == 0) {
        result = mbedtls_dhm_set_group(&context, &prime, &generator);
    }
#else
    if (result == 0) {
        result = mbedtls_mpi_copy(&context.P, &prime);
    }
    if (result == 0) {
        result = mbedtls_mpi_copy(&context.G, &generator);
    }
    if (result == 0) {
        context.len = mbedtls_mpi_size(&prime);
    }
#endif
    mbedtls_mpi_free(&generator);
    mbedtls_mpi_free(&prime);
    return result;
}

bool validate_group14_peer(ByteView encoded_peer) noexcept {
    mbedtls_mpi prime;
    mbedtls_mpi peer;
    mbedtls_mpi subgroup_order;
    mbedtls_mpi upper_bound;
    mbedtls_mpi check;
    mbedtls_mpi rr;
    mbedtls_mpi_init(&prime);
    mbedtls_mpi_init(&peer);
    mbedtls_mpi_init(&subgroup_order);
    mbedtls_mpi_init(&upper_bound);
    mbedtls_mpi_init(&check);
    mbedtls_mpi_init(&rr);

    int result = mbedtls_mpi_read_string(&prime, 16, group14_prime_hex);
    if (result == 0) {
        result = mbedtls_mpi_read_binary(&peer, encoded_peer.data, encoded_peer.size);
    }
    if (result == 0) {
        result = mbedtls_mpi_sub_int(&upper_bound, &prime, 1);
    }
    bool valid_peer = false;
    if (result == 0 && mbedtls_mpi_cmp_int(&peer, 1) > 0 &&
        mbedtls_mpi_cmp_mpi(&peer, &upper_bound) < 0) {
        result = mbedtls_mpi_sub_int(&subgroup_order, &prime, 1);
        if (result == 0) {
            result = mbedtls_mpi_shift_r(&subgroup_order, 1);
        }
        if (result == 0) {
            result = mbedtls_mpi_exp_mod(&check, &peer, &subgroup_order,
                                         &prime, &rr);
        }
        valid_peer = result == 0 && mbedtls_mpi_cmp_int(&check, 1) == 0;
    }

    mbedtls_mpi_free(&rr);
    mbedtls_mpi_free(&check);
    mbedtls_mpi_free(&upper_bound);
    mbedtls_mpi_free(&subgroup_order);
    mbedtls_mpi_free(&peer);
    mbedtls_mpi_free(&prime);
    return valid_peer;
}

mbedtls_ecp_group_id ecp_group(KeyAgreementGroup group) noexcept {
    switch (group) {
    case KeyAgreementGroup::ecdh_group23:
        return MBEDTLS_ECP_DP_SECP256R1;
    case KeyAgreementGroup::ecdh_group28:
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
        return MBEDTLS_ECP_DP_BP256R1;
#else
        return MBEDTLS_ECP_DP_NONE;
#endif
    case KeyAgreementGroup::modp_group14:
        break;
    }
    return MBEDTLS_ECP_DP_NONE;
}
#endif

} // namespace

struct E2eCryptoProvider::Impl {
    CryptoError provider_status{CryptoError::unavailable};
#if defined(CONFIG_MMS_SUPPORT_TLS)
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context drbg;
    std::mutex random_mutex;

    Impl() {
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&drbg);
        provider_status = mbedtls_ctr_drbg_seed(
                              &drbg, mbedtls_entropy_func, &entropy, nullptr, 0) == 0
                              ? CryptoError::none
                              : CryptoError::provider_failure;
    }

    ~Impl() {
        mbedtls_ctr_drbg_free(&drbg);
        mbedtls_entropy_free(&entropy);
    }
#endif
};

struct E2eEphemeralKey::Impl {
    KeyAgreementGroup selected_group{KeyAgreementGroup::modp_group14};
    bool derive_attempted{false};
    std::vector<std::uint8_t> public_value;
    std::shared_ptr<void> provider_lifetime;
    std::mutex* random_mutex{nullptr};
    std::mutex operation_mutex;
#if defined(CONFIG_MMS_SUPPORT_TLS)
    mbedtls_ctr_drbg_context* drbg{nullptr};
    mbedtls_dhm_context dhm;
    mbedtls_ecdh_context ecdh;

    Impl() {
        mbedtls_dhm_init(&dhm);
        mbedtls_ecdh_init(&ecdh);
    }

    ~Impl() {
        mbedtls_ecdh_free(&ecdh);
        mbedtls_dhm_free(&dhm);
    }
#endif
};

E2eCryptoProvider::E2eCryptoProvider() : impl_(std::make_shared<Impl>()) {}
E2eCryptoProvider::~E2eCryptoProvider() = default;

E2eEphemeralKey::E2eEphemeralKey(std::unique_ptr<Impl> impl)
    : impl_(std::move(impl)) {}

E2eEphemeralKey::~E2eEphemeralKey() = default;

KeyAgreementGroup E2eEphemeralKey::group() const noexcept {
    return impl_ ? impl_->selected_group : KeyAgreementGroup::modp_group14;
}

ByteView E2eEphemeralKey::public_key() const noexcept {
    if (!impl_ || impl_->public_value.empty()) {
        return {};
    }
    return {impl_->public_value.data(), impl_->public_value.size()};
}

CryptoError E2eEphemeralKey::derive_shared_secret(
    ByteView peer_public_key, std::vector<std::uint8_t>& shared_secret, KeyAgreementUse use) {
    shared_secret.clear();
    if (!impl_ || !peer_public_key.valid() || !impl_->random_mutex) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    const std::size_t expected_peer_size =
        impl_->selected_group == KeyAgreementGroup::modp_group14
            ? 256u
            : 65u;
    std::lock_guard<std::mutex> operation_lock(impl_->operation_mutex);
    if (peer_public_key.size != expected_peer_size ||
        (use == KeyAgreementUse::initial && impl_->derive_attempted) ||
        (use == KeyAgreementUse::retained_server_rekey && !impl_->derive_attempted)) {
        return CryptoError::invalid_input;
    }
    impl_->derive_attempted = true;

    SensitiveBuffer secret;
    secret.value.resize(expected_peer_size, 0);
    std::size_t secret_size = 0;
    int result = 0;
    {
        std::lock_guard<std::mutex> lock(*impl_->random_mutex);
        if (impl_->selected_group == KeyAgreementGroup::modp_group14) {
            if (!validate_group14_peer(peer_public_key)) {
                return CryptoError::invalid_input;
            }
            result = mbedtls_dhm_read_public(
                &impl_->dhm, peer_public_key.data, peer_public_key.size);
            if (result != 0) {
                return CryptoError::invalid_input;
            }
            result = mbedtls_dhm_calc_secret(
                &impl_->dhm, secret.value.data(), secret.value.size(), &secret_size,
                mbedtls_ctr_drbg_random, impl_->drbg);
        } else {
            // Mbed TLS's ECDH helper accepts the TLS ECPoint framing
            // (one-byte length followed by the SEC1 point). IEC 62351-4
            // carries the point octets directly in DHKey.
            std::array<std::uint8_t, 66> tls_encoded_peer{};
            tls_encoded_peer[0] = static_cast<std::uint8_t>(peer_public_key.size);
            std::copy(peer_public_key.data,
                      peer_public_key.data + peer_public_key.size,
                      tls_encoded_peer.begin() + 1);
            result = mbedtls_ecdh_read_public(&impl_->ecdh,
                                              tls_encoded_peer.data(),
                                              tls_encoded_peer.size());
            if (result != 0) {
                return CryptoError::invalid_input;
            }
            result = mbedtls_ecdh_calc_secret(
                &impl_->ecdh, &secret_size, secret.value.data(), secret.value.size(),
                mbedtls_ctr_drbg_random, impl_->drbg);
        }
    }
    if (result != 0 || secret_size == 0 || secret_size > secret.value.size()) {
        return CryptoError::provider_failure;
    }
    secret.value.resize(secret_size);
    shared_secret.swap(secret.value);
    return CryptoError::none;
#else
    (void)peer_public_key;
    (void)use;
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::generate_ephemeral_key(
    KeyAgreementGroup group, std::unique_ptr<E2eEphemeralKey>& key) {
    key.reset();
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        return status();
    }

    auto context = std::make_unique<E2eEphemeralKey::Impl>();
    context->selected_group = group;
    context->provider_lifetime = impl_;
    context->random_mutex = &impl_->random_mutex;
    context->drbg = &impl_->drbg;

    int result = 0;
    if (group == KeyAgreementGroup::modp_group14) {
        result = set_group14(context->dhm);
        if (result != 0) {
            return CryptoError::provider_failure;
        }
        context->public_value.resize(256);
        std::lock_guard<std::mutex> lock(impl_->random_mutex);
        result = mbedtls_dhm_make_public(
            &context->dhm, 32, context->public_value.data(),
            context->public_value.size(), mbedtls_ctr_drbg_random, &impl_->drbg);
    } else if (group == KeyAgreementGroup::ecdh_group23 ||
               group == KeyAgreementGroup::ecdh_group28) {
        const auto selected_curve = ecp_group(group);
        if (selected_curve == MBEDTLS_ECP_DP_NONE) {
            return CryptoError::unsupported_key;
        }
        result = mbedtls_ecdh_setup(&context->ecdh, selected_curve);
        if (result != 0) {
            return CryptoError::unsupported_key;
        }
        // mbedtls_ecdh_make_public writes a TLS ECPoint (length byte + raw
        // SEC1 point); strip the TLS-only prefix before exposing DHKey bytes.
        context->public_value.resize(66);
        std::size_t public_size = 0;
        {
            std::lock_guard<std::mutex> lock(impl_->random_mutex);
            result = mbedtls_ecdh_make_public(
                &context->ecdh, &public_size, context->public_value.data(),
                context->public_value.size(), mbedtls_ctr_drbg_random, &impl_->drbg);
        }
        if (result == 0) {
            if (public_size != 66 || context->public_value[0] != 65) {
                result = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            } else {
                std::move(context->public_value.begin() + 1,
                          context->public_value.begin() + public_size,
                          context->public_value.begin());
                context->public_value.resize(public_size - 1);
            }
        }
    } else {
        return CryptoError::unsupported_key;
    }
    if (result != 0 || context->public_value.empty()) {
        if (!context->public_value.empty()) {
            mbedtls_platform_zeroize(context->public_value.data(),
                                     context->public_value.size());
        }
        return CryptoError::provider_failure;
    }
    key.reset(new E2eEphemeralKey(std::move(context)));
    return CryptoError::none;
#else
    (void)group;
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::status() const noexcept {
    return impl_ ? impl_->provider_status : CryptoError::unavailable;
}

CryptoError E2eCryptoProvider::random(MutableByteView output) {
    if (!output.valid()) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        zero(output);
        return status();
    }
    if (output.size == 0) {
        return CryptoError::none;
    }
    std::lock_guard<std::mutex> lock(impl_->random_mutex);
    std::size_t offset = 0;
    while (offset < output.size) {
        const auto count = std::min(
            output.size - offset,
            static_cast<std::size_t>(MBEDTLS_CTR_DRBG_MAX_REQUEST));
        if (count == 0 ||
            mbedtls_ctr_drbg_random(&impl_->drbg, output.data + offset, count) != 0) {
            zero(output);
            return CryptoError::provider_failure;
        }
        offset += count;
    }
    return CryptoError::none;
#else
    zero(output);
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::sha256(
    ByteView input, std::array<std::uint8_t, 32>& digest) const noexcept {
    digest.fill(0);
    if (!valid(input)) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        return status();
    }
    const auto* info = sha256_info();
    if (!info || mbedtls_md(info, data_or_empty(input), input.size, digest.data()) != 0) {
        digest.fill(0);
        return CryptoError::provider_failure;
    }
    return CryptoError::none;
#else
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::hmac_sha256(
    ByteView key, ByteView input, std::array<std::uint8_t, 32>& mac) const noexcept {
    mac.fill(0);
    if (!valid(key) || key.empty() || !valid(input)) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        return status();
    }
    const auto* info = sha256_info();
    if (!info || mbedtls_md_hmac(info, data_or_empty(key), key.size,
                                 data_or_empty(input), input.size, mac.data()) != 0) {
        mac.fill(0);
        return CryptoError::provider_failure;
    }
    return CryptoError::none;
#else
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::verify_hmac_sha256(
    ByteView key, ByteView input, ByteView expected_mac) const noexcept {
    if (!valid(key) || key.empty() || !valid(input) || !valid(expected_mac) ||
        expected_mac.empty() || expected_mac.size > 32) {
        return CryptoError::invalid_input;
    }
    std::array<std::uint8_t, 32> calculated{};
    const auto result = hmac_sha256(key, input, calculated);
    if (result != CryptoError::none) {
        return result;
    }
    const bool matches = constant_time_equal(
        {calculated.data(), expected_mac.size}, expected_mac);
#if defined(CONFIG_MMS_SUPPORT_TLS)
    mbedtls_platform_zeroize(calculated.data(), calculated.size());
#else
    volatile std::uint8_t* bytes = calculated.data();
    for (std::size_t i = 0; i < calculated.size(); ++i) {
        bytes[i] = 0;
    }
#endif
    return matches ? CryptoError::none : CryptoError::authentication_failed;
}

CryptoError E2eCryptoProvider::hkdf_sha256(
    ByteView salt, ByteView input_key_material, ByteView info,
    MutableByteView output) const noexcept {
    if (!valid(salt) || !valid(input_key_material) || input_key_material.empty() ||
        !valid(info) || !output.valid() || output.size > 255u * 32u) {
        zero(output);
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        zero(output);
        return status();
    }
    if (output.size == 0) {
        return CryptoError::none;
    }
    const auto* info_sha256 = sha256_info();
    if (!info_sha256 ||
        mbedtls_hkdf(info_sha256, data_or_empty(salt), salt.size,
                     data_or_empty(input_key_material), input_key_material.size,
                     data_or_empty(info), info.size, output.data, output.size) != 0) {
        zero(output);
        return CryptoError::provider_failure;
    }
    return CryptoError::none;
#else
    zero(output);
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::aes_gmac(
    ByteView key, ByteView nonce, ByteView authenticated_data,
    MutableByteView tag) const noexcept {
    if (!valid(key) || (key.size != 16 && key.size != 32) ||
        !valid(nonce) || nonce.empty() || !valid(authenticated_data) ||
        !tag.valid() || !supported_gcm_tag_size(tag.size)) {
        zero(tag);
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        zero(tag);
        return status();
    }
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);
    const int setup = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES,
                                        data_or_empty(key),
                                        static_cast<unsigned int>(key.size * 8));
    unsigned char empty = 0;
    const int result = setup == 0
                           ? mbedtls_gcm_crypt_and_tag(
                                 &gcm, MBEDTLS_GCM_ENCRYPT, 0,
                                 data_or_empty(nonce), nonce.size,
                                 data_or_empty(authenticated_data), authenticated_data.size,
                                 &empty, &empty, tag.size, tag.data)
                           : setup;
    mbedtls_gcm_free(&gcm);
    if (result != 0) {
        zero(tag);
        return CryptoError::provider_failure;
    }
    return CryptoError::none;
#else
    zero(tag);
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::verify_aes_gmac(
    ByteView key, ByteView nonce, ByteView authenticated_data, ByteView tag) const noexcept {
    if (!valid(key) || (key.size != 16 && key.size != 32) ||
        !valid(nonce) || nonce.empty() || !valid(authenticated_data) ||
        !valid(tag) || !supported_gcm_tag_size(tag.size)) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        return status();
    }
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);
    const int setup = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES,
                                        data_or_empty(key),
                                        static_cast<unsigned int>(key.size * 8));
    unsigned char empty = 0;
    const int result = setup == 0
                           ? mbedtls_gcm_auth_decrypt(
                                 &gcm, 0, data_or_empty(nonce), nonce.size,
                                 data_or_empty(authenticated_data), authenticated_data.size,
                                 tag.data, tag.size, &empty, &empty)
                           : setup;
    mbedtls_gcm_free(&gcm);
    if (result == MBEDTLS_ERR_GCM_AUTH_FAILED) {
        return CryptoError::authentication_failed;
    }
    return result == 0 ? CryptoError::none : CryptoError::provider_failure;
#else
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::sign_sha256(
    SignatureScheme scheme, ByteView private_key_der_or_pem, ByteView password,
    ByteView message,
    std::vector<std::uint8_t>& signature) {
    signature.clear();
    if (!valid(private_key_der_or_pem) || private_key_der_or_pem.empty() ||
        !valid(password) || !valid(message)) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        return status();
    }
    const auto* info = sha256_info();
    std::array<std::uint8_t, 32> digest{};
    if (!info || mbedtls_md(info, data_or_empty(message), message.size, digest.data()) != 0) {
        return CryptoError::provider_failure;
    }

    ParsedPrivateKey key;
    SensitiveBuffer pem_buffer;
    const unsigned char* key_data = private_key_der_or_pem.data;
    std::size_t key_size = private_key_der_or_pem.size;
    if (is_pem(private_key_der_or_pem)) {
        pem_buffer.value.assign(private_key_der_or_pem.data,
                                private_key_der_or_pem.data + private_key_der_or_pem.size);
        pem_buffer.value.push_back(0);
        key_data = pem_buffer.value.data();
        key_size = pem_buffer.value.size();
    }

    int parse_result = 0;
    {
        std::lock_guard<std::mutex> lock(impl_->random_mutex);
#if MBEDTLS_VERSION_MAJOR >= 3
        parse_result = mbedtls_pk_parse_key(
            &key.value, key_data, key_size,
            password.empty() ? nullptr : data_or_empty(password), password.size,
            mbedtls_ctr_drbg_random, &impl_->drbg);
#else
        parse_result = mbedtls_pk_parse_key(
            &key.value, key_data, key_size,
            password.empty() ? nullptr : data_or_empty(password), password.size);
#endif
    }
    if (parse_result != 0) {
        mbedtls_platform_zeroize(digest.data(), digest.size());
        return CryptoError::invalid_input;
    }
    if (!key_matches_scheme(key.value, scheme)) {
        mbedtls_platform_zeroize(digest.data(), digest.size());
        return CryptoError::unsupported_key;
    }

    const std::size_t key_bytes = mbedtls_pk_get_len(&key.value);
    if (key_bytes == 0 || key_bytes > 8192) {
        mbedtls_platform_zeroize(digest.data(), digest.size());
        return CryptoError::unsupported_key;
    }
    signature.resize(key_bytes * 2 + 32);
    std::size_t signature_size = 0;
    int result = 0;
    {
        std::lock_guard<std::mutex> lock(impl_->random_mutex);
        result = mbedtls_pk_sign(&key.value, MBEDTLS_MD_SHA256,
                                 digest.data(), digest.size(),
                                 signature.data(), signature.size(), &signature_size,
                                 mbedtls_ctr_drbg_random, &impl_->drbg);
    }
    mbedtls_platform_zeroize(digest.data(), digest.size());
    if (result != 0 || signature_size == 0 || signature_size > signature.size()) {
        mbedtls_platform_zeroize(signature.data(), signature.size());
        signature.clear();
        return CryptoError::provider_failure;
    }
    signature.resize(signature_size);
    return CryptoError::none;
#else
    return CryptoError::unavailable;
#endif
}

CryptoError E2eCryptoProvider::verify_sha256(
    SignatureScheme scheme, ByteView certificate_der, ByteView message,
    ByteView signature) const noexcept {
    if (!valid(certificate_der) || certificate_der.empty() || !valid(message) ||
        !valid(signature) || signature.empty()) {
        return CryptoError::invalid_input;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (status() != CryptoError::none) {
        return status();
    }
    ParsedCertificate certificate;
    if (mbedtls_x509_crt_parse_der(&certificate.value, certificate_der.data,
                                   certificate_der.size) != 0) {
        return CryptoError::invalid_input;
    }
    if (!key_matches_scheme(certificate.value.pk, scheme)) {
        return CryptoError::unsupported_key;
    }
    const auto* info = sha256_info();
    std::array<std::uint8_t, 32> digest{};
    if (!info || mbedtls_md(info, data_or_empty(message), message.size, digest.data()) != 0) {
        return CryptoError::provider_failure;
    }
    const int result = mbedtls_pk_verify(&certificate.value.pk, MBEDTLS_MD_SHA256,
                                         digest.data(), digest.size(),
                                         signature.data, signature.size);
    mbedtls_platform_zeroize(digest.data(), digest.size());
    return result == 0 ? CryptoError::none : CryptoError::authentication_failed;
#else
    return CryptoError::unavailable;
#endif
}

} // namespace ccli::mms::e2e
