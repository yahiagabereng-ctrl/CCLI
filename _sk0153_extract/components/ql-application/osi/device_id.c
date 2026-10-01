/*=============================================================================
 * File       :  DEVICE_ID.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - device ID
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDE
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "device_id.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "device ID" */
static       DEVICE_ID__CONFIG__DEVICE_ID DeviceIdId_Config_DeviceIdId;
static const DEVICE_ID__CONFIG__DEVICE_ID DeviceIdId_Config_DeviceIdIdDefault =
{
    ""   // device ID
};




/*===========================================================================
 * FUNCTION PROTOTYPES
 *===========================================================================*/
/* get/set "device ID" configuration */
void DeviceId_Config_DeviceId_GetDefault(DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);
void DeviceId_Config_DeviceId_Get       (DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);
void DeviceId_Config_DeviceId_Set       (DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);
bool DeviceId_Config_DeviceId_IsValid   (DEVICE_ID__CONFIG__DEVICE_ID *ptr_data);




/*===========================================================================
 * Function   : DeviceId_Config_DeviceId_GetDefault
 *
 * Description: get the default "device ID" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DeviceId_Config_DeviceId_GetDefault(DEVICE_ID__CONFIG__DEVICE_ID *ptr_data)
{
    *ptr_data = DeviceIdId_Config_DeviceIdIdDefault;
}




/*===========================================================================
 * Function   : DeviceId_Config_DeviceId_Get
 *
 * Description: get the "device ID" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DeviceId_Config_DeviceId_Get(DEVICE_ID__CONFIG__DEVICE_ID *ptr_data)
{
    *ptr_data = DeviceIdId_Config_DeviceIdId;
}




/*===========================================================================
 * Function   : DeviceId_Config_DeviceId_Set
 *
 * Description: set the "device ID" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DeviceId_Config_DeviceId_Set(DEVICE_ID__CONFIG__DEVICE_ID *ptr_data)
{
    DeviceIdId_Config_DeviceIdId = *ptr_data;
}




/*===========================================================================
 * Function   : DeviceId_Config_DeviceId_IsValid
 *
 * Description: check if the "device ID" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DeviceId_Config_DeviceId_IsValid(DEVICE_ID__CONFIG__DEVICE_ID*ptr_data)
{
    if (strlen(ptr_data->device_id) > DEVICE_ID__MAX_LENGTH_DEVICE_ID)
        return FALSE;

    return TRUE;
}
