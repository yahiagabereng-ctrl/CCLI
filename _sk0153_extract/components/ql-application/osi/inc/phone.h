/*=============================================================================
 * File       :  PHONE.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - phone manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef _PHONE_H


#define _PHONE_H




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* SMS delay unit */
#define PHONE__SMS_DELAY_UNIT__SECOND    0    /* seconds */   // not used
#define PHONE__SMS_DELAY_UNIT__MINUTE    1    /* minutes */   // not used
#define PHONE__SMS_DELAY_UNIT__HOUR      2    /* hours   */


/*-----------------------------------------------------------------------------
 * maximum string lengths
 *-----------------------------------------------------------------------------*/
/* maximum GPRS APN parameters string length */
#define PHONE__MAX_LENGTH_GPRS_APN_SERVER             40     /* GPRS APN server    */
#define PHONE__MAX_LENGTH_GPRS_APN_USER_NAME          40     /* GPRS APN user name */
#define PHONE__MAX_LENGTH_GPRS_APN_PASSWORD           40     /* GPRS APN password  */

/* maximum GPRS DNS parameters string length */
#define PHONE__MAX_LENGTH_GPRS_DNS_1                  20     /* GPRS DNS1 server */
#define PHONE__MAX_LENGTH_GPRS_DNS_2                  20     /* GPRS DNS2 server */

/* maximum phone ID string lenght  */
#define PHONE__LEN_MAX_MANUFACTURER                   30     /* manufacturer                                   */
#define PHONE__LEN_MAX_MODEL                          30     /* model                                          */
#define PHONE__LEN_MAX_SW_REVISION                    60     /* SW revision                                    */
#define PHONE__LEN_MAX_HW_REVISION                    30     /* HW revision                                    */
#define PHONE__LEN_MAX_OPEN_AT_LIB_INT_REVISION       15     /* internal Open AT library revision (AT+WOPEN=2) */
#define PHONE__LEN_MAX_OPEN_AT_LIB_EXT_REVISION       15     /* external Open AT library revision (AT+WOPEN=2) */
#define PHONE__LEN_MAX_IMEI                           15     /* IMEI (International Mobile Equipment Identity) */
#define PHONE__LEN_MAX_SERIAL_NUMBER                  30     /* serial number                                  */
#define PHONE__LEN_MAX_PRODUCTION_DATE                30     /* production date                                */

/* maximum SIM ID string lenght */
#define PHONE__LEN_MAX_IMSI                           15     /* SIM card IMSI    (International Mobile Subscriber Identity) */
#define PHONE__LEN_MAX_EF_CCID                        25     /* SIM card EF-CCID (Card Identification                     ) */

/* maximum SW component ID string lenght */
#define PHONE__LEN_MAX_FILE_COMPONENT                 50     /* component type             */
#define PHONE__LEN_MAX_FILE_VERSION                   50     /* component software version */
#define PHONE__LEN_MAX_FILE_NAME                      50     /* component name             */
#define PHONE__LEN_MAX_FILE_COMPANY                   50     /* component company          */
#define PHONE__LEN_MAX_FILE_SIZE                      10     /* component size in bytes    */
#define PHONE__LEN_MAX_FILE_TIME_STAMP                20     /* component timestamp        */
#define PHONE__LEN_MAX_FILE_CHECKSUM                  20     /* component checksum         */
#define PHONE__LEN_MAX_FILE_OFFSET                    10     /* component offset address   */

/* maximum SW sub component ID string lenght */
#define PHONE__LEN_MAX_FILE_SUB_COMPONENT             50     /* subcomponent name    */
#define PHONE__LEN_MAX_FILE_SUB_COMPONENT_VERSION     50     /* subcomponent version */


/*-----------------------------------------------------------------------------
 * phone SW component and sub component
 *-----------------------------------------------------------------------------*/
/* number of SW component */
#define PHONE__NUM_SW_COMPONENTS                      10     /* number of SW components    */
#define PHONE__NUM_SW_SUBCOMPONENTS                   10     /* number of SW subcomponents */


/*-----------------------------------------------------------------------------
 * SIM PIN
 *-----------------------------------------------------------------------------*/
