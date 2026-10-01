/*===========================================================================
 * File       :  PROGRAM_FLASH.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - backup flash manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *==========================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdlib.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_fs.h"

/* user     includes */
#include "adc.h"
#include "alarm_input.h"
#include "alarm_power.h"
#include "alarm_tmax.h"
#include "alarm_tmin.h"
#include "alarm.h"
#include "analog.h"
#include "boot.h"
#include "commands_ext.h"
#include "commands.h"
#include "counters.h"
#include "credit.h"
#include "debug_my.h"
#include "device_id.h"
#include "drvtemperature.h"
#include "energy.h"
#include "f_antifrost.h"
#include "f_chrono.h"
#include "f_regulation.h"
#include "forward.h"
#include "fv_action.h"
#include "fw_config.h"
#include "fwd.h"
#include "http_my.h"
#include "id.h"
#include "language.h"
#include "log_status.h"
#include "logs.h"
#include "messages.h"
#include "mode.h"
#include "myn.h"
#include "names.h"
#include "out_key.h"
#include "outputs.h"
#include "password.h"
#include "phone.h"
#include "phonebook.h"
#include "pod.h"
#include "program_flash.h"
#include "queue_my.h"
#include "regulation.h"
#include "report.h"
#include "reset.h"
#include "smtp.h"
#include "startup.h"
#include "synchronize.h"
#include "transmission_sms.h"
#include "typedef.h"
#include "user_gsm_retx.h"
#include "user_gsm_sync.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
#define PROGRAM_FLASH__FILE_PATH     "UFS:program.bin"

/* events */
#define EVENT_FLASH_BACKUP_TO_DO     (10400 | (QL_COMPONENT_APP_START << 16))     /* backup to do */

/* maximum number of writing attempts to flash */
#define NUM_MAX_WRITING_BACKUP       3

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING      200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii      ProgramFlash_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------
 * get default functions
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static void (* const ProgramFlash_Functions_GetDefault[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))Reset_DotaResult_GetDefault,                         // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT
    (void (*)(u8 *))Reset_DotaPhoneNumber_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER
    (void (*)(u8 *))Reset_PowerOnResetType_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE
    (void (*)(u8 *))Startup_ResetResult_GetDefault,                      // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT
    (void (*)(u8 *))Startup_ResetPhoneNumber_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER
    //---------------------------------------------------------
    (void (*)(u8 *))Language_Config_Language_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE
    (void (*)(u8 *))Pod_Config_Pod_GetDefault,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD
    (void (*)(u8 *))Commands_Config_CommandDetachment_GetDefault,        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT
    (void (*)(u8 *))Commands_Config_CommandRestore_GetDefault,           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE
    (void (*)(u8 *))Commands_Config_CommandStatus_GetDefault,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS
    (void (*)(u8 *))Commands_Config_CommandReset_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET
    (void (*)(u8 *))Phone_Config_ApnParameters_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN
    (void (*)(u8 *))Phone_Config_DnsParameters_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS
    (void (*)(u8 *))Phone_Config_SimPin_GetDefault,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN
    (void (*)(u8 *))Myn_Config_MynNumber_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER
    (void (*)(u8 *))Phonebook_Config_Phonebook_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK
    (void (*)(u8 *))Fwd_Config_FwdNumber_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER
    (void (*)(u8 *))Http_Config_Host_GetDefault,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST
    (void (*)(u8 *))Http_Config_HostAccount2_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2
    (void (*)(u8 *))Http_Config_HostTx_GetDefault,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX
    (void (*)(u8 *))Http_Config_HostData_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA
    (void (*)(u8 *))Password_Config_Password_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD
    (void (*)(u8 *))OutKey_Config_Buttons_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS
    (void (*)(u8 *))Credit_Config_Credit_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT
    (void (*)(u8 *))Counters_Config_InputC1_GetDefault,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1
    (void (*)(u8 *))Counters_Config_InputC2_GetDefault,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2
    (void (*)(u8 *))AlarmTMax_Config_AlarmTemperatureMaxInt_GetDefault,  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_TemperatureMaxInt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_AlarmTemperatureMaxExt_GetDefault,  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_TemperatureMaxExt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_AlarmTemperatureMinInt_GetDefault,  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_TemperatureMinInt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_AlarmTemperatureMinExt_GetDefault,  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_TemperatureMinExt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn1_GetDefault,          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn1_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn2_GetDefault,          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn2_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn3_GetDefault,          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn3_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn3Type_GetDefault,           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn4_GetDefault,          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn4_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn4Type_GetDefault,           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE
    (void (*)(u8 *))AlarmPower_Config_AlarmPower_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM
    (void (*)(u8 *))Phone_Config_PhoneActivityCounters_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS
    (void (*)(u8 *))Phone_Config_SmsDelay_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY
    (void (*)(u8 *))Counters_Config_InputC1Bands_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS
    (void (*)(u8 *))Counters_Config_InputC2Bands_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS
    (void (*)(u8 *))Commands_Config_AnswerDetachment_GetDefault,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT
    (void (*)(u8 *))Commands_Config_AnswerRestore_GetDefault,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE
    (void (*)(u8 *))Commands_Config_AnswerStatus_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS
    (void (*)(u8 *))Commands_Config_AnswerReset_GetDefault,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET
    (void (*)(u8 *))Alarm_Config_AlarmMode_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE
    (void (*)(u8 *))Report_Config_Report_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT
    (void (*)(u8 *))FvAction_Config_AutoRestore_GetDefault,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE
    (void (*)(u8 *))FvAction_Config_AutoReconnection_GetDefault,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION
    (void (*)(u8 *))Adc_Config_Adc0Calibration_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION
    (void (*)(u8 *))Adc_Config_Adc1Calibration_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION
    (void (*)(u8 *))Mode_Config_ModeTemperatureInt_GetDefault,           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT
    (void (*)(u8 *))Mode_Config_ModeTemperatureExt_GetDefault,           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT
    (void (*)(u8 *))CommandsExt_Config_CommandsExt_GetDefault,           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT
    (void (*)(u8 *))DrvTemperature_Config_CalibrationInt_GetDefault,     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT
    (void (*)(u8 *))DrvTemperature_Config_CalibrationExt_GetDefault,     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT
    (void (*)(u8 *))LogStatus_Config_StatusLog_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS
    (void (*)(u8 *))Energy_Config_AdcValue_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY
    (void (*)(u8 *))FAntifrost__Config_FAntifrostInt_GetDefault,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT
    (void (*)(u8 *))FAntifrost__Config_FAntifrostExt_GetDefault,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT
    (void (*)(u8 *))FChrono__Config_FChronoInt_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT
    (void (*)(u8 *))FChrono__Config_FChronoExt_GetDefault,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT
    (void (*)(u8 *))Names_Config_NamesOut1_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1
    (void (*)(u8 *))Names_Config_NamesOut2_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2
    (void (*)(u8 *))Names_Config_NamesIn1_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1
    (void (*)(u8 *))Names_Config_NamesIn2_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2
    (void (*)(u8 *))Names_Config_NamesIn3_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3
    (void (*)(u8 *))Names_Config_NamesIn4_GetDefault,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4
    (void (*)(u8 *))Names_Config_NamesTmin_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN
    (void (*)(u8 *))Names_Config_NamesTmax_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX
    (void (*)(u8 *))Names_Config_NamesTminExt_GetDefault,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT
    (void (*)(u8 *))Names_Config_NamesTmaxExt_GetDefault,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT
    (void (*)(u8 *))Names_Config_NamesC1_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1
    (void (*)(u8 *))Names_Config_NamesC2_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2
    (void (*)(u8 *))Messages_Config_MessagesIn1_GetDefault,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1
    (void (*)(u8 *))Messages_Config_MessagesIn2_GetDefault,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2
    (void (*)(u8 *))Messages_Config_MessagesIn3_GetDefault,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3
    (void (*)(u8 *))Messages_Config_MessagesIn4_GetDefault,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4
    (void (*)(u8 *))Messages_Config_MessagesTmin_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN
    (void (*)(u8 *))Messages_Config_MessagesTmax_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX
    (void (*)(u8 *))Messages_Config_MessagesTminExt_GetDefault,          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT
    (void (*)(u8 *))Messages_Config_MessagesTmaxExt_GetDefault,          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT
    (void (*)(u8 *))Outputs_Config_TimerOutputInt_GetDefault,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT
    (void (*)(u8 *))Outputs_Config_TimerOutputExt_GetDefault,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT
    (void (*)(u8 *))Regulation_Config_ActiveStatusOut1_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1
    (void (*)(u8 *))Regulation_Config_ActiveStatusOut2_GetDefault,       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2
    (void (*)(u8 *))Forward_Config_Forward_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD
    (void (*)(u8 *))Messages_Config_MessagesIn1R_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R
    (void (*)(u8 *))Messages_Config_MessagesIn2R_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R
    (void (*)(u8 *))Messages_Config_MessagesIn3R_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R
    (void (*)(u8 *))Messages_Config_MessagesIn4R_GetDefault,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R
    (void (*)(u8 *))Messages_Config_MessagesTminR_GetDefault,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R
    (void (*)(u8 *))Messages_Config_MessagesTmaxR_GetDefault,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R
    (void (*)(u8 *))Messages_Config_MessagesTminExtR_GetDefault,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R
    (void (*)(u8 *))Messages_Config_MessagesTmaxExtR_GetDefault,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R
    (void (*)(u8 *))Analog_Config_AnalogMode_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE
    //---------------------------------------------------------
    (void (*)(u8 *))Phone_PhoneActivity_GetDefault,                      // PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY
    //---------------------------------------------------------
    (void (*)(u8 *))Queue_FwUpgradeQueue_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE
    (void (*)(u8 *))Queue_SmsAnswerToCrsQueue_GetDefault,                // PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE
    (void (*)(u8 *))Queue_FileStatusLogQueue_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE
    (void (*)(u8 *))Queue_FileAlarmQueue_GetDefault,                     // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE
    //---------------------------------------------------------
    (void (*)(u8 *))Alarm_AlarmCounters_GetDefault,                      // PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS
    (void (*)(u8 *))AlarmPower_Status_LastPowerAlarm_GetDefault,         // PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM
    //---------------------------------------------------------
    (void (*)(u8 *))Synchronize_SmsCounter_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER
    (void (*)(u8 *))Synchronize_SynchronizeTime_GetDefault,              // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME
    //---------------------------------------------------------
    (void (*)(u8 *))UserGsmSync_RandomConfig_Synchronize_GetDefault,     // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE
    (void (*)(u8 *))UserGsmRetx_RandomConfig_Retx_GetDefault,            // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION
    //---------------------------------------------------------
    (void (*)(u8 *))Clock_RtcBackupTime_GetDefault,                      // PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME
    //---------------------------------------------------------
    (void (*)(u8 *))UserGsmRetx_RetransmissionStatus_GetDefault,         // PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))AlarmTMax_Status_TemperatureMaxInt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS
    (void (*)(u8 *))AlarmTMax_Status_TemperatureMaxExt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS
    (void (*)(u8 *))AlarmTMin_Status_TemperatureMinInt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS
    (void (*)(u8 *))AlarmTMin_Status_TemperatureMinExt_GetDefault,       // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))AlarmInput_Status_InputIn1_GetDefault,               // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn2_GetDefault,               // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn3_GetDefault,               // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn4_GetDefault,               // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))Credit_Status_AvailableCredit_GetDefault,            // PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT
    //---------------------------------------------------------
    (void (*)(u8 *))Outputs_Status_OutputOut1_GetDefault,                // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS
    (void (*)(u8 *))Outputs_Status_OutputOut2_GetDefault,                // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))Logs_RxCommmand_GetDefault,                          // PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG
    //---------------------------------------------------------
    (void (*)(u8 *))FRegulation_Status_FRegulationInt_GetDefault,        // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS
    (void (*)(u8 *))FRegulation_Status_FRegulationExt_GetDefault,        // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))DeviceId_Config_DeviceId_GetDefault,                 // PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID
    //---------------------------------------------------------
    (void (*)(u8 *))Counters_Status_InputC1_GetDefault,                  // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1
    (void (*)(u8 *))Counters_Status_InputC2_GetDefault,                  // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2
    //---------------------------------------------------------
    (void (*)(u8 *))Id_Config_SerialNumber_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER
    //---------------------------------------------------------
    (void (*)(u8 *))Energy_Status_AdcValue_GetDefault,                   // PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))FwConfig_IdModel_GetDefault,                         // PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * get functions
 *---------------------------------------------------------*/
