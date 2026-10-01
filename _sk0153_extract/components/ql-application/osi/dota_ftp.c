/*=============================================================================
 * File       :  DOTA_FTP.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - FTP manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/


/*
 *** PROVE DA FARE ***
   *** GPRS ***
 - provare APN GPRS errato
 - provare a impostare DNS1 e DNS2                                  FTP numerico: OK
                                                                    FTP stringa : NON VA (con DNS di OpenDNS)    (SIM TIM)  DotaFtp_AdlCallback_Ftp_ChannelConnection  -  WIP_CEV_ERROR -990 o -993 (-986 con DNS errati)
                                                                    FTP stringa : NON VA (con DNS stringa vuota) (SIM TIM)  DotaFtp_AdlCallback_Ftp_ChannelConnection  -  WIP_CEV_ERROR -990 o -993 (-986 con DNS errati)
   *** FTP ***
 - provare server FTP in formato testo e in formato numerico        OK
 - provare server FTP non esistente                                 DotaFtp_AdlCallback_Ftp_ChannelConnection  -                WIP_CEV_ERROR -990 o -993
 - provare server FTP ok, ma user errato                            DotaFtp_AdlCallback_Ftp_ChannelConnection  -                WIP_CEV_ERROR -530
 - provare server FTP ok, ma password errata                        DotaFtp_AdlCallback_Ftp_ChannelConnection  -                WIP_CEV_ERROR -530
 - provare FTP anonimo (no user e pwd) e vedere se scarica          DotaFtp_AdlCallback_Ftp_ChannelConnection  -                WIP_CEV_ERROR -530
 - provare server FTP ok, ma file non esistente (path file errato)  DotaFtp_AdlCallback_Ftp_ChannelConnection  - WIP_CEV_OPEN - WIP_CEV_ERROR -550 (wip_getFileSize)
 - provare server FTP ok, ma file non esistente (nome file errato)  DotaFtp_AdlCallback_Ftp_ChannelConnection  - WIP_CEV_OPEN - WIP_CEV_ERROR -550 (wip_getFileSize)
 - provare FTP passivo                                              OK (default)
 - provare FTP attivo                                               NON VA - si resetta a metà FTP (ADL_INIT_POWER_ON)
 - provare file con estensione non dwl                              OK
 - provare file con estensione file dwl non valido (corrotto)       ADL_INIT_DOWNLOAD_ERROR, riparte vecchio file dwl
 - provare file dwl di diversioni dimensioni, anche grandi          ???

 - provare con SIM WIND, Vodafone, TIM
 - scollegare antenna durante connessione a APN
 - scollegare antenna durante FTP
*/


/*
 *** DA FARE ***
 - gestire i CEV_ERROR
 - DotaFtp_FtpStopConnection, dove mettere i wip_close (verificare la Finalizer)
 - timeout attesa eventi (GPRS, FTP)
 - provare a cambiare la size dei buffer FTP (dimensione maggiore di 536)
 - aggiungere comando AT+WOPEN=6 per leggere size A&D (si usa adl_adGetState)
 - cosa è la stringa Account per FTP???
*/


/*
 *** ERRORI ***
 - valore ritornato a chiamata di libreria adl_ o wip_
        - valore di errore
        - valore ignoto
        - valore di errore possibile
 - risposta a comando AT inviato
        - risposta di errore        (ERROR)
        - risposta non prevista
        - risposta ignota
        - risposta prevista, ma con parametri ignoti
        - risposta non ricevuta entro tempo T
 - evento callback
        - evento di errore
        - evento non previsto
        - evento ignoto
        - evento non ricevuta entro tempo T
 -
*/


/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>
#include <stdio.h>

/* API      includes */
#include "ql_api_datacall.h"
#include "ql_api_osi.h"
#include "ql_ftp_client.h"

/* user     includes */
#include "debug_my.h"
#include "dota_ftp.h"
#include "dota_gprs.h"
#include "dota_main.h"
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* max FTP full file name string length (path + name) */
#define MAX_LENGTH_FTP_FULL_FILE_NAME   (DOTA_FTP__MAX_LENGTH_FTP_FILE_PATH + DOTA_FTP__MAX_LENGTH_FTP_FILE_NAME)

