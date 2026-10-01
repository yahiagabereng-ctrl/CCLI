/*===========================================================================
 * File       :  PROGRAM_FLASH.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - backup flash manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *==========================================================================*/




#ifndef __PROGRAM_FLASH_H__


#define __PROGRAM_FLASH_H__




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 *
 * Flash Objects available size (SL6087 module): 384 KB
 *
 *
 *---------------------------------------------------------------------------
 *                           Flash Object sizes
 *---------------------------------------------------------------------------
 *
 *  |-----------------------------|-------------------------------------|--------------------------------------|------------------------------------------------|
 *  |      Handle                 |               Parameter             |               Size (byte)            |             Change frequency                   |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 000 - Reset                 | DOTA result                         |                 1 |   1 |   3 |    3 |                                                |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  |                             | language                            |                 4 |   4 |   6 |      | At every new configuration                     |
 *  |                             |-------------------------------------|-------------------|-----|-----|      |------------------------------------------------|
 *  |                             | GPRS APN                            |            3 * 41 | 123 | 125 |      | At every new configuration                     |
 *  |                             | GPRS DNS                            |            2 * 31 |  62 |  64 |      | At every new configuration                     |
 *  |                             |-------------------------------------|-------------------|-----|-----|      |------------------------------------------------|
 *  |                             | MYN phone number                    |                21 |  21 |  23 |      | At every new configuration                     |
 *  |                             | phonebook                           |          100 * 36 |3600 |3602 |      | At every new configuration                     |
 *  |                             | FWD phone number                    |                21 |  21 |  23 |      | At every new configuration                     |
 *  |                             |-------------------------------------|-------------------|-----|-----|      |------------------------------------------------|
 *  |                             | password                            |            2 + 15 |  17 |  19 |      | At every new configuration                     |
 *  | 001 - Configurations        |-------------------------------------|-------------------|-----|-----|      |------------------------------------------------|
 *  |                             | tank    alarm                       |                18 |  18 |  20 |      | At every new configuration                     |
 *  |                             | battery alarm                       |                 8 |   8 |  10 |      | At every new configuration                     |
 *  |                             | alarm                               |            4 * 32 | 128 | 130 |      | At every new configuration                     |
 *  |                             | credit  alarm                       |                 4 |   4 |   6 |      | At every new configuration                     |
 *  |                             |-------------------------------------|-------------------|-----|-----|      |------------------------------------------------|
 *  |                             | phone activity counters             |                21 |  21 |  23 |      | At every new configuration                     |
 *  |                             |-------------------------------------|-------------------|-----|-----|      |------------------------------------------------|
 *  |                             | ADC0 calibration                    |             4 * 2 |   8 |  10 |      | At every new configuration                     |
 *  |                             | ADC1 calibration                    |             4 * 2 |   8 |  10 |      | At every new configuration                     |
 *  |                             | ADC2 calibration                    |             4 * 2 |   8 |  10 |      | At every new configuration                     |
 *  |                             | ADC3 calibration                    |             4 * 2 |   8 |  10 |      | At every new configuration                     |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  |                             | phone activity                      |       3 * (4 * 25)| 300 | 302 |  302 | At every SMS sent                              |
 *  | 002 - Phone Info            |                                     |                   |     |     |      | At every SMS received                          |
 *  |                             |                                     |                   |     |     |      | At every phone counters reset (monthly)        |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 003 - Queue                 | FW upgrade queue                    |    3 + (35 *  15) | 528 | 530 |  530 | At every FW upgrade                            |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 004 - Alarm                 | alarm counters                      |           32 * 36 |1152 |1154 | 1154 | At every alarm generated                       |
 *  |                             |                                     |                   |     |     |      | At every alarm counter reset (monthly)         |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 005 - Synchronize           | SMS counter                         |                 2 |   2 |   4 |    4 | At every autosynchronize done                  |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  |                             | HW configuration                    |                 5 |   5 |   7 |      | At every autosynchronize if MYN is not present |
 *  |                             | send log                            |                 5 |   5 |   7 |      | At every autosynchronize if MYN is not present |
 *  | 006 - Random Configurations | send log                            |                 5 |   5 |   7 |   35 | At every autosynchronize if MYN is not present |
 *  |                             | synchronize                         |                 5 |   5 |   7 |      | At every autosynchronize if MYN is not present |
 *  |                             | retransmission                      |                 5 |   5 |   7 |      | At every autosynchronize if MYN is not present |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 007 - Clock                 | RTC time backup                     |                 8 |   8 |  10 |   10 | Every 12 hours                                 |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 008 - Retransmission        | retransmission status               |                 2 |   2 |   4 |    4 | At the end of every transmission window        |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  |                             | internal max temperature status     |                   |     |     |      |                                                |
 *  | 009 - Temperature Info      | external max temperature status     |                   |     |     |      |                                                |
 *  |                             | internal min temperature status     |                   |     |     |      |                                                |
 *  |                             | external min temperature status     |                   |     |     |      |                                                |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 010 - Input       Info      | input IN1 status                    |                   |     |     |      |                                                |
 *  |                             | input IN2 status                    |                   |     |     |      |                                                |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 011 - Credit      Info      | available credit                    |                   |     |     |      |                                                |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 012 - Output      Info      | output OUT1 status                  |                   |     |     |      |                                                |
 *  |                             | output OUT2 status                  |                   |     |     |      |                                                |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 013 - Logs                  | rx command log                      |    3 + (11 * 210) |2313 |2315 | 2315 | At every command SMS received                  |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *  | 014 - F. Regulation         | regulation function                 |         ?         |  ?  |  ?  |  ?   |                                                |
 *  |-----------------------------|-------------------------------------|-------------------|-----|-----|------|------------------------------------------------|
 *                                                                                          TOTAL SIZE  | 6047 |
 *                                                                                                      |------|
 */





/*===========================================================================
 * INCLUDES
 *===========================================================================*/
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/

/*---------------------------------------------------------------------------
 * Handler definition
 *---------------------------------------------------------------------------*/

/*
 * Note: It is not  allowed to delete or modify the values defined below
 *       It is only allowed to append new values (new definitions)
 */

/* flash object handles strings */
#define PROGRAM_FLASH__HANDLE_STRING__000_RESET                   "PROGRAM - 000 - Reset"                  /* 000 - Reset                 */
#define PROGRAM_FLASH__HANDLE_STRING__001_CONFIGURATIONS          "PROGRAM - 001 - Configurations"         /* 001 - Configurations        */
#define PROGRAM_FLASH__HANDLE_STRING__002_PHONE_INFO              "PROGRAM - 002 - Phone Info"             /* 002 - Phone   Info          */
#define PROGRAM_FLASH__HANDLE_STRING__003_QUEUE                   "PROGRAM - 003 - Queue"                  /* 003 - Queue                 */
#define PROGRAM_FLASH__HANDLE_STRING__004_ALARM                   "PROGRAM - 004 - Alarm"                  /* 004 - Alarm                 */
#define PROGRAM_FLASH__HANDLE_STRING__005_SYNCHRONIZE             "PROGRAM - 005 - Synchronize"            /* 005 - Synchronize           */
#define PROGRAM_FLASH__HANDLE_STRING__006_RANDOM_CONFIGURATIONS   "PROGRAM - 006 - Random Configurations"  /* 006 - Random Configurations */
#define PROGRAM_FLASH__HANDLE_STRING__007_CLOCK                   "PROGRAM - 007 - Clock"                  /* 007 - Clock                 */
#define PROGRAM_FLASH__HANDLE_STRING__008_RETRANSMISSION          "PROGRAM - 008 - Retransmission"         /* 008 - Retransmission        */
#define PROGRAM_FLASH__HANDLE_STRING__009_TEMPERATURE_INFO        "PROGRAM - 009 - Temperature Info"       /* 009 - Temperature Info      */
#define PROGRAM_FLASH__HANDLE_STRING__010_INPUT_INFO              "PROGRAM - 010 - Input Info"             /* 010 - Input       Info      */
#define PROGRAM_FLASH__HANDLE_STRING__011_CREDIT_INFO             "PROGRAM - 011 - Credit Info"            /* 011 - Credit      Info      */
#define PROGRAM_FLASH__HANDLE_STRING__012_OUTPUT_INFO             "PROGRAM - 012 - Output Info"            /* 012 - Output      Info      */
#define PROGRAM_FLASH__HANDLE_STRING__013_LOG                     "PROGRAM - 013 - Log"                    /* 013 - Log                   */
#define PROGRAM_FLASH__HANDLE_STRING__014_F_REGULATION            "PROGRAM - 014 - Regulation Function"    /* 014 - Regulation Function   */
#define PROGRAM_FLASH__HANDLE_STRING__015_DEVICE_ID               "PROGRAM - 015 - Device ID"              /* 015 - Device ID             */
#define PROGRAM_FLASH__HANDLE_STRING__016_COUNTERS                "PROGRAM - 016 - Counters"               /* 016 - Counters              */
#define PROGRAM_FLASH__HANDLE_STRING__017_SERIAL_NUMBER           "PROGRAM - 017 - Serial Number"          /* 017 - Serial Number         */
#define PROGRAM_FLASH__HANDLE_STRING__018_ENERGY_INFO             "PROGRAM - 018 - Energy Info"            /* 018 - Energy      Info      */
#define PROGRAM_FLASH__HANDLE_STRING__019_DEBUG                   "PROGRAM - 019 - Debug"                  /* 019 - Debug                 */


