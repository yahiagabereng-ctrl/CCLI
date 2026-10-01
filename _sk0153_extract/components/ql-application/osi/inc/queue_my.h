/*=============================================================================
 * File       :  QUEUE.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - transmitting queues
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __QUEUE_H__


#define __QUEUE_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "alarm.h"
#include "clock.h"
#include "sms_answer_event.h"
#include "synchronize.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* queue sizes */
#define QUEUE__QUEUE_SIZE__FW_UPGRADE        4    /* FW upgrade        queue size */
#define QUEUE__QUEUE_SIZE__ALARM             8    /* alarm             queue size */
#define QUEUE__QUEUE_SIZE__SMS_ANSWER        30   /* SMS answer        queue size */
#define QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS 4    /* SMS answer to CRS queue size */
#define QUEUE__QUEUE_SIZE__SMS_FORWARD       10   /* SMS forward       queue size */
#define QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE   1    /* autosynchronize   queue size */
#define QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT  8    /* SMS answer event  queue size */
#define QUEUE__QUEUE_SIZE__FILE_STATUS_LOG   48   /* file status log   queue size */
#define QUEUE__QUEUE_SIZE__FILE_ALARM        12   /* file alarm        queue size */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Queue record
 *-----------------------------------------------------------------------------*/
/* FW upgrade       record */
typedef struct
{
    /* phone number */
    ascii                                 sms_phone_number[20 + 1];    /* SMS DOTA phone number */

    /* time */
    CLOCK__TIME                           time;                        /* time */

    /* FW upgrade result */
    u8                                    result;                      /* FW upgrade result     */
    u8                                    error_code;                  /* FW upgrade error code */
} QUEUE__FW_UPGRADE_QUEUE_RECORD;


/* alarm queue      record */
typedef ALARM__ALARM                           QUEUE__ALARM_QUEUE_RECORD;


/* SMS answer       record */
typedef struct
{
    /* phone number */
    ascii                                 sms_phone_number[20 + 1];    /* SMS recipient phone number */

    /* time */
    CLOCK__TIME                           time;                        /* time */

    /* text */
    ascii                                 answer_text[160 + 1];        /* answer text */
} QUEUE__SMS_ANSWER_QUEUE_RECORD;


/* SMS answer to CRS record */
typedef struct
{
    /* phone number */
    ascii                                 sms_phone_number[20 + 1];    /* SMS recipient phone number */

    /* time */
    CLOCK__TIME                           time;                        /* time */

    /* text */
    ascii                                 answer_text[160 + 1];        /* answer text */
} QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD;


/* SMS forward      record */
typedef struct
{
    /* time */
    CLOCK__TIME                           time;                        /* time */

    /* text */
    ascii                                 forward_text[160 + 1];       /* forward text */
} QUEUE__SMS_FORWARD_QUEUE_RECORD;


/* autosynchronize  record */
typedef SYNCHRONIZE__SMS_COUNTER               QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD;


/* SMS answer event record */
typedef SMS_ANSWER_EVENT__SMS_ANSWER_EVENT     QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD;


/* file status log  record */
typedef struct
{
    /* time */
    CLOCK__TIME                           time;                        /* time */

    /* log file name */
    ascii                                 log_file_name[120 + 1];      /* log file name */

    /* log file info */
    u8                                    v;                           /* log file version */
    u32                                   n;                           /* lgg file number  */
    CLOCK__TIME                           start_time;                  /* start time       */
    CLOCK__TIME                           stop_time;                   /* stop  time       */
} QUEUE__FILE_STATUS_LOG_QUEUE_RECORD;


/* file alarm       record */
typedef struct
{
    /* time */
    CLOCK__TIME                           time;                        /* time */

    /* alarm file name */
    ascii                                 alarm_file_name[120 + 1];    /* alarm file name */

    /* alarm file info */
    u8                                    v;                           /* alarm file version */
    u32                                   n;                           /* alarm file number  */
    CLOCK__TIME                           start_time;                  /* start time         */
    CLOCK__TIME                           stop_time;                   /* stop  time         */

    /* alarm data */
    u8                                    alarm_type;                  /* alarm type ID   */
    u16                                   alarm_par;                   /* alarm parameter */
} QUEUE__FILE_ALARM_QUEUE_RECORD;