/* sizes */
#define MAX_FTP_FILE_SIZE               (440 * 1024)  /* max file size (bytes) */    // 440 KB
#define FTP_BUFFER_DATA_SIZE            1500          /* FTP buffer data size  */

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING         200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* configuration - "FTP" parameters */
static       DOTA_FTP__FTP_PARAMETERS DotaFtp_Config_FtpParameters;
static const DOTA_FTP__FTP_PARAMETERS DotaFtp_Config_FtpParametersDefault =
{
    DOTA_FTP__FTP_MODE_PASSIVE,  // FTP mode
    21,                          // FTP port
    "",                          // FTP server
    "",                          // FTP user name
    "",                          // FTP password
    "",                          // FTP file path
    "",                          // FTP file name
};

/* indication of FTP file download done */
static       bool                     DotaFtp_FileDownloadDone = FALSE;

/* FTP full file name string (path + name) */
static       ascii                    DotaFtp_FtpFullFileName[MAX_LENGTH_FTP_FULL_FILE_NAME + 1] = "";

/* FTP info */
static       double                   DotaFtp_FtpFileSize  = 0;   // file size (bytes)
static       u32                      DotaFtp_FtpBytesRead = 0;   // number of bytes read

/* debug string */
static       ascii                    DotaFtp_DebugString[MAX_LENGTH_DEBUG_STRING  + 1];

/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* FTP handlers */
static       void                    *DotaFtp_FtpHandler_ChannelConnection = NULL;   // FTP "Connection" channel




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

#ifdef FUNCTION_IMPLEMENTED
/* read data */
static u32  DotaFtp_FtpReadData(void);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* FTP Callback functions */
static void DotaFtp_AdlCallback_Ftp_ChannelDataTransfer(wip_event_t *event, void *ctx);
#endif




/*=============================================================================
 * Function   : DotaFtp_FtpStartConnection
 *
 * Description: Start the FTP connection
 * Input      : -
 * Output     : - FALSE: error
 *              - TRUE : OK
 *=============================================================================*/
