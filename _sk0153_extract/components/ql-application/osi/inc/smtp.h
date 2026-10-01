/*=============================================================================
 * File       :  SMTP.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMTP connection
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __SMTP_H__


#define __SMTP_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* file type */
#define SMTP__FILE_TYPE__LOG                   0      /* status log file */
#define SMTP__FILE_TYPE__ALARM                 1      /* alarm      file */

/* SMTP autentication type */
#define SMTP__SMTP_AUTH_TYPE__NONE             0      /* no authentication required                                                             */
#define SMTP__SMTP_AUTH_TYPE__CLEAR            1      /* authentication with no encryption                                                      */
#define SMTP__SMTP_AUTH_TYPE__MIME64           2      /* authentication used with encrypted username/password in MIME64 during AUTH LOGIN phase */

/* SMTP security type */
#define SMTP__SMTP_SEC_TYPE__NONE              0      /* none */
#define SMTP__SMTP_SEC_TYPE__SSL               1      /* SSL  */

/* maximum SMTP server parameters string length */
#define SMTP__MAX_LENGTH_SMTP_HOST_NAME        64     /* maximum string length - SMTP server host name */
#define SMTP__MAX_LENGTH_SMTP_USER_NAME        64     /* maximum string length - SMTP server user name */
#define SMTP__MAX_LENGTH_SMTP_PASSWORD         64     /* maximum string length - SMTP server password  */

/* maximum mail sender parameters string length */
#define SMTP__MAX_LENGTH_MAIL_SENDER           64     /* maximum string length - mail sender      */
#define SMTP__MAX_LENGTH_MAIL_SENDER_NAME      64     /* maximum string length - mail sender name */

/* maximum mail address           string length */
#define SMTP__MAX_LENGTH_MAIL_ADDRESS_TO       64     /* maximum string length - mail address To     */
#define SMTP__MAX_LENGTH_MAIL_ADDRESS_CC       64     /* maximum string length - mail address Cc     */
#define SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC      64     /* maximum string length - mail address Bcc    */
#define SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP   64     /* maximum string length - mail address backup */

/* maximum number of mail recipient addresses */
#define SMTP__NUM_OF_MAIL_ADDRESSES_TO         2
#define SMTP__NUM_OF_MAIL_ADDRESSES_CC         2
#define SMTP__NUM_OF_MAIL_ADDRESSES_BCC        2
#define SMTP__NUM_OF_MAIL_ADDRESSES_BACKUP     2




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "SMTP server" */
typedef struct
{
    ascii smtp_hostname                                          [SMTP__MAX_LENGTH_SMTP_HOST_NAME      + 1];   // SMTP server name
    u16   smtp_port;                                                                                           // SMTP server port
    ascii smtp_username                                          [SMTP__MAX_LENGTH_SMTP_USER_NAME      + 1];   // SMTP server username
    ascii smtp_password                                          [SMTP__MAX_LENGTH_SMTP_PASSWORD       + 1];   // SMTP server password
    u8    smtp_auth_type;                                                                                      // SMTP server authentication type
    u8    smtp_sec_type;                                                                                       // SMTP server security       type
} SMTP__CONFIG__SMTP_SERVER;


/* configuration - "mail sender" */
typedef struct
{
    ascii mail_sender                                            [SMTP__MAX_LENGTH_MAIL_SENDER         + 1];   // mail sender
} SMTP__CONFIG__MAIL_SENDER;


/* configuration - "mail recipient To" */
typedef struct
{
    ascii mail_address_to    [SMTP__NUM_OF_MAIL_ADDRESSES_TO    ][SMTP__MAX_LENGTH_MAIL_ADDRESS_TO     + 1];   // mail address TO
} SMTP__CONFIG__MAIL_RECIPIENT_TO;


/* configuration - "mail recipient Cc" */
typedef struct
{
    ascii mail_address_cc    [SMTP__NUM_OF_MAIL_ADDRESSES_CC    ][SMTP__MAX_LENGTH_MAIL_ADDRESS_CC     + 1];   // mail address CC
} SMTP__CONFIG__MAIL_RECIPIENT_CC;


