/* Read-only deployment probe. Credentials are read from existing local files. */
#include "iec61850_client.h"
#include "tls_config.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    if (argc != 7) {
        fprintf(stderr, "usage: probe HOST PORT CA CERT KEY OBJECT\n");
        return 2;
    }
    TLSConfiguration tls = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tls, true);
    if (!TLSConfiguration_addCACertificateFromFile(tls, argv[3]) ||
        !TLSConfiguration_setOwnCertificateFromFile(tls, argv[4]) ||
        !TLSConfiguration_setOwnKeyFromFile(tls, argv[5], NULL)) {
        fprintf(stderr, "credential loading failed\n"); return 1;
    }
    IedConnection connection = IedConnection_createWithTlsSupport(tls);
    IedConnection_setConnectTimeout(connection, 5000);
    IedConnection_setRequestTimeout(connection, 5000);
    IedClientError error;
    IedConnection_connect(connection, &error, argv[1], atoi(argv[2]));
    int result = 1;
    if (error == IED_ERROR_OK) {
        MmsValue* value = IedConnection_readObject(connection, &error, argv[6], IEC61850_FC_MX);
        if (error == IED_ERROR_OK && value && MmsValue_getType(value) != MMS_DATA_ACCESS_ERROR) {
            puts("PASS: mutual TLS 1.2, MMS association and measurement read");
            result = 0;
        } else fprintf(stderr, "MMS read failed: client=%d access=%d\n", error,
            value && MmsValue_getType(value) == MMS_DATA_ACCESS_ERROR ? MmsValue_getDataAccessError(value) : -1);
        if (value) MmsValue_delete(value);
    } else fprintf(stderr, "MMS connection failed: %d\n", error);
    IedConnection_close(connection);
    IedConnection_destroy(connection);
    TLSConfiguration_destroy(tls);
    return result;
}