/*-----------------------------------------------------------------------------
 * Queue (FIFO)
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__FW_UPGRADE_QUEUE_RECORD        records[QUEUE__QUEUE_SIZE__FW_UPGRADE        + 1];  // queue buffer
} QUEUE__FW_UPGRADE_QUEUE;


/* alarm            queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__ALARM_QUEUE_RECORD             records[QUEUE__QUEUE_SIZE__ALARM             + 1];  // queue buffer
} QUEUE__ALARM_QUEUE;


/* SMS answer       queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__SMS_ANSWER_QUEUE_RECORD        records[QUEUE__QUEUE_SIZE__SMS_ANSWER        + 1];  // queue buffer
} QUEUE__SMS_ANSWER_QUEUE;



/* SMS answer to CRS queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD records[QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1];  // queue buffer
} QUEUE__SMS_ANSWER_TO_CRS_QUEUE;


/* SMS forward      queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__SMS_FORWARD_QUEUE_RECORD       records[QUEUE__QUEUE_SIZE__SMS_FORWARD       + 1];  // queue buffer
} QUEUE__SMS_FORWARD_QUEUE;


/* autosynchronize  queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD   records[QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE   + 1];  // queue buffer
} QUEUE__AUTOSYNCHRONIZE_QUEUE;


/* SMS answer event queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD  records[QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT  + 1];  // queue buffer
} QUEUE__SMS_ANSWER_EVENT_QUEUE;


/* file status log  queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__FILE_STATUS_LOG_QUEUE_RECORD   records[QUEUE__QUEUE_SIZE__FILE_STATUS_LOG   + 1];  // queue buffer
} QUEUE__FILE_STATUS_LOG_QUEUE;


/* file alarm       queue (FIFO) */
typedef struct
{
    u8                                    ptr_start;                                          // start   pointer (SP)   (SP <= RP <= WP ;  "<=" should be considered in a circular buffer logic)
    u8                                    ptr_rd;                                             // reading pointer (RP)
    u8                                    ptr_wr;                                             // writing pointer (WP)
    QUEUE__FILE_ALARM_QUEUE_RECORD        records[QUEUE__QUEUE_SIZE__FILE_ALARM        + 1];  // queue buffer
} QUEUE__FILE_ALARM_QUEUE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
void   Queue_Init(void);

/* status variables */
void   Queue_UpdateVarCrc(void);
bool   Queue_VerifyVarCrc(void);


/*-----------------------------------------------------------------------------
 * Read queue description
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue - read queue description */
ascii *Queue_FwUpgrade_ReadQueueDescription      (void);

/* alarm            queue - read queue description */
ascii *Queue_Alarm_ReadQueueDescription          (void);

/* SMS answer       queue - read queue description */
ascii *Queue_SmsAnswer_ReadQueueDescription      (void);

/* SMS answer to CRS queue - read queue description */
ascii *Queue_SmsAnswerToCrs_ReadQueueDescription (void);

/* SMS forward      queue - read queue description */
ascii *Queue_SmsForward_ReadQueueDescription     (void);

/* autosynchronize  queue - read queue description */
ascii *Queue_Autosynchronize_ReadQueueDescription(void);

/* SMS answer event queue - read queue description */
ascii *Queue_SmsAnswerEvent_ReadQueueDescription (void);

/* file status log  queue - read queue description */
ascii *Queue_FileStatusLog_ReadQueueDescription  (void);

/* file alarm       queue - read queue description */
ascii *Queue_FileAlarm_ReadQueueDescription      (void);


/*-----------------------------------------------------------------------------
 * Put/get records
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue - get/set queue */
bool   Queue_FwUpgrade_PutRecord      (QUEUE__FW_UPGRADE_QUEUE_RECORD        *ptr_record);
bool   Queue_FwUpgrade_GetRecord      (QUEUE__FW_UPGRADE_QUEUE_RECORD        *ptr_record);

/* alarm            queue - put/get */
bool   Queue_Alarm_PutRecord          (QUEUE__ALARM_QUEUE_RECORD             *ptr_record);
bool   Queue_Alarm_GetRecord          (QUEUE__ALARM_QUEUE_RECORD             *ptr_record);

/* SMS answer       queue - put/get */
bool   Queue_SmsAnswer_PutRecord      (QUEUE__SMS_ANSWER_QUEUE_RECORD        *ptr_record);
bool   Queue_SmsAnswer_GetRecord      (QUEUE__SMS_ANSWER_QUEUE_RECORD        *ptr_record);

/* SMS answer to CRS   queue - put/get */
bool   Queue_SmsAnswerToCrs_PutRecord (QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_record);
bool   Queue_SmsAnswerToCrs_GetRecord (QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_record);

/* SMS forward      queue - put/get */
bool   Queue_SmsForward_PutRecord     (QUEUE__SMS_FORWARD_QUEUE_RECORD       *ptr_record);
bool   Queue_SmsForward_GetRecord     (QUEUE__SMS_FORWARD_QUEUE_RECORD       *ptr_record);

/* autosynchronize  queue - get/set queue */
bool   Queue_Autosynchronize_PutRecord(QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD   *ptr_record);
bool   Queue_Autosynchronize_GetRecord(QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD   *ptr_record);

/* SMS answer event queue - get/set queue */
bool   Queue_SmsAnswerEvent_PutRecord (QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD  *ptr_record);
bool   Queue_SmsAnswerEvent_GetRecord (QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD  *ptr_record);

/* file status log  queue - get/set queue */
bool   Queue_FileStatusLog_PutRecord  (QUEUE__FILE_STATUS_LOG_QUEUE_RECORD   *ptr_record);
bool   Queue_FileStatusLog_GetRecord  (QUEUE__FILE_STATUS_LOG_QUEUE_RECORD   *ptr_record);

/* file alarm       queue - get/set queue */
bool   Queue_FileAlarm_PutRecord      (QUEUE__FILE_ALARM_QUEUE_RECORD        *ptr_record);
bool   Queue_FileAlarm_GetRecord      (QUEUE__FILE_ALARM_QUEUE_RECORD        *ptr_record);


