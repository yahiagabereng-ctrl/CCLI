/*=============================================================================
 * File       :  LANGUAGE.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - language
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __LANGUAGE_H__


#define __LANGUAGE_H__




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* languages */
typedef enum
{
    LANGUAGE__LANGUAGE_ITALIAN,       // Italian
    LANGUAGE__LANGUAGE_ENGLISH,       // English
    LANGUAGE__LANGUAGE_FRENCH,        // French
    LANGUAGE__LANGUAGE_GERMAN,        // German
    LANGUAGE__LANGUAGE_SPANISH,       // Spanish
    LANGUAGE__LANGUAGE_POLISH,        // Polish
    LANGUAGE__LANGUAGE_SWEDISH,       // Swedish

    LANGUAGE__NUMBERS_OF_LANGUAGES    // number of defined languages
} LANGUAGE__LANGUAGE;


/* configuration - "language" */
typedef struct
{
    LANGUAGE__LANGUAGE language;   /* actual language */
} LANGUAGE__CONFIG__LANGUAGE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "language" configuration */
void Language_Config_Language_GetDefault(LANGUAGE__CONFIG__LANGUAGE *ptr_data);
void Language_Config_Language_Get       (LANGUAGE__CONFIG__LANGUAGE *ptr_data);
void Language_Config_Language_Set       (LANGUAGE__CONFIG__LANGUAGE *ptr_data);
bool Language_Config_Language_IsValid   (LANGUAGE__CONFIG__LANGUAGE *ptr_data);




#endif
