/*=============================================================================
 * File       :  ALARM.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - alarms
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdio.h>
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "alarm.h"
#include "boot.h"
#include "clock.h"
#include "debug_my.h"
#include "file_csv.h"
#include "file_system.h"
#include "language.h"
#include "log_status.h"
#include "program_flash.h"
#include "queue_my.h"
#include "status.h"
#include "strings_my.h"
#include "transmission_gprs.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* task message IDs */
#define TASK_MSG_ID__NEW_ALARM        0
#define TASK_MSG_ID__ALARM_FILE_SENT  1

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING       230


/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define ALARM__MAIL_SIZE              68 + 1

/* message queue size */
#define MESSAGE_QUEUE_SIZE            5




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                     // message id
    u32 length_data;               // length of additional data
    u8  data[ALARM__MAIL_SIZE];    //           additional data
} ALARM__MAIL;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii                     Alarm_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * Alarm counters
 *---------------------------------------------------------------------------*/
/* alarm counters */
static       ALARM__ALARM_COUNTERS     Alarm_AlarmCounters;
static const ALARM__ALARM_COUNTERS     Alarm_AlarmCountersDefault =
{
    /* total alarm counter */
    {
     // 1      2      3      4      5        6      7      8      9      10
        0,     0,     0,     0,     0,       0,     0,     0,     0,     0,      // 10
        0,     0,     0,     0,     0,       0,     0,     0,     0,     0,      // 20
    },


    /* yearly alarm counter */
    {
     // 1      2      3      4      5        6      7      8      9      10
        0,     0,     0,     0,     0,       0,     0,     0,     0,     0,      // 10
        0,     0,     0,     0,     0,       0,     0,     0,     0,     0,      // 20
    },


    /* monthly alarm counter */
    {
     // 1      2      3      4      5        6      7      8      9      10
        0,     0,     0,     0,     0,       0,     0,     0,     0,     0,      // 10
        0,     0,     0,     0,     0,       0,     0,     0,     0,     0,      // 20
    },
};



/*---------------------------------------------------------------------------
 * Alarm configuration
 *---------------------------------------------------------------------------*/
/* configuration - "alarm mode" */
static       ALARM__CONFIG__ALARM_MODE Alarm_Config_AlarmMode;
static const ALARM__CONFIG__ALARM_MODE Alarm_Config_AlarmModeDefault =
{
    ALARM__ALARM_MODE__SMS     // alarm mode
};


/*---------------------------------------------------------------------------
 * Alarm description strings
 *---------------------------------------------------------------------------*/
/* alarm descriptions strings */
static const u16                       Alarm_AlarmLongDescriptionStrings[] =
{
    STRINGS__ALARM__TMIN,                // ID =   1 - "Alarm internal temperature too low"
    STRINGS__ALARM__TMINEXT,             // ID =   2 - "Alarm external temperature too low"
    STRINGS__ALARM__TMIN_RESTORED,       // ID =   3 - "Alarm internal temperature too low restored"
    STRINGS__ALARM__TMINEXT_RESTORED,    // ID =   4 - "Alarm external temperature too low restored"
    STRINGS__ALARM__TMAX,                // ID =   5 - "Alarm internal temperature too high"
    STRINGS__ALARM__TMAXEXT,             // ID =   6 - "Alarm external temperature too high"
    STRINGS__ALARM__TMAX_RESTORED,       // ID =   7 - "Alarm internal temperature too high restored"
    STRINGS__ALARM__TMAXEXT_RESTORED,    // ID =   8 - "Alarm external temperature too high restored"
    STRINGS__ALARM__IN1,                 // ID =   9 - "Alarm IN1 input"
    STRINGS__ALARM__IN2,                 // ID =  10 - "Alarm IN2 input"
    STRINGS__ALARM__IN3,                 // ID =  11 - "Alarm IN3 input"
    STRINGS__ALARM__IN4,                 // ID =  12 - "Alarm IN4 input"
    STRINGS__ALARM__IN1_RESTORED,        // ID =  13 - "IN1 input restored"
    STRINGS__ALARM__IN2_RESTORED,        // ID =  14 - "IN2 input restored"
    STRINGS__ALARM__IN3_RESTORED,        // ID =  15 - "IN3 input restored"
    STRINGS__ALARM__IN4_RESTORED,        // ID =  16 - "IN4 input restored"
    STRINGS__ALARM__POWER_OFF,           // ID =  17 - "Mains power interruption alarm"
    STRINGS__ALARM__POWER_RETURN,        // ID =  18 - "Mains power returned"
    STRINGS__ALARM__CREDIT,              // ID =  19 - "Warning, credit is 10 SMS"
};

