/*=============================================================================
 * File       :  SYNCHRONIZE.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - clock synchronize
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __SYNCHRONIZE_H__


#define __SYNCHRONIZE_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* synchronize SMS counter */
typedef struct
{
    u16         sms_counter;       /* synchronize SMS counter */
} SYNCHRONIZE__SMS_COUNTER;


/* synchronize time */
typedef struct
{
    CLOCK__TIME synchronize_time;  /* synchronize time */
} SYNCHRONIZE__SYNCHRONIZE_TIME;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Synchronize_TaskSynchronize(void *argument);

/* get/set synchronize SMS counter */
void Synchronize_SmsCounter_GetDefault     (SYNCHRONIZE__SMS_COUNTER      *ptr_data);
void Synchronize_SmsCounter_Get            (SYNCHRONIZE__SMS_COUNTER      *ptr_data);
void Synchronize_SmsCounter_Set            (SYNCHRONIZE__SMS_COUNTER      *ptr_data);

/* get/set synchronize time */
void Synchronize_SynchronizeTime_GetDefault(SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data);
void Synchronize_SynchronizeTime_Get       (SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data);
void Synchronize_SynchronizeTime_Set       (SYNCHRONIZE__SYNCHRONIZE_TIME *ptr_data);

/* events from external */
void Synchronize_Event_SmsTx(SYNCHRONIZE__SMS_COUNTER sms_counter);
void Synchronize_Event_SmsRx(SYNCHRONIZE__SMS_COUNTER sms_counter, CLOCK__TIME *ptr_time_sms_tx);

/* synchronize */
void Synchronize_SynchronizeRtcTime(CLOCK__TIME *ptr_actual_time, bool synch_to_do);




#endif
