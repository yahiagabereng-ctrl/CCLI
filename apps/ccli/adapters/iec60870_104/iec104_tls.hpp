#pragma once

#include "config/ccli_config.hpp"

#include <string>

namespace cci::adapters {

struct Iec104TlsEvents {
    void (*handler)(void*, int, int, const char*){nullptr};
    void* parameter{nullptr};
};

/** P4-03 / IEC 62351-3 — TLS for CS104 on Eth_B. */
bool iec104_create_tls_config(const core::Iec104TlsConfig& cfg, void** out_tls,
                              std::string& err,
                              Iec104TlsEvents* events = nullptr);
void iec104_destroy_tls_config(void* tls);
bool iec104_tls_available();

}  // namespace cci::adapters
