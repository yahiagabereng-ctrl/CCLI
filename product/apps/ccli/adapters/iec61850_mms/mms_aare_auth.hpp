#pragma once

#include <string>

namespace cci::adapters {

/** 62351-4 §10.5.3 — IED responding AP/AE in AARE (address binding with tls_own_cert). */
inline constexpr const char* kMmsRespondingApTitle = "1.1.1.999.1";
inline constexpr int kMmsRespondingAeQualifier = 12;

/** Configure server cert/key for 62351-4 AARE responder authentication (P3-07). */
bool mms_aare_auth_configure(const std::string& cert_pem_path, const std::string& key_pem_path);

/** Register AARE auth builder with libiec61850 ACSE layer. */
void mms_aare_auth_register_acse_bridge();

}  // namespace cci::adapters
