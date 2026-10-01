/*=============================================================================
 * File       :  LOG_STATUS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - status log
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 *  T1: date period
 *
 *  |-----|-------------|------------|--------------|------------|------------|------------|------------|------------|------------|------------|-----------------|
 *  |  T1 | byte record | record/day | record/month | byte/1 day | byte/2 day | byte/3 day | byte/4 day | byte/5 day | byte/6 day | byte/7 day | byte/month      |
 *  |-----|-------------|------------|--------------|------------|------------|------------|------------|------------|------------|------------|-----------------|
 *  |  1M | 79          | 1440       | 43200        | 113760 (*) | 227520     | 341280     | 455040     | 568800     | 682560     | 796320     | 3412800 ~ 3332K |
 *  |  2M | 79          |  720       | 21600        |  56880 (*) | 113760     | 170640     | 227520     | 284400     | 341280     | 398160     | 1706400 ~ 1666K |
 *  |  3M | 79          |  480       | 14400        |  37920 (*) |  75840     | 113760     | 151680     | 189600     | 227520     | 265440     | 1137600 ~ 1110K |
 *  |  4M | 79          |  360       | 10800        |  28440 (*) |  56880     |  85320     | 113760     | 142200     | 170640     | 199080     |  853200 ~  833K |
 *  |  5M | 79          |  288       |  8640        |  22752 (*) |  45504     |  68256     |  91008     | 113760     | 136512     | 159264     |  682560 ~  666K |
 *  |  6M | 79          |  240       |  7200        |  18960 (*) |  37920     |  56880     |  75840     |  94800     | 113760     | 132720     |  568800 ~  555K |
 *  | 10M | 79          |  144       |  4320        |  11376 (*) |  22752 (*) |  34128 (*) |  45504 (*) |  56880 (*) |  68256 (*) |  79632 (*) |  341280 ~  333K |
 *  | 12M | 79          |  120       |  3600        |  18960 (*) |  18960 (*) |  28440 (*) |  37920 (*) |  47400 (*) |  56880 (*) |  66360 (*) |  568800 ~  555K |
 *  | 15M | 79          |   96       |  2880        |   7584 (*) |  15168 (*) |  22752 (*) |  30336 (*) |  37920 (*) |  45504 (*) |  53088 (*) |  227520 ~  222K |
 *  | 20M | 79          |   72       |  2160        |   5688 (*) |  11376 (*) |  17064 (*) |  22752 (*) |  28440 (*) |  34128 (*) |  39816 (*) |  170640 ~  166K |
 *  | 30M | 79          |   48       |  1440        |   3792 (*) |   7584 (*) |  11376 (*) |  15168 (*) |  18960 (*) |  22752 (*) |  26544 (*) |  113760 ~  111K |
 *  | 60M | 79          |   24       |   720        |   1896 (*) |   3792 (*) |   5688 (*) |   7584 (*) |   9480 (*) |  11376 (*) |  13272 (*) |   56880 ~   55K |
 *  |  1H | 79          |   24       |   720        |   1896 (*) |   3792 (*) |   5688 (*) |   7584 (*) |   9480 (*) |  11376 (*) |  13272 (*) |   56880 ~   55K |
 *  |  2H | 79          |   12       |   360        |    948 (*) |   1896 (*) |   2844 (*) |   3792 (*) |   4740 (*) |   5688 (*) |   6636 (*) |   28440 ~   27K |
 *  |  3H | 79          |    8       |   240        |    632 (*) |   1264 (*) |   1896 (*) |   2528 (*) |   3160 (*) |   3792 (*) |   4424 (*) |   18960 ~   18K |
 *  |  4H | 79          |    6       |   180        |    474 (*) |    948 (*) |   1422 (*) |   1896 (*) |   2370 (*) |   2844 (*) |   3318 (*) |   14220 ~   13K |
 *  |  6H | 79          |    4       |   120        |    316 (*) |    632 (*) |    948 (*) |   1264 (*) |   1580 (*) |   1896 (*) |   2212 (*) |    9480 ~    9K |
 *  |  8H | 79          |    3       |    90        |    237 (*) |    474 (*) |    711 (*) |    948 (*) |   1185 (*) |   1422 (*) |   1659 (*) |    7110 ~    6K |
 *  | 12H | 79          |    2       |    60        |    158 (*) |    316 (*) |    474 (*) |    632 (*) |    790 (*) |    948 (*) |   1106 (*) |    4740 ~    4K |
 *  | 24H | 79          |    1       |    30        |     79 (*) |    158 (*) |    237 (*) |    316 (*) |    395 (*) |    474 (*) |    553 (*) |    2370 ~    2K |
 *  |-----|-------------|------------|--------------|------------|------------|------------|------------|------------|------------|------------|-----------------|
 *
 *  (*) Settings allowed
 */

/*
 *  DATE;TIME;C1;C1_F1;C1_F2;C1_F3;C1_BAND_TYPE;C2;C2_F1;C2_F2;C2_F3;C2_BAND_TYPE;IN1;IN2;OUT1;OUT2;TINT;TEXT;CREG;CGREG;RSSI;BER;POWER;CREDIT;VERSION;CHECKSUM
 *  21/07/2015;10:17:00;1234567890;1234567890;1234567890;1234567890;F1;1234567890;1234567890;1234567890;1234567890;F1;-;-;0;0;-10.3;-10.3;1;0;24;99;1;999;1;6E
 *
 *  Header     size: 155+2 = 157 bytes
 *  Max record size: 154+2 = 156 bytes
 */


/*
 *  |------------|--------------|------------------|---------------|
 *  | Min Period | Max Duration | Max Num. Records | Max File Size |
 *  |------------|--------------|------------------|---------------|
 *  |     1 min  |     24 h     |        1440      |    224797     |    [ 157 + (1440 * 156) = 224797 ]
 *  |    10 min  |    168 h     |        1008      |    157405     |    [ 157 + (1008 * 156) = 157405 ]
 *  |------------|--------------|------------------|---------------|
 */





