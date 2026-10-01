/*
 * CCLI Phase 3 — TLS MMS client with selectable identity (DSO / VIEWER / revoked).
 *
 * Usage:
 *   mms_tls_wlim_client <host> <port> <cert_dir> <identity> [wmax_spt_pct] [mode]
 *
 * identity: dso | viewer | revoked
 * mode:     write (default) | read
 *
 * write: operate Wlim (expect OK for dso, FAIL for viewer)
 * read:  read Mod.stVal only
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

static int operate_float(IedConnection con, const char* ref, float value) {
    ControlObjectClient ctrl = ControlObjectClient_create(ref, con);
    if (ctrl == NULL) {
        printf("FAIL create control %s\n", ref);
        return -1;
    }
    ControlObjectClient_setOrigin(ctrl, NULL, 3);
    MmsValue* ctlVal = MmsValue_newFloat(value);
    const int ok = ControlObjectClient_operate(ctrl, ctlVal, 0) ? 0 : -1;
    printf("%s %s = %.2f\n", ok == 0 ? "OPERATE OK" : "OPERATE FAIL", ref,
           (double)value);
    MmsValue_delete(ctlVal);
    ControlObjectClient_destroy(ctrl);
    return ok;
}

static int operate_int(IedConnection con, const char* ref, int value) {
    ControlObjectClient ctrl = ControlObjectClient_create(ref, con);
    if (ctrl == NULL) {
        printf("FAIL create control %s\n", ref);
        return -1;
    }
    ControlObjectClient_setOrigin(ctrl, NULL, 3);
    MmsValue* ctlVal = MmsValue_newIntegerFromInt32(value);
    const int ok = ControlObjectClient_operate(ctrl, ctlVal, 0) ? 0 : -1;
    printf("%s %s = %d\n", ok == 0 ? "OPERATE OK" : "OPERATE FAIL", ref, value);
    MmsValue_delete(ctlVal);
    ControlObjectClient_destroy(ctrl);
    return ok;
}

int main(int argc, char** argv) {
    const char* host = "192.168.10.1";
    int port = 3782;
    const char* cert_dir = ".";
    const char* identity = "dso";
    float pct = 10.0f;
    const char* mode = "write";

    if (argc > 1) host = argv[1];
    if (argc > 2) port = atoi(argv[2]);
    if (argc > 3) cert_dir = argv[3];
    if (argc > 4) identity = argv[4];
    if (argc > 5) pct = (float)atof(argv[5]);
    if (argc > 6) mode = argv[6];

    const char* key_file = "client.key";
    const char* cert_file = "client.pem";
    if (strcmp(identity, "viewer") == 0) {
        key_file = "viewer.key";
        cert_file = "viewer.pem";
    } else if (strcmp(identity, "revoked") == 0) {
        key_file = "revoked.key";
        cert_file = "revoked.pem";
    } else if (strcmp(identity, "dso") != 0) {
        printf("FAIL unknown identity '%s' (use dso|viewer|revoked)\n", identity);
        return 1;
    }

    char key_path[512], cert_path[512], ca_path[512];
    join_path(key_path, sizeof(key_path), cert_dir, key_file);
    join_path(cert_path, sizeof(cert_path), cert_dir, cert_file);
    join_path(ca_path, sizeof(ca_path), cert_dir, "root_CA.pem");

    printf("=== CCLI MMS TLS Wlim client (P3-08/09) ===\n");
    printf("host=%s port=%d identity=%s mode=%s cert=%s\n", host, port, identity,
           mode, cert_file);

    TLSConfiguration tlsConfig = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tlsConfig, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tlsConfig, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tlsConfig, false);

    if (!TLSConfiguration_setOwnKeyFromFile(tlsConfig, key_path, NULL)) {
        printf("FAIL load key %s\n", key_path);
        TLSConfiguration_destroy(tlsConfig);
        return 1;
    }
    if (!TLSConfiguration_setOwnCertificateFromFile(tlsConfig, cert_path)) {
        printf("FAIL load cert %s\n", cert_path);
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
        printf("CONNECT FAIL err=%d\n", error);
        IedConnection_destroy(con);
        TLSConfiguration_destroy(tlsConfig);
        return 2;
    }
    printf("CONNECT OK\n");

    int rc = 0;
    if (strcmp(mode, "read") == 0) {
        MmsValue* st = IedConnection_readObject(
            con, &error, "CCI016_01LD_Plant/WlimDWMX1.Mod.stVal", IEC61850_FC_ST);
        if (error == IED_ERROR_OK && st != NULL) {
            printf("READ Mod.stVal=%d\n", MmsValue_toInt32(st));
            MmsValue_delete(st);
        } else {
            printf("READ FAIL err=%d\n", error);
            rc = 3;
        }
    } else {
        if (operate_float(con, "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct", pct) != 0) {
            rc = 4;
        }
        Thread_sleep(200);
        if (operate_int(con, "CCI016_01LD_Plant/WlimDWMX1.Mod", 1) != 0) {
            rc = 4;
        }
    }

    IedConnection_close(con);
    IedConnection_destroy(con);
    TLSConfiguration_destroy(tlsConfig);

    if (strcmp(identity, "dso") == 0 && strcmp(mode, "write") == 0) {
        printf("VERDICT: DSO write %s\n", rc == 0 ? "OK" : "FAIL");
    } else if (strcmp(identity, "viewer") == 0 && strcmp(mode, "write") == 0) {
        printf("VERDICT: VIEWER write denied expected — %s\n",
               rc != 0 ? "PASS (denied)" : "FAIL (should deny)");
        return rc != 0 ? 0 : 5;
    } else if (strcmp(identity, "revoked") == 0) {
        printf("VERDICT: unexpected CONNECT for revoked (should fail TLS)\n");
        return 6;
    } else {
        printf("VERDICT: OK\n");
    }
    return rc;
}
