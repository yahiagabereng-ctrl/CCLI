/*=============================================================================
 * File       :  HTTP.C
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
 * *** URL syntax ***
 *
 *     protocol://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 */


/*
 * *** URL syntax for HTTP GET "RX" for WebVision ***
 *
 *     http://<host_name><path_name>?SN=<serial_number>&KEY=<key>&SMS=<sms_text>
 *     http://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 *       - <host_name>    : host name
 *       - <path_name>    : path name
 *       - <serial_nUmber>: Serial Number
 *       - <key>          : key
 *       - <sms_text>     : SMS text
 *
 *     http://proxy.qfp.it/_m2msms2.php?SN=0123456789012&KEY=abcdefghijklmnopq&SMS=text%20SMS%20message
 *     http://46.51.181.117/_m2msms2.php?SN=0123456789012&KEY=abcdefghijklmnopq&SMS=text%20SMS%20message
 *
 *     http://username:password@proxy.qfp.it:80/_m2msms2.php?SN=0123456789012&KEY=abcdefghijklmnopq&SMS=text%20SMS%20message
 */


/*
 * *** URL syntax for HTTP GET "TX" for WebVision ***
 *
 *     http://<host_name><path_name>?SN=<serial_number>&KEY=<key>&ID=<command_id>
 *     http://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 *       - <host_name>    : host name
 *       - <path_name>    : path name
 *       - <serial_nUmber>: Serial Number
 *       - <key>          : key
 *       - <command_id>   : command ID
 *
 *     http://proxy.qfp.it/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=1234
 *     http://46.51.181.117/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=1234
 *
 *     http://username:password@proxy.qfp.it:80/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=1234
 */


 /*
 * *** URL syntax for HTTP POST for WebVision ***
 *
 *     http://<host_name><path_name>?SN=<serial_number>&KEY=<key>&ID=<data_id>&VER=<data_ver>
 *     http://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 *       - <host_name>    : host name
 *       - <path_name>    : path name
 *       - <serial_nUmber>: Serial Number
 *       - <key>          : key
 *       - <data_id>      : data ID
 *       - <data_ver>     : data version
 *
 *     http://proxy.qfp.it/_m2mbus.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=2&VER=1
 *     http://46.51.181.117/_m2mbus.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=2&VER=1
 *
 *     http://username:password@proxy.qfp.it:80/_m2mbus.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=2&VER=1
 */




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* API      includes */
#include "ql_http_client.h"

/* user     includes */
#include "boot.h"
#include "clock.h"
#include "debug_my.h"
#include "file_system.h"
#include "http_my.h"
#include "id.h"
#include "logs.h"
#include "parser_rx.h"
#include "program_flash.h"
#include "queue_my.h"
#include "report.h"
#include "synchronize.h"
#include "transmission_gprs.h"
#include "typedef.h"
#include "user_gsm_retx.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
#define MAX_MSG_LENGTH                            160

#define MAX_LENGTH_HTTP_STR_URL                   (100 + (3 * MAX_MSG_LENGTH) + 1)

#define MAX_LENGTH_DATA_STRING                    (DATA_BUFFER__DATA_STRING_SIZE)

#define RX_DATA_BUFFER_SIZE                       500

/* minimum elapsed    time since last synchronize for a new synchronize (sec) */
/* minimum difference time with actual time       for a new synchronize (sec) */
#define MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE   (60 * 60L)
#define MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME     ( 1 * 60L)

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING                   600


/*-----------------------------------------------------------------------------
 * HTTP GET "RX"
 *-----------------------------------------------------------------------------*/
#define HTTP_GET_RX_STRING_PROTOCOL               "http"

#define HTTP_GET_RX_STRING_QUERY_PAR_CID          "CID"                                                      // CID: customer identifier
#define HTTP_GET_RX_STRING_QUERY_PAR_ID           "ID"                                                       // ID : terminal identifier
#define HTTP_GET_RX_STRING_QUERY_PAR_PW           "PW"                                                       // PW : terminal password

#define HTTP_GET_RX_STRING_QUERY_PAR_SN           "SN"                                                       // SN : terminal Serial Number

#define HTTP_GET_RX_STRING_QUERY_PAR_KEY          "KEY"                                                      // KEY: terminal key

#define HTTP_GET_RX_STRING_QUERY_PAR_SMS          "SMS"                                                      // SMS: SMS text

#define HTTP_GET_RX_ANSWER_OK                     "OK"                                                       // "OK"
#define HTTP_GET_RX_ANSWER_REGFAIL                "REGFAIL"                                                  // "REGFAIL"
#define HTTP_GET_RX_ANSWER_REGFAIL_90             "REGFAIL\n90 Numero Seriale terminale non esistente"       // "REGFAIL 90 Numero Seriale terminale non esistente"
#define HTTP_GET_RX_ANSWER_REGFAIL_91             "REGFAIL\n91 Apikey gia' assegnata"                        // "REGFAIL 91 Apikey gia' assegnata"
#define HTTP_GET_RX_ANSWER_REGFAIL_92             "REGFAIL\n92 Apikey non corrispondente"                    // "REGFAIL 92 Apikey non corrispondente"
#define HTTP_GET_RX_ANSWER_REGFAIL_93             "REGFAIL\n93 Errore inserimento ApiKey"                    // "REGFAIL 93 Errore inserimento ApiKey"
#define HTTP_GET_RX_ANSWER_REGFAIL_94             "REGFAIL\n94 Messaggio non associato a questo terminale"   // "REGFAIL 94 Messaggio non associato a questo terminale"
#define HTTP_GET_RX_ANSWER_REGFAIL_95             "REGFAIL\n95 Max size allowed bytes."                      // "REGFAIL 95 Max size allowed bytes."
#define HTTP_GET_RX_ANSWER_REGFAIL_96             "REGFAIL\n96 Errore data-base"                             // "REGFAIL 96 Errore data-base"
#define HTTP_GET_RX_ANSWER_REGFAIL_97             "REGFAIL\n97 Terminale disabilitato"                       // "REGFAIL 97 Terminale disabilitato"
#define HTTP_GET_RX_ANSWER_REGFAIL_98             "REGFAIL\n98 Autenticazione fallita"                       // "REGFAIL 98 Autenticazione fallita"
#define HTTP_GET_RX_ANSWER_REGFAIL_99             "REGFAIL\n99 Sito off-line"                                // "REGFAIL 99 Sito off-line"


/*-----------------------------------------------------------------------------
 * HTTP GET "TX"
 *-----------------------------------------------------------------------------*/
#define HTTP_GET_TX_STRING_PROTOCOL               "http"

#define HTTP_GET_TX_STRING_QUERY_PAR_CID          "CID"                                                      // CID: customer identifier
#define HTTP_GET_TX_STRING_QUERY_PAR_ID           "ID"                                                       // ID : terminal identifier
#define HTTP_GET_TX_STRING_QUERY_PAR_PW           "PW"                                                       // PW : terminal password

#define HTTP_GET_TX_STRING_QUERY_PAR_SN           "SN"                                                       // SN : terminal Serial Number

#define HTTP_GET_TX_STRING_QUERY_PAR_KEY          "KEY"                                                      // KEY: terminal key

#define HTTP_GET_TX_STRING_QUERY_PAR_ID           "ID"                                                       // ID : command ID

