/*=============================================================================
 * File       :  FWD.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - FWD (ForWrD number)
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "fwd.h"
#include "typedef.h"
#include "utility.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* configuration - "FWD phone number" */
static       FWD__CONFIG__FWD_NUMBER Fwd_Config_FwdNumber;
static const FWD__CONFIG__FWD_NUMBER Fwd_Config_FwdNumberDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00
    }
};




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




/*===========================================================================
 * Function   : Fwd_Config_FwdNumber_GetDefault
 *
 * Description: get the default "FWD phone number" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Fwd_Config_FwdNumber_GetDefault(FWD__CONFIG__FWD_NUMBER *ptr_data)
{
    *ptr_data = Fwd_Config_FwdNumberDefault;
}




/*===========================================================================
 * Function   : Fwd_Config_FwdNumber_Get
 *
 * Description: get the "FWD phone number" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Fwd_Config_FwdNumber_Get(FWD__CONFIG__FWD_NUMBER *ptr_data)
{
    *ptr_data = Fwd_Config_FwdNumber;
}




/*===========================================================================
 * Function   : Fwd_Config_FwdNumber_Set
 *
 * Description: set the "FWD phone number" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Fwd_Config_FwdNumber_Set(FWD__CONFIG__FWD_NUMBER *ptr_data)
{
    Fwd_Config_FwdNumber = *ptr_data;
}




/*===========================================================================
 * Function   : Fwd_Config_FwdNumber_IsValid
 *
 * Description: check if the "FWD phone number" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Fwd_Config_FwdNumber_IsValid(FWD__CONFIG__FWD_NUMBER *ptr_data)
{
    bool result;


    if (strlen(ptr_data->fwd_number) > 0)
    {
        if (strlen(ptr_data->fwd_number) > FWD__MAX_LEN_PHONE_NUMBER)
            return FALSE;

        result = Utility_IsPhoneString(ptr_data->fwd_number);
        if (!result)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Fwd_NumberOfFwdNumbers
 *
 * Description: return the number of FWD phone numbers
 * Input      : -
 * Output     : - number of FWD phone numbers (0-1)
 *===========================================================================*/
u8 Fwd_NumberOfFwdNumbers(void)
{
    bool result;


    result = Utility_IsPhoneString(Fwd_Config_FwdNumber.fwd_number);
    if (result)
        return 1;
    else
        return 0;
}




/*===========================================================================
 * Function   : Fwd_IsFwdNumber
 *
 * Description: verify if a specified phone number is the FWD phone number
 * Input      : - ptr_phone_number: pointer to the phone number to be verified
 * Output     : - FALSE: the phone number is not the FWD phone number
 *              - TRUE : the phone number is     the FWD phone number
 *===========================================================================*/
bool Fwd_IsFwdNumber(ascii *ptr_phone_number)
{
    s32 result;


    result = strncmp(ptr_phone_number, Fwd_Config_FwdNumber.fwd_number, FWD__MAX_LEN_PHONE_NUMBER);
    if (result == 0)
        return TRUE;

    return FALSE;
}
