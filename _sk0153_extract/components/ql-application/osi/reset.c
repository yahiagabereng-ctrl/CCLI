/*=============================================================================
 * File       :  RESET.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - reset info
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDE
 *===========================================================================*/
/* standard includes */
#include <stdio.h>
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_power.h"

/* user     includes */
#include "debug_my.h"
#include "reset.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING    200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
//@@@
//static       POWERON_RESET_TYPE  Reset_PowerOnResetType __attribute__((section("UNINIT")));


/* startup mode */
static       RESET__POWER_ON_RESET_TYPE Reset_PowerOnResetType;
static const RESET__POWER_ON_RESET_TYPE Reset_PowerOnResetTypeDefault =
{
    RESET__POWERON_RESET_TYPE__UNKNOWN,
};


/* DOTA result (actual value and default value) */
static       RESET__DOTA_RESULT         Reset_DotaResult;
static const RESET__DOTA_RESULT         Reset_DotaResultDefault =
{
    FALSE,
    RESET__DOTA_RESULT__UNKNOWN,
    RESET__DOTA_RESULT__ERROR__UNKNOWN,
};


/* DOTA phone number */
static       RESET__DOTA_PHONE_NUMBER   Reset_DotaPhoneNumber;
static const RESET__DOTA_PHONE_NUMBER   Reset_DotaPhoneNumberDefault =
{
    "",
};


/* debug string */
static       ascii                      Reset_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*===========================================================================
 * FUNCTION PROTOTYPES
 *===========================================================================*/
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




/*===========================================================================
 * Function    : Reset_GetStartupType
 *
 * Description : get the startup type
 * Input       : -
 * Output      : - startup type
 *===========================================================================*/
u8 Reset_GetStartupType(void)
{
    ql_errcode_power           is_error;
    u8                         init_type;

    RESET__POWER_ON_RESET_TYPE poweron_reset_type;

    u8                         startup_type;

    bool                       dota_result_stored;
    u8                         dota_result;
    u8                         error_code;

    ascii                      dota_phone_number[RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER + 1];


    /* get ADL init type */
    is_error = ql_get_powerup_reason(&init_type);
    if (is_error)
    {
        init_type = QL_PWRUP_UNKNOWN;
    }

    /* get the power-on reset type */
    Reset_PowerOnResetType_Get(&poweron_reset_type);


    /* get DOTA result (if it is present) */
    dota_result_stored = Reset_DotaResult.dota_result_stored;
    dota_result        = Reset_DotaResult.dota_result;
    error_code         = Reset_DotaResult.error_code;

    snprintf(Reset_DebugString, sizeof(Reset_DebugString), "DOTA result - dota_result_stored: %d", dota_result_stored);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, Reset_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Reset_DebugString, sizeof(Reset_DebugString), "DOTA result - dota_result       : %d", dota_result       );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, Reset_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Reset_DebugString, sizeof(Reset_DebugString), "DOTA result - error_code        : %d", error_code        );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, Reset_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* get DOTA phone number (if it is present) */
    strncpy(dota_phone_number, Reset_DotaPhoneNumber.dota_phone_number, RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER);
    dota_phone_number[RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER] = 0x00;

    snprintf(Reset_DebugString, sizeof(Reset_DebugString), "DOTA phone number - phone number: \"%s\"", dota_phone_number);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, Reset_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* establish the startup type */
    switch (init_type)
    {
        /* Normal power-on */
        case QL_PWRUP_PWRKEY:
        case QL_PWRUP_CHARGE:
            if (dota_result_stored)
            {
                startup_type = RESET__STARTUP_TYPE__DOTA;                  /* startup type - DOTA */
            }
            else
            {
                if      (poweron_reset_type == RESET__POWERON_RESET_TYPE__RESTART        )
                    startup_type = RESET__STARTUP_TYPE__RESTART;           /* startup type - restart         (AT+CFUN=1 for application restart) */
                else if (poweron_reset_type == RESET__POWERON_RESET_TYPE__REBOOT_MANUAL  )
                    startup_type = RESET__STARTUP_TYPE__REBOOT_MANUAL;     /* startup type - reboot manual   (AT+CFUN=1 for anomaly)             */
                else if (poweron_reset_type == RESET__POWERON_RESET_TYPE__REBOOT_INTERNAL)
                    startup_type = RESET__STARTUP_TYPE__REBOOT_INTERNAL;   /* startup type - reboot internal (e.g. watchdog, exception)          */
                else if (poweron_reset_type == RESET__POWERON_RESET_TYPE__UNKNOWN)
                    startup_type = RESET__STARTUP_TYPE__POWER_ON_MANUAL;   /* startup type - power-on manual                                     */
                else
                    startup_type = RESET__STARTUP_TYPE__POWER_ON_MANUAL;   /* startup type - power-on manual                                     */
            }
            break;


        /* Power-on due to an RTC alarm */
        case QL_PWRUP_ALARM:
            startup_type = RESET__STARTUP_TYPE__POWER_ON_AUTOMATIC;        /* startup type - power-on automatic (+CALA alarm) */
            break;


        /* Reboot after an exception */
        case QL_PWRUP_WDG:
        case QL_PWRUP_PANIC:
            startup_type = RESET__STARTUP_TYPE__REBOOT_INTERNAL;           /* startup type - reboot internal (e.g. watchdog, exception) */
            break;


        /* Reboot after a successful install process */
        /* Reboot after an error in  install process */
        /* Unknown */
        case QL_PWRUP_UNKNOWN:
        case QL_PWRUP_PIN_RESET:
        case QL_PWRUP_PSM_WAKEUP:
        default:
            startup_type = RESET__STARTUP_TYPE__UNKNOWN;                   /* startup type - unknown */
            break;
    }



    switch (startup_type)
    {
        /* startup type - unknown */
        case RESET__STARTUP_TYPE__UNKNOWN:
        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__UNKNOWN\""           , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* startup type - power-on manual */
        case RESET__STARTUP_TYPE__POWER_ON_MANUAL:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__POWER_ON_MANUAL\""   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* startup type - power-on automatic (+CALA alarm) */
        case RESET__STARTUP_TYPE__POWER_ON_AUTOMATIC:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__POWER_ON_AUTOMATIC\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* startup type - DOTA */
        case RESET__STARTUP_TYPE__DOTA:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__DOTA\""              , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* startup type - restart (AT+CFUN=1 for application restart) */
        case RESET__STARTUP_TYPE__RESTART:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__RESTART\""           , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* startup type - reboot manual (AT+CFUN=1 for anomaly) */
        case RESET__STARTUP_TYPE__REBOOT_MANUAL:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__REBOOT_MANUAL\""     , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        /* startup type - reboot internal (e.g. watchdog, exception) */
        case RESET__STARTUP_TYPE__REBOOT_INTERNAL:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_RESET, DEBUG_TRACE_TYPE_LOW, "Startup type: \"RESET__STARTUP_TYPE__REBOOT_INTERNAL\""   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }



    return startup_type;
}