#define HTTP_GET_TX_ANSWER_OK                     "OK"                                                       // "OK"
#define HTTP_GET_TX_ANSWER_REGFAIL                "REGFAIL"                                                  // "REGFAIL"
#define HTTP_GET_TX_ANSWER_REGFAIL_90             "REGFAIL\n90 Numero Seriale terminale non esistente"       // "REGFAIL 90 Numero Seriale terminale non esistente"
#define HTTP_GET_TX_ANSWER_REGFAIL_91             "REGFAIL\n91 Apikey gia' assegnata"                        // "REGFAIL 91 Apikey gia' assegnata"
#define HTTP_GET_TX_ANSWER_REGFAIL_92             "REGFAIL\n92 Apikey non corrispondente"                    // "REGFAIL 92 Apikey non corrispondente"
#define HTTP_GET_TX_ANSWER_REGFAIL_93             "REGFAIL\n93 Errore inserimento ApiKey"                    // "REGFAIL 93 Errore inserimento ApiKey"
#define HTTP_GET_TX_ANSWER_REGFAIL_94             "REGFAIL\n94 Messaggio non associato a questo terminale"   // "REGFAIL 94 Messaggio non associato a questo terminale"
#define HTTP_GET_TX_ANSWER_REGFAIL_95             "REGFAIL\n95 Max size allowed bytes."                      // "REGFAIL 95 Max size allowed bytes."
#define HTTP_GET_TX_ANSWER_REGFAIL_96             "REGFAIL\n96 Errore data-base"                             // "REGFAIL 96 Errore data-base"
#define HTTP_GET_TX_ANSWER_REGFAIL_97             "REGFAIL\n97 Terminale disabilitato"                       // "REGFAIL 97 Terminale disabilitato"
#define HTTP_GET_TX_ANSWER_REGFAIL_98             "REGFAIL\n98 Autenticazione fallita"                       // "REGFAIL 98 Autenticazione fallita"
#define HTTP_GET_TX_ANSWER_REGFAIL_99             "REGFAIL\n99 Sito off-line"                                // "REGFAIL 99 Sito off-line"


/*-----------------------------------------------------------------------------
 * HTTP POST
 *-----------------------------------------------------------------------------*/
#define HTTP_POST_STRING_PROTOCOL                 "http"

#define HTTP_POST_STRING_QUERY_PAR_CID            "CID"                                                      // CID: customer identifier
#define HTTP_POST_STRING_QUERY_PAR_ID             "ID"                                                       // ID : terminal identifier
#define HTTP_POST_STRING_QUERY_PAR_PW             "PW"                                                       // PW : terminal password

#define HTTP_POST_STRING_QUERY_PAR_SN             "SN"                                                       // SN : terminal Serial Number

#define HTTP_POST_STRING_QUERY_PAR_KEY            "KEY"                                                      // KEY: terminal key

#define HTTP_POST_STRING_QUERY_PAR_ID             "ID"                                                       // ID : data ID
#define HTTP_POST_STRING_QUERY_PAR_VER            "VER"                                                      // VER: data version

#define HTTP_POST_ANSWER_OK                       "OK"                                                       // "OK"
#define HTTP_POST_ANSWER_REGFAIL                  "REGFAIL"                                                  // "REGFAIL"
#define HTTP_POST_ANSWER_REGFAIL_90               "REGFAIL\n90 Numero Seriale terminale non esistente"       // "REGFAIL 90 Numero Seriale terminale non esistente"
#define HTTP_POST_ANSWER_REGFAIL_91               "REGFAIL\n91 Apikey gia' assegnata"                        // "REGFAIL 91 Apikey gia' assegnata"
#define HTTP_POST_ANSWER_REGFAIL_92               "REGFAIL\n92 Apikey non corrispondente"                    // "REGFAIL 92 Apikey non corrispondente"
#define HTTP_POST_ANSWER_REGFAIL_93               "REGFAIL\n93 Errore inserimento ApiKey"                    // "REGFAIL 93 Errore inserimento ApiKey"
#define HTTP_POST_ANSWER_REGFAIL_94               "REGFAIL\n94 Messaggio non associato a questo terminale"   // "REGFAIL 94 Messaggio non associato a questo terminale"
#define HTTP_POST_ANSWER_REGFAIL_95               "REGFAIL\n95 Max size allowed bytes."                      // "REGFAIL 95 Max size allowed bytes."
#define HTTP_POST_ANSWER_REGFAIL_96               "REGFAIL\n96 Errore data-base"                             // "REGFAIL 96 Errore data-base"
#define HTTP_POST_ANSWER_REGFAIL_97               "REGFAIL\n97 Terminale disabilitato"                       // "REGFAIL 97 Terminale disabilitato"
#define HTTP_POST_ANSWER_REGFAIL_98               "REGFAIL\n98 Autenticazione fallita"                       // "REGFAIL 98 Autenticazione fallita"
#define HTTP_POST_ANSWER_REGFAIL_99               "REGFAIL\n99 Sito off-line"                                // "REGFAIL 99 Sito off-line"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* msg text string */
static       ascii                        Http_MsgTextString[MAX_MSG_LENGTH + 1];

/* synchronize indication */
static       bool                         Http_Synchronize = FALSE;

/* command ID */
static       s32                          Http_CommandId;

/* "file in transmission" info */
static       u8                           Http_FileTx_FileDataId;
static       ascii                        Http_FileTx_FileName[68 + 1];
static       u8                           Http_FileTx_FileVersion;
static       CLOCK__TIME                  Http_FileTx_FileTimeStart;
static       CLOCK__TIME                  Http_FileTx_FileTimeStop;

/* HTTP URL string */
static       ascii                        Http_HttpStringUrl[MAX_LENGTH_HTTP_STR_URL + 1];

/* debug string */
static       ascii                        Http_DebugString  [MAX_LENGTH_DEBUG_STRING + 1];

/* last command ID */
static       HTTP__LAST_COMMAND_ID        Http_LastCommandId;
static const HTTP__LAST_COMMAND_ID        Http_LastCommandIdDefault =
{
    -1                 /* last command ID */
};


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - host */
static       HTTP__CONFIG__HOST           Http_Config_Host;
static const HTTP__CONFIG__HOST           Http_Config_HostDefault =
{
    "46.51.181.117",   /* host name */      // "proxy.qfp.it"
    "_m2msms2.php",    /* path name */
    80                 /* port      */
};


/* configuration - host tx */
static       HTTP__CONFIG__HOST_TX        Http_Config_HostTx;
static const HTTP__CONFIG__HOST_TX        Http_Config_HostTxDefault =
{
    "46.51.181.117",   /* host name */      // "proxy.qfp.it"
    "_m2msmstx2.php",  /* path name */
    80                 /* port      */
};


/* configuration - host data */
static       HTTP__CONFIG__HOST_DATA      Http_Config_HostData;
static const HTTP__CONFIG__HOST_DATA      Http_Config_HostDataDefault =
{
    "46.51.181.117",   /* host name */      // "proxy.qfp.it"
    "_m2mwmbus.php",   /* path name */
    80                 /* port      */
};


/* configuration - host account 2 */
static       HTTP__CONFIG__HOST_ACCOUNT_2 Http_Config_HostAccount2;
static const HTTP__CONFIG__HOST_ACCOUNT_2 Http_Config_HostAccount2Default =
{
    "",                /* key */
};


/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* HTTP handlers */
static       http_client_t                Http_HttpHandler_HttpGetRxChannelConnection;      // HTTP GET "RX" "Connection"    channel handler
static       http_client_t                Http_HttpHandler_HttpGetTxChannelConnection;      // HTTP GET "TX" "Connection"    channel handler
static       http_client_t                Http_HttpHandler_HttpPostChannelConnection;       // HTTP POST     "Connection"    channel handler




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
       void Http_Init(void);

/* HTTP GET/POST */
       bool Http_ClientHttpGetRxRequest(bool synchronize, ascii *ptr_msg_text);
       bool Http_ClientHttpGetTxRequest(bool synchronize, s32    command_id);
       bool Http_ClientHttpPostRequest (bool synchronize, u8 file_data_id, ascii *file_name, u8 file_version, CLOCK__TIME file_time_start, CLOCK__TIME file_time_stop);
       bool Http_ClientHttpGetRxRelease(void);
       bool Http_ClientHttpGetTxRelease(void);
       bool Http_ClientHttpPostRelease (void);
       bool Http_ClientHttpGetRx       (void);
       bool Http_ClientHttpGetTx       (void);
       bool Http_ClientHttpPost        (void);

/* info */
static bool Http_IsPresentHostAccount2(void);

/* build data */
static int  Http_BuildData(http_client_t *client, void *arg, char *data, int size);

/* random generation */
static void Http_GenerateRandomHostAccount2(void);

static void Http_BuildHttpGetRxUrlForHostAccount2(ascii *ptr_msg_text);
static void Http_BuildHttpGetTxUrlForHostAccount2(s32    command_id);
static void Http_BuildHttpPostUrlForHostAccount2 (u8     data_id, u8 data_ver);

