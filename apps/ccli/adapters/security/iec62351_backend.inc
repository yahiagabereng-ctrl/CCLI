/* Included by the reviewed generated mbedTLS 3.6 HAL, after its types and
 * raiseSecurityEvent. No application/E2E cryptographic provider is replaced. */
#include "iec62351_tls.h"
#include "mbedtls/version.h"
#if MBEDTLS_VERSION_NUMBER != 0x03060000
#error "IEC 62351-3 ClientHello policy is reviewed only for pinned mbedTLS 3.6.0"
#endif
#include "mbedtls/ssl_ciphersuites.h"
#include "mbedtls/oid.h"
#include <stdio.h>
#if !defined(MBEDTLS_ECDSA_C) || \
    !defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) || \
    !defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
#error "CCLI certificate validation requires ECDSA P-256 and Brainpool P-256"
#endif
#if !defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#error "Safe TLS resumption requires the cached peer certificate for revalidation"
#endif
/* The transport is pinned to mbedTLS 3.6; inspect its parsed TLS 1.2
 * signature list in the server certificate callback, before any signing. */
#include "ssl_misc.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

static uint64_t ccli_monotonic_ms(void)
{
#ifdef _WIN32
    return GetTickCount64();
#else
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) abort();
    return (uint64_t)now.tv_sec * 1000 + now.tv_nsec / 1000000;
#endif
}

#define CCLI_TLS_CRL_WARNING CCLI_TLS_EVENT_CODE_CRL
#define CCLI_TLS_POLICY_ALARM CCLI_TLS_EVENT_CODE_POLICY
#define CCLI_TLS_DEPRECATED_HASH_WARNING CCLI_TLS_EVENT_CODE_DEPRECATED_HASH
#define CCLI_TLS_REVOKED_SESSION_TERMINATED CCLI_TLS_EVENT_CODE_REVOKED_SESSION_TERMINATED
#define CCLI_TLS_VERSION_CHANGED CCLI_TLS_EVENT_CODE_VERSION_CHANGED
#define CCLI_TLS_MAX_CERT_OCTETS 8192U
#define CCLI_TLS_MAX_RESUMPTION_SECONDS 7200
#define CCLI_TLS_HASH_SHA384 0x01U
#define CCLI_TLS_HASH_SHA512 0x02U

static bool ccli_certificate_chain_within_limit(const mbedtls_x509_crt* cert)
{
    for (; cert && cert->version; cert = cert->next)
        if (cert->raw.len > CCLI_TLS_MAX_CERT_OCTETS) return false;
    return true;
}

/* IEC 62351-3 Table 1 mandatory TLS 1.2 suites. Only RSA-NULL-SHA256
 * is conditional (c1); it is not enabled here. Prefer forward-secret GCM. */
static const int ccli_cipher_suites[] = {
    MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256
};

static void ccli_init_certificate_policy(TLSConfiguration cfg)
{
    uint32_t hashes = MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256);
    uint32_t curves = MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP256R1) |
                      MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_BP256R1);
    size_t index = 0;
    cfg->ccliCertProfile = (mbedtls_x509_crt_profile) {
        hashes,
        MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_RSA) |
        MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_RSASSA_PSS) |
        MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECDSA) |
        MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECKEY),
        curves, 2048
    };
#if defined(MBEDTLS_SHA384_C)
    if (cfg->ccliOptionalHashMask & CCLI_TLS_HASH_SHA384) {
        cfg->ccliCertProfile.allowed_mds |= MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384);
        cfg->ccliSignatureHashes[index++] = MBEDTLS_MD_SHA384;
    }
#endif
#if defined(MBEDTLS_SHA512_C)
    if (cfg->ccliOptionalHashMask & CCLI_TLS_HASH_SHA512) {
        cfg->ccliCertProfile.allowed_mds |= MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA512);
        cfg->ccliSignatureHashes[index++] = MBEDTLS_MD_SHA512;
    }
#endif
    /* SHA-256 stays first and mandatory; optional hashes follow only by policy. */
    for (size_t i = index; i > 0; --i)
        cfg->ccliSignatureHashes[i] = cfg->ccliSignatureHashes[i - 1];
    cfg->ccliSignatureHashes[0] = MBEDTLS_MD_SHA256;
    cfg->ccliSignatureHashes[index + 1] = MBEDTLS_MD_NONE;
}

