/*=============================================================================
 * File       :  FWD.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - FWD (ForWrD number)
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __FWD_H__


#define __FWD_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* FWD phone number max length */
#define FWD__MAX_LEN_PHONE_NUMBER     20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "FWD phone number" */
typedef struct
{
    ascii fwd_number[FWD__MAX_LEN_PHONE_NUMBER + 1];
} FWD__CONFIG__FWD_NUMBER;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set "FWD phone number" configuration */
void Fwd_Config_FwdNumber_GetDefault(FWD__CONFIG__FWD_NUMBER *ptr_data);
void Fwd_Config_FwdNumber_Get       (FWD__CONFIG__FWD_NUMBER *ptr_data);
void Fwd_Config_FwdNumber_Set       (FWD__CONFIG__FWD_NUMBER *ptr_data);
bool Fwd_Config_FwdNumber_IsValid   (FWD__CONFIG__FWD_NUMBER *ptr_data);

/*-----------------------------------------------------------------------------
 * Info
 *-----------------------------------------------------------------------------*/
/* number of FWD phone numbers */
u8   Fwd_NumberOfFwdNumbers(void);

/* verify phone number type */
bool Fwd_IsFwdNumber(ascii *ptr_phone_number);




#endif
