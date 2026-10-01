#include "mms_aare_auth.hpp"
#include "ccli_acse_bridge.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <mutex>
#include <vector>

#if defined(CCLI_HAVE_LIBIEC61850) && defined(CONFIG_MMS_SUPPORT_TLS)

extern "C" {
#include "hal_time.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
}

namespace cci::adapters {
namespace {

std::mutex g_mu;
std::vector<uint8_t> g_server_cert_der;
mbedtls_pk_context g_server_key;
mbedtls_entropy_context g_entropy;
mbedtls_ctr_drbg_context g_drbg;
bool g_ready{false};

/** IEC 62351-4 A-profile / TSP: GeneralizedTime YYYYMMDDHHMMSS. (no ms, no Z). */
int format_a_profile_generalized_time(uint64_t ms_time, uint8_t* gt, int gt_max) {
    if (gt_max < 16) {
        return -1;
    }
    const time_t unix_time = static_cast<time_t>(ms_time / 1000ULL);
    struct tm tm_time {};
#if defined(_WIN32)
    gmtime_s(&tm_time, &unix_time);
#else
    gmtime_r(&unix_time, &tm_time);
#endif
    const int n = std::snprintf(reinterpret_cast<char*>(gt), static_cast<size_t>(gt_max),
                              "%04d%02d%02d%02d%02d%02d.", tm_time.tm_year + 1900,
                              tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour,
                              tm_time.tm_min, tm_time.tm_sec);
    if (n != 15) {
        return -1;
    }
    return n;
}

bool sign_time_field_pkcs1_sha256(const uint8_t* time_field, int time_field_len,
                                  uint8_t* signature, size_t* sig_len) {
    /* §11.2.2: sha256WithRSA over DER-encoded [1] GeneralizedTime field (0x81 TLV).
     * Mbed TLS 3.x pk_sign/pk_verify take the message digest, not the raw TLV. */
    uint8_t hash[32];
    if (mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), time_field,
                   static_cast<size_t>(time_field_len),
                   hash) != 0) {
        return false;
    }
    if (mbedtls_pk_sign(&g_server_key, MBEDTLS_MD_SHA256, hash, sizeof(hash), signature,
                        512, sig_len, mbedtls_ctr_drbg_random, &g_drbg) != 0) {
        std::fprintf(stderr, "mms: AARE auth pk_sign failed\n");
        return false;
    }

    /* Self-verify signature (RSA verify via loaded server key).
     * Do not parse g_server_cert_der here: G.6.2 id-at-objectIdentifier (2.5.4.106) with
     * OBJECT IDENTIFIER value is valid for TSP/OpenSSL but often rejected by mbedTLS
     * x509_crt_parse on the DUT — that must not block AARE auth emission. */
    const int verify_ret =
        mbedtls_pk_verify(&g_server_key, MBEDTLS_MD_SHA256, hash, sizeof(hash), signature,
                          *sig_len);
    if (verify_ret != 0) {
        std::fprintf(stderr, "mms: AARE auth sign self-verify failed ret=%d\n", verify_ret);
        return false;
    }
    return true;
}

int b64_val(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    }
    if (c == '+') {
        return 62;
    }
    if (c == '/') {
        return 63;
    }
    return -1;
}

