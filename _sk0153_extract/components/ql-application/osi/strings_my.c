/*=============================================================================
 * File       :  STRINGS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - text strings
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "language.h"
#include "strings_eng.h"
#include "strings_fre.h"
#include "strings_ger.h"
#include "strings_ita.h"
#include "strings_my.h"
#include "strings_pol.h"
#include "strings_spa.h"
#include "strings_swe.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* strings */
static const ascii *const *const Strings_Strings[LANGUAGE__NUMBERS_OF_LANGUAGES] =
{
    StringsIta_Strings,   // Italian
    StringsEng_Strings,   // English
    StringsFre_Strings,   // French
    StringsGer_Strings,   // German
    StringsSpa_Strings,   // Spanish
    StringsPol_Strings,   // Polish
    StringsSwe_Strings,   // Swedish
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
ascii *Strings_GetStringOfLanguage(u8 id_language, u16 id_string);
ascii *Strings_GetString          (                u16 id_string);




/*=============================================================================
 * Function   : Strings_GetStringOfLanguage
 *
 * Description: get the text of a specified string of a specified language
 * Input      : - id_language: id language
 *              - id_string  : id string
 * Output     : - pointer to the string
 *=============================================================================*/
ascii *Strings_GetStringOfLanguage(u8 id_language, u16 id_string)
{
    ascii *ptr_string;


    if (id_language >= LANGUAGE__NUMBERS_OF_LANGUAGES)
        return "";
    if (id_string   >= STRINGS__NUMBERS_OF_STRINGS   )
        return "";

    /* extract the alarm string */
    ptr_string = (ascii *)Strings_Strings[id_language][id_string];

    return ptr_string;
}




/*=============================================================================
 * Function   : Strings_GetString
 *
 * Description: get the text of a specified string
 * Input      : - id_string  : id string
 * Output     : - pointer to the string
 *=============================================================================*/
ascii *Strings_GetString(u16 id_string)
{
    LANGUAGE__CONFIG__LANGUAGE  language_configuration;
    LANGUAGE__LANGUAGE          language;

    ascii                      *ptr_string;


    if (id_string >= STRINGS__NUMBERS_OF_STRINGS)
        return "*?*";


    /* get language configuration */
    Language_Config_Language_Get(&language_configuration);

    language = language_configuration.language;

    /* verify language configuration */
    if (language >= LANGUAGE__NUMBERS_OF_LANGUAGES)
        language = LANGUAGE__LANGUAGE_ENGLISH;


    /* extract the alarm string */
    ptr_string = (ascii *)Strings_Strings[language][id_string];

    return ptr_string;
}
