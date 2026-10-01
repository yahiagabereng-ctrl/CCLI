/*=============================================================================
 * File       :  HTTP.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - HTTP connection
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __HTTP_H__


#define __HTTP_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* file type */
#define HTTP__FILE_TYPE__LOG           0    /* status log file */
#define HTTP__FILE_TYPE__ALARM         1    /* alarm      file */


#define HTTP__LEN_MAX_HOST_NAME        30   /* maximum string length - host      name    */
#define HTTP__LEN_MAX_PATH_NAME        30   /* maximum string length - path      name    */

#define HTTP__LEN_MAX_HOST_TX_NAME     30   /* maximum string length - host tx   name    */
#define HTTP__LEN_MAX_PATH_TX_NAME     30   /* maximum string length - path tx   name    */

#define HTTP__LEN_MAX_HOST_DATA_NAME   30   /* maximum string length - host data name    */
#define HTTP__LEN_MAX_PATH_DATA_NAME   30   /* maximum string length - path data name    */

#define HTTP__LEN_MAX_KEY              20   /* maximum string length - key               */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* last command ID */
typedef struct
{
    s32   last_command_id;                             /* last command ID */
} HTTP__LAST_COMMAND_ID;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - host */
typedef struct
{
    ascii host_name[HTTP__LEN_MAX_HOST_NAME      + 1];   /* host name */
    ascii path_name[HTTP__LEN_MAX_PATH_NAME      + 1];   /* path name */
    u16   port;                                          /* port      */
} HTTP__CONFIG__HOST;


/* configuration - host tx */
typedef struct
{
    ascii host_name[HTTP__LEN_MAX_HOST_TX_NAME   + 1];   /* host name */
    ascii path_name[HTTP__LEN_MAX_PATH_TX_NAME   + 1];   /* path name */
    u16   port;                                          /* port      */
} HTTP__CONFIG__HOST_TX;


/* configuration - host data */
typedef struct
{
    ascii host_name[HTTP__LEN_MAX_HOST_DATA_NAME + 1];   /* host name */
    ascii path_name[HTTP__LEN_MAX_PATH_DATA_NAME + 1];   /* path name */
    u16   port;                                          /* port      */
} HTTP__CONFIG__HOST_DATA;


/* configuration - host account 2 */
typedef struct
{
    ascii key      [HTTP__LEN_MAX_KEY            + 1];   /* key */
} HTTP__CONFIG__HOST_ACCOUNT_2;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
void Http_Init(void);

/* HTTP GET/POST */
bool Http_ClientHttpGetRxRequest(bool synchronize, ascii *ptr_sms_text);
bool Http_ClientHttpGetTxRequest(bool synchronize, s32    command_id);
bool Http_ClientHttpPostRequest (bool synchronize, u8 file_data_id, ascii *file_name, u8 file_version, CLOCK__TIME file_time_start, CLOCK__TIME file_time_stop);
bool Http_ClientHttpGetRxRelease(void);
bool Http_ClientHttpGetTxRelease(void);
bool Http_ClientHttpPostRelease (void);
bool Http_ClientHttpGetRx       (void);
bool Http_ClientHttpGetTx       (void);
bool Http_ClientHttpPost        (void);

/* get/set "last command ID" */
void Http_LastCommandId_GetDefault      (HTTP__LAST_COMMAND_ID        *ptr_data);
void Http_LastCommandId_Get             (HTTP__LAST_COMMAND_ID        *ptr_data);
void Http_LastCommandId_Set             (HTTP__LAST_COMMAND_ID        *ptr_data);

/*-----------------------------------------------------------------------------
 * get/set configurations
 *-----------------------------------------------------------------------------*/
/* get/set "host"           configuration */
void Http_Config_Host_GetDefault        (HTTP__CONFIG__HOST           *ptr_data);
void Http_Config_Host_Get               (HTTP__CONFIG__HOST           *ptr_data);
void Http_Config_Host_Set               (HTTP__CONFIG__HOST           *ptr_data);
bool Http_Config_Host_IsValid           (HTTP__CONFIG__HOST           *ptr_data);

/* get/set "host tx"        configuration */
void Http_Config_HostTx_GetDefault      (HTTP__CONFIG__HOST_TX        *ptr_data);
void Http_Config_HostTx_Get             (HTTP__CONFIG__HOST_TX        *ptr_data);
void Http_Config_HostTx_Set             (HTTP__CONFIG__HOST_TX        *ptr_data);
bool Http_Config_HostTx_IsValid         (HTTP__CONFIG__HOST_TX        *ptr_data);

/* get/set "host data"      configuration */
void Http_Config_HostData_GetDefault    (HTTP__CONFIG__HOST_DATA      *ptr_data);
void Http_Config_HostData_Get           (HTTP__CONFIG__HOST_DATA      *ptr_data);
void Http_Config_HostData_Set           (HTTP__CONFIG__HOST_DATA      *ptr_data);
bool Http_Config_HostData_IsValid       (HTTP__CONFIG__HOST_DATA      *ptr_data);

/* get/set "host account 2" configuration */
void Http_Config_HostAccount2_GetDefault(HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data);
void Http_Config_HostAccount2_Get       (HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data);
void Http_Config_HostAccount2_Set       (HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data);
bool Http_Config_HostAccount2_IsValid   (HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data);




#endif
