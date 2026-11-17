/* Probe the generated presentation overlay independently of ACSE authentication. */
#include "iso_presentation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc != 3) return 2;
    unsigned char data[8192];
    IsoPresentation presentation;
    IsoPresentation_init(&presentation);
    if (!strcmp(argv[1], "parse")) {
        FILE* input = fopen(argv[2], "rb");
        if (!input) return 2;
        int size = (int)fread(data, 1, sizeof(data), input);
        fclose(input);
        ByteBuffer message;
        ByteBuffer_wrap(&message, data, size, sizeof(data));
        int accepted = IsoPresentation_parseConnect(&presentation, &message);
        printf("accepted=%d contexts=%d acse=%d mms=%d\n", accepted,
               presentation.ccliContextCount, presentation.acseContextId, presentation.mmsContextId);
        return accepted ? 0 : 1;
    }
    if (!strcmp(argv[1], "encode-cert") || !strcmp(argv[1], "encode-tls")) {
        struct sAcseAuthenticationParameter auth = {0};
        auth.mechanism = !strcmp(argv[1], "encode-cert") ? ACSE_AUTH_CERTIFICATE : ACSE_AUTH_TLS;
        struct sIsoConnectionParameters params = {0};
        params.acseAuthParameter = &auth;
        params.localPSelector.size = params.remotePSelector.size = 4;
        params.localPSelector.value[3] = params.remotePSelector.value[3] = 1;
        unsigned char payloadBytes[] = {0x60, 0x00};
        struct sBufferChain header = {0}, payload = {0};
        header.buffer = data;
        header.partMaxLength = sizeof(data);
        BufferChain_init(&payload, sizeof(payloadBytes), sizeof(payloadBytes), NULL, payloadBytes);
        IsoPresentation_createConnectPdu(&presentation, &params, &header, &payload);
        FILE* output = fopen(argv[2], "wb");
        if (!output) return 2;
        fwrite(header.buffer, 1, header.partLength, output);
        fwrite(payloadBytes, 1, sizeof(payloadBytes), output);
        fclose(output);
        return 0;
    }
    return 2;
}