/* SIM status */
#define PHONE__SIM_STATUS__UNKNOWN                    0      /* unknown  */
#define PHONE__SIM_STATUS__READY                      1      /* ready    */
#define PHONE__SIM_STATUS__SIM_PIN                    2      /* need PIN */
#define PHONE__SIM_STATUS__SIM_PUK                    3      /* need PUK */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * IDs
 *-----------------------------------------------------------------------------*/
/* phone info - phone IDs */
typedef struct
{
    /* model */
    ascii manufacturer            [PHONE__LEN_MAX_MANUFACTURER               + 1];     // manufacturer                      (AT+CGMI)
    ascii model                   [PHONE__LEN_MAX_MODEL                      + 1];     // model                             (AT+CGMM)

    /* revision */
    ascii sw_revision             [PHONE__LEN_MAX_SW_REVISION                + 1];     // firmware                 revision (AT+CGMR)
    ascii hw_revision             [PHONE__LEN_MAX_HW_REVISION                + 1];     // hardware                 revision (AT+WHWV)
    ascii open_at_lib_revision_int[PHONE__LEN_MAX_OPEN_AT_LIB_INT_REVISION   + 1];     // internal Open AT library revision (AT+WOPEN=2)
    ascii open_at_lib_revision_ext[PHONE__LEN_MAX_OPEN_AT_LIB_EXT_REVISION   + 1];     // external Open AT library revision (AT+WOPEN=2)

    /* serial number */
    ascii imei                    [PHONE__LEN_MAX_IMEI                       + 1];     // IMEI                              (AT+CGSN)
    ascii serial_number           [PHONE__LEN_MAX_SERIAL_NUMBER              + 1];     // serial number                     (AT+WMSN)

    /* date of production */
    ascii date_of_production      [PHONE__LEN_MAX_PRODUCTION_DATE            + 1];     // date of production                (AT+WDOP)
} PHONE__PHONE_ID;


/* phone info - SIM IDs */
typedef struct
{
    ascii imsi                    [PHONE__LEN_MAX_IMSI                       + 1];     // IMSI    (International Mobile Subscriber Identity) (AT+CIMI)
    ascii ef_ccid                 [PHONE__LEN_MAX_EF_CCID                    + 1];     // EF-CCID (Card Identification                     ) (AT+CCID)
} PHONE__SIM_ID;


/* phone SW component IDs */
typedef struct
{
    ascii component               [PHONE__LEN_MAX_FILE_COMPONENT             + 1];     // component type
    ascii version                 [PHONE__LEN_MAX_FILE_VERSION               + 1];     // component software version
    ascii name                    [PHONE__LEN_MAX_FILE_NAME                  + 1];     // component name
    ascii company                 [PHONE__LEN_MAX_FILE_COMPANY               + 1];     // component company
    ascii size                    [PHONE__LEN_MAX_FILE_SIZE                  + 1];     // component size in bytes
    ascii time_stamp              [PHONE__LEN_MAX_FILE_TIME_STAMP            + 1];     // component timestamp
    ascii checksum                [PHONE__LEN_MAX_FILE_CHECKSUM              + 1];     // component checksum
    ascii offset                  [PHONE__LEN_MAX_FILE_OFFSET                + 1];     // component offset address
} PHONE__PHONE_SW_COMPONENT_ID;


/* phone SW subcomponent IDs */
typedef struct
{
    ascii sub_component           [PHONE__LEN_MAX_FILE_SUB_COMPONENT         + 1];     // subcomponent name
    ascii sub_component_version   [PHONE__LEN_MAX_FILE_SUB_COMPONENT_VERSION + 1];     // subcomponent version
} PHONE__PHONE_SW_SUBCOMPONENT_ID;


/* phone SW component IDs */
typedef struct
{
    PHONE__PHONE_SW_COMPONENT_ID    sw_component_id   [PHONE__NUM_SW_COMPONENTS   ];
    PHONE__PHONE_SW_SUBCOMPONENT_ID sw_subcomponent_id[PHONE__NUM_SW_SUBCOMPONENTS];
} PHONE__PHONE_SW_COMPONENTS_ID;