static void Http_EncodeSpecialCharacters(ascii *ptr_string_1, ascii *ptr_string_2);

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


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* HTTP callback functions */
static void Http_WipCallback_Http_HttpGetRxChannelFinalize(http_client_t *client, int evt, int evt_code, void *arg);
static void Http_WipCallback_Http_HttpGetTxChannelFinalize(http_client_t *client, int evt, int evt_code, void *arg);
static void Http_WipCallback_Http_HttpPostChannelFinalize (http_client_t *client, int evt, int evt_code, void *arg);
static int  Http_WipCallback_Http_HttpGetRxChannelDataTransfer(http_client_t *client, void *arg, char *data, int size, unsigned char end);
static int  Http_WipCallback_Http_HttpGetTxChannelDataTransfer(http_client_t *client, void *arg, char *data, int size, unsigned char end);
static int  Http_WipCallback_Http_HttpPostChannelDataTransfer (http_client_t *client, void *arg, char *data, int size, unsigned char end);




/*=============================================================================
 * Function   : Http_Init
 *
 * Description: init
 *              generate a random host account if it is not configured
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Http_Init(void)
{
    bool result;


    /* synchronize indication */
    Http_Synchronize = FALSE;


    /* check if host account 2 is configured */
    result = Http_IsPresentHostAccount2();
    if (!result)
    {
        /* host account 2 not configured */

        /* generate random strings for host account 2 */
        Http_GenerateRandomHostAccount2();
    }
}




/*===========================================================================
 * Function    : Http_ClientHttpGetRxRequest
 *
 * Description : require to BOOT task to do a HTTP GET "RX"
 * Input       : - synchronize : synchronize indication
 *               - ptr_msg_text: pointer to msg text
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Http_ClientHttpGetRxRequest(bool synchronize, ascii *ptr_msg_text)
{
    bool result;


    /* save "synchronize indication" info */
    Http_Synchronize = synchronize;

    /* save the msg text */
    strncpy(Http_MsgTextString, ptr_msg_text, MAX_MSG_LENGTH);
    Http_MsgTextString[MAX_MSG_LENGTH] = 0x00;


    result = Boot_ClientHttpGetRxRequest();


    return result;
}




/*===========================================================================
 * Function    : Http_ClientHttpGetTxRequest
 *
 * Description : require to BOOT task to do a HTTP GET "TX"
 * Input       : - synchronize: synchronize indication
 *               - command_id : command ID
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Http_ClientHttpGetTxRequest(bool synchronize, s32 command_id)
{
    bool result;


    /* save "synchronize indication" info */
    Http_Synchronize = synchronize;

    /* save the command ID */
    Http_CommandId = command_id;


    result = Boot_ClientHttpGetTxRequest();


    return result;
}




/*===========================================================================
 * Function    : Http_ClientHttpPostRequest
 *
 * Description : require to BOOT task to do a HTTP POST
 * Input       : - synchronize    : synchronize indication
 *               - file_data_id   : file data ID
 *               - file_name      : file name
 *               - file_version   : file data version
 *               - file_time_start: file time start
 *               - file_time_stop : file time stop
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Http_ClientHttpPostRequest(bool synchronize, u8 file_data_id, ascii *file_name, u8 file_version, CLOCK__TIME file_time_start, CLOCK__TIME file_time_stop)
{
    bool result;


    /* save "synchronize indication" info */
    Http_Synchronize = synchronize;

    /* save "file in transmission" info */
    Http_FileTx_FileDataId    = file_data_id;
    strncpy(Http_FileTx_FileName, file_name, 68);
    Http_FileTx_FileName[68]  = 0x00;
    Http_FileTx_FileVersion   = file_version;
    Http_FileTx_FileTimeStart = file_time_start;
    Http_FileTx_FileTimeStop  = file_time_stop;


    result = Boot_ClientHttpPostRequest();


    return result;
}




/*===========================================================================
 * Function    : Http_ClientHttpGetRxRelease
 *
 * Description : HTTP GET "RX"
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Http_ClientHttpGetRxRelease(void)
{
    int res_int;


    res_int = ql_httpc_release(&Http_HttpHandler_HttpGetRxChannelConnection);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_release - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_release - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Http_ClientHttpGetTxRelease
 *
 * Description : HTTP GET "TX"
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Http_ClientHttpGetTxRelease(void)
{
    int res_int;


    res_int = ql_httpc_release(&Http_HttpHandler_HttpGetTxChannelConnection);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_release - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_release - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function    : Http_ClientHttpPostRelease
 *
 * Description : HTTP POST
 * Input       : -
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Http_ClientHttpPostRelease(void)
{
    int res_int;


    res_int = ql_httpc_release(&Http_HttpHandler_HttpPostChannelConnection);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_release - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_release - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Http_ClientHttpGetRx
 *
 * Description: HTTP GET "RX"
 * Input      : -
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
bool Http_ClientHttpGetRx(void)
{
    int res_int;


    /* build the HTTP GET "RX" URL string */
    Http_BuildHttpGetRxUrlForHostAccount2(Http_MsgTextString);


    /* print the msg text string */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "msg text: \"%s\"", Http_MsgTextString);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the HTTP URL string */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "URL: \"%s\"", Http_HttpStringUrl);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    res_int = ql_httpc_new(&Http_HttpHandler_HttpGetRxChannelConnection, Http_WipCallback_Http_HttpGetRxChannelFinalize, NULL);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_new - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_new - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* create an HTTP session channel */
    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_SIM_ID, 0);
    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_PDPCID, 1);

    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_URL, Http_HttpStringUrl);
    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_WRITE_FUNC, Http_WipCallback_Http_HttpGetRxChannelDataTransfer);
  //ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_WRITE_DATA, NULL);                            // no context (NULL)

    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_METHOD, HTTP_METHOD_GET);                     // HTTP method

  //ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, WIP_COPT_HTTP_VERSION, WIP_HTTP_VERSION_1_1);                 // HTTP version (default value)
  //ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "User-Agent: WIPHTTP/1.0");   // HTTP message header field (field name, field value)
  //ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, WIP_COPT_HTTP_DATA_ENCOD, TRUE);                              // data to transfer are encoded (CHUNKED DATA) (default value)

    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Accept: text/html");         // request headers (field name, field value)
    ql_httpc_setopt(&Http_HttpHandler_HttpGetRxChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Accept-Language: it, en");   // request headers (field name, field value)


    res_int = ql_httpc_perform(&Http_HttpHandler_HttpGetRxChannelConnection);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_perform - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_perform - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Http_ClientHttpGetTx
 *
 * Description: HTTP GET "TX"
 * Input      : -
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
bool Http_ClientHttpGetTx(void)
{
    int res_int;


    /* build the HTTP GET "TX" URL string */
    Http_BuildHttpGetTxUrlForHostAccount2(Http_CommandId);


    /* print the command ID */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "Command ID: %ld", Http_CommandId);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the HTTP URL string */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "URL: \"%s\"", Http_HttpStringUrl);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    res_int = ql_httpc_new(&Http_HttpHandler_HttpGetTxChannelConnection, Http_WipCallback_Http_HttpGetTxChannelFinalize, NULL);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_new - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_new - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* create an HTTP session channel */
    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_SIM_ID, 0);
    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_PDPCID, 1);

    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_URL, Http_HttpStringUrl);
    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_WRITE_FUNC, Http_WipCallback_Http_HttpGetTxChannelDataTransfer);
  //ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_WRITE_DATA, NULL);                            // no context (NULL)

    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_METHOD, HTTP_METHOD_GET);                     // HTTP method

  //ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, WIP_COPT_HTTP_VERSION, WIP_HTTP_VERSION_1_1);                 // HTTP version (default value)
  //ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "User-Agent: WIPHTTP/1.0");   // HTTP message header field (field name, field value)
  //ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, WIP_COPT_HTTP_DATA_ENCOD, TRUE);                              // data to transfer are encoded (CHUNKED DATA) (default value)

    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Accept: text/html");         // request headers (field name, field value)
    ql_httpc_setopt(&Http_HttpHandler_HttpGetTxChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Accept-Language: it, en");   // request headers (field name, field value)


    res_int = ql_httpc_perform(&Http_HttpHandler_HttpGetTxChannelConnection);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_perform - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_perform - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Http_ClientHttpPost
 *
 * Description: HTTP POST
 * Input      : -
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
bool Http_ClientHttpPost(void)
{
    s32  file_size;

    int  res_int;
    bool result;


    /* build the HTTP POST URL string */
    Http_BuildHttpPostUrlForHostAccount2(Http_FileTx_FileDataId, Http_FileTx_FileVersion);


    /* print the data ID */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "Data ID     : %d", Http_FileTx_FileDataId );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the data version */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "Data version: %d", Http_FileTx_FileVersion);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the HTTP URL string */
    snprintf(Http_DebugString, sizeof(Http_DebugString), "URL: \"%s\"", Http_HttpStringUrl);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    res_int = ql_httpc_new(&Http_HttpHandler_HttpPostChannelConnection, Http_WipCallback_Http_HttpPostChannelFinalize, NULL);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_new - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_new - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* get the file size (bytes) */
    result = FileSystem_FileSize_FileName(Http_FileTx_FileName, &file_size);
    if (!result)
    {
        return FALSE;
    }

    /* create an HTTP session channel */
    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_SIM_ID, 0);
    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_PDPCID, 1);

    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_URL, Http_HttpStringUrl);
    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_WRITE_FUNC, Http_WipCallback_Http_HttpPostChannelDataTransfer);
  //ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_WRITE_DATA, NULL);                            // no context (NULL)

    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_METHOD, HTTP_METHOD_POST);                    // HTTP method

  //ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, WIP_COPT_HTTP_VERSION, WIP_HTTP_VERSION_1_1);                 // HTTP version (default value)
  //ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "User-Agent: WIPHTTP/1.0");   // HTTP message header field (field name, field value)
  //ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, WIP_COPT_HTTP_DATA_ENCOD, TRUE);                              // data to transfer are encoded (CHUNKED DATA) (default value)

    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Accept: text/html");         // request headers (field name, field value)
    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Accept-Language: it, en");   // request headers (field name, field value)
    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_REQUEST_HEADER, "Content-Type: text/html");   // request headers (field name, field value)

    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_READ_FUNC, Http_BuildData);
  //ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_READ_DATA, NULL);
    ql_httpc_setopt(&Http_HttpHandler_HttpPostChannelConnection, HTTP_CLIENT_OPT_UPLOAD_LEN, file_size);

    res_int = ql_httpc_perform(&Http_HttpHandler_HttpPostChannelConnection);
    if (res_int != HTTP_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_perform - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_perform - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Http_IsPresentHostAccount2
 *
 * Description: verify is host account 2 is present
 * Input      : -
 * Output     : - FALSE: host account 2 is not present
 *              - TRUE : host account 2 is     present
 *=============================================================================*/
bool Http_IsPresentHostAccount2(void)
{
    if (
         (strlen(Http_Config_HostAccount2.key) > 0)
       )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




/*=============================================================================
 * Function   : Http_BuildData
 *
 * Description: build the data text string
 * Input      : -
 * Output     : -
 *=============================================================================*/
static int Http_BuildData(http_client_t *client, void *arg, char *data, int size)
{
    bool result;


    /* read data from the file */
    result = FileSystem_File_Read(Http_FileTx_FileName, size, 0, (u8 *)data);
    if (!result)
        return 0;

    return size;
}




/*=============================================================================
 * Function   : Http_GenerateRandomHostAccount2
 *
 * Description: generate a random host account 2
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Http_GenerateRandomHostAccount2(void)
{
    /* generate random string for host account 2 */
    Utility_RandomAlphanumericString(HTTP__LEN_MAX_KEY, Http_Config_HostAccount2.key);

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__HOST_ACCOUNT_2);
}




