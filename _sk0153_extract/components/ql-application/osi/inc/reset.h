/*=============================================================================
 * File       :  RESET.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - reset info
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __RESET_H__


#define __RESET_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* startup type */
#define RESET__STARTUP_TYPE__UNKNOWN                         0      /* startup type - unknown                                                */
#define RESET__STARTUP_TYPE__POWER_ON_MANUAL                 1      /* startup type - power-on manual                                        */
#define RESET__STARTUP_TYPE__POWER_ON_AUTOMATIC              2      /* startup type - power-on automatic (+CALA alarm)                       */
#define RESET__STARTUP_TYPE__DOTA                            3      /* startup type - DOTA                                                   */
#define RESET__STARTUP_TYPE__RESTART                         4      /* startup type - restart            (AT+CFUN=1 for application restart) */
#define RESET__STARTUP_TYPE__REBOOT_MANUAL                   5      /* startup type - reboot manual      (AT+CFUN=1 for anomaly)             */
#define RESET__STARTUP_TYPE__REBOOT_INTERNAL                 6      /* startup type - reboot internal    (e.g. watchdog, exception)          */


/* QL power-on reset type */
#define RESET__POWERON_RESET_TYPE__UNKNOWN                   0      /* reset   type - unknown                                                */
#define RESET__POWERON_RESET_TYPE__RESTART                   1      /* reset   type - restart            (AT+CFUN=1 for application restart) */
#define RESET__POWERON_RESET_TYPE__REBOOT_MANUAL             2      /* reset   type - reboot manual      (AT+CFUN=1 for anomaly)             */
#define RESET__POWERON_RESET_TYPE__REBOOT_INTERNAL           3      /* reset   type - reboot internal    (e.g. watchdog, exception)          */
#define RESET__POWERON_RESET_TYPE__REBOOT_DOTA               4      /* reset   type - reboot internal    (AT+CFUN=1 for DOTA)                */


/* DOTA result */
#define RESET__DOTA_RESULT__UNKNOWN                          0      /* DOTA result - unknown */
#define RESET__DOTA_RESULT__ERROR                            1      /* DOTA result - error   */
#define RESET__DOTA_RESULT__SUCCESS                          2      /* DOTA result - success */

/* DOTA result - error codes */
#define RESET__DOTA_RESULT__ERROR__UNKNOWN                   0      /* DOTA result error code - unknown                     */
#define RESET__DOTA_RESULT__ERROR__NO_ERROR                  1      /* DOTA result error code - no error                    */
#define RESET__DOTA_RESULT__ERROR__OTHER_ERROR               2      /* DOTA result error code - other error                 */
#define RESET__DOTA_RESULT__ERROR__NO_GSM_REG                3      /* DOTA result error code - no GSM network registration */
#define RESET__DOTA_RESULT__ERROR__NO_GPRS_APN               4      /* DOTA result error code - no GPRS APN   connection    */
#define RESET__DOTA_RESULT__ERROR__NO_FTP_SERVER             5      /* DOTA result error code - no FTP server connection    */
#define RESET__DOTA_RESULT__ERROR__NO_FTP_FILE_DOWNLOAD      6      /* DOTA result error code - no FTP file download        */
#define RESET__DOTA_RESULT__ERROR__NO_FILE_INSTALL           7      /* DOTA result error code - no file install             */


/* DOTA phone number - maximum phone number length */
#define RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER    20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* power-on reset type */
typedef u8    RESET__POWER_ON_RESET_TYPE;


/* DOTA result */
typedef struct
{
    bool  dota_result_stored;                                                        // DOTA result stored [FALSE, TRUE]
    u8    dota_result;                                                               // DOTA result
    u8    error_code;                                                                // DOTA result error code
} RESET__DOTA_RESULT;


/* DOTA phone number */
typedef struct
{
    ascii dota_phone_number[RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER + 1];  // DOTA phone number
} RESET__DOTA_PHONE_NUMBER;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* startup type */
u8   Reset_GetStartupType(void);

/* get/set power-on reset type */
void Reset_PowerOnResetType_GetDefault(RESET__POWER_ON_RESET_TYPE *ptr_data);
void Reset_PowerOnResetType_Get       (RESET__POWER_ON_RESET_TYPE *ptr_data);
void Reset_PowerOnResetType_Set       (RESET__POWER_ON_RESET_TYPE *ptr_data);

/* get/set DOTA result */
void Reset_DotaResult_GetDefault      (RESET__DOTA_RESULT         *ptr_data);
void Reset_DotaResult_Get             (RESET__DOTA_RESULT         *ptr_data);
void Reset_DotaResult_Set             (RESET__DOTA_RESULT         *ptr_data);

/* get/set DOTA phone number */
void Reset_DotaPhoneNumber_GetDefault (RESET__DOTA_PHONE_NUMBER   *ptr_data);
void Reset_DotaPhoneNumber_Get        (RESET__DOTA_PHONE_NUMBER   *ptr_data);
void Reset_DotaPhoneNumber_Set        (RESET__DOTA_PHONE_NUMBER   *ptr_data);




#endif
