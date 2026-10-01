/*=============================================================================
 * File       :  CALENDAR.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - calendar
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "calendar.h"
#include "clock.h"
#include "debug_my.h"
#include "fw_config.h"
#include "program_flash.h"
#include "queue_my.h"
#include "rtc_alarm.h"
#include "startup.h"
#include "synchronize.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* task message IDs */
#define TASK_MSG_ID__RTC_ALARM_AUTOSYNCHRONIZE  (10500 | (QL_COMPONENT_APP_START << 16))     /* event - RTC alarm - autosynchronize */
#define TASK_MSG_ID__RTC_ALARM_SUMMER_TIME      (10501 | (QL_COMPONENT_APP_START << 16))     /* event - RTC alarm - summer time     */
#define TASK_MSG_ID__CLOCK_CHANGED              (10502 | (QL_COMPONENT_APP_START << 16))     /* event - clock changed               */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                 200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* summer time indication */
static bool  Calendar_SummerTime;   // = ...

/* CRC variables */
static u16   Calendar_VarCrc;       // = 0x0000;

/* debug string */
static ascii Calendar_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Calendar_TaskCalendar(void *argument);

/* status variables */
static void Calendar_InitVar        (void);
static u16  Calendar_CalculateVarCrc(void);
       void Calendar_UpdateVarCrc   (void);
       bool Calendar_VerifyVarCrc   (void);

/* events */
       void Calendar_Event_AutoSynchronize(void);
       void Calendar_Event_SummerTime     (void);
       void Calendar_Event_ClockChanged   (void);

/* utility */
static bool Calendar_SetNextEventAutoSynchronize(void);
static bool Calendar_SetNextEventSummerTime     (void);
static bool Calendar_NextEventAutoSynchronize   (CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_event_rtc_time);
static bool Calendar_NextEventSummerTime        (CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_event_rtc_time);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Calendar_AdlCallback_Message_TaskMsg(u32 msg_identifier);




/*=============================================================================
 * Function   : Calendar_TaskCalendar
 *
 * Description: calendar task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Calendar_TaskCalendar(void *argument)
{
    ql_event_t  event;
    QlOSStatus  err;

    bool        result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - CALENDAR - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* delay */
    ql_rtos_task_sleep_ms(500L);


    /* internal status init */
    Calendar_InitVar();


    /* set CALA alarm for next event for autosynchronize */
    /* set CALA alarm for next event for summer time     */
    result = Calendar_SetNextEventAutoSynchronize();
    result = Calendar_SetNextEventSummerTime();


    /* message subscribe */
    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Calendar_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : Calendar_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Calendar_InitVar(void)
{
    /* RTC actual time */
    CLOCK__TIME rtc_actual_time;

    bool        recover_status;

    bool        result;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        result = Clock_GetTime(&rtc_actual_time);
        if (result)
        {
            result = Clock_IsSummerTime(&rtc_actual_time);
            if (result)
                Calendar_SummerTime = TRUE;
            else
                Calendar_SummerTime = FALSE;
        }
        else
        {
            Calendar_SummerTime = FALSE;
        }


        /**** update the variable CRC ****/
        Calendar_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        // NOTHING TO DO
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : Calendar_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 Calendar_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&Calendar_SummerTime, sizeof(Calendar_SummerTime), crc);

    return crc;
}




/*=============================================================================
 * Function   : Calendar_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Calendar_UpdateVarCrc(void)
{
    Calendar_VarCrc = Calendar_CalculateVarCrc();
}




/*=============================================================================
 * Function   : Calendar_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool Calendar_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = Calendar_CalculateVarCrc();

    if (Calendar_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : Calendar_Event_AutoSynchronize
 *
 * Description: signal the event RTC alarm for autosynchronize
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Calendar_Event_AutoSynchronize(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__RTC_ALARM_AUTOSYNCHRONIZE;

    err = ql_rtos_event_send(Boot_TaskRef_Calendar, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Calendar_Event_SummerTime
 *
 * Description: signal the event RTC alarm for summer time
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Calendar_Event_SummerTime(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__RTC_ALARM_SUMMER_TIME;

    err = ql_rtos_event_send(Boot_TaskRef_Calendar, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Calendar_Event_ClockChanged
 *
 * Description: signal the event clock changed
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Calendar_Event_ClockChanged(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__CLOCK_CHANGED;

    err = ql_rtos_event_send(Boot_TaskRef_Calendar, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Calendar_SetNextEventAutoSynchronize
 *
 * Description: set CALA alarm for next event for autosynchronize
 * Input      : -
 * Output     : - FALSE: the nearest RTC time event not available
 *              - TRUE : the nearest RTC time event     available
 *===========================================================================*/
