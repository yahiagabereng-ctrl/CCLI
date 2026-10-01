/*=============================================================================
 * File       :  LOGS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - logs
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "clock.h"
#include "debug_my.h"
#include "logs.h"
#include "program_flash.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* log descriptions */
#define LOG_DESCRIPTION__RX_COMMAND_LOG    "RX COMMAND LOG"   // received command log

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING            200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* logs (FIFO) */
static       LOGS__RX_COMMAND_LOG Logs_LogRxCommand;   // received command log


/* received command log default */
static const LOGS__RX_COMMAND_LOG Logs_LogRxCommandDefault =
{
    0,  // start   pointer (SP)
    0,  // reading pointer (RP)
    0,  // writing pointer (WP)

    // log buffer
    {
    //                                                                                                                    execution command indication
    //                      command type                     command time                   command name      command phone number   |         command text
    //                          |                                 |                              |                     |             |              |
    //                          |                  -------------------------------        ----------------      ----------------     |       ----------------
    //                          |                  |                             |        |              |      |              |     |       |              |
    //                          |                  |year           hour         week_day  |              |      |              |     |       |              |
    //                          |                  ||     month    |  minute    ||        |              |      |              |     |       |              |
    //                          |                  ||     |  day   |  |  second ||        |              |      |              |     |       |              |
    //                          |                  ||     |  |     |  |  |      ||        |              |      |              |     |       |              |
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 1
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 2
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 3
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 4
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 5
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 6
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 7
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 8
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 9
        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 10

        {   LOGS__RX_COMMAND__COMMAND_TYPE__SMS,   {2000, 1, 1,    0, 0, 0,     6},       {0x00, /*...*/ },     {0x00, /*...*/ },   TRUE,    {0x00, /*...*/ }   },   // 11
    }
};


/* debug string */
//static       ascii                Logs_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




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




/*=============================================================================
 * Function   : Logs_RxCommmand_ReadLogDescription
 *
 * Description: read the received command log description
 * Input      : -
 * Output     : - pointer to log description
 *=============================================================================*/
ascii *Logs_RxCommmand_ReadLogDescription(void)
{
    return ((ascii *)LOG_DESCRIPTION__RX_COMMAND_LOG);
}




/*=============================================================================
 * Function   : Logs_RxCommmand_PutRecord
 *
 * Description: put a record in the rx command log
 *              If the log is full, delete the oldest record in the log
 * Input      : - ptr_record: pointer to the record to be put in the log
 * Output     : - FALSE: record not put
 *              - TRUE : record     put
 *=============================================================================*/
