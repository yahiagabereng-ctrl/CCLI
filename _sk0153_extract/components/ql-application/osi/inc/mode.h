/*=============================================================================
 * File       :  MODE.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - temperature mode manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __MODE_H__


#define __MODE_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* temperature modes */
#define MODE__WINTER_MODE    0    /* "winter" temperature mode */
#define MODE__SUMMER_MODE    1    /* "summer" temperature mode */




/*=============================================================================
 * DATA TYPE
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - internal temperature mode */
typedef struct
{
    bool mode;   /* internal temperature mode */
} MODE__CONFIG__MODE_TEMPERATURE_INT;


/* configuration - external temperature mode */
typedef struct
{
    bool mode;   /* external temperature mode */
} MODE__CONFIG__MODE_TEMPERATURE_EXT;




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




#endif