/*---------------------------------------------------------------------------
 * Data size (bytes)
 *---------------------------------------------------------------------------*/
#define PROGRAM_FLASH__DATA_SIZE__DOTA_RESULT                     sizeof(RESET__DOTA_RESULT                           )
#define PROGRAM_FLASH__DATA_SIZE__DOTA_PHONE_NUMBER               sizeof(RESET__DOTA_PHONE_NUMBER                     )
#define PROGRAM_FLASH__DATA_SIZE__POWERON_RESET_TYPE              sizeof(RESET__POWER_ON_RESET_TYPE                   )
#define PROGRAM_FLASH__DATA_SIZE__RESET_RESULT                    sizeof(STARTUP__RESET_RESULT                        )
#define PROGRAM_FLASH__DATA_SIZE__RESET_PHONE_NUMBER              sizeof(STARTUP__RESET_PHONE_NUMBER                  )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__LANGUAGE                        sizeof(LANGUAGE__CONFIG__LANGUAGE                   )
#define PROGRAM_FLASH__DATA_SIZE__POD                             sizeof(POD__CONFIG__POD                             )
#define PROGRAM_FLASH__DATA_SIZE__COMMAND_DETACHMENT              sizeof(COMMANDS__CONFIG__COMMAND_DETACHMENT         )
#define PROGRAM_FLASH__DATA_SIZE__COMMAND_RESTORE                 sizeof(COMMANDS__CONFIG__COMMAND_RESTORE            )
#define PROGRAM_FLASH__DATA_SIZE__COMMAND_STATUS                  sizeof(COMMANDS__CONFIG__COMMAND_STATUS             )
#define PROGRAM_FLASH__DATA_SIZE__COMMAND_RESET                   sizeof(COMMANDS__CONFIG__COMMAND_RESET              )
#define PROGRAM_FLASH__DATA_SIZE__GPRS_APN                        sizeof(PHONE__CONFIG__GPRS_APN_PARAMETERS           )
#define PROGRAM_FLASH__DATA_SIZE__GPRS_DNS                        sizeof(PHONE__CONFIG__GPRS_DNS_PARAMETERS           )
#define PROGRAM_FLASH__DATA_SIZE__SIM_PIN                         sizeof(PHONE__CONFIG__SIM_PIN                       )
#define PROGRAM_FLASH__DATA_SIZE__MYN_NUMBER                      sizeof(MYN__CONFIG__MYN_NUMBER                      )
#define PROGRAM_FLASH__DATA_SIZE__PHONEBOOK                       sizeof(PHONEBOOK__CONFIG__PHONEBOOK                 )
#define PROGRAM_FLASH__DATA_SIZE__FWD_NUMBER                      sizeof(FWD__CONFIG__FWD_NUMBER                      )
#define PROGRAM_FLASH__DATA_SIZE__HOST                            sizeof(HTTP__CONFIG__HOST                           )
#define PROGRAM_FLASH__DATA_SIZE__HOST_ACCOUNT_2                  sizeof(HTTP__CONFIG__HOST_ACCOUNT_2                 )
#define PROGRAM_FLASH__DATA_SIZE__HOST_TX                         sizeof(HTTP__CONFIG__HOST_TX                        )
#define PROGRAM_FLASH__DATA_SIZE__HOST_DATA                       sizeof(HTTP__CONFIG__HOST_DATA                      )
#define PROGRAM_FLASH__DATA_SIZE__PASSWORD                        sizeof(PASSWORD__CONFIG__PASSWORD                   )
#define PROGRAM_FLASH__DATA_SIZE__BUTTONS                         sizeof(OUT_KEY__CONFIG__BUTTONS                     )
#define PROGRAM_FLASH__DATA_SIZE__CREDIT                          sizeof(CREDIT__CONFIG__CREDIT                       )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_C1                        sizeof(COUNTERS__CONFIG__INPUT_C1                   )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_C2                        sizeof(COUNTERS__CONFIG__INPUT_C2                   )
#define PROGRAM_FLASH__DATA_SIZE__T_MAX_INT_ENABLE_ALARM          sizeof(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT)
#define PROGRAM_FLASH__DATA_SIZE__T_MAX_INT_TEMPERATURE_ALARM     sizeof(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT      )
#define PROGRAM_FLASH__DATA_SIZE__T_MAX_EXT_ENABLE_ALARM          sizeof(ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT)
#define PROGRAM_FLASH__DATA_SIZE__T_MAX_EXT_TEMPERATURE_ALARM     sizeof(ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT      )
#define PROGRAM_FLASH__DATA_SIZE__T_MIN_INT_ENABLE_ALARM          sizeof(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT)
#define PROGRAM_FLASH__DATA_SIZE__T_MIN_INT_TEMPERATURE_ALARM     sizeof(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT      )
#define PROGRAM_FLASH__DATA_SIZE__T_MIN_EXT_ENABLE_ALARM          sizeof(ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT)
#define PROGRAM_FLASH__DATA_SIZE__T_MIN_EXT_TEMPERATURE_ALARM     sizeof(ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT      )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_ENABLE_ALARM          sizeof(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1         )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_INPUT_ALARM           sizeof(ALARM_INPUT__CONFIG__INPUT_IN1               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_ENABLE_ALARM          sizeof(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2         )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_INPUT_ALARM           sizeof(ALARM_INPUT__CONFIG__INPUT_IN2               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_ENABLE_ALARM          sizeof(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3         )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_INPUT_ALARM           sizeof(ALARM_INPUT__CONFIG__INPUT_IN3               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_SIGNAL_TYPE           sizeof(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE          )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_ENABLE_ALARM          sizeof(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4         )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_INPUT_ALARM           sizeof(ALARM_INPUT__CONFIG__INPUT_IN4               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_SIGNAL_TYPE           sizeof(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE          )
#define PROGRAM_FLASH__DATA_SIZE__POWER_ENABLE_ALARM              sizeof(ALARM_POWER__CONFIG__ALARM_POWER             )
#define PROGRAM_FLASH__DATA_SIZE__PHONE_ACTIVITY_COUNTERS         sizeof(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS       )
#define PROGRAM_FLASH__DATA_SIZE__SMS_DELAY                       sizeof(PHONE__CONFIG__SMS_DELAY                     )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_C1_BANDS                  sizeof(COUNTERS__CONFIG__INPUT_C1_BANDS             )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_C2_BANDS                  sizeof(COUNTERS__CONFIG__INPUT_C2_BANDS             )
#define PROGRAM_FLASH__DATA_SIZE__ANSWER_DETACHMENT               sizeof(COMMANDS__CONFIG__ANSWER_DETACHMENT          )
#define PROGRAM_FLASH__DATA_SIZE__ANSWER_RESTORE                  sizeof(COMMANDS__CONFIG__ANSWER_RESTORE             )
#define PROGRAM_FLASH__DATA_SIZE__ANSWER_STATUS                   sizeof(COMMANDS__CONFIG__ANSWER_STATUS              )
#define PROGRAM_FLASH__DATA_SIZE__ANSWER_RESET                    sizeof(COMMANDS__CONFIG__ANSWER_RESET               )
#define PROGRAM_FLASH__DATA_SIZE__ALARM_MODE                      sizeof(ALARM__CONFIG__ALARM_MODE                    )
#define PROGRAM_FLASH__DATA_SIZE__REPORT                          sizeof(REPORT__CONFIG__REPORT                       )
#define PROGRAM_FLASH__DATA_SIZE__AUTORESTORE                     sizeof(FV_ACTION__CONFIG__AUTORESTORE               )
#define PROGRAM_FLASH__DATA_SIZE__AUTORECONNECTION                sizeof(FV_ACTION__CONFIG__AUTORECONNECTION          )
#define PROGRAM_FLASH__DATA_SIZE__ADC0_CALIBRATION                sizeof(ADC__CONFIG__ADC0_CALIBRATION                )
#define PROGRAM_FLASH__DATA_SIZE__ADC1_CALIBRATION                sizeof(ADC__CONFIG__ADC1_CALIBRATION                )
#define PROGRAM_FLASH__DATA_SIZE__MODE_TEMPERATURE_INT            sizeof(MODE__CONFIG__MODE_TEMPERATURE_INT           )
#define PROGRAM_FLASH__DATA_SIZE__MODE_TEMPERATURE_EXT            sizeof(MODE__CONFIG__MODE_TEMPERATURE_EXT           )
#define PROGRAM_FLASH__DATA_SIZE__COMMANDS_EXT                    sizeof(COMMANDS_EXT__CONFIG__COMMANDS_EXT           )
#define PROGRAM_FLASH__DATA_SIZE__CALIBRATION_INT                 sizeof(DRVTEMPERATURE__CONFIG__CALIBRATION_INT      )
#define PROGRAM_FLASH__DATA_SIZE__CALIBRATION_EXT                 sizeof(DRVTEMPERATURE__CONFIG__CALIBRATION_EXT      )
#define PROGRAM_FLASH__DATA_SIZE__LOG_STATUS                      sizeof(LOG_STATUS__CONFIG__STATUS_LOG               )
#define PROGRAM_FLASH__DATA_SIZE__ENERGY                          sizeof(ENERGY__CONFIG__ADC_VALUE                    )
#define PROGRAM_FLASH__DATA_SIZE__F_ANTIFROST_INT                 sizeof(F_ANTIFROST__CONFIG_F_ANTIFROST_INT          )
#define PROGRAM_FLASH__DATA_SIZE__F_ANTIFROST_EXT                 sizeof(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT          )
#define PROGRAM_FLASH__DATA_SIZE__F_CHRONO_INT                    sizeof(F_CHRONO__CONFIG_F_CHRONO_INT                )
#define PROGRAM_FLASH__DATA_SIZE__F_CHRONO_EXT                    sizeof(F_CHRONO__CONFIG_F_CHRONO_EXT                )
#define PROGRAM_FLASH__DATA_SIZE__NAME_OUT1                       sizeof(NAMES__CONFIG__NAME_OUT1                     )
#define PROGRAM_FLASH__DATA_SIZE__NAME_OUT2                       sizeof(NAMES__CONFIG__NAME_OUT2                     )
#define PROGRAM_FLASH__DATA_SIZE__NAME_IN1                        sizeof(NAMES__CONFIG__NAME_IN1                      )
#define PROGRAM_FLASH__DATA_SIZE__NAME_IN2                        sizeof(NAMES__CONFIG__NAME_IN2                      )
#define PROGRAM_FLASH__DATA_SIZE__NAME_IN3                        sizeof(NAMES__CONFIG__NAME_IN3                      )
#define PROGRAM_FLASH__DATA_SIZE__NAME_IN4                        sizeof(NAMES__CONFIG__NAME_IN4                      )
#define PROGRAM_FLASH__DATA_SIZE__NAME_TMIN                       sizeof(NAMES__CONFIG__NAME_TMIN                     )
#define PROGRAM_FLASH__DATA_SIZE__NAME_TMAX                       sizeof(NAMES__CONFIG__NAME_TMAX                     )
#define PROGRAM_FLASH__DATA_SIZE__NAME_TMINEXT                    sizeof(NAMES__CONFIG__NAME_TMINEXT                  )
#define PROGRAM_FLASH__DATA_SIZE__NAME_TMAXEXT                    sizeof(NAMES__CONFIG__NAME_TMAXEXT                  )
#define PROGRAM_FLASH__DATA_SIZE__NAME_C1                         sizeof(NAMES__CONFIG__NAME_C1                       )
#define PROGRAM_FLASH__DATA_SIZE__NAME_C2                         sizeof(NAMES__CONFIG__NAME_C2                       )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN1                     sizeof(MESSAGES__CONFIG__MESSAGE_IN1                )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN2                     sizeof(MESSAGES__CONFIG__MESSAGE_IN2                )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN3                     sizeof(MESSAGES__CONFIG__MESSAGE_IN3                )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN4                     sizeof(MESSAGES__CONFIG__MESSAGE_IN4                )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMIN                    sizeof(MESSAGES__CONFIG__MESSAGE_TMIN               )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAX                    sizeof(MESSAGES__CONFIG__MESSAGE_TMAX               )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMINEXT                 sizeof(MESSAGES__CONFIG__MESSAGE_TMINEXT            )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAXEXT                 sizeof(MESSAGES__CONFIG__MESSAGE_TMAXEXT            )
#define PROGRAM_FLASH__DATA_SIZE__TIMER_OUTPUT_INT                sizeof(OUTPUTS__CONFIG__TIMER_OUTPUT_INT            )
#define PROGRAM_FLASH__DATA_SIZE__TIMER_OUTPUT_EXT                sizeof(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT            )
#define PROGRAM_FLASH__DATA_SIZE__ACTIVE_STATUS_OUT1              sizeof(REGULATION__CONFIG__ACTIVE_STATUS_OUT1       )
#define PROGRAM_FLASH__DATA_SIZE__ACTIVE_STATUS_OUT2              sizeof(REGULATION__CONFIG__ACTIVE_STATUS_OUT2       )
#define PROGRAM_FLASH__DATA_SIZE__FORWARD                         sizeof(FORWARD__CONFIG__FORWARD                     )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN1_R                   sizeof(MESSAGES__CONFIG__MESSAGE_IN1_R              )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN2_R                   sizeof(MESSAGES__CONFIG__MESSAGE_IN2_R              )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN3_R                   sizeof(MESSAGES__CONFIG__MESSAGE_IN3_R              )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN4_R                   sizeof(MESSAGES__CONFIG__MESSAGE_IN4_R              )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMIN_R                  sizeof(MESSAGES__CONFIG__MESSAGE_TMIN_R             )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAX_R                  sizeof(MESSAGES__CONFIG__MESSAGE_TMAX_R             )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMINEXT_R               sizeof(MESSAGES__CONFIG__MESSAGE_TMINEXT_R          )
#define PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAXEXT_R               sizeof(MESSAGES__CONFIG__MESSAGE_TMAXEXT_R          )
#define PROGRAM_FLASH__DATA_SIZE__ANALOG_MODE                     sizeof(ANALOG__CONFIG__ANALOG_MODE                  )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__PHONE_ACTIVITY                  sizeof(PHONE__PHONE_ACTIVITY                        )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__FW_UPGRADE_QUEUE                sizeof(QUEUE__FW_UPGRADE_QUEUE                      )
#define PROGRAM_FLASH__DATA_SIZE__SMS_ANSWER_TO_CRS_QUEUE         sizeof(QUEUE__SMS_ANSWER_TO_CRS_QUEUE               )
#define PROGRAM_FLASH__DATA_SIZE__FILE_STATUS_LOG_QUEUE           sizeof(QUEUE__FILE_STATUS_LOG_QUEUE                 )
#define PROGRAM_FLASH__DATA_SIZE__FILE_ALARM_QUEUE                sizeof(QUEUE__FILE_ALARM_QUEUE                      )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__ALARM_COUNTERS                  sizeof(ALARM__ALARM_COUNTERS                        )
#define PROGRAM_FLASH__DATA_SIZE__LAST_POWER_ALARM                sizeof(ALARM_POWER__STATUS__LAST_POWER_ALARM        )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__SMS_COUNTER                     sizeof(SYNCHRONIZE__SMS_COUNTER                     )
#define PROGRAM_FLASH__DATA_SIZE__SYNCHRONIZE_TIME                sizeof(SYNCHRONIZE__SYNCHRONIZE_TIME                )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__SYNCHRONIZE                     sizeof(USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE    )
#define PROGRAM_FLASH__DATA_SIZE__RETRANSMISSION                  sizeof(USER_GSM_RETX__RANDOM_CONFIG__RETX           )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__RTC_BACKUP_TIME                 sizeof(CLOCK__RTC_BACKUP_TIME                       )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__RETX_STATUS                     sizeof(USER_GSM_RETX__RETX_STATUS                   )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MAX_INT_STATUS      sizeof(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT      )
#define PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MAX_EXT_STATUS      sizeof(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT      )
#define PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MIN_INT_STATUS      sizeof(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT      )
#define PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MIN_EXT_STATUS      sizeof(ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT      )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_STATUS                sizeof(ALARM_INPUT__STATUS__INPUT_IN1               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_STATUS                sizeof(ALARM_INPUT__STATUS__INPUT_IN2               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_STATUS                sizeof(ALARM_INPUT__STATUS__INPUT_IN3               )
#define PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_STATUS                sizeof(ALARM_INPUT__STATUS__INPUT_IN4               )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__AVAILABLE_CREDIT                sizeof(CREDIT__STATUS__AVAILABLE_CREDIT             )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__OUTPUT_OUT1_STATUS              sizeof(OUTPUTS__STATUS__OUTPUT_OUT1                 )
#define PROGRAM_FLASH__DATA_SIZE__OUTPUT_OUT2_STATUS              sizeof(OUTPUTS__STATUS__OUTPUT_OUT2                 )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__RX_COMMAND_LOG                  sizeof(LOGS__RX_COMMAND_LOG                         )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__F_REGULATION_INT_STATUS         sizeof(F_REGULATION__STATUS_REGULATION_INT          )
#define PROGRAM_FLASH__DATA_SIZE__F_REGULATION_EXT_STATUS         sizeof(F_REGULATION__STATUS_REGULATION_EXT          )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__DEVICE_ID                       sizeof(DEVICE_ID__CONFIG__DEVICE_ID                 )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__COUNTER_C1                      sizeof(COUNTERS__STATUS__INPUT_C1                   )
#define PROGRAM_FLASH__DATA_SIZE__COUNTER_C2                      sizeof(COUNTERS__STATUS__INPUT_C2                   )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__SERIAL_NUMBER                   sizeof(ID__CONFIG__SERIAL_NUMBER                    )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__ADC_VALUE_STATUS                sizeof(ENERGY__STATUS__ADC_VALUE                    )
//---------------------------------------------------------
#define PROGRAM_FLASH__DATA_SIZE__ID_MODEL                        sizeof(FW_CONFIG__ID_MODEL                          )


