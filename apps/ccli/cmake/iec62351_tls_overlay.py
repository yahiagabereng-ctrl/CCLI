#!/usr/bin/env python3
"""Apply explicit, count-checked TLS HAL corrections after the pinning fix."""
import pathlib
import sys


def patch(text):
    def replace(old, new, count=1):
        nonlocal text
        if text.count(old) != count:
            raise RuntimeError(f'Review IEC 62351 HAL overlay: expected {count} occurrences of {old!r}')
        text = text.replace(old, new)

    # The pinned provider's ssl_misc.h needs private access enabled before its
    # public headers are first parsed. Limit this to the generated HAL unit.
    replace('#include <string.h>', '#define MBEDTLS_ALLOW_PRIVATE_ACCESS\n#include <string.h>')
    replace('static int psaInitCounter = 0;', '''static int psaInitCounter = 0;

typedef struct sCCLITLSOwnIdentity {
    mbedtls_x509_crt certificate;
    mbedtls_pk_context key;
    struct sCCLITLSOwnIdentity* next;
} sCCLITLSOwnIdentity;''')
    replace('    int maxCiphersuites;\n', '    int maxCiphersuites;\n    sCCLITLSOwnIdentity* additionalOwnIdentities;\n    char* ccliCrlFile;\n    int ccliRevocationSeconds;\n    unsigned int ccliOptionalHashMask;\n    mbedtls_x509_crt_profile ccliCertProfile;\n    int ccliSignatureHashes[4];\n    Semaphore ccliSessionCacheLock;\n    bool ccliSessionCacheInitialized;\n    uint64_t ccliSessionCacheCreated[MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES];\n    unsigned char ccliSessionCacheIds[MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES][32];\n    size_t ccliSessionCacheIdLengths[MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES];\n    size_t ccliSessionCacheNext;\n')
    replace('''        mbedtls_ssl_conf_ciphersuites(&(self->conf), self->ciphersuites);''', '''        for (sCCLITLSOwnIdentity* identity = self->additionalOwnIdentities;
             identity; identity = identity->next) {
            int ret = mbedtls_ssl_conf_own_cert(&(self->conf),
                &identity->certificate, &identity->key);
            if (ret != 0) {
                DEBUG_PRINT("TLS", "mbedtls_ssl_conf_own_cert (additional identity) returned -0x%x\\n", -ret);
                return false;
            }
        }

        mbedtls_ssl_conf_ciphersuites(&(self->conf), self->ciphersuites);''')
    replace('''bool
TLSConfiguration_addAllowedCertificate(TLSConfiguration self, uint8_t* certificate, int certLen)''', '''bool
TLSConfiguration_addOwnCertificateAndKeyFromFiles(TLSConfiguration self,
                                                    const char* certificateFilename,
                                                    const char* keyFilename,
                                                    const char* keyPassword)
{
    if (!self || !certificateFilename || !certificateFilename[0] ||
        !keyFilename || !keyFilename[0] || self->setupComplete ||
        mbedtls_ssl_conf_get_endpoint(&self->conf) != MBEDTLS_SSL_IS_SERVER)
        return false;

    sCCLITLSOwnIdentity* identity =
        (sCCLITLSOwnIdentity*)GLOBAL_CALLOC(1, sizeof(*identity));
    if (!identity) return false;
    mbedtls_x509_crt_init(&identity->certificate);
    mbedtls_pk_init(&identity->key);

    int ret = mbedtls_x509_crt_parse_file(&identity->certificate, certificateFilename);
    if (ret == 0)
        ret = mbedtls_pk_parse_keyfile(&identity->key, keyFilename, keyPassword,
            mbedtls_ctr_drbg_random, &self->ctr_drbg);
    if (ret != 0 || !identity->certificate.version ||
        mbedtls_pk_get_bitlen(&identity->key) == 0 ||
        mbedtls_pk_check_pair(&identity->certificate.pk, &identity->key,
            mbedtls_ctr_drbg_random, &self->ctr_drbg) != 0) {
        DEBUG_PRINT("TLS", "additional server identity parse or key-pair validation failed\\n");
        mbedtls_x509_crt_free(&identity->certificate);
        mbedtls_pk_free(&identity->key);
        GLOBAL_FREEMEM(identity);
        return false;
    }

    sCCLITLSOwnIdentity** tail = &self->additionalOwnIdentities;
    while (*tail) tail = &(*tail)->next;
    *tail = identity;
    return true;
}

bool
TLSConfiguration_addAllowedCertificate(TLSConfiguration self, uint8_t* certificate, int certLen)''')
    replace('''        mbedtls_ssl_config_free(&(self->conf));
        mbedtls_ctr_drbg_free(&(self->ctr_drbg));''', '''        mbedtls_ssl_config_free(&(self->conf));

        sCCLITLSOwnIdentity* identity = self->additionalOwnIdentities;
        while (identity) {
            sCCLITLSOwnIdentity* next = identity->next;
            mbedtls_x509_crt_free(&identity->certificate);
            mbedtls_pk_free(&identity->key);
            GLOBAL_FREEMEM(identity);
            identity = next;
        }

        mbedtls_ctr_drbg_free(&(self->ctr_drbg));''')
    replace('    uint64_t lastRenegotiationTime;', '    uint64_t lastRenegotiationTime;\n    int ccliInitialVersion;\n    mbedtls_x509_crl ccliCrl;\n    uint64_t ccliCrlChecked;\n    uint64_t ccliCertificatesChecked;\n    uint64_t ccliRenegotiationStarted;\n    bool ccliPeerVerified;\n    bool ccliEstablished;\n    bool ccliCrlExpiryWarned;')
    replace('static int\nverifyCertificate', '#include "iec62351_backend.inc"\n\nstatic int\nverifyCertificate')
    replace('''    /* We need to clean-up resumption cache (if enabled) to make sure we renegotiate as CRL may have changed data */
    if (self->useSessionResumption == false)
        return;

    if (mbedtls_ssl_conf_get_endpoint(&(self->conf)) == MBEDTLS_SSL_IS_SERVER)
    {
        mbedtls_ssl_cache_free(&(self->cache));
    }''', '''    /* Cache hits revalidate the saved peer certificate against the current CRL.
     * Keep the shared cache intact: freeing it here races active TLS workers. */''')
    replace('    DEBUG_PRINT("TLS", "Verify cert: depth %i\\n", certificate_depth);', '''    if (crt && crt->raw.len > CCLI_TLS_MAX_CERT_OCTETS) {
        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT,
            TLS_EVENT_CODE_ALM_CERT_SIZE_EXCEEDED,
            "alarm: TLS certificate size exceeded", self);
        *flags |= MBEDTLS_X509_BADCERT_OTHER;
        return 1;
    }
    if (crt && !ccli_check_certificate_crypto(self->tlsConfig, crt, self)) {
        *flags |= MBEDTLS_X509_BADCERT_BAD_KEY;
        return 1;
    }

    DEBUG_PRINT("TLS", "Verify cert: depth %i\\n", certificate_depth);''')
    replace('Alarm: certificate validation: trusted individual certificate not available',
            'alarm: certificate validation: trusted individual certificate from authorized CA not available')
    replace('''                raiseSecurityEvent(config,TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_REVOKED, "Alarm: revoked certificate", socket);''', '''                raiseSecurityEvent(config,TLS_SEC_EVT_INCIDENT, TLS_EVENT_CODE_ALM_CERT_REVOKED, "Alarm: revoked certificate", socket);
                raiseSecurityEvent(config,TLS_SEC_EVT_INCIDENT, CCLI_TLS_REVOKED_SESSION_TERMINATED,
                    "alarm: TLS session terminated because the peer certificate was revoked", socket);''')
    replace('"Alarm: certificate unavailable", socket);',
            '"alarm: peer certificate unavailable", socket);')
    replace('    DEBUG_PRINT("TLS", "Verify cert: depth %i\\n", certificate_depth);', '''    /* CRL retrieval failure/expiry alone must not terminate an established
     * association. Revoked, bad-signature and certificate-expiry flags survive. */
    if (self->ccliEstablished && (*flags & (MBEDTLS_X509_BADCRL_EXPIRED | MBEDTLS_X509_BADCRL_FUTURE))) {
        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_WARNING, CCLI_TLS_CRL_WARNING,
            "CRL expired or not yet valid during established session", self);
        *flags &= ~(MBEDTLS_X509_BADCRL_EXPIRED | MBEDTLS_X509_BADCRL_FUTURE);
    }
    DEBUG_PRINT("TLS", "Verify cert: depth %i\\n", certificate_depth);''')
    replace('                self->peerCertLength = 0;', '                if (self->peerCert) GLOBAL_FREEMEM(self->peerCert);\n                self->peerCert = NULL;\n                self->peerCertLength = 0;')
    replace('        if (self->storePeerCert)', '        if (*flags == 0) self->ccliPeerVerified = true;\n        if (self->storePeerCert)')
    replace('        self->peerCertLength = 0;\n\n        TLSConfiguration_setupComplete(configuration);', '''        self->peerCertLength = 0;
        mbedtls_x509_crl_init(&self->ccliCrl);

        if (!TLSConfiguration_setupComplete(configuration)) {
            GLOBAL_FREEMEM(self);
            return NULL;
        }''')
    replace('        self->crlUpdated = configuration->crlUpdated;', '''        self->crlUpdated = configuration->crlUpdated;
        if (configuration->ccliCrlFile) {
            /* Start with the last authenticated startup CRL, then refresh. */
            for (mbedtls_x509_crl* crl = &configuration->crl; crl && crl->version; crl = crl->next) {
                if (mbedtls_x509_crl_parse_der(&self->ccliCrl, crl->raw.p, crl->raw.len) != 0) {
                    mbedtls_x509_crl_free(&self->ccliCrl);
                    GLOBAL_FREEMEM(self);
                    return NULL;
                }
            }
            mbedtls_ssl_conf_ca_chain(&self->conf, &configuration->cacerts, &self->ccliCrl);
            ccli_refresh_crl(self, true);
        }''')
    replace('        mbedtls_ssl_set_bio(', '        mbedtls_ssl_set_user_data_p(&self->ssl, self);\n        mbedtls_ssl_set_bio(')
    replace('        self->lastRenegotiationTime = Hal_getTimeInMs();', '''        if (!ccli_negotiated_policy(self)) {
            mbedtls_ssl_free(&self->ssl);
            mbedtls_x509_crl_free(&self->ccliCrl);
            if (self->peerCert) GLOBAL_FREEMEM(self->peerCert);
            GLOBAL_FREEMEM(self);
            return NULL;
        }
        self->ccliEstablished = true;
        self->lastRenegotiationTime = Hal_getTimeInMs();''')
    replace('                mbedtls_ssl_free(&(self->ssl));', '                mbedtls_ssl_free(&(self->ssl));\n                mbedtls_x509_crl_free(&self->ccliCrl);')
    replace('        mbedtls_x509_crl_free(&(self->crl));', '        mbedtls_x509_crl_free(&(self->crl));\n        if (self->ccliCrlFile) GLOBAL_FREEMEM(self->ccliCrlFile);')
    replace('    mbedtls_ssl_free(&(self->ssl));\n\n    if (self->peerCert)', '    mbedtls_ssl_free(&(self->ssl));\n    mbedtls_x509_crl_free(&self->ccliCrl);\n\n    if (self->peerCert)')
    replace('    if (self->crlUpdated == self->tlsConfig->crlUpdated)', '''    if (self->tlsConfig->ccliCrlFile) {
        ccli_refresh_crl(self, false);
        return;
    }
    if (self->crlUpdated == self->tlsConfig->crlUpdated)''')
    replace('    if (self->tlsConfig->renegotiationTimeInMs <= 0)', '''    if (!ccli_connection_valid(self)) return false;
    if (self->ccliRenegotiationStarted) {
        if (self->ccliPeerVerified && mbedtls_ssl_is_handshake_over(&self->ssl)) {
            if (!ccli_negotiated_policy(self)) return false;
            self->ccliRenegotiationStarted = 0;
            raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INFO,
                TLS_EVENT_CODE_INF_SESSION_RENEGOTIATION, "TLS renegotiation completed", self);
        } else {
            if (ccli_monotonic_ms() - self->ccliRenegotiationStarted >= 30000) {
                raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT, CCLI_TLS_POLICY_ALARM,
                    "Peer did not complete TLS renegotiation within 30 seconds", self);
                return false;
            }
            return true;
        }
    }
    if (self->tlsConfig->renegotiationTimeInMs <= 0)''')
    replace('    if (TLSSocket_performHandshake(self) == false)', '''    self->ccliPeerVerified = false;
    self->ccliRenegotiationStarted = ccli_monotonic_ms();
    if (TLSSocket_performHandshake(self) == false)''')
    replace('        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INFO, TLS_EVENT_CODE_INF_SESSION_RENEGOTIATION, "Alarm: Renegotiation failed", self);', '''        createSecurityEvents(self->tlsConfig, ret,
            mbedtls_ssl_get_verify_result(&self->ssl), self);
        raiseSecurityEvent(self->tlsConfig, TLS_SEC_EVT_INCIDENT,
            TLS_EVENT_CODE_ALM_HANDSHAKE_FAILED_UNKNOWN_REASON, "Renegotiation failed", self);''')
    # Check negotiated EtM again after peer- or locally initiated renegotiation.
    replace('    return ret;\n}\n\nint\nTLSSocket_write', '    if (ret > 0 && !ccli_negotiated_policy(self)) return -1;\n    return ret;\n}\n\nint\nTLSSocket_write')
    replace('int\nTLSSocket_read', '''bool
CCLI_TLS_tick(TLSSocket self)
{
    if (!self) return false;
    checkForCRLUpdate(self);
    return startRenegotiationIfRequired(self);
}

int
TLSSocket_read''')
    replace('                mbedtls_ssl_cache_init( &(self->cache) );', '                if (!self->ccliSessionCacheInitialized) {\n                    mbedtls_ssl_cache_init(&(self->cache));\n                    self->ccliSessionCacheInitialized = true;\n                }\n                if (!self->ccliSessionCacheLock)\n                    self->ccliSessionCacheLock = Semaphore_create(1);\n                if (!self->ccliSessionCacheLock) return false;')
    replace('        self->sessionResumptionInterval = 21600; /* default value: 6h */',
            '        self->sessionResumptionInterval = 3600; /* one-hour default; IEC maximum is two hours */')
    replace('    self->sessionResumptionInterval = intervalInSeconds;',
            '    self->sessionResumptionInterval = intervalInSeconds < 1 ? 1 : (intervalInSeconds > 7200 ? 7200 : intervalInSeconds);')
    replace('mbedtls_ssl_conf_session_cache( &(self->conf), &(self->cache),\n                                   mbedtls_ssl_cache_get,\n                                   mbedtls_ssl_cache_set );', 'mbedtls_ssl_conf_session_cache( &(self->conf), self,\n                                   ccli_tls_session_cache_get,\n                                   ccli_tls_session_cache_set );')
    replace('        self->ccliEstablished = true;\n        self->lastRenegotiationTime = Hal_getTimeInMs();', '        if (!ccli_connection_valid(self, false)) {\n            mbedtls_ssl_free(&self->ssl);\n            mbedtls_x509_crl_free(&self->ccliCrl);\n            if (self->peerCert) GLOBAL_FREEMEM(self->peerCert);\n            GLOBAL_FREEMEM(self);\n            return NULL;\n        }\n        self->ccliPeerVerified = true;\n        self->ccliEstablished = true;\n        self->lastRenegotiationTime = Hal_getTimeInMs();')
    replace('    if (!ccli_connection_valid(self)) return false;', '    if (!ccli_connection_valid(self, true)) return false;')
    replace('                mbedtls_ssl_cache_free(&(self->cache));', '                if (self->ccliSessionCacheInitialized) mbedtls_ssl_cache_free(&(self->cache));\n                if (self->ccliSessionCacheLock) Semaphore_destroy(self->ccliSessionCacheLock);')
    # Syslog runs in a background worker. Bound its handshake/write/close so a
    # stalled collector cannot hang shutdown; existing MMS/104 defaults remain.
    replace('    int ccliRevocationSeconds;', '    int ccliRevocationSeconds;\n    unsigned int ccliIoTimeoutMs;')
    replace('        while( (ret = mbedtls_ssl_handshake(&(self->ssl)) ) != 0 )',
            '        const uint64_t ccliHandshakeStart = ccli_monotonic_ms();\n        while( (ret = mbedtls_ssl_handshake(&(self->ssl)) ) != 0 )')
    replace('            if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )',
            '            if (configuration->ccliIoTimeoutMs && ccli_monotonic_ms() - ccliHandshakeStart >= configuration->ccliIoTimeoutMs)\n                ret = MBEDTLS_ERR_SSL_TIMEOUT;\n            if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )')
    replace('    int len = 0;\n\n    checkForCRLUpdate(self);',
            '    int len = 0;\n    const uint64_t ccliWriteStart = ccli_monotonic_ms();\n\n    checkForCRLUpdate(self);')
    replace('    while (len < size)\n    {',
            '    while (len < size)\n    {\n        if (self->tlsConfig->ccliIoTimeoutMs && ccli_monotonic_ms() - ccliWriteStart >= self->tlsConfig->ccliIoTimeoutMs) return -1;')
    replace('    /* TODO add timeout? */', '    const uint64_t ccliCloseStart = ccli_monotonic_ms();')
    replace('    while ((ret = mbedtls_ssl_close_notify(&(self->ssl))) < 0)\n    {',
            '    while ((ret = mbedtls_ssl_close_notify(&(self->ssl))) < 0)\n    {\n        if (self->tlsConfig->ccliIoTimeoutMs && ccli_monotonic_ms() - ccliCloseStart >= self->tlsConfig->ccliIoTimeoutMs) break;')
    replace('bool\nCCLI_TLS_tick', 'void\nCCLI_TLS_setIoTimeout(TLSConfiguration config, unsigned int timeout_ms)\n{\n    if (config) config->ccliIoTimeoutMs = timeout_ms > 30000 ? 30000 : timeout_ms;\n}\n\nbool\nCCLI_TLS_tick')
    replace('            if (configuration->ccliIoTimeoutMs && ccli_monotonic_ms() - ccliHandshakeStart',
            '            if (configuration->ccliIoTimeoutMs && (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)) Thread_sleep(1);\n            if (configuration->ccliIoTimeoutMs && ccli_monotonic_ms() - ccliHandshakeStart')
    replace('''            (ret == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS) || (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)) {
            continue;''', '''            (ret == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS) || (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)) {
            if (self->tlsConfig->ccliIoTimeoutMs) Thread_sleep(1);
            continue;''')
    # Security intervals must not be postponed by wall-clock corrections.
    text = text.replace('Hal_getTimeInMs()', 'ccli_monotonic_ms()')
    return text


if __name__ == '__main__':
    source = pathlib.Path(sys.argv[1])
    source.write_text(patch(source.read_text()), encoding='utf-8')
