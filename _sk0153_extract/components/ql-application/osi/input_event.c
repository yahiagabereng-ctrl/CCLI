/*=============================================================================
 * File       :  INPUT_EVENT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - input event manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "counters.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "input_event.h"
//#include "phone.h"
#include "pod.h"
#include "queue_my.h"
#include "sms_answer_event.h"
#include "startup.h"
#include "status.h"
#include "typedef.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* status */
#define STATUS__IDLE                                0      // status - idle
#define STATUS__WAIT_INPUT_STATUS                   1      // status - wait for an input status

/* task message IDs */
#define TASK_MSG_ID__INPUT_STATUS_EXPECTED          0      // event - new input status expected
#define TASK_MSG_ID__INPUT_EVENT                    1      // event - new input event
#define TASK_MSG_ID__TIMEOUT_IN1                    2      // event - timeout timer IN1
#define TASK_MSG_ID__TIMEOUT_IN2                    3      // event - timeout timer IN2
#define TASK_MSG_ID__TIMEOUT_INPUT_STATUS_EXPECTED  4      // event - timeout input status expected (not used)

/* timeout for input status change (ms) */
#define TIME_MS__INPUT_STATUS_CHANGE                (3 * 1000L)

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                     200


/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define INPUT_EVENT__MAIL_SIZE                      25

/* message queue size */
#define MESSAGE_QUEUE_SIZE                          5




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                           // message id
    u32 length_data;                     // length of additional data
    u8  data[INPUT_EVENT__MAIL_SIZE];    //           additional data
} INPUT_EVENT__MAIL;




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* status */
static u8         InputEvent_Status;         // = STATUS__IDLE;

/* status */
static bool       InputEvent_InputStatus;    // = FALSE;           // input status expected (CLOSE or OPEN)
static bool       InputEvent_In1;            // = FALSE;           // indication if IN1 input should be controlled
static bool       InputEvent_In2;            // = FALSE;           // indication if IN2 input should be controlled

/* SMS phone number */
static ascii      InputEvent_SmsPhoneNumber[20 + 1];               // SMS phone number

/* indication of expected status for IN1 input and IN2 input */
static bool       InputEvent_In1Ok;          // = FALSE;
static bool       InputEvent_In2Ok;          // = FALSE;

/* indication of status of timer IN1 and timer IN2 */
static bool       InputEvent_TimerIn1;       // = FALSE;
static bool       InputEvent_TimerIn2;       // = FALSE;

/* CRC variables */
static u16        InputEvent_VarCrc;         // = 0x0000;

/* indication of input enabled (IN1 and IN2) */
static bool       InputEvent_InputEnabled_In1 = TRUE;
static bool       InputEvent_InputEnabled_In2 = TRUE;

/* debug string */
static ascii      InputEvent_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* message queue handles */
static ql_queue_t InputEvent_MessageQueueHandler_MessageQueueEvents;

/* timer handler */
static ql_timer_t InputEvent_TimerHandler_TimerIn1;   /* timer for IN1 input */
static ql_timer_t InputEvent_TimerHandler_TimerIn2;   /* timer for IN2 input */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void InputEvent_TaskInputEvent(void *argument);

/* status variables */
static void InputEvent_InitVar        (void);
static u16  InputEvent_CalculateVarCrc(void);
       void InputEvent_UpdateVarCrc   (void);
       bool InputEvent_VerifyVarCrc   (void);

/* task events */
       void InputEvent_Event_InputStatusExpected(bool input_status_expected, bool input_status, bool in1, bool in2, ascii *sms_phone_number);
static void InputEvent_Event_InputEvent         (u8   input_id,              bool input_status);

/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void InputEvent_TimerIn1_Start(u32 time_value, bool periodic);
static void InputEvent_TimerIn1_Stop (void);

static void InputEvent_TimerIn2_Start(u32 time_value, bool periodic);
static void InputEvent_TimerIn2_Stop (void);

/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void InputEvent_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);

/* timer   callback functions */
static void InputEvent_AdlCallback_Timer_TimerIn1(void *context);
static void InputEvent_AdlCallback_Timer_TimerIn2(void *context);




/*===========================================================================
 * Function   : InputEvent_TaskInputEvent
 *
 * Description: task manager input event
 * Input      : -
 * Output     : -
 *===========================================================================*/