bool Logs_RxCommmand_PutRecord(LOGS__RX_COMMAND_RECORD *ptr_record)
{
    LOGS__RX_COMMAND_RECORD *ptr_records;

    u8                      *ptr_ptr_start;
    u8                      *ptr_ptr_rd;
    u8                      *ptr_ptr_wr;


    ptr_records   = &Logs_LogRxCommand.records[0];

    ptr_ptr_start = &Logs_LogRxCommand.ptr_start;
    ptr_ptr_rd    = &Logs_LogRxCommand.ptr_rd;
    ptr_ptr_wr    = &Logs_LogRxCommand.ptr_wr;


    /* verify if the log is full */
    if ((((*ptr_ptr_wr) + 1) % (LOGS__LOG_SIZE__RX_COMMAND + 1)) == (*ptr_ptr_start))
    {
        /* log full */

        /* delete the older record present in the log */
        /* (update the log start pointer and possibly the log reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (LOGS__LOG_SIZE__RX_COMMAND + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (LOGS__LOG_SIZE__RX_COMMAND + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the log */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the log writing pointer */
    if (++(*ptr_ptr_wr) >= (LOGS__LOG_SIZE__RX_COMMAND + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__013_LOG, PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG);  //@@@ TO DO BETTER


    return TRUE;
}




/*=============================================================================
 * Function   : Logs_RxCommmand_GetRecord
 *
 * Description: get the older record not already read present in the rx command log
 * Input      : - ptr_record: pointer to store the record got from the log
 * Output     : - FALSE: record not got (there aren't record not already read in the log)
 *              - TRUE : record     got
 *=============================================================================*/
bool Logs_RxCommmand_GetRecord(LOGS__RX_COMMAND_RECORD *ptr_record)
{
    LOGS__RX_COMMAND_RECORD *ptr_records;

    u8                      *ptr_ptr_rd;
    u8                      *ptr_ptr_wr;


    ptr_records   = &Logs_LogRxCommand.records[0];

    ptr_ptr_rd    = &Logs_LogRxCommand.ptr_rd;
    ptr_ptr_wr    = &Logs_LogRxCommand.ptr_wr;


    /* verify if there are record not already read in the log */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the log */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record log reading pointer */
        if (++(*ptr_ptr_rd) >= (LOGS__LOG_SIZE__RX_COMMAND + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__013_LOG, PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG);  //@@@ TO DO BETTER

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the log */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Logs_RxCommmand_ReadRecord
 *
 * Description: read a specified record in the rx command log
 * Input      : - ptr_record: pointer to store the record read from the log
 *              - index     : record index to be read (1-LOGS__LOG_SIZE__RX_COMMAND)
 *                               - 1                         : most recent record
 *                               - LOGS__LOG_SIZE__RX_COMMAND: oldest      record
 * Output     : - FALSE: record not read (specified record not present)
 *              - TRUE : record     read
 *=============================================================================*/
bool Logs_RxCommmand_ReadRecord(LOGS__RX_COMMAND_RECORD *ptr_record, u8 index)
{
    LOGS__RX_COMMAND_RECORD *ptr_records;

    u8                       num_records;
    u8                       ptr_read;

    u8                       ptr_start;
    u8                       ptr_wr;


    if ((index == 0) || (index > LOGS__LOG_SIZE__RX_COMMAND))
        return FALSE;


    ptr_records = &Logs_LogRxCommand.records[0];

    ptr_start   =  Logs_LogRxCommand.ptr_start;
    ptr_wr      =  Logs_LogRxCommand.ptr_wr;


    /* calculate the number of record in the log */
    if (ptr_wr >= ptr_start)
        num_records =                                    (ptr_wr    - ptr_start);
    else
        num_records = (LOGS__LOG_SIZE__RX_COMMAND + 1) - (ptr_start - ptr_wr   );


    /* verify if there is the specified record in the log */
    if (index <= num_records)
    {
        /* specified record     present in the log */

        ptr_read = ptr_start + (num_records - index);
        if (ptr_read >= (LOGS__LOG_SIZE__RX_COMMAND + 1))
            ptr_read %= (LOGS__LOG_SIZE__RX_COMMAND + 1);

        *ptr_record = *(ptr_records + ptr_read);

        return TRUE;
    }
    else
    {
        /* specified record not present in the log */

        return FALSE;
    }
}



/*=============================================================================
 * Function   : Logs_RxCommmand_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the rx command log
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Logs_RxCommmand_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Logs_LogRxCommand.ptr_start;
    ptr_ptr_rd    = &Logs_LogRxCommand.ptr_rd;

    /* update the log start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__013_LOG, PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG);  //@@@ TO DO BETTER
}




/*=============================================================================
 * Function   : Logs_RxCommmand_RecoverRecordsGot
 *
 * Description: recover the records already got in the received command log (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Logs_RxCommmand_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Logs_LogRxCommand.ptr_start;
    ptr_ptr_rd    = &Logs_LogRxCommand.ptr_rd;

    /* update the log reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__013_LOG, PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG);  //@@@ TO DO BETTER
}




/*=============================================================================
 * Function   : Logs_RxCommmand_NumRecords
 *
 * Description: return the number of records in the received command log
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Logs_RxCommmand_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Logs_LogRxCommand.ptr_start;
    ptr_wr    = Logs_LogRxCommand.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                    (ptr_wr    - ptr_start);
    else
        num_records = (LOGS__LOG_SIZE__RX_COMMAND + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*===========================================================================
 * Function   : Logs_RxCommmand_GetDefault
 *
 * Description: get the default received command log
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Logs_RxCommmand_GetDefault(LOGS__RX_COMMAND_LOG *ptr_data)
{
    *ptr_data = Logs_LogRxCommandDefault;
}




/*===========================================================================
 * Function   : Logs_RxCommmand_Get
 *
 * Description: get the received command log
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Logs_RxCommmand_Get(LOGS__RX_COMMAND_LOG *ptr_data)
{
    *ptr_data = Logs_LogRxCommand;
}




/*===========================================================================
 * Function   : Logs_RxCommmand_Set
 *
 * Description: set the received command log
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Logs_RxCommmand_Set(LOGS__RX_COMMAND_LOG *ptr_data)
{
    Logs_LogRxCommand = *ptr_data;
}
