/*=============================================================================
 * File       :  STARTUP.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - startup
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "calendar.h"
#include "charger.h"
#include "clock.h"
#include "counters.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "drvtemperature.h"
#include "fv_action.h"
#include "input_event.h"
#include "outputs.h"
#include "program_flash.h"
#include "queue_my.h"
#include "regulation.h"
#include "reset.h"
#include "rtc_alarm.h"
#include "sms_answer_event.h"
#include "startup.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING    200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* startup type */
static       u8                          Startup_StartupType = STARTUP__STARTUP_TYPE__UNKNOWN;

/* recover status indication */
static       bool                        Startup_RecoverStatus = FALSE;


/* reset result       (actual value and default value) */
static       STARTUP__RESET_RESULT       Startup_ResetResult;
static const STARTUP__RESET_RESULT       Startup_ResetResultDefault =
{
    FALSE,
};


/* reset phone number (actual value and default value) */
static       STARTUP__RESET_PHONE_NUMBER Startup_ResetPhoneNumber;
static const STARTUP__RESET_PHONE_NUMBER Startup_ResetPhoneNumberDefault =
{
    "",
};




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




/*=============================================================================
 * Function   : Startup_TaskStartup
 *
 * Description: startup task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Startup_TaskStartup(void *argument)
{
    RESET__POWER_ON_RESET_TYPE     poweron_reset_type;

    RESET__DOTA_RESULT             dota_result;
    RESET__DOTA_PHONE_NUMBER       dota_phone_number;

    QUEUE__FW_UPGRADE_QUEUE_RECORD record;
    CLOCK__TIME                    time;

    bool                           recover_status;
    bool                           var_crc_ok;
    bool                           result;

    bool                           result_1;
    bool                           result_2;
    bool                           result_3;
    bool                           result_4;
    bool                           result_5;
    bool                           result_6;
    bool                           result_7;
    bool                           result_8;
    bool                           result_9;
    bool                           result_10;
    bool                           result_11;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - STARTUP - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* get the startup type */
    Startup_StartupType = Reset_GetStartupType();



    /*---------------------------------------------------------------------------------------
     * DOTA management
     *---------------------------------------------------------------------------------------*/
    /* generate the FW upgrade record (if it is DOTA startup) */
    if (Startup_StartupType == STARTUP__STARTUP_TYPE__DOTA)
    {
        /* it is DOTA startup */

        /* get DOTA result       */
        /* get DOTA phone number */
        Reset_DotaResult_Get     (&dota_result      );
        Reset_DotaPhoneNumber_Get(&dota_phone_number);

        /* get the actual time */
        result = Clock_GetTime(&time);

        /* build the FW upgrade record */
        strncpy(record.sms_phone_number, dota_phone_number.dota_phone_number, 20);  /* SMS DOTA phone number */
        record.sms_phone_number[20] = 0x00;
        record.time       = time;                                                      /* time                  */
        record.result     = dota_result.dota_result;                                   /* FW upgrade result     */
        record.error_code = dota_result.error_code;                                    /* FW upgrade error code */

        /* put the FW upgrade record in the FW upgrade queue */
        result = Queue_FwUpgrade_PutRecord(&record);
    }


    /* clean the DOTA result and DOTA phone number (if it is DOTA startup) */
    if (Startup_StartupType == STARTUP__STARTUP_TYPE__DOTA)
    {
        /* it is DOTA startup */

        /* clean the DOTA result */
        dota_result.dota_result_stored = FALSE;
        dota_result.dota_result        = RESET__DOTA_RESULT__UNKNOWN;
        dota_result.error_code         = RESET__DOTA_RESULT__ERROR__UNKNOWN;
        Reset_DotaResult_Set(&dota_result);

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT);


        /* clean the DOTA phone number */
        strncpy(dota_phone_number.dota_phone_number, "", RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER);
        dota_phone_number.dota_phone_number[RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER] = 0x00;
        Reset_DotaPhoneNumber_Set(&dota_phone_number);

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER);
    }



    /*---------------------------------------------------------------------------------------
     * reset management
     *---------------------------------------------------------------------------------------*/
    /* generate the SMS answer event record for RESET command (if it is a reset for RESET command) */
    if (Startup_ResetResult.reset_result_stored)
    {
        /* it is reset for RESET command */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "Generate SMS answer event for RESET command", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* generate SMS answer event for RESET command */
        SmsAnswerEvent_GenerateSmsAnswerEvent(Startup_ResetPhoneNumber.reset_phone_number, SMS_ANSWER_EVENT__ANSWER_TYPE__RESET);
    }


    /* clean the reset result and reset phone number (if it is a reset for RESET command) */
    if (Startup_ResetResult.reset_result_stored)
    {
        /* it is reset for RESET command */

        /* clean the reset result */
        Startup_ResetResult.reset_result_stored = FALSE;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT);


        /* clean the reset phone number */
        strncpy(Startup_ResetPhoneNumber.reset_phone_number, "", STARTUP__MAX_LENGTH_RESET_PHONE_NUMBER);
        Startup_ResetPhoneNumber.reset_phone_number[STARTUP__MAX_LENGTH_RESET_PHONE_NUMBER] = 0x00;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER);
    }



    /*---------------------------------------------------------------------------------------
     * Recover internal status management
     *---------------------------------------------------------------------------------------*/
    /* verify if the startup type contemplates the recover of the internal status */
    switch (Startup_StartupType)
    {
        /* startup type - power-on manual */
        /* startup type - DOTA            */
        case STARTUP__STARTUP_TYPE__POWER_ON_MANUAL:
        case STARTUP__STARTUP_TYPE__DOTA:
            /* init with variables with default values */
            recover_status = FALSE;
            break;

        /* startup type - unknown                          */
        /* startup type - power-on automatic (+CALA alarm) */
        /* unknown value                                   */
        case STARTUP__STARTUP_TYPE__UNKNOWN:
        case STARTUP__STARTUP_TYPE__POWER_ON_AUTOMATIC:
        default:
            /* startup type not used by this application (not expected) */
            /* init with variables with default values */
            recover_status = FALSE;
            break;

        /* startup type - restart            (AT+CFUN=1 for application restart) */
        /* startup type - reboot manual      (AT+CFUN=1 for anomaly)             */
        /* startup type - reboot internal    (e.g. watchdog, exception)          */
        case STARTUP__STARTUP_TYPE__RESTART:
        case STARTUP__STARTUP_TYPE__REBOOT_MANUAL:
        case STARTUP__STARTUP_TYPE__REBOOT_INTERNAL:
            /* init with variables with recovered values */
            recover_status = TRUE;
            break;
    }


    /* verify variables CRC (if it is recovered startup) */
    if (recover_status)
    {
        result_1  = Outputs_VerifyVarCrc();
        result_2  = Regulation_VerifyVarCrc();
        result_3  = DrvGpio_VerifyVarCrc();
        result_4  = Charger_VerifyVarCrc();
        result_5  = RtcAlarm_VerifyVarCrc();
        result_6  = Calendar_VerifyVarCrc();
        result_7  = Queue_VerifyVarCrc();
        result_8  = DrvTemperature_VerifyVarCrc();
        result_9  = InputEvent_VerifyVarCrc();
        result_10 = FvAction_VerifyVarCrc();
        result_11 = Counters_VerifyVarCrc();

        if (
             (result_1 ) &&
             (result_2 ) &&
             (result_3 ) &&
             (result_4 ) &&
             (result_5 ) &&
             (result_6 ) &&
             (result_7 ) &&
             (result_8 ) &&
             (result_9 ) &&
             (result_10) &&
             (result_11)
           )
        {
            var_crc_ok = TRUE;
        }
        else
        {
            var_crc_ok = FALSE;
        }
    }
    else
    {
        var_crc_ok = TRUE;
    }


    if (recover_status)
    {
        if (var_crc_ok)
        {
            /* variables CRC correct */

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "Variables CRC correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* startup with    recover of the internal status (variables with recovered values) */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "Startup with recover of the internal status (variables with recovered values)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Startup_RecoverStatus = TRUE;
        }
        else
        {
            /* variables CRC not correct */

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* startup without recover of the internal status (variables with default values) */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "Startup without recover of the internal status (variables with default values)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Startup_RecoverStatus = FALSE;
        }
    }
    else
    {
        /* startup without recover of the internal status (variables with default values) */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STARTUP, DEBUG_TRACE_TYPE_LOW, "Startup without recover of the internal status (variables with default values)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Startup_RecoverStatus = FALSE;
    }



    /*---------------------------------------------------------------------------------------
     * Internal reboot management
     *---------------------------------------------------------------------------------------*/
  ///* set the type of power-on reset (in order to manage an internal reboot (e.g. watchdog) */
  //poweron_reset_type = RESET__POWERON_RESET_TYPE__REBOOT_INTERNAL;
    poweron_reset_type = RESET__POWERON_RESET_TYPE__UNKNOWN;
    Reset_PowerOnResetType_Set(&poweron_reset_type);

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE);

    /* delete the current task itself */
    ql_rtos_task_delete(NULL);
}




