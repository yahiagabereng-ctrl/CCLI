#!/usr/bin/env python3
"""Generate reviewed ACSE/session extensions; never modify vendored wire sources."""
from pathlib import Path
import argparse
import hashlib
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def matches_reviewed_text(path, expected):
    # Git may check out text as LF or CRLF; code changes must still fail.
    data = path.read_bytes()
    lf = data.replace(b'\r\n', b'\n')
    return expected in {hashlib.sha256(value).hexdigest()
                        for value in (data, lf, lf.replace(b'\n', b'\r\n'))}


def replace_once(source, old, new):
    if source.count(old) != 1:
        raise RuntimeError('Review vendor anchor: ' + old[:100])
    return source.replace(old, new)

def function(source, name, replacement):
    found = re.search(r'(?:static\s+)?(?:[\w*]+\s+)+\n?' + name + r'\s*\([^;]*?\)\s*\{', source)
    if not found: raise RuntimeError('Missing function ' + name)
    depth, end = 1, found.end()
    # These vendor functions contain no braces in strings/comments that affect balance.
    while depth:
        if source[end] == '{': depth += 1
        elif source[end] == '}': depth -= 1
        end += 1
    return source[:found.start()] + replacement + source[end:]

ACSE_SUPPORT = r'''
/* CCLI: certificate authentication is explicit; unknown mechanisms never fall back. */
static const uint8_t ccli_certificate_oid[] = {0x28,0x86,0x48,0x00,0x01,0x00,0x01,0x01};
static struct sAcseAuthenticationParameter ccli_certificate_marker = {.mechanism = ACSE_AUTH_CERTIFICATE};
static int (*ccli_aare_builder)(uint8_t*, int);
static uint8_t ccli_responding_ap[10];
static int ccli_responding_ap_len;
static int ccli_responding_ae;

void CcliDualCert_setAareAuthBuilder(int (*builder)(uint8_t*, int)) { ccli_aare_builder = builder; }
void CcliDualCert_setResponderAddress(const char* ap, int ae) {
    IsoConnectionParameters parameters = IsoConnectionParameters_create();
    IsoConnectionParameters_setLocalApTitle(parameters, ap, ae);
    ccli_responding_ap_len = parameters->localApTitleLen;
    memcpy(ccli_responding_ap, parameters->localApTitle, ccli_responding_ap_len);
    ccli_responding_ae = ae;
    IsoConnectionParameters_destroy(parameters);
}
'''

CHECK_AUTH = r'''static bool
checkAuthentication(AcseConnection* self, uint8_t* authMechanism, int authMechLen, uint8_t* authValue, int authValueLen)
{
    self->securityToken = NULL;
    self->authentication = NULL;
    if (!self->authenticator) return true;
    AcseAuthenticationMechanism mechanism = checkAuthMechanismName(authMechanism, authMechLen);
    if (authMechanism && mechanism == ACSE_AUTH_NONE) return false;
    if (mechanism == ACSE_AUTH_CERTIFICATE) {
        if (!authValue || authValueLen <= 0 || authValueLen > 16384) return false;
        self->authentication = &ccli_certificate_marker;
    }
    if (mechanism == ACSE_AUTH_NONE) {
        if (authValueLen) return false;
#if (CONFIG_MMS_SUPPORT_TLS == 1)
        if (self->tlsSocket) {
            authValue = TLSSocket_getPeerCertificate(self->tlsSocket, &authValueLen);
            if (authValue) mechanism = ACSE_AUTH_TLS;
        }
#endif
    }
    return authenticateClient(self, mechanism, authValue, authValueLen);
}'''