static void (* const ProgramFlash_Functions_Get[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))Reset_DotaResult_Get,                                // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT
    (void (*)(u8 *))Reset_DotaPhoneNumber_Get,                           // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER
    (void (*)(u8 *))Reset_PowerOnResetType_Get,                          // PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE
    (void (*)(u8 *))Startup_ResetResult_Get,                             // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT
    (void (*)(u8 *))Startup_ResetPhoneNumber_Get,                        // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER
    //---------------------------------------------------------
    (void (*)(u8 *))Language_Config_Language_Get,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE
    (void (*)(u8 *))Pod_Config_Pod_Get,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD
    (void (*)(u8 *))Commands_Config_CommandDetachment_Get,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT
    (void (*)(u8 *))Commands_Config_CommandRestore_Get,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE
    (void (*)(u8 *))Commands_Config_CommandStatus_Get,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS
    (void (*)(u8 *))Commands_Config_CommandReset_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET
    (void (*)(u8 *))Phone_Config_ApnParameters_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN
    (void (*)(u8 *))Phone_Config_DnsParameters_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS
    (void (*)(u8 *))Phone_Config_SimPin_Get,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN
    (void (*)(u8 *))Myn_Config_MynNumber_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER
    (void (*)(u8 *))Phonebook_Config_Phonebook_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK
    (void (*)(u8 *))Fwd_Config_FwdNumber_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER
    (void (*)(u8 *))Http_Config_Host_Get,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST
    (void (*)(u8 *))Http_Config_HostAccount2_Get,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2
    (void (*)(u8 *))Http_Config_HostTx_Get,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX
    (void (*)(u8 *))Http_Config_HostData_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA
    (void (*)(u8 *))Password_Config_Password_Get,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD
    (void (*)(u8 *))OutKey_Config_Buttons_Get,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS
    (void (*)(u8 *))Credit_Config_Credit_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT
    (void (*)(u8 *))Counters_Config_InputC1_Get,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1
    (void (*)(u8 *))Counters_Config_InputC2_Get,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2
    (void (*)(u8 *))AlarmTMax_Config_AlarmTemperatureMaxInt_Get,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_TemperatureMaxInt_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_AlarmTemperatureMaxExt_Get,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_TemperatureMaxExt_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_AlarmTemperatureMinInt_Get,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_TemperatureMinInt_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_AlarmTemperatureMinExt_Get,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_TemperatureMinExt_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn1_Get,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn1_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn2_Get,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn2_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn3_Get,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn3_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn3Type_Get,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn4_Get,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn4_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn4Type_Get,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE
    (void (*)(u8 *))AlarmPower_Config_AlarmPower_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM
    (void (*)(u8 *))Phone_Config_PhoneActivityCounters_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS
    (void (*)(u8 *))Phone_Config_SmsDelay_Get,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY
    (void (*)(u8 *))Counters_Config_InputC1Bands_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS
    (void (*)(u8 *))Counters_Config_InputC2Bands_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS
    (void (*)(u8 *))Commands_Config_AnswerDetachment_Get,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT
    (void (*)(u8 *))Commands_Config_AnswerRestore_Get,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE
    (void (*)(u8 *))Commands_Config_AnswerStatus_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS
    (void (*)(u8 *))Commands_Config_AnswerReset_Get,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET
    (void (*)(u8 *))Alarm_Config_AlarmMode_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE
    (void (*)(u8 *))Report_Config_Report_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT
    (void (*)(u8 *))FvAction_Config_AutoRestore_Get,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE
    (void (*)(u8 *))FvAction_Config_AutoReconnection_Get,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION
    (void (*)(u8 *))Adc_Config_Adc0Calibration_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION
    (void (*)(u8 *))Adc_Config_Adc1Calibration_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION
    (void (*)(u8 *))Mode_Config_ModeTemperatureInt_Get,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT
    (void (*)(u8 *))Mode_Config_ModeTemperatureExt_Get,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT
    (void (*)(u8 *))CommandsExt_Config_CommandsExt_Get,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT
    (void (*)(u8 *))DrvTemperature_Config_CalibrationInt_Get,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT
    (void (*)(u8 *))DrvTemperature_Config_CalibrationExt_Get,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT
    (void (*)(u8 *))LogStatus_Config_StatusLog_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS
    (void (*)(u8 *))Energy_Config_AdcValue_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY
    (void (*)(u8 *))FAntifrost__Config_FAntifrostInt_Get,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT
    (void (*)(u8 *))FAntifrost__Config_FAntifrostExt_Get,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT
    (void (*)(u8 *))FChrono__Config_FChronoInt_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT
    (void (*)(u8 *))FChrono__Config_FChronoExt_Get,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT
    (void (*)(u8 *))Names_Config_NamesOut1_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1
    (void (*)(u8 *))Names_Config_NamesOut2_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2
    (void (*)(u8 *))Names_Config_NamesIn1_Get,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1
    (void (*)(u8 *))Names_Config_NamesIn2_Get,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2
    (void (*)(u8 *))Names_Config_NamesIn3_Get,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3
    (void (*)(u8 *))Names_Config_NamesIn4_Get,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4
    (void (*)(u8 *))Names_Config_NamesTmin_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN
    (void (*)(u8 *))Names_Config_NamesTmax_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX
    (void (*)(u8 *))Names_Config_NamesTminExt_Get,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT
    (void (*)(u8 *))Names_Config_NamesTmaxExt_Get,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT
    (void (*)(u8 *))Names_Config_NamesC1_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1
    (void (*)(u8 *))Names_Config_NamesC2_Get,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2
    (void (*)(u8 *))Messages_Config_MessagesIn1_Get,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1
    (void (*)(u8 *))Messages_Config_MessagesIn2_Get,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2
    (void (*)(u8 *))Messages_Config_MessagesIn3_Get,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3
    (void (*)(u8 *))Messages_Config_MessagesIn4_Get,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4
    (void (*)(u8 *))Messages_Config_MessagesTmin_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN
    (void (*)(u8 *))Messages_Config_MessagesTmax_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX
    (void (*)(u8 *))Messages_Config_MessagesTminExt_Get,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT
    (void (*)(u8 *))Messages_Config_MessagesTmaxExt_Get,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT
    (void (*)(u8 *))Outputs_Config_TimerOutputInt_Get,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT
    (void (*)(u8 *))Outputs_Config_TimerOutputExt_Get,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT
    (void (*)(u8 *))Regulation_Config_ActiveStatusOut1_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1
    (void (*)(u8 *))Regulation_Config_ActiveStatusOut2_Get,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2
    (void (*)(u8 *))Forward_Config_Forward_Get,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD
    (void (*)(u8 *))Messages_Config_MessagesIn1R_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R
    (void (*)(u8 *))Messages_Config_MessagesIn2R_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R
    (void (*)(u8 *))Messages_Config_MessagesIn3R_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R
    (void (*)(u8 *))Messages_Config_MessagesIn4R_Get,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R
    (void (*)(u8 *))Messages_Config_MessagesTminR_Get,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R
    (void (*)(u8 *))Messages_Config_MessagesTmaxR_Get,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R
    (void (*)(u8 *))Messages_Config_MessagesTminExtR_Get,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R
    (void (*)(u8 *))Messages_Config_MessagesTmaxExtR_Get,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R
    (void (*)(u8 *))Analog_Config_AnalogMode_Get,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE
    //---------------------------------------------------------
    (void (*)(u8 *))Phone_PhoneActivity_Get,                             // PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY
    //---------------------------------------------------------
    (void (*)(u8 *))Queue_FwUpgradeQueue_Get,                            // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE
    (void (*)(u8 *))Queue_SmsAnswerToCrsQueue_Get,                       // PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE
    (void (*)(u8 *))Queue_FileStatusLogQueue_Get,                        // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE
    (void (*)(u8 *))Queue_FileAlarmQueue_Get,                            // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE
    //---------------------------------------------------------
    (void (*)(u8 *))Alarm_AlarmCounters_Get,                             // PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS
    (void (*)(u8 *))AlarmPower_Status_LastPowerAlarm_Get,                // PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM
    //---------------------------------------------------------
    (void (*)(u8 *))Synchronize_SmsCounter_Get,                          // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER
    (void (*)(u8 *))Synchronize_SynchronizeTime_Get,                     // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME
    //---------------------------------------------------------
    (void (*)(u8 *))UserGsmSync_RandomConfig_Synchronize_Get,            // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE
    (void (*)(u8 *))UserGsmRetx_RandomConfig_Retx_Get,                   // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION
    //---------------------------------------------------------
    (void (*)(u8 *))Clock_RtcBackupTime_Get,                             // PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME
    //---------------------------------------------------------
    (void (*)(u8 *))UserGsmRetx_RetransmissionStatus_Get,                // PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))AlarmTMax_Status_TemperatureMaxInt_Get,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS
    (void (*)(u8 *))AlarmTMax_Status_TemperatureMaxExt_Get,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS
    (void (*)(u8 *))AlarmTMin_Status_TemperatureMinInt_Get,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS
    (void (*)(u8 *))AlarmTMin_Status_TemperatureMinExt_Get,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))AlarmInput_Status_InputIn1_Get,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn2_Get,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn3_Get,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn4_Get,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))Credit_Status_AvailableCredit_Get,                   // PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT
    //---------------------------------------------------------
    (void (*)(u8 *))Outputs_Status_OutputOut1_Get,                       // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS
    (void (*)(u8 *))Outputs_Status_OutputOut2_Get,                       // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))Logs_RxCommmand_Get,                                 // PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG
    //---------------------------------------------------------
    (void (*)(u8 *))FRegulation_Status_FRegulationInt_Get,               // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS
    (void (*)(u8 *))FRegulation_Status_FRegulationExt_Get,               // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))DeviceId_Config_DeviceId_Get,                        // PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID
    //---------------------------------------------------------
    (void (*)(u8 *))Counters_Status_InputC1_Get,                         // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1
    (void (*)(u8 *))Counters_Status_InputC2_Get,                         // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2
    //---------------------------------------------------------
    (void (*)(u8 *))Id_Config_SerialNumber_Get,                          // PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER
    //---------------------------------------------------------
    (void (*)(u8 *))Energy_Status_AdcValue_Get,                          // PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))FwConfig_IdModel_Get,                                // PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * set functions
 *---------------------------------------------------------*/
