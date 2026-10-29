#!/usr/bin/env python3
"""Apply the CCLI native E2E connection hooks to the reviewed compatibility overlay."""
import argparse
import hashlib
from pathlib import Path


def replace_once(text, old, new):
    if text.count(old) != 1:
        raise RuntimeError("Native E2E overlay anchor changed: " + old[:100])
    return text.replace(old, new, 1)

def replace_function(text, name, replacement):
    start = text.index("\nvoid\n" + name + "(") + 1
    brace = text.index("{", start)
    depth, end = 1, brace + 1
    while depth:
        if text[end] == "{": depth += 1
        elif text[end] == "}": depth -= 1
        end += 1
    return text[:start] + replacement + text[end:]


def control_spdu(name, code):
    abort = code == 25
    return f'''void
{name}(IsoSession* self, BufferChain buffer, BufferChain payload)
{{
    (void)self;
    int size = payload->length;
    int total = {3 if abort else 0} + 1 + (size < 255 ? 1 : 3) + size;
    int needed = 1 + (total < 255 ? 1 : 3) + {3 if abort else 0} + 1 + (size < 255 ? 1 : 3);
    if (size < 0 || total > 65535 || needed > buffer->partMaxLength) {{
        buffer->partLength = buffer->length = 0; buffer->nextPart = NULL; return;
    }}
    uint8_t* out = buffer->buffer;
    out[0] = {code};
    int pos = ccli_session_length(out, 1, total);
    {'out[pos++] = 17; out[pos++] = 1; out[pos++] = 11;' if abort else ''}
    out[pos++] = size < 255 ? 193 : 194;
    pos = ccli_session_length(out, pos, size);
    buffer->partLength = pos; buffer->length = pos + size; buffer->nextPart = payload;
}}'''

