#include "mms_acse_auth.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

bool expect_result(const char* label, cci::adapters::MmsAcseAuthResult got,
                   cci::adapters::MmsAcseAuthResult want) {
    if (got != want) {
        std::cerr << label << ": expected " << static_cast<int>(want) << " got "
                  << static_cast<int>(got) << '\n';
        return false;
    }
    return true;
}

}  // namespace

int main() {
    const std::vector<uint8_t> lab_cert{0x30, 0x01, 0x02};
    const uint8_t* buf = lab_cert.data();
    const int len = static_cast<int>(lab_cert.size());

    cci::adapters::MmsAcseCredentialStore open{};
    open.rbac_enabled = false;

    if (!expect_result("TLS open RBAC",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Tls, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptGeneric)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("CERTIFICATE open RBAC",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Certificate, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptGeneric)) {
        return EXIT_FAILURE;
    }

    cci::adapters::MmsAcseCredentialStore rbac{};
    rbac.rbac_enabled = true;
    rbac.dso_der = lab_cert;
    rbac.viewer_der = {0x99};

    if (!expect_result("CERTIFICATE DSO allowlist",
                       cci::adapters::evaluate_acse_authentication(
                           rbac, cci::adapters::MmsAcseMechanism::Certificate, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptDso)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("TLS DSO allowlist",
                       cci::adapters::evaluate_acse_authentication(
                           rbac, cci::adapters::MmsAcseMechanism::Tls, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptDso)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("viewer cert",
                       cci::adapters::evaluate_acse_authentication(
                           rbac, cci::adapters::MmsAcseMechanism::Certificate,
                           rbac.viewer_der.data(),
                           static_cast<int>(rbac.viewer_der.size())),
                       cci::adapters::MmsAcseAuthResult::AcceptViewer)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("password rejected",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Password, buf, len),
                       cci::adapters::MmsAcseAuthResult::RejectMechanism)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("empty cert",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Tls, nullptr, 0),
                       cci::adapters::MmsAcseAuthResult::RejectEmptyCert)) {
        return EXIT_FAILURE;
    }

    if (!cci::adapters::mms_acse_mechanism_supported(cci::adapters::MmsAcseMechanism::Certificate) ||
        !cci::adapters::mms_acse_mechanism_supported(cci::adapters::MmsAcseMechanism::Tls)) {
        std::cerr << "supported mechanism check failed\n";
        return EXIT_FAILURE;
    }

    std::cout << "mms_acse_auth_test: PASS (P3-07 ACSE TLS + CERTIFICATE)\n";
    return EXIT_SUCCESS;
}
