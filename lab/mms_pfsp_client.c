/*
 * CCLI P5-R02 — DSO client writes PFSP (O.9.1.1) to Eth_A MMS server.
 *
 * Usage:
 *   mms_pfsp_client [host] [port] [cosphi] [gn|lod]
 *   mms_pfsp_client 192.168.10.1 102 -0.95 gn
 *
 * Operates:
 *   PFSPDFPF1.PFGnTgtSpt or PFLodTgtSpt = cosphi
 *   PFSPDFPF1.Mod = 1 (Active)
 */
#include "iec61850_client.h"
#include "hal_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* PFSP APC DOs use ctlModel=4 (SBO enhanced) in lab_tg544_eth_a.cfg — select then operate. */
static int operate_float_sbo(IedConnection con, const char* ref, float value) {
    ControlObjectClient ctrl = ControlObjectClient_create(ref, con);
    if (ctrl == NULL) {
        printf("FAIL create control %s\n", ref);
        return -1;
    }
    ControlObjectClient_setOrigin(ctrl, NULL, 3);
    MmsValue* ctlVal = MmsValue_newFloat(value);
    if (!ControlObjectClient_selectWithValue(ctrl, ctlVal)) {
        printf("SELECT FAIL %s\n", ref);
        MmsValue_delete(ctlVal);
        ControlObjectClient_destroy(ctrl);
        return -1;
    }
    const int ok = ControlObjectClient_operate(ctrl, ctlVal, 0) ? 0 : -1;
    if (ok == 0) {
        printf("OPERATE OK %s = %.4f\n", ref, (double)value);
    } else {
        printf("OPERATE FAIL %s\n", ref);
    }
    MmsValue_delete(ctlVal);
    ControlObjectClient_destroy(ctrl);
    return ok;
}

static int operate_int_sbo(IedConnection con, const char* ref, int value) {
    ControlObjectClient ctrl = ControlObjectClient_create(ref, con);
    if (ctrl == NULL) {
        printf("FAIL create control %s\n", ref);
        return -1;
    }
    ControlObjectClient_setOrigin(ctrl, NULL, 3);
    MmsValue* ctlVal = MmsValue_newIntegerFromInt32(value);
    if (!ControlObjectClient_selectWithValue(ctrl, ctlVal)) {
        printf("SELECT FAIL %s\n", ref);
        MmsValue_delete(ctlVal);
        ControlObjectClient_destroy(ctrl);
        return -1;
    }
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
    float cosphi = -0.95f;
    int generation = 1;

    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }
    if (argc > 3) {
        cosphi = (float)atof(argv[3]);
    }
    if (argc > 4 && strcasecmp(argv[4], "lod") == 0) {
        generation = 0;
    }

    const char* tgt_ref = generation
                              ? "CCI016_01LD_Plant/PFSPDFPF1.PFGnTgtSpt"
                              : "CCI016_01LD_Plant/PFSPDFPF1.PFLodTgtSpt";

    printf("=== CCLI MMS PFSP client (P5-R02) ===\n");
    printf("host=%s port=%d cosphi=%.4f %s Mod=1 (Active)\n", host, port,
           (double)cosphi, generation ? "PFGnTgtSpt" : "PFLodTgtSpt");

    IedClientError error;
    IedConnection con = IedConnection_create();
    IedConnection_connect(con, &error, host, port);
    if (error != IED_ERROR_OK) {
        printf("CONNECT FAIL err=%d\n", error);
        IedConnection_destroy(con);
        return 1;
    }
    printf("CONNECT OK\n");

    if (operate_float_sbo(con, tgt_ref, cosphi) != 0) {
        IedConnection_close(con);
        IedConnection_destroy(con);
        return 1;
    }
    if (operate_int_sbo(con, "CCI016_01LD_Plant/PFSPDFPF1.Mod", 1) != 0) {
        IedConnection_close(con);
        IedConnection_destroy(con);
        return 1;
    }

    IedConnection_close(con);
    IedConnection_destroy(con);
    printf("VERDICT: PASS — PFSP Operate sent\n");
    return 0;
}
