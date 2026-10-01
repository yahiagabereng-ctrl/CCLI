/*=============================================================================
 * File       :  LED.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - led manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __LED_H__


#define __LED_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* number of leds */
#define LED__NUMBER_OF_LEDS  1

/* leds */
#define LED__LED_GSM_GREEN   0    /* led GSM green */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Led_TaskLed(void *argument);

/* action on leds */
void Led_LedOn        (u8  led_id);
void Led_LedOff       (u8  led_id);
void Led_LedBlinkStart(u8  led_id,
                       u8  blink_num,
                       u32 led_on,
                       u32 led_off,
                       u8  seq_num,
                       u32 seq_period);




#endif