/*=============================================================================
 * Function   : Http_BuildHttpGetRxUrlForHostAccount2
 *
 * Description: build the HTTP GET "RX" URL string for a SMS text (for host account 2: KEY)
 *
 *                 http://<host_name><path_name>?SN=<serial_number>&KEY=<key>&SMS=<sms_text>
 *                 http://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 *                   - <host_name>    : host name
 *                   - <path_name>    : path name
 *                   - <serial_nUmber>: Serial Number
 *                   - <key>          : key
 *                   - <sms_text      : SMS text
 *
 *                 http://proxy.qfp.it/_m2msms2.php?SN=0123456789012&KEY=abcdefghijklmnopq&SMS=text%20SMS%20message
 *                 http://46.51.181.117/_m2msms2.php?SN=0123456789012&KEY=abcdefghijklmnopq&SMS=text%20SMS%20message
 *                 http://usernam:password@proxy.qfp.it:80/_m2msms2.php?SN=0123456789012&KEY=abcdefghijklmnopq&SMS=text%20SMS%20message
 *
 * Input      : - ptr_msg_text: pointer to msg text
 * Output     : -
 *=============================================================================*/
static void Http_BuildHttpGetRxUrlForHostAccount2(ascii *ptr_msg_text)
{
    static ID__CONFIG__SERIAL_NUMBER config_serial_number;

    static ascii                     msg_text_encoded_string[(3 * MAX_MSG_LENGTH) + 1];


    if (strlen(ptr_msg_text) > MAX_MSG_LENGTH)
        ptr_msg_text[MAX_MSG_LENGTH] = 0x00;


    /* encode special characters (space, ...) for SMS text */
    Http_EncodeSpecialCharacters(ptr_msg_text, msg_text_encoded_string);


    /* get the serial number */
    Id_Config_SerialNumber_Get(&config_serial_number);


    /* protocol + "://" + hostname + "/" + pathname */
    snprintf(Http_HttpStringUrl,
             MAX_LENGTH_HTTP_STR_URL + 1,

             HTTP_GET_RX_STRING_PROTOCOL"://%s:%d/%s?"
             HTTP_GET_RX_STRING_QUERY_PAR_SN"=%s&"
             HTTP_GET_RX_STRING_QUERY_PAR_KEY"=%s&"
             HTTP_GET_RX_STRING_QUERY_PAR_SMS"=%s",

             Http_Config_Host.host_name,   // host name string
             Http_Config_Host.port,        // port      string
             Http_Config_Host.path_name,   // path name string

             /* SN */
             config_serial_number.serial_number,

             /* KEY */
             Http_Config_HostAccount2.key,

             /* SMS */
             msg_text_encoded_string);
}




/*=============================================================================
 * Function   : Http_BuildHttpGetTxUrlForHostAccount2
 *
 * Description: build the HTTP GET "TX" URL string (for host account 2: KEY)
 *
 *                 http://<host_name><path_name>?SN=<serial_number>&EY=<key>&ID=<command_id>>
 *                 http://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 *                   - <host_name>    : host name
 *                   - <path_name>    : path name
 *                   - <serial_nUmber>: Serial Number
 *                   - <key>          : key
 *                   - <command_id>   : command ID
 *
 *                 http://proxy.qfp.it/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=297
 *                 http://46.51.181.117/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=297
 *                 http://usernam:password@proxy.qfp.it:80/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&ID=297
 *
 * Input      : - command_id: command ID
 * Output     : -
 *=============================================================================*/
static void Http_BuildHttpGetTxUrlForHostAccount2(s32 command_id)
{
    static ID__CONFIG__SERIAL_NUMBER config_serial_number;


    /* get the serial number */
    Id_Config_SerialNumber_Get(&config_serial_number);


    /* protocol + "://" + hostname + "/" + pathname */
    snprintf(Http_HttpStringUrl,
             MAX_LENGTH_HTTP_STR_URL + 1,

             HTTP_GET_TX_STRING_PROTOCOL"://%s:%d/%s?"
             HTTP_GET_TX_STRING_QUERY_PAR_SN"=%s&"
             HTTP_GET_TX_STRING_QUERY_PAR_KEY"=%s&"
             HTTP_GET_TX_STRING_QUERY_PAR_ID"=%ld",

             Http_Config_HostTx.host_name,   // host name string
             Http_Config_HostTx.port,        // port      string
             Http_Config_HostTx.path_name,   // path name string

             /* SN */
             config_serial_number.serial_number,

             /* KEY */
             Http_Config_HostAccount2.key,

             /* ID */
             command_id);
}