/* alarm short descriptions strings */
static const ascii                    *Alarm_AlarmShortDescriptionStrings[] =
{
    "T_MIN",                             // ID =   1 - internal temperature below minimum temperature
    "T_MIN_EXT",                         // ID =   2 - external temperature below minimum temperature
    "T_MIN_RESTORED",                    // ID =   3 - internal temperature below minimum temperature restored
    "T_MIN_EXT_RESTORED",                // ID =   4 - external temperature below minimum temperature restored
    "T_MAX",                             // ID =   5 - internal temperature above maximum temperature
    "T_MAX_EXT",                         // ID =   6 - external temperature above maximum temperature
    "T_MAX_RESTORED",                    // ID =   7 - internal temperature above maximum temperature restored
    "T_MAX_EXT_RESTORED",                // ID =   8 - external temperature above maximum temperature restored
    "IN1",                               // ID =   9 - digital input IN1
    "IN2",                               // ID =  10 - digital input IN2
    "IN3",                               // ID =  11 - digital input IN3
    "IN4",                               // ID =  12 - digital input IN4
    "IN1_RESTORED",                      // ID =  13 - digital input IN1 restored
    "IN2_RESTORED",                      // ID =  14 - digital input IN2 restored
    "IN3_RESTORED",                      // ID =  15 - digital input IN3 restored
    "IN4_RESTORED",                      // ID =  16 - digital input IN4 restored
    "POWER_OFF",                         // ID =  17 - main power
    "POWER_RETURN",                      // ID =  18 - main power restored
    "CREDIT",                            // ID =  19 - credit warning
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* message queue handles */
static       ql_queue_t                Alarm_MessageQueueHandler_MessageQueueEvents;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void   Alarm_TaskAlarm(void *argument);

/* events */
       bool   Alarm_Event_NewAlarm     (u8 alarm_type, u16 alarm_par);
       bool   Alarm_Event_AlarmFileSent(ascii *file_name);

/* new alarm */
static bool   Alarm_NewAlarm         (u8 alarm_type, u16 alarm_par);
static bool   Alarm_GenerateAlarmSms (u8 alarm_type, u16 alarm_par);
static bool   Alarm_GenerateAlarmMail(u8 alarm_type, u16 alarm_par);

/* alarm file */
static void   Alarm_AlarmFile_CreateFile      (CLOCK__TIME *ptr_start_time, CLOCK__TIME *ptr_stop_time);
static void   Alarm_AlarmFile_MarkFileToSend  (void);
static void   Alarm_AlarmFile_AppendHeader    (void);
       void   Alarm_AlarmFile_AppendDataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2);
static void   Alarm_AlarmFile_DeleteFile      (ascii *file_name);

/* send alarm files */
static void   Alarm_SendAlarmFile(u8 alarm_type, u16 alarm_par);

/* file system */
static void   Alarm_FileSystem_PrepareFreeSpace(void);

/* alarm string */
       ascii *Alarm_AlarmLongString (u8 alarm_type);
       ascii *Alarm_AlarmShortString(u8 alarm_type);

/* reset alarm counters */
       void   Alarm_AlarmCountersTotalReset  (void);
       void   Alarm_AlarmCountersYearlyReset (void);
       void   Alarm_AlarmCountersMonthlyReset(void);
       void   Alarm_AlarmCountersReset       (void);
       void   Alarm_AlarmCounterTotalReset   (u8 alarm_type);
       void   Alarm_AlarmCounterYearlyReset  (u8 alarm_type);
       void   Alarm_AlarmCounterMonthlyReset (u8 alarm_type);
       void   Alarm_AlarmCounterReset        (u8 alarm_type);

