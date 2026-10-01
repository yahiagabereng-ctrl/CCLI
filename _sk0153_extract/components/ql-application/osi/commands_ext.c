/*=============================================================================
 * File       :  COMMANDS_EXT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - extra commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <ctype.h>
#include <string.h>

/* user     includes */
#include "commands_ext.h"
#include "fw_config.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - extra commands */
static       COMMANDS_EXT__CONFIG__COMMANDS_EXT CommandsExt_Config_CommandsExt;
static const COMMANDS_EXT__CONFIG__COMMANDS_EXT CommandsExt_Config_CommandsExtDefault =
{
    /* extra command text */
    {
        { ""             },   //  1
        { ""             },   //  2
        { ""             },   //  3
        { ""             },   //  4
        { ""             },   //  5

        { ""             },   //  6
        { ""             },   //  7
        { ""             },   //  8
        { ""             },   //  9
        { ""             },   // 10
    },

    /* command text */
    {
        { ""             },   //  1
        { ""             },   //  2
        { ""             },   //  3
        { ""             },   //  4
        { ""             },   //  5

        { ""             },   //  6
        { ""             },   //  7
        { ""             },   //  8
        { ""             },   //  9
        { ""             },   // 10
    }
};




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




/*===========================================================================
 * Function   : CommandsExt_Config_CommandsExt_GetDefault
 *
 * Description: get the default "command DETACHMENT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void CommandsExt_Config_CommandsExt_GetDefault(COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data)
{
    *ptr_data = CommandsExt_Config_CommandsExtDefault;
}




/*===========================================================================
 * Function   : CommandsExt_Config_CommandsExt_Get
 *
 * Description: get the "command DETACHMENT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void CommandsExt_Config_CommandsExt_Get(COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data)
{
    *ptr_data = CommandsExt_Config_CommandsExt;
}




/*===========================================================================
 * Function   : CommandsExt_Config_CommandsExt_Set
 *
 * Description: set the "command DETACHMENT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void CommandsExt_Config_CommandsExt_Set(COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data)
{
    CommandsExt_Config_CommandsExt = *ptr_data;
}




/*===========================================================================
 * Function   : CommandsExt_Config_CommandsExt_IsValid
 *
 * Description: check if the "command DETACHMENT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool CommandsExt_Config_CommandsExt_IsValid(COMMANDS_EXT__CONFIG__COMMANDS_EXT *ptr_data)
{
    u8 i;


    for (i = 0; i < COMMANDS_EXT__NUM_COMMANDS; i++)
    {
        if (strlen(ptr_data->commands_ext[i]) > COMMANDS_EXT__LEN_COMMAND_EXT)
            return FALSE;

        if (strlen(ptr_data->commands    [i]) > COMMANDS_EXT__LEN_COMMAND    )
            return FALSE;
    }


    for (i = 0; i < COMMANDS_EXT__NUM_COMMANDS; i++)
    {
        if (
             (strlen(ptr_data->commands_ext[i]) == 0) &&
             (strlen(ptr_data->commands    [i]) >  0)
           )
        {
            return FALSE;
        }

        if (
             (strlen(ptr_data->commands_ext[i]) >  0) &&
             (strlen(ptr_data->commands    [i]) == 0)
           )
        {
            return FALSE;
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : CommandsExt_SearchCommandExt
 *
 * Description: verify if a configured extra command corresponds to the
 *              specified text (case insensitive)
 * Input      : - ptr_text    : pointer to the specified text
 *              - ptr_text_sub: pointer to save the extra command text
 * Output     : - FALSE: configured extra command not found
 *              - TRUE : configured extra command     found
 *===========================================================================*/
bool CommandsExt_SearchCommandExt(ascii *ptr_text, ascii **ptr_text_sub)
{
    u8 len_text;
    u8 len_command_ext;

    u8 i;
    u8 j;


    /* calculate the text length */
    len_text = (u8)strlen(ptr_text);


    /* verify if a configured extra command corresponds to the specified text */
    for (i = 0; i < COMMANDS_EXT__NUM_COMMANDS; i++)
    {
        /* calculate the i-th configured extra command length */
        len_command_ext = strlen(CommandsExt_Config_CommandsExt.commands_ext[i]);

        /* compare the 2 string lengths */
        if (len_text == len_command_ext)
        {
            /* the 2 string lengths are equal */

            /* compare the 2 strings (case insensitive) */
            for (j = 0; j < len_text; j++)
            {
                if ( (toupper((unsigned char)CommandsExt_Config_CommandsExt.commands_ext[i][j]))  !=
                     (toupper((unsigned char)ptr_text                                      [j])) )
                {
                    break;  // exit from for loop with "j" index (the 2 strings are different)
                }
            }

            if (j == len_text)
            {
                /* the 2 strings are identical */

                /* save the pointer to the configured extra command found */
                *ptr_text_sub = CommandsExt_Config_CommandsExt.commands[i];

                return TRUE;
            }
            else
            {
                break;  // exit from for loop with "i" index (move to next configured extra command)
            }
        }
    }


    return FALSE;
}
