/*=============================================================================
 * File       :  COMMANDS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:PROGRAM - commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 * Answer text to print all the defined variables:
 *   1. "TIME=#TIME# P=#POWER# IN1=#IN1# IN2=#IN2# OUT1=#OUT1# OUT2=#OUT2# TINT=#TINT# TEXT=#TEXT#"
 *   2. "CREG=#CREG# CGREG=#CGREG# RSSI=#RSSI# BER=#BER#"
 *   3. "COPS0=#COPS0# COPS1=#COPS1# COPS2=#COPS2#"
 *   4. "MCC=#MCC# MNC=#MNC# LAC=#LAC# CI=#CI# RESET=#RESETRESULT#"
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "commands.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - command DETACHMENT */
static       COMMANDS__CONFIG__COMMAND_DETACHMENT Commands_Config_CommandDetachment;
static const COMMANDS__CONFIG__COMMAND_DETACHMENT Commands_Config_CommandDetachmentDefault =
{
    "DISTACCO utenza #POD#"
};


/* configuration - command RESTORE */
static       COMMANDS__CONFIG__COMMAND_RESTORE    Commands_Config_CommandRestore;
static const COMMANDS__CONFIG__COMMAND_RESTORE    Commands_Config_CommandRestoreDefault =
{
    "RIPRISTINO utenza #POD#"
};


/* configuration - command STATUS */
static       COMMANDS__CONFIG__COMMAND_STATUS     Commands_Config_CommandStatus;
static const COMMANDS__CONFIG__COMMAND_STATUS     Commands_Config_CommandStatusDefault =
{
    "utenza #POD# stato Input - Output"
};


/* configuration - command RESET */
static       COMMANDS__CONFIG__COMMAND_RESET      Commands_Config_CommandReset;
static const COMMANDS__CONFIG__COMMAND_RESET      Commands_Config_CommandResetDefault =
{
    "RESET utenza #POD#"
};



/* configuration - answer DETACHMENT */
static       COMMANDS__CONFIG__ANSWER_DETACHMENT  Commands_Config_AnswerDetachment;
static const COMMANDS__CONFIG__ANSWER_DETACHMENT  Commands_Config_AnswerDetachmentDefault =
{
    "utenza #POD# distaccata - Input=#IN1# - Output=#OUT1#"
  //"utenza #POD# distaccata - Input=#IN1#,#IN2# - Output=#OUT1#,#OUT2#"
};


/* configuration - answer RESTORE */
static       COMMANDS__CONFIG__ANSWER_RESTORE     Commands_Config_AnswerRestore;
static const COMMANDS__CONFIG__ANSWER_RESTORE     Commands_Config_AnswerRestoreDefault =
{
    "utenza #POD# ripristinata - Input=#IN1# - Output=#OUT1#"
  //"utenza #POD# ripristinata - Input=#IN1#,#IN2# - Output=#OUT1#,#OUT2#"
};


/* configuration - answer STATUS */
static       COMMANDS__CONFIG__ANSWER_STATUS      Commands_Config_AnswerStatus;
static const COMMANDS__CONFIG__ANSWER_STATUS      Commands_Config_AnswerStatusDefault =
{
    "utenza #POD# stato - Input=#IN1# - Output=#OUT1#"
  //"utenza #POD# stato - Input=#IN1#,#IN2# - Output=#OUT1#,#OUT2#"
};


/* configuration - answer RESET */
static       COMMANDS__CONFIG__ANSWER_RESET       Commands_Config_AnswerReset;
static const COMMANDS__CONFIG__ANSWER_RESET       Commands_Config_AnswerResetDefault =
{
    "utenza #POD# RESET #RESETRESULT# - Input=#IN1# - Output=#OUT1#"
  //"utenza #POD# RESET #RESETRESULT# - Input=#IN1#,#IN2# - Output=#OUT1#,#OUT2#"
};




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




