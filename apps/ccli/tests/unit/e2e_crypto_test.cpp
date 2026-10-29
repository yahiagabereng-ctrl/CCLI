#include "e2e_crypto.hpp"
#include "e2e_key_schedule.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

using ccli::mms::e2e::ByteView;
using ccli::mms::e2e::CryptoError;
using ccli::mms::e2e::E2eCryptoProvider;

int failures = 0;

void expect(bool condition, const char* name) {
    if (!condition) {
        std::cerr << "FAIL: " << name << '\n';
        ++failures;
    }
}

ByteView view(const std::vector<std::uint8_t>& bytes) {
    return {bytes.data(), bytes.size()};
}

std::vector<std::uint8_t> from_hex(const std::string& hex) {
    std::vector<std::uint8_t> result;
    if (hex.size() % 2 != 0) {
        return result;
    }
    auto digit = [](char value) -> int {
        if (value >= '0' && value <= '9') return value - '0';
        if (value >= 'a' && value <= 'f') return value - 'a' + 10;
        if (value >= 'A' && value <= 'F') return value - 'A' + 10;
        return -1;
    };
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        const int high = digit(hex[i]);
        const int low = digit(hex[i + 1]);
        if (high < 0 || low < 0) {
            return {};
        }
        result.push_back(static_cast<std::uint8_t>((high << 4) | low));
    }
    return result;
}

bool equal(ByteView left, ByteView right) {
    if (!left.valid() || !right.valid() || left.size != right.size) {
        return false;
    }
    for (std::size_t i = 0; i < left.size; ++i) {
        if (left.data[i] != right.data[i]) {
            return false;
        }
    }
    return true;
}

void test_sha256_empty_message(E2eCryptoProvider& crypto) {
    std::array<std::uint8_t, 32> digest{};
    expect(crypto.sha256({nullptr, 0}, digest) == CryptoError::none,
           "SHA-256 accepts an empty message");
    const auto expected = from_hex(
        "e3b0c44298fc1c149afbf4c8996fb924"
        "27ae41e4649b934ca495991b7852b855");
    expect(equal({digest.data(), digest.size()}, view(expected)),
           "SHA-256 matches the standard empty-message vector");
}

void test_hmac_sha256_rfc4231(E2eCryptoProvider& crypto) {
    const std::vector<std::uint8_t> key(20, 0x0b);
    const std::string message = "Hi There";
    const ByteView input{reinterpret_cast<const std::uint8_t*>(message.data()), message.size()};
    const auto expected = from_hex(
        "b0344c61d8db38535ca8afceaf0bf12b"
        "881dc200c9833da726e9376c2e32cff7");
    std::array<std::uint8_t, 32> mac{};
    expect(crypto.hmac_sha256(view(key), input, mac) == CryptoError::none,
           "HMAC-SHA-256 computes");
    expect(equal({mac.data(), mac.size()}, view(expected)),
           "HMAC-SHA-256 matches RFC 4231 case 1");
    expect(crypto.verify_hmac_sha256(view(key), input, view(expected)) == CryptoError::none,
           "HMAC verification accepts the known answer");
    expect(crypto.verify_hmac_sha256(view(key), input,
                                     {expected.data(), 16}) == CryptoError::none,
           "HMAC verification supports a caller-selected tag prefix");
    auto altered = expected;
    altered[0] ^= 1;
    expect(crypto.verify_hmac_sha256(view(key), input, view(altered)) ==
               CryptoError::authentication_failed,
           "HMAC verification rejects a changed tag");
}

void test_hkdf_rfc5869_case_one(E2eCryptoProvider& crypto) {
    const std::vector<std::uint8_t> ikm(22, 0x0b);
    const auto salt = from_hex("000102030405060708090a0b0c");
    const auto info = from_hex("f0f1f2f3f4f5f6f7f8f9");
    const auto expected = from_hex(
        "3cb25f25faacd57a90434f64d0362f2a"
        "2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
        "34007208d5b887185865");
    std::vector<std::uint8_t> output(expected.size(), 0);
    expect(crypto.hkdf_sha256(view(salt), view(ikm), view(info),
                              {output.data(), output.size()}) == CryptoError::none,
           "HKDF-SHA-256 computes RFC 5869 case 1");
    expect(equal(view(output), view(expected)),
           "HKDF-SHA-256 matches RFC 5869 case 1 output");
}