static void (* const ProgramFlash_Functions_Set[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))Reset_DotaResult_Set,                                // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT
    (void (*)(u8 *))Reset_DotaPhoneNumber_Set,                           // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER
    (void (*)(u8 *))Reset_PowerOnResetType_Set,                          // PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE
    (void (*)(u8 *))Startup_ResetResult_Set,                             // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT
    (void (*)(u8 *))Startup_ResetPhoneNumber_Set,                        // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER
    //---------------------------------------------------------
    (void (*)(u8 *))Language_Config_Language_Set,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE
    (void (*)(u8 *))Pod_Config_Pod_Set,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD
    (void (*)(u8 *))Commands_Config_CommandDetachment_Set,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT
    (void (*)(u8 *))Commands_Config_CommandRestore_Set,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE
    (void (*)(u8 *))Commands_Config_CommandStatus_Set,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS
    (void (*)(u8 *))Commands_Config_CommandReset_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET
    (void (*)(u8 *))Phone_Config_ApnParameters_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN
    (void (*)(u8 *))Phone_Config_DnsParameters_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS
    (void (*)(u8 *))Phone_Config_SimPin_Set,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN
    (void (*)(u8 *))Myn_Config_MynNumber_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER
    (void (*)(u8 *))Phonebook_Config_Phonebook_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK
    (void (*)(u8 *))Fwd_Config_FwdNumber_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER
    (void (*)(u8 *))Http_Config_Host_Set,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST
    (void (*)(u8 *))Http_Config_HostAccount2_Set,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2
    (void (*)(u8 *))Http_Config_HostTx_Set,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX
    (void (*)(u8 *))Http_Config_HostData_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA
    (void (*)(u8 *))Password_Config_Password_Set,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD
    (void (*)(u8 *))OutKey_Config_Buttons_Set,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS
    (void (*)(u8 *))Credit_Config_Credit_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT
    (void (*)(u8 *))Counters_Config_InputC1_Set,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1
    (void (*)(u8 *))Counters_Config_InputC2_Set,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2
    (void (*)(u8 *))AlarmTMax_Config_AlarmTemperatureMaxInt_Set,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_TemperatureMaxInt_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_AlarmTemperatureMaxExt_Set,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMax_Config_TemperatureMaxExt_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_AlarmTemperatureMinInt_Set,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_TemperatureMinInt_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_AlarmTemperatureMinExt_Set,         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM
    (void (*)(u8 *))AlarmTMin_Config_TemperatureMinExt_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn1_Set,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn1_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn2_Set,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn2_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn3_Set,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn3_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn3Type_Set,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE
    (void (*)(u8 *))AlarmInput_Config_AlarmInputIn4_Set,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn4_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM
    (void (*)(u8 *))AlarmInput_Config_InputIn4Type_Set,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE
    (void (*)(u8 *))AlarmPower_Config_AlarmPower_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM
    (void (*)(u8 *))Phone_Config_PhoneActivityCounters_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS
    (void (*)(u8 *))Phone_Config_SmsDelay_Set,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY
    (void (*)(u8 *))Counters_Config_InputC1Bands_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS
    (void (*)(u8 *))Counters_Config_InputC2Bands_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS
    (void (*)(u8 *))Commands_Config_AnswerDetachment_Set,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT
    (void (*)(u8 *))Commands_Config_AnswerRestore_Set,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE
    (void (*)(u8 *))Commands_Config_AnswerStatus_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS
    (void (*)(u8 *))Commands_Config_AnswerReset_Set,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET
    (void (*)(u8 *))Alarm_Config_AlarmMode_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE
    (void (*)(u8 *))Report_Config_Report_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT
    (void (*)(u8 *))FvAction_Config_AutoRestore_Set,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE
    (void (*)(u8 *))FvAction_Config_AutoReconnection_Set,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION
    (void (*)(u8 *))Adc_Config_Adc0Calibration_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION
    (void (*)(u8 *))Adc_Config_Adc1Calibration_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION
    (void (*)(u8 *))Mode_Config_ModeTemperatureInt_Set,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT
    (void (*)(u8 *))Mode_Config_ModeTemperatureExt_Set,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT
    (void (*)(u8 *))CommandsExt_Config_CommandsExt_Set,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT
    (void (*)(u8 *))DrvTemperature_Config_CalibrationInt_Set,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT
    (void (*)(u8 *))DrvTemperature_Config_CalibrationExt_Set,            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT
    (void (*)(u8 *))LogStatus_Config_StatusLog_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS
    (void (*)(u8 *))Energy_Config_AdcValue_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY
    (void (*)(u8 *))FAntifrost__Config_FAntifrostInt_Set,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT
    (void (*)(u8 *))FAntifrost__Config_FAntifrostExt_Set,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT
    (void (*)(u8 *))FChrono__Config_FChronoInt_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT
    (void (*)(u8 *))FChrono__Config_FChronoExt_Set,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT
    (void (*)(u8 *))Names_Config_NamesOut1_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1
    (void (*)(u8 *))Names_Config_NamesOut2_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2
    (void (*)(u8 *))Names_Config_NamesIn1_Set,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1
    (void (*)(u8 *))Names_Config_NamesIn2_Set,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2
    (void (*)(u8 *))Names_Config_NamesIn3_Set,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3
    (void (*)(u8 *))Names_Config_NamesIn4_Set,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4
    (void (*)(u8 *))Names_Config_NamesTmin_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN
    (void (*)(u8 *))Names_Config_NamesTmax_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX
    (void (*)(u8 *))Names_Config_NamesTminExt_Set,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT
    (void (*)(u8 *))Names_Config_NamesTmaxExt_Set,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT
    (void (*)(u8 *))Names_Config_NamesC1_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1
    (void (*)(u8 *))Names_Config_NamesC2_Set,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2
    (void (*)(u8 *))Messages_Config_MessagesIn1_Set,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1
    (void (*)(u8 *))Messages_Config_MessagesIn2_Set,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2
    (void (*)(u8 *))Messages_Config_MessagesIn3_Set,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3
    (void (*)(u8 *))Messages_Config_MessagesIn4_Set,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4
    (void (*)(u8 *))Messages_Config_MessagesTmin_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN
    (void (*)(u8 *))Messages_Config_MessagesTmax_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX
    (void (*)(u8 *))Messages_Config_MessagesTminExt_Set,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT
    (void (*)(u8 *))Messages_Config_MessagesTmaxExt_Set,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT
    (void (*)(u8 *))Outputs_Config_TimerOutputInt_Set,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT
    (void (*)(u8 *))Outputs_Config_TimerOutputExt_Set,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT
    (void (*)(u8 *))Regulation_Config_ActiveStatusOut1_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1
    (void (*)(u8 *))Regulation_Config_ActiveStatusOut2_Set,              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2
    (void (*)(u8 *))Forward_Config_Forward_Set,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD
    (void (*)(u8 *))Messages_Config_MessagesIn1R_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R
    (void (*)(u8 *))Messages_Config_MessagesIn2R_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R
    (void (*)(u8 *))Messages_Config_MessagesIn3R_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R
    (void (*)(u8 *))Messages_Config_MessagesIn4R_Set,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R
    (void (*)(u8 *))Messages_Config_MessagesTminR_Set,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R
    (void (*)(u8 *))Messages_Config_MessagesTmaxR_Set,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R
    (void (*)(u8 *))Messages_Config_MessagesTminExtR_Set,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R
    (void (*)(u8 *))Messages_Config_MessagesTmaxExtR_Set,                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R
    (void (*)(u8 *))Analog_Config_AnalogMode_Set,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE
    //---------------------------------------------------------
    (void (*)(u8 *))Phone_PhoneActivity_Set,                             // PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY
    //---------------------------------------------------------
    (void (*)(u8 *))Queue_FwUpgradeQueue_Set,                            // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE
    (void (*)(u8 *))Queue_SmsAnswerToCrsQueue_Set,                       // PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE
    (void (*)(u8 *))Queue_FileStatusLogQueue_Set,                        // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE
    (void (*)(u8 *))Queue_FileAlarmQueue_Set,                            // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE
    //---------------------------------------------------------
    (void (*)(u8 *))Alarm_AlarmCounters_Set,                             // PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS
    (void (*)(u8 *))AlarmPower_Status_LastPowerAlarm_Set,                // PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM
    //---------------------------------------------------------
    (void (*)(u8 *))Synchronize_SmsCounter_Set,                          // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER
    (void (*)(u8 *))Synchronize_SynchronizeTime_Set,                     // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME
    //---------------------------------------------------------
    (void (*)(u8 *))UserGsmSync_RandomConfig_Synchronize_Set,            // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE
    (void (*)(u8 *))UserGsmRetx_RandomConfig_Retx_Set,                   // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION
    //---------------------------------------------------------
    (void (*)(u8 *))Clock_RtcBackupTime_Set,                             // PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME
    //---------------------------------------------------------
    (void (*)(u8 *))UserGsmRetx_RetransmissionStatus_Set,                // PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))AlarmTMax_Status_TemperatureMaxInt_Set,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS
    (void (*)(u8 *))AlarmTMax_Status_TemperatureMaxExt_Set,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS
    (void (*)(u8 *))AlarmTMin_Status_TemperatureMinInt_Set,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS
    (void (*)(u8 *))AlarmTMin_Status_TemperatureMinExt_Set,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))AlarmInput_Status_InputIn1_Set,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn2_Set,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn3_Set,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS
    (void (*)(u8 *))AlarmInput_Status_InputIn4_Set,                      // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))Credit_Status_AvailableCredit_Set,                   // PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT
    //---------------------------------------------------------
    (void (*)(u8 *))Outputs_Status_OutputOut1_Set,                       // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS
    (void (*)(u8 *))Outputs_Status_OutputOut2_Set,                       // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))Logs_RxCommmand_Set,                                 // PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG
    //---------------------------------------------------------
    (void (*)(u8 *))FRegulation_Status_FRegulationInt_Set,               // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS
    (void (*)(u8 *))FRegulation_Status_FRegulationExt_Set,               // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))DeviceId_Config_DeviceId_Set,                        // PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID
    //---------------------------------------------------------
    (void (*)(u8 *))Counters_Status_InputC1_Set,                         // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1
    (void (*)(u8 *))Counters_Status_InputC2_Set,                         // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2
    //---------------------------------------------------------
    (void (*)(u8 *))Id_Config_SerialNumber_Set,                          // PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER
    //---------------------------------------------------------
    (void (*)(u8 *))Energy_Status_AdcValue_Set,                          // PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS
    //---------------------------------------------------------
    (void (*)(u8 *))FwConfig_IdModel_Set,                                // PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * data address
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static const u16 ProgramFlash_FileOffset[] =
{
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__DOTA_RESULT,                             // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT
    PROGRAM_FLASH__FILE_OFFSET__DOTA_PHONE_NUMBER,                       // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER
    PROGRAM_FLASH__FILE_OFFSET__POWERON_RESET_TYPE,                      // PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE
    PROGRAM_FLASH__FILE_OFFSET__RESET_RESULT,                            // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT
    PROGRAM_FLASH__FILE_OFFSET__RESET_PHONE_NUMBER,                      // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__LANGUAGE,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE
    PROGRAM_FLASH__FILE_OFFSET__POD,                                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD
    PROGRAM_FLASH__FILE_OFFSET__COMMAND_DETACHMENT,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT
    PROGRAM_FLASH__FILE_OFFSET__COMMAND_RESTORE,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE
    PROGRAM_FLASH__FILE_OFFSET__COMMAND_STATUS,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS
    PROGRAM_FLASH__FILE_OFFSET__COMMAND_RESET,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET
    PROGRAM_FLASH__FILE_OFFSET__GPRS_APN,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN
    PROGRAM_FLASH__FILE_OFFSET__GPRS_DNS,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS
    PROGRAM_FLASH__FILE_OFFSET__SIM_PIN,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN
    PROGRAM_FLASH__FILE_OFFSET__MYN_NUMBER,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER
    PROGRAM_FLASH__FILE_OFFSET__PHONEBOOK,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK
    PROGRAM_FLASH__FILE_OFFSET__FWD_NUMBER,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER
    PROGRAM_FLASH__FILE_OFFSET__HOST,                                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST
    PROGRAM_FLASH__FILE_OFFSET__HOST_ACCOUNT_2,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2
    PROGRAM_FLASH__FILE_OFFSET__HOST_TX,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX
    PROGRAM_FLASH__FILE_OFFSET__HOST_DATA,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA
    PROGRAM_FLASH__FILE_OFFSET__PASSWORD,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD
    PROGRAM_FLASH__FILE_OFFSET__BUTTONS,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS
    PROGRAM_FLASH__FILE_OFFSET__CREDIT,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT
    PROGRAM_FLASH__FILE_OFFSET__INPUT_C1,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1
    PROGRAM_FLASH__FILE_OFFSET__INPUT_C2,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2
    PROGRAM_FLASH__FILE_OFFSET__T_MAX_INT_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MAX_INT_TEMPERATURE_ALARM,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MAX_EXT_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MAX_EXT_TEMPERATURE_ALARM,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MIN_INT_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MIN_INT_TEMPERATURE_ALARM,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MIN_EXT_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__T_MIN_EXT_TEMPERATURE_ALARM,             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_INPUT_ALARM,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_INPUT_ALARM,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_INPUT_ALARM,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_SIGNAL_TYPE,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_ENABLE_ALARM,                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_INPUT_ALARM,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_SIGNAL_TYPE,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE
    PROGRAM_FLASH__FILE_OFFSET__POWER_ENABLE_ALARM,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM
    PROGRAM_FLASH__FILE_OFFSET__PHONE_ACTIVITY_COUNTERS,                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS
    PROGRAM_FLASH__FILE_OFFSET__SMS_DELAY,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY
    PROGRAM_FLASH__FILE_OFFSET__INPUT_C1_BANDS,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS
    PROGRAM_FLASH__FILE_OFFSET__INPUT_C2_BANDS,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS
    PROGRAM_FLASH__FILE_OFFSET__ANSWER_DETACHMENT,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT
    PROGRAM_FLASH__FILE_OFFSET__ANSWER_RESTORE,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE
    PROGRAM_FLASH__FILE_OFFSET__ANSWER_STATUS,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS
    PROGRAM_FLASH__FILE_OFFSET__ANSWER_RESET,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET
    PROGRAM_FLASH__FILE_OFFSET__ALARM_MODE,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE
    PROGRAM_FLASH__FILE_OFFSET__REPORT,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT
    PROGRAM_FLASH__FILE_OFFSET__AUTORESTORE,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE
    PROGRAM_FLASH__FILE_OFFSET__AUTORECONNECTION,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION
    PROGRAM_FLASH__FILE_OFFSET__ADC0_CALIBRATION,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION
    PROGRAM_FLASH__FILE_OFFSET__ADC1_CALIBRATION,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION
    PROGRAM_FLASH__FILE_OFFSET__MODE_TEMPERATURE_INT,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT
    PROGRAM_FLASH__FILE_OFFSET__MODE_TEMPERATURE_EXT,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT
    PROGRAM_FLASH__FILE_OFFSET__COMMANDS_EXT,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT
    PROGRAM_FLASH__FILE_OFFSET__CALIBRATION_INT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT
    PROGRAM_FLASH__FILE_OFFSET__CALIBRATION_EXT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT
    PROGRAM_FLASH__FILE_OFFSET__LOG_STATUS,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS
    PROGRAM_FLASH__FILE_OFFSET__ENERGY,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY
    PROGRAM_FLASH__FILE_OFFSET__F_ANTIFROST_INT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT
    PROGRAM_FLASH__FILE_OFFSET__F_ANTIFROST_EXT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT
    PROGRAM_FLASH__FILE_OFFSET__F_CHRONO_INT,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT
    PROGRAM_FLASH__FILE_OFFSET__F_CHRONO_EXT,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT
    PROGRAM_FLASH__FILE_OFFSET__NAME_OUT1,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1
    PROGRAM_FLASH__FILE_OFFSET__NAME_OUT2,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2
    PROGRAM_FLASH__FILE_OFFSET__NAME_IN1,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1
    PROGRAM_FLASH__FILE_OFFSET__NAME_IN2,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2
    PROGRAM_FLASH__FILE_OFFSET__NAME_IN3,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3
    PROGRAM_FLASH__FILE_OFFSET__NAME_IN4,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4
    PROGRAM_FLASH__FILE_OFFSET__NAME_TMIN,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN
    PROGRAM_FLASH__FILE_OFFSET__NAME_TMAX,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX
    PROGRAM_FLASH__FILE_OFFSET__NAME_TMINEXT,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT
    PROGRAM_FLASH__FILE_OFFSET__NAME_TMAXEXT,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT
    PROGRAM_FLASH__FILE_OFFSET__NAME_C1,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1
    PROGRAM_FLASH__FILE_OFFSET__NAME_C2,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN1,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN2,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN3,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN4,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMIN,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAX,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMINEXT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAXEXT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT
    PROGRAM_FLASH__FILE_OFFSET__TIMER_OUTPUT_INT,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT
    PROGRAM_FLASH__FILE_OFFSET__TIMER_OUTPUT_EXT,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT
    PROGRAM_FLASH__FILE_OFFSET__ACTIVE_STATUS_OUT1,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1
    PROGRAM_FLASH__FILE_OFFSET__ACTIVE_STATUS_OUT2,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2
    PROGRAM_FLASH__FILE_OFFSET__FORWARD,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN1_R,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN2_R,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN3_R,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN4_R,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMIN_R,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAX_R,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMINEXT_R,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R
    PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAXEXT_R,                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R
    PROGRAM_FLASH__FILE_OFFSET__ANALOG_MODE,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__PHONE_ACTIVITY,                          // PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__FW_UPGRADE_QUEUE,                        // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE
    PROGRAM_FLASH__FILE_OFFSET__SMS_ANSWER_TO_CRS_QUEUE,                 // PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE
    PROGRAM_FLASH__FILE_OFFSET__FILE_STATUS_LOG_QUEUE,                   // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE
    PROGRAM_FLASH__FILE_OFFSET__FILE_ALARM_QUEUE,                        // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__ALARM_COUNTERS,                          // PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS
    PROGRAM_FLASH__FILE_OFFSET__LAST_POWER_ALARM,                        // PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__SMS_COUNTER,                             // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER
    PROGRAM_FLASH__FILE_OFFSET__SYNCHRONIZE_TIME,                        // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__SYNCHRONIZE,                             // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE
    PROGRAM_FLASH__FILE_OFFSET__RETRANSMISSION,                          // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__RTC_BACKUP_TIME,                         // PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__RETX_STATUS,                             // PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MAX_INT_STATUS,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS
    PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MAX_EXT_STATUS,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS
    PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MIN_INT_STATUS,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS
    PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MIN_EXT_STATUS,              // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_STATUS,                        // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_STATUS,                        // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_STATUS,                        // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS
    PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_STATUS,                        // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__AVAILABLE_CREDIT,                        // PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__OUTPUT_OUT1_STATUS,                      // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS
    PROGRAM_FLASH__FILE_OFFSET__OUTPUT_OUT2_STATUS,                      // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__RX_COMMAND_LOG,                          // PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__F_REGULATION_INT_STATUS,                 // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS
    PROGRAM_FLASH__FILE_OFFSET__F_REGULATION_EXT_STATUS,                 // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__DEVICE_ID,                               // PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__COUNTER_C1,                              // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1
    PROGRAM_FLASH__FILE_OFFSET__COUNTER_C2,                              // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__SERIAL_NUMBER,                           // PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__ADC_VALUE_STATUS,                        // PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__FILE_OFFSET__ID_MODEL,                                // PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * data size (bytes)
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static const u16 ProgramFlash_DataSize[] =
{
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__DOTA_RESULT,                               // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT
    PROGRAM_FLASH__DATA_SIZE__DOTA_PHONE_NUMBER,                         // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER
    PROGRAM_FLASH__DATA_SIZE__POWERON_RESET_TYPE,                        // PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE
    PROGRAM_FLASH__DATA_SIZE__RESET_RESULT,                              // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT
    PROGRAM_FLASH__DATA_SIZE__RESET_PHONE_NUMBER,                        // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__LANGUAGE,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE
    PROGRAM_FLASH__DATA_SIZE__POD,                                       // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD
    PROGRAM_FLASH__DATA_SIZE__COMMAND_DETACHMENT,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT
    PROGRAM_FLASH__DATA_SIZE__COMMAND_RESTORE,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE
    PROGRAM_FLASH__DATA_SIZE__COMMAND_STATUS,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS
    PROGRAM_FLASH__DATA_SIZE__COMMAND_RESET,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET
    PROGRAM_FLASH__DATA_SIZE__GPRS_APN,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN
    PROGRAM_FLASH__DATA_SIZE__GPRS_DNS,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS
    PROGRAM_FLASH__DATA_SIZE__SIM_PIN,                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN
    PROGRAM_FLASH__DATA_SIZE__MYN_NUMBER,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER
    PROGRAM_FLASH__DATA_SIZE__PHONEBOOK,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK
    PROGRAM_FLASH__DATA_SIZE__FWD_NUMBER,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER
    PROGRAM_FLASH__DATA_SIZE__HOST,                                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST
    PROGRAM_FLASH__DATA_SIZE__HOST_ACCOUNT_2,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2
    PROGRAM_FLASH__DATA_SIZE__HOST_TX,                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX
    PROGRAM_FLASH__DATA_SIZE__HOST_DATA,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA
    PROGRAM_FLASH__DATA_SIZE__PASSWORD,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD
    PROGRAM_FLASH__DATA_SIZE__BUTTONS,                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS
    PROGRAM_FLASH__DATA_SIZE__CREDIT,                                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT
    PROGRAM_FLASH__DATA_SIZE__INPUT_C1,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1
    PROGRAM_FLASH__DATA_SIZE__INPUT_C2,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2
    PROGRAM_FLASH__DATA_SIZE__T_MAX_INT_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MAX_INT_TEMPERATURE_ALARM,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MAX_EXT_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MAX_EXT_TEMPERATURE_ALARM,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MIN_INT_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MIN_INT_TEMPERATURE_ALARM,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MIN_EXT_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__T_MIN_EXT_TEMPERATURE_ALARM,               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_INPUT_ALARM,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_INPUT_ALARM,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_INPUT_ALARM,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_SIGNAL_TYPE,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_ENABLE_ALARM,                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_INPUT_ALARM,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_SIGNAL_TYPE,                     // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE
    PROGRAM_FLASH__DATA_SIZE__POWER_ENABLE_ALARM,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM
    PROGRAM_FLASH__DATA_SIZE__PHONE_ACTIVITY_COUNTERS,                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS
    PROGRAM_FLASH__DATA_SIZE__SMS_DELAY,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY
    PROGRAM_FLASH__DATA_SIZE__INPUT_C1_BANDS,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS
    PROGRAM_FLASH__DATA_SIZE__INPUT_C2_BANDS,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS
    PROGRAM_FLASH__DATA_SIZE__ANSWER_DETACHMENT,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT
    PROGRAM_FLASH__DATA_SIZE__ANSWER_RESTORE,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE
    PROGRAM_FLASH__DATA_SIZE__ANSWER_STATUS,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS
    PROGRAM_FLASH__DATA_SIZE__ANSWER_RESET,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET
    PROGRAM_FLASH__DATA_SIZE__ALARM_MODE,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE
    PROGRAM_FLASH__DATA_SIZE__REPORT,                                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT
    PROGRAM_FLASH__DATA_SIZE__AUTORESTORE,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE
    PROGRAM_FLASH__DATA_SIZE__AUTORECONNECTION,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION
    PROGRAM_FLASH__DATA_SIZE__ADC0_CALIBRATION,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION
    PROGRAM_FLASH__DATA_SIZE__ADC1_CALIBRATION,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION
    PROGRAM_FLASH__DATA_SIZE__MODE_TEMPERATURE_INT,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT
    PROGRAM_FLASH__DATA_SIZE__MODE_TEMPERATURE_EXT,                      // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT
    PROGRAM_FLASH__DATA_SIZE__COMMANDS_EXT,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT
    PROGRAM_FLASH__DATA_SIZE__CALIBRATION_INT,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT
    PROGRAM_FLASH__DATA_SIZE__CALIBRATION_EXT,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT
    PROGRAM_FLASH__DATA_SIZE__LOG_STATUS,                                // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS
    PROGRAM_FLASH__DATA_SIZE__ENERGY,                                    // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY
    PROGRAM_FLASH__DATA_SIZE__F_ANTIFROST_INT,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT
    PROGRAM_FLASH__DATA_SIZE__F_ANTIFROST_EXT,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT
    PROGRAM_FLASH__DATA_SIZE__F_CHRONO_INT,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT
    PROGRAM_FLASH__DATA_SIZE__F_CHRONO_EXT,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT
    PROGRAM_FLASH__DATA_SIZE__NAME_OUT1,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1
    PROGRAM_FLASH__DATA_SIZE__NAME_OUT2,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2
    PROGRAM_FLASH__DATA_SIZE__NAME_IN1,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1
    PROGRAM_FLASH__DATA_SIZE__NAME_IN2,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2
    PROGRAM_FLASH__DATA_SIZE__NAME_IN3,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3
    PROGRAM_FLASH__DATA_SIZE__NAME_IN4,                                  // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4
    PROGRAM_FLASH__DATA_SIZE__NAME_TMIN,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN
    PROGRAM_FLASH__DATA_SIZE__NAME_TMAX,                                 // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX
    PROGRAM_FLASH__DATA_SIZE__NAME_TMINEXT,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT
    PROGRAM_FLASH__DATA_SIZE__NAME_TMAXEXT,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT
    PROGRAM_FLASH__DATA_SIZE__NAME_C1,                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1
    PROGRAM_FLASH__DATA_SIZE__NAME_C2,                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN1,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN2,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN3,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN4,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMIN,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAX,                              // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMINEXT,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAXEXT,                           // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT
    PROGRAM_FLASH__DATA_SIZE__TIMER_OUTPUT_INT,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT
    PROGRAM_FLASH__DATA_SIZE__TIMER_OUTPUT_EXT,                          // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT
    PROGRAM_FLASH__DATA_SIZE__ACTIVE_STATUS_OUT1,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1
    PROGRAM_FLASH__DATA_SIZE__ACTIVE_STATUS_OUT2,                        // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2
    PROGRAM_FLASH__DATA_SIZE__FORWARD,                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN1_R,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN2_R,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN3_R,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN4_R,                             // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMIN_R,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAX_R,                            // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMINEXT_R,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R
    PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAXEXT_R,                         // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R
    PROGRAM_FLASH__DATA_SIZE__ANALOG_MODE,                               // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__PHONE_ACTIVITY,                            // PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__FW_UPGRADE_QUEUE,                          // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE
    PROGRAM_FLASH__DATA_SIZE__SMS_ANSWER_TO_CRS_QUEUE,                   // PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE
    PROGRAM_FLASH__DATA_SIZE__FILE_STATUS_LOG_QUEUE,                     // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE
    PROGRAM_FLASH__DATA_SIZE__FILE_ALARM_QUEUE,                          // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__ALARM_COUNTERS,                            // PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS
    PROGRAM_FLASH__DATA_SIZE__LAST_POWER_ALARM,                          // PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__SMS_COUNTER,                               // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER
    PROGRAM_FLASH__DATA_SIZE__SYNCHRONIZE_TIME,                          // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__SYNCHRONIZE,                               // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE
    PROGRAM_FLASH__DATA_SIZE__RETRANSMISSION,                            // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__RTC_BACKUP_TIME,                           // PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__RETX_STATUS,                               // PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MAX_INT_STATUS,                // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS
    PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MAX_EXT_STATUS,                // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS
    PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MIN_INT_STATUS,                // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS
    PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MIN_EXT_STATUS,                // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_STATUS,                          // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_STATUS,                          // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_STATUS,                          // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS
    PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_STATUS,                          // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__AVAILABLE_CREDIT,                          // PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__OUTPUT_OUT1_STATUS,                        // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS
    PROGRAM_FLASH__DATA_SIZE__OUTPUT_OUT2_STATUS,                        // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__RX_COMMAND_LOG,                            // PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__F_REGULATION_INT_STATUS,                   // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS
    PROGRAM_FLASH__DATA_SIZE__F_REGULATION_EXT_STATUS,                   // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__DEVICE_ID,                                 // PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__COUNTER_C1,                                // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1
    PROGRAM_FLASH__DATA_SIZE__COUNTER_C2,                                // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__SERIAL_NUMBER,                             // PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__ADC_VALUE_STATUS,                          // PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS
    //---------------------------------------------------------
    PROGRAM_FLASH__DATA_SIZE__ID_MODEL,                                  // PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * flash first ID
 *---------------------------------------------------------*/
static const PROGRAM_FLASH__FLASH_ID ProgramFlash_FlashIdFirst[] =
{
    PROGRAM_FLASH__FLASH_ID__000_RESET__FIRST,                  // 000 - Reset
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FIRST,         // 001 - Configurations
    PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__FIRST,             // 002 - Phone   Info
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__FIRST,                  // 003 - Queue
    PROGRAM_FLASH__FLASH_ID__004_ALARM__FIRST,                  // 004 - Alarm
    PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__FIRST,            // 005 - Synchronize
    PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__FIRST,  // 006 - Random Configurations
    PROGRAM_FLASH__FLASH_ID__007_CLOCK__FIRST,                  // 007 - Clock
    PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__FIRST,         // 008 - Retransmission
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__FIRST,       // 009 - Temperature Info
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__FIRST,             // 010 - Input       Info
    PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__FIRST,            // 011 - Credit      Info
    PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__FIRST,            // 012 - Output      Info
    PROGRAM_FLASH__FLASH_ID__013_LOG__FIRST,                    // 013 - Log
    PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__FIRST,           // 014 - Regulation Function
    PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__FIRST,              // 015 - Device ID
    PROGRAM_FLASH__FLASH_ID__016_COUNTERS__FIRST,               // 016 - Counters
    PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__FIRST,          // 017 - Serial Number
    PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__FIRST,            // 018 - Energy      Info
    PROGRAM_FLASH__FLASH_ID__019_DEBUG__FIRST,                  // 019 - Debug
};


/*---------------------------------------------------------
 * flash last ID
 *---------------------------------------------------------*/
static const PROGRAM_FLASH__FLASH_ID ProgramFlash_FlashIdLast[] =
{
    PROGRAM_FLASH__FLASH_ID__000_RESET__LAST,                   // 000 - Reset
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LAST,          // 001 - Configurations
    PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__LAST,              // 002 - Phone   Info
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__LAST,                   // 003 - Queue
    PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST,                   // 004 - Alarm
    PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__LAST,             // 005 - Synchronize
    PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__LAST,   // 006 - Random Configurations
    PROGRAM_FLASH__FLASH_ID__007_CLOCK__LAST,                   // 007 - Clock
    PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__LAST,          // 008 - Retransmission
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__LAST,        // 009 - Temperature Info
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__LAST,              // 010 - Input       Info
    PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__LAST,             // 011 - Credit      Info
    PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__LAST,             // 012 - Output      Info
    PROGRAM_FLASH__FLASH_ID__013_LOG__LAST,                     // 013 - Log
    PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__LAST,            // 014 - Regulation Function
    PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__LAST,               // 015 - Device ID
    PROGRAM_FLASH__FLASH_ID__016_COUNTERS__LAST,                // 016 - Counters
    PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__LAST,           // 017 - Serial Number
    PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__LAST,             // 018 - Energy      Info
    PROGRAM_FLASH__FLASH_ID__019_DEBUG__LAST,                   // 019 - Debug
};


/*-----------------------------------------------------------------------------
 * Number of pending backup
 *-----------------------------------------------------------------------------*/
/* number of pending backup for flash object handles */
static       u8         ProgramFlash_NumPendingBackupsHandles[] =
{
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS
    //---------------------------------------------------------
    0,                                                                   // PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL
    //---------------------------------------------------------
};


/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* flash object handle strings */
static const ascii     *ProgramFlash_Handles[] =
{
    PROGRAM_FLASH__HANDLE_STRING__000_RESET,                  /* 000 - Reset                 */
    PROGRAM_FLASH__HANDLE_STRING__001_CONFIGURATIONS,         /* 001 - Configurations        */
    PROGRAM_FLASH__HANDLE_STRING__002_PHONE_INFO,             /* 002 - Phone   Info          */
    PROGRAM_FLASH__HANDLE_STRING__003_QUEUE,                  /* 003 - Queue                 */
    PROGRAM_FLASH__HANDLE_STRING__004_ALARM,                  /* 004 - Alarm                 */
    PROGRAM_FLASH__HANDLE_STRING__005_SYNCHRONIZE,            /* 005 - Synchronize           */
    PROGRAM_FLASH__HANDLE_STRING__006_RANDOM_CONFIGURATIONS,  /* 006 - Random Configurations */
    PROGRAM_FLASH__HANDLE_STRING__007_CLOCK,                  /* 007 - Clock                 */
    PROGRAM_FLASH__HANDLE_STRING__008_RETRANSMISSION,         /* 008 - Retransmission        */
    PROGRAM_FLASH__HANDLE_STRING__009_TEMPERATURE_INFO,       /* 009 - Temperature Info      */
    PROGRAM_FLASH__HANDLE_STRING__010_INPUT_INFO,             /* 010 - Input       Info      */
    PROGRAM_FLASH__HANDLE_STRING__011_CREDIT_INFO,            /* 011 - Credit      Info      */
    PROGRAM_FLASH__HANDLE_STRING__012_OUTPUT_INFO,            /* 012 - Output      Info      */
    PROGRAM_FLASH__HANDLE_STRING__013_LOG,                    /* 013 - Log                   */
    PROGRAM_FLASH__HANDLE_STRING__014_F_REGULATION,           /* 014 - Regulation Function   */
    PROGRAM_FLASH__HANDLE_STRING__015_DEVICE_ID,              /* 015 - Device ID             */
    PROGRAM_FLASH__HANDLE_STRING__016_COUNTERS,               /* 016 - Counters              */
    PROGRAM_FLASH__HANDLE_STRING__017_SERIAL_NUMBER,          /* 017 - Serial Number         */
    PROGRAM_FLASH__HANDLE_STRING__018_ENERGY_INFO,            /* 018 - Energy      Info      */
    PROGRAM_FLASH__HANDLE_STRING__019_DEBUG,                  /* 019 - Debug                 */
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void   ProgramFlash_TaskProgramFlash(void *argument);

/* flash init */
       bool   ProgramFlash_InitFlashAll   (void);
       bool   ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX handle_index);
       bool   ProgramFlash_InitFlashId    (PROGRAM_FLASH__FLASH_ID     id_index    );

/* flash  info */
       ascii *ProgramFlash_HandleString   (PROGRAM_FLASH__HANDLE_INDEX handle_index);
       u16    ProgramFlash_HandleNumMaxIds(PROGRAM_FLASH__HANDLE_INDEX handle_index);

/* flash reading */
       bool   ProgramFlash_ReadFlashAll   (void);
       bool   ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX handle_index);
       bool   ProgramFlash_ReadFlashId    (PROGRAM_FLASH__FLASH_ID     id_index    );

/* request of single ID writing */
       bool   ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX handle_index, PROGRAM_FLASH__FLASH_ID id_index);