/*===========================================================================
 * Function   : Commands_Config_CommandDetachment_GetDefault
 *
 * Description: get the default "command DETACHMENT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandDetachment_GetDefault(COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data)
{
    *ptr_data = Commands_Config_CommandDetachmentDefault;
}




/*===========================================================================
 * Function   : Commands_Config_CommandDetachment_Get
 *
 * Description: get the "command DETACHMENT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandDetachment_Get(COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data)
{
    *ptr_data = Commands_Config_CommandDetachment;
}




/*===========================================================================
 * Function   : Commands_Config_CommandDetachment_Set
 *
 * Description: set the "command DETACHMENT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandDetachment_Set(COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data)
{
    Commands_Config_CommandDetachment = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_CommandDetachment_IsValid
 *
 * Description: check if the "command DETACHMENT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_CommandDetachment_IsValid(COMMANDS__CONFIG__COMMAND_DETACHMENT *ptr_data)
{
    if (strlen(ptr_data->command) > COMMANDS__MAX_LENGTH_COMMAND_DETACHMENT)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_CommandRestore_GetDefault
 *
 * Description: get the default "command RESTORE" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandRestore_GetDefault(COMMANDS__CONFIG__COMMAND_RESTORE *ptr_data)
{
    *ptr_data = Commands_Config_CommandRestoreDefault;
}




/*===========================================================================
 * Function   : Commands_Config_CommandRestore_Get
 *
 * Description: get the "command RESTORE" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandRestore_Get(COMMANDS__CONFIG__COMMAND_RESTORE *ptr_data)
{
    *ptr_data = Commands_Config_CommandRestore;
}




/*===========================================================================
 * Function   : Commands_Config_CommandRestore_Set
 *
 * Description: set the "command RESTORE" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandRestore_Set(COMMANDS__CONFIG__COMMAND_RESTORE *ptr_data)
{
    Commands_Config_CommandRestore = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_CommandRestore_IsValid
 *
 * Description: check if the "command RESTORE" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_CommandRestore_IsValid(COMMANDS__CONFIG__COMMAND_RESTORE *ptr_data)
{
    if (strlen(ptr_data->command) > COMMANDS__MAX_LENGTH_COMMAND_RESTORE)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_CommandStatus_GetDefault
 *
 * Description: get the default "command STATUS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandStatus_GetDefault(COMMANDS__CONFIG__COMMAND_STATUS *ptr_data)
{
    *ptr_data = Commands_Config_CommandStatusDefault;
}




/*===========================================================================
 * Function   : Commands_Config_CommandStatus_Get
 *
 * Description: get the "command STATUS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandStatus_Get(COMMANDS__CONFIG__COMMAND_STATUS *ptr_data)
{
    *ptr_data = Commands_Config_CommandStatus;
}




/*===========================================================================
 * Function   : Commands_Config_CommandStatus_Set
 *
 * Description: set the "command STATUS" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandStatus_Set(COMMANDS__CONFIG__COMMAND_STATUS *ptr_data)
{
    Commands_Config_CommandStatus = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_CommandStatus_IsValid
 *
 * Description: check if the "command STATUS" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_CommandStatus_IsValid(COMMANDS__CONFIG__COMMAND_STATUS *ptr_data)
{
    if (strlen(ptr_data->command) > COMMANDS__MAX_LENGTH_COMMAND_STATUS)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_CommandReset_GetDefault
 *
 * Description: get the default "command RESET" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandReset_GetDefault(COMMANDS__CONFIG__COMMAND_RESET *ptr_data)
{
    *ptr_data = Commands_Config_CommandResetDefault;
}




/*===========================================================================
 * Function   : Commands_Config_CommandReset_Get
 *
 * Description: get the "command RESET" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandReset_Get(COMMANDS__CONFIG__COMMAND_RESET *ptr_data)
{
    *ptr_data = Commands_Config_CommandReset;
}




/*===========================================================================
 * Function   : Commands_Config_CommandReset_Set
 *
 * Description: set the "command RESET" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_CommandReset_Set(COMMANDS__CONFIG__COMMAND_RESET *ptr_data)
{
    Commands_Config_CommandReset = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_CommandReset_IsValid
 *
 * Description: check if the "command RESET" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_CommandReset_IsValid(COMMANDS__CONFIG__COMMAND_RESET *ptr_data)
{
    if (strlen(ptr_data->command) > COMMANDS__MAX_LENGTH_COMMAND_RESET)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerDetachment_GetDefault
 *
 * Description: get the default "answer DETACHMENT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerDetachment_GetDefault(COMMANDS__CONFIG__ANSWER_DETACHMENT *ptr_data)
{
    *ptr_data = Commands_Config_AnswerDetachmentDefault;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerDetachment_Get
 *
 * Description: get the "answer DETACHMENT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerDetachment_Get(COMMANDS__CONFIG__ANSWER_DETACHMENT *ptr_data)
{
    *ptr_data = Commands_Config_AnswerDetachment;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerDetachment_Set
 *
 * Description: set the "answer DETACHMENT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerDetachment_Set(COMMANDS__CONFIG__ANSWER_DETACHMENT *ptr_data)
{
    Commands_Config_AnswerDetachment = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerDetachment_IsValid
 *
 * Description: check if the "answer DETACHMENT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_AnswerDetachment_IsValid(COMMANDS__CONFIG__ANSWER_DETACHMENT *ptr_data)
{
    if (strlen(ptr_data->answer) > COMMANDS__MAX_LENGTH_ANSWER_DETACHMENT)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerRestore_GetDefault
 *
 * Description: get the default "answer RESTORE" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerRestore_GetDefault(COMMANDS__CONFIG__ANSWER_RESTORE *ptr_data)
{
    *ptr_data = Commands_Config_AnswerRestoreDefault;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerRestore_Get
 *
 * Description: get the "answer RESTORE" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerRestore_Get(COMMANDS__CONFIG__ANSWER_RESTORE *ptr_data)
{
    *ptr_data = Commands_Config_AnswerRestore;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerRestore_Set
 *
 * Description: set the "answer RESTORE" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerRestore_Set(COMMANDS__CONFIG__ANSWER_RESTORE *ptr_data)
{
    Commands_Config_AnswerRestore = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerRestore_IsValid
 *
 * Description: check if the "answer RESTORE" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_AnswerRestore_IsValid(COMMANDS__CONFIG__ANSWER_RESTORE *ptr_data)
{
    if (strlen(ptr_data->answer) > COMMANDS__MAX_LENGTH_ANSWER_RESTORE)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerStatus_GetDefault
 *
 * Description: get the default "answer STATUS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerStatus_GetDefault(COMMANDS__CONFIG__ANSWER_STATUS *ptr_data)
{
    *ptr_data = Commands_Config_AnswerStatusDefault;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerStatus_Get
 *
 * Description: get the "answer STATUS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerStatus_Get(COMMANDS__CONFIG__ANSWER_STATUS *ptr_data)
{
    *ptr_data = Commands_Config_AnswerStatus;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerStatus_Set
 *
 * Description: set the "answer STATUS" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerStatus_Set(COMMANDS__CONFIG__ANSWER_STATUS *ptr_data)
{
    Commands_Config_AnswerStatus = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerStatus_IsValid
 *
 * Description: check if the "answer STATUS" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_AnswerStatus_IsValid(COMMANDS__CONFIG__ANSWER_STATUS *ptr_data)
{
    if (strlen(ptr_data->answer) > COMMANDS__MAX_LENGTH_ANSWER_STATUS)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerReset_GetDefault
 *
 * Description: get the default "answer RESET" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerReset_GetDefault(COMMANDS__CONFIG__ANSWER_RESET *ptr_data)
{
    *ptr_data = Commands_Config_AnswerResetDefault;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerReset_Get
 *
 * Description: get the "answer RESET" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerReset_Get(COMMANDS__CONFIG__ANSWER_RESET *ptr_data)
{
    *ptr_data = Commands_Config_AnswerReset;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerReset_Set
 *
 * Description: set the "answer RESET" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Commands_Config_AnswerReset_Set(COMMANDS__CONFIG__ANSWER_RESET *ptr_data)
{
    Commands_Config_AnswerReset = *ptr_data;
}




/*===========================================================================
 * Function   : Commands_Config_AnswerReset_IsValid
 *
 * Description: check if the "answer RESET" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Commands_Config_AnswerReset_IsValid(COMMANDS__CONFIG__ANSWER_RESET *ptr_data)
{
    if (strlen(ptr_data->answer) > COMMANDS__MAX_LENGTH_ANSWER_RESET)
        return FALSE;

    return TRUE;
}
