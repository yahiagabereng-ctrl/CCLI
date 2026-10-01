/*=============================================================================
 * File       :  FV_ACTION.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - FV actions
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *==========================================================================*/
/* standard includes */
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "fv_action.h"
#include "fw_config.h"
#include "input_event.h"
#include "outputs.h"
#include "startup.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* period of backup of autorestore timer (sec) */
#define TIME_MS_ELAPSED_TIME_BACKUP             (1 * 60 * 1000L)

/* task message IDs */
#define TASK_MSG_ID__DETACHMENT                 0      // event - detachment request
#define TASK_MSG_ID__RESTORE                    1      // event - restore    request
#define TASK_MSG_ID__TIMEOUT_AUTORESTORE        2      // event - timeout timer autorestore
#define TASK_MSG_ID__TIMEOUT_AUTORECONNECTION   3      // event - timeout timer autoreconnection

/* auto-reconnection status */
#define STATUS_AUTORECONNECTION_IDLE            0      // auto-reconnection status: idle
#define STATUS_AUTORECONNECTION_DELAY           1      // auto-reconnection status: delay
#define STATUS_AUTORECONNECTION_TIME            2      // auto-reconnection status: time

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                 200


/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define FV_ACTION__MAIL_SIZE                    21

/* message queue size */
#define MESSAGE_QUEUE_SIZE                      5




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                         // message id
    u32 length_data;                   // length of additional data
    u8  data[FV_ACTION__MAIL_SIZE];    //           additional data
} FV_ACTION__MAIL;

/* timer info */
typedef struct
{
    bool status;          // timer status
    bool periodic;        // periodic indication
    u32  time_value;      // timeout value  (ms)
    u32  time_remaining;  // remaining time (ms)
} TIMER_INFO;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* autoreconnection indication */
static       bool                                FvAction_AutoReconnection;                                                  // = FALSE;
static       u8                                  FvAction_AutoReconnectionStatus;                                            // = STATUS_AUTORECONNECTION_IDLE;

/* timer info */
static       TIMER_INFO                          FvAction_TimerAutoRestore_Info;
static       TIMER_INFO                          FvAction_TimerAutoReconnection_Info;

/* CRC variables */
static       u16                                 FvAction_VarCrc;                                                            // = 0x0000;

/* debug string */
static       ascii                               FvAction_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "autorestore" */
static       FV_ACTION__CONFIG__AUTORESTORE      FvAction_Config_AutoRestore;
static const FV_ACTION__CONFIG__AUTORESTORE      FvAction_Config_AutoRestoreDefault =
{
    FALSE,    /* enable status               [OFF/ON] */
    0,        /* wait for autorestore (hour) [0-96  ] */
};


