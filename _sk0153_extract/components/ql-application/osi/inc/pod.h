/*=============================================================================
 * File       :  POD.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - POD
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __POD_H__


#define __POD_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* maximum POD string length */
#define POD__MAX_LENGTH_POD    40




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - POD */
typedef struct
{
    ascii pod[POD__MAX_LENGTH_POD + 1];    /* POD string */
} POD__CONFIG__POD;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "POD" configuration */
void Pod_Config_Pod_GetDefault(POD__CONFIG__POD *ptr_data);
void Pod_Config_Pod_Get       (POD__CONFIG__POD *ptr_data);
void Pod_Config_Pod_Set       (POD__CONFIG__POD *ptr_data);
bool Pod_Config_Pod_IsValid   (POD__CONFIG__POD *ptr_data);




#endif