void test_aes_gmac_nist_vector(E2eCryptoProvider& crypto) {
    const std::vector<std::uint8_t> key(16, 0);
    const std::vector<std::uint8_t> nonce(12, 0);
    const std::vector<std::uint8_t> empty;
    const auto expected = from_hex("58e2fccefa7e3061367f1d57a4e7455a");
    std::array<std::uint8_t, 16> tag{};
    expect(crypto.aes_gmac(view(key), view(nonce), view(empty),
                           {tag.data(), tag.size()}) == CryptoError::none,
           "AES-GMAC computes with an empty authenticated message");
    expect(equal({tag.data(), tag.size()}, view(expected)),
           "AES-GMAC matches the NIST AES-128-GCM zero vector");
    expect(crypto.verify_aes_gmac(view(key), view(nonce), view(empty), view(expected)) ==
               CryptoError::none,
           "AES-GMAC verification accepts the known answer");
    auto altered = expected;
    altered.back() ^= 1;
    expect(crypto.verify_aes_gmac(view(key), view(nonce), view(empty), view(altered)) ==
               CryptoError::authentication_failed,
           "AES-GMAC verification rejects a changed tag");

    const std::vector<std::uint8_t> key256(32, 0);
    const auto expected256 = from_hex("530f8afbc74536b9a963b4f1c4cb738b");
    std::array<std::uint8_t, 16> tag256{};
    expect(crypto.aes_gmac(view(key256), view(nonce), view(empty),
                           {tag256.data(), tag256.size()}) == CryptoError::none,
           "AES-256-GMAC computes");
    expect(equal({tag256.data(), tag256.size()}, view(expected256)),
           "AES-256-GMAC matches the NIST zero vector");
}

void test_ephemeral_key_agreement(E2eCryptoProvider& crypto,
                                  ccli::mms::e2e::KeyAgreementGroup group,
                                  const char* label,
                                  std::size_t expected_public_size) {
    std::unique_ptr<ccli::mms::e2e::E2eEphemeralKey> left;
    std::unique_ptr<ccli::mms::e2e::E2eEphemeralKey> right;
    const auto left_result = crypto.generate_ephemeral_key(group, left);
    if (left_result != CryptoError::none) {
        std::cerr << "DETAIL: " << label << " first key generation returned "
                  << static_cast<unsigned>(left_result) << '\n';
    }
    expect(left_result == CryptoError::none,
           "ephemeral key generation succeeds");
    const auto right_result = crypto.generate_ephemeral_key(group, right);
    if (right_result != CryptoError::none) {
        std::cerr << "DETAIL: " << label << " second key generation returned "
                  << static_cast<unsigned>(right_result) << '\n';
    }
    expect(right_result == CryptoError::none,
           "second ephemeral key generation succeeds");
    if (!left || !right) {
        std::cerr << "SKIP: key agreement exchange for " << label
                  << " because key generation failed\n";
        return;
    }

    expect(left->group() == group && right->group() == group,
           "ephemeral keys retain the selected group");
    expect(left->public_key().size == expected_public_size &&
               right->public_key().size == expected_public_size,
           "ephemeral public key has the group encoding size");
    if (group != ccli::mms::e2e::KeyAgreementGroup::modp_group14) {
        expect(left->public_key().data[0] == 0x04 &&
                   right->public_key().data[0] == 0x04,
               "ECDH public keys expose raw uncompressed SEC1 points");
    }

    std::vector<std::uint8_t> left_secret;
    std::vector<std::uint8_t> right_secret;
    expect(left->derive_shared_secret(right->public_key(), left_secret) ==
               CryptoError::none,
           "left peer derives the shared secret");
    expect(right->derive_shared_secret(left->public_key(), right_secret) ==
               CryptoError::none,
           "right peer derives the shared secret");
    expect(!left_secret.empty() && equal(view(left_secret), view(right_secret)),
           "both peers derive the same secret");
    std::unique_ptr<ccli::mms::e2e::E2eEphemeralKey> refreshed_client;
    expect(crypto.generate_ephemeral_key(group, refreshed_client) == CryptoError::none,
           "fresh client DH key for rekey");
    if (refreshed_client) {
        std::vector<std::uint8_t> client_secret, server_secret, rejected;
        expect(refreshed_client->derive_shared_secret(right->public_key(), client_secret) == CryptoError::none &&
            right->derive_shared_secret(refreshed_client->public_key(), server_secret,
                ccli::mms::e2e::KeyAgreementUse::retained_server_rekey) == CryptoError::none &&
            equal(view(client_secret), view(server_secret)), "retained server key supports a fresh client rekey");
        expect(right->derive_shared_secret(refreshed_client->public_key(), rejected) == CryptoError::invalid_input,
               "initial-use API still rejects accidental key reuse");
    }

    std::unique_ptr<ccli::mms::e2e::E2eEphemeralKey> invalid_peer_key;
    expect(crypto.generate_ephemeral_key(group, invalid_peer_key) == CryptoError::none,
           "fresh key generation for invalid-peer check succeeds");
    if (invalid_peer_key) {
        std::vector<std::uint8_t> malformed(expected_public_size, 0);
        std::vector<std::uint8_t> rejected_secret{0xaa};
        const auto invalid_result = invalid_peer_key->derive_shared_secret(
            view(malformed), rejected_secret);
        expect(invalid_result == CryptoError::invalid_input,
               "invalid peer public value is rejected");
        expect(rejected_secret.empty(),
               "failed key agreement does not retain an output secret");
    }

    (void)label;
}

