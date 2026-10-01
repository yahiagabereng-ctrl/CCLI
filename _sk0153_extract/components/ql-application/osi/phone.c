/*=============================================================================
 * File       :  PHONE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - phone manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 *  ||------------||           ||----------------------||    ||--------------------||--------------------||--------------------||---------------------||--------------------||    ||----------------------------------||
 *  ||     SIM    ||           ||          SIM         ||    ||    enter PIN       ||    enable PIN      || change PIN <ccid>  || change PIN <default>||   disable PIN      ||    ||             SIM after            ||
 *  ||------------||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *  ||  SIM state ||    flag   || <sim_pin> | attempts ||    ||   1st  |    2nd    ||   1st  |    2nd    ||   1st  |    2nd    ||   1st  |    2nd     ||   1st  |    2nd    ||    || SIM state | <sim_pin> | attempts ||    || Program  ||
 *  ||            || "use PIN" ||           |   left   ||    || <ccid> | <default> || <ccid> | <default> || <ccid> | <default> || <ccid> | <default>  || <ccid> | <default> ||    ||           |           |   left   ||    ||          ||
 *  ||------------||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *  ||            ||           || <default> |  ---     ||    ||        |           ||        |           ||        |           ||        |            ||        |           ||    ||   READY   | <default> |     3    ||    ||   START  ||
 *  ||            ||     F     || <ccid>    |  ---     ||    ||        |           ||        |           ||        |           ||        |            ||        |           ||    ||   READY   | <ccid>    |     3    ||    ||   START  ||
 *  ||            ||           || <other>   |  ---     ||    ||        |           ||        |           ||        |           ||        |            ||        |           ||    ||   READY   | <other>   |     3    ||    ||   START  ||
 *  ||    READY   ||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *  ||            ||           || <default> |  3 2 1   ||    ||        |           ||   X (E)|    X      ||   X (E)|     X     ||        |            ||        |           ||    ||  NEED_PIN | <ccid>    |     3    ||    ||   START  ||
 *  ||            ||     T     || <ccid>    |  3 2 1   ||    ||        |           ||   X    |           ||        |           ||        |            ||        |           ||    ||  NEED_PIN | <ccid>    |     3    ||    ||   START  ||
 *  ||            ||           || <other>   |  3 2 1   ||    ||        |           ||   X (E)|    X (E)  ||        |           ||        |            ||        |           ||    ||   READY   | <other>   |     1    ||    || NO START ||
 *  ||------------||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *  ||            ||           || <default> |  3 2 1   ||    ||   X (E)|    X      ||        |           ||        |           ||        |            ||   X (E)|    X      ||    ||   READY   | <default> |     3    ||    ||   START  ||
 *  ||            ||     F     || <ccid>    |  3 2 1   ||    ||   X    |           ||        |           ||        |           ||   X    |            ||   X (E)|    X      ||    ||   READY   | <default> |     3    ||    ||   START  ||
 *  ||            ||           || <other>   |  3 2 1   ||    ||   X (E)|    X (E)  ||        |           ||        |           ||        |            ||        |           ||    ||  NEED_PIN | <other>   |     1    ||    || NO START ||
 *  ||  NEED_PIN  ||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *  ||            ||           || <default> |  3 2 1   ||    ||   X (E)|    X      ||        |           ||   X (E)|     X     ||        |            ||        |           ||    ||  NEED_PIN | <ccid>    |     3    ||    ||   START  ||
 *  ||            ||     T     || <ccid>    |  3 2 1   ||    ||   X    |           ||        |           ||        |           ||        |            ||        |           ||    ||  NEED_PIN | <ccid>    |     3    ||    ||   START  ||
 *  ||            ||           || <other>   |  3 2 1   ||    ||   X (E)|    X (E)  ||        |           ||        |           ||        |            ||        |           ||    ||  NEED_PIN | <other>   |     1    ||    || NO START ||
 *  ||------------||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *  ||  NEED_PUK  ||    ---    ||    ---    |    ---   ||    ||        |           ||        |           ||        |           ||        |            ||        |           ||    ||  NEED_PUK |   ---     |    ---   ||    || NO START ||
 *  ||------------||-----------||-----------|----------||    ||--------|-----------||--------|-----------||--------|-----------||--------|------------||--------|-----------||    ||-----------|-----------|----------||    ||----------||
 *
 *  (*) (E): ERROR (the used PIN is not the <sim_pin>)
 */




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_api_datacall.h"
#include "ql_api_dev.h"
#include "ql_api_nw.h"
#include "ql_api_rtc.h"
#include "ql_api_sim.h"
#include "ql_api_sms.h"
#include "ql_api_virt_at.h"
#include "ql_api_voice_call.h"

/* user     includes */
#include "typedef.h"
#include "boot.h"
#include "calendar.h"
#include "clock.h"
#include "counters.h"
#include "credit.h"
#include "debug_my.h"
#include "f_chrono.h"
#include "forward.h"
#include "fw_config.h"
#include "fwd.h"
#include "led.h"
#include "log_status.h"
#include "logs.h"
#include "main.h"
#include "myn.h"
#include "outputs.h"
#include "parser_rx.h"
#include "phone.h"
#include "phonebook.h"
#include "program_flash.h"
#include "queue_my.h"
#include "regulation.h"
#include "report.h"
#include "strings_my.h"
#include "transmission_gprs.h"
#include "transmission_sms.h"
#include "ui_led.h"
#include "utility.h"

#ifdef FUNCTION_IMPLEMENTED
#include <ctype.h>
#include "terminal.h"
#include "drvgpio.h"
#endif




/*=============================================================================
 * DEFINES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * SIM PIN
 *-----------------------------------------------------------------------------*/
/* SIM status */
#define SIM_STATUS__UNKNOWN                       0     // unknown
#define SIM_STATUS__READY                         1     // ready
#define SIM_STATUS__NEED_PIN                      2     // need PIN
#define SIM_STATUS__NEED_PUK                      3     // need PUK

/* SIM PIN type */
#define SIM_PIN_TYPE__UNKNOWN_PIN                 0     // unknown PIN
#define SIM_PIN_TYPE__DEFAULT_PIN                 1     // default PIN
#define SIM_PIN_TYPE__CCID_PIN                    2     // CCID    PIN

/* default SIM PIN */
#define DEFAULT_PIN                               "1234"


/*-----------------------------------------------------------------------------
 * OS
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define PHONE__TASK_MSG_ID__SMS_RX_IND            (12500 | (QL_COMPONENT_APP_START << 16))
#define PHONE__TASK_MSG_ID__RING_VOICE            (12501 | (QL_COMPONENT_APP_START << 16))
#define PHONE__TASK_MSG_ID__SIM_PIN_WAIT          (12502 | (QL_COMPONENT_APP_START << 16))
#define PHONE__TASK_MSG_ID__SIM_PUK_WAIT          (12503 | (QL_COMPONENT_APP_START << 16))
#define PHONE__TASK_MSG_ID__SIM_FULL_INIT         (12504 | (QL_COMPONENT_APP_START << 16))
#define PHONE__TASK_MSG_ID__TIMEOUT_POLLING       (12505 | (QL_COMPONENT_APP_START << 16))
#define PHONE__TASK_MSG_ID__NITZ_TIME_UPDATE      (12506 | (QL_COMPONENT_APP_START << 16))

/* events */
#define EVENT_FLAG__EVENT_ANSWER_QPINC_OK         (0x00000100)
#define EVENT_FLAG__EVENT_ANSWER_QPINC_ERROR      (0x00000200)

/* all events */
#define EVENT_FLAG__EVENT_ALL                     (EVENT_FLAG__EVENT_ANSWER_QPINC_OK       |   \
                                                   EVENT_FLAG__EVENT_ANSWER_QPINC_ERROR)


/*-----------------------------------------------------------------------------
 * Timeout values
 *-----------------------------------------------------------------------------*/
/* timeout for AT commands (ms) */
#define TIMEOUT_WAIT_ANSWER_QPINC                 11000

/* timeout values (ms) */
#define TIME_MS__GSM_STATUS_POLLING               (5 * 1000L)          // time GSM status polling (ms)


/*-----------------------------------------------------------------------------
 * AT commands
 *-----------------------------------------------------------------------------*/
/* PIN SIM */
#define AT_COM_QPINC                              "AT+QPINC=\"SC\"\r"  // get the number of remaining valid attempts for PIN 1

/* AT answer rx status */
#define AT_ANSWER_STATUS_RX__WAIT_START_CR        0                    // AT answer rx status - waiting for start <CR>
#define AT_ANSWER_STATUS_RX__WAIT_START_LF        1                    // AT answer rx status - waiting for start <LF>
#define AT_ANSWER_STATUS_RX__WAIT_DATA            2                    // AT answer rx status - waiting for a data byte
#define AT_ANSWER_STATUS_RX__WAIT_STOP_LF         3                    // AT answer rx status - waiting for stop  <LF>

/* answer  buffer size (bytes) */
#define SIZE_BUFFER_ANSWER                        20


/*-----------------------------------------------------------------------------
 * ???
 *-----------------------------------------------------------------------------*/
/* SW component and SW subcomponent header strings */
#define SW_COMPONENT_STRING_DWL                   "\"DWL\""
#define SW_COMPONENT_STRING_FW                    "\"FW\""
#define SW_COMPONENT_STRING_OAT                   "\"OAT\""
#define SW_COMPONENT_STRING_ROM                   "\"ROM\""
#define SW_COMPONENT_STRING_RAM                   "\"RAM\""
#define SW_COMPONENT_STRING_DWLNAME               "\"DWLNAME\""
#define SW_SUBCOMPONENT_STRING                    " -"


/*-----------------------------------------------------------------------------
 * String lengths
 *-----------------------------------------------------------------------------*/
/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING                   200

#define MAX_LEN_PHONE_NUMBER                      20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/*  */
typedef struct
{
    u16  len_data;   // length of  data
    u8  *ptr_data;   // pointer to data
} DATA_TX;




/*=============================================================================
 * VARIABLES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "SIM PIN" */
static       PHONE__CONFIG__SIM_PIN                 Phone_Config_SimPin;
static const PHONE__CONFIG__SIM_PIN                 Phone_Config_SimPinDefault =
{
#if (FW_CONFIG__SIM_PIN == FW_CONFIG__SIM_PIN__OFF)
    FALSE                  // use PIN
#endif

#if (FW_CONFIG__SIM_PIN == FW_CONFIG__SIM_PIN__ON)
    TRUE                   // use PIN
#endif
};


/* configuration - "GPRS APN" */
static       PHONE__CONFIG__GPRS_APN_PARAMETERS     Phone_Config_GprsApnParameters;
static const PHONE__CONFIG__GPRS_APN_PARAMETERS     Phone_Config_GprsApnParametersDefault =
{
  //"internet.wind",
  //"internet.wind.biz",
  //"web.omnitel.it",
  //"ibox.tim.it",
    "",                    // APN server
    "",                    // APN user name
    "",                    // APN password
};


/* configuration - "GPRS DNS" */
static       PHONE__CONFIG__GPRS_DNS_PARAMETERS     Phone_Config_GprsDnsParameters;
static const PHONE__CONFIG__GPRS_DNS_PARAMETERS     Phone_Config_GprsDnsParametersDefault =
{
    "",                    // DNS 1
    "",                    // DNS 2
};


/* configuration - "phone activity counter" */
static       PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS Phone_Config_PhoneActivityCounters;
static const PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS Phone_Config_PhoneActivityCountersDefault =
{
    /* monthly counter enable status */
    FALSE,         /* monthly SMS        rx counter enable status */
    FALSE,         /* monthly SMS        tx counter enable status */
    FALSE,         /* monthly voice call rx counter enable status */
    FALSE,         /* monthly voice call tx counter enable status */
    FALSE,         /* monthly data  call rx counter enable status */
    FALSE,         /* monthly data  call tx counter enable status */
    FALSE,         /* monthly GPRS  call tx counter enable status */

    /* monthly counter max */
    0,             /* monthly SMS        rx counter max         */
    0,             /* monthly SMS        tx counter max         */
    0,             /* monthly voice call rx counter max         */
    0,             /* monthly voice call tx counter max         */
    0,             /* monthly data  call rx counter max         */
    0,             /* monthly data  call tx counter max         */
    0,             /* monthly GPRS  call tx counter max         */
};


/* configuration - "SMS delay" */
static       PHONE__CONFIG__SMS_DELAY               Phone_Config_SmsDelay;
static const PHONE__CONFIG__SMS_DELAY               Phone_Config_SmsDelayDefault =
{
    FALSE,                          // enable status                 [FALSE, TRUE]
    0,                              // maximum delay for SMS (hours) [0-96]
    PHONE__SMS_DELAY_UNIT__HOUR,    // SMS delay unit                [hour]
};


/*-----------------------------------------------------------------------------
 * Phone Info
 *-----------------------------------------------------------------------------*/
/* phone info - ID */
static       PHONE__PHONE_ID                        Phone_PhoneId;              /* phone IDs               */
static       PHONE__PHONE_SW_COMPONENTS_ID          Phone_PhoneSwComponentsId;  /* phone SW components IDs */
static       PHONE__SIM_ID                          Phone_SimId;                /* SIM IDs                 */

/* phone info - phone status */
static       PHONE__PHONE_STATUS                    Phone_PhoneStatus;

/* phone info - SIM   status */
static       PHONE__SIM_STATUS                      Phone_SimStatus =
{
    PHONE__SIM_STATUS__UNKNOWN,

    0,
    0,
    0,
    0,
};

/* phone info - activity */
static       PHONE__PHONE_ACTIVITY                  Phone_PhoneActivity;
static const PHONE__PHONE_ACTIVITY                  Phone_PhoneActivityDefault =
{
    /*------------------------------------------------------------------------------
     * Total phone activity
     *------------------------------------------------------------------------------*/
    {
        /*------------------------------------------------------------------------------
         * Counters
         *------------------------------------------------------------------------------*/
        /* SMS counters */
        0,     // n_sms_rx                     :  number of SMS received
        0,     // n_sms_tx_ok                  :  number of SMS transmitted with   success
        0,     // n_sms_tx_fail                :  number of SMS transmitted with insuccess


        /* voice call rx counters */
        0,     // n_call_voice_rx_answered_ok  :  number of voice calls received answered with   success
        0,     // n_call_voice_rx_answered_fail:  number of voice calls received answered with insuccess
        0,     // n_call_voice_rx_missed       :  number of voice calls received missed
        0,     // n_call_voice_rx_refused      :  number of voice calls received refused

        /* voice call tx counters */
        0,     // n_call_voice_tx_ok           :  number of voice calls done with   success
        0,     // n_call_voice_tx_busy         :  number of voice calls done busy
        0,     // n_call_voice_tx_refused      :  number of voice calls done refused
        0,     // n_call_voice_tx_fail         :  number of voice calls done with insuccess


        /* data call rx counters */
        0,     // n_call_data_rx_answered_ok   :  number of data  calls received answered with   success
        0,     // n_call_data_rx_answered_fail :  number of data  calls received answered with insuccess
        0,     // n_call_data_rx_missed        :  number of data  calls received missed
        0,     // n_call_data_rx_refused       :  number of data  calls received refused

        /* data call tx counters */
        0,     // n_call_data_tx_ok            :  number of data  calls done with   success
        0,     // n_call_data_tx_busy          :  number of data  calls done busy
        0,     // n_call_data_tx_refused       :  number of data  calls done refused
        0,     // n_call_data_tx_fail          :  number of data  calls done with insuccess


        /* GPRS call tx counters */
        0,     // n_call_gprs_tx_ok            :  number of GPRS  calls done with   success
        0,     // n_call_gprs_tx_fail          :  number of GPRS  calls done with insuccess


        /*------------------------------------------------------------------------------
         * Timers
         *------------------------------------------------------------------------------*/
        /* voice call timer (sec) */
        0,     // time_call_voice_rx           :  duration of voice call received (sec)
        0,     // time_call_voice_tx           :  duration of voice call done     (sec)

        /* data call  timer (sec) */
        0,     // time_data_voice_rx           :  duration of data  call received (sec)
        0,     // time_data_voice_tx           :  duration of data  call done     (sec)
    },


    /*------------------------------------------------------------------------------
     * Yearly phone activity
     *------------------------------------------------------------------------------*/
    {
        /*------------------------------------------------------------------------------
         * Counters
         *------------------------------------------------------------------------------*/
        /* SMS counters */
        0,     // n_sms_rx                     :  number of SMS received
        0,     // n_sms_tx_ok                  :  number of SMS transmitted with   success
        0,     // n_sms_tx_fail                :  number of SMS transmitted with insuccess


        /* voice call rx counters */
        0,     // n_call_voice_rx_answered_ok  :  number of voice calls received answered with   success
        0,     // n_call_voice_rx_answered_fail:  number of voice calls received answered with insuccess
        0,     // n_call_voice_rx_missed       :  number of voice calls received missed
        0,     // n_call_voice_rx_refused      :  number of voice calls received refused

        /* voice call tx counters */
        0,     // n_call_voice_tx_ok           :  number of voice calls done with   success
        0,     // n_call_voice_tx_busy         :  number of voice calls done busy
        0,     // n_call_voice_tx_refused      :  number of voice calls done refused
        0,     // n_call_voice_tx_fail         :  number of voice calls done with insuccess


        /* data call rx counters */
        0,     // n_call_data_rx_answered_ok   :  number of data  calls received answered with   success
        0,     // n_call_data_rx_answered_fail :  number of data  calls received answered with insuccess
        0,     // n_call_data_rx_missed        :  number of data  calls received missed
        0,     // n_call_data_rx_refused       :  number of data  calls received refused

        /* data call tx counters */
        0,     // n_call_data_tx_ok            :  number of data  calls done with   success
        0,     // n_call_data_tx_busy          :  number of data  calls done busy
        0,     // n_call_data_tx_refused       :  number of data  calls done refused
        0,     // n_call_data_tx_fail          :  number of data  calls done with insuccess


        /* GPRS call tx counters */
        0,     // n_call_gprs_tx_ok            :  number of GPRS  calls done with   success
        0,     // n_call_gprs_tx_fail          :  number of GPRS  calls done with insuccess


        /*------------------------------------------------------------------------------
         * Timers
         *------------------------------------------------------------------------------*/
        /* voice call timer (sec) */
        0,     // time_call_voice_rx           :  duration of voice call received (sec)
        0,     // time_call_voice_tx           :  duration of voice call done     (sec)

        /* data call  timer (sec) */
        0,     // time_data_voice_rx           :  duration of data  call received (sec)
        0,     // time_data_voice_tx           :  duration of data  call done     (sec)
    },


    /*------------------------------------------------------------------------------
     * Monthly phone activity
     *------------------------------------------------------------------------------*/
    {
        /*------------------------------------------------------------------------------
         * Counters
         *------------------------------------------------------------------------------*/
        /* SMS counters */
        0,     // n_sms_rx                     :  number of SMS received
        0,     // n_sms_tx_ok                  :  number of SMS transmitted with   success
        0,     // n_sms_tx_fail                :  number of SMS transmitted with insuccess


        /* voice call rx counters */
        0,     // n_call_voice_rx_answered_ok  :  number of voice calls received answered with   success
        0,     // n_call_voice_rx_answered_fail:  number of voice calls received answered with insuccess
        0,     // n_call_voice_rx_missed       :  number of voice calls received missed
        0,     // n_call_voice_rx_refused      :  number of voice calls received refused

        /* voice call tx counters */
        0,     // n_call_voice_tx_ok           :  number of voice calls done with   success
        0,     // n_call_voice_tx_busy         :  number of voice calls done busy
        0,     // n_call_voice_tx_refused      :  number of voice calls done refused
        0,     // n_call_voice_tx_fail         :  number of voice calls done with insuccess


        /* data call rx counters */
        0,     // n_call_data_rx_answered_ok   :  number of data  calls received answered with   success
        0,     // n_call_data_rx_answered_fail :  number of data  calls received answered with insuccess
        0,     // n_call_data_rx_missed        :  number of data  calls received missed
        0,     // n_call_data_rx_refused       :  number of data  calls received refused

        /* data call tx counters */
        0,     // n_call_data_tx_ok            :  number of data  calls done with   success
        0,     // n_call_data_tx_busy          :  number of data  calls done busy
        0,     // n_call_data_tx_refused       :  number of data  calls done refused
        0,     // n_call_data_tx_fail          :  number of data  calls done with insuccess


        /* GPRS call tx counters */
        0,     // n_call_gprs_tx_ok            :  number of GPRS  calls done with   success
        0,     // n_call_gprs_tx_fail          :  number of GPRS  calls done with insuccess


        /*------------------------------------------------------------------------------
         * Timers
         *------------------------------------------------------------------------------*/
        /* voice call timer (sec) */
        0,     // time_call_voice_rx           :  duration of voice call received (sec)
        0,     // time_call_voice_tx           :  duration of voice call done     (sec)

        /* data call  timer (sec) */
        0,     // time_data_voice_rx           :  duration of data  call received (sec)
        0,     // time_data_voice_tx           :  duration of data  call done     (sec)
    },
};


