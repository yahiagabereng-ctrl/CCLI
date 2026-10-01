/*=============================================================================
 * File       :  ALARM_POWER.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - power alarm
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "alarm_power.h"
#include "alarm.h"
#include "boot.h"
#include "counters.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "fw_config.h"
#include "out_key.h"
#include "program_flash.h"
#include "regulation.h"
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* last power alarm */
#define LAST_POWER_ALARM__UNKNOWN                              0                    /* last power alarm - unknwon                 */
#define LAST_POWER_ALARM__MAIN_POWER_RETURN                    1                    /* last power alarm - main power return       */
#define LAST_POWER_ALARM__MAIN_POWER_INTERRUPTION              2                    /* last power alarm - main power interruption */

/* timeout values (ms) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TIME_MS__DELAY_AFTER_POWER_INTERRUPTION                ( 3 * 60 * 1000L)    /* time delay after main power supply interruption              (ms) */
#define TIME_MS__DELAY_AFTER_POWER_RETURN                      ( 3 * 60 * 1000L)    /* time delay after main power supply return                    (ms) */
#define TIME_MS__DELAY_SHUTDOWN                                (60 * 60 * 1000L)    /* time delay for shutdown after main power supply interruption (ms) */
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TIME_MS__DELAY_AFTER_POWER_INTERRUPTION                (     15 * 1000L)    /* time delay after main power supply interruption              (ms) */
#define TIME_MS__DELAY_AFTER_POWER_RETURN                      (     15 * 1000L)    /* time delay after main power supply return                    (ms) */
#define TIME_MS__DELAY_SHUTDOWN                                ( 2 * 60 * 1000L)    /* time delay for shutdown after main power supply interruption (ms) */
#endif


/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                                200

/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__POWER_ON                                    0                    /* SEM state - power on       */
#define SEM_STATE__POWER_OFF_TEMP                              1                    /* SEM state - power off temp */
#define SEM_STATE__POWER_OFF                                   2                    /* SEM state - power off      */
#define SEM_STATE__POWER_ON_TEMP                               3                    /* SEM state - power on  temp */

/* SEM state strings */
#define SEM_STATE_STRING__POWER_ON                             "SEM_STATE__POWER_ON"
#define SEM_STATE_STRING__POWER_OFF_TEMP                       "SEM_STATE__POWER_OFF_TEMP"
#define SEM_STATE_STRING__POWER_OFF                            "SEM_STATE__POWER_OFF"
#define SEM_STATE_STRING__POWER_ON_TEMP                        "SEM_STATE__POWER_ON_TEMP"

/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__POWER_INTERRUPTION                          0                    /* SEM event - main power supply interruption         */
#define SEM_EVENT__POWER_RETURN                                1                    /* SEM event - main power supply return               */
#define SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION            2                    /* SEM event - timeout delay after power interruption */
#define SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN                  3                    /* SEM event - timeout delay after power return       */
#define SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN                      4                    /* SEM event - timeout delay for shutdown             */

