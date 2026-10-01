/*=============================================================================
 * File       :  ALARM_TMAX.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - maximum temperature alarm manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "alarm_tmax.h"
#include "alarm.h"
#include "boot.h"
#include "debug_my.h"
#include "drvtemperature.h"
#include "fw_config.h"
#include "program_flash.h"
#include "temperature.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* maximum temperature status */
#define MAX_TEMPERATURE__OK                   0                                         // status - temperature regular
#define MAX_TEMPERATURE__MAX                  1                                         // status - temperature too high (above the maximum temperature)

/* threshold for hysteresis of temperature */
#define THRESHOLD_FOR_HYSTERESIS_TEMPERATURE  700                                       // (/10 �C)

/* hysteresis of temperature */
#define HYSTERESIS_TEMPERATURE_1              20                                        // (/10 �C)
#define HYSTERESIS_TEMPERATURE_2              40                                        // (/10 �C)

/* duration before changing temperature status (sec) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TIME_SEC_TEMP_STATUS_CHANGE           (60 + 1)
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TIME_SEC_TEMP_STATUS_CHANGE           (10 + 1)
#endif

/* task message IDs */
#define TASK_MSG_ID__NEW_TEMPERATURE_INT      (11400 | (QL_COMPONENT_APP_START << 16))  // event - new internal temperature available
#define TASK_MSG_ID__NEW_TEMPERATURE_EXT      (11401 | (QL_COMPONENT_APP_START << 16))  // event - new external temperature available
#define TASK_MSG_ID__TIMEOUT_TMAX_INT_START   (11402 | (QL_COMPONENT_APP_START << 16))  // event - timeout internal maximum temperature start
#define TASK_MSG_ID__TIMEOUT_TMAX_INT_STOP    (11403 | (QL_COMPONENT_APP_START << 16))  // event - timeout internal maximum temperature stop
#define TASK_MSG_ID__TIMEOUT_TMAX_EXT_START   (11404 | (QL_COMPONENT_APP_START << 16))  // event - timeout external maximum temperature start
#define TASK_MSG_ID__TIMEOUT_TMAX_EXT_STOP    (11405 | (QL_COMPONENT_APP_START << 16))  // event - timeout external maximum temperature stop
#define TASK_MSG_ID__NEW_CONFIGURATION        (11406 | (QL_COMPONENT_APP_START << 16))  // event - new configuration

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING               200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii                                         AlarmTMax_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - internal temperature status */
static       ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       AlarmTMax_Status_TemperatureMaxInt;
static const ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       AlarmTMax_Status_TemperatureMaxIntDefault =
{
    MAX_TEMPERATURE__OK,    /* internal temperature status       */
    FALSE,                  /* indication of status change timer */
};


/* status - external temperature status */
static       ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       AlarmTMax_Status_TemperatureMaxExt;
static const ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       AlarmTMax_Status_TemperatureMaxExtDefault =
{
    MAX_TEMPERATURE__OK,    /* external temperature status       */
    FALSE,                  /* indication of status change timer */
};


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - internal maximum temperature alarm */
static       ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT AlarmTMax_Config_AlarmTemperatureMaxInt;
static const ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT AlarmTMax_Config_AlarmTemperatureMaxIntDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,                   /* maximum internal temperature          alarm enabled status */
    TRUE,                   /* maximum internal temperature restored alarm enabled status */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,                  /* maximum internal temperature          alarm enabled status */
    FALSE,                  /* maximum internal temperature restored alarm enabled status */
#endif
};


/* configuration - external maximum temperature alarm */
static       ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT AlarmTMax_Config_AlarmTemperatureMaxExt;
static const ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT AlarmTMax_Config_AlarmTemperatureMaxExtDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,                   /* maximum internal temperature          alarm enabled status */
    TRUE,                   /* maximum internal temperature restored alarm enabled status */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,                  /* maximum internal temperature          alarm enabled status */
    FALSE,                  /* maximum internal temperature restored alarm enabled status */
#endif
};


/* configuration - internal maximum temperature */
static       ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       AlarmTMax_Config_TemperatureMaxInt;
static const ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       AlarmTMax_Config_TemperatureMaxIntDefault =
{
    300,                    /* maximum internal temperature for alarm (/10 �C) */
};