bool DotaFtp_FtpStartConnection(void)
{
          u16  sim_cid;
          u16  port_mode;

          bool gprs_connected;

          s64  os_time_stamp_start;
          s64  os_time_stamp_end;

          s64  diff_time_ms;

    const char local_filename[] = "UFS:fota.pac";

          int  result;


    /* print FTP mode */
    if (DotaFtp_Config_FtpParameters.ftp_mode == DOTA_FTP__FTP_MODE_PASSIVE)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "FTP mode     : PASSIVE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    else
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "FTP mode     : ACTIVE" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print FTP port */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP port     : %d"    , DotaFtp_Config_FtpParameters.ftp_port     );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* print FTP server */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP server   : \"%s\"", DotaFtp_Config_FtpParameters.ftp_server   );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print FTP user name */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP user name: \"%s\"", DotaFtp_Config_FtpParameters.ftp_user_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print FTP password */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP password : \"%s\"", DotaFtp_Config_FtpParameters.ftp_password );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print FTP file path */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP file path: \"%s\"", DotaFtp_Config_FtpParameters.ftp_file_path);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print FTP file name */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP file name: \"%s\"", DotaFtp_Config_FtpParameters.ftp_file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* print the full file name (path + file) */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP full file name: %s", DotaFtp_FtpFullFileName);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* indication of FTP file download done */
    DotaFtp_FileDownloadDone = FALSE;


    /* check if the FTP is configured */
    if (
         (strlen(DotaFtp_Config_FtpParameters.ftp_server   ) == 0) ||
         (strlen(DotaFtp_Config_FtpParameters.ftp_file_name) == 0)
       )
    {
        return FALSE;
    }


    /* GPRS connection is not available, do not start the FTP connection */
    gprs_connected = DotaGprs_GprsIsConnected();
    if (!gprs_connected)
        return FALSE;


    /* create the FTP channel */
    DotaFtp_FtpHandler_ChannelConnection = ql_ftp_client_new();
    if (DotaFtp_FtpHandler_ChannelConnection == NULL)
    {
        /* channel not created, return an error */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_ftp_client_new ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_ResetModule();
    }

    /* initiate a connection request to the FTP server */
    if (DotaFtp_Config_FtpParameters.ftp_mode == DOTA_FTP__FTP_MODE_PASSIVE)
        port_mode = 1;
    else
        port_mode = 0;

    ql_bind_sim_and_profile(0, 1, &sim_cid);

    ql_ftp_client_setopt(DotaFtp_FtpHandler_ChannelConnection, QL_FTP_CLIENT_SIM_CID      , sim_cid  );
    ql_ftp_client_setopt(DotaFtp_FtpHandler_ChannelConnection, QL_FTP_CLIENT_OPT_PDP_CID  , 1        );
    ql_ftp_client_setopt(DotaFtp_FtpHandler_ChannelConnection, QL_FTP_CLIENT_OPT_START_POS, 0        );
    ql_ftp_client_setopt(DotaFtp_FtpHandler_ChannelConnection, QL_FTP_CLIENT_PORT_MODE    , port_mode);

    result = ql_ftp_client_open(DotaFtp_FtpHandler_ChannelConnection,
                                DotaFtp_Config_FtpParameters.ftp_server,                        // FTP server
                                DotaFtp_Config_FtpParameters.ftp_user_name,                     // FTP user name
                                DotaFtp_Config_FtpParameters.ftp_password);                     // FTP password

    /* Connection channel is open, FTP session established */

    DotaFtp_FtpFileSize  = 0;
    DotaFtp_FtpBytesRead = 0;

    /* build the file full file name (path + name) */
    strncpy(DotaFtp_FtpFullFileName, DotaFtp_Config_FtpParameters.ftp_file_path, (MAX_LENGTH_FTP_FULL_FILE_NAME                                  ));   /* copy the file path */
    DotaFtp_FtpFullFileName[MAX_LENGTH_FTP_FULL_FILE_NAME] = 0x00;
    strncat(DotaFtp_FtpFullFileName, DotaFtp_Config_FtpParameters.ftp_file_name, (MAX_LENGTH_FTP_FULL_FILE_NAME - strlen(DotaFtp_FtpFullFileName)));   /* copy the file name */

    /* print the full file name (path + file) */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP full file name: %s", DotaFtp_FtpFullFileName);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* get the file size */
    result = ql_ftp_client_size(DotaFtp_FtpHandler_ChannelConnection, DotaFtp_FtpFullFileName, &DotaFtp_FtpFileSize);
    if (result)
    {
        /* get file size error */
        snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "ql_ftp_client_size ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_ResetModule();
    }

    /* file size received */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "File size: %f bytes", DotaFtp_FtpFileSize);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    if (DotaFtp_FtpFileSize > MAX_FTP_FILE_SIZE)
    {
        /* the file size is not OK (too big) */
        DotaMain_ResetModule();
    }

    /* get the OS time stamp (start of FTP) */
    os_time_stamp_start = ql_rtos_up_time_ms();

    /* get the file */
    result = ql_ftp_client_get_ex(DotaFtp_FtpHandler_ChannelConnection, DotaFtp_FtpFullFileName, (char *)local_filename, NULL, NULL);
    if (result)
    {
        /* get file error */
        snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "ql_ftp_client_get_ex ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_ResetModule();
    }

    /* the whole file has been read */

    /* print the file size (bytes) */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "File size : %7.0f", DotaFtp_FtpFileSize );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the number of bytes read */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "Bytes read: %7ld", DotaFtp_FtpBytesRead);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* get the OS time stamp (end of FTP) */
    /* calculate the total FTP time */
    os_time_stamp_end = ql_rtos_up_time_ms();
    if (os_time_stamp_end >= os_time_stamp_start)
        diff_time_ms =  (os_time_stamp_end   - os_time_stamp_start);
    else
        diff_time_ms = -(os_time_stamp_start - os_time_stamp_end  );

    /* print the total FTP time */
    snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP total time: %lld ms", diff_time_ms);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* indication of FTP file download done */
    DotaFtp_FileDownloadDone = TRUE;


    return TRUE;
}




/*=============================================================================
 * Function   : DotaFtp_FtpStopConnection
 *
 * Description: Stop the FTP connection
 * Input      : -
 * Output     : - FALSE: error
 *              - TRUE : OK
 *=============================================================================*/
bool DotaFtp_FtpStopConnection(void)
{
    int result;


    /* close the FTP "Connection" channel */
    result = ql_ftp_client_close(DotaFtp_FtpHandler_ChannelConnection);
    if (result != 0)
    {
        snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "ql_ftp_client_close ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_SystemError();
    }

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_ftp_client_close OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return TRUE;
}




/*=============================================================================
 * Function   : DotaFtp_FtpFileDownloadDone
 *
 * Description: verify if the FTP file download is done
 * Input      : -
 * Output     : - FALSE: FTP file download not done
 *              - TRUE : FTP file download     done
 *=============================================================================*/
bool DotaFtp_FtpFileDownloadDone(void)
{
    return DotaFtp_FileDownloadDone;
}




