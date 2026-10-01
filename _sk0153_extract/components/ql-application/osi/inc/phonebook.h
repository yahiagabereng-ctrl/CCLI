/*=============================================================================
 * File       :  PHONEBOOK.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - phonebook
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __PHONEBOOK_H__


#define __PHONEBOOK_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* phonebook size */
#define PHONEBOOK__PHONEBOOK_SIZE             100   /* phonebook size */

/* max lengthes */
#define PHONEBOOK__MAX_LEN_NAME               14    /* name         max length */
#define PHONEBOOK__MAX_LEN_PHONE_NUMBER       20    /* phone number max length */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* phonebook entry */
typedef struct
{
    ascii                      name        [PHONEBOOK__MAX_LEN_NAME         + 1];   /* name         */
    ascii                      phone_number[PHONEBOOK__MAX_LEN_PHONE_NUMBER + 1];   /* phone number */
} PHONEBOOK__PHONEBOOK_ENTRY;


/* configuration - "phonebook" */
typedef struct
{
    PHONEBOOK__PHONEBOOK_ENTRY phonebook_entry[PHONEBOOK__PHONEBOOK_SIZE];
} PHONEBOOK__CONFIG__PHONEBOOK;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set "phonebook" configuration */
void Phonebook_Config_Phonebook_GetDefault(PHONEBOOK__CONFIG__PHONEBOOK *ptr_data);
void Phonebook_Config_Phonebook_Get       (PHONEBOOK__CONFIG__PHONEBOOK *ptr_data);
void Phonebook_Config_Phonebook_Set       (PHONEBOOK__CONFIG__PHONEBOOK *ptr_data);
bool Phonebook_Config_Phonebook_IsValid   (PHONEBOOK__CONFIG__PHONEBOOK *ptr_data);


/*-----------------------------------------------------------------------------
 * Actions
 *-----------------------------------------------------------------------------*/
bool Phonebook_AddPhonebookEntry         (PHONEBOOK__PHONEBOOK_ENTRY *ptr_phonebook_entry);
bool Phonebook_RemovePhonebookEntry      (PHONEBOOK__PHONEBOOK_ENTRY *ptr_phonebook_entry);
bool Phonebook_RemovePhonebookName       (ascii                      *ptr_phone_name     );
bool Phonebook_RemovePhonebookPhoneNumber(ascii                      *ptr_phone_number   );


/*-----------------------------------------------------------------------------
 * Info
 *-----------------------------------------------------------------------------*/
/* phonebook occupation */
u16  Phonebook_NumberOfPhonebookEntries(void);
bool Phonebook_IsPhonebookEntryOccupied(u16 index);
bool Phonebook_IsPhonebookFull         (void);

/* verify presence */
bool Phonebook_IsPhonebookEntry           (PHONEBOOK__PHONEBOOK_ENTRY *ptr_phonebook_entry, u16 *ptr_index);
bool Phonebook_IsPhonebookName            (ascii                      *ptr_name           , u16 *ptr_index);
bool Phonebook_IsPhonebookPhoneNumber     (ascii                      *ptr_phone_number   , u16 *ptr_index);

/* verify validity */
bool Phonebook_IsValidPhonebookEntry      (PHONEBOOK__PHONEBOOK_ENTRY *ptr_phonebook_entry);
bool Phonebook_IsValidPhonebookName       (ascii                      *ptr_name           );
bool Phonebook_IsValidPhonebookPhoneNumber(ascii                      *ptr_phone_number   );

/* phonebook name / phone number */
bool Phonebook_PhonebookName              (ascii                      *ptr_name           , u16 index);
bool Phonebook_PhonebookPhoneNumber       (ascii                      *ptr_phone_number   , u16 index);


/*-----------------------------------------------------------------------------
 * Debug
 *-----------------------------------------------------------------------------*/
/* print phonebook */
void Phonebook_PrintPhonebook(void);




#endif
