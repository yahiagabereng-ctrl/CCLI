/*=============================================================================
 * File       :  RTC_ALARM.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - RTC alarm
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"
#include "ql_api_rtc.h"

/* user     includes */
#include "boot.h"
#include "calendar.h"
#include "clock.h"
#include "counters.h"
#include "debug_my.h"
#include "f_chrono.h"
#include "fw_config.h"
#include "log_status.h"
#include "rtc_alarm.h"
#include "startup.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* RTC alarm descriptions */
#define RTC_ALARM_DESCRIPTION__NEAREST               "RTC ALARM NEAREST            "   // nearest RTC alarm
#define RTC_ALARM_DESCRIPTION__F_CHRONO_INT          "RTC ALARM F CHRONO INT       "   // internal chrono function
#define RTC_ALARM_DESCRIPTION__F_CHRONO_EXT          "RTC ALARM F CHRONO EXT       "   // external chrono function
#define RTC_ALARM_DESCRIPTION__SUMMER_TIME           "RTC ALARM SUMMER TIME        "   // summer time change
#define RTC_ALARM_DESCRIPTION__AUTOSYNCHRONIZE       "RTC ALARM AUTOSYNCHRONIZE    "   // clock autosychronize
#define RTC_ALARM_DESCRIPTION__LOG_STATUS_DURATION   "RTC ALARM LOG STATUS DURATION"   // log status duration
#define RTC_ALARM_DESCRIPTION__LOG_STATUS_PERIOD     "RTC ALARM LOG STATUS PERIOD  "   // log status period
#define RTC_ALARM_DESCRIPTION__INPUT_C1_BAND         "RTC ALARM INPUT C1 BAND      "   // input C1 band
#define RTC_ALARM_DESCRIPTION__INPUT_C2_BAND         "RTC ALARM INPUT C2 BAND      "   // input C2 band
#define RTC_ALARM_DESCRIPTION__OTHER                 "---                          "   // other

/* task message IDs */
#define TASK_MSG_ID__REQUEST_RTC_ALARM_SET           0     /* event - request RTC alarm set    */
#define TASK_MSG_ID__REQUEST_RTC_ALARM_DELETE        1     /* event - request RTC alarm delete */
#define TASK_MSG_ID__CALA_ALARM                      2     /* event - 'CALA' alarm             */
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
#define TASK_MSG_ID__TIMEOUT_DELAY_JUMP              3     /* event - timeout delay jump       */
#endif

#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
/* timeout for jump mode (sec) */
#define TIME_SEC__DELAY_FROM_RTC_ALARM_SETTING       ( 6)   /* delay  from         RTC alarm setting (sec) */
#define TIME_SEC__OFFSET_FROM_NEAREST_RTC_ALARM      (30)   /* offset from nearest RTC alarm         (sec) */
#endif

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                      200


/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define RTC_ALARM__MAIL_SIZE                         9

/* message queue size */
#define MESSAGE_QUEUE_SIZE                           10




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                         // message id
    u32 length_data;                   // length of additional data
    u8  data[RTC_ALARM__MAIL_SIZE];    //           additional data
} RTC_ALARM__MAIL;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* RTC alarms */
static RTC_ALARM__RTC_ALARM RtcAlarm_RtcAlarms[RTC_ALARM__MAX_NUM_OF_RTC_ALARMS];   // = ...

/* nearest RTC alarm */
static RTC_ALARM__RTC_ALARM RtcAlarm_NearestRtcAlarm;                               // = ...

/* CRC variables */
static u16                  RtcAlarm_VarCrc;                                        // = 0x0000;

/* debug string */
static ascii                RtcAlarm_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* message queue handles */
static ql_queue_t           RtcAlarm_MessageQueueHandler_MessageQueueEvents;

/* timer handlers */
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
static ql_timer_t           RtcAlarm_TimerHandler_TimerDelayJump;
#endif




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void   RtcAlarm_TaskRtcAlarm(void *argument);

/* status variables */
static void   RtcAlarm_InitVar        (void);
static u16    RtcAlarm_CalculateVarCrc(void);
       void   RtcAlarm_UpdateVarCrc   (void);
       bool   RtcAlarm_VerifyVarCrc   (void);

/* info */
       bool   RtcAlarm_GetRtcAlarms      (u8 rtc_alarm_id, RTC_ALARM__RTC_ALARM *ptr_rtc_alarm);
       bool   RtcAlarm_GetNearestRtcAlarm(                 RTC_ALARM__RTC_ALARM *ptr_rtc_alarm);

