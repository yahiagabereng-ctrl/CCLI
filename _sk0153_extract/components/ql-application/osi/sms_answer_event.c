/*=============================================================================
 * File       :  SMS_ANSWER_EVENT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMS answer event
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdio.h>
#include <string.h>

/* user     includes */
#include "debug_my.h"
#include "phone.h"
#include "pod.h"
#include "queue_my.h"
#include "sms_answer_event.h"
#include "status.h"
#include "typedef.h"
#include "variables.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING    200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static ascii SmsAnswerEvent_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* new SMS answer event record */
bool SmsAnswerEvent_GenerateSmsAnswerEvent(ascii *sms_phone_number, u8 answer_type);




/*===========================================================================
 * Function   : SmsAnswerEvent_GenerateSmsAnswerEvent
 *
 * Description: generate a SMS answer event record
 * Input      : - sms_phone_number: pointer to phone number
 *              - answer_type     : answer type
 * Output     : - FALSE: SMS answer event record not generated and not put in the queue
 *              - TRUE : SMS answer event record     generated and     put in the queue
 *===========================================================================*/
bool SmsAnswerEvent_GenerateSmsAnswerEvent(ascii *sms_phone_number, u8 answer_type)
{
    /* SMS answer event */
    static SMS_ANSWER_EVENT__SMS_ANSWER_EVENT sms_answer_event;

    /* variables values */
    static VARIABLES__VARIABLES_VALUES        variables_values;

    /* values */
    static STATUS__DEVICE_STATUS              device_status;    /* device status  */
    static POD__CONFIG__POD                   config_pod;       /* configurations */
    static PHONE__PHONE_STATUS                phone_status;     /* phone  status  */
    static u8                                 reset_result;     /* reset result   */

           bool                               result;


    snprintf(SmsAnswerEvent_DebugString, sizeof(SmsAnswerEvent_DebugString), "New SMS answer event record - answer type: %d, SMS phone number: \"%s\"", answer_type, sms_phone_number);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMS_ANSWER_EVENT, DEBUG_TRACE_TYPE_LOW, SmsAnswerEvent_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* verify alarm type value */
    if (
         (answer_type != SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT) &&
         (answer_type != SMS_ANSWER_EVENT__ANSWER_TYPE__RESTORE   ) &&
         (answer_type != SMS_ANSWER_EVENT__ANSWER_TYPE__STATUS    ) &&
         (answer_type != SMS_ANSWER_EVENT__ANSWER_TYPE__RESET     )
       )
    {
        return FALSE;   // answer type value not valid
    }


    /* read the device status */
    result = Status_ReadDeviceStatus(&device_status);

    /* get the "POD" configuration */
    Pod_Config_Pod_Get(&config_pod);

    /* get the phone status */
    Phone_PhoneStatus_Get(&phone_status);

    /* reset result */
    reset_result = 0;  //@@@


    /* build the variables */
    Variables_BuildVariables(&variables_values, &device_status, &phone_status, &config_pod, &reset_result);


    /* build the SMS answer event */

    /* SMS recipient phone number */
    strncpy(sms_answer_event.sms_phone_number, sms_phone_number, 20);
    sms_answer_event.sms_phone_number[20] = 0x00;

    /* variables values */
    sms_answer_event.variables_values = variables_values;

    /* answer type */
    sms_answer_event.answer_type = answer_type;


    /* put the SMS answer event record in the SMS answer event queue */
    result = Queue_SmsAnswerEvent_PutRecord((QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD *)&sms_answer_event);


    return result;
}
