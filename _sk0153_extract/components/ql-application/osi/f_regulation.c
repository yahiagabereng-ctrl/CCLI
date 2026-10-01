/*=============================================================================
 * File       :  F_REGULATION.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "regulation" function
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 * When the "regulation" function is enabled, then it disables for:
 *   - "regulation" function timeout expiration        (internal or external)
 *   - manual press of an output key                   (internal or external)
 *   - reception of "TURNOFF" SMS command              (internal or external)
 *   - reception of "REGULATE OFF" SMS command         (internal or external)
 *   - calling from a phone number stored in phonebook (internal            )
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <limits.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "f_regulation.h"
#include "fw_config.h"
#include "outputs.h"
#include "program_flash.h"
#include "startup.h"
#include "temperature.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* regulated temperature (/10 �C) */
#define REG_TEMPERATURE_MIN                                             50           /* minimum regulated temperature (/10 �C) */
#define REG_TEMPERATURE_MAX                                             320          /* maximum regulated temperature (/10 �C) */

/* temperature regulation duration (min) */
#define REG_DURATION_MAX                                                (48 * 60)    /* maximum temperature regulation duration (min) */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                                         200


/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__FUNCTION_STATUS__OFF                                 0            /* SEM state - function status - disabled */
#define SEM_STATE__FUNCTION_STATUS__ON                                  1            /* SEM state - function status - enabled  */

/* SEM state strings */
#define SEM_STATE_STRING__FUNCTION_STATUS__OFF                          "SEM_STATE__FUNCTION_STATUS__OFF"
#define SEM_STATE_STRING__FUNCTION_STATUS__ON                           "SEM_STATE__FUNCTION_STATUS__ON"


/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__REGULATION_TEMP_INT_START                            0            /* SEM event - internal temperature regulation start                    */
#define SEM_EVENT__REGULATION_TEMP_INT_STOP                             1            /* SEM event - internal temperature regulation stop                     */
#define SEM_EVENT__REGULATION_TEMP_EXT_START                            2            /* SEM event - external temperature regulation start                    */
#define SEM_EVENT__REGULATION_TEMP_EXT_STOP                             3            /* SEM event - external temperature regulation stop                     */
#define SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_DURATION                 4            /* SEM event - timeout internal temperature regulation - duration       */
#define SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_DURATION                 5            /* SEM event - timeout external temperature regulation - duration       */
#define SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME           6            /* SEM event - timeout internal temperature regulation - remaining time */
#define SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME           7            /* SEM event - timeout external temperature regulation - remaining time */

/* SEM events strings */
#define SEM_EVENT_STRING__REGULATION_TEMP_INT_START                     "SEM_EVENT__REGULATION_TEMP_INT_START"
#define SEM_EVENT_STRING__REGULATION_TEMP_INT_STOP                      "SEM_EVENT__REGULATION_TEMP_INT_STOP"
#define SEM_EVENT_STRING__REGULATION_TEMP_EXT_START                     "SEM_EVENT__REGULATION_TEMP_EXT_START"
#define SEM_EVENT_STRING__REGULATION_TEMP_EXT_STOP                      "SEM_EVENT__REGULATION_TEMP_EXT_STOP"
#define SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_INT_DURATION          "SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_DURATION"
#define SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_EXT_DURATION          "SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_DURATION"
#define SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME    "SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME"
#define SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME    "SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME"