/* get/set alarm counters */
       void   Alarm_AlarmCounters_GetDefault(ALARM__ALARM_COUNTERS *ptr_data);
       void   Alarm_AlarmCounters_Get       (ALARM__ALARM_COUNTERS *ptr_data);
       void   Alarm_AlarmCounters_Set       (ALARM__ALARM_COUNTERS *ptr_data);

/* get/set "alarm mode" configuration */
       void   Alarm_Config_AlarmMode_GetDefault(ALARM__CONFIG__ALARM_MODE *ptr_data);
       void   Alarm_Config_AlarmMode_Get       (ALARM__CONFIG__ALARM_MODE *ptr_data);
       void   Alarm_Config_AlarmMode_Set       (ALARM__CONFIG__ALARM_MODE *ptr_data);
       bool   Alarm_Config_AlarmMode_IsValid   (ALARM__CONFIG__ALARM_MODE *ptr_data);

/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void   Alarm_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);




/*===========================================================================
 * Function   : Alarm_TaskAlarm
 *
 * Description: alarm task
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Alarm_TaskAlarm(void *argument)
{
    ALARM__MAIL message_received;
    QlOSStatus  err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - ALARM - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


#ifdef FUNCTION_IMPLEMENTED
    /* subscribe the current task to the File System service (to do once per task using File System) */
    err = adl_fsEnterFS();
    if (err != ADL_FS_NO_ERROR)
    {
        snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "adl_fsEnterFS ERROR: %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "adl_fsEnterFS OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
#endif

    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&Alarm_MessageQueueHandler_MessageQueueEvents, sizeof(ALARM__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_rtos_queue_wait(Alarm_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(ALARM__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            Alarm_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*===========================================================================
 * Function   : Alarm_Event_NewAlarm
 *
 * Description: signal a new alarm event
 * Input      : - alarm_type: alarm type ID
 *              - alarm_par : alarm parameter (optional)
 * Output     : - FALSE: alarm not signalled
 *              - TRUE : alarm     signalled
 *===========================================================================*/
bool Alarm_Event_NewAlarm(u8 alarm_type, u16 alarm_par)
{
    ALARM__MAIL message_to_be_sent;
    QlOSStatus  err;

    u8          i;


    /* verify alarm type value */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS))
        return FALSE;   // alarm type value not valid


    message_to_be_sent.event       = TASK_MSG_ID__NEW_ALARM;
    message_to_be_sent.length_data = 3;
    message_to_be_sent.data[0]     =       alarm_type;
    message_to_be_sent.data[1]     = (u8)((alarm_par >> 8) & 0x00FF);
    message_to_be_sent.data[2]     = (u8)((alarm_par     ) & 0x00FF);
    for (i = 3; i < ALARM__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(Alarm_MessageQueueHandler_MessageQueueEvents, sizeof(ALARM__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Alarm_Event_AlarmFileSent
 *
 * Description: signal the event alarm file sent
 * Input      : - file_name: file name of file sent
 * Output     : -
 *===========================================================================*/
bool Alarm_Event_AlarmFileSent(ascii *file_name)
{
    ALARM__MAIL message_to_be_sent;
    QlOSStatus  err;

    u8          i;


    message_to_be_sent.event        = TASK_MSG_ID__ALARM_FILE_SENT;
    message_to_be_sent.length_data  = 68 + 1;
    for (i = 0; i < (68 + 1); i++)
         message_to_be_sent.data[i] = (u8)(*(file_name + i));

    err = ql_rtos_queue_release(Alarm_MessageQueueHandler_MessageQueueEvents, sizeof(ALARM__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Alarm_NewAlarm
 *
 * Description: generate a new alarm and put it in the alarm queue.
 *              The alarm can not be generated if:
 *                   - parameters are not valid
 *                   - alarm type is not enabled
 *                   - alarm monthly counter has reached the maximum value
 * Input      : - alarm_type: alarm type ID
 *              - alarm_par : alarm parameter (optional)
 * Output     : - FALSE: alarm not generated and not put in the queue
 *              - TRUE : alarm     generated and     put in the queue
 *=============================================================================*/
static bool Alarm_NewAlarm(u8 alarm_type, u16 alarm_par)
{
    bool result;


    snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "New alarm - alarm type: %d (%d)", alarm_type, alarm_par);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* verify alarm type value */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS))
        return FALSE;   // alarm type value not valid


    /* increment total alarm counter, yearly alarm counter and monthly alarm counter */
    Alarm_AlarmCounters.monthly[alarm_type - 1]++;
    Alarm_AlarmCounters.yearly [alarm_type - 1]++;
    Alarm_AlarmCounters.total  [alarm_type - 1]++;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);


    /* generate the alarm */
    if (Alarm_Config_AlarmMode.mode == ALARM__ALARM_MODE__MAIL)
        result = Alarm_GenerateAlarmMail(alarm_type, alarm_par);
    else
        result = Alarm_GenerateAlarmSms (alarm_type, alarm_par);


    return result;
}




/*=============================================================================
 * Function   : Alarm_GenerateAlarmSms
 *
 * Description: generate a new alarm SMS and put it in the SMS alarm queue.
 * Input      : - alarm_type: alarm type ID
 *              - alarm_par : alarm parameter (optional)
 * Output     : - FALSE: alarm not generated and not put in the queue
 *              - TRUE : alarm     generated and     put in the queue
 *=============================================================================*/
static bool Alarm_GenerateAlarmSms(u8 alarm_type, u16 alarm_par)
{
    static ALARM__ALARM          alarm;
    static STATUS__DEVICE_STATUS device_status;
           bool                  result;

    (void)result;


    /* get device status */
    result = Status_GetDeviceStatus(&device_status);

    /* build alarm */
    alarm.alarm_data.alarm_id  = alarm_type;     // alarm type ID
    alarm.alarm_data.alarm_par = alarm_par;      // alarm parameter (optional)
    alarm.device_status        = device_status;  // alarm device status


    snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "Put the alarm in the alarm queue - alarm type: %d (%d)", alarm_type, alarm_par);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* put the alarm in the SMS alarm queue */
    result = Queue_Alarm_PutRecord((QUEUE__ALARM_QUEUE_RECORD *)&alarm);


    return TRUE;
}




/*=============================================================================
 * Function   : Alarm_GenerateAlarmMail
 *
 * Description: generate a new alarm mail and put it in the mail alarm queue.
 * Input      : - alarm_type: alarm type ID
 *              - alarm_par : alarm parameter (optional)
 * Output     : - FALSE: alarm not generated and not put in the queue
 *              - TRUE : alarm     generated and     put in the queue
 *=============================================================================*/
static bool Alarm_GenerateAlarmMail(u8 alarm_type, u16 alarm_par)
{
   static CLOCK__TIME current_time;
          bool        result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* calculate start time and stop time */
    result = Clock_GetTime(&current_time);


    /* prepare free space in the file system if there is not enough free space */
    Alarm_FileSystem_PrepareFreeSpace();


    /* create a new alarm file */
    Alarm_AlarmFile_CreateFile(&current_time, &current_time);

    /* append the header string to the alarm file */
    Alarm_AlarmFile_AppendHeader();

    /* append a new data record string to the alarm file */
    Alarm_AlarmFile_AppendDataRecord(FILE_CSV__REC_TYPE__ALARM, alarm_type, alarm_par);

    /* mark the alarm file for transmission */
    Alarm_AlarmFile_MarkFileToSend();


    /* put the info of the alarm file in the file alarm queue */
    Alarm_SendAlarmFile(alarm_type, alarm_par);


    return TRUE;
}




/*===========================================================================
 * Function   : Alarm_AlarmFile_CreateFile
 *
 * Description: - create a new alarm file
 * Input      : - ptr_start_time: pointer to start time
 *              - ptr_stop_time : pointer to stop  time
 * Output     : -
 *===========================================================================*/
static void Alarm_AlarmFile_CreateFile(CLOCK__TIME *ptr_start_time, CLOCK__TIME *ptr_stop_time)
{
    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* create a new status log file */
    result = FileSystem_FileBuild_Create(FILE_SYSTEM__FILE_TYPE__ALARM, ptr_start_time, ptr_stop_time);
}




/*===========================================================================
 * Function   : Alarm_AlarmFile_MarkFileToSend
 *
 * Description: mark the current alarm file for transmission
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Alarm_AlarmFile_MarkFileToSend(void)
{
    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* change the status of the current alarm file */
    result = FileSystem_FileBuild_ChangeStatus(FILE_SYSTEM__FILE_TYPE__ALARM, 0);
}




/*===========================================================================
 * Function   : Alarm_AlarmFile_AppendHeader
 *
 * Description: append the header string to the alarm file
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Alarm_AlarmFile_AppendHeader(void)
{
    ascii *header_string;
    bool   result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* build the header string */
    header_string = FileCsv_BuildString_Header();

    snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "header_string: %s", header_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* append the header string to the created file */
    result = FileSystem_FileBuild_AppendString(FILE_SYSTEM__FILE_TYPE__ALARM, header_string);
}




/*===========================================================================
 * Function   : Alarm_AlarmFile_AppendDataRecord
 *
 * Description: append a new data record string to the alarm file
 * Input      : - rec_type     : record type
 *              - rec_subtype_1: record subtype 1
 *              - rec_subtype_2: record subtype 2
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmFile_AppendDataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2)
{
    ascii *record_string;
    bool   result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* build a new data record string */
    record_string = FileCsv_BuildString_DataRecord(rec_type, rec_subtype_1, rec_subtype_2);

    snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "record_string: %s", record_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* append a new data record string to the file */
    result = FileSystem_FileBuild_AppendString(FILE_SYSTEM__FILE_TYPE__ALARM, record_string);
}




/*===========================================================================
 * Function   : Alarm_AlarmFile_DeleteFile
 *
 * Description: delete a specified alarm file
 * Input      : - file_name: file name of the file to be deleted
 * Output     : -
 *===========================================================================*/
static void Alarm_AlarmFile_DeleteFile(ascii *file_name)
{
    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    result = FileSystem_DeleteFiles_FileName(file_name);
}




/*===========================================================================
 * Function   : Alarm_SendAlarmFile
 *
 * Description: put the info of the alarm file to be transmitted
 *              in the file alarm queue
 * Input      : - alarm_type: alarm type ID
 *              - alarm_par : alarm parameter (optional)
 * Output     : -
 *===========================================================================*/
static void Alarm_SendAlarmFile(u8 alarm_type, u16 alarm_par)
{
    static QUEUE__FILE_ALARM_QUEUE_RECORD file_alarm_queue_record;
    static ascii                          file_name[68 + 1];

           CLOCK__TIME                    current_time;
           CLOCK__TIME                    start_time;
           CLOCK__TIME                    stop_time;

           bool                           result_1;
           bool                           result_2;
           bool                           result_3;
           bool                           result_4;


    /* get the current time */
    result_1 = Clock_GetTime(&current_time);

    /* get the "file close" info */
    result_2 = FileSystem_FileClose_GetFileName (FILE_SYSTEM__FILE_TYPE__ALARM, file_name  );
    result_3 = FileSystem_FileClose_GetTimeStart(FILE_SYSTEM__FILE_TYPE__ALARM, &start_time);
    result_4 = FileSystem_FileClose_GetTimeStop (FILE_SYSTEM__FILE_TYPE__ALARM, &stop_time );


    if (result_1 && result_2 && result_3 && result_4)
    {
        /* build a alarm file for the file alarm queue */

        snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "file_name: %s", file_name);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Clock_PrintTime2(DEBUG_TRACE_LEVEL_ALARM, 0, 0, &start_time);
        Clock_PrintTime2(DEBUG_TRACE_LEVEL_ALARM, 1, 0, &stop_time );


        /* change the status of the current file "close" to "send" */
        //@@@
        result_1 = FileSystem_FileClose_ChangeStatus(FILE_SYSTEM__FILE_TYPE__ALARM, 0);


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


        strncpy(file_alarm_queue_record.alarm_file_name, file_name, 68);   /* file name */
        file_alarm_queue_record.alarm_file_name[68] = 0x00;

        file_alarm_queue_record.time       = current_time;                    /* time            */
        file_alarm_queue_record.v          = FILE_CSV__FILE_VERSION;          /* file version    */
        file_alarm_queue_record.n          = 0;                               /* file number     */
        file_alarm_queue_record.start_time = start_time;                      /* start time      */
        file_alarm_queue_record.stop_time  = stop_time;                       /* stop  time      */
        file_alarm_queue_record.alarm_type = alarm_type;                      /* alarm type ID   */
        file_alarm_queue_record.alarm_par  = alarm_par;                       /* alarm parameter */


        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "Put the alarm in the file alarm queue", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* put a alarm file in the file alarm queue */
        //@@@
        result_1 = Queue_FileAlarm_PutRecord((QUEUE__FILE_ALARM_QUEUE_RECORD *)&file_alarm_queue_record);

        /* signal the presence of a new data to be transmitted */
        TransmissionGprs_Event_DataTx();
    }
}




