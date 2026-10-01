/*
 * CCLI Phase 3 Step 5 — lab MMS client (Eth_A)
 * Connects to TG544 CCI016_01, browses MVP objects, enables urcb_PdC_Mis4sec,
 * records TotW reports for ~N seconds to prove ~4 s integrity period.
 *
 * Build (WSL, against host libiec61850 from apps/ccli/build-mms-host):
 *   see lab/tg544-openwrt/build-mms-lab-client.sh
 *
 * Usage:
 *   mms_lab_client [host] [port] [listen_sec]
 *   mms_lab_client 192.168.10.1 102 16
 */
#include "iec61850_client.h"
#include "hal_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

static volatile int g_running = 1;
static int g_report_count = 0;
static uint64_t g_last_ms = 0;
static uint64_t g_first_ms = 0;

static void on_sigint(int s) {
    (void)s;
    g_running = 0;
}

static void print_list(const char* title, LinkedList list) {
    printf("%s\n", title);
    if (list == NULL) {
        printf("  (null)\n");
        return;
    }
    LinkedList entry = LinkedList_getNext(list);
    while (entry != NULL) {
        char* name = (char*)LinkedList_getData(entry);
        printf("  %s\n", name);
        entry = LinkedList_getNext(entry);
    }
}

static void report_cb(void* parameter, ClientReport report) {
    (void)parameter;
    uint64_t now = Hal_getTimeInMs();
    g_report_count++;
    if (g_first_ms == 0) {
        g_first_ms = now;
    }
    int delta = (g_last_ms == 0) ? 0 : (int)(now - g_last_ms);
    g_last_ms = now;

    printf("REPORT #%d  rcb=%s  rptId=%s  delta_ms=%d\n", g_report_count,
           ClientReport_getRcbReference(report), ClientReport_getRptId(report),
           delta);

    MmsValue* values = ClientReport_getDataSetValues(report);
    if (values != NULL) {
        char buf[256];
        MmsValue_printToBuffer(values, buf, sizeof(buf));
        printf("  dataSetValues: %s\n", buf);
    }
    fflush(stdout);
}

static int read_float_object(IedConnection con, const char* ref) {
    IedClientError err;
    MmsValue* v = IedConnection_readObject(con, &err, ref, IEC61850_FC_MX);
    if (err != IED_ERROR_OK || v == NULL) {
        printf("READ FAIL %s err=%d\n", ref, err);
        return -1;
    }
    char buf[256];
    MmsValue_printToBuffer(v, buf, sizeof(buf));
    printf("READ OK   %s = %s\n", ref, buf);
    MmsValue_delete(v);
    return 0;
}

