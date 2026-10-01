/*=============================================================================
 * File       :  ALARM_TMIN.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - minimum temperature alarm manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "alarm_tmin.h"
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
/* minimum temperature status */
#define MIN_TEMPERATURE__OK                   0                                         // status - temperature regular
#define MIN_TEMPERATURE__MIN                  1                                         // status - temperature too high (above the minimum temperature)

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
#define TASK_MSG_ID__NEW_TEMPERATURE_INT      (11300 | (QL_COMPONENT_APP_START << 16))  // event - new internal temperature available
#define TASK_MSG_ID__NEW_TEMPERATURE_EXT      (11301 | (QL_COMPONENT_APP_START << 16))  // event - new external temperature available
#define TASK_MSG_ID__TIMEOUT_TMIN_INT_START   (11302 | (QL_COMPONENT_APP_START << 16))  // event - timeout internal minimum temperature start
#define TASK_MSG_ID__TIMEOUT_TMIN_INT_STOP    (11303 | (QL_COMPONENT_APP_START << 16))  // event - timeout internal minimum temperature stop
#define TASK_MSG_ID__TIMEOUT_TMIN_EXT_START   (11304 | (QL_COMPONENT_APP_START << 16))  // event - timeout external minimum temperature start
#define TASK_MSG_ID__TIMEOUT_TMIN_EXT_STOP    (11305 | (QL_COMPONENT_APP_START << 16))  // event - timeout external minimum temperature stop
#define TASK_MSG_ID__NEW_CONFIGURATION        (11306 | (QL_COMPONENT_APP_START << 16))  // event - new configuration

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING               200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii                                         AlarmTMin_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - internal temperature status */
static       ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       AlarmTMin_Status_TemperatureMinInt;
static const ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       AlarmTMin_Status_TemperatureMinIntDefault =
{
    MIN_TEMPERATURE__OK,    /* internal temperature status       */
    FALSE,                  /* indication of status change timer */
};


/* status - external temperature status */
static       ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       AlarmTMin_Status_TemperatureMinExt;
static const ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       AlarmTMin_Status_TemperatureMinExtDefault =
{
    MIN_TEMPERATURE__OK,    /* internal temperature status       */
    FALSE,                  /* indication of status change timer */
};


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - internal minimum temperature alarm */
static       ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT AlarmTMin_Config_AlarmTemperatureMinInt;
static const ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT AlarmTMin_Config_AlarmTemperatureMinIntDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,                   /* minimum external temperature          alarm enabled status */
    TRUE,                   /* minimum external temperature restored alarm enabled status */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,                   /* minimum external temperature          alarm enabled status */
    FALSE,                   /* minimum external temperature restored alarm enabled status */
#endif
};


/* configuration - external minimum temperature alarm */
static       ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT AlarmTMin_Config_AlarmTemperatureMinExt;
static const ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT AlarmTMin_Config_AlarmTemperatureMinExtDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,                   /* minimum external temperature          alarm enabled status */
    TRUE,                   /* minimum external temperature restored alarm enabled status */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,                  /* minimum external temperature          alarm enabled status */
    FALSE,                  /* minimum external temperature restored alarm enabled status */
#endif
};


/* configuration - internal minimum temperature */
static       ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       AlarmTMin_Config_TemperatureMinInt;
static const ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       AlarmTMin_Config_TemperatureMinIntDefault =
{
    50,                     /* minimum internal temperature for alarm (/10 �C) */
};


/* configuration - external minimum temperature */
static       ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       AlarmTMin_Config_TemperatureMinExt;
static const ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       AlarmTMin_Config_TemperatureMinExtDefault =
{
    50,                     /* minimum internal temperature for alarm (/10 �C) */
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                                    AlarmTMin_TimerHandler_TimerTemperatureMinIntStart;  /* timer for internal minimum temperature start */
static       ql_timer_t                                    AlarmTMin_TimerHandler_TimerTemperatureMinIntStop;   /* timer for internal minimum temperature stop  */
static       ql_timer_t                                    AlarmTMin_TimerHandler_TimerTemperatureMinExtStart;  /* timer for external minimum temperature start */
static       ql_timer_t                                    AlarmTMin_TimerHandler_TimerTemperatureMinExtStop;   /* timer for external minimum temperature stop  */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void AlarmTMin_TaskAlarmTMin(void *argument);

/* task events */
static void AlarmTMin_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext);

