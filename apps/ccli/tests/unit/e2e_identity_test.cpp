#include "e2e_identity.hpp"
#include "mbedtls/x509_crl.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

using Bytes = std::vector<std::uint8_t>;
using ccli::mms::e2e::ByteView;
using ccli::mms::e2e::E2ePeerCertificatePath;
using ccli::mms::e2e::IdentityError;

Bytes read_file(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return {};
    }
    return Bytes(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

ByteView view(const Bytes& bytes) {
    return {bytes.data(), bytes.size()};
}

bool expect(const char* label, IdentityError actual, IdentityError expected) {
    if (actual == expected) {
        return true;
    }
    std::cerr << label << ": got " << static_cast<unsigned int>(actual)
              << ", expected " << static_cast<unsigned int>(expected) << '\n';
    return false;
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: e2e_identity_test <fixture-dir>\n";
        return 2;
    }
    const std::string directory = argv[1];
    const auto root = read_file(directory + "/root.der");
    const auto identity_oid = read_file(directory + "/identity_oid.bin");
    const auto root_crl = read_file(directory + "/root.crl.der");
    const auto revoked_crl = read_file(directory + "/root_revoked.crl.der");
    const auto rsa_leaf = read_file(directory + "/leaf_rsa_oid.der");
    const auto ec_leaf = read_file(directory + "/leaf_ec_oid.der");
    const auto weak_leaf = read_file(directory + "/leaf_weak_rsa_oid.der");
    const auto no_key_agreement = read_file(directory + "/leaf_no_key_agreement_oid.der");
    const auto text_leaf = read_file(directory + "/leaf_utf8_subject.der");
    const auto no_crl_distribution_point = read_file(directory + "/leaf_no_crl_dp_oid.der");
    if (root.empty() || identity_oid.empty() || root_crl.empty() || revoked_crl.empty() ||
        rsa_leaf.empty() || ec_leaf.empty() || weak_leaf.empty() ||
        no_key_agreement.empty() || text_leaf.empty() || no_crl_distribution_point.empty()) {
        std::cerr << "certificate fixture missing or empty\n";
        return 2;
    }

    E2ePeerCertificatePath path;
    path.expected_identity_oid_value = view(identity_oid);
    path.trust_anchors.push_back(view(root));
    path.issuer_crls.push_back(view(root_crl));

    bool ok = true;
    ok &= expect("RSA-2048 OID subject", ccli::mms::e2e::validate_peer_certificate(
                   view(rsa_leaf), path), IdentityError::none);
    ok &= expect("P-256 OID subject", ccli::mms::e2e::validate_peer_certificate(
                   view(ec_leaf), path), IdentityError::none);

    auto wrong_identity = path;
    const Bytes other_oid{0x2a, 0x03, 0x04};
    wrong_identity.expected_identity_oid_value = view(other_oid);
    ok &= expect("association identity mismatch", ccli::mms::e2e::validate_peer_certificate(
                   view(rsa_leaf), wrong_identity), IdentityError::association_identity_mismatch);

    ok &= expect("string-valued subject rejected", ccli::mms::e2e::validate_peer_certificate(
                   view(text_leaf), path), IdentityError::association_identity_mismatch);
    ok &= expect("RSA-1024 rejected", ccli::mms::e2e::validate_peer_certificate(
                   view(weak_leaf), path), IdentityError::unsupported_public_key);
    ok &= expect("missing keyAgreement rejected", ccli::mms::e2e::validate_peer_certificate(
                   view(no_key_agreement), path), IdentityError::key_usage_invalid);
    ok &= expect("missing CRL distribution point rejected",
                   ccli::mms::e2e::validate_peer_certificate(
                       view(no_crl_distribution_point), path),
                   IdentityError::crl_distribution_point_missing);

    auto no_crl = path;
    no_crl.issuer_crls.clear();
    ok &= expect("missing CRL coverage rejected", ccli::mms::e2e::validate_peer_certificate(
                   view(rsa_leaf), no_crl), IdentityError::revocation_unavailable);

    auto revoked_path = path;
    revoked_path.issuer_crls.clear();
    revoked_path.issuer_crls.push_back(view(revoked_crl));
    ok &= expect("revoked peer rejected", ccli::mms::e2e::validate_peer_certificate(
                   view(rsa_leaf), revoked_path), IdentityError::revoked_or_untrusted);
    for (const auto* name : {"matching_scope", "wrong_scope", "ca_only", "partial_reasons", "malformed_scope"}) {
        const auto crl = read_file(directory + "/" + name + ".crl.der");
        if (crl.empty()) { std::cerr << "scope CRL fixture missing\n"; return 2; }
        auto scoped_path = path;
        scoped_path.issuer_crls = {view(crl)};
        const auto result = ccli::mms::e2e::validate_peer_certificate(view(rsa_leaf), scoped_path);
        if (std::string(name) == "matching_scope") {
            ok &= expect("matching distribution-point scope accepted", result, IdentityError::none);
            mbedtls_x509_crl strict; mbedtls_x509_crl_init(&strict);
            const auto strict_result = mbedtls_x509_crl_parse_der(&strict, crl.data(), crl.size());
            mbedtls_x509_crl_free(&strict);
            if (strict_result == 0) { std::cerr << "default/TLS CRL parser unexpectedly accepts unsupported critical IDP\n"; ok = false; }
        } else if (std::string(name) == "malformed_scope") {
            const bool rejected = result == IdentityError::invalid_crl || result == IdentityError::revocation_unavailable;
            if (!rejected) { std::cerr << "malformed CRL scope did not fail closed\n"; ok = false; }
        } else
            ok &= expect(name, result, IdentityError::revocation_unavailable);
    }
    return ok ? 0 : 1;
}
