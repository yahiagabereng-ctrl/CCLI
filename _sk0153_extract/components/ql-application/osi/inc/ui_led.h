/*=============================================================================
 * File       :  UI_LED.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - Led User Interface
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __UI_LED_H__


#define __UI_LED_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void UiLed_TaskUiLed(void *argument);

/* task event */
void UiLed_SignalSimProblemStart  (void);
void UiLed_SignalSimProblemStop   (void);

void UiLed_SignalGsmRegDeniedStart(void);
void UiLed_SignalGsmRegDeniedStop (void);

void UiLed_SignalSmsTxFailStart   (void);
void UiLed_SignalSmsTxFailStop    (void);

void UiLed_SignalGsmNotRegistered (void);
void UiLed_SignalGsmRegistered    (u8 rssi, u8 ber);




#endif
