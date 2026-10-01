/*=============================================================================
 * File       :  CHARGER.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - battery charger
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __CHARGER_H__


#define __CHARGER_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Charger_TaskCharger(void *argument);

/* status variables */
void Charger_UpdateVarCrc(void);
bool Charger_VerifyVarCrc(void);

/*-----------------------------------------------------------------------------
 * set/get charger parameters
 *-----------------------------------------------------------------------------*/
void Charger_GetTimeChargeOn                 (u32 *ptr_timeout_value);
void Charger_SetTimeChargeOn                 (u32 *ptr_timeout_value);
bool Charger_IsValidTimeChargeOn             (u32 *ptr_timeout_value);

void Charger_GetTimeChargeOff                (u32 *ptr_timeout_value);
void Charger_SetTimeChargeOff                (u32 *ptr_timeout_value);
bool Charger_IsValidTimeChargeOff            (u32 *ptr_timeout_value);

void Charger_GetTimeChargeOffAfterPowerOn    (u32 *ptr_timeout_value);
void Charger_SetTimeChargeOffAfterPowerOn    (u32 *ptr_timeout_value);
bool Charger_IsValidTimeChargeOffAfterPowerOn(u32 *ptr_timeout_value);




#endif