void test_key_agreement_groups(E2eCryptoProvider& crypto) {
    using ccli::mms::e2e::KeyAgreementGroup;
    test_ephemeral_key_agreement(crypto, KeyAgreementGroup::modp_group14,
                                 "MODP group 14", 256);
    test_ephemeral_key_agreement(crypto, KeyAgreementGroup::ecdh_group23,
                                 "ECDH group 23", 65);
    test_ephemeral_key_agreement(crypto, KeyAgreementGroup::ecdh_group28,
                                 "ECDH group 28", 65);
}

void test_transcript_salts(E2eCryptoProvider& crypto) {
    const auto data1 = from_hex("010203");
    const auto data2 = from_hex("a0b0");
    const auto expected_initial = from_hex(
        "6220fa2c59ab30c1a4b538315f48b5b1"
        "8445f41cac74d2a6c992e0af8309dc15");
    std::vector<std::uint8_t> salt;
    expect(ccli::mms::e2e::derive_initial_transcript_salt_sha256(
               crypto, view(data1), view(data2), salt) == CryptoError::none,
           "initial transcript salt derives");
    expect(equal(view(salt), view(expected_initial)),
           "initial salt follows the transcript formula with a zero-octet seed");

    const auto data3 = from_hex("10203040");
    const auto expected_rekey = from_hex(
        "c6edffa721788b1a9bb89faf4d3bc9e0"
        "7e8094d1cc939255b513b4a3f5c833a3");
    std::vector<std::uint8_t> rekey_salt;
    expect(ccli::mms::e2e::derive_rekey_transcript_salt_sha256(
               crypto, view(data3), view(salt), rekey_salt) == CryptoError::none,
           "rekey transcript salt derives");
    expect(equal(view(rekey_salt), view(expected_rekey)),
           "rekey salt uses the retained initial salt");
}

