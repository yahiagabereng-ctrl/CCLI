#include "mms_acse_auth.hpp"

#include <cstdio>
#include <cstring>

namespace cci::adapters {
namespace {

bool der_eq(const std::vector<uint8_t>& a, const uint8_t* buf, int len) {
    if (buf == nullptr || len <= 0) {
        return false;
    }
    if (a.size() != static_cast<size_t>(len)) {
        return false;
    }
    return std::memcmp(a.data(), buf, static_cast<size_t>(len)) == 0;
}

}  // namespace

#if defined(CCLI_HAVE_LIBIEC61850)

MmsAcseMechanism mms_acse_mechanism_from_libiec61850(int libiec61850_mechanism) {
    switch (libiec61850_mechanism) {
    case 1:
        return MmsAcseMechanism::Password;
    case 2:
        return MmsAcseMechanism::Certificate;
    case 3:
        return MmsAcseMechanism::Tls;
    default:
        return MmsAcseMechanism::None;
    }
}

#endif

bool mms_acse_mechanism_supported(MmsAcseMechanism mechanism) {
    return mechanism == MmsAcseMechanism::Tls || mechanism == MmsAcseMechanism::Certificate;
}

const char* mms_acse_mechanism_label(MmsAcseMechanism mechanism) {
    switch (mechanism) {
    case MmsAcseMechanism::Tls:
        return "TLS";
    case MmsAcseMechanism::Certificate:
        return "CERTIFICATE";
    case MmsAcseMechanism::Password:
        return "PASSWORD";
    case MmsAcseMechanism::None:
    default:
        return "NONE";
    }
}

MmsAcseAuthResult evaluate_acse_authentication(const MmsAcseCredentialStore& creds,
                                               MmsAcseMechanism mechanism,
                                               const uint8_t* cert_buf, int cert_len) {
    if (!mms_acse_mechanism_supported(mechanism)) {
        return MmsAcseAuthResult::RejectMechanism;
    }
    if (cert_buf == nullptr || cert_len <= 0) {
        return MmsAcseAuthResult::RejectEmptyCert;
    }

    if (creds.rbac_enabled) {
        if (der_eq(creds.dso_der, cert_buf, cert_len)) {
            return MmsAcseAuthResult::AcceptDso;
        }
        if (der_eq(creds.viewer_der, cert_buf, cert_len)) {
            return MmsAcseAuthResult::AcceptViewer;
        }
        return MmsAcseAuthResult::RejectUnknownCert;
    }

    return MmsAcseAuthResult::AcceptGeneric;
}

}  // namespace cci::adapters