/*=============================================================================
 * Function   : Startup_GetStartupType
 *
 * Description: get the startup type
 * Input      : -
 * Output     : -
 *=============================================================================*/
u8 Startup_GetStartupType(void)
{
    return Startup_StartupType;
}




/*=============================================================================
 * Function   : Startup_GetRecoverStatus
 *
 * Description: get the recover status indication
 * Input      : -
 * Output     : -
 *=============================================================================*/
bool Startup_GetRecoverStatus(void)
{
    return Startup_RecoverStatus;
}




/*===========================================================================
 * Function   : Startup_ResetResult_GetDefault
 *
 * Description: get the default reset result
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Startup_ResetResult_GetDefault(STARTUP__RESET_RESULT *ptr_data)
{
    *ptr_data = Startup_ResetResultDefault;
}




/*===========================================================================
 * Function   : Startup_ResetResult_Get
 *
 * Description: get the reset result
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Startup_ResetResult_Get(STARTUP__RESET_RESULT *ptr_data)
{
    *ptr_data = Startup_ResetResult;
}




/*===========================================================================
 * Function   : Startup_ResetResult_Set
 *
 * Description: set the reset result
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Startup_ResetResult_Set(STARTUP__RESET_RESULT *ptr_data)
{
    Startup_ResetResult = *ptr_data;
}




/*===========================================================================
 * Function   : Startup_ResetPhoneNumber_GetDefault
 *
 * Description: get the default reset phone number
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Startup_ResetPhoneNumber_GetDefault(STARTUP__RESET_PHONE_NUMBER *ptr_data)
{
    *ptr_data = Startup_ResetPhoneNumberDefault;
}




/*===========================================================================
 * Function   : Startup_ResetPhoneNumber_Get
 *
 * Description: get the reset phone number
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Startup_ResetPhoneNumber_Get(STARTUP__RESET_PHONE_NUMBER *ptr_data)
{
    *ptr_data = Startup_ResetPhoneNumber;
}




/*===========================================================================
 * Function   : Startup_ResetPhoneNumber_Set
 *
 * Description: set the reset phone number
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Startup_ResetPhoneNumber_Set(STARTUP__RESET_PHONE_NUMBER *ptr_data)
{
    Startup_ResetPhoneNumber = *ptr_data;
}
