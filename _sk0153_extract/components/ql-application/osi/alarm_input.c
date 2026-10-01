/*=============================================================================
 * File       :  ALARM_INPUT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - digital inputs alarm manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "alarm_input.h"
#include "alarm.h"
#include "boot.h"
#include "counters.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "drvtemperature.h"
#include "fw_config.h"
#include "program_flash.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* digital input status */
#define INPUT__OK                             0                                         // status - input not alarmed
#define INPUT__ALARMED                        1                                         // status - input     alarmed

/* time (ms) */
#define TIME_MS__INPUTS_READING               (4 * 1000L)                               // timeout for inputs reading

/* task message IDs */
#define TASK_MSG_ID__INPUT_EVENT              (11200 | (QL_COMPONENT_APP_START << 16))  // event - input event
#define TASK_MSG_ID__NEW_TEMPERATURE          (11201 | (QL_COMPONENT_APP_START << 16))  // event - new temperature available
#define TASK_MSG_ID__TIMEOUT_INPUTS_READING   (11202 | (QL_COMPONENT_APP_START << 16))  // event - timeout inputs reading
#define TASK_MSG_ID__TIMEOUT_INPUT_IN1_START  (11203 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN1 start
#define TASK_MSG_ID__TIMEOUT_INPUT_IN1_STOP   (11204 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN1 stop
#define TASK_MSG_ID__TIMEOUT_INPUT_IN2_START  (11205 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN2 start
#define TASK_MSG_ID__TIMEOUT_INPUT_IN2_STOP   (11206 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN2 stop
#define TASK_MSG_ID__TIMEOUT_INPUT_IN3_START  (11207 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN3 start
#define TASK_MSG_ID__TIMEOUT_INPUT_IN3_STOP   (11208 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN3 stop
#define TASK_MSG_ID__TIMEOUT_INPUT_IN4_START  (11209 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN4 start
#define TASK_MSG_ID__TIMEOUT_INPUT_IN4_STOP   (11210 | (QL_COMPONENT_APP_START << 16))  // event - timeout input IN4 stop
#define TASK_MSG_ID__CHANGED_SIGNAL_TYPE_IN3  (11211 | (QL_COMPONENT_APP_START << 16))  // event - signal type IN3 configuration changed
#define TASK_MSG_ID__CHANGED_SIGNAL_TYPE_IN4  (11212 | (QL_COMPONENT_APP_START << 16))  // event - signal type IN4 configuration changed
#define TASK_MSG_ID__NEW_CONFIGURATION        (11213 | (QL_COMPONENT_APP_START << 16))  // event - new configuration

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING               200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* indication of input enabled (IN1, IN2, IN3 and IN4) */
static       bool                                 AlarmInput_InputEnabled_In1 = TRUE;
static       bool                                 AlarmInput_InputEnabled_In2 = TRUE;
static       bool                                 AlarmInput_InputEnabled_In3 = TRUE;
static       bool                                 AlarmInput_InputEnabled_In4 = TRUE;

/* debug string */
static       ascii                                AlarmInput_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - input IN1 status */
static       ALARM_INPUT__STATUS__INPUT_IN1       AlarmInput_Status_InputIn1;
static const ALARM_INPUT__STATUS__INPUT_IN1       AlarmInput_Status_InputIn1Default =
{
    INPUT__OK,                                    /* input status                      */
    FALSE,                                        /* indication of status change timer */
};


/* status - input IN2 status */
static       ALARM_INPUT__STATUS__INPUT_IN2       AlarmInput_Status_InputIn2;
static const ALARM_INPUT__STATUS__INPUT_IN2       AlarmInput_Status_InputIn2Default =
{
    INPUT__OK,                                    /* input status                      */
    FALSE,                                        /* indication of status change timer */
};


/* status - input IN2 status */
static       ALARM_INPUT__STATUS__INPUT_IN3       AlarmInput_Status_InputIn3;
static const ALARM_INPUT__STATUS__INPUT_IN3       AlarmInput_Status_InputIn3Default =
{
    INPUT__OK,                                    /* input status                      */
    FALSE,                                        /* indication of status change timer */
};


/* status - input IN2 status */
static       ALARM_INPUT__STATUS__INPUT_IN4       AlarmInput_Status_InputIn4;
static const ALARM_INPUT__STATUS__INPUT_IN4       AlarmInput_Status_InputIn4Default =
{
    INPUT__OK,                                    /* input status                      */
    FALSE,                                        /* indication of status change timer */
};


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - IN1 digital input alarm */
static       ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 AlarmInput_Config_AlarmInputIn1;
static const ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 AlarmInput_Config_AlarmInputIn1Default =
{
    FALSE,                                        /* digital input          alarm enabled status */
    FALSE,                                        /* digital input restored alarm enabled status */
};


/* configuration - IN2 digital input alarm */
static       ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 AlarmInput_Config_AlarmInputIn2;
static const ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 AlarmInput_Config_AlarmInputIn2Default =
{
    FALSE,                                        /* digital input          alarm enabled status */
    FALSE,                                        /* digital input restored alarm enabled status */
};


/* configuration - IN2 digital input alarm */
static       ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 AlarmInput_Config_AlarmInputIn3;
static const ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 AlarmInput_Config_AlarmInputIn3Default =
{
    FALSE,                                        /* digital input          alarm enabled status */
    FALSE,                                        /* digital input restored alarm enabled status */
};


/* configuration - IN2 digital input alarm */
static       ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 AlarmInput_Config_AlarmInputIn4;
static const ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 AlarmInput_Config_AlarmInputIn4Default =
{
    FALSE,                                        /* digital input          alarm enabled status */
    FALSE,                                        /* digital input restored alarm enabled status */
};


/* configuration - IN1 digital input */
static       ALARM_INPUT__CONFIG__INPUT_IN1       AlarmInput_Config_InputIn1;
static const ALARM_INPUT__CONFIG__INPUT_IN1       AlarmInput_Config_InputIn1Default =
{
    ALARM_INPUT__ACTIVATION_STATUS__CLOSE,        /* input activation status     */
    3,                                            /* input activation time value */
    ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND,    /* input activation time unit  */
};


/* configuration - IN2 digital input */
static       ALARM_INPUT__CONFIG__INPUT_IN2       AlarmInput_Config_InputIn2;
static const ALARM_INPUT__CONFIG__INPUT_IN2       AlarmInput_Config_InputIn2Default =
{
    ALARM_INPUT__ACTIVATION_STATUS__CLOSE,        /* input activation status     */
    3,                                            /* input activation time value */
    ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND,    /* input activation time unit  */
};


