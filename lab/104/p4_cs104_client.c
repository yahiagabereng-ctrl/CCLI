/*
 * Phase 4 lab CS104 master — lib60870 (P4-02 / P4-03).
 *
 * Regulation: O.13.1.1.1 Eth_B · IEC 60870-5-104 · IEC 62351-3 TLS (optional).
 *
 * Usage:
 *   p4_cs104_client <host> [port] [ca] [ioa] [--tls] [--once]
 *
 * --once: single STARTDT session then disconnect (P4-04 comms-loss test).
 *
 * With --tls (lab PEMs under apps/ccli/config/tls/ via WSL mount):
 *   client: client_tls.pem + client.key
 *   trust:  root_CA.pem · allow server: server_tls.pem
 */

#include "cs101_information_objects.h"
#include "cs104_connection.h"
#include "hal_time.h"
#include "hal_thread.h"
#include "iec60870_common.h"

#if defined(CONFIG_CS104_SUPPORT_TLS)
#include "tls_config.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_expected_ioa = 1001;
static bool g_got_startdt = false;
static bool g_got_totw = false;
static float g_last_value = 0.f;

static void connectionHandler(void* parameter, CS104_Connection connection,
                              CS104_ConnectionEvent event) {
    (void)parameter;
    (void)connection;
    switch (event) {
    case CS104_CONNECTION_OPENED:
        printf("P4: connection OPEN\n");
        break;
    case CS104_CONNECTION_CLOSED:
        printf("P4: connection CLOSED\n");
        break;
    case CS104_CONNECTION_FAILED:
        printf("P4: connection FAILED\n");
        break;
    case CS104_CONNECTION_STARTDT_CON_RECEIVED:
        printf("P4: STARTDT_CON received\n");
        g_got_startdt = true;
        break;
    case CS104_CONNECTION_STOPDT_CON_RECEIVED:
        printf("P4: STOPDT_CON received\n");
        break;
    default:
        break;
    }
}

static bool asduReceivedHandler(void* parameter, int address, CS101_ASDU asdu) {
    (void)parameter;
    (void)address;

    const int type_id = CS101_ASDU_getTypeID(asdu);
    const int n = CS101_ASDU_getNumberOfElements(asdu);
    printf("P4: ASDU %s(%d) cot=%d ca=%d n=%d\n",
           TypeID_toString(type_id), type_id, CS101_ASDU_getCOT(asdu),
           CS101_ASDU_getCA(asdu), n);

    if (type_id != M_ME_NC_1) {
        return true;
    }

    for (int i = 0; i < n; i++) {
        MeasuredValueShort io = (MeasuredValueShort)CS101_ASDU_getElement(asdu, i);
        const int ioa = InformationObject_getObjectAddress((InformationObject)io);
        const float val = MeasuredValueShort_getValue(io);
        printf("P4: M_ME_NC_1 IOA=%d value=%.3f kW\n", ioa, val);
        if (ioa == g_expected_ioa) {
            g_got_totw = true;
            g_last_value = val;
        }
        MeasuredValueShort_destroy(io);
    }
    return true;
}

#if defined(CONFIG_CS104_SUPPORT_TLS)
static TLSConfiguration create_lab_tls_client(void) {
    const char* tls_dir = "/mnt/c/Yahia/projects/CCLI/apps/ccli/config/tls";
    char key[512];
    char cert[512];
    char ca[512];
    char server[512];
    snprintf(key, sizeof(key), "%s/client.key", tls_dir);
    snprintf(cert, sizeof(cert), "%s/client_tls.pem", tls_dir);
    snprintf(ca, sizeof(ca), "%s/root_CA.pem", tls_dir);
    snprintf(server, sizeof(server), "%s/server_tls.pem", tls_dir);

    TLSConfiguration tls = TLSConfiguration_create();
    TLSConfiguration_setClientMode(tls);
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setOwnKeyFromFile(tls, key, NULL);
    TLSConfiguration_setOwnCertificateFromFile(tls, cert);
    TLSConfiguration_addCACertificateFromFile(tls, ca);
    TLSConfiguration_addAllowedCertificateFromFile(tls, server);
    TLSConfiguration_setChainValidation(tls, true);
    TLSConfiguration_setAllowOnlyKnownCertificates(tls, true);
    return tls;
}
#endif