/* configuration - external maximum temperature */
static       ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       AlarmTMax_Config_TemperatureMaxExt;
static const ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       AlarmTMax_Config_TemperatureMaxExtDefault =
{
    300,                    /* maximum internal temperature for alarm (/10 �C) */
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                                    AlarmTMax_TimerHandler_TimerTemperatureMaxIntStart;  /* timer for internal maximum temperature start */
static       ql_timer_t                                    AlarmTMax_TimerHandler_TimerTemperatureMaxIntStop;   /* timer for internal maximum temperature stop  */
static       ql_timer_t                                    AlarmTMax_TimerHandler_TimerTemperatureMaxExtStart;  /* timer for external maximum temperature start */
static       ql_timer_t                                    AlarmTMax_TimerHandler_TimerTemperatureMaxExtStop;   /* timer for external maximum temperature stop  */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void AlarmTMax_TaskAlarmTMax(void *argument);

/* task events */
static void AlarmTMax_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext);

/* ??? */
static void AlarmTMax_AlarmTemperatureMaxInt(s16 temperature);
static void AlarmTMax_AlarmTemperatureMaxExt(s16 temperature);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature status" */
       void AlarmTMax_Status_TemperatureMaxInt_GetDefault     (ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       *ptr_data);
       void AlarmTMax_Status_TemperatureMaxInt_Get            (ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       *ptr_data);
       void AlarmTMax_Status_TemperatureMaxInt_Set            (ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       *ptr_data);