static int ccli_client_hello_policy(mbedtls_ssl_context* ssl)
{
    TLSSocket socket = (TLSSocket)mbedtls_ssl_get_user_data_p(ssl);
    if (!socket || !ssl->MBEDTLS_PRIVATE(handshake))
        return MBEDTLS_ERR_SSL_BAD_CONFIG;

    /* An explicit signature_algorithms extension has already been intersected
     * with our configured list. In 3.6, an absent extension instead injects a
     * SHA-1 fallback outside that intersection. This callback runs after the
     * ClientHello parser on initial handshakes AND renegotiations, before the
     * server selects a suite or sends ServerHello/ServerKeyExchange. */
    const uint16_t* algorithms = ssl->MBEDTLS_PRIVATE(handshake)->received_sig_algs;
    for (size_t i = 0; i < MBEDTLS_RECEIVED_SIG_ALGS_SIZE; ++i) {
        if (algorithms[i] == MBEDTLS_TLS_SIG_NONE)
            return i ? 0 : MBEDTLS_ERR_SSL_BAD_CONFIG;
        if ((algorithms[i] >> 8) == MBEDTLS_SSL_HASH_SHA1) {
            raiseSecurityEvent(socket->tlsConfig, TLS_SEC_EVT_WARNING,
                CCLI_TLS_DEPRECATED_HASH_WARNING,
                "warning: deprecated hash algorithm used", socket);
            raiseSecurityEvent(socket->tlsConfig, TLS_SEC_EVT_INCIDENT,
                TLS_EVENT_CODE_ALM_ALGO_NOT_SUPPORTED,
                "alarm: signature_algorithms extension unavailable; SHA-1 fallback prohibited", socket);
            (void)mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
            return MBEDTLS_ERR_SSL_HANDSHAKE_FAILURE;
        }
    }
    return MBEDTLS_ERR_SSL_BAD_CONFIG;
}

static bool ccli_check_certificate_crypto(TLSConfiguration cfg,
        const mbedtls_x509_crt* cert, TLSSocket socket)
{
    if (cert->MBEDTLS_PRIVATE(sig_md) == MBEDTLS_MD_SHA1)
        raiseSecurityEvent(cfg, TLS_SEC_EVT_WARNING, CCLI_TLS_DEPRECATED_HASH_WARNING,
            "warning: deprecated hash algorithm used", socket);

    if (mbedtls_pk_can_do(&cert->pk, MBEDTLS_PK_RSA) ||
        mbedtls_pk_can_do(&cert->pk, MBEDTLS_PK_RSASSA_PSS)) {
        const size_t bits = mbedtls_pk_get_bitlen(&cert->pk);
        if (bits < 1024) {
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
                "alarm: insufficient key length", socket);
            return false;
        }
        if (bits < 2048) {
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
                "alarm: insufficient RSA key length (<2048 bit)", socket);
            return false;
        }
    } else if ((mbedtls_pk_can_do(&cert->pk, MBEDTLS_PK_ECDSA) ||
                mbedtls_pk_can_do(&cert->pk, MBEDTLS_PK_ECKEY)) &&
               mbedtls_pk_get_bitlen(&cert->pk) < 256) {
        raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
            "alarm: insufficient key length", socket);
        return false;
    }
    return true;
}

static bool ccli_check_certificate_chain_crypto(TLSConfiguration cfg,
        const mbedtls_x509_crt* cert)
{
    for (; cert && cert->version; cert = cert->next)
        if (!ccli_check_certificate_crypto(cfg, cert, NULL)) return false;
    return true;
}

bool CCLI_TLS_setOptionalHashes(TLSConfiguration cfg, bool sha384, bool sha512)
{
    if (!cfg || cfg->setupComplete) return false;
    cfg->ccliOptionalHashMask = 0;
#if defined(MBEDTLS_SHA384_C)
    if (sha384) cfg->ccliOptionalHashMask |= CCLI_TLS_HASH_SHA384;
#else
    if (sha384) return false;
#endif
#if defined(MBEDTLS_SHA512_C)
    if (sha512) cfg->ccliOptionalHashMask |= CCLI_TLS_HASH_SHA512;
#else
    if (sha512) return false;
#endif
    return true;
}

