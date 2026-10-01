/*=============================================================================
 * File       :  SMS_ANSWER_EVENT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMS answer event
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __SMS_ANSWER_EVENT_H__


#define __SMS_ANSWER_EVENT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "variables.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* SMS answer event */
#define SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT   0   // SMS answer event - answer event for DETACHMENT command
#define SMS_ANSWER_EVENT__ANSWER_TYPE__RESTORE      1   // SMS answer event - answer event for RESTORE    command
#define SMS_ANSWER_EVENT__ANSWER_TYPE__STATUS       2   // SMS answer event - answer event for STATUS     command
#define SMS_ANSWER_EVENT__ANSWER_TYPE__RESET        3   // SMS answer event - answer event for RESET      command




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* SMS answer event */
typedef struct
{
    /* phone number */
    ascii                       sms_phone_number[20 + 1];      /* SMS recipient phone number */

    /* variables values */
    VARIABLES__VARIABLES_VALUES variables_values;              /* variables values */

    /* answer type */
    u8                          answer_type;                   /* answer type */
} SMS_ANSWER_EVENT__SMS_ANSWER_EVENT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* new SMS answer event record */
bool SmsAnswerEvent_GenerateSmsAnswerEvent(ascii *sms_phone_number, u8 answer_type);




#endif
