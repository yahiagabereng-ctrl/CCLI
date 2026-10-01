/*=============================================================================
 * File       :  SMTP.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - HTTP connection
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
- TO DO - implementare SMTP con SSL

- TEST  - provare con SIM Vodafone, TIM, WIND
- TEST  - provare con diversi server SMTP e diversi destinatari della mail (libero.it, gmail.com, hotmail.com, shitek.it, ...)

- timeout SMTP 10 minuti !!!
- priorità tra transmission_sms e transmission_mail
*/


/*
AT+SMS="SETAPN SERV="internet.wind" USER="" PWD="""
AT+SMS="SETAPN SERV="internet.wind.biz" USER="" PWD="""
AT+SMS="SETAPN SERV="ibox.tim.it" USER="" PWD="""
AT+SMS="SETAPN SERV="web.omnitel.it" USER="" PWD="""

AT+SMS="SETSMTP SERV="mail.posta.tim.it" PORT=25 USER="pippo.paperino.2009@alice.it" PWD="shitek2009" AUTH=2 SEC=0"
AT+SMS="SETSMTP SERV="mail.posta.tim.it" PORT=25 USER="3316486432" PWD="19041968" AUTH=0 SEC=0"
AT+SMS="SETSMTP SERV="smtp.wind.it" PORT=25 USER="" PWD="" AUTH=0 SEC=0"
AT+SMS="SETSMTP SERV="smtp.libero.it" PORT=25 USER="" PWD="" AUTH=0 SEC=0"
AT+SMS="SETSMTP SERV="smtp.gmail.com" PORT=25 USER="" PWD="" AUTH=0 SEC=0"
AT+SMS="SETSMTP SERV="smtp.gmail.com" PORT=465 USER="" PWD="" AUTH=0 SEC=0"

AT+SMS="SETMAILADDRESSTO ADDRTO1="marco.g@shitek.it" ADDRTO2="marco.g.shitek@gmail.com""
AT+SMS="SETMAILADDRESSCC ADDRCC1="marco.g.shitek@gmail.com" ADDRCC2="marco.g@shitek.it""
AT+SMS="SETMAILADDRESSBCC ADDRBCC1="marco.g@shitek.it" ADDRBCC2="marco.g@shitek.it""
AT+SMS="SETMAILADDRESSBACKUP ADDR1="marco.g.shitek@gmail.com" ADDR2="marco.gardini@libero.it""

AT+SMS="SETMAILADDRESSTO ADDRTO1="marco.g@shitek.it" ADDRTO2="""
AT+SMS="SETMAILADDRESSCC ADDRCC1="marco.g.shitek@gmail.com" ADDRCC2="""
AT+SMS="SETMAILADDRESSBCC ADDRBCC1="" ADDRBCC2="""
AT+SMS="SETMAILADDRESSBACKUP ADDR1="" ADDR2="""

AT+SMS="SETMAILSENDER SENDER="pippo.paperino.2009@alice.it""
AT+SMS="SETMAILSENDER SENDER="marco.gardini@libero.it""

AT+SMS="GETSMTP"
AT+SMS="GETMAILADDRESSTO"
AT+SMS="GETMAILADDRESSCC"
AT+SMS="GETMAILADDRESSBCC"
AT+SMS="GETMAILADDRESSBACKUP"
AT+SMS="GETMAILSENDER"

*** QUESTO FUNZIONA ***
AT+SMS="SETDEVICEID ID="000000012345""
AT+SMS="SETDEVICEID ID="000000030985""
AT+SMS="SETDEVICEID ID="000000030958""
AT+SMS="SETAPN SERV="internet.wind.biz" USER="" PWD="""
AT+SMS="SETSMTP SERV="smtp.libero.it" PORT=25 USER="marco.shitek@libero.it" PWD="shitek2009" AUTH=2 SEC=0"
AT+SMS="SETMAILSENDER SENDER="marco.shitek@libero.it""
AT+SMS="SETMAILADDRESSTO ADDRTO1="marco.g@shitek.it" ADDRTO2="marco.shitek@libero.it""
AT+SMS="SETMAILADDRESSCC ADDRCC1="marco.g.shitek@gmail.com" ADDRCC2="""
AT+SMS="SETMAILADDRESSBCC ADDRBCC1="" ADDRBCC2="""
AT+SMS="SETMAILADDRESSBACKUP ADDR1="" ADDR2="""
*** QUESTO FUNZIONA ***


AT+SMS="SETSMTP SERV="smtp.libero.it" PORT=25 USER="shitek3@libero.it" PWD="shitek2009" AUTH=2 SEC=0"
AT+SMS="SETMAILSENDER SENDER="shitek3@libero.it""
AT+SMS="SETMAILADDRESSTO ADDRTO1="federico.t@shitek.it" ADDRTO2="shitek3@yahoo.com""
AT+SMS="SETMAILADDRESSCC ADDRCC1="shitek3@gmail.com" ADDRCC2="shitek3@libero.it""
AT+SMS="SETMAILADDRESSBCC ADDRBCC1="shitek3@hotmail.com" ADDRBCC2="marco.g@shitek.it""


AT+SMS="AGGIUNGI MARCO +393495266068"
AT+SMS="AGGIUNGI MYN +393898306625"
AT+SMS="AGGIUNGI MYN +393287692637"
AT+SMS="AGGIUNGI MYN +393290074754"
AT+SMS="COUNTER C1 ON"
AT+SMS="COUNTER C2 ON"
AT+SMS="SETLOG ENABLE=T PERIOD=1 DURATION=1"
AT+SMS="SETLOG ENABLE=T PERIOD=1 DURATION=4"
AT+SMS="SETLOG ENABLE=T PERIOD=5 DURATION=1"
AT+SMS="SETLOG ENABLE=T PERIOD=30 DURATION=1"

AT+SMS="SETTIME 02/07/2015 00:00:58"

AT+SMS="DEFAULT1 DATA="QUEUE""
AT+FS=2
AT+FS=5,/EASY__2015.06.26_08.00.00__2015.06.26_10.00.00__(build).csv
AT+FS=5,/EASY__2015.06.26_08.00.00__2015.06.26_10.00.00__(send).csv
AT+FS=7,0

AT+SMS="DOTA M=0 P=21 S="62.149.133.71" U="3645545@aruba.it" P="tk22qhx3" P="/shitek-upgrade.eu/FW/SK0061__EASY/" F="SK0061__EASY.dwl""
*/


/*
smtp.live.com           port 25
smtp.libero.it          port 25
smtpmail.vodafone.it    port 25
mail.inwind.it          port 25
smtp.gmail.com          port 465
smtp.aruba.it           port 25
*/

/*
  SMTPS (SMTP Secured)   usa il TLS/SSL
  Originalmente, nel 1997, la IASNS registrò la porta 465 per l'SMTPS.
  l'interfaccia obsoleta SMTPS sulla porta 465 in aggiunta alla (o al posto della!) porta 587 definita dalla RFC 6409.


  Con TIM     , usando il server TIM come server SMPT, non è richiesta l'autenticazione. Sono riuscito a mandare mail con e senza allegato.
  Con WIND    , sono per ora riuscito solo con WIPSoft, mentre ho ancora un errore facendo da applicazione OpenAT.
  Con Vodafone, non ho ancora provato (tra l'altro attualmente non ho una SIM Vodafone GPRS).

  Vodafone - I settaggi Vodafone da usare per l'invio mail (SMTP), da quello che leggo sono funzione del tipo di SIM (tipo contratto, se referente, ...).

  TIM - Ad esempio con SIM TIM, usando "mail.posta.tim.it" ed inviando una mail a 3 indirizzi (libero.it, hotmail.it, shitek.it), le mail arrivano regolarmente a libero.it e hotmail.it, ma all'indirizzo nostro interno Shitek (shitek.it) arriva con il corpo del testo vuoto (il resto è ok).
  Con Wind e Vodafone ho qualche problema nel collegarmi ai loro server SMTP.
  Sul forum Sierra Wireless leggevo che può essere importante anche l'indirizzo SENDER (il suo dominio, se diverso o uguale a quello del server SMTP, se l'indirizzo è esistente o meno).
  E leggevo poi di un problema o meno a seconda che gli indirizzi mail (mittente e destinatari) fossero racchiusi o meno da < >.

  DA SAMIOLO - Ti allego questo esempio che lavora con la e.mail di google, quindi invia e.mail usando il protocollo SSMTP.

  L'assistenza Vodafone mi ha dato questi parametri per quelle SIM:
  - server SMTP: smtp.net.vodafone.it
  - porta      : 465
  - user       : niente
  - password   : niente
  - SSL        : si

  Ho provato l'esempio che mi ha mandato Samiolo con l'SSL adattandolo e ho trasmesso con successo mail con SIM Vodafone usando il server SMTP di gmail (con SSL) ad un indirizzo libero.it.
  L'esempio di Samiolo era con SIM TIM inviando su server SMTP gmail.
  Ho provato inviando su server SMTP Vodafone, ma ottengo un errore. Probabilmente va cambiato il certificato di sicurezza che nell'esempio è quello di gmail (almeno c'è un commento in tal senso).
  Penso proprio che il Q24Plus versione v6.57d non supporti l'SSL. Da vedere se versione più recente lo supporta, ma ho qualche dubbio.
  Si deve linkare la libreria SSL che è un'altra rispetto alla solita WIP LIB che usiamo per GPRS, FTP, HTTP, ...
  Se non supporta SSL, si deve trovare un server SMTP che usa la porta tradizionale 25 e quindi non usa SSL che lascia passare la richiesta fatta via Vodafone.

  WIPSpoft
  AT+WIPOPT=6,1,2,61,"..."
  AT+WIPOPT=6,1,2,62,"..."
  AT+WIPOPT=6,1,2,63,"..."
  AT+WIPOPT=6,1,2,64,"..."
  AT+WIPOPT=6,1,2,65,"..."
  AT+WIPOPT=6,1,2,66,"..."

  https://forum.sierrawireless.com/viewtopic.php?f=109&t=8414&p=34426&hilit=SMTP#p34426

  http://docs.bvstools.com/home/ssl-documentation/openssl
  s_client -connect smtp.gmail.com:465
*/