void InputEvent_TaskInputEvent(void *argument)
{
    INPUT_EVENT__MAIL          message_received;
    QlOSStatus                 err;

    COUNTERS__CONFIG__INPUT_C1 config_input_c1;
    COUNTERS__CONFIG__INPUT_C2 config_input_c2;

  //bool                       in1_status;
  //bool                       in2_status;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - INPUT_EVENT - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* get the configuration counter input C1 */
    /* get the configuration counter input C2 */
    Counters_Config_InputC1_Get(&config_input_c1);
    Counters_Config_InputC2_Get(&config_input_c2);

    /* check if IN1 and IN2 inputs are enabled */
    if (!config_input_c1.enabled_status)
        InputEvent_InputEnabled_In1 = TRUE;
    else
        InputEvent_InputEnabled_In1 = FALSE;
    if (!config_input_c2.enabled_status)
        InputEvent_InputEnabled_In2 = TRUE;
    else
        InputEvent_InputEnabled_In2 = FALSE;


    /* read initial inputs status */
  //if (InputEvent_InputEnabled_In1)
  //    in1_status = DrvGpio_ReadInput(DRVGPIO__IN__IN1);
  //if (InputEvent_InputEnabled_In2)
  //    in2_status = DrvGpio_ReadInput(DRVGPIO__IN__IN2);

  //if (InputEvent_InputEnabled_In1)
  //{
  //    snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN1, in1_status);
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}

  //if (InputEvent_InputEnabled_In2)
  //{
  //    snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input id: %d, input_status: %d", DRVGPIO__IN__IN2, in2_status);
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}


    /* check the input event */
  //if (InputEvent_InputEnabled_In1)
  //    InputEvent_Event(DRVGPIO__IN__IN1, in1_status);
  //if (InputEvent_InputEnabled_In2)
  //    InputEvent_Event(DRVGPIO__IN__IN2, in2_status);


    /* register the function for signals of digital input event */
    if (InputEvent_InputEnabled_In1)
        DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__IN1, InputEvent_Event_InputEvent);
    if (InputEvent_InputEnabled_In2)
        DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__IN2, InputEvent_Event_InputEvent);


    /* internal status init */
    InputEvent_InitVar();


    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&InputEvent_MessageQueueHandler_MessageQueueEvents, sizeof(INPUT_EVENT__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerIn1" */
    err = ql_rtos_timer_create(&InputEvent_TimerHandler_TimerIn1, QL_TIMER_IN_SERVICE, InputEvent_AdlCallback_Timer_TimerIn1, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerIn2" */
    err = ql_rtos_timer_create(&InputEvent_TimerHandler_TimerIn2, QL_TIMER_IN_SERVICE, InputEvent_AdlCallback_Timer_TimerIn2, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    for (;;)
    {
        err = ql_rtos_queue_wait(InputEvent_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(INPUT_EVENT__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            InputEvent_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*=============================================================================
 * Function   : InputEvent_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void InputEvent_InitVar(void)
{
    bool recover_status;
    u8   i;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* status */
        InputEvent_Status = STATUS__IDLE;

        /* status */
        InputEvent_InputStatus = FALSE;
        InputEvent_In1         = FALSE;
        InputEvent_In2         = FALSE;

        /* SMS phone number */
        for (i = 0; i < (20 + 1); i++)
            InputEvent_SmsPhoneNumber[i] = 0x00;

        /* indication of expected status for IN1 input and IN2 input */
        InputEvent_In1Ok    = FALSE;
        InputEvent_In2Ok    = FALSE;

        /* indication of status of timer IN1 and timer IN2 */
        InputEvent_TimerIn1 = FALSE;
        InputEvent_TimerIn2 = FALSE;


        /**** update the variable CRC ****/
        InputEvent_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** outputs ****/
        // NOTHING TO DO
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time
        // NOTHING TO DO


        /**** outputs ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : InputEvent_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 InputEvent_CalculateVarCrc(void)
{
    u16 crc;
    u8  i;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_Status     , sizeof(InputEvent_Status     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_InputStatus, sizeof(InputEvent_InputStatus), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_In1        , sizeof(InputEvent_In1        ), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_In2        , sizeof(InputEvent_In2        ), crc);
    for (i = 0; i < (20 + 1); i++)
        crc = Utility_CalculateCRC16((u8 *)&InputEvent_SmsPhoneNumber[i], sizeof(InputEvent_SmsPhoneNumber[i]), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_In1Ok      , sizeof(InputEvent_In1Ok      ), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_In2Ok      , sizeof(InputEvent_In2Ok      ), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_TimerIn1   , sizeof(InputEvent_TimerIn1   ), crc);
    crc = Utility_CalculateCRC16((u8 *)&InputEvent_TimerIn2   , sizeof(InputEvent_TimerIn2   ), crc);

    return crc;
}




/*=============================================================================
 * Function   : InputEvent_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void InputEvent_UpdateVarCrc(void)
{
    InputEvent_VarCrc = InputEvent_CalculateVarCrc();
}




/*=============================================================================
 * Function   : InputEvent_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool InputEvent_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = InputEvent_CalculateVarCrc();

    if (InputEvent_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : InputEvent_Event_InputStatusExpected
 *
 * Description: signal if an input status (CLOSE or OPEN) is expected
 * Input      : - input_status_expected: indication if an input status is expected
 *              - input_status         : input status expected (CLOSE or OPEN)
 *              - in1                  : indication if IN1 input should be controlled
 *              - in2                  : indication if IN2 input should be controlled
 *              - sms_phone_number     : SMS phone number for sending the SMS answer event
 * Output     : -
 *===========================================================================*/
void InputEvent_Event_InputStatusExpected(bool input_status_expected, bool input_status, bool in1, bool in2, ascii *sms_phone_number)
{
    INPUT_EVENT__MAIL message_to_be_sent;
    QlOSStatus        err;

    u8                i;


    if (
         ( input_status_expected) &&
         (!in1                  ) &&
         (!in2                  )
       )
    {
        return;
    }

    if (strlen(sms_phone_number) > 0)
    {
        if (!Utility_IsPhoneString(sms_phone_number))
            return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__INPUT_STATUS_EXPECTED;
    message_to_be_sent.length_data = 4 + 20 + 1;
    message_to_be_sent.data[0]     = input_status_expected;
    message_to_be_sent.data[1]     = input_status;
    message_to_be_sent.data[2]     = in1;
    message_to_be_sent.data[3]     = in2;
    for (i = 0; i < 20; i++)
        message_to_be_sent.data[4 + i] = (u8)sms_phone_number[i];
    message_to_be_sent.data[4 + 20] = 0x00;

    err = ql_rtos_queue_release(InputEvent_MessageQueueHandler_MessageQueueEvents, sizeof(INPUT_EVENT__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : InputEvent_Event_InputEvent
 *
 * Description: signal new input event
 * Input      : - input_id    : input ID
 *              - input_status: input status
 * Output     : -
 *===========================================================================*/
static void InputEvent_Event_InputEvent(u8 input_id, bool input_status)
{
    INPUT_EVENT__MAIL message_to_be_sent;
    QlOSStatus        err;

    u8                i;


    if (
         (input_id != DRVGPIO__IN__IN1) &&
         (input_id != DRVGPIO__IN__IN2)
       )
    {
        return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__INPUT_EVENT;
    message_to_be_sent.length_data = 2;
    message_to_be_sent.data[0]     =       input_id;
    message_to_be_sent.data[1]     = (u8)input_status;
    for (i = 2; i < INPUT_EVENT__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(InputEvent_MessageQueueHandler_MessageQueueEvents, sizeof(INPUT_EVENT__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : InputEvent_TimerIn1_Start
 *
 * Description: start the "IN1" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void InputEvent_TimerIn1_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Start \"IN1\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(InputEvent_TimerHandler_TimerIn1, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : InputEvent_TimerIn1_Stop
 *
 * Description: stop the "IN1" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void InputEvent_TimerIn1_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Stop \"IN1\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(InputEvent_TimerHandler_TimerIn1);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : InputEvent_TimerIn2_Start
 *
 * Description: start the "IN2" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void InputEvent_TimerIn2_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Start \"IN2\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(InputEvent_TimerHandler_TimerIn2, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : InputEvent_TimerIn2_Stop
 *
 * Description: stop the "IN2" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void InputEvent_TimerIn2_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Stop \"IN2\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(InputEvent_TimerHandler_TimerIn2);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : InputEvent_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void InputEvent_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    u8    old_status;

    bool  old_input_status;
    bool  old_in1;
    bool  old_in2;

    ascii old_sms_phone_number[20 + 1];

    bool  old_input_event_in1_ok;
    bool  old_input_event_in2_ok;
    bool  old_input_event_timer_in1;
    bool  old_input_event_timer_in2;

    bool  update_var_crc;

    /* event parameters */
    bool  ev1_input_status_expected;
    bool  ev1_input_status;
    bool  ev1_in1;
    bool  ev1_in2;
    ascii ev1_sms_pohone_number[20 + 1];

    /* event parameters */
    u8    ev2_input_id;
    bool  ev2_input_status;

    /* IN1 input and IN2 input actual status */
    bool  in1_status;
    bool  in2_status;

    u8    i;


    /* debug */
    snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "CALLBACK     - MESSAGE     - TASK INPUT_EVENT - msg identifier: %lu, length: %lu", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* remove compiler warning maybe-uninitialized */
    in1_status = FALSE;
    in2_status = FALSE;

    /* check the data length */
    switch (msg_identifier)
    {
        // event - new input status expected
        case TASK_MSG_ID__INPUT_STATUS_EXPECTED:
            if (length != (4 + 20 + 1))
                return;
            break;

        // event - input event
        case TASK_MSG_ID__INPUT_EVENT:
            if (length != 2)
                return;
            break;

        // event - timeout timer IN1
        // event - timeout timer IN2
        // event - timeout input status expected
        case TASK_MSG_ID__TIMEOUT_IN1:
        case TASK_MSG_ID__TIMEOUT_IN2:
        case TASK_MSG_ID__TIMEOUT_INPUT_STATUS_EXPECTED:
            if (length != 0)
                return;
            break;

        /* unknown message identifier */
        default:
            return;
    }



    /* status */
    old_status                  = InputEvent_Status;

    /* status */
    old_input_status            = InputEvent_InputStatus;
    old_in1                     = InputEvent_In1;
    old_in2                     = InputEvent_In2;

    /* SMS phone number */
    for (i = 0; i < (20 + 1); i++)
        old_sms_phone_number[i] = InputEvent_SmsPhoneNumber[i];

    /* indication of expected status for IN1 input and IN2 input */
    old_input_event_in1_ok      = InputEvent_In1Ok;
    old_input_event_in2_ok      = InputEvent_In2Ok;

    /* indication of status of timer IN1 and timer IN2 */
    old_input_event_timer_in1   = InputEvent_TimerIn1;
    old_input_event_timer_in2   = InputEvent_TimerIn2;



    switch (InputEvent_Status)
    {
        // status - idle
        case STATUS__IDLE:
            switch (msg_identifier)
            {
                // event - new input status expected
                case TASK_MSG_ID__INPUT_STATUS_EXPECTED:
                    /* extract the parameters */
                    ev1_input_status_expected = (bool)*(ptr_data    );
                    ev1_input_status          = (bool)*(ptr_data + 1);
                    ev1_in1                   = (bool)*(ptr_data + 2);
                    ev1_in2                   = (bool)*(ptr_data + 3);
                    for (i = 0; i <= 20; i++)
                        ev1_sms_pohone_number[i] = (ascii)*(ptr_data + 4 + i);

                    /* stop timer IN1 */
                    /* stop timer IN2 */
                    //InputEvent_TimerIn1_Stop();
                    //InputEvent_TimerIn2_Stop();
                    //InputEvent_TimerIn1 = FALSE;
                    //InputEvent_TimerIn2 = FALSE;

                    /* check if an input status is expected */
                    if (ev1_input_status_expected)
                    {
                        /* an input status is expected */

                        InputEvent_InputStatus = ev1_input_status;
                        InputEvent_In1         = ev1_in1;
                        InputEvent_In2         = ev1_in2;
                        for (i = 0; i < 20; i++)
                            InputEvent_SmsPhoneNumber[i] = ev1_sms_pohone_number[i];
                        InputEvent_SmsPhoneNumber[20] = ev1_sms_pohone_number[20];

                        /* read inputs status */
                        if (InputEvent_InputEnabled_In1)
                            in1_status = DrvGpio_ReadInput(DRVGPIO__IN__IN1);
                        if (InputEvent_InputEnabled_In2)
                            in2_status = DrvGpio_ReadInput(DRVGPIO__IN__IN2);

                        if (InputEvent_InputEnabled_In1)
                        {
                            snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input IN1: %d", in1_status);
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                        if (InputEvent_InputEnabled_In2)
                        {
                            snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input IN2: %d", in2_status);
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }

                        /* check if IN1 input and IN2 input should be controlled */
                        if (InputEvent_In1)
                        {
                            /* IN1 input should be controlled */
                            if (InputEvent_InputEnabled_In1)
                            {
                                /* IN1 input is enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 is enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (in1_status == InputEvent_InputStatus)
                                {
                                    /* IN1 input has the expected status */

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 status is OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                    if (!InputEvent_TimerIn1)
                                    {
                                        /* start timer IN1 */
                                        InputEvent_TimerIn1_Start(TIME_MS__INPUT_STATUS_CHANGE, FALSE);
                                        InputEvent_TimerIn1 = TRUE;
                                    }
                                }
                                else
                                {
                                    /* IN1 input does not have the expected status */

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 status is not OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                //if (InputEvent_TimerIn1)
                                //{
                                //    /* stop timer IN1 */
                                //    InputEvent_TimerIn1_Stop();
                                //    InputEvent_TimerIn1 = FALSE;
                                //}
                                }
                            }
                            else
                            {
                                /* IN1 input is not enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 is not enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                            }
                        }
                        if (InputEvent_In2)
                        {
                            /* IN2 input should be controlled */
                            if (InputEvent_InputEnabled_In2)
                            {
                                /* IN2 input is enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (in2_status == InputEvent_InputStatus)
                                {
                                    /* IN2 input has the expected status */

                                    if (!InputEvent_TimerIn2)
                                    {
                                        /* start timer IN2 */
                                        InputEvent_TimerIn2_Start(TIME_MS__INPUT_STATUS_CHANGE, FALSE);
                                        InputEvent_TimerIn2 = TRUE;
                                    }

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 status is OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                                }
                                else
                                {
                                    /* IN2 input does not have the expected status */

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 status is not OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                //if (InputEvent_TimerIn2)
                                //{
                                //    /* stop timer IN2 */
                                //    InputEvent_TimerIn1_Stop();
                                //    InputEvent_TimerIn2 = FALSE;
                                //}
                                }
                            }
                            else
                            {
                                /* IN2 input is not enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is not enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                            }
                        }

                        /* start timer */
                        // TO DO

                        InputEvent_Status = STATUS__WAIT_INPUT_STATUS;
                    }
                    else
                    {
                        /* an input status is not expected */

                        // NOTHING TO DO

                        /* clear the variables */
                        InputEvent_InputStatus = FALSE;
                        InputEvent_In1         = FALSE;
                        InputEvent_In2         = FALSE;
                        for (i = 0; i < 20; i++)
                            InputEvent_SmsPhoneNumber[i] = 0x00;
                        InputEvent_SmsPhoneNumber[20] = 0x00;
                        InputEvent_In1Ok    = FALSE;
                        InputEvent_In2Ok    = FALSE;
                        InputEvent_TimerIn1 = FALSE;
                        InputEvent_TimerIn2 = FALSE;

                    //InputEvent_Status = STATUS__IDLE;
                    }
                    break;


                // event - new input event
                case TASK_MSG_ID__INPUT_EVENT:
                    // NOTHING TO DO
                    break;


                // event - timeout timer IN1
                // event - timeout timer IN2
                // event - timeout input status expected
                case TASK_MSG_ID__TIMEOUT_IN1:
                case TASK_MSG_ID__TIMEOUT_IN2:
                case TASK_MSG_ID__TIMEOUT_INPUT_STATUS_EXPECTED:
                    // NOT POSSIBLE (NOTHING TO DO)
                    break;


                // unknown event
                default:
                    break;
            }
            break;



        // status - wait for an input status
        case STATUS__WAIT_INPUT_STATUS:
            switch (msg_identifier)
            {
                // event - new input status expected
                case TASK_MSG_ID__INPUT_STATUS_EXPECTED:
                    /* extract the parameters */
                    ev1_input_status_expected = (bool)*(ptr_data    );
                    ev1_input_status          = (bool)*(ptr_data + 1);
                    ev1_in1                   = (bool)*(ptr_data + 2);
                    ev1_in2                   = (bool)*(ptr_data + 3);
                    for (i = 0; i <= 20; i++)
                        ev1_sms_pohone_number[i] = (ascii)*(ptr_data + 4 + i);

                    /* stop timer IN1 */
                    /* stop timer IN2 */
                    InputEvent_TimerIn1_Stop();
                    InputEvent_TimerIn2_Stop();
                    InputEvent_TimerIn1 = FALSE;
                    InputEvent_TimerIn2 = FALSE;

                    /* stop timer */
                    // TO DO

                    /* check if an input status is expected */
                    if (ev1_input_status_expected)
                    {
                        /* an input status is expected */

                        InputEvent_InputStatus = ev1_input_status;
                        InputEvent_In1         = ev1_in1;
                        InputEvent_In2         = ev1_in2;
                        for (i = 0; i < 20; i++)
                            InputEvent_SmsPhoneNumber[i] = ev1_sms_pohone_number[i];
                        InputEvent_SmsPhoneNumber[20] = ev1_sms_pohone_number[20];

                        /* read inputs status */
                        if (InputEvent_InputEnabled_In1)
                            in1_status = DrvGpio_ReadInput(DRVGPIO__IN__IN1);
                        if (InputEvent_InputEnabled_In2)
                            in2_status = DrvGpio_ReadInput(DRVGPIO__IN__IN2);

                        if (InputEvent_InputEnabled_In1)
                        {
                            snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input IN1: %d", in1_status);
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                        if (InputEvent_InputEnabled_In2)
                        {
                            snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input IN2: %d", in2_status);
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }

                        /* check if IN1 input and IN2 input should be controlled */
                        if (InputEvent_In1)
                        {
                            /* IN1 input should be controlled */

                            if (InputEvent_InputEnabled_In1)
                            {
                                /* IN1 input is enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 is enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (in1_status == InputEvent_InputStatus)
                                {
                                    /* IN1 input has the expected status */

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 status is OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                    if (!InputEvent_TimerIn1)
                                    {
                                        /* start timer IN1 */
                                        InputEvent_TimerIn1_Start(TIME_MS__INPUT_STATUS_CHANGE, FALSE);
                                        InputEvent_TimerIn1 = TRUE;
                                    }
                                }
                                else
                                {
                                    /* IN1 input does not have the expected status */

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 status is not OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                    if (InputEvent_TimerIn1)
                                    {
                                        /* stop timer IN1 */
                                        InputEvent_TimerIn1_Stop();
                                        InputEvent_TimerIn1 = FALSE;
                                    }
                                }
                            }
                            else
                            {
                                /* IN1 input is not enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 is not enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                            }
                        }
                        if (InputEvent_In2)
                        {
                            /* IN2 input should be controlled */

                            if (InputEvent_InputEnabled_In2)
                            {
                                /* IN2 input is enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (in2_status == InputEvent_InputStatus)
                                {
                                    /* IN2 input has the expected status */

                                    if (!InputEvent_TimerIn2)
                                    {
                                        /* start timer IN2 */
                                        InputEvent_TimerIn2_Start(TIME_MS__INPUT_STATUS_CHANGE, FALSE);
                                        InputEvent_TimerIn2 = TRUE;
                                    }

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 status is OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                                }
                                else
                                {
                                    /* IN2 input does not have the expected status */

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 status is not OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                    if (InputEvent_TimerIn2)
                                    {
                                        /* stop timer IN2 */
                                        InputEvent_TimerIn1_Stop();
                                        InputEvent_TimerIn2 = FALSE;
                                    }
                                }
                            }
                            else
                            {
                                /* IN2 input is not enabled */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is not enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                            }
                        }

                        //InputEvent_Status = STATUS__WAIT_INPUT_STATUS;
                    }
                    else
                    {
                        /* an input status is not expected */

                        /* clear the variables */
                        InputEvent_InputStatus = FALSE;
                        InputEvent_In1         = FALSE;
                        InputEvent_In2         = FALSE;
                        for (i = 0; i < 20; i++)
                            InputEvent_SmsPhoneNumber[i] = 0x00;
                        InputEvent_SmsPhoneNumber[20] = 0x00;
                        InputEvent_In1Ok    = FALSE;
                        InputEvent_In2Ok    = FALSE;
                        InputEvent_TimerIn1 = FALSE;
                        InputEvent_TimerIn2 = FALSE;

                        InputEvent_Status = STATUS__IDLE;
                    }
                    break;


                // event - new input event
                case TASK_MSG_ID__INPUT_EVENT:
                    /* extract the parameters */
                    ev2_input_id     =       *(ptr_data    );   // not used
                    ev2_input_status = (bool)*(ptr_data + 1);   // not used

                    snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input id: %d, input_status: %d", ev2_input_id, ev2_input_status);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* read inputs status */
                    if (InputEvent_InputEnabled_In1)
                        in1_status = DrvGpio_ReadInput(DRVGPIO__IN__IN1);
                    if (InputEvent_InputEnabled_In2)
                        in2_status = DrvGpio_ReadInput(DRVGPIO__IN__IN2);

                    if (InputEvent_InputEnabled_In1)
                    {
                        snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input IN1: %d", in1_status);
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }
                    if (InputEvent_InputEnabled_In2)
                    {
                        snprintf(InputEvent_DebugString, sizeof(InputEvent_DebugString), "Input IN2: %d", in2_status);
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, InputEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }

                    /* check if IN1 input and IN2 input should be controlled */
                    if (InputEvent_In1)
                    {
                        /* IN1 input should be controlled */

                        if (InputEvent_InputEnabled_In1)
                        {
                            /* IN1 input is enabled */

                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 is enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            if (in1_status == InputEvent_InputStatus)
                            {
                                /* IN1 input has the expected status */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 status is OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (!InputEvent_TimerIn1)
                                {
                                    /* start timer IN1 */
                                    InputEvent_TimerIn1_Start(TIME_MS__INPUT_STATUS_CHANGE, FALSE);
                                    InputEvent_TimerIn1 = TRUE;
                                }
                            }
                            else
                            {
                                /* IN1 input does not have the expected status */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 status is not OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (InputEvent_TimerIn1)
                                {
                                    /* stop timer IN1 */
                                    InputEvent_TimerIn1_Stop();
                                    InputEvent_TimerIn1 = FALSE;
                                }

                                InputEvent_In1Ok = FALSE;
                            }
                        }
                        else
                        {
                            /* IN2 input is not enabled */

                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is not enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                    }
                    if (InputEvent_In2)
                    {
                        /* IN2 input should be controlled */

                        if (InputEvent_InputEnabled_In2)
                        {
                            /* IN2 input is enabled */

                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            if (in2_status == InputEvent_InputStatus)
                            {
                                /* IN2 input has the expected status */

                                if (!InputEvent_TimerIn2)
                                {
                                    /* start timer IN2 */
                                    InputEvent_TimerIn2_Start(TIME_MS__INPUT_STATUS_CHANGE, FALSE);
                                    InputEvent_TimerIn2 = TRUE;
                                }

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 status is OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                            }
                            else
                            {
                                /* IN2 input does not have the expected status */

                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 status is not OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                if (InputEvent_TimerIn2)
                                {
                                    /* stop timer IN2 */
                                    InputEvent_TimerIn2_Stop();
                                    InputEvent_TimerIn2 = FALSE;
                                }

                                InputEvent_In2Ok = FALSE;
                            }
                        }
                        else
                        {
                            /* IN2 input is not enabled */

                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN2 is not enabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                    }
                    break;


                // event - timeout timer IN1
                case TASK_MSG_ID__TIMEOUT_IN1:
                    InputEvent_TimerIn1 = FALSE;

                    InputEvent_In1Ok = TRUE;

                    /* check if IN1 input and/or IN2 input have the expected status */
                    if (
                         ( (( InputEvent_In1) && (InputEvent_In1Ok)) &&
                           ((!InputEvent_In2)                      ) )  ||

                         ( ((!InputEvent_In1)                      ) &&
                           (( InputEvent_In2) && (InputEvent_In2Ok)) )  ||

                         ( (( InputEvent_In1) && (InputEvent_In1Ok)) &&
                           (( InputEvent_In2) && (InputEvent_In2Ok)) )
                       )
                    {
                        /* IN1 input and/or IN2 input have the expected status */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 and input IN2 status are OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* stop timer */
                        // TO DO

                        if (!InputEvent_InputStatus)
                        {
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Generate SMS answer event for DETACHMENT command", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            /* generate SMS answer event for DETACHMENT command */
                            SmsAnswerEvent_GenerateSmsAnswerEvent(InputEvent_SmsPhoneNumber, SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT);
                        }
                        else
                        {
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Generate SMS answer event for RESTORE command"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            /* generate SMS answer event for RESTORE    command */
                            SmsAnswerEvent_GenerateSmsAnswerEvent(InputEvent_SmsPhoneNumber, SMS_ANSWER_EVENT__ANSWER_TYPE__RESTORE   );
                        }

                        /* clear the variables */
                        InputEvent_InputStatus = FALSE;
                        InputEvent_In1         = FALSE;
                        InputEvent_In2         = FALSE;
                        for (i = 0; i < 20; i++)
                            InputEvent_SmsPhoneNumber[i] = 0x00;
                        InputEvent_SmsPhoneNumber[20] = 0x00;
                        InputEvent_In1Ok    = FALSE;
                        InputEvent_In2Ok    = FALSE;
                        InputEvent_TimerIn1 = FALSE;
                        InputEvent_TimerIn2 = FALSE;

                        InputEvent_Status = STATUS__IDLE;
                    }
                    else
                    {
                        /* IN1 input and/or IN2 input do not have the expected status */

                        // NOTHING TO DO
                    }
                    break;


                // event - timeout timer IN2
                case TASK_MSG_ID__TIMEOUT_IN2:
                    InputEvent_TimerIn2 = FALSE;

                    InputEvent_In2Ok = TRUE;

                    /* check if IN1 input and/or IN2 input have the expected status */
                    if (
                         ( (( InputEvent_In1) && (InputEvent_In1Ok)) &&
                           ((!InputEvent_In2)                      ) )  ||

                         ( ((!InputEvent_In1)                      ) &&
                           (( InputEvent_In2) && (InputEvent_In2Ok)) )  ||

                         ( (( InputEvent_In1) && (InputEvent_In1Ok)) &&
                           (( InputEvent_In2) && (InputEvent_In2Ok)) )
                       )
                    {
                        /* IN1 input and/or IN2 input have the expected status */

                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Input IN1 and input IN2 status are OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        /* stop timer */
                        // TO DO

                        if (!InputEvent_InputStatus)
                        {
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Generate SMS answer event for DETACHMENT command", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            /* generate SMS answer event for DETACHMENT command */
                            SmsAnswerEvent_GenerateSmsAnswerEvent(InputEvent_SmsPhoneNumber, SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT);
                        }
                        else
                        {
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Generate SMS answer event for RESTORE command"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            /* generate SMS answer event for RESTORE    command */
                            SmsAnswerEvent_GenerateSmsAnswerEvent(InputEvent_SmsPhoneNumber, SMS_ANSWER_EVENT__ANSWER_TYPE__RESTORE   );
                        }

                        /* clear the variables */
                        InputEvent_InputStatus = FALSE;
                        InputEvent_In1         = FALSE;
                        InputEvent_In2         = FALSE;
                        for (i = 0; i < 20; i++)
                            InputEvent_SmsPhoneNumber[i] = 0x00;
                        InputEvent_SmsPhoneNumber[20] = 0x00;
                        InputEvent_In1Ok    = FALSE;
                        InputEvent_In2Ok    = FALSE;
                        InputEvent_TimerIn1 = FALSE;
                        InputEvent_TimerIn2 = FALSE;

                        InputEvent_Status = STATUS__IDLE;
                    }
                    else
                    {
                        /* IN1 input and/or IN2 input do not have the expected status */

                        // NOTHING TO DO
                    }
                    break;


                // event - timeout input status expected
                case TASK_MSG_ID__TIMEOUT_INPUT_STATUS_EXPECTED:
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "Expired timeout input status expected", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* stop timer IN1 */
                    /* stop timer IN2 */
                    InputEvent_TimerIn1_Stop();
                    InputEvent_TimerIn2_Stop();
                    InputEvent_TimerIn1 = FALSE;
                    InputEvent_TimerIn2 = FALSE;

                    /* clear the variables */
                    InputEvent_InputStatus = FALSE;
                    InputEvent_In1         = FALSE;
                    InputEvent_In2         = FALSE;
                    for (i = 0; i < 20; i++)
                        InputEvent_SmsPhoneNumber[i] = 0x00;
                    InputEvent_SmsPhoneNumber[20] = 0x00;
                    InputEvent_In1Ok    = FALSE;
                    InputEvent_In2Ok    = FALSE;
                    InputEvent_TimerIn1 = FALSE;
                    InputEvent_TimerIn2 = FALSE;

                    InputEvent_Status = STATUS__IDLE;
                    break;


                // unknown event
                default:
                    break;
            }
            break;
    }



    update_var_crc = FALSE;

    if (
        /* status */
        (InputEvent_Status      != old_status               ) ||

        /* status */
        (InputEvent_InputStatus != old_input_status         ) ||
        (InputEvent_In1         != old_in1                  ) ||
        (InputEvent_In2         != old_in2                  ) ||

        /* indication of expected status for IN1 input and IN2 input */
        (InputEvent_In1Ok       != old_input_event_in1_ok   ) ||
        (InputEvent_In2Ok       != old_input_event_in2_ok   ) ||

        /* indication of status of timer IN1 and timer IN2 */
        (InputEvent_TimerIn1    != old_input_event_timer_in1) ||
        (InputEvent_TimerIn2    != old_input_event_timer_in2)
    )
    {
        update_var_crc = TRUE;
    }

    /* SMS phone number */
    for (i = 0; i < (20 + 1); i++)
    {
        if (InputEvent_SmsPhoneNumber[i] != old_sms_phone_number[i])
            update_var_crc = TRUE;
    }

    if (update_var_crc)
        InputEvent_UpdateVarCrc();
}




/*=============================================================================
 * Function   : InputEvent_AdlCallback_Timer_TimerIn1
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void InputEvent_AdlCallback_Timer_TimerIn1(void *context)
{
    INPUT_EVENT__MAIL message_to_be_sent;
    QlOSStatus        err;

    u8                i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - IN1", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    message_to_be_sent.event       = TASK_MSG_ID__TIMEOUT_IN1;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < INPUT_EVENT__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(InputEvent_MessageQueueHandler_MessageQueueEvents, sizeof(INPUT_EVENT__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : InputEvent_AdlCallback_Timer_TimerIn2
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void InputEvent_AdlCallback_Timer_TimerIn2(void *context)
{
    INPUT_EVENT__MAIL message_to_be_sent;
    QlOSStatus        err;

    u8                i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ALARM_TMIN, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - IN2", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    message_to_be_sent.event       = TASK_MSG_ID__TIMEOUT_IN2;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < INPUT_EVENT__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(InputEvent_MessageQueueHandler_MessageQueueEvents, sizeof(INPUT_EVENT__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_INPUT_EVENT, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