static bool ccli_crl_valid(TLSConfiguration cfg, mbedtls_x509_crl* list,
                           bool allow_expired)
{
    for (mbedtls_x509_crl* crl = list; crl && crl->version; crl = crl->next) {
        if (mbedtls_x509_time_is_future(&crl->this_update) ||
            crl->next_update.year == 0 ||
            (!allow_expired && mbedtls_x509_time_is_past(&crl->next_update)))
            return false;
        const mbedtls_md_info_t* md = mbedtls_md_info_from_type(crl->MBEDTLS_PRIVATE(sig_md));
        if (!md || !(cfg->ccliCertProfile.allowed_mds &
                      MBEDTLS_X509_ID_FLAG(crl->MBEDTLS_PRIVATE(sig_md)))) return false;
        unsigned char hash[MBEDTLS_MD_MAX_SIZE];
        if (mbedtls_md(md, crl->tbs.p, crl->tbs.len, hash) != 0) return false;
        bool verified = false;
        for (mbedtls_x509_crt* ca = &cfg->cacerts; ca && ca->version; ca = ca->next) {
            if (ca->subject_raw.len != crl->issuer_raw.len ||
                memcmp(ca->subject_raw.p, crl->issuer_raw.p, crl->issuer_raw.len)) continue;
            if (mbedtls_x509_crt_check_key_usage(ca, MBEDTLS_X509_KU_CRL_SIGN) != 0) continue;
            if (mbedtls_pk_verify_ext(crl->MBEDTLS_PRIVATE(sig_pk), crl->MBEDTLS_PRIVATE(sig_opts),
                &ca->pk, crl->MBEDTLS_PRIVATE(sig_md), hash, mbedtls_md_get_size(md),
                crl->MBEDTLS_PRIVATE(sig).p, crl->MBEDTLS_PRIVATE(sig).len) == 0) {
                verified = true;
                break;
            }
        }
        if (!verified) return false;
    }
    return list->version != 0;
}

static bool ccli_crl_equal(const mbedtls_x509_crl* a, const mbedtls_x509_crl* b)
{
    while (a && b) {
        if (a->raw.len != b->raw.len || memcmp(a->raw.p, b->raw.p, a->raw.len)) return false;
        a = a->next; b = b->next;
    }
    return a == b;
}

static bool ccli_cached_peer_is_allowed(TLSConfiguration cfg,
                                        const mbedtls_x509_crt* peer)
{
    if (!cfg->allowOnlyKnownCertificates) return true;
    if (!peer) return false;
    LinkedList cert_list = LinkedList_getNext(cfg->allowedCertificates);
    while (cert_list) {
        mbedtls_x509_crt* allowed = (mbedtls_x509_crt*)LinkedList_getData(cert_list);
        if (allowed && compareCertificates(allowed, (mbedtls_x509_crt*)peer)) return true;
        cert_list = LinkedList_getNext(cert_list);
    }
    return false;
}

/* A server-side cache hit is only a hint. Re-validate the serialized peer
 * certificate against the current CRL file and current TLS trust/pin policy
 * before allowing Mbed TLS to resume the session. Invalid or stale CRLs turn
 * the entry into a cache miss, so the peer must complete a full handshake. */
static bool ccli_cached_session_valid(TLSConfiguration cfg,
                                      const mbedtls_ssl_session* session)
{
    if (!cfg || !session || !session->MBEDTLS_PRIVATE(peer_cert) ||
        !cfg->ccliCrlFile || !cfg->ccliCrlFile[0]) return false;

    const mbedtls_x509_crt* peer = session->MBEDTLS_PRIVATE(peer_cert);
    const bool pinned_peer = ccli_cached_peer_is_allowed(cfg, peer);
    if (!ccli_certificate_chain_within_limit(peer) ||
        !ccli_check_certificate_chain_crypto(cfg, peer) ||
        !pinned_peer) return false;

    mbedtls_x509_crl current_crl;
    mbedtls_x509_crl_init(&current_crl);
    const bool loaded = mbedtls_x509_crl_parse_file(&current_crl, cfg->ccliCrlFile) == 0;
    if (!loaded || !ccli_crl_valid(cfg, &current_crl, false)) {
        mbedtls_x509_crl_free(&current_crl);
        return false;
    }

    uint32_t flags = 0;
    const int verify_result = mbedtls_x509_crt_verify_with_profile(
        (mbedtls_x509_crt*)peer, &cfg->cacerts, &current_crl,
        &cfg->ccliCertProfile, NULL, &flags, NULL, NULL);
    mbedtls_x509_crl_free(&current_crl);

    /* A configured individual-certificate pin may replace only the trust-path
     * decision. It never overrides expiry, revocation, bad signatures, or any
     * other validation result. */
    const bool trusted_path = verify_result == 0 && flags == 0;
    const bool pinned_path = cfg->allowOnlyKnownCertificates && pinned_peer &&
        verify_result == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED &&
        flags == MBEDTLS_X509_BADCERT_NOT_TRUSTED;
    if (!trusted_path && !pinned_path) {
        if (flags & MBEDTLS_X509_BADCERT_REVOKED)
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT,
                TLS_EVENT_CODE_ALM_CERT_REVOKED,
                "alarm: revoked peer certificate rejected during TLS session resumption", NULL);
        return false;
    }
    return true;
}