/* configuration - "autoreconnection" */
static       FV_ACTION__CONFIG__AUTORECONNECTION FvAction_Config_AutoReconnection;
static const FV_ACTION__CONFIG__AUTORECONNECTION FvAction_Config_AutoReconnectionDefault =
{
    FALSE,    /* enable status        [OFF/ON] */
    0,        /* delay for OUT2 (sec) [0- 240] */
    0,        /* time  for OUT2 (sec) [0-3600] */
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* message queue handles */
static       ql_queue_t                          FvAction_MessageQueueHandler_MessageQueueEvents;

/* timer handler */
static       ql_timer_t                          FvAction_TimerHandler_TimerAutoRestore;               /* timer for auto restore             */
static       ql_timer_t                          FvAction_TimerHandler_TimerAutoReconnection;          /* timer for auto reconnection        */
static       ql_timer_t                          FvAction_TimerHandler_TimerAutoRestore_Backup;        /* timer for auto restore      backup */
static       ql_timer_t                          FvAction_TimerHandler_TimerAutoReconnection_Backup;   /* timer for auto reconnection backup */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void FvAction_TaskFvAction(void *argument);

/* status variables */
static void FvAction_InitVar        (void);
static u16  FvAction_CalculateVarCrc(void);
       void FvAction_UpdateVarCrc   (void);
       bool FvAction_VerifyVarCrc   (void);

/* task events */
       void FvAction_RequestDetachment(ascii *sms_phone_number);
       void FvAction_RequestRestore   (ascii *sms_phone_number);

/* get/set "autorestore" configuration */
       void FvAction_Config_AutoRestore_GetDefault     (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);
       void FvAction_Config_AutoRestore_Get            (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);
       void FvAction_Config_AutoRestore_Set            (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);
       bool FvAction_Config_AutoRestore_IsValid        (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);

/* get/set "autoreconnection" configuration */
       void FvAction_Config_AutoReconnection_GetDefault(FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);
       void FvAction_Config_AutoReconnection_Get       (FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);
       void FvAction_Config_AutoReconnection_Set       (FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);
       bool FvAction_Config_AutoReconnection_IsValid   (FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void FvAction_TimerAutoRestore_Start            (u32 time_value, bool periodic);
static void FvAction_TimerAutoRestore_Stop             (void);

static void FvAction_TimerAutoReconnection_Start       (u32 time_value, bool periodic);
static void FvAction_TimerAutoReconnection_Stop        (void);

static void FvAction_TimerAutoRestore_Backup_Start     (u32 time_value, bool periodic);
static void FvAction_TimerAutoRestore_Backup_Stop      (void);

static void FvAction_TimerAutoReconnection_Backup_Start(u32 time_value, bool periodic);
static void FvAction_TimerAutoReconnection_Backup_Stop (void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void FvAction_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);

/* timer   callback functions */
static void FvAction_AdlCallback_Timer_TimerAutoRestore            (void *context);
static void FvAction_AdlCallback_Timer_TimerAutoReconnection       (void *context);
static void FvAction_AdlCallback_Timer_TimerAutoRestore_Backup     (void *context);
static void FvAction_AdlCallback_Timer_TimerAutoReconnection_Backup(void *context);




/*=============================================================================
 * Function   : FvAction_TaskFvAction
 *
 * Description: FV action task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FvAction_TaskFvAction(void *argument)
{
    FV_ACTION__MAIL message_received;
    QlOSStatus      err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - FV_ACTION - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&FvAction_MessageQueueHandler_MessageQueueEvents, sizeof(FV_ACTION__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerAutoRestore" */
    err = ql_rtos_timer_create(&FvAction_TimerHandler_TimerAutoRestore, QL_TIMER_IN_SERVICE, FvAction_AdlCallback_Timer_TimerAutoRestore, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerAutoReconnection" */
    err = ql_rtos_timer_create(&FvAction_TimerHandler_TimerAutoReconnection, QL_TIMER_IN_SERVICE, FvAction_AdlCallback_Timer_TimerAutoReconnection, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerAutoRestore_Backup" */
    err = ql_rtos_timer_create(&FvAction_TimerHandler_TimerAutoRestore_Backup, QL_TIMER_IN_SERVICE, FvAction_AdlCallback_Timer_TimerAutoRestore_Backup, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerAutoReconnection_Backup" */
    err = ql_rtos_timer_create(&FvAction_TimerHandler_TimerAutoReconnection_Backup, QL_TIMER_IN_SERVICE, FvAction_AdlCallback_Timer_TimerAutoReconnection_Backup, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* internal status init */
    FvAction_InitVar();


    for (;;)
    {
        err = ql_rtos_queue_wait(FvAction_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(FV_ACTION__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            FvAction_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*=============================================================================
 * Function   : FvAction_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FvAction_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* autoreconnection indication */
        FvAction_AutoReconnection       = FALSE;
        FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;

        /* timer info */
        FvAction_TimerAutoRestore_Info.status              = FALSE;
        FvAction_TimerAutoRestore_Info.periodic            = FALSE;
        FvAction_TimerAutoRestore_Info.time_value          = 0;
        FvAction_TimerAutoRestore_Info.time_remaining      = 0;

        /* timer info */
        FvAction_TimerAutoReconnection_Info.status         = FALSE;
        FvAction_TimerAutoReconnection_Info.periodic       = FALSE;
        FvAction_TimerAutoReconnection_Info.time_value     = 0;
        FvAction_TimerAutoReconnection_Info.time_remaining = 0;


        /**** update the variable CRC ****/
        FvAction_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** outputs ****/
        // NOTHING TO DO
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time

        if (FvAction_TimerAutoRestore_Info.status)
        {
            if (FvAction_TimerAutoRestore_Info.periodic)
            {
                FvAction_TimerAutoRestore_Start       (FvAction_TimerAutoRestore_Info.time_value, TRUE);
                FvAction_TimerAutoRestore_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP              , TRUE);
            }
            else
            {
                if (FvAction_TimerAutoRestore_Info.time_remaining > 0)
                {
                    FvAction_TimerAutoRestore_Start       (FvAction_TimerAutoRestore_Info.time_remaining, FALSE);
                    FvAction_TimerAutoRestore_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                  , TRUE );
                }
            }
        }

        if (FvAction_TimerAutoReconnection_Info.status)
        {
            if (FvAction_TimerAutoReconnection_Info.periodic)
            {
                FvAction_TimerAutoReconnection_Start       (FvAction_TimerAutoReconnection_Info.time_value, TRUE);
                FvAction_TimerAutoReconnection_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                   , TRUE);
            }
            else
            {
                if (FvAction_TimerAutoReconnection_Info.time_remaining > 0)
                {
                    FvAction_TimerAutoReconnection_Start       (FvAction_TimerAutoReconnection_Info.time_remaining, FALSE);
                    FvAction_TimerAutoReconnection_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                       , TRUE );
                }
            }
        }


        /**** outputs ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : FvAction_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 FvAction_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&FvAction_AutoReconnection          , sizeof(FvAction_AutoReconnection          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&FvAction_AutoReconnectionStatus    , sizeof(FvAction_AutoReconnectionStatus    ), crc);

    /* timers info */
    crc = Utility_CalculateCRC16((u8 *)&FvAction_TimerAutoRestore_Info     , sizeof(FvAction_TimerAutoRestore_Info     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&FvAction_TimerAutoReconnection_Info, sizeof(FvAction_TimerAutoReconnection_Info), crc);

    return crc;
}




/*=============================================================================
 * Function   : FvAction_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FvAction_UpdateVarCrc(void)
{
    FvAction_VarCrc = FvAction_CalculateVarCrc();
}




/*=============================================================================
 * Function   : FvAction_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool FvAction_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = FvAction_CalculateVarCrc();

    if (FvAction_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : FvAction_RequestDetachment
 *
 * Description: detachment request
 * Input      : - sms_phone_number: SMS phone number that did the request
 * Output     : -
 *===========================================================================*/
void FvAction_RequestDetachment(ascii *sms_phone_number)
{
    FV_ACTION__MAIL message_to_be_sent;

    QlOSStatus      err;
    u8              i;


    if (strlen(sms_phone_number) > 0)
    {
        if (!Utility_IsPhoneString(sms_phone_number))
            return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__DETACHMENT;
    message_to_be_sent.length_data = 20 + 1;
    for (i = 0; i < 20; i++)
        message_to_be_sent.data[i] = (u8)sms_phone_number[i];
    message_to_be_sent.data[20] = 0x00;

    err = ql_rtos_queue_release(FvAction_MessageQueueHandler_MessageQueueEvents, sizeof(FV_ACTION__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : FvAction_RequestRestore
 *
 * Description: restore request
 * Input      : - sms_phone_number: SMS phone number that did the request
 * Output     : -
 *===========================================================================*/
void FvAction_RequestRestore(ascii *sms_phone_number)
{
    FV_ACTION__MAIL message_to_be_sent;

    QlOSStatus      err;
    u8              i;


    if (strlen(sms_phone_number) > 0)
    {
        if (!Utility_IsPhoneString(sms_phone_number))
            return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__RESTORE;
    message_to_be_sent.length_data = 20 + 1;
    for (i = 0; i < 20; i++)
        message_to_be_sent.data[i] = (u8)sms_phone_number[i];
    message_to_be_sent.data[20] = 0x00;

    err = ql_rtos_queue_release(FvAction_MessageQueueHandler_MessageQueueEvents, sizeof(FV_ACTION__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : FvAction_Config_AutoRestore_GetDefault
 *
 * Description: get the default "autorestore" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FvAction_Config_AutoRestore_GetDefault(FV_ACTION__CONFIG__AUTORESTORE *ptr_data)
{
    *ptr_data = FvAction_Config_AutoRestoreDefault;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoRestore_Get
 *
 * Description: get the "autorestore" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FvAction_Config_AutoRestore_Get(FV_ACTION__CONFIG__AUTORESTORE *ptr_data)
{
    *ptr_data = FvAction_Config_AutoRestore;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoRestore_Set
 *
 * Description: set the "autorestore" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FvAction_Config_AutoRestore_Set(FV_ACTION__CONFIG__AUTORESTORE *ptr_data)
{
    FvAction_Config_AutoRestore = *ptr_data;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoRestore_IsValid
 *
 * Description: check if the "autorestore" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FvAction_Config_AutoRestore_IsValid(FV_ACTION__CONFIG__AUTORESTORE *ptr_data)
{
    if (ptr_data->wait > 96)
        return FALSE;

    if (ptr_data->status)
    {
        if (ptr_data->wait == 0)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoReconnection_GetDefault
 *
 * Description: get the default "autoreconnection" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FvAction_Config_AutoReconnection_GetDefault(FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data)
{
    *ptr_data = FvAction_Config_AutoReconnectionDefault;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoReconnection_Get
 *
 * Description: get the "autoreconnection" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FvAction_Config_AutoReconnection_Get(FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data)
{
    *ptr_data = FvAction_Config_AutoReconnection;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoReconnection_Set
 *
 * Description: set the "autoreconnection" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FvAction_Config_AutoReconnection_Set(FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data)
{
    FvAction_Config_AutoReconnection = *ptr_data;
}




/*===========================================================================
 * Function   : FvAction_Config_AutoReconnection_IsValid
 *
 * Description: check if the "autoreconnection" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FvAction_Config_AutoReconnection_IsValid(FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data)
{
    if (ptr_data->delay >  240)
        return FALSE;

    if (ptr_data->time  > 3600)
        return FALSE;


    if (ptr_data->status)
    {
        if (ptr_data->time == 0)
            return FALSE;
    }


    return TRUE;
}




/*=============================================================================
 * Function   : FvAction_TimerAutoRestore_Start
 *
 * Description: start the "autorestore" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoRestore_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Start \"autorestore\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FvAction_TimerHandler_TimerAutoRestore, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        FvAction_TimerAutoRestore_Info.status         = TRUE;
        FvAction_TimerAutoRestore_Info.periodic       = periodic;
        FvAction_TimerAutoRestore_Info.time_value     = time_value;
        FvAction_TimerAutoRestore_Info.time_remaining = time_value;

        FvAction_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoRestore_Stop
 *
 * Description: stop the "autorestore" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoRestore_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Stop \"autorestore\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FvAction_TimerHandler_TimerAutoRestore);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FvAction_DebugString, sizeof(FvAction_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, FvAction_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        FvAction_TimerAutoRestore_Info.status         = FALSE;
        FvAction_TimerAutoRestore_Info.periodic       = FALSE;
        FvAction_TimerAutoRestore_Info.time_value     = 0;
        FvAction_TimerAutoRestore_Info.time_remaining = 0;

        FvAction_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoReconnection_Start
 *
 * Description: start the "autoreconnection" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoReconnection_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Start \"autoreconnection\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FvAction_TimerHandler_TimerAutoReconnection, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        FvAction_TimerAutoReconnection_Info.status         = TRUE;
        FvAction_TimerAutoReconnection_Info.periodic       = periodic;
        FvAction_TimerAutoReconnection_Info.time_value     = time_value;
        FvAction_TimerAutoReconnection_Info.time_remaining = time_value;

        FvAction_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoReconnection_Stop
 *
 * Description: stop the "autoreconnection" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoReconnection_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Stop \"autoreconnection\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FvAction_TimerHandler_TimerAutoReconnection);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FvAction_DebugString, sizeof(FvAction_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, FvAction_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        FvAction_TimerAutoReconnection_Info.status         = FALSE;
        FvAction_TimerAutoReconnection_Info.periodic       = FALSE;
        FvAction_TimerAutoReconnection_Info.time_value     = 0;
        FvAction_TimerAutoReconnection_Info.time_remaining = 0;

        FvAction_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoRestore_Backup_Start
 *
 * Description: start the "autorestore backup" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoRestore_Backup_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Start \"autorestore backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FvAction_TimerHandler_TimerAutoRestore_Backup, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoRestore_Backup_Stop
 *
 * Description: stop the "autorestore backup" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoRestore_Backup_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Stop \"autorestore backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FvAction_TimerHandler_TimerAutoRestore_Backup);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FvAction_DebugString, sizeof(FvAction_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, FvAction_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoReconnection_Backup_Start
 *
 * Description: start the "autoreconnection backup" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoReconnection_Backup_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Start \"autoreconnection backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(FvAction_TimerHandler_TimerAutoReconnection_Backup, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FvAction_TimerAutoReconnection_Backup_Stop
 *
 * Description: stop the "autoreconnection backup" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void FvAction_TimerAutoReconnection_Backup_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "Stop \"autoreconnection backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(FvAction_TimerHandler_TimerAutoReconnection_Backup);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(FvAction_DebugString, sizeof(FvAction_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, FvAction_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : FvAction_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - msg_identifier:
 *              - source        :
 *              - length        :
 *              - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void FvAction_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    bool   old_auto_reconnection;
    u8     old_auto_reconnection_status;

    bool   update_var_crc;

    bool   auto_reconnection_stop;

    bool   input_status_expected;
    bool   input_status;
    bool   in1;
    bool   in2;
    ascii *ptr_sms_phone_number;

    bool   result;

    /* event parameters */
    ascii  sms_phone_number[20 + 1];

    u8     i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    /* avoid compiler warning maybe-uninitialized */
    auto_reconnection_stop = FALSE;

    /* debug */
    snprintf(FvAction_DebugString, sizeof(FvAction_DebugString), "CALLBACK     - MESSAGE     - TASK FV ACTION - msg identifier: %lu, length: %lu", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, FvAction_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the data length */
    switch (msg_identifier)
    {
        // event - detachment request
        // event - restore    request
        case TASK_MSG_ID__DETACHMENT:
        case TASK_MSG_ID__RESTORE:
            if (length != (20 + 1))
                return;
            break;

        // event - timeout timer autorestore
        // event - timeout timer autoreconnection
        case TASK_MSG_ID__TIMEOUT_AUTORESTORE:
        case TASK_MSG_ID__TIMEOUT_AUTORECONNECTION:
            if (length != 0)
                return;
            break;

        /* unknown message identifier */
        default:
            return;
    }



    old_auto_reconnection        = FvAction_AutoReconnection;
    old_auto_reconnection_status = FvAction_AutoReconnectionStatus;



    switch (msg_identifier)
    {
        // event - detachment request
        case TASK_MSG_ID__DETACHMENT:
            /* extract the parameters */
            for (i = 0; i <= 20; i++)
                sms_phone_number[i] = (ascii)*(ptr_data + i);


            /* enable output 1 */
            result = Outputs_Event_ManualInt_On();

            /* signal the expected event on the IN1 input */
            input_status_expected = TRUE;
            input_status          = FALSE;   // expected input status: CLOSE
            in1                   = TRUE;
            in2                   = FALSE;
            ptr_sms_phone_number  = sms_phone_number;
            InputEvent_Event_InputStatusExpected(input_status_expected, input_status, in1, in2, ptr_sms_phone_number);


            /* start "autorestore" timer (only if it enabled) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
            if (FvAction_Config_AutoRestore.status)
            {
                FvAction_TimerAutoRestore_Stop();
                FvAction_TimerAutoRestore_Backup_Stop();
                FvAction_TimerAutoRestore_Start       (((u32)FvAction_Config_AutoRestore.wait * 3600L * 1000L), FALSE);
                FvAction_TimerAutoRestore_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                            , TRUE );
            }
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
            if (FvAction_Config_AutoRestore.status)
            {
                FvAction_TimerAutoRestore_Stop();
                FvAction_TimerAutoRestore_Backup_Stop();
                FvAction_TimerAutoRestore_Start       (((u32)FvAction_Config_AutoRestore.wait *   60L * 1000L), FALSE);
                FvAction_TimerAutoRestore_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                            , TRUE );
            }
#endif


            /* reset the auto-reconnection (if it is in progress) */
            auto_reconnection_stop = TRUE;
            break;


        // event - restore request
        case TASK_MSG_ID__RESTORE:
            /* extract the parameters */
            for (i = 0; i <= 20; i++)
                sms_phone_number[i] = (ascii)*(ptr_data + i);


            /* stop "autorestore" timer */
            FvAction_TimerAutoRestore_Stop();
            FvAction_TimerAutoRestore_Backup_Stop();


            /* disable output 1 */
            result = Outputs_Event_ManualInt_Off();

            /* signal the expected event on the IN1 input */
            input_status_expected = TRUE;
            input_status          = TRUE;   // expected input status: OPEN
            in1                   = TRUE;
            in2                   = FALSE;
            ptr_sms_phone_number  = sms_phone_number;
            InputEvent_Event_InputStatusExpected(input_status_expected, input_status, in1, in2, ptr_sms_phone_number);


            /* reset the auto-reconnection (if it is in progress) */
            auto_reconnection_stop = TRUE;

            /* do the auto-reconnection (only if it enabled) */
            if (FvAction_Config_AutoReconnection.status)
            {
                FvAction_AutoReconnection       = TRUE;
                auto_reconnection_stop          = FALSE;
                FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
            }
            break;


        // event - timeout timer autorestore
        case TASK_MSG_ID__TIMEOUT_AUTORESTORE:
            FvAction_TimerAutoRestore_Backup_Stop();

            if (FvAction_Config_AutoRestore.status)
            {
                /* autorestore is still enabled */

                /* disable output 1 */
                result = Outputs_Event_ManualInt_Off();

                /* signal the expected event on the IN1 input */
                input_status_expected = TRUE;
                input_status          = TRUE;   // expected input status: OPEN
                in1                   = TRUE;
                in2                   = FALSE;
                ptr_sms_phone_number  = sms_phone_number;
                InputEvent_Event_InputStatusExpected(input_status_expected, input_status, in1, in2, ptr_sms_phone_number);


                /* reset the auto-reconnection (if it is in progress) */
                auto_reconnection_stop = TRUE;

                /* do the auto-reconnection (only if it enabled) */
                if (FvAction_Config_AutoReconnection.status)
                {
                    FvAction_AutoReconnection       = TRUE;
                    auto_reconnection_stop          = FALSE;
                    FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
                }
            }
            break;


        // unknown or other event
        default:
            break;
    }



    if (FvAction_AutoReconnection)
    {
        if (auto_reconnection_stop)
        {
            /* stop "autoreconnection" timer */
            FvAction_TimerAutoReconnection_Stop();
            FvAction_TimerAutoReconnection_Backup_Stop();

            /* disable output 2 */
            result = Outputs_Event_ManualExt_Off();

            FvAction_AutoReconnection       = FALSE;
            auto_reconnection_stop          = FALSE;
            FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
        }
        else
        {
            switch (FvAction_AutoReconnectionStatus)
            {
                /* auto-reconnection status: idle */
                case STATUS_AUTORECONNECTION_IDLE:
                    if (FvAction_Config_AutoReconnection.delay > 0)
                    {
                        /* start "autoreconnection" timer */
                        FvAction_TimerAutoReconnection_Start       (((u32)FvAction_Config_AutoReconnection.delay * 1000L), FALSE);
                        FvAction_TimerAutoReconnection_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                          , TRUE );

                        FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_DELAY;
                    }
                    else
                    {
                        if (FvAction_Config_AutoReconnection.time)
                        {
                            /* enable output 2 */
                            result = Outputs_Event_ManualExt_On();

                            /* start "autoreconnection" timer */
                            FvAction_TimerAutoReconnection_Start       (((u32)FvAction_Config_AutoReconnection.time  * 1000L), FALSE);
                            FvAction_TimerAutoReconnection_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                          , TRUE );

                            FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_TIME;
                        }
                        else
                        {
                            FvAction_AutoReconnection       = FALSE;
                            auto_reconnection_stop          = FALSE;
                            FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
                        }
                    }
                    break;


                /* auto-reconnection status: delay */
                case STATUS_AUTORECONNECTION_DELAY:
                    switch (msg_identifier)
                    {
                        // event - timeout timer autoreconnection
                        case TASK_MSG_ID__TIMEOUT_AUTORECONNECTION:
                            FvAction_TimerAutoReconnection_Backup_Stop();

                            if (FvAction_Config_AutoReconnection.time)
                            {
                                /* enable output 2 */
                                result = Outputs_Event_ManualExt_On();

                                /* start "autoreconnection" timer */
                                FvAction_TimerAutoReconnection_Start       (((u32)FvAction_Config_AutoReconnection.time * 1000L), FALSE);
                                FvAction_TimerAutoReconnection_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                         , TRUE );

                                FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_TIME;
                            }
                            else
                            {
                                FvAction_AutoReconnection       = FALSE;
                                auto_reconnection_stop          = FALSE;
                                FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
                            }
                            break;
                    }
                    break;


                /* auto-reconnection status: time */
                case STATUS_AUTORECONNECTION_TIME:
                    switch (msg_identifier)
                    {
                        // event - timeout timer autoreconnection
                        case TASK_MSG_ID__TIMEOUT_AUTORECONNECTION:
                            FvAction_TimerAutoReconnection_Backup_Stop();

                            /* disable output 2 */
                            result = Outputs_Event_ManualExt_Off();

                            FvAction_AutoReconnection       = FALSE;
                            auto_reconnection_stop          = FALSE;
                            FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
                            break;
                    }
                    break;


                // unknown or other event
                default:
                    break;
            }
        }
    }
    else
    {
        FvAction_AutoReconnection       = FALSE;
        auto_reconnection_stop          = FALSE;
        FvAction_AutoReconnectionStatus = STATUS_AUTORECONNECTION_IDLE;
    }



    update_var_crc = FALSE;

    if (
         (FvAction_AutoReconnection       != old_auto_reconnection       ) ||
         (FvAction_AutoReconnectionStatus != old_auto_reconnection_status)
       )
    {
        update_var_crc = TRUE;
    }

    if (update_var_crc)
        FvAction_UpdateVarCrc();
}




/*=============================================================================
 * Function   : FvAction_AdlCallback_Timer_TimerAutoRestore
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FvAction_AdlCallback_Timer_TimerAutoRestore(void *context)
{
    FV_ACTION__MAIL message_to_be_sent;
    QlOSStatus      err;
    
    u8              i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - AUTORESTORE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (FvAction_TimerAutoRestore_Info.periodic)
    {
      //FvAction_TimerAutoRestore_Info.status         = TRUE;
      //FvAction_TimerAutoRestore_Info.periodic       = TRUE;
      //FvAction_TimerAutoRestore_Info.time_value     = time_value;
        FvAction_TimerAutoRestore_Info.time_remaining = FvAction_TimerAutoRestore_Info.time_value;

        FvAction_UpdateVarCrc();
    }
    else
    {
        FvAction_TimerAutoRestore_Info.status         = FALSE;
        FvAction_TimerAutoRestore_Info.periodic       = FALSE;
        FvAction_TimerAutoRestore_Info.time_value     = 0;
        FvAction_TimerAutoRestore_Info.time_remaining = 0;

        FvAction_UpdateVarCrc();
    }


    message_to_be_sent.event       = TASK_MSG_ID__TIMEOUT_AUTORESTORE;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < FV_ACTION__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(FvAction_MessageQueueHandler_MessageQueueEvents, sizeof(FV_ACTION__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FvAction_AdlCallback_Timer_TimerAutoReconnection
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FvAction_AdlCallback_Timer_TimerAutoReconnection(void *context)
{
    FV_ACTION__MAIL message_to_be_sent;
    QlOSStatus      err;
    
    u8              i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - AUTORECONNECTION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (FvAction_TimerAutoReconnection_Info.periodic)
    {
      //FvAction_TimerAutoReconnection_Info.status         = TRUE;
      //FvAction_TimerAutoReconnection_Info.periodic       = TRUE;
      //FvAction_TimerAutoReconnection_Info.time_value     = time_value;
        FvAction_TimerAutoReconnection_Info.time_remaining = FvAction_TimerAutoReconnection_Info.time_value;

        FvAction_UpdateVarCrc();
    }
    else
    {
        FvAction_TimerAutoReconnection_Info.status         = FALSE;
        FvAction_TimerAutoReconnection_Info.periodic       = FALSE;
        FvAction_TimerAutoReconnection_Info.time_value     = 0;
        FvAction_TimerAutoReconnection_Info.time_remaining = 0;

        FvAction_UpdateVarCrc();
    }


    message_to_be_sent.event       = TASK_MSG_ID__TIMEOUT_AUTORECONNECTION;
    message_to_be_sent.length_data = 0;
    for (i = 0; i < FV_ACTION__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(FvAction_MessageQueueHandler_MessageQueueEvents, sizeof(FV_ACTION__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : FvAction_AdlCallback_Timer_TimerAutoRestore_Backup
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FvAction_AdlCallback_Timer_TimerAutoRestore_Backup(void *context)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - AUTORESTORE BACKUP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (FvAction_TimerAutoRestore_Info.status)
    {
        if (FvAction_TimerAutoRestore_Info.time_remaining >= TIME_MS_ELAPSED_TIME_BACKUP)
            FvAction_TimerAutoRestore_Info.time_remaining -= TIME_MS_ELAPSED_TIME_BACKUP;
        else
            FvAction_TimerAutoRestore_Info.time_remaining  = 0;

        FvAction_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : FvAction_AdlCallback_Timer_TimerAutoReconnection_Backup
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void FvAction_AdlCallback_Timer_TimerAutoReconnection_Backup(void *context)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FV_ACTION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - AUTORECONNECTION BACKUP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (FvAction_TimerAutoReconnection_Info.status)
    {
        if (FvAction_TimerAutoReconnection_Info.time_remaining >= TIME_MS_ELAPSED_TIME_BACKUP)
            FvAction_TimerAutoReconnection_Info.time_remaining -= TIME_MS_ELAPSED_TIME_BACKUP;
        else
            FvAction_TimerAutoReconnection_Info.time_remaining  = 0;

        FvAction_UpdateVarCrc();
    }
}
