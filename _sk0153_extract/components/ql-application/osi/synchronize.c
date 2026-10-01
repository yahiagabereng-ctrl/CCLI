/*=============================================================================
 * File       :  SYNCHRONIZE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - clock synchronize
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 * The device send an SMS to himself (MYN: My Number).
 * It starts a   timer when it sends    the SMS (when it receives the sending confirm from the network: ADL_SMS_EVENT_SENDING_OK).
 * It stops  the timer when it receives the SMS.
 * So it measures the elapsed time for the SMS "sending-receiving".
 * Then it adds this eplased time from the "SMS sending time" indicated by the GSM operator in the received SMS.
 *
 * - actual_time : actual time
 * - time_sms_tx : time of SMS sending indicated by the GSM operator in the received SMS
 * - elapsed_time: measured elapsed time for the SMS "sending-receiving".
 *
 *   actual_time = time_sms_tx + elapsed_time
 */




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "calendar.h"
#include "clock.h"
#include "counters.h"
#include "debug_my.h"
#include "f_chrono.h"
#include "log_status.h"
#include "main.h"
#include "program_flash.h"
#include "synchronize.h"
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* task message IDs */
#define TASK_MSG_ID__SMS_TX        0     /* synchronize SMS trasmission */
#define TASK_MSG_ID__SMS_RX        1     /* synchronize SMS reception   */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING    200


/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define SYNCHRONIZE__MAIL_SIZE     sizeof(SYNCHRONIZE__SMS_COUNTER) + sizeof(CLOCK__TIME)

/* message queue size */
#define MESSAGE_QUEUE_SIZE         5




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                           // message id
    u32 length_data;                     // length of additional data
    u8  data[SYNCHRONIZE__MAIL_SIZE];    //           additional data
} SYNCHRONIZE__MAIL;




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* indication of the transmitted synchronize SMS counter */
static       bool                          Synchronize_SmsCounterTxAcquired = FALSE;


/* synchronize SMS counter */
static       SYNCHRONIZE__SMS_COUNTER      Synchronize_SmsCounter;
static const SYNCHRONIZE__SMS_COUNTER      Synchronize_SmsCounterDefault =
{
    0
};

/* synchronize time */
static       SYNCHRONIZE__SYNCHRONIZE_TIME Synchronize_SynchronizeTime;
static const SYNCHRONIZE__SYNCHRONIZE_TIME Synchronize_SynchronizeTimeDefault =
{
    {
        /* date */
        2000,
        1,
        1,

        /* time */
        0,
        0,
        0,

        /* day of the week */
        6    // Saturday
    }
};


/* debug string */
static       ascii                         Synchronize_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* message queue handles */
static       ql_queue_t                    Synchronize_MessageQueueHandler_MessageQueueEvents;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Synchronize_TaskSynchronize(void *argument);

/* get/set synchronize SMS counter */
       void Synchronize_SmsCounter_GetDefault     (SYNCHRONIZE__SMS_COUNTER      *ptr_data);
       void Synchronize_SmsCounter_Get            (SYNCHRONIZE__SMS_COUNTER      *ptr_data);
       void Synchronize_SmsCounter_Set            (SYNCHRONIZE__SMS_COUNTER      *ptr_data);

/* get/set synchronize time */
       void Synchronize_SynchronizeTime_GetDefault(SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data);
       void Synchronize_SynchronizeTime_Get       (SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data);
       void Synchronize_SynchronizeTime_Set       (SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data);

/* events from external */
       void Synchronize_Event_SmsTx(SYNCHRONIZE__SMS_COUNTER sms_counter);
       void Synchronize_Event_SmsRx(SYNCHRONIZE__SMS_COUNTER sms_counter, CLOCK__TIME *ptr_time_sms_tx);

/* synchronize */
       void Synchronize_SynchronizeRtcTime(CLOCK__TIME *ptr_actual_time, bool synch_to_do);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Synchronize_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);