static int ccli_tls_session_cache_get(void* data,
        const unsigned char* session_id, size_t session_id_len,
        mbedtls_ssl_session* session)
{
    TLSConfiguration cfg = (TLSConfiguration)data;
    if (!cfg || !cfg->ccliSessionCacheLock || session_id_len == 0 ||
        session_id_len > sizeof(cfg->ccliSessionCacheIds[0]))
        return MBEDTLS_ERR_SSL_CACHE_ENTRY_NOT_FOUND;

    Semaphore_wait(cfg->ccliSessionCacheLock);
    const int result = mbedtls_ssl_cache_get(&cfg->cache, session_id,
                                              session_id_len, session);
    if (result != 0) {
        Semaphore_post(cfg->ccliSessionCacheLock);
        return result;
    }

    size_t slot = MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES;
    for (size_t i = 0; i < MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES; ++i) {
        if (cfg->ccliSessionCacheIdLengths[i] == session_id_len &&
            memcmp(cfg->ccliSessionCacheIds[i], session_id, session_id_len) == 0) {
            slot = i;
            break;
        }
    }
    const uint64_t now = ccli_monotonic_ms();
    const int interval_seconds = cfg->sessionResumptionInterval;
    const uint64_t maximum_age = (uint64_t)(interval_seconds < 1 ? 1 :
        (interval_seconds > CCLI_TLS_MAX_RESUMPTION_SECONDS ?
            CCLI_TLS_MAX_RESUMPTION_SECONDS : interval_seconds)) * 1000;
    const bool age_valid = slot < MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES &&
        now >= cfg->ccliSessionCacheCreated[slot] &&
        now - cfg->ccliSessionCacheCreated[slot] < maximum_age;
    if (!age_valid) {
        (void)mbedtls_ssl_cache_remove(&cfg->cache, session_id, session_id_len);
        if (slot < MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES) {
            cfg->ccliSessionCacheIdLengths[slot] = 0;
            cfg->ccliSessionCacheCreated[slot] = 0;
            memset(cfg->ccliSessionCacheIds[slot], 0,
                   sizeof(cfg->ccliSessionCacheIds[slot]));
        }
        Semaphore_post(cfg->ccliSessionCacheLock);
        return MBEDTLS_ERR_SSL_CACHE_ENTRY_NOT_FOUND;
    }
    Semaphore_post(cfg->ccliSessionCacheLock);

    if (ccli_cached_session_valid(cfg, session)) return 0;

    /* Discard an unusable entry; Mbed TLS will continue with a full handshake. */
    Semaphore_wait(cfg->ccliSessionCacheLock);
    (void)mbedtls_ssl_cache_remove(&cfg->cache, session_id, session_id_len);
    if (slot < MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES) {
        cfg->ccliSessionCacheIdLengths[slot] = 0;
        cfg->ccliSessionCacheCreated[slot] = 0;
        memset(cfg->ccliSessionCacheIds[slot], 0,
               sizeof(cfg->ccliSessionCacheIds[slot]));
    }
    Semaphore_post(cfg->ccliSessionCacheLock);
    return MBEDTLS_ERR_SSL_CACHE_ENTRY_NOT_FOUND;
}

static int ccli_tls_session_cache_set(void* data,
        const unsigned char* session_id, size_t session_id_len,
        const mbedtls_ssl_session* session)
{
    TLSConfiguration cfg = (TLSConfiguration)data;
    if (!cfg || !session || !session->MBEDTLS_PRIVATE(peer_cert) ||
        session->MBEDTLS_PRIVATE(verify_result) != 0 || !cfg->ccliSessionCacheLock ||
        session_id_len == 0 || session_id_len > sizeof(cfg->ccliSessionCacheIds[0]))
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;

    Semaphore_wait(cfg->ccliSessionCacheLock);
    int result = mbedtls_ssl_cache_set(&cfg->cache, session_id,
                                        session_id_len, session);
    if (result == 0) {
        size_t slot = MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES;
        for (size_t i = 0; i < MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES; ++i) {
            if (cfg->ccliSessionCacheIdLengths[i] == session_id_len &&
                memcmp(cfg->ccliSessionCacheIds[i], session_id, session_id_len) == 0) {
                slot = i;
                break;
            }
        }
        if (slot == MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES) {
            slot = cfg->ccliSessionCacheNext++ % MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES;
        }
        memset(cfg->ccliSessionCacheIds[slot], 0,
               sizeof(cfg->ccliSessionCacheIds[slot]));
        memcpy(cfg->ccliSessionCacheIds[slot], session_id, session_id_len);
        cfg->ccliSessionCacheIdLengths[slot] = session_id_len;
        cfg->ccliSessionCacheCreated[slot] = ccli_monotonic_ms();
    }
    Semaphore_post(cfg->ccliSessionCacheLock);
    return result;
}