/*---------------------------------------------------------------------------
 * File offset
 *---------------------------------------------------------------------------*/
/* IMPORTANT - Add new definitions at the end because current data Flash addresses must not be changed */
#define PROGRAM_FLASH__FILE_OFFSET__DOTA_RESULT                   0
#define PROGRAM_FLASH__FILE_OFFSET__DOTA_PHONE_NUMBER             (PROGRAM_FLASH__FILE_OFFSET__DOTA_RESULT                 + PROGRAM_FLASH__DATA_SIZE__DOTA_RESULT                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__POWERON_RESET_TYPE            (PROGRAM_FLASH__FILE_OFFSET__DOTA_PHONE_NUMBER           + PROGRAM_FLASH__DATA_SIZE__DOTA_PHONE_NUMBER           + 2)
#define PROGRAM_FLASH__FILE_OFFSET__RESET_RESULT                  (PROGRAM_FLASH__FILE_OFFSET__POWERON_RESET_TYPE          + PROGRAM_FLASH__DATA_SIZE__POWERON_RESET_TYPE          + 2)
#define PROGRAM_FLASH__FILE_OFFSET__RESET_PHONE_NUMBER            (PROGRAM_FLASH__FILE_OFFSET__RESET_RESULT                + PROGRAM_FLASH__DATA_SIZE__RESET_RESULT                + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__LANGUAGE                      (PROGRAM_FLASH__FILE_OFFSET__RESET_PHONE_NUMBER          + PROGRAM_FLASH__DATA_SIZE__RESET_PHONE_NUMBER          + 2)
#define PROGRAM_FLASH__FILE_OFFSET__POD                           (PROGRAM_FLASH__FILE_OFFSET__LANGUAGE                    + PROGRAM_FLASH__DATA_SIZE__LANGUAGE                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__COMMAND_DETACHMENT            (PROGRAM_FLASH__FILE_OFFSET__POD                         + PROGRAM_FLASH__DATA_SIZE__POD                         + 2)
#define PROGRAM_FLASH__FILE_OFFSET__COMMAND_RESTORE               (PROGRAM_FLASH__FILE_OFFSET__COMMAND_DETACHMENT          + PROGRAM_FLASH__DATA_SIZE__COMMAND_DETACHMENT          + 2)
#define PROGRAM_FLASH__FILE_OFFSET__COMMAND_STATUS                (PROGRAM_FLASH__FILE_OFFSET__COMMAND_RESTORE             + PROGRAM_FLASH__DATA_SIZE__COMMAND_RESTORE             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__COMMAND_RESET                 (PROGRAM_FLASH__FILE_OFFSET__COMMAND_STATUS              + PROGRAM_FLASH__DATA_SIZE__COMMAND_STATUS              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__GPRS_APN                      (PROGRAM_FLASH__FILE_OFFSET__COMMAND_RESET               + PROGRAM_FLASH__DATA_SIZE__COMMAND_RESET               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__GPRS_DNS                      (PROGRAM_FLASH__FILE_OFFSET__GPRS_APN                    + PROGRAM_FLASH__DATA_SIZE__GPRS_APN                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__SIM_PIN                       (PROGRAM_FLASH__FILE_OFFSET__GPRS_DNS                    + PROGRAM_FLASH__DATA_SIZE__GPRS_DNS                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MYN_NUMBER                    (PROGRAM_FLASH__FILE_OFFSET__SIM_PIN                     + PROGRAM_FLASH__DATA_SIZE__SIM_PIN                     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__PHONEBOOK                     (PROGRAM_FLASH__FILE_OFFSET__MYN_NUMBER                  + PROGRAM_FLASH__DATA_SIZE__MYN_NUMBER                  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__FWD_NUMBER                    (PROGRAM_FLASH__FILE_OFFSET__PHONEBOOK                   + PROGRAM_FLASH__DATA_SIZE__PHONEBOOK                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__HOST                          (PROGRAM_FLASH__FILE_OFFSET__FWD_NUMBER                  + PROGRAM_FLASH__DATA_SIZE__FWD_NUMBER                  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__HOST_ACCOUNT_2                (PROGRAM_FLASH__FILE_OFFSET__HOST                        + PROGRAM_FLASH__DATA_SIZE__HOST                        + 2)
#define PROGRAM_FLASH__FILE_OFFSET__HOST_TX                       (PROGRAM_FLASH__FILE_OFFSET__HOST_ACCOUNT_2              + PROGRAM_FLASH__DATA_SIZE__HOST_ACCOUNT_2              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__HOST_DATA                     (PROGRAM_FLASH__FILE_OFFSET__HOST_TX                     + PROGRAM_FLASH__DATA_SIZE__HOST_TX                     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__PASSWORD                      (PROGRAM_FLASH__FILE_OFFSET__HOST_DATA                   + PROGRAM_FLASH__DATA_SIZE__HOST_DATA                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__BUTTONS                       (PROGRAM_FLASH__FILE_OFFSET__PASSWORD                    + PROGRAM_FLASH__DATA_SIZE__PASSWORD                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__CREDIT                        (PROGRAM_FLASH__FILE_OFFSET__BUTTONS                     + PROGRAM_FLASH__DATA_SIZE__BUTTONS                     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_C1                      (PROGRAM_FLASH__FILE_OFFSET__CREDIT                      + PROGRAM_FLASH__DATA_SIZE__CREDIT                      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_C2                      (PROGRAM_FLASH__FILE_OFFSET__INPUT_C1                    + PROGRAM_FLASH__DATA_SIZE__INPUT_C1                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MAX_INT_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__INPUT_C2                    + PROGRAM_FLASH__DATA_SIZE__INPUT_C2                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MAX_INT_TEMPERATURE_ALARM   (PROGRAM_FLASH__FILE_OFFSET__T_MAX_INT_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__T_MAX_INT_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MAX_EXT_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__T_MAX_INT_TEMPERATURE_ALARM + PROGRAM_FLASH__DATA_SIZE__T_MAX_INT_TEMPERATURE_ALARM + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MAX_EXT_TEMPERATURE_ALARM   (PROGRAM_FLASH__FILE_OFFSET__T_MAX_EXT_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__T_MAX_EXT_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MIN_INT_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__T_MAX_EXT_TEMPERATURE_ALARM + PROGRAM_FLASH__DATA_SIZE__T_MAX_EXT_TEMPERATURE_ALARM + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MIN_INT_TEMPERATURE_ALARM   (PROGRAM_FLASH__FILE_OFFSET__T_MIN_INT_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__T_MIN_INT_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MIN_EXT_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__T_MIN_INT_TEMPERATURE_ALARM + PROGRAM_FLASH__DATA_SIZE__T_MIN_INT_TEMPERATURE_ALARM + 2)
#define PROGRAM_FLASH__FILE_OFFSET__T_MIN_EXT_TEMPERATURE_ALARM   (PROGRAM_FLASH__FILE_OFFSET__T_MIN_EXT_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__T_MIN_EXT_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__T_MIN_EXT_TEMPERATURE_ALARM + PROGRAM_FLASH__DATA_SIZE__T_MIN_EXT_TEMPERATURE_ALARM + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_INPUT_ALARM         (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_INPUT_ALARM       + PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_INPUT_ALARM       + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_INPUT_ALARM         (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_INPUT_ALARM       + PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_INPUT_ALARM       + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_INPUT_ALARM         (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_SIGNAL_TYPE         (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_INPUT_ALARM       + PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_INPUT_ALARM       + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_ENABLE_ALARM        (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_SIGNAL_TYPE       + PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_SIGNAL_TYPE       + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_INPUT_ALARM         (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_ENABLE_ALARM      + PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_ENABLE_ALARM      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_SIGNAL_TYPE         (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_INPUT_ALARM       + PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_INPUT_ALARM       + 2)
#define PROGRAM_FLASH__FILE_OFFSET__POWER_ENABLE_ALARM            (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_SIGNAL_TYPE       + PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_SIGNAL_TYPE       + 2)
#define PROGRAM_FLASH__FILE_OFFSET__PHONE_ACTIVITY_COUNTERS       (PROGRAM_FLASH__FILE_OFFSET__POWER_ENABLE_ALARM          + PROGRAM_FLASH__DATA_SIZE__POWER_ENABLE_ALARM          + 2)
#define PROGRAM_FLASH__FILE_OFFSET__SMS_DELAY                     (PROGRAM_FLASH__FILE_OFFSET__PHONE_ACTIVITY_COUNTERS     + PROGRAM_FLASH__DATA_SIZE__PHONE_ACTIVITY_COUNTERS     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_C1_BANDS                (PROGRAM_FLASH__FILE_OFFSET__SMS_DELAY                   + PROGRAM_FLASH__DATA_SIZE__SMS_DELAY                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_C2_BANDS                (PROGRAM_FLASH__FILE_OFFSET__INPUT_C1_BANDS              + PROGRAM_FLASH__DATA_SIZE__INPUT_C1_BANDS              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ANSWER_DETACHMENT             (PROGRAM_FLASH__FILE_OFFSET__INPUT_C2_BANDS              + PROGRAM_FLASH__DATA_SIZE__INPUT_C2_BANDS              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ANSWER_RESTORE                (PROGRAM_FLASH__FILE_OFFSET__ANSWER_DETACHMENT           + PROGRAM_FLASH__DATA_SIZE__ANSWER_DETACHMENT           + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ANSWER_STATUS                 (PROGRAM_FLASH__FILE_OFFSET__ANSWER_RESTORE              + PROGRAM_FLASH__DATA_SIZE__ANSWER_RESTORE              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ANSWER_RESET                  (PROGRAM_FLASH__FILE_OFFSET__ANSWER_STATUS               + PROGRAM_FLASH__DATA_SIZE__ANSWER_STATUS               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ALARM_MODE                    (PROGRAM_FLASH__FILE_OFFSET__ANSWER_RESET                + PROGRAM_FLASH__DATA_SIZE__ANSWER_RESET                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__REPORT                        (PROGRAM_FLASH__FILE_OFFSET__ALARM_MODE                  + PROGRAM_FLASH__DATA_SIZE__ALARM_MODE                  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__AUTORESTORE                   (PROGRAM_FLASH__FILE_OFFSET__REPORT                      + PROGRAM_FLASH__DATA_SIZE__REPORT                      + 2)
#define PROGRAM_FLASH__FILE_OFFSET__AUTORECONNECTION              (PROGRAM_FLASH__FILE_OFFSET__AUTORESTORE                 + PROGRAM_FLASH__DATA_SIZE__AUTORESTORE                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ADC0_CALIBRATION              (PROGRAM_FLASH__FILE_OFFSET__AUTORECONNECTION            + PROGRAM_FLASH__DATA_SIZE__AUTORECONNECTION            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ADC1_CALIBRATION              (PROGRAM_FLASH__FILE_OFFSET__ADC0_CALIBRATION            + PROGRAM_FLASH__DATA_SIZE__ADC0_CALIBRATION            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MODE_TEMPERATURE_INT          (PROGRAM_FLASH__FILE_OFFSET__ADC1_CALIBRATION            + PROGRAM_FLASH__DATA_SIZE__ADC1_CALIBRATION            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MODE_TEMPERATURE_EXT          (PROGRAM_FLASH__FILE_OFFSET__MODE_TEMPERATURE_INT        + PROGRAM_FLASH__DATA_SIZE__MODE_TEMPERATURE_INT        + 2)
#define PROGRAM_FLASH__FILE_OFFSET__COMMANDS_EXT                  (PROGRAM_FLASH__FILE_OFFSET__MODE_TEMPERATURE_EXT        + PROGRAM_FLASH__DATA_SIZE__MODE_TEMPERATURE_EXT        + 2)
#define PROGRAM_FLASH__FILE_OFFSET__CALIBRATION_INT               (PROGRAM_FLASH__FILE_OFFSET__COMMANDS_EXT                + PROGRAM_FLASH__DATA_SIZE__COMMANDS_EXT                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__CALIBRATION_EXT               (PROGRAM_FLASH__FILE_OFFSET__CALIBRATION_INT             + PROGRAM_FLASH__DATA_SIZE__CALIBRATION_INT             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__LOG_STATUS                    (PROGRAM_FLASH__FILE_OFFSET__CALIBRATION_EXT             + PROGRAM_FLASH__DATA_SIZE__CALIBRATION_EXT             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__F_ANTIFROST_INT               (PROGRAM_FLASH__FILE_OFFSET__LOG_STATUS                  + PROGRAM_FLASH__DATA_SIZE__LOG_STATUS                  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__F_ANTIFROST_EXT               (PROGRAM_FLASH__FILE_OFFSET__F_ANTIFROST_INT             + PROGRAM_FLASH__DATA_SIZE__F_ANTIFROST_INT             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__F_CHRONO_INT                  (PROGRAM_FLASH__FILE_OFFSET__F_ANTIFROST_EXT             + PROGRAM_FLASH__DATA_SIZE__F_ANTIFROST_EXT             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__F_CHRONO_EXT                  (PROGRAM_FLASH__FILE_OFFSET__F_CHRONO_INT                + PROGRAM_FLASH__DATA_SIZE__F_CHRONO_INT                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_OUT1                     (PROGRAM_FLASH__FILE_OFFSET__F_CHRONO_EXT                + PROGRAM_FLASH__DATA_SIZE__F_CHRONO_EXT                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_OUT2                     (PROGRAM_FLASH__FILE_OFFSET__NAME_OUT1                   + PROGRAM_FLASH__DATA_SIZE__NAME_OUT1                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_IN1                      (PROGRAM_FLASH__FILE_OFFSET__NAME_OUT2                   + PROGRAM_FLASH__DATA_SIZE__NAME_OUT2                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_IN2                      (PROGRAM_FLASH__FILE_OFFSET__NAME_IN1                    + PROGRAM_FLASH__DATA_SIZE__NAME_IN1                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_IN3                      (PROGRAM_FLASH__FILE_OFFSET__NAME_IN2                    + PROGRAM_FLASH__DATA_SIZE__NAME_IN2                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_IN4                      (PROGRAM_FLASH__FILE_OFFSET__NAME_IN3                    + PROGRAM_FLASH__DATA_SIZE__NAME_IN3                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_TMIN                     (PROGRAM_FLASH__FILE_OFFSET__NAME_IN4                    + PROGRAM_FLASH__DATA_SIZE__NAME_IN4                    + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_TMAX                     (PROGRAM_FLASH__FILE_OFFSET__NAME_TMIN                   + PROGRAM_FLASH__DATA_SIZE__NAME_TMIN                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_TMINEXT                  (PROGRAM_FLASH__FILE_OFFSET__NAME_TMAX                   + PROGRAM_FLASH__DATA_SIZE__NAME_TMAX                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_TMAXEXT                  (PROGRAM_FLASH__FILE_OFFSET__NAME_TMINEXT                + PROGRAM_FLASH__DATA_SIZE__NAME_TMINEXT                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_C1                       (PROGRAM_FLASH__FILE_OFFSET__NAME_TMAXEXT                + PROGRAM_FLASH__DATA_SIZE__NAME_TMAXEXT                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__NAME_C2                       (PROGRAM_FLASH__FILE_OFFSET__NAME_C1                     + PROGRAM_FLASH__DATA_SIZE__NAME_C1                     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN1                   (PROGRAM_FLASH__FILE_OFFSET__NAME_C2                     + PROGRAM_FLASH__DATA_SIZE__NAME_C2                     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN2                   (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN1                 + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN1                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN3                   (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN2                 + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN2                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN4                   (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN3                 + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN3                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMIN                  (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN4                 + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN4                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAX                  (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMIN                + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMIN                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMINEXT               (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAX                + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAX                + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAXEXT               (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMINEXT             + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMINEXT             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__TIMER_OUTPUT_INT              (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAXEXT             + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAXEXT             + 2)
#define PROGRAM_FLASH__FILE_OFFSET__TIMER_OUTPUT_EXT              (PROGRAM_FLASH__FILE_OFFSET__TIMER_OUTPUT_INT            + PROGRAM_FLASH__DATA_SIZE__TIMER_OUTPUT_INT            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ACTIVE_STATUS_OUT1            (PROGRAM_FLASH__FILE_OFFSET__TIMER_OUTPUT_EXT            + PROGRAM_FLASH__DATA_SIZE__TIMER_OUTPUT_EXT            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ACTIVE_STATUS_OUT2            (PROGRAM_FLASH__FILE_OFFSET__ACTIVE_STATUS_OUT1          + PROGRAM_FLASH__DATA_SIZE__ACTIVE_STATUS_OUT1          + 2)
#define PROGRAM_FLASH__FILE_OFFSET__FORWARD                       (PROGRAM_FLASH__FILE_OFFSET__ACTIVE_STATUS_OUT2          + PROGRAM_FLASH__DATA_SIZE__ACTIVE_STATUS_OUT2          + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN1_R                 (PROGRAM_FLASH__FILE_OFFSET__FORWARD                     + PROGRAM_FLASH__DATA_SIZE__FORWARD                     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN2_R                 (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN1_R               + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN1_R               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN3_R                 (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN2_R               + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN2_R               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN4_R                 (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN3_R               + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN3_R               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMIN_R                (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_IN4_R               + PROGRAM_FLASH__DATA_SIZE__MESSAGE_IN4_R               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAX_R                (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMIN_R              + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMIN_R              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMINEXT_R             (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAX_R              + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAX_R              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAXEXT_R             (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMINEXT_R           + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMINEXT_R           + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ANALOG_MODE                   (PROGRAM_FLASH__FILE_OFFSET__MESSAGE_TMAXEXT_R           + PROGRAM_FLASH__DATA_SIZE__MESSAGE_TMAXEXT_R           + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__PHONE_ACTIVITY                (PROGRAM_FLASH__FILE_OFFSET__ANALOG_MODE                 + PROGRAM_FLASH__DATA_SIZE__ANALOG_MODE                 + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__FW_UPGRADE_QUEUE              (PROGRAM_FLASH__FILE_OFFSET__PHONE_ACTIVITY              + PROGRAM_FLASH__DATA_SIZE__PHONE_ACTIVITY              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__SMS_ANSWER_TO_CRS_QUEUE       (PROGRAM_FLASH__FILE_OFFSET__FW_UPGRADE_QUEUE            + PROGRAM_FLASH__DATA_SIZE__FW_UPGRADE_QUEUE            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__FILE_STATUS_LOG_QUEUE         (PROGRAM_FLASH__FILE_OFFSET__SMS_ANSWER_TO_CRS_QUEUE     + PROGRAM_FLASH__DATA_SIZE__SMS_ANSWER_TO_CRS_QUEUE     + 2)
#define PROGRAM_FLASH__FILE_OFFSET__FILE_ALARM_QUEUE              (PROGRAM_FLASH__FILE_OFFSET__FILE_STATUS_LOG_QUEUE       + PROGRAM_FLASH__DATA_SIZE__FILE_STATUS_LOG_QUEUE       + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__ALARM_COUNTERS                (PROGRAM_FLASH__FILE_OFFSET__FILE_ALARM_QUEUE            + PROGRAM_FLASH__DATA_SIZE__FILE_ALARM_QUEUE            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__LAST_POWER_ALARM              (PROGRAM_FLASH__FILE_OFFSET__ALARM_COUNTERS              + PROGRAM_FLASH__DATA_SIZE__ALARM_COUNTERS              + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__SMS_COUNTER                   (PROGRAM_FLASH__FILE_OFFSET__LAST_POWER_ALARM            + PROGRAM_FLASH__DATA_SIZE__LAST_POWER_ALARM            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__SYNCHRONIZE_TIME              (PROGRAM_FLASH__FILE_OFFSET__SMS_COUNTER                 + PROGRAM_FLASH__DATA_SIZE__SMS_COUNTER                 + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__SYNCHRONIZE                   (PROGRAM_FLASH__FILE_OFFSET__SYNCHRONIZE_TIME            + PROGRAM_FLASH__DATA_SIZE__SYNCHRONIZE_TIME            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__RETRANSMISSION                (PROGRAM_FLASH__FILE_OFFSET__SYNCHRONIZE                 + PROGRAM_FLASH__DATA_SIZE__SYNCHRONIZE                 + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__RTC_BACKUP_TIME               (PROGRAM_FLASH__FILE_OFFSET__RETRANSMISSION              + PROGRAM_FLASH__DATA_SIZE__RETRANSMISSION              + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__RETX_STATUS                   (PROGRAM_FLASH__FILE_OFFSET__RTC_BACKUP_TIME             + PROGRAM_FLASH__DATA_SIZE__RTC_BACKUP_TIME             + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MAX_INT_STATUS    (PROGRAM_FLASH__FILE_OFFSET__RETX_STATUS                 + PROGRAM_FLASH__DATA_SIZE__RETX_STATUS                 + 2)
#define PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MAX_EXT_STATUS    (PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MAX_INT_STATUS  + PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MAX_INT_STATUS  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MIN_INT_STATUS    (PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MAX_EXT_STATUS  + PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MAX_EXT_STATUS  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MIN_EXT_STATUS    (PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MIN_INT_STATUS  + PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MIN_INT_STATUS  + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_STATUS              (PROGRAM_FLASH__FILE_OFFSET__TEMPERATURE_MIN_EXT_STATUS  + PROGRAM_FLASH__DATA_SIZE__TEMPERATURE_MIN_EXT_STATUS  + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_STATUS              (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN1_STATUS            + PROGRAM_FLASH__DATA_SIZE__INPUT_IN1_STATUS            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_STATUS              (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN2_STATUS            + PROGRAM_FLASH__DATA_SIZE__INPUT_IN2_STATUS            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_STATUS              (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN3_STATUS            + PROGRAM_FLASH__DATA_SIZE__INPUT_IN3_STATUS            + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__AVAILABLE_CREDIT              (PROGRAM_FLASH__FILE_OFFSET__INPUT_IN4_STATUS            + PROGRAM_FLASH__DATA_SIZE__INPUT_IN4_STATUS            + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__OUTPUT_OUT1_STATUS            (PROGRAM_FLASH__FILE_OFFSET__AVAILABLE_CREDIT            + PROGRAM_FLASH__DATA_SIZE__AVAILABLE_CREDIT            + 2)
#define PROGRAM_FLASH__FILE_OFFSET__OUTPUT_OUT2_STATUS            (PROGRAM_FLASH__FILE_OFFSET__OUTPUT_OUT1_STATUS          + PROGRAM_FLASH__DATA_SIZE__OUTPUT_OUT1_STATUS          + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__RX_COMMAND_LOG                (PROGRAM_FLASH__FILE_OFFSET__OUTPUT_OUT2_STATUS          + PROGRAM_FLASH__DATA_SIZE__OUTPUT_OUT2_STATUS          + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__F_REGULATION_INT_STATUS       (PROGRAM_FLASH__FILE_OFFSET__RX_COMMAND_LOG              + PROGRAM_FLASH__DATA_SIZE__RX_COMMAND_LOG              + 2)
#define PROGRAM_FLASH__FILE_OFFSET__F_REGULATION_EXT_STATUS       (PROGRAM_FLASH__FILE_OFFSET__F_REGULATION_INT_STATUS     + PROGRAM_FLASH__DATA_SIZE__F_REGULATION_INT_STATUS     + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__DEVICE_ID                     (PROGRAM_FLASH__FILE_OFFSET__F_REGULATION_EXT_STATUS     + PROGRAM_FLASH__DATA_SIZE__F_REGULATION_EXT_STATUS     + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__COUNTER_C1                    (PROGRAM_FLASH__FILE_OFFSET__DEVICE_ID                   + PROGRAM_FLASH__DATA_SIZE__DEVICE_ID                   + 2)
#define PROGRAM_FLASH__FILE_OFFSET__COUNTER_C2                    (PROGRAM_FLASH__FILE_OFFSET__COUNTER_C1                  + PROGRAM_FLASH__DATA_SIZE__COUNTER_C1                  + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__SERIAL_NUMBER                 (PROGRAM_FLASH__FILE_OFFSET__COUNTER_C2                  + PROGRAM_FLASH__DATA_SIZE__COUNTER_C2                  + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__ENERGY                        (PROGRAM_FLASH__FILE_OFFSET__SERIAL_NUMBER               + PROGRAM_FLASH__DATA_SIZE__SERIAL_NUMBER               + 2)
#define PROGRAM_FLASH__FILE_OFFSET__ADC_VALUE_STATUS              (PROGRAM_FLASH__FILE_OFFSET__ENERGY                      + PROGRAM_FLASH__DATA_SIZE__ENERGY                      + 2)
//---------------------------------------------------------
#define PROGRAM_FLASH__FILE_OFFSET__ID_MODEL                      (PROGRAM_FLASH__FILE_OFFSET__ADC_VALUE_STATUS            + PROGRAM_FLASH__DATA_SIZE__ADC_VALUE_STATUS            + 2)




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*---------------------------------------------------------------------------
 * Flash data
 *---------------------------------------------------------------------------*/