/*
AT+WIPCFG=1                                                 // start IP stack
AT+WIPBR=1,6                                                // open GPRS bearer
AT+WIPBR=2,6,11,"ibox.tim.it"                               // set APN name of GPRS bearer
AT+WIPBR=2,6,11,"web.omnitel.it                             // set APN name of GPRS bearer
AT+WIPBR=2,6,11,"internet.wind"                             // set APN name of GPRS bearer
AT+WIPBR=2,6,11,"internet.wind.biz"                         // set APN name of GPRS bearer
AT+WIPBR=2,6,0,""                                           // set user name
AT+WIPBR=2,6,1,""                                           // set password
AT+WIPBR=4,6,0                                              // start GPRS bearer

AT+WIPCREATE=6,1,"smtp.wind.it",25,"",""                    // connect to remote SMTP server
AT+WIPCREATE=6,1,"smtp.libero.it",25,"",""                  // connect to remote SMTP server
AT+WIPCREATE=6,1,"mail.posta.tim.it",25,"",""               // connect to remote SMTP server
AT+WIPCREATE=6,1,"box.posta.tim.it",25,"",""                // connect to remote SMTP server
AT+WIPCREATE=6,1,"smtp.gmail.com,25,"",""                   // connect to remote SMTP server

+WIPREADY: 6,1
//connection and authentication are successful

AT+WIPOPT=6,1,1,60                                          // get last protocol error code and associated error string
AT+WIPOPT=6,1,2,61,"sender@gmail.com"                       // set sender mail address
AT+WIPOPT=6,1,2,62,"Marco"                                  // set sender mail
AT+WIPOPT=6,1,2,63,"marco.g@shitek.it, marco.g@shitek.it"   // set     receiver mail address
AT+WIPOPT=6,1,2,64,"marco.g@shitek.it, marco.g@shitek.it"   // set CC  receiver mail address
AT+WIPOPT=6,1,2,65,"marco.g@shitek.it, marco.g@shitek.it"   // set BCC receiver mail address
AT+WIPOPT=6,1,2,66,"Mail subject"                           // set mail subject

AT+WIPFILE=6,1,2
CONNECT
<user starts sending mail with the UART in data mode and ends with an [ETX] character>
OK


AT+CMEE=1

AT+WIPCFG=1
AT+WIPBR=1,6
AT+WIPBR=2,6,11,"ibox.tim.it"
AT+WIPBR=2,6,11,"web.omnitel.it
AT+WIPBR=2,6,11,"internet.wind"
AT+WIPBR=2,6,11,"internet.wind.biz"
AT+WIPBR=2,6,0,""
AT+WIPBR=2,6,1,""
AT+WIPBR=4,6,0

AT+WIPCREATE=6,1,"smtp.wind.it",25,"",""
AT+WIPCREATE=6,1,"smtp.libero.it",25,"marco.shitek@libero.it","shitek2009"  //
AT+WIPCREATE=6,1,"mail.posta.tim.it",25,"",""
AT+WIPCREATE=6,1,"smtp.gmail.com,25,"",""
AT+WIPCREATE=6,1,"mail.posta.tim.it",25,"3316486432","19041968"
//AT+WIPCREATE=6,1,"mail.posta.tim.it",25,"pippo.paperino.2009@alice.it","shitek2009"

AT+WIPOPT=6,1,1,60

AT+WIPOPT=6,1,2,61,"marco.shitek@libero.it"     //
AT+WIPOPT=6,1,2,61,"sender@gmail.com"
AT+WIPOPT=6,1,2,61,"pippo.paperino.2009@alice.it"
AT+WIPOPT=6,1,2,62,"Marco"
AT+WIPOPT=6,1,2,63,"marco.g@shitek.it"
AT+WIPOPT=6,1,2,64,"marco.g@shitek.it"
AT+WIPOPT=6,1,2,65,"marco.g@shitek.it"
AT+WIPOPT=6,1,2,66,"Mail subject"

AT+WIPFILE=6,1,2
*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "smtp.h"
#include "typedef.h"

#ifdef FUNCTION_IMPLEMENTED
#include "boot.h"
#include "debug_my.h"
#include "fw_config.h"
#include "clock.h"
#include "utility.h"
#include "transmission_mail.h"
#include "device_id.h"
#include "phone.h"
#include "file_system.h"
#include "alarm.h"
#endif




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* file block size (bytes) */
#define FILE_BLOCK_SIZE           (3 * 5000)   // it must be a multiple of 3

/* max file size (bytes) */
//#define MAX_FILE_SIZE             130000       //@@@
#define MAX_FILE_SIZE             10            //@@@

/* max mail  string length */
#define MAX_LENGTH_MAIL_STRING    ((MAX_FILE_SIZE * 4 / 3) + 4000)

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING   600




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
#ifdef FUNCTION_IMPLEMENTED
/* "file in transmission" info */
static       u8                                  Smtp_FileTx_FileType;
static       ascii                               Smtp_FileTx_FileName[68 + 1];
static       u8                                  Smtp_FileTx_V;
static       u32                                 Smtp_FileTx_N;
static       CLOCK__TIME                         Smtp_FileTx_StartTime;
static       CLOCK__TIME                         Smtp_FileTx_StopTime;
static       u8                                  Smtp_FileTx_AlarmType;
static       u16                                 Smtp_FileTx_AlarmPar;

/* mail string */
static       ascii                               Smtp_MailString [MAX_LENGTH_MAIL_STRING  + 1];

/* debug string */
static       ascii                               Smtp_DebugString[MAX_LENGTH_DEBUG_STRING + 1];
#endif


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "SMTP server" */
static       SMTP__CONFIG__SMTP_SERVER           Smtp_Config_SmtpServer;
static const SMTP__CONFIG__SMTP_SERVER           Smtp_Config_SmtpServerDefault =
{
    "",                           // SMTP server name
    25,                           // SMTP server port
    "",                           // SMTP server username
    "",                           // SMTP server password
    SMTP__SMTP_AUTH_TYPE__NONE,   // SMTP server authentication type
    SMTP__SMTP_SEC_TYPE__NONE,    // SMTP server security       type
};


/* configuration - "mail sender" */
static       SMTP__CONFIG__MAIL_SENDER           Smtp_Config_MailSender;
static const SMTP__CONFIG__MAIL_SENDER           Smtp_Config_MailSenderDefault =
{
    "",                           // mail sender
};


/* configuration - "mail recipient To" */
static       SMTP__CONFIG__MAIL_RECIPIENT_TO     Smtp_Config_MailRecipientTo;
static const SMTP__CONFIG__MAIL_RECIPIENT_TO     Smtp_Config_MailRecipientToDefault =
{
    { "", "" },                   // mail address TO
};


/* configuration - "mail recipient Cc" */
static       SMTP__CONFIG__MAIL_RECIPIENT_CC     Smtp_Config_MailRecipientCc;
static const SMTP__CONFIG__MAIL_RECIPIENT_CC     Smtp_Config_MailRecipientCcDefault =
{
    { "", "" },                   // mail address CC
};


/* configuration - "mail recipient Bcc" */
static       SMTP__CONFIG__MAIL_RECIPIENT_BCC    Smtp_Config_MailRecipientBcc;
static const SMTP__CONFIG__MAIL_RECIPIENT_BCC    Smtp_Config_MailRecipientBccDefault =
{
    { "", "" },                   // mail address BCC
};


/* configuration - "mail recipient backup" */
static       SMTP__CONFIG__MAIL_RECIPIENT_BACKUP Smtp_Config_MailRecipientBackup;
static const SMTP__CONFIG__MAIL_RECIPIENT_BACKUP Smtp_Config_MailRecipientBackupDefault =
{
    { "", "" },                   // mail address backup
};


#ifdef FUNCTION_IMPLEMENTED
/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* SMTP handlers */
static       wip_channel_t                       Smtp_SmtpHandler_ChannelConnection   = (wip_channel_t)NULL;    // "Connection"    channel handler
static       wip_channel_t                       Smtp_SmtpHandler_ChannelDataTransfer = (wip_channel_t)NULL;    // "Data Transfer" channel handler
#endif


////////////////////////////////////////////////////////////////////////////////////////////////
//@@@

/* Pre formatted mail without header (wip smtp generates the header) */
//static const ascii                              *Smtp_StringBody    = "SMTP predefined mail sample:                    \r\n"
//                                                                      "                                                \r\n"
//                                                                      "This body message is intended to illustrate the \r\n"
//                                                                      "SMTP Client Interfaces used for a               \r\n"
//                                                                      "pre configured mail delivery with the predefined\r\n"
//                                                                      "recipients lists.\r\n"
//                                                                      "                 \r\n"
//                                                                      "                 \r\n"
//                                                                      "The WipSender... \r\n"
//                                                                      "                 \r\n";


/* Pre formatted mail + header with file attachment (application pre formats the header) */
//static const ascii                              *Smtp_StringAttMail = "Subject: Data from Easy \r\n"
//                                                                      "From: Easy <easy@shitek.it> \r\n"
//                                                                      "To: marco.g@shitek.it \r\n"
//                                                                      "Cc: marco.gardini@libero.it \r\n"
//                                                                      "Bcc: marco.shitek@libero.it \r\n"
//                                                                      "MIME-Version: 1.0\r\n"
//                                                                      "Content-Type: Multipart/Mixed; \r\n"
//                                                                      "    boundary=\"----=__marking_attachment\"\r\n"
//                                                                      "\r\n"
//                                                                      "\r\n"
//
//                                                                      /****** This ends the application pre formatted header ******/
//
//                                                                      "This is a multi-part message in MIME format.\r\n"    // this text is ignored
//
//                                                                      /****** Start of message text section ******/
//                                                                      "------=__marking_attachment\r\n"
//                                                                      "Content-Type: text/plain\r\n"
//                                                                      "    charset=\"us-ascii\"\r\n"
//                                                                      "Content-Transfer-Encoding: quoted-printable\r\n"
//                                                                      "\r\n"
//                                                                      "Hello, this is a demo mail with an attachment file\r\n"
//                                                                      "\r\n"
//                                                                      "\r\n"
//
//                                                                      /****** Start of attachment section ******/
//                                                                      "------=__marking_attachment\r\n"
//                                                                      "Content-Type: text/plain;\r\n"
//                                                                      "         name=\"attached_file.txt\"\r\n"
//                                                                      "Content-Transfer-Encoding: base64\r\n"
//                                                                      "Content-Description: Attached txt file encrypted mime64\r\n"
//                                                                      "Content-Disposition: attachment; filename=\"attached_file.txt\" \r\n"
//                                                                      "\r\n"
//
//                                                                      /****** Start of encrypted attachment block ******/
//                                                                      "VGhpcyBib2R5IG1lc3NhZ2UgaXMgaW50ZW5kZWQgdG8gaWxsdXN0cmF0ZSB0aGUgDQpTTVRQIENsaWVudCBJbnRlcmZhY2VzIHVzZWQgZm9yIGEgICAgICAgICAgICAgICANCnByZSBjb25maWd1cmVkIG1haWwgZGVsaXZlcnkgd2l0aCB0aGUgcHJlZGVmaW5lZA0KcmVjaXBpZW50cyBsaXN0cy4NCiAgICAgICAgICAgICAgICAgDQogICAgICAgICAgICAgICAgIA0KVGhlIFdpcFNlbmRlci4uLiANCiAgICAgICAgICAgICAgICAgDQo= \r\n\r\n\r\n"
//
//                                                                      /****** End of attachment section ******/
//                                                                      "------=__marking_attachment--\r\n";


//static const ascii                              *Smtp_File_1        = "DATE;TIME;IN1;IN2;OUT1;OUT2;TINT;TEXT;RSSI;R;POWER;CREDIT\r\n"
//                                                                      "10/06/2015;12:00:00;3568;114;0;0;21.5;-3.1;18;-;1;123\r\n"
//                                                                      "10/06/2015;12:02:00;3589;119;0;0;21.7;-3.2;20;-;1;123\r\n"
//                                                                      "10/06/2015;12:04:00;3601;125;0;0;22.0;-3.0;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:06:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n";


//static const ascii                              *Smtp_File_2        = "DATE;TIME;IN1;IN2;OUT1;OUT2;TINT;TEXT;RSSI;R;POWER;CREDIT\r\n"
//                                                                      "10/06/2015;12:00:00;3568;114;0;0;21.5;-3.1;18;-;1;123\r\n"
//                                                                      "10/06/2015;12:02:00;3589;119;0;0;21.7;-3.2;20;-;1;123\r\n"
//                                                                      "10/06/2015;12:04:00;3601;125;0;0;22.0;-3.0;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:06:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:08:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:10:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:12:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:14:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:16:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:18:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:20:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:22:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:24:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:26:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:28:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:30:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:32:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:34:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:36:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:38:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:40:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:42:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:44:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:46:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:48:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:50:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:52:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:54:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:56:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;12:58:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//
//                                                                      "10/06/2015;13:00:00;3568;114;0;0;21.5;-3.1;18;-;1;123\r\n"
//                                                                      "10/06/2015;13:02:00;3589;119;0;0;21.7;-3.2;20;-;1;123\r\n"
//                                                                      "10/06/2015;13:04:00;3601;125;0;0;22.0;-3.0;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:06:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:08:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:10:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:12:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:14:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:16:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:18:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:20:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:22:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:24:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:26:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:28:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:30:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:32:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:34:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:36:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:38:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:40:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:42:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:44:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:46:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:48:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:50:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:52:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:54:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:56:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;13:58:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//
//                                                                      "10/06/2015;14:00:00;3568;114;0;0;21.5;-3.1;18;-;1;123\r\n"
//                                                                      "10/06/2015;14:02:00;3589;119;0;0;21.7;-3.2;20;-;1;123\r\n"
//                                                                      "10/06/2015;14:04:00;3601;125;0;0;22.0;-3.0;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:06:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:08:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:10:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:12:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:14:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:16:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:18:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:20:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:22:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:24:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:26:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:28:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:30:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:32:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:34:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:36:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:38:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:40:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:42:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:44:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:46:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:48:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:50:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:52:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:54:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:56:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;14:58:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//
//                                                                      "10/06/2015;15:00:00;3568;114;0;0;21.5;-3.1;18;-;1;123\r\n"
//                                                                      "10/06/2015;15:02:00;3589;119;0;0;21.7;-3.2;20;-;1;123\r\n"
//                                                                      "10/06/2015;15:04:00;3601;125;0;0;22.0;-3.0;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:06:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:08:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:10:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:12:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:14:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:16:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:18:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:20:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:22:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:24:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:26:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:28:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:30:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:32:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:34:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:36:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:38:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:40:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:42:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:44:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:46:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:48:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:50:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:52:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:54:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:56:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n"
//                                                                      "10/06/2015;15:58:00;3615;128;0;0;23.0;-3.1;19;-;1;123\r\n";
////////////////////////////////////////////////////////////////////////////////////////////////




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
#ifdef FUNCTION_IMPLEMENTED
/* SMTP */
       bool   Smtp_ClientSmtpRequest(u8 file_type, ascii *file_name, u8 v, u32 n, CLOCK__TIME start_time, CLOCK__TIME stop_time, u8 alarm_type, u16 alarm_par);
       bool   Smtp_ClientSmtp       (void);

