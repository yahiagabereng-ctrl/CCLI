/*=============================================================================
 * File       :  CALENDAR.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - calendar
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __CALENDAR_H__


#define __CALENDAR_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Calendar_TaskCalendar(void *argument);

/* status variables */
void Calendar_UpdateVarCrc(void);
bool Calendar_VerifyVarCrc(void);

/* events */
void Calendar_Event_AutoSynchronize(void);
void Calendar_Event_SummerTime     (void);
void Calendar_Event_ClockChanged   (void);




#endif
