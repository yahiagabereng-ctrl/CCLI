/*=============================================================================
 * File       :  TRANSMISSION_SMS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - SMS transmission management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "fwd.h"
#include "myn.h"
#include "parser_tx.h"
#include "phone.h"
#include "phonebook.h"
#include "program_flash.h"
#include "queue_my.h"
#include "synchronize.h"
#include "transmission_sms.h"
#include "typedef.h"
#include "ui_led.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* data tx type */
#define DATA_TX__FW_UPGRADE                          1               /* data tx type - FW upgrade       */
#define DATA_TX__ALARM                               2               /* data tx type - alarm            */
#define DATA_TX__SMS_ANSWER                          3               /* data tx type - SMS answer       */
#define DATA_TX__SMS_FORWARD                         4               /* data tx type - SMS forward      */
#define DATA_TX__AUTOSYNCHRONIZE                     5               /* data tx type - autosynchronize  */
#define DATA_TX__SMS_ANSWER_EVENT                    6               /* data tx type - SMS answer event */

/* number of SMS transmission attempts */
#define NUM_OF_SMS_TX_ATTEMPTS_PHONEBOOK             3               /* number of SMS transmission attempts towards phonebook recipient */
#define NUM_OF_SMS_TX_ATTEMPTS_MYN                   3               /* number of SMS transmission attempts towards MYN       recipient */
#define NUM_OF_SMS_TX_ATTEMPTS_FWD                   3               /* number of SMS transmission attempts towards FWD       recipient */
#define NUM_OF_SMS_TX_ATTEMPTS_OTHER                 3               /* number of SMS transmission attempts towards other     recipient */

/* phone number max length */
#define MAX_LEN_PHONE_NUMBER                        (                                                                \
                                                      (PHONEBOOK__MAX_LEN_PHONE_NUMBER > MYN__MAX_LEN_PHONE_NUMBER)  \
                                                       ?                                                             \
                                                       PHONEBOOK__MAX_LEN_PHONE_NUMBER                               \
                                                       :                                                             \
                                                       MYN__MAX_LEN_PHONE_NUMBER                                     \
                                                    )

/* timeout values (ms) */
#define TIME_MS__DELAY_AFTER_GSM_REG                 (2 * 1000L)     /* time delay after GSM network registration (ms) */
#define TIME_MS__DELAY_AFTER_SMS_TX_OK               (3 * 1000L)     /* time delay after SMS transmission OK      (ms) */
#define TIME_MS__DELAY_AFTER_SMS_TX_FAIL             (5 * 1000L)     /* time delay after SMS transmission fail    (ms) */
#define TIME_MS__CHECK_DATA_TX                       (4 * 1000L)     /* time check data to be transmitted         (ms) */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                      200

/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__IDLE                              0               /* SEM state - idle                 */
#define SEM_STATE__WAITING_FOR_RESOURCE              1               /* SEM state - waiting for resource */
#define SEM_STATE__SMS_TX                            2               /* SEM state - SMS transmission     */
#define SEM_STATE__TX_ATTEMPTS_DELAY                 3               /* SEM state - tx attempts delay    */

/* SEM state strings */
#define SEM_STATE_STRING__IDLE                       "SEM_STATE__IDLE"
#define SEM_STATE_STRING__WAITING_FOR_RESOURCE       "SEM_STATE__WAITING_FOR_RESOURCE"
#define SEM_STATE_STRING__SMS_TX                     "SEM_STATE__SMS_TX"
#define SEM_STATE_STRING__TX_ATTEMPTS_DELAY          "SEM_STATE__TX_ATTEMPTS_DELAY"

/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__DATA_TX                           0               /* SEM event - data to be transmitted                 */
#define SEM_EVENT__GSM_NET_REGISTERED                1               /* SEM event - GSM network registration               */
#define SEM_EVENT__GSM_NET_NOT_REGISTERED            2               /* SEM event - GSM network deregistration             */
#define SEM_EVENT__SMS_TX_OK                         3               /* SEM event - success of SMS transmission            */
#define SEM_EVENT__SMS_TX_FAIL                       4               /* SEM event - failure of SMS transmission            */
#define SEM_EVENT__TIMEOUT_DELAY_AFTER_SMS           5               /* SEM event - timeout after SMS transmission expired */
#define SEM_EVENT__TIMEOUT_CHECK_TX                  6               /* SEM event - timeout check data to be transmitted   */

