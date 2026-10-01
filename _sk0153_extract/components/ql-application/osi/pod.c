/*=============================================================================
 * File       :  POD.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - POD
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "pod.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - POD */
static       POD__CONFIG__POD Pod_Config_Pod;
static const POD__CONFIG__POD Pod_Config_PodDefault =
{
    "",    // POD string
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "POD" configuration */
void Pod_Config_Pod_GetDefault(POD__CONFIG__POD *ptr_data);
void Pod_Config_Pod_Get       (POD__CONFIG__POD *ptr_data);
void Pod_Config_Pod_Set       (POD__CONFIG__POD *ptr_data);
bool Pod_Config_Pod_IsValid   (POD__CONFIG__POD *ptr_data);




/*===========================================================================
 * Function   : Pod_Config_Pod_GetDefault
 *
 * Description: get the default "POD" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Pod_Config_Pod_GetDefault(POD__CONFIG__POD *ptr_data)
{
    *ptr_data = Pod_Config_PodDefault;
}




/*===========================================================================
 * Function   : Pod_Config_Pod_Get
 *
 * Description: get the "POD" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Pod_Config_Pod_Get(POD__CONFIG__POD *ptr_data)
{
    *ptr_data = Pod_Config_Pod;
}




/*===========================================================================
 * Function   : Pod_Config_Pod_Set
 *
 * Description: set the "POD" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Pod_Config_Pod_Set(POD__CONFIG__POD *ptr_data)
{
    Pod_Config_Pod = *ptr_data;
}




/*===========================================================================
 * Function   : Pod_Config_Pod_IsValid
 *
 * Description: check if the "POD" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Pod_Config_Pod_IsValid(POD__CONFIG__POD *ptr_data)
{
    if (strlen(ptr_data->pod) > POD__MAX_LENGTH_POD)
        return FALSE;

    return TRUE;
}
