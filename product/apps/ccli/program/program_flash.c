/*=============================================================================
 * File       :  PROGRAM_FLASH.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - flash objects (persisted configuration / status)
 *
 * TODO(ccli-product-sw-freeze): persist served objects to /etc/ccli (overlay)
 * with CRC so Startup can recover retained status. Today requests are
 * accepted and served without a write; O.14 events already persist through
 * core/event/event_store.
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "program_flash.h"

#include "queue_my.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* backup request record: handle index (bits 15..8) | flash ID (bits 7..0) */
#define PROGRAM_FLASH__RECORD(handle_index, flash_id)   ((((u32)(handle_index)) << 8) | ((u32)(flash_id)))




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* backup requests served since boot */
static u32 ProgramFlash_BackupsServed = 0;




/*=============================================================================
 * Function   : ProgramFlash_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void ProgramFlash_Init(void)
{
    ProgramFlash_BackupsServed = 0;
}




/*=============================================================================
 * Function   : ProgramFlash_RequestWriteBackup
 *
 * Description: request the backup of one flash object
 * Input      : handle_index - PROGRAM_FLASH__HANDLE_INDEX__*
 *              flash_id     - PROGRAM_FLASH__FLASH_ID__*
 * Output     : FALSE when the handle is invalid or the queue is full
 *=============================================================================*/
bool ProgramFlash_RequestWriteBackup(u8 handle_index, u8 flash_id)
{
    if (handle_index >= PROGRAM_FLASH__NUM_HANDLES)
    {
        return false;
    }
    return Queue_PutRecord(QUEUE__QUEUE_ID__FLASH_BACKUP, PROGRAM_FLASH__RECORD(handle_index, flash_id));
}




/*=============================================================================
 * Function   : ProgramFlash_Task
 *
 * Description: serve pending backup requests
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void ProgramFlash_Task(u32 tick)
{
    u32 record;

    (void)tick;

    while (Queue_GetRecord(QUEUE__QUEUE_ID__FLASH_BACKUP, &record))
    {
        ProgramFlash_BackupsServed++;
    }
}




/*=============================================================================
 * Function   : ProgramFlash_NumBackupsServed
 *
 * Description: number of backup requests served since boot
 * Input      : -
 * Output     : count
 *=============================================================================*/
u32 ProgramFlash_NumBackupsServed(void)
{
    return ProgramFlash_BackupsServed;
}
