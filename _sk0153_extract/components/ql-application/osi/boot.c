/*=============================================================================
 * File       :  BOOT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  BOOT - boot
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* API      includes */
#include "ql_api_osi.h"
#include "ql_api_dev.h"
#include "ql_power.h"

/* user     includes */
#include "alarm_input.h"
#include "alarm_power.h"
#include "alarm_tmax.h"
#include "alarm_tmin.h"
#include "alarm.h"
#include "at_debug.h"
#include "boot_flash.h"
#include "boot.h"
#include "calendar.h"
#include "charger.h"
#include "counters.h"
#include "debug_my.h"
#include "dota_flash.h"
#include "dota_main.h"
#include "drvgpio.h"
#include "drvtemperature.h"
#include "energy.h"
#include "f_antifrost.h"
#include "f_chrono.h"
#include "f_regulation.h"
#include "fv_action.h"
#include "http_my.h"
#include "input_event.h"
#include "led.h"
#include "log_status.h"
#include "main.h"
#include "outputs.h"
#include "phone.h"
#include "program_flash.h"
#include "regulation.h"
#include "rtc_alarm.h"
#include "startup.h"
#include "synchronize.h"
#include "transmission_gprs.h"
#include "transmission_sms.h"
#include "ui_led.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING       200


/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define BOOT_MESSAGE__GPRS_APN_START  (13300 | (QL_COMPONENT_APP_START << 16))    // GPRS APN start
#define BOOT_MESSAGE__GPRS_APN_STOP   (13301 | (QL_COMPONENT_APP_START << 16))    // GPRS APN stop
#define BOOT_MESSAGE__HTTP_GET_RX     (13302 | (QL_COMPONENT_APP_START << 16))    // HTTP GET "RX"
#define BOOT_MESSAGE__HTTP_GET_TX     (13303 | (QL_COMPONENT_APP_START << 16))    // HTTP GET "TX"
#define BOOT_MESSAGE__HTTP_POST       (13304 | (QL_COMPONENT_APP_START << 16))    // HTTP POST
#define BOOT_MESSAGE__SMTP            (13305 | (QL_COMPONENT_APP_START << 16))    // SMTP




/*=============================================================================
 * VARIABLES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Application variables
 *-----------------------------------------------------------------------------*/
/* boot mode */
static       BOOT__BOOT_MODE Boot_BootMode;
static const BOOT__BOOT_MODE Boot_BootModeDefault =
{
    BOOT__PROGRAM_MODE,
};

/* debug string */
static       ascii           Boot_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
             ql_task_t       Boot_TaskRef_Boot;
             ql_task_t       Boot_TaskRef_Startup;
             ql_task_t       Boot_TaskRef_Counters;
             ql_task_t       Boot_TaskRef_DrvGpio;
             ql_task_t       Boot_TaskRef_Led;
             ql_task_t       Boot_TaskRef_Main;
             ql_task_t       Boot_TaskRef_UiLed;
             ql_task_t       Boot_TaskRef_RtcAlarm;
             ql_task_t       Boot_TaskRef_Phone;
             ql_task_t       Boot_TaskRef_TransmissionMail;
             ql_task_t       Boot_TaskRef_TransmissionGprs;
             ql_task_t       Boot_TaskRef_TransmissionSms;
             ql_task_t       Boot_TaskRef_DrvTemperature;
             ql_task_t       Boot_TaskRef_Regulation;
             ql_task_t       Boot_TaskRef_Outputs;
             ql_task_t       Boot_TaskRef_FvAction;
             ql_task_t       Boot_TaskRef_FAntifrost;
             ql_task_t       Boot_TaskRef_FRegulation;
             ql_task_t       Boot_TaskRef_FChrono;
             ql_task_t       Boot_TaskRef_Energy;
             ql_task_t       Boot_TaskRef_AlarmTMax;
             ql_task_t       Boot_TaskRef_AlarmTMin;
             ql_task_t       Boot_TaskRef_AlarmInput;
             ql_task_t       Boot_TaskRef_AlarmPower;
             ql_task_t       Boot_TaskRef_Alarm;
             ql_task_t       Boot_TaskRef_LogStatus;
             ql_task_t       Boot_TaskRef_InputEvent;
             ql_task_t       Boot_TaskRef_Charger;
             ql_task_t       Boot_TaskRef_Synchronize;
             ql_task_t       Boot_TaskRef_Calendar;
             ql_task_t       Boot_TaskRef_ProgramFlash;
             ql_task_t       Boot_TaskRef_Dota;


