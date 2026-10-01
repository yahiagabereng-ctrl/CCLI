/*
 * CCLI Phase 3 P3-07 — negative: TLS without client certificate must fail.
 * Usage: mms_tls_noclient 192.168.10.1 3782 /path/to/certs
 * Expects only root_CA.pem in cert_dir (server auth of peer); no client key/cert.
 */
#include "iec61850_client.h"
#include "tls_config.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const char* host = "192.168.10.1";
    int port = 3782;
    const char* cert_dir = ".";
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }
    if (argc > 3) {
        cert_dir = argv[3];
    }

    char ca_path[512];
    snprintf(ca_path, sizeof(ca_path), "%s/root_CA.pem", cert_dir);

    printf("=== CCLI MMS TLS negative (no client cert) P3-07 ===\n");
    printf("host=%s port=%d ca=%s\n", host, port, ca_path);

    TLSConfiguration tlsConfig = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tlsConfig, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tlsConfig, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tlsConfig, false);

    if (!TLSConfiguration_addCACertificateFromFile(tlsConfig, ca_path)) {
        printf("FAIL load CA\n");
        TLSConfiguration_destroy(tlsConfig);
        return 1;
    }

    IedClientError error;
    IedConnection con = IedConnection_createWithTlsSupport(tlsConfig);
    IedConnection_connect(con, &error, host, port);
    if (error == IED_ERROR_OK) {
        printf("UNEXPECTED CONNECT OK — mutual cert not enforced\n");
        IedConnection_close(con);
        IedConnection_destroy(con);
        TLSConfiguration_destroy(tlsConfig);
        return 2;
    }
    printf("CONNECT FAIL err=%d (expected — no client cert)\n", error);
    IedConnection_destroy(con);
    TLSConfiguration_destroy(tlsConfig);
    printf("VERDICT: PASS — association rejected without client certificate\n");
    return 0;
}
