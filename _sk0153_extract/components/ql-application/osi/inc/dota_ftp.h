/*=============================================================================
 * File       :  DOTA_FTP.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - FTP manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DOTA_FTP_H__


#define __DOTA_FTP_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* FTP mode */
#define DOTA_FTP__FTP_MODE_PASSIVE           0     // FTP passive
#define DOTA_FTP__FTP_MODE_ACTIVE            1     // FTP active

/* max FTP parameters string length */
#define DOTA_FTP__MAX_LENGTH_FTP_SERVER      40    // FTP server
#define DOTA_FTP__MAX_LENGTH_FTP_USER_NAME   40    // FTP user name
#define DOTA_FTP__MAX_LENGTH_FTP_PASSWORD    40    // FTP password
#define DOTA_FTP__MAX_LENGTH_FTP_FILE_PATH   40    // FTP file path
#define DOTA_FTP__MAX_LENGTH_FTP_FILE_NAME   40    // FTP file name




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "FTP" parameters */
typedef struct
{
    u8    ftp_mode;                                                 // FTP mode (active/passive)
    u16   ftp_port;                                                 // FTP port
    ascii ftp_server   [DOTA_FTP__MAX_LENGTH_FTP_SERVER    + 1];    // FTP server
    ascii ftp_user_name[DOTA_FTP__MAX_LENGTH_FTP_USER_NAME + 1];    // FTP user name
    ascii ftp_password [DOTA_FTP__MAX_LENGTH_FTP_PASSWORD  + 1];    // FTP password
    ascii ftp_file_path[DOTA_FTP__MAX_LENGTH_FTP_FILE_PATH + 1];    // FTP file path
    ascii ftp_file_name[DOTA_FTP__MAX_LENGTH_FTP_FILE_NAME + 1];    // FTP file name
} DOTA_FTP__FTP_PARAMETERS;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* start/stop */
bool DotaFtp_FtpStartConnection(void);
bool DotaFtp_FtpStopConnection (void);

/* info */
bool DotaFtp_FtpFileDownloadDone(void);

/* get/set "FTP" configuration */
void DotaFtp_Config_FtpParameters_GetDefault(DOTA_FTP__FTP_PARAMETERS *ptr_data);
void DotaFtp_Config_FtpParameters_Get       (DOTA_FTP__FTP_PARAMETERS *ptr_data);
void DotaFtp_Config_FtpParameters_Set       (DOTA_FTP__FTP_PARAMETERS *ptr_data);
bool DotaFtp_Config_FtpParameters_IsValid   (DOTA_FTP__FTP_PARAMETERS *ptr_data);




#endif
