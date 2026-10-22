/*=============================================================================
 * File       :  QUEUE_MY.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - queues (deferred work between program modules)
 *
 * Single-threaded supervisor: no locking (all producers / consumers run in
 * the Main task loop).
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "queue_my.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* ring buffer */
typedef struct
{
    u32 records[QUEUE__MAX_NUM_RECORDS];
    u32 head;                         /* next record to get */
    u32 count;                        /* queued records     */
} QUEUE__RING;




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* queues */
static QUEUE__RING Queue_Rings[QUEUE__NUM_QUEUES];




/*=============================================================================
 * Function   : Queue_Init
 *
 * Description: init (all queues empty)
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Init(void)
{
    memset(Queue_Rings, 0, sizeof(Queue_Rings));
}




/*=============================================================================
 * Function   : Queue_PutRecord
 *
 * Description: put one record at the tail
 * Input      : queue_id - QUEUE__QUEUE_ID__*
 *              record   - record
 * Output     : FALSE when the queue is full or the ID is invalid
 *=============================================================================*/
bool Queue_PutRecord(u8 queue_id, u32 record)
{
    QUEUE__RING *ring;

    if (queue_id >= QUEUE__NUM_QUEUES)
    {
        return false;
    }
    ring = &Queue_Rings[queue_id];
    if (ring->count >= QUEUE__MAX_NUM_RECORDS)
    {
        return false;
    }
    ring->records[(ring->head + ring->count) % QUEUE__MAX_NUM_RECORDS] = record;
    ring->count++;
    return true;
}




/*=============================================================================
 * Function   : Queue_GetRecord
 *
 * Description: get (and remove) the record at the head
 * Input      : queue_id   - QUEUE__QUEUE_ID__*
 * Output     : ptr_record - record; FALSE when empty or the ID is invalid
 *=============================================================================*/
bool Queue_GetRecord(u8 queue_id, u32 *ptr_record)
{
    QUEUE__RING *ring;

    if ((queue_id >= QUEUE__NUM_QUEUES) || (ptr_record == 0))
    {
        return false;
    }
    ring = &Queue_Rings[queue_id];
    if (ring->count == 0)
    {
        return false;
    }
    *ptr_record = ring->records[ring->head];
    *ptr_record = ring->records[ring->head];
    ring->head  = (ring->head + 1) % QUEUE__MAX_NUM_RECORDS;
    ring->count--;
    return true;
}




/*=============================================================================
 * Function   : Queue_NumRecords
 *
 * Description: number of queued records
 * Input      : queue_id - QUEUE__QUEUE_ID__*
 * Output     : number of records (0 for an invalid ID)
 *=============================================================================*/
u32 Queue_NumRecords(u8 queue_id)
{
    if (queue_id >= QUEUE__NUM_QUEUES)
    {
        return 0;
    }
    return Queue_Rings[queue_id].count;
}




/*=============================================================================
 * Function   : Queue_UpdateVarCrc / Queue_VerifyVarCrc
 *
 * Description: status variables CRC (retained state across restart)
 *              TODO(ccli-product-sw-freeze): back with program_flash
 * Input      : -
 * Output     : VerifyVarCrc - TRUE when retained state is valid
 *=============================================================================*/
void Queue_UpdateVarCrc(void)
{
}

bool Queue_VerifyVarCrc(void)
{
    return true;
}