/* get/set "external temperature status" */
       void AlarmTMax_Status_TemperatureMaxExt_GetDefault     (ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       *ptr_data);
       void AlarmTMax_Status_TemperatureMaxExt_Get            (ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       *ptr_data);
       void AlarmTMax_Status_TemperatureMaxExt_Set            (ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "internal maximum temperature alarm" configuration */
       void AlarmTMax_Config_AlarmTemperatureMaxInt_GetDefault(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);
       void AlarmTMax_Config_AlarmTemperatureMaxInt_Get       (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);
       void AlarmTMax_Config_AlarmTemperatureMaxInt_Set       (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);
       bool AlarmTMax_Config_AlarmTemperatureMaxInt_IsValid   (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);

/* get/set "external maximum temperature alarm" configuration */
       void AlarmTMax_Config_AlarmTemperatureMaxExt_GetDefault(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);
       void AlarmTMax_Config_AlarmTemperatureMaxExt_Get       (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);
       void AlarmTMax_Config_AlarmTemperatureMaxExt_Set       (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);
       bool AlarmTMax_Config_AlarmTemperatureMaxExt_IsValid   (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);

/* get/set "internal maximum temperature"       configuration */
       void AlarmTMax_Config_TemperatureMaxInt_GetDefault     (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);
       void AlarmTMax_Config_TemperatureMaxInt_Get            (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);
       void AlarmTMax_Config_TemperatureMaxInt_Set            (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);
       bool AlarmTMax_Config_TemperatureMaxInt_IsValid        (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);

/* get/set "external maximum temperature"       configuration */
       void AlarmTMax_Config_TemperatureMaxExt_GetDefault     (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);
       void AlarmTMax_Config_TemperatureMaxExt_Get            (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);
       void AlarmTMax_Config_TemperatureMaxExt_Set            (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);
       bool AlarmTMax_Config_TemperatureMaxExt_IsValid        (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void AlarmTMax_TimerTemperatureMaxIntStart_Start(u32 time_value, bool periodic);
static void AlarmTMax_TimerTemperatureMaxIntStart_Stop (void);

static void AlarmTMax_TimerTemperatureMaxIntStop_Start (u32 time_value, bool periodic);
static void AlarmTMax_TimerTemperatureMaxIntStop_Stop  (void);

static void AlarmTMax_TimerTemperatureMaxExtStart_Start(u32 time_value, bool periodic);
static void AlarmTMax_TimerTemperatureMaxExtStart_Stop (void);

static void AlarmTMax_TimerTemperatureMaxExtStop_Start (u32 time_value, bool periodic);
static void AlarmTMax_TimerTemperatureMaxExtStop_Stop  (void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void AlarmTMax_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStart(void *context);
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStop (void *context);
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStart(void *context);
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStop (void *context);




/*=============================================================================
 * Function   : AlarmTMax_TaskAlarmTMax
 *
 * Description: maximum temperature alarm task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void AlarmTMax_TaskAlarmTMax(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - ALARM_TMAX - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* registered the function for signal of new available temperatures */
    DrvTemperature_RegisterSignalNewTemperatures(AlarmTMax_NewTemperatures);


    /* creation of timer "TimerTemperatureMaxIntStart" */
    err = ql_rtos_timer_create(&AlarmTMax_TimerHandler_TimerTemperatureMaxIntStart, QL_TIMER_IN_SERVICE, AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStart, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureMaxIntStop" */
    err = ql_rtos_timer_create(&AlarmTMax_TimerHandler_TimerTemperatureMaxIntStop, QL_TIMER_IN_SERVICE, AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureMaxExtStart" */
    err = ql_rtos_timer_create(&AlarmTMax_TimerHandler_TimerTemperatureMaxExtStart, QL_TIMER_IN_SERVICE, AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStart, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureMaxExtStop" */
    err = ql_rtos_timer_create(&AlarmTMax_TimerHandler_TimerTemperatureMaxExtStop, QL_TIMER_IN_SERVICE, AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            AlarmTMax_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*===========================================================================
 * Function   : AlarmTMax_NewTemperatures
 *
 * Description: signal new temperatures available (internal and external)
 * Input      : - sensor_status_int: new internal sensor status
 *              - temperature_int  : new internal temperature (/10 �C)
 *            : - sensor_status_ext: new external sensor status
 *              - temperature_ext  : new external temperature (/10 �C)
 * Output     : -
 *===========================================================================*/
static void AlarmTMax_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext)
{
    ql_event_t event;
    QlOSStatus err;


    /* internal temperature */
    if (
         (sensor_status_int == DRVTEMPERATURE__SENSOR__CONNECTED) &&
         (Temperature_IsValidTemperature(temperature_int)       )
       )
    {
        event.id     = TASK_MSG_ID__NEW_TEMPERATURE_INT;
        event.param1 = temperature_int;

        err = ql_rtos_event_send(Boot_TaskRef_AlarmTMax, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }


    /* external temperature */
    if (
         (sensor_status_ext == DRVTEMPERATURE__SENSOR__CONNECTED) &&
         (Temperature_IsValidTemperature(temperature_ext)       )
       )
    {
        event.id     = TASK_MSG_ID__NEW_TEMPERATURE_EXT;
        event.param1 = temperature_ext;

        err = ql_rtos_event_send(Boot_TaskRef_AlarmTMax, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*=============================================================================
 * Function   : AlarmTMax_AlarmTemperatureMaxInt
 *
 * Description: verify if there is an "internal maximum temperature alarm" condition
 * Input      : - temperature: actual internal temperature (/10 �C)
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_AlarmTemperatureMaxInt(s16 temperature)
{
    u16 hysteresis_temperature;


    /* verify temperature value */
    if (!Temperature_IsValidTemperature(temperature))
        return;


    switch (AlarmTMax_Status_TemperatureMaxInt.status_tmax_int)
    {
        // status - temperature regular
        case MAX_TEMPERATURE__OK:
            /* status - temperature regular */
            if (temperature >= (AlarmTMax_Config_TemperatureMaxInt.temperature_max_int                         ))   // without hysteresis
            {
                /* "new internal temperature" higher (or equal) than "maximum internal temperature for alarm" */
                if (!AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Internal temperature: start MAX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                    AlarmTMax_TimerTemperatureMaxIntStart_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                }
            }
            else
            {
                /* "new internal temperature" lower             than "maximum internal temperature for alarm" */
                if ( AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Internal temperature: stop MAX" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                    AlarmTMax_TimerTemperatureMaxIntStart_Stop();
                }
            }
            break;


        // status - temperature too high (above the maximum temperature)
        case MAX_TEMPERATURE__MAX:
            /* status - temperature too high (above the maximum temperature) */

            if (AlarmTMax_Config_TemperatureMaxInt.temperature_max_int <= ((s16)THRESHOLD_FOR_HYSTERESIS_TEMPERATURE))
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_1;
            else
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_2;

            if (temperature <= (AlarmTMax_Config_TemperatureMaxInt.temperature_max_int - (s16)hysteresis_temperature))   // with    hysteresis
            {
                if (!AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Internal temperature: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                    AlarmTMax_TimerTemperatureMaxIntStop_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                }
            }
            else
            {
                if ( AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Internal temperature: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                    AlarmTMax_TimerTemperatureMaxIntStop_Stop();
                }
            }
            break;


        // status - temperature status unknown
        default:
            //@@@ TO DO
            break;
    }
}




/*=============================================================================
 * Function   : AlarmTMax_AlarmTemperatureMaxExt
 *
 * Description: verify if there is an "external maximum temperature alarm" condition
 * Input      : - temperature: actual external temperature (/10 �C)
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_AlarmTemperatureMaxExt(s16 temperature)
{
    u16 hysteresis_temperature;


    switch (AlarmTMax_Status_TemperatureMaxExt.status_tmax_ext)
    {
        // status - temperature regular
        case MAX_TEMPERATURE__OK:
            /* status - temperature regular */
            if (temperature >= (AlarmTMax_Config_TemperatureMaxExt.temperature_max_ext                         ))   // without hysteresis
            {
                /* "new external temperature" higher (or equal) than "maximum external temperature for alarm" */
                if (!AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "External temperature: start MAX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                    AlarmTMax_TimerTemperatureMaxExtStart_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
               }
            }
            else
            {
                /* "new external temperature" lower             than "maximum external temperature for alarm" */
                if ( AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "External temperature: stop MAX" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                    AlarmTMax_TimerTemperatureMaxExtStart_Stop();
                }
            }
            break;


        // status - temperature too high (above the maximum temperature)
        case MAX_TEMPERATURE__MAX:
            /* status - temperature too high (above the maximum temperature) */

            if (AlarmTMax_Config_TemperatureMaxExt.temperature_max_ext <= ((s16)THRESHOLD_FOR_HYSTERESIS_TEMPERATURE))
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_1;
            else
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_2;

            if (temperature <= (AlarmTMax_Config_TemperatureMaxExt.temperature_max_ext - (s16)hysteresis_temperature))   // with    hysteresis
            {
                if (!AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "External temperature: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                    AlarmTMax_TimerTemperatureMaxExtStop_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                }
            }
            else
            {
                if ( AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "External temperature: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                    AlarmTMax_TimerTemperatureMaxExtStop_Stop();
                }
            }
            break;


        // status - temperature status unknown
        default:
            //@@@ TO DO
            break;
    }
}




/*===========================================================================
 * Function   : AlarmTMax_Status_TemperatureMaxInt_GetDefault
 *
 * Description: get the default "internal temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Status_TemperatureMaxInt_GetDefault(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT *ptr_data)
{
    *ptr_data = AlarmTMax_Status_TemperatureMaxIntDefault;
}




/*===========================================================================
 * Function   : AlarmTMax_Status_TemperatureMaxInt_Get
 *
 * Description: get the "internal temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Status_TemperatureMaxInt_Get(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT *ptr_data)
{
    *ptr_data = AlarmTMax_Status_TemperatureMaxInt;
}




/*===========================================================================
 * Function   : AlarmTMax_Status_TemperatureMaxInt_Set
 *
 * Description: set the "internal temperature" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Status_TemperatureMaxInt_Set(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT *ptr_data)
{
    AlarmTMax_Status_TemperatureMaxInt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMax_Status_TemperatureMaxExt_GetDefault
 *
 * Description: get the default "external temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Status_TemperatureMaxExt_GetDefault(ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT *ptr_data)
{
    *ptr_data = AlarmTMax_Status_TemperatureMaxExtDefault;
}




/*===========================================================================
 * Function   : AlarmTMax_Status_TemperatureMaxExt_Get
 *
 * Description: get the "external temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Status_TemperatureMaxExt_Get(ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT *ptr_data)
{
    *ptr_data = AlarmTMax_Status_TemperatureMaxExt;
}




/*===========================================================================
 * Function   : AlarmTMax_Status_TemperatureMaxExt_Set
 *
 * Description: set the "external temperature" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Status_TemperatureMaxExt_Set(ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT *ptr_data)
{
    AlarmTMax_Status_TemperatureMaxExt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxInt_GetDefault
 *
 * Description: get the default "internal maximum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_AlarmTemperatureMaxInt_GetDefault(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_AlarmTemperatureMaxIntDefault;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxInt_Get
 *
 * Description: get the "internal maximum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_AlarmTemperatureMaxInt_Get(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_AlarmTemperatureMaxInt;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxInt_Set
 *
 * Description: set the "internal maximum temperature alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_AlarmTemperatureMaxInt_Set(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data)
{
    AlarmTMax_Config_AlarmTemperatureMaxInt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxInt_IsValid
 *
 * Description: check if the "internal maximum temperature alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *                - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMax_Config_AlarmTemperatureMaxInt_IsValid(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxExt_GetDefault
 *
 * Description: get the default "external maximum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_AlarmTemperatureMaxExt_GetDefault(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_AlarmTemperatureMaxExtDefault;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxExt_Get
 *
 * Description: get the "external maximum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_AlarmTemperatureMaxExt_Get(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_AlarmTemperatureMaxExt;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxExt_Set
 *
 * Description: set the "external maximum temperature alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_AlarmTemperatureMaxExt_Set(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data)
{
    AlarmTMax_Config_AlarmTemperatureMaxExt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_AlarmTemperatureMaxExt_IsValid
 *
 * Description: check if the "external maximum temperature alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *                - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMax_Config_AlarmTemperatureMaxExt_IsValid(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxInt_GetDefault
 *
 * Description: get the default "internal minimum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_TemperatureMaxInt_GetDefault(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_TemperatureMaxIntDefault;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxInt_Get
 *
 * Description: get the "internal minimum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_TemperatureMaxInt_Get(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_TemperatureMaxInt;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxInt_Set
 *
 * Description: set the "internal minimum temperature" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_TemperatureMaxInt_Set(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT *ptr_data)
{
    AlarmTMax_Config_TemperatureMaxInt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxInt_IsValid
 *
 * Description: check if the "internal minimum temperature" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *                - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMax_Config_TemperatureMaxInt_IsValid(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT *ptr_data)
{
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
    if (
         (ptr_data->temperature_max_int < -300) ||
         (ptr_data->temperature_max_int > 1000)
       )
    {
        return FALSE;   // not in range [-30.0 - +100.0 �C]
    }
#endif
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    if (
         (ptr_data->temperature_max_int <    0) ||
         (ptr_data->temperature_max_int > 1000)
       )
    {
        return FALSE;   // not in range [  0.0 - +100.0 �C]
    }
#endif

    return TRUE;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxExt_GetDefault
 *
 * Description: get the default "external maximum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_TemperatureMaxExt_GetDefault(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_TemperatureMaxExtDefault;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxExt_Get
 *
 * Description: get the "external maximum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_TemperatureMaxExt_Get(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT *ptr_data)
{
    *ptr_data = AlarmTMax_Config_TemperatureMaxExt;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxExt_Set
 *
 * Description: set the "external maximum temperature" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMax_Config_TemperatureMaxExt_Set(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT *ptr_data)
{
    AlarmTMax_Config_TemperatureMaxExt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMax_Config_TemperatureMaxExt_IsValid
 *
 * Description: check if the "external maximum temperature" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMax_Config_TemperatureMaxExt_IsValid(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT *ptr_data)
{
    if (
         (ptr_data->temperature_max_ext < -300) ||
         (ptr_data->temperature_max_ext > 1000)
       )
    {
        return FALSE;   // not in range [-30.0 - +100.0 �C]
    }

    return TRUE;
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxIntStart_Start
 *
 * Description: start the "internal maximum temperature start" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxIntStart_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Start \"internal maximum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMax_TimerHandler_TimerTemperatureMaxIntStart, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxIntStart_Stop
 *
 * Description: stop the "internal maximum temperature start" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxIntStart_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Stop \"internal maximum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMax_TimerHandler_TimerTemperatureMaxIntStart);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxIntStop_Start
 *
 * Description: start the "internal maximum temperature stop" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxIntStop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Start \"internal maximum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMax_TimerHandler_TimerTemperatureMaxIntStop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxIntStop_Stop
 *
 * Description: stop the "internal maximum temperature stop" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxIntStop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Stop \"internal maximum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMax_TimerHandler_TimerTemperatureMaxIntStop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxExtStart_Start
 *
 * Description: start the "external maximum temperature start" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxExtStart_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Start \"external maximum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMax_TimerHandler_TimerTemperatureMaxExtStart, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxExtStart_Stop
 *
 * Description: stop the "external maximum temperature start" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxExtStart_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Stop \"external maximum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMax_TimerHandler_TimerTemperatureMaxExtStart);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxExtStop_Start
 *
 * Description: start the "external maximum temperature stop" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxExtStop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Start \"external maximum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMax_TimerHandler_TimerTemperatureMaxExtStop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_TimerTemperatureMaxExtStop_Stop
 *
 * Description: stop the "external maximum temperature stop" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_TimerTemperatureMaxExtStop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Stop \"external maximum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMax_TimerHandler_TimerTemperatureMaxExtStop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmTMax_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void AlarmTMax_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    s16  temperature_int;
    s16  temperature_ext;

    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    /* debug */
    snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "CALLBACK     - MESSAGE     - TASK ALARM_TMAX - msg identifier: %d", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        // event - new internal temperature available
        case TASK_MSG_ID__NEW_TEMPERATURE_INT:
            /* extract the parameters */
            temperature_int = (s16)msg_identifier->param1;

            snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "Current internal temperature: %d (/10 �C)", temperature_int);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


            snprintf(AlarmTMax_DebugString,
                     sizeof(AlarmTMax_DebugString),
                     "Int. temperature status before new temp.:  %d %d",
                     AlarmTMax_Status_TemperatureMaxInt.status_tmax_int,
                     AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* check the internal temperature */
            AlarmTMax_AlarmTemperatureMaxInt(temperature_int);
            break;


        // event - new external temperature available
        case TASK_MSG_ID__NEW_TEMPERATURE_EXT:
            /* extract the parameters */
            temperature_ext = (s16)msg_identifier->param1;

            snprintf(AlarmTMax_DebugString, sizeof(AlarmTMax_DebugString), "Current external temperature: %d (/10 �C)", temperature_ext);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


            snprintf(AlarmTMax_DebugString,
                     sizeof(AlarmTMax_DebugString),
                     "Ext. temperature status before new temp.:  %d %d",
                     AlarmTMax_Status_TemperatureMaxExt.status_tmax_ext,
                     AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, AlarmTMax_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* check the external temperature */
            AlarmTMax_AlarmTemperatureMaxExt(temperature_ext);
            break;



        // event - timeout internal maximum temperature start
        case TASK_MSG_ID__TIMEOUT_TMAX_INT_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Internal temperature: MAX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the internal temperature status */
            AlarmTMax_Status_TemperatureMaxInt.status_tmax_int         = MAX_TEMPERATURE__MAX;
            AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

            if (AlarmTMax_Config_AlarmTemperatureMaxInt.alarm_enabled_status_tmax_int)
            {
                /* "maximum internal temperature" alarm enabled */

                /* generate "maximum internal temperature" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "\"INTERNAL TEMPERATURE TOO HIGH\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_INT_MAX, 0);
            }
            break;


        // event - timeout internal maximum temperature stop
        case TASK_MSG_ID__TIMEOUT_TMAX_INT_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "Internal temperature: OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the internal temperature status */
            AlarmTMax_Status_TemperatureMaxInt.status_tmax_int         = MAX_TEMPERATURE__OK;
            AlarmTMax_Status_TemperatureMaxInt.status_change_timer_int = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

            if (AlarmTMax_Config_AlarmTemperatureMaxInt.alarm_enabled_status_tmax_int_restored)
            {
                /* "maximum internal temperature restored" alarm enabled */

                /* generate "maximum internal temperature restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "\"INTERNAL TEMPERATURE TOO HIGH RESTORED\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_INT_MAX_RESTORED, 0);
            }
            break;



        // event - timeout external maximum temperature start
        case TASK_MSG_ID__TIMEOUT_TMAX_EXT_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "External temperature: MAX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the external temperature status */
            AlarmTMax_Status_TemperatureMaxExt.status_tmax_ext         = MAX_TEMPERATURE__MAX;
            AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

            if (AlarmTMax_Config_AlarmTemperatureMaxExt.alarm_enabled_status_tmax_ext)
            {
                /* "maximum external temperature" alarm enabled */

                /* generate "maximum external temperature" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "\"EXTERNAL TEMPERATURE TOO HIGH\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_EXT_MAX, 0);
            }
            break;



        // event - timeout external maximum temperature stop
        case TASK_MSG_ID__TIMEOUT_TMAX_EXT_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "External temperature: OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the external temperature status */
            AlarmTMax_Status_TemperatureMaxExt.status_tmax_ext         = MAX_TEMPERATURE__OK;
            AlarmTMax_Status_TemperatureMaxExt.status_change_timer_ext = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

            if (AlarmTMax_Config_AlarmTemperatureMaxExt.alarm_enabled_status_tmax_ext_restored)
            {
                /* "maximum external temperature restored" alarm enabled */

                /* generate "maximum external temperature restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "\"EXTERNAL TEMPERATURE TOO HIGH RESTORED\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_EXT_MAX_RESTORED, 0);
            }
            break;



        // event - new configuration
        case TASK_MSG_ID__NEW_CONFIGURATION:
            //@@@ TO DO
            if (0)
            {
                //...
            }
            break;



        // unknown event
        default:
            break;
    }
}




/*=============================================================================
 * Function   : AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStart
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStart(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MAX INT START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMAX_INT_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMax, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxIntStop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MAX INT STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMAX_INT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMax, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStart
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStart(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MAX EXT START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMAX_EXT_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMax, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMax_AdlCallback_Timer_TimerTemperatureMaxExtStop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MAX EXT STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMAX_EXT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMax, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMAX, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
