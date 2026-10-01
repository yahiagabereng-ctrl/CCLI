/*=============================================================================
 * File       :  REGULATION.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - temperature regulation
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/


//@@@ SISTEMARE LE CHIAMATE "ProgramFlash_RequestWriteBackup"


/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 * Definitions:
 *   - T    : current  temperature                 (�C)
 *   - T_reg:        temperature to be regulated (�C)
 *   - H    : temperature hysteresis               (�C)  [= 0.5 �C]
 *
 * Temperature regulation rules:
 *   - "winter" mode:
 *        - output on  when:T <= (T_reg - H)   [e.g. T_reg = 20.0�C  --->  T <= 19.5 �C]
 *        - output off when:T >= (T_reg    )   [e.g. T_reg = 20.0�C  --->  T >= 20.0 �C]
 *   - "summer" mode:
 *        - output on  when:T >= (T_reg + H)   [e.g. T_reg = 20.0�C  --->  T >= 20.5 �C]
 *        - output off when:T <= (T_reg    )   [e.g. T_reg = 20.0�C  --->  T <= 20.0 �C]
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "drvtemperature.h"
#include "fw_config.h"
#include "mode.h"
#include "regulation.h"
#include "startup.h"
#include "temperature.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                                200

/* temperature status */
#define TEMPERATURE__OK                                        0     /* status - temperature regular                                                                       */
#define TEMPERATURE__TO_BE_REGULATED                           1     /* status - temperature to be regulated (below("winter") / above("summer") the regulated temperature) */


/*---------------------------------------------------------------------------
 * Temperature
 *---------------------------------------------------------------------------*/
/* hysteresis of temperature (/10 �C) */
#define HYSTERESIS_TEMPERATURE                                 5     /* hysteresis of temperature (/10 �C) */

/* number of conscutive temperature not available */
#define NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE           3


/*---------------------------------------------------------------------------
 * Timeout (sec)
 *---------------------------------------------------------------------------*/
/* duration before changing temperature status (sec) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TIME_SEC_TEMP_STATUS_CHANGE                            (60 + 1)
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TIME_SEC_TEMP_STATUS_CHANGE                            (10 + 1)
#endif


/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__TEMPERATURE_REG_STATUS__OFF                 0     /* SEM state - temperature regulation status - off        */
#define SEM_STATE__TEMPERATURE_REG_STATUS__ON                  1     /* SEM state - temperature regulation status - on         */
#define SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION          2     /* SEM state - temperature regulation status - regulation */

/* SEM state strings */
#define SEM_STATE_STRING__TEMPERATURE_REG_STATUS__OFF          "SEM_STATE__TEMPERATURE_REG_STATUS__OFF       "
#define SEM_STATE_STRING__TEMPERATURE_REG_STATUS__ON           "SEM_STATE__TEMPERATURE_REG_STATUS__ON        "
#define SEM_STATE_STRING__TEMPERATURE_REG_STATUS__REGULATION   "SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION"


/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__NEW_TEMPERATURE_INT                         0     /* SEM event - new internal temperature available           */
#define SEM_EVENT__NEW_TEMPERATURE_EXT                         1     /* SEM event - new external temperature available           */
#define SEM_EVENT__REGULATION_TEMP_INT__OFF                    2     /* SEM event - internal temperature regulation - off        */
#define SEM_EVENT__REGULATION_TEMP_INT__ON                     3     /* SEM event - internal temperature regulation - on         */
#define SEM_EVENT__REGULATION_TEMP_INT__REGULATION             4     /* SEM event - internal temperature regulation - regulation */
#define SEM_EVENT__REGULATION_TEMP_EXT__OFF                    5     /* SEM event - external temperature regulation - off        */
#define SEM_EVENT__REGULATION_TEMP_EXT__ON                     6     /* SEM event - external temperature regulation - on         */
#define SEM_EVENT__REGULATION_TEMP_EXT__REGULATION             7     /* SEM event - external temperature regulation - regulation */
#define SEM_EVENT__TIMEOUT_T_INT_START                         8     /* SEM event - timeout internal temperature start           */
#define SEM_EVENT__TIMEOUT_T_INT_STOP                          9     /* SEM event - timeout internal temperature stop            */
#define SEM_EVENT__TIMEOUT_T_EXT_START                         10    /* SEM event - timeout external temperature start           */
#define SEM_EVENT__TIMEOUT_T_EXT_STOP                          11    /* SEM event - timeout external temperature stop            */
#define SEM_EVENT__OUTPUTS_UNBLOCK                             12    /* SEM event - outputs unblock                              */
#define SEM_EVENT__OUTPUTS_BLOCK                               13    /* SEM event - outputs block                                */
#define SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT1                  14    /* SEM event - active status OUT1 configuration changed     */
#define SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT2                  15    /* SEM event - active status OUT2 configuration changed     */

/* SEM events strings */
#define SEM_EVENT_STRING__NEW_TEMPERATURE_INT                  "SEM_EVENT__NEW_TEMPERATURE_INT            "
#define SEM_EVENT_STRING__NEW_TEMPERATURE_EXT                  "SEM_EVENT__NEW_TEMPERATURE_EXT            "
#define SEM_EVENT_STRING__REGULATION_TEMP_INT__OFF             "SEM_EVENT__REGULATION_TEMP_INT__OFF       "
#define SEM_EVENT_STRING__REGULATION_TEMP_INT__ON              "SEM_EVENT__REGULATION_TEMP_INT__ON        "
#define SEM_EVENT_STRING__REGULATION_TEMP_INT__REGULATION      "SEM_EVENT__REGULATION_TEMP_INT__REGULATION"
#define SEM_EVENT_STRING__REGULATION_TEMP_EXT__OFF             "SEM_EVENT__REGULATION_TEMP_EXT__OFF       "
#define SEM_EVENT_STRING__REGULATION_TEMP_EXT__ON              "SEM_EVENT__REGULATION_TEMP_EXT__ON        "
#define SEM_EVENT_STRING__REGULATION_TEMP_EXT__REGULATION      "SEM_EVENT__REGULATION_TEMP_EXT__REGULATION"
#define SEM_EVENT_STRING__TIMEOUT_T_INT_START                  "SEM_EVENT__TIMEOUT_T_INT_START            "
#define SEM_EVENT_STRING__TIMEOUT_T_INT_STOP                   "SEM_EVENT__TIMEOUT_T_INT_STOP             "
#define SEM_EVENT_STRING__TIMEOUT_T_EXT_START                  "SEM_EVENT__TIMEOUT_T_EXT_START            "
#define SEM_EVENT_STRING__TIMEOUT_T_EXT_STOP                   "SEM_EVENT__TIMEOUT_T_EXT_STOP             "
#define SEM_EVENT_STRING__OUTPUTS_UNBLOCK                      "SEM_EVENT__OUTPUTS_UNBLOCK                "
#define SEM_EVENT_STRING__OUTPUTS_BLOCK                        "SEM_EVENT__OUTPUTS_BLOCK                  "
#define SEM_EVENT_STRING__CHANGED_ACTIVE_STATUS_OUT1           "SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT1     "
#define SEM_EVENT_STRING__CHANGED_ACTIVE_STATUS_OUT2           "SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT2     "

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__NEW_TEMPERATURE_INT              (12000 | (QL_COMPONENT_APP_START << 16))    /* event - new internal temperature available           */
#define TASK_MSG_ID__NEW_TEMPERATURE_EXT              (12001 | (QL_COMPONENT_APP_START << 16))    /* event - new external temperature available           */
#define TASK_MSG_ID__REGULATION_TEMP_INT__OFF         (12002 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation - off        */
#define TASK_MSG_ID__REGULATION_TEMP_INT__ON          (12003 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation - on         */
#define TASK_MSG_ID__REGULATION_TEMP_INT__REGULATION  (12004 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation - regulation */
#define TASK_MSG_ID__REGULATION_TEMP_EXT__OFF         (12005 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation - off        */
#define TASK_MSG_ID__REGULATION_TEMP_EXT__ON          (12006 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation - on         */
#define TASK_MSG_ID__REGULATION_TEMP_EXT__REGULATION  (12007 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation - regulation */
#define TASK_MSG_ID__TIMEOUT_T_INT_START              (12008 | (QL_COMPONENT_APP_START << 16))    /* event - timeout internal temperature start           */
#define TASK_MSG_ID__TIMEOUT_T_INT_STOP               (12009 | (QL_COMPONENT_APP_START << 16))    /* event - timeout internal temperature stop            */
#define TASK_MSG_ID__TIMEOUT_T_EXT_START              (12010 | (QL_COMPONENT_APP_START << 16))    /* event - timeout external temperature start           */
#define TASK_MSG_ID__TIMEOUT_T_EXT_STOP               (12011 | (QL_COMPONENT_APP_START << 16))    /* event - timeout external temperature stop            */
#define TASK_MSG_ID__OUTPUTS_UNBLOCK                  (12012 | (QL_COMPONENT_APP_START << 16))    /* event - outputs unblock                              */
#define TASK_MSG_ID__OUTPUTS_BLOCK                    (12013 | (QL_COMPONENT_APP_START << 16))    /* event - outputs block                                */
#define TASK_MSG_ID__CHANGED_ACTIVE_STATUS_OUT1       (12014 | (QL_COMPONENT_APP_START << 16))    /* event - active status OUT1 configuration changed     */
#define TASK_MSG_ID__CHANGED_ACTIVE_STATUS_OUT2       (12015 | (QL_COMPONENT_APP_START << 16))    /* event - active status OUT2 configuration changed     */




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
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
/* temperature regulation status (internal and external) */
static       u8                                     Regulation_SemStateInt;                             // = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
static       u8                                     Regulation_SemStateExt;                             // = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;

/* regulation temperatures (/10 �C) */
static       s16                                    Regulation_RegTemperatureInt;                       // = 0;
static       s16                                    Regulation_RegTemperatureExt;                       // = 0;

/* current    temperatures (/10 �C) */
static       bool                                   Regulation_CurrentTemperatureAvailableInt;          // = FALSE;
static       bool                                   Regulation_CurrentTemperatureAvailableExt;          // = FALSE;
static       u8                                     Regulation_NumOfCurrentTemperatureNotAvailableInt;  // = 0;
static       u8                                     Regulation_NumOfCurrentTemperatureNotAvailableExt;  // = 0;
static       s16                                    Regulation_CurrentTemperatureInt;                   // = 0;
static       s16                                    Regulation_CurrentTemperatureExt;                   // = 0;

/* outputs status (logical status) */
static       bool                                   Regulation_StatusLogicalOut1;                       // = FALSE;
static       bool                                   Regulation_StatusLogicalOut2;                       // = FALSE;

/* outputs status (physical expected status) */
static       bool                                   Regulation_StatusPhysicalExpectedlOut1;             // = FALSE;
static       bool                                   Regulation_StatusPhysicalExpectedlOut2;             // = FALSE;

/* outputs block indication */
static       bool                                   Regulation_OutputsBlocked;                          // = FALSE;

/* timer info */
static       TIMER_INFO                             Regulation_TimerTemperatureIntStart_Info;
static       TIMER_INFO                             Regulation_TimerTemperatureIntStop_Info;
static       TIMER_INFO                             Regulation_TimerTemperatureExtStart_Info;
static       TIMER_INFO                             Regulation_TimerTemperatureExtStop_Info;

/* CRC variables */
static       u16                                    Regulation_VarCrc;                                  // = 0x0000;

/* debug string */
static       ascii                                  Regulation_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - "active status OUT1" */
static       REGULATION__CONFIG__ACTIVE_STATUS_OUT1 Regulation_Config_ActiveStatusOut1;
static const REGULATION__CONFIG__ACTIVE_STATUS_OUT1 Regulation_Config_ActiveStatusOut1Default =
{
    TRUE,    /* active status [FALSE: relay de-energized, TRUE: relay energized] */
};


/* configuration - "active status OUT2" */
static       REGULATION__CONFIG__ACTIVE_STATUS_OUT2 Regulation_Config_ActiveStatusOut2;
static const REGULATION__CONFIG__ACTIVE_STATUS_OUT2 Regulation_Config_ActiveStatusOut2Default =
{
    TRUE,    /* active status [FALSE: relay de-energized, TRUE: relay energized] */
};