SUPPORT = r'''
/* CCLI native E2E: installed before listen, removed only after connections stop. */
static void* ccli_e2e_owner;
static CcliE2eHandler ccli_e2e_handler;
#if defined(_MSC_VER)
static __declspec(thread) IsoConnection ccli_e2e_dispatching;
#else
static __thread IsoConnection ccli_e2e_dispatching;
#endif

int CcliE2e_currentMaximumPdu(void)
{
    IsoConnection self = ccli_e2e_dispatching;
    return self && self->ccliE2eMaximumPdu > 0 ? self->ccliE2eMaximumPdu : CONFIG_MMS_MAXIMUM_PDU_SIZE;
}

void CcliE2e_clearDispatchContext(void* connection)
{
    if (ccli_e2e_dispatching == (IsoConnection) connection) ccli_e2e_dispatching = NULL;
}

static int ccliE2eSetMaximumPdu(void* connection, int maximum)
{
    IsoConnection self = (IsoConnection) connection;
    if (maximum < 1024 || maximum > 64000 || maximum > CONFIG_MMS_MAXIMUM_PDU_SIZE) return 0;
    self->ccliE2eMaximumPdu = maximum;
    return 1;
}

int CcliE2e_requestApplicationAbort(void* token, const uint8_t* diagnostic, int size)
{
    IsoConnection self = ccli_e2e_dispatching;
    if (!self || !self->ccliE2eSession || self->acseConnection->state != connected ||
        !token || token != self->acseConnection->securityToken || self->ccliE2eAbortPending ||
        size < 0 || size > CONFIG_MMS_MAXIMUM_PDU_SIZE || (size && !diagnostic)) return 0;
    uint8_t* copy = size ? (uint8_t*) GLOBAL_MALLOC(size) : NULL;
    if (size && !copy) return 0;
    if (size) memcpy(copy, diagnostic, size);
    self->ccliE2eAbortDiagnostic = copy;
    self->ccliE2eAbortSize = size;
    self->ccliE2eAbortPending = true;
    return 1;
}

int CcliE2e_install(void* owner, CcliE2eHandler handler)
{
    if (!owner || (ccli_e2e_owner && ccli_e2e_owner != owner)) return 0;
    ccli_e2e_owner = handler ? owner : NULL;
    ccli_e2e_handler = handler;
    return 1;
}

static int ccliE2eDispatch(void* connection, const uint8_t* request, int requestSize,
        uint8_t* response, int capacity)
{
    IsoConnection self = (IsoConnection) connection;
    ByteBuffer input, output;
    if (!self->msgRcvdHandler || requestSize < 0 || capacity < CONFIG_MMS_MAXIMUM_PDU_SIZE) return -1;
    ByteBuffer_wrap(&input, (uint8_t*) request, requestSize, requestSize);
    ByteBuffer_wrap(&output, response, 0, capacity);
    IsoConnection previous = ccli_e2e_dispatching;
    ccli_e2e_dispatching = self;
    self->msgRcvdHandler(self->handlerParameter, &input, &output);
    ccli_e2e_dispatching = previous;
    if (self->ccliE2eAbortPending) {
        int size = self->ccliE2eAbortSize;
        if (size <= capacity && size) memcpy(response, self->ccliE2eAbortDiagnostic, size);
        GLOBAL_FREEMEM(self->ccliE2eAbortDiagnostic);
        self->ccliE2eAbortDiagnostic = NULL;
        self->ccliE2eAbortPending = false;
        return size <= capacity ? -size - 2 : -1;
    }
    return output.size;
}
static int ccliE2eSetReference(void* connection, const uint32_t* arcs, int count, uint32_t qualifier)
{
    IsoConnection self = (IsoConnection) connection;
    if (!arcs || count < 2 || count > 10 || qualifier > 2147483647U) return 0;
    self->acseConnection->applicationReference.apTitle.arcCount = count;
    for (int i = 0; i < count; ++i)
        self->acseConnection->applicationReference.apTitle.arc[i] = arcs[i];
    self->acseConnection->applicationReference.aeQualifier = (int) qualifier;
    return 1;
}

/* Caller retains the stack's existing ISO connection send/dispatch lock. */
static int ccliE2eHandle(IsoConnection self, int event, ByteBuffer* input)
{
    if (!ccli_e2e_handler ||
        ccli_e2e_owner != IsoServer_getAuthenticatorParameter(self->isoServer)) return 0;
    CcliE2eIo io;
    memset(&io, 0, sizeof(io));
    io.connection = self;
    io.security_token = &self->acseConnection->securityToken;
    io.peer_address = self->clientAddress;
    io.acse_context = self->presentation->acseContextId;
    io.mms_context = self->presentation->mmsContextId;
    io.e2e_context = self->presentation->ccliE2eContextId;
    io.input_context = self->presentation->nextContextId;
    io.dispatch = ccliE2eDispatch;
    io.set_reference = ccliE2eSetReference;
    io.set_maximum_pdu = ccliE2eSetMaximumPdu;
#if (CONFIG_MMS_SUPPORT_TLS == 1)
    if (self->tlsSocket)
        io.tls_certificate = TLSSocket_getPeerCertificate(self->tlsSocket, &io.tls_certificate_size);
#endif
    if (event != CCLI_E2E_CLOSE && !self->ccliE2eOutput)
        self->ccliE2eOutput = (uint8_t*) GLOBAL_MALLOC(SEND_BUF_SIZE);
    if (event != CCLI_E2E_CLOSE && !self->ccliE2eOutput) {
        self->state = ISO_CON_STATE_STOPPED; return -1;
    }
    CcliE2eReply reply;
    memset(&reply, 0, sizeof(reply));
    int handled = ccli_e2e_handler(ccli_e2e_owner, &self->ccliE2eSession, event, &io,
        input ? input->buffer : NULL, input ? input->size : 0,
        self->ccliE2eOutput, SEND_BUF_SIZE, &reply);
    if (handled < 0 || reply.size < 0 || reply.size > SEND_BUF_SIZE) {
        self->state = ISO_CON_STATE_STOPPED; return -1;
    }
    if (!handled) return 0;
    if (reply.kind == CCLI_E2E_ACCEPT && !reply.close_after_send) {
        self->acseConnection->state = connected;
        self->acseConnection->nextReference = self->presentation->ccliE2eContextId;
    }
    if (reply.size) {
        struct sBufferChain payload, presentation, session;
        BufferChain_init(&payload, reply.size, reply.size, NULL, self->ccliE2eOutput);
        memset(&presentation, 0, sizeof(presentation));
        memset(&session, 0, sizeof(session));
        presentation.buffer = self->sendBuffer;
        presentation.partMaxLength = SEND_BUF_SIZE;
        if (reply.kind == CCLI_E2E_ACCEPT)
            IsoPresentation_createCpaMessage(self->presentation, &presentation, &payload);
        else if (reply.kind == CCLI_E2E_DATA) {
            uint8_t saved = self->presentation->mmsContextId;
            self->presentation->mmsContextId = (uint8_t) self->presentation->ccliE2eContextId;
            IsoPresentation_createUserData(self->presentation, &presentation, &payload);
            self->presentation->mmsContextId = saved;
        }
        else
            IsoPresentation_createUserDataACSE(self->presentation, &presentation, &payload);
        session.buffer = self->sendBuffer + presentation.partLength;
        session.partMaxLength = SEND_BUF_SIZE - presentation.partLength;
        if (reply.kind == CCLI_E2E_ACCEPT)
            IsoSession_createAcceptSpdu(self->session, &session, &presentation);
        else if (reply.kind == CCLI_E2E_ABORT)
            IsoSession_createAbortSpdu(self->session, &session, &presentation);
        else if (reply.kind == CCLI_E2E_RELEASE_RESPONSE)
            IsoSession_createDisconnectSpdu(self->session, &session, &presentation);
        else
            IsoSession_createDataSpdu(self->session, &session, &presentation);
        if (CotpConnection_sendDataMessage(self->cotpConnection, &session) != COTP_OK) {
            self->state = ISO_CON_STATE_STOPPED; return -1;
        }
    }
    if (reply.close_after_send) self->state = ISO_CON_STATE_STOPPED;
    return 1;
}
'''


