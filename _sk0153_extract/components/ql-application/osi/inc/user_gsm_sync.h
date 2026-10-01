/*=============================================================================
 * File       :  USER_GSM_SYNC.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - user GSM - time synchronize
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __USER_GSM_SYNC_H__


#define __USER_GSM_SYNC_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* random configuration - "synchronize" */
typedef struct
{
    bool values_generated;  // random values generated  [FALSE, TRUE]
    u32  offset;            // random time offset (sec) [0-86400]
} USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "synchronize" random configuration */
void UserGsmSync_RandomConfig_Synchronize_GetDefault (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
void UserGsmSync_RandomConfig_Synchronize_Get        (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
void UserGsmSync_RandomConfig_Synchronize_Set        (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
bool UserGsmSync_RandomConfig_Synchronize_IsValid    (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
void UserGsmSync_RandomConfig_Synchronize_Generate   (void);
bool UserGsmSync_RandomConfig_Synchronize_IsGenerated(void);

/* the nearest user wakeup time */
bool UserGsmSync_NextWakeupTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time);

/* update data */
void UserGsmSync_UpdateData(void);




#endif