/* ??? */
static void AlarmTMin_AlarmTemperatureMinInt(s16 temperature);
static void AlarmTMin_AlarmTemperatureMinExt(s16 temperature);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature status" */
       void AlarmTMin_Status_TemperatureMinInt_GetDefault     (ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       *ptr_data);
       void AlarmTMin_Status_TemperatureMinInt_Get            (ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       *ptr_data);
       void AlarmTMin_Status_TemperatureMinInt_Set            (ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       *ptr_data);

/* get/set "external temperature status" */
       void AlarmTMin_Status_TemperatureMinExt_GetDefault     (ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       *ptr_data);
       void AlarmTMin_Status_TemperatureMinExt_Get            (ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       *ptr_data);
       void AlarmTMin_Status_TemperatureMinExt_Set            (ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "internal minimum temperature alarm" configuration */
       void AlarmTMin_Config_AlarmTemperatureMinInt_GetDefault(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);
       void AlarmTMin_Config_AlarmTemperatureMinInt_Get       (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);
       void AlarmTMin_Config_AlarmTemperatureMinInt_Set       (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);
       bool AlarmTMin_Config_AlarmTemperatureMinInt_IsValid   (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);

/* get/set "external minimum temperature alarm" configuration */
       void AlarmTMin_Config_AlarmTemperatureMinExt_GetDefault(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);
       void AlarmTMin_Config_AlarmTemperatureMinExt_Get       (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);
       void AlarmTMin_Config_AlarmTemperatureMinExt_Set       (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);
       bool AlarmTMin_Config_AlarmTemperatureMinExt_IsValid   (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);

/* get/set "internal minimum temperature"       configuration */
       void AlarmTMin_Config_TemperatureMinInt_GetDefault     (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);
       void AlarmTMin_Config_TemperatureMinInt_Get            (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);
       void AlarmTMin_Config_TemperatureMinInt_Set            (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);
       bool AlarmTMin_Config_TemperatureMinInt_IsValid        (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);

/* get/set "external minimum temperature"       configuration */
       void AlarmTMin_Config_TemperatureMinExt_GetDefault     (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);
       void AlarmTMin_Config_TemperatureMinExt_Get            (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);
       void AlarmTMin_Config_TemperatureMinExt_Set            (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);
       bool AlarmTMin_Config_TemperatureMinExt_IsValid        (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void AlarmTMin_TimerTemperatureMinIntStart_Start(u32 time_value, bool periodic);
static void AlarmTMin_TimerTemperatureMinIntStart_Stop (void);

static void AlarmTMin_TimerTemperatureMinIntStop_Start (u32 time_value, bool periodic);
static void AlarmTMin_TimerTemperatureMinIntStop_Stop  (void);

static void AlarmTMin_TimerTemperatureMinExtStart_Start(u32 time_value, bool periodic);
static void AlarmTMin_TimerTemperatureMinExtStart_Stop (void);

static void AlarmTMin_TimerTemperatureMinExtStop_Start (u32 time_value, bool periodic);
static void AlarmTMin_TimerTemperatureMinExtStop_Stop  (void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void AlarmTMin_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStart(void *context);
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStop (void *context);
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStart(void *context);
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStop (void *context);




/*=============================================================================
 * Function   : AlarmTMin_TaskAlarmTMin
 *
 * Description: minimum temperature alarm task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void AlarmTMin_TaskAlarmTMin(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - ALARM_TMIN - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* registered the function for signal of new available temperatures */
    DrvTemperature_RegisterSignalNewTemperatures(AlarmTMin_NewTemperatures);


    /* creation of timer "TimerTemperatureMinIntStart" */
    err = ql_rtos_timer_create(&AlarmTMin_TimerHandler_TimerTemperatureMinIntStart, QL_TIMER_IN_SERVICE, AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStart, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureMinIntStop" */
    err = ql_rtos_timer_create(&AlarmTMin_TimerHandler_TimerTemperatureMinIntStop, QL_TIMER_IN_SERVICE, AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureMinExtStart" */
    err = ql_rtos_timer_create(&AlarmTMin_TimerHandler_TimerTemperatureMinExtStart, QL_TIMER_IN_SERVICE, AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStart, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureMinExtStop" */
    err = ql_rtos_timer_create(&AlarmTMin_TimerHandler_TimerTemperatureMinExtStop, QL_TIMER_IN_SERVICE, AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            AlarmTMin_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*===========================================================================
 * Function   : AlarmTMin_NewTemperatures
 *
 * Description: signal new temperatures available (internal and external)
 * Input      : - sensor_status_int: new internal sensor status
 *              - temperature_int  : new internal temperature (/10 �C)
 *            : - sensor_status_ext: new external sensor status
 *              - temperature_ext  : new external temperature (/10 �C)
 * Output     : -
 *===========================================================================*/
static void AlarmTMin_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext)
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

        err = ql_rtos_event_send(Boot_TaskRef_AlarmTMin, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
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

        err = ql_rtos_event_send(Boot_TaskRef_AlarmTMin, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*=============================================================================
 * Function   : AlarmTMin_AlarmTemperatureMinInt
 *
 * Description: verify if there is an "internal minimum temperature alarm" condition
 * Input      : - temperature: actual internal temperature (/10 �C)
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_AlarmTemperatureMinInt(s16 temperature)
{
    u16 hysteresis_temperature;


    /* verify temperature value */
    if (!Temperature_IsValidTemperature(temperature))
        return;


    switch (AlarmTMin_Status_TemperatureMinInt.status_tmin_int)
    {
        // status - temperature regular
        case MIN_TEMPERATURE__OK:
            /* status - temperature regular */
            if (temperature <= (AlarmTMin_Config_TemperatureMinInt.temperature_min_int                         ))   // without hysteresis
            {
                /* "new internal temperature" lower (or equal) than "minimum internal temperature for alarm" */
                if (!AlarmTMin_Status_TemperatureMinInt.status_change_timer_int)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Internal temperature: start MIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinInt.status_change_timer_int = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS);

                    AlarmTMin_TimerTemperatureMinIntStart_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                }
            }
            else
            {
                /* "new internal temperature" higher           than "minimum internal temperature for alarm" */
                if ( AlarmTMin_Status_TemperatureMinInt.status_change_timer_int)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Internal temperature: stop MIN" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinInt.status_change_timer_int = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS);

                    AlarmTMin_TimerTemperatureMinIntStart_Stop();
                }
            }
            break;


        // status - temperature too low (below the minimum temperature)
        case MIN_TEMPERATURE__MIN:
            /* status - temperature too low (below the minimum temperature) */

            if (AlarmTMin_Config_TemperatureMinInt.temperature_min_int <= ((s16)THRESHOLD_FOR_HYSTERESIS_TEMPERATURE))
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_1;
            else
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_2;

            if (temperature >= (AlarmTMin_Config_TemperatureMinInt.temperature_min_int + (s16)hysteresis_temperature))   // with    hysteresis
            {
                if (!AlarmTMin_Status_TemperatureMinInt.status_change_timer_int)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Internal temperature: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinInt.status_change_timer_int = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS);

                    AlarmTMin_TimerTemperatureMinIntStop_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                }
            }
            else
            {
                if ( AlarmTMin_Status_TemperatureMinInt.status_change_timer_int)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Internal temperature: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinInt.status_change_timer_int = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS);

                    AlarmTMin_TimerTemperatureMinIntStop_Stop();
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
 * Function   : AlarmTMin_AlarmTemperatureMinExt
 *
 * Description: verify if there is an "external minimum temperature alarm" condition
 * Input      : - temperature: actual external temperature (/10 �C)
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_AlarmTemperatureMinExt(s16 temperature)
{
    u16 hysteresis_temperature;


    switch (AlarmTMin_Status_TemperatureMinExt.status_tmin_ext)
    {
        // status - temperature regular
        case MIN_TEMPERATURE__OK:
            /* status - temperature regular */
            if (temperature <= (AlarmTMin_Config_TemperatureMinExt.temperature_min_ext                         ))   // without hysteresis
            {
                /* "new external temperature" lower (or equal) than "minimum external temperature for alarm" */
                if (!AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "External temperature: start MIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS);

                    AlarmTMin_TimerTemperatureMinExtStart_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
               }
            }
            else
            {
                /* "new external temperature" higher           than "minimum external temperature for alarm" */
                if ( AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "External temperature: stop MIN" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS);

                    AlarmTMin_TimerTemperatureMinExtStart_Stop();
                }
            }
            break;


        // status - temperature too low (below the minimum temperature)
        case MIN_TEMPERATURE__MIN:
            /* status - temperature too low (below the minimum temperature) */

            if (AlarmTMin_Config_TemperatureMinExt.temperature_min_ext <= ((s16)THRESHOLD_FOR_HYSTERESIS_TEMPERATURE))
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_1;
            else
                hysteresis_temperature = HYSTERESIS_TEMPERATURE_2;

            if (temperature >= (AlarmTMin_Config_TemperatureMinExt.temperature_min_ext + (s16)hysteresis_temperature))   // with    hysteresis
            {
                if (!AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext)
                {
                    /* status change timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "External temperature: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext = TRUE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS);

                    AlarmTMin_TimerTemperatureMinExtStop_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                }
            }
            else
            {
                if ( AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext)
                {
                    /* status change timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "External temperature: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext = FALSE;

                    /* request the backup to flash objects */
                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS);

                    AlarmTMin_TimerTemperatureMinExtStop_Stop();
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
 * Function   : AlarmTMin_Status_TemperatureMinInt_GetDefault
 *
 * Description: get the default "internal temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Status_TemperatureMinInt_GetDefault(ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT *ptr_data)
{
    *ptr_data = AlarmTMin_Status_TemperatureMinIntDefault;
}




/*===========================================================================
 * Function   : AlarmTMin_Status_TemperatureMinInt_Get
 *
 * Description: get the "internal temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Status_TemperatureMinInt_Get(ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT *ptr_data)
{
    *ptr_data = AlarmTMin_Status_TemperatureMinInt;
}




/*===========================================================================
 * Function   : AlarmTMin_Status_TemperatureMinInt_Set
 *
 * Description: set the "internal temperature" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Status_TemperatureMinInt_Set(ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT *ptr_data)
{
    AlarmTMin_Status_TemperatureMinInt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMin_Status_TemperatureMinExt_GetDefault
 *
 * Description: get the default "external temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Status_TemperatureMinExt_GetDefault(ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT *ptr_data)
{
    *ptr_data = AlarmTMin_Status_TemperatureMinExtDefault;
}




/*===========================================================================
 * Function   : AlarmTMin_Status_TemperatureMinExt_Get
 *
 * Description: get the "external temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Status_TemperatureMinExt_Get(ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT *ptr_data)
{
    *ptr_data = AlarmTMin_Status_TemperatureMinExt;
}




/*===========================================================================
 * Function   : AlarmTMin_Status_TemperatureMinExt_Set
 *
 * Description: set the "external temperature" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Status_TemperatureMinExt_Set(ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT *ptr_data)
{
    AlarmTMin_Status_TemperatureMinExt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinInt_GetDefault
 *
 * Description: get the default "internal minimum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_AlarmTemperatureMinInt_GetDefault(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_AlarmTemperatureMinIntDefault;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinInt_Get
 *
 * Description: get the "internal minimum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_AlarmTemperatureMinInt_Get(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_AlarmTemperatureMinInt;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinInt_Set
 *
 * Description: set the "internal minimum temperature alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_AlarmTemperatureMinInt_Set(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data)
{
    AlarmTMin_Config_AlarmTemperatureMinInt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinInt_IsValid
 *
 * Description: check if the "internal minimum temperature alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMin_Config_AlarmTemperatureMinInt_IsValid(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinExt_GetDefault
 *
 * Description: get the default "external minimum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_AlarmTemperatureMinExt_GetDefault(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_AlarmTemperatureMinExtDefault;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinExt_Get
 *
 * Description: get the "external minimum temperature alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_AlarmTemperatureMinExt_Get(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_AlarmTemperatureMinExt;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinExt_Set
 *
 * Description: set the "external minimum temperature alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_AlarmTemperatureMinExt_Set(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data)
{
    AlarmTMin_Config_AlarmTemperatureMinExt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_AlarmTemperatureMinExt_IsValid
 *
 * Description: check if the "external minimum temperature alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMin_Config_AlarmTemperatureMinExt_IsValid(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinInt_GetDefault
 *
 * Description: get the default "internal minimum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_TemperatureMinInt_GetDefault(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_TemperatureMinIntDefault;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinInt_Get
 *
 * Description: get the "internal minimum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_TemperatureMinInt_Get(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_TemperatureMinInt;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinInt_Set
 *
 * Description: set the "internal minimum temperature" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_TemperatureMinInt_Set(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT *ptr_data)
{
    AlarmTMin_Config_TemperatureMinInt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinInt_IsValid
 *
 * Description: check if the "internal minimum temperature" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMin_Config_TemperatureMinInt_IsValid(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT *ptr_data)
{
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
    if (
         (ptr_data->temperature_min_int < -300) ||
         (ptr_data->temperature_min_int > 1000)
       )
    {
        return FALSE;   // not in range [-30.0 - +100.0 �C]
    }
#endif
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    if (
         (ptr_data->temperature_min_int <    0) ||
         (ptr_data->temperature_min_int > 1000)
       )
    {
        return FALSE;   // not in range [ -0.0 - +100.0 �C]
    }
#endif

    return TRUE;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinExt_GetDefault
 *
 * Description: get the default "external minimum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_TemperatureMinExt_GetDefault(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_TemperatureMinExtDefault;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinExt_Get
 *
 * Description: get the "external minimum temperature" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_TemperatureMinExt_Get(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT *ptr_data)
{
    *ptr_data = AlarmTMin_Config_TemperatureMinExt;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinExt_Set
 *
 * Description: set the "external minimum temperature" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmTMin_Config_TemperatureMinExt_Set(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT *ptr_data)
{
    AlarmTMin_Config_TemperatureMinExt = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmTMin_Config_TemperatureMinExt_IsValid
 *
 * Description: check if the "external minimum temperature" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmTMin_Config_TemperatureMinExt_IsValid(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT *ptr_data)
{
    if (
         (ptr_data->temperature_min_ext < -300) ||
         (ptr_data->temperature_min_ext > 1000)
       )
    {
        return FALSE;   // not in range [-30.0 - +100.0 �C]
    }

    return TRUE;
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinIntStart_Start
 *
 * Description: start the "internal minimum temperature start" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinIntStart_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Start \"internal minimum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMin_TimerHandler_TimerTemperatureMinIntStart, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinIntStart_Stop
 *
 * Description: stop the "internal minimum temperature start" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinIntStart_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Stop \"internal minimum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMin_TimerHandler_TimerTemperatureMinIntStart);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinIntStop_Start
 *
 * Description: start the "internal minimum temperature stop" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinIntStop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Start \"internal minimum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMin_TimerHandler_TimerTemperatureMinIntStop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinIntStop_Stop
 *
 * Description: stop the "internal minimum temperature stop" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinIntStop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Stop \"internal minimum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMin_TimerHandler_TimerTemperatureMinIntStop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinExtStart_Start
 *
 * Description: start the "external minimum temperature start" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinExtStart_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Start \"external minimum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMin_TimerHandler_TimerTemperatureMinExtStart, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinExtStart_Stop
 *
 * Description: stop the "external minimum temperature start" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinExtStart_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Stop \"external minimum temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMin_TimerHandler_TimerTemperatureMinExtStart);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinExtStop_Start
 *
 * Description: start the "external minimum temperature stop" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinExtStop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Start \"external minimum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmTMin_TimerHandler_TimerTemperatureMinExtStop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_TimerTemperatureMinExtStop_Stop
 *
 * Description: stop the "external minimum temperature stop" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_TimerTemperatureMinExtStop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Stop \"external minimum temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmTMin_TimerHandler_TimerTemperatureMinExtStop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmTMin_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void AlarmTMin_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    s16  temperature_int;
    s16  temperature_ext;

    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    /* debug */
    snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "CALLBACK     - MESSAGE     - TASK ALARM_TMIN - msg identifier: %d", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        // event - new internal temperature available
        case TASK_MSG_ID__NEW_TEMPERATURE_INT:
            /* extract the parameters */
            temperature_int = (s16)msg_identifier->param1;

            snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "Current internal temperature: %d (/10 �C)", temperature_int);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


            snprintf(AlarmTMin_DebugString,
                     sizeof(AlarmTMin_DebugString),
                     "Int. temperature status before new temp.:  %d %d",
                     AlarmTMin_Status_TemperatureMinInt.status_tmin_int,
                     AlarmTMin_Status_TemperatureMinInt.status_change_timer_int);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* check the internal temperature */
            AlarmTMin_AlarmTemperatureMinInt(temperature_int);
            break;


        // event - new external temperature available
        case TASK_MSG_ID__NEW_TEMPERATURE_EXT:
            /* extract the parameters */
            temperature_ext = (s16)msg_identifier->param1;

            snprintf(AlarmTMin_DebugString, sizeof(AlarmTMin_DebugString), "Current external temperature: %d (/10 �C)", temperature_ext);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


            snprintf(AlarmTMin_DebugString,
                     sizeof(AlarmTMin_DebugString),
                     "Ext. temperature status before new temp.:  %d %d",
                     AlarmTMin_Status_TemperatureMinExt.status_tmin_ext,
                     AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, AlarmTMin_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* check the external temperature */
            AlarmTMin_AlarmTemperatureMinExt(temperature_ext);
            break;



        // event - timeout internal minimum temperature start
        case TASK_MSG_ID__TIMEOUT_TMIN_INT_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Internal temperature: MIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the internal temperature status */
            AlarmTMin_Status_TemperatureMinInt.status_tmin_int         = MIN_TEMPERATURE__MIN;
            AlarmTMin_Status_TemperatureMinInt.status_change_timer_int = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS);

            if (AlarmTMin_Config_AlarmTemperatureMinInt.alarm_enabled_status_tmin_int)
            {
                /* "minimum internal temperature" alarm enabled */

                /* generate "minimum internal temperature" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "\"INTERNAL TEMPERATURE TOO LOW\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_INT_MIN, 0);
            }
            break;


        // event - timeout internal minimum temperature stop
        case TASK_MSG_ID__TIMEOUT_TMIN_INT_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "Internal temperature: OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the internal temperature status */
            AlarmTMin_Status_TemperatureMinInt.status_tmin_int         = MIN_TEMPERATURE__OK;
            AlarmTMin_Status_TemperatureMinInt.status_change_timer_int = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS);

            if (AlarmTMin_Config_AlarmTemperatureMinInt.alarm_enabled_status_tmin_int_restored)
            {
                /* "minimum internal temperature restored" alarm enabled */

                /* generate "minimum internal temperature restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "\"INTERNAL TEMPERATURE TOO LOW RESTORED\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_INT_MIN_RESTORED, 0);
            }
            break;



        // event - timeout external minimum temperature start
        case TASK_MSG_ID__TIMEOUT_TMIN_EXT_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "External temperature: MIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the external temperature status */
            AlarmTMin_Status_TemperatureMinExt.status_tmin_ext         = MIN_TEMPERATURE__MIN;
            AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS);

            if (AlarmTMin_Config_AlarmTemperatureMinExt.alarm_enabled_status_tmin_ext)
            {
                /* "minimum external temperature" alarm enabled */

                /* generate "minimum external temperature" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "\"EXTERNAL TEMPERATURE TOO LOW\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_EXT_MIN, 0);
            }
            break;



        // event - timeout external minimum temperature stop
        case TASK_MSG_ID__TIMEOUT_TMIN_EXT_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "External temperature: OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the external temperature status */
            AlarmTMin_Status_TemperatureMinExt.status_tmin_ext         = MIN_TEMPERATURE__OK;
            AlarmTMin_Status_TemperatureMinExt.status_change_timer_ext = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS);

            if (AlarmTMin_Config_AlarmTemperatureMinExt.alarm_enabled_status_tmin_ext_restored)
            {
                /* "minimum external temperature restored" alarm enabled */

                /* generate "minimum external temperature restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "\"EXTERNAL TEMPERATURE TOO LOW RESTORED\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__T_EXT_MIN_RESTORED, 0);
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
 * Function   : AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStart
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStart(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MIN INT START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMIN_INT_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMin, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinIntStop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MIN INT STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMIN_INT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMin, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStart
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStart(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MIN EXT START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMIN_EXT_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMin, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmTMin_AdlCallback_Timer_TimerTemperatureMinExtStop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE MIN EXT STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TMIN_EXT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmTMin, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