/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__REGULATION_TEMP_INT_START                          (11600 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation start                    */
#define TASK_MSG_ID__REGULATION_TEMP_INT_STOP                           (11601 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation stop                     */
#define TASK_MSG_ID__REGULATION_TEMP_EXT_START                          (11602 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation start                    */
#define TASK_MSG_ID__REGULATION_TEMP_EXT_STOP                           (11603 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation stop                     */
#define TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_INT_DURATION               (11604 | (QL_COMPONENT_APP_START << 16))    /* event - timeout internal temperature regulation - duration       */
#define TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_EXT_DURATION               (11605 | (QL_COMPONENT_APP_START << 16))    /* event - timeout external temperature regulation - duration       */
#define TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME         (11606 | (QL_COMPONENT_APP_START << 16))    /* event - timeout internal temperature regulation - remaining time */
#define TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME         (11607 | (QL_COMPONENT_APP_START << 16))    /* event - timeout external temperature regulation - remaining time */




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* status - internal "regulation function" */
static       F_REGULATION__STATUS_REGULATION_INT FRegulation_Status_FRegulationInt;
static const F_REGULATION__STATUS_REGULATION_INT FRegulation_Status_FRegulationIntDefault =
{
    FALSE,    /* function status             */
    0,        /* temperature to be regulated */
    0,        /* duration of regulation      */
    0,        /* remaining time              */
};


/* status - external "regulation function" */
static       F_REGULATION__STATUS_REGULATION_EXT FRegulation_Status_FRegulationExt;
static const F_REGULATION__STATUS_REGULATION_EXT FRegulation_Status_FRegulationExtDefault =
{
    FALSE,    /* function status             */
    0,        /* temperature to be regulated */
    0,        /* duration of regulation      */
    0,        /* remaining time              */
};


/* "regulation" function status (internal and external) */
static       u8                                  FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__OFF;
static       u8                                  FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__OFF;

/* debug string */
static       ascii                               FRegulation_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                          FRegulation_TimerHandler_TimerRegulationTempInt_Duration;       /* timer for internal temperature regulation - duration       */
static       ql_timer_t                          FRegulation_TimerHandler_TimerRegulationTempExt_Duration;       /* timer for external temperature regulation - duration       */
static       ql_timer_t                          FRegulation_TimerHandler_TimerRegulationTempInt_RemainingTime;  /* timer for internal temperature regulation - remaining time */
static       ql_timer_t                          FRegulation_TimerHandler_TimerRegulationTempExt_RemainingTime;  /* timer for external temperature regulation - remaining time */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void FRegulation_TaskFRegulation(void *argument);

/* task events */
       void FRegulation_NewTeleControlInt(F_REGULATION__STATUS_REGULATION_INT *ptr_function_status_new, F_REGULATION__STATUS_REGULATION_INT *ptr_function_status_old);
       void FRegulation_NewTeleControlExt(F_REGULATION__STATUS_REGULATION_EXT *ptr_function_status_new, F_REGULATION__STATUS_REGULATION_EXT *ptr_function_status_old);

static bool FRegulation_RegulationTemperatureInt_Start(s16 temperature, u16 duration);
static bool FRegulation_RegulationTemperatureInt_Stop (void);

static bool FRegulation_RegulationTemperatureExt_Start(s16 temperature, u16 duration);
static bool FRegulation_RegulationTemperatureExt_Stop (void);

/* remaining time */
       u32  FRegulation_RemainingTimeInt(void);
       u32  FRegulation_RemainingTimeExt(void);

/* get/set internal "regulation function" status */
       void FRegulation_Status_FRegulationInt_GetDefault(F_REGULATION__STATUS_REGULATION_INT *ptr_data);
       void FRegulation_Status_FRegulationInt_Get       (F_REGULATION__STATUS_REGULATION_INT *ptr_data);
       void FRegulation_Status_FRegulationInt_Set       (F_REGULATION__STATUS_REGULATION_INT *ptr_data);
       bool FRegulation_Status_FRegulationInt_IsValid   (F_REGULATION__STATUS_REGULATION_INT *ptr_data);

/* get/set external "regulation function" status */
       void FRegulation_Status_FRegulationExt_GetDefault(F_REGULATION__STATUS_REGULATION_EXT *ptr_data);
       void FRegulation_Status_FRegulationExt_Get       (F_REGULATION__STATUS_REGULATION_EXT *ptr_data);
       void FRegulation_Status_FRegulationExt_Set       (F_REGULATION__STATUS_REGULATION_EXT *ptr_data);
       bool FRegulation_Status_FRegulationExt_IsValid   (F_REGULATION__STATUS_REGULATION_EXT *ptr_data);

/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void FRegulation_TimerRegulationTempInt_Duration_Start     (u32 time_value, bool periodic);
static void FRegulation_TimerRegulationTempInt_Duration_Stop      (void);

static void FRegulation_TimerRegulationTempExt_Duration_Start     (u32 time_value, bool periodic);
static void FRegulation_TimerRegulationTempExt_Duration_Stop      (void);

static void FRegulation_TimerRegulationTempInt_RemainingTime_Start(u32 time_value, bool periodic);
static void FRegulation_TimerRegulationTempInt_RemainingTime_Stop (void);

static void FRegulation_TimerRegulationTempExt_RemainingTime_Start(u32 time_value, bool periodic);
static void FRegulation_TimerRegulationTempExt_RemainingTime_Stop (void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void FRegulation_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void FRegulation_AdlCallback_Timer_TimerRegulationTempInt_Duration     (void *context);
static void FRegulation_AdlCallback_Timer_TimerRegulationTempExt_Duration     (void *context);
static void FRegulation_AdlCallback_Timer_TimerRegulationTempInt_RemainingTime(void *context);
static void FRegulation_AdlCallback_Timer_TimerRegulationTempExt_RemainingTime(void *context);




/*=============================================================================
 * Function   : FRegulation_TaskFRegulation
 *
 * Description: "regulation" function task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FRegulation_TaskFRegulation(void *argument)
{
    ql_event_t event;
    QlOSStatus err;

    u32        remaining_time_ms_int;
    u32        remaining_time_ms_ext;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - F_REGULATION - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* creation of timer "TimerRegulationTempInt_Duration" */
    err = ql_rtos_timer_create(&FRegulation_TimerHandler_TimerRegulationTempInt_Duration, QL_TIMER_IN_SERVICE, FRegulation_AdlCallback_Timer_TimerRegulationTempInt_Duration, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerRegulationTempExt_Duration" */
    err = ql_rtos_timer_create(&FRegulation_TimerHandler_TimerRegulationTempExt_Duration, QL_TIMER_IN_SERVICE, FRegulation_AdlCallback_Timer_TimerRegulationTempExt_Duration, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerRegulationTempInt_RemainingTime" */
    err = ql_rtos_timer_create(&FRegulation_TimerHandler_TimerRegulationTempInt_RemainingTime, QL_TIMER_IN_SERVICE, FRegulation_AdlCallback_Timer_TimerRegulationTempInt_RemainingTime, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerRegulationTempExt_RemainingTime" */
    err = ql_rtos_timer_create(&FRegulation_TimerHandler_TimerRegulationTempExt_RemainingTime, QL_TIMER_IN_SERVICE, FRegulation_AdlCallback_Timer_TimerRegulationTempExt_RemainingTime, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* internal regulation function */
    if (FRegulation_Status_FRegulationInt.status)
    {
        /* internal "regulation" function enabled */

        FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__ON;

        /* signal internal regulation function on */
        Outputs_Event_FRegulationInt_On(FRegulation_Status_FRegulationInt.temperature);

        /* start the "regulation duration" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
        remaining_time_ms_int = FRegulation_Status_FRegulationInt.remaining_time * 60 * 1000L;
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
        remaining_time_ms_int = FRegulation_Status_FRegulationInt.remaining_time *  1 * 1000L;
#endif
        FRegulation_TimerRegulationTempInt_Duration_Start(remaining_time_ms_int, FALSE);

        /* start the "regulation remaining time" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
        FRegulation_TimerRegulationTempInt_RemainingTime_Start(60 * 1000L, TRUE);
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
        FRegulation_TimerRegulationTempInt_RemainingTime_Start( 1 * 1000L, TRUE);
#endif
    }
    else
    {
        /* internal "regulation" function disabled */
        FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__OFF;
    }


    /* external regulation function */
    if (FRegulation_Status_FRegulationExt.status)
    {
        /* external "regulation" function enabled */

        FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__ON;

        /* signal external regulation function on */
        Outputs_Event_FRegulationExt_On(FRegulation_Status_FRegulationExt.temperature);

        /* start the "regulation duration" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
        remaining_time_ms_ext = FRegulation_Status_FRegulationExt.remaining_time * 60 * 1000L;
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
        remaining_time_ms_ext = FRegulation_Status_FRegulationExt.remaining_time *  1 * 1000L;
#endif
        FRegulation_TimerRegulationTempExt_Duration_Start(remaining_time_ms_ext, FALSE);

        /* start the "regulation remaining time" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
        FRegulation_TimerRegulationTempExt_RemainingTime_Start(60 * 1000L, TRUE);
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
        FRegulation_TimerRegulationTempExt_RemainingTime_Start( 1 * 1000L, TRUE);
#endif
    }
    else
    {
        /* external "regulation" function disabled */
        FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__OFF;
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            FRegulation_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : FRegulation_NewTeleControlInt
 *
 * Description: signal a new telecontrol of internal regulation function
 * Input      : - ptr_config_new: pointer to new function status
 *              - ptr_config_old: pointer to old function status
 * Output     : -
 *=============================================================================*/
void FRegulation_NewTeleControlInt(F_REGULATION__STATUS_REGULATION_INT *ptr_function_status_new, F_REGULATION__STATUS_REGULATION_INT *ptr_function_status_old)
{
    if      ((!ptr_function_status_old->status) && ( ptr_function_status_new->status))
    {
        /* internal regulation function activation */
        FRegulation_RegulationTemperatureInt_Start(ptr_function_status_new->temperature, ptr_function_status_new->duration);
    }
    else if (( ptr_function_status_old->status) && (!ptr_function_status_new->status))
    {
        /* internal regulation function disactivation */
        FRegulation_RegulationTemperatureInt_Stop();
    }
    else if (( ptr_function_status_old->status) && ( ptr_function_status_new->status))
    {
        if (
            (ptr_function_status_old->temperature    != ptr_function_status_new->temperature   ) ||
            (ptr_function_status_old->duration       != ptr_function_status_new->duration      ) ||
            (ptr_function_status_old->remaining_time != ptr_function_status_new->remaining_time)
           )
        {
            /* internal regulation function parameters change */
            FRegulation_RegulationTemperatureInt_Start(ptr_function_status_new->temperature, ptr_function_status_new->duration);
        }
    }
}




/*=============================================================================
 * Function   : FRegulation_NewTeleControlExt
 *
 * Description: signal a new telecontrol of external regulation function
 * Input      : - ptr_config_new: pointer to new function status
 *              - ptr_config_old: pointer to old function status
 * Output     : -
 *=============================================================================*/
void FRegulation_NewTeleControlExt(F_REGULATION__STATUS_REGULATION_EXT *ptr_function_status_new, F_REGULATION__STATUS_REGULATION_EXT *ptr_function_status_old)
{
    if      ((!ptr_function_status_old->status) && ( ptr_function_status_new->status))
    {
        /* external regulation function activation */
        FRegulation_RegulationTemperatureExt_Start(ptr_function_status_new->temperature, ptr_function_status_new->duration);
    }
    else if (( ptr_function_status_old->status) && (!ptr_function_status_new->status))
    {
        /* external regulation function disactivation */
        FRegulation_RegulationTemperatureExt_Stop();
    }
    else if (( ptr_function_status_old->status) && ( ptr_function_status_new->status))
    {
        if (
            (ptr_function_status_old->temperature    != ptr_function_status_new->temperature   ) ||
            (ptr_function_status_old->duration       != ptr_function_status_new->duration      ) ||
            (ptr_function_status_old->remaining_time != ptr_function_status_new->remaining_time)
           )
        {
            /* external regulation function parameters change */
            FRegulation_RegulationTemperatureExt_Start(ptr_function_status_new->temperature, ptr_function_status_new->duration);
        }
    }
}




/*=============================================================================
 * Function   : FRegulation_RegulationTemperatureInt_Start
 *
 * Description: start the internal temperature regulation
 * Input      : - temperature: temperature to be regulated (/10 �C)
 *              - duration   : duration of regulation      (hour)
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
static bool FRegulation_RegulationTemperatureInt_Start(s16 temperature, u16 duration)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;

    if (
         (temperature < REG_TEMPERATURE_MIN) ||
         (temperature > REG_TEMPERATURE_MAX)
       )
    {
        return FALSE;
    }

    if (
         (duration == 0               ) ||
         (duration >  REG_DURATION_MAX)
       )
    {
        return FALSE;
    }


    event.id     = TASK_MSG_ID__REGULATION_TEMP_INT_START;
    event.param1 = temperature;
    event.param2 = duration;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : FRegulation_RegulationTemperatureInt_Stop
 *
 * Description: stop the internal temperature regulation
 * Input      : -
 * Output     : - FALSE: regulation not stopped
 *              - TRUE : regulation     stopped
 *=============================================================================*/
static bool FRegulation_RegulationTemperatureInt_Stop(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_TEMP_INT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : FRegulation_RegulationTemperatureExt_Start
 *
 * Description: start the external temperature regulation
 * Input      : - temperature: temperature to be regulated (/10 �C)
 *              - duration   : duration of regulation      (hour)
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
static bool FRegulation_RegulationTemperatureExt_Start(s16 temperature, u16 duration)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;

    if (
         (temperature < REG_TEMPERATURE_MIN) ||
         (temperature > REG_TEMPERATURE_MAX)
       )
    {
        return FALSE;
    }

    if (
         (duration == 0               ) ||
         (duration >  REG_DURATION_MAX)
       )
    {
        return FALSE;
    }


    event.id     = TASK_MSG_ID__REGULATION_TEMP_EXT_START;
    event.param1 = temperature;
    event.param2 = duration;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : FRegulation_RegulationTemperatureExt_Stop
 *
 * Description: stop the external temperature regulation
 * Input      : -
 * Output     : - FALSE: regulation not stopped
 *              - TRUE : regulation     stopped
 *=============================================================================*/
static bool FRegulation_RegulationTemperatureExt_Stop(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_TEMP_EXT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FRegulation_RemainingTimeInt
 *
 * Description: return the remaining time of the internal regulation function
 * Input      : -
 * Output     : - 0                    (if the function is disabled)
 *              - remaining time (sec) (if the function is enabled )
 *===========================================================================*/
u32 FRegulation_RemainingTimeInt(void)
{
    if (FRegulation_Status_FRegulationInt.status)
    {
        /* function enabled */
        return ((u32)FRegulation_Status_FRegulationInt.remaining_time * 60L);
    }
    else
    {
        /* function disabled */
        return 0;
    }
}




/*===========================================================================
 * Function   : FRegulation_RemainingTimeExt
 *
 * Description: return the remaining time of the external regulation function
 * Input      : -
 * Output     : - 0                    (if the function is disabled)
 *              - remaining time (sec) (if the function is enabled )
 *===========================================================================*/
u32 FRegulation_RemainingTimeExt(void)
{
    if (FRegulation_Status_FRegulationExt.status)
    {
        /* function enabled */
        return ((u32)FRegulation_Status_FRegulationExt.remaining_time * 60L);
    }
    else
    {
        /* function disabled */
        return 0;
    }
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationInt_GetDefault
 *
 * Description: get the default internal "regulation function" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FRegulation_Status_FRegulationInt_GetDefault(F_REGULATION__STATUS_REGULATION_INT *ptr_data)
{
    *ptr_data = FRegulation_Status_FRegulationIntDefault;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationInt_Get
 *
 * Description: get the internal "regulation function" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FRegulation_Status_FRegulationInt_Get(F_REGULATION__STATUS_REGULATION_INT *ptr_data)
{
    *ptr_data = FRegulation_Status_FRegulationInt;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationInt_Set
 *
 * Description: set the internal "regulation function" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FRegulation_Status_FRegulationInt_Set(F_REGULATION__STATUS_REGULATION_INT *ptr_data)
{
    FRegulation_Status_FRegulationInt = *ptr_data;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationInt_IsValid
 *
 * Description: check if the internal "reguation function" status is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: status is not valid
 *              - TRUE : status is     valid
 *===========================================================================*/
bool FRegulation_Status_FRegulationInt_IsValid(F_REGULATION__STATUS_REGULATION_INT *ptr_data)
{
    if (ptr_data->status)
    {
        if (
             (ptr_data->temperature    < REG_TEMPERATURE_MIN) ||
             (ptr_data->temperature    > REG_TEMPERATURE_MAX)
           )
        {
            return FALSE;
        }

        if (
             (ptr_data->duration       == 0                 ) ||
             (ptr_data->duration       >  REG_DURATION_MAX  )
           )
        {
            return FALSE;
        }

        if (
           //(ptr_data->remaining_time == 0                 ) ||
             (ptr_data->remaining_time >  REG_DURATION_MAX  )
           )
        {
            return FALSE;
        }
    }

    return TRUE;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationExt_GetDefault
 *
 * Description: get the default internal "antifrost function" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FRegulation_Status_FRegulationExt_GetDefault(F_REGULATION__STATUS_REGULATION_EXT *ptr_data)
{
    *ptr_data = FRegulation_Status_FRegulationExtDefault;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationExt_Get
 *
 * Description: get the internal "regulation function" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FRegulation_Status_FRegulationExt_Get(F_REGULATION__STATUS_REGULATION_EXT *ptr_data)
{
    *ptr_data = FRegulation_Status_FRegulationExt;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationExt_Set
 *
 * Description: set the internal "regulation function" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FRegulation_Status_FRegulationExt_Set(F_REGULATION__STATUS_REGULATION_EXT *ptr_data)
{
    FRegulation_Status_FRegulationExt = *ptr_data;
}




/*===========================================================================
 * Function   : FRegulation_Status_FRegulationExt_IsValid
 *
 * Description: check if the external "reguation function" status is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: status is not valid
 *              - TRUE : status is     valid
 *===========================================================================*/
bool FRegulation_Status_FRegulationExt_IsValid(F_REGULATION__STATUS_REGULATION_EXT *ptr_data)
{
    if (ptr_data->status)
    {
        if (
             (ptr_data->temperature    < REG_TEMPERATURE_MIN) ||
             (ptr_data->temperature    > REG_TEMPERATURE_MAX)
           )
        {
            return FALSE;
        }

        if (
             (ptr_data->duration       == 0                 ) ||
             (ptr_data->duration       >  REG_DURATION_MAX  )
           )
        {
            return FALSE;
        }

        if (
           //(ptr_data->remaining_time == 0                 ) ||
             (ptr_data->remaining_time >  REG_DURATION_MAX  )
           )
        {
            return FALSE;
        }
    }


    return TRUE;
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempInt_Duration_Start
 *
 * Description: start the "internal temperature regulation - duration" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempInt_Duration_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"internal temperature regulation - duration\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FRegulation_TimerHandler_TimerRegulationTempInt_Duration, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempInt_Duration_Stop
 *
 * Description: stop the "internal temperature regulation - duration" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempInt_Duration_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"internal temperature regulation - duration\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FRegulation_TimerHandler_TimerRegulationTempInt_Duration);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempExt_Duration_Start
 *
 * Description: start the "external temperature regulation - duration" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempExt_Duration_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"external temperature regulation - duration\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FRegulation_TimerHandler_TimerRegulationTempExt_Duration, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempExt_Duration_Stop
 *
 * Description: stop the "external temperature regulation - duration" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempExt_Duration_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"external temperature regulation - duration\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FRegulation_TimerHandler_TimerRegulationTempExt_Duration);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempInt_RemainingTime_Start
 *
 * Description: start the "internal temperature regulation - remaining time" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempInt_RemainingTime_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"internal temperature regulation - remaining time\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FRegulation_TimerHandler_TimerRegulationTempInt_RemainingTime, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempInt_RemainingTime_Stop
 *
 * Description: stop the "internal temperature regulation - remaining time" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempInt_RemainingTime_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"internal temperature regulation - remaining time\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FRegulation_TimerHandler_TimerRegulationTempInt_RemainingTime);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempExt_RemainingTime_Start
 *
 * Description: start the "external temperature regulation - remaining time" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempExt_RemainingTime_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"external temperature regulation - remaining time\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FRegulation_TimerHandler_TimerRegulationTempExt_RemainingTime, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_TimerRegulationTempExt_RemainingTime_Stop
 *
 * Description: stop the "external temperature regulation - remaining time" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FRegulation_TimerRegulationTempExt_RemainingTime_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"external temperature regulation - remaining time\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FRegulation_TimerHandler_TimerRegulationTempExt_RemainingTime);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : FRegulation_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void FRegulation_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    static       u16    counter_backup_int = 0;
    static       u16    counter_backup_ext = 0;

                 u32    sem_event;

    /* "regulation" function parameters */
                 s16    temperature_int;
                 s16    temperature_ext;
                 u16    duration_int;
                 u16    duration_ext;
                 u32    duration_ms_int;
                 u32    duration_ms_ext;

    static const ascii *sem_state_strings[] =
    {
        SEM_STATE_STRING__FUNCTION_STATUS__OFF,
        SEM_STATE_STRING__FUNCTION_STATUS__ON,
    };

    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__REGULATION_TEMP_INT_START,
        SEM_EVENT_STRING__REGULATION_TEMP_INT_STOP,
        SEM_EVENT_STRING__REGULATION_TEMP_EXT_START,
        SEM_EVENT_STRING__REGULATION_TEMP_EXT_STOP,
        SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_INT_DURATION,
        SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_EXT_DURATION,
        SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME,
        SEM_EVENT_STRING__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME,
    };


    /* debug */
    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "CALLBACK     - MESSAGE     - TASK F_REGULATION - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        case TASK_MSG_ID__REGULATION_TEMP_INT_START:
            sem_event = SEM_EVENT__REGULATION_TEMP_INT_START;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_INT_STOP:
            sem_event = SEM_EVENT__REGULATION_TEMP_INT_STOP;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_EXT_START:
            sem_event = SEM_EVENT__REGULATION_TEMP_EXT_START;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_EXT_STOP:
            sem_event = SEM_EVENT__REGULATION_TEMP_EXT_STOP;
            break;

        case TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_INT_DURATION:
            sem_event = SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_DURATION;
            break;

        case TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_EXT_DURATION:
            sem_event = SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_DURATION;
            break;

        case TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME:
            sem_event = SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME;
            break;

        case TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME:
            sem_event = SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME;
            break;

        default:
            return;
    }



    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "SEM state int: %s", sem_state_strings[FRegulation_FunctionStatusInt]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "SEM state ext: %s", sem_state_strings[FRegulation_FunctionStatusExt]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "SEM event    : %s", sem_event_strings[sem_event                    ]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);



    /*---------------------------------------------------------------------------
     * Internal temperature regulation function
     *---------------------------------------------------------------------------*/
    switch (FRegulation_FunctionStatusInt)
    {
        /* function status - disabled */
        case SEM_STATE__FUNCTION_STATUS__OFF:
            switch (sem_event)
            {
                // event - internal temperature regulation start
                case SEM_EVENT__REGULATION_TEMP_INT_START:
                    /* extract the parameters */
                    temperature_int = (s16)msg_identifier->param1;
                    duration_int    = (u16)msg_identifier->param2;

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation internal temperature: %d (/10 �C)", temperature_int);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation internal duration   : %d (min)"   , duration_int   );
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* signal internal regulation function on */
                    Outputs_Event_FRegulationInt_On(temperature_int);

                    /* start the "regulation duration" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    duration_ms_int = (u32)duration_int * 60 * 1000L;
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    duration_ms_int = (u32)duration_int *  1 * 1000L;
#endif
                    FRegulation_TimerRegulationTempInt_Duration_Start(duration_ms_int, FALSE);

                    /* start the "regulation remaining time" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    FRegulation_TimerRegulationTempInt_RemainingTime_Start(60 * 1000L, TRUE);
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    FRegulation_TimerRegulationTempInt_RemainingTime_Start( 1 * 1000L, TRUE);
#endif

                    FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__ON;
                    break;


                // event - internal temperature regulation stop
                // event - timeout internal temperature regulation - duration
                // event - timeout internal temperature regulation - remaining time
                case SEM_EVENT__REGULATION_TEMP_INT_STOP:
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_DURATION:
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME:
                    // NOTHING TO DO
                    break;


                // unknown event
                default:
                    break;
            }
            break;


        /* function status - enabled */
        case SEM_STATE__FUNCTION_STATUS__ON:
            switch (sem_event)
            {
                // event - internal temperature regulation start
                case SEM_EVENT__REGULATION_TEMP_INT_START:
                    /* extract the parameters */
                    temperature_int = (s16)msg_identifier->param1;
                    duration_int    = (u16)msg_identifier->param2;

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation internal temperature: %d (/10 �C)", temperature_int);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation internal duration   : %d (min)"   , duration_int   );
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* signal internal regulation function on */
                    Outputs_Event_FRegulationInt_Off();
                    Outputs_Event_FRegulationInt_On (temperature_int);

                    /* start the "regulation duration" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    duration_ms_int = (u32)duration_int * 60 * 1000L;
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    duration_ms_int = (u32)duration_int *  1 * 1000L;
#endif
                    FRegulation_TimerRegulationTempInt_Duration_Stop ();
                    FRegulation_TimerRegulationTempInt_Duration_Start(duration_ms_int, FALSE);

                    /* start the "regulation remaining time" function timer */
                    FRegulation_TimerRegulationTempInt_RemainingTime_Stop ();
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    FRegulation_TimerRegulationTempInt_RemainingTime_Start(60 * 1000L, TRUE);
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    FRegulation_TimerRegulationTempInt_RemainingTime_Start( 1 * 1000L, TRUE);
#endif

                    FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__ON;
                    break;


                // event - internal temperature regulation stop
                case SEM_EVENT__REGULATION_TEMP_INT_STOP:
                    /* stop the "regulation duration" function timer */
                    FRegulation_TimerRegulationTempInt_Duration_Stop();

                    /* stop the "regulation remaining time" function timer */
                    FRegulation_TimerRegulationTempInt_RemainingTime_Stop();

                    /* signal internal regulation function off */
                    Outputs_Event_FRegulationInt_Off();

                    FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__OFF;
                    break;


                // event - timeout internal temperature regulation - duration
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_DURATION:
                    /* stop the "regulation remaining time" function timer */
                    FRegulation_TimerRegulationTempInt_RemainingTime_Stop();

                    /* signal internal regulation function off */
                    Outputs_Event_FRegulationInt_Off();

                    FRegulation_Status_FRegulationInt.status         = FALSE;
                    FRegulation_Status_FRegulationInt.temperature    = 0;
                    FRegulation_Status_FRegulationInt.duration       = 0;
                    FRegulation_Status_FRegulationInt.remaining_time = 0;

                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS);

                    FRegulation_FunctionStatusInt = SEM_STATE__FUNCTION_STATUS__OFF;
                    break;


                // event - timeout internal temperature regulation - remaining time
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME:
                    if (FRegulation_Status_FRegulationInt.remaining_time > 0)
                    {
                        FRegulation_Status_FRegulationInt.remaining_time--;

                        counter_backup_int++;
                        if (!(counter_backup_int % 10))   // save remaining time in flash every 10 minutes
                        {
                            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS);
                        }
                        else
                        {
                            if (FRegulation_Status_FRegulationInt.remaining_time == 0)
                                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS);
                        }
                    }
                    break;


                // unknown event
                default:
                    break;
            }
            break;
    }



    /*---------------------------------------------------------------------------
     * External temperature regulation function
     *---------------------------------------------------------------------------*/
    switch (FRegulation_FunctionStatusExt)
    {
        /* function status - disabled */
        case SEM_STATE__FUNCTION_STATUS__OFF:
            switch (sem_event)
            {
                // event - external temperature regulation start
                case SEM_EVENT__REGULATION_TEMP_EXT_START:
                    /* extract the parameters */
                    temperature_ext = (s16)msg_identifier->param1;
                    duration_ext    = (u16)msg_identifier->param2;

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation external temperature: %d (/10 �C)", temperature_ext);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation external duration   : %d (min)"   , duration_ext   );
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* signal external regulation function on */
                    Outputs_Event_FRegulationExt_On(temperature_ext);

                    /* start the "regulation duration" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    duration_ms_ext = (u32)duration_ext * 60 * 1000L;
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    duration_ms_ext = (u32)duration_ext *  1 * 1000L;
#endif
                    FRegulation_TimerRegulationTempExt_Duration_Start(duration_ms_ext, FALSE);

                    /* start the "regulation remaining time" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    FRegulation_TimerRegulationTempExt_RemainingTime_Start(60 * 1000L, TRUE);
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    FRegulation_TimerRegulationTempExt_RemainingTime_Start( 1 * 1000L, TRUE);
#endif

                    FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__ON;
                    break;


                // event - external temperature regulation stop
                // event - timeout external temperature regulation - duration
                // event - timeout external temperature regulation - remaining time
                case SEM_EVENT__REGULATION_TEMP_EXT_STOP:
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_DURATION:
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME:
                    // NOTHING TO DO
                    break;


                // unknown event
                default:
                    break;
            }
            break;



        /* function status - enabled */
        case SEM_STATE__FUNCTION_STATUS__ON:
            switch (sem_event)
            {
                // event - external temperature regulation start
                case SEM_EVENT__REGULATION_TEMP_EXT_START:
                    /* extract the parameters */
                    temperature_ext = (s16)msg_identifier->param1;
                    duration_ext    = (u16)msg_identifier->param2;

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation external temperature: %d (/10 �C)", temperature_ext);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    snprintf(FRegulation_DebugString, sizeof(FRegulation_DebugString), "Regulation external duration   : %d (min)"   , duration_ext   );
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, FRegulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* signal external regulation function on */
                    Outputs_Event_FRegulationExt_Off();
                    Outputs_Event_FRegulationExt_On (temperature_ext);

                    /* start the "regulation duration" function timer */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    duration_ms_ext = (u32)duration_ext * 60 * 1000L;
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    duration_ms_ext = (u32)duration_ext *  1 * 1000L;
#endif
                    FRegulation_TimerRegulationTempExt_Duration_Stop ();
                    FRegulation_TimerRegulationTempExt_Duration_Start(duration_ms_ext, FALSE);

                    /* start the "regulation remaining time" function timer */
                    FRegulation_TimerRegulationTempExt_RemainingTime_Stop ();
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
                    FRegulation_TimerRegulationTempExt_RemainingTime_Start(60 * 1000L, TRUE);
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
                    FRegulation_TimerRegulationTempExt_RemainingTime_Start( 1 * 1000L, TRUE);
#endif

                    FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__ON;
                    break;


                // event - external temperature regulation stop
                case SEM_EVENT__REGULATION_TEMP_EXT_STOP:
                    /* stop the "regulation duration" function timer */
                    FRegulation_TimerRegulationTempExt_Duration_Stop();

                    /* stop the "regulation remaining time" function timer */
                    FRegulation_TimerRegulationTempExt_RemainingTime_Stop();

                    /* signal external regulation function off */
                    Outputs_Event_FRegulationExt_Off();

                    FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__OFF;
                    break;


                // event - timeout external temperature regulation - duration
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_DURATION:
                    /* stop the "regulation remaining time" function timer */
                    FRegulation_TimerRegulationTempExt_RemainingTime_Stop();

                    /* signal external regulation function off */
                    Outputs_Event_FRegulationExt_Off();

                    FRegulation_Status_FRegulationExt.status         = FALSE;
                    FRegulation_Status_FRegulationExt.temperature    = 0;
                    FRegulation_Status_FRegulationExt.duration       = 0;
                    FRegulation_Status_FRegulationExt.remaining_time = 0;

                    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS);

                    FRegulation_FunctionStatusExt = SEM_STATE__FUNCTION_STATUS__OFF;
                    break;


                // event - timeout external temperature regulation - remaining time
                case SEM_EVENT__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME:
                    if (FRegulation_Status_FRegulationExt.remaining_time > 0)
                    {
                        FRegulation_Status_FRegulationExt.remaining_time--;

                        counter_backup_ext++;
                        if (!(counter_backup_ext % 10))   // save remaining time in flash every 10 minutes
                        {
                            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS);
                        }
                        else
                        {
                            if (FRegulation_Status_FRegulationExt.remaining_time == 0)
                                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS);
                        }
                    }
                    break;


                // unknown event
                default:
                    break;
            }
            break;
    }
}




/*=============================================================================
 * Function   : FRegulation_AdlCallback_Timer_TimerRegulationTempInt_Duration
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FRegulation_AdlCallback_Timer_TimerRegulationTempInt_Duration(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - REGULATION TEMPERATURE INT - DURATION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_INT_DURATION;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_AdlCallback_Timer_TimerRegulationTempExt_Duration
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FRegulation_AdlCallback_Timer_TimerRegulationTempExt_Duration(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - REGULATION TEMPERATURE EXT - DURATION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_EXT_DURATION;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_AdlCallback_Timer_TimerRegulationTempInt_RemainingTime
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FRegulation_AdlCallback_Timer_TimerRegulationTempInt_RemainingTime(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - REGULATION TEMPERATURE INT - REMAINING TIME", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_INT_REMAINING_TIME;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FRegulation_AdlCallback_Timer_TimerRegulationTempExt_RemainingTime
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FRegulation_AdlCallback_Timer_TimerRegulationTempExt_RemainingTime(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - REGULATION TEMPERATURE EXT - REMAINING TIME", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_REGULATION_TEMP_EXT_REMAINING_TIME;

    err = ql_rtos_event_send(Boot_TaskRef_FRegulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
