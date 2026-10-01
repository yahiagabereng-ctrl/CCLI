#include "iec104_tls.hpp"

#include <iostream>
#include <string>

#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)
extern "C" {
#include "tls_config.h"
}
#endif

namespace cci::adapters {

#if defined(CCLI_HAVE_LIB60870) && defined(CONFIG_CS104_SUPPORT_TLS)

namespace {

bool load_pem(TLSConfiguration tls, const core::Iec104TlsConfig& cfg, std::string& err) {
    if (cfg.own_key.empty() || cfg.own_cert.empty()) {
        err = "tls_own_key and tls_own_cert required (62351-3)";
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
    } else {
        TLSConfiguration_setChainValidation(tls, false);
    }
    if (!cfg.client_cert.empty()) {
        if (!TLSConfiguration_addAllowedCertificateFromFile(tls, cfg.client_cert.c_str())) {
            err = "tls_client_cert allow-list failed: " + cfg.client_cert;
            return false;
        }
        TLSConfiguration_setAllowOnlyKnownCertificates(tls, true);
    } else {
        TLSConfiguration_setAllowOnlyKnownCertificates(tls, false);
    }
    return true;
}

}  // namespace

#endif

bool iec104_create_tls_config(const core::Iec104TlsConfig& cfg, void** out_tls,
                              std::string& err) {
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
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    if (!load_pem(tls, cfg, err)) {
        TLSConfiguration_destroy(tls);
        return false;
    }
    *out_tls = tls;
    return true;
#else
    (void)cfg;
    (void)out_tls;
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
