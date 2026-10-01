/*=============================================================================
 * File       :  DOTA_GPRS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - GPRS manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DOTA_GPRS_H__


#define __DOTA_GPRS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* max GPRS APN parameters string length */
#define DOTA_GPRS__MAX_LENGTH_GPRS_APN_SERVER      40
#define DOTA_GPRS__MAX_LENGTH_GPRS_APN_USER_NAME   40
#define DOTA_GPRS__MAX_LENGTH_GPRS_APN_PASSWORD    40

/* max GPRS DNS parameters string length */
#define DOTA_GPRS__MAX_LENGTH_GPRS_DNS_1           20
#define DOTA_GPRS__MAX_LENGTH_GPRS_DNS_2           20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "GPRS APN" */
typedef struct
{
    ascii apn_server   [DOTA_GPRS__MAX_LENGTH_GPRS_APN_SERVER    + 1];
    ascii apn_user_name[DOTA_GPRS__MAX_LENGTH_GPRS_APN_USER_NAME + 1];
    ascii apn_password [DOTA_GPRS__MAX_LENGTH_GPRS_APN_PASSWORD  + 1];
} DOTA_GPRS__GPRS_APN_PARAMETERS;

/* configuration - "GPRS DNS" */
typedef struct
{
    ascii dns_1        [DOTA_GPRS__MAX_LENGTH_GPRS_DNS_1         + 1];
    ascii dns_2        [DOTA_GPRS__MAX_LENGTH_GPRS_DNS_2         + 1];
} DOTA_GPRS__GPRS_DNS_PARAMETERS;




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




#endif