/* pendent beckup info */
       bool   ProgramFlash_IsBackupPendent(void);

/* action on a single ID */
static bool   ProgramFlash_BackupWriteDefault(PROGRAM_FLASH__FLASH_ID id_index);
       bool   ProgramFlash_BackupWrite       (PROGRAM_FLASH__FLASH_ID id_index);
static bool   ProgramFlash_BackupRead        (PROGRAM_FLASH__FLASH_ID id_index);

/* ID info */
static bool   ProgramFlash_SizeBackup(PROGRAM_FLASH__FLASH_ID id_index, u16 *ptr_data_address, u16 *ptr_data_size);


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void   ProgramFlash_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);




/*===========================================================================
 * Function   : ProgramFlash_TaskProgramFlash
 *
 * Description: task che si occupa del backup nella flash (flash objects)
 * Input      : -
 * Output     : -
 *===========================================================================*/
void ProgramFlash_TaskProgramFlash(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "FLASH - Start Task", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            ProgramFlash_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*===========================================================================
 * Function   : ProgramFlash_InitFlashAll
 *
 * Description: prepare the flash, in order:
 *                 - erase                  unsupported IDs
 *                 - create (write) missing   supported IDs (with default values)
 * Input      : -
 * Output     : - FALSE: operation failed
 *              - TRUE : operation success
 *===========================================================================*/
bool ProgramFlash_InitFlashAll(void)
{
    QFILE                   file;

    PROGRAM_FLASH__FLASH_ID id_index;

    bool                    result_1;
    bool                    result_2;


    result_1 = TRUE;

    file = ql_fopen(PROGRAM_FLASH__FILE_PATH, "r+");
    if (file <= 0)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "ql_fopen ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        file = ql_fopen(PROGRAM_FLASH__FILE_PATH, "w+");
        if (file <= 0)
            return FALSE;

        ql_fclose(file);

        /* write default values in Flash */
        for (id_index = PROGRAM_FLASH__FLASH_ID__FIRST; id_index <= PROGRAM_FLASH__FLASH_ID__LAST; id_index++)
        {
            result_2 = ProgramFlash_InitFlashId(id_index);
            if (!result_2)
                result_1 = FALSE;
        }
    }
    else
    {
        ql_fclose(file);
    }

    return result_1;
}




