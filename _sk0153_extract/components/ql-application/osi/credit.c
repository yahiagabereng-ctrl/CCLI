/*=============================================================================
 * File       :  CREDIT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMS credit
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "alarm.h"
#include "credit.h"
#include "debug_my.h"
#include "program_flash.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - "available credit" */
static       CREDIT__STATUS__AVAILABLE_CREDIT Credit_Status_AvailableCredit;
static const CREDIT__STATUS__AVAILABLE_CREDIT Credit_Status_AvailableCreditDefault =
{
    0,       /* available credit */
};


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "credit" */
static       CREDIT__CONFIG__CREDIT           Credit_Config_Credit;
static const CREDIT__CONFIG__CREDIT           Credit_Config_CreditDefault =
{
    FALSE,   /* status           */
    10,      /* alarm     credit */
};




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




/*===========================================================================
 * Function   : Credit_ConsumeOneCredit
 *
 * Description: signal the consumption of 1 credit
 *              It generates a credit alarm if the available credit reaches the
 *              warning threshold
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Credit_ConsumeOneCredit(void)
{
    bool result;
    
    (void)result;


    /* decrease available credit */
    if (Credit_Status_AvailableCredit.available_credit > 0)
    {
        Credit_Status_AvailableCredit.available_credit--;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__011_CREDIT_INFO, PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT);
    }


    /* check if there is a credit alarm condition */
    if (Credit_Config_Credit.status)
    {
        /* credit function enabled */
        if (Credit_Status_AvailableCredit.available_credit == Credit_Config_Credit.alarm_credit)
        {
            /* available credit equal to configured alarm credit */

            /* generate "credit warning" alarm */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CREDIT, DEBUG_TRACE_TYPE_LOW, "\"credit warning\" Alarm", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            result = Alarm_Event_NewAlarm(ALARM__ALARM_ID__CREDIT_WARNING, 0);
        }
    }
}




/*===========================================================================
 * Function   : Credit_Status_AvailableCredit_GetDefault
 *
 * Description: get the default "available credit" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Credit_Status_AvailableCredit_GetDefault(CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data)
{
    *ptr_data = Credit_Status_AvailableCreditDefault;
}




/*===========================================================================
 * Function   : Credit_Status_AvailableCredit_Get
 *
 * Description: get the "available credit" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Credit_Status_AvailableCredit_Get(CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data)
{
    *ptr_data = Credit_Status_AvailableCredit;
}




/*===========================================================================
 * Function   : Credit_Status_AvailableCredit_Set
 *
 * Description: set the "available credit" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Credit_Status_AvailableCredit_Set(CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data)
{
    Credit_Status_AvailableCredit = *ptr_data;
}




/*===========================================================================
 * Function   : Credit_Status_AvailableCredit_IsValid
 *
 * Description: check if the "available credit" status is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Credit_Status_AvailableCredit_IsValid(CREDIT__STATUS__AVAILABLE_CREDIT *ptr_data)
{
    if (ptr_data->available_credit > 9999)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Credit_Config_Credit_GetDefault
 *
 * Description: get the default "credit" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Credit_Config_Credit_GetDefault(CREDIT__CONFIG__CREDIT *ptr_data)
{
    *ptr_data = Credit_Config_CreditDefault;
}




/*===========================================================================
 * Function   : Credit_Config_Credit_Get
 *
 * Description: get the "credit" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Credit_Config_Credit_Get(CREDIT__CONFIG__CREDIT *ptr_data)
{
    *ptr_data = Credit_Config_Credit;
}




/*===========================================================================
 * Function   : Credit_Config_Credit_Set
 *
 * Description: set the "credit" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Credit_Config_Credit_Set(CREDIT__CONFIG__CREDIT *ptr_data)
{
    Credit_Config_Credit = *ptr_data;
}




/*===========================================================================
 * Function   : Credit_Config_Credit_IsValid
 *
 * Description: check if the "credit" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Credit_Config_Credit_IsValid(CREDIT__CONFIG__CREDIT *ptr_data)
{
    if (ptr_data->alarm_credit > 9999)
        return FALSE;

    return TRUE;
}
