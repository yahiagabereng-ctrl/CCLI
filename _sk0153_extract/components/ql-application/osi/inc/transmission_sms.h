/*=============================================================================
 * File       :  TRANSMISSION_SMS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMS transmission management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __SMS_TRANSMISSION_H__


#define __SMS_RANSMISSION_H__




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* SMS transmission task */
void TransmissionSms_TaskTransmissionSms(void *argument);

/* info */
bool TransmissionSms_IsTransmissionTerminated(void);

/* data to be transmitted */
bool TransmissionSms_ExistDataToBeTransmitted(void);

/*---------------------------------------------------------------------------
 * Events for SEM from external
 *---------------------------------------------------------------------------*/
void TransmissionSms_Event_DataTx                 (void);
void TransmissionSms_Event_GsmNetworkRegistered   (void);
void TransmissionSms_Event_GsmNetworkNotRegistered(void);
void TransmissionSms_Event_SmsTxOk                (void);
void TransmissionSms_Event_SmsTxFail              (void);




#endif