static bool Calendar_SetNextEventAutoSynchronize(void)
{
    /* RTC actual time */
    CLOCK__TIME rtc_actual_time;

    /* next event */
    CLOCK__TIME rtc_next_event_time;

    bool        result;


    /* get the actual time */
    result = Clock_GetTime(&rtc_actual_time);
    if (!result)
        return FALSE;

    /* set/delete the 'CALA" alarm */
    result = Calendar_NextEventAutoSynchronize(&rtc_actual_time, &rtc_next_event_time);
    if (result)
    {
        /* set RTC alarm to "rtc_next_event_time" */
        result = RtcAlarm_RtcAlarmSet   (RTC_ALARM__ALARM_AUTOSYNCHRONIZE, &rtc_next_event_time);
    }
    else
    {
        /* delete RTC alarm  */
        result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_AUTOSYNCHRONIZE);
    }

    return result;
}




/*===========================================================================
 * Function   : Calendar_SetNextEventSummerTime
 *
 * Description: set CALA alarm for next event for summer time
 * Input      : -
 * Output     : - FALSE: the nearest RTC time event not available
 *              - TRUE : the nearest RTC time event     available
 *===========================================================================*/
static bool Calendar_SetNextEventSummerTime(void)
{
    /* RTC actual time */
    CLOCK__TIME rtc_actual_time;

    /* next event */
    CLOCK__TIME rtc_next_event_time;

    bool        result;


    /* get the actual time */
    result = Clock_GetTime(&rtc_actual_time);
    if (!result)
        return FALSE;

    /* set/delete the 'CALA" alarm */
    result = Calendar_NextEventSummerTime(&rtc_actual_time, &rtc_next_event_time);
    if (result)
    {
        /* set RTC alarm to "rtc_next_event_time" */
        result = RtcAlarm_RtcAlarmSet   (RTC_ALARM__ALARM_SUMMER_TIME, &rtc_next_event_time);
    }
    else
    {
        /* delete RTC alarm  */
        result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_SUMMER_TIME);
    }

    return result;
}




/*===========================================================================
 * Function   : Calendar_NextEventAutoSynchronize
 *
 * Description: determine the nearest RTC time event for auto-synchronize
 *                 - 03:31:30 of the 2st day of a month
 * Input      : - ptr_actual_rtc_time    : pointer to      actual      RTC time
 *              - ptr_next_event_rtc_time: pointer to save the nearest RTC time event
 * Output     : - FALSE: the nearest RTC time event not available
 *              - TRUE : the nearest RTC time event     available
 *===========================================================================*/
static bool Calendar_NextEventAutoSynchronize(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_event_rtc_time)
{
    CLOCK__TIME time;

    u8          second;
    u8          minute;
    u8          hour;
    u8          day;
    u8          month;
    u16         year;

    u8          day_sync;


    /*-------------------------------------------------------------------
     * 03:31:30 of the 2st day of the actual or next month
     *-------------------------------------------------------------------*/
    day_sync = 2;

    year   = ptr_actual_rtc_time->year;
    month  = ptr_actual_rtc_time->month;
    day    = ptr_actual_rtc_time->day;
    hour   = ptr_actual_rtc_time->hour;
    minute = ptr_actual_rtc_time->minute;
    second = ptr_actual_rtc_time->second;

    if (
         ((day == 0       )                                                   ) ||  // day = 0  --->  it is not possible
         ((day >  day_sync)                                                   ) ||
         ((day == day_sync) && (hour >  3)                                    ) ||
         ((day == day_sync) && (hour == 3) && (minute >  31)                  ) ||
         ((day == day_sync) && (hour == 3) && (minute == 31) && (second >= 30))
       )
    {
        month++;
        if (month > 12)
        {
            month = 1;
            year++;
        }
    }

    time.second   = 30;
    time.minute   = 31;
    time.hour     = 3;

    time.day      = day_sync;
    time.month    = month;
    time.year     = year;

    time.week_day = Clock_WeekDayOfADay(1, month, year);


    /* save the nearest event RTC time */
    *ptr_next_event_rtc_time = time;


    return TRUE;
}




/*===========================================================================
 * Function   : Calendar_NextEventSummerTime
 *
 * Description: determine the nearest RTC time event for "summer time" change time
 *              'Summer time' is active between:
 *                  - 02:00:00 of last Sunday of March
 *                  - 03:00:00 of last Sunday of October
 * Input      : - ptr_actual_rtc_time    : pointer to      actual      RTC time
 *              - ptr_next_event_rtc_time: pointer to save the nearest RTC time event
 * Output     : - FALSE: the nearest RTC time event not available
 *              - TRUE : the nearest RTC time event     available
 *===========================================================================*/
static bool Calendar_NextEventSummerTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_event_rtc_time)
{
    CLOCK__TIME time;


    /* nearest change of summer time */
    Clock_NextSummerTimeChange(ptr_actual_rtc_time, &time);

    /* save the nearest event RTC time */
    *ptr_next_event_rtc_time = time;

    return TRUE;
}




