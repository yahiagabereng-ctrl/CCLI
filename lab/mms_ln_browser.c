/*
 * CCLI P5 — browse all logical nodes on Eth_A MMS (TSP Compare Model prep).
 *
 * Usage:
 *   mms_ln_browser [host] [port]
 *
 * Prints LN list under LD_Plant, count vs CID expectation (31), and sample reads.
 */
#include "iec61850_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_list(const char* title, LinkedList list) {
    printf("%s\n", title);
    if (list == NULL) {
        printf("  (null)\n");
        return;
    }
    int n = 0;
    LinkedList entry = LinkedList_getNext(list);
    while (entry != NULL) {
        char* name = (char*)LinkedList_getData(entry);
        printf("  LN[%02d] %s\n", ++n, name != NULL ? name : "(null)");
        entry = LinkedList_getNext(entry);
    }
    printf("  TOTAL=%d\n", n);
}

static int read_stval(IedConnection con, const char* ln) {
    char ref[160];
    snprintf(ref, sizeof(ref), "CCI016_01LD_Plant/%s.Mod.stVal", ln);
    IedClientError err;
    MmsValue* v = IedConnection_readObject(con, &err, ref, IEC61850_FC_ST);
    if (err != IED_ERROR_OK || v == NULL) {
        printf("  READ FAIL %s.Mod.stVal err=%d\n", ln, err);
        return -1;
    }
    char buf[128];
    MmsValue_printToBuffer(v, buf, sizeof(buf));
    printf("  READ OK   %s.Mod.stVal = %s\n", ln, buf);
    MmsValue_delete(v);
    return 0;
}

int main(int argc, char** argv) {
    const char* host = "192.168.10.1";
    int port = 102;

    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = atoi(argv[2]);
    }

    printf("=== CCLI MMS LN browser (P5 full CID) ===\n");
    printf("host=%s port=%d expect_ln_count=31 (LLN0 + 30)\n", host, port);

    IedClientError error;
    IedConnection con = IedConnection_create();
    IedConnection_connect(con, &error, host, port);
    if (error != IED_ERROR_OK) {
        printf("CONNECT FAIL err=%d\n", error);
        IedConnection_destroy(con);
        return 1;
    }
    printf("CONNECT OK\n");

    LinkedList devices = IedConnection_getLogicalDeviceList(con, &error);
    print_list("Logical devices:", devices);
    LinkedList_destroy(devices);

    const char* ld_names[] = {"CCI016_01LD_Plant", "LD_Plant"};
    LinkedList nodes = NULL;
    for (size_t i = 0; i < sizeof(ld_names) / sizeof(ld_names[0]); ++i) {
        nodes = IedConnection_getLogicalDeviceDirectory(con, &error, ld_names[i]);
        if (error == IED_ERROR_OK && nodes != NULL) {
            printf("Using LD ref: %s\n", ld_names[i]);
            break;
        }
    }
    if (nodes == NULL || error != IED_ERROR_OK) {
        printf("LN directory FAIL err=%d\n", error);
        IedConnection_close(con);
        IedConnection_destroy(con);
        return 2;
    }

    print_list("Logical nodes under LD_Plant:", nodes);

    int ln_count = 0;
    LinkedList entry = LinkedList_getNext(nodes);
    while (entry != NULL) {
        ln_count++;
        entry = LinkedList_getNext(entry);
    }

    read_stval(con, "WlimDWMX1");
    read_stval(con, "WSdDAGC1");
    read_stval(con, "VArSdDVAR1");
    read_stval(con, "PdCMMXU1");

    LinkedList_destroy(nodes);
    IedConnection_close(con);
    IedConnection_destroy(con);

    printf("=== SUMMARY ===\n");
    printf("ln_count=%d expect=31\n", ln_count);
    if (ln_count == 31) {
        printf("VERDICT: PASS — all CID logical nodes visible\n");
        return 0;
    }
    if (ln_count >= 9) {
        printf("VERDICT: PART — MVP subset only (missing %d LNs)\n", 31 - ln_count);
        return 3;
    }
    printf("VERDICT: FAIL — incomplete model\n");
    return 4;
}
