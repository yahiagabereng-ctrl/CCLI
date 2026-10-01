/*=============================================================================
 * File       :  DRVBATTERY.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - battery voltage driver
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DRVBATTERY_H__


#define __DRVBATTERY_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get battery voltage */
bool DrvBattery_GetBatteryVoltage(u16 *ptr_battery_voltage);




#endif