/* configuration - "mail recipient Bcc" */
typedef struct
{
    ascii mail_address_bcc   [SMTP__NUM_OF_MAIL_ADDRESSES_BCC   ][SMTP__MAX_LENGTH_MAIL_ADDRESS_BCC    + 1];   // mail address BCC
} SMTP__CONFIG__MAIL_RECIPIENT_BCC;


/* configuration - "mail recipient backup" */
typedef struct
{
    ascii mail_address_backup[SMTP__NUM_OF_MAIL_ADDRESSES_BACKUP][SMTP__MAX_LENGTH_MAIL_ADDRESS_BACKUP + 1];   // mail address backup
} SMTP__CONFIG__MAIL_RECIPIENT_BACKUP;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* SMTP */
bool Smtp_ClientSmtpRequest(u8 file_type, ascii *log_file_name, u8 v, u32 n, CLOCK__TIME start_time, CLOCK__TIME stop_time, u8 alarm_type, u16 alarm_par);
bool Smtp_ClientSmtp       (void);


/*-----------------------------------------------------------------------------
 * get/set configurations
 *-----------------------------------------------------------------------------*/
/* get/set "SMTP server"           configuration */
void Smtp_Config_SmtpServer_GetDefault         (SMTP__CONFIG__SMTP_SERVER           *ptr_data);
void Smtp_Config_SmtpServer_Get                (SMTP__CONFIG__SMTP_SERVER           *ptr_data);
void Smtp_Config_SmtpServer_Set                (SMTP__CONFIG__SMTP_SERVER           *ptr_data);
bool Smtp_Config_SmtpServer_IsValid            (SMTP__CONFIG__SMTP_SERVER           *ptr_data);

/* get/set "mail sender"           configuration */
void Smtp_Config_MailSender_GetDefault         (SMTP__CONFIG__MAIL_SENDER           *ptr_data);
void Smtp_Config_MailSender_Get                (SMTP__CONFIG__MAIL_SENDER           *ptr_data);
void Smtp_Config_MailSender_Set                (SMTP__CONFIG__MAIL_SENDER           *ptr_data);
bool Smtp_Config_MailSender_IsValid            (SMTP__CONFIG__MAIL_SENDER           *ptr_data);

/* get/set "mail recipient To"     configuration */
void Smtp_Config_MailRecipientTo_GetDefault    (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);
void Smtp_Config_MailRecipientTo_Get           (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);
void Smtp_Config_MailRecipientTo_Set           (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);
bool Smtp_Config_MailRecipientTo_IsValid       (SMTP__CONFIG__MAIL_RECIPIENT_TO     *ptr_data);

/* get/set "mail recipient Cc"     configuration */
void Smtp_Config_MailRecipientCc_GetDefault    (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);
void Smtp_Config_MailRecipientCc_Get           (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);
void Smtp_Config_MailRecipientCc_Set           (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);
bool Smtp_Config_MailRecipientCc_IsValid       (SMTP__CONFIG__MAIL_RECIPIENT_CC     *ptr_data);

/* get/set "mail recipient Bcc"    configuration */
void Smtp_Config_MailRecipientBcc_GetDefault   (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);
void Smtp_Config_MailRecipientBcc_Get          (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);
void Smtp_Config_MailRecipientBcc_Set          (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);
bool Smtp_Config_MailRecipientBcc_IsValid      (SMTP__CONFIG__MAIL_RECIPIENT_BCC    *ptr_data);

/* get/set "mail recipient backup" configuration */
void Smtp_Config_MailRecipientBackup_GetDefault(SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);
void Smtp_Config_MailRecipientBackup_Get       (SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);
void Smtp_Config_MailRecipientBackup_Set       (SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);
bool Smtp_Config_MailRecipientBackup_IsValid   (SMTP__CONFIG__MAIL_RECIPIENT_BACKUP *ptr_data);




#endif