/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "clock.h"
#include "debug_my.h"
#include "file_csv.h"
#include "file_system.h"
#include "log_status.h"
#include "queue_my.h"
#include "rtc_alarm.h"
#include "transmission_gprs.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* task message ID */
#define TASK_MSG_ID__LOG_STATUS_DURATION   0     /* log status duration  */
#define TASK_MSG_ID__LOG_STATUS_PERIOD     1     /* log status period    */
#define TASK_MSG_ID__NEW_CONFIGURATION     2     /* new configuration    */
#define TASK_MSG_ID__FORCE_TX              4     /* force transmission   */
#define TASK_MSG_ID__CLOCK_CHANGED         5     /* clock changed        */
#define TASK_MSG_ID__LOG_STATUS_FILE_SENT  6     /* log status file sent */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING            400


/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define LOG_STATUS__MAIL_SIZE              68 + 1

/* message queue size */
#define MESSAGE_QUEUE_SIZE                 5




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                          // message id
    u32 length_data;                    // length of additional data
    u8  data[LOG_STATUS__MAIL_SIZE];    //           additional data
} LOG_STATUS__MAIL;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "status log" */
static       LOG_STATUS__CONFIG__STATUS_LOG LogStatus_Config_StatusLog;
static const LOG_STATUS__CONFIG__STATUS_LOG LogStatus_Config_StatusLogDefault =
{
    FALSE,   /* enable status  */
    0,       /* period   (min) */
    0,       /* duration (min) */
};

/* debug string */
static       ascii                          LogStatus_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* message queue handles */
static       ql_queue_t                     LogStatus_MessageQueueHandler_MessageQueueEvents;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void   LogStatus_TaskLogStatus(void *argument);

/* events */
       void   LogStatus_Event_LogStatusDuration(CLOCK__TIME *ptr_rtc_rime);
       void   LogStatus_Event_LogStatusPeriod  (CLOCK__TIME *ptr_rtc_rime);
       void   LogStatus_Event_NewConfiguration (void);
       void   LogStatus_Event_ForceTransmission(void);
       void   LogStatus_Event_ClockChanged     (void);
       void   LogStatus_Event_LogStatusFileSent(ascii *file_name);

/* set next RTC alarm period/duration */
static void   LogStatus_SetNewRtcAlarmLogStatusDuration(void);
static void   LogStatus_SetNewRtcAlarmLogStatusPeriod  (void);

/* next log status period/duration */
static bool   LogStatus_NextLogStatusDuration(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_log_status_duration);
static bool   LogStatus_NextLogStatusPeriod  (CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_log_status_period  );

/* log file */
static void   LogStatus_LogFile_CreateFile      (CLOCK__TIME *ptr_start_time, CLOCK__TIME *ptr_stop_time);
static void   LogStatus_LogFile_MarkFileToSend  (void);
static void   LogStatus_LogFile_AppendHeader    (void);
       void   LogStatus_LogFile_AppendDataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2);
static void   LogStatus_LogFile_DeleteFile      (ascii *file_name);

/* file system */
static void   LogStatus_FileSystem_PrepareFreeSpace(void);

/* send log files */
static void   LogStatus_SendLogFile(void);

/* get/set "status log" configuration */
       void   LogStatus_Config_StatusLog_GetDefault(LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);
       void   LogStatus_Config_StatusLog_Get       (LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);
       void   LogStatus_Config_StatusLog_Set       (LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);
       bool   LogStatus_Config_StatusLog_IsValid   (LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void   LogStatus_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);




/*===========================================================================
 * Function   : LogStatus_TaskLogStatus
 *
 * Description: task status log
 * Input      : -
 * Output     : -
 *===========================================================================*/
