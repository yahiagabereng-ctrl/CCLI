#include "iec104_tls.hpp"

#include <iostream>
#include <string>

#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)
extern "C" {
#include "tls_config.h"
#include "../security/iec62351_tls.h"
}
#endif

namespace cci::adapters {

#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)

namespace {

void tls_event(void* parameter, TLSEventLevel level, int code,
               const char* message, TLSConnection connection) {
    auto* events = static_cast<Iec104TlsEvents*>(parameter);
    char address[61]{};
    const char* peer = connection ? TLSConnection_getPeerAddress(connection, address) : nullptr;
    const std::string detail = std::string(message ? message : "TLS event") +
        (peer ? std::string(" peer=") + peer : "");
    if (events && events->handler) events->handler(events->parameter, level, code, detail.c_str());
    else std::cerr << "iec104: TLS_SECURITY_EVENT severity=" << level << " code=" << code
                   << " " << detail << "\n";
}

bool load_pem(TLSConfiguration tls, const core::Iec104TlsConfig& cfg, std::string& err) {
    if (cfg.own_key.empty() || cfg.own_cert.empty() || cfg.ca_cert.empty() ||
        cfg.crl_path.empty() || !cfg.configuration_valid) {
        err = "tls_own_key, tls_own_cert, tls_ca_cert, tls_crl and valid TLS timers required (62351-3)";
        return false;
    }
    if (!TLSConfiguration_setOwnKeyFromFile(tls, cfg.own_key.c_str(), nullptr)) {
        err = "tls_own_key load failed: " + cfg.own_key;
        return false;
    }
    if (!TLSConfiguration_setOwnCertificateFromFile(tls, cfg.own_cert.c_str())) {
        err = "tls_own_cert load failed: " + cfg.own_cert;
        return false;
    }
    if (!cfg.ca_cert.empty()) {
        if (!TLSConfiguration_addCACertificateFromFile(tls, cfg.ca_cert.c_str())) {
            err = "tls_ca_cert load failed: " + cfg.ca_cert;
            return false;
        }
        TLSConfiguration_setChainValidation(tls, true);
    }
    if (!cfg.crl_path.empty() && !TLSConfiguration_addCRLFromFile(tls, cfg.crl_path.c_str())) {
        err = "tls_crl load failed";
        return false;
    }
    bool have_allow = false;
    if (!cfg.client_cert.empty()) {
        if (!TLSConfiguration_addAllowedCertificateFromFile(tls, cfg.client_cert.c_str())) {
            err = "tls_client_cert allow-list failed: " + cfg.client_cert;
            return false;
        }
        have_allow = true;
    }
    for (const auto& cert : cfg.allowed_client_certs) {
        if (!TLSConfiguration_addAllowedCertificateFromFile(tls, cert.c_str())) {
            err = "tls_allowed_client_cert allow-list failed: " + cert;
            return false;
        }
        have_allow = true;
    }
    TLSConfiguration_setAllowOnlyKnownCertificates(tls, have_allow);
    if (!CCLI_TLS_setOptionalHashes(tls, cfg.allow_sha384, cfg.allow_sha512) ||
        !CCLI_TLS_configure(tls, cfg.crl_path.c_str(), cfg.revocation_check_s,
                           cfg.renegotiation_s, cfg.resumption_s)) {
        err = "IEC 62351-3 TLS identity, revocation or policy validation failed";
        return false;
    }
    return true;
}

}  // namespace

#endif

bool iec104_create_tls_config(const core::Iec104TlsConfig& cfg, void** out_tls,
                              std::string& err,
                              Iec104TlsEvents* events) {
#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)
    if (out_tls == nullptr) {
        err = "null out_tls";
        return false;
    }
    *out_tls = nullptr;
    TLSConfiguration tls = TLSConfiguration_create();
    if (tls == nullptr) {
        err = "TLSConfiguration_create failed";
        return false;
    }
    TLSConfiguration_setEventHandler(tls, tls_event, events);
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
    if (!load_pem(tls, cfg, err)) {
        const bool identity_missing = cfg.own_key.empty() || cfg.own_cert.empty() ||
            err.rfind("tls_own_key load failed:", 0) == 0 ||
            err.rfind("tls_own_cert load failed:", 0) == 0;
        const bool policy_event_already_emitted =
            err == "IEC 62351-3 TLS identity, revocation or policy validation failed";
        const bool crl_problem = cfg.crl_path.empty() || err.rfind("tls_crl", 0) == 0;
        if (identity_missing) {
            tls_event(events, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_UNAVAILABLE,
                      "alarm: endpoint certificate unavailable", nullptr);
        } else if (crl_problem) {
            tls_event(events, TLS_SEC_EVT_INCIDENT, CCLI_TLS_EVENT_CODE_CRL,
                      cfg.crl_path.empty() ?
                          "alarm: TLS requires a configured CRL; peer revocation cannot be checked" :
                          "alarm: configured TLS CRL could not be loaded", nullptr);
        } else if (!policy_event_already_emitted) {
            tls_event(events, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_NOT_CONFIGURED,
                      err.c_str(), nullptr);
        }
        TLSConfiguration_destroy(tls);
        return false;
    }
    *out_tls = tls;
    return true;
#else
    (void)cfg;
    (void)out_tls;
    (void)events;
    err = "104 TLS not compiled (CONFIG_CS104_SUPPORT_TLS / mbedtls)";
    return false;
#endif
}

void iec104_destroy_tls_config(void* tls) {
#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)
    if (tls != nullptr) {
        TLSConfiguration_destroy(static_cast<TLSConfiguration>(tls));
    }
#else
    (void)tls;
#endif
}

bool iec104_tls_available() {
#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)
    return true;
#else
    return false;
#endif
}

}  // namespace cci::adapters
