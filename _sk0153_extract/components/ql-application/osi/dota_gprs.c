/*=============================================================================
 * File       :  DOTA_GPRS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - GPRS manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_api_datacall.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "dota_ftp.h"
#include "dota_gprs.h"
#include "dota_main.h"
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* timeout values (ms) */
#define TIME_MS__DELAY_FTP_CONNECTION   (1000)    /* time delay FTP connection (ms) */

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING         200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
static       bool                           DotaGprs_GprsConnected = FALSE;

/* configuration - "GPRS APN" */
static       DOTA_GPRS__GPRS_APN_PARAMETERS DotaGprs_Config_GprsApnParameters;
static const DOTA_GPRS__GPRS_APN_PARAMETERS DotaGprs_Config_GprsApnParametersDefault =
{
    "",            // APN server
    "",            // APN user name
    "",            // APN password
};

/* configuration - "GPRS DNS" */
static       DOTA_GPRS__GPRS_DNS_PARAMETERS DotaGprs_Config_GprsDnsParameters;
static const DOTA_GPRS__GPRS_DNS_PARAMETERS DotaGprs_Config_GprsDnsParametersDefault =
{
    "",            // DNS 1
    "",            // DNS 2
};

/* debug string */
static       ascii                          DotaGprs_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* bearer handler */
static       int                            DotaGprs_BearerHandler_Gprs = 1;    /* GPRS bearer handler */

/* timer handler */
static       ql_timer_t                     DotaGprs_TimerHandler_DelayFtpConnection;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* start/stop */
       bool DotaGprs_GprsStartConnection(void);
       bool DotaGprs_GprsStopConnection (void);

/* info */
       bool DotaGprs_GprsIsConnected(void);

/* get/set "GPRS APN" configuration */
       void DotaGprs_Config_ApnParameters_GetDefault(DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data);
       void DotaGprs_Config_ApnParameters_Get       (DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data);
       void DotaGprs_Config_ApnParameters_Set       (DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data);
       bool DotaGprs_Config_ApnParameters_IsValid   (DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data);