/*---------------------------------------------------------------------------
 * Tasks table
 *---------------------------------------------------------------------------*/
static const adl_InitTasks_t adl_InitTasks[] =
{
    /* ENTRY POINT                             STACK SIZE      NAME                          PRIORITY */

    /*-----------------------------------------------------------------
     * Program
     *-----------------------------------------------------------------*/
    {Startup_TaskStartup,                      (16 * 1024),    "Startup",             10,    APP_PRIORITY_LOW},    // startup                task   (highest priority)
    {Counters_TaskCounters,                    (16 * 1024),    "Counters",            10,    APP_PRIORITY_LOW},    // counters               task
    {DrvGpio_TaskDrvGpio,                      (16 * 1024),    "DrvGpio",             10,    APP_PRIORITY_LOW},    // GPIOs                  task
    {Led_TaskLed,                              (16 * 1024),    "Led",                 10,    APP_PRIORITY_LOW},    // led                    task
    {Main_TaskMain,                            (10 * 1024),    "Main",                10,    APP_PRIORITY_LOW},    // main                   task
    {UiLed_TaskUiLed,                          (16 * 1024),    "UiLed",               10,    APP_PRIORITY_LOW},    // UI led                 task
    {RtcAlarm_TaskRtcAlarm,                    (16 * 1024),    "RtcAlarm",            10,    APP_PRIORITY_LOW},    // RTC alarm              task
    {Phone_TaskPhone,                          (16 * 1024),    "Phone",               10,    APP_PRIORITY_LOW},    // phone                  task
  //{TransmissionMail_TaskTransmissionMail,    ( 4 * 1024),    "TransmissionMail",    10,    APP_PRIORITY_LOW},    // transmission mail      task
    {TransmissionGprs_TaskTransmissionGprs,    (16 * 1024),    "TransmissionGprs",    10,    APP_PRIORITY_LOW},    // transmission GPRS      task
    {TransmissionSms_TaskTransmissionSms,      (16 * 1024),    "TransmissionSms",     10,    APP_PRIORITY_LOW},    // transmission SMS       task
    {DrvTemperature_TaskDrvTemperature,        (16 * 1024),    "DrvTemperature",      10,    APP_PRIORITY_LOW},    // temperature driver     task
    {Regulation_TaskRegulation,                (16 * 1024),    "Regulation",          10,    APP_PRIORITY_LOW},    // temperature regulation task
    {Outputs_TaskOutputs,                      (16 * 1024),    "Outputs",             10,    APP_PRIORITY_LOW},    // outputs manager        task
    {FvAction_TaskFvAction,                    (16 * 1024),    "FvAction",            10,    APP_PRIORITY_LOW},    // FV action              task
    {FAntifrost_TaskFAntifrost,                (16 * 1024),    "FAntifrost",          10,    APP_PRIORITY_LOW},    // "antifrost"  function  task
    {FRegulation_TaskFRegulation,              (16 * 1024),    "FRegulation",         10,    APP_PRIORITY_LOW},    // "regulation" function  task
    {FChrono_TaskFChrono,                      (16 * 1024),    "FChrono",             10,    APP_PRIORITY_LOW},    // "chrono"     function  task
    {Energy_TaskEnergy,                        ( 8 * 1024),    "Energy",              10,    APP_PRIORITY_LOW},    // energy                 task
    {AlarmTMax_TaskAlarmTMax,                  (16 * 1024),    "AlarmTMax",           10,    APP_PRIORITY_LOW},    // t_max alarm            task
    {AlarmTMin_TaskAlarmTMin,                  (16 * 1024),    "AlarmTMin",           10,    APP_PRIORITY_LOW},    // t_max alarm            task
    {AlarmInput_TaskAlarmInput,                (16 * 1024),    "AlarmInput",          10,    APP_PRIORITY_LOW},    // input alarm            task
    {AlarmPower_TaskAlarmPower,                (16 * 1024),    "AlarmPower",          10,    APP_PRIORITY_LOW},    // power alarm            task
    {Alarm_TaskAlarm,                          ( 8 * 1024),    "Alarm",               10,    APP_PRIORITY_LOW},    // alarm                  task
    {LogStatus_TaskLogStatus,                  (16 * 1024),    "LogStatus",           10,    APP_PRIORITY_LOW},    // log status             task
    {InputEvent_TaskInputEvent,                (16 * 1024),    "InputEvent",          10,    APP_PRIORITY_LOW},    // input event            task
    {Charger_TaskCharger,                      (16 * 1024),    "Charger",             10,    APP_PRIORITY_LOW},    // charger                task
    {Synchronize_TaskSynchronize,              ( 8 * 1024),    "Synchronize",         10,    APP_PRIORITY_LOW},    // synchronize            task
    {Calendar_TaskCalendar,                    (10 * 1024),    "Calendar",            10,    APP_PRIORITY_LOW},    // calendar               task
    {ProgramFlash_TaskProgramFlash,            (16 * 1024),    "ProgramFlash",        25,    APP_PRIORITY_LOW},    // flash                  task

    /*-----------------------------------------------------------------
     * DOTA
     *-----------------------------------------------------------------*/
    {Dota_TaskDota,                            ( 8 * 1024),    "Dota",                10,    APP_PRIORITY_LOW}     // DOTA                   task   (lowest  priority)
};


