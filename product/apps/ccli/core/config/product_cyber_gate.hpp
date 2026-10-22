#pragma once

#include "config/ccli_config.hpp"

namespace cci::core {

/** Frozen product cyber (O.15 / T.3.3.4 / 62351-3): MMS TLS :3782, 104 TLS :2404. */
inline bool product_cyber_config_ok(const CcliConfig& cfg, std::string* reason = nullptr) {
    if (cfg.mms.enabled && (!cfg.mms.tls.enabled || cfg.mms.tcp_port == 102)) {
        if (reason) {
            *reason = "MMS requires tls_enabled and tcp_port!=102 (use product_tr400.yaml / *_5ln.yaml)";
        }
        return false;
    }
    if (cfg.iec104.enabled && !cfg.iec104.tls.enabled) {
        if (reason) {
            *reason = "IEC 104 requires tls_enabled on Eth_B :2404";
        }
        return false;
    }
    return true;
}

} // namespace cci::core
