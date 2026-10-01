/*=============================================================================
 * File       :  ANALOG.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - analog
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ANALOG_H__


#define __ANALOG_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* analog modes */
#define ANALOG__ANALOG_MODE__TEMPERATURES   0   // analog mode - temperatures
#define ANALOG__ANALOG_MODE__ADC_VALUES     1   // analog mode - ADC values




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "analog mode" */
typedef struct
{
    u8 mode;    // analog mode
} ANALOG__CONFIG__ANALOG_MODE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "analog mode" configuration */
void Analog_Config_AnalogMode_GetDefault(ANALOG__CONFIG__ANALOG_MODE *ptr_data);
void Analog_Config_AnalogMode_Get       (ANALOG__CONFIG__ANALOG_MODE *ptr_data);
void Analog_Config_AnalogMode_Set       (ANALOG__CONFIG__ANALOG_MODE *ptr_data);
bool Analog_Config_AnalogMode_IsValid   (ANALOG__CONFIG__ANALOG_MODE *ptr_data);




#endif