/*-----------------------------------------------------------------------------
 * ???
 *-----------------------------------------------------------------------------*/
static       bool                                   Phone_InitDone = FALSE;

static       bool                                   Phone_GsmNetworkRegistered  = FALSE;
static       bool                                   Phone_GprsNetworkRegistered = FALSE;

/* flags */
static       bool                                   Phone_SimFullInit = FALSE;                             // flag: SIM full init done
static       bool                                   Phone_SimPinInit  = FALSE;                             // flag: SIM PIN  init done

/* SIM PIN */
static       u8                                     Phone_SimPinStatus = SIM_STATUS__UNKNOWN;
static       u8                                     Phone_SimPinType   = SIM_PIN_TYPE__UNKNOWN_PIN;

/* SIM CCID */
static       ascii                                  Phone_SimCcid[PHONE__LEN_MAX_EF_CCID + 1];

/* SIM PIN attempts left */
static       u8                                     Phone_SimPinAttemptsLeft = 0;

/* SIM problem indication */
static       bool                                   Phone_SimProblem = FALSE;

/* SMS rx found in SIM */
static       u8                                     Phone_SmsRxFound = 0;

/* debug string */
static       ascii                                  Phone_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * "My GSM"
 *---------------------------------------------------------------------------*/
/* "My GSM" status */
static       bool                                   Phone_MyGsm_MyCreg_Status = FALSE;                     // "My CREG" status
static       bool                                   Phone_MyGsm_MyCsq_Status  = FALSE;                     // "My CSQ"  status
static       bool                                   Phone_MyGsm_MyCgms_Status = FALSE;                     // "My CGMS" status

/* "My GSM" value */
static       u8                                     Phone_MyGsm_MyCreg_Value     = 1;                      // "My CREG" value
static       u8                                     Phone_MyGsm_MyCsq_Value_Rssi = 99;                     // "My CSQ"  value (RSSI)
static       u8                                     Phone_MyGsm_MyCsq_Value_Ber  = 99;                     // "My CSQ"  value (BER )
static       bool                                   Phone_MyGsm_MyCgms_Value     = TRUE;                   // "My CGMS" value


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
static       int                                    Phone_BearerHandler_Gprs = 1;                          // Wip bearer               handler

/* event handlers */
static       ql_egroup_t                            Phone_EventHandler_Events;                             // event handler

/* timer handler */
static       ql_timer_t                             Phone_TimerHandler_TimerPhonePolling;                  // timer for phone polling




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Task
 *-----------------------------------------------------------------------------*/
/* task */
       void   Phone_TaskPhone(void *argument);


/*-----------------------------------------------------------------------------
 * Init
 *-----------------------------------------------------------------------------*/
/* init */
       void   Phone_QuickInit  (void);
static void   Phone_Init       (void);
static void   Phone_InitForSms (void);
static void   Phone_InitForClip(void);
static void   Phone_ReadPhoneId(void);
static void   Phone_ReadSimId  (void);


/*-----------------------------------------------------------------------------
 * Command "STATUS"
 *-----------------------------------------------------------------------------*/
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
static void   Phone_ParseCommandStatus(ascii *ptr_phone_number);
#endif


/*-----------------------------------------------------------------------------
 * "My GSM"
 *-----------------------------------------------------------------------------*/
       void   Phone_MyGsm_MyCreg_Enable   (u8   value);
       void   Phone_MyGsm_MyCsq_Enable    (u8   value_rssi, u8 value_ber);
       void   Phone_MyGsm_MyCgms_Enable   (bool value);
       void   Phone_MyGsm_MyCreg_Disable  (void);
       void   Phone_MyGsm_MyCsq_Disable   (void);
       void   Phone_MyGsm_MyCgms_Disable  (void);
       void   Phone_MyGsm_MyCreg_GetStatus(u8 *ptr_status, u8   *ptr_value);
       void   Phone_MyGsm_MyCsq_GetStatus (u8 *ptr_status, u8   *ptr_value_rssi, u8 *ptr_value_ber);
       void   Phone_MyGsm_MyCgms_GetStatus(u8 *ptr_status, bool *ptr_value);


/*-----------------------------------------------------------------------------
 * Network status
 *-----------------------------------------------------------------------------*/
static void   Phone_ReadPhoneNetworkStatus(void);


/*---------------------------------------------------------------------------
 * Configurations
 *---------------------------------------------------------------------------*/
/* get/set "SIM PIN" configuration */
       void   Phone_Config_SimPin_GetDefault               (PHONE__CONFIG__SIM_PIN                 *ptr_data);
       void   Phone_Config_SimPin_Get                      (PHONE__CONFIG__SIM_PIN                 *ptr_data);
       void   Phone_Config_SimPin_Set                      (PHONE__CONFIG__SIM_PIN                 *ptr_data);
       bool   Phone_Config_SimPin_IsValid                  (PHONE__CONFIG__SIM_PIN                 *ptr_data);

/* get/set "GPRS APN" configuration */
       void   Phone_Config_ApnParameters_GetDefault        (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);
       void   Phone_Config_ApnParameters_Get               (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);
       void   Phone_Config_ApnParameters_Set               (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);
       bool   Phone_Config_ApnParameters_IsValid           (PHONE__CONFIG__GPRS_APN_PARAMETERS     *ptr_data);

/* get/set "GPRS DNS" configuration */
       void   Phone_Config_DnsParameters_GetDefault        (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);
       void   Phone_Config_DnsParameters_Get               (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);
       void   Phone_Config_DnsParameters_Set               (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);
       bool   Phone_Config_DnsParameters_IsValid           (PHONE__CONFIG__GPRS_DNS_PARAMETERS     *ptr_data);

/* get/set "phone activity counter" configuration */
       void   Phone_Config_PhoneActivityCounters_GetDefault(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);
       void   Phone_Config_PhoneActivityCounters_Get       (PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);
       void   Phone_Config_PhoneActivityCounters_Set       (PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);
       bool   Phone_Config_PhoneActivityCounters_IsValid   (PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data);

/* get/set "SMS delay" configuration */
       void   Phone_Config_SmsDelay_GetDefault             (PHONE__CONFIG__SMS_DELAY               *ptr_data);
       void   Phone_Config_SmsDelay_Get                    (PHONE__CONFIG__SMS_DELAY               *ptr_data);
       void   Phone_Config_SmsDelay_Set                    (PHONE__CONFIG__SMS_DELAY               *ptr_data);
       bool   Phone_Config_SmsDelay_IsValid                (PHONE__CONFIG__SMS_DELAY               *ptr_data);


/*-----------------------------------------------------------------------------
 * Phone info
 *-----------------------------------------------------------------------------*/
/* get phone/SIM IDs */
       void   Phone_PhoneId_Get             (PHONE__PHONE_ID               *ptr_data);
       void   Phone_PhoneSwComponent_Get    (PHONE__PHONE_SW_COMPONENTS_ID *ptr_data);
       void   Phone_SimId_Get               (PHONE__SIM_ID                 *ptr_data);

/* get phone/SIM status */
       void   Phone_PhoneStatus_Get         (PHONE__PHONE_STATUS           *ptr_data);
       void   Phone_SimStatus_Get           (PHONE__SIM_STATUS             *ptr_data);

/* get/set phone activity */
       void   Phone_PhoneActivity_GetDefault(PHONE__PHONE_ACTIVITY         *ptr_data);
       void   Phone_PhoneActivity_Get       (PHONE__PHONE_ACTIVITY         *ptr_data);
       void   Phone_PhoneActivity_Set       (PHONE__PHONE_ACTIVITY         *ptr_data);

/* network registration */
       bool   Phone_IsGsmNetworkRegistered (void);
       bool   Phone_IsGprsNetworkRegistered(void);

/* reset phone activity */
       void   Phone_PhoneActivityTotalReset  (void);
       void   Phone_PhoneActivityYearlyReset (void);
       void   Phone_PhoneActivityMonthlyReset(void);
       void   Phone_PhoneActivityReset       (void);


/*-----------------------------------------------------------------------------
 * Timers
 *-----------------------------------------------------------------------------*/
/* action on timers */
static void   Phone_TimerPhonePolling_Start(u32 time_value, bool periodic);
static void   Phone_TimerPhonePolling_Stop (void);


/*-----------------------------------------------------------------------------
 * Actions on phone
 *-----------------------------------------------------------------------------*/
/* send SMS */
       bool   Phone_SendSms(ascii *ptr_phone_num, ascii *ptr_sms_text);

/* GPRS bearer utilities */
       bool   Phone_GprsStart(void);
       bool   Phone_GprsStop (void);

/* SMS rx */
static void   Phone_SmsReceived(ascii *ptr_sms_tel, ql_sms_time_stamp_s *ptr_sms_time_stamp, ascii *ptr_sms_text);


/*-----------------------------------------------------------------------------
 * SIM PIN
 *-----------------------------------------------------------------------------*/
/* enter SIM PIN */
static void   Phone_EnterSimPin(void);

/* manage SIM PIN */
static void   Phone_ManageSimPin(void);

/* SIM PIN procedures */
static bool   Phone_SimPinProcedure_EnterPin               (u8 *ptr_sim_pin_type);
static bool   Phone_SimPinProcedure_EnablePin              (u8 *ptr_sim_pin_type);
static bool   Phone_SimPinProcedure_DisablePin             (u8 *ptr_sim_pin_type);
static bool   Phone_SimPinProcedure_ChangePin_DefaultToCcid(u8 *ptr_sim_pin_type);
static bool   Phone_SimPinProcedure_ChangePin_CcidToDefault(u8 *ptr_sim_pin_type);

/* "CCID PIN" */
static void   Phone_CalculateCcidPin(ascii *ccid, ascii *ccid_pin);

/* AT commands for SIM PIN */
static bool   Phone_GetSimCcid           (void);
static bool   Phone_GetSimPinAttemptsLeft(void);
static bool   Phone_EnablePinRequest     (ascii *ptr_pin);
static bool   Phone_DisablePinRequest    (ascii *ptr_pin);
static bool   Phone_EnterPin             (ascii *ptr_pin);
static bool   Phone_ChangePin            (ascii *ptr_old_pin, ascii *ptr_new_pin);

/* wait AT answer */
static bool   Phone_WaitEvent(ql_event_bits_t event_1, ql_event_bits_t event_2, u32 timeout, u8 *ptr_event_id);