AARE = r'''void
AcseConnection_createAssociateResponseMessage(AcseConnection* self, uint8_t acseResult,
        BufferChain writeBuffer, BufferChain payload)
{
    assert(self && writeBuffer && payload);
    /* Retain the original response byte-for-byte unless the request used ACSE certificates. */
    uint8_t auth[8192];
    int authLen = 0;
    bool certificateRequest = self->authentication && self->authentication->mechanism == ACSE_AUTH_CERTIFICATE;
    if (certificateRequest && acseResult == ACSE_RESULT_ACCEPT) {
        authLen = ccli_aare_builder ? ccli_aare_builder(auth, sizeof(auth)) : -1;
        if (authLen <= 0 || authLen > (int)sizeof(auth)) {
            writeBuffer->length = writeBuffer->partLength = 0;
            writeBuffer->nextPart = NULL;
            return;
        }
    }
    int payloadLength = payload->length;
    int nextRefLength = BerEncoder_UInt32determineEncodedSize(self->nextReference);
    int assocDataLength = 1 + BerEncoder_determineLengthSize(payloadLength) + payloadLength + nextRefLength + 2;
    int userInfoLength = 1 + BerEncoder_determineLengthSize(assocDataLength) + assocDataLength;
    int contentLength = 21 + 1 + BerEncoder_determineLengthSize(userInfoLength) + userInfoLength;
    int aeLength = BerEncoder_UInt32determineEncodedSize(ccli_responding_ae);
    if (authLen) contentLength += 4 + 2 + sizeof(ccli_certificate_oid) + 1 + BerEncoder_determineLengthSize(authLen) + authLen
        + (ccli_responding_ap_len ? 4 + ccli_responding_ap_len + 4 + aeLength : 0);
    int headerLength = 1 + BerEncoder_determineLengthSize(contentLength) + contentLength - payloadLength;
    if (headerLength > writeBuffer->partMaxLength) {
        writeBuffer->length = writeBuffer->partLength = 0; writeBuffer->nextPart = NULL; return;
    }
    uint8_t* buffer = writeBuffer->buffer;
    int pos = BerEncoder_encodeTL(0x61, contentLength, buffer, 0);
    pos = BerEncoder_encodeTL(0xa1, 7, buffer, pos);
    pos = BerEncoder_encodeTL(0x06, 5, buffer, pos);
    memcpy(buffer + pos, appContextNameMms, 5); pos += 5;
    pos = BerEncoder_encodeTL(0xa2, 3, buffer, pos);
    pos = BerEncoder_encodeTL(0x02, 1, buffer, pos); buffer[pos++] = acseResult;
    pos = BerEncoder_encodeTL(0xa3, 5, buffer, pos);
    pos = BerEncoder_encodeTL(0xa1, 3, buffer, pos);
    pos = BerEncoder_encodeTL(0x02, 1, buffer, pos); buffer[pos++] = 0;
    if (authLen) {
        if (ccli_responding_ap_len) {
            pos = BerEncoder_encodeTL(0xa4, ccli_responding_ap_len + 2, buffer, pos);
            pos = BerEncoder_encodeTL(0x06, ccli_responding_ap_len, buffer, pos);
            memcpy(buffer + pos, ccli_responding_ap, ccli_responding_ap_len); pos += ccli_responding_ap_len;
            pos = BerEncoder_encodeTL(0xa5, aeLength + 2, buffer, pos);
            pos = BerEncoder_encodeTL(0x02, aeLength, buffer, pos);
            pos = BerEncoder_encodeUInt32(ccli_responding_ae, buffer, pos);
        }
        pos = BerEncoder_encodeTL(0x88, 2, buffer, pos); buffer[pos++] = 4; buffer[pos++] = 0x80;
        pos = BerEncoder_encodeTL(0x89, sizeof(ccli_certificate_oid), buffer, pos);
        memcpy(buffer + pos, ccli_certificate_oid, sizeof(ccli_certificate_oid)); pos += sizeof(ccli_certificate_oid);
        pos = BerEncoder_encodeTL(0xaa, authLen, buffer, pos); memcpy(buffer + pos, auth, authLen); pos += authLen;
    }
    pos = BerEncoder_encodeTL(0xbe, userInfoLength, buffer, pos);
    pos = BerEncoder_encodeTL(0x28, assocDataLength, buffer, pos);
    pos = BerEncoder_encodeTL(0x02, nextRefLength, buffer, pos);
    pos = BerEncoder_encodeUInt32(self->nextReference, buffer, pos);
    pos = BerEncoder_encodeTL(0xa0, payloadLength, buffer, pos);
    writeBuffer->partLength = pos; writeBuffer->length = pos + payloadLength; writeBuffer->nextPart = payload;
}'''

