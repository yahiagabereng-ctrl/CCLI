/*=============================================================================
 * File       :  FORWARD.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - forward
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "forward.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "forward" */
static       FORWARD__CONFIG__FORWARD Forward_Config_Forward;
static const FORWARD__CONFIG__FORWARD Forward_Config_ForwardDefault =
{
    TRUE
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "forward" configuration */
void Forward_Config_Forward_GetDefault(FORWARD__CONFIG__FORWARD *ptr_data);
void Forward_Config_Forward_Get       (FORWARD__CONFIG__FORWARD *ptr_data);
void Forward_Config_Forward_Set       (FORWARD__CONFIG__FORWARD *ptr_data);
bool Forward_Config_Forward_IsValid   (FORWARD__CONFIG__FORWARD *ptr_data);




/*===========================================================================
 * Function   : Forward_Config_Forward_GetDefault
 *
 * Description: get the default "forward" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Forward_Config_Forward_GetDefault(FORWARD__CONFIG__FORWARD *ptr_data)
{
    *ptr_data = Forward_Config_ForwardDefault;
}




/*===========================================================================
 * Function   : Forward_Config_Forward_Get
 *
 * Description: get the "forward" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Forward_Config_Forward_Get(FORWARD__CONFIG__FORWARD *ptr_data)
{
    *ptr_data = Forward_Config_Forward;
}




/*===========================================================================
 * Function   : Forward_Config_Forward_Set
 *
 * Description: set the "forward" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Forward_Config_Forward_Set(FORWARD__CONFIG__FORWARD *ptr_data)
{
    Forward_Config_Forward = *ptr_data;
}




/*===========================================================================
 * Function   : Forward_Config_Forward_IsValid
 *
 * Description: check if the "forward" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Forward_Config_Forward_IsValid(FORWARD__CONFIG__FORWARD *ptr_data)
{
    return TRUE;
}