/* get/set "GPRS DNS" configuration */
       void DotaGprs_Config_DnsParameters_GetDefault(DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data);
       void DotaGprs_Config_DnsParameters_Get       (DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data);
       void DotaGprs_Config_DnsParameters_Set       (DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data);
       bool DotaGprs_Config_DnsParameters_IsValid   (DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* GPRS event callback functions */
static void DotaGprs_AdlCallback_Gprs_Event(uint8_t sim_id, unsigned int ind_type, int profile_idx, bool result, void *ctx);

/* timer      callback functions */
static void DotaGprs_AdlCallback_Timer_DelayFtpConnection(void *ptr_context);




/*=============================================================================
 * Function   : DotaGprs_GprsStartConnection
 *
 * Description:  Starts the GPRS connection
 * Input      : -
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
bool DotaGprs_GprsStartConnection(void)
{
    ql_datacall_dns_info_s dns_pri;
    ql_datacall_dns_info_s dns_sec;

    ql_datacall_errcode_e  result;


    /* print the GPRS APN server name */
    snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "GPRS APN Server   : \"%s\"", DotaGprs_Config_GprsApnParameters.apn_server   );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the GPRS APN user name */
    snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "GPRS APN User Name: \"%s\"", DotaGprs_Config_GprsApnParameters.apn_user_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the GPRS APN password */
    snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "GPRS APN Password : \"%s\"", DotaGprs_Config_GprsApnParameters.apn_password );
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* print the DNS1 server */
    snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "GPRS DNS1 Server   : \"%s\"", DotaGprs_Config_GprsDnsParameters.dns_1);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the DNS2 server */
    snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "GPRS DNS2 User Name: \"%s\"", DotaGprs_Config_GprsDnsParameters.dns_2);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check if the GPRS APN is configured */
    if (strlen(DotaGprs_Config_GprsApnParameters.apn_server) == 0)
        return FALSE;


    /* open the GPRS bearer */
    result = ql_datacall_register_cb(0, DotaGprs_BearerHandler_Gprs, DotaGprs_AdlCallback_Gprs_Event, NULL);
    if (result != QL_DATACALL_SUCCESS)
    {
        snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_datacall_register_cb ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* configure the GPRS bearer */
    result = ql_set_data_call_asyn_mode(0, DotaGprs_BearerHandler_Gprs, 1);
    if (result != QL_DATACALL_SUCCESS)
    {
        snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_set_data_call_asyn_mode ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* start the GPRS connection */
    result = ql_start_data_call(0, DotaGprs_BearerHandler_Gprs, QL_PDP_TYPE_IP, DotaGprs_Config_GprsApnParameters.apn_server, NULL, NULL, 0);
    if (result != QL_DATACALL_SUCCESS)
    {
        snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_start_data_call ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_SystemError();
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_start_data_call OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* configure the GPRS bearer */
    if (
         (strlen(DotaGprs_Config_GprsDnsParameters.dns_1) > 0) &&
         (strlen(DotaGprs_Config_GprsDnsParameters.dns_1) > 0)
       )
    {
        /* DNS server configured */

        dns_pri.type = QL_PDP_TYPE_IP;
        ip4addr_aton(DotaGprs_Config_GprsDnsParameters.dns_1, &(dns_pri.ip4));

        dns_sec.type = QL_PDP_TYPE_IP;
        ip4addr_aton(DotaGprs_Config_GprsDnsParameters.dns_2, &(dns_sec.ip4));

        result = ql_datacall_set_dns_addr(0, DotaGprs_BearerHandler_Gprs, &dns_pri, &dns_sec);
        if (result != QL_DATACALL_SUCCESS)
        {
            snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_datacall_set_dns_addr ERROR: %d", result);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            DotaMain_SystemError();
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_datacall_set_dns_addr OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }


    return TRUE;
}




/*=============================================================================
 * Function   : DotaGprs_GprsStopConnection
 *
 * Description: Stop the GPRS connection
 * Input      : -
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
bool DotaGprs_GprsStopConnection(void)
{
    ql_datacall_errcode_e result;


    /* stop the GPRS connection */
    result = ql_stop_data_call(0, DotaGprs_BearerHandler_Gprs);
    if (result != QL_DATACALL_SUCCESS)
    {
        snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_stop_data_call ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_SystemError();
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_stop_data_call OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : DotaGprs_GprsIsConnected
 *
 * Description: check if GPRS is connected
 * Input      : -
 * Output     : - FALSE: GPRS is not connected
 *              - TRUE : GPRS is     connected
 *=============================================================================*/
bool DotaGprs_GprsIsConnected(void)
{
    return DotaGprs_GprsConnected;
}




/*===========================================================================
 * Function   : DotaGprs_Config_ApnParameters_GetDefault
 *
 * Description: get the default "GPRS APN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaGprs_Config_ApnParameters_GetDefault(DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data)
{
    *ptr_data = DotaGprs_Config_GprsApnParametersDefault;
}




/*===========================================================================
 * Function   : DotaGprs_Config_ApnParameters_Get
 *
 * Description: get the "GPRS APN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaGprs_Config_ApnParameters_Get(DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data)
{
    *ptr_data = DotaGprs_Config_GprsApnParameters;
}




/*===========================================================================
 * Function   : DotaGprs_Config_ApnParameters_Set
 *
 * Description: set the "GPRS APN" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaGprs_Config_ApnParameters_Set(DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data)
{
    DotaGprs_Config_GprsApnParameters = *ptr_data;
}




/*===========================================================================
 * Function   : DotaGprs_Config_ApnParameters_IsValid
 *
 * Description: check if the "GPRS APN" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DotaGprs_Config_ApnParameters_IsValid(DOTA_GPRS__GPRS_APN_PARAMETERS *ptr_data)
{
    if (strlen(ptr_data->apn_server   ) > DOTA_GPRS__MAX_LENGTH_GPRS_APN_SERVER   )
        return FALSE;

    if (strlen(ptr_data->apn_user_name) > DOTA_GPRS__MAX_LENGTH_GPRS_APN_USER_NAME)
        return FALSE;

    if (strlen(ptr_data->apn_password ) > DOTA_GPRS__MAX_LENGTH_GPRS_APN_PASSWORD )
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : DotaGprs_Config_DnsParameters_GetDefault
 *
 * Description: get the default "GPRS DNS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaGprs_Config_DnsParameters_GetDefault(DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data)
{
    *ptr_data = DotaGprs_Config_GprsDnsParametersDefault;
}




/*===========================================================================
 * Function   : DotaGprs_Config_DnsParameters_Get
 *
 * Description: get the "GPRS DNS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaGprs_Config_DnsParameters_Get(DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data)
{
    *ptr_data = DotaGprs_Config_GprsDnsParameters;
}




/*===========================================================================
 * Function   : DotaGprs_Config_DnsParameters_Set
 *
 * Description: set the "GPRS DNS" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaGprs_Config_DnsParameters_Set(DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data)
{
    DotaGprs_Config_GprsDnsParameters = *ptr_data;
}




/*===========================================================================
 * Function   : DotaGprs_Config_DnsParameters_IsValid
 *
 * Description: check if the "GPRS DNS" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DotaGprs_Config_DnsParameters_IsValid(DOTA_GPRS__GPRS_DNS_PARAMETERS *ptr_data)
{
    if (strlen(ptr_data->dns_1) > DOTA_GPRS__MAX_LENGTH_GPRS_DNS_1)
        return FALSE;

    if (strlen(ptr_data->dns_2) > DOTA_GPRS__MAX_LENGTH_GPRS_DNS_2)
        return FALSE;

    return TRUE;
}




/*=============================================================================
 * Function   : DotaGprs_AdlCallback_Gprs_Event
 *
 * Description: GPRS Bearer Handler
 * Input      : - bearer_handler: bearer handler
 *              - event         : event
 *              - ctx           :
 * Output     : -
 *=============================================================================*/
static void DotaGprs_AdlCallback_Gprs_Event(uint8_t sim_id, unsigned int ind_type, int profile_idx, bool result, void *ctx)
{
    QlOSStatus err;


    if (profile_idx == DotaGprs_BearerHandler_Gprs)
    {
        switch (ind_type)
        {
            /* IP communication ready */
            case QUEC_DATACALL_ACT_RSP_IND:
                if (result)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "QUEC_DATACALL_ACT_RSP_IND", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DotaGprs_GprsConnected = TRUE;

                    /* start the FTP connection */
                    if (DotaMain_GetAutoModeStatus())
                    {
                        /* start timer for FTP connection delay */
                        err = ql_rtos_timer_create(&DotaGprs_TimerHandler_DelayFtpConnection, Boot_TaskRef_Dota, DotaGprs_AdlCallback_Timer_DelayFtpConnection, NULL);
                        if (err != QL_OSI_SUCCESS)
                        {
                            snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_rtos_timer_create ERROR: %d", err);
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                        err = ql_rtos_timer_start(DotaGprs_TimerHandler_DelayFtpConnection, TIME_MS__DELAY_FTP_CONNECTION, FALSE);
                        if (err != QL_OSI_SUCCESS)
                        {
                            snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "ql_rtos_timer_start ERROR: %d", err);
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                            DotaMain_SystemError();
                        }
                        else
                        {
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_rtos_timer_start OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                    }
                }
                else
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "WIP_BEV_CONN_FAILED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DotaMain_ResetModule();

                    DotaGprs_GprsConnected = FALSE;

                    if (DotaMain_GetAutoModeStatus())
                    {
                        /* stop the FTP connection */
                        result = DotaFtp_FtpStopConnection();
                        if (!result)
                            DotaMain_ResetModule();

                        /* stop the GPRS connection */
                        result = DotaGprs_GprsStopConnection();
                        if (!result)
                            DotaMain_ResetModule();
                    }
                }
                break;


            /* Disconnection completed after ql_stop_data_call was called */
            case QUEC_DATACALL_DEACT_RSP_IND:
                if (result)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "WIP_BEV_STOPPED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DotaMain_ResetModule();

                    DotaGprs_GprsConnected = FALSE;

                    if (DotaMain_GetAutoModeStatus())
                    {
                        /* stop the FTP connection */
                        result = DotaFtp_FtpStopConnection();
                        if (!result)
                            DotaMain_ResetModule();

                        /* stop the GPRS connection */
                        result = DotaGprs_GprsStopConnection();
                        if (!result)
                            DotaMain_ResetModule();
                    }
                }
                break;


            /* IP communication terminated */
            case QUEC_DATACALL_PDP_DEACTIVE_IND:
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "WIP_BEV_IP_DISCONNECTED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DotaMain_ResetModule();

                DotaGprs_GprsConnected = FALSE;

                if (DotaMain_GetAutoModeStatus())
                {
                    /* stop the FTP connection */
                    result = DotaFtp_FtpStopConnection();
                    if (!result)
                        DotaMain_ResetModule();

                    /* stop the GPRS connection */
                    result = DotaGprs_GprsStopConnection();
                    if (!result)
                        DotaMain_ResetModule();
                }
                break;


            /* Unknown event */
            default:
                snprintf(DotaGprs_DebugString, sizeof(DotaGprs_DebugString), "QUEC DATACALL EVENT: %u", ind_type);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DotaMain_SystemError();
                break;
        }
    }
}




/*===========================================================================
 * Function    : DotaGprs_AdlCallback_Timer_DelayFtpConnection
 *
 * Description :
 * Input       : - ptr_context:
 * Output      : -
 *===========================================================================*/
static void DotaGprs_AdlCallback_Timer_DelayFtpConnection(void *ptr_context)
{
    bool result;


    /* start FTP connection */
    result = DotaFtp_FtpStartConnection();
    if (!result)
        DotaMain_ResetModule();
}
