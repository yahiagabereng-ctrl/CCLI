/*=============================================================================
 * File       :  PASSWORD.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - password management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __PASSWORD_H__


#define __PASSWORD_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* password lengths */
#define PASSWORD__MIN_LEN_PASSWORD   4     /* password maximum length */
#define PASSWORD__MAX_LEN_PASSWORD   14    /* password minimum length */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "password" */
typedef struct
{
    ascii password[PASSWORD__MAX_LEN_PASSWORD + 1];   // password string
} PASSWORD__CONFIG__PASSWORD;




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




#endif