/* All tasks IDs array */
static ql_task_t * const Boot_AllTasksIDArray[] =
{
    /* Program tasks IDs array */
    &Boot_TaskRef_Startup,              // startup                task
    &Boot_TaskRef_Counters,             // counters               task
    &Boot_TaskRef_DrvGpio,              // GPIOs                  task
    &Boot_TaskRef_Led,                  // led                    task
    &Boot_TaskRef_Main,                 // main                   task
    &Boot_TaskRef_UiLed,                // UI led                 task
    &Boot_TaskRef_RtcAlarm,             // RTC alarm              task
    &Boot_TaskRef_Phone,                // phone                  task
  //&Boot_TaskRef_TransmissionMail,     // transmission mail      task
    &Boot_TaskRef_TransmissionGprs,     // transmission GPRS      task
    &Boot_TaskRef_TransmissionSms,      // transmission SMS       task
    &Boot_TaskRef_DrvTemperature,       // temperature driver     task
    &Boot_TaskRef_Regulation,           // temperature regulation task
    &Boot_TaskRef_Outputs,              // outputs manager        task
    &Boot_TaskRef_FvAction,             // FV action              task
    &Boot_TaskRef_FAntifrost,           // "antifrost"  function  task
    &Boot_TaskRef_FRegulation,          // "regulation" function  task
    &Boot_TaskRef_FChrono,              // "chrono"     function  task
    &Boot_TaskRef_Energy,               // energy                 task
    &Boot_TaskRef_AlarmTMax,            // t_max alarm            task
    &Boot_TaskRef_AlarmTMin,            // t_min alarm            task
    &Boot_TaskRef_AlarmInput,           // input alarm            task
    &Boot_TaskRef_AlarmPower,           // power alarm            task
    &Boot_TaskRef_Alarm,                // alarm                  task
    &Boot_TaskRef_LogStatus,            // log status             task
    &Boot_TaskRef_InputEvent,           // input event            task
    &Boot_TaskRef_Charger,              // charger                task
    &Boot_TaskRef_Synchronize,          // synchronize            task
    &Boot_TaskRef_Calendar,             // calendar               task
    &Boot_TaskRef_ProgramFlash,         // flash                  task

    /* DOTA tasks    IDs array */
    &Boot_TaskRef_Dota,                 // DOTA                   task
};


/* Program tasks IDs array */
static ql_task_t * const Boot_ProgramTasksIDArray[] =
{
    &Boot_TaskRef_Startup,              // startup                task
    &Boot_TaskRef_Counters,             // counters               task
    &Boot_TaskRef_DrvGpio,              // GPIOs                  task
    &Boot_TaskRef_Led,                  // led                    task
    &Boot_TaskRef_Main,                 // main                   task
    &Boot_TaskRef_UiLed,                // UI led                 task
    &Boot_TaskRef_RtcAlarm,             // RTC alarm              task
    &Boot_TaskRef_Phone,                // phone                  task
  //&Boot_TaskRef_TransmissionMail,     // transmission mail      task
    &Boot_TaskRef_TransmissionGprs,     // transmission GPRS      task
    &Boot_TaskRef_TransmissionSms,      // transmission SMS       task
    &Boot_TaskRef_DrvTemperature,       // temperature driver     task
    &Boot_TaskRef_Regulation,           // temperature regulation task
    &Boot_TaskRef_Outputs,              // outputs manager        task
    &Boot_TaskRef_FvAction,             // FV action              task
    &Boot_TaskRef_FAntifrost,           // "antifrost"  function  task
    &Boot_TaskRef_FRegulation,          // "regulation" function  task
    &Boot_TaskRef_FChrono,              // "chrono"     function  task
    &Boot_TaskRef_Energy,               // energy                 task
    &Boot_TaskRef_AlarmTMax,            // t_max alarm            task
    &Boot_TaskRef_AlarmTMin,            // t_min alarm            task
    &Boot_TaskRef_AlarmInput,           // input alarm            task
    &Boot_TaskRef_AlarmPower,           // power alarm            task
    &Boot_TaskRef_Alarm,                // alarm                  task
    &Boot_TaskRef_LogStatus,            // log status             task
    &Boot_TaskRef_InputEvent,           // input event            task
    &Boot_TaskRef_Charger,              // charger                task
    &Boot_TaskRef_Synchronize,          // synchronize            task
    &Boot_TaskRef_Calendar,             // calendar               task
    &Boot_TaskRef_ProgramFlash,         // flash                  task
};


