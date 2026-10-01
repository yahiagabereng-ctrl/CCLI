/*===========================================================================
 * File       :  DRVBATTERY.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - battery voltage driver
 *
 * Copyright Shitek Technology SRL (c) 2010
 *===========================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdio.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_power.h"

/* user     includes */
#include "debug_my.h"
#include "drvbattery.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING   200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* battery voltage (mV) */
static u16   DrvBattery_BatteryVoltage = 0;

/* debug string */
static ascii DrvBattery_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get battery voltage */
bool DrvBattery_GetBatteryVoltage(u16 *ptr_battery_voltage);




/*=============================================================================
 * Function   : DrvBattery_GetBatteryVoltage
 *
 * Description: get the actual battery voltage
 * Input      : - ptr_battery_voltage: pointer to save the battery voltage (mV)
 * Output     : - FALSE: battery voltage not got
 *              - TRUE : battery voltage     got
 *=============================================================================*/
bool DrvBattery_GetBatteryVoltage(u16 *ptr_battery_voltage)
{
    u32               adc_mv_value;

    ql_errcode_charge result;


    result = ql_get_battery_vol(&adc_mv_value);
    if (result != QL_CHARGE_SUCCESS)
    {
        DrvBattery_BatteryVoltage = 0;
        *ptr_battery_voltage      = 0;
        return FALSE;
    }

    DrvBattery_BatteryVoltage = adc_mv_value;
    *ptr_battery_voltage      = DrvBattery_BatteryVoltage;

    snprintf(DrvBattery_DebugString, sizeof(DrvBattery_DebugString), "DrvBattery_BatteryVoltage: %d", DrvBattery_BatteryVoltage);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_BATTERY, DEBUG_TRACE_TYPE_LOW, DrvBattery_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return TRUE;
}
