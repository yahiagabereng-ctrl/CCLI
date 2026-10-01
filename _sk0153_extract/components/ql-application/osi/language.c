/*=============================================================================
 * File       :  LANGUAGE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - language
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "language.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "language" */
static       LANGUAGE__CONFIG__LANGUAGE Language_Config_Language;
static const LANGUAGE__CONFIG__LANGUAGE Language_Config_LanguageDefault =
{
    LANGUAGE__LANGUAGE_ITALIAN
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "language" configuration */
void Language_Config_Language_GetDefault(LANGUAGE__CONFIG__LANGUAGE *ptr_data);
void Language_Config_Language_Get       (LANGUAGE__CONFIG__LANGUAGE *ptr_data);
void Language_Config_Language_Set       (LANGUAGE__CONFIG__LANGUAGE *ptr_data);
bool Language_Config_Language_IsValid   (LANGUAGE__CONFIG__LANGUAGE *ptr_data);




/*===========================================================================
 * Function   : Language_Config_Language_GetDefault
 *
 * Description: get the default "language" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Language_Config_Language_GetDefault(LANGUAGE__CONFIG__LANGUAGE *ptr_data)
{
    *ptr_data = Language_Config_LanguageDefault;
}




/*===========================================================================
 * Function   : Language_Config_Language_Get
 *
 * Description: get the "language" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Language_Config_Language_Get(LANGUAGE__CONFIG__LANGUAGE *ptr_data)
{
    *ptr_data = Language_Config_Language;
}




/*===========================================================================
 * Function   : Language_Config_Language_Set
 *
 * Description: set the "language" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Language_Config_Language_Set(LANGUAGE__CONFIG__LANGUAGE *ptr_data)
{
    Language_Config_Language = *ptr_data;
}




/*===========================================================================
 * Function   : Language_Config_Language_IsValid
 *
 * Description: check if the "language" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Language_Config_Language_IsValid(LANGUAGE__CONFIG__LANGUAGE *ptr_data)
{
    if (
         (ptr_data->language != LANGUAGE__LANGUAGE_ITALIAN) &&   // Italian
         (ptr_data->language != LANGUAGE__LANGUAGE_ENGLISH) &&   // English
         (ptr_data->language != LANGUAGE__LANGUAGE_FRENCH ) &&   // French
         (ptr_data->language != LANGUAGE__LANGUAGE_GERMAN ) &&   // German
         (ptr_data->language != LANGUAGE__LANGUAGE_SPANISH) &&   // Spanish
         (ptr_data->language != LANGUAGE__LANGUAGE_POLISH ) &&   // Polish
         (ptr_data->language != LANGUAGE__LANGUAGE_SWEDISH)      // Swedish
       )
    {
        return FALSE;
    }

    return TRUE;
}