/* SIM problem */
static void   Phone_SimPinProblem(void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* SMS callback functions */
static void   Phone_AdlCallback_Service_Sms(uint8_t sim_id, int event_id, void *ctx);

/* WIP callback functions */
static void   Phone_WipCallback_Service_BearerEvent(uint8_t sim_id, unsigned int ind_type, int profile_idx, bool result, void *ctx);

/* AT response callback functions */
static void   Phone_AdlCallback_AtResponse_QPINC(unsigned int ind_type, unsigned int size);

/* utility */
static ascii *Phone_GetArgString(ascii *dst, const ascii *src, u16 position);
static bool   Phone_StringRemoveCrLf(ascii *ptr_string, const u8 *ptr_bytes, u16 num_bytes);

/* unsolicited response callback functions */
static void   Phone_AdlCallback_UnsolicitedResponse_CGREG(uint8_t sim_id, unsigned int ind_type, void *ind_msg_buf);
static void   Phone_AdlCallback_UnsolicitedResponse_CLIP (uint8_t sim, ql_vc_event_id_e event_id, void *ctx);

/* message callback functions */
static void   Phone_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer callback functions */
static void   Phone_AdlCallback_Timer_TimerPhonePolling(void *ptr_context);




/*===========================================================================
 * Function    : Phone_TaskPhone
 *
 * Description : phone task
 * Input       : -
 * Output      : -
 *===========================================================================*/
void Phone_TaskPhone(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - PHONE - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* init */
    Phone_Init();

    Phone_InitDone = TRUE;


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Phone_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : Phone_QuickInit
 *
 * Description: quick init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Phone_QuickInit(void)
{
    QlOSStatus            err;
    ql_nw_errcode_e       res_nw;

    ql_rtc_cfg_t          config_rtc;
    ql_errcode_rtc_e      res_rtc;

    ql_datacall_errcode_e res_datacall;

    u8                    i;


    /* phone info - phone IDs */
    Phone_PhoneId.manufacturer            [0] = 0x00;
    Phone_PhoneId.model                   [0] = 0x00;
    Phone_PhoneId.sw_revision             [0] = 0x00;
    Phone_PhoneId.hw_revision             [0] = 0x00;
    Phone_PhoneId.open_at_lib_revision_int[0] = 0x00;
    Phone_PhoneId.open_at_lib_revision_ext[0] = 0x00;
    Phone_PhoneId.imei                    [0] = 0x00;
    Phone_PhoneId.serial_number           [0] = 0x00;
    Phone_PhoneId.date_of_production      [0] = 0x00;

    /* phone info - SIM IDs */
    Phone_SimId.imsi    [0] = 0x00;
    Phone_SimId.ef_ccid [0] = 0x00;

    /* phone info - phone SW component IDs */
    for (i = 0; i < PHONE__NUM_SW_COMPONENTS; i++)
    {
        Phone_PhoneSwComponentsId.sw_component_id[i].component [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].version   [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].name      [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].company   [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].size      [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].time_stamp[0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].checksum  [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_component_id[i].offset    [0] = 0x00;
    }
    for (i = 0; i < PHONE__NUM_SW_SUBCOMPONENTS; i++)
    {
        Phone_PhoneSwComponentsId.sw_subcomponent_id[i].sub_component        [0] = 0x00;
        Phone_PhoneSwComponentsId.sw_subcomponent_id[i].sub_component_version[0] = 0x00;
    }

    /* phone info - status */
    Phone_PhoneStatus.creg  = 0;
    Phone_PhoneStatus.cgreg = 0;
    Phone_PhoneStatus.operator_long_string [0] = 0x00;
    Phone_PhoneStatus.operator_short_string[0] = 0x00;
    Phone_PhoneStatus.operator_numeric     [0] = 0x00;
    Phone_PhoneStatus.rssi = 99;
    Phone_PhoneStatus.ber  = 99;
    Phone_PhoneStatus.mcc[0] = 0x00;
    Phone_PhoneStatus.mnc[0] = 0x00;
    Phone_PhoneStatus.lac[0] = 0x00;
    Phone_PhoneStatus.ci [0] = 0x00;


    /* flag: SIM full init done  */
    /* flag: SIM PIN  init done  */
    Phone_SimFullInit = FALSE;
    Phone_SimPinInit  = FALSE;

    /* flags */
    Phone_GsmNetworkRegistered  = FALSE;
    Phone_GprsNetworkRegistered = FALSE;


    /* SIM PIN type */
    Phone_SimPinStatus = SIM_STATUS__UNKNOWN;
    Phone_SimPinType   = SIM_PIN_TYPE__UNKNOWN_PIN;


    /* "My GSM" status */
    Phone_MyGsm_MyCreg_Status    = FALSE;
    Phone_MyGsm_MyCsq_Status     = FALSE;
    Phone_MyGsm_MyCgms_Status    = FALSE;
    Phone_MyGsm_MyCsq_Value_Rssi = 99;
    Phone_MyGsm_MyCsq_Value_Ber  = 99;
    Phone_MyGsm_MyCreg_Value     = 1;
    Phone_MyGsm_MyCgms_Value     = TRUE;


    /*----------------------------------------------------------------
     * Event creation
     *----------------------------------------------------------------*/
    /* event creation */
    err = ql_rtos_event_group_create(&Phone_EventHandler_Events);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_event_group_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_event_group_create - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /*----------------------------------------------------------------
     * Phone services subscription
     *----------------------------------------------------------------*/
    /* configure RTC behaviour */
    config_rtc.nv_cfg  = QUEC_DISABLE;
    config_rtc.rtc_cfg = QUEC_ENABLE;
    config_rtc.nwt_cfg = QUEC_DISABLE;

    res_rtc = ql_rtc_set_cfg(&config_rtc);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtc_set_cfg: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* SMS channel events subscription */
    ql_sms_callback_register(Phone_AdlCallback_Service_Sms);


    /*----------------------------------------------------------------
     * Unsolicited responses subscription
     *----------------------------------------------------------------*/
    /* "+CGREG: " unsolicited response subscription */
    res_nw = ql_nw_register_cb(Phone_AdlCallback_UnsolicitedResponse_CGREG);
    if (res_nw != QL_NW_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_nw_register_cb: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_nw_register_cb: OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /*----------------------------------------------------------------
     * GPRS bearer opening
     *----------------------------------------------------------------*/
    /* GPRS bearer opening */
    res_datacall = ql_datacall_register_cb(0, Phone_BearerHandler_Gprs, Phone_WipCallback_Service_BearerEvent, NULL);
    if (res_datacall != QL_DATACALL_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_datacall_register_cb ERROR: %d", res_datacall);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function    : Phone_Init
 *
 * Description : initialize the module
 * Input       : -
 * Output      : -
 *===========================================================================*/
static void Phone_Init(void)
{
    ql_sim_status_e      card_status;

    ql_sim_errcode_e     res_sim;
    ql_errcode_virt_at_e res_vat;

    u8                   counter;

    ql_event_t           event;
    QlOSStatus           err;


    /* creation of timer "TimerPhonePolling" */
    err = ql_rtos_timer_create(&Phone_TimerHandler_TimerPhonePolling, QL_TIMER_IN_SERVICE, Phone_AdlCallback_Timer_TimerPhonePolling, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /*----------------------------------------------------------------
     * Phone settings
     *----------------------------------------------------------------*/
    res_vat = ql_virt_at_open(QL_VIRT_AT_PORT_1, Phone_AdlCallback_AtResponse_QPINC);
    if (res_vat != QL_VIRT_AT_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_virt_at_open: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /*----------------------------------------------------------------
     * Read phone IDs, phone SW components IDs and SIM IDs
     *----------------------------------------------------------------*/
    Phone_ReadPhoneId();


    /*----------------------------------------------------------------
     * Check the SIM presence
     *----------------------------------------------------------------*/
    counter = 0;
    while (1)
    {
        /* AT+CPIN? - check the SIM presence */
        res_sim = ql_sim_get_card_status(0, &card_status);
        if (res_sim != QL_SIM_SUCCESS)
        {
            card_status = QL_SIM_STATUS_UNKNOW;
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_card_status: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }

        if      (card_status == QL_SIM_STATUS_READY)
        {
            if (Phone_SimStatus.sim_status == SIM_STATUS__UNKNOWN)
                Phone_SimStatus.sim_status = PHONE__SIM_STATUS__READY;

            if (Phone_SimPinStatus == SIM_STATUS__UNKNOWN)
                Phone_SimPinStatus = SIM_STATUS__READY;

            break;  // exit from while loop
        }
        else if (card_status == QL_SIM_STATUS_SIMPIN)
        {
            if (Phone_SimStatus.sim_status == SIM_STATUS__UNKNOWN)
                Phone_SimStatus.sim_status = PHONE__SIM_STATUS__SIM_PIN;

            if (Phone_SimPinStatus == SIM_STATUS__UNKNOWN)
                Phone_SimPinStatus = SIM_STATUS__NEED_PIN;

            /* try to enter the SIM PIN */
            Phone_EnterSimPin();

            break;  // exit from while loop
        }
        else if (card_status == QL_SIM_STATUS_SIMPUK)
        {
            if (Phone_SimStatus.sim_status == SIM_STATUS__UNKNOWN)
                Phone_SimStatus.sim_status = PHONE__SIM_STATUS__SIM_PUK;

            if (Phone_SimPinStatus == SIM_STATUS__UNKNOWN)
                Phone_SimPinStatus = SIM_STATUS__NEED_PUK;

            break;  // exit from while loop
        }

        if (counter++ >= 60)
            break;  // exit from while loop

        ql_rtos_task_sleep_ms(500L);
    }

    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "AT+CPIN? ---> %d", card_status);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* waiting for SIM init */
    counter = 0;
    while (1)
    {
        res_sim = ql_sim_get_card_status(0, &card_status);
        if (res_sim != QL_SIM_SUCCESS)
        {
            card_status = QL_SIM_STATUS_UNKNOW;
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_card_status: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }

        if (card_status == QL_SIM_STATUS_READY)
        {
            Phone_SimStatus.sim_status = PHONE__SIM_STATUS__READY;
            Phone_SimPinStatus = SIM_STATUS__READY;

            /* read SIM IDs */
            Phone_ReadSimId();

            /* init for SMS */
            Phone_InitForSms();

            /* init for CLIP */
            Phone_InitForClip();

            /* flag: SIM full init done */
            Phone_SimFullInit = TRUE;

            event.id = PHONE__TASK_MSG_ID__SIM_FULL_INIT;
            err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
            if (err != QL_OSI_SUCCESS)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            else
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            return;  // exit from while loop
        }

        if (counter++ >= 60)
            break;  // exit from while loop

        ql_rtos_task_sleep_ms(500L);
    }

    /* SIM problem */
    Phone_SimPinProblem();
}




/*===========================================================================
 * Function    : Phone_InitForSms
 *
 * Description : init for SMS
 *
 *               *** It is necessary for a SL6087 bug ***
 *
 *               With SIM from some virtual GSM operators (TotalErg Italy, Telering Austria),
 *               the +CSMP setting is changed from "+CSMP: 1,166,0,0" to "+CSMP: 1,166,255,255"
 *               PID and DCS are set to 255.
 *               DCS (Data Coding Scheme) is set to 255.
 *               With this setting, you always have "+CMS ERROR: 513" when you send a SMS.
 * Input       : -
 * Output      : -
 *===========================================================================*/
static void Phone_InitForSms(void)
{
    ql_sms_errcode_e result;


    /* AT+CSMP=1,167,0,0 - set text mode parameters for SMS */
    result = ql_sms_set_storage(0, ME, ME, ME);
    if (result != QL_SMS_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sms_set_storage: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sms_set_storage: OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function    : Phone_InitForClip
 *
 * Description : init for CLIP (it has to be done after PIN insertion)
 * Input       : -
 * Output      : -
 *===========================================================================*/
static void Phone_InitForClip(void)
{
    /* AT+CLIP=1 - enable the Calling Line Identification Presentation supplementary service */
    ql_voice_call_callback_register(Phone_AdlCallback_UnsolicitedResponse_CLIP);
}




/*===========================================================================
 * Function   : Phone_ReadPhoneId
 *
 * Description: read the phone IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Phone_ReadPhoneId(void)
{
    ql_errcode_dev_e result;


    /* get the phone model */
    result = ql_dev_get_model(Phone_PhoneId.model, PHONE__LEN_MAX_MODEL);
    if (result != QL_DEV_SUCCESS)
        Phone_PhoneId.model[0] = 0x00;
    Phone_PhoneId.model[PHONE__LEN_MAX_MODEL] = 0x00;
    ql_rtos_task_sleep_ms(50L);

    /* get the phone SW revision */
    result = ql_dev_get_firmware_version(Phone_PhoneId.sw_revision, PHONE__LEN_MAX_SW_REVISION);
    if (result != QL_DEV_SUCCESS)
        Phone_PhoneId.sw_revision[0] = 0x00;
    Phone_PhoneId.sw_revision[PHONE__LEN_MAX_SW_REVISION] = 0x00;
    ql_rtos_task_sleep_ms(50L);


    /* get the phone serial number */
    result = ql_dev_get_sn(Phone_PhoneId.serial_number, PHONE__LEN_MAX_SERIAL_NUMBER, 0);
    if (result != QL_DEV_SUCCESS)
        Phone_PhoneId.serial_number[0] = 0x00;
    Phone_PhoneId.serial_number[PHONE__LEN_MAX_SERIAL_NUMBER] = 0x00;
    ql_rtos_task_sleep_ms(50L);

    /* get the phone IMEI */
    result = ql_dev_get_imei(Phone_PhoneId.imei, PHONE__LEN_MAX_IMEI, 0);
    if (result != QL_DEV_SUCCESS)
        Phone_PhoneId.imei[0] = 0x00;
    Phone_PhoneId.imei[PHONE__LEN_MAX_IMEI] = 0x00;
    ql_rtos_task_sleep_ms(50L);
}




/*===========================================================================
 * Function   : Phone_ReadSimId
 *
 * Description: read the SIM IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Phone_ReadSimId(void)
{
    ql_sim_errcode_e result;


    /* get the IMSI of the SIM card */
    result = ql_sim_get_imsi(0, Phone_SimId.imsi, PHONE__LEN_MAX_IMSI);
    if (result != QL_SIM_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_imsi: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        Phone_SimId.imsi[0] = 0x00;
    }
    Phone_SimId.imsi[PHONE__LEN_MAX_IMSI] = 0x00;
    ql_rtos_task_sleep_ms(50L);

    /* get the EF-CCID of the SIM card */
    result = ql_sim_get_iccid(0, Phone_SimId.ef_ccid, PHONE__LEN_MAX_EF_CCID);
    if (result != QL_SIM_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_iccid: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        Phone_SimId.ef_ccid[0] = 0x00;
    }
    Phone_SimId.ef_ccid[PHONE__LEN_MAX_EF_CCID] = 0x00;
    ql_rtos_task_sleep_ms(50L);
}




/*=============================================================================
 * Function   : Phone_ParseCommandStatus
 *
 * Description: build a simulation of "STATUS" SMS
 * Input      : - ptr_phone_number: pointer to phone number sending the simulation of "STATUS" SMS
 * Output     : -
 *=============================================================================*/
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
static void Phone_ParseCommandStatus(ascii *ptr_phone_number)
{
           QUEUE__SMS_ANSWER_QUEUE_RECORD record_sms_answer;
           CLOCK__TIME                    time;

           REPORT__CONFIG__REPORT         config_report;

           CLOCK__TIME                    current_time;
           CLOCK__TIME                    sms_time_tx;

           u8                             id_string;
           ascii                         *ptr_string_cmd;

    static ascii                          command_text[30 + 1];

    static ascii                         *ptr_answer_texts[PARSER_RX__NUM_MAX_ANSWERS];
    static ascii                          answer_texts    [PARSER_RX__NUM_MAX_ANSWERS][160 + 1];

           u8                             num_answers;

           bool                           report;
           bool                           command_valid;
           bool                           answer;

           bool                           result;
           bool                           parser_result;

           u8                             i;


    /*-----------------------------------------------------------------------------
     * Clock time
     *-----------------------------------------------------------------------------*/
    result = Clock_GetTime(&current_time);
    if (result)
    {
        /* current time     available */
        sms_time_tx.year     = current_time.year;
        sms_time_tx.month    = current_time.month;
        sms_time_tx.day      = current_time.day;
        sms_time_tx.hour     = current_time.hour;
        sms_time_tx.minute   = current_time.minute;
        sms_time_tx.second   = current_time.second;
        sms_time_tx.week_day = Clock_WeekDayOfADay(sms_time_tx.day, sms_time_tx.month, sms_time_tx.year);
    }
    else
    {
        /* current time not available */
        sms_time_tx.year     = 2000;
        sms_time_tx.month    = 1;
        sms_time_tx.day      = 1;
        sms_time_tx.hour     = 0;
        sms_time_tx.minute   = 0;
        sms_time_tx.second   = 0;
        sms_time_tx.week_day = 6;  // Saturday
    }
    //Clock_PrintTime2(1, &sms_time_tx);


    /*-----------------------------------------------------------------------------
     * Received SMS parsing
     *-----------------------------------------------------------------------------*/
    /* build a simulation of "STATUS" SMS */
    id_string      = STRINGS__COMMAND__STATUS;
    ptr_string_cmd = Strings_GetString(id_string);
    strcpy(command_text, ptr_string_cmd);

    /* parse the simulation of "STATUS" SMS */
    for (i = 0; i < PARSER_RX__NUM_MAX_ANSWERS; i++)
    {
        answer_texts    [i][0] = 0x00;
        ptr_answer_texts[i]    = &answer_texts[i][0];
    }

    parser_result = ParserRx_ParseCommandText(FALSE, &sms_time_tx, "", TRUE, FALSE, command_text, &report, &command_valid, &num_answers, ptr_answer_texts, 160);
    if (parser_result)
    {
        /* parser OK */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PARSER OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

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
                    record_sms_answer.time = time;                                       /* time                       */
                    strncpy(record_sms_answer.sms_phone_number, ptr_phone_number,  20);  /* SMS recipient phone number */
                    strncpy(record_sms_answer.answer_text     , answer_texts[i] , 160);  /* answer text                */
                    record_sms_answer.sms_phone_number[ 20] = 0x00;
                    record_sms_answer.answer_text     [160] = 0x00;

                    /* put the SMS answer record in the SMS answer queue */
                    result = Queue_SmsAnswer_PutRecord(&record_sms_answer);
                }
            }
        }
    }
    else
    {
        /* parser error */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PARSER ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
#endif




/*=============================================================================
 * Function   : Phone_MyGsm_MyCreg_Enable
 *
 * Description: enable "My CREG"
 * Input      : - value:
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCreg_Enable(u8 value)
{
    Phone_MyGsm_MyCreg_Status = TRUE;
    Phone_MyGsm_MyCreg_Value  = value;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCsq_Enable
 *
 * Description: enable "My CSQ"
 * Input      : - value_rssi:
 *              - value_ber :
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCsq_Enable(u8 value_rssi, u8 value_ber)
{
    Phone_MyGsm_MyCsq_Status     = TRUE;
    Phone_MyGsm_MyCsq_Value_Rssi = value_rssi;
    Phone_MyGsm_MyCsq_Value_Ber  = value_ber;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCgms_Enable
 *
 * Description: enable "My CGMS"
 * Input      : - value:
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCgms_Enable(bool value)
{
    Phone_MyGsm_MyCgms_Status = TRUE;
    Phone_MyGsm_MyCgms_Value  = value;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCreg_Disable
 *
 * Description: disable "My CREG"
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCreg_Disable(void)
{
    Phone_MyGsm_MyCreg_Status = FALSE;
    Phone_MyGsm_MyCreg_Value  = 1;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCsq_Disable
 *
 * Description: disable "My CSQ"
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCsq_Disable(void)
{
    Phone_MyGsm_MyCsq_Status     = FALSE;
    Phone_MyGsm_MyCsq_Value_Rssi = 99;
    Phone_MyGsm_MyCsq_Value_Ber  = 99;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCgms_Disable
 *
 * Description: disable "My CGMS"
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCgms_Disable(void)
{
    Phone_MyGsm_MyCgms_Status = FALSE;
    Phone_MyGsm_MyCgms_Value  = TRUE;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCreg_GetStatus
 *
 * Description: return the "My CREG" status
 * Input      : - ptr_status:
 *              - ptr_value :
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCreg_GetStatus(u8 *ptr_status, u8 *ptr_value)
{
    *ptr_status = Phone_MyGsm_MyCreg_Status;
    *ptr_value  = Phone_MyGsm_MyCreg_Value;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCsq_GetStatus
 *
 * Description: return the "My CSQ" status
 * Input      : - ptr_status    :
 *              - ptr_value_rssi:
 *              - ptr_value_ber :
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCsq_GetStatus(u8 *ptr_status, u8 *ptr_value_rssi, u8 *ptr_value_ber)
{
    *ptr_status     = Phone_MyGsm_MyCsq_Status;
    *ptr_value_rssi = Phone_MyGsm_MyCsq_Value_Rssi;
    *ptr_value_ber  = Phone_MyGsm_MyCsq_Value_Ber;
}




/*=============================================================================
 * Function   : Phone_MyGsm_MyCgms_GetStatus
 *
 * Description: return the "My CGMS" status
 * Input      : - ptr_status:
 *              - ptr_value :
 * Output     : -
 *=============================================================================*/
void Phone_MyGsm_MyCgms_GetStatus(u8 *ptr_status, bool *ptr_value)
{
    *ptr_status = Phone_MyGsm_MyCgms_Status;
    *ptr_value  = Phone_MyGsm_MyCgms_Value;
}




/*=============================================================================
 * Function   : Phone_ReadPhoneNetworkStatus
 *
 * Description: Read the GSM network status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Phone_ReadPhoneNetworkStatus(void)
{
    static u8                      num_of_cpms_found = 0;   // number of consecutive +CPMS answeres that inidicate SMSs stored in the SIM
           u8                      i;

           ql_sms_stor_info_s      stor_info;
           ql_sms_recv_s           sms_recv;

           ql_sms_errcode_e        res_sms;

           u8                      rssi;
           ql_nw_operator_info_s   oper_info;
           ql_nw_reg_status_info_s reg_info;

           ql_nw_errcode_e         res_nw;


    /* AT+CSQ */
    res_nw = ql_nw_get_csq(0, &rssi);
    if (res_nw == QL_NW_SUCCESS)
    {
        /* debug info */
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "\"CSQ\": %u", rssi);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Phone_PhoneStatus.rssi = rssi;

        if (Phone_MyGsm_MyCsq_Status)
        {
            /* "My CSQ" enabled */
            Phone_PhoneStatus.rssi = Phone_MyGsm_MyCsq_Value_Rssi;
            Phone_PhoneStatus.ber  = Phone_MyGsm_MyCsq_Value_Ber;
        }
    }


    /* AT+COPS? */
    res_nw = ql_nw_get_operator_name(0, &oper_info);
    if (res_nw == QL_NW_SUCCESS)
    {
        strncpy(Phone_PhoneStatus.operator_long_string, oper_info.long_oper_name, 32);    // GSM network operator - long  alphanumeric format (AT+COPS?)
        Phone_PhoneStatus.operator_long_string[32] = 0x00;

        strncpy(Phone_PhoneStatus.operator_short_string, oper_info.short_oper_name, 10);  // GSM network operator - short alphanumeric format (AT+COPS?)
        Phone_PhoneStatus.operator_short_string[10] = 0x00;

        strncpy(Phone_PhoneStatus.operator_numeric, oper_info.mcc, 3);                    // GSM network operator - numeric            format (AT+COPS?)
        Phone_PhoneStatus.operator_numeric[3] = 0x00;
        strncat(Phone_PhoneStatus.operator_numeric, oper_info.mnc, 3);
        Phone_PhoneStatus.operator_numeric[6] = 0x00;

        strncpy(Phone_PhoneStatus.mcc, oper_info.mcc, 3);
        Phone_PhoneStatus.mcc[3] = 0x00;
        strncpy(Phone_PhoneStatus.mnc, oper_info.mnc, 3);
        Phone_PhoneStatus.mnc[3] = 0x00;
    }
    else
    {
        Phone_PhoneStatus.operator_long_string [0] = 0x00;  // GSM network operator - long  alphanumeric format (AT+COPS?)
        Phone_PhoneStatus.operator_short_string[0] = 0x00;  // GSM network operator - short alphanumeric format (AT+COPS?)
        Phone_PhoneStatus.operator_numeric     [0] = 0x00;  // GSM network operator - numeric            format (AT+COPS?)

        Phone_PhoneStatus.mcc[0] = 0x00;
        Phone_PhoneStatus.mnc[0] = 0x00;
    }


    /* AT+CREG?  */
    /* AT+CGREG? */
    res_nw = ql_nw_get_reg_status(0, &reg_info);
    if (res_nw == QL_NW_SUCCESS)
    {
        /* debug info */
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "\"CREG\": %d", reg_info.voice_reg.state);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "\"CGREG\": %d", reg_info.data_reg.state);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Phone_PhoneStatus.creg  = reg_info.voice_reg.state;
        Phone_PhoneStatus.cgreg = reg_info.data_reg.state;

        if (Phone_MyGsm_MyCreg_Status)
        {
            /* "My CREG" enabled */
            Phone_PhoneStatus.creg = Phone_MyGsm_MyCreg_Value;
        }

        switch (reg_info.voice_reg.state)
        {
            // not registered, ME is not currently searching for a new operator
            // not registered, ME currently searching for a new operator
            // registration denied
            // unknown
            case QL_NW_REG_STATE_NOT_REGISTERED:
            case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
            case QL_NW_REG_STATE_DENIED:
            case QL_NW_REG_STATE_UNKNOWN:
                // not registered in network
                Phone_GsmNetworkRegistered = FALSE;
                break;

            // registered, home network
            // registered, roaming
            case QL_NW_REG_STATE_HOME_NETWORK:
            case QL_NW_REG_STATE_ROAMING:
                // registered in network
                Phone_GsmNetworkRegistered = TRUE;
                break;
        }

        /* update phone status */
        switch (reg_info.voice_reg.state)
        {
            // not registered, ME is not currently searching for a new operator
            // not registered, ME currently searching for a new operator
            // unknown
            case QL_NW_REG_STATE_NOT_REGISTERED:
            case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
            case QL_NW_REG_STATE_UNKNOWN:
                UiLed_SignalGsmRegDeniedStop();
                UiLed_SignalGsmNotRegistered();
                break;

            // registration denied
            case QL_NW_REG_STATE_DENIED:
                UiLed_SignalGsmRegDeniedStart();
                UiLed_SignalGsmNotRegistered();
                break;

            // registered, home network
            // registered, roaming
            case QL_NW_REG_STATE_HOME_NETWORK:
            case QL_NW_REG_STATE_ROAMING:
                UiLed_SignalGsmRegDeniedStop();
                UiLed_SignalGsmRegistered(Phone_PhoneStatus.rssi, Phone_PhoneStatus.ber);
                break;
        }

        /* update phone status */
        switch (reg_info.data_reg.state)
        {
            // not registered, ME is not currently searching for a new operator
            // not registered, ME currently searching for a new operator
            // unknown
            case QL_NW_REG_STATE_NOT_REGISTERED:
            case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
            case QL_NW_REG_STATE_UNKNOWN:
                // not registered in network
                Phone_PhoneStatus.lac[0] = 0x00;
                Phone_PhoneStatus.ci [0] = 0x00;

                Phone_GprsNetworkRegistered = FALSE;
                break;

            // registration denied
            case QL_NW_REG_STATE_DENIED:
                // not registered in network
                Phone_PhoneStatus.lac[0] = 0x00;
                Phone_PhoneStatus.ci [0] = 0x00;

                Phone_GprsNetworkRegistered = FALSE;
                break;

            // registered, home network
            // registered, roaming
            case QL_NW_REG_STATE_HOME_NETWORK:
            case QL_NW_REG_STATE_ROAMING:
                // registered in network
                snprintf(Phone_PhoneStatus.lac, sizeof(Phone_PhoneStatus.lac), "%04X", (uint16_t)(reg_info.data_reg.lac));
                Phone_PhoneStatus.lac[sizeof(Phone_PhoneStatus.lac) - 1] = 0x00;

                snprintf(Phone_PhoneStatus.ci , sizeof(Phone_PhoneStatus.ci ), "%08X",           (reg_info.data_reg.cid));
                Phone_PhoneStatus.ci [sizeof(Phone_PhoneStatus.ci ) - 1] = 0x00;

                Phone_GprsNetworkRegistered = TRUE;
                break;
        }


        /* signal new phone registration status */
        switch (reg_info.voice_reg.state)
        {
            // not registered, ME is not currently searching for a new operator
            // not registered, ME currently searching for a new operator
            // registration denied
            // unknown
            case QL_NW_REG_STATE_NOT_REGISTERED:
            case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
            case QL_NW_REG_STATE_DENIED:
            case QL_NW_REG_STATE_UNKNOWN:
                // not registered in network
                TransmissionSms_Event_GsmNetworkNotRegistered();
                break;

            // registered, home network
            // registered, roaming
            case QL_NW_REG_STATE_HOME_NETWORK:
            case QL_NW_REG_STATE_ROAMING:
                // registered in network
                if (Phone_SimFullInit && Phone_SimPinInit)
                {
                    TransmissionSms_Event_GsmNetworkRegistered();
                }
                break;
        }

        /* signal new phone registration status */
        switch (reg_info.data_reg.state)
        {
            // not registered, ME is not currently searching for a new operator
            // not registered, ME currently searching for a new operator
            // registration denied
            // unknown
            case QL_NW_REG_STATE_NOT_REGISTERED:
            case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
            case QL_NW_REG_STATE_DENIED:
            case QL_NW_REG_STATE_UNKNOWN:
                // not registered in network
                TransmissionGprs_Event_GsmNetworkNotRegistered();
                break;

            // registered, home network
            // registered, roaming
            case QL_NW_REG_STATE_HOME_NETWORK:
            case QL_NW_REG_STATE_ROAMING:
                // registered in network
                if (Phone_SimFullInit && Phone_SimPinInit)
                {
                    TransmissionGprs_Event_GsmNetworkRegistered();
                }
                break;
        }
    }


    /* AT+CPMS? */
    res_sms = ql_sms_get_storage_info(0, &stor_info);
    if (res_sms == QL_SMS_SUCCESS)
    {
        /* delete SMS stored in the SMS */

        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_sms_get_storage_info: \"ME\",%d,%d", stor_info.usedSlotME, stor_info.totalSlotME);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if (stor_info.usedSlotME > 0)
        {
            if (++num_of_cpms_found >= 3)
            {
                num_of_cpms_found = 0;

                Phone_SmsRxFound = 0;

                for (i = 0; i < stor_info.totalSlotME; i++)
                {
                    if (Phone_SmsRxFound >= stor_info.usedSlotME)
                        break;

                    /* AT+CMGR */
                    res_sms = ql_sms_read_msg_ex(0, i + 1, TEXT, &sms_recv);
                    if (res_sms == QL_SMS_SUCCESS)
                    {
                        /* SMS received */
                        Phone_SmsReceived(sms_recv.oa, &sms_recv.scts, (ascii *)&sms_recv.data);

                        Phone_SmsRxFound++;

                        /* AT+CMGD */
                        res_sms = ql_sms_delete_msg(0, i + 1);
                        if (res_sms != QL_SMS_SUCCESS)
                        {
                            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sms_delete_msg: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                        }
                    }
                    else
                    {
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sms_read_msg_ex: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }
                }
            }
        }
        else
        {
            num_of_cpms_found = 0;
        }
    }
}




/*===========================================================================
 * Function   : Phone_Config_SimPin_GetDefault
 *
 * Description: get the default "SIM PIN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_SimPin_GetDefault(PHONE__CONFIG__SIM_PIN *ptr_data)
{
    *ptr_data = Phone_Config_SimPinDefault;
}




/*===========================================================================
 * Function   : Phone_Config_SimPin_Get
 *
 * Description: get the "SIM PIN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_SimPin_Get(PHONE__CONFIG__SIM_PIN *ptr_data)
{
    *ptr_data = Phone_Config_SimPin;
}




/*===========================================================================
 * Function   : Phone_Config_SimPin_Set
 *
 * Description: set the "SIM PIN" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Phone_Config_SimPin_Set(PHONE__CONFIG__SIM_PIN *ptr_data)
{
    Phone_Config_SimPin = *ptr_data;
}




/*===========================================================================
 * Function   : Phone_Config_SimPin_IsValid
 *
 * Description: check if the "SIM PIN" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Phone_Config_SimPin_IsValid(PHONE__CONFIG__SIM_PIN*ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : Phone_Config_ApnParameters_GetDefault
 *
 * Description: get the default "GPRS APN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_ApnParameters_GetDefault(PHONE__CONFIG__GPRS_APN_PARAMETERS *ptr_data)
{
    *ptr_data = Phone_Config_GprsApnParametersDefault;
}




/*===========================================================================
 * Function   : Phone_Config_ApnParameters_Get
 *
 * Description: get the "GPRS APN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_ApnParameters_Get(PHONE__CONFIG__GPRS_APN_PARAMETERS *ptr_data)
{
    *ptr_data = Phone_Config_GprsApnParameters;
}




/*===========================================================================
 * Function   : Phone_Config_ApnParameters_Set
 *
 * Description: set the "GPRS APN" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Phone_Config_ApnParameters_Set(PHONE__CONFIG__GPRS_APN_PARAMETERS *ptr_data)
{
    Phone_Config_GprsApnParameters = *ptr_data;
}




/*===========================================================================
 * Function   : Phone_Config_ApnParameters_IsValid
 *
 * Description: check if the "GPRS APN" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Phone_Config_ApnParameters_IsValid(PHONE__CONFIG__GPRS_APN_PARAMETERS*ptr_data)
{
    if (strlen(ptr_data->apn_server   ) > PHONE__MAX_LENGTH_GPRS_APN_SERVER   )
        return FALSE;

    if (strlen(ptr_data->apn_user_name) > PHONE__MAX_LENGTH_GPRS_APN_USER_NAME)
        return FALSE;

    if (strlen(ptr_data->apn_password ) > PHONE__MAX_LENGTH_GPRS_APN_PASSWORD )
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_Config_DnsParameters_GetDefault
 *
 * Description: get the default "GPRS DNS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_DnsParameters_GetDefault(PHONE__CONFIG__GPRS_DNS_PARAMETERS *ptr_data)
{
    *ptr_data = Phone_Config_GprsDnsParametersDefault;
}




/*===========================================================================
 * Function   : Phone_Config_DnsParameters_Get
 *
 * Description: get the "GPRS DNS" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_DnsParameters_Get(PHONE__CONFIG__GPRS_DNS_PARAMETERS *ptr_data)
{
    *ptr_data = Phone_Config_GprsDnsParameters;
}




/*===========================================================================
 * Function   : Phone_Config_DnsParameters_Set
 *
 * Description: set the "GPRS DNS" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Phone_Config_DnsParameters_Set(PHONE__CONFIG__GPRS_DNS_PARAMETERS *ptr_data)
{
    Phone_Config_GprsDnsParameters = *ptr_data;
}




/*===========================================================================
 * Function   : Phone_Config_DnsParameters_IsValid
 *
 * Description: check if the "GPRS DNS" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Phone_Config_DnsParameters_IsValid(PHONE__CONFIG__GPRS_DNS_PARAMETERS *ptr_data)
{
    if (strlen(ptr_data->dns_1) > PHONE__MAX_LENGTH_GPRS_DNS_1)
        return FALSE;

    if (strlen(ptr_data->dns_2) > PHONE__MAX_LENGTH_GPRS_DNS_2)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_Config_PhoneActivityCounters_GetDefault
 *
 * Description: get the default "phone activity counters" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_PhoneActivityCounters_GetDefault(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data)
{
    *ptr_data = Phone_Config_PhoneActivityCountersDefault;
}




/*===========================================================================
 * Function   : Phone_Config_PhoneActivityCounters_Get
 *
 * Description: get the "phone activity counters" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_PhoneActivityCounters_Get(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data)
{
    *ptr_data = Phone_Config_PhoneActivityCounters;
}




/*===========================================================================
 * Function   : Phone_Config_PhoneActivityCounters_Set
 *
 * Description: set the "phone activity counters" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Phone_Config_PhoneActivityCounters_Set(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data)
{
    Phone_Config_PhoneActivityCounters = *ptr_data;
}




/*===========================================================================
 * Function   : Phone_Config_PhoneActivityCounters_IsValid
 *
 * Description: check if the "phone activity counters" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Phone_Config_PhoneActivityCounters_IsValid(PHONE__CONFIG__PHONE_ACTIVITY_COUNTERS *ptr_data)
{
    // EVERY VALUE IS VALID

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_Config_SmsDelay_GetDefault
 *
 * Description: get the default "SMS delay" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_SmsDelay_GetDefault(PHONE__CONFIG__SMS_DELAY *ptr_data)
{
    *ptr_data = Phone_Config_SmsDelayDefault;
}




/*===========================================================================
 * Function   : Phone_Config_SmsDelay_Get
 *
 * Description: get the "SMS delay" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_Config_SmsDelay_Get(PHONE__CONFIG__SMS_DELAY *ptr_data)
{
    *ptr_data = Phone_Config_SmsDelay;
}




/*===========================================================================
 * Function   : Phone_Config_SmsDelay_Set
 *
 * Description: set the "SMS delay" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Phone_Config_SmsDelay_Set(PHONE__CONFIG__SMS_DELAY *ptr_data)
{
    Phone_Config_SmsDelay = *ptr_data;
}




/*===========================================================================
 * Function   : Phone_Config_SmsDelay_IsValid
 *
 * Description: check if the "SMS delay" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Phone_Config_SmsDelay_IsValid(PHONE__CONFIG__SMS_DELAY *ptr_data)
{
    if (
       //(ptr_data->sms_delay_unit != PHONE__SMS_DELAY_UNIT__SECOND) &&   // not used
       //(ptr_data->sms_delay_unit != PHONE__SMS_DELAY_UNIT__MINUTE) &&   // not used
         (ptr_data->sms_delay_unit != PHONE__SMS_DELAY_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    if (ptr_data->sms_delay_value > 96)
        return FALSE;

    if (ptr_data->status)
    {
        if (ptr_data->sms_delay_value == 0)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_PhoneId_Get
 *
 * Description: get the phone IDs
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_PhoneId_Get(PHONE__PHONE_ID *ptr_data)
{
    *ptr_data = Phone_PhoneId;
}




/*===========================================================================
 * Function   : Phone_PhoneSwComponent_Get
 *
 * Description: get the phone SW component IDs
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_PhoneSwComponent_Get(PHONE__PHONE_SW_COMPONENTS_ID *ptr_data)
{
    *ptr_data = Phone_PhoneSwComponentsId;
}




/*===========================================================================
 * Function   : Phone_SimId_Get
 *
 * Description: get the SIM IDs
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_SimId_Get(PHONE__SIM_ID *ptr_data)
{
    *ptr_data = Phone_SimId;
}




/*===========================================================================
 * Function   : Phone_PhoneStatus_Get
 *
 * Description: get the phone status info
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_PhoneStatus_Get(PHONE__PHONE_STATUS *ptr_data)
{
    *ptr_data = Phone_PhoneStatus;
}




/*===========================================================================
 * Function   : Phone_SimStatus_Get
 *
 * Description: get the SIM status info
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_SimStatus_Get(PHONE__SIM_STATUS *ptr_data)
{
    *ptr_data = Phone_SimStatus;
}




/*===========================================================================
 * Function   : Phone_PhoneActivity_GetDefault
 *
 * Description: get the default phone activity info
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivity_GetDefault(PHONE__PHONE_ACTIVITY *ptr_data)
{
    *ptr_data = Phone_PhoneActivityDefault;
}




/*===========================================================================
 * Function   : Phone_PhoneActivity_Get
 *
 * Description: get the phone activity info
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivity_Get(PHONE__PHONE_ACTIVITY *ptr_data)
{
    *ptr_data = Phone_PhoneActivity;
}




/*===========================================================================
 * Function   : Phone_PhoneActivity_Set
 *
 * Description: set the phone activity info
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivity_Set(PHONE__PHONE_ACTIVITY *ptr_data)
{
    Phone_PhoneActivity = *ptr_data;
}




/*=============================================================================
 * Function   : Phone_IsGsmNetworkRegistered
 *
 * Description: return if the phone is registerd on the GSM network
 * Input      : -
 * Output     : - FALSE: not registered on GSM network
 *              - TRUE :     registered on GSM network
 *=============================================================================*/
bool Phone_IsGsmNetworkRegistered(void)
{
    return Phone_GsmNetworkRegistered;
}




/*=============================================================================
 * Function   : Phone_IsGprsNetworkRegistered
 *
 * Description: return if the phone is registerd on the GPRS network
 * Input      : -
 * Output     : - FALSE: not registered on GPRS network
 *              - TRUE :     registered on GPRS network
 *=============================================================================*/
bool Phone_IsGprsNetworkRegistered(void)
{
    return Phone_GprsNetworkRegistered;
}




/*===========================================================================
 * Function   : Phone_PhoneActivityTotalReset
 *
 * Description: reset total phone activity counters and timers
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivityTotalReset(void)
{
    /* reset total phone activity counters and timers */

    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    Phone_PhoneActivity.phone_activity_total.n_sms_rx                      = 0;       // number of SMS received
    Phone_PhoneActivity.phone_activity_total.n_sms_tx_ok                   = 0;       // number of SMS transmitted with   success
    Phone_PhoneActivity.phone_activity_total.n_sms_tx_fail                 = 0;       // number of SMS transmitted with insuccess


    /* voice call rx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_answered_ok   = 0;       // number of voice calls received answered with   success
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_answered_fail = 0;       // number of voice calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_missed        = 0;       // number of voice calls received missed
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_refused       = 0;       // number of voice calls received refused

    /* voice call tx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_ok            = 0;       // number of voice calls done with   success
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_busy          = 0;       // number of voice calls done busy
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_refused       = 0;       // number of voice calls done refused
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_fail          = 0;       // number of voice calls done with insuccess


    /* data call rx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_answered_ok    = 0;       // number of data  calls received answered with   success
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_answered_fail  = 0;       // number of data  calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_missed         = 0;       // number of data  calls received missed
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_refused        = 0;       // number of data  calls received refused

    /* data call tx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_ok             = 0;       // number of data  calls done with   success
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_busy           = 0;       // number of data  calls done busy
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_refused        = 0;       // number of data  calls done refused
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_fail           = 0;       // number of data  calls done with insuccess


    /* GPRS call tx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_gprs_tx_ok             = 0;       // number of GPRS  calls done with   success
    Phone_PhoneActivity.phone_activity_total.n_call_gprs_tx_fail           = 0;       // number of GPRS  calls done with insuccess


    /*-------------------------------------------------------------------------
     * Timers
     *-------------------------------------------------------------------------*/
    /* voice call timer (sec) */
    Phone_PhoneActivity.phone_activity_total.time_call_voice_rx            = 0;       // duration of voice call received (sec)
    Phone_PhoneActivity.phone_activity_total.time_call_voice_tx            = 0;       // duration of voice call done     (sec)

    /* data call  timer (sec) */
    Phone_PhoneActivity.phone_activity_total.time_data_voice_rx            = 0;       // duration of data  call received (sec)
    Phone_PhoneActivity.phone_activity_total.time_data_voice_tx            = 0;       // duration of data  call done     (sec)


    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);
}