SESSION_HELPERS = r'''
/* ISO 8327 length indicator: one octet through 254, otherwise ff + uint16. */
static int ccli_session_length(uint8_t* out, int pos, int size) {
    if (size < 255) out[pos++] = size;
    else { out[pos++] = 255; out[pos++] = size >> 8; out[pos++] = size & 255; }
    return pos;
}
static int ccli_session_read_length(const uint8_t* in, int end, int* pos) {
    if (*pos >= end) return -1;
    int size = in[(*pos)++];
    if (size == 255) {
        if (end - *pos < 2) return -1;
        size = (in[*pos] << 8) | in[*pos + 1]; *pos += 2;
    }
    return size;
}
'''

SESSION_PARAMETERS = r'''static IsoSessionIndication
parseSessionHeaderParameters(IsoSession* self, ByteBuffer* message, int parametersOctets)
{
    int pos = 1;
    int length = ccli_session_read_length(message->buffer, message->size, &pos);
    if (length != parametersOctets || length != message->size - pos) return SESSION_ERROR;
    const int end = message->size;
    while (pos < end) {
        int tag = message->buffer[pos++];
        int size = ccli_session_read_length(message->buffer, end, &pos);
        if (size < 0 || size > end - pos) return SESSION_ERROR;
        if (tag == 193 || tag == 194) {
            if (pos + size != end) return SESSION_ERROR;
            ByteBuffer_wrap(&self->userData, message->buffer + pos, size, message->maxSize - pos);
            return SESSION_OK;
        }
        if (tag == 5 && parseAcceptParameters(self, message, pos, size) != size) return SESSION_ERROR;
        if (tag == 20) {
            if (size != 2) return SESSION_ERROR;
            self->sessionRequirement = (message->buffer[pos] << 8) | message->buffer[pos+1];
        }
        if (tag == 51 || tag == 52) {
            if (size > 16) return SESSION_ERROR;
            if (tag == 51) { self->callingSessionSelector.size = size; memcpy(self->callingSessionSelector.value, message->buffer+pos, size); }
            else { self->calledSessionSelector.size = size; memcpy(self->calledSessionSelector.value, message->buffer+pos, size); }
        }
        pos += size;
    }
    return SESSION_ERROR;
}'''

def session_connect(accept=False):
    name = 'IsoSession_createAcceptSpdu' if accept else 'IsoSession_createConnectSpdu'
    params = '' if accept else 'IsoConnectionParameters isoParameters, '
    return f'''void
{name}(IsoSession* self, {params}BufferChain buffer, BufferChain payload)
{{
    uint8_t parameters[128];
    int size = encodeConnectAcceptItem(parameters, 0, self->protocolOptions);
    size = encodeSessionRequirement(self, parameters, size);
    {'' if accept else 'self->callingSessionSelector = isoParameters->localSSelector; self->calledSessionSelector = isoParameters->remoteSSelector;'}
    {'' if accept else 'size = encodeCallingSessionSelector(self, parameters, size);'}
    size = encodeCalledSessionSelector(self, parameters, size);
    int dataLengthSize = payload->length < 255 ? 1 : 3;
    int total = size + 1 + dataLengthSize + payload->length;
    int needed = 1 + (total < 255 ? 1 : 3) + size + 1 + dataLengthSize;
    if (payload->length < 0 || total > 65535 || needed > buffer->partMaxLength) {{
        buffer->length = buffer->partLength = 0; buffer->nextPart = NULL; return;
    }}
    if (total >= 255) fprintf(stderr, "mms: iso session {"ACCEPT" if accept else "CONNECT"} extended LI (spdu=%d payload=%d)\\n", total, payload->length);
    uint8_t* out = buffer->buffer;
    out[0] = {14 if accept else 13};
    int pos = ccli_session_length(out, 1, total);
    memcpy(out+pos, parameters, size); pos += size;
    out[pos++] = payload->length < 255 ? 193 : 194;
    pos = ccli_session_length(out, pos, payload->length);
    buffer->partLength = pos; buffer->length = pos + payload->length; buffer->nextPart = payload;
}}'''