int main(int argc, char** argv) {
    const char* host = "192.168.10.1";
    int port = 102;
    int listen_sec = 16;

    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }
    if (argc > 3) {
        listen_sec = atoi(argv[3]);
    }

    signal(SIGINT, on_sigint);

    printf("=== CCLI MMS lab client (Step 5) ===\n");
    printf("host=%s port=%d listen_sec=%d\n", host, port, listen_sec);

    IedClientError error;
    IedConnection con = IedConnection_create();
    IedConnection_connect(con, &error, host, port);

    if (error != IED_ERROR_OK) {
        printf("CONNECT FAIL err=%d\n", error);
        IedConnection_destroy(con);
        return 1;
    }
    printf("CONNECT OK\n");

    /* Browse */
    LinkedList devices = IedConnection_getLogicalDeviceList(con, &error);
    print_list("Logical devices:", devices);
    LinkedList_destroy(devices);

    LinkedList nodes =
        IedConnection_getLogicalDeviceDirectory(con, &error, "CCI016_01LD_Plant");
    if (error != IED_ERROR_OK) {
        /* try alternate LD name forms */
        nodes = IedConnection_getLogicalDeviceDirectory(con, &error, "LD_Plant");
    }
    print_list("LN under LD_Plant:", nodes);
    LinkedList_destroy(nodes);

    /* Direct reads — TotW + Wlim stubs */
    read_float_object(con, "CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f");
    read_float_object(con, "CCI016_01LD_Plant/PdCMMXU1.TotVAr.mag.f");
    /* fallback paths if mag.f layout differs */
    {
        IedClientError err;
        MmsValue* v = IedConnection_readObject(con, &err,
                                               "CCI016_01LD_Plant/PdCMMXU1.TotW",
                                               IEC61850_FC_MX);
        if (err == IED_ERROR_OK && v != NULL) {
            char buf[256];
            MmsValue_printToBuffer(v, buf, sizeof(buf));
            printf("READ OK   CCI016_01LD_Plant/PdCMMXU1.TotW (MX) = %s\n", buf);
            MmsValue_delete(v);
        } else {
            printf("READ FAIL CCI016_01LD_Plant/PdCMMXU1.TotW err=%d\n", err);
        }
    }

    const char* ds_ref = "CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec";
    const char* rcb_ref = NULL;
    static const char* rcb_candidates[] = {
        "CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec01",
        "CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec01",
        "CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec02",
        "CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec",
        "CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec",
        "LD_Plant/LLN0.urcb_PdC_Mis4sec01",
        "LD_Plant/LLN0.urcb_PdC_Mis4sec",
        NULL};

    LinkedList ds_dir =
        IedConnection_getDataSetDirectory(con, &error, ds_ref, NULL);
    if (error != IED_ERROR_OK) {
        printf("DataSet directory FAIL %s err=%d — trying without IED prefix\n",
               ds_ref, error);
        ds_ref = "LD_Plant/LLN0.DS_R_PdC_Mis4sec";
        ds_dir = IedConnection_getDataSetDirectory(con, &error, ds_ref, NULL);
    }
    print_list("DataSet members:", ds_dir);

    ClientDataSet ds =
        IedConnection_readDataSetValues(con, &error, ds_ref, NULL);
    if (ds == NULL) {
        printf("readDataSetValues FAIL err=%d\n", error);
    } else {
        printf("DataSet read OK\n");
    }

    ClientReportControlBlock rcb = NULL;
    for (int i = 0; rcb_candidates[i] != NULL; ++i) {
        rcb_ref = rcb_candidates[i];
        error = IED_ERROR_OK;
        rcb = IedConnection_getRCBValues(con, &error, rcb_ref, NULL);
        if (error == IED_ERROR_OK && rcb != NULL) {
            printf("getRCBValues OK %s\n", rcb_ref);
            break;
        }
        printf("getRCBValues FAIL %s err=%d\n", rcb_ref, error);
        rcb = NULL;
    }

    if (rcb != NULL) {
        const char* rpt_id = ClientReportControlBlock_getRptId(rcb);
        printf("RCB IntgPd=%u RptEna=%d RptId=%s\n",
               ClientReportControlBlock_getIntgPd(rcb),
               ClientReportControlBlock_getRptEna(rcb),
               rpt_id != NULL ? rpt_id : "(null)");

        /* Handler MUST use RptId; install before enabling RCB */
        IedConnection_installReportHandler(con, rcb_ref, rpt_id, report_cb, NULL);

        ClientReportControlBlock_setIntgPd(rcb, 4000);
        ClientReportControlBlock_setTrgOps(rcb, TRG_OPT_DATA_CHANGED |
                                                    TRG_OPT_QUALITY_CHANGED |
                                                    TRG_OPT_INTEGRITY |
                                                    TRG_OPT_GI);
        ClientReportControlBlock_setOptFlds(
            rcb, RPT_OPT_SEQ_NUM | RPT_OPT_TIME_STAMP |
                     RPT_OPT_REASON_FOR_INCLUSION | RPT_OPT_DATA_SET |
                     RPT_OPT_DATA_REFERENCE);
        ClientReportControlBlock_setRptEna(rcb, true);

        IedConnection_setRCBValues(
            con, &error, rcb,
            RCB_ELEMENT_RPT_ENA | RCB_ELEMENT_TRG_OPS | RCB_ELEMENT_INTG_PD |
                RCB_ELEMENT_OPT_FLDS,
            true);
        if (error != IED_ERROR_OK) {
            printf("setRCBValues (enable) FAIL err=%d\n", error);
        } else {
            printf("RCB enabled — listening %d s for integrity reports...\n",
                   listen_sec);
        }

        /* Separate GI trigger (common libiec61850 pattern) */
        Thread_sleep(200);
        ClientReportControlBlock_setGI(rcb, true);
        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_GI, true);
        if (error != IED_ERROR_OK) {
            printf("GI trigger FAIL err=%d\n", error);
        } else {
            printf("GI triggered\n");
        }

        int waited = 0;
        while (g_running && waited < listen_sec * 10) {
            Thread_sleep(100);
            waited++;
            if ((waited % 50) == 0) {
                IedConnectionState st = IedConnection_getState(con);
                if (st != IED_STATE_CONNECTED) {
                    printf("Connection lost (state=%d)\n", st);
                    break;
                }
            }
        }

        ClientReportControlBlock_setRptEna(rcb, false);
        IedConnection_setRCBValues(con, &error, rcb, RCB_ELEMENT_RPT_ENA, true);
        ClientReportControlBlock_destroy(rcb);
    }

    if (ds != NULL) {
        ClientDataSet_destroy(ds);
    }
    if (ds_dir != NULL) {
        LinkedList_destroy(ds_dir);
    }

    IedConnection_close(con);
    IedConnection_destroy(con);

    printf("=== SUMMARY ===\n");
    printf("reports_received=%d\n", g_report_count);
    if (g_report_count >= 2 && g_first_ms != 0 && g_last_ms > g_first_ms) {
        double avg =
            (double)(g_last_ms - g_first_ms) / (double)(g_report_count - 1);
        printf("avg_interval_ms=%.0f (expect ~4000)\n", avg);
        if (avg >= 3000.0 && avg <= 5500.0) {
            printf("VERDICT: P3-03 PART/PASS candidate — ~4 s integrity OK\n");
            return 0;
        }
    }
    if (g_report_count >= 1) {
        printf("VERDICT: PART — connected + at least one report (tune interval)\n");
        return 0;
    }
    printf("VERDICT: FAIL — no reports (browse may still be OK above)\n");
    return 2;
}
