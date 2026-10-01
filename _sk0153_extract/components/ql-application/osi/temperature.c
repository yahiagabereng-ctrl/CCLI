/*=============================================================================
 * File       :  TEMPERATURE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - temperature
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 * A temperature is valid if it is in the range:
 *     [-35.0 - +105.0 �C]
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "debug_my.h"
#include "fw_config.h"
#include "temperature.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* minimum/maximum temperatures (/10 �C) */
#define TEMPERATURE_MIN    ( -350)   // minimum temperature (/10 �C)
#define TEMPERATURE_MAX    (+1050)   // maximum temperature (/10 �C)




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
bool Temperature_IsValidTemperature(s16 temperature);




/*===========================================================================
 * Function   : Temperature_IsValidTemperature
 *
 * Description: verify if a temperature is valid temperature
 *              A temperature is valid if it is in the range [-35.0 - +105.0 �C]
 * Input      : - temperature: temperature (/10 �C)
 * Output     : - FALSE: the temperature is     valid
 *              - TRUE : the temperature is not valid
 *===========================================================================*/
bool Temperature_IsValidTemperature(s16 temperature)
{
    if (
         (temperature >= TEMPERATURE_MIN) &&
         (temperature <= TEMPERATURE_MAX)
       )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