/* action on RTC alarms */
       bool   RtcAlarm_RtcAlarmSet   (u8 rtc_alarm_id, CLOCK__TIME *ptr_rtc_alarm_time);
       bool   RtcAlarm_RtcAlarmDelete(u8 rtc_alarm_id);

/* nearest RTC alarm */
static void   RtcAlarm_CalculateNearestRtcAlarm(void);

/* read RTC alarm description */
       ascii *RtcAlarm_ReadRtcAlarmDescription(u8 rtc_alarm_id);


/*-----------------------------------------------------------------------------
 * 'CALA' alarm
 *-----------------------------------------------------------------------------*/
/* set/delete 'CALA' alarm */
static bool   RtcAlarm_CalaAlarmSet   (CLOCK__TIME *ptr_cala_alarm_time);
static bool   RtcAlarm_CalaAlarmDelete(void);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
static void   RtcAlarm_TimerDelayJump_Start(u32 time_value, bool periodic);
static void   RtcAlarm_TimerDelayJump_Stop (void);
#endif


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message              callback functions */
static void RtcAlarm_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);

/* unsolicited response callback functions */
static void RtcAlarm_AdlCallback_UnsolicitedResponse_CALA(void);

/* timer                callback functions */
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
static void RtcAlarm_AdlCallback_Timer_TimerDelayJump(void *context);
#endif