/*===========================================================================
 * Function   : Synchronize_TaskSynchronize
 *
 * Description: task
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Synchronize_TaskSynchronize(void *argument)
{
    SYNCHRONIZE__MAIL message_received;
    QlOSStatus        err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - SYNCHRONIZE - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* indication of the transmitted synchronize SMS counter */
    Synchronize_SmsCounterTxAcquired = FALSE;


    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&Synchronize_MessageQueueHandler_MessageQueueEvents, sizeof(SYNCHRONIZE__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_rtos_queue_wait(Synchronize_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(SYNCHRONIZE__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            Synchronize_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*===========================================================================
 * Function   : Synchronize_SmsCounter_GetDefault
 *
 * Description: get the default synchronize SMS counter
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Synchronize_SmsCounter_GetDefault(SYNCHRONIZE__SMS_COUNTER *ptr_data)
{
    *ptr_data = Synchronize_SmsCounterDefault;
}




/*===========================================================================
 * Function   : Synchronize_SmsCounter_Get
 *
 * Description: get the synchronize SMS counter
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Synchronize_SmsCounter_Get(SYNCHRONIZE__SMS_COUNTER *ptr_data)
{
    *ptr_data = Synchronize_SmsCounter;
}




/*===========================================================================
 * Function   : Synchronize_SmsCounter_Set
 *
 * Description: set the synchronize SMS counter
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Synchronize_SmsCounter_Set(SYNCHRONIZE__SMS_COUNTER *ptr_data)
{
    Synchronize_SmsCounter = *ptr_data;
}




/*===========================================================================
 * Function   : Synchronize_SynchronizeTime_GetDefault
 *
 * Description: get the default synchronize time
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Synchronize_SynchronizeTime_GetDefault(SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data)
{
    *ptr_data = Synchronize_SynchronizeTimeDefault;
}




/*===========================================================================
 * Function   : Synchronize_SynchronizeTime_Get
 *
 * Description: get the synchronize time
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Synchronize_SynchronizeTime_Get(SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data)
{
    *ptr_data = Synchronize_SynchronizeTime;
}




/*===========================================================================
 * Function   : Synchronize_SynchronizeTime_Set
 *
 * Description: set the synchronize time
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Synchronize_SynchronizeTime_Set(SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data)
{
    Synchronize_SynchronizeTime = *ptr_data;
}




/*=============================================================================
 * Function   : Synchronize_Event_SmsTx
 *
 * Description: signal the synchronize SMS transmission
 * Input      : - sms_counter: synchronize SMS counter transmitted
 * Output     : -
 *=============================================================================*/
void Synchronize_Event_SmsTx(SYNCHRONIZE__SMS_COUNTER sms_counter)
{
    SYNCHRONIZE__MAIL message_to_be_sent;
    QlOSStatus        err;

    u8                i;


    message_to_be_sent.event       = TASK_MSG_ID__SMS_TX;
    message_to_be_sent.length_data = sizeof(SYNCHRONIZE__SMS_COUNTER);
    message_to_be_sent.data[0]     = (u8)(sms_counter.sms_counter     );
    message_to_be_sent.data[1]     = (u8)(sms_counter.sms_counter >> 8);
    for (i = 2; i < SYNCHRONIZE__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(Synchronize_MessageQueueHandler_MessageQueueEvents, sizeof(SYNCHRONIZE__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Synchronize_Event_SmsRx
 *
 * Description: signal the synchronize SMS reception
 * Input      : - sms_counter    : synchronize SMS counter received
 *              - ptr_time_sms_tx: pointer to SMS transmission time
 * Output     : -
 *=============================================================================*/
void Synchronize_Event_SmsRx(SYNCHRONIZE__SMS_COUNTER sms_counter, CLOCK__TIME *ptr_time_sms_tx)
{
    SYNCHRONIZE__MAIL  message_to_be_sent;
    QlOSStatus         err;

    u8                *ptr_data;
    u8                 i;
    u8                 j;


    message_to_be_sent.event = TASK_MSG_ID__SMS_RX;

    message_to_be_sent.length_data = sizeof(SYNCHRONIZE__SMS_COUNTER) + sizeof(CLOCK__TIME);

    j = 0;

    ptr_data = (u8 *)&sms_counter;
    for (i = 0; i < sizeof(SYNCHRONIZE__SMS_COUNTER); i++)
        message_to_be_sent.data[j++] = *ptr_data++;

    ptr_data = (u8 *)ptr_time_sms_tx;
    for (i = 0; i < sizeof(CLOCK__TIME             ); i++)
        message_to_be_sent.data[j++] = *ptr_data++;


    err = ql_rtos_queue_release(Synchronize_MessageQueueHandler_MessageQueueEvents, sizeof(SYNCHRONIZE__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Synchronize_SynchronizeRtcTime
 *
 * Description: set RTC time with the actual time
 * Input      : - ptr_actual_time: pointer to actual RTC time
 *              - synch_to_do    : "synchronize to do" indication
 * Output     : -
 *===========================================================================*/
void Synchronize_SynchronizeRtcTime(CLOCK__TIME *ptr_actual_time, bool synch_to_do)
{
    CLOCK__TIME actual_time;
    bool        result;


    /* set RTC time with the actual time (only if not debug mode) */
    if (synch_to_do)
    {
        result = Clock_SetTime(ptr_actual_time);
        if (result)
        {
            /* signal the event to internal chrono function */
            /* signal the event to external chrono function */
            FChrono_ChronoEventInt();
            FChrono_ChronoEventExt();

            /* signal to counters */
            Counters_Event_ClockChanged();

            /* signal to log status */
            LogStatus_Event_ClockChanged();

            /* signal to calendar */
            Calendar_Event_ClockChanged();
        }
    }


    /* get current time */
    if (synch_to_do)
    {
        actual_time = *ptr_actual_time;
    }
    else
    {
        ql_rtos_task_sleep_ms(300L);

        result = Clock_GetTime(&actual_time);
        if (!result)
            actual_time = *ptr_actual_time;
    }

    /* save synchronize time */
    Synchronize_SynchronizeTime.synchronize_time = actual_time;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE, PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME);


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW , "Clock synchronization done", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* signal the end of the synchronize */
    Main_EndOfSynchronize();
}




/*===========================================================================
 * Function   : Synchronize_AdlCallback_Message_TaskMsg
 *
 * Description: task synchronize message callback
 * Input      : - msg_identifier:
 *              - length        :
 *              - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void Synchronize_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    /* OS time */
    static s64                      os_time_start_ms;   /* OS start time           (ms) */
           s64                      os_time_stop_ms;    /* OS stop  time           (ms) */
           s64                      elapsed_time_ms;    /* elapsed  time since ... (ms) */

    /* synchronize SMS counter */
    static SYNCHRONIZE__SMS_COUNTER sms_counter_tx;
    static SYNCHRONIZE__SMS_COUNTER sms_counter_rx;

    /* time */
           CLOCK__TIME              sms_tx_time;
           CLOCK__TIME              actual_time;

           bool                     synch_to_do;


    snprintf(Synchronize_DebugString, sizeof(Synchronize_DebugString), "CALLBACK     - MESSAGE     - TASK SYNCHRONIZE - msg_identifier: %ld, length: %ld", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW, Synchronize_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier)
    {
        /* synchronize SMS transmission */
        case TASK_MSG_ID__SMS_TX:
            if (length != sizeof(SYNCHRONIZE__SMS_COUNTER))
                return;

            /* get OS time (ms) */
            os_time_start_ms = ql_rtos_up_time_ms();

            /* the transmitted synchronize SMS counter has been acquired */
            Synchronize_SmsCounterTxAcquired = TRUE;

            /* extract the transmitted synchronize SMS counter */
            sms_counter_tx = *((SYNCHRONIZE__SMS_COUNTER *)ptr_data);
            break;


        /* synchronize SMS reception */
        case TASK_MSG_ID__SMS_RX:
            if (length != (sizeof(SYNCHRONIZE__SMS_COUNTER) + sizeof(CLOCK__TIME)))
                return;

            /* verify if the transmitted synchronize SMS counter has been acquired */
            if (!Synchronize_SmsCounterTxAcquired)
                return;

            /* get OS time (ms) */
            os_time_stop_ms = ql_rtos_up_time_ms();

            /* extract the received synchronize SMS counter */
            sms_counter_rx = *((SYNCHRONIZE__SMS_COUNTER *)(ptr_data                                   ));

            /* extract the received SMS transmission time */
            sms_tx_time    = *((CLOCK__TIME              *)(ptr_data + sizeof(SYNCHRONIZE__SMS_COUNTER)));

            /* calculate elapsed time (ms) */
            if (os_time_stop_ms >= os_time_start_ms)
                elapsed_time_ms =  (os_time_stop_ms  - os_time_start_ms);
            else
                elapsed_time_ms = -(os_time_start_ms - os_time_stop_ms );

            /* print start, stop and elapsed times */
            snprintf(Synchronize_DebugString, sizeof(Synchronize_DebugString), "os_time_start_ms: %lld ms,  os_time_stop_ms: %lld ms,  elapsed_time_ms: %lld ms", os_time_start_ms, os_time_stop_ms, elapsed_time_ms);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW , Synchronize_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (sms_counter_rx.sms_counter == sms_counter_tx.sms_counter)
            {
                /* received synchronize SMS counter equal to the transmitted one */

                /* calculate actual_time (actual_time = sms_tx_time + elapsed_time) */
                Clock_AddOffset(&sms_tx_time, &actual_time, (s32)(elapsed_time_ms / 1000));

                if (sms_counter_tx.sms_counter == (Synchronize_SmsCounterDefault.sms_counter + 1))
                    synch_to_do = TRUE;
                else
                    synch_to_do = FALSE;


                /* set RTC time with the actual time */
                Synchronize_SynchronizeRtcTime(&actual_time, synch_to_do);

                /* the transmitted synchronize SMS counter has not been acquired */
                Synchronize_SmsCounterTxAcquired = FALSE;

                /* signal the end of the synchronize */
                Main_EndOfSynchronize();
            }
            break;


        /* unknown message */
        default:
            break;
    }
}