/*===========================================================================
 * Function   : ProgramFlash_InitFlashHandle
 *
 * Description: prepare handle index of the flash, in order:
 *                 - erase                  unsupported IDs of the handle index
 *                 - create (write) missing   supported IDs of the handle index (with default values)
 * Input      : - handle_index: handle index
 * Output     : - FALSE: operation failed
 *              - TRUE : operation success
s *===========================================================================*/
bool ProgramFlash_InitFlashHandle(PROGRAM_FLASH__HANDLE_INDEX handle_index)
{
    PROGRAM_FLASH__FLASH_ID data_id;
    PROGRAM_FLASH__FLASH_ID data_id_first;
    PROGRAM_FLASH__FLASH_ID data_id_last;

    bool                    result_1;
    bool                    result_2;


    result_1 = TRUE;

    data_id_first = ProgramFlash_FlashIdFirst[(u8)handle_index];
    data_id_last  = ProgramFlash_FlashIdLast [(u8)handle_index];

    for (data_id = data_id_first; data_id <= data_id_last; data_id++)
    {
        result_2 = ProgramFlash_InitFlashId(data_id);
        if (!result_2)
            result_1 = FALSE;
    }

    return result_1;
}




/*===========================================================================
 * Function   : ProgramFlash_InitFlashId
 *
 * Description: prepare an ID (if supported) of the flash, in order:
 *                 - erase                  unsupported IDs of the handle index
 *                 - create (write) missing   supported IDs of the handle index (with default values)
 * Input      : - handle_index: handle index
 *              - id_index    : ID     index
 * Output     : - FALSE: operation failed
 *              - TRUE : operation success
 *===========================================================================*/
