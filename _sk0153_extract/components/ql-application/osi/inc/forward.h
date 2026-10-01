/*=============================================================================
 * File       :  FORWARD.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - report
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __FORWARD_H__


#define __FORWARD_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "forward" */
typedef struct
{
    bool status;   /* forward status */
} FORWARD__CONFIG__FORWARD;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "forward" configuration */
void Forward_Config_Forward_GetDefault(FORWARD__CONFIG__FORWARD *ptr_data);
void Forward_Config_Forward_Get       (FORWARD__CONFIG__FORWARD *ptr_data);
void Forward_Config_Forward_Set       (FORWARD__CONFIG__FORWARD *ptr_data);
bool Forward_Config_Forward_IsValid   (FORWARD__CONFIG__FORWARD *ptr_data);




#endif
