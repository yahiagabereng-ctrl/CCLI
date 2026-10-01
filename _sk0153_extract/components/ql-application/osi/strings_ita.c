/*=============================================================================
 * File       :  STRINGS_ITA.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - text strings - Italian
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "fw_config.h"
#include "strings_ita.h"
#include "strings_my.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* strings - Italian */
const ascii *const StringsIta_Strings[] =
{
    "",                                                                                                              // STRINGS__EMPTY

    /*---------------------------------------------------------------------------
     * Alarm strings
     *---------------------------------------------------------------------------*/
    "Allarme temperatura IN3 troppo bassa",                                                                          // STRINGS__ALARM__TMIN
    "Allarme temperatura IN4 troppo bassa",                                                                          // STRINGS__ALARM__TMINEXT
    "Ripristino temperatura IN3 troppo bassa",                                                                       // STRINGS__ALARM__TMIN_RESTORED
    "Ripristino temperatura IN4 troppo bassa",                                                                       // STRINGS__ALARM__TMINEXT_RESTORED

    "Allarme temperatura IN3 troppo alta",                                                                           // STRINGS__ALARM__TMAX
    "Allarme temperatura IN4 troppo alta",                                                                           // STRINGS__ALARM__TMAXEXT
    "Ripristino temperatura IN3 troppo alta",                                                                        // STRINGS__ALARM__TMAX_RESTORED
    "Ripristino temperatura IN4 troppo alta",                                                                        // STRINGS__ALARM__TMAXEXT_RESTORED

    "Allarme ingresso IN1",                                                                                          // STRINGS__ALARM__IN1
    "Allarme ingresso IN2",                                                                                          // STRINGS__ALARM__IN2
    "Allarme ingresso IN3",                                                                                          // STRINGS__ALARM__IN3
    "Allarme ingresso IN4",                                                                                          // STRINGS__ALARM__IN4
    "Ripristino ingresso IN1",                                                                                       // STRINGS__ALARM__IN1_RESTORED
    "Ripristino ingresso IN2",                                                                                       // STRINGS__ALARM__IN2_RESTORED
    "Ripristino ingresso IN3",                                                                                       // STRINGS__ALARM__IN3_RESTORED
    "Ripristino ingresso IN4",                                                                                       // STRINGS__ALARM__IN4_RESTORED

#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    "Allarme interruzione rete elettrica",                                                                           // STRINGS__ALARM__POWER_OFF
    "Rete elettrica ripristinata",                                                                                   // STRINGS__ALARM__POWER_RETURN
#endif
#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    "Allarme interruzione alimentazione principale",                                                                 // STRINGS__ALARM__POWER_OFF
    "Alimentazione principale ripristinata",                                                                         // STRINGS__ALARM__POWER_RETURN
#endif

    "Attenzione, il credito risulta pari a 10 SMS",                                                                  // STRINGS__ALARM__CREDIT


    /*---------------------------------------------------------------------------
     * Weekday strings
     *---------------------------------------------------------------------------*/
    "Lun",                                                                                                           // STRINGS__WEEKDAY__MON
    "Mar",                                                                                                           // STRINGS__WEEKDAY__TUE
    "Mer",                                                                                                           // STRINGS__WEEKDAY__WED
    "Gio",                                                                                                           // STRINGS__WEEKDAY__THU
    "Ven",                                                                                                           // STRINGS__WEEKDAY__FRI
    "Sab",                                                                                                           // STRINGS__WEEKDAY__SAT
    "Dom",                                                                                                           // STRINGS__WEEKDAY__SUN


    /*---------------------------------------------------------------------------
     * FW upgrade result
     *---------------------------------------------------------------------------*/
    /* labels */
    "Esito aggiornamento firmware",                                                                                  // STRINGS__FW_UPGRADE__LABEL__FIRMWARE_UPGRADE_RESULT
    "Errore",                                                                                                        // STRINGS__FW_UPGRADE__LABEL__ERROR
    "Attuale versione",                                                                                              // STRINGS__FW_UPGRADE__LABEL__ACTUAL_VERSION

    /* values */
    "sconosciuto",                                                                                                   // STRINGS__FW_UPGRADE__VALUE__RESULT__UNKNOWN
    "fallito",                                                                                                       // STRINGS__FW_UPGRADE__VALUE__RESULT__FAILED
    "eseguito",                                                                                                      // STRINGS__FW_UPGRADE__VALUE__RESULT__EXECUTED
    "sconosciuto",                                                                                                   // STRINGS__FW_UPGRADE__VALUE__ERROR__UNKNOWN
    "no errori",                                                                                                     // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_ERROR
    "altro errore",                                                                                                  // STRINGS__FW_UPGRADE__VALUE__ERROR__OTHER_ERROR
    "no registrazione alla rete GSM",                                                                                // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_GSM_REGISTRATION
    "no connessione all'APN GPRS",                                                                                   // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_APN_CONNECTION
    "no connessione al server FTP",                                                                                  // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_SERVER_CONNECTION
    "no download FTP del file",                                                                                      // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_FILE_DOWNLOAD
    "no installazione del file",                                                                                     // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FILE_INSTALLATION


    /*---------------------------------------------------------------------------
     * Generic answers to SMS commands
     *---------------------------------------------------------------------------*/
  //"Comando eseguito",                                                                                              // STRINGS__ANSWER__COMMAND_EXECUTED
  //"Comando non eseguito",                                                                                          // STRINGS__ANSWER__COMMAND_NOT_EXECUTED
    "Errore comando",                                                                                                // STRINGS__ANSWER__COMMAND_ERROR
  //"Errore parametri comando",                                                                                      // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR

    /* password */
    "PASSWORD RICHIESTA",                                                                                            // STRINGS__ANSWER__PASSWORD_REQUIRED
    "PASSWORD NON CORRETTA",                                                                                         // STRINGS__ANSWER__PASSWORD_INCORRECT

    /* configuration (set) */
    "CONFIGURAZIONE ACCETTATA",                                                                                      // STRINGS__ANSWER__CONFIGURATION_ACCEPTED
    "CONFIGURAZIONE NON ACCETTATA",                                                                                  // STRINGS__ANSWER__CONFIGURATION_NOT_ACCEPTED
    "ERRORE SINTASSI CONFIGURAZIONE",                                                                                // STRINGS__ANSWER__CONFIGURATION_SYNTAX_ERROR
    "ERRORE PARAMETRI CONFIGURAZIONE",                                                                               // STRINGS__ANSWER__CONFIGURATION_PARAMETERS_ERROR

    /* telecontrol */
    "COMANDO ESEGUITO",                                                                                              // STRINGS__ANSWER__COMMAND_EXECUTED
    "COMANDO NON ESEGUITO",                                                                                          // STRINGS__ANSWER__COMMAND_NOT_EXECUTED
    "COMANDO NON ESEGUIBILE",                                                                                        // STRINGS__ANSWER__COMMAND_NOT_EXECUTABLE
    "ERRORE SINTASSI COMANDO",                                                                                       // STRINGS__ANSWER__COMMAND_SYNTAX_ERROR
    "ERRORE PARAMETRI COMANDO",                                                                                      // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR


    /*---------------------------------------------------------------------------
     * Commands
     *---------------------------------------------------------------------------*/
    /* status reading */
    "STATO",                                                                                                         // STRINGS__COMMAND__STATUS
    "VERSIONE",                                                                                                      // STRINGS__COMMAND__VERSION
    "GETTIME",                                                                                                       // STRINGS__COMMAND__GETTIME

    /* configuration (get) */
    "SETUP",                                                                                                         // STRINGS__COMMAND__SETUP

    /* configuration (set) */
    "AGGIUNGI",                                                                                                      // STRINGS__COMMAND__ADD
    "RIMUOVI",                                                                                                       // STRINGS__COMMAND__REMOVE
    "RUBRICA",                                                                                                       // STRINGS__COMMAND__CONTACTS
    "NEWNAME",                                                                                                       // STRINGS__COMMAND__NEWNAME
    "ALLARME",                                                                                                       // STRINGS__COMMAND__ALARM
    "MESSAGGIO",                                                                                                     // STRINGS__COMMAND__MESSAGE
    "CALIBRA",                                                                                                       // STRINGS__COMMAND__CALIBRATE
    "SETPSW",                                                                                                        // STRINGS__COMMAND__SETPSW
    "SETREPORT",                                                                                                     // STRINGS__COMMAND__SETREPORT
    "LINGUA",                                                                                                        // STRINGS__COMMAND__LANGUAGE
    "LANGUAGE",     // in English for all languages                                                                  // STRINGS__COMMAND__LANGUAGE_ENG
    "TIMEROUT",                                                                                                      // STRINGS__COMMAND__TIMEROUT
    "INOLTRA",                                                                                                       // STRINGS__COMMAND__FORWARD
    "CRONO",                                                                                                         // STRINGS__COMMAND__CHRONO
    "ESTATE",                                                                                                        // STRINGS__COMMAND__SUMMER
    "INVERNO",                                                                                                       // STRINGS__COMMAND__WINTER
    "PULSANTI",                                                                                                      // STRINGS__COMMAND__BUTTONS
    "RITARDOSMS",                                                                                                    // STRINGS__COMMAND__SMSDELAY
    "COUNTER",                                                                                                       // STRINGS__COMMAND__COUNTER
    "COUNTERBANDS",                                                                                                  // STRINGS__COMMAND__COUNTERBANDS

    /* telecontrol */
    "ACCENDI",                                                                                                       // STRINGS__COMMAND__TURNON
    "SPEGNI",                                                                                                        // STRINGS__COMMAND__TURNOFF
    "REGOLA",                                                                                                        // STRINGS__COMMAND__REGULATE
    "ANTIGELO",                                                                                                      // STRINGS__COMMAND__ANTIFROST
    "CREDITO",                                                                                                       // STRINGS__COMMAND__CREDIT
    "DEFAULT",                                                                                                       // STRINGS__COMMAND__DEFAULT
    "SETTIME",                                                                                                       // STRINGS__COMMAND__SETTIME
    "SINCRONIZZA",                                                                                                   // STRINGS__COMMAND__SYNCHRONIZE
    "RESET",                                                                                                         // STRINGS__COMMAND__RESET
    "SETCOUNTER",                                                                                                    // STRINGS__COMMAND__SETCOUNTER
    "SETCOUNTERBANDS",                                                                                               // STRINGS__COMMAND__SETCOUNTERBANDS

    /* data acquistion */
    "COMANDI",                                                                                                       // STRINGS__COMMAND__COMMANDS



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
    "TEMP.",                                                                                                         // STRINGS__STATUS__ANS__LABEL__TEMP
    "TIN3",                                                                                                          // STRINGS__STATUS__ANS__LABEL__TINT
    "TIN4",                                                                                                          // STRINGS__STATUS__ANS__LABEL__TEXT
    "OUT1",                                                                                                          // STRINGS__STATUS__ANS__LABEL__OUT1
    "OUT2",                                                                                                          // STRINGS__STATUS__ANS__LABEL__OUT2
    "REGOLA",                                                                                                        // STRINGS__STATUS__ANS__LABEL__REGULATE
    "REG1",                                                                                                          // STRINGS__STATUS__ANS__LABEL__REG1
    "REG2",                                                                                                          // STRINGS__STATUS__ANS__LABEL__REG2
    "ANTIGELO",                                                                                                      // STRINGS__STATUS__ANS__LABEL__ANTIFROST
    "ANT1",                                                                                                          // STRINGS__STATUS__ANS__LABEL__ANT1
    "ANT2",                                                                                                          // STRINGS__STATUS__ANS__LABEL__ANT2
    "CRONO",                                                                                                         // STRINGS__STATUS__ANS__LABEL__CHRONO
    "CR1",                                                                                                           // STRINGS__STATUS__ANS__LABEL__CHR1
    "CR2",                                                                                                           // STRINGS__STATUS__ANS__LABEL__CHR2
    "IN1",                                                                                                           // STRINGS__STATUS__ANS__LABEL__IN1
    "IN2",                                                                                                           // STRINGS__STATUS__ANS__LABEL__IN2
    "IN3",                                                                                                           // STRINGS__STATUS__ANS__LABEL__IN3
    "IN4",                                                                                                           // STRINGS__STATUS__ANS__LABEL__IN4
    "C1",                                                                                                            // STRINGS__STATUS__ANS__LABEL__C1
    "C2",                                                                                                            // STRINGS__STATUS__ANS__LABEL__C2
    "SEGNALE",                                                                                                       // STRINGS__STATUS__ANS__LABEL__SIGNAL
    "GSM",                                                                                                           // STRINGS__STATUS__ANS__LABEL__GSM
    "CREDITO",                                                                                                       // STRINGS__STATUS__ANS__LABEL__CREDIT
    "SMS",                                                                                                           // STRINGS__STATUS__ANS__LABEL__SMS
    "POWER",                                                                                                         // STRINGS__STATUS__ANS__LABEL__POWER
    "ADC_INT",                                                                                                       // STRINGS__STATUS__ANS__LABEL__ADC_INT
    "ADC_EXT",                                                                                                       // STRINGS__STATUS__ANS__LABEL__ADC_EXT

    /* answer  values */
    "ERR",                                                                                                           // STRINGS__STATUS__ANS__VALUE__TEMPERATURE__ERROR
    "ON",                                                                                                            // STRINGS__STATUS__ANS__VALUE__ON
    "OFF",                                                                                                           // STRINGS__STATUS__ANS__VALUE__OFF
    "APERTO",                                                                                                        // STRINGS__STATUS__ANS__VALUE__OPEN
    "CHIUSO",                                                                                                        // STRINGS__STATUS__ANS__VALUE__CLOSE
    "SCARSO",                                                                                                        // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__LOW
    "MEDIO",                                                                                                         // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__MEDIUM
    "BUONO",                                                                                                         // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__GOOD
    "OTTIMO",                                                                                                        // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__EXCELLENT
    "SI",                                                                                                            // STRINGS__STATUS__ANS__VALUE__YES
    "NO",                                                                                                            // STRINGS__STATUS__ANS__VALUE__NO


    /*-----------------------------------------------------------------------------
     * "VERSION"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    "COSTRUTTORE",                                                                                                   // STRINGS__VERSION__ANS__LABEL__MANUFACTURER
    "MODELLO",                                                                                                       // STRINGS__VERSION__ANS__LABEL__MODEL
    "VERSIONE",                                                                                                      // STRINGS__VERSION__ANS__LABEL__VERSION

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
    "CRONO",                                                                                                         // STRINGS__SETUP__CMD__VALUE__CHRONO
    "COUNTERBANDS",                                                                                                  // STRINGS__SETUP__CMD__VALUE__COUNTERBANDS
    "OUT1",                                                                                                          // STRINGS__SETUP__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__SETUP__CMD__VALUE__OUT2
    "C1",                                                                                                            // STRINGS__SETUP__CMD__VALUE__C1
    "C2",                                                                                                            // STRINGS__SETUP__CMD__VALUE__C2

    /* answer labels */
    "TIN3MIN",                                                                                                       // STRINGS__SETUP__ANS__LABEL__TMIN
    "TIN4MIN",                                                                                                       // STRINGS__SETUP__ANS__LABEL__TMINEXT
    "TIN3MAX",                                                                                                       // STRINGS__SETUP__ANS__LABEL__TMAX
    "TIN4MAX",                                                                                                       // STRINGS__SETUP__ANS__LABEL__TMAXEXT
    "C1",                                                                                                            // STRINGS__SETUP__ANS__LABEL__C1
    "C2",                                                                                                            // STRINGS__SETUP__ANS__LABEL__C2
    "IN1",                                                                                                           // STRINGS__SETUP__ANS__LABEL__IN1
    "IN2",                                                                                                           // STRINGS__SETUP__ANS__LABEL__IN2
    "IN3",                                                                                                           // STRINGS__SETUP__ANS__LABEL__IN3
    "IN4",                                                                                                           // STRINGS__SETUP__ANS__LABEL__IN4
    "TOUT1",                                                                                                         // STRINGS__SETUP__ANS__LABEL__TOUT1
    "TOUT2",                                                                                                         // STRINGS__SETUP__ANS__LABEL__TOUT2
    "TIN3",                                                                                                          // STRINGS__SETUP__ANS__LABEL__TINT
    "TIN4",                                                                                                          // STRINGS__SETUP__ANS__LABEL__TEXT
    "POWER",                                                                                                         // STRINGS__SETUP__ANS__LABEL__POWER
    "REPORT",                                                                                                        // STRINGS__SETUP__ANS__LABEL__REPORT
    "INOLTRA",                                                                                                       // STRINGS__SETUP__ANS__LABEL__FORWARD
    "RITARDOSMS",                                                                                                    // STRINGS__SETUP__ANS__LABEL__SMSDELAY
    "PULSANTI",                                                                                                      // STRINGS__SETUP__ANS__LABEL__BUTTONS
    "CRONO",                                                                                                         // STRINGS__SETUP__ANS__LABEL__CHRONO
    "COUNTERBANDS",                                                                                                  // STRINGS__SETUP__ANS__LABEL__COUNTERBANDS
    "OUT1",                                                                                                          // STRINGS__SETUP__ANS__LABEL__OUT1
    "OUT2",                                                                                                          // STRINGS__SETUP__ANS__LABEL__OUT2

    /* answer values */
    "ON",                                                                                                            // STRINGS__SETUP__ANS__VALUE__ON
    "OFF",                                                                                                           // STRINGS__SETUP__ANS__VALUE__OFF
    "O",                                                                                                             // STRINGS__SETUP__ANS__VALUE__O
    "C",                                                                                                             // STRINGS__SETUP__ANS__VALUE__C
    "S",                                                                                                             // STRINGS__SETUP__ANS__VALUE__S
    "M",                                                                                                             // STRINGS__SETUP__ANS__VALUE__M
    "H",                                                                                                             // STRINGS__SETUP__ANS__VALUE__H
    "EST",                                                                                                           // STRINGS__SETUP__ANS__VALUE__SUM
    "INV",                                                                                                           // STRINGS__SETUP__ANS__VALUE__WIN


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
    "Rubrica vuota",                                                                                                 // STRINGS__CONTACTS__ANS__VALUE__PHONEBOOK_EMPTY


    /*-----------------------------------------------------------------------------
     * "NEWNAME" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    "OUT1",                                                                                                          // STRINGS__NEWNAME__CMD__LABEL__OUT1
    "OUT2",                                                                                                          // STRINGS__NEWNAME__CMD__LABEL__OUT2
    "IN1",                                                                                                           // STRINGS__NEWNAME__CMD__LABEL__IN1
    "IN2",                                                                                                           // STRINGS__NEWNAME__CMD__LABEL__IN2
    "IN3",                                                                                                           // STRINGS__NEWNAME__CMD__LABEL__IN3
    "IN4",                                                                                                           // STRINGS__NEWNAME__CMD__LABEL__IN4
    "TIN3MIN",                                                                                                       // STRINGS__NEWNAME__CMD__LABEL__TMIN
    "TIN3MAX",                                                                                                       // STRINGS__NEWNAME__CMD__LABEL__TMAX
    "TIN4MIN",                                                                                                       // STRINGS__NEWNAME__CMD__LABEL__TMINEXT
    "TIN4MAX",                                                                                                       // STRINGS__NEWNAME__CMD__LABEL__TMAXEXT
    "C1",                                                                                                            // STRINGS__NEWNAME__CMD__LABEL__C1
    "C2",                                                                                                            // STRINGS__NEWNAME__CMD__LABEL__C2

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
    "TIN3MIN",                                                                                                       // STRINGS__ALARM__CMD__VALUE__TMIN
    "TIN4MIN",                                                                                                       // STRINGS__ALARM__CMD__VALUE__TMINEXT
    "TIN3MAX",                                                                                                       // STRINGS__ALARM__CMD__VALUE__TMAX
    "TIN4MAX",                                                                                                       // STRINGS__ALARM__CMD__VALUE__TMAXEXT
    "IN1",                                                                                                           // STRINGS__ALARM__CMD__VALUE__IN1
    "IN2",                                                                                                           // STRINGS__ALARM__CMD__VALUE__IN2
    "IN3",                                                                                                           // STRINGS__ALARM__CMD__VALUE__IN3
    "IN4",                                                                                                           // STRINGS__ALARM__CMD__VALUE__IN4
    "POWER",                                                                                                         // STRINGS__ALARM__CMD__VALUE__POWER
    "ATTIVO",                                                                                                        // STRINGS__ALARM__CMD__VALUE__ENABLED
    "DISATTIVO",                                                                                                     // STRINGS__ALARM__CMD__VALUE__DISABLED
    "APERTO",                                                                                                        // STRINGS__ALARM__CMD__VALUE__OPEN
    "CHIUSO",                                                                                                        // STRINGS__ALARM__CMD__VALUE__CLOSE
    "S",                                                                                                             // STRINGS__ALARM__CMD__VALUE__S
    "M",                                                                                                             // STRINGS__ALARM__CMD__VALUE__M
    "H",                                                                                                             // STRINGS__ALARM__CMD__VALUE__H

    /* answer labels */
    "TIN3MIN",                                                                                                       // STRINGS__ALARM__ANS__LABEL__TMIN
    "TIN4MIN",                                                                                                       // STRINGS__ALARM__ANS__LABEL__TMINEXT
    "TIN3MAX",                                                                                                       // STRINGS__ALARM__ANS__LABEL__TMAX
    "TIN4MAX",                                                                                                       // STRINGS__ALARM__ANS__LABEL__TMAXEXT
    "IN1",                                                                                                           // STRINGS__ALARM__ANS__LABEL__IN1
    "IN2",                                                                                                           // STRINGS__ALARM__ANS__LABEL__IN2
    "IN3",                                                                                                           // STRINGS__ALARM__ANS__LABEL__IN3
    "IN4",                                                                                                           // STRINGS__ALARM__ANS__LABEL__IN4
    "POWER",                                                                                                         // STRINGS__ALARM__ANS__LABEL__POWER
    "SETREPORT",                                                                                                     // STRINGS__ALARM__ANS__LABEL__REPORT

    /* answer values */
    "ATTIVO",                                                                                                        // STRINGS__ALARM__ANS__VALUE__ENABLED
    "DISATTIVO",                                                                                                     // STRINGS__ALARM__ANS__VALUE__DISABLED
    "APERTO",                                                                                                        // STRINGS__ALARM__ANS__VALUE__OPEN
    "CHIUSO",                                                                                                        // STRINGS__ALARM__ANS__VALUE__CLOSE
    "S",                                                                                                             // STRINGS__ALARM__ANS__VALUE__S
    "M",                                                                                                             // STRINGS__ALARM__ANS__VALUE__M
    "H",                                                                                                             // STRINGS__ALARM__ANS__VALUE__H


    /*-----------------------------------------------------------------------------
     * "MESSAGE" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    "IN1",                                                                                                           // STRINGS__MESSAGE__CMD__VALUE__IN1
    "IN2",                                                                                                           // STRINGS__MESSAGE__CMD__VALUE__IN2
    "IN3",                                                                                                           // STRINGS__MESSAGE__CMD__VALUE__IN3
    "IN4",                                                                                                           // STRINGS__MESSAGE__CMD__VALUE__IN4
    "TIN3MIN",                                                                                                       // STRINGS__MESSAGE__CMD__VALUE__TMIN
    "TIN4MIN",                                                                                                       // STRINGS__MESSAGE__CMD__VALUE__TMINEXT
    "TIN3MAX",                                                                                                       // STRINGS__MESSAGE__CMD__VALUE__TMAX
    "TIN4MAX",                                                                                                       // STRINGS__MESSAGE__CMD__VALUE__TMAXEXT

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
    "IN3",                                                                                                           // STRINGS__CALIBRATE__CMD__VALUE__INT
    "IN4",                                                                                                           // STRINGS__CALIBRATE__CMD__VALUE__EXT

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
    "OFF",                                                                                                           // STRINGS__SETREPORT__CMD__VALUE__STATUS__OFF
    "ON",                                                                                                            // STRINGS__SETREPORT__CMD__VALUE__STATUS__ON

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
    "ITA",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__ITA
    "ENG",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__ENG
    "FRE",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__FRE
    "GER",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__GER
    "SPA",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__SPA
    "POL",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__POL
    "SWE",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE__SWE

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "LANGUAGE" command    // in English for all languages
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    "ITA",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__ITA
    "ENG",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__ENG
    "FRE",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__FRE
    "GER",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__GER
    "SPA",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__SPA
    "POL",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__POL
    "SWE",                                                                                                           // STRINGS__LANGUAGE__CMD__VALUE__LANGUAGE_ENG__SWE

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
    "OUT1",                                                                                                          // STRINGS__TIMEROUT__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__TIMEROUT__CMD__VALUE__OUT2
    "S",                                                                                                             // STRINGS__TIMEROUT__CMD__VALUE__OUT1__S
    "M",                                                                                                             // STRINGS__TIMEROUT__CMD__VALUE__OUT1__M
    "H",                                                                                                             // STRINGS__TIMEROUT__CMD__VALUE__OUT1__H
    "S",                                                                                                             // STRINGS__TIMEROUT__CMD__VALUE__OUT2__S
    "M",                                                                                                             // STRINGS__TIMEROUT__CMD__VALUE__OUT2__M
    "H",                                                                                                             // STRINGS__TIMEROUT__CMD__VALUE__OUT2__H

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
    "OFF",                                                                                                           // STRINGS__FORWARD__CMD__VALUE__STATUS__OFF
    "ON",                                                                                                            // STRINGS__FORWARD__CMD__VALUE__STATUS__ON

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
    "OUT1",                                                                                                          // STRINGS__CHRONO__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__CHRONO__CMD__VALUE__OUT2
    "OFF",                                                                                                           // STRINGS__CHRONO__CMD__VALUE__STATUS__OFF
    "ON",                                                                                                            // STRINGS__CHRONO__CMD__VALUE__STATUS__ON
    "OFF",                                                                                                           // STRINGS__CHRONO__CMD__VALUE__WINDOW__OFF

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
    "IN3",                                                                                                           // STRINGS__SUMMER__CMD__VALUE__INT
    "IN4",                                                                                                           // STRINGS__SUMMER__CMD__VALUE__EXT

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "WINTER" command
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    "IN3",                                                                                                           // STRINGS__WINTER__CMD__VALUE__INT
    "IN4",                                                                                                           // STRINGS__WINTER__CMD__VALUE__EXT

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
    "OFF",                                                                                                           // STRINGS__BUTTONS__CMD__VALUE__STATUS__OFF
    "ON",                                                                                                            // STRINGS__BUTTONS__CMD__VALUE__STATUS__ON

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
    "OFF",                                                                                                           // STRINGS__SMSDELAY__CMD__VALUE__OFF
    "S",                                                                                                             // STRINGS__SMSDELAY__CMD__VALUE__S
    "M",                                                                                                             // STRINGS__SMSDELAY__CMD__VALUE__M
    "H",                                                                                                             // STRINGS__SMSDELAY__CMD__VALUE__H

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
    "C1",                                                                                                            // STRINGS__COUNTER__CMD__VALUE__C1
    "C2",                                                                                                            // STRINGS__COUNTER__CMD__VALUE__C2
    "OFF",                                                                                                           // STRINGS__COUNTER__CMD__VALUE__STATUS__OFF
    "ON",                                                                                                            // STRINGS__COUNTER__CMD__VALUE__STATUS__ON

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
    "C1",                                                                                                            // STRINGS__COUNTERBANDS__CMD__VALUE__C1
    "C2",                                                                                                            // STRINGS__COUNTERBANDS__CMD__VALUE__C2
    "OFF",                                                                                                           // STRINGS__COUNTERBANDS__CMD__VALUE__STATUS__OFF
    "ON",                                                                                                            // STRINGS__COUNTERBANDS__CMD__VALUE__STATUS__ON
    "OFF",                                                                                                           // STRINGS__COUNTERBANDS__CMD__VALUE__BAND__OFF

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
    "OUT1",                                                                                                          // STRINGS__TURNON__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__TURNON__CMD__VALUE__OUT2

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
    "OUT1",                                                                                                          // STRINGS__TURNOFF__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__TURNOFF__CMD__VALUE__OUT2

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
    "OUT1",                                                                                                          // STRINGS__REGULATE__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__REGULATE__CMD__VALUE__OUT2
    "OFF",                                                                                                           // STRINGS__REGULATE__CMD__VALUE__OFF

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
    "OUT1",                                                                                                          // STRINGS__ANTIFROST__CMD__VALUE__OUT1
    "OUT2",                                                                                                          // STRINGS__ANTIFROST__CMD__VALUE__OUT2
    "OFF",                                                                                                           // STRINGS__ANTIFROST__CMD__VALUE__OFF

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
    "OFF",                                                                                                           // STRINGS__CREDIT__CMD__VALUE__OFF

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
    "RUBRICA",                                                                                                       // STRINGS__DEFAULT__CMD__VALUE__CONTACTS
    "COMANDI",                                                                                                       // STRINGS__DEFAULT__CMD__VALUE__COMMANDS
    "TUTTO",                                                                                                         // STRINGS__DEFAULT__CMD__VALUE__ALL

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "SETTIME"
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
    "C1",                                                                                                            // STRINGS__SETCOUNTER__CMD__VALUE__C1
    "C2",                                                                                                            // STRINGS__SETCOUNTER__CMD__VALUE__C2

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
    "C1",                                                                                                            // STRINGS__SETCOUNTERBANDS__CMD__VALUE__C1
    "C2",                                                                                                            // STRINGS__SETCOUNTERBANDS__CMD__VALUE__C2

    /* answer labels */
    // ---

    /* answer values */
    // ---


    /*-----------------------------------------------------------------------------
     * "COMMANDS"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    // ---

    /* answer values */
    "Nessun comando",                                                                                                // STRINGS__COMMANDS__ANS__VALUE__NO_COMMANDS
};
