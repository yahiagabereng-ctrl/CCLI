/*=============================================================================
 * File       :  USER_GSM_RETX.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - user GSM - retransmission
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __USER_GSM_RETX_H__


#define __USER_GSM_RETX_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* random configuration - "retransmission" */
typedef struct
{
    bool values_generated;  // random values generated  [FALSE, TRUE]
    u32  offset;            // random time offset (sec) [0-86400]
} USER_GSM_RETX__RANDOM_CONFIG__RETX;


/* retransmission status */
typedef struct
{
    bool status;            // status [FALSE, TRUE]
    u8   counter;           // counter
} USER_GSM_RETX__RETX_STATUS;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "retransmission" random configuration */
void UserGsmRetx_RandomConfig_Retx_GetDefault   (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
void UserGsmRetx_RandomConfig_Retx_Get          (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
void UserGsmRetx_RandomConfig_Retx_Set          (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
bool UserGsmRetx_RandomConfig_Retx_IsValid      (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
void UserGsmRetx_RandomConfig_Retx_Generate     (void);
bool UserGsmRetx_RandomConfig_Retx_IsGenerated  (void);

/* get/set retransmission status */
void UserGsmRetx_RetransmissionStatus_GetDefault(USER_GSM_RETX__RETX_STATUS         *ptr_data);
void UserGsmRetx_RetransmissionStatus_Get       (USER_GSM_RETX__RETX_STATUS         *ptr_data);
void UserGsmRetx_RetransmissionStatus_Set       (USER_GSM_RETX__RETX_STATUS         *ptr_data);

/* the nearest user wakeup time */
bool UserGsmRetx_NextWakeupTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time);

/* udpate data */
void UserGsmRetx_UpdateData(void);

/* reset retransmission status */
void UserGsmRetx_ResetRetransmissionStatus(void);

/* debug info */
void UserGsmRetx_PrintRetransmissionStatus(void);




#endif
