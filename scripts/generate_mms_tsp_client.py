#!/usr/bin/env python3
"""Client-only overlay matching the observed TSP association, without vendor edits."""
import argparse
from pathlib import Path
from generate_mms_certificate_auth import replace_once, function


PRESENTATION = r'''static void
createConnectPdu(IsoPresentation* self, BufferChain writeBuffer, BufferChain payload)
{
    /* TSP: MMS 3, ACSE 1, authentication 5; only calling P-selector. */
    int contextsLength = 35 + (self->ccliSendAuthContext ? 13 + sizeof(ccli_auth_as) : 0);
    int selectorLength = self->callingPresentationSelector.size;
    int normalLength = 2 + selectorLength + 2 + contextsLength
        + encodeUserData(NULL, 0, payload, false, self->acseContextId);
    int contentLength = 5 + 1 + BerEncoder_determineLengthSize(normalLength) + normalLength;
    int headerLength = 1 + BerEncoder_determineLengthSize(contentLength) + contentLength - payload->length;
    if (headerLength > writeBuffer->partMaxLength) {
        writeBuffer->length = writeBuffer->partLength = 0; writeBuffer->nextPart = NULL; return;
    }
    uint8_t* buffer = writeBuffer->buffer;
    int pos = BerEncoder_encodeTL(0x31, contentLength, buffer, 0);
    pos = BerEncoder_encodeTL(0xa0, 3, buffer, pos);
    pos = BerEncoder_encodeTL(0x80, 1, buffer, pos); buffer[pos++] = 1;
    pos = BerEncoder_encodeTL(0xa2, normalLength, buffer, pos);
    pos = BerEncoder_encodeTL(0x81, selectorLength, buffer, pos);
    memcpy(buffer + pos, self->callingPresentationSelector.value, selectorLength); pos += selectorLength;
    pos = BerEncoder_encodeTL(0xa4, contextsLength, buffer, pos);
    for (int index = 0; index < (self->ccliSendAuthContext ? 3 : 2); ++index) {
        const uint8_t* syntax = index == 0 ? asn_id_mms : index == 1 ? asn_id_as_acse : ccli_auth_as;
        int syntaxLength = index == 0 ? sizeof(asn_id_mms) : index == 1 ? sizeof(asn_id_as_acse) : sizeof(ccli_auth_as);
        pos = BerEncoder_encodeTL(0x30, 11 + syntaxLength, buffer, pos);
        pos = BerEncoder_encodeTL(0x02, 1, buffer, pos); buffer[pos++] = index == 0 ? 3 : index == 1 ? 1 : 5;
        pos = BerEncoder_encodeTL(0x06, syntaxLength, buffer, pos);
        memcpy(buffer + pos, syntax, syntaxLength); pos += syntaxLength;
        pos = BerEncoder_encodeTL(0x30, 4, buffer, pos);
        pos = BerEncoder_encodeTL(0x06, 2, buffer, pos);
        memcpy(buffer + pos, ber_id, 2); pos += 2;
    }
    pos = encodeUserData(buffer, pos, payload, true, self->acseContextId);
    writeBuffer->partLength = pos;
    writeBuffer->length = pos + payload->length;
    writeBuffer->nextPart = payload;
}'''