bool load_pem_der(const std::string& path, std::vector<uint8_t>& out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    std::string pem((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const auto begin = pem.find("-----BEGIN CERTIFICATE-----");
    const auto end = pem.find("-----END CERTIFICATE-----");
    if (begin == std::string::npos || end == std::string::npos || end <= begin) {
        return false;
    }
    std::string b64;
    for (size_t i = begin + 27; i < end; ++i) {
        const char c = pem[i];
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
            continue;
        }
        b64.push_back(c);
    }
    out.clear();
    out.reserve(b64.size() * 3 / 4);
    int val = 0;
    int valb = -8;
    for (char c : b64) {
        const int d = b64_val(c);
        if (d < 0) {
            continue;
        }
        val = (val << 6) + d;
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return !out.empty();
}

int append_tl(uint8_t* buf, int pos, uint8_t tag, const uint8_t* payload, int payload_len) {
    buf[pos++] = tag;
    if (payload_len < 128) {
        buf[pos++] = static_cast<uint8_t>(payload_len);
    } else if (payload_len < 256) {
        buf[pos++] = 0x81;
        buf[pos++] = static_cast<uint8_t>(payload_len);
    } else {
        buf[pos++] = 0x82;
        buf[pos++] = static_cast<uint8_t>((payload_len >> 8) & 0xFF);
        buf[pos++] = static_cast<uint8_t>(payload_len & 0xFF);
    }
    std::memcpy(buf + pos, payload, static_cast<size_t>(payload_len));
    return pos + payload_len;
}

int build_mms_auth_value(uint8_t* out, int max_out) {
    std::lock_guard<std::mutex> lock(g_mu);
    if (!g_ready || g_server_cert_der.empty()) {
        return -1;
    }

    uint8_t gt[32];
    const int gt_len = format_a_profile_generalized_time(Hal_getTimeInMs(), gt, sizeof(gt));
    if (gt_len <= 0) {
        return -1;
    }

    uint8_t time_field[32];
    const int time_field_len = append_tl(time_field, 0, 0x81, gt, gt_len);

    uint8_t signature[512];
    size_t sig_len = 0;
    if (!sign_time_field_pkcs1_sha256(time_field, time_field_len, signature, &sig_len)) {
        std::fprintf(stderr, "mms: AARE auth sign/self-verify failed\n");
        return -1;
    }

    uint8_t seq_body[8192];
    int seq_len = 0;
    seq_len = append_tl(seq_body, seq_len, 0x80, g_server_cert_der.data(),
                        static_cast<int>(g_server_cert_der.size()));
    seq_len = append_tl(seq_body, seq_len, 0x81, gt, gt_len);
    seq_len = append_tl(seq_body, seq_len, 0x82, signature, static_cast<int>(sig_len));

    if (seq_len + 4 > max_out) {
        return -1;
    }
    /* MMS-Authentication-value certificate-based [0] IMPLICIT SEQUENCE -> 0xa0 { [0] cert, [1] time, [2] sig }.
     * Caller (acse.c) wraps this in EXTERNAL + responding-authentication-value [10]. */
    return append_tl(out, 0, 0xa0, seq_body, seq_len);
}

extern "C" int ccli_aare_auth_build_tramp(uint8_t* out, int max_out) {
    return build_mms_auth_value(out, max_out);
}

}  // namespace

bool mms_aare_auth_configure(const std::string& cert_pem_path, const std::string& key_pem_path) {
    std::lock_guard<std::mutex> lock(g_mu);
    g_ready = false;
    g_server_cert_der.clear();
    mbedtls_pk_free(&g_server_key);
    mbedtls_pk_init(&g_server_key);
    mbedtls_entropy_free(&g_entropy);
    mbedtls_ctr_drbg_free(&g_drbg);
    mbedtls_entropy_init(&g_entropy);
    mbedtls_ctr_drbg_init(&g_drbg);

    if (!load_pem_der(cert_pem_path, g_server_cert_der)) {
        std::fprintf(stderr, "mms: AARE auth — failed to load server cert DER %s\n",
                     cert_pem_path.c_str());
        return false;
    }
    const int seed_ret =
        mbedtls_ctr_drbg_seed(&g_drbg, mbedtls_entropy_func, &g_entropy, nullptr, 0);
    if (seed_ret != 0) {
        std::fprintf(stderr, "mms: AARE auth — DRBG seed failed ret=%d\n", seed_ret);
        g_server_cert_der.clear();
        mbedtls_pk_free(&g_server_key);
        mbedtls_entropy_free(&g_entropy);
        mbedtls_ctr_drbg_free(&g_drbg);
        return false;
    }
    const int key_ret = mbedtls_pk_parse_keyfile(&g_server_key, key_pem_path.c_str(), nullptr,
                                                 mbedtls_ctr_drbg_random, &g_drbg);
    if (key_ret != 0) {
        std::fprintf(stderr, "mms: AARE auth — failed to load server key %s (ret=%d)\n",
                     key_pem_path.c_str(), key_ret);
        g_server_cert_der.clear();
        mbedtls_pk_free(&g_server_key);
        mbedtls_entropy_free(&g_entropy);
        mbedtls_ctr_drbg_free(&g_drbg);
        return false;
    }
    g_ready = true;
    std::fprintf(stderr, "mms: AARE 62351-4 responder auth ready (cert=%zu octets)\n",
                 g_server_cert_der.size());
    return true;
}

void mms_aare_auth_register_acse_bridge() {
    AcseConnection_setCcliRespondingApTitle(kMmsRespondingApTitle, kMmsRespondingAeQualifier);
    AcseConnection_setCcliAareAuthBuilder(ccli_aare_auth_build_tramp);
    std::fprintf(stderr, "mms: AARE responding titles AP=%s AE=%d\n",
                 kMmsRespondingApTitle, kMmsRespondingAeQualifier);
}

#endif

}  // namespace cci::adapters
