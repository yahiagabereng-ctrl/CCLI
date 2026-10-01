/*=============================================================================
 * File       :  DEVICE_ID.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - device ID
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DEVICE_ID_H__


#define __DEVICE_ID_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* maximum device ID  string length */
#define DEVICE_ID__MAX_LENGTH_DEVICE_ID      20     /* device ID */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "device ID" */
typedef struct
{
    ascii device_id[DEVICE_ID__MAX_LENGTH_DEVICE_ID + 1];
} DEVICE_ID__CONFIG__DEVICE_ID;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "device ID" configuration */
void DeviceId_Config_DeviceId_GetDefault(DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);
void DeviceId_Config_DeviceId_Get       (DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);
void DeviceId_Config_DeviceId_Set       (DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);
bool DeviceId_Config_DeviceId_IsValid   (DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);




#endif