/*=============================================================================
 * Function   : RtcAlarm_TaskRtcAlarm
 *
 * Description: RTC alarm task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void RtcAlarm_TaskRtcAlarm(void *argument)
{
    RTC_ALARM__MAIL  message_received;
    QlOSStatus       err;

    ql_errcode_rtc_e res_rtc;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - RTC_ALARM - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* "+CALA:" unsolicited response subscription */
    res_rtc = ql_rtc_register_cb(RtcAlarm_AdlCallback_UnsolicitedResponse_CALA);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtc_register_cb: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtc_register_cb: OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&RtcAlarm_MessageQueueHandler_MessageQueueEvents, sizeof(RTC_ALARM__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
    /* creation of timer "TimerDelayJump" */
    err = ql_rtos_timer_create(&RtcAlarm_TimerHandler_TimerDelayJump, QL_TIMER_IN_SERVICE, RtcAlarm_AdlCallback_Timer_TimerDelayJump, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
#endif


    /* internal status init */
    RtcAlarm_InitVar();


    for (;;)
    {
        err = ql_rtos_queue_wait(RtcAlarm_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(RTC_ALARM__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            RtcAlarm_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*=============================================================================
 * Function   : RtcAlarm_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void RtcAlarm_InitVar(void)
{
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
    CLOCK__TIME time_offset;
    s32         offset;
#endif

    bool        recover_status;

    u8          i;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* delete all RTC alarms */
        for (i = 0; i < RTC_ALARM__MAX_NUM_OF_RTC_ALARMS; i++)
        {
            RtcAlarm_RtcAlarms[i].status = FALSE;

            RtcAlarm_RtcAlarms[i].rtc_alarm_time.year     = 2000;
            RtcAlarm_RtcAlarms[i].rtc_alarm_time.month    = 1;
            RtcAlarm_RtcAlarms[i].rtc_alarm_time.day      = 1;
            RtcAlarm_RtcAlarms[i].rtc_alarm_time.hour     = 0;
            RtcAlarm_RtcAlarms[i].rtc_alarm_time.minute   = 0;
            RtcAlarm_RtcAlarms[i].rtc_alarm_time.second   = 0;
            RtcAlarm_RtcAlarms[i].rtc_alarm_time.week_day = 6;  // Saturday
        }


        /* delete nearest RTC alarm */
        RtcAlarm_NearestRtcAlarm.status = FALSE;

        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.year     = 2000;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.month    = 1;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.day      = 1;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.hour     = 0;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.minute   = 0;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.second   = 0;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.week_day = 6;  // Saturday


        /**** update the variable CRC ****/
        RtcAlarm_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        /* delete all 'CALA' alarms set in the module */
        RtcAlarm_CalaAlarmDelete();
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        /* delete all 'CALA' alarms set in the module */
        RtcAlarm_CalaAlarmDelete();

        /* set the nearest 'CALA' alarm in the module (if at least one 'CALA' alarm is set) */
        if (RtcAlarm_NearestRtcAlarm.status)
        {
            RtcAlarm_CalaAlarmSet(&RtcAlarm_NearestRtcAlarm.rtc_alarm_time);

#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
            RtcAlarm_TimerDelayJump_Stop();
            RtcAlarm_TimerDelayJump_Start((TIME_SEC__DELAY_FROM_RTC_ALARM_SETTING * 1000L), FALSE);
#endif
        }
    }
}




/*=============================================================================
 * Function   : RtcAlarm_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 RtcAlarm_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&RtcAlarm_RtcAlarms      , sizeof(RtcAlarm_RtcAlarms      ), crc);
    crc = Utility_CalculateCRC16((u8 *)&RtcAlarm_NearestRtcAlarm, sizeof(RtcAlarm_NearestRtcAlarm), crc);

    return crc;
}




/*=============================================================================
 * Function   : RtcAlarm_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void RtcAlarm_UpdateVarCrc(void)
{
    RtcAlarm_VarCrc = RtcAlarm_CalculateVarCrc();
}




/*=============================================================================
 * Function   : RtcAlarm_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool RtcAlarm_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = RtcAlarm_CalculateVarCrc();

    if (RtcAlarm_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : RtcAlarm_GetRtcAlarms
 *
 * Description: get a RTC alarm
 * Input      : - rtc_alarm_id : RTC alarm id [0 - (RTC_ALARM__MAX_NUM_OF_RTC_ALARMS-1)]
 *              - ptr_rtc_alarm: pointer to save RTC alarm got
 * Output     : - FALSE: RTC alarm not got
 *              - TRUE : RTC alarm     got
 *===========================================================================*/
bool RtcAlarm_GetRtcAlarms(u8 rtc_alarm_id, RTC_ALARM__RTC_ALARM *ptr_rtc_alarm)
{
    /* verify RTC alarm id value */
    if (rtc_alarm_id >= RTC_ALARM__MAX_NUM_OF_RTC_ALARMS)
    {
        ptr_rtc_alarm->status = FALSE;

        ptr_rtc_alarm->rtc_alarm_time.year     = 2000;
        ptr_rtc_alarm->rtc_alarm_time.month    = 1;
        ptr_rtc_alarm->rtc_alarm_time.day      = 1;
        ptr_rtc_alarm->rtc_alarm_time.hour     = 0;
        ptr_rtc_alarm->rtc_alarm_time.minute   = 0;
        ptr_rtc_alarm->rtc_alarm_time.second   = 0;
        ptr_rtc_alarm->rtc_alarm_time.week_day = 6;  // Saturday

        return FALSE;
    }


    *ptr_rtc_alarm = RtcAlarm_RtcAlarms[rtc_alarm_id];

    return TRUE;
}




/*===========================================================================
 * Function   : RtcAlarm_GetNearestRtcAlarm
 *
 * Description: get nearest RTC alarm
 * Input      : - ptr_rtc_alarm: pointer to save nearest RTC alarm got
 * Output     : - FALSE: nearest RTC alarm not got
 *              - TRUE : nearest RTC alarm     got
 *===========================================================================*/
bool RtcAlarm_GetNearestRtcAlarm(RTC_ALARM__RTC_ALARM *ptr_rtc_alarm)
{
    *ptr_rtc_alarm = RtcAlarm_NearestRtcAlarm;

    return TRUE;
}




/*===========================================================================
 * Function   : RtcAlarm_RtcAlarmSet
 *
 * Description: set a RTC alarm
 * Input      : - rtc_alarm_id      : RTC alarm id [0 - (RTC_ALARM__MAX_NUM_OF_RTC_ALARMS-1)]
 *              - ptr_rtc_alarm_time: pointer to RTC alarm time to be set
 * Output     : - FALSE: RTC alarm not set
 *              - TRUE : RTC alarm     set
 *===========================================================================*/
bool RtcAlarm_RtcAlarmSet(u8 rtc_alarm_id, CLOCK__TIME *ptr_rtc_alarm_time)
{
    CLOCK__TIME     rtc_actual_time;   /* RTC actual time */

    RTC_ALARM__MAIL message_to_be_sent;

    QlOSStatus      err;
    s8              result_s8;
    bool            result;


    /* verify RTC alarm id value */
    if (rtc_alarm_id >= RTC_ALARM__MAX_NUM_OF_RTC_ALARMS)
        return FALSE;


    /* get the actual time */
    result = Clock_GetTime(&rtc_actual_time);
    if (!result)
        return FALSE;

    /* verify RTC alarm time (not in the past or in the present) */
    result_s8 = Clock_CompareRtcTimes(ptr_rtc_alarm_time, &rtc_actual_time);
    if ((result_s8 == -1) || (result_s8 == 0))
        return FALSE;


    message_to_be_sent.event       = TASK_MSG_ID__REQUEST_RTC_ALARM_SET;
    message_to_be_sent.length_data = 9;
    message_to_be_sent.data[0]     = rtc_alarm_id;
    message_to_be_sent.data[1]     = (u8)(ptr_rtc_alarm_time->year >> 8);
    message_to_be_sent.data[2]     = (u8)(ptr_rtc_alarm_time->year     );
    message_to_be_sent.data[3]     =      ptr_rtc_alarm_time->month;
    message_to_be_sent.data[4]     =      ptr_rtc_alarm_time->day;
    message_to_be_sent.data[5]     =      ptr_rtc_alarm_time->hour;
    message_to_be_sent.data[6]     =      ptr_rtc_alarm_time->minute;
    message_to_be_sent.data[7]     =      ptr_rtc_alarm_time->second;
    message_to_be_sent.data[8]     =      ptr_rtc_alarm_time->week_day;

    err = ql_rtos_queue_release(RtcAlarm_MessageQueueHandler_MessageQueueEvents, sizeof(RTC_ALARM__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : RtcAlarm_RtcAlarmDelete
 *
 * Description: delete a RTC alarm
 * Input      : - rtc_alarm_id: RTC alarm id [0 - (RTC_ALARM__MAX_NUM_OF_RTC_ALARMS-1)]
 * Output     : - FALSE: RTC alarm not deleted
 *              - TRUE : RTC alarm     deleted
 *===========================================================================*/
bool RtcAlarm_RtcAlarmDelete(u8 rtc_alarm_id)
{
    RTC_ALARM__MAIL message_to_be_sent;
    QlOSStatus      err;

    u8              i;


    /* verify RTC alarm id value */
    if (rtc_alarm_id >= RTC_ALARM__MAX_NUM_OF_RTC_ALARMS)
        return FALSE;


    message_to_be_sent.event       = TASK_MSG_ID__REQUEST_RTC_ALARM_DELETE;
    message_to_be_sent.length_data = 1;
    message_to_be_sent.data[0]     = rtc_alarm_id;
    for (i = 1; i < RTC_ALARM__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(RtcAlarm_MessageQueueHandler_MessageQueueEvents, sizeof(RTC_ALARM__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : RtcAlarm_CalculateNearestRtcAlarm
 *
 * Description: Calculate the nearest RTC alarm set.
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void RtcAlarm_CalculateNearestRtcAlarm(void)
{
    CLOCK__TIME nearest_rtc_alarm_time;   /* nearest RTC alarm time */
    bool        nearest_rtc_alarm_found;

    s8          result_s8;
    u8          i;


    nearest_rtc_alarm_found = FALSE;

    /* search the nearest RTC alarm */
    for (i = 0; i < RTC_ALARM__MAX_NUM_OF_RTC_ALARMS; i++)
    {
        /* check i-th RTC alarm */
        if (RtcAlarm_RtcAlarms[i].status)
        {
            /* i-th RTC alarm enabled */

            if (!nearest_rtc_alarm_found)
            {
                nearest_rtc_alarm_time  = RtcAlarm_RtcAlarms[i].rtc_alarm_time;
                nearest_rtc_alarm_found = TRUE;
            }
            else
            {
                result_s8 = Clock_CompareRtcTimes(&RtcAlarm_RtcAlarms[i].rtc_alarm_time, &nearest_rtc_alarm_time);
                if (result_s8 == -1)
                {
                    /* i-th RTC alarm previous than current 'nearest_rtc_alarm_time' */

                    nearest_rtc_alarm_time = RtcAlarm_RtcAlarms[i].rtc_alarm_time;
                    nearest_rtc_alarm_found = TRUE;
                }
            }
        }
    }


    /* save the nearest RTC alarm */
    if (nearest_rtc_alarm_found)
    {
        /* at least one RTC alarm set */

        RtcAlarm_NearestRtcAlarm.status         = TRUE;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time = nearest_rtc_alarm_time;
    }
    else
    {
        /* no RTC alarm set */

        RtcAlarm_NearestRtcAlarm.status = FALSE;

        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.year     = 2000;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.month    = 1;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.day      = 1;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.hour     = 0;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.minute   = 0;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.second   = 0;
        RtcAlarm_NearestRtcAlarm.rtc_alarm_time.week_day = 6;  // Saturday
    }

    RtcAlarm_UpdateVarCrc();
}




/*=============================================================================
 * Function   : RtcAlarm_ReadRtcAlarmDescription
 *
 * Description: read a RTC alarm description
 * Input      : - rtc_alarm_id: RTC alarm id [0 - (RTC_ALARM__MAX_NUM_OF_RTC_ALARMS-1)]
 * Output     : - pointer to RTC alarm description
 *=============================================================================*/
ascii *RtcAlarm_ReadRtcAlarmDescription(u8 rtc_alarm_id)
{
    /* verify RTC alarm id value */
    if ((rtc_alarm_id >= RTC_ALARM__MAX_NUM_OF_RTC_ALARMS) && (rtc_alarm_id != RTC_ALARM__ALARM_NEAREST))
        return "???";


    switch (rtc_alarm_id)
    {
        /* nearest */
        case RTC_ALARM__ALARM_NEAREST:
            return ((ascii *)RTC_ALARM_DESCRIPTION__NEAREST            );

        /* internal chrono function */
        case RTC_ALARM__ALARM_F_CHRONO_INT:
            return ((ascii *)RTC_ALARM_DESCRIPTION__F_CHRONO_INT       );

        /* external chrono function */
        case RTC_ALARM__ALARM_F_CHRONO_EXT:
            return ((ascii *)RTC_ALARM_DESCRIPTION__F_CHRONO_EXT       );

        /* summer time change */
        case RTC_ALARM__ALARM_SUMMER_TIME:
            return ((ascii *)RTC_ALARM_DESCRIPTION__SUMMER_TIME        );

        /* clock autosychronize */
        case RTC_ALARM__ALARM_AUTOSYNCHRONIZE:
            return ((ascii *)RTC_ALARM_DESCRIPTION__AUTOSYNCHRONIZE    );

        /* log status duration */
        case RTC_ALARM__ALARM_LOG_STATUS_DURATION:
            return ((ascii *)RTC_ALARM_DESCRIPTION__LOG_STATUS_DURATION);

        /* log status period */
        case RTC_ALARM__ALARM_LOG_STATUS_PERIOD:
            return ((ascii *)RTC_ALARM_DESCRIPTION__LOG_STATUS_PERIOD  );

        /* input C1 band */
        case RTC_ALARM__ALARM_INPUT_C1_BAND:
            return ((ascii *)RTC_ALARM_DESCRIPTION__INPUT_C1_BAND      );

        /* input C2 band */
        case RTC_ALARM__ALARM_INPUT_C2_BAND:
            return ((ascii *)RTC_ALARM_DESCRIPTION__INPUT_C2_BAND      );

        /* default */
        default:
            return ((ascii *)RTC_ALARM_DESCRIPTION__OTHER              );
    }
}




/*===========================================================================
 * Function   : RtcAlarm_CalaAlarmSet
 *
 * Description: set a 'CALA' alarm in the module at a specified RTC time
 *              It uses AT+CALA command (AT+CALA="yy/MM/dd,hh:mm:ss")
 *              eg. "AT+CALA="11/09/01,12:30:00"
 *              The maximum number of 'CALA' alarms that can be set in the module is 16.
 * Input      : - ptr_cala_alarm_time: pointer to 'CALA' alarm time
 * Output     :   - FALSE: 'CALA' alarm not set
 *                - TRUE : 'CALA' alarm     set
 *===========================================================================*/
static bool RtcAlarm_CalaAlarmSet(CLOCK__TIME *ptr_cala_alarm_time)
{
    ql_rtc_time_t    s_alarm;
    ql_errcode_rtc_e res_rtc;


    /* build the AT+CALA command string */
    s_alarm.tm_year =  ptr_cala_alarm_time->year;
    s_alarm.tm_mon  =  ptr_cala_alarm_time->month;
    s_alarm.tm_mday =  ptr_cala_alarm_time->day;
    s_alarm.tm_wday = (ptr_cala_alarm_time->week_day == 7) ? 0 : ptr_cala_alarm_time->week_day;
    s_alarm.tm_hour =  ptr_cala_alarm_time->hour;
    s_alarm.tm_min  =  ptr_cala_alarm_time->minute;
    s_alarm.tm_sec  =  ptr_cala_alarm_time->second;


    /* send tha "AT+CALA" command for 'CALA' alarm (set date/time 'CALA' alarm) */
    res_rtc = ql_rtc_set_alarm(&s_alarm);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "ql_rtc_set_alarm ERROR: %d", res_rtc);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
  //else
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtc_set_alarm OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}


    /* wait for the AT answer */
    res_rtc = ql_rtc_enable_alarm(1);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "ql_rtc_enable_alarm ERROR: %d", res_rtc);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
  //else
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtc_enable_alarm OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}


    return TRUE;
}




/*===========================================================================
 * Function   : RtcAlarm_CalaAlarmDelete
 *
 * Description: delete a 'CALA' alarm in the module
 *              It uses AT+CALA command (AT+CALA="",<index>)
 *              eg. "AT+CALA="",2
 * Input      : -
 * Output     : -
 *===========================================================================*/
static bool RtcAlarm_CalaAlarmDelete(void)
{
    ql_errcode_rtc_e res_rtc;


    /* send the "AT+CALA" command for alarm (delete alarm) */
    res_rtc = ql_rtc_enable_alarm(0);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "ql_rtc_enable_alarm ERROR: %d", res_rtc);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
  //else
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtc_enable_alarm OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}


    return TRUE;
}




/*=============================================================================
 * Function   : RtcAlarm_TimerDelayJump_Start
 *
 * Description: start the "delay jump" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
static void RtcAlarm_TimerDelayJump_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "Start \"delay jump\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(RtcAlarm_TimerHandler_TimerDelayJump, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
#endif




/*=============================================================================
 * Function   : RtcAlarm_TimerDelayJump_Stop
 *
 * Description: stop the "delay jump" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
static void RtcAlarm_TimerDelayJump_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "Stop \"delay jump\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(RtcAlarm_TimerHandler_TimerDelayJump);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
#endif




/*===========================================================================
 * Function   : RtcAlarm_AdlCallback_Message_TaskMsg
 *
 * Description: - task RTC alarm message callback
 * Input      : - msg_identifier:
 *              - source        :
 *              - length        :
 *              - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void RtcAlarm_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    u8          rtc_alarm_id;
    CLOCK__TIME rtc_alarm_time;
    CLOCK__TIME cala_alarm_time;

    s8          result_s8;
    u8          i;

#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
    CLOCK__TIME time_offset;
    s32         offset;
    bool        result;
#endif


    /* debug */
    snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "CALLBACK     - MESSAGE     - TASK RTC ALARM - msg identifier: %ld, length: %ld", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the data length */
    switch (msg_identifier)
    {
        /* event - request RTC alarm set */
        case TASK_MSG_ID__REQUEST_RTC_ALARM_SET:
            if (length != 9)
                return;
            break;

        /* event - request RTC alarm delete */
        case TASK_MSG_ID__REQUEST_RTC_ALARM_DELETE:
            if (length != 1)
                return;
            break;

        /* event - 'CALA' alarm */
        case TASK_MSG_ID__CALA_ALARM:
            if (length != 8)
                return;
            break;

#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
        /* event - timeout delay jump */
        case TASK_MSG_ID__TIMEOUT_DELAY_JUMP:
            if (length != 0)
                return;
            break;
#endif

        /* unknown message identifier */
        default:
            return;
    }



    switch (msg_identifier)
    {
        /* event - request RTC alarm set */
        case TASK_MSG_ID__REQUEST_RTC_ALARM_SET:
            /* extract the parameters */
            rtc_alarm_id            =       *(ptr_data     );
            rtc_alarm_time.year     = ((u16)*(ptr_data +  1) << 8) |
                                      ((u16)*(ptr_data +  2)     );
            rtc_alarm_time.month    =       *(ptr_data +  3);
            rtc_alarm_time.day      =       *(ptr_data +  4);
            rtc_alarm_time.hour     =       *(ptr_data +  5);
            rtc_alarm_time.minute   =       *(ptr_data +  6);
            rtc_alarm_time.second   =       *(ptr_data +  7);
            rtc_alarm_time.week_day =       *(ptr_data +  8);

            snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "rtc_alarm_id: %d", rtc_alarm_id);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Clock_PrintTime2(DEBUG_TRACE_LEVEL_RTC_ALARM, 0, 0, &rtc_alarm_time);

            /* set the RTC alarm */
            RtcAlarm_RtcAlarms[rtc_alarm_id].status         = TRUE;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time = rtc_alarm_time;

            RtcAlarm_UpdateVarCrc();

            /* calculate the nearest RTC alarm set */
            RtcAlarm_CalculateNearestRtcAlarm();
            break;


        /* event - request RTC alarm delete */
        case TASK_MSG_ID__REQUEST_RTC_ALARM_DELETE:
            /* extract the parameters */
            rtc_alarm_id = *ptr_data;

            snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "rtc_alarm_id: %d", rtc_alarm_id);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* delete the RTC alarm */
            RtcAlarm_RtcAlarms[rtc_alarm_id].status = FALSE;

            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.year     = 2000;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.month    = 1;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.day      = 1;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.hour     = 0;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.minute   = 0;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.second   = 0;
            RtcAlarm_RtcAlarms[rtc_alarm_id].rtc_alarm_time.week_day = 6;  // Saturday

            RtcAlarm_UpdateVarCrc();

            /* calculate the nearest RTC alarm set (if at least one RTC alarm is set) */
            RtcAlarm_CalculateNearestRtcAlarm();
            break;


        /* event - 'CALA' alarm */
        case TASK_MSG_ID__CALA_ALARM:
            /* extract the parameters */
            cala_alarm_time.year     = ((u16)*(ptr_data     ) << 8) |
                                       ((u16)*(ptr_data +  1)     );
            cala_alarm_time.month    =       *(ptr_data +  2);
            cala_alarm_time.day      =       *(ptr_data +  3);
            cala_alarm_time.hour     =       *(ptr_data +  4);
            cala_alarm_time.minute   =       *(ptr_data +  5);
            cala_alarm_time.second   =       *(ptr_data +  6);
            cala_alarm_time.week_day =       *(ptr_data +  7);

            Clock_PrintTime2(DEBUG_TRACE_LEVEL_RTC_ALARM, 0, 0, &cala_alarm_time);

            /* delete all the enabled RTC alarms in the past or in the present */
            for (i = 0; i < RTC_ALARM__MAX_NUM_OF_RTC_ALARMS; i++)
            {
                if (RtcAlarm_RtcAlarms[i].status)
                {
                    /* i-th RTC alarm enabled */

                    result_s8 = Clock_CompareRtcTimes(&RtcAlarm_RtcAlarms[i].rtc_alarm_time, &cala_alarm_time);
                    if ((result_s8 == -1) || (result_s8 == 0))
                    {
                        /* i-th RTC alarm elapsed (in the past or in the present) */

                        snprintf(RtcAlarm_DebugString, sizeof(RtcAlarm_DebugString), "%d-th RTC alarm event", i + 1);
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, RtcAlarm_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        RtcAlarm_RtcAlarms[i].status = FALSE;

                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.year     = 2000;
                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.month    = 1;
                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.day      = 1;
                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.hour     = 0;
                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.minute   = 0;
                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.second   = 0;
                        RtcAlarm_RtcAlarms[i].rtc_alarm_time.week_day = 6;  // Saturday

                        RtcAlarm_UpdateVarCrc();

                        /* signal the i-th RTC alarm elapsed */
                        switch (i)
                        {
                            /* internal chrono function */
                            case RTC_ALARM__ALARM_F_CHRONO_INT:
                                FChrono_ChronoEventInt();
                                break;

                            /* external chrono function */
                            case RTC_ALARM__ALARM_F_CHRONO_EXT:
                                FChrono_ChronoEventExt();
                                break;

                            /* summer time change */
                            case RTC_ALARM__ALARM_SUMMER_TIME:
                                Calendar_Event_SummerTime();
                                break;

                            /* clock autosychronize */
                            case RTC_ALARM__ALARM_AUTOSYNCHRONIZE:
                                Calendar_Event_AutoSynchronize();
                                break;

                            /* log status duration */
                            case RTC_ALARM__ALARM_LOG_STATUS_DURATION:
                                LogStatus_Event_LogStatusDuration(&cala_alarm_time);
                                break;

                            /* log status period */
                            case RTC_ALARM__ALARM_LOG_STATUS_PERIOD:
                                LogStatus_Event_LogStatusPeriod  (&cala_alarm_time);
                                break;

                            /* input C1 band */
                            case RTC_ALARM__ALARM_INPUT_C1_BAND:
                                Counters_Event_InputC1Band();
                                break;

                            /* input C2 band */
                            case RTC_ALARM__ALARM_INPUT_C2_BAND:
                                Counters_Event_InputC2Band();
                                break;
                        }
                    }
                }
            }

            /* calculate the nearest RTC alarm set */
            RtcAlarm_CalculateNearestRtcAlarm();
            break;


#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
        /* event - timeout delay jump */
        case TASK_MSG_ID__TIMEOUT_DELAY_JUMP:
            if (RtcAlarm_NearestRtcAlarm.status)
            {
                /* modify the RTC time (for debugging) */
                offset = -TIME_SEC__OFFSET_FROM_NEAREST_RTC_ALARM;
                Clock_AddOffset(&RtcAlarm_NearestRtcAlarm.rtc_alarm_time, &time_offset, offset);
                result = Clock_SetTime(&time_offset);
            }
            break;
#endif


        /* unknown event */
        default:
            break;
    }



    switch (msg_identifier)
    {
        /* event - request RTC alarm set    */
        /* event - request RTC alarm delete */
        /* event - 'CALA' alarm             */
        case TASK_MSG_ID__REQUEST_RTC_ALARM_SET:
        case TASK_MSG_ID__REQUEST_RTC_ALARM_DELETE:
        case TASK_MSG_ID__CALA_ALARM:
            /* delete all 'CALA' alarms set in the module */
            RtcAlarm_CalaAlarmDelete();

            /* set the nearest 'CALA' alarm in the module (if at least one 'CALA' alarm is set) */
            if (RtcAlarm_NearestRtcAlarm.status)
            {
                RtcAlarm_CalaAlarmSet(&RtcAlarm_NearestRtcAlarm.rtc_alarm_time);

#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
                RtcAlarm_TimerDelayJump_Stop();
                RtcAlarm_TimerDelayJump_Start((TIME_SEC__DELAY_FROM_RTC_ALARM_SETTING * 1000L), FALSE);
#endif
            }
            break;


#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
        /* event - timeout delay jump */
        case TASK_MSG_ID__TIMEOUT_DELAY_JUMP:
            // NOTHING TO DO
            break;
#endif


        /* unknown event */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : RtcAlarm_AdlCallback_UnsolicitedResponse_CALA
 *                  "+CALA: "14/01/02,04:00:00",1"
 *
 * Description:
 * Input      : - ptr_params:
 * Output     : -
 *=============================================================================*/
static void RtcAlarm_AdlCallback_UnsolicitedResponse_CALA(void)
{
    RTC_ALARM__MAIL  message_to_be_sent;
    QlOSStatus       err;

    ql_rtc_time_t    cala_alarm_time;
    ql_errcode_rtc_e res_rtc;


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - UNSOLICITED RESPONSE - +CALA", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* extract 'CALA' alarm time */
    /* "+CALA: "14/01/02,04:00:00",1" */
    res_rtc = ql_rtc_get_alarm(&cala_alarm_time);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        cala_alarm_time.tm_year = 2000;
        cala_alarm_time.tm_mon  = 1;
        cala_alarm_time.tm_mday = 1;
        cala_alarm_time.tm_hour = 0;
        cala_alarm_time.tm_min  = 0;
        cala_alarm_time.tm_sec  = 0;
        cala_alarm_time.tm_wday = 6;  // Saturday
    }


    message_to_be_sent.event       = TASK_MSG_ID__CALA_ALARM;
    message_to_be_sent.length_data = 8;
    message_to_be_sent.data[0]     = (u8)(cala_alarm_time.tm_year >> 8);
    message_to_be_sent.data[1]     = (u8)(cala_alarm_time.tm_year     );
    message_to_be_sent.data[2]     =      cala_alarm_time.tm_mon;
    message_to_be_sent.data[3]     =      cala_alarm_time.tm_mday;
    message_to_be_sent.data[4]     =      cala_alarm_time.tm_hour;
    message_to_be_sent.data[5]     =      cala_alarm_time.tm_min;
    message_to_be_sent.data[6]     =      cala_alarm_time.tm_sec;
    message_to_be_sent.data[7]     =     (cala_alarm_time.tm_wday == 0) ? 7 : cala_alarm_time.tm_wday;
    message_to_be_sent.data[8]     = 0x00;

    err = ql_rtos_queue_release(RtcAlarm_MessageQueueHandler_MessageQueueEvents, sizeof(RTC_ALARM__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : RtcAlarm_AdlCallback_Timer_TimerDelayJump
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
#if (FW_CONFIG__TEST__MODE_2 == FW_CONFIG__TEST__MODE_2__JUMP)
static void RtcAlarm_AdlCallback_Timer_TimerDelayJump(void *context)
{
    RTC_ALARM__MAIL message_to_be_sent;
    QlOSStatus      err;

    u8              i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - DELAY JUMP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    message_to_be_sent.event       = TASK_MSG_ID__TIMEOUT_DELAY_JUMP;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < RTC_ALARM__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(RtcAlarm_MessageQueueHandler_MessageQueueEvents, sizeof(RTC_ALARM__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RTC_ALARM, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
#endif