/* Flash object IDs */
typedef enum
{
    PROGRAM_FLASH__FLASH_ID__FIRST = 0,

    /*---------------------------------------------------------------------------
     * "000 - Reset" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__000_RESET__FIRST                                  = 0,
    PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT                            = PROGRAM_FLASH__FLASH_ID__000_RESET__FIRST,
    PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER,
    PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE,
    PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_RESULT,
    PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER,
    PROGRAM_FLASH__FLASH_ID__000_RESET__LAST                                   = PROGRAM_FLASH__FLASH_ID__000_RESET__RESET_PHONE_NUMBER,

    /*---------------------------------------------------------------------------
     * "001 - Configurations" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FIRST                         = PROGRAM_FLASH__FLASH_ID__000_RESET__LAST + 1,

    /* general configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LANGUAGE                      = PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FIRST,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POD,

    /* commands */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_DETACHMENT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESTORE,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_STATUS,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMAND_RESET,

    /* SIM configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_APN,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__GPRS_DNS,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SIM_PIN,

    /* center configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MYN_NUMBER,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONEBOOK,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FWD_NUMBER,

    /* HTTP */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_TX,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_DATA,

    /* password configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PASSWORD,

    /* buttons */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__BUTTONS,

    /* credit configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CREDIT,

    /* counter inputs */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2,

    /* alarm configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_INPUT_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN3_SIGNAL_TYPE,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_ENABLE_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_INPUT_ALARM,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN4_SIGNAL_TYPE,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__POWER_ENABLE_ALARM,

    /* phone configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__PHONE_ACTIVITY_COUNTERS,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__SMS_DELAY,

    /* counter inputs */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C1_BANDS,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_C2_BANDS,

    /* answers */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_DETACHMENT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESTORE,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_STATUS,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANSWER_RESET,

    /* alarm mode */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ALARM_MODE,

    /* report configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__REPORT,

    /* FV action configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORESTORE,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__AUTORECONNECTION,

    /* ADC calibration configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC0_CALIBRATION,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ADC1_CALIBRATION,

    /* temperature mode */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_INT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MODE_TEMPERATURE_EXT,

    /* extra commands */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__COMMANDS_EXT,

    /* calibration */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_INT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__CALIBRATION_EXT,

    /* log status */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LOG_STATUS,

    /* log status */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ENERGY,

    /* functions */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_INT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_ANTIFROST_EXT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_INT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__F_CHRONO_EXT,

    /* names */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT1,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_OUT2,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN1,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN2,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN3,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_IN4,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMIN,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAX,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMINEXT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_TMAXEXT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C1,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__NAME_C2,

    /* messages */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT,

    /* timer output */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_INT,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__TIMER_OUTPUT_EXT,

    /* output configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT1,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ACTIVE_STATUS_OUT2,

    /* forward configurations */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__FORWARD,

    /* messages */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN1_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN2_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN3_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_IN4_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMIN_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAX_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMINEXT_R,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__MESSAGE_TMAXEXT_R,

    /* analog */
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE,
    PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LAST                          = PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__ANALOG_MODE,

    /*---------------------------------------------------------------------------
     * "002 - Phone Info" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__FIRST                             = PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY                    = PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__FIRST,
    PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__LAST                              = PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY,

    /*---------------------------------------------------------------------------
     * "003 - Queue" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__FIRST                                  = PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE                       = PROGRAM_FLASH__FLASH_ID__003_QUEUE__FIRST,
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE,
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE,
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE,
    PROGRAM_FLASH__FLASH_ID__003_QUEUE__LAST                                   = PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE,

    /*---------------------------------------------------------------------------
     * "004 - Alarm" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__004_ALARM__FIRST                                  = PROGRAM_FLASH__FLASH_ID__003_QUEUE__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__004_ALARM__ALARM_COUNTERS                         = PROGRAM_FLASH__FLASH_ID__004_ALARM__FIRST,
    PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM,
    PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST                                   = PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST_POWER_ALARM,

    /*---------------------------------------------------------------------------
     * "005 - Synchronize" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__FIRST                            = PROGRAM_FLASH__FLASH_ID__004_ALARM__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER                      = PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__FIRST,
    PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME,
    PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__LAST                             = PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SYNCHRONIZE_TIME,

    /*---------------------------------------------------------------------------
     * "006 - Random Configurations" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__FIRST                  = PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE            = PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__FIRST,
    PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION,
    PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__LAST                   = PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION,

    /*---------------------------------------------------------------------------
     * "007 - Clock" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__007_CLOCK__FIRST                                  = PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME                        = PROGRAM_FLASH__FLASH_ID__007_CLOCK__FIRST,
    PROGRAM_FLASH__FLASH_ID__007_CLOCK__LAST                                   = PROGRAM_FLASH__FLASH_ID__007_CLOCK__RTC_BACKUP_TIME,

    /*---------------------------------------------------------------------------
     * "008 - Retransmission" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__FIRST                         = PROGRAM_FLASH__FLASH_ID__007_CLOCK__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS                   = PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__FIRST,
    PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__LAST                          = PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS,

    /*---------------------------------------------------------------------------
     * "009 - Temperature Info" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__FIRST                       = PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS  = PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__FIRST,
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS,
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_INT_STATUS,
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS,
    PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__LAST                        = PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MIN_EXT_STATUS,

    /*---------------------------------------------------------------------------
     * "010 - Input Info" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__FIRST                             = PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN1_STATUS                  = PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__FIRST,
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN2_STATUS,
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN3_STATUS,
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS,
    PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__LAST                              = PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__INPUT_IN4_STATUS,

    /*---------------------------------------------------------------------------
     * "011 - Credit Info" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__FIRST                            = PROGRAM_FLASH__FLASH_ID__010_INPUT_INFO__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT                 = PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__FIRST,
    PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__LAST                             = PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__AVAILABLE_CREDIT,

    /*---------------------------------------------------------------------------
     * "012 - Output Info" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__FIRST                            = PROGRAM_FLASH__FLASH_ID__011_CREDIT_INFO__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS               = PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__FIRST,
    PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS,
    PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__LAST                             = PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS,

    /*---------------------------------------------------------------------------
     * "013 - Log" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__013_LOG__FIRST                                    = PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG                           = PROGRAM_FLASH__FLASH_ID__013_LOG__FIRST,
    PROGRAM_FLASH__FLASH_ID__013_LOG__LAST                                     = PROGRAM_FLASH__FLASH_ID__013_LOG__RX_COMMAND_LOG,

    /*---------------------------------------------------------------------------
     * "014 - Regulation Function" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__FIRST                           = PROGRAM_FLASH__FLASH_ID__013_LOG__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS         = PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__FIRST,
    PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS,
    PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__LAST                            = PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS,

    /*---------------------------------------------------------------------------
     * "015 - Device ID" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__FIRST                              = PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID                          = PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__FIRST,
    PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__LAST                               = PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__DEVICE_ID,

    /*---------------------------------------------------------------------------
     * "016 - Counters" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__016_COUNTERS__FIRST                               = PROGRAM_FLASH__FLASH_ID__015_DEVICE_ID__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1                          = PROGRAM_FLASH__FLASH_ID__016_COUNTERS__FIRST,
    PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2,
    PROGRAM_FLASH__FLASH_ID__016_COUNTERS__LAST                                = PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2,

    /*---------------------------------------------------------------------------
     * "017 - Serial Number" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__FIRST                          = PROGRAM_FLASH__FLASH_ID__016_COUNTERS__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER                  = PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__FIRST,
    PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__LAST                           = PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__SERIAL_NUMBER,

    /*---------------------------------------------------------------------------
     * "018 - Serial Number" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__FIRST                            = PROGRAM_FLASH__FLASH_ID__017_SERIAL_NUMBER__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS                 = PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__FIRST,
    PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__LAST                             = PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__ADC_VALUE_STATUS,

    /*---------------------------------------------------------------------------
     * "019 - Debug" - flash object IDs
     *---------------------------------------------------------------------------*/
    PROGRAM_FLASH__FLASH_ID__019_DEBUG__FIRST                                  = PROGRAM_FLASH__FLASH_ID__018_ENERGY_INFO__LAST + 1,
    PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL                               = PROGRAM_FLASH__FLASH_ID__019_DEBUG__FIRST,
    PROGRAM_FLASH__FLASH_ID__019_DEBUG__LAST                                   = PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL,

    PROGRAM_FLASH__FLASH_ID__LAST = PROGRAM_FLASH__FLASH_ID__019_DEBUG__ID_MODEL,
} PROGRAM_FLASH__FLASH_ID;

