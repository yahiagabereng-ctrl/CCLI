/*=============================================================================
 * File       :  LOGS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - logs
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __LOGS_H__


#define __LOGS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* log sizes */
#define LOGS__LOG_SIZE__RX_COMMAND             10    /* received command log size */

/* command type */
#define LOGS__RX_COMMAND__COMMAND_TYPE__SMS    0     /* command type - SMS  */
#define LOGS__RX_COMMAND__COMMAND_TYPE__PC     1     /* command type - PC   */
#define LOGS__RX_COMMAND__COMMAND_TYPE__RING   2     /* command type - RING */




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/

/*-----------------------------------------------------------------------------
 * Log record
 *-----------------------------------------------------------------------------*/
/* received command */
typedef struct
{
    /* command type */
    u8                      command_type;                             // command type (SMS, PC or RING)

    /* time */
    CLOCK__TIME             time;                                     // command time

    /* name/number (only for "text command") */
    ascii                   name        [ 14 + 1];                    // command name
    ascii                   phone_number[ 20 + 1];                    // command phone number

    /* execution command indication */
    bool                    command_exe;                              // execution command indication

    /* text        (only for "text command") */
    ascii                   text        [160 + 1];                    // command text
} LOGS__RX_COMMAND_RECORD;


/*-----------------------------------------------------------------------------
 * Log (FIFO)
 *-----------------------------------------------------------------------------*/
/* received command log (FIFO) */
typedef struct
{
    u8                      ptr_start;                                // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                      ptr_rd;                                   // reading pointer (RP)
    u8                      ptr_wr;                                   // writing pointer (WP)
    LOGS__RX_COMMAND_RECORD records[LOGS__LOG_SIZE__RX_COMMAND + 1];  // log buffer
} LOGS__RX_COMMAND_LOG;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/*-----------------------------------------------------------------------------
 * Read log description
 *-----------------------------------------------------------------------------*/
/* received command log - read log description */
ascii *Logs_RxCommmand_ReadLogDescription(void);


/*-----------------------------------------------------------------------------
 * Put/get records
 *-----------------------------------------------------------------------------*/
/* received command log - put/get */
bool   Logs_RxCommmand_PutRecord(LOGS__RX_COMMAND_RECORD *ptr_record);
bool   Logs_RxCommmand_GetRecord(LOGS__RX_COMMAND_RECORD *ptr_record);


/*-----------------------------------------------------------------------------
 * Read records
 *-----------------------------------------------------------------------------*/
/* received command log - read */
bool   Logs_RxCommmand_ReadRecord(LOGS__RX_COMMAND_RECORD *ptr_record, u8 index);


/*-----------------------------------------------------------------------------
 * Delete/recover records
 *-----------------------------------------------------------------------------*/
/* received command log - delete/recover */
void   Logs_RxCommmand_DeleteRecordsGot (void);
void   Logs_RxCommmand_RecoverRecordsGot(void);


/*-----------------------------------------------------------------------------
 * Number of records
 *-----------------------------------------------------------------------------*/
/* received command log - number of records */
u8     Logs_RxCommmand_NumRecords(void);


/*-----------------------------------------------------------------------------
 * Get/set log
 *-----------------------------------------------------------------------------*/
/* received command log - get/set log */
void   Logs_RxCommmand_GetDefault(LOGS__RX_COMMAND_LOG *ptr_data);
void   Logs_RxCommmand_Get       (LOGS__RX_COMMAND_LOG *ptr_data);
void   Logs_RxCommmand_Set       (LOGS__RX_COMMAND_LOG *ptr_data);




#endif
