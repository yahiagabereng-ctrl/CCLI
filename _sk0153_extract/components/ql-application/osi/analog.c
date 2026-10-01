/*=============================================================================
 * File       :  ANALOG.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - analog
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "analog.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "alarm mode" */
static       ANALOG__CONFIG__ANALOG_MODE Analog_Config_AnalogMode;
static const ANALOG__CONFIG__ANALOG_MODE Analog_Config_AnalogModeDefault =
{
    ANALOG__ANALOG_MODE__TEMPERATURES     // analog mode
};





/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "analog mode" configuration */
void Analog_Config_AnalogMode_GetDefault(ANALOG__CONFIG__ANALOG_MODE *ptr_data);
void Analog_Config_AnalogMode_Get       (ANALOG__CONFIG__ANALOG_MODE *ptr_data);
void Analog_Config_AnalogMode_Set       (ANALOG__CONFIG__ANALOG_MODE *ptr_data);
bool Analog_Config_AnalogMode_IsValid   (ANALOG__CONFIG__ANALOG_MODE *ptr_data);




/*===========================================================================
 * Function   : Analog_Config_AnalogMode_GetDefault
 *
 * Description: get the default "analog mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Analog_Config_AnalogMode_GetDefault(ANALOG__CONFIG__ANALOG_MODE *ptr_data)
{
    *ptr_data = Analog_Config_AnalogModeDefault;
}




/*===========================================================================
 * Function   : Analog_Config_AnalogMode_Get
 *
 * Description: get the "analog mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Analog_Config_AnalogMode_Get(ANALOG__CONFIG__ANALOG_MODE *ptr_data)
{
    *ptr_data = Analog_Config_AnalogMode;
}




/*===========================================================================
 * Function   : Analog_Config_AnalogMode_Set
 *
 * Description: set the "analog mode" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Analog_Config_AnalogMode_Set(ANALOG__CONFIG__ANALOG_MODE *ptr_data)
{
    Analog_Config_AnalogMode = *ptr_data;
}




/*===========================================================================
 * Function   : Analog_Config_AnalogMode_IsValid
 *
 * Description: check if the "analog mode" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Analog_Config_AnalogMode_IsValid(ANALOG__CONFIG__ANALOG_MODE *ptr_data)
{
    if (
         (ptr_data->mode != ANALOG__ANALOG_MODE__TEMPERATURES) &&
         (ptr_data->mode != ANALOG__ANALOG_MODE__ADC_VALUES  )
       )
    {
        return FALSE;
    }

    return TRUE;
}
