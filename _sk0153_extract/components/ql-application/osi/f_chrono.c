/*=============================================================================
 * File       :  F_CHRONO.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "chrono-thermostat" function
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
#include "clock.h"
#include "debug_my.h"
#include "f_chrono.h"
#include "outputs.h"
#include "rtc_alarm.h"
#include "startup.h"
#include "temperature.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* chrono temperature (/10 �C) */
#define CHRONO_TEMPERATURE_MIN             0     /* minimum chrono temperature (/10 �C) */
#define CHRONO_TEMPERATURE_MAX             320   /* maximum chrono temperature (/10 �C) */

/* task message IDs */
#define TASK_MSG_ID__CHRONO_EVENT_INT      (11500 | (QL_COMPONENT_APP_START << 16))     // event - internal chrono event
#define TASK_MSG_ID__CHRONO_EVENT_EXT      (11501 | (QL_COMPONENT_APP_START << 16))     // event - external chrono event

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING            200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - internal chrono function */
static       F_CHRONO__CONFIG_F_CHRONO_INT FChrono_Config_FChronoInt;
static const F_CHRONO__CONFIG_F_CHRONO_INT FChrono_Config_FChronoIntDefault =
{
    /* chrono function status */
    FALSE,

    /* chrono windows */
    {
        //    window status
        //    |       temperature to be regulated
        //    |       |     window start hour
        //    |       |     |   window start minute
        //    |       |     |   |   window stop hour
        //    |       |     |   |   |   window stop minute
        //    |       |     |   |   |   |

        /* Monday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Tuesday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Wednesday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Thursday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Friday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Saturday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Sunday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        }
    }
};


/* configuration - external chrono function */
static       F_CHRONO__CONFIG_F_CHRONO_EXT FChrono_Config_FChronoExt;
static const F_CHRONO__CONFIG_F_CHRONO_EXT FChrono_Config_FChronoExtDefault =
{
    /* chrono function status */
    FALSE,

    /* chrono windows */
    {
        //    window status
        //    |       temperature to be regulated
        //    |       |     window start hour
        //    |       |     |   window start minute
        //    |       |     |   |   window stop hour
        //    |       |     |   |   |   window stop minute
        //    |       |     |   |   |   |

        /* Monday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Tuesday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Wednesday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Thursday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Friday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Saturday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        },

        /* Sunday */
        {
            { FALSE,  200,   6, 0,   7, 0 },
            { FALSE,  200,  12, 0,  13, 0 },
            { FALSE,  200,  18, 0,  19, 0 }
        }
    }
};


/* debug string */
static       ascii                         FChrono_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void FChrono_TaskFChrono(void *argument);

/* status variables */
static void FChrono_InitVar(void);

/* task event */
       void FChrono_ChronoEventInt(void);
       void FChrono_ChronoEventExt(void);

/* utility */
static bool FChrono_TimeStaysInChronoWindowInt(CLOCK__TIME *ptr_time, u8 *ptr_week_day, u8 *ptr_chrono_window_index, s16 *ptr_temperature);
static bool FChrono_TimeStaysInChronoWindowExt(CLOCK__TIME *ptr_time, u8 *ptr_week_day, u8 *ptr_chrono_window_index, s16 *ptr_temperature);

/* utility */
static bool FChrono_NextChronoEventInt        (CLOCK__TIME *ptr_time, CLOCK__TIME *ptr_next_chrono_event);
static bool FChrono_NextChronoEventExt        (CLOCK__TIME *ptr_time, CLOCK__TIME *ptr_next_chrono_event);


/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set "internal chrono function" configuration */
       void FChrono__Config_FChronoInt_GetDefault(F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);
       void FChrono__Config_FChronoInt_Get       (F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);
       void FChrono__Config_FChronoInt_Set       (F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);
       bool FChrono__Config_FChronoInt_IsValid   (F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);

/* get/set "external chrono function" configuration */
       void FChrono__Config_FChronoExt_GetDefault(F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);
       void FChrono__Config_FChronoExt_Get       (F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);
       void FChrono__Config_FChronoExt_Set       (F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);
       bool FChrono__Config_FChronoExt_IsValid   (F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void FChrono_AdlCallback_Message_TaskMsg(u32 msg_identifier);




/*=============================================================================
 * Function   : FChrono_TaskFChrono
 *
 * Description:
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FChrono_TaskFChrono(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - F_CHRONO - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* delay */
    ql_rtos_task_sleep_ms(5000L);


    /* internal status init */
    FChrono_InitVar();


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            FChrono_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : FChrono_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FChrono_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        /* signal the event to internal chrono function */
        /* signal the event to external chrono function */
        FChrono_ChronoEventInt();
        FChrono_ChronoEventExt();
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


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
 * Function   : FChrono_ChronoEventInt
 *
 * Description: signal a internal chrono event
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FChrono_ChronoEventInt(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__CHRONO_EVENT_INT;

    err = ql_rtos_event_send(Boot_TaskRef_FChrono, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FChrono_ChronoEventExt
 *
 * Description: signal a external chrono event
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FChrono_ChronoEventExt(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__CHRONO_EVENT_EXT;

    err = ql_rtos_event_send(Boot_TaskRef_FChrono, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : FChrono_TimeStaysInChronoWindowInt
 *
 * Description: verify if a RTC time stay in a configured enabled internal chrono window
 *                 - chrono window start time          stays in the chrono window
 *                 - chrono window stop  time does not stays in the chrono window
 * Input      : - ptr_time               : pointer to RTC time
 *              - ptr_week_day           : pointer to save week day                of the enabled external chrono window
 *              - ptr_chrono_window_index: pointer to save the chrono window index of the enabled internal chrono window [0-(CHRONO__NUM_OF_WINDOWS -1)]
 *              - ptr_temperature        : pointer to save regulation temperature  of the enabled internal chrono window
 * Output     : - FALSE: the RTC time          stays in a configured enabled internal chrono window
 *              - TRUE : the RTC time does not stay  in a configured enabled internal chrono window
 *===========================================================================*/
static bool FChrono_TimeStaysInChronoWindowInt(CLOCK__TIME *ptr_time, u8 *ptr_week_day, u8 *ptr_chrono_window_index, s16 *ptr_temperature)
{
    u8 week_day;
    u8 hour;
    u8 minute;

    s8 result_1;
    s8 result_2;
    u8 i;


    week_day = ptr_time->week_day;
    hour     = ptr_time->hour;
    minute   = ptr_time->minute;

    if ((week_day == 0) || (week_day > 7))
        return FALSE;


    if (FChrono_Config_FChronoInt.status)
    {
        /* internal chrono function enabled */

        for (i = 0; i < CHRONO__NUM_OF_WINDOWS; i++)
        {
            if (FChrono_Config_FChronoInt.chrono_windows[week_day - 1][i].status)
            {
                /* i-th window of weekday enabled */

                result_1 = Clock_CompareTimes(hour, minute, 0, FChrono_Config_FChronoInt.chrono_windows[week_day - 1][i].start_hour, FChrono_Config_FChronoInt.chrono_windows[week_day - 1][i].start_minute, 0);
                result_2 = Clock_CompareTimes(hour, minute, 0, FChrono_Config_FChronoInt.chrono_windows[week_day - 1][i].stop_hour , FChrono_Config_FChronoInt.chrono_windows[week_day - 1][i].stop_minute , 0);

                if (
                     ((result_1 == 0) || (result_1 == +1))  &&    // start_time <= RTC time
                     (                   (result_2 == -1))        //               RTC time < stop_time
                   )
                {
                    /* RTC time stays in the i-th window of weekday */

                    *ptr_week_day            = week_day;
                    *ptr_chrono_window_index = i;
                    *ptr_temperature         = FChrono_Config_FChronoInt.chrono_windows[week_day - 1][i].temperature;

                    return TRUE;
                }
            }
        }
    }


    *ptr_week_day            = 1;
    *ptr_chrono_window_index = 0;
    *ptr_temperature         = 0;

    return FALSE;
}




/*===========================================================================
 * Function   : FChrono_TimeStaysInChronoWindowExt
 *
 * Description: verify if a RTC time stay in a configured enabled external chrono window
 *                 - chrono window start time          stays in the chrono window
 *                 - chrono window stop  time does not stays in the chrono window
 * Input      : - ptr_time               : pointer to RTC time
 *              - ptr_week_day           : pointer to save week day                of the enabled external chrono window
 *              - ptr_chrono_window_index: pointer to save the chrono window index of the enabled external chrono window [0-(CHRONO__NUM_OF_WINDOWS -1)]
 *              - ptr_temperature        : pointer to save regulation temperature  of the enabled external chrono window
 * Output     : - FALSE: the RTC time          stays in a configured enabled external chrono window
 *              - TRUE : the RTC time does not stay  in a configured enabled external chrono window
 *===========================================================================*/
static bool FChrono_TimeStaysInChronoWindowExt(CLOCK__TIME *ptr_time, u8 *ptr_week_day, u8 *ptr_chrono_window_index, s16 *ptr_temperature)
{
    u8 week_day;
    u8 hour;
    u8 minute;

    s8 result_1;
    s8 result_2;
    u8 i;


    week_day = ptr_time->week_day;
    hour     = ptr_time->hour;
    minute   = ptr_time->minute;

    if ((week_day == 0) || (week_day > 7))
        return FALSE;


    if (FChrono_Config_FChronoExt.status)
    {
        /* external chrono function enabled */

        for (i = 0; i < CHRONO__NUM_OF_WINDOWS; i++)
        {
            if (FChrono_Config_FChronoExt.chrono_windows[week_day - 1][i].status)
            {
                /* i-th window of weekday enabled */

                result_1 = Clock_CompareTimes(hour, minute, 0, FChrono_Config_FChronoExt.chrono_windows[week_day - 1][i].start_hour, FChrono_Config_FChronoExt.chrono_windows[week_day - 1][i].start_minute, 0);
                result_2 = Clock_CompareTimes(hour, minute, 0, FChrono_Config_FChronoExt.chrono_windows[week_day - 1][i].stop_hour , FChrono_Config_FChronoExt.chrono_windows[week_day - 1][i].stop_minute , 0);

                if (
                     ((result_1 == 0) || (result_1 == +1))  &&    // start_time <= RTC time
                     (                   (result_2 == -1))        //               RTC time < stop_time
                   )
                {
                    /* RTC time stays in the i-th window of weekday */

                    *ptr_week_day            = week_day;
                    *ptr_chrono_window_index = i;
                    *ptr_temperature         = FChrono_Config_FChronoExt.chrono_windows[week_day - 1][i].temperature;

                    return TRUE;
                }
            }
        }
    }


    *ptr_week_day            = 1;
    *ptr_chrono_window_index = 0;
    *ptr_temperature         = 0;

    return FALSE;
}




/*===========================================================================
 * Function   : FChrono_NextChronoEventInt
 *
 * Description: calculate next internal chrono event
 * Input      : - ptr_time             : pointer to RTC time
 *              - ptr_next_chrono_event: pointer to save next internal chrono event
 * Output     : - FALSE: next internal chrono event not available (internal chrono function disabled)
 *              - TRUE : next internal chrono event     available (internal chrono function enabled )
 *===========================================================================*/
static bool FChrono_NextChronoEventInt(CLOCK__TIME *ptr_time, CLOCK__TIME *ptr_next_chrono_event)
{
    /* next chrono event */
    CLOCK__TIME next_chrono_event;

    u8          week_day;
    u8          chrono_window_index;
    s16         temperature;

    u16         next_year;
    u8          next_month;
    u8          next_day;

    u8          days_offset;

    bool        result;
    s8          result_s8;
    u8          i;
    u8          j;


    /* verify if internal chrono function is enabled */
    if (!FChrono_Config_FChronoInt.status)
    {
        next_chrono_event.year     = 2000;
        next_chrono_event.month    = 1;
        next_chrono_event.day      = 1;
        next_chrono_event.hour     = 0;
        next_chrono_event.minute   = 0;
        next_chrono_event.second   = 0;
        next_chrono_event.week_day = 6;

        *ptr_next_chrono_event = next_chrono_event;

        return FALSE;
    }


    /* verify if there are internal chrono windows enabled */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            if (FChrono_Config_FChronoInt.chrono_windows[i][j].status)
                break;
        }
        if (j < CHRONO__NUM_OF_WINDOWS)
            break;
    }
    if ((i >= 7) && (j >= CHRONO__NUM_OF_WINDOWS))
    {
        next_chrono_event.year     = 2000;
        next_chrono_event.month    = 1;
        next_chrono_event.day      = 1;
        next_chrono_event.hour     = 0;
        next_chrono_event.minute   = 0;
        next_chrono_event.second   = 0;
        next_chrono_event.week_day = 6;

        *ptr_next_chrono_event = next_chrono_event;

        return FALSE;
    }


    /* verify if RTC time stays in a configured enabled internal chrono window */
    result = FChrono_TimeStaysInChronoWindowInt(ptr_time, &week_day, &chrono_window_index, &temperature);
    if (result)
    {
        /* RTC time          stays in a configured enabled internal chrono window */

        next_chrono_event.year     = ptr_time->year;
        next_chrono_event.month    = ptr_time->month;
        next_chrono_event.day      = ptr_time->day;
        next_chrono_event.hour     = FChrono_Config_FChronoInt.chrono_windows[week_day - 1][chrono_window_index].stop_hour;    // stop time
        next_chrono_event.minute   = FChrono_Config_FChronoInt.chrono_windows[week_day - 1][chrono_window_index].stop_minute;  // stop time
        next_chrono_event.second   = 0;
        next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

        *ptr_next_chrono_event = next_chrono_event;

        return TRUE;
    }
    else
    {
        /* RTC time does not stay  in a configured enabled internal chrono window */

        /* "actual week_day" - after actual RTC time */
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            if (FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].status)
            {
                /* i-th window of weekday enabled */

                result_s8 = Clock_CompareTimes(ptr_time->hour, ptr_time->minute, 0, FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_hour, FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_minute, 0);
                if (result_s8 == -1)    // RTC time < start_time
                {
                    /* RTC time is before than start time of the i-th window of weekday */

                    next_year  = ptr_time->year;
                    next_month = ptr_time->month;
                    next_day   = ptr_time->day;

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }

        /* since ("actual week day" + 1) until Sunday */
        for (i = ptr_time->week_day; i < 7; i++)
        {
            for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
            {
                if (FChrono_Config_FChronoInt.chrono_windows[i][j].status)
                {
                    /* i-th window of weekday enabled */

                    days_offset = i - ptr_time->week_day + 1;
                    Clock_NextDay(ptr_time->year, ptr_time->month, ptr_time->day, days_offset, &next_year, &next_month, &next_day);

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoInt.chrono_windows[i][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoInt.chrono_windows[i][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }

        /* since Monday until ("actual week_day" - 1) */
        for (i = 0; i < (ptr_time->week_day - 1); i++)
        {
            for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
            {
                if (FChrono_Config_FChronoInt.chrono_windows[i][j].status)
                {
                    /* i-th window of weekday enabled */

                    days_offset = (6 - ptr_time->week_day + 1) + (i + 1);
                    Clock_NextDay(ptr_time->year, ptr_time->month, ptr_time->day, days_offset, &next_year, &next_month, &next_day);

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoInt.chrono_windows[i][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoInt.chrono_windows[i][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }

        /* "actual week_day" - before actual RTC time or exactly at actual RTC time */
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            if (FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].status)
            {
                /* i-th window of weekday enabled */

                result_s8 = Clock_CompareTimes(ptr_time->hour, ptr_time->minute, 0, FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_hour, FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_minute, 0);
                if ((result_s8 == 0) || (result_s8 == +1))    // RTC time >= start_time
                {
                    /* RTC time is after than or exactly at start time of the i-th window of weekday */

                    days_offset = 7;
                    Clock_NextDay(ptr_time->year, ptr_time->month, ptr_time->day, days_offset, &next_year, &next_month, &next_day);

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoInt.chrono_windows[ptr_time->week_day - 1][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }
    }


    next_chrono_event.year     = 2000;
    next_chrono_event.month    = 1;
    next_chrono_event.day      = 1;
    next_chrono_event.hour     = 0;
    next_chrono_event.minute   = 0;
    next_chrono_event.second   = 0;
    next_chrono_event.week_day = 6;

    *ptr_next_chrono_event = next_chrono_event;

    return FALSE;
}




/*===========================================================================
 * Function   : FChrono_NextChronoEventExt
 *
 * Description: calculate next external chrono event
 * Input      : - ptr_time             : pointer to RTC time
 *              - ptr_next_chrono_event: pointer to save next external chrono event
 * Output     : - FALSE: next external chrono event not available (external chrono function disabled)
 *              - TRUE : next external chrono event     available (external chrono function enabled )
 *===========================================================================*/
static bool FChrono_NextChronoEventExt(CLOCK__TIME *ptr_time, CLOCK__TIME *ptr_next_chrono_event)
{
    /* next chrono event */
    CLOCK__TIME next_chrono_event;

    u8          week_day;
    u8          chrono_window_index;
    s16         temperature;

    u16         next_year;
    u8          next_month;
    u8          next_day;

    u8          days_offset;

    bool        result;
    s8          result_s8;
    u8          i;
    u8          j;


    /* verify if external chrono function is enabled */
    if (!FChrono_Config_FChronoExt.status)
    {
        next_chrono_event.year     = 2000;
        next_chrono_event.month    = 1;
        next_chrono_event.day      = 1;
        next_chrono_event.hour     = 0;
        next_chrono_event.minute   = 0;
        next_chrono_event.second   = 0;
        next_chrono_event.week_day = 6;

        *ptr_next_chrono_event = next_chrono_event;

        return FALSE;
    }


    /* verify if there are external chrono windows enabled */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            if (FChrono_Config_FChronoExt.chrono_windows[i][j].status)
                break;
        }
        if (j < CHRONO__NUM_OF_WINDOWS)
            break;
    }
    if ((i >= 7) && (j >= CHRONO__NUM_OF_WINDOWS))
    {
        next_chrono_event.year     = 2000;
        next_chrono_event.month    = 1;
        next_chrono_event.day      = 1;
        next_chrono_event.hour     = 0;
        next_chrono_event.minute   = 0;
        next_chrono_event.second   = 0;
        next_chrono_event.week_day = 6;

        *ptr_next_chrono_event = next_chrono_event;

        return FALSE;
    }


    /* verify if RTC time stays in a configured enabled external chrono window */
    result = FChrono_TimeStaysInChronoWindowExt(ptr_time, &week_day, &chrono_window_index, &temperature);
    if (result)
    {
        /* RTC time          stays in a configured enabled external chrono window */

        next_chrono_event.year     = ptr_time->year;
        next_chrono_event.month    = ptr_time->month;
        next_chrono_event.day      = ptr_time->day;
        next_chrono_event.hour     = FChrono_Config_FChronoExt.chrono_windows[week_day - 1][chrono_window_index].stop_hour;    // stop time
        next_chrono_event.minute   = FChrono_Config_FChronoExt.chrono_windows[week_day - 1][chrono_window_index].stop_minute;  // stop time
        next_chrono_event.second   = 0;
        next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

        *ptr_next_chrono_event = next_chrono_event;

        return TRUE;
    }
    else
    {
        /* RTC time does not stay  in a configured enabled external chrono window */

        /* "actual week_day" - after actual RTC time */
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            if (FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].status)
            {
                /* i-th window of weekday enabled */

                result_s8 = Clock_CompareTimes(ptr_time->hour, ptr_time->minute, 0, FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_hour, FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_minute, 0);
                if (result_s8 == -1)    // RTC time < start_time
                {
                    /* RTC time is before than start time of the i-th window of weekday */

                    next_year  = ptr_time->year;
                    next_month = ptr_time->month;
                    next_day   = ptr_time->day;

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }

        /* since ("actual week day" + 1) until Sunday */
        for (i = ptr_time->week_day; i < 7; i++)
        {
            for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
            {
                if (FChrono_Config_FChronoExt.chrono_windows[i][j].status)
                {
                    /* i-th window of weekday enabled */

                    days_offset = i - ptr_time->week_day + 1;
                    Clock_NextDay(ptr_time->year, ptr_time->month, ptr_time->day, days_offset, &next_year, &next_month, &next_day);

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoExt.chrono_windows[i][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoExt.chrono_windows[i][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }

        /* since Monday until ("actual week_day" - 1) */
        for (i = 0; i < (ptr_time->week_day - 1); i++)
        {
            for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
            {
                if (FChrono_Config_FChronoExt.chrono_windows[i][j].status)
                {
                    /* i-th window of weekday enabled */

                    days_offset = (6 - ptr_time->week_day + 1) + (i + 1);
                    Clock_NextDay(ptr_time->year, ptr_time->month, ptr_time->day, days_offset, &next_year, &next_month, &next_day);

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoExt.chrono_windows[i][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoExt.chrono_windows[i][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }

        /* "actual week_day" - before actual RTC time or exactly at actual RTC time */
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            if (FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].status)
            {
                /* i-th window of weekday enabled */

                result_s8 = Clock_CompareTimes(ptr_time->hour, ptr_time->minute, 0, FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_hour, FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_minute, 0);
                if ((result_s8 == 0) || (result_s8 == +1))    // RTC time >= start_time
                {
                    /* RTC time is after than or exactly at start time of the i-th window of weekday */

                    days_offset = 7;
                    Clock_NextDay(ptr_time->year, ptr_time->month, ptr_time->day, days_offset, &next_year, &next_month, &next_day);

                    next_chrono_event.year     = next_year;
                    next_chrono_event.month    = next_month;
                    next_chrono_event.day      = next_day;
                    next_chrono_event.hour     = FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_hour;    // start time
                    next_chrono_event.minute   = FChrono_Config_FChronoExt.chrono_windows[ptr_time->week_day - 1][j].start_minute;  // start time
                    next_chrono_event.second   = 0;
                    next_chrono_event.week_day = Clock_WeekDayOfADay(next_chrono_event.day, next_chrono_event.month, next_chrono_event.year);

                    *ptr_next_chrono_event = next_chrono_event;

                    return TRUE;
                }
            }
        }
    }


    next_chrono_event.year     = 2000;
    next_chrono_event.month    = 1;
    next_chrono_event.day      = 1;
    next_chrono_event.hour     = 0;
    next_chrono_event.minute   = 0;
    next_chrono_event.second   = 0;
    next_chrono_event.week_day = 6;

    *ptr_next_chrono_event = next_chrono_event;

    return FALSE;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoInt_GetDefault
 *
 * Description: get the default "internal chrono function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FChrono__Config_FChronoInt_GetDefault(F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data)
{
    *ptr_data = FChrono_Config_FChronoIntDefault;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoInt_Get
 *
 * Description: get the "internal chrono function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FChrono__Config_FChronoInt_Get(F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data)
{
    *ptr_data = FChrono_Config_FChronoInt;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoInt_Set
 *
 * Description: set the "internal chrono function" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FChrono__Config_FChronoInt_Set(F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data)
{
    FChrono_Config_FChronoInt = *ptr_data;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoInt_IsValid
 *
 * Description: check if the "internal chrono function" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FChrono__Config_FChronoInt_IsValid(F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data)
{
    s8 result;
    u8 i;
    u8 j;


    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            /* temperature to be regulated */
            if (
                 (ptr_data->chrono_windows[i][j].temperature < CHRONO_TEMPERATURE_MIN) ||
                 (ptr_data->chrono_windows[i][j].temperature > CHRONO_TEMPERATURE_MAX)
               )
            {
                return FALSE;
            }

            /* window start time and window stop time */
            if (
                 (ptr_data->chrono_windows[i][j].start_hour   > 23) ||
                 (ptr_data->chrono_windows[i][j].start_minute > 59) ||
                 (ptr_data->chrono_windows[i][j].stop_hour    > 23) ||
                 (ptr_data->chrono_windows[i][j].stop_minute  > 59)
               )
            {
                return FALSE;
            }

            /* window start time < window stop time (only for chrono windows enabled) */
            if (ptr_data->chrono_windows[i][j].status)
            {
                result = Clock_CompareTimes(ptr_data->chrono_windows[i][j].start_hour,
                                            ptr_data->chrono_windows[i][j].start_minute,
                                            0,
                                            ptr_data->chrono_windows[i][j].stop_hour,
                                            ptr_data->chrono_windows[i][j].stop_minute,
                                            0);
                if ((result == 0) || (result == +1))
                    return FALSE;   // start_time[i][j] >= stop_time[i][j]     (start_time[i][j] == stop_time[i][j]  is not OK)
            }

            /* j-th window stop time <= (j-1)-th window start time (only for chrono windows enabled) */
            if (j < (CHRONO__NUM_OF_WINDOWS - 1))
            {
                if ((ptr_data->chrono_windows[i][j].status) && (ptr_data->chrono_windows[i][j + 1].status))
                {
                    result = Clock_CompareTimes(ptr_data->chrono_windows[i][j    ].stop_hour,
                                                ptr_data->chrono_windows[i][j    ].stop_minute,
                                                0,
                                                ptr_data->chrono_windows[i][j + 1].start_hour,
                                                ptr_data->chrono_windows[i][j + 1].start_minute,
                                                0);
                    if (result == +1)
                        return FALSE;   // stop_time[i][j] > start_time[i][j+1]    (stop_time[i][j]  == start_time[i][j+1]  is OK)
                }
            }
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoExt_GetDefault
 *
 * Description: get the default "external chrono function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FChrono__Config_FChronoExt_GetDefault(F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data)
{
    *ptr_data = FChrono_Config_FChronoExtDefault;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoExt_Get
 *
 * Description: get the "external chrono function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FChrono__Config_FChronoExt_Get(F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data)
{
    *ptr_data = FChrono_Config_FChronoExt;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoExt_Set
 *
 * Description: set the "external chrono function" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FChrono__Config_FChronoExt_Set(F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data)
{
    FChrono_Config_FChronoExt = *ptr_data;
}




/*===========================================================================
 * Function   : FChrono__Config_FChronoExt_IsValid
 *
 * Description: check if the "external chrono function" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FChrono__Config_FChronoExt_IsValid(F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data)
{
    s8 result;
    u8 i;
    u8 j;


    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < CHRONO__NUM_OF_WINDOWS; j++)
        {
            /* temperature to be regulated */
            if (
                 (ptr_data->chrono_windows[i][j].temperature < CHRONO_TEMPERATURE_MIN) ||
                 (ptr_data->chrono_windows[i][j].temperature > CHRONO_TEMPERATURE_MAX)
               )
            {
                return FALSE;
            }

            /* window start time and window stop time */
            if (
                 (ptr_data->chrono_windows[i][j].start_hour   > 23) ||
                 (ptr_data->chrono_windows[i][j].start_minute > 59) ||
                 (ptr_data->chrono_windows[i][j].stop_hour    > 23) ||
                 (ptr_data->chrono_windows[i][j].stop_minute  > 59)
               )
            {
                return FALSE;
            }

            /* window start time < window stop time (only for chrono windows enabled) */
            if (ptr_data->chrono_windows[i][j].status)
            {
                result = Clock_CompareTimes(ptr_data->chrono_windows[i][j].start_hour,
                                            ptr_data->chrono_windows[i][j].start_minute,
                                            0,
                                            ptr_data->chrono_windows[i][j].stop_hour,
                                            ptr_data->chrono_windows[i][j].stop_minute,
                                            0);
                if ((result == 0) || (result == +1))
                    return FALSE;   // start_time[i][j] >= stop_time[i][j]     (start_time[i][j] == stop_time[i][j]  is not OK)
            }

            /* j-th window stop time <= (j-1)-th window start time (only for chrono windows enabled) */
            if (j < (CHRONO__NUM_OF_WINDOWS - 1))
            {
                if ((ptr_data->chrono_windows[i][j].status) && (ptr_data->chrono_windows[i][j + 1].status))
                {
                    result = Clock_CompareTimes(ptr_data->chrono_windows[i][j    ].stop_hour,
                                                ptr_data->chrono_windows[i][j    ].stop_minute,
                                                0,
                                                ptr_data->chrono_windows[i][j + 1].start_hour,
                                                ptr_data->chrono_windows[i][j + 1].start_minute,
                                                0);
                    if (result == +1)
                        return FALSE;   // stop_time[i][j] > start_time[i][j+1]    (stop_time[i][j]  == start_time[i][j+1]  is OK)
                }
            }
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FChrono_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - msg_identifier:
 * Output     : -
 *===========================================================================*/
static void FChrono_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    /* RTC actual time */
    CLOCK__TIME rtc_actual_time;

    /* next chrono event */
    CLOCK__TIME next_chrono_event_int;
    CLOCK__TIME next_chrono_event_ext;

    /* week day */
    u8          week_day_int;
    u8          week_day_ext;

    /* chrono window index */
    u8          chrono_window_index_int;
    u8          chrono_window_index_ext;

    /* "chrono" temperature */
    s16         temperature_int;
    s16         temperature_ext;

    bool        result;


    /* debug */
    snprintf(FChrono_DebugString, sizeof(FChrono_DebugString), "CALLBACK     - MESSAGE     - TASK F_CHRONO - msg identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, FChrono_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier)
    {
        /*---------------------------------------------------------------------------
        * Internal chrono function
        *---------------------------------------------------------------------------*/
        // event - internal chrono event
        case TASK_MSG_ID__CHRONO_EVENT_INT:
            /* get the actual time */
            result = Clock_GetTime(&rtc_actual_time);
            if (!result)
                break;


            /* verify if actual time stays in a enabled internal chrono window */
            result = FChrono_TimeStaysInChronoWindowInt(&rtc_actual_time, &week_day_int, &chrono_window_index_int, &temperature_int);
            if (result)
            {
                /* actual time          stays in a enabled internal chrono window */

                snprintf(FChrono_DebugString, sizeof(FChrono_DebugString), "Actual time stays in a enabled internal chrono window - week_day_int: %d, chrono_window_index_int: %d, temperature_int: %d", week_day_int, chrono_window_index_int, temperature_int);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, FChrono_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* signal internal chrono function on */
                result = Outputs_Event_FChronoInt_On(temperature_int);
            }
            else
            {
                /* actual time does not stay  in a enabled internal chrono window */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Actual time does not stay in a enabled internal chrono window", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* signal internal chrono function off */
                result = Outputs_Event_FChronoInt_Off();
            }


            /* calculate next internal chrono event */
            result = FChrono_NextChronoEventInt(&rtc_actual_time, &next_chrono_event_int);
            if (result)
            {
                /* next internal chrono event available (internal chrono function enabled) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Next internal chrono event available"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                Clock_PrintTime2(DEBUG_TRACE_LEVEL_F_CHRONO, 0, 0, &next_chrono_event_int);

                /* set RTC alarm to "next_chrono_event_int" */
                result = RtcAlarm_RtcAlarmSet(RTC_ALARM__ALARM_F_CHRONO_INT, &next_chrono_event_int);
            }
            else
            {
                /* next internal chrono event not available (internal chrono function disabled) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Next internal chrono event not available", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* delete RTC alarm */
                result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_F_CHRONO_INT);
            }
            break;



        /*---------------------------------------------------------------------------
        * External chrono function
        *---------------------------------------------------------------------------*/
        // event - external chrono event
        case TASK_MSG_ID__CHRONO_EVENT_EXT:
            /* get the actual time */
            result = Clock_GetTime(&rtc_actual_time);
            if (!result)
                break;


            /* verify if actual time stays in a enabled external chrono window */
            result = FChrono_TimeStaysInChronoWindowExt(&rtc_actual_time, &week_day_ext,&chrono_window_index_ext, &temperature_ext);
            if (result)
            {
                /* actual time          stays in a enabled external chrono window */

                snprintf(FChrono_DebugString, sizeof(FChrono_DebugString), "Actual time stays in a enabled external chrono window - week_day_ext: %d, chrono_window_index_ext: %d, temperature_ext: %d", week_day_ext, chrono_window_index_ext, temperature_ext);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, FChrono_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* signal external chrono function on */
                result = Outputs_Event_FChronoExt_On(temperature_ext);
            }
            else
            {
                /* actual time does not stay  in a enabled external chrono window */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Actual time does not stay in a enabled external chrono window", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* signal external chrono function off */
                result = Outputs_Event_FChronoExt_Off();
            }


            /* calculate next external chrono event */
            result = FChrono_NextChronoEventExt(&rtc_actual_time, &next_chrono_event_ext);
            if (result)
            {
                /* next external chrono event available (external chrono function enabled) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Next external chrono event available"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                Clock_PrintTime2(DEBUG_TRACE_LEVEL_F_CHRONO, 0, 0, &next_chrono_event_ext);

                /* set RTC alarm to "next_chrono_event_ext" */
                result = RtcAlarm_RtcAlarmSet(RTC_ALARM__ALARM_F_CHRONO_EXT, &next_chrono_event_ext);
            }
            else
            {
                /* next external chrono event not available (external chrono function disabled) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_CHRONO, DEBUG_TRACE_TYPE_LOW, "Next external chrono event not available", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* delete RTC alarm */
                result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_F_CHRONO_EXT);
            }
            break;



        // unknown event
        default:
            break;
    }
}