/*===========================================================================
 * Function   : Reset_PowerOnResetType_GetDefault
 *
 * Description: get the default power-on reset type
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Reset_PowerOnResetType_GetDefault(RESET__POWER_ON_RESET_TYPE *ptr_data)
{
    *ptr_data = Reset_PowerOnResetTypeDefault;
}




/*===========================================================================
 * Function   : Reset_PowerOnResetType_Get
 *
 * Description: get the power-on reset type
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Reset_PowerOnResetType_Get(RESET__POWER_ON_RESET_TYPE *ptr_data)
{
    *ptr_data = Reset_PowerOnResetType;
}




/*===========================================================================
 * Function   : Reset_PowerOnResetType_Set
 *
 * Description: set the power-on reset type
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Reset_PowerOnResetType_Set(RESET__POWER_ON_RESET_TYPE *ptr_data)
{
    Reset_PowerOnResetType = *ptr_data;
}




/*===========================================================================
 * Function   : Reset_DotaResult_GetDefault
 *
 * Description: get the default DOTA result
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Reset_DotaResult_GetDefault(RESET__DOTA_RESULT *ptr_data)
{
    *ptr_data = Reset_DotaResultDefault;
}




/*===========================================================================
 * Function   : Reset_DotaResult_Get
 *
 * Description: get the DOTA result
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Reset_DotaResult_Get(RESET__DOTA_RESULT *ptr_data)
{
    *ptr_data = Reset_DotaResult;
}




/*===========================================================================
 * Function   : Reset_DotaResult_Set
 *
 * Description: set the DOTA result
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Reset_DotaResult_Set(RESET__DOTA_RESULT *ptr_data)
{
    Reset_DotaResult = *ptr_data;
}




/*===========================================================================
 * Function   : Reset_DotaPhoneNumber_GetDefault
 *
 * Description: get the default DOTA phone number
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Reset_DotaPhoneNumber_GetDefault(RESET__DOTA_PHONE_NUMBER *ptr_data)
{
    *ptr_data = Reset_DotaPhoneNumberDefault;
}




/*===========================================================================
 * Function   : Reset_DotaPhoneNumber_Get
 *
 * Description: get the DOTA phone number
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Reset_DotaPhoneNumber_Get(RESET__DOTA_PHONE_NUMBER *ptr_data)
{
    *ptr_data = Reset_DotaPhoneNumber;
}




/*===========================================================================
 * Function   : Reset_DotaPhoneNumber_Set
 *
 * Description: set the DOTA phone number
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Reset_DotaPhoneNumber_Set(RESET__DOTA_PHONE_NUMBER *ptr_data)
{
    Reset_DotaPhoneNumber = *ptr_data;
}