/* flash object handles indexes */
typedef enum
{
    PROGRAM_FLASH__HANDLE_INDEX__000_RESET,                  // 000 - Reset
    PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS,         // 001 - Configurations
    PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO,             // 002 - Phone   Info
    PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE,                  // 003 - Queue
    PROGRAM_FLASH__HANDLE_INDEX__004_ALARM,                  // 004 - Alarm
    PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE,            // 005 - Synchronize
    PROGRAM_FLASH__HANDLE_INDEX__006_RANDOM_CONFIGURATIONS,  // 006 - Random Configurations
    PROGRAM_FLASH__HANDLE_INDEX__007_CLOCK,                  // 007 - Clock
    PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION,         // 008 - Retransmission
    PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO,       // 009 - Temperature Info
    PROGRAM_FLASH__HANDLE_INDEX__010_INPUT_INFO,             // 010 - Input       Info
    PROGRAM_FLASH__HANDLE_INDEX__011_CREDIT_INFO,            // 011 - Credit      Info
    PROGRAM_FLASH__HANDLE_INDEX__012_OUTPUT_INFO,            // 012 - Output      Info
    PROGRAM_FLASH__HANDLE_INDEX__013_LOG,                    // 013 - Log
    PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION,           // 014 - Regulation Function
    PROGRAM_FLASH__HANDLE_INDEX__015_DEVICE_ID,              // 015 - Device ID
    PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS,               // 016 - Counters
    PROGRAM_FLASH__HANDLE_INDEX__017_SERIAL_NUMBER,          // 017 - Serial Number
    PROGRAM_FLASH__HANDLE_INDEX__018_ENERGY_INFO,            // 018 - Energy      Info
    PROGRAM_FLASH__HANDLE_INDEX__019_DEBUG,                  // 019 - Debug
} PROGRAM_FLASH__HANDLE_INDEX;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void   ProgramFlash_TaskProgramFlash(void *argument);

/* flash init */
bool   ProgramFlash_InitFlashAll    (void);
bool   ProgramFlash_InitFlashHandle (PROGRAM_FLASH__HANDLE_INDEX handle_index);
bool   ProgramFlash_InitFlashId     (PROGRAM_FLASH__FLASH_ID     id_index    );

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
bool   ProgramFlash_BackupWrite(PROGRAM_FLASH__FLASH_ID id_index);




#endif
