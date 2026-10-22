/*=============================================================================
 * File       :  queue_my_test.c
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  L1 unit tests for program/queue_my.c (Main messages and
 *               program_flash backup requests share this ring).
 *=============================================================================*/
#include "queue_my.h"

#include <stdio.h>
#include <stdlib.h>

static int expect_get(const char *label, u8 queue_id, u32 expected) {
    u32 record = 0xFFFFFFFFu;
    if (!Queue_GetRecord(queue_id, &record) || record != expected) {
        fprintf(stderr, "%s: expected %u, got %u\n", label, (unsigned)expected, (unsigned)record);
        return 0;
    }
    return 1;
}

int main(void) {
    u32 record = 0;
    u32 i;

    Queue_Init();

    /* L1-Q-01: FIFO order and record value returned. */
    if (!Queue_PutRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, 12800u) ||
        !Queue_PutRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, 12801u)) {
        fprintf(stderr, "L1-Q-01: put failed\n");
        return EXIT_FAILURE;
    }
    if (!expect_get("L1-Q-01 first", QUEUE__QUEUE_ID__MAIN_MESSAGE, 12800u) ||
        !expect_get("L1-Q-01 second", QUEUE__QUEUE_ID__MAIN_MESSAGE, 12801u)) {
        return EXIT_FAILURE;
    }

    /* L1-Q-02: empty queue returns FALSE. */
    if (Queue_GetRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, &record)) {
        fprintf(stderr, "L1-Q-02: get on empty queue succeeded\n");
        return EXIT_FAILURE;
    }

    /* L1-Q-03: queues are independent. */
    (void)Queue_PutRecord(QUEUE__QUEUE_ID__FLASH_BACKUP, 0x0100u);
    if (Queue_NumRecords(QUEUE__QUEUE_ID__MAIN_MESSAGE) != 0 ||
        !expect_get("L1-Q-03 flash", QUEUE__QUEUE_ID__FLASH_BACKUP, 0x0100u)) {
        fprintf(stderr, "L1-Q-03: queues not independent\n");
        return EXIT_FAILURE;
    }

    /* L1-Q-04: full queue rejects, wrap-around keeps order. */
    for (i = 0; i < QUEUE__MAX_NUM_RECORDS; ++i) {
        if (!Queue_PutRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, i)) {
            fprintf(stderr, "L1-Q-04: put %u failed before full\n", (unsigned)i);
            return EXIT_FAILURE;
        }
    }
    if (Queue_PutRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, 999u)) {
        fprintf(stderr, "L1-Q-04: put on full queue succeeded\n");
        return EXIT_FAILURE;
    }
    for (i = 0; i < QUEUE__MAX_NUM_RECORDS; ++i) {
        if (!expect_get("L1-Q-04 drain", QUEUE__QUEUE_ID__MAIN_MESSAGE, i)) {
            return EXIT_FAILURE;
        }
    }

    /* L1-Q-05: invalid queue id. */
    if (Queue_PutRecord(QUEUE__NUM_QUEUES, 1u) || Queue_GetRecord(QUEUE__NUM_QUEUES, &record)) {
        fprintf(stderr, "L1-Q-05: invalid queue id accepted\n");
        return EXIT_FAILURE;
    }

    printf("queue_my_test: all L1-Q cases passed\n");
    return EXIT_SUCCESS;
}