/** P4-05 / O.14 — send operator command ASDUs; expect ACT_CON negative on monitor_only slave. */
static bool run_reject_test(const char* ip, int port, int ca, bool use_tls) {
    CS104_Connection con = NULL;
#if defined(CONFIG_CS104_SUPPORT_TLS)
    TLSConfiguration tls = NULL;
    if (use_tls) {
        tls = create_lab_tls_client();
        con = CS104_Connection_createSecure(ip, (uint16_t)port, tls);
    } else
#endif
    {
        con = CS104_Connection_create(ip, (uint16_t)port);
    }

    CS104_Connection_setConnectionHandler(con, connectionHandler, NULL);
    CS104_Connection_setASDUReceivedHandler(con, asduReceivedHandler, NULL);

    if (!CS104_Connection_connect(con)) {
        printf("P4-05: connect failed\n");
        CS104_Connection_destroy(con);
#if defined(CONFIG_CS104_SUPPORT_TLS)
        if (tls != NULL) {
            TLSConfiguration_destroy(tls);
        }
#endif
        return false;
    }

    CS104_Connection_sendStartDT(con);
    Thread_sleep(500);

    /* C_SC_NA_1 (45) — O.10.3.1 class command; must reject in monitor_only */
    SingleCommand sc = SingleCommand_create(NULL, 2001, true, false, 0);
    const bool sc_ok = CS104_Connection_sendProcessCommandEx(
        con, CS101_COT_ACTIVATION, ca, (InformationObject)sc);
    SingleCommand_destroy(sc);
    printf("P4-05: C_SC_NA_1(45) IOA=2001 sent=%s\n", sc_ok ? "yes" : "no");
    Thread_sleep(800);

    /* C_CS_NA_1 (103) — clock sync; operator_allow_clock_sync=false in lab yaml */
    struct sCP56Time2a cstime_storage;
    CP56Time2a cstime = CP56Time2a_createFromMsTimestamp(&cstime_storage, Hal_getTimeInMs());
    const bool cs_ok = CS104_Connection_sendClockSyncCommand(con, ca, cstime);
    printf("P4-05: C_CS_NA_1(103) clock sync sent=%s\n", cs_ok ? "yes" : "no");
    Thread_sleep(800);

    CS104_Connection_sendStopDT(con);
    Thread_sleep(300);
    CS104_Connection_close(con);
    CS104_Connection_destroy(con);
#if defined(CONFIG_CS104_SUPPORT_TLS)
    if (tls != NULL) {
        TLSConfiguration_destroy(tls);
    }
#endif
    return sc_ok;
}

/** P4-05 / O.10.3.1 — C_SE_NC_1 MSD set-point; expect ACT_CON on command-mode slave. */
static bool run_accept_test(const char* ip, int port, int ca, int ioa_msd, float sp_kw,
                            bool use_tls) {
    CS104_Connection con = NULL;
#if defined(CONFIG_CS104_SUPPORT_TLS)
    TLSConfiguration tls = NULL;
    if (use_tls) {
        tls = create_lab_tls_client();
        con = CS104_Connection_createSecure(ip, (uint16_t)port, tls);
    } else
#endif
    {
        con = CS104_Connection_create(ip, (uint16_t)port);
    }

    CS104_Connection_setConnectionHandler(con, connectionHandler, NULL);
    CS104_Connection_setASDUReceivedHandler(con, asduReceivedHandler, NULL);

    if (!CS104_Connection_connect(con)) {
        printf("P4-05: accept-test connect failed\n");
        CS104_Connection_destroy(con);
#if defined(CONFIG_CS104_SUPPORT_TLS)
        if (tls != NULL) {
            TLSConfiguration_destroy(tls);
        }
#endif
        return false;
    }

    CS104_Connection_sendStartDT(con);
    Thread_sleep(500);

    SetpointCommandShort sp =
        SetpointCommandShort_create(NULL, ioa_msd, sp_kw, false, 0);
    const bool sp_ok = CS104_Connection_sendProcessCommandEx(
        con, CS101_COT_ACTIVATION, ca, (InformationObject)sp);
    SetpointCommandShort_destroy(sp);
    printf("P4-05: C_SE_NC_1(50) IOA=%d value=%.3f kW sent=%s\n", ioa_msd, sp_kw,
           sp_ok ? "yes" : "no");
    Thread_sleep(1200);

    CS104_Connection_sendStopDT(con);
    Thread_sleep(300);
    CS104_Connection_close(con);
    CS104_Connection_destroy(con);
#if defined(CONFIG_CS104_SUPPORT_TLS)
    if (tls != NULL) {
        TLSConfiguration_destroy(tls);
    }
#endif
    return sp_ok;
}