void test_directional_key_schedule(E2eCryptoProvider& crypto) {
    const std::vector<std::uint8_t> shared_secret(22, 0x0b);
    const auto salt = from_hex(
        "000102030405060708090a0b0c0d0e0f"
        "101112131415161718191a1b1c1d1e1f");
    const auto expected64 = from_hex(
        "95db16190b27b09d636e5373d0912c97"
        "73cead4617a75af0593c31603548c86c"
        "9dc53d3efb747e5ab4fc2dd2021b7d77"
        "7bba8bb27b006b3e57c880ca4d7dfe06");
    const auto expected96 = from_hex(
        "95db16190b27b09d636e5373d0912c97"
        "73cead4617a75af0593c31603548c86c"
        "9dc53d3efb747e5ab4fc2dd2021b7d77"
        "7bba8bb27b006b3e57c880ca4d7dfe06"
        "455048e4c535cfa77b202c3e6bc902c9"
        "a0c2ec2bd8ec07db467a064ca97ad27e");

    ccli::mms::e2e::E2eDirectionalKeyMaterial keys;
    ccli::mms::e2e::E2eKeyScheduleLengths two_key_lengths{32, 32, 0, 0};
    expect(ccli::mms::e2e::derive_directional_key_material_sha256(
               crypto, view(shared_secret), view(salt), two_key_lengths, keys) ==
               CryptoError::none,
           "two-direction key schedule derives");
    expect(equal(view(keys.client_directional_key),
                 {expected64.data(), 32}) &&
               equal(view(keys.server_directional_key),
                     {expected64.data() + 32, 32}),
           "two-key OKM splits client first and server second");
    expect(keys.client_content_encryption_key.empty() &&
               keys.server_content_encryption_key.empty(),
           "two-key mode has no separate content-encryption keys");

    ccli::mms::e2e::E2eKeyScheduleLengths four_key_lengths{16, 16, 16, 16};
    expect(ccli::mms::e2e::derive_directional_key_material_sha256(
               crypto, view(shared_secret), view(salt), four_key_lengths, keys) ==
               CryptoError::none,
           "four-directional-key schedule derives");
    expect(equal(view(keys.client_directional_key),
                 {expected64.data(), 16}) &&
               equal(view(keys.server_directional_key),
                     {expected64.data() + 16, 16}) &&
               equal(view(keys.client_content_encryption_key),
                     {expected64.data() + 32, 16}) &&
               equal(view(keys.server_content_encryption_key),
                     {expected64.data() + 48, 16}),
           "four-key OKM uses IEC client/server ICV/encryption order");

    ccli::mms::e2e::E2eKeyScheduleLengths mixed_aes_sizes{16, 16, 32, 32};
    expect(ccli::mms::e2e::derive_directional_key_material_sha256(
               crypto, view(shared_secret), view(salt), mixed_aes_sizes, keys) ==
               CryptoError::none,
           "AES-256 content-key schedule derives");
    expect(equal(view(keys.client_directional_key),
                 {expected96.data(), 16}) &&
               equal(view(keys.server_directional_key),
                     {expected96.data() + 16, 16}) &&
               equal(view(keys.client_content_encryption_key),
                     {expected96.data() + 32, 32}) &&
               equal(view(keys.server_content_encryption_key),
                     {expected96.data() + 64, 32}),
           "AES-256 content keys follow both ICV keys in the OKM");

    ccli::mms::e2e::E2eKeyScheduleLengths invalid_lengths{15, 15, 0, 0};
    expect(ccli::mms::e2e::derive_directional_key_material_sha256(
               crypto, view(shared_secret), view(salt), invalid_lengths, keys) ==
               CryptoError::invalid_input,
           "unsupported key sizes are rejected");
    expect(keys.client_directional_key.empty() && keys.server_directional_key.empty(),
           "rejected key schedule leaves no old key material");
}

std::vector<std::uint8_t> read_fixture(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<std::uint8_t> bytes;
    char value = 0;
    while (file.get(value)) {
        bytes.push_back(static_cast<std::uint8_t>(
            static_cast<unsigned char>(value)));
    }
    return file.bad() ? std::vector<std::uint8_t>{} : bytes;
}

void test_signature_fixture(E2eCryptoProvider& crypto,
                            const std::string& fixture_dir,
                            const char* fixture_name,
                            ccli::mms::e2e::SignatureScheme scheme,
                            const char* label) {
    const auto private_key = read_fixture(
        fixture_dir + "/" + fixture_name + ".key.pem");
    const auto certificate = read_fixture(
        fixture_dir + "/" + fixture_name + ".cert.der");
    expect(!private_key.empty() && !certificate.empty(),
           "signature fixture files are readable");
    if (private_key.empty() || certificate.empty()) {
        return;
    }

    const std::string message = "IEC 62351-4 E2E signature fixture";
    const ByteView message_view{
        reinterpret_cast<const std::uint8_t*>(message.data()), message.size()};
    std::vector<std::uint8_t> signature;
    const auto sign_result = crypto.sign_sha256(
        scheme, view(private_key), {nullptr, 0}, message_view, signature);
    if (sign_result != CryptoError::none) {
        std::cerr << "DETAIL: " << label << " signing returned "
                  << static_cast<unsigned>(sign_result) << '\n';
    }
    expect(sign_result == CryptoError::none && !signature.empty(),
           "supported private key signs SHA-256 message");
    if (signature.empty()) {
        return;
    }
    expect(crypto.verify_sha256(scheme, view(certificate), message_view,
                                view(signature)) == CryptoError::none,
           "matching certificate verifies signature");

    const std::string changed_message = "IEC 62351-4 altered message";
    const ByteView changed_view{
        reinterpret_cast<const std::uint8_t*>(changed_message.data()),
        changed_message.size()};
    expect(crypto.verify_sha256(scheme, view(certificate), changed_view,
                                view(signature)) ==
               CryptoError::authentication_failed,
           "signature rejects a changed message");

    auto changed_signature = signature;
    changed_signature[changed_signature.size() / 2] ^= 1;
    expect(crypto.verify_sha256(scheme, view(certificate), message_view,
                                view(changed_signature)) ==
               CryptoError::authentication_failed,
           "signature rejects a changed signature value");
}