/* Run in the connection's own I/O thread, never mutate the shared CA/CRL or
 * session cache while other connections use them. Failed refresh retains the
 * last valid CRL and warns; it does not itself kill an established session. */
static void ccli_refresh_crl(TLSSocket self, bool initial)
{
    TLSConfiguration cfg = self->tlsConfig;
    if (!cfg->ccliCrlFile) return;
    const uint64_t now = ccli_monotonic_ms();
    bool expired = false;
    for (mbedtls_x509_crl* crl = &self->ccliCrl; crl && crl->version; crl = crl->next)
        if (mbedtls_x509_time_is_past(&crl->next_update)) expired = true;
    if (!initial && now - self->ccliCrlChecked < (uint64_t)cfg->ccliRevocationSeconds * 1000 &&
        (!expired || self->ccliCrlExpiryWarned)) return;
    self->ccliCrlChecked = now;
    if (expired && !self->ccliCrlExpiryWarned)
        raiseSecurityEvent(cfg, TLS_SEC_EVT_WARNING, CCLI_TLS_CRL_WARNING,
            "warning: CRL expired; retaining established session and known revocations", self);
    self->ccliCrlExpiryWarned = expired;
    mbedtls_x509_crl next;
    mbedtls_x509_crl_init(&next);
    if (mbedtls_x509_crl_parse_file(&next, cfg->ccliCrlFile) != 0 || !ccli_crl_valid(cfg, &next, false)) {
        raiseSecurityEvent(cfg, TLS_SEC_EVT_WARNING, CCLI_TLS_CRL_WARNING,
            "CRL unavailable, expired, not yet valid or invalid signature; retaining last valid CRL", self);
        mbedtls_x509_crl_free(&next);
        return;
    }
    for (mbedtls_x509_crl* old = &self->ccliCrl; old && old->version; old = old->next) {
        bool covered = false;
        for (mbedtls_x509_crl* candidate = &next; candidate && candidate->version; candidate = candidate->next) {
            if (old->issuer_raw.len == candidate->issuer_raw.len &&
                !memcmp(old->issuer_raw.p, candidate->issuer_raw.p, old->issuer_raw.len) &&
                memcmp(&candidate->this_update, &old->this_update, sizeof(old->this_update)) == 0)
                covered = true;
            /* Compare the six time components numerically (struct byte order is not chronological). */
            if (old->issuer_raw.len == candidate->issuer_raw.len &&
                !memcmp(old->issuer_raw.p, candidate->issuer_raw.p, old->issuer_raw.len)) {
                const mbedtls_x509_time* a = &candidate->this_update;
                const mbedtls_x509_time* b = &old->this_update;
                int av[] = {a->year,a->mon,a->day,a->hour,a->min,a->sec};
                int bv[] = {b->year,b->mon,b->day,b->hour,b->min,b->sec};
                for (int i = 0; i < 6; ++i) {
                    if (av[i] > bv[i]) { covered = true; break; }
                    if (av[i] < bv[i]) break;
                }
            }
        }
        if (!covered) {
            raiseSecurityEvent(cfg, TLS_SEC_EVT_WARNING, CCLI_TLS_CRL_WARNING,
                "CRL rollback or missing issuer rejected; retaining last valid CRL", self);
            mbedtls_x509_crl_free(&next);
            return;
        }
    }
    const bool changed = !ccli_crl_equal(&self->ccliCrl, &next);
    self->ccliCrlExpiryWarned = false;
    if (changed) {
        mbedtls_x509_crl_free(&self->ccliCrl);
        self->ccliCrl = next;
        mbedtls_ssl_conf_ca_chain(&self->conf, &cfg->cacerts, &self->ccliCrl);
        if (!initial) self->lastRenegotiationTime = 0;
    } else mbedtls_x509_crl_free(&next);
}