/*===========================================================================
 * Function   : Alarm_FileSystem_PrepareFreeSpace
 *
 * Description: prepare free space in the file system if there is not enough
 *              free space
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Alarm_FileSystem_PrepareFreeSpace(void)
{
    static ascii file_name_oldest[68 + 1];

           bool  result;


    do
    {
        /* check if there is enough free space in the file system for alarm file */
        result = FileSystem_FreeSpaceExists(FILE_SYSTEM__FILE_TYPE__ALARM);
        if (!result)
        {
            /* there is not enough free space in the file system */

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "Not enough free space in the file system for alarm file", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* find oldest alarm file in the file system */
            result = FileSystem_FileOldest_Type(FILE_SYSTEM__FILE_TYPE__ALARM, file_name_oldest);
            if (!result)
                break;  // exit from "do-while"

            /* delete oldest alarm file in the file system (if found) */
            result = FileSystem_DeleteFiles_FileName(file_name_oldest);
            if (!result)
                break;  // exit from "do-while"
        }
        else
        {
            /* there is     enough free space in the file system for alarm file */

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "Enough free space in the file system for alarm file", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            break;  // exit from "do-while"
        }
    } while (1);
}




/*=============================================================================
 * Function   : Alarm_AlarmLongString
 *
 * Description: get the alarm long string of a specified alarm type ID
 * Input      : - alarm_type: alarm type ID
 * Output     : - pointer to alarm long string
 *=============================================================================*/