void test_signature_provider(E2eCryptoProvider& crypto,
                             const std::string& fixture_dir) {
    using ccli::mms::e2e::SignatureScheme;
    test_signature_fixture(crypto, fixture_dir, "rsa_2048",
                           SignatureScheme::rsa_pkcs1_v15_sha256,
                           "RSA-2048/SHA-256");
    test_signature_fixture(crypto, fixture_dir, "secp256r1",
                           SignatureScheme::ecdsa_sha256,
                           "secp256r1 ECDSA/SHA-256");
    test_signature_fixture(crypto, fixture_dir, "brainpool256r1",
                           SignatureScheme::ecdsa_sha256,
                           "Brainpool P-256 ECDSA/SHA-256");

    const auto small_rsa_key = read_fixture(
        fixture_dir + "/rsa_1024.key.pem");
    const auto small_rsa_certificate = read_fixture(
        fixture_dir + "/rsa_1024.cert.der");
    expect(!small_rsa_key.empty() && !small_rsa_certificate.empty(),
           "small RSA negative fixtures are readable");
    if (!small_rsa_key.empty() && !small_rsa_certificate.empty()) {
        const std::string message = "undersized RSA key must be rejected";
        const ByteView message_view{
            reinterpret_cast<const std::uint8_t*>(message.data()), message.size()};
        std::vector<std::uint8_t> signature{0xaa};
        expect(crypto.sign_sha256(SignatureScheme::rsa_pkcs1_v15_sha256,
                                  view(small_rsa_key), {nullptr, 0},
                                  message_view, signature) ==
                   CryptoError::unsupported_key && signature.empty(),
               "RSA keys below 2048 bits are rejected for signing");
        expect(crypto.verify_sha256(SignatureScheme::rsa_pkcs1_v15_sha256,
                                    view(small_rsa_certificate), message_view,
                                    {reinterpret_cast<const std::uint8_t*>("x"), 1}) ==
                   CryptoError::unsupported_key,
               "RSA certificates below 2048 bits are rejected for verification");
    }

    const auto small_ec_key = read_fixture(
        fixture_dir + "/secp224r1.key.pem");
    const auto small_ec_certificate = read_fixture(
        fixture_dir + "/secp224r1.cert.der");
    expect(!small_ec_key.empty() && !small_ec_certificate.empty(),
           "small EC negative fixtures are readable");
    if (!small_ec_key.empty() && !small_ec_certificate.empty()) {
        const std::string message = "undersized EC key must be rejected";
        const ByteView message_view{
            reinterpret_cast<const std::uint8_t*>(message.data()), message.size()};
        std::vector<std::uint8_t> signature{0xaa};
        expect(crypto.sign_sha256(SignatureScheme::ecdsa_sha256,
                                  view(small_ec_key), {nullptr, 0},
                                  message_view, signature) != CryptoError::none &&
                   signature.empty(),
               "EC keys below 256 bits are rejected for signing");
        const std::uint8_t placeholder = 0;
        expect(crypto.verify_sha256(SignatureScheme::ecdsa_sha256,
                                    view(small_ec_certificate), message_view,
                                    {&placeholder, 1}) != CryptoError::none,
               "EC certificates below 256 bits are rejected for verification");
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: e2e_crypto_test <fixture-directory>\n";
        return 2;
    }
    E2eCryptoProvider crypto;
    expect(crypto.status() == CryptoError::none,
           "Mbed TLS crypto provider initializes");
    if (crypto.status() == CryptoError::none) {
        test_sha256_empty_message(crypto);
        test_hmac_sha256_rfc4231(crypto);
        test_hkdf_rfc5869_case_one(crypto);
        test_aes_gmac_nist_vector(crypto);
        test_key_agreement_groups(crypto);
        test_transcript_salts(crypto);
        test_directional_key_schedule(crypto);
        test_signature_provider(crypto, argv[1]);
    }
    if (failures != 0) {
        std::cerr << failures << " E2E crypto check(s) failed\n";
        return 1;
    }
    std::cout << "E2E crypto provider checks passed\n";
    return 0;
}