/* SEM events strings */
#define SEM_EVENT_STRING__DATA_TX                    "SEM_EVENT__DATA_TX"
#define SEM_EVENT_STRING__GSM_NET_REGISTERED         "SEM_EVENT__GSM_NET_REGISTERED"
#define SEM_EVENT_STRING__GSM_NET_NOT_REGISTERED     "SEM_EVENT__GSM_NET_NOT_REGISTERED"
#define SEM_EVENT_STRING__SMS_TX_OK                  "SEM_EVENT__SMS_TX_OK"
#define SEM_EVENT_STRING__SMS_TX_FAIL                "SEM_EVENT__SMS_TX_FAIL"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_AFTER_SMS    "SEM_EVENT__TIMEOUT_DELAY_AFTER_SMS"
#define SEM_EVENT_STRING__TIMEOUT_CHECK_TX           "SEM_EVENT__TIMEOUT_CHECK_TX"

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__DATA_TX                         (12200 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__GSM_NET_REGISTERED              (12201 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__GSM_NET_NOT_REGISTERED          (12202 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__SMS_TX_OK                       (12203 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__SMS_TX_FAIL                     (12204 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_AFTER_SMS         (12205 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_CHECK_TX                (12206 | (QL_COMPONENT_APP_START << 16))




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* number of transmission attempts */
static       u8                       TransmissionSms_NumOfTxAttemptPhonebook = 0;
static       u8                       TransmissionSms_NumOfTxAttemptMyn       = 0;
static       u8                       TransmissionSms_NumOfTxAttemptFwd       = 0;
static       u8                       TransmissionSms_NumOfTxAttemptOther     = 0;

/* indication of transmission in progress */
static       bool                     TransmissionSms_TransmissionInProgress = FALSE;

/* last phonebook index */
static       u16                      TransmissionSms_PhonebookIndexForAlarm = 1;

/* synchronize SMS counter */
static       SYNCHRONIZE__SMS_COUNTER TransmissionSms_SynchronizeSmsCounter;

/* debug string */
static       ascii                    TransmissionSms_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* timer handlers */
static       ql_timer_t               TransmissionSms_TimerHandler_TimerDelaySmsTx;    // timer for ...
static       ql_timer_t               TransmissionSms_TimerHandler_TimerCheckTx;       // timer for ...




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* SMS transmission task */
       void TransmissionSms_TaskTransmissionSms(void *argument);

/* info */
       bool TransmissionSms_IsTransmissionTerminated(void);

/* SMS sending */
static bool TransmissionSms_SendSms(u8 data_tx_type);

/* data to be transmitted */
       bool TransmissionSms_ExistDataToBeTransmitted(void);
static bool TransmissionSms_DataToBeTransmitted     (u8 *ptr_data_tx_type);

/* recipient phone number */
static bool TransmissionSms_ExistRecipientPhoneNumber(u8 data_tx_type);

/* number of transmission attempts */
static u8   TransmissionSms_NumMaxOfTxAttempt       (u8 data_tx_type);
static u8   TransmissionSms_NumOfTxAttempt          (u8 data_tx_type);     // not used
static void TransmissionSms_ResetNumOfTxAttempt     (u8 data_tx_type);
static void TransmissionSms_IncreaseNumOfTxAttempt  (u8 data_tx_type);

/* delete/recover data */
static void TransmissionSms_DeleteData    (u8 data_tx_type);
static void TransmissionSms_DeleteDataGot (u8 data_tx_type);
static void TransmissionSms_RecoverDataGot(u8 data_tx_type);

/*---------------------------------------------------------------------------
 * Events for SEM from external
 *---------------------------------------------------------------------------*/
       void TransmissionSms_Event_DataTx                 (void);
       void TransmissionSms_Event_GsmNetworkRegistered   (void);
       void TransmissionSms_Event_GsmNetworkNotRegistered(void);
       void TransmissionSms_Event_SmsTxOk                (void);
       void TransmissionSms_Event_SmsTxFail              (void);

/*---------------------------------------------------------------------------
 * Timers
 *---------------------------------------------------------------------------*/
/* action on timers */

/* timer for  */
static void TransmissionSms_TimerDelaySmsTx_Start(u32 time_value, bool periodic);

/* timer for  */
static void TransmissionSms_TimerCheckTx_Start   (u32 time_value, bool periodic);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void TransmissionSms_AdlCallback_Message_TaskMsg(u32 msg_identifier);

/* timer   callback functions */
static void TransmissionSms_AdlCallback_Timer_TimerDelaySmsTx(void *ptr_context);
static void TransmissionSms_AdlCallback_Timer_TimerCheckTx   (void *ptr_context);




/*=============================================================================
 * Function   : TransmissionSms_TaskTransmissionSms
 *
 * Description: SMS transmission task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionSms_TaskTransmissionSms(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - TRANSMISSION_SMS - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* number of transmission attempt */
    TransmissionSms_NumOfTxAttemptPhonebook = 0;
    TransmissionSms_NumOfTxAttemptMyn       = 0;
    TransmissionSms_NumOfTxAttemptFwd       = 0;
    TransmissionSms_NumOfTxAttemptOther     = 0;

    /* indication of transmission in progress */
    TransmissionSms_TransmissionInProgress = FALSE;

    /* last phonebook index */
    TransmissionSms_PhonebookIndexForAlarm = 1;


    /* creation of timer "TimerDelaySmsTx" */
    err = ql_rtos_timer_create(&TransmissionSms_TimerHandler_TimerDelaySmsTx, QL_TIMER_IN_SERVICE, TransmissionSms_AdlCallback_Timer_TimerDelaySmsTx, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerCheckTx" */
    err = ql_rtos_timer_create(&TransmissionSms_TimerHandler_TimerCheckTx, QL_TIMER_IN_SERVICE, TransmissionSms_AdlCallback_Timer_TimerCheckTx, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* start timer check data to be transmitted */
    TransmissionSms_TimerCheckTx_Start(TIME_MS__CHECK_DATA_TX, TRUE);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            TransmissionSms_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : TransmissionSms_IsTransmissionTerminated
 *
 * Description: verify if the SMS transmission is terminated
 * Input      : -
 * Output     : - FALSE: the SMS transmission is not terminated
 *              - TRUE : the SMS transmission is     terminated
 *=============================================================================*/
bool TransmissionSms_IsTransmissionTerminated(void)
{
    u8   data_tx_type;
    bool result;


    /* verify if a transmission is in progress */
    if (TransmissionSms_TransmissionInProgress)
    {
        /* a transmission is in progress */
        return FALSE;
    }
    else
    {
        /* a transmission is not in progress */
        if (
             (TransmissionSms_NumOfTxAttemptPhonebook >= NUM_OF_SMS_TX_ATTEMPTS_PHONEBOOK) &&
             (TransmissionSms_NumOfTxAttemptMyn       >= NUM_OF_SMS_TX_ATTEMPTS_MYN      ) &&
             (TransmissionSms_NumOfTxAttemptFwd       >= NUM_OF_SMS_TX_ATTEMPTS_FWD      ) &&
             (TransmissionSms_NumOfTxAttemptOther     >= NUM_OF_SMS_TX_ATTEMPTS_OTHER    )
           )
        {
            /* transmission attempts terminated */
            return TRUE;
        }
    }


    /* verify if there are data to be transmitted by SMS */
    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
    if (!result)
    {
        /* there are data to be transmitted by SMS */
        return TRUE;
    }


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionSms_SendSms
 *
 * Description: transmit a data by SMS
 * Input      : - data_tx_type: data type to be transmitted
 * Output     : - FALSE: SMS not physically transmitted
 *              - TRUE : SMS     physically transmitted
 *=============================================================================*/
static bool TransmissionSms_SendSms(u8 data_tx_type)
{
    /* SMS text */
    static ascii                                sms_text[160 + 1];

    /* SMS phone number */
    static ascii                                sms_phone_number[MAX_LEN_PHONE_NUMBER + 1];

    /* phonebook, MYN and FWD numbers */
    static PHONEBOOK__CONFIG__PHONEBOOK         config_phonebook;
    static MYN__CONFIG__MYN_NUMBER              config_myn_number;
    static FWD__CONFIG__FWD_NUMBER              config_fwd_number;

    /* queue record */
    static QUEUE__FW_UPGRADE_QUEUE_RECORD       queue_fw_upgrade_record;
    static QUEUE__ALARM_QUEUE_RECORD            queue_alarm_record;
    static QUEUE__SMS_ANSWER_QUEUE_RECORD       queue_sms_answer_record;
    static QUEUE__SMS_FORWARD_QUEUE_RECORD      queue_sms_forward_record;
    static QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD  queue_autosynchronize_record;
    static QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD queue_sms_answer_event_record;

           u16                                  num_of_phonebook_numbers;

           bool                                 result;


    /*-----------------------------------------------------------------------------
     * SMS text
     *-----------------------------------------------------------------------------*/
    /* build the SMS text */
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            /* get the record to be transmitted from the queue */
            result = Queue_FwUpgrade_GetRecord      (&queue_fw_upgrade_record);
            if (!result)
                return FALSE;

            /* build the SMS */
            ParserTx_BuildSmsFwUpgrade              ((PARSER_TX__FW_UPGRADE_RECORD       *)&queue_fw_upgrade_record      , sms_text);
            break;


        /* data tx type - alarm */
        case DATA_TX__ALARM:
            /* get the record to be transmitted from the queue */
            result = Queue_Alarm_GetRecord          (&queue_alarm_record);
            if (!result)
                return FALSE;

            /* build the SMS */
            ParserTx_BuildSmsAlarm                  ((PARSER_TX__ALARM_RECORD            *)&queue_alarm_record           , sms_text);
            break;


        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            /* get the record to be transmitted from the queue */
            result = Queue_SmsAnswer_GetRecord      (&queue_sms_answer_record);
            if (!result)
                return FALSE;

            /* build the SMS text to be transmitted */
            ParserTx_BuildSmsSmsAnswer              ((PARSER_TX__SMS_ANSWER_RECORD       *)&queue_sms_answer_record      , sms_text);
            break;


        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            /* get the record to be transmitted from the queue */
            result = Queue_SmsForward_GetRecord     (&queue_sms_forward_record);
            if (!result)
                return FALSE;

            /* build the SMS text to be transmitted */
            ParserTx_BuildSmsSmsForward             ((PARSER_TX__SMS_FORWARD_RECORD      *)&queue_sms_forward_record     , sms_text);
            break;


        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            /* get the record to be transmitted from the queue */
            result = Queue_Autosynchronize_GetRecord(&queue_autosynchronize_record);
            if (!result)
                return FALSE;

            /* build the SMS text to be transmitted */
            ParserTx_BuildSmsAutosynchronize        ((PARSER_TX__AUTOSYNCHRONIZE_RECORD  *)&queue_autosynchronize_record , sms_text);

            /* save the autosynchronize SMS counter */
            TransmissionSms_SynchronizeSmsCounter = queue_autosynchronize_record;
            break;


        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            /* get the record to be transmitted from the queue */
            result = Queue_SmsAnswerEvent_GetRecord (&queue_sms_answer_event_record);
            if (!result)
                return FALSE;

            /* build the SMS text to be transmitted */
            ParserTx_BuildSmsSmsAnswerEvent         ((PARSER_TX__SMS_ANSWER_EVENT_RECORD *)&queue_sms_answer_event_record, sms_text);
            break;


        /* data tx type - unknown */
        default:
            return FALSE;
    }

    /* print the SMS text */
    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "SMS text        : \"%s\"", sms_text);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /*-----------------------------------------------------------------------------
     * SMS recipient phone number
     *-----------------------------------------------------------------------------*/
    /* get the SMS recipient phone number */
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            strncpy(sms_phone_number, queue_fw_upgrade_record.sms_phone_number                                      , MAX_LEN_PHONE_NUMBER);
            sms_phone_number[MAX_LEN_PHONE_NUMBER] = 0x00;
            break;


        /* data tx type - alarm */
        case DATA_TX__ALARM:
            num_of_phonebook_numbers = Phonebook_NumberOfPhonebookEntries();
            if (                                      (num_of_phonebook_numbers    > PHONEBOOK__PHONEBOOK_SIZE))
                return FALSE;

            if ((TransmissionSms_PhonebookIndexForAlarm      == 0) || (TransmissionSms_PhonebookIndexForAlarm      > num_of_phonebook_numbers))
                return FALSE;

            Phonebook_Config_Phonebook_Get(&config_phonebook);
            strncpy(sms_phone_number, config_phonebook.phonebook_entry[TransmissionSms_PhonebookIndexForAlarm - 1].phone_number, MAX_LEN_PHONE_NUMBER);
            sms_phone_number[MAX_LEN_PHONE_NUMBER] = 0x00;
            break;


        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            strncpy(sms_phone_number, queue_sms_answer_record.sms_phone_number                                      , MAX_LEN_PHONE_NUMBER);
            sms_phone_number[MAX_LEN_PHONE_NUMBER] = 0x00;
            break;


        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            Fwd_Config_FwdNumber_Get(&config_fwd_number);
            strncpy(sms_phone_number, config_fwd_number.fwd_number                                                  , MAX_LEN_PHONE_NUMBER);
            sms_phone_number[MAX_LEN_PHONE_NUMBER] = 0x00;
            break;


        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            Myn_Config_MynNumber_Get(&config_myn_number);
            strncpy(sms_phone_number, config_myn_number.myn_number                                                  , MAX_LEN_PHONE_NUMBER);
            sms_phone_number[MAX_LEN_PHONE_NUMBER] = 0x00;
            break;


        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            strncpy(sms_phone_number, queue_sms_answer_event_record.sms_phone_number                                , MAX_LEN_PHONE_NUMBER);
            sms_phone_number[MAX_LEN_PHONE_NUMBER] = 0x00;
            break;


        /* data tx type - unknown */
        default:
            return FALSE;
    }

    /* print the SMS recipient phone number */
    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "SMS phone number: \"%s\"", sms_phone_number);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* verify the SMS recipient phone number */
    result = Utility_IsPhoneString(sms_phone_number);
    if (!result)
        return FALSE;


    /*-----------------------------------------------------------------------------
     * SMS sending
     *-----------------------------------------------------------------------------*/
    /* send the SMS */
    result = Phone_SendSms(sms_phone_number, sms_text);
    if (!result)
        return FALSE;


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionSms_DataToBeTransmitted
 *
 * Description: verify the data type to be transmitted (if it is present);
 *              priority of transmission:
 *                   - towards phonebook recipient   (highest)
 *                         - ALARM
 *                   - towards other     recipient
 *                         - FW_UPGRADE
 *                         - SMS_ANSWER
 *                   - towards FWD       recipient
 *                         - SMS_FORWARD
 *                   - towards MYN       recipient
 *                         - AUTOSYNCHRONIZE         (lowest )
 * Input      : - ptr_data_tx_type: pointer to save the data type to be transmitted
 * Output     : - FALSE: there is no data type to be transmitted
 *              - TRUE : there is    data type to be transmitted
 *=============================================================================*/
static bool TransmissionSms_DataToBeTransmitted(u8 *ptr_data_tx_type)
{
    u8 num_records;


    /* verify if there is something to be transmitted */

    /*-----------------------------------------------------------------------------
     * Phonebook recipient
     *-----------------------------------------------------------------------------*/
    /* alarm queue */
    num_records = Queue_Alarm_NumRecords();
    if (num_records > 0)
    {
        *ptr_data_tx_type = DATA_TX__ALARM;
        return TRUE;
    }


    /*-----------------------------------------------------------------------------
     * other recipient
     *-----------------------------------------------------------------------------*/
    /* FW upgrade queue */
    num_records = Queue_FwUpgrade_NumRecords();
    if (num_records > 0)
    {
        *ptr_data_tx_type = DATA_TX__FW_UPGRADE;
        return TRUE;
    }

    /* SMS answer queue */
    num_records = Queue_SmsAnswer_NumRecords();
    if (num_records > 0)
    {
        *ptr_data_tx_type = DATA_TX__SMS_ANSWER;
        return TRUE;
    }

    /* SMS answer event queue */
    num_records = Queue_SmsAnswerEvent_NumRecords();
    if (num_records > 0)
    {
        *ptr_data_tx_type = DATA_TX__SMS_ANSWER_EVENT;
        return TRUE;
    }


    /*-----------------------------------------------------------------------------
     * FWD recipient
     *-----------------------------------------------------------------------------*/
    /* SMS forward queue */
    num_records = Queue_SmsForward_NumRecords();
    if (num_records > 0)
    {
        *ptr_data_tx_type = DATA_TX__SMS_FORWARD;
        return TRUE;
    }


    /*-----------------------------------------------------------------------------
     * MYN recipient
     *-----------------------------------------------------------------------------*/
    /* autosynchronize queue */
    num_records = Queue_Autosynchronize_NumRecords();
    if (num_records > 0)
    {
        *ptr_data_tx_type = DATA_TX__AUTOSYNCHRONIZE;
        return TRUE;
    }


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionSms_ExistDataToBeTransmitted
 *
 * Description: verify if data to be transmitted exists
 *              it checks:
 *                   - if SMS transmission is allowed
 *                   - if the recipient phone number exists
 * Input      : -
 * Output     : - FALSE: there is no data to be transmitted
 *              - TRUE : there is    data to be transmitted
 *=============================================================================*/
bool TransmissionSms_ExistDataToBeTransmitted(void)
{
    u8 num_records;


    /* verify if there is something to be transmitted */

    /*-----------------------------------------------------------------------------
     * Phonebook recipient
     *-----------------------------------------------------------------------------*/
    /* alarm queue */
    num_records = Queue_Alarm_NumRecords();
    if (num_records > 0)
        return TRUE;


    /*-----------------------------------------------------------------------------
     * other recipient
     *-----------------------------------------------------------------------------*/
    /* FW upgrade queue */
    num_records = Queue_FwUpgrade_NumRecords();
    if (num_records > 0)
        return TRUE;

    /* SMS answer queue */
    num_records = Queue_SmsAnswer_NumRecords();
    if (num_records > 0)
        return TRUE;

    /* SMS answer event queue */
    num_records = Queue_SmsAnswerEvent_NumRecords();
    if (num_records > 0)
        return TRUE;


    /*-----------------------------------------------------------------------------
     * FWD recipient
     *-----------------------------------------------------------------------------*/
    /* SMS forward queue */
    num_records = Queue_SmsForward_NumRecords();
    if (num_records > 0)


    /*-----------------------------------------------------------------------------
     * MYN recipient
     *-----------------------------------------------------------------------------*/
    /* autosynchronize queue */
    num_records = Queue_Autosynchronize_NumRecords();
    if (num_records > 0)
        return TRUE;


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionSms_ExistRecipientPhoneNumber
 *
 * Description: verify if the recipient phone number for the data type to be transmitted
 *              exists
 * Input      : - data_tx_type: data type to be transmitted
 * Output     : - FALSE: recipient phone number for the data type to be transmitted doesn't exists
 *              - TRUE : recipient phone number for the data type to be transmitted         exists
 *=============================================================================*/
static bool TransmissionSms_ExistRecipientPhoneNumber(u8 data_tx_type)
{
    u16 num_of_phonebook_numbers;
    u8  num_of_myn_numbers;
    u8  num_of_fwd_numbers;


    /* get the SMS recipient phone number */
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            // NO CHECK TO DO
            return TRUE;


        /* data tx type - alarm */
        case DATA_TX__ALARM:
            num_of_phonebook_numbers = Phonebook_NumberOfPhonebookEntries();
            if (num_of_phonebook_numbers > 0)
                return TRUE;
            else
                return FALSE;
            break;


        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            // NO CHECK TO DO
            return TRUE;


        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            num_of_fwd_numbers = Fwd_NumberOfFwdNumbers();
            if (num_of_fwd_numbers > 0)
                return TRUE;
            else
                return FALSE;
            break;


        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            num_of_myn_numbers = Myn_NumberOfMynNumbers();
            if (num_of_myn_numbers > 0)
                return TRUE;
            else
                return FALSE;
            break;


        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            // NO CHECK TO DO
            return TRUE;


        /* data tx type - unknown */
        default:
            return FALSE;
    }


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionSms_NumMaxOfTxAttempt
 *
 * Description: return the maximum number of transmission attempt for a specified data type
 * Input      : - data_tx_type: data type
 * Output     : - maximum number of transmission attempt
 *=============================================================================*/
static u8 TransmissionSms_NumMaxOfTxAttempt(u8 data_tx_type)
{
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            return NUM_OF_SMS_TX_ATTEMPTS_OTHER;

        /* data tx type - alarm */
        case DATA_TX__ALARM:
            return NUM_OF_SMS_TX_ATTEMPTS_PHONEBOOK;

        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            return NUM_OF_SMS_TX_ATTEMPTS_OTHER;

        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            return NUM_OF_SMS_TX_ATTEMPTS_FWD;

        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            return NUM_OF_SMS_TX_ATTEMPTS_MYN;

        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            return NUM_OF_SMS_TX_ATTEMPTS_OTHER;

        /* data tx type - unknown */
        default:
            return 0;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_NumOfTxAttempt
 *
 * Description: return the number of transmission attempt for a specified data type
 * Input      : - data_tx_type: data type for which the number of transmission attempt has to be resetted
 * Output     : - number of transmission attempt
 *=============================================================================*/
static u8 TransmissionSms_NumOfTxAttempt(u8 data_tx_type)
{
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            return TransmissionSms_NumOfTxAttemptOther;

        /* data tx type - alarm */
        case DATA_TX__ALARM:
            return TransmissionSms_NumOfTxAttemptPhonebook;

        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            return TransmissionSms_NumOfTxAttemptOther;

        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            return TransmissionSms_NumOfTxAttemptFwd;

        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            return TransmissionSms_NumOfTxAttemptMyn;

        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            return TransmissionSms_NumOfTxAttemptOther;

        /* data tx type - unknown */
        default:
            return 0;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_ResetNumOfTxAttempt
 *
 * Description: reset the number of transmission attempt for a specified data type
 * Input      : - data_tx_type: data type for which the number of transmission attempt has to be resetted
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_ResetNumOfTxAttempt(u8 data_tx_type)
{
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            TransmissionSms_NumOfTxAttemptOther     = 0;
            break;

        /* data tx type - alarm */
        case DATA_TX__ALARM:
            TransmissionSms_NumOfTxAttemptPhonebook = 0;
            break;

        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            TransmissionSms_NumOfTxAttemptOther     = 0;
            break;

        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            TransmissionSms_NumOfTxAttemptFwd       = 0;
            break;

        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            TransmissionSms_NumOfTxAttemptMyn       = 0;
            break;

        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            TransmissionSms_NumOfTxAttemptOther     = 0;
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_IncreaseNumOfTxAttempt
 *
 * Description: increase the number of transmission attempt for a specified data type
 * Input      : - data_tx_type: data type for which the number of transmission attempt has to be increased
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_IncreaseNumOfTxAttempt(u8 data_tx_type)
{
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            TransmissionSms_NumOfTxAttemptOther++;
            break;

        /* data tx type - alarm */
        case DATA_TX__ALARM:
            TransmissionSms_NumOfTxAttemptPhonebook++;
            break;

        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            TransmissionSms_NumOfTxAttemptOther++;
            break;

        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            TransmissionSms_NumOfTxAttemptFwd++;
            break;

        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            TransmissionSms_NumOfTxAttemptMyn++;
            break;

        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            TransmissionSms_NumOfTxAttemptOther++;
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_DeleteData
 *
 * Description: delete data from the queue (for the data type specified)
 * Input      : - data_tx_type: data type to be deleted
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_DeleteData(u8 data_tx_type)
{
    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            /* delete the records to be transmitted from the queue */
            Queue_FwUpgrade_DeleteRecords();
            break;


        /* data tx type - alarm */
        case DATA_TX__ALARM:
            /* delete the records to be transmitted from the queue */
            Queue_Alarm_DeleteRecords();
            break;


        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            /* delete the records to be transmitted from the queue */
            Queue_SmsAnswer_DeleteRecords();
            break;


        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            /* delete the records to be transmitted from the queue */
            Queue_SmsForward_DeleteRecords();
            break;


        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            /* delete the records to be transmitted from the queue */
            Queue_Autosynchronize_DeleteRecords();
            break;


        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            /* delete the records to be transmitted from the queue */
            Queue_SmsAnswerEvent_DeleteRecords();
            break;


        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_DeleteDataGot
 *
 * Description: delete data already got from the queue (for the data type specified)
 * Input      : - data_tx_type: data type to be deleted
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_DeleteDataGot(u8 data_tx_type)
{
    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "Delete data got... (data type: %d)", data_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            Queue_FwUpgrade_DeleteRecordsGot();
            break;

        /* data tx type - alarm */
        case DATA_TX__ALARM:
            Queue_Alarm_DeleteRecordsGot();
            break;

        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            Queue_SmsAnswer_DeleteRecordsGot();
            break;

        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            Queue_SmsForward_DeleteRecordsGot();
            break;

        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            Queue_Autosynchronize_DeleteRecordsGot();
            break;

        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            Queue_SmsAnswerEvent_DeleteRecordsGot();
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_RecoverDataGot
 *
 * Description: recover data already got from the queue (for the data type specfied)
 * Input      : - data_tx_type: data type to be recovered
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_RecoverDataGot(u8 data_tx_type)
{
    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "Recover data got... (data type: %d)", data_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (data_tx_type)
    {
        /* data tx type - FW upgrade */
        case DATA_TX__FW_UPGRADE:
            Queue_FwUpgrade_RecoverRecordsGot();
            break;

        /* data tx type - alarm */
        case DATA_TX__ALARM:
            Queue_Alarm_RecoverRecordsGot();
            break;

        /* data tx type - SMS answer */
        case DATA_TX__SMS_ANSWER:
            Queue_SmsAnswer_RecoverRecordsGot();
            break;

        /* data tx type - SMS forward */
        case DATA_TX__SMS_FORWARD:
            Queue_SmsForward_RecoverRecordsGot();
            break;

        /* data tx type - autosynchronize */
        case DATA_TX__AUTOSYNCHRONIZE:
            Queue_Autosynchronize_RecoverRecordsGot();
            break;

        /* data tx type - SMS answer event */
        case DATA_TX__SMS_ANSWER_EVENT:
            Queue_SmsAnswerEvent_RecoverRecordsGot();
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_Event_DataTx
 *
 * Description: signal presence of data to be transmitted
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionSms_Event_DataTx(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__DATA_TX;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_Event_GsmNetworkRegistered
 *
 * Description: signal GSM network registration
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionSms_Event_GsmNetworkRegistered(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_NET_REGISTERED;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_Event_GsmNetworkNotRegistered
 *
 * Description: signal GSM network deregistration
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionSms_Event_GsmNetworkNotRegistered(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_NET_NOT_REGISTERED;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_Event_SmsTxOk
 *
 * Description: signal success of SMS physical transmission
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionSms_Event_SmsTxOk(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__SMS_TX_OK;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_Event_SmsTxFail
 *
 * Description: signal failure of SMS physical transmission
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionSms_Event_SmsTxFail(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__SMS_TX_FAIL;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_TimerDelaySmsTx_Start
 *
 * Description: start the "delay SMS tx" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_TimerDelaySmsTx_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "Start \"delay SMS tx\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionSms_TimerHandler_TimerDelaySmsTx, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_TimerCheckTx_Start
 *
 * Description: start the "check tx" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_TimerCheckTx_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "Start \"check tx\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionSms_TimerHandler_TimerCheckTx, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : TransmissionSms_AdlCallback_Message_TaskMsg
 *
 * Description: - task SMS transmission message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void TransmissionSms_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    static       u8     sem_state      = SEM_STATE__IDLE;
    static       bool   gsm_registered = FALSE;
    static       u8     data_tx_type;

                 u16    num_of_phonebook_numbers;
                 u16    phonebook_index;

                 u8     num_of_tx_attempt;
                 u8     max_num_of_tx_attempt;

                 u32    timeout;
                 u32    sem_event;
                 bool   result;

    static const ascii *sem_state_strings[] =
    {
        SEM_STATE_STRING__IDLE,
        SEM_STATE_STRING__WAITING_FOR_RESOURCE,
        SEM_STATE_STRING__SMS_TX,
        SEM_STATE_STRING__TX_ATTEMPTS_DELAY,
    };

    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__DATA_TX,
        SEM_EVENT_STRING__GSM_NET_REGISTERED,
        SEM_EVENT_STRING__GSM_NET_NOT_REGISTERED,
        SEM_EVENT_STRING__SMS_TX_OK,
        SEM_EVENT_STRING__SMS_TX_FAIL,
        SEM_EVENT_STRING__TIMEOUT_DELAY_AFTER_SMS,
        SEM_EVENT_STRING__TIMEOUT_CHECK_TX,
    };


    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "CALLBACK     - MESSAGE     - TASK TRANSMISSION SMS - msg_identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


  //sem_event = msg_identifier;
    switch (msg_identifier)
    {
        case TASK_MSG_ID__DATA_TX:
            sem_event = SEM_EVENT__DATA_TX;
            break;

        case TASK_MSG_ID__GSM_NET_REGISTERED:
            sem_event = SEM_EVENT__GSM_NET_REGISTERED;
            break;

        case TASK_MSG_ID__GSM_NET_NOT_REGISTERED:
            sem_event = SEM_EVENT__GSM_NET_NOT_REGISTERED;
            break;

        case TASK_MSG_ID__SMS_TX_OK:
            sem_event = SEM_EVENT__SMS_TX_OK;
            break;

        case TASK_MSG_ID__SMS_TX_FAIL:
            sem_event = SEM_EVENT__SMS_TX_FAIL;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_AFTER_SMS:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_AFTER_SMS;
            break;

        case TASK_MSG_ID__TIMEOUT_CHECK_TX:
            sem_event = SEM_EVENT__TIMEOUT_CHECK_TX;
            break;

        default:
            return;
    }


    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "SEM state: %s", sem_state_strings[sem_state]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "SEM event: %s", sem_event_strings[sem_event]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(TransmissionSms_DebugString, sizeof(TransmissionSms_DebugString), "SMS Tx attempt (Phonebook, Other, MYN, FWD): %d, %d, %d, %d", TransmissionSms_NumOfTxAttemptPhonebook, TransmissionSms_NumOfTxAttemptOther, TransmissionSms_NumOfTxAttemptMyn, TransmissionSms_NumOfTxAttemptFwd);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, TransmissionSms_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (sem_event)
    {
        /* SEM event - GSM network registration */
        case SEM_EVENT__GSM_NET_REGISTERED:
            gsm_registered = TRUE;
            break;

        /* SEM event - GSM network deregistration */
        case SEM_EVENT__GSM_NET_NOT_REGISTERED:
            gsm_registered = FALSE;
            break;

        /* event - other events */
        default:
            break;
    }



    switch (sem_state)
    {
        /* SEM state - idle */
        case SEM_STATE__IDLE:
            switch (sem_event)
            {
                /* SEM event - data to be transmitted */
                /* SEM event - timeout check data to be transmitted */
                case SEM_EVENT__DATA_TX:
                case SEM_EVENT__TIMEOUT_CHECK_TX:
                    /* verify if there are data to be transmitted */
                    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
                    if (result)
                    {
                        /* there is data to be transmitted */

                        /* verify if the recipient phone number exists */
                        result = TransmissionSms_ExistRecipientPhoneNumber(data_tx_type);
                        if (result)
                        {
                            /* the recipient phone number exists */

                            if (gsm_registered)
                            {
                                /* phone registered in GSM network */

                                timeout = TIME_MS__DELAY_AFTER_GSM_REG;
                                TransmissionSms_TimerDelaySmsTx_Start(timeout, FALSE);
                                sem_state = SEM_STATE__TX_ATTEMPTS_DELAY;
                            }
                            else
                            {
                                /* phone not registered in GSM network */

                                sem_state = SEM_STATE__WAITING_FOR_RESOURCE;
                            }

                            /* indication of transmission in progress */
                            TransmissionSms_TransmissionInProgress = TRUE;
                        }
                        else
                        {
                            /* the recipient phone number does not exist */

                            /* delete the data from the transmission queue */
                            TransmissionSms_DeleteData(data_tx_type);

                            TransmissionSms_TransmissionInProgress = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted */
                        TransmissionSms_TransmissionInProgress = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - waiting for resource */
        case SEM_STATE__WAITING_FOR_RESOURCE:
            switch (sem_event)
            {
                /* SEM event - GSM network registration */
                case SEM_EVENT__GSM_NET_REGISTERED:
                    /* verify if there are data to be transmitted */
                    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
                    if (result)
                    {
                        /* there is data to be transmitted */

                        /* verify if the recipient phone number exists */
                        result = TransmissionSms_ExistRecipientPhoneNumber(data_tx_type);
                        if (result)
                        {
                            /* the recipient phone number exists */

                            if (gsm_registered)
                            {
                                /* phone registered in GSM network */

                                timeout = TIME_MS__DELAY_AFTER_GSM_REG;
                                TransmissionSms_TimerDelaySmsTx_Start(timeout, FALSE);
                                sem_state = SEM_STATE__TX_ATTEMPTS_DELAY;
                            }
                            else
                            {
                                /* phone not registered in GSM network */

                                sem_state = SEM_STATE__WAITING_FOR_RESOURCE;
                            }
                        }
                        else
                        {
                            /* the recipient phone number does not exist */

                            /* delete the data from the transmission queue */
                            TransmissionSms_DeleteData(data_tx_type);

                            TransmissionSms_TransmissionInProgress = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted */
                        TransmissionSms_TransmissionInProgress = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - SMS transmission */
        case SEM_STATE__SMS_TX:
            switch (sem_event)
            {
                /* SEM event - success of SMS transmission */
                case SEM_EVENT__SMS_TX_OK:
                    if (data_tx_type == DATA_TX__AUTOSYNCHRONIZE)
                    {
                        /* the SMS just transmitted is an autosynchronize SMS */

                        /* signal the synchronize SMS transmission */
                        Synchronize_Event_SmsTx(TransmissionSms_SynchronizeSmsCounter);
                    }


                    if (data_tx_type == DATA_TX__ALARM)
                    {
                        /* transmission to phonebook */

                        if (data_tx_type == DATA_TX__ALARM)
                            phonebook_index = TransmissionSms_PhonebookIndexForAlarm;

                        num_of_phonebook_numbers = Phonebook_NumberOfPhonebookEntries();

                        if (phonebook_index < num_of_phonebook_numbers)
                        {
                            /* there is another phonebook entry */
                            /* transmission of that data (to phonebook) not finished */
                            /* start transmission to next phonebook entry */
                            TransmissionSms_RecoverDataGot     (data_tx_type);  /* recover the data from the transmission queue */
                            TransmissionSms_ResetNumOfTxAttempt(data_tx_type);  /* reset the number of transmission attempt     */
                            phonebook_index++;
                        }
                        else
                        {
                            /* there is no other phonebook entry */
                            /* transmission of that data (to phonebook)     finished */
                            TransmissionSms_DeleteDataGot      (data_tx_type);  /* delete the data from the transmission queue */
                            TransmissionSms_ResetNumOfTxAttempt(data_tx_type);  /* reset the number of transmission attempt    */
                            phonebook_index = 1;

                            UiLed_SignalSmsTxFailStop();
                        }

                        if (data_tx_type == DATA_TX__ALARM)
                            TransmissionSms_PhonebookIndexForAlarm = phonebook_index;
                    }
                    else
                    {
                        /* transmission to MYN or FWD or other */
                        /* transmission of that data (to MYN or FWD or to other) finished */
                        TransmissionSms_DeleteDataGot      (data_tx_type);      /* delete the data from the transmission queue */
                        TransmissionSms_ResetNumOfTxAttempt(data_tx_type);      /* reset the number of transmission attempt    */

                        UiLed_SignalSmsTxFailStop();
                    }


                    /* verify if there are data to be transmitted */
                    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
                    if (result)
                    {
                        /* there is data to be transmitted */

                        /* verify if the recipient phone number exists */
                        result = TransmissionSms_ExistRecipientPhoneNumber(data_tx_type);
                        if (result)
                        {
                            /* the recipient phone number exists */

                            timeout = TIME_MS__DELAY_AFTER_SMS_TX_OK;
                            TransmissionSms_TimerDelaySmsTx_Start(timeout, FALSE);
                            sem_state = SEM_STATE__TX_ATTEMPTS_DELAY;
                        }
                        else
                        {
                            /* the recipient phone number does not exist */

                            /* delete the data from the transmission queue */
                            TransmissionSms_DeleteData(data_tx_type);

                            TransmissionSms_TransmissionInProgress = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted */
                        TransmissionSms_TransmissionInProgress = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* SEM event - failure of SMS transmission */
                case SEM_EVENT__SMS_TX_FAIL:
                    num_of_tx_attempt     = TransmissionSms_NumOfTxAttempt   (data_tx_type);
                    max_num_of_tx_attempt = TransmissionSms_NumMaxOfTxAttempt(data_tx_type);

                    if (data_tx_type == DATA_TX__ALARM)
                    {
                        /* transmission to phonebook */
                        if (num_of_tx_attempt < max_num_of_tx_attempt)
                        {
                            /* number of transmission attempts                not finished */
                            /* transmission of that data (to phonebook entry) not finished */
                            TransmissionSms_RecoverDataGot         (data_tx_type);          /* recover the data from the transmission queue */
                        }
                        else
                        {
                            /* number of transmission attempts                     finished */

                            if (data_tx_type == DATA_TX__ALARM)
                                phonebook_index = TransmissionSms_PhonebookIndexForAlarm;

                            num_of_phonebook_numbers = Phonebook_NumberOfPhonebookEntries();

                            if (phonebook_index < num_of_phonebook_numbers)
                            {
                                /* there is another phonebook entry */
                                /* transmission of that data (to phonebook) not finished */
                                /* start transmission to next phonebook entry */
                                TransmissionSms_RecoverDataGot     (data_tx_type);          /* recover the data from the transmission queue */
                                TransmissionSms_ResetNumOfTxAttempt(data_tx_type);          /* reset the number of transmission attempt     */
                                phonebook_index++;
                            }
                            else
                            {
                                /* there is no other phonebook entry */
                                /* transmission of that data (to phonebook)     finished */
                                TransmissionSms_DeleteDataGot      (data_tx_type);          /* delete the data from the transmission queue */
                                TransmissionSms_ResetNumOfTxAttempt(data_tx_type);          /* reset the number of transmission attempt    */
                                phonebook_index = 1;

                                UiLed_SignalSmsTxFailStart();
                            }

                            if (data_tx_type == DATA_TX__ALARM)
                                TransmissionSms_PhonebookIndexForAlarm = phonebook_index;
                        }
                    }
                    else
                    {
                        /* transmission to MYN or FWD or other */
                        if (num_of_tx_attempt < max_num_of_tx_attempt)
                        {
                            /* number of transmission attempts                       not finished */
                            /* transmission of that data (to MYN or FWD or to other) not finished */
                            TransmissionSms_RecoverDataGot     (data_tx_type);              /* recover the data from the transmission queue */
                        }
                        else
                        {
                            /* number of transmission attempts                           finished */
                            /* transmission of that data (to MYN or FWD or to other)     finished */
                            TransmissionSms_DeleteDataGot      (data_tx_type);              /* delete the data from the transmission queue */
                            TransmissionSms_ResetNumOfTxAttempt(data_tx_type);              /* reset the number of transmission attempt    */

                            UiLed_SignalSmsTxFailStart();
                        }
                    }


                    /* verify if there are data to be transmitted */
                    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
                    if (result)
                    {
                        /* there is data to be transmitted */

                        /* verify if the recipient phone number exists */
                        result = TransmissionSms_ExistRecipientPhoneNumber(data_tx_type);
                        if (result)
                        {
                            /* the recipient phone number exists */

                            timeout = TIME_MS__DELAY_AFTER_SMS_TX_FAIL;
                            TransmissionSms_TimerDelaySmsTx_Start(timeout, FALSE);
                            sem_state = SEM_STATE__TX_ATTEMPTS_DELAY;
                        }
                        else
                        {
                            /* the recipient phone number does not exist */

                            /* delete the data from the transmission queue */
                            TransmissionSms_DeleteData(data_tx_type);

                            TransmissionSms_TransmissionInProgress = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted */
                        TransmissionSms_TransmissionInProgress = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - tx attempts delay */
        case SEM_STATE__TX_ATTEMPTS_DELAY:
            switch (sem_event)
            {
                /* SEM event - timeout after SMS tranmission expired */
                case SEM_EVENT__TIMEOUT_DELAY_AFTER_SMS:
                    /* verify if there are data to be transmitted */
                    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
                    if (result)
                    {
                        /* there is data to be transmitted */

                        /* verify if the recipient phone number exists */
                        result = TransmissionSms_ExistRecipientPhoneNumber(data_tx_type);
                        if (result)
                        {
                            /* the recipient phone number exists */

                            if (gsm_registered)
                            {
                                /* phone registered in GSM network */

                                /* increase the number of transmission attempt */
                                TransmissionSms_IncreaseNumOfTxAttempt(data_tx_type);

                                /* send SMS */
                                result = TransmissionSms_SendSms(data_tx_type);
                                if (result)
                                {
                                    /* SMS physically sent, we wait for the result */

                                    sem_state = SEM_STATE__SMS_TX;
                                }
                                else
                                {
                                    /* SMS physically not sent */

                                    num_of_tx_attempt     = TransmissionSms_NumOfTxAttempt   (data_tx_type);
                                    max_num_of_tx_attempt = TransmissionSms_NumMaxOfTxAttempt(data_tx_type);

                                    if (data_tx_type == DATA_TX__ALARM)
                                    {
                                        /* transmission to phonebook */
                                        if (num_of_tx_attempt < max_num_of_tx_attempt)
                                        {
                                            /* number of transmission attempts                not finished */
                                            /* transmission of that data (to phonebook entry) not finished */
                                            TransmissionSms_RecoverDataGot        (data_tx_type);    /* recover the data from the transmission queue */
                                        }
                                        else
                                        {
                                            /* number of transmission attempts                    finished */

                                            if (data_tx_type == DATA_TX__ALARM)
                                                phonebook_index = TransmissionSms_PhonebookIndexForAlarm;

                                            num_of_phonebook_numbers = Phonebook_NumberOfPhonebookEntries();

                                            if (phonebook_index < num_of_phonebook_numbers)
                                            {
                                                /* there is another phonebook entry */
                                                /* transmission of that data (to phonebook) not finished */
                                                /* start transmission to next phonebook entry */
                                                TransmissionSms_RecoverDataGot     (data_tx_type);   /* recover the data from the transmission queue */
                                                TransmissionSms_ResetNumOfTxAttempt(data_tx_type);   /* reset the number of transmission attempt     */
                                                phonebook_index++;
                                            }
                                            else
                                            {
                                                /* there is no other phonebook entry */
                                                /* transmission of that data (to phonebook) finished */
                                                TransmissionSms_DeleteDataGot      (data_tx_type);   /* delete the data from the transmission queue */
                                                TransmissionSms_ResetNumOfTxAttempt(data_tx_type);   /* reset the number of transmission attempt    */
                                                phonebook_index = 1;

                                                UiLed_SignalSmsTxFailStop();
                                            }

                                            if (data_tx_type == DATA_TX__ALARM)
                                                TransmissionSms_PhonebookIndexForAlarm = phonebook_index;
                                        }
                                    }
                                    else
                                    {
                                        /* transmission to MYN or FWD or other */
                                        if (num_of_tx_attempt < max_num_of_tx_attempt)
                                        {
                                            /* number of transmission attempts                       not finished */
                                            /* transmission of that data (to MYN or FWD or to other) not finished */
                                            TransmissionSms_RecoverDataGot        (data_tx_type);    /* recover the data from the transmission queue */
                                        }
                                        else
                                        {
                                            /* number of transmission attempts                           finished */
                                            /* transmission of that data (to MYN or FWD or to other)     finished */
                                            TransmissionSms_DeleteDataGot      (data_tx_type);       /* delete the data from the transmission queue */
                                            TransmissionSms_ResetNumOfTxAttempt(data_tx_type);       /* reset the number of transmission attempt    */

                                            UiLed_SignalSmsTxFailStop();
                                        }
                                    }


                                    /* verify if there are data to be transmitted */
                                    result = TransmissionSms_DataToBeTransmitted(&data_tx_type);
                                    if (result)
                                    {
                                        /* there is    data to be transmitted */

                                        /* verify if the recipient phone number exists */
                                        result = TransmissionSms_ExistRecipientPhoneNumber(data_tx_type);
                                        if (result)
                                        {
                                            /* the recipient phone number exists */

                                            timeout = TIME_MS__DELAY_AFTER_SMS_TX_FAIL;
                                            TransmissionSms_TimerDelaySmsTx_Start(timeout, FALSE);
                                            sem_state = SEM_STATE__TX_ATTEMPTS_DELAY;
                                        }
                                        else
                                        {
                                            /* the recipient phone number does not exist */

                                            /* delete the data from the transmission queue */
                                            TransmissionSms_DeleteData(data_tx_type);

                                            TransmissionSms_TransmissionInProgress = FALSE;
                                            sem_state = SEM_STATE__IDLE;
                                        }
                                    }
                                    else
                                    {
                                        /* there is no data to be transmitted */
                                        TransmissionSms_TransmissionInProgress = FALSE;
                                        sem_state = SEM_STATE__IDLE;
                                    }
                                }
                            }
                            else
                            {
                                /* phone not registered in GSM network */

                                sem_state = SEM_STATE__WAITING_FOR_RESOURCE;
                            }
                        }
                        else
                        {
                            /* the recipient phone number does not exist */

                            /* delete the data from the transmission queue */
                            TransmissionSms_DeleteDataGot(data_tx_type);

                            UiLed_SignalSmsTxFailStop();

                            TransmissionSms_TransmissionInProgress = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted */
                        TransmissionSms_TransmissionInProgress = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - unknown state */
        default:
            TransmissionSms_TransmissionInProgress = FALSE;
            sem_state = SEM_STATE__IDLE;
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionSms_AdlCallback_Timer_TimerDelaySmsTx
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_AdlCallback_Timer_TimerDelaySmsTx(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - DELAY SMS TX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_AFTER_SMS;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionSms_AdlCallback_Timer_TimerCheckTx
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionSms_AdlCallback_Timer_TimerCheckTx(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - CHECK TX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_CHECK_TX;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionSms, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_SMS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
