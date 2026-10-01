/*=============================================================================
 * File       :  MAIN.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - main
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_api_dev.h"
#include "ql_api_rtc.h"
#include "ql_power.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "fw_config.h"
#include "http_my.h"
#include "main.h"
#include "out_key.h"
#include "program_flash.h"
#include "queue_my.h"
#include "reset.h"
#include "startup.h"
#include "synchronize.h"

#ifdef FUNCTION_IMPLEMENTED
#include "file_system.h"
#include "terminal.h"
#endif




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* timeout values (ms) */
#define TIME_MS__PERIOD__WDT_REARM               ( 4 * 1000L)                            /* time period for re-arm Application Watchdog (ms) */
#define TIME_MS__POLLING                         (10 * 1000L)                            /* time polling                                (ms) */

/* timeout values (min) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TIME_MIN__PERIODIC_RESET_TIMER           ((24 * 60) + 5)                         /* time for periodic reset (with timer  ) (min) */
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TIME_MIN__PERIODIC_RESET_TIMER           ((     10)    )                         /* time for periodic reset (with timer  ) (min) */
#endif

/* battery voltage threshold (mV) */
#define BATTERY_VOLTAGE_CRITICAL_POINT           3400                                    /* critical point voltage threshold (mV) */

/* task message IDs */
#define MAIN__TASK_MSG_ID__END_OF_SYNCHRONIZE    (12800 | (QL_COMPONENT_APP_START << 16))

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                  200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* debug string */
static ascii      Main_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* timer handlers */
static ql_timer_t Main_TimerHandler_TimerWdtRearm;   // timer for Application Watchdog rearm
static ql_timer_t Main_TimerHandler_TimerReset;      // timer for periodic reset
static ql_timer_t Main_TimerHandler_TimerPolling;    // timer for polling




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* main task */
       void Main_TaskMain(void *argument);

/* event signal */
       void Main_EndOfSynchronize(void);

/* reset/restart/shutdown module */
       void Main_ResetModule   (void);
       void Main_RestartModule (void);
       void Main_ShutdownModule(void);


/*---------------------------------------------------------------------------
 * Timers
 *---------------------------------------------------------------------------*/
/* action on timers */

/* timer for Application Watchdog rearm */
static void Main_TimerWdtRearm_Start(u32 time_value, bool periodic);
static void Main_TimerWdtRearm_Stop (void);

/* timer for periodic reset */
static void Main_TimerReset_Start   (u32 time_value, bool periodic);

/* timer for polling */
static void Main_TimerPolling_Start (u32 time_value, bool periodic);


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message              callback functions */
static void Main_AdlCallback_Message_TaskMsg(u32 msg_identifier);

/* timer                callback functions */
static void Main_AdlCallback_Timer_TimerWdtRearm(void *ptr_context);
static void Main_AdlCallback_Timer_TimerReset   (void *ptr_context);
static void Main_AdlCallback_Timer_TimerPolling (void *ptr_context);




