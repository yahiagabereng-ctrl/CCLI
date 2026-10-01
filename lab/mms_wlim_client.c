/*
 * CCLI Phase 3 P3-04 — DSO client writes Wlim (O.9.2.2) to Eth_A MMS server.
 *
 * Usage:
 *   mms_wlim_client [host] [port] [wmax_spt_pct]
 *   mms_wlim_client 192.168.10.1 102 50
 *
 * Operates:
 *   CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct = pct
 *   CCI016_01LD_Plant/WlimDWMX1.Mod = 1 (Active)
 */
#include "iec61850_client.h"
#include "hal_thread.h"

#include <stdio.h>
#include <stdlib.h>

static int operate_float(IedConnection con, const char* ref, float value) {
    IedClientError err;
    ControlObjectClient ctrl = ControlObjectClient_create(ref, con);
    if (ctrl == NULL) {
        printf("FAIL create control %s\n", ref);
        return -1;
    }
    ControlObjectClient_setOrigin(ctrl, NULL, 3);
    MmsValue* ctlVal = MmsValue_newFloat(value);
    const int ok = ControlObjectClient_operate(ctrl, ctlVal, 0) ? 0 : -1;
    if (ok == 0) {
        printf("OPERATE OK %s = %.2f\n", ref, (double)value);
    } else {
        printf("OPERATE FAIL %s\n", ref);
    }
    MmsValue_delete(ctlVal);
    ControlObjectClient_destroy(ctrl);
    (void)err;
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
    if (ok == 0) {
        printf("OPERATE OK %s = %d\n", ref, value);
    } else {
        printf("OPERATE FAIL %s\n", ref);
    }
    MmsValue_delete(ctlVal);
    ControlObjectClient_destroy(ctrl);
    return ok;
}

int main(int argc, char** argv) {
    const char* host = "192.168.10.1";
    int port = 102;
    float pct = 50.0f;

    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }
    if (argc > 3) {
        pct = (float)atof(argv[3]);
    }

    printf("=== CCLI MMS Wlim client (P3-04) ===\n");
    printf("host=%s port=%d WMaxSptPct=%.1f Mod=1 (Active)\n", host, port,
           (double)pct);

    IedClientError error;
    IedConnection con = IedConnection_create();
    IedConnection_connect(con, &error, host, port);
    if (error != IED_ERROR_OK) {
        printf("CONNECT FAIL err=%d\n", error);
        IedConnection_destroy(con);
        return 1;
    }
    printf("CONNECT OK\n");

    const char* wmax_ref = "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct";
    const char* mod_ref = "CCI016_01LD_Plant/WlimDWMX1.Mod";

    int rc = 0;
    if (operate_float(con, wmax_ref, pct) != 0) {
        rc = 2;
    }
    Thread_sleep(200);
    if (operate_int(con, mod_ref, 1) != 0) {
        rc = 2;
    }

    Thread_sleep(500);

    /* Read back status */
    MmsValue* st = IedConnection_readObject(con, &error, "CCI016_01LD_Plant/WlimDWMX1.Mod.stVal",
                                            IEC61850_FC_ST);
    if (error == IED_ERROR_OK && st != NULL) {
        printf("READ Mod.stVal=%d\n", MmsValue_toInt32(st));
        MmsValue_delete(st);
    }

    IedConnection_close(con);
    IedConnection_destroy(con);

    if (rc == 0) {
        printf("VERDICT: Wlim write sent — check TG log for mms→pf2 threshold update\n");
    } else {
        printf("VERDICT: FAIL — operate rejected (check ctlModel / server handlers)\n");
    }
    return rc;
}