ascii *Alarm_AlarmLongString(u8 alarm_type)
{
    LANGUAGE__CONFIG__LANGUAGE  language_configuration;
    LANGUAGE__LANGUAGE          language;
    ascii                      *alarm_string;
    u16                         id_string;


    /* verify alarm type ID */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS))
    {
        alarm_string = "???";
        return alarm_string;
    }


    /* get language configuration */
    Language_Config_Language_Get(&language_configuration);

    language = language_configuration.language;

    /* verify language configuration */
    if (language >= LANGUAGE__NUMBERS_OF_LANGUAGES)
        language = LANGUAGE__LANGUAGE_ENGLISH;


    /* extract the alarm long string */
    id_string    = Alarm_AlarmLongDescriptionStrings[alarm_type - 1];
    alarm_string = Strings_GetString(id_string);

    return alarm_string;
}




/*=============================================================================
 * Function   : Alarm_AlarmShortString
 *
 * Description: get the alarm short string of a specified alarm type ID
 * Input      : - alarm_type: alarm type ID
 * Output     : - pointer to alarm short string
 *=============================================================================*/
ascii *Alarm_AlarmShortString(u8 alarm_type)
{
    ascii *alarm_string;


    /* verify alarm type ID */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS))
    {
        alarm_string = "???";
        return alarm_string;
    }


    /* extract the alarm short string */
    alarm_string = (ascii *)Alarm_AlarmShortDescriptionStrings[alarm_type - 1];

    return alarm_string;
}