/* opening of session/data channel */
static bool   Smtp_ClientSmtpOpenChannel_Session(void);
static bool   Smtp_ClientSmtpOpenChannel_Data   (void);

/* build mail */
static void   Smtp_BuildMail(void);

/* build strings */
static ascii *Smtp_BuildMailRecTo              (bool angle_brackets);
static ascii *Smtp_BuildMailRecCc              (bool angle_brackets);
static ascii *Smtp_BuildMailRecBcc             (bool angle_brackets);
static ascii *Smtp_BuildMailSender             (bool angle_brackets);
static ascii *Smtp_BuildMailSenderName         (bool angle_brackets);
static ascii *Smtp_BuildMailSenderAndSenderName(bool angle_brackets, bool quotation_marks);
static ascii *Smtp_BuildMailSubject            (void);
static ascii *Smtp_BuildMailBody               (void);
static ascii *Smtp_BuildMailFileName           (void);
static ascii *Smtp_BuildMailFileData           (void);
static ascii *Smtp_BuildMailDeviceId           (void);
static ascii *Smtp_BuildMailImei               (void);

/* code file */
static u32    Smtp_CodeFileBase64(u8 *ptr_file, u32 file_size, ascii *ptr_file_out);
#endif


/*-----------------------------------------------------------------------------
 * get/set configurations
 *-----------------------------------------------------------------------------*/
/* get/set "SMTP server"           configuration */
       void   Smtp_Config_SmtpServer_GetDefault         (SMTP__CONFIG__SMTP_SERVER           *ptr_data);
       void   Smtp_Config_SmtpServer_Get                (SMTP__CONFIG__SMTP_SERVER           *ptr_data);
       void   Smtp_Config_SmtpServer_Set                (SMTP__CONFIG__SMTP_SERVER           *ptr_data);
       bool   Smtp_Config_SmtpServer_IsValid            (SMTP__CONFIG__SMTP_SERVER           *ptr_data);

/* get/set "mail sender"           configuration */
       void   Smtp_Config_MailSender_GetDefault         (SMTP__CONFIG__MAIL_SENDER           *ptr_data);
       void   Smtp_Config_MailSender_Get                (SMTP__CONFIG__MAIL_SENDER           *ptr_data);
       void   Smtp_Config_MailSender_Set                (SMTP__CONFIG__MAIL_SENDER           *ptr_data);
       bool   Smtp_Config_MailSender_IsValid            (SMTP__CONFIG__MAIL_SENDER           *ptr_data);

/* get/set "mail recipient To"     configuration */
       void   Smtp_Config_MailRecipientTo_GetDefault    (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);
       void   Smtp_Config_MailRecipientTo_Get           (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);
       void   Smtp_Config_MailRecipientTo_Set           (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);
       bool   Smtp_Config_MailRecipientTo_IsValid       (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);

/* get/set "mail recipient Cc"     configuration */
       void   Smtp_Config_MailRecipientCc_GetDefault    (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);
       void   Smtp_Config_MailRecipientCc_Get           (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);
       void   Smtp_Config_MailRecipientCc_Set           (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);
       bool   Smtp_Config_MailRecipientCc_IsValid       (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);

/* get/set "mail recipient Bcc"    configuration */
       void   Smtp_Config_MailRecipientBcc_GetDefault   (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);
       void   Smtp_Config_MailRecipientBcc_Get          (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);
       void   Smtp_Config_MailRecipientBcc_Set          (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);
       bool   Smtp_Config_MailRecipientBcc_IsValid      (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);

/* get/set "mail recipient backup" configuration */
       void   Smtp_Config_MailRecipientBackup_GetDefault(SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);
       void   Smtp_Config_MailRecipientBackup_Get       (SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);
       void   Smtp_Config_MailRecipientBackup_Set       (SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);
       bool   Smtp_Config_MailRecipientBackup_IsValid   (SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* SMTP callback functions */
#ifdef FUNCTION_IMPLEMENTED
static void   Smtp_WipCallback_Smtp_ChannelSession     (wip_event_t *ev, void *ctx);
static void   Smtp_WipCallback_Smtp_ChannelDataTransfer(wip_event_t *ev, void *ctx);
static void   Smtp_WipCallback_Smtp_ChannelFinalize    (                 void *ctx);
#endif




#ifdef FUNCTION_IMPLEMENTED
/*===========================================================================
 * Function    : Smtp_ClientSmtpRequest
 *
 * Description : require to BOOT task to do a SMTP
 * Input       : - file_type : file type (LOG or ALARM)
 *               - file_name : file name
 *               - v         : file version
 *               - n         : file number
 *               - start_time: start time
 *               - stop_time : stop  time
 *               - alarm_type: alarm type ID
 *               - alarm_par : alarm parameter
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Smtp_ClientSmtpRequest(u8 file_type, ascii *file_name, u8 v, u32 n, CLOCK__TIME start_time, CLOCK__TIME stop_time, u8 alarm_type, u16 alarm_par)
{
    bool result;


    if (
         (file_type != SMTP__FILE_TYPE__LOG  ) &&
         (file_type != SMTP__FILE_TYPE__ALARM)
       )
    {
        return FALSE;
    }


    /* save the "file in transmission" info */
    Smtp_FileTx_FileType    = file_type;
    strncpy(Smtp_FileTx_FileName, file_name, 68);
    Smtp_FileTx_FileName[68] = 0x00;
    Smtp_FileTx_V           = v;
    Smtp_FileTx_N           = n;
    Smtp_FileTx_StartTime   = start_time;
    Smtp_FileTx_StopTime    = stop_time;
    Smtp_FileTx_AlarmType   = alarm_type;
    Smtp_FileTx_AlarmPar    = alarm_par;


    result = Boot_ClientSmtpRequest();


    return result;
}




/*=============================================================================
 * Function   : Smtp_ClientSmtp
 *
 * Description: try to start a connection with the configured SMTP server
 * Input      : -
 * Output     : - FALSE: connection with SMTP server not started
 *              - TRUE : connection with SMTP server     started (not yet connected)
 *=============================================================================*/
bool Smtp_ClientSmtp(void)
{
    bool result;


    result = Smtp_ClientSmtpOpenChannel_Session();

    return result;
}




/*=============================================================================
 * Function   : Smtp_ClientSmtpOpenChannel_Session
 *
 * Description: try to start a connection with the configured SMTP server
 * Input      : -
 * Output     : - FALSE: connection with SMTP server not started
 *              - TRUE : connection with SMTP server     started (not yet connected)
 *=============================================================================*/
static bool Smtp_ClientSmtpOpenChannel_Session(void)
{
    wip_smtpClientAuthTypes_e smtp_auth_type;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "Starting connection with SMTP server...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "SMTP Host name          : \"%s\"", Smtp_Config_SmtpServer.smtp_hostname );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "SMTP port               : %d"    , Smtp_Config_SmtpServer.smtp_port     );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "SMTP user               : \"%s\"", Smtp_Config_SmtpServer.smtp_username );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "SMTP password           : \"%s\"", Smtp_Config_SmtpServer.smtp_password );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "SMTP authentication type: %d"    , Smtp_Config_SmtpServer.smtp_auth_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* SMTP authentication type */
    if      (Smtp_Config_SmtpServer.smtp_auth_type == SMTP__SMTP_AUTH_TYPE__NONE  )
        smtp_auth_type = WIP_SMTP_AUTH_NONE;
    else if (Smtp_Config_SmtpServer.smtp_auth_type == SMTP__SMTP_AUTH_TYPE__CLEAR )
        smtp_auth_type = WIP_SMTP_AUTH_CLEAR;
    else if (Smtp_Config_SmtpServer.smtp_auth_type == SMTP__SMTP_AUTH_TYPE__MIME64)
        smtp_auth_type = WIP_SMTP_AUTH_MIME64;
    else
        smtp_auth_type = WIP_SMTP_AUTH_NONE;


    /* create a SMTP session channel */
    Smtp_SmtpHandler_ChannelConnection = wip_SMTPClientCreateOpts(Smtp_Config_SmtpServer.smtp_hostname,                            // SMTP server (hostname)
                                                                  Smtp_WipCallback_Smtp_ChannelSession,                            // callback handler which receives the network events related to the channel
                                                                  NULL,                                                            // context (not used)

                                                                  /* options */
                                                                  WIP_COPT_PEER_PORT,      Smtp_Config_SmtpServer.smtp_port,       // port number of the SMTP mail server (default 25)

                                                                  WIP_COPT_USER,           Smtp_Config_SmtpServer.smtp_username,   // username (default "NULL")
                                                                  WIP_COPT_PASSWORD,       Smtp_Config_SmtpServer.smtp_password,   // password (default "NULL")
                                                                  WIP_COPT_SMTP_AUTH_TYPE, smtp_auth_type,                         // authentication type used for authentication

                                                                  WIP_COPT_FINALIZER,      Smtp_WipCallback_Smtp_ChannelFinalize,  // callback function called after the channel has been completely closed

                                                                  WIP_COPT_END);
    if (Smtp_SmtpHandler_ChannelConnection == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "wip_SMTPClientCreateOpts - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_SMTPClientCreateOpts - OK - %lX", Smtp_SmtpHandler_ChannelConnection);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    if (Smtp_SmtpHandler_ChannelConnection == NULL)
        return FALSE;


    return TRUE;
}




/*=============================================================================
 * Function   : Smtp_ClientSmtpOpenChannel_Data
 *
 * Description: try to open the data channel with the SMTP server to send the mail
 * Input      : -
 * Output     : - FALSE: data channel not open
 *              - TRUE : data channel     open (mail not yet sent)
 *=============================================================================*/
