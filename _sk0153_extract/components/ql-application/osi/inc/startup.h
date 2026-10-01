/*=============================================================================
 * File       :  STARTUP.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - startup
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __STARTUP_H__


#define __STARTUP_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "reset.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* startup type */
#define STARTUP__STARTUP_TYPE__UNKNOWN             (RESET__STARTUP_TYPE__UNKNOWN           )   /* startup type - unknown                                                */
#define STARTUP__STARTUP_TYPE__POWER_ON_MANUAL     (RESET__STARTUP_TYPE__POWER_ON_MANUAL   )   /* startup type - power-on manual                                        */
#define STARTUP__STARTUP_TYPE__POWER_ON_AUTOMATIC  (RESET__STARTUP_TYPE__POWER_ON_AUTOMATIC)   /* startup type - power-on automatic (+CALA alarm)                       */
#define STARTUP__STARTUP_TYPE__DOTA                (RESET__STARTUP_TYPE__DOTA              )   /* startup type - DOTA                                                   */
#define STARTUP__STARTUP_TYPE__RESTART             (RESET__STARTUP_TYPE__RESTART           )   /* startup type - restart            (AT+CFUN=1 for application restart) */
#define STARTUP__STARTUP_TYPE__REBOOT_MANUAL       (RESET__STARTUP_TYPE__REBOOT_MANUAL     )   /* startup type - reboot manual      (AT+CFUN=1 for anomaly)             */
#define STARTUP__STARTUP_TYPE__REBOOT_INTERNAL     (RESET__STARTUP_TYPE__REBOOT_INTERNAL   )   /* startup type - reboot internal    (e.g. watchdog, exception)          */

/* reset phone number - maximum phone number length */
#define STARTUP__MAX_LENGTH_RESET_PHONE_NUMBER     20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* reset result */
typedef struct
{
    bool  reset_result_stored;                                             // reset result stored [FALSE, TRUE]
} STARTUP__RESET_RESULT;


/* reset phone number */
typedef struct
{
    ascii reset_phone_number[STARTUP__MAX_LENGTH_RESET_PHONE_NUMBER + 1];  // reset phone number
} STARTUP__RESET_PHONE_NUMBER;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Startup_TaskStartup(void *argument);

/* get startup type */
u8   Startup_GetStartupType(void);

/* get recover status */
bool Startup_GetRecoverStatus(void);

/* get/set reset result */
void Startup_ResetResult_GetDefault     (STARTUP__RESET_RESULT       *ptr_data);
void Startup_ResetResult_Get            (STARTUP__RESET_RESULT       *ptr_data);
void Startup_ResetResult_Set            (STARTUP__RESET_RESULT       *ptr_data);

/* get/set reset phone number */
void Startup_ResetPhoneNumber_GetDefault(STARTUP__RESET_PHONE_NUMBER *ptr_data);
void Startup_ResetPhoneNumber_Get       (STARTUP__RESET_PHONE_NUMBER *ptr_data);
void Startup_ResetPhoneNumber_Set       (STARTUP__RESET_PHONE_NUMBER *ptr_data);




#endif