/* configuration - IN3 digital input */
static       ALARM_INPUT__CONFIG__INPUT_IN3       AlarmInput_Config_InputIn3;
static const ALARM_INPUT__CONFIG__INPUT_IN3       AlarmInput_Config_InputIn3Default =
{
    ALARM_INPUT__ACTIVATION_STATUS__CLOSE,        /* input activation status     */
    3,                                            /* input activation time value */
    ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND,    /* input activation time unit  */
};


/* configuration - IN4 digital input */
static       ALARM_INPUT__CONFIG__INPUT_IN4       AlarmInput_Config_InputIn4;
static const ALARM_INPUT__CONFIG__INPUT_IN4       AlarmInput_Config_InputIn4Default =
{
    ALARM_INPUT__ACTIVATION_STATUS__CLOSE,        /* input activation status     */
    3,                                            /* input activation time value */
    ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND,    /* input activation time unit  */
};


/* configuration - IN3 digital input */
static       ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  AlarmInput_Config_InputIn3Type;
static const ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  AlarmInput_Config_InputIn3TypeDefault =
{
    FALSE,                                        /* signal type [FALSE: analog signal, TRUE: digital signal] */
};


/* configuration - IN4 digital input */
static       ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  AlarmInput_Config_InputIn4Type;
static const ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  AlarmInput_Config_InputIn4TypeDefault =
{
    FALSE,                                        /* signal type [FALSE: analog signal, TRUE: digital signal] */
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputsReading;  /* timer for inputs reading  */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn1Start;  /* timer for input IN1 start */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn1Stop;   /* timer for input IN1 stop  */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn2Start;  /* timer for input IN2 start */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn2Stop;   /* timer for input IN2 stop  */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn3Start;  /* timer for input IN2 start */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn3Stop;   /* timer for input IN2 stop  */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn4Start;  /* timer for input IN2 start */
static       ql_timer_t                           AlarmInput_TimerHandler_TimerInputIn4Stop;   /* timer for input IN2 stop  */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void AlarmInput_TaskAlarmInput(void *argument);

/* task events */
static void AlarmInput_InputEvent(u8 input_id, bool input_status);
static void AlarmInput_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext);

/* new signal type input configuration */
       void AlarmInput_NewConfigSignalTypeIn3(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_config_new, ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_config_old);
       void AlarmInput_NewConfigSignalTypeIn4(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_config_new, ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_config_old);

/* check the input event */
static void AlarmInput_AlarmInput(u8 input_id, bool input_status);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "input 1 status" */
       void AlarmInput_Status_InputIn1_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN1       *ptr_data);
       void AlarmInput_Status_InputIn1_Get            (ALARM_INPUT__STATUS__INPUT_IN1       *ptr_data);
       void AlarmInput_Status_InputIn1_Set            (ALARM_INPUT__STATUS__INPUT_IN1       *ptr_data);

/* get/set "input 2 status" */
       void AlarmInput_Status_InputIn2_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN2       *ptr_data);
       void AlarmInput_Status_InputIn2_Get            (ALARM_INPUT__STATUS__INPUT_IN2       *ptr_data);
       void AlarmInput_Status_InputIn2_Set            (ALARM_INPUT__STATUS__INPUT_IN2       *ptr_data);

/* get/set "input 3 status" */
       void AlarmInput_Status_InputIn3_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN3       *ptr_data);
       void AlarmInput_Status_InputIn3_Get            (ALARM_INPUT__STATUS__INPUT_IN3       *ptr_data);
       void AlarmInput_Status_InputIn3_Set            (ALARM_INPUT__STATUS__INPUT_IN3       *ptr_data);

/* get/set "input 4 status" */
       void AlarmInput_Status_InputIn4_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN4       *ptr_data);
       void AlarmInput_Status_InputIn4_Get            (ALARM_INPUT__STATUS__INPUT_IN4       *ptr_data);
       void AlarmInput_Status_InputIn4_Set            (ALARM_INPUT__STATUS__INPUT_IN4       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "IN1 digital input alarm" configuration */
       void AlarmInput_Config_AlarmInputIn1_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);
       void AlarmInput_Config_AlarmInputIn1_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);
       void AlarmInput_Config_AlarmInputIn1_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);
       bool AlarmInput_Config_AlarmInputIn1_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);

/* get/set "IN2 digital input alarm" configuration */
       void AlarmInput_Config_AlarmInputIn2_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);
       void AlarmInput_Config_AlarmInputIn2_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);
       void AlarmInput_Config_AlarmInputIn2_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);
       bool AlarmInput_Config_AlarmInputIn2_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);

/* get/set "IN3 digital input alarm" configuration */
       void AlarmInput_Config_AlarmInputIn3_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);
       void AlarmInput_Config_AlarmInputIn3_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);
       void AlarmInput_Config_AlarmInputIn3_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);
       bool AlarmInput_Config_AlarmInputIn3_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);

/* get/set "IN4 digital input alarm" configuration */
       void AlarmInput_Config_AlarmInputIn4_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);
       void AlarmInput_Config_AlarmInputIn4_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);
       void AlarmInput_Config_AlarmInputIn4_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);
       bool AlarmInput_Config_AlarmInputIn4_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);

/* get/set "IN1 digital input"       configuration */
       void AlarmInput_Config_InputIn1_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);
       void AlarmInput_Config_InputIn1_Get            (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);
       void AlarmInput_Config_InputIn1_Set            (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);
       bool AlarmInput_Config_InputIn1_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);

/* get/set "IN2 digital input"       configuration */
       void AlarmInput_Config_InputIn2_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);
       void AlarmInput_Config_InputIn2_Get            (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);
       void AlarmInput_Config_InputIn2_Set            (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);
       bool AlarmInput_Config_InputIn2_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);

/* get/set "IN3 digital input"       configuration */
       void AlarmInput_Config_InputIn3_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);
       void AlarmInput_Config_InputIn3_Get            (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);
       void AlarmInput_Config_InputIn3_Set            (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);
       bool AlarmInput_Config_InputIn3_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);