/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - internal temperature status */
static       REGULATION__STATUS__TEMPERATURE_INT    Regulation_Status_TemperatureInt;                   // =
                                                                                                        // {
                                                                                                        //     TEMPERATURE__OK,  /* internal temperature status             */
                                                                                                        //     FALSE,            /* indication of status change start timer */
                                                                                                        //     FALSE,            /* indication of status change stop  timer */
                                                                                                        // };
static const REGULATION__STATUS__TEMPERATURE_INT    Regulation_Status_TemperatureIntDefault =
{
    TEMPERATURE__OK,  /* internal temperature status             */
    FALSE,            /* indication of status change start timer */
    FALSE,            /* indication of status change stop  timer */
};


/* status - external temperature status */
static       REGULATION__STATUS__TEMPERATURE_EXT    Regulation_Status_TemperatureExt;                   // =
                                                                                                        // {
                                                                                                        //     TEMPERATURE__OK,  /* external temperature status             */
                                                                                                        //     FALSE,            /* indication of status change start timer */
                                                                                                        //     FALSE,            /* indication of status change stop  timer */
                                                                                                        // };
static const REGULATION__STATUS__TEMPERATURE_EXT    Regulation_Status_TemperatureExtDefault =
{
    TEMPERATURE__OK,  /* external temperature status             */
    FALSE,            /* indication of status change start timer */
    FALSE,            /* indication of status change stop  timer */
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                             Regulation_TimerHandler_TimerTemperatureIntStart;  /* timer for internal temperature start */
static       ql_timer_t                             Regulation_TimerHandler_TimerTemperatureIntStop;   /* timer for internal temperature stop  */
static       ql_timer_t                             Regulation_TimerHandler_TimerTemperatureExtStart;  /* timer for external temperature start */
static       ql_timer_t                             Regulation_TimerHandler_TimerTemperatureExtStop;   /* timer for external temperature stop  */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Regulation_TaskRegulation(void *argument);

/* status variables */
static void Regulation_InitVar        (void);
static u16  Regulation_CalculateVarCrc(void);
       void Regulation_UpdateVarCrc   (void);
       bool Regulation_VerifyVarCrc   (void);

/* temperature regulation */
static s8   Regulation_TemperatureRegulation(s16 current_temperature    , s16 regulated_temperature    , bool winter_mode    );

/* temperature regulation */
static void Regulation_TemperatureInt       (s16 current_temperature_int, s16 regulated_temperature_int, bool winter_mode_int);
static void Regulation_TemperatureExt       (s16 current_temperature_ext, s16 regulated_temperature_ext, bool winter_mode_ext);

/* outputs status (logical status) */
       bool Regulation_StatusLogicalOutputInt(void);
       bool Regulation_StatusLogicalOutputExt(void);

/* outputs status (expected physical status) */
       bool Regulation_StatusPhysicalExpectedOutputInt(void);
       bool Regulation_StatusPhysicalExpectedOutputExt(void);


/*-----------------------------------------------------------------------------
 * task events
 *-----------------------------------------------------------------------------*/
/* new available temperatures */
static void Regulation_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext);

/* internal temperature regulation */
       bool Regulation_RegulationTemperatureInt_Off       (void);
       bool Regulation_RegulationTemperatureInt_On        (void);
       bool Regulation_RegulationTemperatureInt_Regulation(s16 temperature);

/* external temperature regulation */
       bool Regulation_RegulationTemperatureExt_Off       (void);
       bool Regulation_RegulationTemperatureExt_On        (void);
       bool Regulation_RegulationTemperatureExt_Regulation(s16 temperature);

/* outputs block/unblock */
       void Regulation_OutputsBlock  (void);
       void Regulation_OutputsUnblock(void);

/* new active status output configuration */
       void Regulation_NewConfigActiveStatusOut1(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_config_new, REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_config_old);
       void Regulation_NewConfigActiveStatusOut2(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_config_new, REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_config_old);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "active status OUT1" configuration */
       void Regulation_Config_ActiveStatusOut1_GetDefault(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);
       void Regulation_Config_ActiveStatusOut1_Get       (REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);
       void Regulation_Config_ActiveStatusOut1_Set       (REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);
       bool Regulation_Config_ActiveStatusOut1_IsValid   (REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);

