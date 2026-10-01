/*=============================================================================
 * File       :  MODE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - mode manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "debug_my.h"
#include "mode.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
//#define MAX_LENGTH_DEBUG_STRING   200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
//static ascii                                    Mode_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - internal temperature mode */
static       MODE__CONFIG__MODE_TEMPERATURE_INT Mode_Config_ModeTemperatureInt;
static const MODE__CONFIG__MODE_TEMPERATURE_INT Mode_Config_ModeTemperatureIntDefault =
{
    MODE__WINTER_MODE   /* internal temperature mode */
};


/* configuration - external temperature mode */
static       MODE__CONFIG__MODE_TEMPERATURE_EXT Mode_Config_ModeTemperatureExt;
static const MODE__CONFIG__MODE_TEMPERATURE_EXT Mode_Config_ModeTemperatureExtDefault =
{
    MODE__WINTER_MODE   /* external temperature mode */
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature mode" configuration */
void Mode_Config_ModeTemperatureInt_GetDefault(MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data);
void Mode_Config_ModeTemperatureInt_Get       (MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data);
void Mode_Config_ModeTemperatureInt_Set       (MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data);
bool Mode_Config_ModeTemperatureInt_IsValid   (MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data);

/* get/set "external temperature mode" configuration */
void Mode_Config_ModeTemperatureExt_GetDefault(MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data);
void Mode_Config_ModeTemperatureExt_Get       (MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data);
void Mode_Config_ModeTemperatureExt_Set       (MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data);
bool Mode_Config_ModeTemperatureExt_IsValid   (MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data);




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureInt_GetDefault
 *
 * Description: get the default "internal temperature mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Mode_Config_ModeTemperatureInt_GetDefault(MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data)
{
    *ptr_data = Mode_Config_ModeTemperatureIntDefault;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureInt_Get
 *
 * Description: get the "internal temperature mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Mode_Config_ModeTemperatureInt_Get(MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data)
{
    *ptr_data = Mode_Config_ModeTemperatureInt;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureInt_Set
 *
 * Description: set the "internal temperature mode" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Mode_Config_ModeTemperatureInt_Set(MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data)
{
    Mode_Config_ModeTemperatureInt = *ptr_data;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureInt_IsValid
 *
 * Description: check if the "internal temperature mode" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Mode_Config_ModeTemperatureInt_IsValid(MODE__CONFIG__MODE_TEMPERATURE_INT *ptr_data)
{
    if (
         (ptr_data->mode != MODE__WINTER_MODE) &&
         (ptr_data->mode != MODE__SUMMER_MODE)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureExt_GetDefault
 *
 * Description: get the default "external temperature mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Mode_Config_ModeTemperatureExt_GetDefault(MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data)
{
    *ptr_data = Mode_Config_ModeTemperatureExtDefault;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureExt_Get
 *
 * Description: get the "external temperature mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Mode_Config_ModeTemperatureExt_Get(MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data)
{
    *ptr_data = Mode_Config_ModeTemperatureExt;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureExt_Set
 *
 * Description: set the "external temperature mode" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Mode_Config_ModeTemperatureExt_Set(MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data)
{
    Mode_Config_ModeTemperatureExt = *ptr_data;
}




/*===========================================================================
 * Function   : Mode_Config_ModeTemperatureExt_IsValid
 *
 * Description: check if the "external temperature mode" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Mode_Config_ModeTemperatureExt_IsValid(MODE__CONFIG__MODE_TEMPERATURE_EXT *ptr_data)
{
    if (
         (ptr_data->mode != MODE__WINTER_MODE) &&
         (ptr_data->mode != MODE__SUMMER_MODE)
       )
    {
        return FALSE;
    }

    return TRUE;
}
