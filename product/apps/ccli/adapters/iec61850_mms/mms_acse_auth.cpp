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

int ber_tlv_size(const uint8_t* buf, int avail) {
    if (avail < 2) {
        return -1;
    }
    int pos = 1;
    int content_len = 0;
    const uint8_t lb = buf[1];
    if ((lb & 0x80) == 0) {
        content_len = lb;
        pos = 2;
    }
    else {
        const int n = lb & 0x7F;
        /* Length octets follow the long-form length indicator (buf[1]), not the tag. */
        if (n == 0 || n > 3 || 2 + n > avail) {
            return -1;
        }
        for (int i = 0; i < n; ++i) {
            content_len = (content_len << 8) | buf[2 + i];
        }
        pos = 2 + n;
    }
    const int total = pos + content_len;
    if (total > avail || total <= 0) {
        return -1;
    }
    return total;
}

bool matches_role(const std::vector<uint8_t>& role_der, const uint8_t* buf, int len) {
    if (role_der.empty() || buf == nullptr || len <= 0) {
        return false;
    }
    if (der_eq(role_der, buf, len)) {
        return true;
    }
    /* TSP often passes auth blobs with trailing octets after the X.509 SEQUENCE. */
    for (int i = 0; i + 4 <= len; ++i) {
        if (buf[i] != 0x30) {
            continue;
        }
        const int tlv = ber_tlv_size(buf + i, len - i);
        if (tlv > 0 && der_eq(role_der, buf + i, tlv)) {
            return true;
        }
    }
    return false;
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
        if (matches_role(creds.dso_der, cert_buf, cert_len) ||
            matches_role(creds.dso_tls_der, cert_buf, cert_len)) {
            return MmsAcseAuthResult::AcceptDso;
        }
        if (matches_role(creds.viewer_der, cert_buf, cert_len)) {
            return MmsAcseAuthResult::AcceptViewer;
        }
        return MmsAcseAuthResult::RejectUnknownCert;
    }

    return MmsAcseAuthResult::AcceptGeneric;
}

}  // namespace cci::adapters