def generate(vendor, output):
    presentation = (output / 'iso_presentation.c').read_text()
    presentation = function(presentation, 'createConnectPdu', PRESENTATION)
    (output / 'iso_presentation.c').write_text(presentation)
    client = (output / 'iso_client_connection.c').read_text()
    client = replace_once(client, '                socketExtensionBuffer, socketExtensionBufferSize);',
        '                socketExtensionBuffer, socketExtensionBufferSize);\n'
        '        self->cotpConnection->localRef = 4;\n'
        '        CotpConnection_setTpduSize(self->cotpConnection, 1024);')
    start = client.index('    case INT_STATE_WAIT_FOR_ACSE_RESP:')
    end = client.index('    case INT_STATE_WAIT_FOR_DATA_MSG:', start)
    response = client[start:end]
    response = replace_once(response, '                    if (cotpIndication != COTP_DATA_INDICATION)',
        '''                    if (cotpIndication == COTP_MORE_FRAGMENTS_FOLLOW) {
                        /* Keep the accumulated payload until EOT, as TSP does. */
                        waits = true;
                    }
                    else if (cotpIndication != COTP_DATA_INDICATION)''')
    client = client[:start] + response + client[end:]
    (output / 'iso_client_connection.c').write_text(client)

    mms_client = (vendor / 'src/mms/iso_mms/client/mms_client_connection.c').read_text()
    mms_client = replace_once(mms_client, 'static MmsError\nconvertRejectCodesToMmsError', '''static bool
ccliWriteRequestEncodingSucceeded(MmsConnection self, ByteBuffer* payload,
        int encodedLength, MmsError* mmsError)
{
    if (payload != NULL && encodedLength > 0 &&
        encodedLength == ByteBuffer_getSize(payload))
        return true;

    if (payload != NULL)
        IsoClientConnection_releaseTransmitBuffer(self->isoClient);

    if (mmsError)
        *mmsError = MMS_ERROR_RESOURCE_OTHER;

    return false;
}

static MmsError
convertRejectCodesToMmsError''')
    write_calls = [
        ('mmsClient_createWriteRequest(invokeId, domainId, itemId, value, payload);',
         'mmsClient_createWriteRequest(invokeId, domainId, itemId, value, payload)'),
        ('mmsClient_createWriteRequestAlternateAccessSingleIndexComponent(invokeId, domainId, itemId, arrayIndex,\n'
         '            componentId, value, payload);',
         'mmsClient_createWriteRequestAlternateAccessSingleIndexComponent(invokeId, domainId, itemId, arrayIndex,\n'
         '            componentId, value, payload)'),
        ('mmsClient_createWriteRequestComponent(invokeId, domainId, itemId, componentId, value, payload);',
         'mmsClient_createWriteRequestComponent(invokeId, domainId, itemId, componentId, value, payload)'),
        ('mmsClient_createWriteRequestArray(invokeId, domainId, itemId, index, numberOfElements, value, payload);',
         'mmsClient_createWriteRequestArray(invokeId, domainId, itemId, index, numberOfElements, value, payload)'),
        ('mmsClient_createWriteRequestNamedVariableList(invokeId, isAssociationSpecific, domainId, itemId, values, payload);',
         'mmsClient_createWriteRequestNamedVariableList(invokeId, isAssociationSpecific, domainId, itemId, values, payload)'),
    ]
    for old_call, encoder_call in write_calls:
        replacement = ('int encodedLength = ' + encoder_call + ';\n'
            '    if (!ccliWriteRequestEncodingSucceeded(self, payload, encodedLength, mmsError))\n'
            '        goto exit_function;')
        mms_client = replace_once(mms_client, old_call, replacement)

    mms_client = replace_once(mms_client, '''    if (mmsClient_createWriteMultipleItemsRequest(invokeId, domainId, items, values, payload) != -1)
    {
        MmsClientInternalParameter intParam;
        intParam.ptr = NULL;

        MmsError err = sendAsyncRequest(self, invokeId, payload, MMS_CALL_TYPE_WRITE_MULTIPLE_VARIABLES, handler, parameter, intParam);

        if (mmsError)
            *mmsError = err;
    }
    else
    {
        *mmsError = MMS_ERROR_RESOURCE_OTHER;
    }''', '''    int encodedLength = mmsClient_createWriteMultipleItemsRequest(invokeId, domainId, items, values, payload);
    if (!ccliWriteRequestEncodingSucceeded(self, payload, encodedLength, mmsError))
        goto exit_function;

    MmsClientInternalParameter intParam;
    intParam.ptr = NULL;

    MmsError err = sendAsyncRequest(self, invokeId, payload, MMS_CALL_TYPE_WRITE_MULTIPLE_VARIABLES, handler, parameter, intParam);

    if (mmsError)
        *mmsError = err;''')
    (output / 'mms_client_connection.c').write_text(mms_client)

    acse = (output / 'acse.c').read_text()
    start = acse.index('void\nAcseConnection_createAssociateRequestMessage')
    end = acse.index('\nvoid\nAcseConnection_createAbortMessage', start)
    request = acse[start:end]
    request = replace_once(request, '    int contentLength = 0;', '    int contentLength = 10; /* calling AP/AE invocation IDs */')
    request = replace_once(request, '    /* association data */\n    int assocDataLength',
        '    userInfoLength += 4; /* explicit BER direct-reference, as sent by TSP */\n\n    /* association data */\n    int assocDataLength')
    marker = '    if (authParameter != NULL)\n    {\n        /* sender requirements */'
    request = replace_once(request, marker, '''    bufPos = BerEncoder_encodeTL(0xa8, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos); buffer[bufPos++] = 10;
    bufPos = BerEncoder_encodeTL(0xa9, 3, buffer, bufPos);
    bufPos = BerEncoder_encodeTL(0x02, 1, buffer, bufPos); buffer[bufPos++] = 10;

''' + marker)
    request = replace_once(request, '        buffer[bufPos++] = 0x04;', '        buffer[bufPos++] = 0x07; /* one significant requirement bit */')
    request = replace_once(request, '    /* indirect-reference */', '''    bufPos = BerEncoder_encodeTL(0x06, 2, buffer, bufPos);
    buffer[bufPos++] = 0x51; buffer[bufPos++] = 0x01; /* BER */

    /* indirect-reference */''')
    acse = acse[:start] + request + acse[end:]
    acse = replace_once(acse, 'static struct sAcseAuthenticationParameter ccli_certificate_marker', '''static int (*ccli_client_aare_verifier)(const uint8_t*, int);
void CcliDualCert_setClientAareVerifier(int (*verifier)(const uint8_t*, int)) {
    ccli_client_aare_verifier = verifier;
}
static struct sAcseAuthenticationParameter ccli_certificate_marker''')
    aare_start = acse.index('parseAarePdu(AcseConnection* self,')
    aare_end = acse.index('static AcseIndication\nparseAarqPdu', aare_start)
    aare = acse[aare_start:aare_end]
    aare = replace_once(aare, '    bool userInfoValid = false;', '''    if (ccli_client_aare_verifier &&
        !ccli_client_aare_verifier(buffer + bufPos, maxBufPos - bufPos)) return ACSE_ASSOCIATE_FAILED;
    bool userInfoValid = false;''')
    acse = acse[:aare_start] + aare + acse[aare_end:]
    (output / 'acse.c').write_text(acse)
    initiate = (vendor / 'src/mms/iso_mms/client/mms_client_initiate.c').read_text()
    initiate = replace_once(initiate, '0x04, 0x08, 0x00', '0x04, 0x02, 0x00')
    initiate = replace_once(initiate, 'int dataStructureNestingLevel = DEFAULT_DATA_STRUCTURE_NESTING_LEVEL;',
                            'int dataStructureNestingLevel = 5; /* TSP capture */')
    (output / 'mms_client_initiate.c').write_text(initiate)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--vendor', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    generate(args.vendor, args.output)
