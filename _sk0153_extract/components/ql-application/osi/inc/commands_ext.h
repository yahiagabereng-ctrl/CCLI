/*=============================================================================
 * File       :  COMMANDS_EXT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - extra commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __COMMANDS_EXT_H__


#define __COMMANDS_EXT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* number of extra commands */
#define COMMANDS_EXT__NUM_COMMANDS      10

/* length of extra commands */
#define COMMANDS_EXT__LEN_COMMAND_EXT   20
#define COMMANDS_EXT__LEN_COMMAND       80




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - extra commands */
typedef struct
{
    ascii commands_ext[COMMANDS_EXT__NUM_COMMANDS][COMMANDS_EXT__LEN_COMMAND_EXT + 1];   /* extra command text */
    ascii commands    [COMMANDS_EXT__NUM_COMMANDS][COMMANDS_EXT__LEN_COMMAND     + 1];   /*       command text */
} COMMANDS_EXT__CONFIG__COMMANDS_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "extra commands" configuration */
void CommandsExt_Config_CommandsExt_GetDefault(COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data);
void CommandsExt_Config_CommandsExt_Get       (COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data);
void CommandsExt_Config_CommandsExt_Set       (COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data);
bool CommandsExt_Config_CommandsExt_IsValid   (COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data);

/* search extra commands */
bool CommandsExt_SearchCommandExt(ascii *ptr_text, ascii **ptr_text_sub);




#endif