/*===========================================================================
 * Function   : Phone_PhoneActivityYearlyReset
 *
 * Description: reset yearly phone activity counters and timers
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivityYearlyReset(void)
{
    /* reset yearly phone activity counters and timers */

    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    Phone_PhoneActivity.phone_activity_yearly.n_sms_rx                      = 0;       // number of SMS received
    Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_ok                   = 0;       // number of SMS transmitted with   success
    Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_fail                 = 0;       // number of SMS transmitted with insuccess


    /* voice call rx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_answered_ok   = 0;       // number of voice calls received answered with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_answered_fail = 0;       // number of voice calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_missed        = 0;       // number of voice calls received missed
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_refused       = 0;       // number of voice calls received refused

    /* voice call tx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_ok            = 0;       // number of voice calls done with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_busy          = 0;       // number of voice calls done busy
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_refused       = 0;       // number of voice calls done refused
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_fail          = 0;       // number of voice calls done with insuccess


    /* data call rx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_answered_ok    = 0;       // number of data  calls received answered with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_answered_fail  = 0;       // number of data  calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_missed         = 0;       // number of data  calls received missed
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_refused        = 0;       // number of data  calls received refused

    /* data call tx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_ok             = 0;       // number of data  calls done with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_busy           = 0;       // number of data  calls done busy
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_refused        = 0;       // number of data  calls done refused
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_fail           = 0;       // number of data  calls done with insuccess


    /* GPRS call tx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_gprs_tx_ok             = 0;       // number of GPRS  calls done with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_gprs_tx_fail           = 0;       // number of GPRS  calls done with insuccess


    /*--------------------------------------------------------------------
     * Timers
     *--------------------------------------------------------------------*/
    /* voice call timer (sec) */
    Phone_PhoneActivity.phone_activity_yearly.time_call_voice_rx            = 0;       // duration of voice call received (sec)
    Phone_PhoneActivity.phone_activity_yearly.time_call_voice_tx            = 0;       // duration of voice call done     (sec)

    /* data call  timer (sec) */
    Phone_PhoneActivity.phone_activity_yearly.time_data_voice_rx            = 0;       // duration of data  call received (sec)
    Phone_PhoneActivity.phone_activity_yearly.time_data_voice_tx            = 0;       // duration of data  call done     (sec)


    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);
}




/*===========================================================================
 * Function   : Phone_PhoneActivityMonthlyReset
 *
 * Description: reset monthly phone activity counters and timers
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivityMonthlyReset(void)
{
    /* reset monthly phone activity counters and timers */

    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    Phone_PhoneActivity.phone_activity_monthly.n_sms_rx                      = 0;       // number of SMS received
    Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_ok                   = 0;       // number of SMS transmitted with   success
    Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_fail                 = 0;       // number of SMS transmitted with insuccess


    /* voice call rx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_answered_ok   = 0;       // number of voice calls received answered with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_answered_fail = 0;       // number of voice calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_missed        = 0;       // number of voice calls received missed
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_refused       = 0;       // number of voice calls received refused

    /* voice call tx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_ok            = 0;       // number of voice calls done with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_busy          = 0;       // number of voice calls done busy
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_refused       = 0;       // number of voice calls done refused
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_fail          = 0;       // number of voice calls done with insuccess


    /* data call rx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_answered_ok    = 0;       // number of data  calls received answered with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_answered_fail  = 0;       // number of data  calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_missed         = 0;       // number of data  calls received missed
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_refused        = 0;       // number of data  calls received refused

    /* data call tx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_ok             = 0;       // number of data  calls done with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_busy           = 0;       // number of data  calls done busy
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_refused        = 0;       // number of data  calls done refused
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_fail           = 0;       // number of data  calls done with insuccess


    /* GPRS call tx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_gprs_tx_ok             = 0;       // number of GPRS  calls done with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_gprs_tx_fail           = 0;       // number of GPRS  calls done with insuccess


    /*-------------------------------------------------------------------------
     * Timers
     *-------------------------------------------------------------------------*/
    /* voice call timer (sec) */
    Phone_PhoneActivity.phone_activity_monthly.time_call_voice_rx            = 0;       // duration of voice call received (sec)
    Phone_PhoneActivity.phone_activity_monthly.time_call_voice_tx            = 0;       // duration of voice call done     (sec)

    /* data call  timer (sec) */
    Phone_PhoneActivity.phone_activity_monthly.time_data_voice_rx            = 0;       // duration of data  call received (sec)
    Phone_PhoneActivity.phone_activity_monthly.time_data_voice_tx            = 0;       // duration of data  call done     (sec)


    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);
}




