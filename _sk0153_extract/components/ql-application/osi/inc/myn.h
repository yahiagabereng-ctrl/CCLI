/*=============================================================================
 * File       :  MYN.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - MYN (MY Number)
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __MYN_H__


#define __MYN_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* MYN phone number max length */
#define MYN__MAX_LEN_PHONE_NUMBER     20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "MYN phone number" */
typedef struct
{
    ascii myn_number[MYN__MAX_LEN_PHONE_NUMBER + 1];
} MYN__CONFIG__MYN_NUMBER;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set "MYN phone number" configuration */
void Myn_Config_MynNumber_GetDefault(MYN__CONFIG__MYN_NUMBER *ptr_data);
void Myn_Config_MynNumber_Get       (MYN__CONFIG__MYN_NUMBER *ptr_data);
void Myn_Config_MynNumber_Set       (MYN__CONFIG__MYN_NUMBER *ptr_data);
bool Myn_Config_MynNumber_IsValid   (MYN__CONFIG__MYN_NUMBER *ptr_data);

/*-----------------------------------------------------------------------------
 * Info
 *-----------------------------------------------------------------------------*/
/* number of MYN phone numbers */
u8   Myn_NumberOfMynNumbers(void);

/* verify phone number type */
bool Myn_IsMynNumber(ascii *ptr_phone_number);




#endif