static bool Smtp_ClientSmtpOpenChannel_Data(void)
{
    static ascii         copt_smpt_sender     [    (SMTP__MAX_LENGTH_MAIL_SENDER         )     + 1];
    static ascii         copt_smpt_sender_name[    (60                                   )     + 1];

    static ascii         copt_smpt_rec        [2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO  + 2) + 1 + 1];
    static ascii         copt_smpt_cc_rec     [2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC  + 2) + 1 + 1];
    static ascii         copt_smpt_bcc_rec    [2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC + 2) + 1 + 1];

           ascii        *ptr_mail_sender;
           ascii        *ptr_mail_sender_name;

           ascii        *ptr_mail_rec_to;
           ascii        *ptr_mail_rec_cc;
           ascii        *ptr_mail_rec_bcc;

           wip_channel_t channel_copy;

           int           result_int;
           u8            i;


    /* build mail recipient strings */
    ptr_mail_rec_to  = Smtp_BuildMailRecTo (TRUE);
    ptr_mail_rec_cc  = Smtp_BuildMailRecCc (TRUE);
    ptr_mail_rec_bcc = Smtp_BuildMailRecBcc(TRUE);

    /* build mail sender    strings */
    ptr_mail_sender      = Smtp_BuildMailSender    (FALSE);
    ptr_mail_sender_name = Smtp_BuildMailSenderName(TRUE );


    strncpy(copt_smpt_rec        , ptr_mail_rec_to     , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO  + 2) + 1 + 1));
    strncpy(copt_smpt_cc_rec     , ptr_mail_rec_cc     , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC  + 2) + 1 + 1));
    strncpy(copt_smpt_bcc_rec    , ptr_mail_rec_bcc    , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC + 2) + 1 + 1));

    strncpy(copt_smpt_sender     , ptr_mail_sender     , (    (SMTP__MAX_LENGTH_MAIL_SENDER         )     + 1));
    strncpy(copt_smpt_sender_name, ptr_mail_sender_name, (    (60                                   )     + 1));


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "Opening data channel with SMTP server...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "copt_smpt_sender     : \"%s\""      , copt_smpt_sender     );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "copt_smpt_sender_name: \"%s\""      , copt_smpt_sender_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "copt_smpt_rec        : \"%s\"", copt_smpt_rec    );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "copt_smpt_cc_rec     : \"%s\"", copt_smpt_cc_rec );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "copt_smpt_bcc_rec    : \"%s\"", copt_smpt_bcc_rec);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* data channel creation */
    Smtp_SmtpHandler_ChannelDataTransfer = wip_putFileOpts(Smtp_SmtpHandler_ChannelConnection,                   // Session channel
                                                           NULL,                                                 // file name (not used)
                                                           Smtp_WipCallback_Smtp_ChannelDataTransfer,            // data callback handler
                                                           NULL,                                                 // context   (not used)

                                                           /* options */

                                                           WIP_COPT_SMTP_SENDER,        copt_smpt_sender,        // sender Email address                                (mandatory option) (default: 0)
                                                           WIP_COPT_SMTP_SENDERNAME,    copt_smpt_sender_name,   // sender name                                                            (default: 0)

                                                           WIP_COPT_SMTP_REC,           copt_smpt_rec,           //                   Recipients addresses list pointer (mandatory option) (default: 0)
                                                           WIP_COPT_SMTP_CC_REC,        copt_smpt_cc_rec ,       //       Carbon Copy Recipients addresses list pointer (mandatory option) (default: 0)
                                                           WIP_COPT_SMTP_BCC_REC,       copt_smpt_bcc_rec,       // Blind Carbon Copy Recipients addresses list pointer (mandatory option) (default: 0)

                                                         //WIP_COPT_SMTP_SUBJ,          "Mail subject",          // subject of the mail (not mandatory)
                                                           WIP_COPT_SMTP_FORMAT_HEADER, 0,                       // mail header information will be generated and formatted by application

                                                           WIP_COPT_END);
    if (Smtp_SmtpHandler_ChannelDataTransfer == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "wip_putFileOpts - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_putFileOpts - OK - %lX", Smtp_SmtpHandler_ChannelDataTransfer);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    if (Smtp_SmtpHandler_ChannelDataTransfer == NULL)
    {
        channel_copy = Smtp_SmtpHandler_ChannelConnection;
        result_int = wip_close(Smtp_SmtpHandler_ChannelConnection);
        if (result_int != 0)
        {
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        return FALSE;
    }


    /*
     * From this point a WIP_CEV_WRITE will be notify,
     * to signal to the application that it can start wip_write()
     */

     return TRUE;
}




/*=============================================================================
 * Function   : Smtp_BuildMail
 *
 * Description: build the mail text string
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Smtp_BuildMail(void)
{
    ascii *ptr_mail_device_id;
    ascii *ptr_mail_imei;
    ascii *ptr_mail_subject;
    ascii *ptr_mail_sender;
    ascii *ptr_mail_rec_to;
    ascii *ptr_mail_rec_cc;
    ascii *ptr_mail_rec_bcc;
    ascii *ptr_mail_body;
    ascii *ptr_mail_file_name;
    ascii *ptr_mail_file_data;

    u32    len_mail_device_id;
    u32    len_mail_imei;
    u32    len_mail_subject;
    u32    len_mail_sender;
    u32    len_mail_rec_to;
    u32    len_mail_rec_cc;
    u32    len_mail_rec_bcc;
    u32    len_mail_body;
    u32    len_mail_file_name;
    u32    len_mail_file_data;

    u32    len_string;


    /* build mail strings */
    ptr_mail_device_id = Smtp_BuildMailDeviceId();
    ptr_mail_imei      = Smtp_BuildMailImei();
    ptr_mail_subject   = Smtp_BuildMailSubject();
    ptr_mail_sender    = Smtp_BuildMailSenderAndSenderName(TRUE, TRUE);
    ptr_mail_rec_to    = Smtp_BuildMailRecTo (TRUE);
    ptr_mail_rec_cc    = Smtp_BuildMailRecCc (TRUE);
    ptr_mail_rec_bcc   = Smtp_BuildMailRecBcc(TRUE);
    ptr_mail_body      = Smtp_BuildMailBody();
    ptr_mail_file_name = Smtp_BuildMailFileName();
    ptr_mail_file_data = Smtp_BuildMailFileData();


    len_mail_device_id = strlen(ptr_mail_device_id);
    len_mail_imei      = strlen(ptr_mail_imei     );
    len_mail_subject   = strlen(ptr_mail_subject  );
    len_mail_sender    = strlen(ptr_mail_sender   );
    len_mail_rec_to    = strlen(ptr_mail_rec_to   );
    len_mail_rec_cc    = strlen(ptr_mail_rec_cc   );
    len_mail_rec_bcc   = strlen(ptr_mail_rec_bcc  );
    len_mail_body      = strlen(ptr_mail_body     );
    len_mail_file_name = strlen(ptr_mail_file_name);
    len_mail_file_data = strlen(ptr_mail_file_data);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_device_id: %ld", len_mail_device_id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_imei     : %ld", len_mail_imei);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_subject  : %ld", len_mail_subject);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_sender   : %ld", len_mail_sender);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_rec_to   : %ld", len_mail_rec_to);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_rec_cc   : %ld", len_mail_rec_cc);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_rec_bcc  : %ld", len_mail_rec_bcc);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_body     : %ld", len_mail_body);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_file_name: %ld", len_mail_file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "len_mail_file_data: %ld", len_mail_file_data);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* init string (empty) */
    Smtp_MailString[0] = '\0';



    /****** Application pre formatted header ******/

    /* "Device-ID" */
    strncat(Smtp_MailString, "Device-ID: "                                                , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_device_id                                           , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "IMEI" */
    strncat(Smtp_MailString, "IMEI: "                                                     , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_imei                                                , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "Subject" */
    strncat(Smtp_MailString, "Subject: "                                                  , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_subject                                             , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "From" */
    strncat(Smtp_MailString, "From: "                                                     , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_sender                                              , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "To" */
    strncat(Smtp_MailString, "To: "                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_rec_to                                              , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "Cc" */
    strncat(Smtp_MailString, "Cc: "                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_rec_cc                                              , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "Bcc" */
    strncat(Smtp_MailString, "Bcc: "                                                      , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_rec_bcc                                             , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "MIME-Version" */
    strncat(Smtp_MailString, "MIME-Version: "                                             , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "1.0"                                                        , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /* "Content-Type" */
    strncat(Smtp_MailString, "Content-Type: "                                             , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "multipart/mixed; "                                          , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    strncat(Smtp_MailString, "    boundary=\"__frontier__gc0p4Jq0M2Yt08jU534c0p\"\r\n"    , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);    /* This ends the application pre formatted header */


    /****** Message to users of old non-MIME clients ******/
    strncat(Smtp_MailString, "This is a multi-part message in MIME format.\r\n"           , MAX_LENGTH_MAIL_STRING);    /* This text is ignored */


    /****** Start of message text section ******/

    strncat(Smtp_MailString, "--__frontier__gc0p4Jq0M2Yt08jU534c0p\r\n"                   , MAX_LENGTH_MAIL_STRING);

    /*** Content header ***/
    strncat(Smtp_MailString, "Content-Type: text/plain\r\n"                               , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "    charset=\"us-ascii\"\r\n"                               , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "Content-Transfer-Encoding: quoted-printable\r\n"            , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /*** Body ***/
    strncat(Smtp_MailString, ptr_mail_body                                                , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);


    /****** Start of attachment section ******/

    strncat(Smtp_MailString, "--__frontier__gc0p4Jq0M2Yt08jU534c0p\r\n"                   , MAX_LENGTH_MAIL_STRING);

    /*** Content header ***/
    strncat(Smtp_MailString, "Content-Type: text/plain;\r\n"                              , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "         name="                                             , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_file_name                                           , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "Content-Transfer-Encoding: base64\r\n"                      , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "Content-Description: Attached csv file encrypted mime64\r\n", MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "Content-Disposition: attachment;"                           , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, " filename="                                                 , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, ptr_mail_file_name                                           , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, " "                                                          , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);

    /*** Body (encrypted attachment block) ***/
    strncat(Smtp_MailString, ptr_mail_file_data                                           , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);
    strncat(Smtp_MailString, "\r\n"                                                       , MAX_LENGTH_MAIL_STRING);


    /****** End of last section ******/
    strncat(Smtp_MailString, "--__frontier__gc0p4Jq0M2Yt08jU534c0p--\r\n"                 , MAX_LENGTH_MAIL_STRING);


    Smtp_MailString[MAX_LENGTH_MAIL_STRING] = 0x00;


    /* end of string */
    len_string = (u32)strlen(Smtp_MailString);
    Smtp_MailString[len_string] = '\0';


    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "Length of mail string: %ld", len_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}




/*===========================================================================
 * Function   : Smtp_BuildMailRecTo
 *
 * Description: build the mail recipient To string
 * Input      : - angle_brackets: indication for using angle brackets
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailRecTo(bool angle_brackets)
{
    static ascii mail_rec_to[2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO + 2) + 1 + 1];

           u8    len_to[SMTP__NUM_OF_MAIL_ADDRESSES_TO];

           bool  rec_added_to;
           int   result_int;
           u8    i;


    rec_added_to = FALSE;

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_TO; i++)
        len_to[i] = (u8)strlen(Smtp_Config_MailRecipientTo.mail_address_to[i]);


    /* init string (empty) */
    mail_rec_to[0] = 0x00;

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_TO;  i++)
    {
        if (len_to[i] > 0)
        {
            if (rec_added_to)
                strncat(mail_rec_to, ","                                           , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO + 2) + 1));

            if (angle_brackets)
                strncat(mail_rec_to, "<"                                           , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO + 2) + 1));

            strncat    (mail_rec_to, Smtp_Config_MailRecipientTo.mail_address_to[i], (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO + 2) + 1));

            if (angle_brackets)
                strncat(mail_rec_to, ">"                                           , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO + 2) + 1));

            rec_added_to  = TRUE;
        }
    }

    mail_rec_to[2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_TO + 2) + 1] = 0x00;


    return mail_rec_to;
}




/*===========================================================================
 * Function   : Smtp_BuildMailRecCc
 *
 * Description: build the mail recipient Cc string
 * Input      : - angle_brackets: indication for using angle brackets
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailRecCc(bool angle_brackets)
{
    static ascii mail_rec_cc[2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC + 2) + 1 + 1];

           u8    len_cc[SMTP__NUM_OF_MAIL_ADDRESSES_CC];

           bool  rec_added_cc;
           int   result_int;
           u8    i;


    rec_added_cc  = FALSE;

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_CC; i++)
        len_cc[i] = (u8)strlen(Smtp_Config_MailRecipientCc.mail_address_cc[i]);


    /* init string (empty) */
    mail_rec_cc[0] = 0x00;

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_CC;  i++)
    {
        if (len_cc[i] > 0)
        {
            if (rec_added_cc)
                strncat(mail_rec_cc, ","                                           , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC + 2) + 1));

            if (angle_brackets)
                strncat(mail_rec_cc, "<"                                           , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC + 2) + 1));

            strncat    (mail_rec_cc, Smtp_Config_MailRecipientCc.mail_address_cc[i], (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC + 2) + 1));

            if (angle_brackets)
                strncat(mail_rec_cc, ">"                                           , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC + 2) + 1));

            rec_added_cc  = TRUE;
        }
    }

    mail_rec_cc[2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_CC  + 2) + 1] = 0x00;


    return mail_rec_cc;
}