/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* phone info - status */
typedef struct
{
    /* network registration */
    u8    creg;                                         // GSM  network registration status                 (AT+CREG?)
    u8    cgreg;                                        // GPRS network registration status                 (AT+CGREG?)

    /* GSM network operator */
    ascii operator_long_string [32 + 1];                // GSM network operator - long  alphanumeric format (AT+COPS?)
    ascii operator_short_string[10 + 1];                // GSM network operator - short alphanumeric format (AT+COPS?)
    ascii operator_numeric     [ 6 + 1];                // GSM network operator - numeric            format (AT+COPS?)

    /* signal quality */
    u8    rssi;                                         // RSSI (received signal strength)                  (AT+CSQ)
    u8    ber;                                          // BER  (channel bit error rate  )                  (AT+CSQ)

    /* local information */
    ascii mcc[3 + 1];                                   // MCC (Mobile Country Code)
    ascii mnc[3 + 1];                                   // MNC (Mobile Network Code)
    ascii lac[4 + 1];                                   // LAC (Location Area  Code)
    ascii ci [8 + 1];                                   // CI  (Cell Identifier)
} PHONE__PHONE_STATUS;


/* phone info - activity */
typedef struct
{
    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    u32   n_sms_rx;                                     // number of SMS received
    u32   n_sms_tx_ok;                                  // number of SMS transmitted with   success
    u32   n_sms_tx_fail;                                // number of SMS transmitted with insuccess


    /* voice call rx counters */
    u32   n_call_voice_rx_answered_ok;                  // number of voice calls received answered with   success
    u32   n_call_voice_rx_answered_fail;                // number of voice calls received answered with insuccess
    u32   n_call_voice_rx_missed;                       // number of voice calls received missed
    u32   n_call_voice_rx_refused;                      // number of voice calls received refused

    /* voice call tx counters */
    u32   n_call_voice_tx_ok;                           // number of voice calls done with   success
    u32   n_call_voice_tx_busy;                         // number of voice calls done busy
    u32   n_call_voice_tx_refused;                      // number of voice calls done refused
    u32   n_call_voice_tx_fail;                         // number of voice calls done with insuccess


    /* data call rx counters */
    u32   n_call_data_rx_answered_ok;                   // number of data  calls received answered with   success
    u32   n_call_data_rx_answered_fail;                 // number of data  calls received answered with insuccess
    u32   n_call_data_rx_missed;                        // number of data  calls received missed
    u32   n_call_data_rx_refused;                       // number of data  calls received refused

    /* data call tx counters */
    u32   n_call_data_tx_ok;                            // number of data  calls done with   success
    u32   n_call_data_tx_busy;                          // number of data  calls done busy
    u32   n_call_data_tx_refused;                       // number of data  calls done refused
    u32   n_call_data_tx_fail;                          // number of data  calls done with insuccess


    /* GPRS call tx counters */
    u32   n_call_gprs_tx_ok;                            // number of GPRS  calls done with   success
    u32   n_call_gprs_tx_fail;                          // number of GPRS  calls done with insuccess


    /*-------------------------------------------------------------------------
     * Timers
     *-------------------------------------------------------------------------*/
    /* voice call timer (sec) */
    u32   time_call_voice_rx;                           // duration of voice call received (sec)
    u32   time_call_voice_tx;                           // duration of voice call done     (sec)

    /* data call  timer (sec) */
    u32   time_data_voice_rx;                           // duration of data  call received (sec)
    u32   time_data_voice_tx;                           // duration of data  call done     (sec)
} PHONE__PHONE_ACTIVITY_INFO;


/* phone info - activity */
typedef struct
{
    PHONE__PHONE_ACTIVITY_INFO phone_activity_total;    // total   phone activity
    PHONE__PHONE_ACTIVITY_INFO phone_activity_yearly;   // yearly  phone activity
    PHONE__PHONE_ACTIVITY_INFO phone_activity_monthly;  // monthly phone activity
} PHONE__PHONE_ACTIVITY;