static bool ccli_connection_valid(TLSSocket self, bool allow_expired_crl)
{
    if (!self->tlsConfig->ccliRevocationSeconds) return true;
    uint64_t now = ccli_monotonic_ms();
    if (now - self->ccliCertificatesChecked <
        (uint64_t)self->tlsConfig->ccliRevocationSeconds * 1000 &&
        self->lastRenegotiationTime != 0 &&
        now - self->lastRenegotiationTime < (uint64_t)self->tlsConfig->renegotiationTimeInMs) return true;
    self->ccliCertificatesChecked = now;
    const mbedtls_x509_crt* peer = mbedtls_ssl_get_peer_cert(&self->ssl);
    mbedtls_x509_crl* crl = self->tlsConfig->ccliCrlFile ? &self->ccliCrl : &self->tlsConfig->crl;
    if (!allow_expired_crl && !ccli_crl_valid(self->tlsConfig, crl, false)) {
        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT, CCLI_TLS_CRL_WARNING,
            "alarm: TLS session cannot be established or resumed without a current, valid CRL", self);
        return false;
    }
    for (int own = 0; own < 2; ++own) {
        uint32_t flags = 0;
        mbedtls_x509_crt* cert = own ? &self->tlsConfig->ownCertificate : (mbedtls_x509_crt*)peer;
        if (!cert) return false;
        int result = mbedtls_x509_crt_verify_with_profile(cert, &self->tlsConfig->cacerts, crl,
            &self->tlsConfig->ccliCertProfile, NULL, &flags, NULL, NULL);
        if (result && !flags) flags = MBEDTLS_X509_BADCERT_OTHER;
        if (allow_expired_crl)
            flags &= ~(MBEDTLS_X509_BADCRL_EXPIRED | MBEDTLS_X509_BADCRL_FUTURE);
        if (flags) {
            const bool revoked = (flags & MBEDTLS_X509_BADCERT_REVOKED) != 0;
            raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT,
                revoked ? TLS_EVENT_CODE_ALM_CERT_REVOKED :
                (flags & MBEDTLS_X509_BADCERT_EXPIRED) ? TLS_EVENT_CODE_ALM_CERT_EXPIRED :
                TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
                own ? "Local TLS certificate no longer valid" : "Peer TLS certificate no longer valid", self);
            if (revoked)
                raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT,
                    CCLI_TLS_REVOKED_SESSION_TERMINATED,
                    "alarm: TLS session terminated because the peer certificate was revoked", self);
            return false;
        }
    }
    for (sCCLITLSOwnIdentity* identity = self->tlsConfig->additionalOwnIdentities;
         identity; identity = identity->next) {
        uint32_t flags = 0;
        int result = mbedtls_x509_crt_verify_with_profile(&identity->certificate,
            &self->tlsConfig->cacerts, crl, &self->tlsConfig->ccliCertProfile,
            NULL, &flags, NULL, NULL);
        if (result && !flags) flags = MBEDTLS_X509_BADCERT_OTHER;
        if (allow_expired_crl)
            flags &= ~(MBEDTLS_X509_BADCRL_EXPIRED | MBEDTLS_X509_BADCRL_FUTURE);
        if (flags) {
            const bool revoked = (flags & MBEDTLS_X509_BADCERT_REVOKED) != 0;
            raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT,
                revoked ? TLS_EVENT_CODE_ALM_CERT_REVOKED :
                (flags & MBEDTLS_X509_BADCERT_EXPIRED) ? TLS_EVENT_CODE_ALM_CERT_EXPIRED :
                TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
                "Local additional TLS certificate no longer valid", self);
            return false;
        }
    }
    return true;
}