/*=============================================================================
 * Function   : Http_BuildHttpPostUrlForHostAccount2
 *
 * Description: build the HTTP POST URL string (for host account 2: KEY)
 *
 *                 http://<host_name><path_name>?SN=<serial_number>&KEY=<key>&DATA_ID=<data_id>&DATA_VER=<data_ver>
 *                 http://<username:password@>nomehost<:porta></percorso><?querystring><#fragment identifier>
 *                   - <host_name>    : host name
 *                   - <path_name>    : path name
 *                   - <serial_nUmber>: Serial Number
 *                   - <key>          : key
 *                   - <data_id>      : data ID
 *                   - <data_ver>     : data version
 *
 *                 http://proxy.qfp.it/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&DATA_ID=2&DATA_VER=1
 *                 http://46.51.181.117/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&DATA_ID=2&DATA_VER=1
 *                 http://username:password@proxy.qfp.it:80/_m2msmstx2.php?SN=0123456789012&KEY=abcdefghijklmnopq&DATA_ID=2&DATA_VER=1
 *
 * Input      : - data_id : data ID
 *              - data_ver: data version
 * Output     : -
 *=============================================================================*/
static void Http_BuildHttpPostUrlForHostAccount2(u8 data_id, u8 data_ver)
{
    static ID__CONFIG__SERIAL_NUMBER config_serial_number;


    /* get the serial number */
    Id_Config_SerialNumber_Get(&config_serial_number);


    /* protocol + "://" + hostname + "/" + pathname */
    snprintf(Http_HttpStringUrl,
             MAX_LENGTH_HTTP_STR_URL + 1,

             HTTP_POST_STRING_PROTOCOL"://%s:%d/%s?"
             HTTP_POST_STRING_QUERY_PAR_SN"=%s&"
             HTTP_POST_STRING_QUERY_PAR_KEY"=%s&"
             HTTP_POST_STRING_QUERY_PAR_ID"=%d&"
             HTTP_POST_STRING_QUERY_PAR_VER"=%d",

             Http_Config_HostData.host_name,   // host name string
             Http_Config_HostData.port,        // port      string
             Http_Config_HostData.path_name,   // path name string

             /* SN */
             config_serial_number.serial_number,

             /* KEY */
             Http_Config_HostAccount2.key,

             /* ID */
             data_id,

             /* VER */
             data_ver);
}




/*===========================================================================
 * Function   : Http_EncodeSpecialCharacters
 *
 * Description: encode special characters in the string
 *
 *                 ' '
 *                 ';'  '/'  '?'  ':'  '@'  '&'  '='  '+'  '$'  ','
 *                 '{'  '}'  '|'  '\'  '^'  '['  ']'  '`'
 *                 '<'  '>'  '#'  '%'  '"'
 *                 '!'  '*'  '\'' '('  ')'
 *
 *                 ! * ' ( ) ; : @ & = + $ , / ? % # [ ]
 *
 *                 Description        Symbol      Code
 *                 Punto e virgola    ;           %3B
 *                 Barra              /           %2F
 *                 Punto di domanda   ?           %3F
 *                 Due punti          :           %3A
 *                 Chiocciola         @           %40
 *                 Uguale             =           %3D
 *                 e Commerciale      &           %26
 *
 * Input      : - ptr_string_1: pointer to string to be encoded
 * Input      : - ptr_string_2: pointer to string       encoded
 * Output     : -
 *===========================================================================*/
static void Http_EncodeSpecialCharacters(ascii *ptr_string_1, ascii *ptr_string_2)
{
    u16   len_string;

    ascii character;

    u8    lo_nibble;
    u8    hi_nibble;

    ascii hi_nibble_char;
    ascii lo_nibble_char;

    u16   i;
    u16   j;


    /* calculate the string length */
    len_string = strlen(ptr_string_1);

    ptr_string_2[0] = 0x00;
    j = 0;


    for (i = 0; i < len_string; i++)
    {
        /* extract the i-th character */
        character = ptr_string_1[i];

        switch (character)
        {
            /*
            ';'  '/'  '?'  ':'  '@'  '&'  '='  '+'  '$'  ','
            '{'  '}'  '|'  '\'  '^'  '['  ']'  '`'
            '<'  '>'  '#'  '%'  '"'
            '!'  '*'  '\'' '('  ')'

            ! * ' ( ) ; : @ & = + $ , / ? % # [ ]

            ------------------------------------
            Description        Symbol      Code
            ------------------------------------
            Punto e virgola    ;           %3B
            Barra              /           %2F
            Punto di domanda   ?           %3F
            Due punti          :           %3A
            Chiocciola         @           %40
            Uguale             =           %3D
            e Commerciale      &           %26
            ------------------------------------
            */
            case ' ':
            case ';':
            case '/':
          //case '?':
            case ':':
            case '@':
            case '&':
            case '=':
            case '+':
            case '$':
            case ',':
            case '{':
            case '}':
            case '|':
            case '\\':
            case '^':
            case '[':
            case ']':
            case '`':
            case '<':
            case '>':
            case '#':
            case '%':
            case '"':
            case '!':
            case '*':
            case '\'':
            case '(':
            case ')':
                hi_nibble      = ((character >> 4) & 0x0F);
                lo_nibble      = ((character     ) & 0x0F);

                hi_nibble_char = (hi_nibble <= 9)  ?  ('0' + (ascii)hi_nibble)  :  ('A' + (ascii)(hi_nibble - 10));
                lo_nibble_char = (lo_nibble <= 9)  ?  ('0' + (ascii)lo_nibble)  :  ('A' + (ascii)(lo_nibble - 10));

                ptr_string_2[j++] = '%';
                ptr_string_2[j++] = hi_nibble_char;
                ptr_string_2[j++] = lo_nibble_char;
                break;


            default:
                ptr_string_2[j++] = character;
                break;
        }
    }


    ptr_string_2[j] = 0x00;
}