/*===========================================================================
 * Function   : DotaFtp_Config_FtpParameters_GetDefault
 *
 * Description: get the default "FTP" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaFtp_Config_FtpParameters_GetDefault(DOTA_FTP__FTP_PARAMETERS *ptr_data)
{
    *ptr_data = DotaFtp_Config_FtpParametersDefault;
}




/*===========================================================================
 * Function   : DotaFtp_Config_FtpParameters_Get
 *
 * Description: get the "FTP" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaFtp_Config_FtpParameters_Get(DOTA_FTP__FTP_PARAMETERS  *ptr_data)
{
    *ptr_data = DotaFtp_Config_FtpParameters;
}




/*===========================================================================
 * Function   : DotaFtp_Config_FtpParameters_Set
 *
 * Description: set the "FTP" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaFtp_Config_FtpParameters_Set(DOTA_FTP__FTP_PARAMETERS *ptr_data)
{
    DotaFtp_Config_FtpParameters = *ptr_data;
}




/*===========================================================================
 * Function   : DotaFtp_Config_FtpParameters_IsValid
 *
 * Description: check if the "FTP" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DotaFtp_Config_FtpParameters_IsValid(DOTA_FTP__FTP_PARAMETERS *ptr_data)
{
    if (
         (ptr_data->ftp_mode != DOTA_FTP__FTP_MODE_PASSIVE) &&
         (ptr_data->ftp_mode != DOTA_FTP__FTP_MODE_ACTIVE )
       )
    {
        return FALSE;
    }

    if (strlen(ptr_data->ftp_server   ) > DOTA_FTP__MAX_LENGTH_FTP_SERVER   )
        return FALSE;

    if (strlen(ptr_data->ftp_user_name) > DOTA_FTP__MAX_LENGTH_FTP_USER_NAME)
        return FALSE;

    if (strlen(ptr_data->ftp_password ) > DOTA_FTP__MAX_LENGTH_FTP_PASSWORD )
        return FALSE;

    if (strlen(ptr_data->ftp_file_path) > DOTA_FTP__MAX_LENGTH_FTP_FILE_PATH)
        return FALSE;

    if (strlen(ptr_data->ftp_file_name) > DOTA_FTP__MAX_LENGTH_FTP_FILE_NAME)
        return FALSE;

    return TRUE;
}




#ifdef FUNCTION_IMPLEMENTED
/*=============================================================================
 * Function   : DotaFtp_FtpReadData
 *
 * Description: read data (a part of the file) on the FTP channel and write it
 *              to A&D memory
 * Input      : -
 * Output     : - number of bytes read
 *=============================================================================*/
static u32 DotaFtp_FtpReadData(void)
{
    static u8   data_buffer[FTP_BUFFER_DATA_SIZE];
           u32  num_bytes;
           int  len;
           bool result;


    num_bytes = 0;

    /* read data on the FTP channel until data is available */
    do
    {
        /* read data */
        len = wip_read(DotaFtp_FtpHandler_ChannelDataTransfer, (void *)data_buffer, sizeof(data_buffer));
        if (len < 0)
        {
            snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "wip_read ERROR: %d", len);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            DotaMain_SystemError();
        }
        else
        {
            snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "wip_read OK: %d"   , len);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }


        /* write read data to A&D memory (if it has read data) */
        if (len > 0)
        {
            num_bytes += len;

            /* write data to A&D memory */
            result = DotaAnD_AnDWriteData(data_buffer, len);
            if (!result)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DotaAnD_AnDWriteData ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
          //else
          //{
          //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DotaAnD_AnDWriteData OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
          //}

          //if (!result)  //@@@ TO DO BETTER
          //    break;    //@@@ TO DO BETTER
        }
    }
    while (len > 0);


    return num_bytes;
}




/*=============================================================================
 * Function   : DotaFtp_AdlCallback_Ftp_ChannelDataTransfer
 *
 * Description:
 * Input      : - event:
 *              - ctx  :
 * Output     : -
 *=============================================================================*/
static void DotaFtp_AdlCallback_Ftp_ChannelDataTransfer(wip_event_t *event, void *ctx)
{
    u32 num_bytes_read;


    switch (event->kind)
    {
        case WIP_CEV_READ:
            snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "WIP_CEV_READ"                  );
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* data transfer channel is open */

            /* read the data from FTP */
            num_bytes_read = DotaFtp_FtpReadData();

            DotaFtp_FtpBytesRead += num_bytes_read;

            /* print FTP info */
            snprintf(DotaFtp_DebugString, sizeof(DotaFtp_DebugString), "FTP info: %7ld/%ld", DotaFtp_FtpBytesRead, DotaFtp_FtpFileSize);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaFtp_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}
#endif
