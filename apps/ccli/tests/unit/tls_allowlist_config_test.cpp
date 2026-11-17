#include "config/ccli_config.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
    const auto file = std::filesystem::temp_directory_path() /
        ("ccli-tls-pins-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".yaml");
    {
        std::ofstream out(file);
        out << "mms:\n  tls_client_cert: /existing/dso.pem\n"
               "  tls_additional_own_key: /identity/server-ecdsa.key\n"
               "  tls_additional_own_cert: /identity/server-ecdsa.pem\n"
               "  tls_crl: /pki/cci.crl\n"
               "  rbac_policy: /etc/ccli/rbac/roles.xml\n"
               "  rbac_owner_cert: /etc/ccli/rbac/owner.pem\n"
               "  tls_additional_client_cert: /existing/transport.pem\n"
               "  tls_allowed_client_cert: /roles/viewer.pem\n"
               "  tls_allowed_client_cert: /roles/operator.pem\n"
               "  tls_allowed_client_cert: /roles/engineer.pem\n"
               "iec104:\n  tls_client_cert: /existing/104-legacy.pem\n"
               "  tls_allowed_client_cert: /roles/104-operator.pem\n"
               "  tls_allowed_client_cert: /roles/104-maintenance.pem\n";
    }
    const auto cfg = cci::core::load_config(file.string());
    std::filesystem::remove(file);
    if (!cfg.loaded_from_file || cfg.mms.tls.client_cert != "/existing/dso.pem" ||
        cfg.mms.tls.additional_client_cert != "/existing/transport.pem" ||
        cfg.mms.tls.allowed_client_certs != std::vector<std::string>{
            "/roles/viewer.pem", "/roles/operator.pem", "/roles/engineer.pem"} ||
        cfg.iec104.tls.client_cert != "/existing/104-legacy.pem" ||
        cfg.iec104.tls.allowed_client_certs != std::vector<std::string>{
            "/roles/104-operator.pem", "/roles/104-maintenance.pem"} ||
        std::find(cfg.event_log.platform_credential_paths.begin(), cfg.event_log.platform_credential_paths.end(),
                  "/identity/server-ecdsa.pem") == cfg.event_log.platform_credential_paths.end() ||
        std::find(cfg.event_log.platform_credential_paths.begin(), cfg.event_log.platform_credential_paths.end(),
                  "/pki/cci.crl") == cfg.event_log.platform_credential_paths.end() ||
        std::find(cfg.event_log.platform_credential_paths.begin(), cfg.event_log.platform_credential_paths.end(),
                  "/etc/ccli/rbac/roles.xml") == cfg.event_log.platform_credential_paths.end() ||
        std::find(cfg.event_log.platform_credential_paths.begin(), cfg.event_log.platform_credential_paths.end(),
                  "/roles/104-maintenance.pem") == cfg.event_log.platform_credential_paths.end()) {
        std::cerr << "Repeatable TLS pins or legacy configuration was lost\n";
        return 1;
    }
    for (const auto& bad : {"0", "599", "86401", "-1", "600junk", "999999999999999999", ""}) {
        { std::ofstream out(file); out << "mms:\n  tls_renegotiation_s: " << bad
            << "\niec104:\n  tls_renegotiation_s: " << bad << "\n"; }
        const auto invalid = cci::core::load_config(file.string());
        if (invalid.mms.tls.configuration_valid || invalid.iec104.tls.configuration_valid) return 2;
    }
    {
        std::ofstream out(file);
        out << "mms:\n  tls_renegotiation_s: 600\n  tls_resumption_s: 7200\n  tls_revocation_check_s: 1\n"
               "  tls_allow_sha384: false\n  tls_allow_sha512: true\n"
               "iec104:\n  tls_crl: /public/crl.pem\n  tls_renegotiation_s: 86400\n"
               "  tls_resumption_s: 1\n  tls_revocation_check_s: 86400\n"
               "  tls_allow_sha384: true\n";
    }
    const auto timing = cci::core::load_config(file.string());
    std::filesystem::remove(file);
    if (!timing.mms.tls.configuration_valid || !timing.iec104.tls.configuration_valid ||
        timing.mms.tls.renegotiation_s != 600 || timing.mms.tls.resumption_s != 7200 ||
        timing.mms.tls.revocation_check_s != 1 || timing.iec104.tls.renegotiation_s != 86400 ||
        timing.iec104.tls.resumption_s != 1 || timing.iec104.tls.revocation_check_s != 86400 ||
        timing.iec104.tls.crl_path != "/public/crl.pem" ||
        timing.mms.tls.allow_sha384 || !timing.mms.tls.allow_sha512 ||
        !timing.iec104.tls.allow_sha384 || timing.iec104.tls.allow_sha512) return 3;
    {
        std::ofstream out(file);
        out << "mms:\n  tls_allow_sha384: sometimes\niec104:\n  tls_allow_sha512: maybe\n";
    }
    const auto invalid_hash_policy = cci::core::load_config(file.string());
    std::filesystem::remove(file);
    if (invalid_hash_policy.mms.tls.configuration_valid ||
        invalid_hash_policy.iec104.tls.configuration_valid) return 4;
    return 0;
}