/*===========================================================================
 * Function   : Http_LastCommandId_GetDefault
 *
 * Description: get the default "last command ID"
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_LastCommandId_GetDefault(HTTP__LAST_COMMAND_ID *ptr_data)
{
    *ptr_data = Http_LastCommandIdDefault;
}




/*===========================================================================
 * Function   : Http_LastCommandId_Get
 *
 * Description: get the "last command ID"
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_LastCommandId_Get(HTTP__LAST_COMMAND_ID *ptr_data)
{
    *ptr_data = Http_LastCommandId;
}




/*===========================================================================
 * Function   : Http_LastCommandId_Set
 *
 * Description: set the "last command ID"
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Http_LastCommandId_Set(HTTP__LAST_COMMAND_ID *ptr_data)
{
    Http_LastCommandId = *ptr_data;
}




/*===========================================================================
 * Function   : Http_Config_Host_GetDefault
 *
 * Description: get the default "host" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_Host_GetDefault(HTTP__CONFIG__HOST *ptr_data)
{
    *ptr_data = Http_Config_HostDefault;
}




/*===========================================================================
 * Function   : Http_Config_Host_Get
 *
 * Description: get the "host" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_Host_Get(HTTP__CONFIG__HOST *ptr_data)
{
    *ptr_data = Http_Config_Host;
}




/*===========================================================================
 * Function   : Http_Config_Host_Set
 *
 * Description: set the "host" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Http_Config_Host_Set(HTTP__CONFIG__HOST *ptr_data)
{
    Http_Config_Host = *ptr_data;
}




/*===========================================================================
 * Function   : Http_Config_Host_IsValid
 *
 * Description: check if the "host" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Http_Config_Host_IsValid(HTTP__CONFIG__HOST *ptr_data)
{
    if (strlen(ptr_data->host_name) > HTTP__LEN_MAX_HOST_NAME)
        return FALSE;

    if (strlen(ptr_data->path_name) > HTTP__LEN_MAX_PATH_NAME)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Http_Config_HostTx_GetDefault
 *
 * Description: get the default "host tx" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_HostTx_GetDefault(HTTP__CONFIG__HOST_TX *ptr_data)
{
    *ptr_data = Http_Config_HostTxDefault;
}




/*===========================================================================
 * Function   : Http_Config_HostTx_Get
 *
 * Description: get the "host tx" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_HostTx_Get(HTTP__CONFIG__HOST_TX *ptr_data)
{
    *ptr_data = Http_Config_HostTx;
}




/*===========================================================================
 * Function   : Http_Config_HostTx_Set
 *
 * Description: set the "host tx" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Http_Config_HostTx_Set(HTTP__CONFIG__HOST_TX *ptr_data)
{
    Http_Config_HostTx = *ptr_data;
}




/*===========================================================================
 * Function   : Http_Config_HostTx_IsValid
 *
 * Description: check if the "host tx" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Http_Config_HostTx_IsValid(HTTP__CONFIG__HOST_TX *ptr_data)
{
    if (strlen(ptr_data->host_name) > HTTP__LEN_MAX_HOST_TX_NAME)
        return FALSE;

    if (strlen(ptr_data->path_name) > HTTP__LEN_MAX_PATH_TX_NAME)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Http_Config_HostData_GetDefault
 *
 * Description: get the default "host data" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_HostData_GetDefault(HTTP__CONFIG__HOST_DATA *ptr_data)
{
    *ptr_data = Http_Config_HostDataDefault;
}




/*===========================================================================
 * Function   : Http_Config_HostData_Get
 *
 * Description: get the "host data" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_HostData_Get(HTTP__CONFIG__HOST_DATA *ptr_data)
{
    *ptr_data = Http_Config_HostData;
}




/*===========================================================================
 * Function   : Http_Config_HostData_Set
 *
 * Description: set the "host data" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Http_Config_HostData_Set(HTTP__CONFIG__HOST_DATA *ptr_data)
{
    Http_Config_HostData = *ptr_data;
}




/*===========================================================================
 * Function   : Http_Config_HostData_IsValid
 *
 * Description: check if the "host data" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Http_Config_HostData_IsValid(HTTP__CONFIG__HOST_DATA *ptr_data)
{
    if (strlen(ptr_data->host_name) > HTTP__LEN_MAX_HOST_DATA_NAME)
        return FALSE;

    if (strlen(ptr_data->path_name) > HTTP__LEN_MAX_PATH_DATA_NAME)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Http_Config_HostAccount2_GetDefault
 *
 * Description: get the default "host account 2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_HostAccount2_GetDefault(HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data)
{
    *ptr_data = Http_Config_HostAccount2Default;
}




/*===========================================================================
 * Function   : Http_Config_HostAccount2_Get
 *
 * Description: get the "host account 2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Http_Config_HostAccount2_Get(HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data)
{
    *ptr_data = Http_Config_HostAccount2;
}




/*===========================================================================
 * Function   : Http_Config_HostAccount2_Set
 *
 * Description: set the "host account 2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Http_Config_HostAccount2_Set(HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data)
{
    Http_Config_HostAccount2 = *ptr_data;
}




/*===========================================================================
 * Function   : Http_Config_HostAccount2_IsValid
 *
 * Description: check if the "host account 2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Http_Config_HostAccount2_IsValid(HTTP__CONFIG__HOST_ACCOUNT_2 *ptr_data)
{
    if (strlen(ptr_data->key) > HTTP__LEN_MAX_KEY)
        return FALSE;

    return TRUE;
}




/*=============================================================================
 * Function   : Http_WipCallback_Http_HttpGetRxChannelFinalize
 *
 * Description:
 * Input      : - ctx:
 * Output     : -
 *=============================================================================*/
static void Http_WipCallback_Http_HttpGetRxChannelFinalize(http_client_t *client, int evt, int evt_code, void *arg)
{
#ifdef FUNCTION_IMPLEMENTED
    ascii                         *temp_buffer = NULL;

    SYNCHRONIZE__SYNCHRONIZE_TIME  synchronize_time;
    CLOCK__TIME                    current_rtc_time;
    CLOCK__TIME                    actual_time;

    s32                            elapsed_time;
    s32                            difference_time;

    bool                           result;
#endif

    int                            status_code;
    int                            result_int;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - HTTP GET RX - FINALIZER", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (evt)
    {
        case HTTP_EVENT_SESSION_ESTABLISH:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_SESSION_ESTABLISH - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code != HTTP_SUCCESS)
            {
                TransmissionGprs_Event_HttpGetRxFail();
            }
            break;

        case HTTP_EVENT_RESPONE_STATE_LINE:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_RESPONE_STATE_LINE - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code == HTTP_SUCCESS)
            {
                // show response information
                result_int = ql_httpc_getinfo(client, HTTP_INFO_RESPONSE_CODE, &status_code);   // the 3-digit response status code
                if (result_int != 0)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_getinfo ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "ql_httpc_getinfo - Status code: %d", status_code);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }

#ifdef FUNCTION_IMPLEMENTED
                // show response information
                result_int = ql_httpc_getinfo(client, HTTP_INFO_DATE, &temp_buffer);
                if (result_int != 0)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_getinfo ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "ql_httpc_getinfo - Date: \"%s\"", temp_buffer);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* get last synchronize time */
                    Synchronize_SynchronizeTime_Get(&synchronize_time);

                    /* get current RTC time */
                    result = Clock_GetTime(&current_rtc_time);

                    // extract actual GMT time from the header (format: "Tue, 13 Jan 2015 15:42:26 GMT")
                    result = Clock_ConverTimeString1ToClockTime(temp_buffer, &actual_time);   // GMT   time
                    if (result)
                    {
                        /* calculate elapsed    time since last synchronize */
                        /* calculate difference time with actual time       */
                        elapsed_time    = Clock_DifferenceRtcTimes(&synchronize_time.synchronize_time, &current_rtc_time);
                        difference_time = Clock_DifferenceRtcTimes(&actual_time                      , &current_rtc_time);

                        if (
                             (Http_Synchronize)                                                                                                            ||
                             ((elapsed_time    < -MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE) || (elapsed_time    > MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE)) ||
                             ((difference_time < -MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME  ) || (difference_time > MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME  ))
                           )
                        {
                            Synchronize_SynchronizeRtcTime(&actual_time, TRUE);
                        }
                    }

                    /* free allocated memory */
                    free(temp_buffer);
                }
#endif
            }
            break;

        case HTTP_EVENT_SESSION_DISCONNECT:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_SESSION_DISCONNECT - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code != HTTP_SUCCESS)
            {
                TransmissionGprs_Event_HttpGetRxFail();
            }
            break;
    }
}




/*=============================================================================
 * Function   : Http_WipCallback_Http_HttpGetTxChannelFinalize
 *
 * Description:
 * Input      : - ctx:
 * Output     : -
 *=============================================================================*/
static void Http_WipCallback_Http_HttpGetTxChannelFinalize(http_client_t *client, int evt, int evt_code, void *arg)
{
#ifdef FUNCTION_IMPLEMENTED
    ascii                         *temp_buffer = NULL;

    SYNCHRONIZE__SYNCHRONIZE_TIME  synchronize_time;
    CLOCK__TIME                    current_rtc_time;
    CLOCK__TIME                    actual_time;

    s32                            elapsed_time;
    s32                            difference_time;

    bool                           result;
#endif

    int                            status_code;
    int                            result_int;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - HTTP GET TX - FINALIZER", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (evt)
    {
        case HTTP_EVENT_SESSION_ESTABLISH:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_SESSION_ESTABLISH - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code != HTTP_SUCCESS)
            {
                TransmissionGprs_Event_HttpGetTxFail();
            }
            break;

        case HTTP_EVENT_RESPONE_STATE_LINE:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_RESPONE_STATE_LINE - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code == HTTP_SUCCESS)
            {
                // show response information
                result_int = ql_httpc_getinfo(client, HTTP_INFO_RESPONSE_CODE, &status_code);   // the 3-digit response status code
                if (result_int != 0)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_getinfo ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "ql_httpc_getinfo - Status code: %d", status_code);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }

#ifdef FUNCTION_IMPLEMENTED
                // show response information
                result_int = ql_httpc_getinfo(client, HTTP_INFO_DATE, &temp_buffer);
                if (result_int != 0)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_getinfo ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "ql_httpc_getinfo - Date: \"%s\"", temp_buffer);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* get last synchronize time */
                    Synchronize_SynchronizeTime_Get(&synchronize_time);

                    /* get current RTC time */
                    result = Clock_GetTime(&current_rtc_time);

                    // extract actual GMT time from the header (format: "Tue, 13 Jan 2015 15:42:26 GMT")
                    result = Clock_ConverTimeString1ToClockTime(temp_buffer, &actual_time);   // GMT   time
                    if (result)
                    {
                        /* calculate elapsed    time since last synchronize */
                        /* calculate difference time with actual time       */
                        elapsed_time    = Clock_DifferenceRtcTimes(&synchronize_time.synchronize_time, &current_rtc_time);
                        difference_time = Clock_DifferenceRtcTimes(&actual_time                      , &current_rtc_time);

                        if (
                             (Http_Synchronize)                                                                                                            ||
                             ((elapsed_time    < -MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE) || (elapsed_time    > MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE)) ||
                             ((difference_time < -MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME  ) || (difference_time > MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME  ))
                           )
                        {
                            Synchronize_SynchronizeRtcTime(&actual_time, TRUE);
                        }
                    }

                    /* free allocated memory */
                    free(temp_buffer);
                }
#endif
            }
            break;

        case HTTP_EVENT_SESSION_DISCONNECT:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_SESSION_DISCONNECT - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code != HTTP_SUCCESS)
            {
                TransmissionGprs_Event_HttpGetTxFail();
            }
            break;
    }
}




/*=============================================================================
 * Function   : Http_WipCallback_Http_HttpPostChannelFinalize
 *
 * Description:
 * Input      : - ctx:
 * Output     : -
 *=============================================================================*/
static void Http_WipCallback_Http_HttpPostChannelFinalize(http_client_t *client, int evt, int evt_code, void *arg)
{
#ifdef FUNCTION_IMPLEMENTED
    ascii                         *temp_buffer = NULL;

    SYNCHRONIZE__SYNCHRONIZE_TIME  synchronize_time;
    CLOCK__TIME                    current_rtc_time;
    CLOCK__TIME                    actual_time;

    s32                            elapsed_time;
    s32                            difference_time;

    bool                           result;
#endif

    int                            status_code;
    int                            result_int;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - HTTP POST - FINALIZER", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (evt)
    {
        case HTTP_EVENT_SESSION_ESTABLISH:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_SESSION_ESTABLISH - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code != HTTP_SUCCESS)
            {
                TransmissionGprs_Event_HttpPostFail(evt_code);
            }
            break;

        case HTTP_EVENT_RESPONE_STATE_LINE:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_RESPONE_STATE_LINE - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code == HTTP_SUCCESS)
            {
                // show response information
                result_int = ql_httpc_getinfo(client, HTTP_INFO_RESPONSE_CODE, &status_code);   // the 3-digit response status code
                if (result_int != 0)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_getinfo ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "ql_httpc_getinfo - Status code: %d", status_code);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }

#ifdef FUNCTION_IMPLEMENTED
                // show response information
                result_int = ql_httpc_getinfo(client, HTTP_INFO_DATE, &temp_buffer);
                if (result_int != 0)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "ql_httpc_getinfo ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "ql_httpc_getinfo - Date: \"%s\"", temp_buffer);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* get last synchronize time */
                    Synchronize_SynchronizeTime_Get(&synchronize_time);

                    /* get current RTC time */
                    result = Clock_GetTime(&current_rtc_time);

                    // extract actual GMT time from the header (format: "Tue, 13 Jan 2015 15:42:26 GMT")
                    result = Clock_ConverTimeString1ToClockTime(temp_buffer, &actual_time);   // GMT   time
                    if (result)
                    {
                        /* calculate elapsed    time since last synchronize */
                        /* calculate difference time with actual time       */
                        elapsed_time    = Clock_DifferenceRtcTimes(&synchronize_time.synchronize_time, &current_rtc_time);
                        difference_time = Clock_DifferenceRtcTimes(&actual_time                      , &current_rtc_time);

                        if (
                             (Http_Synchronize)                                                                                                            ||
                             ((elapsed_time    < -MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE) || (elapsed_time    > MIN_ELAPSED_TIME_SINCE_LAST_SYNCHRONIZE)) ||
                             ((difference_time < -MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME  ) || (difference_time > MIN_DIFFERENCE_TIME_WICTH_ACTUAL_TIME  ))
                           )
                        {
                            Synchronize_SynchronizeRtcTime(&actual_time, TRUE);
                        }
                    }

                    /* free allocated memory */
                    free(temp_buffer);
                }
#endif
            }
            break;

        case HTTP_EVENT_SESSION_DISCONNECT:
            snprintf(Http_DebugString, sizeof(Http_DebugString), "evt: HTTP_EVENT_SESSION_DISCONNECT - evt_code: %d", evt_code);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (evt_code != HTTP_SUCCESS)
            {
                TransmissionGprs_Event_HttpPostFail(0);
            }
            break;
    }
}




/*=============================================================================
 * Function   : Http_WipCallback_Http_HttpGetRxChannelDataTransfer
 *
 * Description: Handler for the HTTP GET RX data channel
 * Input      : - ev :
 *              - ctx:
 * Output     : -
 *=============================================================================*/