bool CCLI_TLS_configure(TLSConfiguration cfg, const char* crl_file,
                       int revocation_s, int renegotiation_s, int resumption_s)
{
    if (!cfg) return false;
    uint32_t flags = 0;
    if (!crl_file || !crl_file[0]) goto crl_unavailable;
    if (cfg->setupComplete || revocation_s < 1 || revocation_s > 86400 ||
        renegotiation_s < 600 || renegotiation_s > 86400 ||
        resumption_s < 1 || resumption_s > CCLI_TLS_MAX_RESUMPTION_SECONDS || !cfg->chainValidation ||
        !cfg->cacerts.version) {
        raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, CCLI_TLS_POLICY_ALARM,
            "TLS policy rejected: invalid timer bounds, CA, chain validation, or TLS setup state", NULL);
        return false;
    }
    ccli_init_certificate_policy(cfg);
    if (!cfg->ownCertificate.version || mbedtls_pk_get_bitlen(&cfg->ownKey) == 0 ||
        mbedtls_pk_check_pair(&cfg->ownCertificate.pk, &cfg->ownKey,
                             mbedtls_ctr_drbg_random, &cfg->ctr_drbg) != 0)
        goto endpoint_unavailable;
    if (!ccli_certificate_chain_within_limit(&cfg->ownCertificate))
        goto certificate_size_exceeded;
    if (!ccli_check_certificate_chain_crypto(cfg, &cfg->ownCertificate)) {
        raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT,
            TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
            "TLS endpoint certificate cryptographic policy rejected", NULL);
        return false;
    }
    const int verify_result = mbedtls_x509_crt_verify_with_profile(&cfg->ownCertificate,
        &cfg->cacerts, &cfg->crl, &cfg->ccliCertProfile, NULL, &flags, NULL, NULL);
    const bool client_mode = mbedtls_ssl_conf_get_endpoint(&cfg->conf) == MBEDTLS_SSL_IS_CLIENT;
    const int eku_result = mbedtls_x509_crt_check_extended_key_usage(&cfg->ownCertificate,
        client_mode ? MBEDTLS_OID_CLIENT_AUTH : MBEDTLS_OID_SERVER_AUTH,
        client_mode ? MBEDTLS_OID_SIZE(MBEDTLS_OID_CLIENT_AUTH) : MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH));
    /* CRL freshness must not prevent a listener from starting. Permit only
     * this specific startup verification failure; revoked/expired certificates,
     * invalid signatures, CA trust and every other validation error still fail.
     * Initial TLS handshakes retain Mbed TLS's CRL freshness checks. */
    const bool expired_crl_only =
        verify_result == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED &&
        flags == MBEDTLS_X509_BADCRL_EXPIRED;
    if ((verify_result != 0 && !expired_crl_only) || eku_result != 0) {
        char reason[128];
        (void)snprintf(reason, sizeof(reason),
            "alarm: TLS endpoint certificate chain/EKU validation failed (flags=0x%08x, verify=%d, eku=%d)",
            (unsigned)flags, verify_result, eku_result);
        raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT,
            TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED, reason, NULL);
        return false;
    }
    for (sCCLITLSOwnIdentity* identity = cfg->additionalOwnIdentities;
         identity; identity = identity->next) {
        if (!identity->certificate.version || mbedtls_pk_get_bitlen(&identity->key) == 0 ||
            mbedtls_pk_check_pair(&identity->certificate.pk, &identity->key,
                mbedtls_ctr_drbg_random, &cfg->ctr_drbg) != 0)
            goto endpoint_unavailable;
        if (!ccli_certificate_chain_within_limit(&identity->certificate))
            goto certificate_size_exceeded;
        if (!ccli_check_certificate_chain_crypto(cfg, &identity->certificate)) {
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT,
                TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED,
                "TLS additional endpoint certificate cryptographic policy rejected", NULL);
            return false;
        }
        uint32_t identity_flags = 0;
        const int identity_verify_result = mbedtls_x509_crt_verify_with_profile(
            &identity->certificate, &cfg->cacerts, &cfg->crl, &cfg->ccliCertProfile,
            NULL, &identity_flags, NULL, NULL);
        const int identity_eku_result = mbedtls_x509_crt_check_extended_key_usage(
            &identity->certificate, MBEDTLS_OID_SERVER_AUTH,
            MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH));
        const bool identity_expired_crl_only =
            identity_verify_result == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED &&
            identity_flags == MBEDTLS_X509_BADCRL_EXPIRED;
        if ((identity_verify_result != 0 && !identity_expired_crl_only) ||
            identity_eku_result != 0) {
            char reason[128];
            (void)snprintf(reason, sizeof(reason),
                "alarm: additional TLS endpoint certificate chain/EKU validation failed (flags=0x%08x, verify=%d, eku=%d)",
                (unsigned)identity_flags, identity_verify_result, identity_eku_result);
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT,
                TLS_EVENT_CODE_ALM_CERT_VALIDATION_FAILED, reason, NULL);
            return false;
        }
    }
    if (crl_file && crl_file[0]) {
        if (!ccli_crl_valid(cfg, &cfg->crl, true)) {
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, CCLI_TLS_CRL_WARNING,
                "alarm: configured TLS CRL issuer, signature, or validity check failed", NULL);
            return false;
        }
        for (mbedtls_x509_crl* crl = &cfg->crl; crl && crl->version; crl = crl->next) {
            if (mbedtls_x509_time_is_past(&crl->next_update)) {
                raiseSecurityEvent(cfg, TLS_SEC_EVT_WARNING, CCLI_TLS_CRL_WARNING,
                    "warning: CRL expired; listener available, new TLS handshakes require current revocation data", NULL);
                break;
            }
        }
        size_t length = strlen(crl_file) + 1;
        cfg->ccliCrlFile = (char*) GLOBAL_MALLOC(length);
        if (!cfg->ccliCrlFile) {
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, CCLI_TLS_POLICY_ALARM,
                "TLS policy rejected: cannot retain configured CRL path", NULL);
            return false;
        }
        memcpy(cfg->ccliCrlFile, crl_file, length);
    }
    cfg->ccliRevocationSeconds = revocation_s;
    cfg->renegotiationTimeInMs = renegotiation_s * 1000;
    cfg->sessionResumptionInterval = resumption_s;
    cfg->useSessionResumption = true;
    cfg->minVersion = TLS_VERSION_TLS_1_2;
    cfg->maxVersion = TLS_VERSION_TLS_1_2;
    mbedtls_ssl_conf_authmode(&cfg->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    if (!client_mode)
        mbedtls_ssl_conf_preference_order(&cfg->conf,
            MBEDTLS_SSL_SRV_CIPHERSUITE_ORDER_CLIENT);
    TLSConfiguration_clearCipherSuiteList(cfg);
    for (size_t i = 0; i < sizeof(ccli_cipher_suites) / sizeof(ccli_cipher_suites[0]); ++i) {
        if (!mbedtls_ssl_ciphersuite_from_id(ccli_cipher_suites[i])) {
            char reason[96];
            (void)snprintf(reason, sizeof(reason),
                "TLS policy rejected: mandatory TLS 1.2 suite 0x%04x is unavailable",
                ccli_cipher_suites[i]);
            raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, CCLI_TLS_POLICY_ALARM, reason, NULL);
            return false;
        }
        TLSConfiguration_addCipherSuite(cfg, ccli_cipher_suites[i]);
    }
    mbedtls_ssl_conf_cert_profile(&cfg->conf, &cfg->ccliCertProfile);
    mbedtls_ssl_conf_sig_hashes(&cfg->conf, cfg->ccliSignatureHashes);
    if (!client_mode) mbedtls_ssl_conf_cert_cb(&cfg->conf, ccli_client_hello_policy);
    mbedtls_ssl_conf_legacy_renegotiation(&cfg->conf, MBEDTLS_SSL_LEGACY_BREAK_HANDSHAKE);
    mbedtls_ssl_conf_renegotiation_enforced(&cfg->conf, 16);
    mbedtls_ssl_conf_encrypt_then_mac(&cfg->conf, MBEDTLS_SSL_ETM_ENABLED);
    return true;