/*===========================================================================
 * Function   : Smtp_BuildMailRecBcc
 *
 * Description: build the mail recipient Bcc string
 * Input      : - angle_brackets: indication for using angle brackets
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailRecBcc(bool angle_brackets)
{
    static ascii mail_rec_bcc[2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                              2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1];

           u8    len_bcc   [SMTP__NUM_OF_MAIL_ADDRESSES_BCC   ];
           u8    len_backup[SMTP__NUM_OF_MAIL_ADDRESSES_BACKUP];

           bool  rec_added_bcc;
           int   result_int;
           u8    i;


    rec_added_bcc = FALSE;

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_BCC;    i++)
        len_bcc   [i] = (u8)strlen(Smtp_Config_MailRecipientBcc.mail_address_bcc      [i]);

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_BACKUP; i++)
        len_backup[i] = (u8)strlen(Smtp_Config_MailRecipientBackup.mail_address_backup[i]);


    /* init string (empty) */
    mail_rec_bcc[0] = 0x00;

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_BCC; i++)
    {
        if (len_bcc[i] > 0)
        {
            if (rec_added_bcc)
                strncat(mail_rec_bcc, ","                                                   , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));

            if (angle_brackets)
            strncat(mail_rec_bcc    , "<"                                                   , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));
                strncat(mail_rec_bcc, Smtp_Config_MailRecipientBcc.mail_address_bcc[i]      , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));
            if (angle_brackets)
                strncat(mail_rec_bcc, ">"                                                   , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));

            rec_added_bcc = TRUE;
        }
    }

    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_BACKUP; i++)
    {
        if (len_backup[i] > 0)
        {
            if (rec_added_bcc)
                strncat(mail_rec_bcc, ","                                                   , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));

            if (angle_brackets)
                strncat(mail_rec_bcc, "<"                                                   , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));
            strncat(mail_rec_bcc    , Smtp_Config_MailRecipientBackup.mail_address_backup[i], (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));
            if (angle_brackets)
                strncat(mail_rec_bcc, ">"                                                   , (2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                                                                                                  2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1));
            rec_added_bcc = TRUE;
        }
    }

    mail_rec_bcc[2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 2) + 1 +
                 2 * (SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 2) + 1 + 1] = 0x00;;


    return mail_rec_bcc;
}




/*===========================================================================
 * Function   : Smtp_BuildMailSender
 *
 * Description: build the mail sender string
 * Input      : - angle_brackets: indication for using angle brackets
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailSender(bool angle_brackets)
{
    static ascii mail_sender[SMTP__MAX_LENGTH_MAIL_SENDER + 1];

           u8    len;


    len = (u8)strlen(Smtp_Config_MailSender.mail_sender);


    /* init string (empty) */
    mail_sender[0] = 0x00;

    if (len > 0)
    {
        if (angle_brackets)
            strncat(mail_sender, "<"                               , SMTP__MAX_LENGTH_MAIL_SENDER);

        strncat    (mail_sender, Smtp_Config_MailSender.mail_sender, SMTP__MAX_LENGTH_MAIL_SENDER);

        if (angle_brackets)
            strncat(mail_sender, ">"                               , SMTP__MAX_LENGTH_MAIL_SENDER);
    }

    mail_sender[SMTP__MAX_LENGTH_MAIL_SENDER] = 0x00;


    return mail_sender;
}




/*===========================================================================
 * Function   : Smtp_BuildMailSenderName
 *
 * Description: build the mail sender name string
 * Input      : - angle_brackets: indication for using angle brackets
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailSenderName(bool angle_brackets)
{
    static ascii  mail_sender_name[60 + 1];

           ascii *ptr_id;

           u8    len;


    // From: "EASY - ID: 00000001234"

    ptr_id = Smtp_BuildMailDeviceId();

    len = (u8)strlen(ptr_id);


    /* init string (empty) */
    mail_sender_name[0] = 0x00;

    strncat        (mail_sender_name, "EASY"   , 60);

    strncat        (mail_sender_name, " - ID: ", 60);

    if (len > 0)
    {
        if (angle_brackets)
            strncat(mail_sender_name, "<"      , 60);

        strncat    (mail_sender_name, ptr_id   , 60);

        if (angle_brackets)
            strncat(mail_sender_name, ">"      , 60);
    }

    mail_sender_name[60] = 0x00;


    return mail_sender_name;
}




/*===========================================================================
 * Function   : Smtp_BuildMailSenderAndSenderName
 *
 * Description: build the mail sender and mail sender name string
 * Input      : - angle_brackets : indication for using angle brackets
 *              - quotation_marks: indication for using quotation marks
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailSenderAndSenderName(bool angle_brackets, bool quotation_marks)
{
    static ascii  mail_sender[120 + 1];

           ascii *ptr_id;

           u8     len_1;
           u8     len_2;


    // From: "EASY - ID: 00000001234"

    ptr_id = Smtp_BuildMailDeviceId();


    len_1 = (u8)strlen(Smtp_Config_MailSender.mail_sender);
    len_2 = (u8)strlen(ptr_id                            );


    /* init string (empty) */
    mail_sender[0] = 0x00;

    if (quotation_marks)
        strncat    (mail_sender, "\""                              , 120);

    strncat        (mail_sender, "EASY"                            , 120);

    strncat        (mail_sender, " - ID: "                         , 120);

    if (len_1 > 0)
    {
        strncat    (mail_sender, ptr_id                            , 120);
    }

    if (quotation_marks)
        strncat    (mail_sender, "\""                              , 120);

    if (len_2 > 0)
    {
        strncat    (mail_sender, " "                               , 120);

        if (angle_brackets)
            strncat(mail_sender, "<"                               , 120);

        strncat    (mail_sender, Smtp_Config_MailSender.mail_sender, 120);

        if (angle_brackets)
            strncat(mail_sender, ">"                               , 120);
    }

    mail_sender[120] = 0x00;


    return mail_sender;
}




/*===========================================================================
 * Function   : Smtp_BuildMailSubject
 *
 * Description: build the mail subject string
 * Input      : -
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailSubject(void)
{
    static ascii  mail_subject     [120 + 1];

    static ascii  string_v         [  2 + 1];
    static ascii  string_id        [ 20 + 1];
    static ascii  string_n         [  6 + 1];
    static ascii  string_start_time[ 19 + 1];
    static ascii  string_stop_time [ 19 + 1];
    static ascii  string_alarm_type[ 25 + 1];

           ascii *ptr_id;
           ascii *string_alarm_descr;

           bool   result;


    // EASY - LOG - V: 01 - ID: 000000012345 - N: 000000 - Start: 2015/05/06 04:00:00 - Stop: 2015/05/06 08:00:00
    // EASY - ALARM - V: 01 - ID: 000000012345 - N: 000000 - Time: 2015/05/06 04:00:00 - Alarm: TMIN_RESTORED

    ptr_id = Smtp_BuildMailDeviceId();

    result = Clock_IsValidClockTime(&Smtp_FileTx_StartTime);
    if (!result)
    {
        Smtp_FileTx_StartTime.year     = 2000;
        Smtp_FileTx_StartTime.month    = 1;
        Smtp_FileTx_StartTime.day      = 1;
        Smtp_FileTx_StartTime.hour     = 0;
        Smtp_FileTx_StartTime.minute   = 0;
        Smtp_FileTx_StartTime.second   = 0;
        Smtp_FileTx_StartTime.week_day = 6;  // Saturday
    }

    result = Clock_IsValidClockTime(&Smtp_FileTx_StopTime );
    if (!result)
    {
        Smtp_FileTx_StopTime.year      = 2000;
        Smtp_FileTx_StopTime.month     = 1;
        Smtp_FileTx_StopTime.day       = 1;
        Smtp_FileTx_StopTime.hour      = 0;
        Smtp_FileTx_StopTime.minute    = 0;
        Smtp_FileTx_StopTime.second    = 0;
        Smtp_FileTx_StopTime.week_day  = 6;  // Saturday
    }

    string_alarm_descr = Alarm_AlarmShortString(Smtp_FileTx_AlarmType);

    sprintf(string_v         , "%02d"                         , Smtp_FileTx_V);
    sprintf(string_id        , "%s"                           , ptr_id       );
    sprintf(string_n         , "%06ld"                        , Smtp_FileTx_N);
    sprintf(string_start_time, "%04d/%02d/%02d %02d:%02d:%02d", Smtp_FileTx_StartTime.year, Smtp_FileTx_StartTime.month, Smtp_FileTx_StartTime.day, Smtp_FileTx_StartTime.hour, Smtp_FileTx_StartTime.minute, Smtp_FileTx_StartTime.second);
    sprintf(string_stop_time , "%04d/%02d/%02d %02d:%02d:%02d", Smtp_FileTx_StopTime.year , Smtp_FileTx_StopTime.month , Smtp_FileTx_StopTime.day , Smtp_FileTx_StopTime.hour , Smtp_FileTx_StopTime.minute , Smtp_FileTx_StopTime.second );
    sprintf(string_alarm_type, "%s"                           , string_alarm_descr);


    /* init string (empty) */
    mail_subject[0] = '\0';

    if (Smtp_FileTx_FileType == SMTP__FILE_TYPE__LOG)
    {
        // EASY - LOG - V: 01 - ID: 000000012345 - N: 000000 - Start: 2015/05/06 04:00:00 - Stop: 2015/05/06 08:00:00

        strncat(mail_subject, "EASY"           , 120);

        strncat(mail_subject, " - LOG"         , 120);

        strncat(mail_subject, " - V: "         , 120);
        strncat(mail_subject, string_v         , 120);

        strncat(mail_subject, " - ID: "        , 120);
        strncat(mail_subject, string_id        , 120);

        strncat(mail_subject, " - N: "         , 120);
        strncat(mail_subject, string_n         , 120);

        strncat(mail_subject, " - Start: "     , 120);
        strncat(mail_subject, string_start_time, 120);

        strncat(mail_subject, " - Stop: "      , 120);
        strncat(mail_subject, string_stop_time , 120);
    }
    else
    {
        // EASY - ALARM - V: 01 - ID: 000000012345 - N: 000000 - Time: 2015/05/06 04:00:00 - Alarm: TMIN_RESTORED

        strncat(mail_subject, "EASY"           , 120);

        strncat(mail_subject, " - ALARM"       , 120);

        strncat(mail_subject, " - V: "         , 120);
        strncat(mail_subject, string_v         , 120);

        strncat(mail_subject, " - ID: "        , 120);
        strncat(mail_subject, string_id        , 120);

        strncat(mail_subject, " - N: "         , 120);
        strncat(mail_subject, string_n         , 120);

        strncat(mail_subject, " - Time: "      , 120);
        strncat(mail_subject, string_start_time, 120);

        strncat(mail_subject, " - Alarm: "     , 120);
        strncat(mail_subject, string_alarm_type, 120);
    }

    mail_subject[120] = '\0';


    return mail_subject;
}