static int Http_WipCallback_Http_HttpGetRxChannelDataTransfer(http_client_t *client, void *arg, char *data, int size, unsigned char end)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - WIP HTTP GET \"RX\" - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the received answer */
    if (size > 0)
    {
        snprintf(Http_DebugString, sizeof(Http_DebugString), "Received answer: \"%s\"", data);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if      (   (                                             (sizeof(HTTP_GET_RX_ANSWER_OK        ) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_RX_ANSWER_OK        , (sizeof(HTTP_GET_RX_ANSWER_OK        ) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"OK\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetRxOk();
        }
        else if (   (                                             (sizeof(HTTP_GET_RX_ANSWER_REGFAIL_90) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_RX_ANSWER_REGFAIL_90, (sizeof(HTTP_GET_RX_ANSWER_REGFAIL_90) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 90 Numero Seriale terminale non esistente\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetTxFail();
        }
        else if (   (                                             (sizeof(HTTP_GET_RX_ANSWER_REGFAIL_91) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_RX_ANSWER_REGFAIL_91, (sizeof(HTTP_GET_RX_ANSWER_REGFAIL_91) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 91 Apikey gia' assegnata\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* generate random strings for host account 2 */
            Http_GenerateRandomHostAccount2();

            TransmissionGprs_Event_HttpGetTxFail();
        }
        else if (   (                                             (sizeof(HTTP_GET_RX_ANSWER_REGFAIL_98) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_RX_ANSWER_REGFAIL_98, (sizeof(HTTP_GET_RX_ANSWER_REGFAIL_98) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 98 Autenticazione fallita\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetRxFail();
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: other", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetRxFail();
        }
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "No received answer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        TransmissionGprs_Event_HttpGetRxFail();
    }


    return size;
}




/*=============================================================================
 * Function   : Http_WipCallback_Http_HttpGetTxChannelDataTransfer
 *
 * Description: Handler for the HTTP GET TX data channel
 * Input      : - ev :
 *              - ctx:
 * Output     : -
 *=============================================================================*/
static int Http_WipCallback_Http_HttpGetTxChannelDataTransfer(http_client_t *client, void *arg, char *data, int size, unsigned char end)
{
           u32                                    num_char;
           bool                                   string_error;

    static QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD  queue_sms_answer_to_crs_record;
    static LOGS__RX_COMMAND_RECORD                rx_command_record;

           REPORT__CONFIG__REPORT                 config_report;

           CLOCK__TIME                            time;
           CLOCK__TIME                            command_time_tx;

           bool                                   report;
           bool                                   command_valid;
           bool                                   answer;

           u8                                     num_answers;

           s32                                    command_id;
    static ascii                                  command_text[160 + 1];

    static ascii                                 *ptr_answer_texts[QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS];
    static ascii                                  answer_texts    [QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS][160 + 1];

           bool                                   parser_result;
           int                                    num_parameters;
           u8                                     i;

           bool                                   result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - WIP HTTP GET \"TX\" - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the received answer */
    if (size > 0)
    {
        snprintf(Http_DebugString, sizeof(Http_DebugString), "Received answer: \"%s\"", data);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if      ( /*(                                             (sizeof(HTTP_GET_TX_ANSWER_OK        ) - 1)  == size)     && */
                    (strncmp(data, HTTP_GET_TX_ANSWER_OK        , (sizeof(HTTP_GET_TX_ANSWER_OK        ) - 1)) == 0   )   )
        {
            if (size == 2)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"OK\""    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "No messages from the server", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                command_id = -1;
            }
            else
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"OK ...\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* extract the values from the string */

                string_error = FALSE;

                num_parameters = sscanf(data, "OK\r\n%ld\r\n%n", &command_id, (int *)&num_char);
                if (num_parameters != 1)
                    string_error = TRUE;

                if (
                     (data[size - 2] != '\r') ||
                     (data[size - 1] != '\n')
                   )
                {
                    string_error = TRUE;
                }

                data[size - 2] = 0x00;
                strncpy(command_text, (ascii *)(data + num_char), 160);
                command_text[160] = 0x00;

                if (!string_error)
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "command_id  : %ld"   , command_id  );
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    snprintf(Http_DebugString, sizeof(Http_DebugString), "command_text: \"%s\"", command_text);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    if (command_id != Http_LastCommandId.last_command_id)
                    {
                        if (strlen(command_text) > 0)
                        {
                            /* get the actual time (command time) */
                            result = Clock_GetTime(&command_time_tx);

                            /* parse the received SMS */
                            for (i = 0; i < QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS; i++)
                            {
                                answer_texts    [i][0] = 0x00;
                                ptr_answer_texts[i]    = &answer_texts[i][0];
                            }

                            /* parse the received message */
                            parser_result = ParserRx_ParseCommandText(FALSE, &command_time_tx, "", TRUE, FALSE, command_text, &report, &command_valid, &num_answers, ptr_answer_texts, 160);

                            if (num_answers > QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS)
                                num_answers = QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS;

                            if (parser_result)
                            {
                                /* parser OK */
                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "PARSER OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                /*------------------------------------------------------------------
                                 * Verify if the answer has to be transmitted
                                 *------------------------------------------------------------------*/
                                if (report)
                                {
                                    /* get the report configuration */
                                    Report_Config_Report_Get(&config_report);
                                    if (config_report.status)
                                    {
                                        /* report enabled */
                                        answer = TRUE;
                                    }
                                    else
                                    {
                                        /* report disabled */
                                        answer = FALSE;
                                    }
                                }
                                else
                                {
                                    answer = TRUE;
                                }

                                if (answer)
                                {
                                    /* get the actual time */
                                    result = Clock_GetTime(&time);

                                    for (i = 0; i < num_answers; i++)
                                    {
                                        strncpy(answer_texts[i], ptr_answer_texts[i], 160);
                                        answer_texts[i][160] = 0x00;
                                        if (strlen(answer_texts[i]) > 0)
                                        {
                                            /* build the SMS answer record */
                                            queue_sms_answer_to_crs_record.time = time;                                      /* time                       */
                                            strncpy(queue_sms_answer_to_crs_record.sms_phone_number, ""             ,  20);  /* SMS recipient phone number */
                                            strncpy(queue_sms_answer_to_crs_record.answer_text     , answer_texts[i], 160);  /* answer text                */
                                            queue_sms_answer_to_crs_record.sms_phone_number[ 20] = 0x00;
                                            queue_sms_answer_to_crs_record.answer_text     [160] = 0x00;

                                            /* put the SMS answer record in the SMS answer to CRS queue */
                                            result = Queue_SmsAnswerToCrs_PutRecord(&queue_sms_answer_to_crs_record);
                                        }
                                    }

                                    /* reset retransmission status */
                                    UserGsmRetx_ResetRetransmissionStatus();
                                }


                                /*------------------------------------------------------------------
                                 * Build and put the rx command record for the rx command log
                                 *------------------------------------------------------------------*/
                                if (command_valid)
                                {
                                    /* get actual time */
                                    result = Clock_GetTime(&time);

                                    /* build the rx command record */
                                    rx_command_record.command_type = LOGS__RX_COMMAND__COMMAND_TYPE__SMS;
                                    rx_command_record.time         = time;
                                    rx_command_record.command_exe  = TRUE;
                                    strncpy(rx_command_record.name        , ""          ,  14);
                                    strncpy(rx_command_record.phone_number, ""          ,  20);
                                    strncpy(rx_command_record.text        , command_text, 160);
                                    rx_command_record.name        [ 14] = 0x00;
                                    rx_command_record.phone_number[ 20] = 0x00;
                                    rx_command_record.text        [160] = 0x00;

                                    /* put the rx command record in the rx command log */
                                    result = Logs_RxCommmand_PutRecord(&rx_command_record);

                                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Created a rx commmand log record", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                                }
                            }
                        }

                        /* update last command ID */
                        Http_LastCommandId.last_command_id = command_id;

                        /* request the backup to flash objects */
                      //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__025_COMMAND_ID, PROGRAM_FLASH__FLASH_ID__025_COMMAND_ID__LAST_COMMAND_ID);
                    }
                }
                else
                {
                    command_id = -1;
                }
            }

            TransmissionGprs_Event_HttpGetTxOk(command_id);
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_90) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_90, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_90) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 90 Numero Seriale terminale non esistente\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetTxFail();
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_91) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_91, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_91) - 1)) == 0   )   )
        {
            /* generate random strings for host account 2 */
            Http_GenerateRandomHostAccount2();

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 91 Apikey gia' assegnata\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetTxFail();
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_94) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_94, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_94) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 94 Messaggio non associato a questo terminale\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetTxFail();
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_98) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_98, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_98) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 98 Autenticazione fallita\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetTxFail();
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: other", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpGetTxFail();
        }
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "No received answer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        TransmissionGprs_Event_HttpGetTxFail();
    }


    return size;
}




/*=============================================================================
 * Function   : Http_WipCallback_Http_HttpPostChannelDataTransfer
 *
 * Description: Handler for the HTTP POST data channel
 * Input      : - ev :
 *              - ctx:
 * Output     : -
 *=============================================================================*/
static int Http_WipCallback_Http_HttpPostChannelDataTransfer(http_client_t *client, void *arg, char *data, int size, unsigned char end)
{
    u32 num_bytes_uploaded;

    int num_parameters;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - WIP HTTP POST - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the received answer */
    if (size > 0)
    {
        snprintf(Http_DebugString, sizeof(Http_DebugString), "Received answer: \"%s\"", data);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if      ( /*(                                             (sizeof(HTTP_GET_TX_ANSWER_OK        ) - 1)  == size)     && */
                    (strncmp(data, HTTP_GET_TX_ANSWER_OK        , (sizeof(HTTP_GET_TX_ANSWER_OK        ) - 1)) == 0   )   )
        {
            if (size == 2)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"OK\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"OK ...\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* extract the values from the string */

                num_parameters = sscanf(data, "OK\r\n%ld bytes uploaded\r\n", &num_bytes_uploaded);
                if (num_parameters == 1)
                {
                    snprintf(Http_DebugString, sizeof(Http_DebugString), "num_bytes_uploaded: %ld", num_bytes_uploaded);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, Http_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
            }

            TransmissionGprs_Event_HttpPostOk();
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_90) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_90, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_90) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 90 Numero Seriale terminale non esistente\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpPostFail(0);
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_91) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_91, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_91) - 1)) == 0   )   )
        {
            /* generate random strings for host account 2 */
            Http_GenerateRandomHostAccount2();

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 91 Apikey gia' assegnata\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpPostFail(0);
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_94) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_94, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_94) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 94 Messaggio non associato a questo terminale\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpPostFail(0);
        }
        else if (   (                                             (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_98) - 1)  == size)     &&
                    (strncmp(data, HTTP_GET_TX_ANSWER_REGFAIL_98, (sizeof(HTTP_GET_TX_ANSWER_REGFAIL_98) - 1)) == 0   )   )
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: \"REGFAIL 98 Autenticazione fallita\"", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpPostFail(0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "Received answer: other", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_HttpPostFail(0);
        }
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_HTTP, DEBUG_TRACE_TYPE_LOW, "No received answer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        TransmissionGprs_Event_HttpPostFail(0);
    }


    return size;
}