/* DOTA tasks IDs array */
static ql_task_t * const Boot_DotaTasksIDArray[] =
{
    &Boot_TaskRef_Dota,                 // DOTA                   task
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Boot_TaskBoot(void *argument);


/*-----------------------------------------------------------------------------
 * Actions on phone
 *-----------------------------------------------------------------------------*/
/* GPRS connection */
       bool Boot_GprsConnectionStart(void);
       bool Boot_GprsConnectionStop (void);
       bool Boot_ClientHttpGetRxRequest(void);
       bool Boot_ClientHttpGetTxRequest(void);
       bool Boot_ClientHttpPostRequest (void);
       bool Boot_ClientSmtpRequest(void);


/*-----------------------------------------------------------------------------
 * ???
 *-----------------------------------------------------------------------------*/
/* get/set boot mode */
       void Boot_BootMode_GetDefault(BOOT__BOOT_MODE *ptr_data);
       void Boot_BootMode_Get       (BOOT__BOOT_MODE *ptr_data);
       void Boot_BootMode_Set       (BOOT__BOOT_MODE *ptr_data);

/* callbacks */
static void Boot_PwrkeyCallback(void);
static void Boot_PwrkeyLongPressCallback(void);
static void Boot_PwrkeyPressCallback(void);
static void Boot_PwrkeyReleaseCallback(void);

/* init */
       void ql_sk_app_init(void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* task message handler */
static void Boot_AdlCallback_Message_TaskMsg(u32 msg_identifier);




/*===========================================================================
 * Function   : Boot_TaskBoot
 *
 * Description: task boot
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Boot_TaskBoot(void *argument)
{
    ql_event_t       event;
    QlOSStatus       err;

    u8               init_type;

    ql_errcode_dev_e res_dev;
    ql_errcode_power res_pw;

    bool             result;


    /* activate Application Watchdog */
    res_dev = ql_dev_cfg_wdt(1);
    if (res_dev != QL_DEV_SUCCESS)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_dev_cfg_wdt ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* debug init */
    Debug_Init();
    AtDebug_Init();


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - BOOT - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


   /*---------------------------------------------------------------------------
    * Suspend all other tasks
    *---------------------------------------------------------------------------*/
    for (int i = 0; i < (sizeof(Boot_AllTasksIDArray) / sizeof(Boot_AllTasksIDArray[0])); i++)
    {
        err = ql_rtos_task_suspend(*Boot_AllTasksIDArray[i]);
    }
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_task_suspend ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* verify init type */
    res_pw = ql_get_powerup_reason(&init_type);
    if (res_pw)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_get_powerup_reason ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        init_type = QL_PWRUP_UNKNOWN;
    }

    switch (init_type)
    {
        // Power-on by power key
        case QL_PWRUP_PWRKEY:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_PWRKEY"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Power-on by pin reset
        case QL_PWRUP_PIN_RESET:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_PIN_RESET" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Power-on by alarm
        case QL_PWRUP_ALARM:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_ALARM"     , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Power-on by charge in
        case QL_PWRUP_CHARGE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_CHARGE"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Power-on by watchdog
        case QL_PWRUP_WDG:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_WDG"       , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Power-on from PSM wakeup
        case QL_PWRUP_PSM_WAKEUP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_PSM_WAKEUP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Power-on by panic reset
        case QL_PWRUP_PANIC:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: QL_PWRUP_PANIC"     , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        // Unknown reason
        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "init_type: UNKNOWN - ERROR"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }


    /* verify the boot mode */
    BootFlash_InitFlashAll();
    BootFlash_ReadFlashAll();


  //Boot_BootMode = BOOT__PROGRAM_MODE;
  //Boot_BootMode = BOOT__DOTA_MODE;


    /* start the mode */
    switch (Boot_BootMode)
    {
        /* Program boot mode */
        case BOOT__PROGRAM_MODE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Boot mode: PROGRAM MODE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* quick init */
            Phone_QuickInit();

            /* erase flash */
            DotaFlash_EraseFlashAll();

            /* init flash and read flash */
            result = ProgramFlash_InitFlashAll();
            if (!result)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ProgramFlash_InitFlashAll ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            result = ProgramFlash_ReadFlashAll();
            if (!result)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ProgramFlash_ReadFlashAll ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* DOTA boot mode */
        case BOOT__DOTA_MODE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Boot mode: DOTA MODE"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* init flash and read flash */
            DotaFlash_InitFlashAll();
            DotaFlash_ReadFlashAll();
            break;

        /* unknown boot mode */
        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Boot mode: UNKNOWN"     , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }


    /* re-arm the Application Watchdog */
    res_dev = ql_dev_feed_wdt();
    if (res_dev != QL_DEV_SUCCESS)
    {
        snprintf(Boot_DebugString, sizeof(Boot_DebugString), "ql_dev_feed_wdt ERROR: %d", res_dev);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, Boot_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* start the mode */
    switch (Boot_BootMode)
    {
        /* Program boot mode */
        case BOOT__PROGRAM_MODE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Start \"Program\" boot mode", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* resume all Program tasks */
            for (int i = 0; i < (sizeof(Boot_ProgramTasksIDArray) / sizeof(Boot_ProgramTasksIDArray[0])); i++)
            {
                err = ql_rtos_task_resume(*Boot_ProgramTasksIDArray[i]);
            }
            if (err != QL_OSI_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_task_resume ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        /* DOTA boot mode */
        case BOOT__DOTA_MODE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Start \"DOTA\" boot mode"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* resume all DOTA tasks */
            for (int i = 0; i < (sizeof(Boot_DotaTasksIDArray) / sizeof(Boot_DotaTasksIDArray[0])); i++)
            {
                err = ql_rtos_task_resume(*Boot_DotaTasksIDArray[i]);
            }
            if (err != QL_OSI_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_task_resume ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        /* unknown boot mode */
        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Unknown boot mode: ERROR"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "End BOOT task", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Boot_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*===========================================================================
 * Function    : Boot_GprsConnectionStart
 *
 * Description : - require to BOOT task to start a GPRS connection
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Boot_GprsConnectionStart(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = BOOT_MESSAGE__GPRS_APN_START;

    err = ql_rtos_event_send(Boot_TaskRef_Boot, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Boot_GprsConnectionStop
 *
 * Description : - require to BOOT task to stop a GPRS connection
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Boot_GprsConnectionStop(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = BOOT_MESSAGE__GPRS_APN_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_Boot, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Boot_ClientHttpGetRxRequest
 *
 * Description : require to BOOT task to do a HTTP GET "RX"
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Boot_ClientHttpGetRxRequest(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = BOOT_MESSAGE__HTTP_GET_RX;

    err = ql_rtos_event_send(Boot_TaskRef_Boot, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Boot_ClientHttpGetTxRequest
 *
 * Description : require to BOOT task to do a HTTP GET "TX"
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Boot_ClientHttpGetTxRequest(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = BOOT_MESSAGE__HTTP_GET_TX;

    err = ql_rtos_event_send(Boot_TaskRef_Boot, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Boot_ClientHttpPostRequest
 *
 * Description : require to BOOT task to do a HTTP POST
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Boot_ClientHttpPostRequest(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = BOOT_MESSAGE__HTTP_POST;

    err = ql_rtos_event_send(Boot_TaskRef_Boot, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Boot_ClientSmtpRequest
 *
 * Description : require to BOOT task to do a SMTP
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Boot_ClientSmtpRequest(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = BOOT_MESSAGE__SMTP;

    err = ql_rtos_event_send(Boot_TaskRef_Boot, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Boot_BootMode_GetDefault
 *
 * Description: get the default Boot Mode
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Boot_BootMode_GetDefault(BOOT__BOOT_MODE *ptr_data)
{
    *ptr_data = Boot_BootModeDefault;
}




/*===========================================================================
 * Function   : Boot_BootMode_Get
 *
 * Description: get the Boot Mode
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Boot_BootMode_Get(BOOT__BOOT_MODE *ptr_data)
{
    *ptr_data = Boot_BootMode;
}




/*===========================================================================
 * Function   : Boot_BootMode_Set
 *
 * Description: set the Boot Mode
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Boot_BootMode_Set(BOOT__BOOT_MODE *ptr_data)
{
    Boot_BootMode = *ptr_data;
}




/*===========================================================================
 * Function   : Boot_PwrkeyCallback
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Boot_PwrkeyCallback(void)
{
    // NOTHING TO DO
}




/*===========================================================================
 * Function   : Boot_PwrkeyLongPressCallback
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Boot_PwrkeyLongPressCallback(void)
{
    // NOTHING TO DO
}




/*===========================================================================
 * Function   : Boot_PwrkeyPressCallback
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Boot_PwrkeyPressCallback(void)
{
    // NOTHING TO DO
}




/*===========================================================================
 * Function   : Boot_PwrkeyReleaseCallback
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Boot_PwrkeyReleaseCallback(void)
{
    // NOTHING TO DO
}




/*===========================================================================
 * Function   : ql_sk_app_init
 *
 * Description: init SK application
 * Input      : -
 * Output     : -
 *===========================================================================*/
void ql_sk_app_init(void)
{
    QlOSStatus err;


    /* callbacks */
    ql_pwrkey_shutdown_time_set    (3000);                                // long pressed 3s shutdown
    ql_pwrkey_callback_register    (Boot_PwrkeyCallback);                 // long press & release trigger
    ql_pwrkey_longpress_cb_register(Boot_PwrkeyLongPressCallback, 7000);  // long press & not release, long pressed 7s trigger
    ql_pwrkey_press_cb_register    (Boot_PwrkeyPressCallback);
    ql_pwrkey_release_cb_register  (Boot_PwrkeyReleaseCallback);

    /* message subscribe */
    err = ql_rtos_task_create(&Boot_TaskRef_Boot, (16 * 1024), APP_PRIORITY_BELOW_NORMAL, "Boot", Boot_TaskBoot, NULL, 10);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_task_create ERROR: Boot", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    for (int i = 0; i < (sizeof(Boot_AllTasksIDArray) / sizeof(Boot_AllTasksIDArray[0])); i++)
    {
        err = ql_rtos_task_create(Boot_AllTasksIDArray[i],
                                  adl_InitTasks[i].stackSize,
                                  adl_InitTasks[i].priority,
                                  adl_InitTasks[i].taskName,
                                  adl_InitTasks[i].taskStart,
                                  NULL,
                                  adl_InitTasks[i].event_count);

        if (err != QL_OSI_SUCCESS)
        {
            snprintf(Boot_DebugString, sizeof(Boot_DebugString), "ql_rtos_task_create ERROR: %s", adl_InitTasks[i].taskName);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, Boot_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*===========================================================================
 * Function    : Boot_AdlCallback_Message_TaskMsg
 *
 * Description : - task boot message callback
 * Input       : - msg_identifier:
 * Output      : -
 *===========================================================================*/
static void Boot_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    bool result;


    snprintf(Boot_DebugString, sizeof(Boot_DebugString), "CALLBACK     - MESSAGE     - TASK BOOT - msg identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, Boot_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier)
    {
        // GPRS APN start
        case BOOT_MESSAGE__GPRS_APN_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "BOOT_MESSAGE__GPRS_APN_START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            result = Phone_GprsStart();
            if (!result)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Phone_GprsStart ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        // GPRS APN stop
        case BOOT_MESSAGE__GPRS_APN_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "BOOT_MESSAGE__GPRS_APN_STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            result = Phone_GprsStop();
            if (!result)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "Phone_GprsStop ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        // HTTP GET "RX"
        case BOOT_MESSAGE__HTTP_GET_RX:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "BOOT_MESSAGE__HTTP_GET_RX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Http_ClientHttpGetRx();
            break;


        // HTTP GET "TX"
        case BOOT_MESSAGE__HTTP_GET_TX:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "BOOT_MESSAGE__HTTP_GET_TX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Http_ClientHttpGetTx();
            break;


        // HTTP POST
        case BOOT_MESSAGE__HTTP_POST:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "BOOT_MESSAGE__HTTP_POST", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Http_ClientHttpPost();
            break;


#ifdef FUNCTION_IMPLEMENTED
        // SMTP
        case BOOT_MESSAGE__SMTP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_BOOT, DEBUG_TRACE_TYPE_LOW, "BOOT_MESSAGE__SMTP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Smtp_ClientSmtp();
            break;
#endif
    }
}