/*=============================================================================
 * Function   : Main_TaskMain
 *
 * Description: main task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_TaskMain(void *argument)
{
    ql_event_t               event;
    QlOSStatus               err;

    /* synchronize request SMS counters */
    SYNCHRONIZE__SMS_COUNTER synchronize_sms_counter;

    u8                       startup_type;

    bool                     result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - MAIN - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /*----------------------------------------------------------------
     * Get startup type
     *----------------------------------------------------------------*/
    /* get the startup type */
    startup_type = Startup_GetStartupType();


    /*----------------------------------------------------------------
     * Timers creation
     *----------------------------------------------------------------*/
    /* creation of timer "TimerWdtRearm" */
    err = ql_rtos_timer_create(&Main_TimerHandler_TimerWdtRearm, Boot_TaskRef_Main, Main_AdlCallback_Timer_TimerWdtRearm, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerReset" */
    err = ql_rtos_timer_create(&Main_TimerHandler_TimerReset, Boot_TaskRef_Main, Main_AdlCallback_Timer_TimerReset, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerPolling" */
    err = ql_rtos_timer_create(&Main_TimerHandler_TimerPolling, Boot_TaskRef_Main, Main_AdlCallback_Timer_TimerPolling, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /*----------------------------------------------------------------
     * Manage Application watchdog
     *----------------------------------------------------------------*/
    /* active Application Watchdog */
  //adl_wdActiveAppWd(TIME_MS__APPLICATION_WATCHDOG);

    /* start periodic timer for Application Watchdog rearm */
    Main_TimerWdtRearm_Start(TIME_MS__PERIOD__WDT_REARM, TRUE);


    /*----------------------------------------------------------------
     * Manage +WBCI indication
     *----------------------------------------------------------------*/
    /* "+WBCI: " unsolicited response subscription */
    Main_TimerPolling_Start(TIME_MS__POLLING, TRUE);


    /*----------------------------------------------------------------
     * Init
     *----------------------------------------------------------------*/
#ifdef FUNCTION_IMPLEMENTED
    /* terminal init */
    Terminal_Init();
#endif

    /* init */
    OutKey_Init();
    Queue_Init();

    /* HTTP init */
    Http_Init();


    /*----------------------------------------------------------------
     * Generate a autosynchronize record
     *----------------------------------------------------------------*/
    if (
         /* startup type - power-on manual */
         /* startup type - DOTA            */
         (startup_type == RESET__STARTUP_TYPE__POWER_ON_MANUAL) ||
         (startup_type == RESET__STARTUP_TYPE__DOTA           )
       )
    {
        /* get    the autosynchronize SMS counter */
        Synchronize_SmsCounter_Get(&synchronize_sms_counter);

        /* update the autosynchronize SMS counter */
        synchronize_sms_counter.sms_counter++;
        Synchronize_SmsCounter_Set(&synchronize_sms_counter);

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE, PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER);

        /* put a record in the autosynchronize transmission queue */
        result = Queue_Autosynchronize_PutRecord((QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *)&synchronize_sms_counter);
    }


    /*----------------------------------------------------------------
     * Timer for periodic reset
     *----------------------------------------------------------------*/
    /* start timer for periodic reset */
    Main_TimerReset_Start((TIME_MIN__PERIODIC_RESET_TIMER * 60 * 1000L), FALSE);


    /*----------------------------------------------------------------
     * init delay
     *----------------------------------------------------------------*/
    /* init delay */
    ql_rtos_task_sleep_ms(200L);    


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Main_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : Main_EndOfSynchronize
 *
 * Description: signal the end of synchronize
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_EndOfSynchronize(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = MAIN__TASK_MSG_ID__END_OF_SYNCHRONIZE;

    err = ql_rtos_event_send(Boot_TaskRef_Main, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Main_ResetModule
 *
 * Description: reset the module
 *                  - stop refreshing watchdog
 *                  - infinite loop
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_ResetModule(void)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Reset the module...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop periodic timer for Application Watchdog rearm */
    Main_TimerWdtRearm_Stop();

    /* infinite loop */
    while (1);
}




/*=============================================================================
 * Function   : Main_RestartModule
 *
 * Description: restart the module
 *                  - AT+CFUN=1
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_RestartModule(void)
{
    ql_errcode_power res_pm;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Restart the module...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    res_pm = ql_power_reset(RESET_NORMAL);
    if (res_pm != QL_POWER_RESET_SUCCESS)
    {
        snprintf(Main_DebugString, sizeof(Main_DebugString), "ql_power_reset ERROR: %d", res_pm);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_power_reset OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Main_ShutdownModule
 *
 * Description: shutdown the module
 *                  - AT+CPOF
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_ShutdownModule(void)
{
    ql_errcode_power res_pm;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Shutdown the module...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    res_pm = ql_power_down(POWD_NORMAL);
    if (res_pm != QL_POWER_POWD_SUCCESS)
    {
        snprintf(Main_DebugString, sizeof(Main_DebugString), "ql_power_down ERROR: %d", res_pm);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_power_down OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Main_TimerWdtRearm_Start
 *
 * Description: start the "WDT rearm" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Main_TimerWdtRearm_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Start \"WDT rearm\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Main_TimerHandler_TimerWdtRearm, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Main_TimerWdtRearm_Stop
 *
 * Description: stop the "WDT rearm" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Main_TimerWdtRearm_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Stop \"WDT rearm\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Main_TimerHandler_TimerWdtRearm);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Main_DebugString, sizeof(Main_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Main_TimerReset_Start
 *
 * Description: start the "reset" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Main_TimerReset_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Start \"reset\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Main_TimerHandler_TimerReset, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Main_TimerPolling_Start
 *
 * Description: start the "polling" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Main_TimerPolling_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "Start \"polling\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Main_TimerHandler_TimerPolling, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Main_AdlCallback_Message_TaskMsg
 *
 * Description: - task main message callback
 * Input      : - msg_identifier:
 * Output     : -
 *===========================================================================*/
static void Main_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    snprintf(Main_DebugString, sizeof(Main_DebugString), "CALLBACK     - MESSAGE     - TASK MAIN - msg_identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}




/*===========================================================================
 * Function    : Main_AdlCallback_Timer_TimerWdtRearm
 *
 * Description :
 * Input       : - ptr_context:
 * Output      : -
 *===========================================================================*/
static void Main_AdlCallback_Timer_TimerWdtRearm(void *ptr_context)
{
    ql_errcode_dev_e result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - WDT REARM", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* re-arm the Application Watchdog */
    result = ql_dev_feed_wdt();
    if (result != QL_DEV_SUCCESS)
    {
        snprintf(Main_DebugString, sizeof(Main_DebugString), "ql_dev_feed_wdt ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_dev_feed_wdt OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function    : Main_AdlCallback_Timer_TimerReset
 *
 * Description :
 * Input       : - ptr_context:
 * Output      : -
 *===========================================================================*/
static void Main_AdlCallback_Timer_TimerReset(void *ptr_context)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - RESET", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
#ifdef FUNCTION_IMPLEMENTED
    RESET__POWER_ON_RESET_TYPE poweron_reset_type;

    u8                         counter;

    bool                       result;
    ql_errcode_power           res_pm;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - RESET", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* set the type of power-on reset (application restart) */
    poweron_reset_type = RESET__POWERON_RESET_TYPE__RESTART;
    Reset_PowerOnResetType_Set(&poweron_reset_type);

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE);


    /* variable delay before module reset (it is necessary to complete the flash backup) */
    counter = 0;
    while (counter < 20)
    {
        result = ProgramFlash_IsBackupPendent();

        if (!result)
            break;

        ql_rtos_task_sleep_ms(100L);

        counter++;
    }


    /* additional delay before resetting the module */
    ql_rtos_task_sleep_ms(500L);


    /* reset the module */
    res_pm = ql_power_reset(RESET_NORMAL);  //@@@
    if (res_pm != QL_POWER_RESET_SUCCESS)
    {
        snprintf(Main_DebugString, sizeof(Main_DebugString), "ql_power_reset ERROR: %d", res_pm);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        while(1);  // cause reset for Application Watchdog
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_power_reset OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
#endif
}




/*=============================================================================
 * Function   : Main_AdlCallback_Timer_TimerPolling
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void Main_AdlCallback_Timer_TimerPolling(void *ptr_context)
{
    u32               adc_mv_value;

    ql_errcode_charge res_charge;
    ql_errcode_power  res_power;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - MAIN POLLING", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    res_charge = ql_get_battery_vol(&adc_mv_value);
    if (res_charge == QL_CHARGE_SUCCESS)
    {
        if (adc_mv_value < BATTERY_VOLTAGE_CRITICAL_POINT)
        {
            /* "+WBCI: 0" */

            /* send the "AT+CPOF" AT command to turn off the module */
            res_power = ql_power_down(POWD_NORMAL);
            if (res_power != QL_POWER_POWD_SUCCESS)
            {
                snprintf(Main_DebugString, sizeof(Main_DebugString), "ql_power_down ERROR: %d", res_power);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, Main_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_MAIN, DEBUG_TRACE_TYPE_LOW, "ql_power_down OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
        }
    }
}