/*===========================================================================
 * Function   : Phone_PhoneActivityReset
 *
 * Description: reset total, yearly and monthly phone activity counters and timers
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Phone_PhoneActivityReset(void)
{
    /* reset total, yearly and monthly counters and timers */

    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    Phone_PhoneActivity.phone_activity_total.n_sms_rx                      = 0;       // number of SMS received
    Phone_PhoneActivity.phone_activity_total.n_sms_tx_ok                   = 0;       // number of SMS transmitted with   success
    Phone_PhoneActivity.phone_activity_total.n_sms_tx_fail                 = 0;       // number of SMS transmitted with insuccess


    /* voice call rx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_answered_ok   = 0;       // number of voice calls received answered with   success
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_answered_fail = 0;       // number of voice calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_missed        = 0;       // number of voice calls received missed
    Phone_PhoneActivity.phone_activity_total.n_call_voice_rx_refused       = 0;       // number of voice calls received refused

    /* voice call tx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_ok            = 0;       // number of voice calls done with   success
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_busy          = 0;       // number of voice calls done busy
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_refused       = 0;       // number of voice calls done refused
    Phone_PhoneActivity.phone_activity_total.n_call_voice_tx_fail          = 0;       // number of voice calls done with insuccess


    /* data call rx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_answered_ok    = 0;       // number of data  calls received answered with   success
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_answered_fail  = 0;       // number of data  calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_missed         = 0;       // number of data  calls received missed
    Phone_PhoneActivity.phone_activity_total.n_call_data_rx_refused        = 0;       // number of data  calls received refused

    /* data call tx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_ok             = 0;       // number of data  calls done with   success
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_busy           = 0;       // number of data  calls done busy
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_refused        = 0;       // number of data  calls done refused
    Phone_PhoneActivity.phone_activity_total.n_call_data_tx_fail           = 0;       // number of data  calls done with insuccess


    /* GPRS call tx counters */
    Phone_PhoneActivity.phone_activity_total.n_call_gprs_tx_ok             = 0;       // number of GPRS  calls done with   success
    Phone_PhoneActivity.phone_activity_total.n_call_gprs_tx_fail           = 0;       // number of GPRS  calls done with insuccess


    /*-------------------------------------------------------------------------
     * Timers
     *-------------------------------------------------------------------------*/
    /* voice call timer (sec) */
    Phone_PhoneActivity.phone_activity_total.time_call_voice_rx            = 0;       // duration of voice call received (sec)
    Phone_PhoneActivity.phone_activity_total.time_call_voice_tx            = 0;       // duration of voice call done     (sec)

    /* data call  timer (sec) */
    Phone_PhoneActivity.phone_activity_total.time_data_voice_rx            = 0;       // duration of data  call received (sec)
    Phone_PhoneActivity.phone_activity_total.time_data_voice_tx            = 0;       // duration of data  call done     (sec)



    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    Phone_PhoneActivity.phone_activity_yearly.n_sms_rx                      = 0;       // number of SMS received
    Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_ok                   = 0;       // number of SMS transmitted with   success
    Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_fail                 = 0;       // number of SMS transmitted with insuccess


    /* voice call rx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_answered_ok   = 0;       // number of voice calls received answered with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_answered_fail = 0;       // number of voice calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_missed        = 0;       // number of voice calls received missed
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_rx_refused       = 0;       // number of voice calls received refused

    /* voice call tx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_ok            = 0;       // number of voice calls done with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_busy          = 0;       // number of voice calls done busy
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_refused       = 0;       // number of voice calls done refused
    Phone_PhoneActivity.phone_activity_yearly.n_call_voice_tx_fail          = 0;       // number of voice calls done with insuccess


    /* data call rx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_answered_ok    = 0;       // number of data  calls received answered with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_answered_fail  = 0;       // number of data  calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_missed         = 0;       // number of data  calls received missed
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_rx_refused        = 0;       // number of data  calls received refused

    /* data call tx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_ok             = 0;       // number of data  calls done with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_busy           = 0;       // number of data  calls done busy
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_refused        = 0;       // number of data  calls done refused
    Phone_PhoneActivity.phone_activity_yearly.n_call_data_tx_fail           = 0;       // number of data  calls done with insuccess


    /* GPRS call tx counters */
    Phone_PhoneActivity.phone_activity_yearly.n_call_gprs_tx_ok             = 0;       // number of GPRS  calls done with   success
    Phone_PhoneActivity.phone_activity_yearly.n_call_gprs_tx_fail           = 0;       // number of GPRS  calls done with insuccess


    /*--------------------------------------------------------------------
     * Timers
     *--------------------------------------------------------------------*/
    /* voice call timer (sec) */
    Phone_PhoneActivity.phone_activity_yearly.time_call_voice_rx            = 0;       // duration of voice call received (sec)
    Phone_PhoneActivity.phone_activity_yearly.time_call_voice_tx            = 0;       // duration of voice call done     (sec)

    /* data call  timer (sec) */
    Phone_PhoneActivity.phone_activity_yearly.time_data_voice_rx            = 0;       // duration of data  call received (sec)
    Phone_PhoneActivity.phone_activity_yearly.time_data_voice_tx            = 0;       // duration of data  call done     (sec)



    /*-------------------------------------------------------------------------
     * Counters
     *-------------------------------------------------------------------------*/
    /* SMS counters */
    Phone_PhoneActivity.phone_activity_monthly.n_sms_rx                      = 0;       // number of SMS received
    Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_ok                   = 0;       // number of SMS transmitted with   success
    Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_fail                 = 0;       // number of SMS transmitted with insuccess


    /* voice call rx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_answered_ok   = 0;       // number of voice calls received answered with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_answered_fail = 0;       // number of voice calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_missed        = 0;       // number of voice calls received missed
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_rx_refused       = 0;       // number of voice calls received refused

    /* voice call tx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_ok            = 0;       // number of voice calls done with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_busy          = 0;       // number of voice calls done busy
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_refused       = 0;       // number of voice calls done refused
    Phone_PhoneActivity.phone_activity_monthly.n_call_voice_tx_fail          = 0;       // number of voice calls done with insuccess


    /* data call rx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_answered_ok    = 0;       // number of data  calls received answered with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_answered_fail  = 0;       // number of data  calls received answered with insuccess
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_missed         = 0;       // number of data  calls received missed
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_rx_refused        = 0;       // number of data  calls received refused

    /* data call tx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_ok             = 0;       // number of data  calls done with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_busy           = 0;       // number of data  calls done busy
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_refused        = 0;       // number of data  calls done refused
    Phone_PhoneActivity.phone_activity_monthly.n_call_data_tx_fail           = 0;       // number of data  calls done with insuccess


    /* GPRS call tx counters */
    Phone_PhoneActivity.phone_activity_monthly.n_call_gprs_tx_ok             = 0;       // number of GPRS  calls done with   success
    Phone_PhoneActivity.phone_activity_monthly.n_call_gprs_tx_fail           = 0;       // number of GPRS  calls done with insuccess


    /*-------------------------------------------------------------------------
     * Timers
     *-------------------------------------------------------------------------*/
    /* voice call timer (sec) */
    Phone_PhoneActivity.phone_activity_monthly.time_call_voice_rx            = 0;       // duration of voice call received (sec)
    Phone_PhoneActivity.phone_activity_monthly.time_call_voice_tx            = 0;       // duration of voice call done     (sec)

    /* data call  timer (sec) */
    Phone_PhoneActivity.phone_activity_monthly.time_data_voice_rx            = 0;       // duration of data  call received (sec)
    Phone_PhoneActivity.phone_activity_monthly.time_data_voice_tx            = 0;       // duration of data  call done     (sec)



    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);
}




/*=============================================================================
 * Function   : Phone_TimerPhonePolling_Start
 *
 * Description: start the "phone polling" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Phone_TimerPhonePolling_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Start \"phone polling\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Phone_TimerHandler_TimerPhonePolling, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Phone_TimerPhonePolling_Stop
 *
 * Description: stop the "phone polling" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Phone_TimerPhonePolling_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Stop \"phone polling\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Phone_TimerHandler_TimerPhonePolling);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function    : Phone_SendSms
 *
 * Description : - send an SMS in text format
 * Input       : - ptr_phone_num: pointer to the receiver phone number
 *               - ptr_sms_text : pointer to the text message
 * Output      : - FALSE: error
 *               - TRUE : OK
 *===========================================================================*/
bool Phone_SendSms(ascii *ptr_phone_num, ascii *ptr_sms_text)
{
    bool             sms_tx_ok;

    ql_sms_errcode_e res_sms;


    /* verify of input arguments */
    if (!Utility_IsPhoneString(ptr_phone_num))
        return FALSE;
    if (strlen(ptr_sms_text) == 0)
        return FALSE;


    /* verify SMS tx counter enable status */
    if (Phone_Config_PhoneActivityCounters.monthly_counter_enable_sms_tx)
    {
        /* verify SMS tx counter value */
        if (Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_ok >= Phone_Config_PhoneActivityCounters.monthly_counter_max_sms_tx)
            return FALSE;   // monthly SMS tx counter greater than max monthly SMS tx counter
    }


    /* print SMS phone number and SMS text */
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "number: \"%s\", text: \"%s\"", ptr_phone_num, ptr_sms_text);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* send the SMS */
    res_sms = ql_sms_send_msg(0, ptr_phone_num, ptr_sms_text, GSM);
    if (res_sms != QL_SMS_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_sms_send_msg ERROR %d", res_sms);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW , Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if (!Phone_MyGsm_MyCgms_Status)
        {
            /* "My CGMS" disabled */
            sms_tx_ok = FALSE;
        }
        else
        {
            /* "My CGMS" enabled */
            if (Phone_MyGsm_MyCgms_Value)
                sms_tx_ok = TRUE;
            else
                sms_tx_ok = FALSE;
        }

        if (!sms_tx_ok)
        {
            /* increase the total SMS transmitted counter, yearly SMS transmitted counter and monthly SMS transmitted counter with insuccess */
            Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_fail++;
            Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_fail++;
            Phone_PhoneActivity.phone_activity_total.n_sms_tx_fail++;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);

            TransmissionSms_Event_SmsTxFail();
        }
        else
        {
            TransmissionSms_Event_SmsTxOk();
        }
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sms_send_msg OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if (!Phone_MyGsm_MyCgms_Status)
        {
            /* "My CGMS" disabled */
            sms_tx_ok = TRUE;
        }
        else
        {
            /* "My CGMS" enabled */
            if (Phone_MyGsm_MyCgms_Value)
                sms_tx_ok = TRUE;
            else
                sms_tx_ok = FALSE;
        }

        if (sms_tx_ok)
        {
            /* increase the total SMS transmitted counter, yearly SMS transmitted counter and monthly SMS transmitted counter with success */
            Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_ok++;
            Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_ok++;
            Phone_PhoneActivity.phone_activity_total.n_sms_tx_ok++;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);

            /* update the available SMS credit */
            Credit_ConsumeOneCredit();

            TransmissionSms_Event_SmsTxOk();
        }
        else
        {
            TransmissionSms_Event_SmsTxFail();
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Phone_GprsStart
 *
 * Description: -
 * Input      : -
 * Output     : - FALSE: error
 *              - TRUE : OK
 *===========================================================================*/
bool Phone_GprsStart(void)
{
    ql_datacall_dns_info_s dns_pri;
    ql_datacall_dns_info_s dns_sec;

    ql_datacall_errcode_e  ret;


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_start_data_call...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check if the GPRS APN is configured */
    if (strlen(Phone_Config_GprsApnParameters.apn_server) == 0)
        return FALSE;


    /* configure the GPRS bearer */
    ret = ql_set_data_call_asyn_mode(0, Phone_BearerHandler_Gprs, 1);
    if (ret != QL_DATACALL_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_set_data_call_asyn_mode ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }


    if (
         (strlen(Phone_Config_GprsApnParameters.apn_user_name) > 0) &&
         (strlen(Phone_Config_GprsApnParameters.apn_password ) > 0)
       )
    {
        ret = ql_start_data_call(0, Phone_BearerHandler_Gprs, QL_PDP_TYPE_IP, Phone_Config_GprsApnParameters.apn_server, Phone_Config_GprsApnParameters.apn_user_name, Phone_Config_GprsApnParameters.apn_password, 1);
    }
    else
    {
        ret = ql_start_data_call(0, Phone_BearerHandler_Gprs, QL_PDP_TYPE_IP, Phone_Config_GprsApnParameters.apn_server, NULL, NULL, 0);
    }

    if (ret != QL_DATACALL_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_start_data_call ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_start_data_call OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    if (
         (strlen(Phone_Config_GprsDnsParameters.dns_1) > 0) &&
         (strlen(Phone_Config_GprsDnsParameters.dns_2) > 0)
       )
    {
        /* DNS server configured */

        dns_pri.type = QL_PDP_TYPE_IP;
        ip4addr_aton(Phone_Config_GprsDnsParameters.dns_1, &(dns_pri.ip4));

        dns_sec.type = QL_PDP_TYPE_IP;
        ip4addr_aton(Phone_Config_GprsDnsParameters.dns_2, &(dns_sec.ip4));

        ret = ql_datacall_set_dns_addr(0, Phone_BearerHandler_Gprs, &dns_pri, &dns_sec);
        if (ret != QL_DATACALL_SUCCESS)
        {
            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_datacall_set_dns_addr ERROR: %d", ret);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            return FALSE;
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Phone_GprsStop
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Phone_GprsStop(void)
{
    ql_datacall_errcode_e ret;


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_stop_data_call...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* GPRS bearer stop */
    ret = ql_stop_data_call(0, Phone_BearerHandler_Gprs);
    if (ret != QL_DATACALL_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_stop_data_call ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_stop_data_call OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Phone_EnterSimPin
 *
 * Description: enter the SIM PIN
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Phone_EnterSimPin(void)
{
    u8   num_pin_attempts_left;

    bool result;


    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        Phone_SimPinProblem();
        return;
    }


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Try to enter the PIN...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* try to enter the PIN */
    result = Phone_SimPinProcedure_EnterPin(&Phone_SimPinType);
    if (!result)
    {
        Phone_SimPinProblem();
        return;
    }
}




/*=============================================================================
 * Function   : Phone_ManageSimPin
 *
 * Description: manage the SIM PIN
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Phone_ManageSimPin(void)
{
    u8   num_pin_attempts_left;

    bool result;


    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        Phone_SimPinProblem();
        return;
    }


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "Phone_SimPinStatus: %d", Phone_SimPinStatus);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* manage the SIM status */
    switch (Phone_SimPinStatus)
    {
        /* SIM ready (no PIN1 or PUK1 required) */
        case SIM_STATUS__READY:
            if (Phone_Config_SimPin.use_pin)
            {
                /* PIN has to be used */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Try to enable PIN request...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* try to enable the PIN request */
                result = Phone_SimPinProcedure_EnablePin(&Phone_SimPinType);
                if (!result)
                {
                    Phone_SimPinProblem();
                    return;
                }

                if (Phone_SimPinType == SIM_PIN_TYPE__DEFAULT_PIN)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Try to change PIN from <default_pin> to <ccid_sim>...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* try to change the PIN from <default_pin> to <ccid_sim> */
                    result = Phone_SimPinProcedure_ChangePin_DefaultToCcid(&Phone_SimPinType);
                    if (!result)
                    {
                        Phone_SimPinProblem();
                        return;
                    }
                }

                Phone_SimPinInit = TRUE;
            }
            else
            {
                /* PIN has not to be used */

                // NOTHING TO DO

                Phone_SimPinInit = TRUE;
            }
            break;


        /* PIN1 is required */
        case SIM_STATUS__NEED_PIN:
            if (Phone_Config_SimPin.use_pin)
            {
                /* PIN has to be used */

                if (Phone_SimPinType == SIM_PIN_TYPE__DEFAULT_PIN)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Try to change PIN from <default_pin> to <ccid_sim>...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* try to change the PIN from <default_pin> to <ccid_sim> */
                    result = Phone_SimPinProcedure_ChangePin_DefaultToCcid(&Phone_SimPinType);
                    if (!result)
                    {
                        Phone_SimPinProblem();
                        return;
                    }
                }

                Phone_SimPinInit = TRUE;
            }
            else
            {
                /* PIN has not to be used */

                if (Phone_SimPinType == SIM_PIN_TYPE__CCID_PIN)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Try to change PIN from <ccid_sim> to <default_pin>...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* try to change the PIN from <ccid_sim> to <default_pin> */
                    result = Phone_SimPinProcedure_ChangePin_CcidToDefault(&Phone_SimPinType);
                    if (!result)
                    {
                        Phone_SimPinProblem();
                        return;
                    }
                }

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Try to disable PIN request...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* try to disable the PIN request */
                result = Phone_SimPinProcedure_DisablePin(&Phone_SimPinType);
                if (!result)
                {
                    Phone_SimPinProblem();
                    return;
                }

                Phone_SimPinInit = TRUE;
            }
            break;


        /* PUK1 is required */
        case SIM_STATUS__NEED_PUK:
            Phone_SimPinProblem();
            return;
    }
}




/*=============================================================================
 * Function   : Phone_SimPinProcedure_EnterPin
 *
 * Description: try to enter the SIM PIN
 *                  - 1st attempt: using "CCID    PIN"
 *                  - 2nd attempt: using "default PIN"
 *              The attempts are done only if there are at least 2 PIN attempts left
 * Input      : -
 * Output     : - FALSE: procedure not terminated with success
 *              - TRUE : procedure     terminated with success
 *=============================================================================*/
static bool Phone_SimPinProcedure_EnterPin(u8 *ptr_sim_pin_type)
{
    ascii ccid_pin   [4 + 1];   // CCID    PIN (4 digits)
    ascii default_pin[4 + 1];   // default PIN (4 digits)

    u8    num_pin_attempts_left;

    bool  result;


    /* get the SIM CCID */
    Phone_GetSimCcid();                             // AT+CCID

    /* calculate the "CCID PIN" */
    Phone_CalculateCcidPin(Phone_SimCcid, ccid_pin);


    /* --- 1st ATTEMPTS --- */

    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to enter the "CCID PIN" */
    result = Phone_EnterPin(ccid_pin);              // AT+CPIN=<ccid_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__CCID_PIN;
        return TRUE;
    }


    /* --- 2nd ATTEMPTS --- */

    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to enter the "default PIN" */
    strncpy(default_pin, DEFAULT_PIN, 4);
    default_pin[4] = 0x00;
    result = Phone_EnterPin(default_pin);           // AT+CPIN=<default_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__DEFAULT_PIN;
        return TRUE;
    }


    *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
    return FALSE;
}




/*=============================================================================
 * Function   : Phone_SimPinProcedure_EnablePin
 *
 * Description: try to enable the SIM PIN request
 *                  - 1st attempt: using "CCID    PIN"
 *                  - 2nd attempt: using "default PIN"
 *              The attempts are done only if there are at least 2 PIN attempts left
 * Input      : - ptr_sim_pin_type: pointer to save the SIM PIN type
 * Output     : - FALSE: procedure not terminated with success
 *              - TRUE : procedure     terminated with success
 *=============================================================================*/
static bool Phone_SimPinProcedure_EnablePin(u8 *ptr_sim_pin_type)
{
    ascii ccid_pin   [4 + 1];   // CCID    PIN (4 digits)
    ascii default_pin[4 + 1];   // default PIN (4 digits)

    u8    num_pin_attempts_left;

    bool  result;


    /* get the SIM CCID */
    Phone_GetSimCcid();                             // AT+CCID

    /* calculate the "CCID PIN" */
    Phone_CalculateCcidPin(Phone_SimCcid, ccid_pin);


    /* --- 1st ATTEMPTS --- */

    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* 1st attempt - try to enable the PIN request using the "CCID PIN" */
    result = Phone_EnablePinRequest(ccid_pin);      // AT+CLCK="SC",1,<ccid_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__CCID_PIN;
        return TRUE;
    }


    /* --- 2nd ATTEMPTS --- */

    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to enable the PIN request using the "default PIN" */
    strncpy(default_pin, DEFAULT_PIN, 4);
    default_pin[4] = 0x00;
    result = Phone_EnablePinRequest(default_pin);   // AT+CLCK="SC",1,<default_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__DEFAULT_PIN;
        return TRUE;
    }


    *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
    return FALSE;
}




/*=============================================================================
 * Function   : Phone_SimPinProcedure_DisablePin
 *
 * Description: try to disable the SIM PIN request
 *                  - 1st attempt: using "CCID    PIN"
 *                  - 2nd attempt: using "default PIN"
 *              The attempts are done only if there are at least 2 PIN attempts left
 * Input      : -
 * Output     : - FALSE: procedure not terminated with success
 *              - TRUE : procedure     terminated with success
 *=============================================================================*/