bool ProgramFlash_InitFlashId(PROGRAM_FLASH__FLASH_ID id_index)
{
    bool result;


    result = ProgramFlash_BackupWriteDefault(id_index);
    if (!result)
    {
        snprintf(ProgramFlash_DebugString, sizeof(ProgramFlash_DebugString), "ProgramFlash_BackupWriteDefault ERROR - id_index: %d", id_index);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, ProgramFlash_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return result;
}




/*===========================================================================
 * Function   : ProgramFlash_HandleString
 *
 * Description: return the handle string of a specified handle
 * Input      : - handle_index: handle index
 * Output     : - handle string of the specified handle
 *===========================================================================*/
ascii *ProgramFlash_HandleString(PROGRAM_FLASH__HANDLE_INDEX handle_index)
{
    return ((ascii *)ProgramFlash_Handles[handle_index]);
}




/*===========================================================================
 * Function   : ProgramFlash_HandleNumMaxIds
 *
 * Description: return the maximum number of IDs of the specified handle
 * Input      : - handle_index: handle index
 * Output     : - maximum number of IDs of the specified handle
 *===========================================================================*/
u16 ProgramFlash_HandleNumMaxIds(PROGRAM_FLASH__HANDLE_INDEX handle_index)
{
    PROGRAM_FLASH__FLASH_ID data_id_first;
    PROGRAM_FLASH__FLASH_ID data_id_last;


    data_id_first = ProgramFlash_FlashIdFirst[(u8)handle_index];
    data_id_last  = ProgramFlash_FlashIdLast [(u8)handle_index];


    return (data_id_last - data_id_first + 1);
}




/*===========================================================================
 * Function   : ProgramFlash_ReadFlashAll
 *
 * Description: read all (supported) IDs from flash and copy them to RAM
 * Input      : -
 * Output     : - FALSE: operation failed
 *              - TRUE : operation success
 *===========================================================================*/
bool ProgramFlash_ReadFlashAll(void)
{
    PROGRAM_FLASH__FLASH_ID id_index;

    bool                    result_1;
    bool                    result_2;


    result_1 = TRUE;

    for (id_index = PROGRAM_FLASH__FLASH_ID__FIRST; id_index <= PROGRAM_FLASH__FLASH_ID__LAST; id_index++)
    {
        result_2 = ProgramFlash_ReadFlashId(id_index);
        if (!result_2)
            result_1 = FALSE;
    }

    return result_1;
}




/*===========================================================================
 * Function   : ProgramFlash_ReadFlashHandle
 *
 * Description: read all (supported) IDs of a handle index from flash and copy them to RAM
 * Input      : - handle_index: handle index
 * Output     : - FALSE: operation failed
 *              - TRUE : operation success
 *===========================================================================*/
bool ProgramFlash_ReadFlashHandle(PROGRAM_FLASH__HANDLE_INDEX handle_index)
{
    PROGRAM_FLASH__FLASH_ID data_id;
    PROGRAM_FLASH__FLASH_ID data_id_first;
    PROGRAM_FLASH__FLASH_ID data_id_last;

    bool                    result_1;
    bool                    result_2;


    result_1 = TRUE;

    data_id_first = ProgramFlash_FlashIdFirst[(u8)handle_index];
    data_id_last  = ProgramFlash_FlashIdLast [(u8)handle_index];

    for (data_id = data_id_first; data_id <= data_id_last; data_id++)
    {
        result_2 = ProgramFlash_ReadFlashId(data_id);
        if (!result_2)
            result_1 = FALSE;
    }

    return result_1;
}




/*===========================================================================
 * Function   : ProgramFlash_ReadFlashId
 *
 * Description: read an ID (if supported) from flash and copy it to RAM
 * Input      : - handle_index: handle index
 *              - id_index    : ID     index
 * Output     : - FALSE: operation failed
 *              - TRUE : operation success
 *===========================================================================*/
bool ProgramFlash_ReadFlashId(PROGRAM_FLASH__FLASH_ID id_index)
{
    bool result;


    /* reading of the ID of the handle */
    result = ProgramFlash_BackupRead(id_index);
    if (!result)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "ProgramFlash_BackupRead ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return result;
}




/*===========================================================================
 * Function   : ProgramFlash_RequestWriteBackup
 *
 * Description: signal a request of writing of an ID of a specified handle
 * Input      : - handle_index: handle index
 *              - id_index    : ID     index
 * Output     : - FALSE: request not signaled (parameters not valid)
 *              - TRUE : request     signaled
 *===========================================================================*/
bool ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX handle_index, PROGRAM_FLASH__FLASH_ID id_index)
{
    ql_event_t event;
    QlOSStatus err;


    /* increment the number of pendent backup (for those handle index and id index) */
    if (ProgramFlash_NumPendingBackupsHandles[id_index] < 255)
        ProgramFlash_NumPendingBackupsHandles[id_index]++;


    /* event signal */
    event.id     = EVENT_FLASH_BACKUP_TO_DO;
    event.param1 = handle_index;  // not used
    event.param2 = id_index;

    err = ql_rtos_event_send(Boot_TaskRef_ProgramFlash, &event);
    if (err != QL_OSI_SUCCESS)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return TRUE;
}




