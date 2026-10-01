/*
 * CCLI Phase 3 P3-06 — DSO MMS client over TLS (IEC 62351-3 / Annex T T.3.3.4).
 *
 * Usage:
 *   mms_tls_client [host] [port] [cert_dir]
 *   mms_tls_client 192.168.10.1 3782 /path/to/certs
 *
 * Expects in cert_dir:
 *   client.key  client.pem  root_CA.pem
 */
#include "iec61850_client.h"
#include "tls_config.h"
#include "hal_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void join_path(char* out, size_t n, const char* dir, const char* file) {
    snprintf(out, n, "%s/%s", dir, file);
}

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

    char key_path[512];
    char cert_path[512];
    char ca_path[512];
    join_path(key_path, sizeof(key_path), cert_dir, "client.key");
    join_path(cert_path, sizeof(cert_path), cert_dir, "client.pem");
    join_path(ca_path, sizeof(ca_path), cert_dir, "root_CA.pem");

    printf("=== CCLI MMS TLS client (P3-06) ===\n");
    printf("host=%s port=%d cert_dir=%s\n", host, port, cert_dir);

    TLSConfiguration tlsConfig = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tlsConfig, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tlsConfig, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tlsConfig, false);

    if (!TLSConfiguration_setOwnKeyFromFile(tlsConfig, key_path, NULL)) {
        printf("FAIL load client key %s\n", key_path);
        TLSConfiguration_destroy(tlsConfig);
        return 1;
    }
    if (!TLSConfiguration_setOwnCertificateFromFile(tlsConfig, cert_path)) {
        printf("FAIL load client cert %s\n", cert_path);
        TLSConfiguration_destroy(tlsConfig);
        return 1;
    }
    if (!TLSConfiguration_addCACertificateFromFile(tlsConfig, ca_path)) {
        printf("FAIL load CA %s\n", ca_path);
        TLSConfiguration_destroy(tlsConfig);
        return 1;
    }

    IedClientError error;
    IedConnection con = IedConnection_createWithTlsSupport(tlsConfig);
    IedConnection_connect(con, &error, host, port);
    if (error != IED_ERROR_OK) {
        printf("CONNECT FAIL err=%d (TLS handshake or cert reject)\n", error);
        IedConnection_destroy(con);
        TLSConfiguration_destroy(tlsConfig);
        return 2;
    }
    printf("CONNECT OK (TLS)\n");

    MmsValue* st = IedConnection_readObject(
        con, &error, "CCI016_01LD_Plant/WlimDWMX1.Mod.stVal", IEC61850_FC_ST);
    if (error == IED_ERROR_OK && st != NULL) {
        printf("READ Mod.stVal=%d\n", MmsValue_toInt32(st));
        MmsValue_delete(st);
    } else {
        printf("READ Mod.stVal FAIL err=%d\n", error);
    }

    MmsValue* totw = IedConnection_readObject(
        con, &error, "CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX);
    if (error == IED_ERROR_OK && totw != NULL) {
        printf("READ OK TotW.mag.f=%.3f\n", (double)MmsValue_toFloat(totw));
        MmsValue_delete(totw);
    } else {
        printf("READ FAIL TotW.mag.f err=%d\n", error);
    }

    MmsValue* totvar = IedConnection_readObject(
        con, &error, "CCI016_01LD_Plant/PdCMMXU1.TotVAr.mag.f", IEC61850_FC_MX);
    if (error == IED_ERROR_OK && totvar != NULL) {
        printf("READ OK TotVAr.mag.f=%.3f\n", (double)MmsValue_toFloat(totvar));
        MmsValue_delete(totvar);
    } else {
        printf("READ FAIL TotVAr.mag.f err=%d\n", error);
    }

    MmsValue* ppv = IedConnection_readObject(
        con, &error,
        "CCI016_01LD_Plant/PdCMMXU1.PPV.phsAB.cVal.mag.f", IEC61850_FC_MX);
    if (error == IED_ERROR_OK && ppv != NULL) {
        printf("READ OK PPV.phsAB.cVal.mag.f=%.3f\n", (double)MmsValue_toFloat(ppv));
        MmsValue_delete(ppv);
    } else {
        printf("READ FAIL PPV.phsAB.cVal.mag.f err=%d\n", error);
    }

    IedConnection_close(con);
    IedConnection_destroy(con);
    TLSConfiguration_destroy(tlsConfig);

    printf("VERDICT: TLS MMS session OK — cleartext :102 should be closed on DUT\n");
    return 0;
}