static bool Phone_SimPinProcedure_DisablePin(u8 *ptr_sim_pin_type)
{
    ascii ccid_pin   [4 + 1];   // CCID    PIN (4 digits)
    ascii default_pin[4 + 1];   // default PIN (4 digits)

    u8    num_pin_attempts_left;

    bool  result;


    /* get the SIM CCID */
    Phone_GetSimCcid();                             // AT+CCID

    /* calculate the "CCID PIN" */
    Phone_CalculateCcidPin(Phone_SimCcid, ccid_pin);


    /* --- 1st ATTEMPTS --- */

    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to disable the PIN request using the "CCID PIN" */
    result = Phone_DisablePinRequest(ccid_pin);     // AT+CLCK="SC",0,<ccid_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__CCID_PIN;
        return TRUE;
    }


    /* --- 2nd ATTEMPTS --- */

    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to disable the PIN request using the "default PIN" */
    strncpy(default_pin, DEFAULT_PIN, 4);
    default_pin[4] = 0x00;
    result = Phone_DisablePinRequest(default_pin);  // AT+CLCK="SC",0,<default_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__DEFAULT_PIN;
        return TRUE;
    }


    *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
    return FALSE;
}




/*=============================================================================
 * Function   : Phone_SimPinProcedure_ChangePin_DefaultToCcid
 *
 * Description: try to change the SIM PIN from "default PIN" to "CCID PIN"
 *              The attempt is done only if there are at least 2 PIN attempts left
 * Input      : -
 * Output     : - FALSE: procedure not terminated with success
 *              - TRUE : procedure     terminated with success
 *=============================================================================*/
static bool Phone_SimPinProcedure_ChangePin_DefaultToCcid(u8 *ptr_sim_pin_type)
{
    ascii ccid_pin   [4 + 1];   // CCID    PIN (4 digits)
    ascii default_pin[4 + 1];   // default PIN (4 digits)

    u8    num_pin_attempts_left;

    bool  result;


    /* get the SIM CCID */
    Phone_GetSimCcid();                                 // AT+CCID

    /* calculate the "CCID PIN" */
    Phone_CalculateCcidPin(Phone_SimCcid, ccid_pin);


    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to change PIN from "default PIN" to "CCID PIN" */
    strncpy(default_pin, DEFAULT_PIN, 4);
    default_pin[4] = 0x00;
    result = Phone_ChangePin(default_pin, ccid_pin);    // AT+CPWD="SC",<default_pin>,<ccid_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__DEFAULT_PIN;
        return TRUE;
    }


    *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
    return FALSE;
}




/*=============================================================================
 * Function   : Phone_SimPinProcedure_ChangePin_CcidToDefault
 *
 * Description: try to change the SIM PIN from "CCID PIN" to "default PIN"
 *              The attempt is done only if there are at least 2 PIN attempts left
 * Input      : -
 * Output     : - FALSE: procedure not terminated with success
 *              - TRUE : procedure     terminated with success
 *=============================================================================*/
static bool Phone_SimPinProcedure_ChangePin_CcidToDefault(u8 *ptr_sim_pin_type)
{
    ascii ccid_pin   [4 + 1];   // CCID    PIN (4 digits)
    ascii default_pin[4 + 1];   // default PIN (4 digits)

    u8    num_pin_attempts_left;

    bool  result;


    /* get the SIM CCID */
    Phone_GetSimCcid();                                 // AT+CCID

    /* calculate the "CCID PIN" */
    Phone_CalculateCcidPin(Phone_SimCcid, ccid_pin);


    /* read the number of PIN attempts left */
    result = Phone_GetSimPinAttemptsLeft();
    num_pin_attempts_left = Phone_SimPinAttemptsLeft;
    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "SIM PIN attempts left: %d", num_pin_attempts_left);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    if (num_pin_attempts_left < 2)
    {
        /* minus than 2 PIN attempts left */
        *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
        return FALSE;
    }

    /* try to change PIN from "CCID PIN" to " default PIN" */
    strncpy(default_pin, DEFAULT_PIN, 4);
    default_pin[4] = 0x00;
    result = Phone_ChangePin(ccid_pin, default_pin);    // AT+CPWD="SC",<ccid_pin>,<default_pin>
    if (result)
    {
        *ptr_sim_pin_type = SIM_PIN_TYPE__CCID_PIN;
        return TRUE;
    }


    *ptr_sim_pin_type = SIM_PIN_TYPE__UNKNOWN_PIN;
    return FALSE;
}




/*=============================================================================
 * Function   : Phone_CalculateCcidPin
 *
 * Description: build the "CCID PIN" (4 digits) using the SIM CCID
 *
 *              Example:
 *
 *                 CCID: 8939880342012645091
 *
 *                 1) 342012645091    (take last 12 digits of CCID string)
 *                 2) 3420 1264 5091  (build 3 blocks of 4 digits)
 *                 3) Sum the digits in the following way
 *                       3 + 1 + 5  =  9                9
 *                       4 + 2 + 0  =  6                6
 *                       2 + 6 + 9  =  17  -->  1 + 7 = 8
 *                       0 + 4 + 1  =  5                5
 *                 4) PIN = 9685  (4 digits)
 *
 *              If the CCID string has problems (not digit string or string length < 12),
 *              the default PIN ("1234") is used
 *
 * Input      : - ccid    : pointer to the          SIM CCID   string
 *              - ccid_pin: pointer to the save the "CCID PIN" string
 * Output     : - pointer to the "PIN CCID" string
 *=============================================================================*/
static void Phone_CalculateCcidPin(ascii *ccid, ascii *ccid_pin)
{
    u8   pin_digit_1;
    u8   pin_digit_2;
    u8   pin_digit_3;
    u8   pin_digit_4;

    u8   len_ccid;

    bool ccid_ok;


    len_ccid = (u8)strlen(ccid);

    if (
         (Utility_IsNumString(ccid)) &&
         (len_ccid >= 12           )
       )
    {
        ccid_ok = TRUE;
    }
    else
    {
        ccid_ok = FALSE;
    }


    if (ccid_ok)
    {
        /* CCID string is OK */

        /* sum the digits */
        pin_digit_1 = (ccid[len_ccid - 12    ] - '0')  +  (ccid[len_ccid - 12 + 4] - '0')  +  (ccid[len_ccid - 12 +  8] - '0');
        pin_digit_2 = (ccid[len_ccid - 12 + 1] - '0')  +  (ccid[len_ccid - 12 + 5] - '0')  +  (ccid[len_ccid - 12 +  9] - '0');
        pin_digit_3 = (ccid[len_ccid - 12 + 2] - '0')  +  (ccid[len_ccid - 12 + 6] - '0')  +  (ccid[len_ccid - 12 + 10] - '0');
        pin_digit_4 = (ccid[len_ccid - 12 + 3] - '0')  +  (ccid[len_ccid - 12 + 7] - '0')  +  (ccid[len_ccid - 12 + 11] - '0');

        /* if a result is >= 10, sum the 2 digits of the result (1st time) */
        if (pin_digit_1 >= 10)
            pin_digit_1 = (pin_digit_1 / 10) + (pin_digit_1 % 10);
        if (pin_digit_2 >= 10)
            pin_digit_2 = (pin_digit_2 / 10) + (pin_digit_2 % 10);
        if (pin_digit_3 >= 10)
            pin_digit_3 = (pin_digit_3 / 10) + (pin_digit_3 % 10);
        if (pin_digit_4 >= 10)
            pin_digit_4 = (pin_digit_4 / 10) + (pin_digit_4 % 10);

        /* if a result is >= 10, sum the 2 digits of the result (2nd time) */
        if (pin_digit_1 >= 10)
            pin_digit_1 = (pin_digit_1 / 10) + (pin_digit_1 % 10);
        if (pin_digit_2 >= 10)
            pin_digit_2 = (pin_digit_2 / 10) + (pin_digit_2 % 10);
        if (pin_digit_3 >= 10)
            pin_digit_3 = (pin_digit_3 / 10) + (pin_digit_3 % 10);
        if (pin_digit_4 >= 10)
            pin_digit_4 = (pin_digit_4 / 10) + (pin_digit_4 % 10);

        /* build the "CCID PIN" ASCII string */
        ccid_pin[0] = pin_digit_1 + '0';
        ccid_pin[1] = pin_digit_2 + '0';
        ccid_pin[2] = pin_digit_3 + '0';
        ccid_pin[3] = pin_digit_4 + '0';
        ccid_pin[4] = 0x00;
    }
    else
    {
        /* CCID string is not OK */

        /* build the default ASCII string ("1234") */
        strncpy(ccid_pin, DEFAULT_PIN, 4);
        ccid_pin[4] = 0x00;
    }


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "\"CCID PIN\": %s", ccid_pin);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}




/*===========================================================================
 * Function   : Phone_GetSimCcid
 *
 * Description: get the SIM CCID
 *              It uses the AT+CCID command (AT+CCID)
 *              eg.   AT+CCID
 * Input      : -
 * Output     : - FALSE: action not done with success
 *              - TRUE : action     done with success
 *===========================================================================*/
static bool Phone_GetSimCcid(void)
{
    ql_sim_errcode_e ret;


    /* clear SIM CCID string */
    Phone_SimCcid[0] = 0x00;


    /* send the "AT+CCID" command */
    ret = ql_sim_get_iccid(0, Phone_SimCcid, PHONE__LEN_MAX_EF_CCID);
    if (ret != QL_SIM_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_sim_get_iccid ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Phone_SimCcid[0] = 0x00;

        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_iccid OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        Phone_SimCcid[PHONE__LEN_MAX_EF_CCID] = 0x00;

        return TRUE;
    }
}




/*===========================================================================
 * Function   : Phone_GetSimPinAttemptsLeft
 *
 * Description: get the SIM PIN attempts left
 *              It uses the AT+QPINC command (AT+QPINC)
 *              eg.   AT+QPINC
 * Input      : -
 * Output     : - FALSE: action not done with success
 *              - TRUE : action     done with success
 *===========================================================================*/
static bool Phone_GetSimPinAttemptsLeft(void)
{
    const char at_cmd_string[] = AT_COM_QPINC;

          u8   event_id;
          int  res_int;
          bool result;


    /* clear SIM PIN attempts left */
    Phone_SimPinAttemptsLeft = 0;


    /* send the "AT+QPINC" command */
    res_int = ql_virt_at_write(QL_VIRT_AT_PORT_1, (unsigned char *)at_cmd_string, sizeof(at_cmd_string) - 1);
    if (res_int != sizeof(at_cmd_string) - 1)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_virt_at_write ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }


    /* wait for the AT answer */
    result = Phone_WaitEvent(EVENT_FLAG__EVENT_ANSWER_QPINC_OK, EVENT_FLAG__EVENT_ANSWER_QPINC_ERROR, TIMEOUT_WAIT_ANSWER_QPINC, &event_id);
    if (result)
    {
        if (event_id == 1)
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        return FALSE;
    }
}




/*===========================================================================
 * Function   : Phone_EnablePinRequest
 *
 * Description: try to enable the SIM PIN request
 *              It uses the AT+CLCK command (AT+CLCK="SC",1,<pin>)
 *              eg.   AT+CLCK="SC",1,1234
 * Input      : - ptr_pin: pointer to PIN string
 * Output     : - FALSE: action not done with success
 *              - TRUE : action     done with success
 *===========================================================================*/
static bool Phone_EnablePinRequest(ascii *ptr_pin)
{
    ql_sim_verify_pin_info_s pin;
    ql_sim_errcode_e         ret;


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "Enable the PIN \"%s\"...", ptr_pin);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* enable the pin */
    strncpy((char *)(pin.pin_value), ptr_pin, QL_SIM_PIN_LEN_MAX);
    pin.pin_value[QL_SIM_PIN_LEN_MAX] = 0x00;

    ret = ql_sim_enable_pin(0, &pin);
    if (ret != QL_SIM_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_sim_enable_pin ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }

    Phone_SimStatus.sim_status = PHONE__SIM_STATUS__SIM_PIN;

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_DisablePinRequest
 *
 * Description: try to disable the SIM PIN request
 *              It uses the AT+CLCK command (AT+CLCK="SC",0,<pin>)
 *              eg.   AT+CLCK="SC",0,1234
 * Input      : - ptr_pin: pointer to PIN string
 * Output     : - FALSE: action not done with success
 *              - TRUE : action     done with success
 *===========================================================================*/
static bool Phone_DisablePinRequest(ascii *ptr_pin)
{
    ql_sim_verify_pin_info_s pin;
    ql_sim_errcode_e         ret;


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "Disable the PIN \"%s\"...", ptr_pin);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* disable the pin */
    strncpy((char *)(pin.pin_value), ptr_pin, QL_SIM_PIN_LEN_MAX);
    pin.pin_value[QL_SIM_PIN_LEN_MAX] = 0x00;

    ret = ql_sim_disable_pin(0, &pin);
    if (ret != QL_SIM_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_sim_disable_pin ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }

    Phone_SimStatus.sim_status = PHONE__SIM_STATUS__READY;

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_EnterPin
 *
 * Description: try to enter the SIM PIN
 *              It uses the AT+CPIN command (AT+CPIN=<pin>)
 *              eg.   AT+CPIN=1234
 * Input      : - ptr_pin: pointer to PIN string
 * Output     : - FALSE: action not done with success
 *              - TRUE : action     done with success
 *===========================================================================*/
static bool Phone_EnterPin(ascii *ptr_pin)
{
    ql_sim_verify_pin_info_s pin;
    ql_sim_errcode_e         ret;


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "Enter the PIN \"%s\"...", ptr_pin);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* enter the pin */
    strncpy((char *)(pin.pin_value), ptr_pin, QL_SIM_PIN_LEN_MAX);
    pin.pin_value[QL_SIM_PIN_LEN_MAX] = 0x00;

    ret = ql_sim_verify_pin(0, &pin);
    if      (ret == QL_SIM_SUCCESS)
    {
        /* PIN code OK */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PIN code OK"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return TRUE;
    }
    else if (ret == QL_SIM_CFW_PIN_VERIFY_REQUEST_ERR)
    {
        /* PIN code error */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PIN code error" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
    else
    {
        /* Other SIM state */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Other SIM state", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
}




/*===========================================================================
 * Function   : Phone_ChangePin
 *
 * Description: try to change the SIM PIN
 *              It uses the AT+CPWD command (AT+CPWD="SC",<old_pin>,<new_pin>     )
 *              eg.   AT+CPWD="SC",1234,5678
 * Input      : - ptr_old_pin: pointer to old PIN string
 *              - ptr_new_pin: pointer to new PIN string
 * Output     : - FALSE: action not done with success
 *              - TRUE : action     done with success
 *===========================================================================*/
static bool Phone_ChangePin(ascii *ptr_old_pin, ascii *ptr_new_pin)
{
    ql_sim_change_pin_info_s pin_change;
    ql_sim_errcode_e         ret;


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "Change the PIN \"%s\" with \"%s\"...", ptr_old_pin, ptr_new_pin);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* change the pin */
    strncpy((char *)(pin_change.old_pin_value), ptr_old_pin, QL_SIM_PIN_LEN_MAX);
    pin_change.old_pin_value[QL_SIM_PIN_LEN_MAX] = 0x00;

    strncpy((char *)(pin_change.new_pin_value), ptr_new_pin, QL_SIM_PIN_LEN_MAX);
    pin_change.new_pin_value[QL_SIM_PIN_LEN_MAX] = 0x00;

    ret = ql_sim_change_pin(0, &pin_change);
    if (ret != QL_SIM_SUCCESS)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ql_sim_change_pin ERROR: %d", ret);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Phone_WaitEvent
 *
 * Description: wait for an event with waiting timeout
 * Input      : - event_1     : event 1 to be waited
 *            : - event_2     : event 2 to be waited
 *              - timeout     : waiting timeout (ms)
 *              - ptr_event_id: pointer to save the event id happened (1 or 2)
 * Output     : - FALSE: expected event not received in time
 *              - TRUE : expected event     received in time
 *===========================================================================*/
static bool Phone_WaitEvent(ql_event_bits_t event_1, ql_event_bits_t event_2, u32 timeout, u8 *ptr_event_id)
{
    ql_event_bits_t out_event_flags;
    QlOSStatus      res_int;


    while (1)
    {
        /* wait for the event with timeout */
        res_int = ql_rtos_event_group_wait(Phone_EventHandler_Events, (event_1 | event_2), TRUE, FALSE, &out_event_flags, timeout);
        if (res_int != QL_OSI_SUCCESS)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_event_group_wait - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        else
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_event_group_wait - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* analyze the event waiting result */
        if      (res_int == QL_OSI_SUCCESS)
        {
            if      (out_event_flags & event_1)
            {
                /* expected answer in time
                 * received answer equal to expected answer 1 */

                *ptr_event_id = 1;

                return TRUE;
            }
            else if (out_event_flags & event_2)
            {
                /* expected answer in time
                 * received answer equal to expected answer 2 */

                *ptr_event_id = 2;

                return TRUE;
            }
            else
            {
                /* no expected answer in time */

                /* reset module */
                Main_RestartModule();

                *ptr_event_id = 0;

                return FALSE;
            }
        }
        else if (res_int == QL_OSI_EGROUP_WAIT_BITS_FAIL)
        {
            //////////////////////////////////////////////////////
            /* the adl_eventWait function was called from Task 0 context */
            /* SHUTDOWN or DOTA command (by SMS or AT command) */

            /* temporary solution !!! */
            //@@@ TO DO BETTER

            *ptr_event_id = 0;

            return TRUE;
            //////////////////////////////////////////////////////
            /* no expected answer in time */

            /* reset module */
          //Main_ResetModule();

          //*ptr_event_id = 0;

          //return FALSE;
            //////////////////////////////////////////////////////
        }
    }


    *ptr_event_id = 0;

    return FALSE;
}




/*=============================================================================
 * Function   : Phone_SimPinProblem
 *
 * Description:
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Phone_SimPinProblem(void)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "SIM problem", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* SIM problem indication */
    Phone_SimProblem = TRUE;

    UiLed_SignalSimProblemStart();

    return;
}




#ifdef FUNCTION_IMPLEMENTED
/*===========================================================================
 * Function   : Phone_AdlCallback_Service_Sim
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Phone_AdlCallback_Service_Sim(u8 event)
{
    ql_event_t event;
    QlOSStatus err;


    switch (event)
    {
        // SIM removed
        case ADL_SIM_STATE_REMOVED:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_REMOVED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Phone_SimPinProblem();
            break;


        // SIM inserted, PIN code not entered yet
        case ADL_SIM_STATE_PIN_WAIT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_PIN_WAIT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (Phone_SimStatus.sim_status == SIM_STATUS__UNKNOWN)
                Phone_SimStatus.sim_status = PHONE__SIM_STATUS__SIM_PIN;

            if (Phone_SimPinStatus == SIM_STATUS__UNKNOWN)
                Phone_SimPinStatus = SIM_STATUS__NEED_PIN;

            event.id = PHONE__TASK_MSG_ID__SIM_PIN_WAIT;
            err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
            if (err != QL_OSI_SUCCESS)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            else
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;


        // PIN locked, PUK is required
        case ADL_SIM_STATE_PUK_WAIT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_PUK_WAIT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (Phone_SimStatus.sim_status == SIM_STATUS__UNKNOWN)
                Phone_SimStatus.sim_status = PHONE__SIM_STATUS__SIM_PUK;

            if (Phone_SimPinStatus == SIM_STATUS__UNKNOWN)
                Phone_SimPinStatus = SIM_STATUS__NEED_PUK;

            event.id = PHONE__TASK_MSG_ID__SIM_PUK_WAIT;
            err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
            if (err != QL_OSI_SUCCESS)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            else
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;


        // SIM Full Init done
        case ADL_SIM_STATE_FULL_INIT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_FULL_INIT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (Phone_SimStatus.sim_status == SIM_STATUS__UNKNOWN)
                Phone_SimStatus.sim_status = PHONE__SIM_STATUS__READY;

            /* read SIM IDs */
            Phone_ReadSimId();

            /* init for SMS */
            Phone_InitForSms();

            /* init for CLIP */
            Phone_InitForClip();

            /* flag: SIM full init done */
            Phone_SimFullInit = TRUE;

            event.id = PHONE__TASK_MSG_ID__SIM_FULL_INIT;
            err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
            if (err != QL_OSI_SUCCESS)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            else
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;


        // Wrong PUK input
        case ADL_SIM_STATE_PUK_ERROR:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_PUK_ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Phone_SimPinProblem();
            break;


        // PUK locked, SIM unavailable
        case ADL_SIM_STATE_FAILURE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_FAILURE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Phone_SimPinProblem();
            break;


        // Network locked, phone to SIM
        case ADL_SIM_STATE_NET_LOCK:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SIM - ADL_SIM_STATE_NET_LOCK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Phone_SimPinProblem();
            break;


        // unknown
        default:
            Phone_SimPinProblem();
            break;
    }
}
#endif




