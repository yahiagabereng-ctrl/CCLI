/*=============================================================================
 * File       :  TRANSMISSION_GPRS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - GPRS transmission management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __TRANSMISSION_GPRS_H__


#define __TRANSMISSION_GPRS_H__




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* transmission task */
void TransmissionGprs_TaskTransmissionGprs(void *argument);

/* info */
bool TransmissionGprs_IsTransmissionTerminated(void);

/* msg to be transmitted */
bool TransmissionGprs_ExistMsgToBeTransmitted (void);

/* data to be transmitted */
bool TransmissionGprs_ExistDataToBeTransmitted(void);

/* msg to be transmitted */
void TransmissionGprs_SmsAnswerToCrsToBeTxed(void);

/* msg to be transmitted */
void TransmissionGprs_Init1ToBeTxed(void);

/*---------------------------------------------------------------------------
 * Events for SEM from external
 *---------------------------------------------------------------------------*/
void TransmissionGprs_Event_DataTxForce            (void);
void TransmissionGprs_Event_DataTx                 (void);
void TransmissionGprs_Event_GsmNetworkRegistered   (void);
void TransmissionGprs_Event_GsmNetworkNotRegistered(void);
void TransmissionGprs_Event_ApnConnectionOk        (void);
void TransmissionGprs_Event_ApnConnectionFail      (void);
void TransmissionGprs_Event_HttpGetRxOk            (void);
void TransmissionGprs_Event_HttpGetRxFail          (void);
void TransmissionGprs_Event_HttpGetTxOk            (s32 command_id);
void TransmissionGprs_Event_HttpGetTxFail          (void);
void TransmissionGprs_Event_HttpPostOk             (void);
void TransmissionGprs_Event_HttpPostFail           (int evt_code);
void TransmissionGprs_Event_ApnDisconnectionOk     (void);
void TransmissionGprs_Event_ApnDisconnectionFail   (void);
void TransmissionGprs_Event_ApnConnectionDown      (void);




#endif
