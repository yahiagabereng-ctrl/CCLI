/*=============================================================================
 * File       :  COMMANDS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __COMMANDS_H__


#define __COMMANDS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* maximum commands string length */
#define COMMANDS__MAX_LENGTH_COMMAND_DETACHMENT   100
#define COMMANDS__MAX_LENGTH_COMMAND_RESTORE      100
#define COMMANDS__MAX_LENGTH_COMMAND_STATUS       100
#define COMMANDS__MAX_LENGTH_COMMAND_RESET        100

/* maximum answers  string length */
#define COMMANDS__MAX_LENGTH_ANSWER_DETACHMENT    100
#define COMMANDS__MAX_LENGTH_ANSWER_RESTORE       100
#define COMMANDS__MAX_LENGTH_ANSWER_STATUS        100
#define COMMANDS__MAX_LENGTH_ANSWER_RESET         100




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - command DETACHMENT */
typedef struct
{
    ascii command[COMMANDS__MAX_LENGTH_COMMAND_DETACHMENT + 1];     /* command string */
} COMMANDS__CONFIG__COMMAND_DETACHMENT;


/* configuration - command RESTORE */
typedef struct
{
    ascii command[COMMANDS__MAX_LENGTH_COMMAND_RESTORE    + 1];     /* command string */
} COMMANDS__CONFIG__COMMAND_RESTORE;


/* configuration - command STATUS */
typedef struct
{
    ascii command[COMMANDS__MAX_LENGTH_COMMAND_STATUS     + 1];     /* command string */
} COMMANDS__CONFIG__COMMAND_STATUS;


/* configuration - command RESET */
typedef struct
{
    ascii command[COMMANDS__MAX_LENGTH_COMMAND_RESET      + 1];     /* command string */
} COMMANDS__CONFIG__COMMAND_RESET;



/* configuration - answer  DETACHMENT */
typedef struct
{
    ascii answer [COMMANDS__MAX_LENGTH_ANSWER_DETACHMENT  + 1];     /* answer  string */
} COMMANDS__CONFIG__ANSWER_DETACHMENT;


/* configuration - answer  RESTORE */
typedef struct
{
    ascii answer [COMMANDS__MAX_LENGTH_ANSWER_RESTORE     + 1];     /* answer  string */
} COMMANDS__CONFIG__ANSWER_RESTORE;


/* configuration - answer  STATUS */
typedef struct
{
    ascii answer [COMMANDS__MAX_LENGTH_ANSWER_STATUS      + 1];     /* answer  string */
} COMMANDS__CONFIG__ANSWER_STATUS;


/* configuration - answer  RESET */
typedef struct
{
    ascii answer [COMMANDS__MAX_LENGTH_ANSWER_RESET       + 1];     /* answer  string */
} COMMANDS__CONFIG__ANSWER_RESET;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "command DETACHMENT" configuration */
void Commands_Config_CommandDetachment_GetDefault(COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data);
void Commands_Config_CommandDetachment_Get       (COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data);
void Commands_Config_CommandDetachment_Set       (COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data);
bool Commands_Config_CommandDetachment_IsValid   (COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data);

/* get/set "command RESTORE"    configuration */
void Commands_Config_CommandRestore_GetDefault   (COMMANDS__CONFIG__COMMAND_RESTORE    *ptr_data);
void Commands_Config_CommandRestore_Get          (COMMANDS__CONFIG__COMMAND_RESTORE    *ptr_data);
void Commands_Config_CommandRestore_Set          (COMMANDS__CONFIG__COMMAND_RESTORE    *ptr_data);
bool Commands_Config_CommandRestore_IsValid      (COMMANDS__CONFIG__COMMAND_RESTORE    *ptr_data);

/* get/set "command STATUS"     configuration */
void Commands_Config_CommandStatus_GetDefault    (COMMANDS__CONFIG__COMMAND_STATUS     *ptr_data);
void Commands_Config_CommandStatus_Get           (COMMANDS__CONFIG__COMMAND_STATUS     *ptr_data);
void Commands_Config_CommandStatus_Set           (COMMANDS__CONFIG__COMMAND_STATUS     *ptr_data);
bool Commands_Config_CommandStatus_IsValid       (COMMANDS__CONFIG__COMMAND_STATUS     *ptr_data);

/* get/set "command RESET"      configuration */
void Commands_Config_CommandReset_GetDefault     (COMMANDS__CONFIG__COMMAND_RESET      *ptr_data);
void Commands_Config_CommandReset_Get            (COMMANDS__CONFIG__COMMAND_RESET      *ptr_data);
void Commands_Config_CommandReset_Set            (COMMANDS__CONFIG__COMMAND_RESET      *ptr_data);
bool Commands_Config_CommandReset_IsValid        (COMMANDS__CONFIG__COMMAND_RESET      *ptr_data);


/* get/set "answer DETACHMENT"  configuration */
void Commands_Config_AnswerDetachment_GetDefault (COMMANDS__CONFIG__ANSWER_DETACHMENT  *ptr_data);
void Commands_Config_AnswerDetachment_Get        (COMMANDS__CONFIG__ANSWER_DETACHMENT  *ptr_data);
void Commands_Config_AnswerDetachment_Set        (COMMANDS__CONFIG__ANSWER_DETACHMENT  *ptr_data);
bool Commands_Config_AnswerDetachment_IsValid    (COMMANDS__CONFIG__ANSWER_DETACHMENT  *ptr_data);

/* get/set "answer RESTORE"     configuration */
void Commands_Config_AnswerRestore_GetDefault    (COMMANDS__CONFIG__ANSWER_RESTORE     *ptr_data);
void Commands_Config_AnswerRestore_Get           (COMMANDS__CONFIG__ANSWER_RESTORE     *ptr_data);
void Commands_Config_AnswerRestore_Set           (COMMANDS__CONFIG__ANSWER_RESTORE     *ptr_data);
bool Commands_Config_AnswerRestore_IsValid       (COMMANDS__CONFIG__ANSWER_RESTORE     *ptr_data);

/* get/set "answer STATUS"      configuration */
void Commands_Config_AnswerStatus_GetDefault     (COMMANDS__CONFIG__ANSWER_STATUS      *ptr_data);
void Commands_Config_AnswerStatus_Get            (COMMANDS__CONFIG__ANSWER_STATUS      *ptr_data);
void Commands_Config_AnswerStatus_Set            (COMMANDS__CONFIG__ANSWER_STATUS      *ptr_data);
bool Commands_Config_AnswerStatus_IsValid        (COMMANDS__CONFIG__ANSWER_STATUS      *ptr_data);

/* get/set "answer RESET"       configuration */
void Commands_Config_AnswerReset_GetDefault      (COMMANDS__CONFIG__ANSWER_RESET       *ptr_data);
void Commands_Config_AnswerReset_Get             (COMMANDS__CONFIG__ANSWER_RESET       *ptr_data);
void Commands_Config_AnswerReset_Set             (COMMANDS__CONFIG__ANSWER_RESET       *ptr_data);
bool Commands_Config_AnswerReset_IsValid         (COMMANDS__CONFIG__ANSWER_RESET       *ptr_data);




#endif
