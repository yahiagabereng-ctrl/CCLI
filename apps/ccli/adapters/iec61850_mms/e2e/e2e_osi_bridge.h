#ifndef CCLI_E2E_OSI_BRIDGE_H
#define CCLI_E2E_OSI_BRIDGE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { CCLI_E2E_ASSOCIATE = 1, CCLI_E2E_RECEIVE, CCLI_E2E_SEND,
       CCLI_E2E_RELEASE, CCLI_E2E_CLOSE, CCLI_E2E_TICK, CCLI_E2E_CONTROL,
       CCLI_E2E_PRESENTATION, CCLI_E2E_PRESENTATION_RELEASE, CCLI_E2E_PRESENTATION_ABORT };
enum { CCLI_E2E_ACCEPT = 1, CCLI_E2E_DATA, CCLI_E2E_ABORT, CCLI_E2E_RELEASE_RESPONSE };
typedef struct {
    void* connection;
    void** security_token;
    const char* peer_address;
    const uint8_t* tls_certificate;
    int tls_certificate_size;
    int acse_context;
    int mms_context;
    int e2e_context;
    int input_context;
    int (*dispatch)(void* connection, const uint8_t* request, int request_size,
                    uint8_t* response, int response_capacity);
    int (*set_reference)(void* connection, const uint32_t* ap_arcs, int arc_count, uint32_t qualifier);
    int (*set_maximum_pdu)(void* connection, int maximum);
} CcliE2eIo;
typedef struct { int size; int kind; int close_after_send; } CcliE2eReply;
/* Return 0 only for an explicitly allowed compatibility association; -1 closes.
 * The hook is installed before listening and removed after all connections stop.
 * Session storage belongs to the individual ISO connection. */
typedef int (*CcliE2eHandler)(void* owner, void** session, int event, const CcliE2eIo* io,
    const uint8_t* input, int input_size, uint8_t* output, int capacity, CcliE2eReply* reply);
int CcliE2e_install(void* owner, CcliE2eHandler handler);
/* Called only from a synchronous MMS service callback on this connection.
 * Queues signed ApplAbort after callback return, avoiding recursive send/locks.
 * Optional diagnostic is an encoded MMS PDU; no key material is accepted. */
int CcliE2e_requestApplicationAbort(void* security_token, const uint8_t* diagnostic, int size);
/* MMS association overlay uses the native dispatch context, or the unchanged
 * stack maximum for compatibility connections. */
int CcliE2e_currentMaximumPdu(void);
/* Internal unwind cleanup if a C++ MMS callback throws through the C bridge. */
void CcliE2e_clearDispatchContext(void* connection);
#ifdef __cplusplus
}
#endif
#endif