/* SIM status */
typedef struct
{
    u8    sim_status;                                   // SIM status

    u8    pin1_attempts_left;                           // PIN1 attempts left
    u8    pin2_attempts_left;                           // PUK2 attempts left
    u8    puk1_attempts_left;                           // PIN1 attempts left
    u8    puk2_attempts_left;                           // PUK2 attempts left
} PHONE__SIM_STATUS;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "SIM PIN" */
typedef struct
{
    bool use_pin;
} PHONE__CONFIG__SIM_PIN;


/* configuration - "GPRS APN" */
typedef struct
{
    ascii apn_server   [PHONE__MAX_LENGTH_GPRS_APN_SERVER    + 1];
    ascii apn_user_name[PHONE__MAX_LENGTH_GPRS_APN_USER_NAME + 1];
    ascii apn_password [PHONE__MAX_LENGTH_GPRS_APN_PASSWORD  + 1];
} PHONE__CONFIG__GPRS_APN_PARAMETERS;


/* configuration - "GPRS DNS" */
typedef struct
{
    ascii dns_1        [PHONE__MAX_LENGTH_GPRS_DNS_1         + 1];
    ascii dns_2        [PHONE__MAX_LENGTH_GPRS_DNS_2         + 1];
} PHONE__CONFIG__GPRS_DNS_PARAMETERS;


/* configuration - "phone activity counter" */
typedef struct
{
    /* monthly counter enable status */
    bool  monthly_counter_enable_sms_rx;                // monthly SMS        rx counter enable status
    bool  monthly_counter_enable_sms_tx;                // monthly SMS        tx counter enable status
    bool  monthly_counter_enable_call_voice_rx;         // monthly voice call rx counter enable status
    bool  monthly_counter_enable_call_voice_tx;         // monthly voice call tx counter enable status
    bool  monthly_counter_enable_call_data_rx;          // monthly data  call rx counter enable status
    bool  monthly_counter_enable_call_data_tx;          // monthly data  call tx counter enable status
    bool  monthly_counter_enable_call_gprs_tx;          // monthly GPRS  call tx counter enable status

    /* monthly counter max */
    u16   monthly_counter_max_sms_rx;                   // monthly SMS        rx counter max
    u16   monthly_counter_max_sms_tx;                   // monthly SMS        tx counter max
    u16   monthly_counter_max_call_voice_rx;            // monthly voice call rx counter max
    u16   monthly_counter_max_call_voice_tx;            // monthly voice call tx counter max
    u16   monthly_counter_max_call_data_rx;             // monthly data  call rx counter max
    u16   monthly_counter_max_call_data_tx;             // monthly data  call tx counter max
    u16   monthly_counter_max_call_gprs_tx;             // monthly GPRS  call tx counter max
} PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS;


/* configuration - "SMS delay" */
typedef struct
{
    bool status;                                        // enable status   [FALSE, TRUE]
    u8   sms_delay_value;                               // SMS delay value [0-96]
    u8   sms_delay_unit;                                // SMS delay unit  [hour]
} PHONE__CONFIG__SMS_DELAY;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*---------------------------------------------------------------------------
 * Task
 *---------------------------------------------------------------------------*/
/* task */
void Phone_TaskPhone(void *argument);


/*-----------------------------------------------------------------------------
 * Init
 *-----------------------------------------------------------------------------*/
/* init */
void Phone_QuickInit(void);


/*-----------------------------------------------------------------------------
 * "My GSM"
 *-----------------------------------------------------------------------------*/
void Phone_MyGsm_MyCreg_Enable   (u8   value);
void Phone_MyGsm_MyCsq_Enable    (u8   value_rssi, u8 value_ber);
void Phone_MyGsm_MyCgms_Enable   (bool value);
void Phone_MyGsm_MyCreg_Disable  (void);
void Phone_MyGsm_MyCsq_Disable   (void);
void Phone_MyGsm_MyCgms_Disable  (void);
void Phone_MyGsm_MyCreg_GetStatus(u8 *ptr_status, u8   *ptr_value);
void Phone_MyGsm_MyCsq_GetStatus (u8 *ptr_status, u8   *ptr_value_rssi, u8 *ptr_value_ber);
void Phone_MyGsm_MyCgms_GetStatus(u8 *ptr_status, bool *ptr_value);