/* get/set "IN4 digital input"       configuration */
       void AlarmInput_Config_InputIn4_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);
       void AlarmInput_Config_InputIn4_Get            (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);
       void AlarmInput_Config_InputIn4_Set            (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);
       bool AlarmInput_Config_InputIn4_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);

/* get/set "IN3 digital input type"  configuration */
       void AlarmInput_Config_InputIn3Type_GetDefault (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);
       void AlarmInput_Config_InputIn3Type_Get        (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);
       void AlarmInput_Config_InputIn3Type_Set        (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);
       bool AlarmInput_Config_InputIn3Type_IsValid    (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);

/* get/set "IN4 digital input type"  configuration */
       void AlarmInput_Config_InputIn4Type_GetDefault (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);
       void AlarmInput_Config_InputIn4Type_Get        (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);
       void AlarmInput_Config_InputIn4Type_Set        (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);
       bool AlarmInput_Config_InputIn4Type_IsValid    (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void AlarmInput_TimerInputsReading_Start(u32 time_value, bool periodic);

static void AlarmInput_TimerInputIn1Start_Start(u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn1Start_Stop (void);

static void AlarmInput_TimerInputIn1Stop_Start (u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn1Stop_Stop  (void);

static void AlarmInput_TimerInputIn2Start_Start(u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn2Start_Stop (void);

static void AlarmInput_TimerInputIn2Stop_Start (u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn2Stop_Stop  (void);

static void AlarmInput_TimerInputIn3Start_Start(u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn3Start_Stop (void);

static void AlarmInput_TimerInputIn3Stop_Start (u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn3Stop_Stop  (void);

static void AlarmInput_TimerInputIn4Start_Start(u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn4Start_Stop (void);

static void AlarmInput_TimerInputIn4Stop_Start (u32 time_value, bool periodic);
static void AlarmInput_TimerInputIn4Stop_Stop  (void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void AlarmInput_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void AlarmInput_AdlCallback_Timer_TimerInputsReading(void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn1Start(void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn1Stop (void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn2Start(void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn2Stop (void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn3Start(void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn3Stop (void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn4Start(void *context);
static void AlarmInput_AdlCallback_Timer_TimerInputIn4Stop (void *context);




/*=============================================================================
 * Function   : AlarmInput_TaskAlarmInput
 *
 * Description: digital input alarm task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void AlarmInput_TaskAlarmInput(void *argument)
{
    ql_event_t                 event;
    QlOSStatus                 err;

    COUNTERS__CONFIG__INPUT_C1 config_input_c1;
    COUNTERS__CONFIG__INPUT_C2 config_input_c2;

    bool                       in1_status;
    bool                       in2_status;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - ALARM_INPUT - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* get the configuration counter input C1 */
    /* get the configuration counter input C2 */
    Counters_Config_InputC1_Get(&config_input_c1);
    Counters_Config_InputC2_Get(&config_input_c2);

    /* check if IN1 and IN2 inputs are enabled */
    if (!config_input_c1.enabled_status)
        AlarmInput_InputEnabled_In1 = TRUE;
    else
        AlarmInput_InputEnabled_In1 = FALSE;
    if (!config_input_c2.enabled_status)
        AlarmInput_InputEnabled_In2 = TRUE;
    else
        AlarmInput_InputEnabled_In2 = FALSE;

    /* check if IN3 and IN4 inputs are enabled */
    if (AlarmInput_Config_InputIn3Type.signal_type)
        AlarmInput_InputEnabled_In3 = TRUE;
    else
        AlarmInput_InputEnabled_In3 = FALSE;
    if (AlarmInput_Config_InputIn4Type.signal_type)
        AlarmInput_InputEnabled_In4 = TRUE;
    else
        AlarmInput_InputEnabled_In4 = FALSE;


    snprintf(AlarmInput_DebugString,
             sizeof(AlarmInput_DebugString),
             "Input status before input event (IN1, IN2):  %d %d,  %d %d",
             AlarmInput_Status_InputIn1.status_input,
             AlarmInput_Status_InputIn1.status_change_timer,
             AlarmInput_Status_InputIn2.status_input,
             AlarmInput_Status_InputIn2.status_change_timer);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* read initial inputs status */
    /* check the input event */
    if (AlarmInput_InputEnabled_In1)
    {
        in1_status = DrvGpio_ReadInput(DRVGPIO__IN__IN1);

        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN1, in1_status);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        
        AlarmInput_AlarmInput(DRVGPIO__IN__IN1, in1_status);
    }
    if (AlarmInput_InputEnabled_In2)
    {
        in2_status = DrvGpio_ReadInput(DRVGPIO__IN__IN2);

        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN2, in2_status);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        
        AlarmInput_AlarmInput(DRVGPIO__IN__IN2, in2_status);
    }


    /* register the function for signals of digital input event */
    if (AlarmInput_InputEnabled_In1)
        DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__IN1, AlarmInput_InputEvent);
    if (AlarmInput_InputEnabled_In2)
        DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__IN2, AlarmInput_InputEvent);

    /* registered the function for signal of new available temperatures */
    DrvTemperature_RegisterSignalNewTemperatures(AlarmInput_NewTemperatures);


    /* creation of timer "TimerInputsReading" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputsReading, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputsReading, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn1Start" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn1Start, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn1Start, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn1Stop" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn1Stop, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn1Stop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn2Start" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn2Start, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn2Start, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn2Stop" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn2Stop, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn2Stop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn3Start" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn3Start, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn3Start, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn3Stop" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn3Stop, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn3Stop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn4Start" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn4Start, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn4Start, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerInputIn4Stop" */
    err = ql_rtos_timer_create(&AlarmInput_TimerHandler_TimerInputIn4Stop, QL_TIMER_IN_SERVICE, AlarmInput_AdlCallback_Timer_TimerInputIn4Stop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* start timer for inputs reading */
    AlarmInput_TimerInputsReading_Start(TIME_MS__INPUTS_READING, TRUE);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            AlarmInput_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*===========================================================================
 * Function   : AlarmInput_InputEvent
 *
 * Description: signal new input event
 * Input      : - input_id    : input ID
 *              - input_status: input status
 * Output     : -
 *===========================================================================*/
static void AlarmInput_InputEvent(u8 input_id, bool input_status)
{
    ql_event_t event;
    QlOSStatus err;


    if (
         (input_id != DRVGPIO__IN__IN1) &&
         (input_id != DRVGPIO__IN__IN2)
       )
    {
        return;
    }


    event.id     = TASK_MSG_ID__INPUT_EVENT;
    event.param1 = input_id;
    event.param2 = (u8)input_status;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmInput_NewTemperatures
 *
 * Description: signal new temperatures available (internal and external)
 * Input      : - sensor_status_int: new internal sensor status
 *              - temperature_int  : new internal temperature (/10 �C)
 *            : - sensor_status_ext: new external sensor status
 *              - temperature_ext  : new external temperature (/10 �C)
 * Output     : -
 *===========================================================================*/
static void AlarmInput_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext)
{
    ql_event_t event;
    QlOSStatus err;

    u8         in3_status;
    u8         in4_status;


    /* internal temperature */

    switch (sensor_status_int)
    {
        case DRVTEMPERATURE__SENSOR__UNKNOWN:
        default:
            in3_status = UINT8_MAX;
            break;

        case DRVTEMPERATURE__SENSOR__DISCONNECTED:
            in3_status = TRUE;
            break;

        case DRVTEMPERATURE__SENSOR__OUT_OF_ORDER:
            in3_status = FALSE;
            break;

        case DRVTEMPERATURE__SENSOR__CONNECTED:
            if      (temperature_int > 35)
            {
                in3_status = FALSE;
            }
            else if (temperature_int > -250)
            {
                in3_status = UINT8_MAX;
            }
            else
            {
                in3_status = FALSE;
            }
            break;
    }


    /* external temperature */
    switch (sensor_status_ext)
    {
        case DRVTEMPERATURE__SENSOR__UNKNOWN:
        default:
            in4_status = UINT8_MAX;
            break;

        case DRVTEMPERATURE__SENSOR__DISCONNECTED:
            in4_status = TRUE;
            break;

        case DRVTEMPERATURE__SENSOR__OUT_OF_ORDER:
            in4_status = FALSE;
            break;

        case DRVTEMPERATURE__SENSOR__CONNECTED:
            if      (temperature_ext > 35)
            {
                in4_status = FALSE;
            }
            else if (temperature_ext > -250)
            {
                in4_status = UINT8_MAX;
            }
            else
            {
                in4_status = FALSE;
            }
            break;
    }

    event.id     = TASK_MSG_ID__NEW_TEMPERATURE;
    event.param1 = in3_status;
    event.param2 = in4_status;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmInput_NewConfigSignalTypeIn3
 *
 * Description: signal a new "signal type IN3" configuration
 * Input      : - ptr_config_new: pointer to the new configuration
 *              - ptr_config_old: pointer to the old configuration
 * Output     : -
 *===========================================================================*/
void AlarmInput_NewConfigSignalTypeIn3(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_config_new, ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_config_old)
{
    ql_event_t event;
    QlOSStatus err;


    if (
         ((!ptr_config_old->signal_type) && ( ptr_config_new->signal_type)) ||
         (( ptr_config_old->signal_type) && (!ptr_config_new->signal_type))
       )
    {
        event.id = TASK_MSG_ID__CHANGED_SIGNAL_TYPE_IN3;

        err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*===========================================================================
 * Function   : AlarmInput_NewConfigSignalTypeIn4
 *
 * Description: signal a new "signal type IN4" configuration
 * Input      : - ptr_config_new: pointer to the new configuration
 *              - ptr_config_old: pointer to the old configuration
 * Output     : -
 *===========================================================================*/
void AlarmInput_NewConfigSignalTypeIn4(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_config_new, ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_config_old)
{
    ql_event_t event;
    QlOSStatus err;


    if (
         ((!ptr_config_old->signal_type) && ( ptr_config_new->signal_type)) ||
         (( ptr_config_old->signal_type) && (!ptr_config_new->signal_type))
       )
    {
        event.id = TASK_MSG_ID__CHANGED_SIGNAL_TYPE_IN4;

        err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*=============================================================================
 * Function   : AlarmInput_AlarmInput
 *
 * Description: verify if there is an "input alarm" condition
 * Input      :   - input_id    : input ID
 *                - input_status: input status
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AlarmInput(u8 input_id, bool input_status)
{
    u32 activation_time_value_ms;


    /* verify input ID value */
    if (
         (input_id != DRVGPIO__IN__IN1) &&
         (input_id != DRVGPIO__IN__IN2) &&
         (input_id != DRVGPIO__IN__IN3) &&
         (input_id != DRVGPIO__IN__IN4)
       )
    {
        return;
    }


    /*-------------------------------------------------------
     * IN1 input
     *-------------------------------------------------------*/
    if (input_id == DRVGPIO__IN__IN1)
    {
        /* input event on IN1 input */

        switch (AlarmInput_Status_InputIn1.status_input)
        {
            // status - input not alarmed
            case INPUT__OK:
                if (input_status == AlarmInput_Config_InputIn1.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status activation */
                    if (!AlarmInput_Status_InputIn1.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN1: start ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn1.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn1.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn1.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn1.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS);

                        /* start IN1 input start timer */
                        AlarmInput_TimerInputIn1Start_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status de-activation */
                    if ( AlarmInput_Status_InputIn1.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN1: stop ALARMED" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn1.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS);

                        /* stop IN1 input start timer */
                        AlarmInput_TimerInputIn1Start_Stop();
                    }
                }
                break;


            // status - input     alarmed
            case INPUT__ALARMED:
                if (input_status != AlarmInput_Config_InputIn1.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status de-activation */
                    if (!AlarmInput_Status_InputIn1.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN1: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn1.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn1.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn1.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn1.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn1.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS);

                        /* start IN1 input stop timer */
                        AlarmInput_TimerInputIn1Stop_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status activation */
                    if ( AlarmInput_Status_InputIn1.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN1: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn1.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS);

                        /* stop IN1 input stop timer */
                        AlarmInput_TimerInputIn1Stop_Stop();
                    }
                }
                break;
        }
    }


    /*-------------------------------------------------------
     * IN2 input
     *-------------------------------------------------------*/
    if (input_id == DRVGPIO__IN__IN2)
    {
        /* input event on IN2 input */

        switch (AlarmInput_Status_InputIn2.status_input)
        {
            // status - input not alarmed
            case INPUT__OK:
                if (input_status == AlarmInput_Config_InputIn2.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status activation */
                    if (!AlarmInput_Status_InputIn2.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN2: start ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn2.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn2.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn2.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn2.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS);

                        /* start IN2 input start timer */
                        AlarmInput_TimerInputIn2Start_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status de-activation */
                    if ( AlarmInput_Status_InputIn2.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN2: stop ALARMED" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn2.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS);

                        /* stop IN2 input start timer */
                        AlarmInput_TimerInputIn2Start_Stop();
                    }
                }
                break;


            // status - input     alarmed
            case INPUT__ALARMED:
                if (input_status != AlarmInput_Config_InputIn2.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status de-activation */
                    if (!AlarmInput_Status_InputIn2.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN2: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn2.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn2.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn2.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn2.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn2.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS);

                        /* start IN2 input stop timer */
                        AlarmInput_TimerInputIn2Stop_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status activation */
                    if ( AlarmInput_Status_InputIn2.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN2: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn2.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS);

                        /* stop IN2 input stop timer */
                        AlarmInput_TimerInputIn2Stop_Stop();
                    }
                }
                break;
        }
    }


    /*-------------------------------------------------------
     * IN3 input
     *-------------------------------------------------------*/
    if (input_id == DRVGPIO__IN__IN3)
    {
        /* input event on IN3 input */

        switch (AlarmInput_Status_InputIn3.status_input)
        {
            // status - input not alarmed
            case INPUT__OK:
                if (input_status == AlarmInput_Config_InputIn3.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status activation */
                    if (!AlarmInput_Status_InputIn3.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN3: start ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn3.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn3.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn3.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn3.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);

                        /* start IN3 input start timer */
                        AlarmInput_TimerInputIn3Start_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status de-activation */
                    if ( AlarmInput_Status_InputIn3.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN3: stop ALARMED" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn3.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);

                        /* stop IN3 input start timer */
                        AlarmInput_TimerInputIn3Start_Stop();
                    }
                }
                break;


            // status - input     alarmed
            case INPUT__ALARMED:
                if (input_status != AlarmInput_Config_InputIn3.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status de-activation */
                    if (!AlarmInput_Status_InputIn3.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN3: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn3.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn3.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn3.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn3.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn3.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);

                        /* start IN3 input stop timer */
                        AlarmInput_TimerInputIn3Stop_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status activation */
                    if ( AlarmInput_Status_InputIn3.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN3: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn3.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);

                        /* stop IN3 input stop timer */
                        AlarmInput_TimerInputIn3Stop_Stop();
                    }
                }
                break;
        }
    }


    /*-------------------------------------------------------
     * IN4 input
     *-------------------------------------------------------*/
    if (input_id == DRVGPIO__IN__IN4)
    {
        /* input event on IN4 input */

        switch (AlarmInput_Status_InputIn4.status_input)
        {
            // status - input not alarmed
            case INPUT__OK:
                if (input_status == AlarmInput_Config_InputIn4.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status activation */
                    if (!AlarmInput_Status_InputIn4.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN4: start ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn4.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn4.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn4.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn4.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);

                        /* start IN4 input start timer */
                        AlarmInput_TimerInputIn4Start_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status de-activation */
                    if ( AlarmInput_Status_InputIn4.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN4: stop ALARMED" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn4.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);

                        /* stop IN4 input start timer */
                        AlarmInput_TimerInputIn4Start_Stop();
                    }
                }
                break;


            // status - input     alarmed
            case INPUT__ALARMED:
                if (input_status != AlarmInput_Config_InputIn4.activation_status)    /* input activation status */   // [open/close]
                {
                    /* input status de-activation */
                    if (!AlarmInput_Status_InputIn4.status_change_timer)
                    {
                        /* status change timer not active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN4: start OK" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* calculate activation time in ms */
                        if      (AlarmInput_Config_InputIn4.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value          ) * 1000;
                        else if (AlarmInput_Config_InputIn4.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value *      60) * 1000;
                        else if (AlarmInput_Config_InputIn4.activation_time_unit == ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ms = ((u32)AlarmInput_Config_InputIn4.activation_time_value          ) * 1000;

                        AlarmInput_Status_InputIn4.status_change_timer = TRUE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);

                        /* start IN4 input stop timer */
                        AlarmInput_TimerInputIn4Stop_Start(activation_time_value_ms, FALSE);
                    }
                }
                else
                {
                    /* input status activation */
                    if ( AlarmInput_Status_InputIn4.status_change_timer)
                    {
                        /* status change timer     active */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN4: stop OK"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        AlarmInput_Status_InputIn4.status_change_timer = FALSE;

                        /* request the backup to flash objects */
                        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);

                        /* stop IN4 input stop timer */
                        AlarmInput_TimerInputIn4Stop_Stop();
                    }
                }
                break;
        }
    }
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn1_GetDefault
 *
 * Description: get the default "input IN1" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn1_GetDefault(ALARM_INPUT__STATUS__INPUT_IN1 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn1Default;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn1_Get
 *
 * Description: get the "input IN1" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn1_Get(ALARM_INPUT__STATUS__INPUT_IN1 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn1;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn1_Set
 *
 * Description: set the "input IN1" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn1_Set(ALARM_INPUT__STATUS__INPUT_IN1 *ptr_data)
{
    AlarmInput_Status_InputIn1 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn2_GetDefault
 *
 * Description: get the default "input IN2" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn2_GetDefault(ALARM_INPUT__STATUS__INPUT_IN2 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn2Default;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn2_Get
 *
 * Description: get the "input IN2" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn2_Get(ALARM_INPUT__STATUS__INPUT_IN2 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn2;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn2_Set
 *
 * Description: set the "input IN2" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn2_Set(ALARM_INPUT__STATUS__INPUT_IN2 *ptr_data)
{
    AlarmInput_Status_InputIn2 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn3_GetDefault
 *
 * Description: get the default "input IN3" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn3_GetDefault(ALARM_INPUT__STATUS__INPUT_IN3 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn3Default;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn3_Get
 *
 * Description: get the "input IN3" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn3_Get(ALARM_INPUT__STATUS__INPUT_IN3 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn3;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn3_Set
 *
 * Description: set the "input IN3" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn3_Set(ALARM_INPUT__STATUS__INPUT_IN3 *ptr_data)
{
    AlarmInput_Status_InputIn3 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn4_GetDefault
 *
 * Description: get the default "input IN4" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn4_GetDefault(ALARM_INPUT__STATUS__INPUT_IN4 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn4Default;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn4_Get
 *
 * Description: get the "input IN4" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn4_Get(ALARM_INPUT__STATUS__INPUT_IN4 *ptr_data)
{
    *ptr_data = AlarmInput_Status_InputIn4;
}




/*===========================================================================
 * Function   : AlarmInput_Status_InputIn4_Set
 *
 * Description: set the "input IN4" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Status_InputIn4_Set(ALARM_INPUT__STATUS__INPUT_IN4 *ptr_data)
{
    AlarmInput_Status_InputIn4 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn1_GetDefault
 *
 * Description: get the default "IN1 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn1_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn1Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn1_Get
 *
 * Description: get the "IN1 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn1_Get(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn1;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn1_Set
 *
 * Description: set the "IN1 digital input alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn1_Set(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data)
{
    AlarmInput_Config_AlarmInputIn1 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn1_IsValid
 *
 * Description: check if the "IN1 digital input alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_AlarmInputIn1_IsValid(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn2_GetDefault
 *
 * Description: get the default "IN2 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn2_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn2Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn2_Get
 *
 * Description: get the "IN2 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn2_Get(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn2;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn2_Set
 *
 * Description: set the "IN2 digital input alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn2_Set(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data)
{
    AlarmInput_Config_AlarmInputIn2 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn2_IsValid
 *
 * Description: check if the "IN2 digital input alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_AlarmInputIn2_IsValid(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn3_GetDefault
 *
 * Description: get the default "IN3 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn3_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn3Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn3_Get
 *
 * Description: get the "IN3 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn3_Get(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn3;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn3_Set
 *
 * Description: set the "IN3 digital input alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn3_Set(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data)
{
    AlarmInput_Config_AlarmInputIn3 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn3_IsValid
 *
 * Description: check if the "IN3 digital input alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_AlarmInputIn3_IsValid(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn4_GetDefault
 *
 * Description: get the default "IN4 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn4_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn4Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn4_Get
 *
 * Description: get the "IN4 digital input alarm" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn4_Get(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data)
{
    *ptr_data = AlarmInput_Config_AlarmInputIn4;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn4_Set
 *
 * Description: set the "IN4 digital input alarm" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_AlarmInputIn4_Set(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data)
{
    AlarmInput_Config_AlarmInputIn4 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_AlarmInputIn4_IsValid
 *
 * Description: check if the "IN4 digital input alarm" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_AlarmInputIn4_IsValid(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn1_GetDefault
 *
 * Description: get the default "IN1 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn1_GetDefault(ALARM_INPUT__CONFIG__INPUT_IN1 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn1Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn1_Get
 *
 * Description: get the "IN1 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn1_Get(ALARM_INPUT__CONFIG__INPUT_IN1 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn1;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn1_Set
 *
 * Description: set the "IN1 digital input" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn1_Set(ALARM_INPUT__CONFIG__INPUT_IN1 *ptr_data)
{
    AlarmInput_Config_InputIn1 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn1_IsValid
 *
 * Description: check if the "IN1 digital input" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_InputIn1_IsValid(ALARM_INPUT__CONFIG__INPUT_IN1 *ptr_data)
{
    if (ptr_data->activation_time_value == 0)
    {
        return FALSE;
    }

    if (
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn2_GetDefault
 *
 * Description: get the default "IN2 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn2_GetDefault(ALARM_INPUT__CONFIG__INPUT_IN2 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn2Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn2_Get
 *
 * Description: get the "IN2 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn2_Get(ALARM_INPUT__CONFIG__INPUT_IN2 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn2;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn2_Set
 *
 * Description: set the "IN2 digital input" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn2_Set(ALARM_INPUT__CONFIG__INPUT_IN2 *ptr_data)
{
    AlarmInput_Config_InputIn2 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn2_IsValid
 *
 * Description: check if the "IN2 digital input" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_InputIn2_IsValid(ALARM_INPUT__CONFIG__INPUT_IN2 *ptr_data)
{
    if (ptr_data->activation_time_value == 0)
    {
        return FALSE;
    }

    if (
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3_GetDefault
 *
 * Description: get the default "IN3 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn3_GetDefault(ALARM_INPUT__CONFIG__INPUT_IN3 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn3Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3_Get
 *
 * Description: get the "IN3 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn3_Get(ALARM_INPUT__CONFIG__INPUT_IN3 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn3;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3_Set
 *
 * Description: set the "IN3 digital input" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn3_Set(ALARM_INPUT__CONFIG__INPUT_IN3 *ptr_data)
{
    AlarmInput_Config_InputIn3 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3_IsValid
 *
 * Description: check if the "IN3 digital input" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_InputIn3_IsValid(ALARM_INPUT__CONFIG__INPUT_IN3 *ptr_data)
{
    if (ptr_data->activation_time_value == 0)
    {
        return FALSE;
    }

    if (
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4_GetDefault
 *
 * Description: get the default "IN4 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn4_GetDefault(ALARM_INPUT__CONFIG__INPUT_IN4 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn4Default;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4_Get
 *
 * Description: get the "IN4 digital input" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn4_Get(ALARM_INPUT__CONFIG__INPUT_IN4 *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn4;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4_Set
 *
 * Description: set the "IN4 digital input" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn4_Set(ALARM_INPUT__CONFIG__INPUT_IN4 *ptr_data)
{
    AlarmInput_Config_InputIn4 = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4_IsValid
 *
 * Description: check if the "IN4 digital input" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_InputIn4_IsValid(ALARM_INPUT__CONFIG__INPUT_IN4 *ptr_data)
{
    if (ptr_data->activation_time_value == 0)
    {
        return FALSE;
    }

    if (
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE) &&
         (ptr_data->activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3Type_GetDefault
 *
 * Description: get the default "IN3 digital input type" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn3Type_GetDefault(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn3TypeDefault;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3Type_Get
 *
 * Description: get the "IN3 digital input type" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn3Type_Get(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn3Type;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3Type_Set
 *
 * Description: set the "IN3 digital input type" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn3Type_Set(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_data)
{
    AlarmInput_Config_InputIn3Type = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn3Type_IsValid
 *
 * Description: check if the "IN3 digital input type" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_InputIn3Type_IsValid(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4Type_GetDefault
 *
 * Description: get the default "IN4 digital input type" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn4Type_GetDefault(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn4TypeDefault;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4Type_Get
 *
 * Description: get the "IN4 digital input type" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn4Type_Get(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_data)
{
    *ptr_data = AlarmInput_Config_InputIn4Type;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4Type_Set
 *
 * Description: set the "IN4 digital input type" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void AlarmInput_Config_InputIn4Type_Set(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_data)
{
    AlarmInput_Config_InputIn4Type = *ptr_data;
}




/*===========================================================================
 * Function   : AlarmInput_Config_InputIn4Type_IsValid
 *
 * Description: check if the "IN4 digital input type" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool AlarmInput_Config_InputIn4Type_IsValid(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_data)
{
    return TRUE;
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputsReading_Start
 *
 * Description: start the "inputs reading" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputsReading_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"inputs reading\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputsReading, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn1Start_Start
 *
 * Description: start the "input IN1 START" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn1Start_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN1 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn1Start, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn1Start_Stop
 *
 * Description: stop the "input IN1 START" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn1Start_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN1 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn1Start);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn1Stop_Start
 *
 * Description: start the "input IN1 STOP" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn1Stop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN1 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn1Stop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn1Stop_Stop
 *
 * Description: stop the "input IN1 STOP" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn1Stop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN1 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn1Stop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn2Start_Start
 *
 * Description: start the "input IN2 START" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn2Start_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN2 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn2Start, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn2Start_Stop
 *
 * Description: stop the "input IN2 START" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn2Start_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN2 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn2Start);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn2Stop_Start
 *
 * Description: start the "input IN2 STOP" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn2Stop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN2 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn2Stop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn2Stop_Stop
 *
 * Description: stop the "input IN2 STOP" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn2Stop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN2 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn2Stop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn3Start_Start
 *
 * Description: start the "input IN3 START" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn3Start_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN3 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn3Start, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn3Start_Stop
 *
 * Description: stop the "input IN3 START" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn3Start_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN3 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn3Start);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn3Stop_Start
 *
 * Description: start the "input IN3 STOP" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn3Stop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN3 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn3Stop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn3Stop_Stop
 *
 * Description: stop the "input IN3 STOP" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn3Stop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN3 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn3Stop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn4Start_Start
 *
 * Description: start the "input IN4 START" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn4Start_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN4 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn4Start, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn4Start_Stop
 *
 * Description: stop the "input IN4 START" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn4Start_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN4 START\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn4Start);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn4Stop_Start
 *
 * Description: start the "input IN4 STOP" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn4Stop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Start \"input IN4 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(AlarmInput_TimerHandler_TimerInputIn4Stop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_TimerInputIn4Stop_Stop
 *
 * Description: stop the "input IN4 STOP" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AlarmInput_TimerInputIn4Stop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Stop \"input IN4 STOP\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(AlarmInput_TimerHandler_TimerInputIn4Stop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : AlarmInput_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void AlarmInput_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    u8   input_id;
    bool input_status;

    bool in1_status;
    bool in2_status;

    u8   in3_status;
    u8   in4_status;

    bool result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    /* debug */
    snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "CALLBACK     - MESSAGE     - TASK ALARM_INPUT - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        // event - input event
        case TASK_MSG_ID__INPUT_EVENT:
            /* extract the parameters */
            input_id     = (u8  )msg_identifier->param1;
            input_status = (bool)msg_identifier->param2;

            snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", input_id, input_status);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input status before input event (IN1, IN2):  %d %d,  %d %d",
                                            AlarmInput_Status_InputIn1.status_input,
                                            AlarmInput_Status_InputIn1.status_change_timer,
                                            AlarmInput_Status_InputIn2.status_input,
                                            AlarmInput_Status_InputIn2.status_change_timer);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* check the input event */
            AlarmInput_AlarmInput(input_id, input_status);
            break;



        // event - new temperature available
        case TASK_MSG_ID__NEW_TEMPERATURE:
            /* extract the parameters */
            in3_status = (u8)msg_identifier->param1;
            in4_status = (u8)msg_identifier->param2;

            /* read inputs status */
            /* check the input event */
            if (AlarmInput_InputEnabled_In3)
            {
                if (in3_status != UINT8_MAX)
                {
                    input_status = (bool)in3_status;

                    snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN3, input_status);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmInput_AlarmInput(DRVGPIO__IN__IN3, input_status);
                }
            }
            if (AlarmInput_InputEnabled_In4)
            {
                if (in4_status != UINT8_MAX)
                {
                    input_status = (bool)in4_status;

                    snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN4, input_status);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    AlarmInput_AlarmInput(DRVGPIO__IN__IN4, input_status);
                }
            }
            break;



        // event - timeout inputs reading
        case TASK_MSG_ID__TIMEOUT_INPUTS_READING:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Timeout inputs reading", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            snprintf(AlarmInput_DebugString,
                     sizeof(AlarmInput_DebugString),
                     "Input status before input event (IN1, IN2):  %d %d,  %d %d",
                     AlarmInput_Status_InputIn1.status_input,
                     AlarmInput_Status_InputIn1.status_change_timer,
                     AlarmInput_Status_InputIn2.status_input,
                     AlarmInput_Status_InputIn2.status_change_timer);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* read inputs status */
            /* check the input event */
            if (AlarmInput_InputEnabled_In1)
            {
                in1_status = DrvGpio_ReadInput(DRVGPIO__IN__IN1);

                snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN1, in1_status);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                
                AlarmInput_AlarmInput(DRVGPIO__IN__IN1, in1_status);
            }
            if (AlarmInput_InputEnabled_In2)
            {
                in2_status = DrvGpio_ReadInput(DRVGPIO__IN__IN2);

                snprintf(AlarmInput_DebugString, sizeof(AlarmInput_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN2, in2_status);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, AlarmInput_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                
                AlarmInput_AlarmInput(DRVGPIO__IN__IN2, in2_status);
            }
            break;



        // event - timeout input IN1 START
        case TASK_MSG_ID__TIMEOUT_INPUT_IN1_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN1 ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn1.status_input        = INPUT__ALARMED;
            AlarmInput_Status_InputIn1.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS);

            if (AlarmInput_Config_AlarmInputIn1.alarm_enabled_status_input)
            {
                /* "IN1 digital input" alarm enabled */

                /* generate "IN1 digital input" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN1 input\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN1, 0);
            }
            break;



        // event - timeout input IN1 STOP
        case TASK_MSG_ID__TIMEOUT_INPUT_IN1_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN1 OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn1.status_input        = INPUT__OK;
            AlarmInput_Status_InputIn1.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS);

            if (AlarmInput_Config_AlarmInputIn1.alarm_enabled_status_input_restored)
            {
                /* "IN1 digital input restored" alarm enabled */

                /* generate "IN1 digital input restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN1 input restored\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN1_RESTORED, 0);
            }
            break;



        // event - timeout input IN2 START
        case TASK_MSG_ID__TIMEOUT_INPUT_IN2_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN2 ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn2.status_input        = INPUT__ALARMED;
            AlarmInput_Status_InputIn2.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS);

            if (AlarmInput_Config_AlarmInputIn2.alarm_enabled_status_input)
            {
                /* "IN2 digital input" alarm enabled */

                /* generate "IN2 digital input" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN2 input\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN2, 0);
            }
            break;



        // event - timeout input IN2 STOP
        case TASK_MSG_ID__TIMEOUT_INPUT_IN2_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN2 OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn2.status_input        = INPUT__OK;
            AlarmInput_Status_InputIn2.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS);

            if (AlarmInput_Config_AlarmInputIn2.alarm_enabled_status_input_restored)
            {
                /* "IN2 digital input restored" alarm enabled */

                /* generate "IN2 digital input restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN2 input restored\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN2_RESTORED, 0);
            }
            break;



        // event - timeout input IN3 START
        case TASK_MSG_ID__TIMEOUT_INPUT_IN3_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN3 ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn3.status_input        = INPUT__ALARMED;
            AlarmInput_Status_InputIn3.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);

            if (AlarmInput_Config_AlarmInputIn3.alarm_enabled_status_input)
            {
                /* "IN3 digital input" alarm enabled */

                /* generate "IN3 digital input" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN3 input\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN3, 0);
            }
            break;



        // event - timeout input IN3 STOP
        case TASK_MSG_ID__TIMEOUT_INPUT_IN3_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN3 OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn3.status_input        = INPUT__OK;
            AlarmInput_Status_InputIn3.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);

            if (AlarmInput_Config_AlarmInputIn3.alarm_enabled_status_input_restored)
            {
                /* "IN3 digital input restored" alarm enabled */

                /* generate "IN3 digital input restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN3 input restored\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN3_RESTORED, 0);
            }
            break;



        // event - timeout input IN4 START
        case TASK_MSG_ID__TIMEOUT_INPUT_IN4_START:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN4 ALARMED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn4.status_input        = INPUT__ALARMED;
            AlarmInput_Status_InputIn4.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);

            if (AlarmInput_Config_AlarmInputIn4.alarm_enabled_status_input)
            {
                /* "IN4 digital input" alarm enabled */

                /* generate "IN4 digital input" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN4 input\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN4, 0);
            }
            break;



        // event - timeout input IN4 STOP
        case TASK_MSG_ID__TIMEOUT_INPUT_IN4_STOP:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "Input IN4 OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* update the IN1 input status */
            AlarmInput_Status_InputIn4.status_input        = INPUT__OK;
            AlarmInput_Status_InputIn4.status_change_timer = FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);

            if (AlarmInput_Config_AlarmInputIn4.alarm_enabled_status_input_restored)
            {
                /* "IN4 digital input restored" alarm enabled */

                /* generate "IN4 digital input restored" alarm */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "\"IN4 input restored\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__IN4_RESTORED, 0);
            }
            break;



        // event - signal type IN3 configuration changed
        case TASK_MSG_ID__CHANGED_SIGNAL_TYPE_IN3:
            if (AlarmInput_Config_InputIn3Type.signal_type)
                AlarmInput_InputEnabled_In3 = TRUE;
            else
                AlarmInput_InputEnabled_In3 = FALSE;

            if (AlarmInput_Config_InputIn3Type.signal_type)
            {
                switch (AlarmInput_Status_InputIn3.status_input)
                {
                    // status - input not alarmed
                    case INPUT__OK:
                        input_status = !AlarmInput_Config_InputIn3.activation_status;    /* input activation status */   // [open/close]
                        break;


                    // status - input     alarmed
                    case INPUT__ALARMED:
                        input_status =  AlarmInput_Config_InputIn3.activation_status;    /* input activation status */   // [open/close]
                        break;
                }

                /* check the input event */
                AlarmInput_AlarmInput(DRVGPIO__IN__IN3, input_status);

                /* update the IN3 input status */
                AlarmInput_Status_InputIn3.status_input = INPUT__OK;

                /* request the backup to flash objects */
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS);
            }
            break;



        // event - signal type IN3 configuration changed
        case TASK_MSG_ID__CHANGED_SIGNAL_TYPE_IN4:
            if (AlarmInput_Config_InputIn4Type.signal_type)
                AlarmInput_InputEnabled_In4 = TRUE;
            else
                AlarmInput_InputEnabled_In4 = FALSE;

            if (AlarmInput_Config_InputIn4Type.signal_type)
            {
                switch (AlarmInput_Status_InputIn4.status_input)
                {
                    // status - input not alarmed
                    case INPUT__OK:
                        input_status = !AlarmInput_Config_InputIn4.activation_status;    /* input activation status */   // [open/close]
                        break;


                    // status - input     alarmed
                    case INPUT__ALARMED:
                        input_status =  AlarmInput_Config_InputIn4.activation_status;    /* input activation status */   // [open/close]
                        break;
                }

                /* check the input event */
                AlarmInput_AlarmInput(DRVGPIO__IN__IN4, input_status);

                /* update the IN4 input status */
                AlarmInput_Status_InputIn4.status_input = INPUT__OK;

                /* request the backup to flash objects */
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO, PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS);
            }
            break;



        // event - new configuration
        case TASK_MSG_ID__NEW_CONFIGURATION:
            //@@@ TO DO
            break;



        // unknown event
        default:
            break;
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputsReading
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputsReading(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUTS READING", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUTS_READING;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn1Start
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn1Start(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN1 START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN1_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn1Stop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn1Stop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN1 STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN1_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn2Start
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn2Start(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN2 START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN2_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn2Stop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn2Stop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN2 STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN2_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn3Start
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn3Start(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN3 START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN3_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn3Stop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn3Stop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN3 STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN3_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn4Start
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn4Start(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN4 START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN4_START;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : AlarmInput_AdlCallback_Timer_TimerInputIn4Stop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void AlarmInput_AdlCallback_Timer_TimerInputIn4Stop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - INPUT IN4 STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_INPUT_IN4_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_AlarmInput, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_INPUT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