/* get/set "active status OUT2" configuration */
       void Regulation_Config_ActiveStatusOut2_GetDefault(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);
       void Regulation_Config_ActiveStatusOut2_Get       (REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);
       void Regulation_Config_ActiveStatusOut2_Set       (REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);
       bool Regulation_Config_ActiveStatusOut2_IsValid   (REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature status" */
       void Regulation_Status_TemperatureInt_GetDefault(REGULATION__STATUS__TEMPERATURE_INT *ptr_data);
       void Regulation_Status_TemperatureInt_Get       (REGULATION__STATUS__TEMPERATURE_INT *ptr_data);
       void Regulation_Status_TemperatureInt_Set       (REGULATION__STATUS__TEMPERATURE_INT *ptr_data);

/* get/set "external temperature status" */
       void Regulation_Status_TemperatureExt_GetDefault(REGULATION__STATUS__TEMPERATURE_EXT *ptr_data);
       void Regulation_Status_TemperatureExt_Get       (REGULATION__STATUS__TEMPERATURE_EXT *ptr_data);
       void Regulation_Status_TemperatureExt_Set       (REGULATION__STATUS__TEMPERATURE_EXT *ptr_data);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void Regulation_TimerTemperatureIntStart_Start(u32 time_value, bool periodic);
static void Regulation_TimerTemperatureIntStart_Stop (void);

static void Regulation_TimerTemperatureIntStop_Start (u32 time_value, bool periodic);
static void Regulation_TimerTemperatureIntStop_Stop  (void);

static void Regulation_TimerTemperatureExtStart_Start(u32 time_value, bool periodic);
static void Regulation_TimerTemperatureExtStart_Stop (void);

static void Regulation_TimerTemperatureExtStop_Start (u32 time_value, bool periodic);
static void Regulation_TimerTemperatureExtStop_Stop  (void);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Regulation_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void Regulation_AdlCallback_Timer_TimerTemperatureIntStart(void *context);
static void Regulation_AdlCallback_Timer_TimerTemperatureIntStop (void *context);
static void Regulation_AdlCallback_Timer_TimerTemperatureExtStart(void *context);
static void Regulation_AdlCallback_Timer_TimerTemperatureExtStop (void *context);




/*=============================================================================
 * Function   : Regulation_TaskRegulation
 *
 * Description: temperature regulation task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Regulation_TaskRegulation(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - REGULATION - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* creation of timer "TimerTemperatureIntStart" */
    err = ql_rtos_timer_create(&Regulation_TimerHandler_TimerTemperatureIntStart, QL_TIMER_IN_SERVICE, Regulation_AdlCallback_Timer_TimerTemperatureIntStart, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureIntStop" */
    err = ql_rtos_timer_create(&Regulation_TimerHandler_TimerTemperatureIntStop, QL_TIMER_IN_SERVICE, Regulation_AdlCallback_Timer_TimerTemperatureIntStop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureExtStart" */
    err = ql_rtos_timer_create(&Regulation_TimerHandler_TimerTemperatureExtStart, QL_TIMER_IN_SERVICE, Regulation_AdlCallback_Timer_TimerTemperatureExtStart, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerTemperatureExtStop" */
    err = ql_rtos_timer_create(&Regulation_TimerHandler_TimerTemperatureExtStop, QL_TIMER_IN_SERVICE, Regulation_AdlCallback_Timer_TimerTemperatureExtStop, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* internal status init */
    Regulation_InitVar();


    /* registered the function for signal of new available temperatures */
    DrvTemperature_RegisterSignalNewTemperatures(Regulation_NewTemperatures);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Regulation_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : Regulation_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Regulation_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* temperature regulation status (internal and external) */
        Regulation_SemStateInt = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
        Regulation_SemStateExt = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;

        /* regulation temperatures (/10 �C) */
        Regulation_RegTemperatureInt = 0;
        Regulation_RegTemperatureExt = 0;

        /* current    temperatures (/10 �C) */
        Regulation_CurrentTemperatureAvailableInt         = FALSE;
        Regulation_CurrentTemperatureAvailableExt         = FALSE;
        Regulation_NumOfCurrentTemperatureNotAvailableInt = 0;
        Regulation_NumOfCurrentTemperatureNotAvailableExt = 0;
        Regulation_CurrentTemperatureInt                  = 0;
        Regulation_CurrentTemperatureExt                  = 0;

        /* outputs status (logical status) */
        Regulation_StatusLogicalOut1 = FALSE;
        Regulation_StatusLogicalOut2 = FALSE;

        /* outputs status (physical expected status) */
        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
        Regulation_StatusPhysicalExpectedlOut2 = FALSE;

        /* outputs block indication */
        Regulation_OutputsBlocked = FALSE;

        /* status - internal temperature status */
        Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__OK;
        Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
        Regulation_Status_TemperatureInt.status_change_timer_stop_int  = FALSE;

        /* status - external temperature status */
        Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__OK;
        Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
        Regulation_Status_TemperatureExt.status_change_timer_stop_ext  = FALSE;

        /* timer info */
        Regulation_TimerTemperatureIntStart_Info.status         = FALSE;
        Regulation_TimerTemperatureIntStart_Info.periodic       = FALSE;
        Regulation_TimerTemperatureIntStart_Info.time_value     = 0;
        Regulation_TimerTemperatureIntStart_Info.time_remaining = 0;

        /* timer info */
        Regulation_TimerTemperatureIntStop_Info.status          = FALSE;
        Regulation_TimerTemperatureIntStop_Info.periodic        = FALSE;
        Regulation_TimerTemperatureIntStop_Info.time_value      = 0;
        Regulation_TimerTemperatureIntStop_Info.time_remaining  = 0;

        /* timer info */
        Regulation_TimerTemperatureExtStart_Info.status         = FALSE;
        Regulation_TimerTemperatureExtStart_Info.periodic       = FALSE;
        Regulation_TimerTemperatureExtStart_Info.time_value     = 0;
        Regulation_TimerTemperatureExtStart_Info.time_remaining = 0;

        /* timer info */
        Regulation_TimerTemperatureExtStop_Info.status          = FALSE;
        Regulation_TimerTemperatureExtStop_Info.periodic        = FALSE;
        Regulation_TimerTemperatureExtStop_Info.time_value      = 0;
        Regulation_TimerTemperatureExtStop_Info.time_remaining  = 0;


        /**** update the variable CRC ****/
        Regulation_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** outputs ****/
        /* disable output OUT1 */
        // NOTE: Commented because the HW REV01 leaves the OUT1 on at the startup if it was on when power supply has been removed
      //DrvGpio_DisactivateOut1();       /* disactivate digital output OUT1 */

        /* disable output OUT2 */
        // NOTE: Commented because the HW REV01 leaves the OUT2 on at the startup if it was on when power supply has been removed
      //DrvGpio_DisactivateOut2();       /* disactivate digital output OUT2 */
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time

        if (Regulation_TimerTemperatureIntStart_Info.status)
        {
            if (Regulation_TimerTemperatureIntStart_Info.periodic)
            {
                Regulation_TimerTemperatureIntStart_Start       (Regulation_TimerTemperatureIntStart_Info.time_value, TRUE);
              //Regulation_TimerTemperatureIntStart_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                        , TRUE);
            }
            else
            {
                if (Regulation_TimerTemperatureIntStart_Info.time_remaining > 0)
                {
                    Regulation_TimerTemperatureIntStart_Start       (Regulation_TimerTemperatureIntStart_Info.time_remaining, FALSE);
                  //Regulation_TimerTemperatureIntStart_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                            , TRUE );
                }
            }
        }

        if (Regulation_TimerTemperatureIntStop_Info.status)
        {
            if (Regulation_TimerTemperatureIntStop_Info.periodic)
            {
                Regulation_TimerTemperatureIntStop_Start       (Regulation_TimerTemperatureIntStop_Info.time_value, TRUE);
              //Regulation_TimerTemperatureIntStop_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                       , TRUE);
            }
            else
            {
                if (Regulation_TimerTemperatureIntStop_Info.time_remaining > 0)
                {
                    Regulation_TimerTemperatureIntStop_Start       (Regulation_TimerTemperatureIntStop_Info.time_remaining, FALSE);
                  //Regulation_TimerTemperatureIntStop_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                           , TRUE );
                }
            }
        }

        if (Regulation_TimerTemperatureExtStart_Info.status)
        {
            if (Regulation_TimerTemperatureExtStart_Info.periodic)
            {
                Regulation_TimerTemperatureExtStart_Start       (Regulation_TimerTemperatureExtStart_Info.time_value, TRUE);
              //Regulation_TimerTemperatureExtStart_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                        , TRUE);
            }
            else
            {
                if (Regulation_TimerTemperatureExtStart_Info.time_remaining > 0)
                {
                    Regulation_TimerTemperatureExtStart_Start       (Regulation_TimerTemperatureExtStart_Info.time_remaining, FALSE);
                  //Regulation_TimerTemperatureExtStart_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                            , TRUE );
                }
            }
        }

        if (Regulation_TimerTemperatureExtStop_Info.status)
        {
            if (Regulation_TimerTemperatureExtStop_Info.periodic)
            {
                Regulation_TimerTemperatureExtStop_Start       (Regulation_TimerTemperatureExtStop_Info.time_value, TRUE);
              //Regulation_TimerTemperatureExtStop_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                       , TRUE);
            }
            else
            {
                if (Regulation_TimerTemperatureExtStop_Info.time_remaining > 0)
                {
                    Regulation_TimerTemperatureExtStop_Start       (Regulation_TimerTemperatureExtStop_Info.time_remaining, FALSE);
                  //Regulation_TimerTemperatureExtStop_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP                           , TRUE );
                }
            }
        }


        /**** outputs ****/
        // NOTHING TO DO
   }
}




/*=============================================================================
 * Function   : Regulation_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 Regulation_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&Regulation_SemStateInt                           , sizeof(Regulation_SemStateInt                           ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_SemStateExt                           , sizeof(Regulation_SemStateExt                           ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_RegTemperatureInt                     , sizeof(Regulation_RegTemperatureInt                     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_RegTemperatureExt                     , sizeof(Regulation_RegTemperatureExt                     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_CurrentTemperatureAvailableInt        , sizeof(Regulation_CurrentTemperatureAvailableInt        ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_CurrentTemperatureAvailableExt        , sizeof(Regulation_CurrentTemperatureAvailableExt        ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_NumOfCurrentTemperatureNotAvailableInt, sizeof(Regulation_NumOfCurrentTemperatureNotAvailableInt), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_NumOfCurrentTemperatureNotAvailableExt, sizeof(Regulation_NumOfCurrentTemperatureNotAvailableExt), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_CurrentTemperatureInt                 , sizeof(Regulation_CurrentTemperatureInt                 ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_CurrentTemperatureExt                 , sizeof(Regulation_CurrentTemperatureExt                 ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_StatusLogicalOut1                     , sizeof(Regulation_StatusLogicalOut1                     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_StatusLogicalOut2                     , sizeof(Regulation_StatusLogicalOut2                     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_StatusPhysicalExpectedlOut1           , sizeof(Regulation_StatusPhysicalExpectedlOut1           ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_StatusPhysicalExpectedlOut2           , sizeof(Regulation_StatusPhysicalExpectedlOut2           ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_OutputsBlocked                        , sizeof(Regulation_OutputsBlocked                        ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_Status_TemperatureInt                 , sizeof(Regulation_Status_TemperatureInt                 ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_Status_TemperatureExt                 , sizeof(Regulation_Status_TemperatureExt                 ), crc);

    /* timers info */
    crc = Utility_CalculateCRC16((u8 *)&Regulation_TimerTemperatureIntStart_Info         , sizeof(Regulation_TimerTemperatureIntStart_Info         ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_TimerTemperatureIntStop_Info          , sizeof(Regulation_TimerTemperatureIntStop_Info          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_TimerTemperatureExtStart_Info         , sizeof(Regulation_TimerTemperatureExtStart_Info         ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Regulation_TimerTemperatureExtStop_Info          , sizeof(Regulation_TimerTemperatureExtStop_Info          ), crc);

    return crc;
}




/*=============================================================================
 * Function   : Regulation_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Regulation_UpdateVarCrc(void)
{
    Regulation_VarCrc = Regulation_CalculateVarCrc();
}




/*=============================================================================
 * Function   : Regulation_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool Regulation_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = Regulation_CalculateVarCrc();

    if (Regulation_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : Regulation_TemperatureRegulation
 *
 * Description: depending on:
 *                 - current   temperature
 *                 - regulated temperature
 *                 - mode ("winter" or "summer")
 *               it checks if it would be necessary to enable or disable the output
 *               in order to regulate the temperature
 * Input      : - current_temperature  : current   temperature (/10 �C)
 *              - regulated_temperature: regulated temperature (/10 �C)
 *              - winter_mode          : indication of "winter" mode
 * Output     : - -1: it would be necessary to disable the output
 *              -  0: nothing to do (the output can stay unchanged)
 *              - +1: it would be necessary to enable  the output
 *===========================================================================*/
static s8 Regulation_TemperatureRegulation(s16 current_temperature, s16 regulated_temperature, bool winter_mode)
{
    if (winter_mode)
    {
        /* "winter" mode */

        if      (current_temperature <= (regulated_temperature - HYSTERESIS_TEMPERATURE))
        {
            /* T <= (T_reg - H) */
            return +1;   // it is necessary to enable  the output
        }
        else if (current_temperature >= (regulated_temperature                         ))
        {
            /* T >= T_reg */
            return -1;   // it is necessary to disable the output
        }
        else
        {
            /* (T_reg - H) < T < T_reg */
            return  0;   // nothing to do (the output stays unchanged)
        }
    }
    else
    {
        /* "summer" mode */

        if      (current_temperature >= (regulated_temperature + HYSTERESIS_TEMPERATURE))
        {
            /* T >= (T_reg + H) */
            return +1;   // it is necessary to enable  the output
        }
        else if (current_temperature <= (regulated_temperature                         ))
        {
            /* T <= T_reg */
            return -1;   // it is necessary to disable the output
        }
        else
        {
            /* T_reg < (T_reg + H) < T */
            return  0;   // nothing to do (the output stays unchanged)
        }
    }
}




/*=============================================================================
 * Function   : Regulation_TemperatureInt
 *
 * Description: verify the internal temperature regulation
 * Input      : - current_temperature_int  : internal current   temperature (/10 �C)
 *              - regulated_temperature_int: internal regulated temperature (/10 �C)
 *              - winter_mode_int          : internal indication of "winter" mode
 * Output     : -
 *=============================================================================*/
static void Regulation_TemperatureInt(s16 current_temperature_int, s16 regulated_temperature_int, bool winter_mode_int)
{
    s8 result;


    /* verify temperature values */
    if (!Temperature_IsValidTemperature(current_temperature_int  ))
        return;
    if (!Temperature_IsValidTemperature(regulated_temperature_int))
        return;


    switch (Regulation_Status_TemperatureInt.status_t_int)
    {
        /* status - temperature regular */
        case TEMPERATURE__OK:
            result = Regulation_TemperatureRegulation(current_temperature_int, regulated_temperature_int, winter_mode_int);
            if (result == +1)
            {
                /* it would be necessary to enable the output */
                if (!Regulation_Status_TemperatureInt.status_change_timer_start_int)
                {
                    /* status change start timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Internal temperature: start TO BE REGULATED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureInt.status_change_timer_start_int = TRUE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                    Regulation_UpdateVarCrc();

                  //Regulation_TimerTemperatureIntStart_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                    Regulation_TimerTemperatureIntStart_Start(20, FALSE);   // NOTE: no duration before changing temperature status (so very small timeout)
                }
            }
            else
            {
                /* it would be necessary to disable the output */
                if ( Regulation_Status_TemperatureInt.status_change_timer_start_int)
                {
                    /* status change start timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Internal temperature: stop TO BE REGULATED" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                    Regulation_UpdateVarCrc();

                    Regulation_TimerTemperatureIntStart_Stop();
                }
            }
            break;


        /* status - temperature to be regulated (below("winter") / above("summer") the regulated temperature) */
        case TEMPERATURE__TO_BE_REGULATED:
            result = Regulation_TemperatureRegulation(current_temperature_int, regulated_temperature_int, winter_mode_int);
            if (result == -1)
            {
                /* it would be necessary to disable the output */
                if (!Regulation_Status_TemperatureInt.status_change_timer_stop_int)
                {
                    /* status change timer stop not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Internal temperature: start OK"             , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureInt.status_change_timer_stop_int = TRUE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    Regulation_UpdateVarCrc();

                  //Regulation_TimerTemperatureIntStop_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                    Regulation_TimerTemperatureIntStop_Start(20, FALSE);   // NOTE: no duration before changing temperature status (so very small timeout)
                }
            }
            else
            {
                /* it would be necessary to enable the output */
                if ( Regulation_Status_TemperatureInt.status_change_timer_stop_int)
                {
                    /* status change timer stop     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Internal temperature: stop OK"              , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureInt.status_change_timer_stop_int = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    Regulation_UpdateVarCrc();

                    Regulation_TimerTemperatureIntStop_Stop();
                }
            }
            break;


        /* status - temperature status unknown */
        default:
            //@@@ TO DO
            break;
    }
}




/*=============================================================================
 * Function   : Regulation_TemperatureExt
 *
 * Description: verify the external temperature regulation
 * Input      : - current_temperature_ext  : external current   temperature (/10 �C)
 *              - regulated_temperature_ext: external regulated temperature (/10 �C)
 *              - winter_mode_ext          : external indication of "winter" mode
 * Output     : -
 *=============================================================================*/
static void Regulation_TemperatureExt(s16 current_temperature_ext, s16 regulated_temperature_ext, bool winter_mode_ext)
{
    s8 result;


    /* verify temperature values */
    if (!Temperature_IsValidTemperature(current_temperature_ext  ))
        return;
    if (!Temperature_IsValidTemperature(regulated_temperature_ext))
        return;


    switch (Regulation_Status_TemperatureExt.status_t_ext)
    {
        /* status - temperature regular */
        case TEMPERATURE__OK:
            result = Regulation_TemperatureRegulation(current_temperature_ext, regulated_temperature_ext, winter_mode_ext);
            if (result == +1)
            {
                /* it would be necessary to enable the output */
                if (!Regulation_Status_TemperatureExt.status_change_timer_start_ext)
                {
                    /* status change start timer not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "External temperature: start TO BE REGULATED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = TRUE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    Regulation_UpdateVarCrc();

                    //Regulation_TimerTemperatureExtStart_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                    Regulation_TimerTemperatureExtStart_Start(20, FALSE);   // NOTE: no duration before changing temperature status (so very small timeout)
                }
            }
            else
            {
                /* it would be necessary to disable the output */
                if ( Regulation_Status_TemperatureExt.status_change_timer_start_ext)
                {
                    /* status change start timer     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "External temperature: stop TO BE REGULATED" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    Regulation_UpdateVarCrc();

                    Regulation_TimerTemperatureExtStart_Stop();
                }
            }
            break;


        /* status - temperature to be regulated (below("winter") / above("summer") the regulated temperature) */
        case TEMPERATURE__TO_BE_REGULATED:
            result = Regulation_TemperatureRegulation(current_temperature_ext, regulated_temperature_ext, winter_mode_ext);
            if (result == -1)
            {
                /* it would be necessary to disable the output */
                if (!Regulation_Status_TemperatureExt.status_change_timer_stop_ext)
                {
                    /* status change timer stop not active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "External temperature: start OK"             , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext = TRUE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    Regulation_UpdateVarCrc();

                    //Regulation_TimerTemperatureExtStop_Start(TIME_SEC_TEMP_STATUS_CHANGE * 1000L, FALSE);
                    Regulation_TimerTemperatureExtStop_Start(20, FALSE);   // NOTE: no duration before changing temperature status (so very small timeout)
                }
            }
            else
            {
                /* it would be necessary to enable the output */
                if ( Regulation_Status_TemperatureExt.status_change_timer_stop_ext)
                {
                    /* status change timer stop     active */

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "External temperature: stop OK"              , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    Regulation_UpdateVarCrc();

                    Regulation_TimerTemperatureExtStop_Stop();
                }
            }
            break;


        /* status - temperature status unknown */
        default:
            //@@@ TO DO
            break;
    }
}




/*===========================================================================
 * Function   : Regulation_StatusLogicalOutputInt
 *
 * Description: get the output status (logical status)
 * Input      : -
 * Output     : - output status (logical status)
 *===========================================================================*/
bool Regulation_StatusLogicalOutputInt(void)
{
    return Regulation_StatusLogicalOut1;
}




/*===========================================================================
 * Function   : Regulation_StatusLogicalOutputExt
 *
 * Description: get the output status (logical status)
 * Input      : -
 * Output     : - output status (logical status)
 *===========================================================================*/
bool Regulation_StatusLogicalOutputExt(void)
{
    return Regulation_StatusLogicalOut2;
}




/*===========================================================================
 * Function   : Regulation_StatusPhysicalExpectedOutputInt
 *
 * Description: get the output status (expected physical  status)
 * Input      : -
 * Output     : - output status (expected physical  status)
 *===========================================================================*/
bool Regulation_StatusPhysicalExpectedOutputInt(void)
{
    return Regulation_StatusPhysicalExpectedlOut1;
}




/*===========================================================================
 * Function   : Regulation_StatusPhysicalExpectedOutputExt
 *
 * Description: get the output status (expected physical  status)
 * Input      : -
 * Output     : - output status (expected physical  status)
 *===========================================================================*/
bool Regulation_StatusPhysicalExpectedOutputExt(void)
{
    return Regulation_StatusPhysicalExpectedlOut2;
}




/*===========================================================================
 * Function   : Regulation_NewTemperatures
 *
 * Description: signal new temperatures available (internal and external)
 * Input      : - sensor_status_int: new internal sensor status
 *              - temperature_int  : new internal temperature (/10 �C)
 *              - sensor_status_ext: new external sensor status
 *              - temperature_ext  : new external temperature (/10 �C)
 * Output     : -
 *===========================================================================*/
static void Regulation_NewTemperatures(u8 sensor_status_int, s16 temperature_int, u8 sensor_status_ext, s16 temperature_ext)
{
    ql_event_t event;
    QlOSStatus err;

    bool       temperature_available_int;
    bool       temperature_available_ext;


    /* internal temperature */

    if (
         (sensor_status_int == DRVTEMPERATURE__SENSOR__CONNECTED) &&
         (Temperature_IsValidTemperature(temperature_int)       )
       )
    {
        temperature_available_int = TRUE;
    }
    else
    {
        temperature_available_int = FALSE;
    }

    event.id     = TASK_MSG_ID__NEW_TEMPERATURE_INT;
    event.param1 = temperature_available_int;
    event.param2 = temperature_int;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }



    /* external temperature */

    if (
         (sensor_status_ext == DRVTEMPERATURE__SENSOR__CONNECTED) &&
         (Temperature_IsValidTemperature(temperature_ext)       )
       )
    {
        temperature_available_ext = TRUE;
    }
    else
    {
        temperature_available_ext = FALSE;
    }

    event.id     = TASK_MSG_ID__NEW_TEMPERATURE_EXT;
    event.param1 = temperature_available_ext;
    event.param2 = temperature_ext;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Regulation_RegulationTemperatureInt_Off
 *
 * Description: internal temperature regulation - off
 * Input      : -
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
bool Regulation_RegulationTemperatureInt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_TEMP_INT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : Regulation_RegulationTemperatureInt_On
 *
 * Description: internal temperature regulation - on
 * Input      : -
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
bool Regulation_RegulationTemperatureInt_On(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_TEMP_INT__ON;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : Regulation_RegulationTemperatureInt_Regulation
 *
 * Description: internal temperature regulation - regulation
 * Input      : - temperature to be regulated (/10 �C) [0.0-30.0 �C]
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
bool Regulation_RegulationTemperatureInt_Regulation(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__REGULATION_TEMP_INT__REGULATION;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : Regulation_RegulationTemperatureExt_Off
 *
 * Description: external temperature regulation - off
 * Input      : -
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
bool Regulation_RegulationTemperatureExt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_TEMP_EXT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : Regulation_RegulationTemperatureExt_On
 *
 * Description: external temperature regulation - on
 * Input      : -
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
bool Regulation_RegulationTemperatureExt_On(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_TEMP_EXT__ON;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : Regulation_RegulationTemperatureExt_Regulation
 *
 * Description: external temperature regulation - regulation
 * Input      : - temperature to be regulated (/10 �C) [0.0-30.0 �C]
 * Output     : - FALSE: regulation not started
 *              - TRUE : regulation     started
 *=============================================================================*/
bool Regulation_RegulationTemperatureExt_Regulation(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__REGULATION_TEMP_EXT__REGULATION;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : Regulation_OutputsUnblock
 *
 * Description: signal that outputs are unblocked
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Regulation_OutputsUnblock(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__OUTPUTS_UNBLOCK;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Regulation_OutputsBlock
 *
 * Description: signal that outputs are blocked
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Regulation_OutputsBlock(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__OUTPUTS_BLOCK;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Regulation_NewConfigActiveStatusOut1
 *
 * Description: signal a new "active status OUT1" configuration
 * Input      : - ptr_config_new: pointer to the new configuration
 *              - ptr_config_old: pointer to the old configuration
 * Output     : -
 *===========================================================================*/
void Regulation_NewConfigActiveStatusOut1(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_config_new, REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_config_old)
{
    ql_event_t event;
    QlOSStatus err;


    if (
         ((!ptr_config_old->active_status) && ( ptr_config_new->active_status)) ||
         (( ptr_config_old->active_status) && (!ptr_config_new->active_status))
       )
    {
        event.id = TASK_MSG_ID__CHANGED_ACTIVE_STATUS_OUT1;

        err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*===========================================================================
 * Function   : Regulation_NewConfigActiveStatusOut2
 *
 * Description: signal a new "active status OUT2" configuration
 * Input      : - ptr_config_new: pointer to the new configuration
 *              - ptr_config_old: pointer to the old configuration
 * Output     : -
 *===========================================================================*/
void Regulation_NewConfigActiveStatusOut2(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_config_new, REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_config_old)
{
    ql_event_t event;
    QlOSStatus err;


    if (
         ((!ptr_config_old->active_status) && ( ptr_config_new->active_status)) ||
         (( ptr_config_old->active_status) && (!ptr_config_new->active_status))
       )
    {
        event.id = TASK_MSG_ID__CHANGED_ACTIVE_STATUS_OUT2;

        err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut1_GetDefault
 *
 * Description: get the default "active status OUT1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Config_ActiveStatusOut1_GetDefault(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data)
{
    *ptr_data = Regulation_Config_ActiveStatusOut1Default;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut1_Get
 *
 * Description: get the "active status OUT1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Config_ActiveStatusOut1_Get(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data)
{
    *ptr_data = Regulation_Config_ActiveStatusOut1;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut1_Set
 *
 * Description: set the "active status OUT1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Regulation_Config_ActiveStatusOut1_Set(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data)
{
    Regulation_Config_ActiveStatusOut1 = *ptr_data;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut1_IsValid
 *
 * Description: check if the "active status OUT1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Regulation_Config_ActiveStatusOut1_IsValid(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data)
{

    return TRUE;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut2_GetDefault
 *
 * Description: get the default "active status OUT2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Config_ActiveStatusOut2_GetDefault(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data)
{
    *ptr_data = Regulation_Config_ActiveStatusOut2Default;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut2_Get
 *
 * Description: get the "active status OUT2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Config_ActiveStatusOut2_Get(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data)
{
    *ptr_data = Regulation_Config_ActiveStatusOut2;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut2_Set
 *
 * Description: set the "active status OUT2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Regulation_Config_ActiveStatusOut2_Set(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data)
{
    Regulation_Config_ActiveStatusOut2 = *ptr_data;
}




/*===========================================================================
 * Function   : Regulation_Config_ActiveStatusOut2_IsValid
 *
 * Description: check if the "active status OUT2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Regulation_Config_ActiveStatusOut2_IsValid(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data)
{

    return TRUE;
}




/*===========================================================================
 * Function   : Regulation_Status_TemperatureInt_GetDefault
 *
 * Description: get the default "internal temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Status_TemperatureInt_GetDefault(REGULATION__STATUS__TEMPERATURE_INT *ptr_data)
{
    *ptr_data = Regulation_Status_TemperatureIntDefault;
}




/*===========================================================================
 * Function   : Regulation_Status_TemperatureInt_Get
 *
 * Description: get the "internal temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Status_TemperatureInt_Get(REGULATION__STATUS__TEMPERATURE_INT *ptr_data)
{
    *ptr_data = Regulation_Status_TemperatureInt;
}




/*===========================================================================
 * Function   : Regulation_Status_TemperatureInt_Set
 *
 * Description: set the "internal temperature" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Regulation_Status_TemperatureInt_Set(REGULATION__STATUS__TEMPERATURE_INT *ptr_data)
{
    Regulation_Status_TemperatureInt = *ptr_data;

    Regulation_UpdateVarCrc();
}




/*===========================================================================
 * Function   : Regulation_Status_TemperatureExt_GetDefault
 *
 * Description: get the default "external temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Status_TemperatureExt_GetDefault(REGULATION__STATUS__TEMPERATURE_EXT *ptr_data)
{
    *ptr_data = Regulation_Status_TemperatureExtDefault;
}




/*===========================================================================
 * Function   : Regulation_Status_TemperatureExt_Get
 *
 * Description: get the "external temperature" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Regulation_Status_TemperatureExt_Get(REGULATION__STATUS__TEMPERATURE_EXT *ptr_data)
{
    *ptr_data = Regulation_Status_TemperatureExt;
}




/*===========================================================================
 * Function   : Regulation_Status_TemperatureExt_Set
 *
 * Description: set the "external temperature" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Regulation_Status_TemperatureExt_Set(REGULATION__STATUS__TEMPERATURE_EXT *ptr_data)
{
    Regulation_Status_TemperatureExt = *ptr_data;

    Regulation_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureIntStart_Start
 *
 * Description: start the "internal temperature start" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureIntStart_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"internal temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Regulation_TimerHandler_TimerTemperatureIntStart, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureIntStart_Info.status         = TRUE;
        Regulation_TimerTemperatureIntStart_Info.periodic       = periodic;
        Regulation_TimerTemperatureIntStart_Info.time_value     = time_value;
        Regulation_TimerTemperatureIntStart_Info.time_remaining = time_value;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureIntStart_Stop
 *
 * Description: stop the "internal temperature start" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureIntStart_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"internal temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Regulation_TimerHandler_TimerTemperatureIntStart);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureIntStart_Info.status         = FALSE;
        Regulation_TimerTemperatureIntStart_Info.periodic       = FALSE;
        Regulation_TimerTemperatureIntStart_Info.time_value     = 0;
        Regulation_TimerTemperatureIntStart_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureIntStop_Start
 *
 * Description: start the "internal temperature stop" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureIntStop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"internal temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Regulation_TimerHandler_TimerTemperatureIntStop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureIntStop_Info.status         = TRUE;
        Regulation_TimerTemperatureIntStop_Info.periodic       = periodic;
        Regulation_TimerTemperatureIntStop_Info.time_value     = time_value;
        Regulation_TimerTemperatureIntStop_Info.time_remaining = time_value;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureIntStop_Stop
 *
 * Description: stop the "internal temperature stop" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureIntStop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"internal temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Regulation_TimerHandler_TimerTemperatureIntStop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureIntStop_Info.status         = FALSE;
        Regulation_TimerTemperatureIntStop_Info.periodic       = FALSE;
        Regulation_TimerTemperatureIntStop_Info.time_value     = 0;
        Regulation_TimerTemperatureIntStop_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureExtStart_Start
 *
 * Description: start the "external temperature start" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureExtStart_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"external temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Regulation_TimerHandler_TimerTemperatureExtStart, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureExtStart_Info.status         = TRUE;
        Regulation_TimerTemperatureExtStart_Info.periodic       = periodic;
        Regulation_TimerTemperatureExtStart_Info.time_value     = time_value;
        Regulation_TimerTemperatureExtStart_Info.time_remaining = time_value;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureExtStart_Stop
 *
 * Description: stop the "external temperature start" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureExtStart_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"external temperature start\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Regulation_TimerHandler_TimerTemperatureExtStart);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureExtStart_Info.status         = FALSE;
        Regulation_TimerTemperatureExtStart_Info.periodic       = FALSE;
        Regulation_TimerTemperatureExtStart_Info.time_value     = 0;
        Regulation_TimerTemperatureExtStart_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureExtStop_Start
 *
 * Description: start the "external temperature stop" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureExtStop_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Start \"external temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Regulation_TimerHandler_TimerTemperatureExtStop, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureExtStop_Info.status         = TRUE;
        Regulation_TimerTemperatureExtStop_Info.periodic       = periodic;
        Regulation_TimerTemperatureExtStop_Info.time_value     = time_value;
        Regulation_TimerTemperatureExtStop_Info.time_remaining = time_value;

        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_TimerTemperatureExtStop_Stop
 *
 * Description: stop the "external temperature stop" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Regulation_TimerTemperatureExtStop_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Stop \"external temperature stop\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Regulation_TimerHandler_TimerTemperatureExtStop);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Regulation_TimerTemperatureExtStop_Info.status         = FALSE;
        Regulation_TimerTemperatureExtStop_Info.periodic       = FALSE;
        Regulation_TimerTemperatureExtStop_Info.time_value     = 0;
        Regulation_TimerTemperatureExtStop_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }
}




/*===========================================================================
 * Function   : Regulation_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void Regulation_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    /* temperature regulation status (internal and external) */
                 u8                                 old_sem_state_int;
                 u8                                 old_sem_state_ext;

    /* regulation temperatures (/10 �C) */
                 s16                                old_reg_temperature_int;
                 s16                                old_reg_temperature_ext;

    /* current    temperatures (/10 �C) */
                 bool                               old_current_temperature_available_int;
                 bool                               old_current_temperature_available_ext;
                 u8                                 old_num_of_current_temperature_not_available_int;
                 u8                                 old_num_of_current_temperature_not_available_ext;
                 s16                                old_current_temperature_int;
                 s16                                old_current_temperature_ext;

    /* outputs status (logical status) */
                 bool                               old_status_logical_out1;
                 bool                               old_status_logical_out2;

    /* outputs status (physical expected status) */
                 bool                               old_status_physical_expected_out1;
                 bool                               old_status_physical_expected_out2;

    /* outputs block indication */
                 bool                               old_outputs_blocked;

    /* status - internal temperature status */
                 u8                                 old_status_temperature_int_status_t_int;
                 bool                               old_status_temperature_int_status_change_timer_start_int;
                 bool                               old_status_temperature_int_status_change_timer_stop_int;

    /* status - external temperature status */
                 u8                                 old_status_temperature_ext_status_t_ext;
                 bool                               old_status_temperature_ext_status_change_timer_start_ext;
                 bool                               old_status_temperature_ext_status_change_timer_stop_ext;

                 u32                                sem_event;

                 MODE__CONFIG__MODE_TEMPERATURE_INT config_mode_temperature_int;
                 MODE__CONFIG__MODE_TEMPERATURE_EXT config_mode_temperature_ext;

                 bool                               winter_mode_int;
                 bool                               winter_mode_ext;

    static const ascii                             *sem_state_strings[] =
    {
        SEM_STATE_STRING__TEMPERATURE_REG_STATUS__OFF,
        SEM_STATE_STRING__TEMPERATURE_REG_STATUS__ON,
        SEM_STATE_STRING__TEMPERATURE_REG_STATUS__REGULATION,
    };

    static const ascii                             *sem_event_strings[] =
    {
        SEM_EVENT_STRING__NEW_TEMPERATURE_INT,
        SEM_EVENT_STRING__NEW_TEMPERATURE_EXT,
        SEM_EVENT_STRING__REGULATION_TEMP_INT__OFF,
        SEM_EVENT_STRING__REGULATION_TEMP_INT__ON,
        SEM_EVENT_STRING__REGULATION_TEMP_INT__REGULATION,
        SEM_EVENT_STRING__REGULATION_TEMP_EXT__OFF,
        SEM_EVENT_STRING__REGULATION_TEMP_EXT__ON,
        SEM_EVENT_STRING__REGULATION_TEMP_EXT__REGULATION,
        SEM_EVENT_STRING__TIMEOUT_T_INT_START,
        SEM_EVENT_STRING__TIMEOUT_T_INT_STOP,
        SEM_EVENT_STRING__TIMEOUT_T_EXT_START,
        SEM_EVENT_STRING__TIMEOUT_T_EXT_STOP,
        SEM_EVENT_STRING__OUTPUTS_UNBLOCK,
        SEM_EVENT_STRING__OUTPUTS_BLOCK,
        SEM_EVENT_STRING__CHANGED_ACTIVE_STATUS_OUT1,
        SEM_EVENT_STRING__CHANGED_ACTIVE_STATUS_OUT2,
    };


    /* debug */
    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "CALLBACK     - MESSAGE     - TASK REGULATION - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        case TASK_MSG_ID__NEW_TEMPERATURE_INT:
            sem_event = SEM_EVENT__NEW_TEMPERATURE_INT;
            break;

        case TASK_MSG_ID__NEW_TEMPERATURE_EXT:
            sem_event = SEM_EVENT__NEW_TEMPERATURE_EXT;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_INT__OFF:
            sem_event = SEM_EVENT__REGULATION_TEMP_INT__OFF;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_INT__ON:
            sem_event = SEM_EVENT__REGULATION_TEMP_INT__ON;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_INT__REGULATION:
            sem_event = SEM_EVENT__REGULATION_TEMP_INT__REGULATION;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_EXT__OFF:
            sem_event = SEM_EVENT__REGULATION_TEMP_EXT__OFF;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_EXT__ON:
            sem_event = SEM_EVENT__REGULATION_TEMP_EXT__ON;
            break;

        case TASK_MSG_ID__REGULATION_TEMP_EXT__REGULATION:
            sem_event = SEM_EVENT__REGULATION_TEMP_EXT__REGULATION;
            break;

        case TASK_MSG_ID__TIMEOUT_T_INT_START:
            sem_event = SEM_EVENT__TIMEOUT_T_INT_START;
            break;

        case TASK_MSG_ID__TIMEOUT_T_INT_STOP:
            sem_event = SEM_EVENT__TIMEOUT_T_INT_STOP;
            break;

        case TASK_MSG_ID__TIMEOUT_T_EXT_START:
            sem_event = SEM_EVENT__TIMEOUT_T_EXT_START;
            break;

        case TASK_MSG_ID__TIMEOUT_T_EXT_STOP:
            sem_event = SEM_EVENT__TIMEOUT_T_EXT_STOP;
            break;

        case TASK_MSG_ID__OUTPUTS_UNBLOCK:
            sem_event = SEM_EVENT__OUTPUTS_UNBLOCK;
            break;

        case TASK_MSG_ID__OUTPUTS_BLOCK:
            sem_event = SEM_EVENT__OUTPUTS_BLOCK;
            break;

        case TASK_MSG_ID__CHANGED_ACTIVE_STATUS_OUT1:
            sem_event = SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT1;
            break;

        case TASK_MSG_ID__CHANGED_ACTIVE_STATUS_OUT2:
            sem_event = SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT2;
            break;

        default:
            return;
    }



    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "SEM state int: %s", sem_state_strings[Regulation_SemStateInt]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "SEM state ext: %s", sem_state_strings[Regulation_SemStateExt]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "SEM event    : %s", sem_event_strings[sem_event    ]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Old temperature int. - current: %d (/ 10 �C) [%d],  regulated: %d (/ 10 �C)", Regulation_CurrentTemperatureInt, Regulation_CurrentTemperatureAvailableInt, Regulation_RegTemperatureInt);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Old temperature ext. - current: %d (/ 10 �C) [%d],  regulated: %d (/ 10 �C)", Regulation_CurrentTemperatureExt, Regulation_CurrentTemperatureAvailableExt, Regulation_RegTemperatureExt);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* temperature regulation status (internal and external) */
    old_sem_state_int                                        = Regulation_SemStateInt;
    old_sem_state_ext                                        = Regulation_SemStateExt;

    /* regulation temperatures (/10 �C) */
    old_reg_temperature_int                                  = Regulation_RegTemperatureInt;
    old_reg_temperature_ext                                  = Regulation_RegTemperatureExt;

    /* current    temperatures (/10 �C) */
    old_current_temperature_available_int                    = Regulation_CurrentTemperatureAvailableInt;
    old_current_temperature_available_ext                    = Regulation_CurrentTemperatureAvailableExt;
    old_num_of_current_temperature_not_available_int         = Regulation_NumOfCurrentTemperatureNotAvailableInt;
    old_num_of_current_temperature_not_available_ext         = Regulation_NumOfCurrentTemperatureNotAvailableExt;
    old_current_temperature_int                              = Regulation_CurrentTemperatureInt;
    old_current_temperature_ext                              = Regulation_CurrentTemperatureExt;

    /* outputs status (logical status) */
    old_status_logical_out1                                  = Regulation_StatusLogicalOut1;
    old_status_logical_out2                                  = Regulation_StatusLogicalOut2;

    /* outputs status (physical expected status) */
    old_status_physical_expected_out1                        = Regulation_StatusPhysicalExpectedlOut1;
    old_status_physical_expected_out2                        = Regulation_StatusPhysicalExpectedlOut2;

    /* outputs block indication */
    old_outputs_blocked                                      = Regulation_OutputsBlocked;

    /* status - internal temperature status */
    old_status_temperature_int_status_t_int                  = Regulation_Status_TemperatureInt.status_t_int;
    old_status_temperature_int_status_change_timer_start_int = Regulation_Status_TemperatureInt.status_change_timer_start_int;
    old_status_temperature_int_status_change_timer_stop_int  = Regulation_Status_TemperatureInt.status_change_timer_stop_int;

    /* status - external temperature status */
    old_status_temperature_ext_status_t_ext                  = Regulation_Status_TemperatureExt.status_t_ext;
    old_status_temperature_ext_status_change_timer_start_ext = Regulation_Status_TemperatureExt.status_change_timer_start_ext;
    old_status_temperature_ext_status_change_timer_stop_ext  = Regulation_Status_TemperatureExt.status_change_timer_stop_ext;



    /*---------------------------------------------------------------------------
     * Outputs block/unblock events
     *---------------------------------------------------------------------------*/
    switch (sem_event)
    {
        /* SEM event - outputs unblock */
        case SEM_EVENT__OUTPUTS_UNBLOCK:
            Regulation_OutputsBlocked = FALSE;

            if (Regulation_StatusLogicalOut1)
            {
                /* enable output OUT1 */
                if ( Regulation_Config_ActiveStatusOut1.active_status)
                    DrvGpio_ActivateOut1();       /* activate    digital output OUT1 */
            }
            else
            {
                /* enable output OUT1 */
                if (!Regulation_Config_ActiveStatusOut1.active_status)
                    DrvGpio_ActivateOut1();       /* activate    digital output OUT1 */
            }

            if (Regulation_StatusLogicalOut2)
            {
                /* enable output OUT2 */
                if ( Regulation_Config_ActiveStatusOut2.active_status)
                    DrvGpio_ActivateOut2();       /* activate    digital output OUT2 */
            }
            else
            {
                /* enable output OUT2 */
                if (!Regulation_Config_ActiveStatusOut2.active_status)
                    DrvGpio_ActivateOut2();       /* activate    digital output OUT1 */
            }
            break;


        /* SEM event - outputs block */
        case SEM_EVENT__OUTPUTS_BLOCK:
            Regulation_OutputsBlocked = TRUE;

            /* disable output OUT1 */
            DrvGpio_DisactivateOut1();            /* disactivate digital output OUT1 */

            /* disable output OUT2 */
            DrvGpio_DisactivateOut2();            /* disactivate digital output OUT2 */
            break;
    }



    /*---------------------------------------------------------------------------
     * Active status output configuration changed events
     *---------------------------------------------------------------------------*/
    switch (sem_event)
    {
        /* SEM event - active status OUT1 configuration changed */
        case SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT1:
            if (Regulation_StatusLogicalOut1)
            {
                if (Regulation_Config_ActiveStatusOut1.active_status)
                    Regulation_StatusPhysicalExpectedlOut1 = TRUE;
                else
                    Regulation_StatusPhysicalExpectedlOut1 = FALSE;
            }
            else
            {
                if (Regulation_Config_ActiveStatusOut1.active_status)
                    Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                else
                    Regulation_StatusPhysicalExpectedlOut1 = TRUE;
            }

            if (!Regulation_OutputsBlocked)
            {
                if (Regulation_StatusLogicalOut1)
                {
                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    else
                        DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                }
                else
                {
                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                    else
                        DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                }
            }
            break;


        /* SEM event - active status OUT2 configuration changed */
        case SEM_EVENT__CHANGED_ACTIVE_STATUS_OUT2:
            if (Regulation_StatusLogicalOut1)
            {
                if (Regulation_Config_ActiveStatusOut2.active_status)
                    Regulation_StatusPhysicalExpectedlOut2 = TRUE;
                else
                    Regulation_StatusPhysicalExpectedlOut2 = FALSE;
            }
            else
            {
                if (Regulation_Config_ActiveStatusOut2.active_status)
                    Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                else
                    Regulation_StatusPhysicalExpectedlOut2 = TRUE;
            }

            if (!Regulation_OutputsBlocked)
            {
                if (Regulation_StatusLogicalOut2)
                {
                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    else
                        DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                }
                else
                {
                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                    else
                        DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                }
            }
            break;
    }



    /*---------------------------------------------------------------------------
     * Internal temperature regulation
     *---------------------------------------------------------------------------*/
    switch (Regulation_SemStateInt)
    {
        /*----------------------------------------------
         * SEM state - temperature regulation status - disabled
         *----------------------------------------------*/
        case SEM_STATE__TEMPERATURE_REG_STATUS__OFF:
            switch (sem_event)
            {
                // SEM event - new internal temperature available
                case SEM_EVENT__NEW_TEMPERATURE_INT:
                    /* extract the parameters */
                    Regulation_CurrentTemperatureAvailableInt = (bool)msg_identifier->param1;
                    Regulation_CurrentTemperatureInt          = (s16 )msg_identifier->param2;

                    if (!Regulation_CurrentTemperatureAvailableInt)
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableInt < NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                            Regulation_NumOfCurrentTemperatureNotAvailableInt++;
                    }
                    else
                    {
                        Regulation_NumOfCurrentTemperatureNotAvailableInt = 0;
                    }

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Current internal temperature: %d (/10 �C) [%d %d]", Regulation_CurrentTemperatureInt, Regulation_CurrentTemperatureAvailableInt, Regulation_NumOfCurrentTemperatureNotAvailableInt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    // NOTHING TO DO (no temperature regulation)
                    break;



                // SEM event - internal temperature regulation - off
                case SEM_EVENT__REGULATION_TEMP_INT__OFF:
                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }

                    Regulation_RegTemperatureInt = 0;
                    Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
                    break;



                // SEM event - internal temperature regulation - on
                case SEM_EVENT__REGULATION_TEMP_INT__ON:
                    Regulation_StatusLogicalOut1 = TRUE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;

                    /* enable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                        else
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                    }

                    Regulation_RegTemperatureInt = 0;
                    Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__ON;
                    break;



                // SEM event - internal temperature regulation - regulation
                case SEM_EVENT__REGULATION_TEMP_INT__REGULATION:
                    /* extract the parameters */
                    Regulation_RegTemperatureInt = (s16)msg_identifier->param1;

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Regulation internal temperature: %d (/10 �C)", Regulation_RegTemperatureInt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }

                    /* get internal temperature regulation mode ("winter" or "summer" mode) */
                    Mode_Config_ModeTemperatureInt_Get(&config_mode_temperature_int);
                    if (config_mode_temperature_int.mode == MODE__WINTER_MODE)
                        winter_mode_int = TRUE;
                    else
                        winter_mode_int = FALSE;

                    Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                    Regulation_Status_TemperatureInt.status_change_timer_stop_int  = FALSE;
                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    if (Regulation_CurrentTemperatureAvailableInt)
                    {
                        /* start internal temperature regulation */
                        Regulation_TemperatureInt(Regulation_CurrentTemperatureInt, Regulation_RegTemperatureInt, winter_mode_int);
                    }

                    Regulation_SemStateInt = SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION;
                    break;



                /* SEM event - timeout internal temperature start */
                /* SEM event - timeout internal temperature stop  */
                case SEM_EVENT__TIMEOUT_T_INT_START:
                case SEM_EVENT__TIMEOUT_T_INT_STOP:
                    // NOTHING TO DO
                    break;



                // unknown event
                default:
                    break;
            }
            break;




        /*----------------------------------------------
         * SEM state - temperature regulation status - enabled
         *----------------------------------------------*/
        case SEM_STATE__TEMPERATURE_REG_STATUS__ON:
            switch (sem_event)
            {
                // SEM event - new internal temperature available
                case SEM_EVENT__NEW_TEMPERATURE_INT:
                    /* extract the parameters */
                    Regulation_CurrentTemperatureAvailableInt = (bool)msg_identifier->param1;
                    Regulation_CurrentTemperatureInt          = (s16 )msg_identifier->param2;

                    if (!Regulation_CurrentTemperatureAvailableInt)
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableInt < NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                            Regulation_NumOfCurrentTemperatureNotAvailableInt++;
                    }
                    else
                    {
                        Regulation_NumOfCurrentTemperatureNotAvailableInt = 0;
                    }

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Current internal temperature: %d (/10 �C) [%d %d]", Regulation_CurrentTemperatureInt, Regulation_CurrentTemperatureAvailableInt, Regulation_NumOfCurrentTemperatureNotAvailableInt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    // NOTHING TO DO (no temperature regulation)
                    break;



                // SEM event - internal temperature regulation - off
                case SEM_EVENT__REGULATION_TEMP_INT__OFF:
                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }

                    Regulation_RegTemperatureInt = 0;
                    Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
                    break;



                // SEM event - internal temperature regulation - on
                case SEM_EVENT__REGULATION_TEMP_INT__ON:
                    Regulation_StatusLogicalOut1 = TRUE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;

                    /* enable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                        else
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                    }

                    Regulation_RegTemperatureInt = 0;
                    Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__ON;
                    break;



                // SEM event - internal temperature regulation - regulation
                case SEM_EVENT__REGULATION_TEMP_INT__REGULATION:
                    /* extract the parameters */
                    Regulation_RegTemperatureInt = (s16)msg_identifier->param1;

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Regulation internal temperature: %d (/10 �C)", Regulation_RegTemperatureInt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }

                    /* get internal temperature regulation mode ("winter" or "summer" mode) */
                    Mode_Config_ModeTemperatureInt_Get(&config_mode_temperature_int);
                    if (config_mode_temperature_int.mode == MODE__WINTER_MODE)
                        winter_mode_int = TRUE;
                    else
                        winter_mode_int = FALSE;

                    Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                    Regulation_Status_TemperatureInt.status_change_timer_stop_int  = FALSE;
                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    if (Regulation_CurrentTemperatureAvailableInt)
                    {
                        /* start internal temperature regulation */
                        Regulation_TemperatureInt(Regulation_CurrentTemperatureInt, Regulation_RegTemperatureInt, winter_mode_int);
                    }

                    Regulation_SemStateInt = SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION;
                    break;



                /* SEM event - timeout internal temperature start */
                /* SEM event - timeout internal temperature stop  */
                case SEM_EVENT__TIMEOUT_T_INT_START:
                case SEM_EVENT__TIMEOUT_T_INT_STOP:
                    // NOTHING TO DO
                    break;



                // unknown event
                default:
                    break;
            }
            break;




        /*----------------------------------------------
         * SEM state - temperature regulation status - regulation
         *----------------------------------------------*/
        case SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION:
            switch (sem_event)
            {
                // SEM event - new internal temperature available
                case SEM_EVENT__NEW_TEMPERATURE_INT:
                    /* extract the parameters */
                    Regulation_CurrentTemperatureAvailableInt = (bool)msg_identifier->param1;
                    Regulation_CurrentTemperatureInt          = (s16 )msg_identifier->param2;

                    if (!Regulation_CurrentTemperatureAvailableInt)
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableInt < NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                            Regulation_NumOfCurrentTemperatureNotAvailableInt++;
                    }
                    else
                    {
                        Regulation_NumOfCurrentTemperatureNotAvailableInt = 0;
                    }

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Current internal temperature: %d (/10 �C) [%d %d]", Regulation_CurrentTemperatureInt, Regulation_CurrentTemperatureAvailableInt, Regulation_NumOfCurrentTemperatureNotAvailableInt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    if (Regulation_CurrentTemperatureAvailableInt)
                    {
                        /* get internal temperature regulation mode ("winter" or "summer" mode) */
                        Mode_Config_ModeTemperatureInt_Get(&config_mode_temperature_int);
                        if (config_mode_temperature_int.mode == MODE__WINTER_MODE)
                            winter_mode_int = TRUE;
                        else
                            winter_mode_int = FALSE;

                        /* internal temperature regulation */
                        Regulation_TemperatureInt(Regulation_CurrentTemperatureInt, Regulation_RegTemperatureInt, winter_mode_int);
                    }
                    else
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableInt >= NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                        {
                            if (Regulation_StatusLogicalOut1)
                            {
                                Regulation_StatusLogicalOut1 = FALSE;

                                if (Regulation_Config_ActiveStatusOut1.active_status)
                                    Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                                else
                                    Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                                /* disable output OUT1 */
                                if (!Regulation_OutputsBlocked)
                                {
                                    if (Regulation_Config_ActiveStatusOut1.active_status)
                                        DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                                    else
                                        DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                                }

                                /* stop possibly active timers */
                                if (Regulation_Status_TemperatureInt.status_change_timer_start_int)
                                {
                                    Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                                    /* request the backup to flash objects */
                                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                                    Regulation_TimerTemperatureIntStart_Stop();
                                }
                                if (Regulation_Status_TemperatureInt.status_change_timer_stop_int)
                                {
                                    Regulation_Status_TemperatureInt.status_change_timer_stop_int = FALSE;
                                    /* request the backup to flash objects */
                                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                                    Regulation_TimerTemperatureIntStop_Stop();
                                }

                                Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__OK;
                                Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                                Regulation_Status_TemperatureInt.status_change_timer_stop_int  = FALSE;
                                /* request the backup to flash objects */
                                //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);
                            }
                        }
                    }
                    break;


                // SEM event - internal temperature regulation - off
                case SEM_EVENT__REGULATION_TEMP_INT__OFF:
                    /* stop possibly active timers */
                    if (Regulation_Status_TemperatureInt.status_change_timer_start_int)
                    {
                        Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                        Regulation_TimerTemperatureIntStart_Stop();
                    }
                    if (Regulation_Status_TemperatureInt.status_change_timer_stop_int)
                    {
                        Regulation_Status_TemperatureInt.status_change_timer_stop_int = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                        Regulation_TimerTemperatureIntStop_Stop();
                    }

                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }

                    Regulation_RegTemperatureInt = 0;
                    Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
                    break;



                // SEM event - internal temperature regulation - on
                case SEM_EVENT__REGULATION_TEMP_INT__ON:
                    if (Regulation_Status_TemperatureInt.status_change_timer_start_int)
                    {
                        Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                        Regulation_TimerTemperatureIntStart_Stop();
                    }
                    if (Regulation_Status_TemperatureInt.status_change_timer_stop_int)
                    {
                        Regulation_Status_TemperatureInt.status_change_timer_stop_int = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_INT_STATUS);

                        Regulation_TimerTemperatureIntStop_Stop();
                    }

                    Regulation_StatusLogicalOut1 = TRUE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;

                    /* enable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                        else
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                    }

                    Regulation_RegTemperatureInt = 0;
                    Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__ON;
                    break;



                // SEM event - internal temperature regulation - regulation
                case SEM_EVENT__REGULATION_TEMP_INT__REGULATION:
                    /* extract the parameters */
                    Regulation_RegTemperatureInt = (s16)msg_identifier->param1;

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Regulation internal temperature: %d (/10 �C)", Regulation_RegTemperatureInt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }

                    /* stop possibly active timers */
                    if (Regulation_Status_TemperatureInt.status_change_timer_start_int)
                    {
                        Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                        Regulation_TimerTemperatureIntStart_Stop();
                    }
                    if (Regulation_Status_TemperatureInt.status_change_timer_stop_int)
                    {
                        Regulation_Status_TemperatureInt.status_change_timer_stop_int = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                        Regulation_TimerTemperatureIntStop_Stop();
                    }

                    /* get internal temperature regulation mode ("winter" or "summer" mode) */
                    Mode_Config_ModeTemperatureInt_Get(&config_mode_temperature_int);
                    if (config_mode_temperature_int.mode == MODE__WINTER_MODE)
                        winter_mode_int = TRUE;
                    else
                        winter_mode_int = FALSE;

                    Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;
                    Regulation_Status_TemperatureInt.status_change_timer_stop_int  = FALSE;
                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    if (Regulation_CurrentTemperatureAvailableInt)
                    {
                        /* start internal temperature regulation */
                        Regulation_TemperatureInt(Regulation_CurrentTemperatureInt, Regulation_RegTemperatureInt, winter_mode_int);
                    }
                    break;



                /* SEM event - timeout internal temperature start */
                case SEM_EVENT__TIMEOUT_T_INT_START:
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Internal temperature: TO BE REGULATED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* update the internal temperature status */
                    Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__TO_BE_REGULATED;
                    Regulation_Status_TemperatureInt.status_change_timer_start_int = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    Regulation_StatusLogicalOut1 = TRUE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;

                    /* enable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                        else
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                    }
                    break;



                /* SEM event - timeout internal temperature stop */
                case SEM_EVENT__TIMEOUT_T_INT_STOP:
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "Internal temperature: OK"             , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* update the internal temperature status */
                    Regulation_Status_TemperatureInt.status_t_int                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureInt.status_change_timer_stop_int  = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_INT_STATUS);

                    Regulation_StatusLogicalOut1 = FALSE;

                    if (Regulation_Config_ActiveStatusOut1.active_status)
                        Regulation_StatusPhysicalExpectedlOut1 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut1 = TRUE;

                    /* disable output OUT1 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut1.active_status)
                            DrvGpio_DisactivateOut1();        /* disactivate digital output OUT1 */
                        else
                            DrvGpio_ActivateOut1();           /* activate    digital output OUT1 */
                    }
                    break;



                // unknown event
                default:
                    break;
            }
            break;




        /*----------------------------------------------
         * temperature regulation status - unknown
         *----------------------------------------------*/
        default:
            Regulation_RegTemperatureInt = 0;
            Regulation_SemStateInt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
            break;
    }




    /*---------------------------------------------------------------------------
     * External temperature regulation
     *---------------------------------------------------------------------------*/
    switch (Regulation_SemStateExt)
    {
        /*----------------------------------------------
         * SEM state - temperature regulation status - disabled
         *----------------------------------------------*/
        case SEM_STATE__TEMPERATURE_REG_STATUS__OFF:
            switch (sem_event)
            {
                // SEM event - new external temperature available
                case SEM_EVENT__NEW_TEMPERATURE_EXT:
                    /* extract the parameters */
                    Regulation_CurrentTemperatureAvailableExt = (bool)msg_identifier->param1;
                    Regulation_CurrentTemperatureExt          = (s16 )msg_identifier->param2;

                    if (!Regulation_CurrentTemperatureAvailableExt)
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableExt < NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                            Regulation_NumOfCurrentTemperatureNotAvailableExt++;
                    }
                    else
                    {
                        Regulation_NumOfCurrentTemperatureNotAvailableExt = 0;
                    }

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Current external temperature: %d (/10 �C) [%d %d]", Regulation_CurrentTemperatureExt, Regulation_CurrentTemperatureAvailableExt, Regulation_NumOfCurrentTemperatureNotAvailableExt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    // NOTHING TO DO (no temperature regulation)
                    break;



                // SEM event - external temperature regulation - off
                case SEM_EVENT__REGULATION_TEMP_EXT__OFF:
                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }

                    Regulation_RegTemperatureExt = 0;
                    Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
                    break;



                // SEM event - external temperature regulation - on
                case SEM_EVENT__REGULATION_TEMP_EXT__ON:
                    Regulation_StatusLogicalOut2 = TRUE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;

                    /* enable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                        else
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                    }

                    Regulation_RegTemperatureExt = 0;
                    Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__ON;
                    break;



                // SEM event - external temperature regulation - regulation
                case SEM_EVENT__REGULATION_TEMP_EXT__REGULATION:
                    /* extract the parameters */
                    Regulation_RegTemperatureExt = (s16)msg_identifier->param1;

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Regulation external temperature: %d (/10 �C)", Regulation_RegTemperatureExt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }

                    /* get external temperature regulation mode ("winter" or "summer" mode) */
                    Mode_Config_ModeTemperatureExt_Get(&config_mode_temperature_ext);
                    if (config_mode_temperature_ext.mode == MODE__WINTER_MODE)
                        winter_mode_ext = TRUE;
                    else
                        winter_mode_ext = FALSE;

                    Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext  = FALSE;
                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    if (Regulation_CurrentTemperatureAvailableExt)
                    {
                        /* start external temperature regulation */
                        Regulation_TemperatureExt(Regulation_CurrentTemperatureExt, Regulation_RegTemperatureExt, winter_mode_ext);
                    }

                    Regulation_SemStateExt = SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION;
                    break;



                /* SEM event - timeout external temperature start */
                /* SEM event - timeout external temperature stop  */
                case SEM_EVENT__TIMEOUT_T_EXT_START:
                case SEM_EVENT__TIMEOUT_T_EXT_STOP:
                    // NOTHING TO DO
                    break;



                // unknown event
                default:
                    break;
            }
            break;




        /*----------------------------------------------
         * SEM state - temperature regulation status - enabled
         *----------------------------------------------*/
        case SEM_STATE__TEMPERATURE_REG_STATUS__ON:
            switch (sem_event)
            {
                // SEM event - new external temperature available
                case SEM_EVENT__NEW_TEMPERATURE_EXT:
                    /* extract the parameters */
                    Regulation_CurrentTemperatureAvailableExt = (bool)msg_identifier->param1;
                    Regulation_CurrentTemperatureExt          = (s16 )msg_identifier->param2;

                    if (!Regulation_CurrentTemperatureAvailableExt)
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableExt < NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                            Regulation_NumOfCurrentTemperatureNotAvailableExt++;
                    }
                    else
                    {
                        Regulation_NumOfCurrentTemperatureNotAvailableExt = 0;
                    }

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Current external temperature: %d (/10 �C) [%d %d]", Regulation_CurrentTemperatureExt, Regulation_CurrentTemperatureAvailableExt, Regulation_NumOfCurrentTemperatureNotAvailableExt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    // NOTHING TO DO (no temperature regulation)
                    break;



                // SEM event - external temperature regulation - off
                case SEM_EVENT__REGULATION_TEMP_EXT__OFF:
                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }

                    Regulation_RegTemperatureExt = 0;
                    Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
                    break;



                // SEM event - external temperature regulation - on
                case SEM_EVENT__REGULATION_TEMP_EXT__ON:
                    Regulation_StatusLogicalOut2 = TRUE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;

                    /* enable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                        else
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                    }

                    Regulation_RegTemperatureExt = 0;
                    Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__ON;
                    break;



                // SEM event - external temperature regulation - regulation
                case SEM_EVENT__REGULATION_TEMP_EXT__REGULATION:
                    /* extract the parameters */
                    Regulation_RegTemperatureExt = (s16)msg_identifier->param1;

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Regulation external temperature: %d (/10 �C)", Regulation_RegTemperatureExt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }

                    /* get external temperature regulation mode ("winter" or "summer" mode) */
                    Mode_Config_ModeTemperatureExt_Get(&config_mode_temperature_ext);
                    if (config_mode_temperature_ext.mode == MODE__WINTER_MODE)
                        winter_mode_ext = TRUE;
                    else
                        winter_mode_ext = FALSE;

                    Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext  = FALSE;
                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    if (Regulation_CurrentTemperatureAvailableExt)
                    {
                        /* start external temperature regulation */
                        Regulation_TemperatureExt(Regulation_CurrentTemperatureExt, Regulation_RegTemperatureExt, winter_mode_ext);
                    }

                    Regulation_SemStateExt = SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION;
                    break;



                /* SEM event - timeout external temperature start */
                /* SEM event - timeout external temperature stop  */
                case SEM_EVENT__TIMEOUT_T_EXT_START:
                case SEM_EVENT__TIMEOUT_T_EXT_STOP:
                    // NOTHING TO DO
                    break;



                // unknown event
                default:
                    break;
            }
            break;




        /*----------------------------------------------
         * SEM state - temperature regulation status - regulation
         *----------------------------------------------*/
        case SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION:
            switch (sem_event)
            {
                // SEM event - new external temperature available
                case SEM_EVENT__NEW_TEMPERATURE_EXT:
                    /* extract the parameters */
                    Regulation_CurrentTemperatureAvailableExt = (bool)msg_identifier->param1;
                    Regulation_CurrentTemperatureExt          = (s16 )msg_identifier->param2;

                    if (!Regulation_CurrentTemperatureAvailableExt)
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableExt < NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                            Regulation_NumOfCurrentTemperatureNotAvailableExt++;
                    }
                    else
                    {
                        Regulation_NumOfCurrentTemperatureNotAvailableExt = 0;
                    }

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Current external temperature: %d (/10 �C) [%d %d]", Regulation_CurrentTemperatureExt, Regulation_CurrentTemperatureAvailableExt, Regulation_NumOfCurrentTemperatureNotAvailableExt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    if (Regulation_CurrentTemperatureAvailableExt)
                    {
                        /* get external temperature regulation mode ("winter" or "summer" mode) */
                        Mode_Config_ModeTemperatureExt_Get(&config_mode_temperature_ext);
                        if (config_mode_temperature_ext.mode == MODE__WINTER_MODE)
                            winter_mode_ext = TRUE;
                        else
                            winter_mode_ext = FALSE;

                        /* external temperature regulation */
                        Regulation_TemperatureExt(Regulation_CurrentTemperatureExt, Regulation_RegTemperatureExt, winter_mode_ext);
                    }
                    else
                    {
                        if (Regulation_NumOfCurrentTemperatureNotAvailableExt >= NUM_OF_CONSECUTIVE_TEMPERATURE_NOT_AVAILABLE)
                        {
                            if (Regulation_StatusLogicalOut2)
                            {
                                Regulation_StatusLogicalOut2 = FALSE;

                                if (Regulation_Config_ActiveStatusOut2.active_status)
                                    Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                                else
                                    Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                                /* disable output OUT2 */
                                if (!Regulation_OutputsBlocked)
                                {
                                    if (Regulation_Config_ActiveStatusOut2.active_status)
                                        DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                                    else
                                        DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                                }

                                /* stop possibly active timers */
                                if (Regulation_Status_TemperatureExt.status_change_timer_start_ext)
                                {
                                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                                    /* request the backup to flash objects */
                                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                                    Regulation_TimerTemperatureExtStart_Stop();
                                }
                                if (Regulation_Status_TemperatureExt.status_change_timer_stop_ext)
                                {
                                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext = FALSE;
                                    /* request the backup to flash objects */
                                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                                    Regulation_TimerTemperatureExtStop_Stop();
                                }

                                Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__OK;
                                Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                                Regulation_Status_TemperatureExt.status_change_timer_stop_ext  = FALSE;
                                /* request the backup to flash objects */
                                //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);
                            }
                        }
                    }
                    break;



                // SEM event - external temperature regulation - off
                case SEM_EVENT__REGULATION_TEMP_EXT__OFF:
                    /* stop possibly active timers */
                    if (Regulation_Status_TemperatureExt.status_change_timer_start_ext)
                    {
                        Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                        Regulation_TimerTemperatureExtStart_Stop();
                    }
                    if (Regulation_Status_TemperatureExt.status_change_timer_stop_ext)
                    {
                        Regulation_Status_TemperatureExt.status_change_timer_stop_ext = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                        Regulation_TimerTemperatureExtStop_Stop();
                    }

                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }

                    Regulation_RegTemperatureExt = 0;
                    Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
                    break;



                // SEM event - external temperature regulation - on
                case SEM_EVENT__REGULATION_TEMP_EXT__ON:
                    if (Regulation_Status_TemperatureExt.status_change_timer_start_ext)
                    {
                        Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                        Regulation_TimerTemperatureExtStart_Stop();
                    }
                    if (Regulation_Status_TemperatureExt.status_change_timer_stop_ext)
                    {
                        Regulation_Status_TemperatureExt.status_change_timer_stop_ext = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_MAX_EXT_STATUS);

                        Regulation_TimerTemperatureExtStop_Stop();
                    }

                    Regulation_StatusLogicalOut2 = TRUE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;

                    /* enable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                        else
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                    }

                    Regulation_RegTemperatureExt = 0;
                    Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__ON;
                    break;



                // SEM event - external temperature regulation - regulation
                case SEM_EVENT__REGULATION_TEMP_EXT__REGULATION:
                    /* extract the parameters */
                    Regulation_RegTemperatureExt = (s16)msg_identifier->param1;

                    snprintf(Regulation_DebugString, sizeof(Regulation_DebugString), "Regulation external temperature: %d (/10 �C)", Regulation_RegTemperatureExt);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, Regulation_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }

                    /* stop possibly active timers */
                    if (Regulation_Status_TemperatureExt.status_change_timer_start_ext)
                    {
                        Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                        Regulation_TimerTemperatureExtStart_Stop();
                    }
                    if (Regulation_Status_TemperatureExt.status_change_timer_stop_ext)
                    {
                        Regulation_Status_TemperatureExt.status_change_timer_stop_ext = FALSE;
                        /* request the backup to flash objects */
                        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                        Regulation_TimerTemperatureExtStop_Stop();
                    }

                    /* get external temperature regulation mode ("winter" or "summer" mode) */
                    Mode_Config_ModeTemperatureExt_Get(&config_mode_temperature_ext);
                    if (config_mode_temperature_ext.mode == MODE__WINTER_MODE)
                        winter_mode_ext = TRUE;
                    else
                        winter_mode_ext = FALSE;

                    Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;
                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext  = FALSE;

                    if (Regulation_CurrentTemperatureAvailableExt)
                    {
                        /* external temperature regulation */
                        Regulation_TemperatureExt(Regulation_CurrentTemperatureExt, Regulation_RegTemperatureExt, winter_mode_ext);
                    }

                    Regulation_SemStateExt = SEM_STATE__TEMPERATURE_REG_STATUS__REGULATION;
                    break;



                /* SEM event - timeout external temperature start */
                case SEM_EVENT__TIMEOUT_T_EXT_START:
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "External temperature: TO BE REGULATED", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* update the external temperature status */
                    Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__TO_BE_REGULATED;
                    Regulation_Status_TemperatureExt.status_change_timer_start_ext = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    Regulation_StatusLogicalOut2 = TRUE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;

                    /* enable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                        else
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                    }
                    break;



                /* SEM event - timeout external temperature stop */
                case SEM_EVENT__TIMEOUT_T_EXT_STOP:
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "External temperature: OK"             , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* update the external temperature status */
                    Regulation_Status_TemperatureExt.status_t_ext                  = TEMPERATURE__OK;
                    Regulation_Status_TemperatureExt.status_change_timer_stop_ext  = FALSE;

                    /* request the backup to flash objects */
                    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__009_TEMPERATURE_INFO, PROGRAM_FLASH__FLASH_ID__009_TEMPERATURE_INFO__TEMPERATURE_EXT_STATUS);

                    Regulation_StatusLogicalOut2 = FALSE;

                    if (Regulation_Config_ActiveStatusOut2.active_status)
                        Regulation_StatusPhysicalExpectedlOut2 = FALSE;
                    else
                        Regulation_StatusPhysicalExpectedlOut2 = TRUE;

                    /* disable output OUT2 */
                    if (!Regulation_OutputsBlocked)
                    {
                        if (Regulation_Config_ActiveStatusOut2.active_status)
                            DrvGpio_DisactivateOut2();        /* disactivate digital output OUT2 */
                        else
                            DrvGpio_ActivateOut2();           /* activate    digital output OUT2 */
                    }
                    break;



                // unknown event
                default:
                    break;
            }
            break;




        /*----------------------------------------------
         * temperature regulation status - unknown
         *----------------------------------------------*/
        default:
            Regulation_RegTemperatureExt = 0;
            Regulation_SemStateExt       = SEM_STATE__TEMPERATURE_REG_STATUS__OFF;
            break;
    }



    if (
         /* temperature regulation status (internal and external) */
         (Regulation_SemStateInt                                         != old_sem_state_int                                       ) ||
         (Regulation_SemStateExt                                         != old_sem_state_ext                                       ) ||

         /* regulation temperatures (/10 �C) */
         (Regulation_RegTemperatureInt                                   != old_reg_temperature_int                                 ) ||
         (Regulation_RegTemperatureExt                                   != old_reg_temperature_ext                                 ) ||

         /* current    temperatures (/10 �C) */
         (Regulation_CurrentTemperatureAvailableInt                      != old_current_temperature_available_int                   ) ||
         (Regulation_CurrentTemperatureAvailableExt                      != old_current_temperature_available_ext                   ) ||
         (Regulation_NumOfCurrentTemperatureNotAvailableInt              != old_num_of_current_temperature_not_available_int        ) ||
         (Regulation_NumOfCurrentTemperatureNotAvailableExt              != old_num_of_current_temperature_not_available_ext        ) ||
         (Regulation_CurrentTemperatureInt                               != old_current_temperature_int                             ) ||
         (Regulation_CurrentTemperatureExt                               != old_current_temperature_ext                             ) ||

         /* outputs status (logical status) */
         (Regulation_StatusLogicalOut1                                   != old_status_logical_out1                                 ) ||
         (Regulation_StatusLogicalOut2                                   != old_status_logical_out2                                 ) ||

         /* outputs status (logical status) */
         (Regulation_StatusPhysicalExpectedlOut1                         != old_status_physical_expected_out1                       ) ||
         (Regulation_StatusPhysicalExpectedlOut2                         != old_status_physical_expected_out2                       ) ||

         /* outputs block indication */
         (Regulation_OutputsBlocked                                      != old_outputs_blocked                                     ) ||

         /* status - internal temperature status */
         (Regulation_Status_TemperatureInt.status_t_int                  != old_status_temperature_int_status_t_int                 ) ||
         (Regulation_Status_TemperatureInt.status_change_timer_start_int != old_status_temperature_int_status_change_timer_start_int) ||
         (Regulation_Status_TemperatureInt.status_change_timer_stop_int  != old_status_temperature_int_status_change_timer_stop_int ) ||

         /* status - external temperature status */
         (Regulation_Status_TemperatureExt.status_t_ext                  != old_status_temperature_ext_status_t_ext                 ) ||
         (Regulation_Status_TemperatureExt.status_change_timer_start_ext != old_status_temperature_ext_status_change_timer_start_ext) ||
         (Regulation_Status_TemperatureExt.status_change_timer_stop_ext  != old_status_temperature_ext_status_change_timer_stop_ext )
       )
    {
        Regulation_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Regulation_AdlCallback_Timer_TimerTemperatureIntStart
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Regulation_AdlCallback_Timer_TimerTemperatureIntStart(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE INT START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Regulation_TimerTemperatureIntStart_Info.periodic)
    {
      //Regulation_TimerTemperatureIntStart_Info.status         = TRUE;
      //Regulation_TimerTemperatureIntStart_Info.periodic       = TRUE;
      //Regulation_TimerTemperatureIntStart_Info.time_value     = time_value;
        Regulation_TimerTemperatureIntStart_Info.time_remaining = Regulation_TimerTemperatureIntStart_Info.time_value;

        Regulation_UpdateVarCrc();
    }
    else
    {
        Regulation_TimerTemperatureIntStart_Info.status         = FALSE;
        Regulation_TimerTemperatureIntStart_Info.periodic       = FALSE;
        Regulation_TimerTemperatureIntStart_Info.time_value     = 0;
        Regulation_TimerTemperatureIntStart_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_T_INT_START;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Regulation_AdlCallback_Timer_TimerTemperatureIntStop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Regulation_AdlCallback_Timer_TimerTemperatureIntStop(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE INT STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Regulation_TimerTemperatureIntStop_Info.periodic)
    {
      //Regulation_TimerTemperatureIntStop_Info.status         = TRUE;
      //Regulation_TimerTemperatureIntStop_Info.periodic       = TRUE;
      //Regulation_TimerTemperatureIntStop_Info.time_value     = time_value;
        Regulation_TimerTemperatureIntStop_Info.time_remaining = Regulation_TimerTemperatureIntStop_Info.time_value;

        Regulation_UpdateVarCrc();
    }
    else
    {
        Regulation_TimerTemperatureIntStop_Info.status         = FALSE;
        Regulation_TimerTemperatureIntStop_Info.periodic       = FALSE;
        Regulation_TimerTemperatureIntStop_Info.time_value     = 0;
        Regulation_TimerTemperatureIntStop_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_T_INT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Regulation_AdlCallback_Timer_TimerTemperatureExtStart
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Regulation_AdlCallback_Timer_TimerTemperatureExtStart(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE EXT START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Regulation_TimerTemperatureExtStart_Info.periodic)
    {
      //Regulation_TimerTemperatureExtStart_Info.status         = TRUE;
      //Regulation_TimerTemperatureExtStart_Info.periodic       = TRUE;
      //Regulation_TimerTemperatureExtStart_Info.time_value     = time_value;
        Regulation_TimerTemperatureExtStart_Info.time_remaining = Regulation_TimerTemperatureExtStart_Info.time_value;

        Regulation_UpdateVarCrc();
    }
    else
    {
        Regulation_TimerTemperatureExtStart_Info.status         = FALSE;
        Regulation_TimerTemperatureExtStart_Info.periodic       = FALSE;
        Regulation_TimerTemperatureExtStart_Info.time_value     = 0;
        Regulation_TimerTemperatureExtStart_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_T_EXT_START;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Regulation_AdlCallback_Timer_TimerTemperatureExtStop
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Regulation_AdlCallback_Timer_TimerTemperatureExtStop(void *context)
{
    ql_event_t event;
    QlOSStatus err;



    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - TEMPERATURE EXT STOP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Regulation_TimerTemperatureExtStop_Info.periodic)
    {
      //Regulation_TimerTemperatureExtStop_Info.status         = TRUE;
      //Regulation_TimerTemperatureExtStop_Info.periodic       = TRUE;
      //Regulation_TimerTemperatureExtStop_Info.time_value     = time_value;
        Regulation_TimerTemperatureExtStop_Info.time_remaining = Regulation_TimerTemperatureExtStop_Info.time_value;

        Regulation_UpdateVarCrc();
    }
    else
    {
        Regulation_TimerTemperatureExtStop_Info.status         = FALSE;
        Regulation_TimerTemperatureExtStop_Info.periodic       = FALSE;
        Regulation_TimerTemperatureExtStop_Info.time_value     = 0;
        Regulation_TimerTemperatureExtStop_Info.time_remaining = 0;

        Regulation_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_T_EXT_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_Regulation, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_REGULATION, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