/*---------------------------------------------------------------------------
 * Configurations
 *---------------------------------------------------------------------------*/
/* get/set "SIM PIN" configuration */
void Phone_Config_SimPin_GetDefault               (PHONE__CONFIG__SIM_PIN                 *ptr_data);
void Phone_Config_SimPin_Get                      (PHONE__CONFIG__SIM_PIN                 *ptr_data);
void Phone_Config_SimPin_Set                      (PHONE__CONFIG__SIM_PIN                 *ptr_data);
bool Phone_Config_SimPin_IsValid                  (PHONE__CONFIG__SIM_PIN                 *ptr_data);

/* get/set "GPRS APN" configuration */
void Phone_Config_ApnParameters_GetDefault        (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);
void Phone_Config_ApnParameters_Get               (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);
void Phone_Config_ApnParameters_Set               (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);
bool Phone_Config_ApnParameters_IsValid           (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);

/* get/set "GPRS DNS" configuration */
void Phone_Config_DnsParameters_GetDefault        (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);
void Phone_Config_DnsParameters_Get               (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);
void Phone_Config_DnsParameters_Set               (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);
bool Phone_Config_DnsParameters_IsValid           (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);

/* get/set "phone activity counter" configuration */
void Phone_Config_PhoneActivityCounters_GetDefault(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);
void Phone_Config_PhoneActivityCounters_Get       (PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);
void Phone_Config_PhoneActivityCounters_Set       (PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);
bool Phone_Config_PhoneActivityCounters_IsValid   (PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);

/* get/set "SMS delay" configuration */
void Phone_Config_SmsDelay_GetDefault             (PHONE__CONFIG__SMS_DELAY               *ptr_data);
void Phone_Config_SmsDelay_Get                    (PHONE__CONFIG__SMS_DELAY               *ptr_data);
void Phone_Config_SmsDelay_Set                    (PHONE__CONFIG__SMS_DELAY               *ptr_data);
bool Phone_Config_SmsDelay_IsValid                (PHONE__CONFIG__SMS_DELAY               *ptr_data);


/*---------------------------------------------------------------------------
 * Phone info
 *---------------------------------------------------------------------------*/
/* get phone/SIM IDs */
void Phone_PhoneId_Get             (PHONE__PHONE_ID               *ptr_data);
void Phone_PhoneSwComponent_Get    (PHONE__PHONE_SW_COMPONENTS_ID *ptr_data);
void Phone_SimId_Get               (PHONE__SIM_ID                 *ptr_data);

/* get phone/SIM status */
void Phone_PhoneStatus_Get         (PHONE__PHONE_STATUS           *ptr_data);
void Phone_SimStatus_Get           (PHONE__SIM_STATUS             *ptr_data);

/* get/set phone activity */
void Phone_PhoneActivity_GetDefault(PHONE__PHONE_ACTIVITY         *ptr_data);
void Phone_PhoneActivity_Get       (PHONE__PHONE_ACTIVITY         *ptr_data);
void Phone_PhoneActivity_Set       (PHONE__PHONE_ACTIVITY         *ptr_data);

/* network registartion */
bool Phone_IsGsmNetworkRegistered (void);
bool Phone_IsGprsNetworkRegistered(void);

/* reset phone activity */
void Phone_PhoneActivityTotalReset  (void);
void Phone_PhoneActivityYearlyReset (void);
void Phone_PhoneActivityMonthlyReset(void);
void Phone_PhoneActivityReset       (void);


/*---------------------------------------------------------------------------
 * Actions on phone
 *---------------------------------------------------------------------------*/
/* send SMS */
bool Phone_SendSms(ascii *ptr_phone_num, ascii *ptr_sms_text);

/* GPRS bearer utilities */
bool Phone_GprsStart(void);
bool Phone_GprsStop (void);

/* GSM data channel tx/rx */
void Phone_CallDataTxData(u16 len_data, u8 *ptr_data);




#endif
