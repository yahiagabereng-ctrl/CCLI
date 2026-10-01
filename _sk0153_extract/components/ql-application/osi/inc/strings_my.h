/*=============================================================================
 * File       :  STRINGS_MY.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - text strings
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __STRINGS_MY_H__


#define __STRINGS_MY_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
typedef enum
{
    STRINGS__EMPTY,                                               // ""

    /*---------------------------------------------------------------------------
     * Alarm strings
     *---------------------------------------------------------------------------*/
    STRINGS__ALARM__TMIN,                                         // "Alarm internal temperature too low"
    STRINGS__ALARM__TMINEXT,                                      // "Alarm external temperature too low"
    STRINGS__ALARM__TMIN_RESTORED,                                // "Alarm internal temperature too low restored"
    STRINGS__ALARM__TMINEXT_RESTORED,                             // "Alarm external temperature too low restored"
    STRINGS__ALARM__TMAX,                                         // "Alarm internal temperature too high"
    STRINGS__ALARM__TMAXEXT,                                      // "Alarm external temperature too high
    STRINGS__ALARM__TMAX_RESTORED,                                // "Alarm internal temperature too high restored"
    STRINGS__ALARM__TMAXEXT_RESTORED,                             // "Alarm external temperature too high restored

    STRINGS__ALARM__IN1,                                          // "Alarm IN1 input"
    STRINGS__ALARM__IN2,                                          // "Alarm IN2 input"
    STRINGS__ALARM__IN3,                                          // "Alarm IN3 input"
    STRINGS__ALARM__IN4,                                          // "Alarm IN4 input"
    STRINGS__ALARM__IN1_RESTORED,                                 // "IN1 input restored"
    STRINGS__ALARM__IN2_RESTORED,                                 // "IN2 input restored"
    STRINGS__ALARM__IN3_RESTORED,                                 // "IN3 input restored"
    STRINGS__ALARM__IN4_RESTORED,                                 // "IN4 input restored"

    STRINGS__ALARM__POWER_OFF,                                    // "Mains power interruption alarm"
    STRINGS__ALARM__POWER_RETURN,                                 // "Mains power returned"

    STRINGS__ALARM__CREDIT,                                       // "Warning, credit is 10 SMS"


    /*---------------------------------------------------------------------------
     * Weekday strings
     *---------------------------------------------------------------------------*/
    STRINGS__WEEKDAY__MON,                                        // "Mon"
    STRINGS__WEEKDAY__TUE,                                        // "Tue"
    STRINGS__WEEKDAY__WED,                                        // "Wed"
    STRINGS__WEEKDAY__THU,                                        // "Thu"
    STRINGS__WEEKDAY__FRI,                                        // "Fri"
    STRINGS__WEEKDAY__SAT,                                        // "Sat"
    STRINGS__WEEKDAY__SUN,                                        // "Sun"


    /*---------------------------------------------------------------------------
     * FW upgrade result
     *---------------------------------------------------------------------------*/
    /* labels */
    STRINGS__FW_UPGRADE__LABEL__FIRMWARE_UPGRADE_RESULT,          // "Firmware upgrade result"
    STRINGS__FW_UPGRADE__LABEL__ERROR,                            // "Error"
    STRINGS__FW_UPGRADE__LABEL__ACTUAL_VERSION,                   // "Actual version"

    /* values */
    STRINGS__FW_UPGRADE__VALUE__RESULT__UNKNOWN,                  // "unknown"
    STRINGS__FW_UPGRADE__VALUE__RESULT__FAILED,                   // "failed"
    STRINGS__FW_UPGRADE__VALUE__RESULT__EXECUTED,                 // "executed"
    STRINGS__FW_UPGRADE__VALUE__ERROR__UNKNOWN,                   // "unknown"
    STRINGS__FW_UPGRADE__VALUE__ERROR__NO_ERROR,                  // "no error"
    STRINGS__FW_UPGRADE__VALUE__ERROR__OTHER_ERROR,               // "other error"
    STRINGS__FW_UPGRADE__VALUE__ERROR__NO_GSM_REGISTRATION,       // "no GSM network registration"
    STRINGS__FW_UPGRADE__VALUE__ERROR__NO_APN_CONNECTION,         // "no GPRS APN connection"
    STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_SERVER_CONNECTION,  // "no FTP server connection"
    STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_FILE_DOWNLOAD,      // "no FTP file download"
    STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FILE_INSTALLATION,      // "no file installation"


    /*---------------------------------------------------------------------------
     * Generic answers to SMS commands
     *---------------------------------------------------------------------------*/
  //STRINGS__ANSWER__COMMAND_EXECUTED,                            // "Command executed"
  //STRINGS__ANSWER__COMMAND_NOT_EXECUTED,                        // "Command not executed"
    STRINGS__ANSWER__COMMAND_ERROR,                               // "Command error"
  //STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR,                    // "Parameters command error"

    /* password */
    STRINGS__ANSWER__PASSWORD_REQUIRED,                           // "PASSWORD REQUIRED"
    STRINGS__ANSWER__PASSWORD_INCORRECT,                          // "INCORRECT PASSWORD"

    /* configuration (set) */
    STRINGS__ANSWER__CONFIGURATION_ACCEPTED,                      // "CONFIGURATION ACCEPTED"
    STRINGS__ANSWER__CONFIGURATION_NOT_ACCEPTED,                  // "CONFIGURATION NOT ACCEPTED"
    STRINGS__ANSWER__CONFIGURATION_SYNTAX_ERROR,                  // "CONFIGURATION SYNTAX ERROR"
    STRINGS__ANSWER__CONFIGURATION_PARAMETERS_ERROR,              // "CONFIGURATION PARAMETERS ERROR"

    /* telecontrol */
    STRINGS__ANSWER__COMMAND_EXECUTED,                            // "COMMAND EXECUTED"
    STRINGS__ANSWER__COMMAND_NOT_EXECUTED,                        // "COMMAND NOT EXECUTED"
    STRINGS__ANSWER__COMMAND_NOT_EXECUTABLE,                      // "COMMAND NOT EXECUTABLE"
    STRINGS__ANSWER__COMMAND_SYNTAX_ERROR,                        // "COMMAND SYNTAX ERROR"
    STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR,                    // "COMMAND PARAMETERS ERROR"


    /*---------------------------------------------------------------------------
     * Commands
     *---------------------------------------------------------------------------*/
    /* status reading */
    STRINGS__COMMAND__STATUS,                                     // "STATUS"
    STRINGS__COMMAND__VERSION,                                    // "VERSION"
    STRINGS__COMMAND__GETTIME,                                    // "GETTIME"

    /* configuration (get) */
    STRINGS__COMMAND__SETUP,                                      // "SETUP"

    /* configuration (set) */
    STRINGS__COMMAND__ADD,                                        // "ADD"
    STRINGS__COMMAND__REMOVE,                                     // "REMOVE"
    STRINGS__COMMAND__CONTACTS,                                   // "CONTACTS"
    STRINGS__COMMAND__NEWNAME,                                    // "NEWNAME"
    STRINGS__COMMAND__ALARM,                                      // "ALARM"
    STRINGS__COMMAND__MESSAGE,                                    // "MESSAGE"
    STRINGS__COMMAND__CALIBRATE,                                  // "CALIBRATE"
    STRINGS__COMMAND__SETPSW,                                     // "SETPSW"
    STRINGS__COMMAND__SETREPORT,                                  // "SETREPORT"
    STRINGS__COMMAND__LANGUAGE,                                   // "LANGUAGE"
    STRINGS__COMMAND__LANGUAGE_ENG,                               // "LANGUAGE"     // in English for all languages
    STRINGS__COMMAND__TIMEROUT,                                   // "TIMEROUT"
    STRINGS__COMMAND__FORWARD,                                    // "FORWARD"
    STRINGS__COMMAND__CHRONO,                                     // "CHRONO"
    STRINGS__COMMAND__SUMMER,                                     // "SUMMER"
    STRINGS__COMMAND__WINTER,                                     // "WINTER"
    STRINGS__COMMAND__BUTTONS,                                    // "BUTTONS"
    STRINGS__COMMAND__SMSDELAY,                                   // "SMSDELAY"
    STRINGS__COMMAND__COUNTER,                                    // "COUNTER"
    STRINGS__COMMAND__COUNTERBANDS,                               // "COUNTERBANDS"

    /* telecontrol */
    STRINGS__COMMAND__TURNON,                                     // "TURNON"
    STRINGS__COMMAND__TURNOFF,                                    // "TURNOFF"
    STRINGS__COMMAND__REGULATE,                                   // "REGULATE"
    STRINGS__COMMAND__ANTIFROST,                                  // "ANTIFROST"
    STRINGS__COMMAND__CREDIT,                                     // "CREDIT"
    STRINGS__COMMAND__DEFAULT,                                    // "DEFAULT"
    STRINGS__COMMAND__SETTIME,                                    // "SETTIME"
    STRINGS__COMMAND__SYNCHRONIZE,                                // "SYNCHRONIZE"
    STRINGS__COMMAND__RESET,                                      // "RESET"
    STRINGS__COMMAND__SETCOUNTER,                                 // "SETCOUNTER"
    STRINGS__COMMAND__SETCOUNTERBANDS,                            // "SETCOUNTERBANDS"


    /* data acquistion */
    STRINGS__COMMAND__COMMANDS,                                   // "COMMANDS"


    /*---------------------------------------------------------------------------
     * Labels and values for commands and answers
     *---------------------------------------------------------------------------*/

     /*-----------------------------------------------------------------------------
     * "STATUS"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer  labels */
    STRINGS__STATUS__ANS__LABEL__TEMP,                            // "TEMP."
    STRINGS__STATUS__ANS__LABEL__TINT,                            // "TIN3"
    STRINGS__STATUS__ANS__LABEL__TEXT,                            // "TIN4"
    STRINGS__STATUS__ANS__LABEL__OUT1,                            // "OUT1"
    STRINGS__STATUS__ANS__LABEL__OUT2,                            // "OUT2"
    STRINGS__STATUS__ANS__LABEL__REGULATE,                        // "REGOLA"
    STRINGS__STATUS__ANS__LABEL__REG1,                            // "REG1"
    STRINGS__STATUS__ANS__LABEL__REG2,                            // "REG2"
    STRINGS__STATUS__ANS__LABEL__ANTIFROST,                       // "ANTIGELO"
    STRINGS__STATUS__ANS__LABEL__ANT1,                            // "ANT1"
    STRINGS__STATUS__ANS__LABEL__ANT2,                            // "ANT2"
    STRINGS__STATUS__ANS__LABEL__CHRONO,                          // "CRONO"
    STRINGS__STATUS__ANS__LABEL__CHR1,                            // "CR1"
    STRINGS__STATUS__ANS__LABEL__CHR2,                            // "CR2"
    STRINGS__STATUS__ANS__LABEL__IN1,                             // "IN1"
    STRINGS__STATUS__ANS__LABEL__IN2,                             // "IN2"
    STRINGS__STATUS__ANS__LABEL__IN3,                             // "IN3"
    STRINGS__STATUS__ANS__LABEL__IN4,                             // "IN4"
    STRINGS__STATUS__ANS__LABEL__C1,                              // "C1"
    STRINGS__STATUS__ANS__LABEL__C2,                              // "C2"
    STRINGS__STATUS__ANS__LABEL__SIGNAL,                          // "SEGNALE"
    STRINGS__STATUS__ANS__LABEL__GSM,                             // "GSM"
    STRINGS__STATUS__ANS__LABEL__CREDIT,                          // "CREDITO"
    STRINGS__STATUS__ANS__LABEL__SMS,                             // "SMS"
    STRINGS__STATUS__ANS__LABEL__POWER,                           // "POWER"
    STRINGS__STATUS__ANS__LABEL__ADC_INT,                         // "ADC_INT"
    STRINGS__STATUS__ANS__LABEL__ADC_EXT,                         // "ADC_EXT"

    /* answer  values */
    STRINGS__STATUS__ANS__VALUE__TEMPERATURE__ERROR,              // "ERR"
    STRINGS__STATUS__ANS__VALUE__ON,                              // "ON"
    STRINGS__STATUS__ANS__VALUE__OFF,                             // "OFF"
    STRINGS__STATUS__ANS__VALUE__OPEN,                            // "APERTO"
    STRINGS__STATUS__ANS__VALUE__CLOSE,                           // "CHIUSO"
    STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__LOW,                 // "SCARSO"
    STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__MEDIUM,              // "MEDIO"
    STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__GOOD,                // "BUONO"
    STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__EXCELLENT,           // "OTTIMO"
    STRINGS__STATUS__ANS__VALUE__YES,                             // "SI"
    STRINGS__STATUS__ANS__VALUE__NO,                              // "NO"

    /*-----------------------------------------------------------------------------
     * "VERSION"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    STRINGS__VERSION__ANS__LABEL__MANUFACTURER,                   // "MANUFACTURER"
    STRINGS__VERSION__ANS__LABEL__MODEL,                          // "MODEL"
    STRINGS__VERSION__ANS__LABEL__VERSION,                        // "VERSION"

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "GETTIME"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETUP" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__SETUP__CMD__VALUE__CHRONO,                           // "CHRONO"
    STRINGS__SETUP__CMD__VALUE__COUNTERBANDS,                     // "COUNTERBANDS"
    STRINGS__SETUP__CMD__VALUE__OUT1,                             // "OUT1"
    STRINGS__SETUP__CMD__VALUE__OUT2,                             // "OUT2"
    STRINGS__SETUP__CMD__VALUE__C1,                               // "C1"
    STRINGS__SETUP__CMD__VALUE__C2,                               // "C2"

    /* answer labels */
    STRINGS__SETUP__ANS__LABEL__TMIN,                             // "TIN3MIN"
    STRINGS__SETUP__ANS__LABEL__TMINEXT,                          // "TIN4MIN"
    STRINGS__SETUP__ANS__LABEL__TMAX,                             // "TIN3MAX"
    STRINGS__SETUP__ANS__LABEL__TMAXEXT,                          // "TIN4MAX"
    STRINGS__SETUP__ANS__LABEL__C1,                               // "C1"
    STRINGS__SETUP__ANS__LABEL__C2,                               // "C2"
    STRINGS__SETUP__ANS__LABEL__IN1,                              // "IN1"
    STRINGS__SETUP__ANS__LABEL__IN2,                              // "IN2"
    STRINGS__SETUP__ANS__LABEL__IN3,                              // "IN3"
    STRINGS__SETUP__ANS__LABEL__IN4,                              // "IN4"
    STRINGS__SETUP__ANS__LABEL__TOUT1,                            // "TOUT1"
    STRINGS__SETUP__ANS__LABEL__TOUT2,                            // "TOUT2"
    STRINGS__SETUP__ANS__LABEL__TINT,                             // "TIN3"
    STRINGS__SETUP__ANS__LABEL__TEXT,                             // "TIN4"
    STRINGS__SETUP__ANS__LABEL__POWER,                            // "POWER"
    STRINGS__SETUP__ANS__LABEL__REPORT,                           // "REPORT"
    STRINGS__SETUP__ANS__LABEL__FORWARD,                          // "FORWARD"
    STRINGS__SETUP__ANS__LABEL__SMSDELAY,                         // "SMSDELAY"
    STRINGS__SETUP__ANS__LABEL__BUTTONS,                          // "BUTTONS"
    STRINGS__SETUP__ANS__LABEL__CHRONO,                           // "CHRONO"
    STRINGS__SETUP__ANS__LABEL__COUNTERBANDS,                     // "COUNTERBANDS"
    STRINGS__SETUP__ANS__LABEL__OUT1,                             // "OUT1"
    STRINGS__SETUP__ANS__LABEL__OUT2,                             // "OUT2"

    /* answer values */
    STRINGS__SETUP__ANS__VALUE__ON,                               // "ON"
    STRINGS__SETUP__ANS__VALUE__OFF,                              // "OFF"
    STRINGS__SETUP__ANS__VALUE__O,                                // "O"
    STRINGS__SETUP__ANS__VALUE__C,                                // "C"
    STRINGS__SETUP__ANS__VALUE__S,                                // "S"
    STRINGS__SETUP__ANS__VALUE__M,                                // "M"
    STRINGS__SETUP__ANS__VALUE__H,                                // "H"
    STRINGS__SETUP__ANS__VALUE__SUM,                              // "EST"
    STRINGS__SETUP__ANS__VALUE__WIN,                              // "INV"


    /*-----------------------------------------------------------------------------
     * "ADD" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "REMOVE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "CONTACTS" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    STRINGS__CONTACTS__ANS__VALUE__PHONEBOOK_EMPTY,               // "Phonebook empty"


    /*-----------------------------------------------------------------------------
     * "NEWNAME" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    STRINGS__NEWNAME__CMD__LABEL__OUT1,                           // "OUT1"
    STRINGS__NEWNAME__CMD__LABEL__OUT2,                           // "OUT2"
    STRINGS__NEWNAME__CMD__LABEL__IN1,                            // "IN1"
    STRINGS__NEWNAME__CMD__LABEL__IN2,                            // "IN2"
    STRINGS__NEWNAME__CMD__LABEL__IN3,                            // "IN3"
    STRINGS__NEWNAME__CMD__LABEL__IN4,                            // "IN4"
    STRINGS__NEWNAME__CMD__LABEL__TMIN,                           // "TIN3MIN"
    STRINGS__NEWNAME__CMD__LABEL__TMAX,                           // "TIN3MAX"
    STRINGS__NEWNAME__CMD__LABEL__TMINEXT,                        // "TIN4MIN"
    STRINGS__NEWNAME__CMD__LABEL__TMAXEXT,                        // "TIN4MAX"
    STRINGS__NEWNAME__CMD__LABEL__C1,                             // "C1"
    STRINGS__NEWNAME__CMD__LABEL__C2,                             // "C2"

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */


    /*-----------------------------------------------------------------------------
     * "ALARM" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__ALARM__CMD__VALUE__TMIN,                             // "TIN3MIN"
    STRINGS__ALARM__CMD__VALUE__TMINEXT,                          // "TIN4MIN"
    STRINGS__ALARM__CMD__VALUE__TMAX,                             // "TIN3MAX"
    STRINGS__ALARM__CMD__VALUE__TMAXEXT,                          // "TIN4MAX"
    STRINGS__ALARM__CMD__VALUE__IN1,                              // "IN1"
    STRINGS__ALARM__CMD__VALUE__IN2,                              // "IN2"
    STRINGS__ALARM__CMD__VALUE__IN3,                              // "IN3"
    STRINGS__ALARM__CMD__VALUE__IN4,                              // "IN4"
    STRINGS__ALARM__CMD__VALUE__POWER,                            // "POWER"
    STRINGS__ALARM__CMD__VALUE__ENABLED,                          // "ATTIVO"
    STRINGS__ALARM__CMD__VALUE__DISABLED,                         // "DISATTIVO"
    STRINGS__ALARM__CMD__VALUE__OPEN,                             // "APERTO"
    STRINGS__ALARM__CMD__VALUE__CLOSE,                            // "CHIUSO"
    STRINGS__ALARM__CMD__VALUE__S,                                // "S"
    STRINGS__ALARM__CMD__VALUE__M,                                // "M"
    STRINGS__ALARM__CMD__VALUE__H,                                // "H"

    /* answer labels */
    STRINGS__ALARM__ANS__LABEL__TMIN,                             // "TIN3MIN"
    STRINGS__ALARM__ANS__LABEL__TMINEXT,                          // "TIN4MIN"
    STRINGS__ALARM__ANS__LABEL__TMAX,                             // "TIN3MAX"
    STRINGS__ALARM__ANS__LABEL__TMAXEXT,                          // "TIN4MAX"
    STRINGS__ALARM__ANS__LABEL__IN1,                              // "IN1"
    STRINGS__ALARM__ANS__LABEL__IN2,                              // "IN2"
    STRINGS__ALARM__ANS__LABEL__IN3,                              // "IN3"
    STRINGS__ALARM__ANS__LABEL__IN4,                              // "IN4"
    STRINGS__ALARM__ANS__LABEL__POWER,                            // "POWER"
    STRINGS__ALARM__ANS__LABEL__REPORT,                           // "REPORT"

    /* answer values */
    STRINGS__ALARM__ANS__VALUE__ENABLED,                          // "ATTIVO"
    STRINGS__ALARM__ANS__VALUE__DISABLED,                         // "DISATTIVO"
    STRINGS__ALARM__ANS__VALUE__OPEN,                             // "APERTO"
    STRINGS__ALARM__ANS__VALUE__CLOSE,                            // "CHIUSO"
    STRINGS__ALARM__ANS__VALUE__S,                                // "S"
    STRINGS__ALARM__ANS__VALUE__M,                                // "M"
    STRINGS__ALARM__ANS__VALUE__H,                                // "H"


    /*-----------------------------------------------------------------------------
     * "MESSAGE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__MESSAGE__CMD__VALUE__IN1,                            // "IN1"
    STRINGS__MESSAGE__CMD__VALUE__IN2,                            // "IN2"
    STRINGS__MESSAGE__CMD__VALUE__IN3,                            // "IN3"
    STRINGS__MESSAGE__CMD__VALUE__IN4,                            // "IN4"
    STRINGS__MESSAGE__CMD__VALUE__TMIN,                           // "TIN3MIN"
    STRINGS__MESSAGE__CMD__VALUE__TMINEXT,                        // "TIN4MIN"
    STRINGS__MESSAGE__CMD__VALUE__TMAX,                           // "TIN3MAX"
    STRINGS__MESSAGE__CMD__VALUE__TMAXEXT,                        // "TIN4MAX"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "CALIBRATE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__CALIBRATE__CMD__VALUE__INT,                          // "IN3"
    STRINGS__CALIBRATE__CMD__VALUE__EXT,                          // "IN4"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETPSW" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETREPORT" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__SETREPORT__CMD__VALUE__STATUS__OFF,                  // "OFF"
    STRINGS__SETREPORT__CMD__VALUE__STATUS__ON,                   // "ON"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "LANGUAGE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__ITA,                 // "ITA"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__ENG,                 // "ENG"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__FRE,                 // "FRE"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__GER,                 // "GER"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__SPA,                 // "SPA"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__POL,                 // "POL"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__SWE,                 // "SWE"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "LANGUAGE" command     // in English for all languages
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__ITA,             // "ITA"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__ENG,             // "ENG"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__FRE,             // "FRE"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__GER,             // "GER"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__SPA,             // "SPA"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__POL,             // "POL"
    STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__SWE,             // "SWE"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "TIMEROUT" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__TIMEROUT__CMD__VALUE__OUT1,                          // "OUT1"
    STRINGS__TIMEROUT__CMD__VALUE__OUT2,                          // "OUT2"
    STRINGS__TIMEROUT__CMD__VALUE__OUT1__S,                       // "S"
    STRINGS__TIMEROUT__CMD__VALUE__OUT1__M,                       // "M"
    STRINGS__TIMEROUT__CMD__VALUE__OUT1__H,                       // "H"
    STRINGS__TIMEROUT__CMD__VALUE__OUT2__S,                       // "S"
    STRINGS__TIMEROUT__CMD__VALUE__OUT2__M,                       // "M"
    STRINGS__TIMEROUT__CMD__VALUE__OUT2__H,                       // "H"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "FORWARD" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__FORWARD__CMD__VALUE__STATUS__OFF,                    // "OFF"
    STRINGS__FORWARD__CMD__VALUE__STATUS__ON,                     // "ON"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "CHRONO" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__CHRONO__CMD__VALUE__OUT1,                            // "OUT1"
    STRINGS__CHRONO__CMD__VALUE__OUT2,                            // "OUT2"
    STRINGS__CHRONO__CMD__VALUE__STATUS__OFF,                     // "OFF"
    STRINGS__CHRONO__CMD__VALUE__STATUS__ON,                      // "ON"
    STRINGS__CHRONO__CMD__VALUE__WINDOW__OFF,                     // "OFF"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SUMMER" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__SUMMER__CMD__VALUE__INT,                             // "IN3"
    STRINGS__SUMMER__CMD__VALUE__EXT,                             // "IN4"

    /* answer labels */
    // ---

    /* answer values */
    // ---

    /* answer labels */
    // ---


    /*-----------------------------------------------------------------------------
     * "WINTER" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__WINTER__CMD__VALUE__INT,                             // "IN3"
    STRINGS__WINTER__CMD__VALUE__EXT,                             // "IN4"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "BUTTONS" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__BUTTONS__CMD__VALUE__STATUS__OFF,                    // "OFF"
    STRINGS__BUTTONS__CMD__VALUE__STATUS__ON,                     // "ON"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "DELAYSMS" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__SMSDELAY__CMD__VALUE__OFF,                           // "OFF"
    STRINGS__SMSDELAY__CMD__VALUE__S,                             // "S"
    STRINGS__SMSDELAY__CMD__VALUE__M,                             // "M"
    STRINGS__SMSDELAY__CMD__VALUE__H,                             // "H"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "COUNTER" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__COUNTER__CMD__VALUE__C1,                              // "C1"
    STRINGS__COUNTER__CMD__VALUE__C2,                              // "C2"
    STRINGS__COUNTER__CMD__VALUE__STATUS__OFF,                     // "OFF"
    STRINGS__COUNTER__CMD__VALUE__STATUS__ON,                      // "ON"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "COUNTERBANDS" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__COUNTERBANDS__CMD__VALUE__C1,                         // "C1"
    STRINGS__COUNTERBANDS__CMD__VALUE__C2,                         // "C2"
    STRINGS__COUNTERBANDS__CMD__VALUE__STATUS__OFF,                // "OFF"
    STRINGS__COUNTERBANDS__CMD__VALUE__STATUS__ON,                 // "ON"
    STRINGS__COUNTERBANDS__CMD__VALUE__BAND__OFF,                  // "OFF"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "TURNON" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__TURNON__CMD__VALUE__OUT1,                            // "OUT1"
    STRINGS__TURNON__CMD__VALUE__OUT2,                            // "OUT2"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "TURNOFF" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__TURNOFF__CMD__VALUE__OUT1,                           // "OUT1"
    STRINGS__TURNOFF__CMD__VALUE__OUT2,                           // "OUT2"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "REGULATE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__REGULATE__CMD__VALUE__OUT1,                          // "OUT1"
    STRINGS__REGULATE__CMD__VALUE__OUT2,                          // "OUT2"
    STRINGS__REGULATE__CMD__VALUE__OFF,                           // "OFF"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "ANTIFROST" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__ANTIFROST__CMD__VALUE__OUT1,                         // "OUT1"
    STRINGS__ANTIFROST__CMD__VALUE__OUT2,                         // "OUT2"
    STRINGS__ANTIFROST__CMD__VALUE__OFF,                          // "OFF"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "CREDIT" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__CREDIT__CMD__VALUE__OFF,                             // "OFF"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "DEFAULT" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__DEFAULT__CMD__VALUE__CONTACTS,                       // "CONTACTS"
    STRINGS__DEFAULT__CMD__VALUE__COMMANDS,                       // "COMMANDS"
    STRINGS__DEFAULT__CMD__VALUE__ALL,                            // "ALL"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETTIME" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SYNCHRONIZE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "RESET" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETCOUNTER" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__SETCOUNTER__CMD__VALUE__C1,                        // "C1"
    STRINGS__SETCOUNTER__CMD__VALUE__C2,                        // "C2"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETCOUNTERBANDS" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    STRINGS__SETCOUNTERBANDS__CMD__VALUE__C1,                   // "C1"
    STRINGS__SETCOUNTERBANDS__CMD__VALUE__C2,                   // "C2"

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "COMMANDS" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    STRINGS__COMMANDS__ANS__VALUE__NO_COMMANDS,               // "No commands"


    /*-----------------------------------------------------------------------------
     * number of defined strings
     *-----------------------------------------------------------------------------*/
    STRINGS__NUMBERS_OF_STRINGS
} STRINGS__ID_STRING;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
ascii *Strings_GetStringOfLanguage(u8 id_language, u16 id_string);
ascii *Strings_GetString          (                u16 id_string);




#endif