/*=============================================================================
 * Function   : Phone_SmsReceived
 *
 * Description:
 * Input      : - ptr_sms_tel       : pointer to originating telephone number of the SMS          (in text mode)
 *              - ptr_sms_time_stamp: pointer to the SMS time stamp (e.g. "12/03/23,11:13:50+04") (in text mode)
 *              - ptr_sms_text      : pointer to the SMS text                                     (in text mode)
 * Output     : -
 *=============================================================================*/
static void Phone_SmsReceived(ascii *ptr_sms_tel, ql_sms_time_stamp_s *ptr_sms_time_stamp, ascii *ptr_sms_text)
{
    static QUEUE__SMS_ANSWER_QUEUE_RECORD  record_sms_answer;
    static QUEUE__SMS_FORWARD_QUEUE_RECORD record_sms_forward;
    static LOGS__RX_COMMAND_RECORD         rx_command_record;

           REPORT__CONFIG__REPORT          config_report;
           FORWARD__CONFIG__FORWARD        config_forward;

           CLOCK__TIME                     sms_time_tx;
           CLOCK__TIME                     current_time;
           CLOCK__TIME                     time;

           s32                             sms_delay_sec;
           u32                             sms_delay_max_sec;

    static ascii                          *ptr_answer_texts[PARSER_RX__NUM_MAX_ANSWERS];
    static ascii                           answer_texts    [PARSER_RX__NUM_MAX_ANSWERS][160 + 1];

           u8                              num_answers;

           bool                            is_adm;
           bool                            is_crs;
           bool                            is_fwd;

           u16                             phonebook_index;
    static ascii                           name[14 + 1];

           bool                            report;
           bool                            command_valid;
           bool                            answer;
           bool                            forward;

           bool                            command_discarded;

           bool                            result;
           bool                            result_1;
           bool                            result_2;
           bool                            parser_result;

           u8                              i;


    /*-----------------------------------------------------------------------------
     * Debug info
     *-----------------------------------------------------------------------------*/
    if (ptr_sms_tel)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ptr_sms_tel        : %s", ptr_sms_tel        );
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ptr_sms_tel        : (NULL)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (ptr_sms_time_stamp)
    {
        snprintf(Phone_DebugString,
                 sizeof(Phone_DebugString),
                 "ptr_sms_time_stamp: \"%02u/%02u/%02u\",\"%02u:%02u:%02u\"",
                 ptr_sms_time_stamp->uDay,
                 ptr_sms_time_stamp->uMonth,
                 ptr_sms_time_stamp->uYear,
                 ptr_sms_time_stamp->uHour,
                 ptr_sms_time_stamp->uMinute,
                 ptr_sms_time_stamp->uSecond);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ptr_sms_time_stamp: (NULL)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (ptr_sms_text)
    {
        snprintf(Phone_DebugString, sizeof(Phone_DebugString), "ptr_sms_text       : %s", ptr_sms_text       );
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ptr_sms_text       : (NULL)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    if ((!ptr_sms_tel) || (!ptr_sms_time_stamp) || (!ptr_sms_text))
        return;


    /*-----------------------------------------------------------------------------
     * Phone activity counters
     *-----------------------------------------------------------------------------*/
    /* increase the total SMS received counter, yearly SMS received counter and monthly SMS received counter */
    Phone_PhoneActivity.phone_activity_monthly.n_sms_rx++;
    Phone_PhoneActivity.phone_activity_yearly.n_sms_rx++;
    Phone_PhoneActivity.phone_activity_total.n_sms_rx++;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);


    /*-----------------------------------------------------------------------------
     * Originating telephone number
     *-----------------------------------------------------------------------------*/
    /* verify if the SMS sender is a ADM phone number or/and a FWD number */
    result_1 = Myn_IsMynNumber                 (ptr_sms_tel                  );
    result_2 = Phonebook_IsPhonebookPhoneNumber(ptr_sms_tel, &phonebook_index);
    if (result_1 || result_2)
        is_adm = TRUE;
    else
        is_adm = FALSE;
    is_fwd = Fwd_IsFwdNumber(ptr_sms_tel);
    is_crs = FALSE;


    /*-----------------------------------------------------------------------------
     * SMS time stamp
     *-----------------------------------------------------------------------------*/
    sms_time_tx.year     = ptr_sms_time_stamp->uYear + 2000;
    sms_time_tx.month    = ptr_sms_time_stamp->uMonth;
    sms_time_tx.day      = ptr_sms_time_stamp->uDay;
    sms_time_tx.hour     = ptr_sms_time_stamp->uHour;
    sms_time_tx.minute   = ptr_sms_time_stamp->uMinute;
    sms_time_tx.second   = ptr_sms_time_stamp->uSecond;
    sms_time_tx.week_day = Clock_WeekDayOfADay(sms_time_tx.day, sms_time_tx.month, sms_time_tx.year);
    result = Clock_IsValidClockTime(&sms_time_tx);
    if (!result)
    {
        sms_time_tx.year     = 2000;
        sms_time_tx.month    = 1;
        sms_time_tx.day      = 1;
        sms_time_tx.hour     = 0;
        sms_time_tx.minute   = 0;
        sms_time_tx.second   = 0;
        sms_time_tx.week_day = 6;  // Saturday
    }
    //Clock_PrintTime2(1, &sms_time_tx);


    /*-----------------------------------------------------------------------------
     * Check the delay of the received SMS (only if "SMS delay" is enabled)
     *-----------------------------------------------------------------------------*/
    command_discarded = FALSE;

    if (Phone_Config_SmsDelay.status)
    {
        /* "SMS delay" enabled */

        /* get current time */
        result = Clock_GetTime(&current_time);
        if (result)
        {
            /* current time available */

            /* calculate the maximum SMS allowed delay (sec) */
            if      (Phone_Config_SmsDelay.sms_delay_unit == PHONE__SMS_DELAY_UNIT__SECOND)
                sms_delay_max_sec = ((u32)Phone_Config_SmsDelay.sms_delay_value          );
            else if (Phone_Config_SmsDelay.sms_delay_unit == PHONE__SMS_DELAY_UNIT__MINUTE)
                sms_delay_max_sec = ((u32)Phone_Config_SmsDelay.sms_delay_value *      60);
            else if (Phone_Config_SmsDelay.sms_delay_unit == PHONE__SMS_DELAY_UNIT__HOUR  )
                sms_delay_max_sec = ((u32)Phone_Config_SmsDelay.sms_delay_value * 60 * 60);
            else
                sms_delay_max_sec = ((u32)Phone_Config_SmsDelay.sms_delay_value          );

            /* calculate the delay of the received SMS (sec) */
            sms_delay_sec = Clock_DifferenceRtcTimes(&current_time, &sms_time_tx);

            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "sms_delay_sec: %ld", sms_delay_sec);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (sms_delay_sec > 0)
            {
                /* current time is after SMS tx time */

                if (((u32)sms_delay_sec) > sms_delay_max_sec)
                {
                    /* SMS delay too much big */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "SMS delay too much big --> SMS discarded (if it is a command)", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    command_discarded = TRUE;   // the received SMS is discarded (if it is a command)
                }
            }
            else
            {
                /* current time is before SMS tx time (maybe synchronization not yet done) */

                // NOTHING TO DO
            }
        }
    }


    /*-----------------------------------------------------------------------------
     * Received SMS parsing
     *-----------------------------------------------------------------------------*/
    /* parse the received SMS */
    for (i = 0; i < PARSER_RX__NUM_MAX_ANSWERS; i++)
    {
        answer_texts    [i][0] = 0x00;
        ptr_answer_texts[i]    = &answer_texts[i][0];
    }

    parser_result = ParserRx_ParseCommandText(command_discarded, &sms_time_tx, ptr_sms_tel, is_adm, is_crs, ptr_sms_text, &report, &command_valid, &num_answers, ptr_answer_texts, 160);
    if (parser_result)
    {
        /* parser OK */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PARSER OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /*------------------------------------------------------------------
         * Verify if the answer has to be transmitted
         *------------------------------------------------------------------*/
        if (!command_discarded)
        {
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
                        record_sms_answer.time = time;                                         /* time                       */
                        strncpy(record_sms_answer.sms_phone_number, ptr_sms_tel    ,  20);  /* SMS recipient phone number */
                        strncpy(record_sms_answer.answer_text     , answer_texts[i], 160);  /* answer text                */
                        record_sms_answer.sms_phone_number[ 20] = 0x00;
                        record_sms_answer.answer_text     [160] = 0x00;

                        /* put the SMS answer record in the SMS answer queue */
                        result = Queue_SmsAnswer_PutRecord(&record_sms_answer);
                    }
                }
            }
        }


        /*------------------------------------------------------------------
         * Build and put the rx command record for the rx command log
         *------------------------------------------------------------------*/
        if (command_valid)
        {
            /* get actual time */
            result = Clock_GetTime(&current_time);
            if (result)
            {
                /* current time     available */
                time.year     = current_time.year;
                time.month    = current_time.month;
                time.day      = current_time.day;
                time.hour     = current_time.hour;
                time.minute   = current_time.minute;
                time.second   = current_time.second;
                time.week_day = Clock_WeekDayOfADay(time.day, time.month, time.year);
            }
            else
            {
                /* current time not available */
                time.year     = 2000;
                time.month    = 1;
                time.day      = 1;
                time.hour     = 0;
                time.minute   = 0;
                time.second   = 0;
                time.week_day = 6;  // Saturday
            }

            /* verify if the calling number is a phonebook number */
            result = Phonebook_IsPhonebookPhoneNumber(ptr_sms_tel, &phonebook_index);
            if (result)
            {
                /* get phonebook name */
                result = Phonebook_PhonebookName(name, phonebook_index);
                if (!result)
                    name[0] = 0x00;
            }
            else
            {
                name[0] = 0x00;
            }

            /* build the rx command record */
            rx_command_record.command_type = LOGS__RX_COMMAND__COMMAND_TYPE__SMS;
            rx_command_record.time         = time;
            rx_command_record.command_exe  = (!command_discarded);
            strncpy(rx_command_record.name        , name        ,  14);
            strncpy(rx_command_record.phone_number, ptr_sms_tel ,  20);
            strncpy(rx_command_record.text        , ptr_sms_text, 160);
            rx_command_record.name        [ 14] = 0x00;
            rx_command_record.phone_number[ 20] = 0x00;
            rx_command_record.text        [160] = 0x00;

            /* put the rx command record in the rx command log */
            result = Logs_RxCommmand_PutRecord(&rx_command_record);

            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Created a rx commmand log record", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
    else
    {
        /* parser error */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PARSER ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /*------------------------------------------------------------------
         * Verify if the SMS has to be forwarded
         *------------------------------------------------------------------*/
        if ((!is_fwd) && (!is_adm))
        {
            /* get the forward configuration */
            Forward_Config_Forward_Get(&config_forward);
            if (config_forward.status)
            {
                /* forward enabled */
                forward = TRUE;
            }
            else
            {
                /* forward disabled */
                forward = FALSE;
            }
        }
        else
        {
            forward = FALSE;
        }

        if (forward)
        {
            /* get the actual time */
            result = Clock_GetTime(&time);

            if (strlen(ptr_sms_text) > 0)
            {
                /* build the SMS forward record */
                record_sms_forward.time = time;                                  /* time         */
                strncpy(record_sms_forward.forward_text, ptr_sms_text, 160);  /* forward text */
                record_sms_forward.forward_text[160] = 0x00;

                /* put the SMS forward record in the SMS forward queue */
                result = Queue_SmsForward_PutRecord(&record_sms_forward);

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Added a record in the SMS forward queue", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
        }
    }
}




/*===========================================================================
 * Function   : Phone_AdlCallback_Service_Sms
 *
 * Description:
 * Input      : - ptr_sms_tel       : pointer to originating telephone number of the SMS          (in text mode)
 *              - ptr_sms_time_stamp: pointer to the SMS time stamp (e.g. "12/03/23,11:13:50+04") (in text mode)
 *              - ptr_sms_text      : pointer to the SMS text                                     (in text mode)
 * Output     : -
 *===========================================================================*/
static void Phone_AdlCallback_Service_Sms(uint8_t sim_id, int event_id, void *ctx)
{
    ql_sms_new_s *msg;

    ql_event_t    event;
    QlOSStatus    err;


    switch (event_id)
    {
        case QL_SMS_INIT_OK_IND:
            break;

        case QL_SMS_NEW_MSG_IND:
            msg = (ql_sms_new_s *)ctx;

            /* SMS received */
            event.id     = PHONE__TASK_MSG_ID__SMS_RX_IND;
            event.param1 = msg->index;

            err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
            if (err != QL_OSI_SUCCESS)
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            else
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;

        case QL_SMS_LIST_IND:
        case QL_SMS_LIST_EX_IND:
        case QL_SMS_LIST_END_IND:
        case QL_SMS_MEM_FULL_IND:
        default :
            break;
    }
}




#ifdef FUNCTION_IMPLEMENTED
/*===========================================================================
 * Function   : Phone_AdlCallback_Service_Sms_TxResult
 *
 * Description: -
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Phone_AdlCallback_Service_Sms_TxResult(u8 event, u16 nb)
{
    bool sms_tx_ok;


    switch (event)
    {
        case ADL_SMS_EVENT_SENDING_OK:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SMS TX - ADL_SMS_EVENT_SENDING_OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (!Phone_MyGsm_MyCgms_Status)
            {
                /* "My CGMS" disabled */
                sms_tx_ok = TRUE;
            }
            else
            {
                /* "My CGMS" enabled */
                if (Phone_MyGsm_MyCgms_Value)
                    sms_tx_ok = TRUE;
                else
                    sms_tx_ok = FALSE;
            }

            if (sms_tx_ok)
            {
                /* increase the total SMS transmitted counter, yearly SMS transmitted counter and monthly SMS transmitted counter with success */
                Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_ok++;
                Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_ok++;
                Phone_PhoneActivity.phone_activity_total.n_sms_tx_ok++;

                /* request the backup to flash objects */
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);

                /* update the available SMS credit */
                Credit_ConsumeOneCredit();

                TransmissionSms_Event_SmsTxOk();
            }
            else
            {
                TransmissionSms_Event_SmsTxFail();
            }
            break;


        case ADL_SMS_EVENT_SENDING_ERROR:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SMS TX - ADL_SMS_EVENT_SENDING_ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (!Phone_MyGsm_MyCgms_Status)
            {
                /* "My CGMS" disabled */
                sms_tx_ok = FALSE;
            }
            else
            {
                /* "My CGMS" enabled */
                if (Phone_MyGsm_MyCgms_Value)
                    sms_tx_ok = TRUE;
                else
                    sms_tx_ok = FALSE;
            }

            if (!sms_tx_ok)
            {
                /* increase the total SMS transmitted counter, yearly SMS transmitted counter and monthly SMS transmitted counter with insuccess */
                Phone_PhoneActivity.phone_activity_monthly.n_sms_tx_fail++;
                Phone_PhoneActivity.phone_activity_yearly.n_sms_tx_fail++;
                Phone_PhoneActivity.phone_activity_total.n_sms_tx_fail++;

                /* request the backup to flash objects */
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__002_PHONE_INFO, PROGRAM_FLASH__FLASH_ID__002_PHONE_INFO__PHONE_ACTIVITY);

                TransmissionSms_Event_SmsTxFail();
            }
            else
            {
                TransmissionSms_Event_SmsTxOk();
            }
            break;


        case ADL_SMS_EVENT_SENDING_MR:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SMS TX - ADL_SMS_EVENT_SENDING_MR"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;


        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE SMS TX - UNKNOWN EVENT ERROR"        , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}
#endif




/*===========================================================================
 * Function   : Phone_WipCallback_Service_BearerEvent
 *
 * Description: -
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Phone_WipCallback_Service_BearerEvent(uint8_t sim_id, unsigned int ind_type, int profile_idx, bool result, void *ctx)
{
    if (profile_idx != Phone_BearerHandler_Gprs)
        return;

    switch (ind_type)
    {
        // IP communication ready
        case QUEC_DATACALL_ACT_RSP_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - QUEC_DATACALL_ACT_RSP_IND"     , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (result)
            {
                TransmissionGprs_Event_ApnConnectionOk();
            }
            else
            {
                TransmissionGprs_Event_ApnConnectionFail();
            }
            break;

        // Disconnection completed after ql_stop_data_call was called
        case QUEC_DATACALL_DEACT_RSP_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - QUEC_DATACALL_DEACT_RSP_IND"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if (result)
            {
                TransmissionGprs_Event_ApnConnectionDown();
            }
            break;

        // IP communication terminated
        case QUEC_DATACALL_PDP_DEACTIVE_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - QUEC_DATACALL_PDP_DEACTIVE_IND", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            TransmissionGprs_Event_ApnConnectionDown();
            break;

        // unknown event
        default:
            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "CALLBACK     - SERVICE BEARER - UNKNOWN EVENT ERROR: %d", ind_type);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_HIGH, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}




/*=============================================================================
 * Function   : Phone_AdlCallback_AtResponse_QPINC
 *
 * Description: callback function for response to "+QPINC" AT command
 * Input      : - params: information about AT response
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
static void Phone_AdlCallback_AtResponse_QPINC(unsigned int ind_type, unsigned int size)
{
    ql_event_bits_t curr_event_flags;
    ql_event_bits_t in_event_flags;

    QlOSStatus      res_osi;

    u8             *ptr_data;
    int             res_int;

    ascii           qpinc_string[SIZE_BUFFER_ANSWER + 1];

    ascii           pin_string[2 + 1] = {0x00, 0x00, 0x00};
    ascii           puk_string[2 + 1] = {0x00, 0x00, 0x00};

    u8              pin;
    u8              puk;


    if (ind_type == QUEC_VIRT_AT_RX_RECV_DATA_IND)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - AT RESPONSE - AT+QPINC", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        ptr_data = (u8 *)malloc(size + 1);
        if (ptr_data == NULL)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            return;
        }

        memset(ptr_data, 0, size + 1);

        res_int = ql_virt_at_read(QL_VIRT_AT_PORT_1, ptr_data, size);
        if (res_int != (int)size)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_virt_at_read - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* free allocated memory */
            free(ptr_data);
            return;
        }

        if (  (                                              (sizeof(AT_COM_QPINC) - 1)  == size)  &&
              (strncmp((const char *)ptr_data, AT_COM_QPINC, (sizeof(AT_COM_QPINC) - 1)) == 0   )  )
        {
            /* echo */

            /* free allocated memory */
            free(ptr_data);
            return;
        }

        memset(qpinc_string, 0, SIZE_BUFFER_ANSWER + 1);
        Phone_StringRemoveCrLf(qpinc_string, ptr_data, size);

        /* free allocated memory */
        free(ptr_data);

        if (strncmp(qpinc_string, "+QPINC:", strlen("+QPINC:")) == 0)
        {
            /* +QPINC: "SC",<PIN_counter>,<PUK_counter> */

            Phone_GetArgString(pin_string, qpinc_string, 2);
            Phone_GetArgString(puk_string, qpinc_string, 3);

            pin = (u8)atoi(pin_string);
            puk = (u8)atoi(puk_string);

            Phone_SimStatus.pin1_attempts_left = pin;
            Phone_SimStatus.puk1_attempts_left = puk;

            Phone_SimPinAttemptsLeft = pin;

            /* debug info */
            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "AT+QPINC ---> +QPINC: \"SC\",%d,%d", pin, puk);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }


        if (strncmp(qpinc_string, "OK", (sizeof("OK") - 1)) == 0)
        {
            /* OK */

            /* debug info */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "AT+QPINC ---> OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            in_event_flags = EVENT_FLAG__EVENT_ANSWER_QPINC_OK;

            res_osi = ql_rtos_event_group_set(Phone_EventHandler_Events, in_event_flags, &curr_event_flags);
            if (res_osi != QL_OSI_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_event_group_set - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
        }


        if (
             (strncmp(qpinc_string, "ERROR"      , (sizeof("ERROR"      ) - 1)) == 0) ||
             (strncmp(qpinc_string, "+CME ERROR:", (sizeof("+CME ERROR:") - 1)) == 0)
           )
        {
            /* ERROR, +CME ERROR: */

            /* debug info */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "AT+QPINC ---> ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            in_event_flags = EVENT_FLAG__EVENT_ANSWER_QPINC_ERROR;

            res_osi = ql_rtos_event_group_set(Phone_EventHandler_Events, in_event_flags, &curr_event_flags);
            if (res_osi != QL_OSI_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_event_group_set - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
        }
    }
}




