/*=============================================================================
 * File       :  F_ANTIFROST.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "antifrost" function
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __F_ANTIFROST_H__


#define __F_ANTIFROST_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - internal "antifrost" function */
typedef struct
{
    bool status;        /* function status [FALSE/TRUE]                                                  */
    s16  temperature;   /* antifrost temperature (/10 °C) (valid only if "status" is TRUE) [0.0-18.0 °C] */
} F_ANTIFROST__CONFIG_F_ANTIFROST_INT;


/* configuration - external "antifrost" function */
typedef struct
{
    bool status;        /* function status [FALSE/TRUE]                                                  */
    s16  temperature;   /* antifrost temperature (/10 °C) (valid only if "status" is TRUE) [0.0-18.0 °C] */
} F_ANTIFROST__CONFIG_F_ANTIFROST_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void FAntifrost_TaskFAntifrost(void *argument);

/* task events */
void FAntifrost_NewConfigurationInt(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_config_new, F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_config_old);
void FAntifrost_NewConfigurationExt(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_config_new, F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_config_old);


/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set "internal antifrost function" configuration */
void FAntifrost__Config_FAntifrostInt_GetDefault(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);
void FAntifrost__Config_FAntifrostInt_Get       (F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);
void FAntifrost__Config_FAntifrostInt_Set       (F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);
bool FAntifrost__Config_FAntifrostInt_IsValid   (F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);

/* get/set "external antifrost function" configuration */
void FAntifrost__Config_FAntifrostExt_GetDefault(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);
void FAntifrost__Config_FAntifrostExt_Get       (F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);
void FAntifrost__Config_FAntifrostExt_Set       (F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);
bool FAntifrost__Config_FAntifrostExt_IsValid   (F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);




#endif