/*===========================================================================
 * Function   : Alarm_AlarmCountersTotalReset
 *
 * Description: reset total alarm counter of all alarm type IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCountersTotalReset(void)
{
    u8 i;


    /* reset total alarm counters of all alarm type IDs */
    for (i = 0; i < ALARM__NUM_OF_ALARM_IDS_AVAILABLE; i++)
        Alarm_AlarmCounters.total[i] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCountersYearlyReset
 *
 * Description: reset yearly alarm counter of all alarm type IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCountersYearlyReset(void)
{
    u8 i;


    /* reset yearly alarm counters of all alarm type IDs */
    for (i = 0; i < ALARM__NUM_OF_ALARM_IDS_AVAILABLE; i++)
        Alarm_AlarmCounters.yearly[i] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCountersMonthlyReset
 *
 * Description: reset monthly alarm counter of all alarm type IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCountersMonthlyReset(void)
{
    u8 i;


    /* reset monthly alarm counters of all alarm type IDs */
    for (i = 0; i < ALARM__NUM_OF_ALARM_IDS_AVAILABLE; i++)
        Alarm_AlarmCounters.monthly[i] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCountersReset
 *
 * Description: reset total, yearly and monthly alarm counters of all alarm type IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCountersReset(void)
{
    u8 i;


    /* reset total, yearly and monthly alarm counters of all alarm type IDs */
    for (i = 0; i < ALARM__NUM_OF_ALARM_IDS_AVAILABLE; i++)
    {
        Alarm_AlarmCounters.total  [i] = 0;
        Alarm_AlarmCounters.yearly [i] = 0;
        Alarm_AlarmCounters.monthly[i] = 0;
    }

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCounterTotalReset
 *
 * Description: reset total alarm counter of a specified alarm type ID
 * Input      : - alarm_type: alarm type ID
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounterTotalReset(u8 alarm_type)
{
    /* verify alarm type value */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS_AVAILABLE))
        return;   // alarm type value not valid


    /* reset total alarm counter of the specified alarm type ID */
    Alarm_AlarmCounters.total[alarm_type - 1] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCounterYearlyReset
 *
 * Description: reset yearly alarm counter of a specified alarm type ID
 * Input      : - alarm_type: alarm type ID
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounterYearlyReset(u8 alarm_type)
{
    /* verify alarm type value */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS_AVAILABLE))
        return;   // alarm type value not valid


    /* reset yearly alarm counter of the specified alarm type ID */
    Alarm_AlarmCounters.yearly[alarm_type - 1] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCounterMonthlyReset
 *
 * Description: reset monthly alarm counter of a specified alarm type ID
 * Input      : - alarm_type: alarm type ID
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounterMonthlyReset(u8 alarm_type)
{
    /* verify alarm type value */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS_AVAILABLE))
        return;   // alarm type value not valid


    /* reset monthly alarm counter of the specified alarm type ID */
    Alarm_AlarmCounters.monthly[alarm_type - 1] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCounterReset
 *
 * Description: reset total, yearly and monthly alarm counters of a specified alarm type ID
 * Input      : - alarm_type: alarm type ID
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounterReset(u8 alarm_type)
{
    /* verify alarm type value */
    if ((alarm_type == 0) || (alarm_type > ALARM__NUM_OF_ALARM_IDS_AVAILABLE))
        return;   // alarm type value not valid


    /* reset total and monthly alarm counters of the specified alarm type ID */

    Alarm_AlarmCounters.total  [alarm_type - 1] = 0;
    Alarm_AlarmCounters.yearly [alarm_type - 1] = 0;
    Alarm_AlarmCounters.monthly[alarm_type - 1] = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS);
}