/*=============================================================================
 * Function   : Phone_GetArgString
 *
 * Description: If src is a string formatted as an AT response (for example "+RESP:P1,P2,P3") or
 *              as an AT command (for example "AT+CMD=P1,P2,P3"), the function copies the
 *              parameter at position offset (starting from 1) if it is present in the src buffer, and
 *              returns a pointer on dst. It returns NULL otherwise.
 *              if the parameter is a quoted string (for example "AT+CMD=P1,"P2",P3"),
 *              the 2 quotes are removed.
 * Input      : - dst     : destination string
 *              - src     : source      string
 *              - position: position of the parameter to be found (starting from 1)
 * Output     : - pointer to dst string if the parameter is     found
 *              - NULL                  if the parameter is not found
 *=============================================================================*/
static ascii *Phone_GetArgString(ascii *dst, const ascii *src, u16 position)
{
    u16    len_src;
    u16    len_src_temp;

    ascii *token;
    ascii *remainder;
    u16    len_token;
    u8     num_arg;
    u8     i;


    /* calculate the length of src string */
    len_src = (u16)strlen(src);

    num_arg = 0;
    len_src_temp = 0;

    token = strtok_r((ascii *)src, "=:", &remainder);
    if (token != NULL)
    {
        len_token     = strlen(token);
        len_src_temp += len_token;
        if (len_src_temp < len_src)
        {
            token[len_token] = '=';   // put the delimiter removed by strtok (put '=' in both cases)
            len_src_temp++;
        }
    }

    while (token != NULL)
    {
        token = strtok_r(NULL, ",", &remainder);
        if (token != NULL)
        {
            len_token     = strlen(token);
            len_src_temp += len_token;
            if (len_src_temp < len_src)
            {
                token[len_token] = ',';   // put the delimiter removed by strtok
                len_src_temp++;
            }

            num_arg++;
            if (num_arg == position)
            {
                if (len_token <= 2)
                {
                    if (
                         (token[0            ] == '"') &&
                         (token[len_token - 1] == '"')
                       )
                    {
                        /* quoted string */
                        for (i = 0; i < (len_token - 2); i++)
                            dst[i] = token[i + 1];
                        dst[len_token - 2] = 0x00;
                    }
                    else
                    {
                        /* no quoted string */
                        for (i = 0; i < len_token; i++)
                            dst[i] = token[i];
                        dst[len_token] = 0x00;
                    }

                    return dst;
                }
                else
                {
                    return NULL;
                }
            }
        }
    }


    return NULL;
}




/*=============================================================================
 * Function   : Phone_StringRemoveCrLf
 *
 * Description:
 * Input      : - params:
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
static bool Phone_StringRemoveCrLf(ascii *ptr_string, const u8 *ptr_bytes, u16 num_bytes)
{
    u8  byte_rx;
    u8  at_answer_status_rx;

    u8  at_answer_buffer[SIZE_BUFFER_ANSWER];
    u8  at_answer_buffer_index;

    u8  i;
    u8  j;


    at_answer_buffer_index = 0;
    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;

    /* check if a possible AT answer or a prompt from the GSM module has been received */
    for (i = 0; i < num_bytes; i++)
    {
        byte_rx = *ptr_bytes++;

        /* check if a possible AT answer from the GSM module has been received */
        switch (at_answer_status_rx)
        {
            // AT answer rx status - waiting for start <CR>
            case AT_ANSWER_STATUS_RX__WAIT_START_CR:
                /* check if a <CR> has been received */
                if (byte_rx == '\r')
                {
                    /* <CR> received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_LF;
                }
                else
                {
                    /* other byte received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                }
                break;


            // AT answer rx status - waiting for start <LF>
            case AT_ANSWER_STATUS_RX__WAIT_START_LF:
                /* check if a <LF> has been received */
                if      (byte_rx == '\n')
                {
                    /* <LF> received */
                    at_answer_buffer_index = 0;
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_DATA;
                }
                else if (byte_rx == '\r')
                {
                    /* <CR> received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_LF;
                }
                else
                {
                    /* other byte received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                }
                break;


            // AT answer rx status - waiting for a data byte
            case AT_ANSWER_STATUS_RX__WAIT_DATA:
                /* check if a <CR> has been received */
                if (byte_rx == '\r')
                {
                    /* <CR> received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_STOP_LF;
                }
                else
                {
                    /* other byte received */

                    /* save the received data byte */
                    if (at_answer_buffer_index < SIZE_BUFFER_ANSWER)
                        at_answer_buffer[at_answer_buffer_index++] = byte_rx;
                }
                break;


            // AT answer rx status - waiting for stop  <LF>
            case AT_ANSWER_STATUS_RX__WAIT_STOP_LF:
                /* check if a <LF> has been received */
                if      (byte_rx == '\n')
                {
                    /* <LF> received */

                    /* copy all the received bytes (except <LF> and <CR> bytes) */
                    for (j = 0; j < at_answer_buffer_index; j++)
                        *(ptr_string + j) = at_answer_buffer[j];

                    /* terminate the string with NULL */
                    *(ptr_string + at_answer_buffer_index) = '\0';

                    return TRUE;
                }
                else if (byte_rx == '\r')
                {
                    /* <CR> received */

                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_LF;
                }
                else
                {
                    /* other byte received */

                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                }
                break;


            // AT answer rx status - unknown
            default:
                at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                break;
        }
    }


    return FALSE;
}




/*=============================================================================
 * Function   : Phone_AdlCallback_UnsolicitedResponse_CGREG
 *
 * Description: +CREG: <stat>[,<lac>,<cid>[,<AcT>]]     (<stat>!=3: Nominal  case)
 *              +CREG: <stat>[,<rejectCause>]           (<stat>=3 : Specific case)
 * Input      : - params: information about AT response
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
static void Phone_AdlCallback_UnsolicitedResponse_CGREG(uint8_t sim_id, unsigned int ind_type, void *ind_msg_buf)
{
    ql_nw_common_reg_status_info_s *data_reg_status;

    ql_nw_nitz_time_info_s         *nitz_info;
    time_t                          unix_time;
    bool                            result;

    ql_event_t                      event;
    QlOSStatus                      err;


    switch (ind_type)
    {
        case QUEC_NW_VOICE_REG_STATUS_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - UNSOLICITED RESPONSE - +CREG", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;


        case QUEC_NW_DATA_REG_STATUS_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - UNSOLICITED RESPONSE - +CGREG", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            data_reg_status = (ql_nw_common_reg_status_info_s *)ind_msg_buf;

            /* debug info */
            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "+CGREG: %d", data_reg_status->state);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            Phone_PhoneStatus.cgreg = data_reg_status->state;

            /* update phone status */
            switch (data_reg_status->state)
            {
                // not registered, ME is not currently searching for a new operator
                // not registered, ME currently searching for a new operator
                // unknown
                case QL_NW_REG_STATE_NOT_REGISTERED:
                case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
                case QL_NW_REG_STATE_UNKNOWN:
                    // not registered in network
                    Phone_PhoneStatus.lac[0] = 0x00;
                    Phone_PhoneStatus.ci [0] = 0x00;
                    Phone_GprsNetworkRegistered = FALSE;

                    UiLed_SignalGsmRegDeniedStop();
                    UiLed_SignalGsmNotRegistered();
                    break;

                // registration denied
                case QL_NW_REG_STATE_DENIED:
                    // not registered in network
                    Phone_PhoneStatus.lac[0] = 0x00;
                    Phone_PhoneStatus.ci [0] = 0x00;
                    Phone_GprsNetworkRegistered = FALSE;

                    UiLed_SignalGsmRegDeniedStart();
                    UiLed_SignalGsmNotRegistered();
                    break;

                // registered, home network
                // registered, roaming
                case QL_NW_REG_STATE_HOME_NETWORK:
                case QL_NW_REG_STATE_ROAMING:
                    // registered in network
                    snprintf(Phone_PhoneStatus.lac, sizeof(Phone_PhoneStatus.lac), "%04X", (uint16_t)(data_reg_status->lac));
                    Phone_PhoneStatus.lac[sizeof(Phone_PhoneStatus.lac) - 1] = 0x00;

                    snprintf(Phone_PhoneStatus.ci , sizeof(Phone_PhoneStatus.ci ), "%08X",           (data_reg_status->cid));
                    Phone_PhoneStatus.ci [sizeof(Phone_PhoneStatus.ci ) - 1] = 0x00;

                    Phone_GprsNetworkRegistered = TRUE;

                    UiLed_SignalGsmRegDeniedStop();
                    UiLed_SignalGsmRegistered(Phone_PhoneStatus.rssi, Phone_PhoneStatus.ber);
                    break;
            }

            /* read the GSM network status */
            if (Phone_SimPinInit == TRUE)
            {
                Phone_TimerPhonePolling_Stop();
                Phone_ReadPhoneNetworkStatus();
                Phone_TimerPhonePolling_Start(TIME_MS__GSM_STATUS_POLLING, TRUE);
            }
            break;


        case QUEC_NW_NITZ_TIME_UPDATE_IND:
            nitz_info = (ql_nw_nitz_time_info_s *)ind_msg_buf;

            result = Clock_ConverTimeString3ToClockTime(nitz_info->nitz_time, &unix_time);
            if (result)
            {
                event.id     = PHONE__TASK_MSG_ID__NITZ_TIME_UPDATE;
                event.param1 = (uint32)(unix_time >> 32);
                event.param2 = (uint32)(unix_time      );

                /* signal NITZ time update */
                err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
                if (err != QL_OSI_SUCCESS)
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
            }
            break;


        default:
            break;
    }
}




/*=============================================================================
 * Function   : Phone_AdlCallback_UnsolicitedResponse_CLIP
 *
 * Description: +CLIP: <number>,<type>[,<subaddr>,<satype>,<alpha>]
 *              +CLIP: "+393451234567",129,1,,"FRED"
 * Input      : - ptr_params:
 * Output     : -
 *=============================================================================*/
static void Phone_AdlCallback_UnsolicitedResponse_CLIP(uint8_t sim, ql_vc_event_id_e event_id, void *ctx)
{
           ql_event_t              event;
           QlOSStatus              err;

           ascii                  *token;
           ascii                   number[PHONEBOOK__MAX_LEN_PHONE_NUMBER + 1];
           u8                      idx;

           bool                    result;

#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
           u16                     phonebook_index;
           bool                    out1;

    static LOGS__RX_COMMAND_RECORD rx_command_record;
           REPORT__CONFIG__REPORT  config_report;

    static ascii                   name[14 + 1];

           CLOCK__TIME             current_time;
           CLOCK__TIME             time;
#endif


    switch (event_id)
    {
        // voice phone call ringing
        case QL_VC_RING_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE CALL - QL_VC_RING_IND", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            idx = 0;
            token = strchr((const char *)ctx, '+');
            if (token == NULL)
            {
                number[idx++] = '+';
            }
            number[idx] = 0x00;

            strncat(number, (const char *)ctx, PHONEBOOK__MAX_LEN_PHONE_NUMBER);
            number[PHONEBOOK__MAX_LEN_PHONE_NUMBER] = 0x00;

            /* debug info */
            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "+CLIP: %s", number);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            result = Utility_IsPhoneString(number);
            if (!result)
                return;

#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
            /* verify if the calling number is a phonebook number */
            result = Phonebook_IsPhonebookPhoneNumber(number, &phonebook_index);
            if (result)
            {
                /* the calling phone number is a phonebook number */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "The calling phone number is stored in phonebook", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


                /*----------------------------------------------------------------------------
                 * Toggle digital output OUT1
                 *----------------------------------------------------------------------------*/
                /* get digital output OUT1 logical status */
                out1 = Regulation_StatusLogicalOutputInt();

                /* invert the digital output OUT1 logical status */
                if (out1)
                {
                    /* OUT1 is activated */

                    /* disable output 1 */
                    result = Outputs_Event_ManualInt_Off();
                }
                else
                {
                    /* OUT1 is disactivated */

                    /* enable  output 1 */
                    result = Outputs_Event_ManualInt_On();
                }


                /*----------------------------------------------------------------------------
                 * Build and put the rx command record for the rx command log
                 *----------------------------------------------------------------------------*/
                /* get actual time */
                result = Clock_GetTime(&current_time);
                if (result)
                {
                    /* current time     available */
                    time.year     = current_time.year;
                    time.month    = current_time.month;
                    time.day      = current_time.day;
                    time.hour     = current_time.hour;
                    time.minute   = current_time.minute;
                    time.second   = current_time.second;
                    time.week_day = Clock_WeekDayOfADay(time.day, time.month, time.year);
                }
                else
                {
                    /* current time not available */
                    time.year     = 2000;
                    time.month    = 1;
                    time.day      = 1;
                    time.hour     = 0;
                    time.minute   = 0;
                    time.second   = 0;
                    time.week_day = 6;  // Saturday
                }

                /* verify if the calling number is a phonebook number */
                result = Phonebook_IsPhonebookPhoneNumber(number, &phonebook_index);
                if (result)
                {
                    /* get phonebook name */
                    result = Phonebook_PhonebookName(name, phonebook_index);
                    if (!result)
                        name[0] = 0x00;
                }
                else
                {
                    name[0] = 0x00;
                }

                /* build the rx command record */
                rx_command_record.command_type = LOGS__RX_COMMAND__COMMAND_TYPE__RING;
                rx_command_record.time         = time;
                rx_command_record.command_exe  = TRUE;
                strncpy(rx_command_record.name        , name  ,  14);
                strncpy(rx_command_record.phone_number, number,  20);
                strncpy(rx_command_record.text        , ""    , 160);
                rx_command_record.name        [ 14] = 0x00;
                rx_command_record.phone_number[ 20] = 0x00;
                rx_command_record.text        [160] = 0x00;

                /* put the rx command record in the rx command log */
                result = Logs_RxCommmand_PutRecord(&rx_command_record);

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Created a rx commmand log record", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


                /*----------------------------------------------------------------------------
                 * Build and parse a simulation of "STATUS" SMS
                 *----------------------------------------------------------------------------*/
                /* get the report configuration */
                Report_Config_Report_Get(&config_report);
                if (config_report.status)
                {
                    /* report enabled */

                    /* build and parse a simulation of "STATUS" SMS */
                    Phone_ParseCommandStatus(number);
                }
            }
            else
            {
                /* the calling phone number is not a phonebook number */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "The calling phone number is not stored in phonebook", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
#endif

            /* signal incoming call */
            event.id = PHONE__TASK_MSG_ID__RING_VOICE;

            err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
            if (err != QL_OSI_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        // Unknown event
        default:
            snprintf(Phone_DebugString, sizeof(Phone_DebugString), "CALLBACK     - SERVICE CALL - event_id: %d", event_id);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}




/*===========================================================================
 * Function   : Phone_AdlCallback_Message_TaskMsg
 *
 * Description: - task phone message callback
 * Input      : - msg_identifier:
 * Output     : -
 *===========================================================================*/
static void Phone_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    u8               sms_index;
    ql_sms_recv_s    sms_recv;

    ql_sms_errcode_e res_sms;
    ql_vc_errcode_e  res_vc;

    time_t           unix_time;
    struct tm        std_time;
    CLOCK__TIME      actual_time;


    snprintf(Phone_DebugString, sizeof(Phone_DebugString), "CALLBACK     - MESSAGE     - TASK PHONE - msg_identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, Phone_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        case PHONE__TASK_MSG_ID__SMS_RX_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PHONE__TASK_MSG_ID__SMS_RX_IND", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            sms_index = msg_identifier->param1;

            res_sms = ql_sms_read_msg_ex(0, sms_index, TEXT, &sms_recv);
            if (res_sms == QL_SMS_SUCCESS)
            {
                /* SMS received */
                Phone_SmsReceived(sms_recv.oa, &sms_recv.scts, (ascii *)&sms_recv.data);

                /* delete SMS */
                ql_sms_delete_msg(0, sms_index);
            }
            break;


        case PHONE__TASK_MSG_ID__RING_VOICE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PHONE__TASK_MSG_ID__RING_VOICE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            res_vc = ql_voice_call_end(0);
            if (res_vc != QL_VC_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_voice_call_end ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            else
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_voice_call_end OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        case PHONE__TASK_MSG_ID__SIM_PIN_WAIT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PHONE__TASK_MSG_ID__SIM_PIN_WAIT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* try to enter the SIM PIN */
            Phone_EnterSimPin();
            break;


        case PHONE__TASK_MSG_ID__SIM_PUK_WAIT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PHONE__TASK_MSG_ID__SIM_PUK_WAIT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* SIM problem */
            Phone_SimPinProblem();
            break;


        case PHONE__TASK_MSG_ID__SIM_FULL_INIT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "PHONE__TASK_MSG_ID__SIM_FULL_INIT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* manage SIM PIN */
            Phone_ManageSimPin();

            /* start the "phone polling" timer */
            Phone_TimerPhonePolling_Start(TIME_MS__GSM_STATUS_POLLING, TRUE);
            break;


        case PHONE__TASK_MSG_ID__TIMEOUT_POLLING:
            Phone_ReadPhoneNetworkStatus();
            break;


        case PHONE__TASK_MSG_ID__NITZ_TIME_UPDATE:
            /* extract the parameters */
            unix_time = ((time_t)msg_identifier->param1 << 32) |
                        ((time_t)msg_identifier->param2      );

            /* convert time stamp to time */
            (void)gmtime_r(&unix_time, &std_time);

            actual_time.year     =  std_time.tm_year + 1900;
            actual_time.month    =  std_time.tm_mon  + 1;
            actual_time.day      =  std_time.tm_mday;
            actual_time.week_day = (std_time.tm_wday == 0) ? 7 : std_time.tm_wday;
            actual_time.hour     =  std_time.tm_hour;
            actual_time.minute   =  std_time.tm_min;
            actual_time.second   =  std_time.tm_sec;

            Synchronize_SynchronizeRtcTime(&actual_time, TRUE);
            break;


        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "Unknown event", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}




/*=============================================================================
 * Function   : Phone_AdlCallback_Timer_TimerPhonePolling
 *
 * Description: Monitor the network registration
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void Phone_AdlCallback_Timer_TimerPhonePolling(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_HIGH, "CALLBACK     - TIMER       - PHONE POLLING", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = PHONE__TASK_MSG_ID__TIMEOUT_POLLING;

    err = ql_rtos_event_send(Boot_TaskRef_Phone, &event);
    if (err != QL_OSI_SUCCESS)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    else
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PHONE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}
