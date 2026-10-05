/*
 * P5-G02 — read/write GoCB GoEna via GetGoCBValues / SetGoCBValues (cleartext :102).
 *
 * Uses ACSI object references (LD/LLN0.gcbName), not MMS $ST$/$CO$ paths.
 * Based on libiec61850 client_example_ClientGooseControl.
 *
 * Usage:
 *   mms_goena_client [host] [port]
 *   mms_goena_client 192.168.10.1 102
 */
#include "iec61850_client.h"

#include <stdio.h>
#include <stdlib.h>

static void print_gocb_list(const char* title, LinkedList list) {
    printf("%s\n", title);
    if (list == NULL) {
        printf("  (null)\n");
        return;
    }
    int n = 0;
    LinkedList entry = LinkedList_getNext(list);
    while (entry != NULL) {
        char* name = (char*)LinkedList_getData(entry);
        printf("  GoCB[%02d] %s\n", ++n, name != NULL ? name : "(null)");
        entry = LinkedList_getNext(entry);
    }
    printf("  TOTAL=%d\n", n);
}

static void print_gocb(ClientGooseControlBlock goCB) {
    if (goCB == NULL) {
        printf("  (null GoCB)\n");
        return;
    }
    printf("  GoEna   = %d\n", ClientGooseControlBlock_getGoEna(goCB));
    printf("  GoID    = %s\n", ClientGooseControlBlock_getGoID(goCB));
    printf("  DatSet  = %s\n", ClientGooseControlBlock_getDatSet(goCB));
    printf("  ConfRev = %u\n", ClientGooseControlBlock_getConfRev(goCB));
    printf("  NdsCom  = %d\n", ClientGooseControlBlock_getNdsComm(goCB));
    printf("  MinTime = %u ms\n", ClientGooseControlBlock_getMinTime(goCB));
    printf("  MaxTime = %u ms\n", ClientGooseControlBlock_getMaxTime(goCB));

    PhyComAddress dst = ClientGooseControlBlock_getDstAddress(goCB);
    printf("  Dst MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
           dst.dstAddress[0], dst.dstAddress[1], dst.dstAddress[2],
           dst.dstAddress[3], dst.dstAddress[4], dst.dstAddress[5]);
    printf("  Dst APPID = 0x%04x  VLAN-PRI=%u VID=%u\n",
           dst.appId, dst.vlanPriority, dst.vlanId);
}

static ClientGooseControlBlock read_gocb(IedConnection con, const char* ref, IedClientError* err) {
    ClientGooseControlBlock goCB =
        IedConnection_getGoCBValues(con, err, ref, NULL);
    if (*err != IED_ERROR_OK || goCB == NULL) {
        printf("GET FAIL  %s err=%d\n", ref, *err);
        return NULL;
    }
    printf("GET OK    %s\n", ref);
    print_gocb(goCB);
    return goCB;
}

static int write_goena(IedConnection con, ClientGooseControlBlock goCB, bool ena) {
    if (goCB == NULL) {
        return -1;
    }
    ClientGooseControlBlock_setGoEna(goCB, ena);

    IedClientError err;
    IedConnection_setGoCBValues(con, &err, goCB, GOCB_ELEMENT_GO_ENA, true);
    if (err != IED_ERROR_OK) {
        printf("SET FAIL  GoEna=%d err=%d\n", ena ? 1 : 0, err);
        return -1;
    }
    printf("SET OK    GoEna=%d\n", ena ? 1 : 0);
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

    /* MMS domain is IED+LD (genconfig); LD_Plant/... alone fails on wire. */
    static const char* gcb_primary[] = {
        "CCI016_01LD_Plant/LLN0.gcb_PdC_Mis4sec",
        "CCI016_01LD_Plant/LLN0.gcb_Stato_Allarmi",
    };
    static const char* gcb_alt[] = {
        "LD_Plant/LLN0.gcb_PdC_Mis4sec",
        "LD_Plant/LLN0.gcb_Stato_Allarmi",
    };

    printf("=== CCLI MMS GoCB client (P5-G02) ===\n");
    printf("host=%s port=%d\n", host, port);
    printf("API: IedConnection_getGoCBValues / setGoCBValues (ACSI refs)\n");

    IedClientError error;
    IedConnection con = IedConnection_create();
    IedConnection_connect(con, &error, host, port);
    if (error != IED_ERROR_OK) {
        printf("CONNECT FAIL err=%d\n", error);
        IedConnection_destroy(con);
        return 1;
    }
    printf("CONNECT OK\n");

    static const char* ln_refs[] = {
        "LD_Plant/LLN0",
        "CCI016_01LD_Plant/LLN0",
    };
    for (size_t i = 0; i < sizeof(ln_refs) / sizeof(ln_refs[0]); ++i) {
        LinkedList gocbs =
            IedConnection_getLogicalNodeDirectory(con, &error, ln_refs[i], ACSI_CLASS_GoCB);
        if (error == IED_ERROR_OK && gocbs != NULL) {
            char title[128];
            snprintf(title, sizeof(title), "GoCB directory (%s):", ln_refs[i]);
            print_gocb_list(title, gocbs);
            LinkedList_destroy(gocbs);
        } else {
            printf("GoCB directory FAIL %s err=%d\n", ln_refs[i], error);
        }
    }

    int pass = 0;
    for (size_t i = 0; i < sizeof(gcb_primary) / sizeof(gcb_primary[0]); ++i) {
        ClientGooseControlBlock goCB = read_gocb(con, gcb_primary[i], &error);
        if (goCB != NULL) {
            pass++;
            ClientGooseControlBlock_destroy(goCB);
        }
    }
    for (size_t i = 0; i < sizeof(gcb_alt) / sizeof(gcb_alt[0]); ++i) {
        ClientGooseControlBlock goCB = read_gocb(con, gcb_alt[i], &error);
        if (goCB != NULL) {
            ClientGooseControlBlock_destroy(goCB);
        }
    }

    ClientGooseControlBlock gcb1 = read_gocb(con, gcb_primary[0], &error);
    ClientGooseControlBlock gcb2 = read_gocb(con, gcb_primary[1], &error);

    if (gcb1 != NULL) {
        write_goena(con, gcb1, true);
        read_gocb(con, gcb_primary[0], &error);
        ClientGooseControlBlock_destroy(gcb1);
    }
    if (gcb2 != NULL) {
        write_goena(con, gcb2, true);
        read_gocb(con, gcb_primary[1], &error);
        ClientGooseControlBlock_destroy(gcb2);
    }

    IedConnection_close(con);
    IedConnection_destroy(con);

    printf("=== SUMMARY ===\n");
    printf("readable_gocb_refs=%d/2 (primary)\n", pass);
    if (pass >= 2) {
        printf("VERDICT: PASS — GoCB visible via GetGoCBValues\n");
        return 0;
    }
    if (pass > 0) {
        printf("VERDICT: PART — some GoCB refs work\n");
        return 2;
    }
    printf("VERDICT: FAIL — no GoCB on MMS (check model_cfg GC blocks / CCLI_HAVE_GOOSE build)\n");
    return 3;
}