/*===========================================================================
 * Function   : Alarm_AlarmCounters_GetDefault
 *
 * Description: get the default alarm counters
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounters_GetDefault(ALARM__ALARM_COUNTERS *ptr_data)
{
    *ptr_data = Alarm_AlarmCountersDefault;
}




/*===========================================================================
 * Function   : Alarm_AlarmCounters_Get
 *
 * Description: get the alarm counters
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounters_Get(ALARM__ALARM_COUNTERS *ptr_data)
{
    *ptr_data = Alarm_AlarmCounters;
}




/*===========================================================================
 * Function   : Alarm_AlarmCounters_Set
 *
 * Description: set the alarm counters
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Alarm_AlarmCounters_Set(ALARM__ALARM_COUNTERS *ptr_data)
{
    Alarm_AlarmCounters = *ptr_data;
}




/*===========================================================================
 * Function   : Alarm_Config_AlarmMode_GetDefault
 *
 * Description: get the default "alarm mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Alarm_Config_AlarmMode_GetDefault(ALARM__CONFIG__ALARM_MODE *ptr_data)
{
    *ptr_data = Alarm_Config_AlarmModeDefault;
}




/*===========================================================================
 * Function   : Alarm_Config_AlarmMode_Get
 *
 * Description: get the "alarm mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Alarm_Config_AlarmMode_Get(ALARM__CONFIG__ALARM_MODE *ptr_data)
{
    *ptr_data = Alarm_Config_AlarmMode;
}




/*===========================================================================
 * Function   : Alarm_Config_AlarmMode_Set
 *
 * Description: set the "alarm mode" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Alarm_Config_AlarmMode_Set(ALARM__CONFIG__ALARM_MODE *ptr_data)
{
    Alarm_Config_AlarmMode = *ptr_data;
}




/*===========================================================================
 * Function   : Alarm_Config_AlarmMode_IsValid
 *
 * Description: check if the "alarm mode" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Alarm_Config_AlarmMode_IsValid(ALARM__CONFIG__ALARM_MODE *ptr_data)
{
    if (
         (ptr_data->mode != ALARM__ALARM_MODE__SMS ) &&
         (ptr_data->mode != ALARM__ALARM_MODE__MAIL)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Alarm_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - msg_identifier:
 *              - source        :
 *              - length        :
 *              - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void Alarm_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    ascii file_name[68 + 1];

    u8    alarm_type;
    u16   alarm_par;

    u8    i;


    /* debug */
    snprintf(Alarm_DebugString, sizeof(Alarm_DebugString), "CALLBACK     - MESSAGE     - TASK ALARM - msg identifier: %lu, length: %lu", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, Alarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the data length */
    switch (msg_identifier)
    {
        /* new alarm */
        case TASK_MSG_ID__NEW_ALARM:
            if (length != 3)
                return;
            break;

        /* alarm file sent */
        case TASK_MSG_ID__ALARM_FILE_SENT:
            if (length != (68 + 1))
                return;
            break;

        /* unknown message identifier */
        default:
            return;
    }


    switch (msg_identifier)
    {
        /* new alarm */
        case TASK_MSG_ID__NEW_ALARM:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "New alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* extract the parameters */
            alarm_type =             *(ptr_data    );
            alarm_par  = (s16)(((u16)*(ptr_data + 1) << 8) |
                               ((u16)*(ptr_data + 2)     ));

            /* new alarm */
            Alarm_NewAlarm(alarm_type, alarm_par);

            /* force the current data log file for transmission */
            LogStatus_Event_ForceTransmission();

            /* force a data log transmission */
            TransmissionGprs_Event_DataTxForce();
            break;


        /* alarm file sent */
        case TASK_MSG_ID__ALARM_FILE_SENT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM, DEBUG_TRACE_TYPE_LOW, "Alarm file sent", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* extract the parameters */
            for (i = 0; i < (68 + 1); i++)
                file_name[i] = *(ptr_data + i);

            /* delete the specified status log file */
            Alarm_AlarmFile_DeleteFile(file_name);
            break;


        /* unknown */
        default:
            break;
    }
}