/* SEM events strings */
#define SEM_EVENT_STRING__POWER_INTERRUPTION                   "SEM_EVENT__POWER_INTERRUPTION"
#define SEM_EVENT_STRING__POWER_RETURN                         "SEM_EVENT__POWER_RETURN"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_POWER_INTERRUPTION     "SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_POWER_RETURN           "SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_SHUTDOWN               "SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN"

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__POWER_INTERRUPTION                        (11100 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__POWER_RETURN                              (11101 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_POWER_INTERRUPTION          (11102 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_POWER_RETURN                (11103 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_SHUTDOWN                    (11104 | (QL_COMPONENT_APP_START << 16))




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* SEM state */
static       u8                                    AlarmPower_SemState = SEM_STATE__POWER_ON;

/* configuration - power alarm */
static       ALARM_POWER__CONFIG__ALARM_POWER      AlarmPower_Config_AlarmPower;
static const ALARM_POWER__CONFIG__ALARM_POWER      AlarmPower_Config_AlarmPowerDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,                         /* power         alarm enabled status */
    TRUE,                         /* power restore alarm enabled status */
#endif
#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,                        /* power         alarm enabled status */
    FALSE,                        /* power restore alarm enabled status */
#endif
};

/* status - last power alarm */
static       ALARM_POWER__STATUS__LAST_POWER_ALARM AlarmPower_Status_LastAlarmPower;
static const ALARM_POWER__STATUS__LAST_POWER_ALARM AlarmPower_Status_LastAlarmPowerDefault =
{
    LAST_POWER_ALARM__UNKNOWN     /* last power alarm */
};

/* debug string */
static       ascii                                 AlarmPower_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                            AlarmPower_TimerHandler_TimerDelayPowerInterruption;    /* timer for delay after main power interruption */
static       ql_timer_t                            AlarmPower_TimerHandler_TimerDelayPowerReturn;          /* timer for delay after main power return       */
static       ql_timer_t                            AlarmPower_TimerHandler_TimerDelayShutdown;             /* timer for delay for shutdown                  */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* alarm power task */
       void AlarmPower_TaskAlarmPower(void *argument);

/* task events */
static void AlarmPower_PowerOkInputEvent(u8 input_id, bool input_status);

/* get/set "power alarm" configuration */
       void AlarmPower_Config_AlarmPower_GetDefault    (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);
       void AlarmPower_Config_AlarmPower_Get           (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);
       void AlarmPower_Config_AlarmPower_Set           (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);
       bool AlarmPower_Config_AlarmPower_IsValid       (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);

/* get/set "last power alarm" status */
       void AlarmPower_Status_LastPowerAlarm_GetDefault(ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data);
       void AlarmPower_Status_LastPowerAlarm_Get       (ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data);
       void AlarmPower_Status_LastPowerAlarm_Set       (ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data);

/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void AlarmPower_TimerDelayPowerInterruption_Start(u32 time_value, bool periodic);
static void AlarmPower_TimerDelayPowerInterruption_Stop (void);

static void AlarmPower_TimerDelayPowerReturn_Start      (u32 time_value, bool periodic);
static void AlarmPower_TimerDelayPowerReturn_Stop       (void);

static void AlarmPower_TimerDelayShutdownReturn_Start   (u32 time_value, bool periodic);
static void AlarmPower_TimerDelayShutdownReturn_Stop    (void);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void AlarmPower_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void AlarmPower_AdlCallback_Timer_TimerDelayPowerInterruption(void *context);
static void AlarmPower_AdlCallback_Timer_TimerDelayPowerReturn      (void *context);
static void AlarmPower_AdlCallback_Timer_TimerDelayShutdown         (void *context);




/*=============================================================================
 * Function   : AlarmPower_TaskAlarmPower
 *
 * Description: Power task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void AlarmPower_TaskAlarmPower(void *argument)
{
    ql_event_t event;
    QlOSStatus err;

    bool       pwr_ok;
    bool       result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - ALARM_POWER - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    pwr_ok = DrvGpio_ReadInput(DRVGPIO__IN__PWR_OK);

    if (pwr_ok)
    {
        /* main power supply     present */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Main power supply present", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        AlarmPower_SemState = SEM_STATE__POWER_ON;

        /* "output keys" unblock */
        OutKey_OutKeysUnblock();

        /* outputs OUT1 and OUT2 are unblocked */
        Regulation_OutputsUnblock();

        /* possible "main power return" alarm */
        if (AlarmPower_Status_LastAlarmPower.last_power_alarm == LAST_POWER_ALARM__MAIN_POWER_INTERRUPTION)
        {
            if (AlarmPower_Config_AlarmPower.alarm_enabled_status_power_restored)
            {
                /* "power restored" alarm enabled */

                /* generate "main power return" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "\"main power return\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__MAIN_POWER_RESTORED, 0);
                if (result)
                {
                    /* update last power alarm */
                    AlarmPower_Status_LastAlarmPower.last_power_alarm = LAST_POWER_ALARM__MAIN_POWER_RETURN;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM);
                }
            }
        }
    }
    else
    {
        /* main power supply not present */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Main power supply not present", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        AlarmPower_SemState = SEM_STATE__POWER_OFF;

        /* "output keys" block */
        OutKey_OutKeysBlock();

        /* outputs OUT1 and OUT2 are blocked */
        Regulation_OutputsBlock();
    }


    /* register the function for signals of digital input event */
    DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__PWR_OK, AlarmPower_PowerOkInputEvent);


    /* creation of timer "TimerDelayPowerInterruption" */
    err = ql_rtos_timer_create(&AlarmPower_TimerHandler_TimerDelayPowerInterruption, QL_TIMER_IN_SERVICE, AlarmPower_AdlCallback_Timer_TimerDelayPowerInterruption, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerDelayPowerReturn" */
    err = ql_rtos_timer_create(&AlarmPower_TimerHandler_TimerDelayPowerReturn, QL_TIMER_IN_SERVICE, AlarmPower_AdlCallback_Timer_TimerDelayPowerReturn, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerDelayShutdown" */
    err = ql_rtos_timer_create(&AlarmPower_TimerHandler_TimerDelayShutdown, QL_TIMER_IN_SERVICE, AlarmPower_AdlCallback_Timer_TimerDelayShutdown, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            AlarmPower_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*===========================================================================
 * Function   : AlarmPower_PowerOkInputEvent
 *
 * Description: signal new PWR_OK input event
 * Input      : - input_id    : input ID
 *              - input_status: input status
 * Output     : -
 *===========================================================================*/
static void AlarmPower_PowerOkInputEvent(u8 input_id, bool input_status)
{
    ql_event_t event;
    QlOSStatus err;


    if (input_id != DRVGPIO__IN__PWR_OK)
        return;


    if (input_status)
        event.id = TASK_MSG_ID__POWER_RETURN;
    else
        event.id = TASK_MSG_ID__POWER_INTERRUPTION;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmPower, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmPower_Config_AlarmPower_GetDefault
 *
 * Description: get the default "power alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmPower_Config_AlarmPower_GetDefault(ALARM_POWER__CONFIG__ALARM_POWER *ptr_data)
{
    *ptr_data = AlarmPower_Config_AlarmPowerDefault;
}




/*===========================================================================
 * Function   : AlarmPower_Config_AlarmPower_Get
 *
 * Description: get the "power alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmPower_Config_AlarmPower_Get(ALARM_POWER__CONFIG__ALARM_POWER *ptr_data)
{
    *ptr_data = AlarmPower_Config_AlarmPower;
}




/*===========================================================================
 * Function   : AlarmPower_Config_AlarmPower_Set
 *
 * Description: set the "power alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmPower_Config_AlarmPower_Set(ALARM_POWER__CONFIG__ALARM_POWER *ptr_data)
{
    AlarmPower_Config_AlarmPower = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmPower_Config_AlarmPower_IsValid
 *
 * Description: check if the "power alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmPower_Config_AlarmPower_IsValid(ALARM_POWER__CONFIG__ALARM_POWER *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmPower_Status_LastPowerAlarm_GetDefault
 *
 * Description: get the default "power alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmPower_Status_LastPowerAlarm_GetDefault(ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data)
{
    *ptr_data = AlarmPower_Status_LastAlarmPowerDefault;
}




/*===========================================================================
 * Function   : AlarmPower_Status_LastPowerAlarm_Get
 *
 * Description: get the "power alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmPower_Status_LastPowerAlarm_Get(ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data)
{
    *ptr_data = AlarmPower_Status_LastAlarmPower;
}




/*===========================================================================
 * Function   : AlarmPower_Status_LastPowerAlarm_Set
 *
 * Description: set the "power alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmPower_Status_LastPowerAlarm_Set(ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data)
{
    AlarmPower_Status_LastAlarmPower = *ptr_data;
}




/*=============================================================================
 * Function   : AlarmPower_TimerDelayPowerInterruption_Start
 *
 * Description: start the "delay power interruption" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmPower_TimerDelayPowerInterruption_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Start \"delay power interruption\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmPower_TimerHandler_TimerDelayPowerInterruption, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_TimerDelayPowerInterruption_Stop
 *
 * Description: stop the "delay power interruption" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmPower_TimerDelayPowerInterruption_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Stop \"delay power interruption\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmPower_TimerHandler_TimerDelayPowerInterruption);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmPower_DebugString, sizeof(AlarmPower_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, AlarmPower_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_TimerDelayPowerReturn_Start
 *
 * Description: start the "delay power return" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmPower_TimerDelayPowerReturn_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Start \"delay power return\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmPower_TimerHandler_TimerDelayPowerReturn, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_TimerDelayPowerReturn_Stop
 *
 * Description: stop the "delay power return" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmPower_TimerDelayPowerReturn_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Stop \"delay power interruption\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmPower_TimerHandler_TimerDelayPowerReturn);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmPower_DebugString, sizeof(AlarmPower_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, AlarmPower_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_TimerDelayShutdownReturn_Start
 *
 * Description: start the "delay for shutdown" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmPower_TimerDelayShutdownReturn_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Start \"delay for shutdown\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmPower_TimerHandler_TimerDelayShutdown, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_TimerDelayShutdownReturn_Stop
 *
 * Description: stop the "delay for shutdown" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmPower_TimerDelayShutdownReturn_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "Stop \"delay for shutdown\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmPower_TimerHandler_TimerDelayShutdown);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmPower_DebugString, sizeof(AlarmPower_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, AlarmPower_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmPower_AdlCallback_Message_TaskMsg
 *
 * Description: - task power message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void AlarmPower_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
                 u32    sem_event;

                 bool   result;

    static const ascii *sem_state_strings[] =
    {
        SEM_STATE_STRING__POWER_ON,
        SEM_STATE_STRING__POWER_OFF_TEMP,
        SEM_STATE_STRING__POWER_OFF,
        SEM_STATE_STRING__POWER_ON_TEMP,
    };

    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__POWER_INTERRUPTION,
        SEM_EVENT_STRING__POWER_RETURN,
        SEM_EVENT_STRING__TIMEOUT_DELAY_POWER_INTERRUPTION,
        SEM_EVENT_STRING__TIMEOUT_DELAY_POWER_RETURN,
        SEM_EVENT_STRING__TIMEOUT_DELAY_SHUTDOWN,
    };


    /* debug */
    snprintf(AlarmPower_DebugString, sizeof(AlarmPower_DebugString), "CALLBACK     - MESSAGE     - TASK ALARM POWER - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, AlarmPower_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        case TASK_MSG_ID__POWER_INTERRUPTION:
            sem_event = SEM_EVENT__POWER_INTERRUPTION;
            break;

        case TASK_MSG_ID__POWER_RETURN:
            sem_event = SEM_EVENT__POWER_RETURN;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_POWER_INTERRUPTION:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_POWER_RETURN:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_SHUTDOWN:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN;
            break;

        default:
            return;
    }


    snprintf(AlarmPower_DebugString, sizeof(AlarmPower_DebugString), "SEM state: %s", sem_state_strings[AlarmPower_SemState]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, AlarmPower_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(AlarmPower_DebugString, sizeof(AlarmPower_DebugString), "SEM event: %s", sem_event_strings[sem_event          ]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, AlarmPower_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (AlarmPower_SemState)
    {
        /* SEM state - power on */
        case SEM_STATE__POWER_ON:
            switch (sem_event)
            {
                /* SEM event - main power supply interruption */
                case SEM_EVENT__POWER_INTERRUPTION:
                    /* start timer delay after main power interruption (3 min) */
                    AlarmPower_TimerDelayPowerInterruption_Start(TIME_MS__DELAY_AFTER_POWER_INTERRUPTION, FALSE);

                    /* start timer delay for shutdown after main power interruption (30 min) */
                    AlarmPower_TimerDelayShutdownReturn_Start(TIME_MS__DELAY_SHUTDOWN, FALSE);

                    /* "output keys" block */
                    OutKey_OutKeysBlock();

                    /* outputs OUT1 and OUT2 are blocked */
                    Regulation_OutputsBlock();

                    AlarmPower_SemState = SEM_STATE__POWER_OFF_TEMP;
                    break;


                /* SEM event - main power supply return               */
                /* SEM event - timeout delay after power interruption */
                /* SEM event - timeout delay after power return       */
                /* SEM event - timeout delay for shutdown             */
                case SEM_EVENT__POWER_RETURN:
                case SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION:
                case SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN:
                case SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN:
                    // NOTHING TO DO
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - power off temp */
        case SEM_STATE__POWER_OFF_TEMP:
            switch (sem_event)
            {
                /* SEM event - main power supply return */
                case SEM_EVENT__POWER_RETURN:
                    /* stop timer delay after main power interruption (3 min) */
                    AlarmPower_TimerDelayPowerInterruption_Stop();

                    /* stop timer delay for shutdown after main power interruption (30 min) */
                    AlarmPower_TimerDelayShutdownReturn_Stop();

                    /* "output keys" unblock */
                    OutKey_OutKeysUnblock();

                    /* outputs OUT1 and OUT2 are unblocked */
                    Regulation_OutputsUnblock();

                    AlarmPower_SemState = SEM_STATE__POWER_ON;
                    break;


                /* SEM event - timeout delay after power interruption */
                case SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION:
                    if (AlarmPower_Config_AlarmPower.alarm_enabled_status_power)
                    {
                        /* "power" alarm enabled */

                        /* generate "main power return" alarm */
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "\"main power interruption\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__MAIN_POWER, 0);
                        if (result)
                        {
                            /* update last power alarm */
                            AlarmPower_Status_LastAlarmPower.last_power_alarm = LAST_POWER_ALARM__MAIN_POWER_INTERRUPTION;

                            /* request the backup to flash objects */
                            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM);
                        }
                    }

                    /* signal the main power supply problem to counters manager */
                    Counters_Event_MainPowerSupplyProblem();

                    AlarmPower_SemState = SEM_STATE__POWER_OFF;
                    break;


                /* SEM event - main power supply interruption   */
                /* SEM event - timeout delay after power return */
                case SEM_EVENT__POWER_INTERRUPTION:
                case SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN:
                    // NOTHING TO DO
                    break;


                /* SEM event - timeout delay for shutdown */
                case SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN:
                    /* signal the main power supply problem to counters manager */
                    Counters_Event_MainPowerSupplyProblem();

                    /* shutdown the module */
#ifdef FUNCTION_IMPLEMENTED
                    Main_ShutdownModule();
#endif
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - power off */
        case SEM_STATE__POWER_OFF:
            switch (sem_event)
            {
                /* SEM event - main power supply return */
                case SEM_EVENT__POWER_RETURN:
                    /* start timer delay after main power return (3 min) */
                    AlarmPower_TimerDelayPowerReturn_Start(TIME_MS__DELAY_AFTER_POWER_RETURN, FALSE);

                    /* stop timer delay for shutdown after main power interruption (30 min) */
                    AlarmPower_TimerDelayShutdownReturn_Stop();

                    /* "output keys" unblock */
                    OutKey_OutKeysUnblock();

                    /* outputs OUT1 and OUT2 are unblocked */
                    Regulation_OutputsUnblock();

                    AlarmPower_SemState = SEM_STATE__POWER_ON_TEMP;
                    break;


                /* SEM event - main power supply interruption         */
                /* SEM event - timeout delay after power interruption */
                /* SEM event - timeout delay after power return       */
                case SEM_EVENT__POWER_INTERRUPTION:
                case SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION:
                case SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN:
                    // NOTHING TO DO
                    break;


                /* SEM event - timeout delay for shutdown */
                case SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN:
                    /* signal the main power supply problem to counters manager */
                    Counters_Event_MainPowerSupplyProblem();

                    /* shutdown the module */
#ifdef FUNCTION_IMPLEMENTED
                    Main_ShutdownModule();
#endif
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - power on temp */
        case SEM_STATE__POWER_ON_TEMP:
            switch (sem_event)
            {
                /* SEM event - main power supply interruption */
                case SEM_EVENT__POWER_INTERRUPTION:
                    /* stop timer delay after main power return (3 min) */
                    AlarmPower_TimerDelayPowerReturn_Stop();

                    /* start timer delay for shutdown after main power interruption (30 min) */
                    AlarmPower_TimerDelayShutdownReturn_Start(TIME_MS__DELAY_SHUTDOWN, FALSE);

                    /* "output keys" block */
                    OutKey_OutKeysBlock();

                    /* outputs OUT1 and OUT2 are blocked */
                    Regulation_OutputsBlock();

                    AlarmPower_SemState = SEM_STATE__POWER_OFF;
                    break;


                /* SEM event - timeout delay after power return */
                case SEM_EVENT__TIMEOUT_DELAY_POWER_RETURN:
                    if (AlarmPower_Config_AlarmPower.alarm_enabled_status_power_restored)
                    {
                        /* "power restored" alarm enabled */

                        /* generate "main power return" alarm */
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "\"main power return\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__MAIN_POWER_RESTORED, 0);
                        if (result)
                        {
                            /* update last power alarm */
                            AlarmPower_Status_LastAlarmPower.last_power_alarm = LAST_POWER_ALARM__MAIN_POWER_RETURN;

                            /* request the backup to flash objects */
                            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__004_ALARM, PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM);
                        }
                    }

                    AlarmPower_SemState = SEM_STATE__POWER_ON;
                    break;


                /* SEM event - main power supply return               */
                /* SEM event - timeout delay after power interruption */
                /* SEM event - timeout delay for shutdown             */
                case SEM_EVENT__POWER_RETURN:
                case SEM_EVENT__TIMEOUT_DELAY_POWER_INTERRUPTION:
                case SEM_EVENT__TIMEOUT_DELAY_SHUTDOWN:
                    // NOTHING TO DO
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - unknown state */
        default:
            AlarmPower_SemState = SEM_STATE__POWER_ON;
            break;
    }
}




/*=============================================================================
 * Function   : AlarmPower_AdlCallback_Timer_TimerDelayPowerInterruption
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmPower_AdlCallback_Timer_TimerDelayPowerInterruption(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - Delay Power Interruption", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_POWER_INTERRUPTION;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmPower, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_AdlCallback_Timer_TimerDelayPowerReturn
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmPower_AdlCallback_Timer_TimerDelayPowerReturn(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - Delay Power Return", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_POWER_RETURN;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmPower, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmPower_AdlCallback_Timer_TimerDelayShutdown
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmPower_AdlCallback_Timer_TimerDelayShutdown(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - Delay for Shutdown", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_SHUTDOWN;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmPower, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_POWER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
