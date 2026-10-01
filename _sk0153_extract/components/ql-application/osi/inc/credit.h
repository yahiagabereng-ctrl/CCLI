/*=============================================================================
 * File       :  CREDIT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMS credit
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __CREDIT_H__


#define __CREDIT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPE
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - "available credit" */
typedef struct
{
    u16  available_credit;   /* available credit [0-9999] */
} CREDIT__STATUS__AVAILABLE_CREDIT;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "credit" */
typedef struct
{
    bool status;             /* status           [OFF/ON] */
    u16  alarm_credit;       /* alarm     credit [0-9999] */
} CREDIT__CONFIG__CREDIT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
void Credit_ConsumeOneCredit(void);

/*-----------------------------------------------------------------------------
 * get/set
 *-----------------------------------------------------------------------------*/
/* get/set "available credit" status */
void Credit_Status_AvailableCredit_GetDefault(CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data);
void Credit_Status_AvailableCredit_Get       (CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data);
void Credit_Status_AvailableCredit_Set       (CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data);
bool Credit_Status_AvailableCredit_IsValid   (CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data);

/* get/set "credit" configuration */
void Credit_Config_Credit_GetDefault         (CREDIT__CONFIG__CREDIT           *ptr_data);
void Credit_Config_Credit_Get                (CREDIT__CONFIG__CREDIT           *ptr_data);
void Credit_Config_Credit_Set                (CREDIT__CONFIG__CREDIT           *ptr_data);
bool Credit_Config_Credit_IsValid            (CREDIT__CONFIG__CREDIT           *ptr_data);




#endif