/*===========================================================================
 * Function   : Calendar_AdlCallback_Message_TaskMsg
 *
 * Description: - task calendar message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void Calendar_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    CLOCK__TIME              rtc_actual_time;
    CLOCK__TIME              rtc_actual_time_new;

    /* synchronize SMS counter */
    SYNCHRONIZE__SMS_COUNTER synchronize_sms_counter;

    bool                     clock_changed;

    bool                     result;


    snprintf(Calendar_DebugString, sizeof(Calendar_DebugString), "CALLBACK     - MESSAGE     - TASK CALENDAR - msg_identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CALENDAR, DEBUG_TRACE_TYPE_LOW, Calendar_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier)
    {
        /* event - RTC alarm - autosynchronize */
        case TASK_MSG_ID__RTC_ALARM_AUTOSYNCHRONIZE:
            /*----------------------------------------------------------
             * Set CALA alarm
             *----------------------------------------------------------*/
            /* set CALA alarm for next event for autosynchronize */
            result = Calendar_SetNextEventAutoSynchronize();


            /*----------------------------------------------------------
             * Generate a autosynchronize record
             *----------------------------------------------------------*/
            /* get    the autosynchronize SMS counter */
            Synchronize_SmsCounter_Get(&synchronize_sms_counter);

            /* update the autosynchronize SMS counter */
            synchronize_sms_counter.sms_counter++;
            Synchronize_SmsCounter_Set(&synchronize_sms_counter);

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE, PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER);

            /* put a record in the autosynchronize transmission queue */
            result = Queue_Autosynchronize_PutRecord((QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *)&synchronize_sms_counter);
            break;



        /* event - RTC alarm - summer time */
        case TASK_MSG_ID__RTC_ALARM_SUMMER_TIME:
            /*----------------------------------------------------------
             * Change clock
             *----------------------------------------------------------*/
            clock_changed = FALSE;

            /* get the RTC time */
            result = Clock_GetTime(&rtc_actual_time);
            if (!result)
                break;

            if (rtc_actual_time.month == 3)
            {
                if (!Calendar_SummerTime)
                {
                    Calendar_SummerTime = TRUE;
                    Calendar_UpdateVarCrc();

                    Clock_AddOffset(&rtc_actual_time, &rtc_actual_time_new, +3600);  // +1 hour (on March  : from winter time to summer time)

                    clock_changed= TRUE;
                }
            }
            else
            {
                if ( Calendar_SummerTime)
                {
                    Calendar_SummerTime = FALSE;
                    Calendar_UpdateVarCrc();

                    Clock_AddOffset(&rtc_actual_time, &rtc_actual_time_new, -3600);  // -1 hour (on October: from summer time to winter time)

                    clock_changed = TRUE;
                }
            }

            /* set the RTC time */
            if (clock_changed)
            {
                result = Clock_SetTime(&rtc_actual_time_new);
                if (!result)
                    break;
            }


            /*----------------------------------------------------------
             * Set CALA alarm
             *----------------------------------------------------------*/
            /* set CALA alarm for next event for summer time */
            result = Calendar_SetNextEventSummerTime();
            if (!result)
                break;


            /*----------------------------------------------------------
             * Generate a autosynchronize record
             *----------------------------------------------------------*/
            /* get    the autosynchronize SMS counter */
            Synchronize_SmsCounter_Get(&synchronize_sms_counter);

            /* update the autosynchronize SMS counter */
            synchronize_sms_counter.sms_counter++;
            Synchronize_SmsCounter_Set(&synchronize_sms_counter);

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE, PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER);

            /* put a record in the autosynchronize transmission queue */
          //result = Queue_Autosynchronize_PutRecord((QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *)&synchronize_sms_counter);
            break;



        /* event - clock changed */
        case TASK_MSG_ID__CLOCK_CHANGED:
            /*----------------------------------------------------------
             * Summer time
             *----------------------------------------------------------*/
            result = Clock_GetTime(&rtc_actual_time);
            if (result)
            {
                result = Clock_IsSummerTime(&rtc_actual_time);
                if (result)
                    Calendar_SummerTime = TRUE;
                else
                    Calendar_SummerTime = FALSE;
            }
            else
            {
                Calendar_SummerTime = FALSE;
            }
            Calendar_UpdateVarCrc();


            /*----------------------------------------------------------
             * Set CALA alarm
             *----------------------------------------------------------*/
            /* set CALA alarm for next event for autosynchronize */
            /* set CALA alarm for next event for summer time     */
            result = Calendar_SetNextEventAutoSynchronize();
            result = Calendar_SetNextEventSummerTime();
            break;



        /* unknown event */
        default:
            break;
    }
}
