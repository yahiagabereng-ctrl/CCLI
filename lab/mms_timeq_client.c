/*
 * CCLI P3-11 — read TotW.t / TotW.q over TLS (TimeQuality evidence).
 * Usage: mms_timeq_client [host] [port] [cert_dir]
 */
#include "iec61850_client.h"
#include "tls_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void join_path(char* out, size_t n, const char* dir, const char* file) {
    snprintf(out, n, "%s/%s", dir, file);
}

int main(int argc, char** argv) {
    const char* host = "192.168.10.1";
    int port = 3782;
    const char* cert_dir = ".";
    if (argc > 1) host = argv[1];
    if (argc > 2) port = atoi(argv[2]);
    if (argc > 3) cert_dir = argv[3];

    char key_path[512], cert_path[512], ca_path[512];
    join_path(key_path, sizeof(key_path), cert_dir, "client.key");
    join_path(cert_path, sizeof(cert_path), cert_dir, "client.pem");
    join_path(ca_path, sizeof(ca_path), cert_dir, "root_CA.pem");

    printf("=== CCLI MMS TimeQuality client (P3-11 / T.3.3.4.5) ===\n");

    TLSConfiguration tlsConfig = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tlsConfig, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tlsConfig, true);
    if (!TLSConfiguration_setOwnKeyFromFile(tlsConfig, key_path, NULL) ||
        !TLSConfiguration_setOwnCertificateFromFile(tlsConfig, cert_path) ||
        !TLSConfiguration_addCACertificateFromFile(tlsConfig, ca_path)) {
        printf("FAIL load certs\n");
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

    MmsValue* mag = IedConnection_readObject(
        con, &error, "CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX);
    if (error == IED_ERROR_OK && mag != NULL) {
        printf("READ TotW.mag.f=%.3f\n", (double)MmsValue_toFloat(mag));
        MmsValue_delete(mag);
    }

    MmsValue* t = IedConnection_readObject(
        con, &error, "CCI016_01LD_Plant/PdCMMXU1.TotW.t", IEC61850_FC_MX);
    if (error == IED_ERROR_OK && t != NULL) {
        uint64_t ms = MmsValue_getUtcTimeInMs(t);
        time_t sec = (time_t)(ms / 1000ULL);
        struct tm tm_buf;
        gmtime_r(&sec, &tm_buf);
        char iso[64];
        strftime(iso, sizeof(iso), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);
        uint8_t tq = MmsValue_getUtcTimeQuality(t);
        printf("READ TotW.t utc=%s ms=%llu timeQuality=0x%02X "
               "(bit5 clockNotSync=%d)\n",
               iso, (unsigned long long)ms, tq, (tq >> 5) & 1);
        MmsValue_delete(t);
    } else {
        printf("READ TotW.t FAIL err=%d\n", error);
    }

    MmsValue* q = IedConnection_readObject(
        con, &error, "CCI016_01LD_Plant/PdCMMXU1.TotW.q", IEC61850_FC_MX);
    if (error == IED_ERROR_OK && q != NULL) {
        Quality qq = Quality_fromMmsValue(q);
        printf("READ TotW.q raw=0x%04X validity=%d\n", (unsigned)qq,
               (int)Quality_getValidity(&qq));
        MmsValue_delete(q);
    } else {
        printf("READ TotW.q FAIL err=%d\n", error);
    }

    IedConnection_close(con);
    IedConnection_destroy(con);
    TLSConfiguration_destroy(tlsConfig);
    printf("VERDICT: dump TotW.t/q for P3-11 evidence\n");
    return 0;
}
