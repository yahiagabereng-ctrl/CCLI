/*=============================================================================
 * File       :  STRINGS_ENG.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - text strings - English
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "fw_config.h"
#include "strings_eng.h"
#include "strings_my.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* strings - English */
const ascii *const StringsEng_Strings[] =
{
    "",                                                                                                              // STRINGS__EMPTY

    /*---------------------------------------------------------------------------
     * Alarm strings
     *---------------------------------------------------------------------------*/
    "Alarm IN3 temperature too low",                       // "Allarme temperatura IN3 troppo bassa"                 // STRINGS__ALARM__TMIN
    "Alarm IN4 temperature too low",                       // "Allarme temperatura IN4 troppo bassa"                 // STRINGS__ALARM__TMINEXT
    "IN3 temperature too low restored",                    // "Ripristino temperatura IN3 troppo bassa"              // STRINGS__ALARM__TMIN_RESTORED
    "IN4 temperature too low restored",                    // "Ripristino temperatura IN4 troppo bassa"              // STRINGS__ALARM__TMINEXT_RESTORED

    "Alarm IN3 temperature too high",                      // "Allarme temperatura IN3 troppo alta"                  // STRINGS__ALARM__TMAX
    "Alarm IN4 temperature too high",                      // "Allarme temperatura IN4 troppo alta"                  // STRINGS__ALARM__TMAXEXT
    "IN3 temperature too high restored",                   // "Ripristino temperatura IN3 troppo alta"               // STRINGS__ALARM__TMAX_RESTORED
    "IN4 temperature too high restored",                   // "Ripristino temperatura IN4 troppo alta"               // STRINGS__ALARM__TMAXEXT_RESTORED

    "Alarm IN1 input",                                     // "Allarme ingresso IN1"                                 // STRINGS__ALARM__IN1
    "Alarm IN2 input",                                     // "Allarme ingresso IN2"                                 // STRINGS__ALARM__IN2
    "Alarm IN3 input",                                     // "Allarme ingresso IN3"                                 // STRINGS__ALARM__IN3
    "Alarm IN4 input",                                     // "Allarme ingresso IN4"                                 // STRINGS__ALARM__IN4
    "IN1 input restored",                                  // "Ripristino ingresso IN1"                              // STRINGS__ALARM__IN1_RESTORED
    "IN2 input restored",                                  // "Ripristino ingresso IN2"                              // STRINGS__ALARM__IN2_RESTORED
    "IN3 input restored",                                  // "Ripristino ingresso IN3"                              // STRINGS__ALARM__IN3_RESTORED
    "IN4 input restored",                                  // "Ripristino ingresso IN4"                              // STRINGS__ALARM__IN4_RESTORED

    "Mains power interruption alarm",                      // "Allarme interruzione rete elettrica"                  // STRINGS__ALARM__POWER_OFF
    "Mains power returned",                                // "Rete elettrica ripristinata"                          // STRINGS__ALARM__POWER_RETURN

    "Warning, credit is 10 SMS",                           // "Attenzione, il credito risulta pari a 10 SMS"         // STRINGS__ALARM__CREDIT


    /*---------------------------------------------------------------------------
     * Weekday strings
     *---------------------------------------------------------------------------*/
    "Mon",                                                 // "Lun"                                                  // STRINGS__WEEKDAY__MON
    "Tue",                                                 // "Mar"                                                  // STRINGS__WEEKDAY__TUE
    "Wed",                                                 // "Mer"                                                  // STRINGS__WEEKDAY__WED
    "Thu",                                                 // "Gio"                                                  // STRINGS__WEEKDAY__THU
    "Fri",                                                 // "Ven"                                                  // STRINGS__WEEKDAY__FRI
    "Sat",                                                 // "Sab"                                                  // STRINGS__WEEKDAY__SAT
    "Sun",                                                 // "Dom"                                                  // STRINGS__WEEKDAY__SUN


    /*---------------------------------------------------------------------------
     * FW upgrade result
     *---------------------------------------------------------------------------*/
    /* labels */
    "Firmware upgrade result",                             // "Esito aggiornamento firmware"                         // STRINGS__FW_UPGRADE__LABEL__FIRMWARE_UPGRADE_RESULT
    "Error",                                               // "Errore"                                               // STRINGS__FW_UPGRADE__LABEL__ERROR
    "Actual version",                                      // "Attuale versione"                                     // STRINGS__FW_UPGRADE__LABEL__ACTUAL_VERSION

    /* values */
    "unknown",                                             // "sconosciuto"                                          // STRINGS__FW_UPGRADE__VALUE__RESULT__UNKNOWN
    "failed",                                              // "fallito"                                              // STRINGS__FW_UPGRADE__VALUE__RESULT__FAILED
    "executed",                                            // "eseguito"                                             // STRINGS__FW_UPGRADE__VALUE__RESULT__EXECUTED
    "unknown",                                             // "sconosciuto"                                          // STRINGS__FW_UPGRADE__VALUE__ERROR__UNKNOWN
    "no error",                                            // "no errori"                                            // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_ERROR
    "other error",                                         // "altro errore"                                         // STRINGS__FW_UPGRADE__VALUE__ERROR__OTHER_ERROR
    "no GSM network registration",                         // "no registrazione alla rete GSM"                       // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_GSM_REGISTRATION
    "no GPRS APN connection",                              // "no connessione all'APN GPRS"                          // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_APN_CONNECTION
    "no FTP server connection",                            // "no connessione al server FTP"                         // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_SERVER_CONNECTION
    "no FTP file download",                                // "no download FTP del file"                             // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_FILE_DOWNLOAD
    "no file installation",                                // "no installazione del file"                            // STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FILE_INSTALLATION


    /*---------------------------------------------------------------------------
     * Generic answers to SMS commands
     *---------------------------------------------------------------------------*/
  //"Command executed",                                    // "Comando eseguito"                                     // STRINGS__ANSWER__COMMAND_EXECUTED
  //"Command not executed",                                // "Comando non eseguito"                                 // STRINGS__ANSWER__COMMAND_NOT_EXECUTED
    "Command error",                                       // "Errore comando"                                       // STRINGS__ANSWER__COMMAND_ERROR
  //"Parameters command error",                            // "Errore parametri comando"                             // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR

    /* password */
    "PASSWORD REQUIRED",                                   // "PASSWORD RICHIESTA"                                   // STRINGS__ANSWER__PASSWORD_REQUIRED
    "INCORRECT PASSWORD",                                  // "PASSWORD NON CORRETTA"                                // STRINGS__ANSWER__PASSWORD_INCORRECT

    /* configuration (set) */
    "CONFIGURATION ACCEPTED",                              // "CONFIGURAZIONE ACCETTATA"                             // STRINGS__ANSWER__CONFIGURATION_ACCEPTED
    "CONFIGURATION NOT ACCEPTED",                          // "CONFIGURAZIONE NON ACCETTATA"                         // STRINGS__ANSWER__CONFIGURATION_NOT_ACCEPTED
    "CONFIGURATION SYNTAX ERROR",                          // "ERRORE SINTASSI CONFIGURAZIONE"                       // STRINGS__ANSWER__CONFIGURATION_SYNTAX_ERROR
    "CONFIGURATION PARAMETERS ERROR",                      // "ERRORE PARAMETRI CONFIGURAZIONE"                      // STRINGS__ANSWER__CONFIGURATION_PARAMETERS_ERROR

    /* telecontrol */
    "COMMAND EXECUTED",                                    // "COMANDO ESEGUITO"                                     // STRINGS__ANSWER__COMMAND_EXECUTED
    "COMMAND NOT EXECUTED",                                // "COMANDO NON ESEGUITO"                                 // STRINGS__ANSWER__COMMAND_NOT_EXECUTED
    "COMMAND NOT EXECUTABLE",                              // "COMANDO NON ESEGUIBILE"                               // STRINGS__ANSWER__COMMAND_NOT_EXECUTABLE
    "COMMAND SYNTAX ERROR",                                // "ERRORE SINTASSI COMANDO"                              // STRINGS__ANSWER__COMMAND_SYNTAX_ERROR
    "COMMAND PARAMETERS ERROR",                            // "ERRORE PARAMETRI COMANDO"                             // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR


    /*---------------------------------------------------------------------------
     * Commands
     *---------------------------------------------------------------------------*/
    /* status reading */
    "STATUS",                                              // "STATO"                                                // STRINGS__COMMAND__STATUS
    "VERSION",                                             // "VERSIONE"                                             // STRINGS__COMMAND__VERSION
    "GETTIME",                                             // "GETTIME"                                              // STRINGS__COMMAND__GETTIME

    /* configuration (get) */
    "SETUP",                                               // "SETUP"                                                // STRINGS__COMMAND__SETUP

    /* configuration (set) */
    "ADD",                                                 // "AGGIUNGI"                                             // STRINGS__COMMAND__ADD
    "REMOVE",                                              // "RIMUOVI"                                              // STRINGS__COMMAND__REMOVE
    "CONTACTS",                                            // "RUBRICA"                                              // STRINGS__COMMAND__CONTACTS
    "NEWNAME",                                             // "NEWNAME"                                              // STRINGS__COMMAND__NEWNAME
    "ALARM",                                               // "ALLARME"                                              // STRINGS__COMMAND__ALARM
    "MESSAGE",                                             // "MESSAGGIO"                                            // STRINGS__COMMAND__MESSAGE
    "CALIBRATE",                                           // "CALIBRA"                                              // STRINGS__COMMAND__CALIBRATE
    "SETPSW",                                              // "SETPSW"                                               // STRINGS__COMMAND__SETPSW
    "SETREPORT",                                           // "SETREPORT"                                            // STRINGS__COMMAND__SETREPORT
    "LANGUAGE",                                            // "LINGUA"                                               // STRINGS__COMMAND__LANGUAGE
    "LANGUAGE",                                            // "LANGUAGE"         // in English for all languages     // STRINGS__COMMAND__LANGUAGE_ENG
    "TIMEROUT",                                            // "TIMEROUT"                                             // STRINGS__COMMAND__TIMEROUT
    "FORWARD",                                             // "INOLTRA"                                              // STRINGS__COMMAND__FORWARD
    "CHRONO",                                              // "CRONO"                                                // STRINGS__COMMAND__CHRONO
    "SUMMER",                                              // "ESTATE"                                               // STRINGS__COMMAND__SUMMER
    "WINTER",                                              // "INVERNO"                                              // STRINGS__COMMAND__WINTER
    "BUTTONS",                                             // "PULSANTI"                                             // STRINGS__COMMAND__BUTTONS
    "DELAYSMS",                                            // "RITARDOSMS"                                           // STRINGS__COMMAND__SMSDELAY
    "COUNTER",                                             // "COUNTER"                                              // STRINGS__COMMAND__COUNTER
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__COMMAND__COUNTERBANDS

    /* telecontrol */
    "TURNON",                                              // "ACCENDI"                                              // STRINGS__COMMAND__TURNON
    "TURNOFF",                                             // "SPEGNI"                                               // STRINGS__COMMAND__TURNOFF
    "REGULATE",                                            // "REGOLA"                                               // STRINGS__COMMAND__REGULATE
    "ANTIFROST",                                           // "ANTIGELO"                                             // STRINGS__COMMAND__ANTIFROST
    "CREDIT",                                              // "CREDITO"                                              // STRINGS__COMMAND__CREDIT
    "DEFAULT",                                             // "DEFAULT"                                              // STRINGS__COMMAND__DEFAULT
    "SETTIME",                                             // "SETTIME"                                              // STRINGS__COMMAND__SETTIME
    "SYNCHRONIZE",                                         // "SINCRONIZZA"                                          // STRINGS__COMMAND__SYNCHRONIZE
    "RESET",                                               // "RESET"                                                // STRINGS__COMMAND__RESET
    "SETCOUNTER",                                          // "SETCOUNTER"                                           // STRINGS__COMMAND__SETCOUNTER
    "SETCOUNTERBANDS",                                     // "SETCOUNTERBANDS"                                      // STRINGS__COMMAND__SETCOUNTERBANDS

    /* data acquistion */
    "COMMANDS",                                            // "COMANDI"                                              // STRINGS__COMMAND__COMMANDS



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
    "TEMP.",    /* TEMPERATURE */                          // "TEMP."     /* TEMPERATURA */                          // STRINGS__STATUS__ANS__LABEL__TEMP
    "TIN3",                                                // "TIN3"                                                 // STRINGS__STATUS__ANS__LABEL__TINT
    "TIN4",                                                // "TIN4"                                                 // STRINGS__STATUS__ANS__LABEL__TEXT
    "OUT1",                                                // "OUT1"                                                 // STRINGS__STATUS__ANS__LABEL__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__STATUS__ANS__LABEL__OUT2
    "REGULATE",                                            // "REGOLA"                                               // STRINGS__STATUS__ANS__LABEL__REGULATE
    "REG1",                                                // "REG1"                                                 // STRINGS__STATUS__ANS__LABEL__REG1
    "REG2",                                                // "REG2"                                                 // STRINGS__STATUS__ANS__LABEL__REG2
    "ANTIFROST",                                           // "ANTIGELO"                                             // STRINGS__STATUS__ANS__LABEL__ANTIFROST
    "ANT1",                                                // "ANT1"                                                 // STRINGS__STATUS__ANS__LABEL__ANT1
    "ANT2",                                                // "ANT2"                                                 // STRINGS__STATUS__ANS__LABEL__ANT2
    "CHRONO",                                              // "CRONO"                                                // STRINGS__STATUS__ANS__LABEL__CHRONO
    "CHR1",                                                // "CR1"                                                  // STRINGS__STATUS__ANS__LABEL__CHR1
    "CHR2",                                                // "CR2"                                                  // STRINGS__STATUS__ANS__LABEL__CHR2
    "IN1",                                                 // "IN1"                                                  // STRINGS__STATUS__ANS__LABEL__IN1
    "IN2",                                                 // "IN2"                                                  // STRINGS__STATUS__ANS__LABEL__IN2
    "IN3",                                                 // "IN3"                                                  // STRINGS__STATUS__ANS__LABEL__IN3
    "IN4",                                                 // "IN4"                                                  // STRINGS__STATUS__ANS__LABEL__IN4
    "C1",                                                  // "C1"                                                   // STRINGS__STATUS__ANS__LABEL__C1
    "C2",                                                  // "C2"                                                   // STRINGS__STATUS__ANS__LABEL__C2
    "SIGNAL",   /* GSM SIGNAL */                           // "SEGNALE"   /* SEGNALE GSM */                          // STRINGS__STATUS__ANS__LABEL__SIGNAL
    "GSM",                                                 // "GSM"                                                  // STRINGS__STATUS__ANS__LABEL__GSM
    "CREDIT",                                              // "CREDITO"                                              // STRINGS__STATUS__ANS__LABEL__CREDIT
    "SMS",                                                 // "SMS"                                                  // STRINGS__STATUS__ANS__LABEL__SMS
    "POWER",                                               // "POWER"                                                // STRINGS__STATUS__ANS__LABEL__POWER
    "ADC_INT",                                             // "ADC_INT"                                              // STRINGS__STATUS__ANS__LABEL__ADC_INT
    "ADC_EXT",                                             // "ADC_EXT"                                              // STRINGS__STATUS__ANS__LABEL__ADC_EXT

    /* answer  values */
    "ERR",      /* ERROR */                                // "ERR"       /* ERRORE */                               // STRINGS__STATUS__ANS__VALUE__TEMPERATURE__ERROR
    "ON",                                                  // "ON"                                                   // STRINGS__STATUS__ANS__VALUE__ON
    "OFF",                                                 // "OFF"                                                  // STRINGS__STATUS__ANS__VALUE__OFF
    "OPEN",                                                // "APERTO"                                               // STRINGS__STATUS__ANS__VALUE__OPEN
    "CLOSE",                                               // "CHIUSO"                                               // STRINGS__STATUS__ANS__VALUE__CLOSE
    "POOR",     /* GSM SIGNAL */                           // "SCARSO"    /* SEGNALE GSM */                          // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__LOW
    "FAIR",                                                // "MEDIO"                                                // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__MEDIUM
    "GOOD",                                                // "BUONO"                                                // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__GOOD
    "EXCELLENT",                                           // "OTTIMO"                                               // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__EXCELLENT
    "YES",                                                 // "SI"                                                   // STRINGS__STATUS__ANS__VALUE__YES
    "NO",                                                  // "NO"                                                   // STRINGS__STATUS__ANS__VALUE__NO


    /*-----------------------------------------------------------------------------
     * "VERSION"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    "MANUFACTURER",                                        // "COSTRUTTORE"                                          // STRINGS__VERSION__ANS__LABEL__MANUFACTURER
    "MODEL",                                               // "MODELLO"                                              // STRINGS__VERSION__ANS__LABEL__MODEL
    "VERSION",                                             // "VERSIONE"                                             // STRINGS__VERSION__ANS__LABEL__VERSION

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
    "CHRONO",                                              // "CRONO"                                                // STRINGS__SETUP__CMD__VALUE__CHRONO
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__SETUP__CMD__VALUE__COUNTERBANDS
    "OUT1",                                                // "OUT1"                                                 // STRINGS__SETUP__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__SETUP__CMD__VALUE__OUT2
    "C1",                                                  // "C1"                                                   // STRINGS__SETUP__CMD__VALUE__C1
    "C2",                                                  // "C2"                                                   // STRINGS__SETUP__CMD__VALUE__C2

    /* answer labels */
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__SETUP__ANS__LABEL__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__SETUP__ANS__LABEL__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__SETUP__ANS__LABEL__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__SETUP__ANS__LABEL__TMAXEXT
    "C1",                                                  // "C1"                                                   // STRINGS__SETUP__ANS__LABEL__C1
    "C2",                                                  // "C2"                                                   // STRINGS__SETUP__ANS__LABEL__C2
    "IN1",                                                 // "IN1"                                                  // STRINGS__SETUP__ANS__LABEL__IN1
    "IN2",                                                 // "IN2"                                                  // STRINGS__SETUP__ANS__LABEL__IN2
    "IN3",                                                 // "IN3"                                                  // STRINGS__SETUP__ANS__LABEL__IN3
    "IN4",                                                 // "IN4"                                                  // STRINGS__SETUP__ANS__LABEL__IN4
    "TOUT1",                                               // "TOUT1"                                                // STRINGS__SETUP__ANS__LABEL__TOUT1
    "TOUT2",                                               // "TOUT2"                                                // STRINGS__SETUP__ANS__LABEL__TOUT2
    "TIN3",                                                // "TIN3"                                                 // STRINGS__SETUP__ANS__LABEL__TINT
    "TIN4",                                                // "TIN4"                                                 // STRINGS__SETUP__ANS__LABEL__TEXT
    "POWER",                                               // "POWER"                                                // STRINGS__SETUP__ANS__LABEL__POWER
    "REPORT",                                              // "REPORT"                                               // STRINGS__SETUP__ANS__LABEL__REPORT
    "FORWARD",                                             // "INOLTRA"                                              // STRINGS__SETUP__ANS__LABEL__FORWARD
    "DELAYSMS",                                            // "RITARDOSMS"                                           // STRINGS__SETUP__ANS__LABEL__SMSDELAY
    "BUTTONS",                                             // "PULSANTI"                                             // STRINGS__SETUP__ANS__LABEL__BUTTONS
    "CHRONO",                                              // "CRONO"                                                // STRINGS__SETUP__ANS__LABEL__CHRONO
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__SETUP__ANS__LABEL__COUNTERBANDS
    "OUT1",                                                // "OUT1"                                                 // STRINGS__SETUP__ANS__LABEL__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__SETUP__ANS__LABEL__OUT2

    /* answer values */
    "ON",                                                  // "ON"                                                   // STRINGS__SETUP__ANS__VALUE__ON
    "OFF",                                                 // "OFF"                                                  // STRINGS__SETUP__ANS__VALUE__OFF
    "O",                                                   // "O"                                                    // STRINGS__SETUP__ANS__VALUE__O
    "C",                                                   // "C"                                                    // STRINGS__SETUP__ANS__VALUE__C
    "S",        /* SECONDS */                              // "S"                                                    // STRINGS__SETUP__ANS__VALUE__S
    "M",        /* MINUTES */                              // "M"                                                    // STRINGS__SETUP__ANS__VALUE__M
    "H",        /* HOURS   */                              // "H"                                                    // STRINGS__SETUP__ANS__VALUE__H
    "SUM",                                                 // "EST"                                                  // STRINGS__SETUP__ANS__VALUE__SUM
    "WIN",                                                 // "INV"                                                  // STRINGS__SETUP__ANS__VALUE__WIN


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
    "Empty phonebook",                                     // "Rubrica vuota"                                        // STRINGS__CONTACTS__ANS__VALUE__PHONEBOOK_EMPTY


    /*----------------------------------------------------------------------------------------------
     * "NEWNAME" command
     *----------------------------------------------------------------------------------------------*/
    /* command labels */
    "OUT1",                                                // "OUT1"                                                 // STRINGS__NEWNAME__CMD__LABEL__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__NEWNAME__CMD__LABEL__OUT2
    "IN1",                                                 // "IN1"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN1
    "IN2",                                                 // "IN2"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN2
    "IN3",                                                 // "IN3"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN3
    "IN4",                                                 // "IN4"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN4
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__NEWNAME__CMD__LABEL__TMIN
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__NEWNAME__CMD__LABEL__TMAX
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__NEWNAME__CMD__LABEL__TMINEXT
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__NEWNAME__CMD__LABEL__TMAXEXT
    "C1",                                                  // "C1"                                                   // STRINGS__NEWNAME__CMD__LABEL__C1
    "C2",                                                  // "C2"                                                   // STRINGS__NEWNAME__CMD__LABEL__C2

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
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__ALARM__CMD__VALUE__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__ALARM__CMD__VALUE__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__ALARM__CMD__VALUE__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__ALARM__CMD__VALUE__TMAXEXT
    "IN1",                                                 // "IN1"                                                  // STRINGS__ALARM__CMD__VALUE__IN1
    "IN2",                                                 // "IN2"                                                  // STRINGS__ALARM__CMD__VALUE__IN2
    "IN3",                                                 // "IN3"                                                  // STRINGS__ALARM__CMD__VALUE__IN3
    "IN4",                                                 // "IN4"                                                  // STRINGS__ALARM__CMD__VALUE__IN4
    "POWER",                                               // "POWER"                                                // STRINGS__ALARM__CMD__VALUE__POWER
    "ENABLED",                                             // "ATTIVO"                                               // STRINGS__ALARM__CMD__VALUE__ENABLED
    "DISABLED",                                            // "DISATTIVO"                                            // STRINGS__ALARM__CMD__VALUE__DISABLED
    "OPEN",                                                // "APERTO"                                               // STRINGS__ALARM__CMD__VALUE__OPEN
    "CLOSE",                                               // "CHIUSO"                                               // STRINGS__ALARM__CMD__VALUE__CLOSE
    "S",        /* SECONDS */                              // "S"         /* SECONDI */                              // STRINGS__ALARM__CMD__VALUE__S
    "M",        /* MINUTES */                              // "M"         /* MINUTI  */                              // STRINGS__ALARM__CMD__VALUE__M
    "H",        /* HOURS   */                              // "H"         /* ORE     */                              // STRINGS__ALARM__CMD__VALUE__H

    /* answer labels */
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__ALARM__ANS__LABEL__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__ALARM__ANS__LABEL__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__ALARM__ANS__LABEL__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__ALARM__ANS__LABEL__TMAXEXT
    "IN1",                                                 // "IN1"                                                  // STRINGS__ALARM__ANS__LABEL__IN1
    "IN2",                                                 // "IN2"                                                  // STRINGS__ALARM__ANS__LABEL__IN2
    "IN3",                                                 // "IN3"                                                  // STRINGS__ALARM__ANS__LABEL__IN3
    "IN4",                                                 // "IN4"                                                  // STRINGS__ALARM__ANS__LABEL__IN4
    "POWER",                                               // "POWER"                                                // STRINGS__ALARM__ANS__LABEL__POWER
    "SETREPORT",                                           // "REPORT"                                               // STRINGS__ALARM__ANS__LABEL__REPORT

    /* answer values */
    "ENABLED",                                             // "ATTIVO"                                               // STRINGS__ALARM__ANS__VALUE__ENABLED
    "DISABLED",                                            // "DISATTIVO"                                            // STRINGS__ALARM__ANS__VALUE__DISABLED
    "OPEN",                                                // "APERTO"                                               // STRINGS__ALARM__ANS__VALUE__OPEN
    "CLOSE",                                               // "CHIUSO"                                               // STRINGS__ALARM__ANS__VALUE__CLOSE
    "S",        /* SECONDS */                              // "S"         /* SECONDI */                              // STRINGS__ALARM__ANS__VALUE__S
    "M",        /* MINUTES */                              // "M"         /* MINUTI  */                              // STRINGS__ALARM__ANS__VALUE__M
    "H",        /* HOURS   */                              // "H"         /* ORE     */                              // STRINGS__ALARM__ANS__VALUE__H


    /*--------------------------------------------------------------------------------------
     * "MESSAGE" command
     *--------------------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    "IN1",                                                 // "IN1"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN1
    "IN2",                                                 // "IN2"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN2
    "IN3",                                                 // "IN3"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN3
    "IN4",                                                 // "IN4"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN4
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__MESSAGE__CMD__VALUE__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__MESSAGE__CMD__VALUE__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__MESSAGE__CMD__VALUE__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__MESSAGE__CMD__VALUE__TMAXEXT

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
    "IN3",                                                 // "IN3"                                                  // STRINGS__CALIBRATE__CMD__VALUE__INT
    "IN4",                                                 // "IN4"                                                  // STRINGS__CALIBRATE__CMD__VALUE__EXT

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
    "OFF",                                                 // "OFF"                                                  // STRINGS__SETREPORT__CMD__VALUE__STATUS__OFF
    "ON",                                                  // "ON"                                                   // STRINGS__SETREPORT__CMD__VALUE__STATUS__ON

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
    "OUT1",                                                // "OUT1"                                                 // STRINGS__TIMEROUT__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__TIMEROUT__CMD__VALUE__OUT2
    "S",        /* SECONDS */                              // "S"         /* SECONDI */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__S
    "M",        /* MINUTES */                              // "M"         /* MINUTI  */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__M
    "H",        /* HOURS   */                              // "H"         /* ORE     */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__H
    "S",        /* SECONDS */                              // "S"         /* SECONDI */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__S
    "M",        /* MINUTES */                              // "M"         /* MINUTI  */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__M
    "H",        /* HOURS   */                              // "H"         /* ORE     */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__H

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
    "OFF",                                                 // "OFF"                                                  // STRINGS__FORWARD__CMD__VALUE__STATUS__OFF
    "ON",                                                  // "ON"                                                   // STRINGS__FORWARD__CMD__VALUE__STATUS__ON

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
    "OUT1",                                                // "OUT1"                                                 // STRINGS__CHRONO__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__CHRONO__CMD__VALUE__OUT2
    "OFF",                                                 // "OFF"                                                  // STRINGS__CHRONO__CMD__VALUE__STATUS__OFF
    "ON",                                                  // "ON"                                                   // STRINGS__CHRONO__CMD__VALUE__STATUS__ON
    "OFF",                                                 // "OFF"                                                  // STRINGS__CHRONO__CMD__VALUE__WINDOW__OFF

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
    "IN3",                                                 // "IN3"                                                  // STRINGS__SUMMER__CMD__VALUE__INT
    "IN4",                                                 // "IN4"                                                  // STRINGS__SUMMER__CMD__VALUE__EXT

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
    "IN3",                                                 // "IN3"                                                  // STRINGS__WINTER__CMD__VALUE__INT
    "IN4",                                                 // "IN4"                                                  // STRINGS__WINTER__CMD__VALUE__EXT

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
    "OFF",                                                 // "OFF"                                                  // STRINGS__BUTTONS__CMD__VALUE__STATUS__OFF
    "ON",                                                  // "ON"                                                   // STRINGS__BUTTONS__CMD__VALUE__STATUS__ON

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
    "OFF",                                                 // "OFF"                                                  // STRINGS__SMSDELAY__CMD__VALUE__OFF
    "S",        /* SECONDS */                              // "S"                                                    // STRINGS__SMSDELAY__CMD__VALUE__S
    "M",        /* MINUTES */                              // "M"                                                    // STRINGS__SMSDELAY__CMD__VALUE__M
    "H",        /* HOURS   */                              // "H"                                                    // STRINGS__SMSDELAY__CMD__VALUE__H

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
    "C1",                                                  // "C1"                                                   // STRINGS__COUNTER__CMD__VALUE__C1
    "C2",                                                  // "C2"                                                   // STRINGS__COUNTER__CMD__VALUE__C2
    "OFF",                                                 // "OFF"                                                  // STRINGS__COUNTER__CMD__VALUE__STATUS__OFF
    "ON",                                                  // "ON"                                                   // STRINGS__COUNTER__CMD__VALUE__STATUS__ON

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
    "C1",                                                  // "C1"                                                   // STRINGS__COUNTERBANDS__CMD__VALUE__C1
    "C2",                                                  // "C2"                                                   // STRINGS__COUNTERBANDS__CMD__VALUE__C2
    "OFF",                                                 // "OFF"                                                  // STRINGS__COUNTERBANDS__CMD__VALUE__STATUS__OFF
    "ON",                                                  // "ON"                                                   // STRINGS__COUNTERBANDS__CMD__VALUE__STATUS__ON
    "OFF",                                                 // "OFF"                                                  // STRINGS__COUNTERBANDS__CMD__VALUE__BAND__OFF

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
    "OUT1",                                                // "OUT1"                                                 // STRINGS__TURNON__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__TURNON__CMD__VALUE__OUT2

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
    "OUT1",                                                // "OUT1"                                                 // STRINGS__TURNOFF__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__TURNOFF__CMD__VALUE__OUT2

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
    "OUT1",                                                // "OUT1"                                                 // STRINGS__REGULATE__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__REGULATE__CMD__VALUE__OUT2
    "OFF",                                                 // "OFF"                                                  // STRINGS__REGULATE__CMD__VALUE__OFF

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
    "OUT1",                                                // "OUT1"                                                 // STRINGS__ANTIFROST__CMD__VALUE__OUT1
    "OUT2",                                                // "OUT2"                                                 // STRINGS__ANTIFROST__CMD__VALUE__OUT2
    "OFF",                                                 // "OFF"                                                  // STRINGS__ANTIFROST__CMD__VALUE__OFF

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
    "OFF",                                                 // "OFF"                                                  // STRINGS__CREDIT__CMD__VALUE__OFF

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
    "CONTACTS",                                            // "RUBRICA"                                              // STRINGS__DEFAULT__CMD__VALUE__CONTACTS
    "COMMANDS",                                            // "COMANDI"                                              // STRINGS__DEFAULT__CMD__VALUE__COMMANDS
    "ALL",                                                 // "TUTTO"                                                // STRINGS__DEFAULT__CMD__VALUE__ALL

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
    "C1",                                                  // "C1"                                                   // STRINGS__SETCOUNTER__CMD__VALUE__C1
    "C2",                                                  // "C2"                                                   // STRINGS__SETCOUNTER__CMD__VALUE__C2

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
    "C1",                                                  // "C1"                                                   // STRINGS__SETCOUNTERBANDS__CMD__VALUE__C1
    "C2",                                                  // "C2"                                                   // STRINGS__SETCOUNTERBANDS__CMD__VALUE__C2

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
    "No commands",                                         // "Nessun comando"                                       // STRINGS__COMMANDS__ANS__VALUE__NO_COMMANDS
};