/*===========================================================================
 * Function   : ProgramFlash_IsBackupPendent
 *
 * Description: verify if there is at least one pendent backup
 * Input      : -
 * Output     : - FALSE: there is at least one pendent backup
 *              - TRUE : there is no           pendent backups
 *===========================================================================*/
bool ProgramFlash_IsBackupPendent(void)
{
    PROGRAM_FLASH__FLASH_ID id_index;


    for (id_index = PROGRAM_FLASH__FLASH_ID__FIRST; id_index < PROGRAM_FLASH__FLASH_ID__LAST; id_index++)
    {
        if (ProgramFlash_NumPendingBackupsHandles[id_index] > 0)
            return TRUE;
    }

    return FALSE;
}





/*===========================================================================
 * Function   : ProgramFlash_BackupWriteDefault
 *
 * Description: write to backup the default values of an specified ID of an specified handle
 * Input      : - id_index: ID index
 * Output     : - FALSE: action error
 *              - TRUE : action OK
 *===========================================================================*/
static bool ProgramFlash_BackupWriteDefault(PROGRAM_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calculated;

    int    res_int;


    /* get data info (data address, data size) */
    ProgramFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(PROGRAM_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        ql_fclose(file);
        return FALSE;
    }


    /* get the variables from RAM */
    if (ProgramFlash_Functions_GetDefault[id_index])
        ProgramFlash_Functions_GetDefault[id_index](ptr_data);


    /* CRC calculus */
    crc_calculated = Utility_CalculateCRC16(ptr_data, data_size, 0);
    *(ptr_data + data_size    ) = (u8)((crc_calculated >> 8) & 0x00FF);
    *(ptr_data + data_size + 1) = (u8)((crc_calculated     ) & 0x00FF);


    /* writing to flash (data + CRC) */
    res_int = ql_fwrite(ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* free allocated memory */
    if (ptr_data)
        free(ptr_data);

    if (res_int != (int)(data_size + 2))
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_fwrite - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : ProgramFlash_BackupWrite
 *
 * Description: write to backup the values of an specified ID of an specified handle
 * Input      : - id_index: ID index
 * Output     : - FALSE: action error
 *              - TRUE : action OK
 *===========================================================================*/
bool ProgramFlash_BackupWrite(PROGRAM_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calculated;

    int    res_int;


    /* get data info (data address, data size) */
    ProgramFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(PROGRAM_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        ql_fclose(file);
        return FALSE;
    }


    /* get the variables from RAM */
    if (ProgramFlash_Functions_Get[id_index])
        ProgramFlash_Functions_Get[id_index](ptr_data);


    /* CRC calculus */
    crc_calculated = Utility_CalculateCRC16(ptr_data, data_size, 0);
    *(ptr_data + data_size    ) = (u8)((crc_calculated >> 8) & 0x00FF);
    *(ptr_data + data_size + 1) = (u8)((crc_calculated     ) & 0x00FF);


    /* writing to flash (data + CRC) */
    res_int = ql_fwrite(ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* free memory allocation */
    if (ptr_data)
        free(ptr_data);

    if (res_int != (int)(data_size + 2))
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_fwrite - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : ProgramFlash_BackupRead
 *
 * Description: read from backup the backup values of an specified ID of an specified handle
 * Input      : - id_index: ID index
 * Output     : - FALSE: action error
 *              - TRUE : action OK
 *===========================================================================*/
static bool ProgramFlash_BackupRead(PROGRAM_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calculated;
    u16    crc_read;
    bool   crc_ok;

    int    res_int;


    /* get data info (data address, data size) */
    ProgramFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(PROGRAM_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        ql_fclose(file);
        return FALSE;
    }


    /* reading from flash objects (data + CRC) */
    res_int = ql_fread(ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* CRC verify */
    if (res_int == (int)(data_size + 2))
    {
        crc_read       = (((u16)(*(ptr_data + data_size    ))) << 8) |
                         (((u16)(*(ptr_data + data_size + 1)))     );
        crc_calculated = Utility_CalculateCRC16(ptr_data, data_size, 0);

        if (crc_calculated == crc_read)
            crc_ok = TRUE;
        else
            crc_ok = FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_fread - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        crc_ok = FALSE;
    }


    /* set the variables in RAM */
    if (
         (ProgramFlash_Functions_GetDefault[id_index]) &&
         (ProgramFlash_Functions_Set       [id_index])
       )
    {
        if (!crc_ok)
            ProgramFlash_Functions_GetDefault[id_index](ptr_data);
        ProgramFlash_Functions_Set           [id_index](ptr_data);
    }


    /* free allocated memory */
    if (ptr_data)
        free(ptr_data);


    return crc_ok;
}




/*===========================================================================
 * Function   : ProgramFlash_SizeBackup
 *
 * Description: get the data info (data address, data size) of the specified data ID
 * Inputs     : - id_index        : data ID
 *              - ptr_data_address: pointer to save data address
 *              - ptr_data_size   : pointer to save data size (bytes)
 * Outputs    : - FALSE: data info not got (the specified data ID does not exist)
 *              - TRUE : data info     got
 *===========================================================================*/
static bool ProgramFlash_SizeBackup(PROGRAM_FLASH__FLASH_ID id_index, u16 *ptr_data_address, u16 *ptr_data_size)
{
    *ptr_data_address = ProgramFlash_FileOffset[id_index];
    *ptr_data_size    = ProgramFlash_DataSize  [id_index];

    return TRUE;
}




/*===========================================================================
 * Function   : ProgramFlash_AdlCallback_Message_TaskMsg
 *
 * Description: message callback function for task Flash
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void ProgramFlash_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    PROGRAM_FLASH__FLASH_ID id_index;
    u8                      num_writings;

    bool                    result;


    snprintf(ProgramFlash_DebugString, sizeof(ProgramFlash_DebugString), "CALLBACK     - MESSAGE     - TASK FLASH - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, ProgramFlash_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        /* backup to do */
        case EVENT_FLASH_BACKUP_TO_DO:
            /* extraction of ID index */
            id_index = (PROGRAM_FLASH__FLASH_ID)msg_identifier->param2;

            /* backup writing */
            num_writings = 0;
            do
            {
                /* writing */
                result = ProgramFlash_BackupWrite(id_index);
                if (!result)
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "ProgramFlash_BackupWrite ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                num_writings++;
            } while ((!result) && (num_writings < NUM_MAX_WRITING_BACKUP));

            /* decrement the number of pendent backup (for those handle index and id index) */
            if (ProgramFlash_NumPendingBackupsHandles[id_index] > 0)
                ProgramFlash_NumPendingBackupsHandles[id_index]--;
            break;


        /* unknown event */
        default:
            break;
    }
}
