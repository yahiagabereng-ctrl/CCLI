/*=============================================================================
 * File       :  INPUT_EVENT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - input event manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __INPUT_EVENT_H__


#define __INPUT_EVENT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void InputEvent_TaskInputEvent(void *argument);

/* status variables */
void InputEvent_UpdateVarCrc(void);
bool InputEvent_VerifyVarCrc(void);

/* task events */
void InputEvent_Event_InputStatusExpected(bool input_status_expected, bool input_status, bool in1, bool in2, ascii *sms_phone_number);




#endif
