#ifndef CCLI_IEC62351_TLS_H
#define CCLI_IEC62351_TLS_H
#include "tls_config.h"
#include "tls_socket.h"
#define CCLI_TLS_EVENT_CODE_CRL 1001
#define CCLI_TLS_EVENT_CODE_POLICY 1002
#define CCLI_TLS_EVENT_CODE_DEPRECATED_HASH 1003
#define CCLI_TLS_EVENT_CODE_REVOKED_SESSION_TERMINATED 1004
#define CCLI_TLS_EVENT_CODE_VERSION_CHANGED 1005
#ifdef __cplusplus
extern "C" {
#endif
/* Configure before accepting connections, after loading identity, CA chain and
 * at least one valid CRL. Resumption is enabled only for validated peer
 * sessions and is limited to 1..7200 seconds (one-hour default). A missing or
 * invalid CRL rejects new/resumed sessions.
 * Returns false and emits an alarm for invalid identity, CRL or timers.
 * CRLs are replaced atomically by the operator; each connection owns its copy.
 * This backend does not implement OCSP stapling (see the TLS gap report). */
bool CCLI_TLS_configure(TLSConfiguration config, const char* crl_file,
                       int revocation_check_s, int renegotiation_s,
                       int resumption_s);
/* SHA-384/SHA-512 are optional certificate/signature hashes and are disabled
 * by default. SHA-256 remains mandatory and preferred. Call before configure. */
bool CCLI_TLS_setOptionalHashes(TLSConfiguration config, bool sha384, bool sha512);
/* Call from the connection's existing I/O thread, including while idle. */
bool CCLI_TLS_tick(TLSSocket socket);
/* Bound client handshake/write/close operations (milliseconds; 0 = legacy).
 * Set before creating the socket; used by the background Syslog TLS sender. */
void CCLI_TLS_setIoTimeout(TLSConfiguration config, unsigned int timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
