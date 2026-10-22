#pragma once

#include <cstdint>
#include <vector>

namespace cci::adapters {

/** Mirrors libiec61850 AcseAuthenticationMechanism (iso_connection_parameters.h). */
enum class MmsAcseMechanism : int {
    None = 0,
    Password = 1,
    Certificate = 2,
    Tls = 3,
};

/** Lab allowlist DER blobs loaded at MMS server start (P3-08 RBAC). */
struct MmsAcseCredentialStore {
    /** G.6.2 MMS Security profile (client.pem). */
    std::vector<uint8_t> dso_der;
    /** Transport TLS profile (client_tls.pem) — TSP sometimes embeds this in AARQ. */
    std::vector<uint8_t> dso_tls_der;
    std::vector<uint8_t> viewer_der;
    bool rbac_enabled{false};
};

enum class MmsAcseAuthResult {
    AcceptDso,
    AcceptViewer,
    AcceptGeneric,
    RejectMechanism,
    RejectEmptyCert,
    RejectUnknownCert,
};

/** P3-07: accept TLS-layer or ACSE-layer (62351-4 lab) client certificates. */
MmsAcseAuthResult evaluate_acse_authentication(const MmsAcseCredentialStore& creds,
                                               MmsAcseMechanism mechanism,
                                               const uint8_t* cert_buf, int cert_len);

const char* mms_acse_mechanism_label(MmsAcseMechanism mechanism);

bool mms_acse_mechanism_supported(MmsAcseMechanism mechanism);

#if defined(CCLI_HAVE_LIBIEC61850)
MmsAcseMechanism mms_acse_mechanism_from_libiec61850(int libiec61850_mechanism);
#endif

}  // namespace cci::adapters