/*===========================================================================
 * Function   : Smtp_BuildMailBody
 *
 * Description: build the mail body string
 * Input      : -
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *Smtp_BuildMailBody(void)
{
    static ascii  mail_body        [250 + 1];

    static ascii  string_v         [  2 + 1];
    static ascii  string_id        [ 20 + 1];
    static ascii  string_n         [  6 + 1];
    static ascii  string_start_time[ 19 + 1];
    static ascii  string_stop_time [ 19 + 1];
    static ascii  string_alarm_type[ 25 + 1];

           ascii *ptr_id;
           ascii *string_alarm_descr;

           bool   result;


    // In attachment data from EASY device.
    // - Data type: LOG
    // - Version: 01
    // - Device ID: 000000012345
    // - N: 000001
    // - Data Start: 2015/05/06 04:00:00
    // - Data Stop: 2015/05/06 08:00:00

    // In attachment data from EASY device.
    // - Data type: ALARM
    // - Version: 01
    // - Device ID: 000000012345
    // - N: 000001
    // - Alarm Time: 2015/05/06 04:00:00
    // - Alarm Type: TMIN_RESTORED


    ptr_id = Smtp_BuildMailDeviceId();

    result = Clock_IsValidClockTime(&Smtp_FileTx_StartTime);
    if (!result)
    {
        Smtp_FileTx_StartTime.year     = 2000;
        Smtp_FileTx_StartTime.month    = 1;
        Smtp_FileTx_StartTime.day      = 1;
        Smtp_FileTx_StartTime.hour     = 0;
        Smtp_FileTx_StartTime.minute   = 0;
        Smtp_FileTx_StartTime.second   = 0;
        Smtp_FileTx_StartTime.week_day = 6;  // Saturday
    }

    result = Clock_IsValidClockTime(&Smtp_FileTx_StopTime );
    if (!result)
    {
        Smtp_FileTx_StopTime.year      = 2000;
        Smtp_FileTx_StopTime.month     = 1;
        Smtp_FileTx_StopTime.day       = 1;
        Smtp_FileTx_StopTime.hour      = 0;
        Smtp_FileTx_StopTime.minute    = 0;
        Smtp_FileTx_StopTime.second    = 0;
        Smtp_FileTx_StopTime.week_day  = 6;  // Saturday
    }

    string_alarm_descr = Alarm_AlarmShortString(Smtp_FileTx_AlarmType);

    sprintf(string_v         , "%02d"                         , Smtp_FileTx_V);
    sprintf(string_id        , "%s"                           , ptr_id       );
    sprintf(string_n         , "%06ld"                        , Smtp_FileTx_N);
    sprintf(string_start_time, "%04d/%02d/%02d %02d:%02d:%02d", Smtp_FileTx_StartTime.year, Smtp_FileTx_StartTime.month, Smtp_FileTx_StartTime.day, Smtp_FileTx_StartTime.hour, Smtp_FileTx_StartTime.minute, Smtp_FileTx_StartTime.second);
    sprintf(string_stop_time , "%04d/%02d/%02d %02d:%02d:%02d", Smtp_FileTx_StopTime.year , Smtp_FileTx_StopTime.month , Smtp_FileTx_StopTime.day , Smtp_FileTx_StopTime.hour , Smtp_FileTx_StopTime.minute , Smtp_FileTx_StopTime.second );
    sprintf(string_alarm_type, "%s"                           , string_alarm_descr);


    /* init string (empty) */
    mail_body[0] = '\0';

    if (Smtp_FileTx_FileType == SMTP__FILE_TYPE__LOG)
    {
        // In attachment data from EASY device.
        // - Data type: LOG
        // - Version: 01
        // - Device ID: 000000012345
        // - N: 000001
        // - Data Start: 2015/05/06 04:00:00
        // - Data Stop: 2015/05/06 08:00:00

        strncat(mail_body, "In attachment data from EASY device.", 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Data type: "                       , 250);
        strncat(mail_body, "LOG"                                 , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Version: "                         , 250);
        strncat(mail_body, string_v                              , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Device ID: "                       , 250);
        strncat(mail_body, string_id                             , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- N: "                               , 250);
        strncat(mail_body, string_n                              , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Start: "                           , 250);
        strncat(mail_body, string_start_time                     , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Stop: "                            , 250);
        strncat(mail_body, string_stop_time                      , 250);
        strncat(mail_body, "\r\n"                                , 250);
    }
    else
    {
        // In attachment data from EASY device.
        // - Data type: ALARM
        // - Version: 01
        // - Device ID: 000000012345
        // - N: 000001
        // - Alarm Time: 2015/05/06 04:00:00
        // - Alarm Type: TMIN_RESTORED

        strncat(mail_body, "In attachment data from EASY device.", 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Data type: "                       , 250);
        strncat(mail_body, "ALARM"                               , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Version: "                         , 250);
        strncat(mail_body, string_v                              , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Device ID: "                       , 250);
        strncat(mail_body, string_id                             , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- N: "                               , 250);
        strncat(mail_body, string_n                              , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Alarm Time: "                      , 250);
        strncat(mail_body, string_start_time                     , 250);
        strncat(mail_body, "\r\n"                                , 250);

        strncat(mail_body, "- Alarm Type: "                      , 250);
        strncat(mail_body, string_alarm_type                     , 250);
        strncat(mail_body, "\r\n"                                , 250);
    }

    mail_body[250] = '\0';


    return mail_body;
}




/*=============================================================================
 * Function   : Smtp_BuildMailFileName
 *
 * Description: build the mail file name string
 * Input      : -
 * Output     : - pointer to save the string built
 *=============================================================================*/
static ascii *Smtp_BuildMailFileName(void)
{
    static ascii  mail_file_name   [120 + 1];

    static ascii  string_v         [  2 + 1];
    static ascii  string_id        [ 20 + 1];
    static ascii  string_n         [  6 + 1];
    static ascii  string_start_time[ 19 + 1];
    static ascii  string_stop_time [ 19 + 1];
    static ascii  string_alarm_type[ 25 + 1];

           ascii *ptr_id;
           ascii *string_alarm_descr;

           bool   result;


    // "EASY__LOG__V_00__ID_000000012345__N_000000__Start_2015.05.06_04.00.00__Stop_2015.05.06_08.00.00.csv"
    // "EASY__ALARM__V_00__ID_000000012345__N_000000__Time_2015.05.06_04.00.00__Alarm_TMIN_RESTORED.csv"

    ptr_id = Smtp_BuildMailDeviceId();

    result = Clock_IsValidClockTime(&Smtp_FileTx_StartTime);
    if (!result)
    {
        Smtp_FileTx_StartTime.year     = 2000;
        Smtp_FileTx_StartTime.month    = 1;
        Smtp_FileTx_StartTime.day      = 1;
        Smtp_FileTx_StartTime.hour     = 0;
        Smtp_FileTx_StartTime.minute   = 0;
        Smtp_FileTx_StartTime.second   = 0;
        Smtp_FileTx_StartTime.week_day = 6;  // Saturday
    }

    result = Clock_IsValidClockTime(&Smtp_FileTx_StopTime );
    if (!result)
    {
        Smtp_FileTx_StopTime.year      = 2000;
        Smtp_FileTx_StopTime.month     = 1;
        Smtp_FileTx_StopTime.day       = 1;
        Smtp_FileTx_StopTime.hour      = 0;
        Smtp_FileTx_StopTime.minute    = 0;
        Smtp_FileTx_StopTime.second    = 0;
        Smtp_FileTx_StopTime.week_day  = 6;  // Saturday
    }

    string_alarm_descr = Alarm_AlarmShortString(Smtp_FileTx_AlarmType);

    sprintf(string_v         , "%02d"                         , Smtp_FileTx_V);
    sprintf(string_id        , "%s"                           , ptr_id       );
    sprintf(string_n         , "%06ld"                        , Smtp_FileTx_N);
    sprintf(string_start_time, "%04d.%02d.%02d_%02d.%02d.%02d", Smtp_FileTx_StartTime.year, Smtp_FileTx_StartTime.month, Smtp_FileTx_StartTime.day, Smtp_FileTx_StartTime.hour, Smtp_FileTx_StartTime.minute, Smtp_FileTx_StartTime.second);
    sprintf(string_stop_time , "%04d.%02d.%02d_%02d.%02d.%02d", Smtp_FileTx_StopTime.year , Smtp_FileTx_StopTime.month , Smtp_FileTx_StopTime.day , Smtp_FileTx_StopTime.hour , Smtp_FileTx_StopTime.minute , Smtp_FileTx_StopTime.second );
    sprintf(string_alarm_type, "%s"                           , string_alarm_descr);


    /* init string (empty) */
    mail_file_name[0] = '\0';


    if (Smtp_FileTx_FileType == SMTP__FILE_TYPE__LOG)
    {
        // "EASY__LOG__V_00__ID_000000012345__N_000000__Start_2015.05.06_04.00.00__Stop_2015.05.06_08.00.00.csv"

        strncat(mail_file_name, "\""             , 120);

        strncat(mail_file_name, "EASY"           , 120);

        strncat(mail_file_name, "__LOG"          , 120);

        strncat(mail_file_name, "__V_"           , 120);
        strncat(mail_file_name, string_v         , 120);

        strncat(mail_file_name, "__ID_"          , 120);
        strncat(mail_file_name, string_id        , 120);

        strncat(mail_file_name, "__N_"           , 120);
        strncat(mail_file_name, string_n         , 120);

        strncat(mail_file_name, "__Start_"       , 120);
        strncat(mail_file_name, string_start_time, 120);

        strncat(mail_file_name, "__Stop_"        , 120);
        strncat(mail_file_name, string_stop_time , 120);


        strncat(mail_file_name, ".csv"           , 120);

        strncat(mail_file_name, "\""             , 120);
    }
    else
    {
        // "EASY__ALARM__V_00__ID_000000012345__N_000000__Time_2015.05.06_04.00.00__Alarm_TMIN_RESTORED.csv"

        strncat(mail_file_name, "\""             , 120);

        strncat(mail_file_name, "EASY"           , 120);

        strncat(mail_file_name, "__ALARM"        , 120);

        strncat(mail_file_name, "__V_"           , 120);
        strncat(mail_file_name, string_v         , 120);

        strncat(mail_file_name, "__ID_"          , 120);
        strncat(mail_file_name, string_id        , 120);

        strncat(mail_file_name, "__N_"           , 120);
        strncat(mail_file_name, string_n         , 120);

        strncat(mail_file_name, "__Time_"        , 120);
        strncat(mail_file_name, string_start_time, 120);

        strncat(mail_file_name, "__Alarm_"       , 120);
        strncat(mail_file_name, string_alarm_type, 120);


        strncat(mail_file_name, ".csv"           , 120);

        strncat(mail_file_name, "\""             , 120);
    }

    mail_file_name[120] = '\0';


    return mail_file_name;
}




/*=============================================================================
 * Function   : Smtp_BuildMailFileData
 *
 * Description: build the mail file data string
 * Input      : -
 * Output     : - pointer to save the string built
 *=============================================================================*/
static ascii *Smtp_BuildMailFileData(void)
{
    static u8     mail_file_data_block[FILE_BLOCK_SIZE];

    static ascii  mail_file_data_coded[(MAX_FILE_SIZE * 4 / 3) + 100 + 1];

           ascii *ptr_mail_file_data_coded;

           u32    mail_file_data_coded_len;

           u32    file_size;
           u32    block_coded_size;

           u32    n;
           u32    r;

           bool   result;
           u32    i;


    /* get the file size (bytes) */
    result = FileSystem_FileSize_FileName(Smtp_FileTx_FileName, &file_size);
    if (!result)
        return NULL;

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "file_size: %ld", file_size);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 50);

    /* check maximum file size */
    if (file_size > MAX_FILE_SIZE)
        file_size = MAX_FILE_SIZE;


    n = file_size / FILE_BLOCK_SIZE;
    r = file_size % FILE_BLOCK_SIZE;

    ptr_mail_file_data_coded = mail_file_data_coded;

    for (i = 0; i < n; i++)
    {
        /* read data from the file */
        result = FileSystem_File_Read(Smtp_FileTx_FileName, FILE_BLOCK_SIZE, (i * FILE_BLOCK_SIZE), mail_file_data_block);
        if (!result)
            return NULL;

        /* print the data read */
      //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "Data read:", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
      //Utility_PrintData(DEBUG_TRACE_LEVEL_SMTP, FILE_BLOCK_SIZE, mail_file_data_block);

        /* code the data read to Base64 */
        block_coded_size = Smtp_CodeFileBase64(      mail_file_data_block, FILE_BLOCK_SIZE       , ptr_mail_file_data_coded);
      //block_coded_size = Smtp_CodeFileBase64((u8 *)Smtp_File_1         , strlen(Smtp_File_1), ptr_mail_file_data_coded);
      //block_coded_size = Smtp_CodeFileBase64((u8 *)Smtp_File_2         , strlen(Smtp_File_2), ptr_mail_file_data_coded);

        ptr_mail_file_data_coded += block_coded_size;
    }

    if (r > 0)
    {
        /* read data from the file */
        result = FileSystem_File_Read(Smtp_FileTx_FileName, r, (n * FILE_BLOCK_SIZE), mail_file_data_block);
        if (!result)
            return NULL;

        /* print the data read */
      //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "Data read:", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
      //Utility_PrintData(DEBUG_TRACE_LEVEL_SMTP, r, mail_file_data_block);

        /* code the data read to Base64 */
        block_coded_size = Smtp_CodeFileBase64(      mail_file_data_block, r                     , ptr_mail_file_data_coded);
      //block_coded_size = Smtp_CodeFileBase64((u8 *)Smtp_File_1         , strlen(Smtp_File_1), ptr_mail_file_data_coded);
      //block_coded_size = Smtp_CodeFileBase64((u8 *)Smtp_File_2         , strlen(Smtp_File_2), ptr_mail_file_data_coded);

        ptr_mail_file_data_coded += block_coded_size;
    }

    *ptr_mail_file_data_coded = 0x00;

    mail_file_data_coded_len = strlen(mail_file_data_coded);

    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "mail_file_data_coded_len: %ld", mail_file_data_coded_len);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return mail_file_data_coded;
}




/*=============================================================================
 * Function   : Smtp_BuildMailDeviceId
 *
 * Description: build the mail device ID string
 * Input      : -
 * Output     : - pointer to save the string built
 *=============================================================================*/
static ascii *Smtp_BuildMailDeviceId(void)
{
    static DEVICE_ID__CONFIG__DEVICE_ID config_device_id;

    static ascii                        mail_device_id[DEVICE_ID__MAX_LENGTH_DEVICE_ID + 1];


    /* get the "device ID" configuration */
    DeviceId_Config_DeviceId_Get(&config_device_id);


    /* init string (empty) */
    mail_device_id[0] = 0x00;

    if (strlen(config_device_id.device_id) > 0)
        strncat(mail_device_id, config_device_id.device_id, DEVICE_ID__MAX_LENGTH_DEVICE_ID);
    else
        strncat(mail_device_id, "XXXXXXXXXXXX"            , DEVICE_ID__MAX_LENGTH_DEVICE_ID);

    mail_device_id[DEVICE_ID__MAX_LENGTH_DEVICE_ID] = 0x00;

    return mail_device_id;
}




/*=============================================================================
 * Function   : Smtp_BuildMailImei
 *
 * Description: build the mail IMEI string
 * Input      : -
 * Output     : - pointer to save the string built
 *=============================================================================*/
static ascii *Smtp_BuildMailImei(void)
{
    static PHONE__PHONE_ID phone_id;

    static ascii           mail_imei[PHONE__LEN_MAX_IMEI + 1];


    /* get the IMEI */
    Phone_PhoneId_Get(&phone_id);


    /* init string (empty) */
    mail_imei[0] = 0x00;

    strncat(mail_imei, phone_id.imei, PHONE__LEN_MAX_IMEI);

    mail_imei[PHONE__LEN_MAX_IMEI] = 0x00;

    return mail_imei;
}




/*=============================================================================
 * Function   : Smtp_CodeFileBase64
 *
 * Description: code a file with Base64
 * Input      : - ptr_file_in : pointer to file to be coded
 *              - file_in_size: file size       to be coded (bytes)
 *              - ptr_file_out: pointer to file       coded
 * Output     : - size of the coded file
 *=============================================================================*/
static u32 Smtp_CodeFileBase64(u8 *ptr_file_in, u32 file_in_size, ascii *ptr_file_out)
{
    u8    *ptr_data_in;

    u32    n;
    u32    r;

    u32    data_in;
    ascii  data_out[4 + 1];

    u32    data_out_size;

    u32    i;


    if (file_in_size == 0)
        return 0;


    ptr_data_in = ptr_file_in;

    n = file_in_size / 3;
    r = file_in_size % 3;

    data_out_size = 0;

    for (i = 0; i < n; i++)
    {
        data_in = ((u32)(*(ptr_data_in    )) << 16) |
                  ((u32)(*(ptr_data_in + 1)) <<  8) |
                  ((u32)(*(ptr_data_in + 2))      );

        Utility_Integer24BitsToBase64WithPadding(data_in, data_out);

        *(ptr_file_out    ) = data_out[0];
        *(ptr_file_out + 1) = data_out[1];
        *(ptr_file_out + 2) = data_out[2];
        *(ptr_file_out + 3) = data_out[3];

        ptr_data_in  += 3;
        ptr_file_out += 4;

        data_out_size +=4;
    }

    if      (r == 1)
    {
        data_in = ((u32)(*(ptr_data_in    )) << 16);

        Utility_Integer8BitsToBase64WithPadding (data_in, data_out);

        *(ptr_file_out    ) = data_out[0];
        *(ptr_file_out + 1) = data_out[1];
        *(ptr_file_out + 2) = data_out[2];
        *(ptr_file_out + 3) = data_out[3];

        data_out_size +=4;
    }
    else if (r == 2)
    {
        data_in = ((u32)(*(ptr_data_in    )) << 16) |
                  ((u32)(*(ptr_data_in + 1)) <<  8);

        Utility_Integer16BitsToBase64WithPadding(data_in, data_out);

        *(ptr_file_out    ) = data_out[0];
        *(ptr_file_out + 1) = data_out[1];
        *(ptr_file_out + 2) = data_out[2];
        *(ptr_file_out + 3) = data_out[3];

        data_out_size +=4;
    }


    return data_out_size;
}
#endif




/*===========================================================================
 * Function   : Smtp_Config_SmtpServer_GetDefault
 *
 * Description: get the default "SMTP server" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_SmtpServer_GetDefault(SMTP__CONFIG__SMTP_SERVER *ptr_data)
{
    *ptr_data = Smtp_Config_SmtpServerDefault;
}




/*===========================================================================
 * Function   : Smtp_Config_SmtpServer_Get
 *
 * Description: get the "SMTP server" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_SmtpServer_Get(SMTP__CONFIG__SMTP_SERVER *ptr_data)
{
    *ptr_data = Smtp_Config_SmtpServer;
}




/*===========================================================================
 * Function   : Smtp_Config_SmtpServer_Set
 *
 * Description: set the "SMTP server" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Smtp_Config_SmtpServer_Set(SMTP__CONFIG__SMTP_SERVER *ptr_data)
{
    Smtp_Config_SmtpServer = *ptr_data;
}




/*===========================================================================
 * Function   : Smtp_Config_SmtpServer_IsValid
 *
 * Description: check if the "SMTP server" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Smtp_Config_SmtpServer_IsValid(SMTP__CONFIG__SMTP_SERVER *ptr_data)
{
    if (strlen(ptr_data->smtp_hostname) > SMTP__MAX_LENGTH_SMTP_HOST_NAME)
        return FALSE;

    if (strlen(ptr_data->smtp_username) > SMTP__MAX_LENGTH_SMTP_USER_NAME)
        return FALSE;

    if (strlen(ptr_data->smtp_password) > SMTP__MAX_LENGTH_SMTP_PASSWORD )
        return FALSE;

    if (
         (ptr_data->smtp_auth_type != SMTP__SMTP_AUTH_TYPE__NONE  ) &&
         (ptr_data->smtp_auth_type != SMTP__SMTP_AUTH_TYPE__CLEAR ) &&
         (ptr_data->smtp_auth_type != SMTP__SMTP_AUTH_TYPE__MIME64)
       )
    {
        return FALSE;
    }

    if (
         (ptr_data->smtp_sec_type  != SMTP__SMTP_SEC_TYPE__NONE   ) &&
         (ptr_data->smtp_sec_type  != SMTP__SMTP_SEC_TYPE__SSL    )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Smtp_Config_MailSender_GetDefault
 *
 * Description: get the default "mail sender" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailSender_GetDefault(SMTP__CONFIG__MAIL_SENDER *ptr_data)
{
    *ptr_data = Smtp_Config_MailSenderDefault;
}




/*===========================================================================
 * Function   : Smtp_Config_MailSender_Get
 *
 * Description: get the "mail sender" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailSender_Get(SMTP__CONFIG__MAIL_SENDER *ptr_data)
{
    *ptr_data = Smtp_Config_MailSender;
}




/*===========================================================================
 * Function   : Smtp_Config_MailSender_Set
 *
 * Description: set the "mail sender" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailSender_Set(SMTP__CONFIG__MAIL_SENDER *ptr_data)
{
    Smtp_Config_MailSender = *ptr_data;
}




/*===========================================================================
 * Function   : Smtp_Config_MailSender_IsValid
 *
 * Description: check if the "mail sender" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Smtp_Config_MailSender_IsValid(SMTP__CONFIG__MAIL_SENDER *ptr_data)
{
    if (strlen(ptr_data->mail_sender) > SMTP__MAX_LENGTH_MAIL_SENDER)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientTo_GetDefault
 *
 * Description: get the default "mail recipient To" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientTo_GetDefault(SMTP__CONFIG__MAIL_RECIPIENT_TO *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientToDefault;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientTo_Get
 *
 * Description: get the "mail recipient To" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientTo_Get(SMTP__CONFIG__MAIL_RECIPIENT_TO *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientTo;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientTo_Set
 *
 * Description: set the "mail recipient To" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientTo_Set(SMTP__CONFIG__MAIL_RECIPIENT_TO *ptr_data)
{
    Smtp_Config_MailRecipientTo = *ptr_data;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientTo_IsValid
 *
 * Description: check if the "mail recipient To" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Smtp_Config_MailRecipientTo_IsValid(SMTP__CONFIG__MAIL_RECIPIENT_TO *ptr_data)
{
    u8 i;


    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_TO;  i++)
    {
        if (strlen(ptr_data->mail_address_to [0]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_TO)
            return FALSE;
        if (strlen(ptr_data->mail_address_to [1]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_TO)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientCc_GetDefault
 *
 * Description: get the default "mail recipient Cc" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientCc_GetDefault(SMTP__CONFIG__MAIL_RECIPIENT_CC *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientCcDefault;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientCc_Get
 *
 * Description: get the "mail recipient Cc" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientCc_Get(SMTP__CONFIG__MAIL_RECIPIENT_CC *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientCc;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientCc_Set
 *
 * Description: set the "mail recipient Cc" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientCc_Set(SMTP__CONFIG__MAIL_RECIPIENT_CC *ptr_data)
{
    Smtp_Config_MailRecipientCc = *ptr_data;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientCc_IsValid
 *
 * Description: check if the "mail recipient Cc" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Smtp_Config_MailRecipientCc_IsValid(SMTP__CONFIG__MAIL_RECIPIENT_CC *ptr_data)
{
    u8 i;


    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_CC; i++)
    {
        if (strlen(ptr_data->mail_address_cc[0]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_CC)
            return FALSE;
        if (strlen(ptr_data->mail_address_cc[1]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_CC)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBcc_GetDefault
 *
 * Description: get the default "mail recipient Bcc" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientBcc_GetDefault(SMTP__CONFIG__MAIL_RECIPIENT_BCC *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientBccDefault;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBcc_Get
 *
 * Description: get the "mail recipient Bcc" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientBcc_Get(SMTP__CONFIG__MAIL_RECIPIENT_BCC *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientBcc;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBcc_Set
 *
 * Description: set the "mail recipient Bcc" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientBcc_Set(SMTP__CONFIG__MAIL_RECIPIENT_BCC *ptr_data)
{
    Smtp_Config_MailRecipientBcc = *ptr_data;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBcc_IsValid
 *
 * Description: check if the "mail recipient Bcc" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Smtp_Config_MailRecipientBcc_IsValid(SMTP__CONFIG__MAIL_RECIPIENT_BCC *ptr_data)
{
    u8 i;


    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_BCC; i++)
    {
        if (strlen(ptr_data->mail_address_bcc[0]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC)
            return FALSE;
        if (strlen(ptr_data->mail_address_bcc[1]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBackup_GetDefault
 *
 * Description: get the default "mail recipient backup" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientBackup_GetDefault(SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientBackupDefault;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBackup_Get
 *
 * Description: get the "mail recipient backup" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientBackup_Get(SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data)
{
    *ptr_data = Smtp_Config_MailRecipientBackup;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBackup_Set
 *
 * Description: set the "mail recipient backup" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Smtp_Config_MailRecipientBackup_Set(SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data)
{
    Smtp_Config_MailRecipientBackup = *ptr_data;
}




/*===========================================================================
 * Function   : Smtp_Config_MailRecipientBackup_IsValid
 *
 * Description: check if the "mail recipient backup" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Smtp_Config_MailRecipientBackup_IsValid(SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data)
{
    u8 i;


    for (i = 0; i < SMTP__NUM_OF_MAIL_ADDRESSES_BACKUP; i++)
    {
        if (strlen(ptr_data->mail_address_backup[0]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP)
            return FALSE;
        if (strlen(ptr_data->mail_address_backup[1]) > SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP)
            return FALSE;
    }

    return TRUE;
}




#ifdef FUNCTION_IMPLEMENTED
/*=============================================================================
 * Function   : Smtp_WipCallback_Smtp_ChannelSession
 *
 * Description: Handler for the SMTP session channel
 * Input      : - ev :
 *              - ctx:
 * Output     : -
 *=============================================================================*/
static void Smtp_WipCallback_Smtp_ChannelSession(wip_event_t *ev, void *ctx)
{
    wip_channel_t channel_copy;

    u32           error_code;
    u32           status_code;
    ascii       **ptr_error_string;

    int           result_int;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - WIP SMTP SESSION - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (ev->kind)
    {
        // This event is received when the session channel is established
        case WIP_CEV_OPEN:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_OPEN - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Smtp_SmtpHandler_ChannelConnection = ev->channel;

            /* open the data channel to send the mail */
            Smtp_ClientSmtpOpenChannel_Data();
            break;


        // This event is received when the peer closes down
        case WIP_CEV_PEER_CLOSE:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_PEER_CLOSE - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* close the data channel */
            //if (Smtp_SmtpHandler_ChannelDataTransfer)
            //{
            //    channel_copy = Smtp_SmtpHandler_ChannelDataTransfer;
            //    result_int = wip_close(Smtp_SmtpHandler_ChannelDataTransfer);
            //    if (result_int != 0)
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    else
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    Smtp_SmtpHandler_ChannelDataTransfer = (wip_channel_t)NULL;
            //}

            /* close the session channel */
            channel_copy = ev->channel;
            result_int = wip_close(ev->channel);
            if (result_int != 0)
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            Smtp_SmtpHandler_ChannelConnection = (wip_channel_t)NULL;

            /* signal mail transmission fail */
            TransmissionMail_Event_SmtpFail();
            break;


        // This event is received when a socket error or protocol error occurs during the session
        case WIP_CEV_ERROR:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_ERROR: %d - %lX", ev->content.error.errnum, ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            // The error code is negative, if error is due to Internet Library library socket.
            // The error code is positive, if error is due to Internet Library SMTP library  or SMTP protocol.

            /* get the the error code and the status code */
            result_int = wip_getOpts(ev->channel,
                                     WIP_COPT_ERROR,            &error_code,                      // error  code
                                     WIP_COPT_SMTP_STATUS_CODE, &status_code, &ptr_error_string,  // status code
                                     WIP_COPT_END);
            if (result_int != 0)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "wip_getOpts ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_getOpts - Error code  : %ld"   , error_code      );
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_getOpts - Status code : %ld"   , status_code     );
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_getOpts - Error string: \"%s\"", ptr_error_string);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }

            /* close the data channel */
            //if (Smtp_SmtpHandler_ChannelDataTransfer)
            //{
            //    channel_copy = Smtp_SmtpHandler_ChannelDataTransfer;
            //    result_int = wip_close(Smtp_SmtpHandler_ChannelDataTransfer);
            //    if (result_int != 0)
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    else
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    Smtp_SmtpHandler_ChannelDataTransfer = (wip_channel_t)NULL;
            //}

            /* close the session channel */
            channel_copy = ev->channel;
            result_int = wip_close(ev->channel);
            if (result_int != 0)
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            Smtp_SmtpHandler_ChannelConnection = (wip_channel_t)NULL;

            /* signal mail transmission fail */
            TransmissionMail_Event_SmtpFail();
            break;


        // This event is received when the data channel is closed to end the mail sending and that the transaction has ended properly.
        // Otherwise, WIP_CEV_ERROR event will be received.
        case WIP_CEV_DONE:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_DONE - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* close the data channel */
            //if (Smtp_SmtpHandler_ChannelDataTransfer)
            //{
            //    channel_copy = Smtp_SmtpHandler_ChannelDataTransfer;
            //    result_int = wip_close(Smtp_SmtpHandler_ChannelDataTransfer);
            //    if (result_int != 0)
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    else
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    Smtp_SmtpHandler_ChannelDataTransfer = (wip_channel_t)NULL;
            //}

            /* close the session channel */
            channel_copy = ev->channel;
            result_int = wip_close(ev->channel);
            if (result_int != 0)
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            Smtp_SmtpHandler_ChannelConnection = (wip_channel_t)NULL;

            /* signal mail transmission success */
            TransmissionMail_Event_SmtpOk();
            break;


        default:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "UNKNOWN EVENT: %d", ev->kind);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}




/*=============================================================================
 * Function   : Smtp_WipCallback_Smtp_ChannelDataTransfer
 *
 * Description: Handler for the SMTP data transfer channel
 * Input      : - ev :
 *              - ctx:
 * Output     : -
 *=============================================================================*/
static void Smtp_WipCallback_Smtp_ChannelDataTransfer(wip_event_t *ev, void *ctx)
{
           wip_channel_t channel_copy;

    static u8           *ptr_data_tx;
    static u32           data_tx_length = 0;

           int           num_bytes_written;

           u32           error_code;
           u32           status_code;
           ascii       **ptr_error_string;

           bool          result;
           int           result_int;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - WIP SMTP DATA - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (ev->kind)
    {
        // This event is sent when the response message header has been received
        case WIP_CEV_OPEN:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_OPEN - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Smtp_SmtpHandler_ChannelDataTransfer = ev->channel;

            /* build the mail */
            Smtp_BuildMail();

            /* send the mail with attachment file */
            ptr_data_tx    = (u8 *)Smtp_MailString;
            data_tx_length = strlen(Smtp_MailString);
          //ptr_data_tx    = (u8 *)Smtp_StringAttMail;
          //data_tx_length = strlen(Smtp_StringAttMail);

            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "Bytes to be written: %ld", data_tx_length);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;



        // This event is sent when request message body data can be written by the application.
        case WIP_CEV_WRITE:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_WRITE - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* if opened and write ready, directly send mail through the data channel */

            /* while there are data to send */
            while (data_tx_length > 0)
            {
                num_bytes_written = wip_write(ev->channel, ptr_data_tx, data_tx_length);
                if (num_bytes_written < 0)
                {
                    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_write ERROR: %d", num_bytes_written);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_write OK: %d"   , num_bytes_written);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }

                /* update current pointer and length */
                if (num_bytes_written > 0)
                {
                    ptr_data_tx += num_bytes_written;

                    if (data_tx_length >= num_bytes_written)
                        data_tx_length -= num_bytes_written;
                    else
                        data_tx_length = 0;
                }

                if      (num_bytes_written < 0)
                {
                    /* if wip_write() not ready or failed,  end loop */
                    break;
                }
                else if (num_bytes_written < data_tx_length)
                {
                    /* if wip_write() did not write all bytes, end loop */
                    break;
                }
            }


            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "Bytes left to be written: %ld", data_tx_length);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


            /* check if entire data block has been written */
            if (data_tx_length == 0)
            {
                /* close the data channel */
                channel_copy = ev->channel;
                result_int = wip_close(ev->channel);
                if (result_int != 0)
                {
                    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
            }
            break;



        // This event is sent when response message body data is available for reading by the application
        case WIP_CEV_READ:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_READ - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;



        // This event is sent when a socket error has occurred. The error number is passed in the content.error.errnum field of the event data structure.
        case WIP_CEV_ERROR:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_ERROR: %d - %lX", ev->content.error.errnum, ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            // The error code is negative, if error is due to Internet Library library socket.
            // The error code is positive, if error is due to Internet Library SMTP library  or SMTP protocol.

            /* get the the error code and the status code */
            result_int = wip_getOpts(ev->channel,
                                     WIP_COPT_ERROR,            &error_code,                      // error  code
                                     WIP_COPT_SMTP_STATUS_CODE, &status_code, &ptr_error_string,  // status code
                                     WIP_COPT_END);
            if (result_int != 0)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "wip_getOpts ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_getOpts - Error code  : %ld"   , error_code      );
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_getOpts - Status code : %ld"   , status_code     );
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_getOpts - Error string: \"%s\"", ptr_error_string);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }

            /* close the data channel */
            channel_copy = ev->channel;
            result_int = wip_close(ev->channel);
            if (result_int != 0)
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            Smtp_SmtpHandler_ChannelDataTransfer = (wip_channel_t)NULL;

            /* close the session channel */
            //if (Smtp_SmtpHandler_ChannelConnection)
            //{
            //    channel_copy = Smtp_SmtpHandler_ChannelConnection;
            //    result_int = wip_close(Smtp_SmtpHandler_ChannelConnection);
            //    if (result_int != 0)
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    else
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    Smtp_SmtpHandler_ChannelConnection = (wip_channel_t)NULL;
            //}

            /* signal mail transmission fail */
            TransmissionMail_Event_SmtpFail();
            break;



        // This event is sent after the entire response message, including response header and response body data, has been received
        case WIP_CEV_PEER_CLOSE:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_PEER_CLOSE - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* close the data channel */
            channel_copy = ev->channel;
            result_int = wip_close(ev->channel);
            if (result_int != 0)
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            Smtp_SmtpHandler_ChannelDataTransfer = (wip_channel_t)NULL;

            /* close the session channel */
            //channel_copy = Smtp_SmtpHandler_ChannelConnection;
            //if (Smtp_SmtpHandler_ChannelConnection)
            //{
            //    result_int = wip_close(Smtp_SmtpHandler_ChannelConnection);
            //    if (result_int != 0)
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - ERROR: %d - %lX", result_int, channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    else
            //    {
            //        snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "wip_close - OK - %lX", channel_copy);
            //        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            //    }
            //    Smtp_SmtpHandler_ChannelConnection = (wip_channel_t)NULL;
            //}
            break;



        case WIP_CEV_DONE:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_DONE - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;



        case WIP_CEV_PING:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "WIP_CEV_PING - %lX", ev->channel);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;



        // unexpected event
        default:
            snprintf(Smtp_DebugString, sizeof(Smtp_DebugString), "UNKNOWN EVENT: %d", ev->kind);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, Smtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}




/*=============================================================================
 * Function   : Smtp_WipCallback_Smtp_ChannelFinalize
 *
 * Description:
 * Input      : - ctx:
 * Output     : -
 *=============================================================================*/
static void Smtp_WipCallback_Smtp_ChannelFinalize(void *ctx)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_SMTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SMTP - FINALIZER", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}
#endif