endpoint_unavailable:
    raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_UNAVAILABLE,
        "alarm: endpoint certificate unavailable", NULL);
    return false;
certificate_size_exceeded:
    raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_SIZE_EXCEEDED,
        "alarm: TLS certificate size exceeded", NULL);
    return false;
crl_unavailable:
    raiseSecurityEvent(cfg, TLS_SEC_EVT_INCIDENT, CCLI_TLS_CRL_WARNING,
        "alarm: TLS requires a configured CRL; peer revocation cannot be checked", NULL);
    return false;
}

static bool ccli_negotiated_policy(TLSSocket self)
{
    const int negotiated_version = (int)mbedtls_ssl_get_version_number(&self->ssl);
    if (self->ccliInitialVersion == 0) {
        self->ccliInitialVersion = negotiated_version;
    } else if (self->ccliInitialVersion != negotiated_version) {
        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT,
            CCLI_TLS_VERSION_CHANGED,
            "alarm: negotiated TLS version changed during renegotiation", self);
        return false;
    }
    if (!self->tlsConfig->ccliRevocationSeconds) return true;
    const mbedtls_ssl_ciphersuite_t* suite = mbedtls_ssl_ciphersuite_from_id(
        mbedtls_ssl_get_ciphersuite_id(mbedtls_ssl_get_ciphersuite(&self->ssl)));
    if (!suite) return false;
    /* TLS_RSA_WITH_AES_128_CBC_SHA256 must negotiate RFC 7366 EtM. */
    if (mbedtls_ssl_get_ciphersuite_id_from_ssl(&self->ssl) == 0x003c &&
        !self->ssl.MBEDTLS_PRIVATE(session)->MBEDTLS_PRIVATE(encrypt_then_mac)) {
        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT, CCLI_TLS_POLICY_ALARM,
            "CBC negotiated without encrypt-then-MAC; terminating TLS", self);
        return false;
    }
    return true;
}
