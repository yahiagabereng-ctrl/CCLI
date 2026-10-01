/*=============================================================================
 * File       :  TRANSMISSION_GPRS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - GPRS transmission management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "alarm.h"
#include "boot.h"
#include "clock.h"
#include "debug_my.h"
#include "file_system.h"
#include "fw_config.h"
#include "http_my.h"
#include "log_status.h"
#include "main.h"
#include "parser_tx.h"
#include "phone.h"
#include "program_flash.h"
#include "queue_my.h"
#include "transmission_gprs.h"
#include "transmission_Sms.h"
#include "typedef.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* msg tx type */
#define MSG_TX__SMS_ANSWER_TO_CRS                                 0                         /* msg tx type - init 1 */

/* data tx type */
#define DATA_TX__FILE_STATUS_LOG                                  0                         /* data tx type - file status log */
#define DATA_TX__FILE_ALARM                                       1                         /* data tx type - file alarm      */

/* number of attempts */
#define NUM_OF_APN_CONNECTION_ATTEMPTS                            2                         /* number of APN connection    attempts */
#define NUM_OF_HTTP_REQUEST_ATTEMPTS                              2                         /* number of HTTP request      attempts */
#define NUM_OF_APN_DISCONNECTION_ATTEMPTS                         2                         /* number of APN disconnection attempts */

/* timeout values (ms) */
#define TIME_MS__CHECK_DATA_TX                                    (           4 * 1000L)    /* time check data to be transmitted                                (ms) */
#define TIME_MS__COMMAND_REQUEST                                  (24 * 60 * 60 * 1000L)    /* time command request                                             (ms) */

#define TIME_MS__APN_CONNECT_DELAY__AFTER_GSM_REG                 (           5 * 1000L)    /* time delay before APN connection after GSM network registration  (ms) */
#define TIME_MS__APN_CONNECT_DELAY__AFTER_APN_CONNECT_FAIL        (           5 * 1000L)    /* time delay before APN connection after APN connection    fail    (ms) */
#define TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION       (           3 * 1000L)    /* time delay before APN connection after APN disconnection         (ms) */

#define TIME_MS__HTTP_GET_DELAY__AFTER_APN_CONNECT_OK             (                500L)    /* time delay before HTTP GET after APN connection OK               (ms) */
#define TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_OK            (                500L)    /* time delay before HTTP GET after HTTP request   OK               (ms) */
#define TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_FAIL          (           1 * 1000L)    /* time delay before HTTP GET after HTTP request   fail             (ms) */

#define TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST    (                500L)    /* time delay before APN disconnection after last HTTP request      (ms) */
#define TIME_MS__APN_DISCONNECT_DELAY__AFTER_APN_DISCONNECT_FAIL  (           2 * 1000L)    /* time delay before APN disconnection after APN disconnection fail (ms) */

#define TIME_MS__WAIT_APN_CONNECT                                 (          60 * 1000L)    /* time wait for the end of APN connection                          (ms) */
#define TIME_MS__WAIT_HTTP_REQUEST                                (          60 * 1000L)    /* time wait for the end of HTTP request                            (ms) */
#define TIME_MS__WAIT_APN_DISCONNECT                              (          30 * 1000L)    /* time wait for the end of APN disconnection                       (ms) */

/* number of consecutive fail transmission for alarm generation */
#define NUM_OF_CONSECUTIVE_FAIL_TX_FOR_ALARM                      20

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                                   200

/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__IDLE                                           0               /* SEM state - idle                          */
#define SEM_STATE__WAITING_FOR_RESOURCE                           1               /* SEM state - waiting for resource          */
#define SEM_STATE__APN_CONNECT_DELAY                              2               /* SEM state - APN connection    delay       */
#define SEM_STATE__APN_CONNECT_IN_PROGRESS                        3               /* SEM state - APN connection    in progress */
#define SEM_STATE__HTTP_REQUEST_DELAY                             4               /* SEM state - HTTP request      delay       */
#define SEM_STATE__HTTP_REQUEST_IN_PROGRESS                       5               /* SEM state - HTTP request      in progress */
#define SEM_STATE__APN_DISCONNECT_DELAY                           6               /* SEM state - APN disconnection delay       */
#define SEM_STATE__APN_DISCONNECT_IN_PROGRESS                     7               /* SEM state - APN disconnection in progress */

/* SEM state strings */
#define SEM_STATE_STRING__IDLE                                    "SEM_STATE__IDLE"
#define SEM_STATE_STRING__WAITING_FOR_RESOURCE                    "SEM_STATE__WAITING_FOR_RESOURCE"
#define SEM_STATE_STRING__APN_CONNECT_DELAY                       "SEM_STATE__APN_CONNECT_DELAY"
#define SEM_STATE_STRING__APN_CONNECT_IN_PROGRESS                 "SEM_STATE__APN_CONNECT_IN_PROGRESS"
#define SEM_STATE_STRING__HTTP_REQUEST_DELAY                      "SEM_STATE__HTTP_REQUEST_DELAY"
#define SEM_STATE_STRING__HTTP_REQUEST_IN_PROGRESS                "SEM_STATE__HTTP_REQUEST_IN_PROGRESS"
#define SEM_STATE_STRING__APN_DISCONNECT_DELAY                    "SEM_STATE__APN_DISCONNECT_DELAY"
#define SEM_STATE_STRING__APN_DISCONNECT_IN_PROGRESS              "SEM_STATE__APN_DISCONNECT_IN_PROGRESS"

/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__DATA_TX_FORCE                                  0               /* SEM event - force data transmission                 */
#define SEM_EVENT__DATA_TX                                        1               /* SEM event - data to be transmitted                  */
#define SEM_EVENT__GSM_NET_REGISTERED                             2               /* SEM event - GSM network registration                */
#define SEM_EVENT__GSM_NET_NOT_REGISTERED                         3               /* SEM event - GSM network deregistration              */
#define SEM_EVENT__APN_CONNECT_OK                                 4               /* SEM event - success of APN connection               */
#define SEM_EVENT__APN_CONNECT_FAIL                               5               /* SEM event - failure of APN connection               */
#define SEM_EVENT__APN_HTTP_GET_RX_OK                             6               /* SEM event - success of HTTP GET "RX"                */
#define SEM_EVENT__APN_HTTP_GET_RX_FAIL                           7               /* SEM event - failure of HTTP GET "RX"                */
#define SEM_EVENT__APN_HTTP_GET_TX_OK                             8               /* SEM event - success of HTTP GET "TX"                */
#define SEM_EVENT__APN_HTTP_GET_TX_FAIL                           9               /* SEM event - failure of HTTP GET "TX"                */
#define SEM_EVENT__APN_HTTP_POST_OK                               10              /* SEM event - success of HTTP POST                    */
#define SEM_EVENT__APN_HTTP_POST_FAIL                             11              /* SEM event - failure of HTTP POST                    */
#define SEM_EVENT__APN_DISCONNECT_OK                              12              /* SEM event - success of APN disconnection            */
#define SEM_EVENT__APN_DISCONNECT_FAIL                            13              /* SEM event - failure of APN disconnection            */
#define SEM_EVENT__APN_CONNECTION_DOWN                            14              /* SEM event - APN connection down                     */
#define SEM_EVENT__TIMEOUT_CHECK_TX                               15              /* SEM event - timeout check data to be transmitted    */
#define SEM_EVENT__TIMEOUT_COMMAND_REQUEST                        16              /* SEM event - timeout command request                 */
#define SEM_EVENT__TIMEOUT_DELAY_APN_CONNECT                      17              /* SEM event - timeout after APN connection    expired */
#define SEM_EVENT__TIMEOUT_DELAY_HTTP_REQUEST                     18              /* SEM event - timeout after HTTP request      expired */
#define SEM_EVENT__TIMEOUT_DELAY_APN_DISCONNECT                   19              /* SEM event - timeout after APN disconnection expired */
#define SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS                20              /* SEM event - timeout wait action in progress expired */