def generate(vendor, output, baseline_path):
    baseline = json.loads(baseline_path.read_text())['protected_source_sha256']
    sources = {}
    for filename, subdir in [('acse.c', 'iso_acse'), ('iso_session.c', 'iso_session'), ('iso_presentation.c', 'iso_presentation')]:
        path = vendor / 'src/mms' / subdir / filename
        expected = baseline['apps/ccli/third_party/libiec61850/src/mms/' + subdir + '/' + filename]
        if not matches_reviewed_text(path, expected):
            raise RuntimeError('Review changed vendor source: ' + str(path))
        sources[filename] = path.read_text()
    client = (vendor / 'src/mms/iso_client/iso_client_connection.c').read_text()
    client = replace_once(client, '''    IsoSession_init(self->session);
    IsoSession_createConnectSpdu''', '''    sessionBuffer->partMaxLength = ISO_CLIENT_BUFFER_SIZE - presentationBuffer->length;
    IsoSession_init(self->session);
    IsoSession_createConnectSpdu''')
    acse = sources['acse.c']
    acse = replace_once(acse, 'static uint8_t requirements_authentication[] = { 0x80 };',
                        'static uint8_t requirements_authentication[] = { 0x80 };\n' + ACSE_SUPPORT)
    acse = replace_once(acse, '    return authenticationMechanism;', '''    if (authMechanism && authMechLen == sizeof(ccli_certificate_oid) &&
        memcmp(authMechanism, ccli_certificate_oid, sizeof(ccli_certificate_oid)) == 0)
        authenticationMechanism = ACSE_AUTH_CERTIFICATE;
    return authenticationMechanism;''')
    acse = replace_once(acse, 'else if (mechanism == ACSE_AUTH_TLS)',
                       'else if (mechanism == ACSE_AUTH_TLS || mechanism == ACSE_AUTH_CERTIFICATE)')
    acse = function(acse, 'checkAuthentication', CHECK_AUTH)
    acse = replace_once(acse, '    self->state = idle;', '    self->state = idle;\n    self->authentication = NULL;')
    start = acse.index('        case 0xac: /* authentication value */')
    end = acse.index('        case 0xbe:', start)
    acse = acse[:start] + '''        case 0xac: /* bounded Authentication-value CHOICE */
            if (authValue != NULL || len < 2) return ACSE_ASSOCIATE_FAILED;
            {
                int end = bufPos + len;
                if (buffer[bufPos] == 0x80) {
                    int size;
                    int inner = BerDecoder_decodeLength(buffer, &size, bufPos + 1, end);
                    if (inner < 0 || size != end - inner) return ACSE_ASSOCIATE_FAILED;
                    authValue = buffer + inner; authValueLen = size;
                } else {
                    authValue = buffer + bufPos; authValueLen = len;
                }
                bufPos = end;
            }
            break;

''' + acse[end:]
    acse = function(acse, 'AcseConnection_createAssociateResponseMessage', AARE)
    # The public certificate authentication parameter contains an already encoded
    # Authentication-value CHOICE. Its certificate/time/signature is made by caller.
    acse = replace_once(acse, '        contentLength += 5;\n\n        /* authentication value */',
        '        contentLength += authParameter->mechanism == ACSE_AUTH_CERTIFICATE ? 2 + sizeof(ccli_certificate_oid) : 5;\n\n        /* authentication value */')
    acse = replace_once(acse, '''        else
        {
            contentLength += 2;
        }
    }

    /* user information */''', '''        else if (authParameter->mechanism == ACSE_AUTH_CERTIFICATE) {
            int size = authParameter->value.certificate.length;
            if (size <= 0 || size > 16384) { writeBuffer->length = writeBuffer->partLength = 0; return; }
            contentLength += 1 + BerEncoder_determineLengthSize(size) + size;
        }
        else { contentLength += 2; }
    }

    /* user information */''')
    acse = replace_once(acse, '''        else
        { /* AUTH_NONE */''', '''        else if (authParameter->mechanism == ACSE_AUTH_CERTIFICATE) {
            buffer[bufPos++] = requirements_authentication[0];
            bufPos = BerEncoder_encodeTL(0x8b, sizeof(ccli_certificate_oid), buffer, bufPos);
            memcpy(buffer + bufPos, ccli_certificate_oid, sizeof(ccli_certificate_oid)); bufPos += sizeof(ccli_certificate_oid);
            int size = authParameter->value.certificate.length;
            bufPos = BerEncoder_encodeTL(0xac, size, buffer, bufPos);
            memcpy(buffer + bufPos, authParameter->value.certificate.buf, size); bufPos += size;
        }
        else
        { /* AUTH_NONE */''')
    connection_path = vendor / 'src/mms/iso_server/iso_connection.c'
    if not matches_reviewed_text(connection_path, 'cb1b83830619e20e312c8b1ebc77cb66c41f6ab15ca38cffd1cfac09a2673d9a'):
        raise RuntimeError('Review changed vendor connection source')
    connection = connection_path.read_text()
    connection = replace_once(connection, '            case SESSION_CONNECT:',
        '            case SESSION_CONNECT:\n                fprintf(stderr, "mms: iso session CONNECT (payload=%d)\\n", cotpPayload->size);')
    connection = replace_once(connection, '            case SESSION_ERROR:',
        '            case SESSION_ERROR:\n                fprintf(stderr, "mms: iso session error (payload=%d spdu=0x%02x)\\n", cotpPayload->size, cotpPayload->size ? cotpPayload->buffer[0] : 0);')
    session = sources['iso_session.c']
    session = replace_once(session, 'static int\nparseAcceptParameters', SESSION_HELPERS + '\nstatic int\nparseAcceptParameters')
    session = function(session, 'parseSessionHeaderParameters', SESSION_PARAMETERS)
    session = function(session, 'IsoSession_createConnectSpdu', session_connect())
    session = function(session, 'IsoSession_createAcceptSpdu', session_connect(True))
    session = replace_once(session, '    uint8_t length;\n', '    int length;\n    int headerSize = 2;\n')
    session = replace_once(session, '        length = buffer[1];', '''        int offset = 1;
        length = ccli_session_read_length(buffer, message->size, &offset);
        if (length < 0) return SESSION_ERROR;
        headerSize = offset;''')
    session = session.replace('length != (message->size - 2)', 'length != (message->size - headerSize)')
    presentation = sources['iso_presentation.c']
    presentation = replace_once(presentation, 'static uint8_t ber_id[]',
        '''/* Authentication abstract syntax observed in TSP context 5: 1.0.840.0.1.1.4.1. */
static const uint8_t ccli_auth_as[] = {0x28,0x86,0x48,0x00,0x01,0x01,0x04,0x01};
/* Existing commissioning clients used the extra-zero encoding before r63.
 * Accept only this explicit legacy alias; authentication/role checks still run. */
static const uint8_t ccli_legacy_auth_as[] = {0x28,0x86,0x48,0x00,0x00,0x01,0x01,0x04,0x01};

static uint8_t ber_id[]''')
    presentation = replace_once(presentation, '    bool isMms = false;', '    bool isMms = false;\n    bool isAuth = false;')
    presentation = replace_once(presentation, '            if (len == 5) {',
        '            if ((len == sizeof(ccli_auth_as) && memcmp(buffer + bufPos, ccli_auth_as, len) == 0) ||\n'
        '                (len == sizeof(ccli_legacy_auth_as) && memcmp(buffer + bufPos, ccli_legacy_auth_as, len) == 0)) isAuth = true;\n            if (len == 5) {')
    presentation = replace_once(presentation, '    if ((isAcse == false) && (isMms == false)) {',
        '    if (++self->ccliContextCount > 3 || contextId > 127) return -1;\n    if (isAuth) return bufPos;\n    if ((isAcse == false) && (isMms == false)) {')
    presentation = replace_once(presentation, '    int endPos = bufPos + totalLength;\n\n    while (bufPos < endPos)',
        '    int endPos = bufPos + totalLength;\n    self->ccliContextCount = 0;\n\n    while (bufPos < endPos)')
    presentation = replace_once(presentation, '''IsoPresentation_init(IsoPresentation* self)
{
    (void)self;
}''', '''IsoPresentation_init(IsoPresentation* self)
{
    memset(self, 0, sizeof(*self));
}''')
    presentation = replace_once(presentation, '    int pclLength = 27; /* 35; */',
        '    int authContextLength = self->ccliSendAuthContext ? 13 + (int)sizeof(ccli_auth_as) : 0;\n    int pclLength = 27 + authContextLength; /* 35; */')
    presentation = replace_once(presentation, '    bufPos = BerEncoder_encodeTL(0xa4, 35, buffer, bufPos);',
        '    bufPos = BerEncoder_encodeTL(0xa4, 35 + authContextLength, buffer, bufPos);')
    auth_context = '''    if (self->ccliSendAuthContext) {
        bufPos = BerEncoder_encodeTL(0x30, 11 + sizeof(ccli_auth_as), buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos); buffer[bufPos++] = 5;
        bufPos = BerEncoder_encodeTL(0x06, sizeof(ccli_auth_as), buffer, bufPos);
        memcpy(buffer + bufPos, ccli_auth_as, sizeof(ccli_auth_as)); bufPos += sizeof(ccli_auth_as);
        bufPos = BerEncoder_encodeTL(0x30, 4, buffer, bufPos);
        bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos); memcpy(buffer + bufPos, ber_id, 2); bufPos += 2;
    }
'''
    start = presentation.index('static void\ncreateConnectPdu')
    end = presentation.index('static int\nparseFullyEncodedData', start)
    part = presentation[start:end]
    part = replace_once(part, '    /* encode user data */', auth_context + '\n    /* encode user data */')
    presentation = presentation[:start] + part + presentation[end:]
    presentation = replace_once(presentation, '    self->acseContextId = 1;',
        '    self->ccliSendAuthContext = parameters->acseAuthParameter && parameters->acseAuthParameter->mechanism == ACSE_AUTH_CERTIFICATE;\n    self->acseContextId = 1;')
    presentation = replace_once(presentation, '    normalModeLength += 20; /* context-definition-result-list */',
        '    int contextCount = self->ccliContextCount ? self->ccliContextCount : 2;\n    normalModeLength += 2 + 9 * contextCount; /* context-definition-result-list */')
    presentation = replace_once(presentation, '''    bufPos = BerEncoder_encodeTL(0xa5, 18, buffer, bufPos);
    bufPos = encodeAcceptBer(buffer, bufPos); /* accept for acse */
    bufPos = encodeAcceptBer(buffer, bufPos); /* accept for mms */''',
        '''    bufPos = BerEncoder_encodeTL(0xa5, 9 * contextCount, buffer, bufPos);
    for (int context = 0; context < contextCount; ++context) bufPos = encodeAcceptBer(buffer, bufPos);''')
    header = (vendor / 'src/mms/inc_private/iso_presentation.h').read_text()
    header = replace_once(header, '\tByteBuffer nextPayload;', '\tByteBuffer nextPayload;\n    int ccliContextCount;\n    bool ccliSendAuthContext;')
    output.mkdir(parents=True, exist_ok=True)
    for name, content in [('acse.c', acse), ('iso_connection.c', connection), ('iso_session.c', session), ('iso_presentation.c', presentation), ('iso_presentation.h', header), ('iso_client_connection.c', client)]:
        path = output / name
        if not path.exists() or path.read_text() != content: path.write_text(content)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--vendor', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, default=ROOT / 'lab/conformance/CCI_RBAC_TLS_BASELINE.json')
    args = parser.parse_args()
    generate(args.vendor, args.output, args.baseline)