static bool run_session(const char* ip, int port, int ca, int wait_ms, bool use_tls) {
    g_got_startdt = false;
    g_got_totw = false;
    g_last_value = 0.f;

    CS104_Connection con = NULL;
#if defined(CONFIG_CS104_SUPPORT_TLS)
    TLSConfiguration tls = NULL;
    if (use_tls) {
        tls = create_lab_tls_client();
        con = CS104_Connection_createSecure(ip, (uint16_t)port, tls);
        printf("P4: TLS mode (62351-3)\n");
    } else
#endif
    {
        con = CS104_Connection_create(ip, (uint16_t)port);
    }

    CS104_Connection_setConnectionHandler(con, connectionHandler, NULL);
    CS104_Connection_setASDUReceivedHandler(con, asduReceivedHandler, NULL);

    if (!CS104_Connection_connect(con)) {
        printf("P4: connect failed to %s:%d tls=%d\n", ip, port, use_tls ? 1 : 0);
        CS104_Connection_destroy(con);
#if defined(CONFIG_CS104_SUPPORT_TLS)
        if (tls != NULL) {
            TLSConfiguration_destroy(tls);
        }
#endif
        return false;
    }

    CS104_Connection_sendStartDT(con);
    Thread_sleep(500);

    CS104_Connection_sendInterrogationCommand(con, CS101_COT_ACTIVATION, ca,
                                              IEC60870_QOI_STATION);
    printf("P4: GI sent CA=%d, waiting %d ms\n", ca, wait_ms);
    Thread_sleep((unsigned int)wait_ms);

    CS104_Connection_sendStopDT(con);
    Thread_sleep(300);
    CS104_Connection_close(con);
    CS104_Connection_destroy(con);
#if defined(CONFIG_CS104_SUPPORT_TLS)
    if (tls != NULL) {
        TLSConfiguration_destroy(tls);
    }
#endif

    printf("P4: session summary startdt=%s totw_ioa%d=%s value=%.3f\n",
           g_got_startdt ? "YES" : "NO", g_expected_ioa, g_got_totw ? "YES" : "NO",
           g_last_value);
    return g_got_startdt && g_got_totw;
}

int main(int argc, char** argv) {
    const char* ip = "192.168.1.130";
    int port = IEC_60870_5_104_DEFAULT_PORT;
    int ca = 1;
    bool use_tls = false;
    bool once = false;
    bool reject_test = false;
    bool accept_test = false;
    int msd_ioa = 3001;
    float msd_kw = 42.5f;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tls") == 0) {
            use_tls = true;
        } else if (strcmp(argv[i], "--once") == 0) {
            once = true;
        } else if (strcmp(argv[i], "--reject-test") == 0) {
            reject_test = true;
        } else if (strcmp(argv[i], "--accept-test") == 0) {
            accept_test = true;
        } else if (strcmp(argv[i], "--msd-ioa") == 0 && i + 1 < argc) {
            msd_ioa = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--msd-kw") == 0 && i + 1 < argc) {
            msd_kw = (float)atof(argv[++i]);
        } else if (argv[i][0] != '-') {
            static int pos = 0;
            switch (pos++) {
            case 0:
                ip = argv[i];
                break;
            case 1:
                port = atoi(argv[i]);
                break;
            case 2:
                ca = atoi(argv[i]);
                break;
            case 3:
                g_expected_ioa = atoi(argv[i]);
                break;
            default:
                break;
            }
        }
    }

#if !defined(CONFIG_CS104_SUPPORT_TLS)
    if (use_tls) {
        fprintf(stderr, "P4: --tls requested but client built without CONFIG_CS104_SUPPORT_TLS\n");
        return 2;
    }
#endif

    printf("=== P4 CS104 client (lib60870) ===\n");
    printf("target=%s:%d ca=%d ioa=%d tls=%s\n", ip, port, ca, g_expected_ioa,
           use_tls ? "on" : "off");

    if (reject_test) {
        const bool sent = run_reject_test(ip, port, ca, use_tls);
        printf("P4-05: reject-test commands sent=%s — check DUT log for O.14 result=reject\n",
               sent ? "yes" : "no");
        return sent ? 0 : 1;
    }

    if (accept_test) {
        const bool sent = run_accept_test(ip, port, ca, msd_ioa, msd_kw, use_tls);
        printf("P4-05: accept-test MSD SP sent=%s — check DUT log for O.14 result=accept\n",
               sent ? "yes" : "no");
        return sent ? 0 : 1;
    }

    const bool pass1 = run_session(ip, port, ca, once ? 2000 : 6000, use_tls);
    if (once) {
        const bool pass = pass1 && g_got_startdt;
        printf("P4: VERDICT %s (once/disconnect for P4-04)\n", pass ? "PASS" : "FAIL");
        return pass ? 0 : 1;
    }

    printf("P4: reconnect after 2s\n");
    Thread_sleep(2000);
    const bool pass2 = run_session(ip, port, ca, 4000, use_tls);

    const bool pass = pass1 && pass2;
    printf("P4: VERDICT %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}