def generate(source, output, header, vendor):
    path = source / "iso_connection.c"
    connection = path.read_text()
    connection = replace_once(connection, '#include "iso_server_private.h"',
                              '#include "iso_server_private.h"\n#include "e2e_osi_bridge.h"')
    connection = connection.replace("CONFIG_MMS_MAXIMUM_PDU_SIZE + 100",
                                    "CONFIG_MMS_MAXIMUM_PDU_SIZE + 16384")
    connection = replace_once(connection, "    IsoServer isoServer;",
        "    IsoServer isoServer;\n    void* ccliE2eSession;\n    uint8_t* ccliE2eOutput;\n"
        "    bool ccliE2eAbortPending;\n    uint8_t* ccliE2eAbortDiagnostic;\n    int ccliE2eAbortSize;\n"
        "    int ccliE2eMaximumPdu;")
    connection = replace_once(connection, "static void\nfinalizeIsoConnection", SUPPORT + "\nstatic void\nfinalizeIsoConnection")
    connection = replace_once(connection, "finalizeIsoConnection(IsoConnection self)\n{",
        "finalizeIsoConnection(IsoConnection self)\n{\n"
        "    ccliE2eHandle(self, CCLI_E2E_CLOSE, NULL);\n"
        "    GLOBAL_FREEMEM(self->ccliE2eOutput);\n    self->ccliE2eOutput = NULL;\n"
        "    GLOBAL_FREEMEM(self->ccliE2eAbortDiagnostic);\n    self->ccliE2eAbortDiagnostic = NULL;")
    connection = replace_once(connection, "    CotpConnection_flushBuffer(self->cotpConnection);",
        "    CotpConnection_flushBuffer(self->cotpConnection);\n"
        "    if (self->ccliE2eSession) {\n"
        "        IsoConnection_lock(self);\n"
        "        ccliE2eHandle(self, CCLI_E2E_TICK, NULL);\n"
        "        IsoConnection_unlock(self);\n    }")
    connection = replace_once(connection,
        "                if (IsoPresentation_parseConnect(self->presentation, sessionUserData)) {",
        """                /* Native profile limit applies to the complete CONNECT
                 * presentation payload, including selectors/contexts and
                 * headers, rather than only the smaller inner AARQ. */
                if (ccli_e2e_handler && ccli_e2e_owner ==
                    IsoServer_getAuthenticatorParameter(self->isoServer) &&
                    (sessionUserData->size < 0 || sessionUserData->size > 10240)) {
                    self->state = ISO_CON_STATE_STOPPED;
                    break;
                }
                if (IsoPresentation_parseConnect(self->presentation, sessionUserData)) {""")
    connection = replace_once(connection,
        "                    AcseIndication aIndication = AcseConnection_parseMessage(self->acseConnection, acseBuffer);",
        """                    IsoConnection_lock(self);
                    int nativeHandled = ccliE2eHandle(self, CCLI_E2E_ASSOCIATE, acseBuffer);
                    IsoConnection_unlock(self);
                    if (nativeHandled) break;
                    if (self->presentation->ccliE2eContextId) {
                        self->state = ISO_CON_STATE_STOPPED;
                        break;
                    }
                    AcseIndication aIndication = AcseConnection_parseMessage(self->acseConnection, acseBuffer);""")
    connection = replace_once(connection,
        "                if (!IsoPresentation_parseUserData(self->presentation, sessionUserData)) {",
        """                if (self->ccliE2eSession) {
                    IsoConnection_lock(self);
                    ccliE2eHandle(self, CCLI_E2E_PRESENTATION, sessionUserData);
                    IsoConnection_unlock(self);
                    break;
                }
                if (!IsoPresentation_parseUserData(self->presentation, sessionUserData)) {""")
    connection = replace_once(connection,
        '            case SESSION_FINISH:\n',
        """            case SESSION_FINISH:
                if (self->ccliE2eSession) {
                    IsoConnection_lock(self);
                    ccliE2eHandle(self, CCLI_E2E_PRESENTATION_RELEASE, sessionUserData);
                    IsoConnection_unlock(self);
                    self->state = ISO_CON_STATE_STOPPED;
                    break;
                }
""")
    connection = replace_once(connection, "            case SESSION_ABORT:\n",
        """            case SESSION_ABORT:
                if (self->ccliE2eSession) {
                    IsoConnection_lock(self);
                    ccliE2eHandle(self, CCLI_E2E_PRESENTATION_ABORT, sessionUserData);
                    IsoConnection_unlock(self);
                }
""")
    connection = replace_once(connection, "    bool success = false;",
        """    bool success = false;
    if (self->ccliE2eSession) {
        if (self->state == ISO_CON_STATE_STOPPED) return false;
        return ccliE2eHandle(self, CCLI_E2E_SEND, message) > 0 &&
            self->state != ISO_CON_STATE_STOPPED;
    }""")
    presentation = (source / "iso_presentation.c").read_text()
    presentation = replace_once(presentation, "    bool isAuth = false;",
        """    bool isAuth = false;
    bool isE2e = false;
    bool ccliBer = false;""")
    presentation = replace_once(presentation, "        case 0x06: /* abstract-syntax-name */",
        """        case 0x06: /* abstract-syntax-name */
            if (len == 8 && memcmp(buffer + bufPos, "\\x28\\x83\\xe7\\x0f\\x04\\x01\\x02\\x01", 8) == 0)
                isE2e = true;""")
    presentation = replace_once(presentation, "        case 0x30: /* transfer-syntax-name */",
        """        case 0x30: /* transfer-syntax-name */
            if (len == 4 && memcmp(buffer + bufPos, "\\x06\\x02\\x51\\x01", 4) == 0)
                ccliBer = true;""")
    presentation = replace_once(presentation,
        "    if (++self->ccliContextCount > 3 || contextId > 127) return -1;\n    if (isAuth) return bufPos;",
        """    if (++self->ccliContextCount > 3 || contextId <= 0 || contextId > 127 ||
        !(contextId & 1) || self->ccliContextSeen[contextId]) return -1;
    self->ccliContextSeen[contextId] = true;
    if ((int)isAcse + (int)isMms + (int)isAuth + (int)isE2e != 1) return -1;
    if (isE2e) {
        if (!ccliBer || self->ccliE2eContextId) return -1;
        self->ccliE2eContextId = contextId;
        return bufPos;
    }
    if (isAuth) return bufPos;""")
    presentation = replace_once(presentation, "    self->ccliContextCount = 0;",
        "    self->ccliContextCount = 0;\n    self->ccliE2eContextId = 0;\n"
        "    memset(self->ccliContextSeen, 0, sizeof(self->ccliContextSeen));")
    presentation_header = (source / "iso_presentation.h").read_text()
    presentation_header = replace_once(presentation_header, "    int ccliContextCount;",
        "    int ccliContextCount;\n    int ccliE2eContextId;\n    bool ccliContextSeen[128];")
    session = (source / "iso_session.c").read_text()
    for name, code in [("IsoSession_createAbortSpdu", 25), ("IsoSession_createFinishSpdu", 9),
                       ("IsoSession_createDisconnectSpdu", 10)]:
        session = replace_function(session, name, control_spdu(name, code))
    session = replace_once(session, "    case 25: /* ABORT SPDU */\n        return SESSION_ABORT;",
        """    case 25: /* ABORT SPDU */
        ByteBuffer_wrap(&self->userData, NULL, 0, 0);
        if (length != message->size - headerSize) return SESSION_ERROR;
        if (parseSessionHeaderParameters(self, message, length) != SESSION_OK) return SESSION_ERROR;
        return SESSION_ABORT;""")
    # Legacy ACSE must never accept an unhandled native application context.
    acse = (source / "acse.c").read_text()
    start = acse.index("parseAarqPdu(")
    before, aarq = acse[:start], acse[start:]
    aarq = replace_once(aarq, "        case 0xa1: /* application context name */\n            bufPos += len;",
        """        case 0xa1: /* application context name */
            if (len != 7 || buffer[bufPos] != 0x06 || buffer[bufPos + 1] != 5 ||
                memcmp(buffer + bufPos + 2, appContextNameMms, 5) != 0)
                return ACSE_ASSOCIATE_FAILED;
            bufPos += len;""")
    server_path = vendor / "src/mms/iso_server/iso_server.c"
    association_path = vendor / "src/mms/iso_mms/server/mms_association_service.c"
    server = server_path.read_text()
    association = association_path.read_text()
    for name, content, expected in [
        ("iso_server.c", server, "fdd4b5c2e209a0487a7b15731544ff9ca57dd3da8ef19a8bd8d8c815fa4c4c74"),
        ("mms_association_service.c", association, "cd2924e974f09c1bb053b803d7f6e0b914fff31892f4b85d094e35c11177a31e")]:
        if hashlib.sha256(content.encode()).hexdigest() != expected:
            raise RuntimeError("Native E2E reviewed vendor source changed: " + name)
    server = replace_once(server,
        "        callTickHandlerForClientConnections(self);\n",
        "        callTickHandlerForClientConnections(self);\n"
        "        /* Reap timer-stopped connections even without new socket input. */\n"
        "        removeTerminatedConnections(self, true);\n")
    association = replace_once(association, '#include "mms_server_internal.h"',
        '#include "mms_server_internal.h"\n#include "e2e_osi_bridge.h"')
    association = replace_once(association, "\n    self->maxPduSize = CONFIG_MMS_MAXIMUM_PDU_SIZE;",
        "\n    const int ccliMaximumPdu = CcliE2e_currentMaximumPdu();\n"
        "    self->maxPduSize = ccliMaximumPdu;")
    association = replace_once(association,
        "            if (self->maxPduSize > CONFIG_MMS_MAXIMUM_PDU_SIZE)\n"
        "                self->maxPduSize = CONFIG_MMS_MAXIMUM_PDU_SIZE;",
        "            if (self->maxPduSize > ccliMaximumPdu)\n"
        "                self->maxPduSize = ccliMaximumPdu;")
    output.mkdir(parents=True, exist_ok=True)
    for name, content in [("iso_connection.c", connection), ("iso_presentation.c", presentation),
                          ("iso_presentation.h", presentation_header), ("acse.c", before + aarq),
                          ("iso_session.c", session),
                          ("iso_server.c", server), ("mms_association_service.c", association),
                          ("iso_client_connection.c", (source / "iso_client_connection.c").read_text()),
                          ("e2e_osi_bridge.h", header.read_text())]:
        path = output / name
        if not path.exists() or path.read_text() != content:
            path.write_text(content)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--header", required=True, type=Path)
    parser.add_argument("--vendor", required=True, type=Path)
    args = parser.parse_args()
    generate(args.input, args.output, args.header, args.vendor)