/* SEM events strings */
#define SEM_EVENT_STRING__DATA_TX_FORCE                           "SEM_EVENT__DATA_TX_FORCE"
#define SEM_EVENT_STRING__DATA_TX                                 "SEM_EVENT__DATA_TX"
#define SEM_EVENT_STRING__GSM_NET_REGISTERED                      "SEM_EVENT__GSM_NET_REGISTERED"
#define SEM_EVENT_STRING__GSM_NET_NOT_REGISTERED                  "SEM_EVENT__GSM_NET_NOT_REGISTERED"
#define SEM_EVENT_STRING__APN_CONNECT_OK                          "SEM_EVENT__APN_CONNECT_OK"
#define SEM_EVENT_STRING__APN_CONNECT_FAIL                        "SEM_EVENT__APN_CONNECT_FAIL"
#define SEM_EVENT_STRING__APN_HTTP_GET_RX_OK                      "SEM_EVENT__APN_HTTP_GET_RX_OK"
#define SEM_EVENT_STRING__APN_HTTP_GET_RX_FAIL                    "SEM_EVENT__APN_HTTP_GET_RX_FAIL"
#define SEM_EVENT_STRING__APN_HTTP_GET_TX_OK                      "SEM_EVENT__APN_HTTP_GET_TX_OK"
#define SEM_EVENT_STRING__APN_HTTP_GET_TX_FAIL                    "SEM_EVENT__APN_HTTP_GET_TX_FAIL"
#define SEM_EVENT_STRING__APN_HTTP_POST_OK                        "SEM_EVENT__APN_HTTP_POST_OK"
#define SEM_EVENT_STRING__APN_HTTP_POST_FAIL                      "SEM_EVENT__APN_HTTP_POST_FAIL"
#define SEM_EVENT_STRING__APN_DISCONNECT_OK                       "SEM_EVENT__APN_DISCONNECT_OK"
#define SEM_EVENT_STRING__APN_DISCONNECT_FAIL                     "SEM_EVENT__APN_DISCONNECT_FAIL"
#define SEM_EVENT_STRING__APN_CONNECTION_DOWN                     "SEM_EVENT__APN_CONNECTION_DOWN"
#define SEM_EVENT_STRING__TIMEOUT_CHECK_TX                        "SEM_EVENT__TIMEOUT_CHECK_TX"
#define SEM_EVENT_STRING__TIMEOUT_COMMAND_REQUEST                 "SEM_EVENT__TIMEOUT_COMMAND_REQUEST"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_APN_CONNECT               "SEM_EVENT__TIMEOUT_DELAY_APN_CONNECT"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_HTTP_REQUEST              "SEM_EVENT__TIMEOUT_DELAY_HTTP_REQUEST"
#define SEM_EVENT_STRING__TIMEOUT_DELAY_APN_DISCONNECT            "SEM_EVENT__TIMEOUT_DELAY_APN_DISCONNECT"
#define SEM_EVENT_STRING__TIMEOUT_WAIT_ACTION_IN_PROGRESS         "SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS"

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__DATA_TX_FORCE                                (12300 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__DATA_TX                                      (12301 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__GSM_NET_REGISTERED                           (12302 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__GSM_NET_NOT_REGISTERED                       (12303 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_CONNECT_OK                               (12304 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_CONNECT_FAIL                             (12305 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_HTTP_GET_RX_OK                           (12306 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_HTTP_GET_RX_FAIL                         (12307 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_HTTP_GET_TX_OK                           (12308 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_HTTP_GET_TX_FAIL                         (12309 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_HTTP_POST_OK                             (12310 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_HTTP_POST_FAIL                           (12311 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_DISCONNECT_OK                            (12312 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_DISCONNECT_FAIL                          (12313 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__APN_CONNECTION_DOWN                          (12314 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_CHECK_TX                             (12315 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_COMMAND_REQUEST                      (12316 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_APN_CONNECT                    (12317 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_HTTP_REQUEST                   (12318 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_DELAY_APN_DISCONNECT                 (12319 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_WAIT_ACTION_IN_PROGRESS              (12320 | (QL_COMPONENT_APP_START << 16))




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* indication of attempts terminated */
static       bool         TransmissionGprs_AttemptsTerminated = FALSE;

/* indication of transmission in progress */
static       bool         TransmissionGprs_TransmissionInProgress = FALSE;

/* number of attempts */
static       u8           TransmissionGprs_NumOfApnConnection    = 0;
static       u8           TransmissionGprs_NumOfHttpRequest      = 0;
static       u8           TransmissionGprs_NumOfApnDisconnection = 0;

/* indication of msg type to be transmitted */
static       bool         TransmissionGprs_TxSmsAnwserToCrs = FALSE;    // init 1

/* indication of file type to be transmitted */
static       bool         TransmissionGprs_TxFileStatusLog  = FALSE;    // status log file
static       bool         TransmissionGprs_TxFileAlarm      = FALSE;    // alarm      file

/* "command request to do" indication */
static       bool         TransmissionGprs_CommandRequestToDo = TRUE;

/* command ID */
static       s32          TransmissionGprs_CommandId = -1;

/* "file in transmission" info */
static       u8           TransmissionGprs_FileTx_FileDataId;
static       ascii        TransmissionGprs_FileTx_FileName[68 + 1];
static       u8           TransmissionGprs_FileTx_FileVersion;
static       CLOCK__TIME  TransmissionGprs_FileTx_FileTimeStart;
static       CLOCK__TIME  TransmissionGprs_FileTx_FileTimeStop;

/* debug string */
static       ascii        TransmissionGprs_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* timer handlers */
static       ql_timer_t   TransmissionGprs_TimerHandler_TimerCheckTx;                // timer for ...
static       ql_timer_t   TransmissionGprs_TimerHandler_TimerCommandRequest;         // timer for ...
static       ql_timer_t   TransmissionGprs_TimerHandler_TimerDelayApnConnection;     // timer for ...
static       ql_timer_t   TransmissionGprs_TimerHandler_TimerDelayHttpRequest;       // timer for ...
static       ql_timer_t   TransmissionGprs_TimerHandler_TimerDelayApnDisconnection;  // timer for ...
static       ql_timer_t   TransmissionGprs_TimerHandler_TimerWaitActionInProgress;   // timer for ...




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* transmission task */
       void TransmissionGprs_TaskTransmissionGprs(void *argument);

/* info */
       bool TransmissionGprs_IsTransmissionTerminated(void);

/* APN connection/disconnection */
static bool TransmissionGprs_ApnConnection   (void);
static bool TransmissionGprs_ApnDisconnection(void);

/* HTTP GET/POST */
static bool TransmissionGprs_HttpGetRx(u8  msg_tx_type);
static bool TransmissionGprs_HttpGetTx(s32 command_id );
static bool TransmissionGprs_HttpPost (u8 data_tx_type);

/* build msg/data */
static bool TransmissionGprs_BuildSms (u8 msg_tx_type, ascii *ptr_msg_text);
static bool TransmissionGprs_BuildData(u8 data_tx_type);

/* GPRS allowed */
static bool TransmissionGprs_IsGprsAllowed(void);

/* msg to be transmitted */
static bool TransmissionGprs_MsgToBeTransmitted      (u8 *ptr_msg_tx_type , bool check_num_of_attempts);
       bool TransmissionGprs_ExistMsgToBeTransmitted (void);

/* data to be transmitted */
static bool TransmissionGprs_DataToBeTransmitted     (u8 *ptr_data_tx_type, bool check_num_of_attempts);
       bool TransmissionGprs_ExistDataToBeTransmitted(void);

/* command request to be done */
static bool TransmissionGprs_CommandRequestToBeDone  (                      bool check_num_of_attempts);

/* APN and host */
static bool TransmissionGprs_ExistApn        (void);
static bool TransmissionGprs_ExistHost       (void);
static bool TransmissionGprs_ExistHostTx     (void);
static bool TransmissionGprs_ExistHostData   (void);
static bool TransmissionGprs_ExistHostAccount(void);

/* delete/recover data */
static void TransmissionGprs_DeleteMsgGot  (u8 msg_tx_type );
static void TransmissionGprs_RecoverMsgGot (u8 msg_tx_type );
static void TransmissionGprs_DeleteDataGot (u8 data_tx_type);
static void TransmissionGprs_RecoverDataGot(u8 data_tx_type);

/* delete file data */
static void TransmissionGprs_DeleteFileData(u8 data_tx_type);

/* msg to be transmitted */
       void TransmissionGprs_SmsAnswerToCrsToBeTxed(void);

/*---------------------------------------------------------------------------
 * Events for SEM from external
 *---------------------------------------------------------------------------*/
       void TransmissionGprs_Event_DataTxForce            (void);
       void TransmissionGprs_Event_DataTx                 (void);
       void TransmissionGprs_Event_GsmNetworkRegistered   (void);
       void TransmissionGprs_Event_GsmNetworkNotRegistered(void);
       void TransmissionGprs_Event_ApnConnectionOk        (void);
       void TransmissionGprs_Event_ApnConnectionFail      (void);
       void TransmissionGprs_Event_HttpGetRxOk            (void);
       void TransmissionGprs_Event_HttpGetRxFail          (void);
       void TransmissionGprs_Event_HttpGetTxOk            (s32 command_id);
       void TransmissionGprs_Event_HttpGetTxFail          (void);
       void TransmissionGprs_Event_HttpPostOk             (void);
       void TransmissionGprs_Event_HttpPostFail           (int evt_code);
       void TransmissionGprs_Event_ApnDisconnectionOk     (void);
       void TransmissionGprs_Event_ApnDisconnectionFail   (void);
       void TransmissionGprs_Event_ApnConnectionDown      (void);

/*---------------------------------------------------------------------------
 * Timers
 *---------------------------------------------------------------------------*/
/* action on timers */

/* timer for  */
static void TransmissionGprs_TimerCheckTx_Start              (u32 time_value, bool periodic);

/* timer for  */
static void TransmissionGprs_TimerCommandRequest_Start       (u32 time_value, bool periodic);

/* timer for  */
static void TransmissionGprs_TimerDelayApnConnection_Start   (u32 time_value, bool periodic);
static void TransmissionGprs_TimerDelayApnConnection_Stop    (void);

/* timer for  */
static void TransmissionGprs_TimerDelayHttpRequest_Start     (u32 time_value, bool periodic);
static void TransmissionGprs_TimerDelayHttpRequest_Stop      (void);

/* timer for  */
static void TransmissionGprs_TimerDelayApnDisconnection_Start(u32 time_value, bool periodic);
static void TransmissionGprs_TimerDelayApnDisconnection_Stop (void);

/* timer for  */
static void TransmissionGprs_TimerWaitActionInProgress_Start (u32 time_value, bool periodic);
static void TransmissionGprs_TimerWaitActionInProgress_Stop  (void);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void TransmissionGprs_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void TransmissionGprs_AdlCallback_Timer_TimerCheckTx              (void *ptr_context);
static void TransmissionGprs_AdlCallback_Timer_TimerCommandRequest       (void *ptr_context);
static void TransmissionGprs_AdlCallback_Timer_TimerDelayApnConnection   (void *ptr_context);
static void TransmissionGprs_AdlCallback_Timer_TimerDelayHttpRequest     (void *ptr_context);
static void TransmissionGprs_AdlCallback_Timer_TimerDelayApnDisconnection(void *ptr_context);
static void TransmissionGprs_AdlCallback_Timer_TimerWaitActionInProgress (void *ptr_context);




/*=============================================================================
 * Function   : TransmissionGprs_TaskTransmissionGprs
 *
 * Description: Transmission GPRS task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_TaskTransmissionGprs(void *argument)
{
    ql_event_t event;
    QlOSStatus err;

    u8         i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - TransmissionGprs_TaskTransmissionGprs - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* indication of attempts terminated */
    TransmissionGprs_AttemptsTerminated = FALSE;

    /* indication of transmission in progress */
    TransmissionGprs_TransmissionInProgress = FALSE;

    /* number of attempts */
    TransmissionGprs_NumOfApnConnection    = 0;
    TransmissionGprs_NumOfHttpRequest      = 0;
    TransmissionGprs_NumOfApnDisconnection = 0;

    /* indication of data type to be transmitted */
    TransmissionGprs_TxSmsAnwserToCrs = FALSE;

    /* indication of data type to be transmitted */
    TransmissionGprs_TxFileStatusLog  = FALSE;
    TransmissionGprs_TxFileAlarm      = FALSE;

    /* command ID */
    TransmissionGprs_CommandId = -1;

    /* "file in transmission" info */
    TransmissionGprs_FileTx_FileDataId = HTTP__FILE_TYPE__LOG;
    for (i = 0; i < (68 + 1); i++)
        TransmissionGprs_FileTx_FileName[i] = 0x00;
    TransmissionGprs_FileTx_FileVersion = 1;
  //TransmissionGprs_FileTx_FileTimeStart
  //TransmissionGprs_FileTx_FileTimeStop


    /* "command request to do" indication */
    TransmissionGprs_CommandRequestToDo = TRUE;


    /* creation of timer "TimerCheckTx" */
    err = ql_rtos_timer_create(&TransmissionGprs_TimerHandler_TimerCheckTx              , QL_TIMER_IN_SERVICE, TransmissionGprs_AdlCallback_Timer_TimerCheckTx              , NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerCommandRequest" */
    err = ql_rtos_timer_create(&TransmissionGprs_TimerHandler_TimerCommandRequest       , QL_TIMER_IN_SERVICE, TransmissionGprs_AdlCallback_Timer_TimerCommandRequest       , NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerDelayApnConnection" */
    err = ql_rtos_timer_create(&TransmissionGprs_TimerHandler_TimerDelayApnConnection   , QL_TIMER_IN_SERVICE, TransmissionGprs_AdlCallback_Timer_TimerDelayApnConnection   , NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerDelayHttpRequest" */
    err = ql_rtos_timer_create(&TransmissionGprs_TimerHandler_TimerDelayHttpRequest     , QL_TIMER_IN_SERVICE, TransmissionGprs_AdlCallback_Timer_TimerDelayHttpRequest     , NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerDelayApnDisconnection" */
    err = ql_rtos_timer_create(&TransmissionGprs_TimerHandler_TimerDelayApnDisconnection, QL_TIMER_IN_SERVICE, TransmissionGprs_AdlCallback_Timer_TimerDelayApnDisconnection, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerWaitActionInProgress" */
    err = ql_rtos_timer_create(&TransmissionGprs_TimerHandler_TimerWaitActionInProgress , QL_TIMER_IN_SERVICE, TransmissionGprs_AdlCallback_Timer_TimerWaitActionInProgress , NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* start the periodic "command request" timer */
    TransmissionGprs_TimerCommandRequest_Start(TIME_MS__COMMAND_REQUEST, TRUE);

    /* start timer check data to be transmitted */
    TransmissionGprs_TimerCheckTx_Start(TIME_MS__CHECK_DATA_TX, TRUE);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            TransmissionGprs_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_IsTransmissionTerminated
 *
 * Description: verify if the GPRS transmission is terminated
 * Input      : -
 * Output     : - FALSE: the GPRS transmission is not terminated
 *              - TRUE : the GPRS transmission is     terminated
 *=============================================================================*/
bool TransmissionGprs_IsTransmissionTerminated(void)
{
    u8   msg_tx_type;
    u8   data_tx_type;

    bool result;
    bool result_1;
    bool result_2;


    /* verify if a transmission is in progress */
    if (TransmissionGprs_TransmissionInProgress)
    {
        /* a transmission is in progress */
        return FALSE;
    }
    else
    {
        /* a transmission is not in progress */
        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
        {
            /* transmission attempts terminated */
            return TRUE;
        }
    }


    /* verify if GPRS transmission is allowed */
    result = TransmissionGprs_IsGprsAllowed();
    if (!result)
    {
        /* GPRS transmission is not allowed */
        return TRUE;
    }


    /* verify if there are msg to be transmitted by GPRS */
    result_1 = TransmissionGprs_MsgToBeTransmitted (&msg_tx_type , TRUE);
    result_2 = TransmissionGprs_DataToBeTransmitted(&data_tx_type, TRUE);
    if (!(result_1 || result_2))
    {
        /* there are no msg/data to be transmitted by GPRS */
        return TRUE;
    }


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ApnConnection
 *
 * Description: start the APN connection
 * Input      : -
 * Output     : - FALSE: APN connection not physically started
 *              - TRUE : APN connection     physically started
 *=============================================================================*/
static bool TransmissionGprs_ApnConnection(void)
{
    static PHONE__CONFIG__GPRS_APN_PARAMETERS config_gprs_apn_parameters;
           bool                               result;


    /* check if the GPRS APN is configured */
    Phone_Config_ApnParameters_Get(&config_gprs_apn_parameters);
    if (strlen(config_gprs_apn_parameters.apn_server) == 0)
        return FALSE;

    /* start the APN connection */
    result = Boot_GprsConnectionStart();

    return result;
}




/*=============================================================================
 * Function   : TransmissionGprs_ApnDisconnection
 *
 * Description: start the APN disconnection
 * Input      : -
 * Output     : - FALSE: APN disconnection not physically started
 *              - TRUE : APN disconnection     physically started
 *=============================================================================*/
static bool TransmissionGprs_ApnDisconnection(void)
{
    bool result;


    /* start the APN disconnection */
    result = Boot_GprsConnectionStop();

    return result;
}




/*=============================================================================
 * Function   : TransmissionGprs_HttpGetRx
 *
 * Description: transmit a data by HTTP GET "RX"
 * Input      : - data_tx_type: data type to be transmitted
 * Output     : - FALSE: HTTP GET not physically transmitted
 *              - TRUE : HTTP GET     physically transmitted
 *=============================================================================*/
static bool TransmissionGprs_HttpGetRx(u8 msg_tx_type)
{
    static ascii msg_text[160 + 1];

           bool  synchronize;
           bool  result;


    synchronize = FALSE;

    /* build the msg text */
    result = TransmissionGprs_BuildSms(msg_tx_type, msg_text);
    if (!result)
        return FALSE;

    /* do the HTTP GET "RX" */
    result = Http_ClientHttpGetRxRequest(synchronize, msg_text);
    if (!result)
        return FALSE;


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_HttpGetTx
 *
 * Description: transmit a data by HTTP GET "TX"
 * Input      : -  command_id: command ID
 * Output     : - FALSE: HTTP GET not physically transmitted
 *              - TRUE : HTTP GET     physically transmitted
 *=============================================================================*/
static bool TransmissionGprs_HttpGetTx(s32 command_id)
{
    bool synchronize;
    bool result;


    synchronize = FALSE;

    /* do the HTTP GET "TX" */
    result = Http_ClientHttpGetTxRequest(synchronize, command_id);
    if (!result)
        return FALSE;


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_HttpPost
 *
 * Description: transmit a data by HTTP POST
 * Input      : -  data_tx_type: data type to be transmitted
 * Output     : - FALSE: HTTP GET not physically transmitted
 *              - TRUE : HTTP GET     physically transmitted
 *=============================================================================*/
static bool TransmissionGprs_HttpPost(u8 data_tx_type)
{
    bool synchronize;
    bool result;


    synchronize = FALSE;

    /* build the data */
    result = TransmissionGprs_BuildData(data_tx_type);
    if (!result)
        return FALSE;

    /* do the HTTP POST */
    result = Http_ClientHttpPostRequest(synchronize,
                                        TransmissionGprs_FileTx_FileDataId,
                                        TransmissionGprs_FileTx_FileName,
                                        TransmissionGprs_FileTx_FileVersion,
                                        TransmissionGprs_FileTx_FileTimeStart,
                                        TransmissionGprs_FileTx_FileTimeStop);
    if (!result)
        return FALSE;


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_BuildSms
 *
 * Description: build the SMS to be transmitted
 * Input      : - data_tx_type: data type to be transmitted
 * Output     : - FALSE: SMS not built
 *              - TRUE : SMS     built
 *=============================================================================*/
static bool TransmissionGprs_BuildSms(u8 data_tx_type, ascii *ptr_msg_text)
{
    /* SMS text */
    static ascii                                 msg_text[160 + 1];

    /* queue record */
    static QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD queue_sms_answer_to_crs_record;

           bool                                  result;


    /* build the SMS text */
    switch (data_tx_type)
    {
        /* msg tx type - SMS answer to CRS */
        case MSG_TX__SMS_ANSWER_TO_CRS:
            /* get the record to be transmitted from the queue */
            result = Queue_SmsAnswerToCrs_GetRecord    (&queue_sms_answer_to_crs_record);
            if (!result)
                return FALSE;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE);

            /* build the msg text to be transmitted */
            ParserTx_BuildSmsSmsAnswerToCrs((PARSER_TX__SMS_ANSWER_TO_CRS_RECORD   *)&queue_sms_answer_to_crs_record, msg_text);
            break;


        /* msg tx type - unknown */
        default:
            *ptr_msg_text = 0x00;
            return FALSE;
    }


    /* print the msg text */
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Msg text: \"%s\"", msg_text);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* copy the msg text */
    strncpy(ptr_msg_text, msg_text, 160);


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_BuildData
 *
 * Description: build the data to be transmitted
 * Input      : - data_tx_type: data type to be transmitted
 * Output     : - FALSE: data not built
 *              - TRUE : data     built
 *=============================================================================*/
static bool TransmissionGprs_BuildData(u8 data_tx_type)
{
    /* queue record */
    static QUEUE__FILE_STATUS_LOG_QUEUE_RECORD queue_file_status_log_record;
    static QUEUE__FILE_ALARM_QUEUE_RECORD      queue_file_alarm_record;

           bool                                result;


    /* build the file name */
    switch (data_tx_type)
    {
        /* data tx type - file status log */
        case DATA_TX__FILE_STATUS_LOG:
            /* get the record to be transmitted from the queue */
            result = Queue_FileStatusLog_GetRecord(&queue_file_status_log_record);
            if (!result)
                return FALSE;

            /* request the backup to flash objects */
          //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@ GIA' PRESENTE IN MODULO QUEUE.C

            /* check if the file to be transmitted exists */
            result = FileSystem_FileExists(queue_file_status_log_record.log_file_name);
            if (!result)
            {
                /* delete the data from the transmission queue */
                TransmissionGprs_DeleteDataGot(data_tx_type);

                return FALSE;
            }

            /* save the "file in transmission" info */
            TransmissionGprs_FileTx_FileDataId = HTTP__FILE_TYPE__LOG;
            strncpy(TransmissionGprs_FileTx_FileName, queue_file_status_log_record.log_file_name, 68);
            TransmissionGprs_FileTx_FileName[68]  = 0x00;
            TransmissionGprs_FileTx_FileVersion   = queue_file_status_log_record.v;
            TransmissionGprs_FileTx_FileTimeStart = queue_file_status_log_record.start_time;
            TransmissionGprs_FileTx_FileTimeStop  = queue_file_status_log_record.stop_time;
            break;


        /* data tx type - file alarm */
        case DATA_TX__FILE_ALARM:
            /* get the record to be transmitted from the queue */
            result = Queue_FileAlarm_GetRecord(&queue_file_alarm_record);
            if (!result)
                return FALSE;

            /* request the backup to flash objects */
          //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE     );   //@@@ GIA' PRESENTE IN MODULO QUEUE.C

            /* check if the file to be transmitted exists */
            result = FileSystem_FileExists(queue_file_alarm_record.alarm_file_name);
            if (!result)
            {
                /* delete the data from the transmission queue */
                TransmissionGprs_DeleteDataGot(data_tx_type);

                return FALSE;
            }

            /* save the "file in transmission" info */
            TransmissionGprs_FileTx_FileDataId = HTTP__FILE_TYPE__ALARM;
            strncpy(TransmissionGprs_FileTx_FileName, queue_file_alarm_record.alarm_file_name, 68);
            TransmissionGprs_FileTx_FileName[68]  = 0x00;
            TransmissionGprs_FileTx_FileVersion   = queue_file_alarm_record.v;
            TransmissionGprs_FileTx_FileTimeStart = queue_file_alarm_record.start_time;
            TransmissionGprs_FileTx_FileTimeStop  = queue_file_alarm_record.stop_time;
            break;


        /* msg tx type - unknown */
        default:
            return FALSE;
    }


    /* print the file name */
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "File name      : \"%s\"", TransmissionGprs_FileTx_FileName);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the V */
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "File Version   : %d"    , TransmissionGprs_FileTx_FileVersion);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* print the file time start */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "File time start:", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    Clock_PrintTime2(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, 0, 0, &TransmissionGprs_FileTx_FileTimeStart);

    /* print the file time stop */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "File time stop:", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    Clock_PrintTime2(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, 0, 0, &TransmissionGprs_FileTx_FileTimeStop);


    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_IsGprsAllowed
 *
 * Description: verify if GPRS transmission is allowed
 * Input      : -
 * Output     : - FALSE: GPRS transmission not allowed
 *              - TRUE : GPRS transmission     allowed
 *=============================================================================*/
static bool TransmissionGprs_IsGprsAllowed(void)
{
    return TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_MsgToBeTransmitted
 *
 * Description: verify the msg type to be transmitted (if it is present);
 *              it checks:
 *                   - if GPRS is allowed
 *                   - if the number of transmission attempts is terminated
 *                   - if the APN                      exists
 *                   - if the destination host         exists
 *                   - if the destination host account exists
 *              priority of transmission:
 *                   - INIT_1                (highest)
 *                   - ALARM
 *                   - MONTHLY_CONSUMPTION  (lowest)
 * Input      : - ptr_msg_tx_type      : pointer to save the msg type to be transmitted
 *              - check_num_of_attempts: indication if to check the number of attempts
 * Output     : - FALSE: there is no msg type to be transmitted
 *              - TRUE : there is    msg type to be transmitted
 *=============================================================================*/
static bool TransmissionGprs_MsgToBeTransmitted(u8 *ptr_msg_tx_type, bool check_num_of_attempts)
{
  //TRANSMISSION__BLOCK__MIDNIGHT_STATUS block_midnight_status;
  //TRANSMISSION__BLOCK__LOG_STATUS      block_log_status;

    u8                                   num_records;
    u8                                   msg_tx_type;

  //bool                                 tx_sms_answer_to_crs;
  //bool                                 tx_retransmission;

    bool                                 result;
    bool                                 result_1;
    bool                                 result_2;
    bool                                 result_3;


  //tx_sms_answer_to_crs = TransmissionGprs_TxSmsAnwserToCrs;

  //tx_retransmission    = TransmissionGprs_TxRetransmission;


    /* verify if GPRS is allowed */
    result = TransmissionGprs_IsGprsAllowed();
    if (!result)
    {
        /* GPRS not allowed */
        return FALSE;
    }


    /* check if number of attempts is to be checked */
    if (check_num_of_attempts)
    {
        /* number of attempts is to be checked */
        if (TransmissionGprs_AttemptsTerminated)
        {
            /* transmission attempts terminated */
            return FALSE;
        }
    }


    /* verify if there is something to be transmitted */

    /* SMS answer to CRS queue */
  //if (tx_sms_answer_to_crs)   //@@@ TO DO BETTER
  //{
        num_records = Queue_SmsAnswerToCrs_NumRecords();
        if (num_records > 0)
        {
            msg_tx_type = MSG_TX__SMS_ANSWER_TO_CRS;
    
            /* verify if the APN, the destination host and the destination host account exist */
            result_1 = TransmissionGprs_ExistApn();
            result_2 = TransmissionGprs_ExistHost();
            result_3 = TransmissionGprs_ExistHostAccount();
            if (result_1 && result_2 && result_3)
            {
                /* the APN, the destination host and the destination host account exist */
                *ptr_msg_tx_type = msg_tx_type;
                return TRUE;
            }
        }
  //}


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistMsgToBeTransmitted
 *
 * Description: verify if data to be transmitted exists
 * Input      : -
 * Output     : - FALSE: there is no data to be transmitted
 *              - TRUE : there is    data to be transmitted
 *=============================================================================*/
bool TransmissionGprs_ExistMsgToBeTransmitted(void)
{
    //TRANSMISSION__BLOCK__LOG_STATUS block_log_status;

    //u8                              num_records;


    /* verify if there is something to be transmitted */


    /* init 1 */
    //num_records = Queue_Init1_NumRecords();
    //if (num_records > 0)
    //    return TRUE;

    /* log C2 queue */
    //num_records = Queue_LogC2_NumRecords();
    //if (num_records > 0)
    //{
    //    Transmission_BlockLogStatus_Get(&block_log_status);
    //    if (!block_log_status.block_status)
    //        return TRUE;
    //}


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_DataToBeTransmitted
 *
 * Description: verify the data type to be transmitted (if it is present);
 *              it checks:
 *                   - if GPRS is allowed
 *                   - if the number of transmission attempts is terminated
 *                   - if the APN                      exists
 *                   - if the destination host         exists
 *                   - if the destination host account exists
 *              priority of transmission:
 *                   - log               (lowest)
 * Input      : - ptr_data_tx_type     : pointer to save the data type to be transmitted
 *              - check_num_of_attempts: indication if to check the number of attempts
 * Output     : - FALSE: there is no data type to be transmitted
 *              - TRUE : there is    data type to be transmitted
 *=============================================================================*/
static bool TransmissionGprs_DataToBeTransmitted(u8 *ptr_data_tx_type, bool check_num_of_attempts)
{
    u8   num_records;
    u8   data_tx_type;

  //bool tx_file_status_log;
  //bool tx_file_alarm;

    bool result;
    bool result_1;
    bool result_2;
    bool result_3;


  //tx_file_status_log = TransmissionGprs_TxFileStatusLog;
  //tx_file_alarm      = TransmissionGprs_TxFileAlarm;


    /* verify if GPRS is allowed */
    result = TransmissionGprs_IsGprsAllowed();
    if (!result)
    {
        /* GPRS not allowed */
        return FALSE;
    }


    /* check if number of attempts is to be checked */
    if (check_num_of_attempts)
    {
        /* number of attempts is to be checked */
        if (TransmissionGprs_AttemptsTerminated)
        {
            /* transmission attempts terminated */
            return FALSE;
        }
    }


    /* verify if there is something to be transmitted */

    /* file status log */
    //if (tx_file_status_log)
    //{
        num_records = Queue_FileStatusLog_NumRecords();
        if (num_records > 0)
        {
            data_tx_type = DATA_TX__FILE_STATUS_LOG;

            /* verify if the APN, the destination host and the destination host account exist */
            result_1 = TransmissionGprs_ExistApn();
            result_2 = TransmissionGprs_ExistHostData();
            result_3 = TransmissionGprs_ExistHostAccount();
            if (result_1 && result_2 && result_3)
            {
                /* the APN, the destination host and the destination host account exist */
                *ptr_data_tx_type = data_tx_type;
                return TRUE;
            }
        }
    //}


    /* file alarm */
    //if (tx_file_alarm)
    //{
        num_records = Queue_FileAlarm_NumRecords();
        if (num_records > 0)
        {
            data_tx_type = DATA_TX__FILE_ALARM;

            /* verify if the APN, the destination host and the destination host account exist */
            result_1 = TransmissionGprs_ExistApn();
            result_2 = TransmissionGprs_ExistHostData();
            result_3 = TransmissionGprs_ExistHostAccount();
            if (result_1 && result_2 && result_3)
            {
                /* the APN, the destination host and the destination host account exist */
                *ptr_data_tx_type = data_tx_type;
                return TRUE;
            }
        }
    //}


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistDataToBeTransmitted
 *
 * Description: verify if data to be transmitted exists
 * Input      : -
 * Output     : - FALSE: there is no data to be transmitted
 *              - TRUE : there is    data to be transmitted
 *=============================================================================*/
bool TransmissionGprs_ExistDataToBeTransmitted(void)
{
    u8 num_records;


    /* verify if there is something to be transmitted */


    /* file status log */
    num_records = Queue_FileStatusLog_NumRecords();
    if (num_records > 0)
        return TRUE;


    /* file alarm */
    num_records = Queue_FileAlarm_NumRecords();
    if (num_records > 0)
        return TRUE;


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_CommandRequestToBeDone
 *
 * Description: verify if a command request has to be done
 *              it checks:
 *                   - if GPRS is allowed
 *                   - if the number of transmission attempts is terminated
 *                   - if the APN                      exists
 *                   - if the destination host tx      exists
 *                   - if the destination host account exists
 * Input      : - check_num_of_attempts: indication if to check the number of attempts
 * Output     : - FALSE: there is no data type to be transmitted
 *              - TRUE : there is    data type to be transmitted
 *=============================================================================*/
static bool TransmissionGprs_CommandRequestToBeDone(bool check_num_of_attempts)
{
    bool result;
    bool result_1;
    bool result_2;
    bool result_3;


    /* verify if GPRS is allowed */
    result = TransmissionGprs_IsGprsAllowed();
    if (!result)
    {
        /* GPRS not allowed */
        return FALSE;
    }


    /* check if number of attempts is to be checked */
    if (check_num_of_attempts)
    {
        /* number of attempts is to be checked */
        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
        {
            /* transmission attempts terminated */
            return FALSE;
        }
    }


    /* verify if a command request has to be done */
    if (TransmissionGprs_CommandRequestToDo)
    {
        /* verify if the APN, the destination host tx and the destination host account exist */
        result_1 = TransmissionGprs_ExistApn();
        result_2 = TransmissionGprs_ExistHostTx();
        result_3 = TransmissionGprs_ExistHostAccount();
        if (result_1 && result_2 && result_3)
        {
            /* the APN, the destination host tx and the destination host account exist */
            return TRUE;
        }
    }


    return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistApn
 *
 * Description: verify if the APN exists
 * Input      : -
 * Output     : - FALSE: APN does not exist
 *              - TRUE : APN          exists
 *=============================================================================*/
static bool TransmissionGprs_ExistApn(void)
{
    PHONE__CONFIG__GPRS_APN_PARAMETERS config_gprs_apn_parameters;


    /* get the "GPRS APN" configuration */
    Phone_Config_ApnParameters_Get(&config_gprs_apn_parameters);

    if (strlen(config_gprs_apn_parameters.apn_server) > 0)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistHost
 *
 * Description: verify if the host exists
 * Input      : -
 * Output     : - FALSE: host does not exist
 *              - TRUE : host          exists
 *=============================================================================*/
static bool TransmissionGprs_ExistHost(void)
{
    HTTP__CONFIG__HOST config_host;


    /* get the "host" configuration */
    Http_Config_Host_Get(&config_host);

    if (strlen(config_host.host_name) > 0)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistHostTx
 *
 * Description: verify if the host tx exists
 * Input      : -
 * Output     : - FALSE: host does not exist
 *              - TRUE : host          exists
 *=============================================================================*/
static bool TransmissionGprs_ExistHostTx(void)
{
    HTTP__CONFIG__HOST_TX config_host_tx;


    /* get the "host tx" configuration */
    Http_Config_HostTx_Get(&config_host_tx);

    if (strlen(config_host_tx.host_name) > 0)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistHostData
 *
 * Description: verify if the host data exists
 * Input      : -
 * Output     : - FALSE: host data does not exist
 *              - TRUE : host data          exists
 *=============================================================================*/
static bool TransmissionGprs_ExistHostData(void)
{
    HTTP__CONFIG__HOST_DATA config_host_data;


    /* get the "host data" configuration */
    Http_Config_HostData_Get(&config_host_data);

    if (strlen(config_host_data.host_name) > 0)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_ExistHostAccount
 *
 * Description: verify if the host account exists
 * Input      : -
 * Output     : - FALSE: host account does not exist
 *              - TRUE : host account          exists
 *=============================================================================*/
static bool TransmissionGprs_ExistHostAccount(void)
{
    HTTP__CONFIG__HOST_ACCOUNT_2 config_host_account_2;


    /* get the "host acount 2" configuration */
    Http_Config_HostAccount2_Get(&config_host_account_2);

    if (strlen(config_host_account_2.key) > 0)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : TransmissionGprs_DeleteMsgGot
 *
 * Description: delete msg already got from the queue (for the msg type specified)
 * Input      : - msg_tx_type: msg type to be deleted
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_DeleteMsgGot(u8 msg_tx_type)
{
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Delete msg got... (msg type: %d)", msg_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_tx_type)
    {
        /* msg tx type - init 1 */
        case MSG_TX__SMS_ANSWER_TO_CRS:
            Queue_SmsAnswerToCrs_DeleteRecordsGot();
            break;

        /* msg tx type - unknown */
        default:
            break;
    }


    switch (msg_tx_type)
    {
        /* msg tx type - init 1 */
        case MSG_TX__SMS_ANSWER_TO_CRS:
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE);
            break;

        /* msg tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_RecoverMsgGot
 *
 * Description: recover msg already got from the queue (for the msg type specfied)
 * Input      : - msg_tx_type: msg type to be recovered
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_RecoverMsgGot(u8 msg_tx_type)
{
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Recover msg got... (msg type: %d)", msg_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_tx_type)
    {
        /* msg tx type - init 1 */
        case MSG_TX__SMS_ANSWER_TO_CRS:
            Queue_SmsAnswerToCrs_RecoverRecordsGot();
            break;

        /* msg tx type - unknown */
        default:
            break;
    }


    switch (msg_tx_type)
    {
        /* msg tx type - init 1 */
        case MSG_TX__SMS_ANSWER_TO_CRS:
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_TO_CRS_QUEUE);
            break;

        /* msg tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_DeleteDataGot
 *
 * Description: delete data already got from the queue (for the data type specified)
 * Input      : - data_tx_type: data type to be deleted
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_DeleteDataGot(u8 data_tx_type)
{
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Delete data got... (data type: %d)", data_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (data_tx_type)
    {
        /* data tx type - file status log */
        case DATA_TX__FILE_STATUS_LOG:
            Queue_FileStatusLog_DeleteRecordsGot();
            break;

        /* data tx type - file alarm */
        case DATA_TX__FILE_ALARM:
            Queue_FileAlarm_DeleteRecordsGot();
            break;

        /* data tx type - unknown */
        default:
            break;
    }


    switch (data_tx_type)
    {
        /* data tx type - file status log */
        case DATA_TX__FILE_STATUS_LOG:
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);
            break;

        /* data tx type - file alarm */
        case DATA_TX__FILE_ALARM:
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE     );
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_RecoverDataGot
 *
 * Description: recover data already got from the queue (for the data type specified)
 * Input      : - data_tx_type: data type to be recovered
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_RecoverDataGot(u8 data_tx_type)
{
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Recover data got... (data type: %d)", data_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (data_tx_type)
    {
        /* data tx type - file status log */
        case DATA_TX__FILE_STATUS_LOG:
            Queue_FileStatusLog_RecoverRecordsGot();
            break;

        /* data tx type - file alarm */
        case DATA_TX__FILE_ALARM:
            Queue_FileAlarm_RecoverRecordsGot();
            break;

        /* data tx type - unknown */
        default:
            break;
    }


    switch (data_tx_type)
    {
        /* data tx type - file status log */
        case DATA_TX__FILE_STATUS_LOG:
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);
            break;

        /* data tx type - file alarm */
        case DATA_TX__FILE_ALARM:
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE     );
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_DeleteFileData
 *
 * Description: delete file data (for the data type specified)
 * Input      : - data_tx_type: data type to be deleted
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_DeleteFileData(u8 data_tx_type)
{
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Delete file data... (data type: %d)", data_tx_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (data_tx_type)
    {
        /* data tx type - file status log */
        case DATA_TX__FILE_STATUS_LOG:
            LogStatus_Event_LogStatusFileSent(TransmissionGprs_FileTx_FileName);
            break;

        /* data tx type - file alarm */
        case DATA_TX__FILE_ALARM:
            Alarm_Event_AlarmFileSent        (TransmissionGprs_FileTx_FileName);
            break;

        /* data tx type - unknown */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_SmsAnswerToCrsToBeTxed
 *
 * Description: indication of SMS answer to CRS data to be transmitted
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_SmsAnswerToCrsToBeTxed(void)
{
    TransmissionGprs_TxSmsAnwserToCrs = TRUE;
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_DataTxForce
 *
 * Description: signal force data transmission
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_DataTxForce(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__DATA_TX_FORCE;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_DataTx
 *
 * Description: signal presence of data to be transmitted
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_DataTx(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__DATA_TX;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_GsmNetworkRegistered
 *
 * Description: signal GSM network registration
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_GsmNetworkRegistered(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_NET_REGISTERED;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_GsmNetworkNotRegistered
 *
 * Description: signal GSM network deregistration
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_GsmNetworkNotRegistered(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_NET_NOT_REGISTERED;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_ApnConnectionOk
 *
 * Description: signal success of APN connection
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_ApnConnectionOk(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_CONNECT_OK;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_ApnConnectionFail
 *
 * Description: signal failure of APN connection
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_ApnConnectionFail(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_CONNECT_FAIL;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_HttpGetRxOk
 *
 * Description: signal success of HTTP GET "RX"
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_HttpGetRxOk(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_HTTP_GET_RX_OK;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_HttpGetRxFail
 *
 * Description: signal failure of HTTP GET "RX"
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_HttpGetRxFail(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_HTTP_GET_RX_FAIL;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_HttpGetTxOk
 *
 * Description: signal success of HTTP GET "TX"
 * Input      : - command_id: command ID
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_HttpGetTxOk(s32 command_id)
{
    ql_event_t event;
    QlOSStatus err;


    TransmissionGprs_CommandId = command_id;


    event.id = TASK_MSG_ID__APN_HTTP_GET_TX_OK;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_HttpGetTxFail
 *
 * Description: signal failure of HTTP GET "TX"
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_HttpGetTxFail(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_HTTP_GET_TX_FAIL;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_HttpPostOk
 *
 * Description: signal success of HTTP POST
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_HttpPostOk(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_HTTP_POST_OK;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_HttpPostFail
 *
 * Description: signal failure of HTTP POST
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_HttpPostFail(int evt_code)
{
    ql_event_t event;
    QlOSStatus err;


    event.id     = TASK_MSG_ID__APN_HTTP_POST_FAIL;
    event.param1 = (uint32)evt_code;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_ApnDisconnectionOk
 *
 * Description: signal success of APN disconnection
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_ApnDisconnectionOk(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_DISCONNECT_OK;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_ApnDisconnectionFail
 *
 * Description: signal failure of APN disconnection
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_ApnDisconnectionFail(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_DISCONNECT_FAIL;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_Event_ApnConnectionDown
 *
 * Description: signal APN connection down
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGprs_Event_ApnConnectionDown(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__APN_CONNECTION_DOWN;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerCheckTx_Start
 *
 * Description: start the "check tx" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerCheckTx_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Start \"check tx\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionGprs_TimerHandler_TimerCheckTx, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerCommandRequest_Start
 *
 * Description: start the "command request" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerCommandRequest_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Start \"command request\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionGprs_TimerHandler_TimerCommandRequest, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerDelayApnConnection_Start
 *
 * Description: start the "delay APN connection" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerDelayApnConnection_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Start \"delay APN connection\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionGprs_TimerHandler_TimerDelayApnConnection, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerDelayApnConnection_Stop
 *
 * Description: stop the "delay APN connection" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerDelayApnConnection_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Stop \"delay APN connection\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(TransmissionGprs_TimerHandler_TimerDelayApnConnection);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerDelayHttpRequest_Start
 *
 * Description: start the "delay HTTP request" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerDelayHttpRequest_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Start \"delay HTTP request\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionGprs_TimerHandler_TimerDelayHttpRequest, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerDelayHttpRequest_Stop
 *
 * Description: stop the "delay HTTP request" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerDelayHttpRequest_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Stop \"delay HTTP request\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(TransmissionGprs_TimerHandler_TimerDelayHttpRequest);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerDelayApnDisconnection_Start
 *
 * Description: start the "delay APN disconnection" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerDelayApnDisconnection_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Start \"delay APN disconnection\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionGprs_TimerHandler_TimerDelayApnDisconnection, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerDelayApnDisconnection_Stop
 *
 * Description: stop the "delay APN disconnection" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerDelayApnDisconnection_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Stop \"delay APN disconnection\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(TransmissionGprs_TimerHandler_TimerDelayApnDisconnection);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerWaitActionInProgress_Start
 *
 * Description: start the "wait action in progress" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerWaitActionInProgress_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Start \"wait action in progress\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(TransmissionGprs_TimerHandler_TimerWaitActionInProgress, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_TimerWaitActionInProgress_Stop
 *
 * Description: stop the "wait action in progress" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_TimerWaitActionInProgress_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "Stop \"wait action in progress\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(TransmissionGprs_TimerHandler_TimerWaitActionInProgress);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : TransmissionGprs_AdlCallback_Message_TaskMsg
 *
 * Description: - task transmission message callback
 * Input      : - msg_identifier:
 * Output     : -
 *===========================================================================*/
static void TransmissionGprs_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    static       u8     sem_state      = SEM_STATE__IDLE;
    static       bool   gsm_registered = FALSE;
    static       u8     msg_tx_type;
    static       u8     data_tx_type;

                 bool   sms_tx_terminated;

                 bool   msg_to_be_transmitted;
                 bool   data_to_be_transmitted;
                 bool   command_request_to_be_done;

                 int    evt_code;

                 u32    timeout;
                 u32    sem_event;
                 bool   result;
                 u8     i;

    static const ascii *sem_state_strings[] =
    {
        SEM_STATE_STRING__IDLE,
        SEM_STATE_STRING__WAITING_FOR_RESOURCE,
        SEM_STATE_STRING__APN_CONNECT_DELAY,
        SEM_STATE_STRING__APN_CONNECT_IN_PROGRESS,
        SEM_STATE_STRING__HTTP_REQUEST_DELAY,
        SEM_STATE_STRING__HTTP_REQUEST_IN_PROGRESS,
        SEM_STATE_STRING__APN_DISCONNECT_DELAY,
        SEM_STATE_STRING__APN_DISCONNECT_IN_PROGRESS,
    };

    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__DATA_TX_FORCE,
        SEM_EVENT_STRING__DATA_TX,
        SEM_EVENT_STRING__GSM_NET_REGISTERED,
        SEM_EVENT_STRING__GSM_NET_NOT_REGISTERED,
        SEM_EVENT_STRING__APN_CONNECT_OK,
        SEM_EVENT_STRING__APN_CONNECT_FAIL,
        SEM_EVENT_STRING__APN_HTTP_GET_RX_OK,
        SEM_EVENT_STRING__APN_HTTP_GET_RX_FAIL,
        SEM_EVENT_STRING__APN_HTTP_GET_TX_OK,
        SEM_EVENT_STRING__APN_HTTP_GET_TX_FAIL,
        SEM_EVENT_STRING__APN_HTTP_POST_OK,
        SEM_EVENT_STRING__APN_HTTP_POST_FAIL,
        SEM_EVENT_STRING__APN_DISCONNECT_OK,
        SEM_EVENT_STRING__APN_DISCONNECT_FAIL,
        SEM_EVENT_STRING__APN_CONNECTION_DOWN,
        SEM_EVENT_STRING__TIMEOUT_CHECK_TX,
        SEM_EVENT_STRING__TIMEOUT_COMMAND_REQUEST,
        SEM_EVENT_STRING__TIMEOUT_DELAY_APN_CONNECT,
        SEM_EVENT_STRING__TIMEOUT_DELAY_HTTP_REQUEST,
        SEM_EVENT_STRING__TIMEOUT_DELAY_APN_DISCONNECT,
        SEM_EVENT_STRING__TIMEOUT_WAIT_ACTION_IN_PROGRESS,
    };


    /* debug */
    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "CALLBACK     - MESSAGE     - TASK TRANSMISSION - msg_identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


  //sem_event = msg_identifier->id;
    switch (msg_identifier->id)
    {
        case TASK_MSG_ID__DATA_TX_FORCE:
            sem_event = SEM_EVENT__DATA_TX_FORCE;
            break;

        case TASK_MSG_ID__DATA_TX:
            sem_event = SEM_EVENT__DATA_TX;
            break;

        case TASK_MSG_ID__GSM_NET_REGISTERED:
            sem_event = SEM_EVENT__GSM_NET_REGISTERED;
            break;

        case TASK_MSG_ID__GSM_NET_NOT_REGISTERED:
            sem_event = SEM_EVENT__GSM_NET_NOT_REGISTERED;
            break;

        case TASK_MSG_ID__APN_CONNECT_OK:
            sem_event = SEM_EVENT__APN_CONNECT_OK;
            break;

        case TASK_MSG_ID__APN_CONNECT_FAIL:
            sem_event = SEM_EVENT__APN_CONNECT_FAIL;
            break;

        case TASK_MSG_ID__APN_HTTP_GET_RX_OK:
            sem_event = SEM_EVENT__APN_HTTP_GET_RX_OK;
            break;

        case TASK_MSG_ID__APN_HTTP_GET_RX_FAIL:
            sem_event = SEM_EVENT__APN_HTTP_GET_RX_FAIL;
            break;

        case TASK_MSG_ID__APN_HTTP_GET_TX_OK:
            sem_event = SEM_EVENT__APN_HTTP_GET_TX_OK;
            break;

        case TASK_MSG_ID__APN_HTTP_GET_TX_FAIL:
            sem_event = SEM_EVENT__APN_HTTP_GET_TX_FAIL;
            break;

        case TASK_MSG_ID__APN_HTTP_POST_OK:
            sem_event = SEM_EVENT__APN_HTTP_POST_OK;
            break;

        case TASK_MSG_ID__APN_HTTP_POST_FAIL:
            sem_event = SEM_EVENT__APN_HTTP_POST_FAIL;
            break;

        case TASK_MSG_ID__APN_DISCONNECT_OK:
            sem_event = SEM_EVENT__APN_DISCONNECT_OK;
            break;

        case TASK_MSG_ID__APN_DISCONNECT_FAIL:
            sem_event = SEM_EVENT__APN_DISCONNECT_FAIL;
            break;

        case TASK_MSG_ID__APN_CONNECTION_DOWN:
            sem_event = SEM_EVENT__APN_CONNECTION_DOWN;
            break;

        case TASK_MSG_ID__TIMEOUT_CHECK_TX:
            sem_event = SEM_EVENT__TIMEOUT_CHECK_TX;
            break;

        case TASK_MSG_ID__TIMEOUT_COMMAND_REQUEST:
            sem_event = SEM_EVENT__TIMEOUT_COMMAND_REQUEST;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_APN_CONNECT:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_APN_CONNECT;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_HTTP_REQUEST:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_HTTP_REQUEST;
            break;

        case TASK_MSG_ID__TIMEOUT_DELAY_APN_DISCONNECT:
            sem_event = SEM_EVENT__TIMEOUT_DELAY_APN_DISCONNECT;
            break;

        case TASK_MSG_ID__TIMEOUT_WAIT_ACTION_IN_PROGRESS:
            sem_event = SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS;
            break;

        default:
            return;
    }


    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "SEM state: %s", sem_state_strings[sem_state]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "SEM event: %s", sem_event_strings[sem_event]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(TransmissionGprs_DebugString, sizeof(TransmissionGprs_DebugString), "Attempts (APN connection, HTTP, APN disconnection): %d, %d, %d", TransmissionGprs_NumOfApnConnection, TransmissionGprs_NumOfHttpRequest, TransmissionGprs_NumOfApnDisconnection);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, TransmissionGprs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);



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

        /* SEM event - force data transmission */
        /* SEM event - data to be transmitted  */
        case SEM_EVENT__DATA_TX_FORCE:
        case SEM_EVENT__DATA_TX:
            TransmissionGprs_AttemptsTerminated = FALSE;
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
                    /* verify if the SMS transmission is terminated */
                    sms_tx_terminated = TransmissionSms_IsTransmissionTerminated();
                    if (sms_tx_terminated)
                    {
                        /* the SMS transmission is terminated */

                        /* verify if there are msg  to be transmitted */
                        msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                        /* verify if there are data to be transmitted */
                        data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                        /* verify if a command request to be done */
                        command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                        if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                        {
                            /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */
                            if (gsm_registered)
                            {
                                /* phone registered in GSM network */

                                if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                    TransmissionGprs_AttemptsTerminated = TRUE;
                                TransmissionGprs_NumOfApnConnection    = 0;
                                TransmissionGprs_NumOfHttpRequest      = 0;
                                TransmissionGprs_NumOfApnDisconnection = 0;

                                TransmissionGprs_CommandRequestToDo = TRUE;

                                timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_GSM_REG;
                                TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                                sem_state = SEM_STATE__APN_CONNECT_DELAY;
                            }
                            else
                            {
                                /* phone not registered in GSM network */

                                if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                    TransmissionGprs_AttemptsTerminated = TRUE;
                                TransmissionGprs_NumOfApnConnection    = 0;
                                TransmissionGprs_NumOfHttpRequest      = 0;
                                TransmissionGprs_NumOfApnDisconnection = 0;

                                TransmissionGprs_CommandRequestToDo = TRUE;

                                sem_state = SEM_STATE__WAITING_FOR_RESOURCE;
                            }

                            /* indication of transmission in progress */
                            TransmissionGprs_TransmissionInProgress = TRUE;
                        }
                        else
                        {
                            /* there is no data to be transmitted  AND  there is no command request to do */
                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    break;


                /* SEM event - timeout command request */
                case SEM_EVENT__TIMEOUT_COMMAND_REQUEST:
                    TransmissionGprs_CommandRequestToDo = TRUE;
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
                    /* verify if the SMS transmission is terminated */
                    sms_tx_terminated = TransmissionSms_IsTransmissionTerminated();
                    if (sms_tx_terminated)
                    {
                        /* the SMS transmission is terminated */

                        /* verify if there are msg  to be transmitted */
                        msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                        /* verify if there are data to be transmitted */
                        data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                        /* verify if a command request to be done */
                        command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                        if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                        {
                            /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */
                            if (gsm_registered)
                            {
                                /* phone registered in GSM network */

                                timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_GSM_REG;
                                TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                                sem_state = SEM_STATE__APN_CONNECT_DELAY;
                            }
                            else
                            {
                                /* phone not registered in GSM network */

                                sem_state = SEM_STATE__WAITING_FOR_RESOURCE;
                            }
                        }
                        else
                        {
                            /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */
                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - APN connection delay */
        case SEM_STATE__APN_CONNECT_DELAY:
            switch (sem_event)
            {
                /* SEM event - timeout after APN connection expired */
                case SEM_EVENT__TIMEOUT_DELAY_APN_CONNECT:
                    /* verify if the SMS transmission is terminated */
                    sms_tx_terminated = TransmissionSms_IsTransmissionTerminated();
                    if (sms_tx_terminated)
                    {
                        /* the SMS transmission is terminated */

                        /* verify if there are msg  to be transmitted */
                        msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                        /* verify if there are data to be transmitted */
                        data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                        /* verify if a command request to be done */
                        command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                        if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                        {
                            /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */
                            if (gsm_registered)
                            {
                                /* phone registered in GSM network */

                                /* increase the number of APN connection */
                                TransmissionGprs_NumOfApnConnection++;

                                /* APN connection */
                                result = TransmissionGprs_ApnConnection();
                                if (result)
                                {
                                    /* APN connection physically started, we wait for the result */

                                    TransmissionGprs_TimerWaitActionInProgress_Start(TIME_MS__WAIT_APN_CONNECT, FALSE);

                                    sem_state = SEM_STATE__APN_CONNECT_IN_PROGRESS;
                                }
                                else
                                {
                                    /* APN connection physically not started */

                                    /* verify if there are msg  to be transmitted */
                                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                                    /* verify if there are data to be transmitted */
                                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                                    /* verify if a command request to be done */
                                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                                    {
                                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */
                                        timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_CONNECT_FAIL;
                                        TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__APN_CONNECT_DELAY;
                                    }
                                    else
                                    {
                                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */
                                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                            TransmissionGprs_AttemptsTerminated = TRUE;
                                        TransmissionGprs_NumOfApnConnection     = 0;
                                        TransmissionGprs_NumOfHttpRequest       = 0;
                                        TransmissionGprs_NumOfApnDisconnection  = 0;
                                        TransmissionGprs_TransmissionInProgress = FALSE;
                                        TransmissionGprs_CommandRequestToDo     = FALSE;
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
                            /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */
                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* the SMS transmission is not terminated */

                        timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_GSM_REG;
                        TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_CONNECT_DELAY;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - APN connection in progress */
        case SEM_STATE__APN_CONNECT_IN_PROGRESS:
            switch (sem_event)
            {
                /* SEM event - success of APN connection */
                case SEM_EVENT__APN_CONNECT_OK:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    TransmissionGprs_NumOfHttpRequest = 0;

                    timeout = TIME_MS__HTTP_GET_DELAY__AFTER_APN_CONNECT_OK;
                    TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                    sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                    break;


                /* SEM event - failure of APN connection */
                /* SEM event - timeout wait action in progress expired */
                case SEM_EVENT__APN_CONNECT_FAIL:
                case SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS:
                    if (SEM_EVENT__APN_CONNECT_FAIL == SEM_EVENT__APN_CONNECT_FAIL)
                        TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* verify if there are other attempts of APN connection */
                    if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                    {
                        /* there are    other attempts of APN connection */

                        timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_CONNECT_FAIL;
                        TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_CONNECT_DELAY;
                    }
                    else
                    {
                        /* there are no other attempts of APN connection */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* SEM event - APN connection down */
                case SEM_EVENT__APN_CONNECTION_DOWN:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of APN connection */
                        if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN connection */

                            timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                            TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_CONNECT_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of APN connection */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted  AND  there is no command request to do */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - HTTP request delay */
        case SEM_STATE__HTTP_REQUEST_DELAY:
            switch (sem_event)
            {
                /* SEM event - timeout after HTTP request expired */
                case SEM_EVENT__TIMEOUT_DELAY_HTTP_REQUEST:
                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        if (msg_to_be_transmitted)
                        {
                            /* there is msg to be transmitted */

                            /* increase the number of HTTP GET */
                            TransmissionGprs_NumOfHttpRequest++;

                            /* HTTP GET "RX" */
                            result = TransmissionGprs_HttpGetRx(msg_tx_type);
                            if (result)
                            {
                                /* HTTP GET "RX" physically sent, we wait for the result */

                                TransmissionGprs_TimerWaitActionInProgress_Start(TIME_MS__WAIT_HTTP_REQUEST, FALSE);

                                sem_state = SEM_STATE__HTTP_REQUEST_IN_PROGRESS;
                            }
                            else
                            {
                                /* HTTP GET "RX" physically not sent */

                                /* recover the msg from the transmission queue */
                                TransmissionGprs_RecoverMsgGot(msg_tx_type);

                                /* verify if there are msg  to be transmitted */
                                msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                                /* verify if there are data to be transmitted */
                                data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                                /* verify if a command request to be done */
                                command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                                if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                                {
                                    /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                                    /* verify if there are other attempts of HTTP GET */
                                    if (TransmissionGprs_NumOfHttpRequest < NUM_OF_HTTP_REQUEST_ATTEMPTS)
                                    {
                                        /* there are    other attempts of HTTP GET */

                                        timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_FAIL;
                                        TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                                    }
                                    else
                                    {
                                        /* there are no other attempts of HTTP GET */

                                        TransmissionGprs_NumOfApnDisconnection = 0;

                                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                                    }
                                }
                                else
                                {
                                    /* there is no data to be transmitted */

                                    TransmissionGprs_NumOfApnDisconnection = 0;

                                    timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                                    TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                                    sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                                }
                            }
                        }
                        else if (data_to_be_transmitted)
                        {
                            /* there is data to be transmitted */

                            /* increase the number of HTTP request */
                            TransmissionGprs_NumOfHttpRequest++;

                            /* HTTP POST */
                            result = TransmissionGprs_HttpPost(data_tx_type);
                            if (result)
                            {
                                /* HTTP POST physically sent, we wait for the result */

                                TransmissionGprs_TimerWaitActionInProgress_Start(TIME_MS__WAIT_HTTP_REQUEST, FALSE);

                                sem_state = SEM_STATE__HTTP_REQUEST_IN_PROGRESS;
                            }
                            else
                            {
                                /* HTTP POST physically not sent */

                                /* verify if there are msg  to be transmitted */
                                msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                                /* verify if there are data to be transmitted */
                                data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                                /* verify if a command request to be done */
                                command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                                if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                                {
                                    /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                                    /* verify if there are other attempts of HTTP request */
                                    if (TransmissionGprs_NumOfHttpRequest < NUM_OF_HTTP_REQUEST_ATTEMPTS)
                                    {
                                        /* there are    other attempts of HTTP request */

                                        timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_FAIL;
                                        TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                                    }
                                    else
                                    {
                                        /* there are no other attempts of HTTP request */

                                        TransmissionGprs_NumOfApnDisconnection = 0;

                                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                                    }
                                }
                                else
                                {
                                    /* there is no msg to be transmitted */

                                    TransmissionGprs_NumOfApnDisconnection = 0;

                                    timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                                    TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                                    sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                                }
                            }
                        }
                        else if (command_request_to_be_done)
                        {
                            /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                            /* increase the number of HTTP GET */
                            TransmissionGprs_NumOfHttpRequest++;

                            /* HTTP GET "TX" */
                            result = TransmissionGprs_HttpGetTx(TransmissionGprs_CommandId);
                            if (result)
                            {
                                /* HTTP GET "TX" physically sent, we wait for the result */

                                TransmissionGprs_TimerWaitActionInProgress_Start(TIME_MS__WAIT_HTTP_REQUEST, FALSE);

                                sem_state = SEM_STATE__HTTP_REQUEST_IN_PROGRESS;
                            }
                            else
                            {
                                /* HTTP GET "TX" physically not sent */

                                /* verify if there are msg  to be transmitted */
                                msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                                /* verify if there are data to be transmitted */
                                data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                                /* verify if a command request to be done */
                                command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                                if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                                {
                                    /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                                    /* verify if there are other attempts of HTTP GET */
                                    if (TransmissionGprs_NumOfHttpRequest < NUM_OF_HTTP_REQUEST_ATTEMPTS)
                                    {
                                        /* there are    other attempts of HTTP GET */

                                        timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_FAIL;
                                        TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                                    }
                                    else
                                    {
                                        /* there are no other attempts of HTTP GET */

                                        TransmissionGprs_NumOfApnDisconnection = 0;

                                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                                    }
                                }
                                else
                                {
                                    /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                                    TransmissionGprs_NumOfApnDisconnection = 0;

                                    timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                                    TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                                    sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                                }
                            }
                        }
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        TransmissionGprs_NumOfApnDisconnection = 0;

                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                    }
                    break;


                /* SEM event - APN connection down */
                case SEM_EVENT__APN_CONNECTION_DOWN:
                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of APN connection */
                        if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN connection */

                            timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                            TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_CONNECT_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of APN connection */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - HTTP request in progress */
        case SEM_STATE__HTTP_REQUEST_IN_PROGRESS:
            switch (sem_event)
            {
                /* SEM event - success of HTTP GET "RX" */
                case SEM_EVENT__APN_HTTP_GET_RX_OK:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* release HTTP client resources */
                    Http_ClientHttpGetRxRelease();

                    /* delete the msg from the transmission queue */
                    TransmissionGprs_DeleteMsgGot(msg_tx_type);

                    /* reset the number of HTTP GET attempts */
                    TransmissionGprs_NumOfHttpRequest = 0;

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_OK;
                        TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                        sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        TransmissionGprs_NumOfApnConnection = 0;

                        TransmissionGprs_NumOfApnDisconnection = 0;

                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                    }
                    break;


                /* SEM event - success of HTTP GET "TX" */
                case SEM_EVENT__APN_HTTP_GET_TX_OK:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* release HTTP client resources */
                    Http_ClientHttpGetTxRelease();

                    /* reset the number of HTTP GET attempts */
                    TransmissionGprs_NumOfHttpRequest = 0;

                    if (TransmissionGprs_CommandId != -1)
                    {
                        /* a  message has been downloaded */
                        TransmissionGprs_CommandRequestToDo = TRUE;
                    }
                    else
                    {
                        /* no message has been downloaded */
                        TransmissionGprs_CommandRequestToDo = FALSE;
                    }

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_OK;
                        TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                        sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        TransmissionGprs_NumOfApnConnection = 0;

                        TransmissionGprs_NumOfApnDisconnection = 0;

                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                    }
                    break;


                /* SEM event - success of HTTP POST */
                case SEM_EVENT__APN_HTTP_POST_OK:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* release HTTP client resources */
                    Http_ClientHttpPostRelease();

                    /* delete the file data */
                    TransmissionGprs_DeleteFileData(data_tx_type);

                    /* delete the data from the transmission queue */
                    TransmissionGprs_DeleteDataGot(data_tx_type);

                    /* clear the "file in transmission" info */
                    TransmissionGprs_FileTx_FileDataId = HTTP__FILE_TYPE__LOG;
                    for (i = 0; i < (68 + 1); i++)
                        TransmissionGprs_FileTx_FileName[i] = 0x00;
                    TransmissionGprs_FileTx_FileVersion = 1;
                //TransmissionGprs_FileTx_FileDataTime

                    /* reset the number of HTTP request attempts */
                    TransmissionGprs_NumOfHttpRequest = 0;

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , FALSE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, FALSE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               FALSE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_OK;
                        TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                        sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        TransmissionGprs_NumOfApnConnection = 0;

                        TransmissionGprs_NumOfApnDisconnection = 0;

                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                    }
                    break;


                /* SEM event - failure of HTTP GET "RX" */
                /* SEM event - failure of HTTP GET "TX" */
                /* SEM event - failure of HTTP POST */
                /* SEM event - timeout wait action in progress expired */
                case SEM_EVENT__APN_HTTP_GET_RX_FAIL:
                case SEM_EVENT__APN_HTTP_GET_TX_FAIL:
                case SEM_EVENT__APN_HTTP_POST_FAIL:
                case SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS:
                    if (
                        (sem_event == SEM_EVENT__APN_HTTP_GET_RX_FAIL) ||
                        (sem_event == SEM_EVENT__APN_HTTP_GET_TX_FAIL) ||
                        (sem_event == SEM_EVENT__APN_HTTP_POST_FAIL  )
                    )
                    {
                        TransmissionGprs_TimerWaitActionInProgress_Stop();
                    }

                    /* recover the msg/data from the transmission queue */
                    if (sem_event == SEM_EVENT__APN_HTTP_GET_RX_FAIL)
                    {
                        /* release HTTP client resources */
                        Http_ClientHttpGetRxRelease();

                        TransmissionGprs_RecoverMsgGot (msg_tx_type );
                    }
                    if (sem_event == SEM_EVENT__APN_HTTP_GET_TX_FAIL)
                    {
                        /* release HTTP client resources */
                        Http_ClientHttpGetTxRelease();
                    }
                    if (sem_event == SEM_EVENT__APN_HTTP_POST_FAIL  )
                    {
                        /* release HTTP client resources */
                        Http_ClientHttpPostRelease();

                        TransmissionGprs_RecoverDataGot(data_tx_type);

                        /* temporary solution */
                        evt_code = (int)msg_identifier->param1;
                        if (evt_code)
                        {
                            /* restart module */
                            Main_RestartModule();
                        }
                    }
                    if (sem_event == SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS)
                    {
                        /* release HTTP client resources */
                        Http_ClientHttpGetRxRelease();
                        Http_ClientHttpGetTxRelease();
                        Http_ClientHttpPostRelease();

                        TransmissionGprs_RecoverMsgGot (msg_tx_type );
                        TransmissionGprs_RecoverDataGot(data_tx_type);
                    }

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of HTTP GET */
                        if (TransmissionGprs_NumOfHttpRequest < NUM_OF_HTTP_REQUEST_ATTEMPTS)
                        {
                            /* there are    other attempts of HTTP GET */

                            timeout = TIME_MS__HTTP_GET_DELAY__AFTER_HTTP_REQUEST_FAIL;
                            TransmissionGprs_TimerDelayHttpRequest_Start(timeout, FALSE);
                            sem_state = SEM_STATE__HTTP_REQUEST_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of HTTP GET */

                            TransmissionGprs_NumOfApnDisconnection = 0;

                            timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                            TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                        }
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        TransmissionGprs_NumOfApnDisconnection = 0;

                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_LAST_HTTP_REQUEST;
                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                    }
                    break;


                /* SEM event - APN connection down */
                case SEM_EVENT__APN_CONNECTION_DOWN:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of APN connection */
                        if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN connection */

                            timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                            TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_CONNECT_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of APN connection */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - APN disconnection delay */
        case SEM_STATE__APN_DISCONNECT_DELAY:
            switch (sem_event)
            {
                /* SEM event - timeout after APN disconnection expired */
                case SEM_EVENT__TIMEOUT_DELAY_APN_DISCONNECT:
                    /* increase the number of APN disconnection */
                    TransmissionGprs_NumOfApnDisconnection++;

                    /* APN disconnection */
                    result = TransmissionGprs_ApnDisconnection();
                    if (result)
                    {
                        /* APN disconnection physically started, we wait for the result */

                        TransmissionGprs_TimerWaitActionInProgress_Start(TIME_MS__WAIT_APN_DISCONNECT, FALSE);

                        sem_state = SEM_STATE__APN_DISCONNECT_IN_PROGRESS;
                    }
                    else
                    {
                        /* APN disconnection physically not started */

                        /* verify if there are other attempts of APN disconnection */
                        if (TransmissionGprs_NumOfApnDisconnection < NUM_OF_APN_DISCONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN disconnection */

                            timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_APN_DISCONNECT_FAIL;
                            TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                        }
                        else
                        {
                            /* verify if there are msg  to be transmitted */
                            msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                            /* verify if there are data to be transmitted */
                            data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                            /* verify if a command request to be done */
                            command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                            if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                            {
                                /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                                /* verify if there are other attempts of APN connection */
                                if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                                {
                                    /* there are    other attempts of APN connection */

                                    timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                                    TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                                    sem_state = SEM_STATE__APN_CONNECT_DELAY;
                                }
                                else
                                {
                                    /* there are no other attempts of APN connection */

                                    if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                        TransmissionGprs_AttemptsTerminated = TRUE;
                                    TransmissionGprs_NumOfApnConnection     = 0;
                                    TransmissionGprs_NumOfHttpRequest       = 0;
                                    TransmissionGprs_NumOfApnDisconnection  = 0;
                                    TransmissionGprs_TransmissionInProgress = FALSE;
                                    TransmissionGprs_CommandRequestToDo     = FALSE;
                                    sem_state = SEM_STATE__IDLE;
                                }
                            }
                            else
                            {
                                /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                                if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                    TransmissionGprs_AttemptsTerminated = TRUE;
                                TransmissionGprs_NumOfApnConnection     = 0;
                                TransmissionGprs_NumOfHttpRequest       = 0;
                                TransmissionGprs_NumOfApnDisconnection  = 0;
                                TransmissionGprs_TransmissionInProgress = FALSE;
                                TransmissionGprs_CommandRequestToDo     = FALSE;
                                sem_state = SEM_STATE__IDLE;
                            }
                        }
                    }
                    break;


                /* SEM event - APN connection down */
                case SEM_EVENT__APN_CONNECTION_DOWN:
                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of APN connection */
                        if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN connection */

                            timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                            TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_CONNECT_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of APN connection */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted  AND  there is no command request to do */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - APN disconnection in progress */
        case SEM_STATE__APN_DISCONNECT_IN_PROGRESS:
            switch (sem_event)
            {
                /* SEM event - success of APN disconnection */
                case SEM_EVENT__APN_DISCONNECT_OK:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of APN connection */
                        if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN connection */

                            timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                            TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_CONNECT_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of APN connection */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no data to be transmitted  AND  there is no command request to do */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
                        sem_state = SEM_STATE__IDLE;
                    }
                    break;


                /* SEM event - failure of APN disconnection */
                /* SEM event - timeout wait action in progress expired */
                case SEM_EVENT__APN_DISCONNECT_FAIL:
                case SEM_EVENT__TIMEOUT_WAIT_ACTION_IN_PROGRESS:
                    if (sem_event == SEM_EVENT__APN_DISCONNECT_FAIL)
                        TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* verify if there are other attempts of APN disconnection */
                    if (TransmissionGprs_NumOfApnDisconnection < NUM_OF_APN_DISCONNECTION_ATTEMPTS)
                    {
                        /* there are    other attempts of APN disconnection */

                        timeout = TIME_MS__APN_DISCONNECT_DELAY__AFTER_APN_DISCONNECT_FAIL;
                        TransmissionGprs_TimerDelayApnDisconnection_Start(timeout, FALSE);
                        sem_state = SEM_STATE__APN_DISCONNECT_DELAY;
                    }
                    else
                    {
                        /* there are no other attempts of APN disconnection */

                        /* verify if there are msg  to be transmitted */
                        msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                        /* verify if there are data to be transmitted */
                        data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                        /* verify if a command request to be done */
                        command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                        if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                        {
                            /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                            /* verify if there are other attempts of APN connection */
                            if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                            {
                                /* there are    other attempts of APN connection */

                                timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                                TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                                sem_state = SEM_STATE__APN_CONNECT_DELAY;
                            }
                            else
                            {
                                /* there are no other attempts of APN connection */

                                if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                    TransmissionGprs_AttemptsTerminated = TRUE;
                                TransmissionGprs_NumOfApnConnection     = 0;
                                TransmissionGprs_NumOfHttpRequest       = 0;
                                TransmissionGprs_NumOfApnDisconnection  = 0;
                                TransmissionGprs_TransmissionInProgress = FALSE;
                                TransmissionGprs_CommandRequestToDo     = FALSE;
                                sem_state = SEM_STATE__IDLE;
                            }
                        }
                        else
                        {
                            /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    break;


                /* SEM event - APN connection down */
                case SEM_EVENT__APN_CONNECTION_DOWN:
                    TransmissionGprs_TimerWaitActionInProgress_Stop();

                    /* verify if there are msg  to be transmitted */
                    msg_to_be_transmitted      = TransmissionGprs_MsgToBeTransmitted    (&msg_tx_type , TRUE);

                    /* verify if there are data to be transmitted */
                    data_to_be_transmitted     = TransmissionGprs_DataToBeTransmitted   (&data_tx_type, TRUE);

                    /* verify if a command request to be done */
                    command_request_to_be_done = TransmissionGprs_CommandRequestToBeDone(               TRUE);

                    if (msg_to_be_transmitted || data_to_be_transmitted || command_request_to_be_done)
                    {
                        /* there is    msg to be transmitted  OR   there is    data to be transmitted  OR   there is a  command request to do */

                        /* verify if there are other attempts of APN connection */
                        if (TransmissionGprs_NumOfApnConnection < NUM_OF_APN_CONNECTION_ATTEMPTS)
                        {
                            /* there are    other attempts of APN connection */

                            timeout = TIME_MS__APN_CONNECT_DELAY__AFTER_APN_DISCONNECTION;
                            TransmissionGprs_TimerDelayApnConnection_Start(timeout, FALSE);
                            sem_state = SEM_STATE__APN_CONNECT_DELAY;
                        }
                        else
                        {
                            /* there are no other attempts of APN connection */

                            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                                TransmissionGprs_AttemptsTerminated = TRUE;
                            TransmissionGprs_NumOfApnConnection     = 0;
                            TransmissionGprs_NumOfHttpRequest       = 0;
                            TransmissionGprs_NumOfApnDisconnection  = 0;
                            TransmissionGprs_TransmissionInProgress = FALSE;
                            TransmissionGprs_CommandRequestToDo     = FALSE;
                            sem_state = SEM_STATE__IDLE;
                        }
                    }
                    else
                    {
                        /* there is no msg to be transmitted  AND  there is no data to be transmitted  AND  there is no command request to do */

                        if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                            TransmissionGprs_AttemptsTerminated = TRUE;
                        TransmissionGprs_NumOfApnConnection     = 0;
                        TransmissionGprs_NumOfHttpRequest       = 0;
                        TransmissionGprs_NumOfApnDisconnection  = 0;
                        TransmissionGprs_TransmissionInProgress = FALSE;
                        TransmissionGprs_CommandRequestToDo     = FALSE;
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
            TransmissionGprs_TimerDelayApnConnection_Stop();
            TransmissionGprs_TimerDelayHttpRequest_Stop();
            TransmissionGprs_TimerDelayApnDisconnection_Stop();
            TransmissionGprs_TimerWaitActionInProgress_Stop();

            if (TransmissionGprs_NumOfApnConnection >= NUM_OF_APN_CONNECTION_ATTEMPTS)
                TransmissionGprs_AttemptsTerminated = TRUE;
            TransmissionGprs_NumOfApnConnection     = 0;
            TransmissionGprs_NumOfHttpRequest       = 0;
            TransmissionGprs_NumOfApnDisconnection  = 0;

            TransmissionGprs_TransmissionInProgress = FALSE;
            TransmissionGprs_CommandRequestToDo     = FALSE;

            sem_state = SEM_STATE__IDLE;
            break;
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_AdlCallback_Timer_TimerCheckTx
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_AdlCallback_Timer_TimerCheckTx(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - CHECK TX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_CHECK_TX;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_AdlCallback_Timer_TimerCommandRequest
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_AdlCallback_Timer_TimerCommandRequest(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - COMMAND REQUEST", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_COMMAND_REQUEST;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_AdlCallback_Timer_TimerDelayApnConnection
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_AdlCallback_Timer_TimerDelayApnConnection(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - DELAY APN CONNECTION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_APN_CONNECT;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_AdlCallback_Timer_TimerDelayHttpRequest
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_AdlCallback_Timer_TimerDelayHttpRequest(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - DELAY HTTP request", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_HTTP_REQUEST;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_AdlCallback_Timer_TimerDelayApnDisconnection
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_AdlCallback_Timer_TimerDelayApnDisconnection(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - DELAY APN DISCONNECTION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_DELAY_APN_DISCONNECT;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : TransmissionGprs_AdlCallback_Timer_TimerWaitActionInProgress
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void TransmissionGprs_AdlCallback_Timer_TimerWaitActionInProgress(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - WAIT ACTION IN PROGRESS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_WAIT_ACTION_IN_PROGRESS;

    err = ql_rtos_event_send(Boot_TaskRef_TransmissionGprs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