/*-----------------------------------------------------------------------------
 * Delete/recover records
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue - delete/recover */
void   Queue_FwUpgrade_DeleteRecords          (void);
void   Queue_FwUpgrade_DeleteRecordsGot       (void);
void   Queue_FwUpgrade_RecoverRecordsGot      (void);

/* alarm            queue - delete/recover */
void   Queue_Alarm_DeleteRecords              (void);
void   Queue_Alarm_DeleteRecordsGot           (void);
void   Queue_Alarm_RecoverRecordsGot          (void);

/* SMS answer       queue - delete/recover */
void   Queue_SmsAnswer_DeleteRecords          (void);
void   Queue_SmsAnswer_DeleteRecordsGot       (void);
void   Queue_SmsAnswer_RecoverRecordsGot      (void);

/* SMS answer to CRS   queue - delete/recover */
void   Queue_SmsAnswerToCrs_DeleteRecords     (void);
void   Queue_SmsAnswerToCrs_DeleteRecordsGot  (void);
void   Queue_SmsAnswerToCrs_RecoverRecordsGot (void);

/* SMS forward      queue - delete/recover */
void   Queue_SmsForward_DeleteRecords         (void);
void   Queue_SmsForward_DeleteRecordsGot      (void);
void   Queue_SmsForward_RecoverRecordsGot     (void);

/* autosynchronize  queue - delete/recover */
void   Queue_Autosynchronize_DeleteRecords    (void);
void   Queue_Autosynchronize_DeleteRecordsGot (void);
void   Queue_Autosynchronize_RecoverRecordsGot(void);

/* SMS answer event queue - delete/recover */
void   Queue_SmsAnswerEvent_DeleteRecords     (void);
void   Queue_SmsAnswerEvent_DeleteRecordsGot  (void);
void   Queue_SmsAnswerEvent_RecoverRecordsGot (void);

/* file status log  queue - delete/recover */
void   Queue_FileStatusLog_DeleteRecords      (void);
void   Queue_FileStatusLog_DeleteRecordsGot   (void);
void   Queue_FileStatusLog_RecoverRecordsGot  (void);

/* file alarm       queue - delete/recover */
void   Queue_FileAlarm_DeleteRecords          (void);
void   Queue_FileAlarm_DeleteRecordsGot       (void);
void   Queue_FileAlarm_RecoverRecordsGot      (void);


/*-----------------------------------------------------------------------------
 * Number of records
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue - number of records */
u8     Queue_FwUpgrade_NumRecords      (void);

/* alarm            queue - number of records */
u8     Queue_Alarm_NumRecords          (void);

/* SMS answer       queue - number of records */
u8     Queue_SmsAnswer_NumRecords      (void);

/* SMS answer to CRS   queue - number of records */
u8     Queue_SmsAnswerToCrs_NumRecords (void);

/* SMS forward      queue - number of records */
u8     Queue_SmsForward_NumRecords     (void);

/* autosynchronize  queue - number of records */
u8     Queue_Autosynchronize_NumRecords(void);

/* SMS answer event queue - number of records */
u8     Queue_SmsAnswerEvent_NumRecords (void);

/* file status log  queue - number of records */
u8     Queue_FileStatusLog_NumRecords  (void);

/* file alarm       queue - number of records */
u8     Queue_FileAlarm_NumRecords      (void);


/*-----------------------------------------------------------------------------
 * Get/set queue
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue - get/set queue */
void   Queue_FwUpgradeQueue_GetDefault     (QUEUE__FW_UPGRADE_QUEUE        *ptr_data);
void   Queue_FwUpgradeQueue_Get            (QUEUE__FW_UPGRADE_QUEUE        *ptr_data);
void   Queue_FwUpgradeQueue_Set            (QUEUE__FW_UPGRADE_QUEUE        *ptr_data);

/* SMS answer to CRS   queue - get/set queue */
void   Queue_SmsAnswerToCrsQueue_GetDefault(QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data);
void   Queue_SmsAnswerToCrsQueue_Get       (QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data);
void   Queue_SmsAnswerToCrsQueue_Set       (QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data);

/* file status log  queue - get/set queue */
void   Queue_FileStatusLogQueue_GetDefault (QUEUE__FILE_STATUS_LOG_QUEUE   *ptr_data);
void   Queue_FileStatusLogQueue_Get        (QUEUE__FILE_STATUS_LOG_QUEUE   *ptr_data);
void   Queue_FileStatusLogQueue_Set        (QUEUE__FILE_STATUS_LOG_QUEUE   *ptr_data);

/* file alarm       queue - get/set queue */
void   Queue_FileAlarmQueue_GetDefault     (QUEUE__FILE_ALARM_QUEUE        *ptr_data);
void   Queue_FileAlarmQueue_Get            (QUEUE__FILE_ALARM_QUEUE        *ptr_data);
void   Queue_FileAlarmQueue_Set            (QUEUE__FILE_ALARM_QUEUE        *ptr_data);




#endif
