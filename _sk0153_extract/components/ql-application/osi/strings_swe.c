/*=============================================================================
 * File       :  STRINGS_SWE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - text strings - Swedish
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "fw_config.h"
#include "strings_my.h"
#include "strings_swe.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* strings - Swedish */
const ascii *const StringsSwe_Strings[] =
{
    "",                                                                                                              // STRINGS__EMPTY

    /*---------------------------------------------------------------------------
     * Alarm strings
     *---------------------------------------------------------------------------*/
    "Larm INGANG3 temperaturen ar for lag",                // "Allarme temperatura IN3 troppo bassa"                 // STRINGS__ALARM__TMIN
    "Larm INGANG4 temperaturen ar for lag",                 // "Allarme temperatura IN4 troppo bassa"                 // STRINGS__ALARM__TMINEXT
    "INGANG3 temperaturen ar for lag restored",            // "Ripristino temperatura IN3 troppo bassa"              // STRINGS__ALARM__TMIN_RESTORED
    "INGANG4 temperaturen ar for lag restored",             // "Ripristino temperatura IN4 troppo bassa"              // STRINGS__ALARM__TMINEXT_RESTORED

    "Larm INGANG3 for hog temperatur",                     // "Allarme temperatura IN3 troppo alta"                  // STRINGS__ALARM__TMAX
    "Larm INGANG4 for hog temperatur",                     // "Allarme temperatura IN4 troppo alta"                  // STRINGS__ALARM__TMAXEXT
    "INGANG3 for hog temperatur restored",                 // "Ripristino temperatura IN3 troppo alta"               // STRINGS__ALARM__TMAX_RESTORED
    "INGANG4 for hog temperatur restored",                 // "Ripristino temperatura IN4 troppo alta"               // STRINGS__ALARM__TMAXEXT_RESTORED

    "Larm INGANG1",                                        // "Allarme ingresso IN1"                                 // STRINGS__ALARM__IN1
    "Larm INGANG2",                                        // "Allarme ingresso IN2"                                 // STRINGS__ALARM__IN2
    "Larm INGANG3",                                        // "Allarme ingresso IN3"                                 // STRINGS__ALARM__IN3
    "Larm INGANG4",                                        // "Allarme ingresso IN4"                                 // STRINGS__ALARM__IN4
    "INGANG1 restored",                                    // "Ripristino ingresso IN1"                              // STRINGS__ALARM__IN1_RESTORED
    "INGANG2 restored",                                    // "Ripristino ingresso IN2"                              // STRINGS__ALARM__IN2_RESTORED
    "INGANG3 restored",                                    // "Ripristino ingresso IN3"                              // STRINGS__ALARM__IN3_RESTORED
    "INGANG4 restored",                                    // "Ripristino ingresso IN4"                              // STRINGS__ALARM__IN4_RESTORED

    "Larm avbrott driftspanning",                          // "Allarme interruzione rete elettrica"                  // STRINGS__ALARM__POWER_OFF
    "Driftspanning ater",                                  // "Rete elettrica ripristinata"                          // STRINGS__ALARM__POWER_RETURN

    "Varning, 10 st SMS aterstar",                         // "Attenzione, il credito risulta pari a 10 SMS"         // STRINGS__ALARM__CREDIT


    /*---------------------------------------------------------------------------
     * Weekday strings
     *---------------------------------------------------------------------------*/
    "Man",                                                 // "Lun"                                                  // STRINGS__WEEKDAY__MON
    "Tis",                                                 // "Mar"                                                  // STRINGS__WEEKDAY__TUE
    "Ons",                                                 // "Mer"                                                  // STRINGS__WEEKDAY__WED
    "Tor",                                                 // "Gio"                                                  // STRINGS__WEEKDAY__THU
    "Fre",                                                 // "Ven"                                                  // STRINGS__WEEKDAY__FRI
    "Lor",                                                 // "Sab"                                                  // STRINGS__WEEKDAY__SAT
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
    "Kommandofel",                                         // "Errore comando"                                       // STRINGS__ANSWER__COMMAND_ERROR
  //"Parameters command error",                            // "Errore parametri comando"                             // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR

    /* password */
    "LOSENORD KRAVS",                                      // "PASSWORD RICHIESTA"                                   // STRINGS__ANSWER__PASSWORD_REQUIRED
    "FELAKTIGT LOSENORD",                                  // "PASSWORD NON CORRETTA"                                // STRINGS__ANSWER__PASSWORD_INCORRECT

    /* configuration (set) */
    "KONFIGURATION ACCEPTERAD",                            // "CONFIGURAZIONE ACCETTATA"                             // STRINGS__ANSWER__CONFIGURATION_ACCEPTED
    "KONFIGURATION EJ ACCEPTERAD",                         // "CONFIGURAZIONE NON ACCETTATA"                         // STRINGS__ANSWER__CONFIGURATION_NOT_ACCEPTED
    "KONFIGURATION SYNTAX FEL",                            // "ERRORE SINTASSI CONFIGURAZIONE"                       // STRINGS__ANSWER__CONFIGURATION_SYNTAX_ERROR
    "KONFIGURATION PARAMETER FEL",                         // "ERRORE PARAMETRI CONFIGURAZIONE"                      // STRINGS__ANSWER__CONFIGURATION_PARAMETERS_ERROR

    /* telecontrol */
    "KOMMANDO UTFORT",                                     // "COMANDO ESEGUITO"                                     // STRINGS__ANSWER__COMMAND_EXECUTED
    "KOMMANDO EJ UTFORT",                                  // "COMANDO NON ESEGUITO"                                 // STRINGS__ANSWER__COMMAND_NOT_EXECUTED
    "KOMMANDO KAN EJ UTFORAS",                             // "COMANDO NON ESEGUIBILE"                               // STRINGS__ANSWER__COMMAND_NOT_EXECUTABLE
    "KOMMANDO SYNTAX FEL",                                 // "ERRORE SINTASSI COMANDO"                              // STRINGS__ANSWER__COMMAND_SYNTAX_ERROR
    "KOMMANDO PARAMETERFEL",                               // "ERRORE PARAMETRI COMANDO"                             // STRINGS__ANSWER__COMMAND_PARAMETERS_ERROR


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
    "LAGGTILL",                                            // "AGGIUNGI"                                             // STRINGS__COMMAND__ADD
    "TABORT",                                              // "RIMUOVI"                                              // STRINGS__COMMAND__REMOVE
    "KONTAKTER",                                           // "RUBRICA"                                              // STRINGS__COMMAND__CONTACTS
    "NYTTNAMN",                                            // "NEWNAME"                                              // STRINGS__COMMAND__NEWNAME
    "LARM",                                                // "ALLARME"                                              // STRINGS__COMMAND__ALARM
    "MEDDELANDE",                                          // "MESSAGGIO"                                            // STRINGS__COMMAND__MESSAGE
    "KALIBRERA",                                           // "CALIBRA"                                              // STRINGS__COMMAND__CALIBRATE
    "ANDRALOSENORD",                                       // "SETPSW"                                               // STRINGS__COMMAND__SETPSW
    "BEKRAFTELSESMS",                                      // "SETREPORT"                                            // STRINGS__COMMAND__SETREPORT
    "SPRAK",                                               // "LINGUA"                                               // STRINGS__COMMAND__LANGUAGE
    "LANGUAGE",                                            // "LANGUAGE"         // in English for all languages     // STRINGS__COMMAND__LANGUAGE_ENG
    "TIMEROUT",                                            // "TIMEROUT"                                             // STRINGS__COMMAND__TIMEROUT
    "EFTERSANDA",                                          // "INOLTRA"                                              // STRINGS__COMMAND__FORWARD
    "KRONO",                                               // "CRONO"                                                // STRINGS__COMMAND__CHRONO
    "SOMMAR",                                              // "ESTATE"                                               // STRINGS__COMMAND__SUMMER
    "VINTER",                                              // "INVERNO"                                              // STRINGS__COMMAND__WINTER
    "KNAPPAR",                                             // "PULSANTI"                                             // STRINGS__COMMAND__BUTTONS
    "FORDROJNINGSMS",                                      // "RITARDOSMS"                                           // STRINGS__COMMAND__SMSDELAY
    "COUNTER",                                             // "COUNTER"                                              // STRINGS__COMMAND__COUNTER
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__COMMAND__COUNTERBANDS

    /* telecontrol */
    "SLATILL",                                             // "ACCENDI"                                              // STRINGS__COMMAND__TURNON
    "SLAFRAN",                                             // "SPEGNI"                                               // STRINGS__COMMAND__TURNOFF
    "REGLERA",                                             // "REGOLA"                                               // STRINGS__COMMAND__REGULATE
    "ANTIFROST",                                           // "ANTIGELO"                                             // STRINGS__COMMAND__ANTIFROST
    "KREDIT",                                              // "CREDITO"                                              // STRINGS__COMMAND__CREDIT
    "DEFAULT",                                             // "DEFAULT"                                              // STRINGS__COMMAND__DEFAULT
    "SETTIME",                                             // "SETTIME"                                              // STRINGS__COMMAND__SETTIME
    "SYNKRONISERAR",                                       // "SINCRONIZZA"                                          // STRINGS__COMMAND__SYNCHRONIZE
    "RESET",                                               // "RESET"                                                // STRINGS__COMMAND__RESET
    "SETCOUNTER",                                          // "SETCOUNTER"                                           // STRINGS__COMMAND__SETCOUNTER
    "SETCOUNTERBANDS",                                     // "SETCOUNTERBANDS"                                      // STRINGS__COMMAND__SETCOUNTERBANDS

    /* data acquistion */
    "KOMMANDON",                                           // "COMANDI"                                              // STRINGS__COMMAND__COMMANDS



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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__STATUS__ANS__LABEL__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__STATUS__ANS__LABEL__OUT2
    "REGLERA",                                             // "REGOLA"                                               // STRINGS__STATUS__ANS__LABEL__REGULATE
    "REG1",                                                // "REG1"                                                 // STRINGS__STATUS__ANS__LABEL__REG1
    "REG2",                                                // "REG2"                                                 // STRINGS__STATUS__ANS__LABEL__REG2
    "ANTIFROST",                                           // "ANTIGELO"                                             // STRINGS__STATUS__ANS__LABEL__ANTIFROST
    "ANT1",                                                // "ANT1"                                                 // STRINGS__STATUS__ANS__LABEL__ANT1
    "ANT2",                                                // "ANT2"                                                 // STRINGS__STATUS__ANS__LABEL__ANT2
    "KRONO",                                               // "CRONO"                                                // STRINGS__STATUS__ANS__LABEL__CHRONO
    "KR1",                                                 // "CR1"                                                  // STRINGS__STATUS__ANS__LABEL__CHR1
    "KR2",                                                 // "CR2"                                                  // STRINGS__STATUS__ANS__LABEL__CHR2
    "INGANG1",                                             // "IN1"                                                  // STRINGS__STATUS__ANS__LABEL__IN1
    "INGANG2",                                             // "IN2"                                                  // STRINGS__STATUS__ANS__LABEL__IN2
    "INGANG3",                                             // "IN3"                                                  // STRINGS__STATUS__ANS__LABEL__IN3
    "INGANG4",                                             // "IN4"                                                  // STRINGS__STATUS__ANS__LABEL__IN4
    "C1",                                                  // "C1"                                                   // STRINGS__STATUS__ANS__LABEL__C1
    "C2",                                                  // "C2"                                                   // STRINGS__STATUS__ANS__LABEL__C2
    "SIGNAL",   /* GSM SIGNAL */                           // "SEGNALE"   /* SEGNALE GSM */                          // STRINGS__STATUS__ANS__LABEL__SIGNAL
    "GSM",                                                 // "GSM"                                                  // STRINGS__STATUS__ANS__LABEL__GSM
    "KREDIT",                                              // "CREDITO"                                              // STRINGS__STATUS__ANS__LABEL__CREDIT
    "SMS",                                                 // "SMS"                                                  // STRINGS__STATUS__ANS__LABEL__SMS
    "DRIFTSPANNING",                                       // "POWER"                                                // STRINGS__STATUS__ANS__LABEL__POWER
    "ADC_INT",                                             // "ADC_INT"                                              // STRINGS__STATUS__ANS__LABEL__ADC_INT
    "ADC_EXT",                                             // "ADC_EXT"                                              // STRINGS__STATUS__ANS__LABEL__ADC_EXT

    /* answer  values */
    "FEL",      /* FEL */                                  // "ERR"       /* ERRORE */                               // STRINGS__STATUS__ANS__VALUE__TEMPERATURE__ERROR
    "TILL",                                                // "ON"                                                   // STRINGS__STATUS__ANS__VALUE__ON
    "FRAN",                                                // "OFF"                                                  // STRINGS__STATUS__ANS__VALUE__OFF
    "OPPNA",                                               // "APERTO"                                               // STRINGS__STATUS__ANS__VALUE__OPEN
    "STANG",                                               // "CHIUSO"                                               // STRINGS__STATUS__ANS__VALUE__CLOSE
    "SVAG",     /* GSM SIGNAL */                           // "SCARSO"    /* SEGNALE GSM */                          // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__LOW
    "HYGGLIG",                                             // "MEDIO"                                                // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__MEDIUM
    "BRA",                                                 // "BUONO"                                                // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__GOOD
    "UTMARKT",                                             // "OTTIMO"                                               // STRINGS__STATUS__ANS__VALUE__GSM_SIGNAL__EXCELLENT
    "JA",                                                  // "SI"                                                   // STRINGS__STATUS__ANS__VALUE__YES
    "NEJ",                                                 // "NO"                                                   // STRINGS__STATUS__ANS__VALUE__NO


    /*-----------------------------------------------------------------------------
     * "VERSION"
     *-----------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    // ---

    /* answer labels */
    "TILLVERKARE",                                         // "COSTRUTTORE"                                          // STRINGS__VERSION__ANS__LABEL__MANUFACTURER
    "MODELL",                                              // "MODELLO"                                              // STRINGS__VERSION__ANS__LABEL__MODEL
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
    "KRONO",                                               // "CRONO"                                                // STRINGS__SETUP__CMD__VALUE__CHRONO
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__SETUP__CMD__VALUE__COUNTERBANDS
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__SETUP__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__SETUP__CMD__VALUE__OUT2
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
    "DRIFTSPANNING",                                       // "POWER"                                                // STRINGS__SETUP__ANS__LABEL__POWER
    "BEKRAFTELSESMS",                                      // "REPORT"                                               // STRINGS__SETUP__ANS__LABEL__REPORT
    "EFTERSANDA",                                          // "INOLTRA"                                              // STRINGS__SETUP__ANS__LABEL__FORWARD
    "FORDROJNINGSMS",                                      // "RITARDOSMS"                                           // STRINGS__SETUP__ANS__LABEL__SMSDELAY
    "KNAPPAR ",                                            // "PULSANTI"                                             // STRINGS__SETUP__ANS__LABEL__BUTTONS
    "KRONO",                                               // "CRONO"                                                // STRINGS__SETUP__ANS__LABEL__CHRONO
    "COUNTERBANDS",                                        // "COUNTERBANDS"                                         // STRINGS__SETUP__ANS__LABEL__COUNTERBANDS
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__SETUP__ANS__LABEL__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__SETUP__ANS__LABEL__OUT2

    /* answer values */
    "TILL",                                                // "ON"                                                   // STRINGS__SETUP__ANS__VALUE__ON
    "FRAN",                                                // "OFF"                                                  // STRINGS__SETUP__ANS__VALUE__OFF
    "O",                                                   // "O"                                                    // STRINGS__SETUP__ANS__VALUE__O
    "C",                                                   // "C"                                                    // STRINGS__SETUP__ANS__VALUE__C
    "S",        /* SEKUNDER */                             // "S"         /* SECONDI */                              // STRINGS__SETUP__ANS__VALUE__S
    "M",        /* MINUTER  */                             // "M"         /* MINUTI  */                              // STRINGS__SETUP__ANS__VALUE__M
    "H",        /* TIMMAR   */                             // "H"         /* ORE     */                              // STRINGS__SETUP__ANS__VALUE__H
    "SOM",                                                 // "EST"                                                  // STRINGS__SETUP__ANS__VALUE__SUM
    "VIN",                                                 // "INV"                                                  // STRINGS__SETUP__ANS__VALUE__WIN


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
    "Kontaktlistan tom",                                   // "Rubrica vuota"                                        // STRINGS__CONTACTS__ANS__VALUE__PHONEBOOK_EMPTY


    /*----------------------------------------------------------------------------------------------
     * "NEWNAME" command
     *----------------------------------------------------------------------------------------------*/
    /* command labels */
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__NEWNAME__CMD__LABEL__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__NEWNAME__CMD__LABEL__OUT2
    "INGANG1",                                             // "IN1"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN1
    "INGANG2",                                             // "IN2"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN2
    "INGANG3",                                             // "IN3"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN3
    "INGANG4",                                             // "IN4"                                                  // STRINGS__NEWNAME__CMD__LABEL__IN4
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
    "INGANG1",                                             // "IN1"                                                  // STRINGS__ALARM__CMD__VALUE__IN1
    "INGANG2",                                             // "IN2"                                                  // STRINGS__ALARM__CMD__VALUE__IN2
    "INGANG3",                                             // "IN3"                                                  // STRINGS__ALARM__CMD__VALUE__IN3
    "INGANG4",                                             // "IN4"                                                  // STRINGS__ALARM__CMD__VALUE__IN4
    "DRIFTSPANNING",                                       // "POWER"                                                // STRINGS__ALARM__CMD__VALUE__POWER
    "AKTIVERAD",                                           // "ATTIVO"                                               // STRINGS__ALARM__CMD__VALUE__ENABLED
    "DEAKTIVERAD",                                         // "DISATTIVO"                                            // STRINGS__ALARM__CMD__VALUE__DISABLED
    "OPPNA",                                               // "APERTO"                                               // STRINGS__ALARM__CMD__VALUE__OPEN
    "STANG",                                               // "CHIUSO"                                               // STRINGS__ALARM__CMD__VALUE__CLOSE
    "S",        /* SEKUNDER */                             // "S"         /* SECONDI */                              // STRINGS__ALARM__CMD__VALUE__S
    "M",        /* MINUTER  */                             // "M"         /* MINUTI  */                              // STRINGS__ALARM__CMD__VALUE__M
    "H",        /* TIMMAR   */                             // "H"         /* ORE     */                              // STRINGS__ALARM__CMD__VALUE__H

    /* answer labels */
    "TIN3MIN",                                             // "TIN3MIN"                                              // STRINGS__ALARM__ANS__LABEL__TMIN
    "TIN4MIN",                                             // "TIN4MIN"                                              // STRINGS__ALARM__ANS__LABEL__TMINEXT
    "TIN3MAX",                                             // "TIN3MAX"                                              // STRINGS__ALARM__ANS__LABEL__TMAX
    "TIN4MAX",                                             // "TIN4MAX"                                              // STRINGS__ALARM__ANS__LABEL__TMAXEXT
    "INGANG1",                                             // "IN1"                                                  // STRINGS__ALARM__ANS__LABEL__IN1
    "INGANG2",                                             // "IN2"                                                  // STRINGS__ALARM__ANS__LABEL__IN2
    "INGANG3",                                             // "IN3"                                                  // STRINGS__ALARM__ANS__LABEL__IN3
    "INGANG4",                                             // "IN4"                                                  // STRINGS__ALARM__ANS__LABEL__IN4
    "DRIFTSPANNING",                                       // "POWER"                                                // STRINGS__ALARM__ANS__LABEL__POWER
    "BEKRAFTELSESMS",                                      // "SETREPORT"                                            // STRINGS__ALARM__ANS__LABEL__REPORT

    /* answer values */
    "AKTIVERAD",                                           // "ATTIVO"                                               // STRINGS__ALARM__ANS__VALUE__ENABLED
    "DEAKTIVERAD",                                         // "DISATTIVO"                                            // STRINGS__ALARM__ANS__VALUE__DISABLED
    "OPPNA",                                               // "APERTO"                                               // STRINGS__ALARM__ANS__VALUE__OPEN
    "STANG",                                               // "CHIUSO"                                               // STRINGS__ALARM__ANS__VALUE__CLOSE
    "S",        /* SEKUNDER */                             // "S"         /* SECONDI */                              // STRINGS__ALARM__ANS__VALUE__S
    "M",        /* MINUTER  */                             // "M"         /* MINUTI  */                              // STRINGS__ALARM__ANS__VALUE__M
    "H",        /* TIMMAR   */                             // "H"         /* ORE     */                              // STRINGS__ALARM__ANS__VALUE__H


    /*--------------------------------------------------------------------------------------
     * "MESSAGE" command
     *--------------------------------------------------------------------------------------*/
    /* command labels */
    // ---

    /* command values */
    "INGANG1",                                             // "IN1"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN1
    "INGANG2",                                             // "IN2"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN2
    "INGANG3",                                             // "IN3"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN3
    "INGANG4",                                             // "IN4"                                                  // STRINGS__MESSAGE__CMD__VALUE__IN4
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
    "FRAN",                                                // "OFF"                                                  // STRINGS__SETREPORT__CMD__VALUE__STATUS__OFF
    "TILL",                                                // "ON"                                                   // STRINGS__SETREPORT__CMD__VALUE__STATUS__ON

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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__TIMEROUT__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__TIMEROUT__CMD__VALUE__OUT2
    "S",        /* SEKUNDER */                             // "S"         /* SECONDI */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__S
    "M",        /* MINUTER  */                             // "M"         /* MINUTI  */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__M
    "H",        /* TIMMAR   */                             // "H"         /* ORE     */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT1__H
    "S",        /* SEKUNDER */                             // "S"         /* SECONDI */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__S
    "M",        /* MINUTER  */                             // "M"         /* MINUTI  */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__M
    "H",        /* TIMMAR   */                             // "H"         /* ORE     */                              // STRINGS__TIMEROUT__CMD__VALUE__OUT2__H

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
    "FRAN",                                                // "OFF"                                                  // STRINGS__FORWARD__CMD__VALUE__STATUS__OFF
    "TILL",                                                // "ON"                                                   // STRINGS__FORWARD__CMD__VALUE__STATUS__ON

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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__CHRONO__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__CHRONO__CMD__VALUE__OUT2
    "FRAN",                                                // "OFF"                                                  // STRINGS__CHRONO__CMD__VALUE__STATUS__OFF
    "TILL",                                                // "ON"                                                   // STRINGS__CHRONO__CMD__VALUE__STATUS__ON
    "FRAN",                                                // "OFF"                                                  // STRINGS__CHRONO__CMD__VALUE__WINDOW__OFF

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
    "FRAN",                                                // "OFF"                                                  // STRINGS__BUTTONS__CMD__VALUE__STATUS__OFF
    "TILL",                                                // "ON"                                                   // STRINGS__BUTTONS__CMD__VALUE__STATUS__ON

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
    "FRAN",                                                // "OFF"                                                  // STRINGS__SMSDELAY__CMD__VALUE__OFF
    "S",        /* SEKUNDER */                             // "S"         /* SECONDI */                              // STRINGS__SMSDELAY__CMD__VALUE__S
    "M",        /* MINUTER  */                             // "M"         /* MINUTI  */                              // STRINGS__SMSDELAY__CMD__VALUE__M
    "H",        /* TIMMAR   */                             // "H"         /* ORE     */                              // STRINGS__SMSDELAY__CMD__VALUE__H

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
    "FRAN",                                                // "OFF"                                                  // STRINGS__COUNTER__CMD__VALUE__STATUS__OFF
    "TILL",                                                // "ON"                                                   // STRINGS__COUNTER__CMD__VALUE__STATUS__ON

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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__TURNON__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__TURNON__CMD__VALUE__OUT2

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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__TURNOFF__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__TURNOFF__CMD__VALUE__OUT2

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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__REGULATE__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__REGULATE__CMD__VALUE__OUT2
    "FRAN",                                                // "OFF"                                                  // STRINGS__REGULATE__CMD__VALUE__OFF

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
    "UTGANG1",                                             // "OUT1"                                                 // STRINGS__ANTIFROST__CMD__VALUE__OUT1
    "UTGANG2",                                             // "OUT2"                                                 // STRINGS__ANTIFROST__CMD__VALUE__OUT2
    "FRAN",                                                // "OFF"                                                  // STRINGS__ANTIFROST__CMD__VALUE__OFF

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
    "FRAN",                                                // "OFF"                                                  // STRINGS__CREDIT__CMD__VALUE__OFF

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
    "KONTAKTER",                                           // "RUBRICA"                                              // STRINGS__DEFAULT__CMD__VALUE__CONTACTS
    "KOMMANDON",                                           // "COMANDI"                                              // STRINGS__DEFAULT__CMD__VALUE__COMMANDS
    "HELA",                                                // "TUTTO"                                                // STRINGS__DEFAULT__CMD__VALUE__ALL

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
    "Inget kommando",                                     // "Nessun comando"                                        // STRINGS__COMMANDS__ANS__VALUE__NO_COMMANDS
};