void LogStatus_TaskLogStatus(void *argument)
{
    LOG_STATUS__MAIL message_received;
    QlOSStatus       err;

    CLOCK__TIME      current_time;
    CLOCK__TIME      next_log_status_duration;

    bool             result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - LOG_STATUS - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


#ifdef FUNCTION_IMPLEMENTED
    /* subscribe the current task to the File System service (to do once per task using File System) */
    err = adl_fsEnterFS();
    if (err != ADL_FS_NO_ERROR)
    {
        snprintf(LogStatus_DebugString, sizeof(LogStatus_DebugString), "adl_fsEnterFS ERROR: %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, LogStatus_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "adl_fsEnterFS OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
#endif


    if (LogStatus_Config_StatusLog.status)
    {
        /* status log enabled */

        result = FileSystem_FileBuild_Exist(FILE_SYSTEM__FILE_TYPE__LOG);
        if (!result)
        {
            /* the file "build" does not exist */

            /* prepare free space in the file system if there is not enough free space */
            LogStatus_FileSystem_PrepareFreeSpace();

            /* calculate start time and stop time */
            result = Clock_GetTime(&current_time);
            result = LogStatus_NextLogStatusDuration(&current_time, &next_log_status_duration);

            /* create a new status log file */
            LogStatus_LogFile_CreateFile(&current_time, &next_log_status_duration);

            /* append the header string to the status log file */
            LogStatus_LogFile_AppendHeader();
        }
    }


    /* set next RTC alarm (for next log status duration) */
    /* set next RTC alarm (for next log status period  ) */
    LogStatus_SetNewRtcAlarmLogStatusDuration();
    LogStatus_SetNewRtcAlarmLogStatusPeriod();


    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_rtos_queue_wait(LogStatus_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(LOG_STATUS__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            LogStatus_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*===========================================================================
 * Function   : LogStatus_Event_LogStatusDuration
 *
 * Description: signal the event time for log status storing
 * Input      : - ptr_rtc_rime: pointer to RTC time
 * Output     : -
 *===========================================================================*/
void LogStatus_Event_LogStatusDuration(CLOCK__TIME *ptr_rtc_rime)
{
    LOG_STATUS__MAIL message_to_be_sent;
    QlOSStatus       err;

    u8               i;


    message_to_be_sent.event       = TASK_MSG_ID__LOG_STATUS_DURATION;
    message_to_be_sent.length_data = 8;
    message_to_be_sent.data[0]     = (u8)(ptr_rtc_rime->year >> 8);
    message_to_be_sent.data[1]     = (u8)(ptr_rtc_rime->year     );
    message_to_be_sent.data[2]     =      ptr_rtc_rime->month;
    message_to_be_sent.data[3]     =      ptr_rtc_rime->day;
    message_to_be_sent.data[4]     =      ptr_rtc_rime->hour;
    message_to_be_sent.data[5]     =      ptr_rtc_rime->minute;
    message_to_be_sent.data[6]     =      ptr_rtc_rime->second;
    message_to_be_sent.data[7]     =      ptr_rtc_rime->week_day;
    for (i = 8; i < (68 + 1); i++)
         message_to_be_sent.data[i] = 0x00;

    err = ql_rtos_queue_release(LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : LogStatus_Event_LogStatusPeriod
 *
 * Description: signal the event time for log status storing
 * Input      : - ptr_rtc_rime: pointer to RTC time
 * Output     : -
 *===========================================================================*/
void LogStatus_Event_LogStatusPeriod(CLOCK__TIME *ptr_rtc_rime)
{
    LOG_STATUS__MAIL message_to_be_sent;
    QlOSStatus       err;

    u8               i;


    message_to_be_sent.event       = TASK_MSG_ID__LOG_STATUS_PERIOD;
    message_to_be_sent.length_data = 8;
    message_to_be_sent.data[0]     = (u8)(ptr_rtc_rime->year >> 8);
    message_to_be_sent.data[1]     = (u8)(ptr_rtc_rime->year     );
    message_to_be_sent.data[2]     =      ptr_rtc_rime->month;
    message_to_be_sent.data[3]     =      ptr_rtc_rime->day;
    message_to_be_sent.data[4]     =      ptr_rtc_rime->hour;
    message_to_be_sent.data[5]     =      ptr_rtc_rime->minute;
    message_to_be_sent.data[6]     =      ptr_rtc_rime->second;
    message_to_be_sent.data[7]     =      ptr_rtc_rime->week_day;
    for (i = 8; i < (68 + 1); i++)
         message_to_be_sent.data[i] = 0x00;

    err = ql_rtos_queue_release(LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : LogStatus_Event_NewConfiguration
 *
 * Description: signal the event new configuration
 * Input      : -
 * Output     : -
 *===========================================================================*/
void LogStatus_Event_NewConfiguration(void)
{
    LOG_STATUS__MAIL message_to_be_sent;
    QlOSStatus       err;

    u8               i;


    message_to_be_sent.event       = TASK_MSG_ID__NEW_CONFIGURATION;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < (68 + 1); i++)
         message_to_be_sent.data[i] = 0x00;

    err = ql_rtos_queue_release(LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : LogStatus_Event_ForceTransmission
 *
 * Description: signal the event force transmission
 * Input      : -
 * Output     : -
 *===========================================================================*/
void LogStatus_Event_ForceTransmission(void)
{
    LOG_STATUS__MAIL message_to_be_sent;
    QlOSStatus       err;

    u8               i;


    message_to_be_sent.event       = TASK_MSG_ID__FORCE_TX;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < (68 + 1); i++)
         message_to_be_sent.data[i] = 0x00;

    err = ql_rtos_queue_release(LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : LogStatus_Event_ClockChanged
 *
 * Description: signal the event clock changed
 * Input      : -
 * Output     : -
 *===========================================================================*/
void LogStatus_Event_ClockChanged(void)
{
    LOG_STATUS__MAIL message_to_be_sent;
    QlOSStatus       err;

    u8               i;


    message_to_be_sent.event       = TASK_MSG_ID__CLOCK_CHANGED;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < (68 + 1); i++)
        message_to_be_sent.data[i] = 0x00;

    err = ql_rtos_queue_release(LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : LogStatus_Event_LogStatusFileSent
 *
 * Description: signal the event log status file sent
 * Input      : - file_name: file name of file sent
 * Output     : -
 *===========================================================================*/
void LogStatus_Event_LogStatusFileSent(ascii *file_name)
{
    LOG_STATUS__MAIL message_to_be_sent;
    QlOSStatus       err;

    u8               i;


    message_to_be_sent.event       = TASK_MSG_ID__LOG_STATUS_FILE_SENT;
    message_to_be_sent.length_data = 68 + 1;
    for (i = 0; i < (68 + 1); i++)
        message_to_be_sent.data[i] = (u8)(*(file_name + i));

    err = ql_rtos_queue_release(LogStatus_MessageQueueHandler_MessageQueueEvents, sizeof(LOG_STATUS__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : LogStatus_SetNewRtcAlarmLogStatusDuration
 *
 * Description: Set next RTC alarm (for next log status duration)
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void LogStatus_SetNewRtcAlarmLogStatusDuration(void)
{
    CLOCK__TIME current_time;
    CLOCK__TIME next_duration_time;

    bool        result;


    /* get current time */
    result = Clock_GetTime(&current_time);

    /* calculate the next log status duration */
    result = LogStatus_NextLogStatusDuration(&current_time, &next_duration_time);
    if (result)
    {
        /* next log status duration available (status log enabled) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Next log status duration available"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Clock_PrintTime2(DEBUG_TRACE_LEVEL_LOG_STATUS, 0, 0, &next_duration_time);

        /* set RTC alarm to "next_duration_time" */
        result = RtcAlarm_RtcAlarmSet(RTC_ALARM__ALARM_LOG_STATUS_DURATION, &next_duration_time);
    }
    else
    {
        /* next log status duration not available (status log disabled) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Next log status duration not available", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* delete RTC alarm */
        result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_LOG_STATUS_DURATION);
    }
}




/*===========================================================================
 * Function   : LogStatus_SetNewRtcAlarmLogStatusPeriod
 *
 * Description: Set next RTC alarm (for next log status period)
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void LogStatus_SetNewRtcAlarmLogStatusPeriod(void)
{
    CLOCK__TIME current_time;
    CLOCK__TIME next_period_time;

    bool        result;


    /* get current time */
    result = Clock_GetTime(&current_time);

    /* calculate the next log status period */
    result = LogStatus_NextLogStatusPeriod(&current_time, &next_period_time);
    if (result)
    {
        /* next log status period available (status log enabled) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Next log status period available"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Clock_PrintTime2(DEBUG_TRACE_LEVEL_LOG_STATUS, 0, 0, &next_period_time);

        /* set RTC alarm to "next_period_time" */
        result = RtcAlarm_RtcAlarmSet(RTC_ALARM__ALARM_LOG_STATUS_PERIOD, &next_period_time);
    }
    else
    {
        /* next log status period not available (status log disabled) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Next log status period not available", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* delete RTC alarm */
        result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_LOG_STATUS_PERIOD);
    }
}




/*===========================================================================
 * Function   : LogStatus_NextLogStatusDuration
 *
 * Description: determine next log status duration
 *
 *                 |----------|----------------------------------------------------------------------------------------------|
 *                 | duration | 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 |
 *                 |----------|----------------------------------------------------------------------------------------------|
 *                 |    1d    | x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  |
 *                 |    2d    |    x     x     x     x     x     x     x     x     x     x     x     x     x     x     x  X  |
 *                 |    3d    |       x        x        x        x        x        x        x        x        x        x  X  |
 *                 |    4d    |          x           x           x           x           x           x           x           |
 *                 |    5d    |             x              x              x              x              x              x  X  |
 *                 |    6d    |                x                 x                 x                 x                 x  X  |
 *                 |    7d    |                   x                    x                    x                    x        X  |
 *                 |----------|----------------------------------------------------------------------------------------------|
 *
 * Input      : - ptr_actual_time             : pointer to      actual                   time
 *              - ptr_next_log_status_duration: pointer to save next log status duration time
 * Output     : - FALSE: next log time not available
 *              - TRUE : next log time     available
 *===========================================================================*/
bool LogStatus_NextLogStatusDuration(CLOCK__TIME *ptr_actual_time, CLOCK__TIME *ptr_next_log_status_duration)
{
    /* actual time */
          CLOCK__TIME actual_time;

    /* date */
          u16         year;
          u8          month;
          u8          day;

    /* time */
          u8          hour;
          u8          minute;
          u8          second;

    /* last day of the month */
          u8          last_day;

    /* duration */
          u16         duration;
          u16         duration_min;
          u16         duration_hour;
          u16         duration_day;

          u8          i;

    /* duration values allowed (min) */
    const u16         duration_allowed[] =
    {
        10,
        12,
        15,
        20,
        30,
        60,      // =   1h
        120,     // =   2h
        180,     // =   3h
        240,     // =   4h
        360,     // =   6h
        480,     // =   8h
        720,     // =  12h
        1440,    // =  24h = 1d
        2880,    // =  48h = 2d
        4320,    // =  72h = 3d
        5760,    // =  96h = 4d
        7200,    // = 120h = 5d
        8640,    // = 144h = 6d
        10080,   // = 168h = 7d
    };


    /* verify if it is disabled */
    if (!LogStatus_Config_StatusLog.status)
        return FALSE;  // it is disabled


    /* copy actual time */
    actual_time = *ptr_actual_time;


    /* verify if the duration value is an allowed value */
    duration = LogStatus_Config_StatusLog.duration;
    for (i = 0; i < (sizeof(duration_allowed) / sizeof(u16)); i++)
    {
        if (duration == duration_allowed[i])
            break;
    }
    if (i == (sizeof(duration_allowed) / sizeof(u16)))
    {
        /* duration value not allowed */
        duration = 1440;
    }


    duration_min  = duration;
    duration_hour = duration / (     60);
    duration_day  = duration / (24 * 60);


    /* determine next log status duration time (depending on the duration value and the actual time) */
    if      (duration_min < 60)
    {
        /* duration < 60 (= 1h) */
        second = 0;
        minute = (u8)duration_min  * ((actual_time.minute / (u8)duration_min ) + 1);
        hour   = actual_time.hour;
        day    = actual_time.day;
        month  = actual_time.month;
        year   = actual_time.year;
    }
    else if (duration_min < 1440)
    {
        /* 60 (= 1h) <= duration < 1440 (= 1d) */
        second = 0;
        minute = 0;
        hour   = (u8)duration_hour * ((actual_time.hour   / (u8)duration_hour) + 1);
        day    = actual_time.day;
        month  = actual_time.month;
        year   = actual_time.year;
    }
    else
    {
        /* 1440 (= 1d) <= duration <= 168 (= 7d) */
        second = 0;
        minute = 0;
        hour   = 0;
        day    = 1;
        for (i = 1; i <= 31; i++)
        {
            if (((i * (u8)duration_day) + 1) > actual_time.day)
            {
                day = (i * (u8)duration_day) + 1;
                break;
            }
        }
        month  = actual_time.month;
        year   = actual_time.year;
    }


    /* check if a hour, day, month, year is finished */
    last_day = Clock_DaysInAMonth(month, year);
    if (minute > 59)
    {
        minute = 0;
        hour   = actual_time.hour + 1;
    }
    if (hour   > 23)
    {
        hour   = 0;
        day++;
    }
    if (day    > last_day)
    {
        day    = 1;
        month++;
    }
    if (month  > 12)
    {
        month  = 1;
        year++;
    }


    /* save next log time */

    ptr_next_log_status_duration->second   = second;
    ptr_next_log_status_duration->minute   = minute;
    ptr_next_log_status_duration->hour     = hour;

    ptr_next_log_status_duration->day      = day;
    ptr_next_log_status_duration->month    = month;
    ptr_next_log_status_duration->year     = year;

    ptr_next_log_status_duration->week_day = Clock_WeekDayOfADay(day, month, year);


    return TRUE;
}




/*===========================================================================
 * Function   : LogStatus_NextLogStatusPeriod
 *
 * Description: determine next log status period
 * Input      : - ptr_actual_time           : pointer to      actual                 time
 *              - ptr_next_log_status_period: pointer to save next log status period time
 * Output     : - FALSE: next log time not available
 *              - TRUE : next log time     available
 *===========================================================================*/
bool LogStatus_NextLogStatusPeriod(CLOCK__TIME *ptr_actual_time, CLOCK__TIME *ptr_next_log_status_period)
{
    /* actual time */
          CLOCK__TIME actual_time;

    /* date */
          u16         year;
          u8          month;
          u8          day;

    /* time */
          u8          hour;
          u8          minute;
          u8          second;

    /* last day of the month */
          u8          last_day;

    /* period */
          u16         period;
          u16         period_min;
          u16         period_hour;

          u8          i;

    /* period values allowed (min) */
    const u16         period_allowed[] =
    {
        1,
        2,
        3,
        4,
        5,
        6,
        10,
        12,
        15,
        20,
        30,
        60,    // =  1h
        120,   // =  2h
        180,   // =  3h
        240,   // =  4h
        360,   // =  6h
        480,   // =  8h
        720,   // = 12h
        1440,  // = 24h = 1d
    };


    /* verify if it is disabled */
    if (!LogStatus_Config_StatusLog.status)
        return FALSE;  // it is disabled


    /* copy actual time */
    actual_time = *ptr_actual_time;


    /* verify if the period value is an allowed value */
    period = LogStatus_Config_StatusLog.period;
    for (i = 0; i < (sizeof(period_allowed) / sizeof(u16)); i++)
    {
        if (period == period_allowed[i])
            break;
    }
    if (i == (sizeof(period_allowed) / sizeof(u16)))
    {
        /* period value not allowed */
        period = 60;
    }


    period_min  = period;
    period_hour = period / 60;


    /* determine next log status period time (depending on the period value and the actual time) */
    if      (period_min < 60)
    {
        /* period < 60 (= 1h) */
        second = 0;
        minute = period_min  * ((actual_time.minute / period_min ) + 1);
        hour   = actual_time.hour;
        day    = actual_time.day;
        month  = actual_time.month;
        year   = actual_time.year;
    }
    else if (period_min < 1440)
    {
        /* 60 (= 1h) <= period < 1440 (= 24h) */
        second = 0;
        minute = 0;
        hour   = period_hour * ((actual_time.hour   / period_hour) + 1);
        day    = actual_time.day;
        month  = actual_time.month;
        year   = actual_time.year;
    }
    else
    {
        /* period == 1440 (= 24h) */
        second = 0;
        minute = 0;
        hour   = 0;
        day    = actual_time.day + 1;
        month  = actual_time.month;
        year   = actual_time.year;
    }


    /* check if a hour, day, month, year is finished */
    last_day = Clock_DaysInAMonth(month, year);
    if (minute > 59)
    {
        minute = 0;
        hour   = actual_time.hour + 1;
    }
    if (hour   > 23)
    {
        hour   = 0;
        day++;
    }
    if (day    > last_day)
    {
        day    = 1;
        month++;
    }
    if (month  > 12)
    {
        month  = 1;
        year++;
    }


    /* save next log time */

    ptr_next_log_status_period->second   = second;
    ptr_next_log_status_period->minute   = minute;
    ptr_next_log_status_period->hour     = hour;

    ptr_next_log_status_period->day      = day;
    ptr_next_log_status_period->month    = month;
    ptr_next_log_status_period->year     = year;

    ptr_next_log_status_period->week_day = Clock_WeekDayOfADay(day, month, year);


    return TRUE;
}




/*===========================================================================
 * Function   : LogStatus_LogFile_CreateFile
 *
 * Description: - create a new status log file
 * Input      : - ptr_start_time: pointer to start time
 *              - ptr_stop_time : pointer to stop  time
 * Output     : -
 *===========================================================================*/
static void LogStatus_LogFile_CreateFile(CLOCK__TIME *ptr_start_time, CLOCK__TIME *ptr_stop_time)
{
    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* create a new status log file */
    result = FileSystem_FileBuild_Create(FILE_SYSTEM__FILE_TYPE__LOG, ptr_start_time, ptr_stop_time);
}




/*===========================================================================
 * Function   : LogStatus_LogFile_MarkFileToSend
 *
 * Description: mark the current status log file for transmission
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void LogStatus_LogFile_MarkFileToSend(void)
{
    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* change the status of the current status log file */
    result = FileSystem_FileBuild_ChangeStatus(FILE_SYSTEM__FILE_TYPE__LOG, 0);
}




/*===========================================================================
 * Function   : LogStatus_LogFile_AppendHeader
 *
 * Description: append the header string to the status log file
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void LogStatus_LogFile_AppendHeader(void)
{
    ascii *header_string;
    bool   result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* build the header string */
    header_string = FileCsv_BuildString_Header();

    snprintf(LogStatus_DebugString, sizeof(LogStatus_DebugString), "header_string: %s", header_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, LogStatus_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* append the header string to the created file */
    result = FileSystem_FileBuild_AppendString(FILE_SYSTEM__FILE_TYPE__LOG, header_string);
}




/*===========================================================================
 * Function   : LogStatus_LogFile_AppendDataRecord
 *
 * Description: append a new data record string to the status log file
 * Input      : - rec_type     : record type
 *              - rec_subtype_1: record subtype 1
 *              - rec_subtype_2: record subtype 2
 * Output     : -
 *===========================================================================*/
void LogStatus_LogFile_AppendDataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2)
{
    ascii *record_string;
    bool   result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* build a new data record string */
    record_string = FileCsv_BuildString_DataRecord(rec_type, rec_subtype_1, rec_subtype_2);

    snprintf(LogStatus_DebugString, sizeof(LogStatus_DebugString), "record_string: %s", record_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, LogStatus_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* append a new data record string to status log the file */
    result = FileSystem_FileBuild_AppendString(FILE_SYSTEM__FILE_TYPE__LOG, record_string);
}




/*===========================================================================
 * Function   : LogStatus_LogFile_DeleteFile
 *
 * Description: delete a specified status log file
 * Input      : - file_name: file name of the file to be deleted
 * Output     : -
 *===========================================================================*/
static void LogStatus_LogFile_DeleteFile(ascii *file_name)
{
    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    result = FileSystem_DeleteFiles_FileName(file_name);
}




/*===========================================================================
 * Function   : LogStatus_FileSystem_PrepareFreeSpace
 *
 * Description: prepare free space in the file system if there is not enough
 *              free space
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void LogStatus_FileSystem_PrepareFreeSpace(void)
{
    ascii file_name_oldest[68 + 1];

    bool  result;


    do
    {
        /* check if there is enough free space in the file system for log status file */
        result = FileSystem_FreeSpaceExists(FILE_SYSTEM__FILE_TYPE__LOG);
        if (!result)
        {
            /* there is not enough free space in the file system */

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Not enough free space in the file system for log status file", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* find oldest log status file in the file system */
            result = FileSystem_FileOldest_Type(FILE_SYSTEM__FILE_TYPE__LOG, file_name_oldest);
            if (!result)
                break;  // exit from "do-while"

            /* delete oldest log status file in the file system (if found) */
            result = FileSystem_DeleteFiles_FileName(file_name_oldest);
            if (!result)
                break;  // exit from "do-while"
        }
        else
        {
            /* there is     enough free space in the file system for log status file */

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Enough free space in the file system for log status file"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            break;  // exit from "do-while"
        }
    } while (1);
}




/*===========================================================================
 * Function   : LogStatus_SendLogFile
 *
 * Description: put the info of the status log file to be transmitted
 *              in the file log status queue
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void LogStatus_SendLogFile(void)
{
    QUEUE__FILE_STATUS_LOG_QUEUE_RECORD file_status_loq_queue_record;
    ascii                               file_name[68 + 1];

    CLOCK__TIME                         current_time;
    CLOCK__TIME                         start_time;
    CLOCK__TIME                         stop_time;

    bool                                result_1;
    bool                                result_2;
    bool                                result_3;
    bool                                result_4;


    /* get the current time */
    result_1 = Clock_GetTime(&current_time);

    /* get the "file close" info */
    result_2 = FileSystem_FileClose_GetFileName (FILE_SYSTEM__FILE_TYPE__LOG, file_name  );
    result_3 = FileSystem_FileClose_GetTimeStart(FILE_SYSTEM__FILE_TYPE__LOG, &start_time);
    result_4 = FileSystem_FileClose_GetTimeStop (FILE_SYSTEM__FILE_TYPE__LOG, &stop_time );


    if (result_1 && result_2 && result_3 && result_4)
    {
        /* build a status log file for the file log status queue */

        snprintf(LogStatus_DebugString, sizeof(LogStatus_DebugString), "file_name: %s", file_name);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, LogStatus_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Clock_PrintTime2(DEBUG_TRACE_LEVEL_LOG_STATUS, 0, 0, &start_time);
        Clock_PrintTime2(DEBUG_TRACE_LEVEL_LOG_STATUS, 1, 0, &stop_time );


        /* change the status of the current file "close" to "send" */
        //@@@
        result_1 = FileSystem_FileClose_ChangeStatus(FILE_SYSTEM__FILE_TYPE__LOG, 0);


        //@@@
        file_name[56] = 's';
        file_name[57] = 'e';
        file_name[58] = 'n';
        file_name[59] = 'd';

        file_name[60] = ')';

        file_name[61] = '.';

        file_name[62] = 'c';
        file_name[63] = 's';
        file_name[64] = 'v';

        file_name[65] = 0x00;


        strncpy(file_status_loq_queue_record.log_file_name, file_name, 68);   /* log file name */
        file_status_loq_queue_record.log_file_name[68] = 0x00;

        file_status_loq_queue_record.time       = current_time;                  /* time             */
        file_status_loq_queue_record.v          = FILE_CSV__FILE_VERSION;        /* log file version */
        file_status_loq_queue_record.n          = 0;                             /* log file number  */
        file_status_loq_queue_record.start_time = start_time;                    /* start time       */
        file_status_loq_queue_record.stop_time  = stop_time;                     /* stop  time       */

        /* put a status log file in the file log status queue */
        //@@@
        result_1 = Queue_FileStatusLog_PutRecord((QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *)&file_status_loq_queue_record);

        /* signal the presence of a new data to be transmitted */
        TransmissionGprs_Event_DataTx();
    }
}




/*===========================================================================
 * Function   : LogStatus_Config_StatusLog_GetDefault
 *
 * Description: get the default "status log" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void LogStatus_Config_StatusLog_GetDefault(LOG_STATUS__CONFIG__STATUS_LOG *ptr_data)
{
    *ptr_data = LogStatus_Config_StatusLogDefault;
}




/*===========================================================================
 * Function   : LogStatus_Config_StatusLog_Get
 *
 * Description: get the "status log" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void LogStatus_Config_StatusLog_Get(LOG_STATUS__CONFIG__STATUS_LOG *ptr_data)
{
    *ptr_data = LogStatus_Config_StatusLog;
}




/*===========================================================================
 * Function   : LogStatus_Config_StatusLog_Set
 *
 * Description: set the "status log" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void LogStatus_Config_StatusLog_Set(LOG_STATUS__CONFIG__STATUS_LOG *ptr_data)
{
    LogStatus_Config_StatusLog = *ptr_data;
}




/*===========================================================================
 * Function   : LogStatus_Config_StatusLog_IsValid
 *
 * Description: check if the "status log" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool LogStatus_Config_StatusLog_IsValid(LOG_STATUS__CONFIG__STATUS_LOG *ptr_data)
{
    /* period values allowed (min) */
    const u16 period_allowed[] =
    {
        1,
        2,
        3,
        4,
        5,
        6,
        10,
        12,
        15,
        20,
        30,
        60,      // =  1h
        120,     // =  2h
        180,     // =  3h
        240,     // =  4h
        360,     // =  6h
        480,     // =  8h
        720,     // = 12h
        1440,    // = 24h = 1d
    };

    /* duration values allowed (min) */
    const u16 duration_allowed[] =
    {
        10,
        12,
        15,
        20,
        30,
        60,      // =   1h
        120,     // =   2h
        180,     // =   3h
        240,     // =   4h
        360,     // =   6h
        480,     // =   8h
        720,     // =  12h
        1440,    // =  24h = 1d
        2880,    // =  48h = 2d
        4320,    // =  72h = 3d
        5760,    // =  96h = 4d
        7200,    // = 120h = 5d
        8640,    // = 144h = 6d
        10080,   // = 168h = 7d
    };

          u8  i;


    /* verify period value */
    for (i = 0; i < (sizeof(period_allowed) / sizeof(u16)); i++)
    {
        if (ptr_data->period == period_allowed[i])
            break;
    }
    if (i == (sizeof(period_allowed) / sizeof(u16)))
    {
        if (ptr_data->status)
        {
            return FALSE;   // period value not allowed
        }
        else
        {
            if (ptr_data->period != 0)
                return FALSE;
        }
    }


    /* verify duration value */
    for (i = 0; i < (sizeof(duration_allowed) / sizeof(u16)); i++)
    {
        if (ptr_data->duration == duration_allowed[i])
            break;
    }
    if (i == (sizeof(duration_allowed) / sizeof(u16)))
    {
        if (ptr_data->status)
        {
            return FALSE;   // duration value not allowed
        }
        else
        {
            if (ptr_data->duration != 0)
                return FALSE;
        }
    }


    /* verify period/duration values */
    if (ptr_data->period > (ptr_data->duration * 60))
        return FALSE;   // period > duration

    /* verify period/duration values */
    if ((ptr_data->period < 10) && (ptr_data->duration > 24))
        return FALSE;   // (period < 10 min)  &&  (duration > 24 hours)


    return TRUE;
}




/*===========================================================================
 * Function   : LogStatus_AdlCallback_Message_TaskMsg
 *
 * Description: task log status callback
 * Input      : - msg_identifier:
 *              - source        :
 *              - length        :
 *              - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void LogStatus_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    ascii       file_name[68 + 1];

    CLOCK__TIME rtc_time;

    CLOCK__TIME current_time;
    CLOCK__TIME next_log_status_duration;

    bool        result;
    u8          i;


    /* debug */
    snprintf(LogStatus_DebugString, sizeof(LogStatus_DebugString), "CALLBACK     - MESSAGE     - TASK LOG STATUS - msg identifier: %lu, length: %lu", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, LogStatus_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the data length */
    switch (msg_identifier)
    {
        /* log status duration */
        /* log status period   */
        case TASK_MSG_ID__LOG_STATUS_DURATION:
        case TASK_MSG_ID__LOG_STATUS_PERIOD:
            if (length != 8)
                return;
            break;

        /* new configuration  */
        /* clock changed      */
        /* force transmission */
        case TASK_MSG_ID__NEW_CONFIGURATION:
        case TASK_MSG_ID__CLOCK_CHANGED:
        case TASK_MSG_ID__FORCE_TX:
            if (length != 0)
                return;
            break;

        /* log status file sent */
        case TASK_MSG_ID__LOG_STATUS_FILE_SENT:
            if (length != (68 + 1))
                return;
            break;

        /* unknown message identifier */
        default:
            return;
    }



    switch (msg_identifier)
    {
        /* log status duration */
        case TASK_MSG_ID__LOG_STATUS_DURATION:
            /* extract the parameters */
            rtc_time.year     = ((u16)*(ptr_data    ) << 8) |
                                ((u16)*(ptr_data + 1)     );
            rtc_time.month    =       *(ptr_data + 2);
            rtc_time.day      =       *(ptr_data + 3);
            rtc_time.hour     =       *(ptr_data + 4);
            rtc_time.minute   =       *(ptr_data + 5);
            rtc_time.second   =       *(ptr_data + 6);
            rtc_time.week_day =       *(ptr_data + 7);

            Clock_PrintTime2(DEBUG_TRACE_LEVEL_LOG_STATUS, 0, 0, &rtc_time);


            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Log status duration", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* mark the status log file for transmission */
            LogStatus_LogFile_MarkFileToSend();

            /* put the info of the status log file in the file log status queue */
            LogStatus_SendLogFile();

            if (LogStatus_Config_StatusLog.status)
            {
                /* status log enabled */

                result = FileSystem_FileBuild_Exist(FILE_SYSTEM__FILE_TYPE__LOG);
                if (!result)
                {
                    /* the file "build" does not exist */

                    /* prepare free space in the file system if there is not enough free space */
                    LogStatus_FileSystem_PrepareFreeSpace();

                    /* calculate start time and stop time */
                    result = Clock_GetTime(&current_time);
                    result = LogStatus_NextLogStatusDuration(&current_time, &next_log_status_duration);

                    /* create a new status log file */
                    LogStatus_LogFile_CreateFile(&current_time, &next_log_status_duration);

                    /* append the header string to the status log file */
                    LogStatus_LogFile_AppendHeader();
                }
            }

            /* set next RTC alarm (for next log status duration) */
            LogStatus_SetNewRtcAlarmLogStatusDuration();
            break;



        /* log status period */
        case TASK_MSG_ID__LOG_STATUS_PERIOD:
            /* extract the parameters */
            rtc_time.year     = ((u16)*(ptr_data    ) << 8) |
                                ((u16)*(ptr_data + 1)     );
            rtc_time.month    =       *(ptr_data + 2);
            rtc_time.day      =       *(ptr_data + 3);
            rtc_time.hour     =       *(ptr_data + 4);
            rtc_time.minute   =       *(ptr_data + 5);
            rtc_time.second   =       *(ptr_data + 6);
            rtc_time.week_day =       *(ptr_data + 7);

            Clock_PrintTime2(DEBUG_TRACE_LEVEL_LOG_STATUS, 0, 0, &rtc_time);


            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Log status period", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (LogStatus_Config_StatusLog.status)
            {
                /* status log enabled */

                result = FileSystem_FileBuild_Exist(FILE_SYSTEM__FILE_TYPE__LOG);
                if (!result)
                {
                    /* the file "build" does not exist */

                    /* prepare free space in the file system if there is not enough free space */
                    LogStatus_FileSystem_PrepareFreeSpace();

                    /* calculate start time and stop time */
                    result = Clock_GetTime(&current_time);
                    result = LogStatus_NextLogStatusDuration(&current_time, &next_log_status_duration);

                    /* create a new status log file */
                    LogStatus_LogFile_CreateFile(&current_time, &next_log_status_duration);

                    /* append the header string to the status log file */
                    LogStatus_LogFile_AppendHeader();
                }

                /* append a new data record string to status log the file */
                LogStatus_LogFile_AppendDataRecord(FILE_CSV__REC_TYPE__LOG, 0, 0);   // subtype 1 and subtype 2 not used);

            }

            /* set next RTC alarm (for next log status period) */
            LogStatus_SetNewRtcAlarmLogStatusPeriod();
            break;



        /* new configuration */
        case TASK_MSG_ID__NEW_CONFIGURATION:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "New \"log status\" configuration", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* mark the status log file for transmission */
            LogStatus_LogFile_MarkFileToSend();

            /* put the info of the status log file in the file log status queue */
            LogStatus_SendLogFile();

            if (LogStatus_Config_StatusLog.status)
            {
                /* status log enabled */

                result = FileSystem_FileBuild_Exist(FILE_SYSTEM__FILE_TYPE__LOG);
                if (!result)
                {
                    /* the file "build" does not exist */

                    /* prepare free space in the file system if there is not enough free space */
                    LogStatus_FileSystem_PrepareFreeSpace();

                    /* calculate start time and stop time */
                    result = Clock_GetTime(&current_time);
                    result = LogStatus_NextLogStatusDuration(&current_time, &next_log_status_duration);

                    /* create a new status log file */
                    LogStatus_LogFile_CreateFile(&current_time, &next_log_status_duration);

                    /* append the header string to the status log file */
                    LogStatus_LogFile_AppendHeader();
                }
            }

            /* set next RTC alarm (for next log status duration) */
            /* set next RTC alarm (for next log status period  ) */
            LogStatus_SetNewRtcAlarmLogStatusDuration();
            LogStatus_SetNewRtcAlarmLogStatusPeriod();
            break;



        /* force transmission */
        case TASK_MSG_ID__FORCE_TX:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Force transmission", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* mark the status log file for transmission */
            LogStatus_LogFile_MarkFileToSend();

            /* put the info of the status log file in the file log status queue */
            LogStatus_SendLogFile();

            if (LogStatus_Config_StatusLog.status)
            {
                /* status log enabled */

                result = FileSystem_FileBuild_Exist(FILE_SYSTEM__FILE_TYPE__LOG);
                if (!result)
                {
                    /* the file "build" does not exist */

                    /* prepare free space in the file system if there is not enough free space */
                    LogStatus_FileSystem_PrepareFreeSpace();

                    /* calculate start time and stop time */
                    result = Clock_GetTime(&current_time);
                    result = LogStatus_NextLogStatusDuration(&current_time, &next_log_status_duration);

                    /* create a new status log file */
                    LogStatus_LogFile_CreateFile(&current_time, &next_log_status_duration);

                    /* append the header string to the status log file */
                    LogStatus_LogFile_AppendHeader();
                }
            }

            /* set next RTC alarm (for next log status duration) */
            /* set next RTC alarm (for next log status period  ) */
            LogStatus_SetNewRtcAlarmLogStatusDuration();
            LogStatus_SetNewRtcAlarmLogStatusPeriod();
            break;



        /* clock changed */
        case TASK_MSG_ID__CLOCK_CHANGED:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Clock changed", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            // NOTE: clock change does not involve the sending of the status log file

            /* mark the status log file for transmission */
          //LogStatus_LogFile_MarkFileToSend();

            /* put the info of the status log file in the file log status queue */
          //LogStatus_SendLogFile();

          //if (LogStatus_Config_StatusLog.status)
          //{
          //    /* status log enabled */
          //
          //    result = FileSystem_FileBuild_Exist(FILE_SYSTEM__FILE_TYPE__LOG);
          //    if (!result)
          //    {
          //        /* the file "build" does not exist */
          //
          //        /* prepare free space in the file system if there is not enough free space */
          //        LogStatus_FileSystem_PrepareFreeSpace();
          //
          //        /* calculate start time and stop time */
          //        result = Clock_GetTime(&current_time);
          //        result = LogStatus_NextLogStatusDuration(&current_time, &next_log_status_duration);
          //
          //        /* create a new status log file */
          //        LogStatus_LogFile_CreateFile(&current_time, &next_log_status_duration);
          //
          //        /* append the header string to the status log file */
          //        LogStatus_LogFile_AppendHeader();
          //    }
          //}

            /* set next RTC alarm (for next log status duration) */
            /* set next RTC alarm (for next log status period  ) */
            LogStatus_SetNewRtcAlarmLogStatusDuration();
            LogStatus_SetNewRtcAlarmLogStatusPeriod();
            break;



        /* log status file sent */
        case TASK_MSG_ID__LOG_STATUS_FILE_SENT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LOG_STATUS, DEBUG_TRACE_TYPE_LOW, "Log status file sent", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* extract the parameters */
            for (i = 0; i < (68 + 1); i++)
                file_name[i] = *(ptr_data + i);

            /* delete the specified status log file */
            LogStatus_LogFile_DeleteFile(file_name);
            break;



        /* unknown event */
        default:
            return;
    }
}
