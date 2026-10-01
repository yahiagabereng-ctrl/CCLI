/*=============================================================================
 * File       :  MYN.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - MYN (MY Number)
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "myn.h"
#include "typedef.h"
#include "utility.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* configuration - "MYN phone number" */
static       MYN__CONFIG__MYN_NUMBER Myn_Config_MynNumber;
static const MYN__CONFIG__MYN_NUMBER Myn_Config_MynNumberDefault =
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




/*===========================================================================
 * Function   : Myn_Config_MynNumber_GetDefault
 *
 * Description: get the default "MYN phone number" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Myn_Config_MynNumber_GetDefault(MYN__CONFIG__MYN_NUMBER *ptr_data)
{
    *ptr_data = Myn_Config_MynNumberDefault;
}




/*===========================================================================
 * Function   : Myn_Config_MynNumber_Get
 *
 * Description: get the "MYN phone number" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Myn_Config_MynNumber_Get(MYN__CONFIG__MYN_NUMBER *ptr_data)
{
    *ptr_data = Myn_Config_MynNumber;
}




/*===========================================================================
 * Function   : Myn_Config_MynNumber_Set
 *
 * Description: set the "MYN phone number" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Myn_Config_MynNumber_Set(MYN__CONFIG__MYN_NUMBER *ptr_data)
{
    Myn_Config_MynNumber = *ptr_data;
}




/*===========================================================================
 * Function   : Myn_Config_MynNumber_IsValid
 *
 * Description: check if the "MYN phone number" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Myn_Config_MynNumber_IsValid(MYN__CONFIG__MYN_NUMBER *ptr_data)
{
    bool result;


    if (strlen(ptr_data->myn_number) > 0)
    {
        if (strlen(ptr_data->myn_number) > MYN__MAX_LEN_PHONE_NUMBER)
            return FALSE;

        result = Utility_IsPhoneString(ptr_data->myn_number);
        if (!result)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Myn_NumberOfMynNumbers
 *
 * Description: return the number of MYN phone numbers
 * Input      : -
 * Output     : - number of MYN phone numbers (0-1)
 *===========================================================================*/
u8 Myn_NumberOfMynNumbers(void)
{
    bool result;


    result = Utility_IsPhoneString(Myn_Config_MynNumber.myn_number);
    if (result)
        return 1;
    else
        return 0;
}




/*===========================================================================
 * Function   : Myn_IsMynNumber
 *
 * Description: verify if a specified phone number is the MYN phone number
 * Input      : - ptr_phone_number: pointer to the phone number to be verified
 * Output     : - FALSE: the phone number is not the MYN phone number
 *              - TRUE : the phone number is     the MYN phone number
 *===========================================================================*/
bool Myn_IsMynNumber(ascii *ptr_phone_number)
{
    s32 result;


    result = strncmp(ptr_phone_number, Myn_Config_MynNumber.myn_number, MYN__MAX_LEN_PHONE_NUMBER);
    if (result == 0)
        return TRUE;

    return FALSE;
}
