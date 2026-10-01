/*=============================================================================
 * File       :  STRINGS_GER.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - text strings - German
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "fw_config.h"
#include "strings_ger.h"
#include "strings_my.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* strings - German */
const ascii *const StringsGer_Strings[] =
{
    "",                                                                                                              // STRINGS__EMPTY

    /*---------------------------------------------------------------------------
     * Alarm strings
     *---------------------------------------------------------------------------*/
    "Alarm temperatur EING3 zu niedrig",                   // "Allarme temperatura IN3 troppo bassa"                 // STRINGS__ALARM__TMIN
    "Alarm temperatur EING4 zu niedrig",                   // "Allarme temperatura IN4 troppo bassa"                 // STRINGS__ALARM__TMINEXT
    "Wiederherstellen temperatur EING3 zu niedrig",        // "Ripristino temperatura IN3 troppo bassa"              // STRINGS__ALARM__TMIN_RESTORED
    "Wiederherstellen temperatur EING4 zu niedrig",        // "Ripristino temperatura IN4 troppo bassa"              // STRINGS__ALARM__TMINEXT_RESTORED

    "Alarm temperatur EING3 zu hoch",                      // "Allarme temperatura IN3 troppo alta"                  // STRINGS__ALARM__TMAX
    "Alarm temperatur EING4 zu hoch",                      // "Allarme temperatura IN4 troppo alta"                  // STRINGS__ALARM__TMAXEXT
    "Wiederherstellen temperatur EING3 zu hoch",           // "Ripristino temperatura IN3 troppo alta"               // STRINGS__ALARM__TMAX_RESTORED
    "Wiederherstellen temperatur EING4 zu hoch",           // "Ripristino temperatura IN4 troppo alta"               // STRINGS__ALARM__TMAXEXT_RESTORED

    "Alarm EING1",                                         // "Allarme ingresso IN1"                                 // STRINGS__ALARM__IN1
    "Alarm EING2",                                         // "Allarme ingresso IN2"                                 // STRINGS__ALARM__IN2
    "Alarm EING3",                                         // "Allarme ingresso IN3"                                 // STRINGS__ALARM__IN3
    "Alarm EING4",                                         // "Allarme ingresso IN4"                                 // STRINGS__ALARM__IN4
    "Wiederherstellen EING1",                              // "Ripristino ingresso IN1"                              // STRINGS__ALARM__IN1_RESTORED
    "Wiederherstellen EING2",                              // "Ripristino ingresso IN2"                              // STRINGS__ALARM__IN2_RESTORED
    "Wiederherstellen EING3",                              // "Ripristino ingresso IN3"                              // STRINGS__ALARM__IN3_RESTORED
    "Wiederherstellen EING4",                              // "Ripristino ingresso IN4"                              // STRINGS__ALARM__IN4_RESTORED

    "Alarm Unterbruch Betriebsspannung",                   // "Allarme interruzione rete elettrica"                  // STRINGS__ALARM__POWER_OFF
    "Betriebsspannung zruck",                              // "Rete elettrica ripristinata"                          // STRINGS__ALARM__POWER_RETURN

    "Warnung, nur noch 10 SMS",                            // "Attenzione, il credito risulta pari a 10 SMS"         // STRINGS__ALARM__CREDIT


    /*---------------------------------------------------------------------------
     * Weekday strings
     *---------------------------------------------------------------------------*/
    "Mon",                                                 // "Lun"                                                  // STRINGS__WEEKDAY__MON
    "Die",                                                 // "Mar"                                                  // STRINGS__WEEKDAY__TUE
    "Mit",                                                 // "Mer"                                                  // STRINGS__WEEKDAY__WED
    "Don",                                                 // "Gio"                                                  // STRINGS__WEEKDAY__THU
    "Fre",                                                 // "Ven"                                                  // STRINGS__WEEKDAY__FRI
    "Sam",                                                 // "Sab"                                                  // STRINGS__WEEKDAY__SAT
    "Son",                                                 // "Dom"                                                  // STRINGS__WEEKDAY__SUN


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
    "Falscher Befehl",                                     // "Errore comando"                                       // STRINGS__ANSWER__COMMAND_ERROR
  //"Parameters command error",                            // "Errore parametri comando"                             // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR

    /* password */
    "PASSWORT NOTWENDIG",                                  // "PASSWORD RICHIESTA"                                   // STRINGS__ANSWER__PASSWORD_REQUIRED
    "Falsches PASSWORT",                                   // "PASSWORD NON CORRETTA"                                // STRINGS__ANSWER__PASSWORD_INCORRECT

    /* configuration (set) */
    "KONFIGURATION OK",                                    // "CONFIGURAZIONE ACCETTATA"                             // STRINGS__ANSWER__CONFIGURATION_ACCEPTED
    "KONFIGURATION NICHT OK",                              // "CONFIGURAZIONE NON ACCETTATA"                         // STRINGS__ANSWER__CONFIGURATION_NOT_ACCEPTED
    "KONFIGURATION SYNTAX FEHLER",                         // "ERRORE SINTASSI CONFIGURAZIONE"                       // STRINGS__ANSWER__CONFIGURATION_SYNTAX_ERROR
    "KONFIGURATION PARAMETER FALSCH",                      // "ERRORE PARAMETRI CONFIGURAZIONE"                      // STRINGS__ANSWER__CONFIGURATION_PARAMETERS_ERROR

    /* telecontrol */
    "BEFEHL AUSGEFUHRT",                                   // "COMANDO ESEGUITO"                                     // STRINGS__ANSWER__COMMAND_EXECUTED
    "BEFEHL NICHT AUSGEFUHRT",                             // "COMANDO NON ESEGUITO"                                 // STRINGS__ANSWER__COMMAND_NOT_EXECUTED
    "BEFEHL NICHT AUSFUHRBAR",                             // "COMANDO NON ESEGUIBILE"                               // STRINGS__ANSWER__COMMAND_NOT_EXECUTABLE
    "FEHLER BEFEHLSSTRUKTUR",                              // "ERRORE SINTASSI COMANDO"                              // STRINGS__ANSWER__COMMAND_SYNTAX_ERROR
    "BEFEHL FALSCHER PARAMETER",                           // "ERRORE PARAMETRI COMANDO"                             // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR


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
    "HINZU",                                               // "AGGIUNGI"                                             // STRINGS__COMMAND__ADD
    "ENFERNEN",                                            // "RIMUOVI"                                              // STRINGS__COMMAND__REMOVE
    "KONTAKT",                                             // "RUBRICA"                                              // STRINGS__COMMAND__CONTACTS
    "NEUEREINTRAG",                                        // "NEWNAME"                                              // STRINGS__COMMAND__NEWNAME
    "ALARM",                                               // "ALLARME"                                              // STRINGS__COMMAND__ALARM
    "MELDUNG",                                             // "MESSAGGIO"                                            // STRINGS__COMMAND__MESSAGE
    "JUSTAGE",                                             // "CALIBRA"                                              // STRINGS__COMMAND__CALIBRATE
    "SETPW",                                               // "SETPSW"                                               // STRINGS__COMMAND__SETPSW
    "SETREPORT",                                           // "SETREPORT"                                            // STRINGS__COMMAND__SETREPORT
    "SPRACHE",                                             // "LINGUA"                                               // STRINGS__COMMAND__LANGUAGE
    "LANGUAGE",                                            // "LANGUAGE"         // in English for all languages     // STRINGS__COMMAND__LANGUAGE_ENG
    "TIMEROUT",                                            // "TIMEROUT"                                             // STRINGS__COMMAND__TIMEROUT
    "WEITERLEITEN",                                        // "INOLTRA"                                              // STRINGS__COMMAND__FORWARD
    "CHRONO",                                              // "CRONO"                                                // STRINGS__COMMAND__CHRONO
    "SOMMER",                                              // "ESTATE"                                               // STRINGS__COMMAND__SUMMER
    "WINTER",                                              // "INVERNO"                                              // STRINGS__COMMAND__WINTER
    "TASTEN",                                              // "PULSANTI"                                             // STRINGS__COMMAND__BUTTONS
    "VERZOGERUNGSMS",                                      // "RITARDOSMS"                                           // STRINGS__COMMAND__SMSDELAY
    "COUNTER",                                             // "COUNTER"                                              // STRINGS__COMMAND__COUNTER
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__COMMAND__COUNTERBANDS

    /* telecontrol */
    "EIN",                                                 // "ACCENDI"                                              // STRINGS__COMMAND__TURNON
    "AUS",                                                 // "SPEGNI"                                               // STRINGS__COMMAND__TURNOFF
    "REGELN",                                              // "REGOLA"                                               // STRINGS__COMMAND__REGULATE
    "ANTIFROST",                                           // "ANTIGELO"                                             // STRINGS__COMMAND__ANTIFROST
    "KREDIT",                                              // "CREDITO"                                              // STRINGS__COMMAND__CREDIT
    "WERKEIN",                                             // "DEFAULT"                                              // STRINGS__COMMAND__DEFAULT
    "SETTIME",                                             // "SETTIME"                                              // STRINGS__COMMAND__SETTIME
    "SYNCHRONISIEREN",                                     // "SINCRONIZZA"                                          // STRINGS__COMMAND__SYNCHRONIZE
    "RESET",                                               // "RESET"                                                // STRINGS__COMMAND__RESET
    "SETCOUNTER",                                          // "SETCOUNTER"                                           // STRINGS__COMMAND__SETCOUNTER
    "SETCOUNTERBANDS",                                     // "SETCOUNTERBANDS"                                      // STRINGS__COMMAND__SETCOUNTERBANDS

    /* data acquistion */
    "BEFEHLE",                                             // "COMANDI"                                              // STRINGS__COMMAND__COMMANDS



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
    "TEMP.",    /* TEMPERATUR */                           // "TEMP."     /* TEMPERATURA */                          // STRINGS__STATUS__ANS__LABEL__TEMP
    "TIN3",                                                // "TIN3"                                                 // STRINGS__STATUS__ANS__LABEL__TINT
    "TIN4",                                                // "TIN4"                                                 // STRINGS__STATUS__ANS__LABEL__TEXT
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__STATUS__ANS__LABEL__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__STATUS__ANS__LABEL__OUT2
    "REGELN",                                              // "REGOLA"                                               // STRINGS__STATUS__ANS__LABEL__REGULATE
    "REG1",                                                // "REG1"                                                 // STRINGS__STATUS__ANS__LABEL__REG1
    "REG2",                                                // "REG2"                                                 // STRINGS__STATUS__ANS__LABEL__REG2
    "ANTIFROST",                                           // "ANTIGELO"                                             // STRINGS__STATUS__ANS__LABEL__ANTIFROST
    "ANT1",                                                // "ANT1"                                                 // STRINGS__STATUS__ANS__LABEL__ANT1
    "ANT2",                                                // "ANT2"                                                 // STRINGS__STATUS__ANS__LABEL__ANT2
    "CHRONO",                                              // "CRONO"                                                // STRINGS__STATUS__ANS__LABEL__CHRONO
    "CHR1",                                                // "CR1"                                                  // STRINGS__STATUS__ANS__LABEL__CHR1
    "CHR2",                                                // "CR2"                                                  // STRINGS__STATUS__ANS__LABEL__CHR2
    "EING1",                                               // "IN1"                                                  // STRINGS__STATUS__ANS__LABEL__IN1
    "EING2",                                               // "IN2"                                                  // STRINGS__STATUS__ANS__LABEL__IN2
    "EING3",                                               // "IN3"                                                  // STRINGS__STATUS__ANS__LABEL__IN3
    "EING4",                                               // "IN4"                                                  // STRINGS__STATUS__ANS__LABEL__IN4
    "C1",                                                  // "C1"                                                   // STRINGS__STATUS__ANS__LABEL__C1
    "C2",                                                  // "C2"                                                   // STRINGS__STATUS__ANS__LABEL__C2
    "SIGNAL",   /* GSM SIGNAL */                           // "SEGNALE"   /* SEGNALE GSM */                          // STRINGS__STATUS__ANS__LABEL__SIGNAL
    "GSM",                                                 // "GSM"                                                  // STRINGS__STATUS__ANS__LABEL__GSM
    "KREDIT",                                              // "CREDITO"                                              // STRINGS__STATUS__ANS__LABEL__CREDIT
    "SMS",                                                 // "SMS"                                                  // STRINGS__STATUS__ANS__LABEL__SMS
    "POWER",                                               // "POWER"                                                // STRINGS__STATUS__ANS__LABEL__POWER
    "ADC_INT",                                             // "ADC_INT"                                              // STRINGS__STATUS__ANS__LABEL__ADC_INT
    "ADC_EXT",                                             // "ADC_EXT"                                              // STRINGS__STATUS__ANS__LABEL__ADC_EXT

    /* answer  values */
    "FEHL",     /* ERROR */                                // "ERR"       /* ERRORE */                               // STRINGS__STATUS__ANS__VALUE__TEMPERATURE__ERROR
    "EIN",                                                 // "ON"                                                   // STRINGS__STATUS__ANS__VALUE__ON
    "AUS",                                                 // "OFF"                                                  // STRINGS__STATUS__ANS__VALUE__OFF
    "OFFEN",                                               // "APERTO"                                               // STRINGS__STATUS__ANS__VALUE__OPEN
    "GESCHLOSSEN",                                         // "CHIUSO"                                               // STRINGS__STATUS__ANS__VALUE__CLOSE
    "SCHLECHT", /* GSM SIGNAL */                           // "SCARSO"    /* SEGNALE GSM */                          // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__LOW
    "OK",                                                  // "MEDIO"                                                // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__MEDIUM
    "GUT",                                                 // "BUONO"                                                // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__GOOD
    "SEHR GUT",                                            // "OTTIMO"                                               // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__EXCELLENT
    "JA",                                                  // "SI"                                                   // STRINGS__STATUS__ANS__VALUE__YES
    "NEIN",                                                // "NO"                                                   // STRINGS__STATUS__ANS__VALUE__NO


    /*-----------------------------------------------------------------------------
     * "VERSION"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    "HERSTELLER",                                          // "COSTRUTTORE"                                          // STRINGS__VERSION__ANS__LABEL__MANUFACTURER
    "TYP",                                                 // "MODELLO"                                              // STRINGS__VERSION__ANS__LABEL__MODEL
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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__SETUP__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__SETUP__CMD__VALUE__OUT2
    "C1",                                                  // "C1"                                                   // STRINGS__SETUP__CMD__VALUE__C1
    "C2",                                                  // "C2"                                                   // STRINGS__SETUP__CMD__VALUE__C2

    /* answer labels */
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__SETUP__ANS__LABEL__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__SETUP__ANS__LABEL__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__SETUP__ANS__LABEL__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__SETUP__ANS__LABEL__TMAXEXT
    "C1",                                                  // "C1"                                                   // STRINGS__SETUP__ANS__LABEL__C1
    "C2",                                                  // "C2"                                                   // STRINGS__SETUP__ANS__LABEL__C2
    "EING1",                                               // "IN1"                                                  // STRINGS__SETUP__ANS__LABEL__IN1
    "EING2",                                               // "IN2"                                                  // STRINGS__SETUP__ANS__LABEL__IN2
    "EING3",                                               // "IN3"                                                  // STRINGS__SETUP__ANS__LABEL__IN3
    "EING4",                                               // "IN4"                                                  // STRINGS__SETUP__ANS__LABEL__IN4
    "TAUSG1",                                              // "TOUT1"                                                // STRINGS__SETUP__ANS__LABEL__TOUT1
    "TAUSG2",                                              // "TOUT2"                                                // STRINGS__SETUP__ANS__LABEL__TOUT2
    "TIN3",                                                // "TIN3"                                                 // STRINGS__SETUP__ANS__LABEL__TINT
    "TIN4",                                                // "TIN4"                                                 // STRINGS__SETUP__ANS__LABEL__TEXT
    "POWER",                                               // "POWER"                                                // STRINGS__SETUP__ANS__LABEL__POWER
    "REPORT",                                              // "REPORT"                                               // STRINGS__SETUP__ANS__LABEL__REPORT
    "WEITERLEITEN",                                        // "INOLTRA"                                              // STRINGS__SETUP__ANS__LABEL__FORWARD
    "VERZOGERUNGSMS",                                      // "RITARDOSMS"                                           // STRINGS__SETUP__ANS__LABEL__SMSDELAY
    "TASTEN",                                              // "PULSANTI"                                             // STRINGS__SETUP__ANS__LABEL__BUTTONS
    "CHRONO",                                              // "CRONO"                                                // STRINGS__SETUP__ANS__LABEL__CHRONO
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__SETUP__ANS__LABEL__COUNTERBANDS
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__SETUP__ANS__LABEL__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__SETUP__ANS__LABEL__OUT2

    /* answer values */
    "EIN",                                                 // "ON"                                                   // STRINGS__SETUP__ANS__VALUE__ON
    "AUS",                                                 // "OFF"                                                  // STRINGS__SETUP__ANS__VALUE__OFF
    "O",                                                   // "O"                                                    // STRINGS__SETUP__ANS__VALUE__O
    "C",                                                   // "C"                                                    // STRINGS__SETUP__ANS__VALUE__C
    "S",        /* SEKUNDEN */                             // "S"         /* SECONDI */                              // STRINGS__SETUP__ANS__VALUE__S
    "M",        /* MINUTEN  */                             // "M"         /* MINUTI  */                              // STRINGS__SETUP__ANS__VALUE__M
    "H",        /* STUNDEN  */                             // "H"         /* ORE     */                              // STRINGS__SETUP__ANS__VALUE__H
    "SOM",                                                 // "EST"                                                  // STRINGS__SETUP__ANS__VALUE__SUM
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
    "kein Eintrag im Telefonbuch",                         // "Rubrica vuota"                                        // STRINGS__CONTACTS__ANS__VALUE__PHONEBOOK_EMPTY


    /*----------------------------------------------------------------------------------------------
     * "NEWNAME" command
     *----------------------------------------------------------------------------------------------*/
    /* command labels */
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__NEWNAME__CMD__LABEL__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__NEWNAME__CMD__LABEL__OUT2
    "EING1",                                               // "IN1"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN1
    "EING2",                                               // "IN2"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN2
    "EING3",                                               // "IN3"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN3
    "EING4",                                               // "IN4"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN4
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
    "EING1",                                               // "IN1"                                                  // STRINGS__ALARM__CMD__VALUE__IN1
    "EING2",                                               // "IN2"                                                  // STRINGS__ALARM__CMD__VALUE__IN2
    "EING3",                                               // "IN3"                                                  // STRINGS__ALARM__CMD__VALUE__IN3
    "EING4",                                               // "IN4"                                                  // STRINGS__ALARM__CMD__VALUE__IN4
    "POWER",                                               // "POWER"                                                // STRINGS__ALARM__CMD__VALUE__POWER
    "AKTIVIERT",                                           // "ATTIVO"                                               // STRINGS__ALARM__CMD__VALUE__ENABLED
    "DEAKTIVIERT",                                         // "DISATTIVO"                                            // STRINGS__ALARM__CMD__VALUE__DISABLED
    "OFFEN",                                               // "APERTO"                                               // STRINGS__ALARM__CMD__VALUE__OPEN
    "GESCHLOSSEN",                                         // "CHIUSO"                                               // STRINGS__ALARM__CMD__VALUE__CLOSE
    "S",        /* SEKUNDEN */                             // "S"         /* SECONDI */                              // STRINGS__ALARM__CMD__VALUE__S
    "M",        /* MINUTEN  */                             // "M"         /* MINUTI  */                              // STRINGS__ALARM__CMD__VALUE__M
    "H",        /* STUNDEN  */                             // "H"         /* ORE     */                              // STRINGS__ALARM__CMD__VALUE__H

    /* answer labels */
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__ALARM__ANS__LABEL__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__ALARM__ANS__LABEL__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__ALARM__ANS__LABEL__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__ALARM__ANS__LABEL__TMAXEXT
    "EING1",                                               // "IN1"                                                  // STRINGS__ALARM__ANS__LABEL__IN1
    "EING2",                                               // "IN2"                                                  // STRINGS__ALARM__ANS__LABEL__IN2
    "EING3",                                               // "IN3"                                                  // STRINGS__ALARM__ANS__LABEL__IN3
    "EING4",                                               // "IN4"                                                  // STRINGS__ALARM__ANS__LABEL__IN4
    "POWER",                                               // "POWER"                                                // STRINGS__ALARM__ANS__LABEL__POWER
    "SETREPORT",                                           // "SETREPORT"                                            // STRINGS__ALARM__ANS__LABEL__REPORT

    /* answer values */
    "AKTIVIERT",                                           // "ATTIVO"                                               // STRINGS__ALARM__ANS__VALUE__ENABLED
    "DEAKTIVIERT",                                         // "DISATTIVO"                                            // STRINGS__ALARM__ANS__VALUE__DISABLED
    "OFFEN",                                               // "APERTO"                                               // STRINGS__ALARM__ANS__VALUE__OPEN
    "GESCHLOSSEN",                                         // "CHIUSO"                                               // STRINGS__ALARM__ANS__VALUE__CLOSE
    "S",        /* SEKUNDEN */                             // "S"         /* SECONDI */                              // STRINGS__ALARM__ANS__VALUE__S
    "M",        /* MINUTEN  */                             // "M"         /* MINUTI  */                              // STRINGS__ALARM__ANS__VALUE__M
    "H",        /* STUNDEN  */                             // "H"         /* ORE     */                              // STRINGS__ALARM__ANS__VALUE__H


    /*--------------------------------------------------------------------------------------
     * "MESSAGE" command
     *--------------------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    "EING1",                                               // "IN1"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN1
    "EING2",                                               // "IN2"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN2
    "EING3",                                               // "IN3"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN3
    "EING4",                                               // "IN4"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN4
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
    "AUS",                                                 // "OFF"                                                  // STRINGS__SETREPORT__CMD__VALUE__STATUS__OFF
    "EIN",                                                 // "ON"                                                   // STRINGS__SETREPORT__CMD__VALUE__STATUS__ON

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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__TIMEROUT__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__TIMEROUT__CMD__VALUE__OUT2
    "S",        /* SEKUNDEN */                             // "S"         /* SECONDI */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__S
    "M",        /* MINUTEN  */                             // "M"         /* MINUTI  */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__M
    "H",        /* STUNDEN  */                             // "H"         /* ORE     */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__H
    "S",        /* SEKUNDEN */                             // "S"         /* SECONDI */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__S
    "M",        /* MINUTEN  */                             // "M"         /* MINUTI  */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__M
    "H",        /* STUNDEN  */                             // "H"         /* ORE     */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__H

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
    "AUS",                                                 // "OFF"                                                  // STRINGS__FORWARD__CMD__VALUE__STATUS__OFF
    "EIN",                                                 // "ON"                                                   // STRINGS__FORWARD__CMD__VALUE__STATUS__ON

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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__CHRONO__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__CHRONO__CMD__VALUE__OUT2
    "AUS",                                                 // "OFF"                                                  // STRINGS__CHRONO__CMD__VALUE__STATUS__OFF
    "EIN",                                                 // "ON"                                                   // STRINGS__CHRONO__CMD__VALUE__STATUS__ON
    "AUS",                                                 // "OFF"                                                  // STRINGS__CHRONO__CMD__VALUE__WINDOW__OFF

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
    "AUS",                                                 // "OFF"                                                  // STRINGS__BUTTONS__CMD__VALUE__STATUS__OFF
    "EIN",                                                 // "ON"                                                   // STRINGS__BUTTONS__CMD__VALUE__STATUS__ON

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
    "AUS",                                                 // "OFF"                                                  // STRINGS__SMSDELAY__CMD__VALUE__OFF
    "S",        /* SEKUNDEN */                             // "S"         /* SECONDI */                              // STRINGS__SMSDELAY__CMD__VALUE__S
    "M",        /* MINUTEN  */                             // "M"         /* MINUTI  */                              // STRINGS__SMSDELAY__CMD__VALUE__M
    "H",        /* STUNDEN  */                             // "H"         /* ORE     */                              // STRINGS__SMSDELAY__CMD__VALUE__H

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
    "AUS",                                                 // "OFF"                                                  // STRINGS__COUNTER__CMD__VALUE__STATUS__OFF
    "EIN",                                                 // "ON"                                                   // STRINGS__COUNTER__CMD__VALUE__STATUS__ON

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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__TURNON__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__TURNON__CMD__VALUE__OUT2

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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__TURNOFF__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__TURNOFF__CMD__VALUE__OUT2

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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__REGULATE__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__REGULATE__CMD__VALUE__OUT2
    "AUS",                                                 // "OFF"                                                  // STRINGS__REGULATE__CMD__VALUE__OFF

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
    "AUSG1",                                               // "OUT1"                                                 // STRINGS__ANTIFROST__CMD__VALUE__OUT1
    "AUSG2",                                               // "OUT2"                                                 // STRINGS__ANTIFROST__CMD__VALUE__OUT2
    "AUS",                                                 // "OFF"                                                  // STRINGS__ANTIFROST__CMD__VALUE__OFF

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
    "AUS",                                                 // "OFF"                                                  // STRINGS__CREDIT__CMD__VALUE__OFF

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
    "KONTAKT",                                             // "RUBRICA"                                              // STRINGS__DEFAULT__CMD__VALUE__CONTACTS
    "BEFEHLE",                                             // "COMANDI"                                              // STRINGS__DEFAULT__CMD__VALUE__COMMANDS
    "UBERALL",                                             // "TUTTO"                                                // STRINGS__DEFAULT__CMD__VALUE__ALL

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
    "Kein Befehl",                                         // "Nessun comando"                                       // STRINGS__COMMANDS__ANS__VALUE__NO_COMMANDS
};
