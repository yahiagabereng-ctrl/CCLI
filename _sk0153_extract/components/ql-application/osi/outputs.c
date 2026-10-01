/*=============================================================================
 * File       :  OUTPUTS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - digital outputs manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdlib.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "f_regulation.h"
#include "fw_config.h"
#include "mode.h"
#include "outputs.h"
#include "program_flash.h"
#include "regulation.h"
#include "startup.h"
#include "temperature.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* period of backup of temperature output timer (sec) */
#define TIME_MS_ELAPSED_TIME_BACKUP       (1 * 60 * 1000L)

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING           200

/* output manual status */
#define OUTPUT_MANUAL_OFF                 0     /* output manual status: output off                                */
#define OUTPUT_MANUAL_ON                  1     /* output manual status: output on                                 */
#define OUTPUT_MANUAL_REGULATION          2     /* output manual status: output regulation ("regulation" function) */

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__MANUAL_INT__OFF      (11900 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature manual command      off */
#define TASK_MSG_ID__MANUAL_INT__ON       (11901 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature manual command      on  */
#define TASK_MSG_ID__ANTIFROST_INT__OFF   (11902 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature antifrost  function off */
#define TASK_MSG_ID__ANTIFROST_INT__ON    (11903 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature antifrost  function on  */
#define TASK_MSG_ID__REGULATION_INT__OFF  (11904 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation function off */
#define TASK_MSG_ID__REGULATION_INT__ON   (11905 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature regulation function on  */
#define TASK_MSG_ID__CHRONO_INT__OFF      (11906 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature chrono     function off */
#define TASK_MSG_ID__CHRONO_INT__ON       (11907 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature chrono     function on  */
#define TASK_MSG_ID__TIMEOUT_OUTPUT_INT   (11908 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature output timeout          */

#define TASK_MSG_ID__MANUAL_EXT__OFF      (11909 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature manual command      off */
#define TASK_MSG_ID__MANUAL_EXT__ON       (11910 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature manual command      on  */
#define TASK_MSG_ID__ANTIFROST_EXT__OFF   (11911 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature antifrost  function off */
#define TASK_MSG_ID__ANTIFROST_EXT__ON    (11912 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature antifrost  function on  */
#define TASK_MSG_ID__REGULATION_EXT__OFF  (11913 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation function off */
#define TASK_MSG_ID__REGULATION_EXT__ON   (11914 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature regulation function on  */
#define TASK_MSG_ID__CHRONO_EXT__OFF      (11915 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature chrono     function off */
#define TASK_MSG_ID__CHRONO_EXT__ON       (11916 | (QL_COMPONENT_APP_START << 16))    /* event - internal temperature chrono     function on  */
#define TASK_MSG_ID__TIMEOUT_OUTPUT_EXT   (11917 | (QL_COMPONENT_APP_START << 16))    /* event - external temperature output timeout          */




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
/* output manual status */
static u8                                      Outputs_OutputManualStatusInt;   // = OUTPUT_MANUAL_OFF;
static u8                                      Outputs_OutputManualStatusExt;   // = OUTPUT_MANUAL_OFF;

/* internal/external antifrost  function status */
static bool                                    Outputs_FAntifrostInt;           // = FALSE;
static s16                                     Outputs_FAntifrostIntTemp;       // = 0;
static bool                                    Outputs_FAntifrostExt;           // = FALSE;
static s16                                     Outputs_FAntifrostExtTemp;       // = 0;

/* internal/external regulation function status */
static bool                                    Outputs_FRegulationInt;          // = FALSE;
static s16                                     Outputs_FRegulationIntTemp;      // = 0;
static bool                                    Outputs_FRegulationExt;          // = FALSE;
static s16                                     Outputs_FRegulationExtTemp;      // = 0;

/* internal/external chrono     function status */
static bool                                    Outputs_FChronoInt;              // = FALSE;
static s16                                     Outputs_FChronoIntTemp;          // = 0;
static bool                                    Outputs_FChronoExt;              // = FALSE;
static s16                                     Outputs_FChronoExtTemp;          // = 0;

/* internal/external temperature output timer status */
static bool                                    Outputs_TimerOutputInt;          // = FALSE;
static bool                                    Outputs_TimerOutputExt;          // = FALSE;

/* timer info */
static TIMER_INFO                              Outputs_TimerOutputInt_Info;
static TIMER_INFO                              Outputs_TimerOutputExt_Info;

/* CRC variables */
static u16                                     Outputs_VarCrc;                  // = 0x0000;


/* debug string */
static ascii                                   Outputs_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
/* configuration - internal timer output */
static       OUTPUTS__CONFIG__TIMER_OUTPUT_INT Outputs_Config_TimerOutputInt;
static const OUTPUTS__CONFIG__TIMER_OUTPUT_INT Outputs_Config_TimerOutputIntDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    30,                                      /* output activation time value */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    0,                                       /* output activation time value */
#endif

    OUTPUTS__ACTIVATION_TIME_UNIT__SECOND,   /* output activation time unit  */
};


/* configuration - external timer output */
static       OUTPUTS__CONFIG__TIMER_OUTPUT_EXT Outputs_Config_TimerOutputExt;
static const OUTPUTS__CONFIG__TIMER_OUTPUT_EXT Outputs_Config_TimerOutputExtDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    30,                                      /* output activation time value */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    0,                                       /* output activation time value */
#endif

    OUTPUTS__ACTIVATION_TIME_UNIT__SECOND,   /* output activation time unit  */
};


/*---------------------------------------------------------------------------
 * Status
 *---------------------------------------------------------------------------*/
/* status - indication of OUT1 output manual status */
static       OUTPUTS__STATUS__OUTPUT_OUT1      Outputs_Status_OutputOut1;
static const OUTPUTS__STATUS__OUTPUT_OUT1      Outputs_Status_OutputOut1Default =
{
    OUTPUT_MANUAL_OFF                        /* indication of OUT1 output manual status */
};


/* status - indication of OUT2 output manual status */
static       OUTPUTS__STATUS__OUTPUT_OUT2      Outputs_Status_OutputOut2;
static const OUTPUTS__STATUS__OUTPUT_OUT2      Outputs_Status_OutputOut2Default =
{
    OUTPUT_MANUAL_OFF                        /* indication of OUT2 output manual status */
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static ql_timer_t                              Outputs_TimerHandler_TimerOutputInt;         /* timer for internal temperature output        */
static ql_timer_t                              Outputs_TimerHandler_TimerOutputExt;         /* timer for external temperature output        */
static ql_timer_t                              Outputs_TimerHandler_TimerOutputInt_Backup;  /* timer for internal temperature output backup */
static ql_timer_t                              Outputs_TimerHandler_TimerOutputExt_Backup;  /* timer for external temperature output backup */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Outputs_TaskOutputs(void *argument);

/* status variables */
static void Outputs_InitVar        (void);
static u16  Outputs_CalculateVarCrc(void);
       void Outputs_UpdateVarCrc   (void);
       bool Outputs_VerifyVarCrc   (void);

/* get/set "internal timer output" configuration */
       void Outputs_Config_TimerOutputInt_GetDefault(OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);
       void Outputs_Config_TimerOutputInt_Get       (OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);
       void Outputs_Config_TimerOutputInt_Set       (OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);
       bool Outputs_Config_TimerOutputInt_IsValid   (OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);

/* get/set "external timer output" configuration */
       void Outputs_Config_TimerOutputExt_GetDefault(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);
       void Outputs_Config_TimerOutputExt_Get       (OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);
       void Outputs_Config_TimerOutputExt_Set       (OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);
       bool Outputs_Config_TimerOutputExt_IsValid   (OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);

/* get/set "indication of OUT1 output manual status" configuration */
       void Outputs_Status_OutputOut1_GetDefault    (OUTPUTS__STATUS__OUTPUT_OUT1      *ptr_data);
       void Outputs_Status_OutputOut1_Get           (OUTPUTS__STATUS__OUTPUT_OUT1      *ptr_data);
       void Outputs_Status_OutputOut1_Set           (OUTPUTS__STATUS__OUTPUT_OUT1      *ptr_data);

/* get/set "indication of OUT2 output manual status" configuration */
       void Outputs_Status_OutputOut2_GetDefault    (OUTPUTS__STATUS__OUTPUT_OUT2      *ptr_data);
       void Outputs_Status_OutputOut2_Get           (OUTPUTS__STATUS__OUTPUT_OUT2      *ptr_data);
       void Outputs_Status_OutputOut2_Set           (OUTPUTS__STATUS__OUTPUT_OUT2      *ptr_data);

/* task events */

/* internal temperature */
       bool Outputs_Event_ManualInt_Off     (void);
       bool Outputs_Event_ManualInt_On      (void);
       bool Outputs_Event_FAntifrostInt_Off (void);
       bool Outputs_Event_FAntifrostInt_On  (s16 temperature);
       bool Outputs_Event_FRegulationInt_Off(void);
       bool Outputs_Event_FRegulationInt_On (s16 temperature);
       bool Outputs_Event_FChronoInt_Off    (void);
       bool Outputs_Event_FChronoInt_On     (s16 temperature);

/* external temperature */
       bool Outputs_Event_ManualExt_Off     (void);
       bool Outputs_Event_ManualExt_On      (void);
       bool Outputs_Event_FAntifrostExt_Off (void);
       bool Outputs_Event_FAntifrostExt_On  (s16 temperature);
       bool Outputs_Event_FRegulationExt_Off(void);
       bool Outputs_Event_FRegulationExt_On (s16 temperature);
       bool Outputs_Event_FChronoExt_Off    (void);
       bool Outputs_Event_FChronoExt_On     (s16 temperature);

/* regulation function stop */
static void Outputs_FRegulationIntStop(void);
static void Outputs_FRegulationExtStop(void);

/* utilities */
static bool Outputs_RegulationTemperatureInt(s16 *ptr_temperature_reg);
static bool Outputs_RegulationTemperatureExt(s16 *ptr_temperature_reg);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void Outputs_TimerOutputInt_Start       (u32 time_value, bool periodic);
static void Outputs_TimerOutputInt_Stop        (void);

static void Outputs_TimerOutputExt_Start       (u32 time_value, bool periodic);
static void Outputs_TimerOutputExt_Stop        (void);

static void Outputs_TimerOutputInt_Backup_Start(u32 time_value, bool periodic);
static void Outputs_TimerOutputInt_Backup_Stop (void);

static void Outputs_TimerOutputExt_Backup_Start(u32 time_value, bool periodic);
static void Outputs_TimerOutputExt_Backup_Stop (void);

/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Outputs_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* timer   callback functions */
static void Outputs_AdlCallback_Timer_TimerOutputInt       (void *context);
static void Outputs_AdlCallback_Timer_TimerOutputExt       (void *context);
static void Outputs_AdlCallback_Timer_TimerOutputInt_Backup(void *context);
static void Outputs_AdlCallback_Timer_TimerOutputExt_Backup(void *context);




/*=============================================================================
 * Function   : Outputs_TaskOutputs
 *
 * Description: output manager task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Outputs_TaskOutputs(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - OUTPUTS - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* creation of timer "TimerOutputInt" */
    err = ql_rtos_timer_create(&Outputs_TimerHandler_TimerOutputInt, QL_TIMER_IN_SERVICE, Outputs_AdlCallback_Timer_TimerOutputInt, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOutputExt" */
    err = ql_rtos_timer_create(&Outputs_TimerHandler_TimerOutputExt, QL_TIMER_IN_SERVICE, Outputs_AdlCallback_Timer_TimerOutputExt, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOutputInt_Backup" */
    err = ql_rtos_timer_create(&Outputs_TimerHandler_TimerOutputInt_Backup, QL_TIMER_IN_SERVICE, Outputs_AdlCallback_Timer_TimerOutputInt_Backup, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOutputExt_Backup" */
    err = ql_rtos_timer_create(&Outputs_TimerHandler_TimerOutputExt_Backup, QL_TIMER_IN_SERVICE, Outputs_AdlCallback_Timer_TimerOutputExt_Backup, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* internal status init */
    Outputs_InitVar();


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Outputs_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : Outputs_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Outputs_InitVar(void)
{
    bool recover_status;

    /* internal/external temperature output time value (ms) */
    u32  activation_time_value_int_ms;
    u32  activation_time_value_ext_ms;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* output manual status */
        Outputs_OutputManualStatusInt = OUTPUT_MANUAL_OFF;
        Outputs_OutputManualStatusExt = OUTPUT_MANUAL_OFF;

        /* internal/external antifrost  function status */
        Outputs_FAntifrostInt      = FALSE;
        Outputs_FAntifrostIntTemp  = 0;
        Outputs_FAntifrostExt      = FALSE;
        Outputs_FAntifrostExtTemp  = 0;

        /* internal/external regulation function status */
        Outputs_FRegulationInt     = FALSE;
        Outputs_FRegulationIntTemp = 0;
        Outputs_FRegulationExt     = FALSE;
        Outputs_FRegulationExtTemp = 0;

        /* internal/external chrono     function status */
        Outputs_FChronoInt         = FALSE;
        Outputs_FChronoIntTemp     = 0;
        Outputs_FChronoExt         = FALSE;
        Outputs_FChronoExtTemp     = 0;

        /* internal/external temperature output timer status */
        Outputs_TimerOutputInt = FALSE;
        Outputs_TimerOutputExt = FALSE;

        /* timer info */
        Outputs_TimerOutputInt_Info.status         = FALSE;
        Outputs_TimerOutputInt_Info.periodic       = FALSE;
        Outputs_TimerOutputInt_Info.time_value     = 0;
        Outputs_TimerOutputInt_Info.time_remaining = 0;

        /* timer info */
        Outputs_TimerOutputExt_Info.status         = FALSE;
        Outputs_TimerOutputExt_Info.periodic       = FALSE;
        Outputs_TimerOutputExt_Info.time_value     = 0;
        Outputs_TimerOutputExt_Info.time_remaining = 0;


        /**** update the variable CRC ****/
        Outputs_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        Outputs_OutputManualStatusInt = Outputs_Status_OutputOut1.manual_status;
        if (Outputs_OutputManualStatusInt == OUTPUT_MANUAL_ON)
        {
            /* enable OUT1 */
            Regulation_RegulationTemperatureInt_On();

            /* start timer for internal temperature output */
            if (Outputs_Config_TimerOutputInt.activation_time_value > 0)
            {
                /* calculate activation time in ms */
                if      (Outputs_Config_TimerOutputInt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__SECOND)
                    activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value          ) * 1000;
                else if (Outputs_Config_TimerOutputInt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE)
                    activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value *      60) * 1000;
                else if (Outputs_Config_TimerOutputInt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__HOUR  )
                    activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value * 60 * 60) * 1000;
                else
                    activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value          ) * 1000;

                /* start timer for internal temperature output */
                Outputs_TimerOutputInt = TRUE;
                Outputs_TimerOutputInt_Start(activation_time_value_int_ms, FALSE);

                /* start timer for internal temperature output backup */
                Outputs_TimerOutputInt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP, TRUE);
            }
        }
        else
        {
            /* disable OUT1 */
            Regulation_RegulationTemperatureInt_Off();
        }

        Outputs_OutputManualStatusExt = Outputs_Status_OutputOut2.manual_status;
        if (Outputs_OutputManualStatusExt == OUTPUT_MANUAL_ON)
        {
            /* enable OUT2 */
            Regulation_RegulationTemperatureExt_On();

            /* start timer for external temperature output */
            if (Outputs_Config_TimerOutputExt.activation_time_value > 0)
            {
                /* calculate activation time in ms */
                if      (Outputs_Config_TimerOutputExt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__SECOND)
                    activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value          ) * 1000;
                else if (Outputs_Config_TimerOutputExt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE)
                    activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value *      60) * 1000;
                else if (Outputs_Config_TimerOutputExt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__HOUR  )
                    activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value * 60 * 60) * 1000;
                else
                    activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value          ) * 1000;

                /* start timer for external temperature output */
                Outputs_TimerOutputExt = TRUE;
                Outputs_TimerOutputExt_Start(activation_time_value_ext_ms, FALSE);

                /* start timer for external temperature output backup */
                Outputs_TimerOutputExt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP, TRUE);
            }
        }
        else
        {
            /* disable OUT2 */
            Regulation_RegulationTemperatureExt_Off();
        }


        /**** update the variable CRC ****/
        Outputs_UpdateVarCrc();
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time

        if (Outputs_TimerOutputInt_Info.status)
        {
            if (Outputs_TimerOutputInt_Info.periodic)
            {
                Outputs_TimerOutputInt_Start       (Outputs_TimerOutputInt_Info.time_value, TRUE);
                Outputs_TimerOutputInt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP           , TRUE);
            }
            else
            {
                if (Outputs_TimerOutputInt_Info.time_remaining > 0)
                {
                    Outputs_TimerOutputInt_Start       (Outputs_TimerOutputInt_Info.time_remaining, FALSE);
                    Outputs_TimerOutputInt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP               , TRUE );
                }
            }
        }

        if (Outputs_TimerOutputExt_Info.status)
        {
            if (Outputs_TimerOutputExt_Info.periodic)
            {
                Outputs_TimerOutputExt_Start       (Outputs_TimerOutputExt_Info.time_value, TRUE);
                Outputs_TimerOutputExt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP           , TRUE);
            }
            else
            {
                if (Outputs_TimerOutputExt_Info.time_remaining > 0)
                {
                    Outputs_TimerOutputExt_Start       (Outputs_TimerOutputExt_Info.time_remaining, FALSE);
                    Outputs_TimerOutputExt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP               , TRUE );
                }
            }
        }


        /**** other ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : Outputs_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 Outputs_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&Outputs_OutputManualStatusInt, sizeof(Outputs_OutputManualStatusInt), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_OutputManualStatusExt, sizeof(Outputs_OutputManualStatusExt), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FAntifrostInt        , sizeof(Outputs_FAntifrostInt        ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FAntifrostIntTemp    , sizeof(Outputs_FAntifrostIntTemp    ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FAntifrostExt        , sizeof(Outputs_FAntifrostExt        ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FAntifrostExtTemp    , sizeof(Outputs_FAntifrostExtTemp    ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FRegulationInt       , sizeof(Outputs_FRegulationInt       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FRegulationIntTemp   , sizeof(Outputs_FRegulationIntTemp   ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FRegulationExt       , sizeof(Outputs_FRegulationExt       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FRegulationExtTemp   , sizeof(Outputs_FRegulationExtTemp   ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FChronoInt           , sizeof(Outputs_FChronoInt           ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FChronoIntTemp       , sizeof(Outputs_FChronoIntTemp       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FChronoExt           , sizeof(Outputs_FChronoExt           ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_FChronoExtTemp       , sizeof(Outputs_FChronoExtTemp       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_TimerOutputInt       , sizeof(Outputs_TimerOutputInt       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_TimerOutputExt       , sizeof(Outputs_TimerOutputExt       ), crc);

    /* timers info */
    crc = Utility_CalculateCRC16((u8 *)&Outputs_TimerOutputInt_Info  , sizeof(Outputs_TimerOutputInt_Info  ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Outputs_TimerOutputExt_Info  , sizeof(Outputs_TimerOutputExt_Info  ), crc);

    return crc;
}




/*=============================================================================
 * Function   : Outputs_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Outputs_UpdateVarCrc(void)
{
    Outputs_VarCrc = Outputs_CalculateVarCrc();
}




/*=============================================================================
 * Function   : Outputs_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool Outputs_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = Outputs_CalculateVarCrc();

    if (Outputs_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputInt_GetDefault
 *
 * Description: get the default "internal timer output" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Config_TimerOutputInt_GetDefault(OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data)
{
    *ptr_data = Outputs_Config_TimerOutputIntDefault;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputInt_Get
 *
 * Description: get the "internal timer output" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Config_TimerOutputInt_Get(OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data)
{
    *ptr_data = Outputs_Config_TimerOutputInt;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputInt_Set
 *
 * Description: set the "internal timer output" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Outputs_Config_TimerOutputInt_Set(OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data)
{
    Outputs_Config_TimerOutputInt = *ptr_data;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputInt_IsValid
 *
 * Description: check if the "internal timer output" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Outputs_Config_TimerOutputInt_IsValid(OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data)
{
    if (
         (ptr_data->activation_time_unit != OUTPUTS__ACTIVATION_TIME_UNIT__SECOND) &&
         (ptr_data->activation_time_unit != OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE) &&
         (ptr_data->activation_time_unit != OUTPUTS__ACTIVATION_TIME_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputExt_GetDefault
 *
 * Description: get the default "external timer output" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Config_TimerOutputExt_GetDefault(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data)
{
    *ptr_data = Outputs_Config_TimerOutputExtDefault;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputExt_Get
 *
 * Description: get the "external timer output" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Config_TimerOutputExt_Get(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data)
{
    *ptr_data = Outputs_Config_TimerOutputExt;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputExt_Set
 *
 * Description: set the "external timer output" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Outputs_Config_TimerOutputExt_Set(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data)
{
    Outputs_Config_TimerOutputExt = *ptr_data;
}




/*===========================================================================
 * Function   : Outputs_Config_TimerOutputExt_IsValid
 *
 * Description: check if the "external timer output" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Outputs_Config_TimerOutputExt_IsValid(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data)
{
    if (
         (ptr_data->activation_time_unit != OUTPUTS__ACTIVATION_TIME_UNIT__SECOND) &&
         (ptr_data->activation_time_unit != OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE) &&
         (ptr_data->activation_time_unit != OUTPUTS__ACTIVATION_TIME_UNIT__HOUR  )
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Status_OutputOut1_GetDefault
 *
 * Description: get the default "indication of OUT1 output manual status" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Status_OutputOut1_GetDefault(OUTPUTS__STATUS__OUTPUT_OUT1 *ptr_data)
{
    *ptr_data = Outputs_Status_OutputOut1Default;
}




/*===========================================================================
 * Function   : Outputs_Status_OutputOut1_Get
 *
 * Description: get the "indication of OUT1 output manual status" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Status_OutputOut1_Get(OUTPUTS__STATUS__OUTPUT_OUT1 *ptr_data)
{
    *ptr_data = Outputs_Status_OutputOut1;
}




/*===========================================================================
 * Function   : Outputs_Status_OutputOut1_Set
 *
 * Description: set the "indication of OUT1 output manual status" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Outputs_Status_OutputOut1_Set(OUTPUTS__STATUS__OUTPUT_OUT1 *ptr_data)
{
    Outputs_Status_OutputOut1 = *ptr_data;
}




/*===========================================================================
 * Function   : Outputs_Status_OutputOut2_GetDefault
 *
 * Description: get the default "indication of OUT2 output manual status" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Status_OutputOut2_GetDefault(OUTPUTS__STATUS__OUTPUT_OUT2 *ptr_data)
{
    *ptr_data = Outputs_Status_OutputOut2Default;
}




/*===========================================================================
 * Function   : Outputs_Status_OutputOut2_Get
 *
 * Description: get the "indication of OUT2 output manual status" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Outputs_Status_OutputOut2_Get(OUTPUTS__STATUS__OUTPUT_OUT2 *ptr_data)
{
    *ptr_data = Outputs_Status_OutputOut2;
}




/*===========================================================================
 * Function   : Outputs_Status_OutputOut2_Set
 *
 * Description: set the "indication of OUT2 output manual status" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Outputs_Status_OutputOut2_Set(OUTPUTS__STATUS__OUTPUT_OUT2 *ptr_data)
{
    Outputs_Status_OutputOut2 = *ptr_data;
}




/*===========================================================================
 * Function   : Outputs_Event_ManualInt_Off
 *
 * Description: signal internal temperature manual command off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_ManualInt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__MANUAL_INT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_ManualInt_On
 *
 * Description: signal internal temperature manual command on
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_ManualInt_On(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__MANUAL_INT__ON;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FAntifrostInt_Off
 *
 * Description: signal internal temperature antifrost function off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FAntifrostInt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__ANTIFROST_INT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FAntifrostInt_On
 *
 * Description: signal internal temperature antifrost function on
 * Input      : - temperature: internal antifrost temperature (/10 °C)
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FAntifrostInt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__ANTIFROST_INT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FRegulationInt_Off
 *
 * Description: signal internal temperature regulation function off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FRegulationInt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_INT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FRegulationInt_On
 *
 * Description: signal internal temperature regulation function on
 * Input      : - temperature: internal regulation temperature (/10 °C)
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FRegulationInt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__REGULATION_INT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FChronoInt_Off
 *
 * Description: signal internal temperature chrono function off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FChronoInt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__CHRONO_INT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FChronoInt_On
 *
 * Description: signal internal temperature chrono function on
 * Input      : - temperature: internal chrono temperature (/10 °C)
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FChronoInt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__CHRONO_INT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_ManualExt_Off
 *
 * Description: signal external temperature manual command off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_ManualExt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__MANUAL_EXT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_ManualExt_On
 *
 * Description: signal external temperature manual command on
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_ManualExt_On(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__MANUAL_EXT__ON;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FAntifrostExt_Off
 *
 * Description: signal external temperature antifrost function off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FAntifrostExt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__ANTIFROST_EXT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FAntifrostExt_On
 *
 * Description: signal external temperature antifrost function on
 * Input      : - temperature: external antifrost temperature (/10 °C)
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FAntifrostExt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__ANTIFROST_EXT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FRegulationExt_Off
 *
 * Description: signal external temperature regulation function off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FRegulationExt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__REGULATION_EXT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FRegulationExt_On
 *
 * Description: signal external temperature regulation function on
 * Input      : - temperature: external regulation temperature (/10 °C)
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FRegulationExt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__REGULATION_EXT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FChronoExt_Off
 *
 * Description: signal external temperature chrono function off
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FChronoExt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__CHRONO_EXT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_Event_FChronoExt_On
 *
 * Description: signal external temperature chrono function on
 * Input      : - temperature: external chrono temperature (/10 °C)
 * Output     : -
 *===========================================================================*/
bool Outputs_Event_FChronoExt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;


    event.id     = TASK_MSG_ID__CHRONO_EXT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Outputs_FRegulationInt_Stop
 *
 * Description: stop the internal regulation function
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Outputs_FRegulationIntStop(void)
{
    /* regulation function configuration */
    F_REGULATION__STATUS_REGULATION_INT  status_f_regulation_int;
    F_REGULATION__STATUS_REGULATION_INT  status_f_regulation_int_old;


    /* build the status */
    status_f_regulation_int.status         = FALSE;
    status_f_regulation_int.temperature    = 0;
    status_f_regulation_int.duration       = 0;
    status_f_regulation_int.remaining_time = 0;

    /* get the status */
    FRegulation_Status_FRegulationInt_Get(&status_f_regulation_int_old);

    /* set the status */
    FRegulation_Status_FRegulationInt_Set(&status_f_regulation_int    );

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_INT_STATUS);

    /* signal a new status */
    FRegulation_NewTeleControlInt(&status_f_regulation_int, &status_f_regulation_int_old);
}




/*===========================================================================
 * Function   : Outputs_FRegulationExtStop
 *
 * Description: stop the external regulation function
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void Outputs_FRegulationExtStop(void)
{
    /* regulation function configuration */
    F_REGULATION__STATUS_REGULATION_EXT status_f_regulation_ext;
    F_REGULATION__STATUS_REGULATION_EXT status_f_regulation_ext_old;


    /* build the status */
    status_f_regulation_ext.status         = FALSE;
    status_f_regulation_ext.temperature    = 0;
    status_f_regulation_ext.duration       = 0;
    status_f_regulation_ext.remaining_time = 0;

    /* get the status */
    FRegulation_Status_FRegulationExt_Get(&status_f_regulation_ext_old);

    /* set the status */
    FRegulation_Status_FRegulationExt_Set(&status_f_regulation_ext    );

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__014_F_REGULATION, PROGRAM_FLASH__FLASH_ID__014_F_REGULATION__F_REGULATION_EXT_STATUS);

    /* signal a new status */
    FRegulation_NewTeleControlExt(&status_f_regulation_ext, &status_f_regulation_ext_old);
}




/*=============================================================================
 * Function   : Outputs_RegulationTemperatureInt
 *
 * Description: calculate the regulation internal temperature.
 *              It considers:
 *                - "regulation" function (if it is enabled)
 *                - "antifrost"  function (if it is enabled)
 *                - "chrono"     function (if it is enabled)
 *              It also considers the configured temperature mode (winter or summer)
 * Input      : - ptr_temperature_reg: pointer to save the regulation temperature (/10 °C)
 * Output     : - FALSE: regulation internal temperature not available
 *              - TRUE : regulation internal temperature     available
 *=============================================================================*/
static bool Outputs_RegulationTemperatureInt(s16 *ptr_temperature_reg)
{
    MODE__CONFIG__MODE_TEMPERATURE_INT config_mode_temperature_int;
    s16                                temperature_reg;


    /* get internal temperature regulation mode ("winter" or "summer" mode) */
    Mode_Config_ModeTemperatureInt_Get(&config_mode_temperature_int);


    if (config_mode_temperature_int.mode == MODE__WINTER_MODE)
    {
        /* winter mode */

        temperature_reg = -200;

        /* "regulation" function */
        if (Outputs_FRegulationInt)
                temperature_reg = Outputs_FRegulationIntTemp;

        /* "antifrost"  function */
        if (Outputs_FAntifrostInt)
        {
            if (Outputs_FAntifrostIntTemp > temperature_reg)
                temperature_reg = Outputs_FAntifrostIntTemp;
        }

        /* "chrono"     function */
        if (Outputs_FChronoInt)
        {
            if (Outputs_FChronoIntTemp    > temperature_reg)
                temperature_reg = Outputs_FChronoIntTemp;
        }
    }
    else
    {
        /* summer mode */

        temperature_reg = 1000;

        /* "regulation" function */
        if (Outputs_FRegulationInt)
                temperature_reg = Outputs_FRegulationIntTemp;

        /* "antifrost"  function */    // "antifrost" function  not considered in "summer" mode
      //if (Outputs_FAntifrostInt)
      //{
      //    if (Outputs_FAntifrostIntTemp < temperature_reg)
      //        temperature_reg = Outputs_FAntifrostIntTemp;
      //}

        /* "chrono"     function */
        if (Outputs_FChronoInt)
        {
            if (Outputs_FChronoIntTemp    < temperature_reg)
                temperature_reg = Outputs_FChronoIntTemp;
        }
    }


    if (
         (Outputs_FRegulationInt) ||
         (Outputs_FAntifrostInt ) ||
         (Outputs_FChronoInt    )
       )
    {
        *ptr_temperature_reg = temperature_reg;
        return TRUE;
    }
    else
    {
        *ptr_temperature_reg = 0;
        return FALSE;
    }
}




/*=============================================================================
 * Function   : Outputs_RegulationTemperatureExt
 *
 * Description: calculate the regulation external temperature.
 *              It considers:
 *                - "regulation" function (if it is enabled)
 *                - "antifrost"  function (if it is enabled)
 *                - "chrono"     function (if it is enabled)
 *              It also considers the configured temperature mode (winter or summer)
 * Input      : - ptr_temperature_reg: pointer to save the regulation temperature (/10 °C)
 * Output     : - FALSE: regulation external temperature not available
 *              - TRUE : regulation external temperature     available
 *=============================================================================*/
static bool Outputs_RegulationTemperatureExt(s16 *ptr_temperature_reg)
{
    MODE__CONFIG__MODE_TEMPERATURE_EXT config_mode_temperature_ext;
    s16                                temperature_reg;


    /* get external temperature regulation mode ("winter" or "summer" mode) */
    Mode_Config_ModeTemperatureExt_Get(&config_mode_temperature_ext);


    if (config_mode_temperature_ext.mode == MODE__WINTER_MODE)
    {
        /* winter mode */

        temperature_reg = -200;

        /* "regulation" function */
        if (Outputs_FRegulationExt)
                temperature_reg = Outputs_FRegulationExtTemp;

        /* "antifrost"  function */
        if (Outputs_FAntifrostExt)
        {
            if (Outputs_FAntifrostExtTemp > temperature_reg)
                temperature_reg = Outputs_FAntifrostExtTemp;
        }

        /* "chrono"     function */
        if (Outputs_FChronoExt)
        {
            if (Outputs_FChronoExtTemp    > temperature_reg)
                temperature_reg = Outputs_FChronoExtTemp;
        }
    }
    else
    {
        /* summer mode */

        temperature_reg = 1000;

        /* "regulation" function */
        if (Outputs_FRegulationExt)
                temperature_reg = Outputs_FRegulationExtTemp;

        /* "antifrost"  function */
        if (Outputs_FAntifrostExt)
        {
            if (Outputs_FAntifrostExtTemp < temperature_reg)
                temperature_reg = Outputs_FAntifrostExtTemp;
        }

        /* "chrono"     function */
        if (Outputs_FChronoExt)
        {
            if (Outputs_FChronoExtTemp    < temperature_reg)
                temperature_reg = Outputs_FChronoExtTemp;
        }
    }


    if (
         (Outputs_FRegulationExt) ||
         (Outputs_FAntifrostExt ) ||
         (Outputs_FChronoExt    )
       )
    {
        *ptr_temperature_reg = temperature_reg;
        return TRUE;
    }
    else
    {
        *ptr_temperature_reg = 0;
        return FALSE;
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputInt_Start
 *
 * Description: start the "internal temperature output" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputInt_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Start \"internal temperature output\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Outputs_TimerHandler_TimerOutputInt, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Outputs_TimerOutputInt_Info.status         = TRUE;
        Outputs_TimerOutputInt_Info.periodic       = periodic;
        Outputs_TimerOutputInt_Info.time_value     = time_value;
        Outputs_TimerOutputInt_Info.time_remaining = time_value;

        Outputs_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputInt_Stop
 *
 * Description: stop the "internal temperature output" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputInt_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Stop \"internal temperature output\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Outputs_TimerHandler_TimerOutputInt);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Outputs_DebugString, sizeof(Outputs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, Outputs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Outputs_TimerOutputInt_Info.status         = FALSE;
        Outputs_TimerOutputInt_Info.periodic       = FALSE;
        Outputs_TimerOutputInt_Info.time_value     = 0;
        Outputs_TimerOutputInt_Info.time_remaining = 0;

        Outputs_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputExt_Start
 *
 * Description: start the "external temperature output" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputExt_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Start \"external temperature output\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Outputs_TimerHandler_TimerOutputExt, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Outputs_TimerOutputExt_Info.status         = TRUE;
        Outputs_TimerOutputExt_Info.periodic       = periodic;
        Outputs_TimerOutputExt_Info.time_value     = time_value;
        Outputs_TimerOutputExt_Info.time_remaining = time_value;

        Outputs_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputExt_Stop
 *
 * Description: stop the "external temperature output" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputExt_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Stop \"external temperature output\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Outputs_TimerHandler_TimerOutputExt);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Outputs_DebugString, sizeof(Outputs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, Outputs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Outputs_TimerOutputExt_Info.status         = FALSE;
        Outputs_TimerOutputExt_Info.periodic       = FALSE;
        Outputs_TimerOutputExt_Info.time_value     = 0;
        Outputs_TimerOutputExt_Info.time_remaining = 0;

        Outputs_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputInt_Backup_Start
 *
 * Description: start the "internal temperature output backup" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputInt_Backup_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Start \"internal temperature output backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Outputs_TimerHandler_TimerOutputInt_Backup, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputInt_Backup_Stop
 *
 * Description: stop the "internal temperature output backup" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputInt_Backup_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Stop \"internal temperature output backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Outputs_TimerHandler_TimerOutputInt_Backup);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Outputs_DebugString, sizeof(Outputs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, Outputs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputExt_Backup_Start
 *
 * Description: start the "external temperature output backup" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputExt_Backup_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Start \"external temperature output backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Outputs_TimerHandler_TimerOutputExt_Backup, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Outputs_TimerOutputExt_Backup_Stop
 *
 * Description: stop the "external temperature output backup" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Outputs_TimerOutputExt_Backup_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "Stop \"external temperature output backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Outputs_TimerHandler_TimerOutputExt_Backup);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Outputs_DebugString, sizeof(Outputs_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, Outputs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Outputs_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - msg_identifier:
 *              - source        :
 *              - length        :
 *              - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void Outputs_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    /* output manual status */
    u8    old_output_manual_status_int;
    u8    old_output_manual_status_ext;

    /* internal/external antifrost  function status */
    bool  old_f_antifrost_int;
    s16   old_f_antifrost_int_temp;
    bool  old_f_antifrost_ext;
    s16   old_f_antifrost_ext_temp;

    /* internal/external regulation function status */
    bool  old_f_regulation_int;
    s16   old_f_regulation_int_temp;
    bool  old_f_regulation_ext;
    s16   old_f_regulation_ext_temp;

    /* internal/external chrono     function status */
    bool  old_f_chrono_int;
    s16   old_f_chrono_int_temp;
    bool  old_f_chrono_ext;
    s16   old_f_chrono_ext_temp;

    /* internal/external temperature output timer status */
    bool  old_timer_output_int;
    bool  old_timer_output_ext;


    /* internal/external temperature output time value (ms) */
    u32   activation_time_value_int_ms;
    u32   activation_time_value_ext_ms;

    /* indication of internal/external manual          event */
    bool  manual_event_int;
    bool  manual_event_ext;

    /* indication of internal/external function status event */
    bool  function_status_event_int;
    bool  function_status_event_ext;

    /* internal/external regulation temperature (/10 °C) */
    s16   temperature_reg_int;
    s16   temperature_reg_ext;

    bool  result;


    /* debug */
    snprintf(Outputs_DebugString, sizeof(Outputs_DebugString), "CALLBACK     - MESSAGE     - TASK OUTPUTS - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, Outputs_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* output manual status */
    old_output_manual_status_int = Outputs_OutputManualStatusInt;
    old_output_manual_status_ext = Outputs_OutputManualStatusExt;

    /* internal/external antifrost  function status */
    old_f_antifrost_int          = Outputs_FAntifrostInt;
    old_f_antifrost_int_temp     = Outputs_FAntifrostIntTemp;
    old_f_antifrost_ext          = Outputs_FAntifrostExt;
    old_f_antifrost_ext_temp     = Outputs_FAntifrostExtTemp;

    /* internal/external regulation function status */
    old_f_regulation_int         = Outputs_FRegulationInt;
    old_f_regulation_int_temp    = Outputs_FRegulationIntTemp;
    old_f_regulation_ext         = Outputs_FRegulationExt;
    old_f_regulation_ext_temp    = Outputs_FRegulationExtTemp;

    /* internal/external chrono     function status */
    old_f_chrono_int             = Outputs_FChronoInt;
    old_f_chrono_int_temp        = Outputs_FChronoIntTemp;
    old_f_chrono_ext             = Outputs_FChronoExt;
    old_f_chrono_ext_temp        = Outputs_FChronoExtTemp;

    /* internal/external temperature output timer status */
    old_timer_output_int         = Outputs_TimerOutputInt;
    old_timer_output_ext         = Outputs_TimerOutputExt;



    /*-----------------------------------------------------------------------------
     * Disable "regulation" function (if there is a manual command)
     *-----------------------------------------------------------------------------*/

    /*=========================
     * internal temperature
     *=========================*/
    switch (msg_identifier->id)
    {
        /* event - internal temperature manual command off */
        /* event - internal temperature manual command on  */
        case TASK_MSG_ID__MANUAL_INT__OFF:
        case TASK_MSG_ID__MANUAL_INT__ON:
            /* disable regulation function (if it is enabled) */
            if (Outputs_FRegulationInt)
            {
                Outputs_FRegulationInt     = FALSE;
                Outputs_FRegulationIntTemp = 0;

                Outputs_FRegulationIntStop();
            }
            break;
    }


    /*=========================
     * external temperature
     *=========================*/
    switch (msg_identifier->id)
    {
        /* event - external temperature manual command off */
        /* event - external temperature manual command on  */
        case TASK_MSG_ID__MANUAL_EXT__OFF:
        case TASK_MSG_ID__MANUAL_EXT__ON:
            /* disable regulation function (if it is enabled) */
            if (Outputs_FRegulationExt)
            {
                Outputs_FRegulationExt     = FALSE;
                Outputs_FRegulationExtTemp = 0;

                Outputs_FRegulationExtStop();
            }
            break;
    }



    /*-----------------------------------------------------------------------------
     * Determine new output manual status (off, on, regulation)
     *-----------------------------------------------------------------------------*/

    manual_event_int = FALSE;
    manual_event_ext = FALSE;

    /*=========================
     * internal temperature
     *=========================*/

    switch (msg_identifier->id)
    {
        /* event - internal temperature manual command off */
        case TASK_MSG_ID__MANUAL_INT__OFF:
            Outputs_OutputManualStatusInt = OUTPUT_MANUAL_OFF;
            manual_event_int = TRUE;
            break;

        /* event - internal temperature manual command on  */
        case TASK_MSG_ID__MANUAL_INT__ON:
            Outputs_OutputManualStatusInt = OUTPUT_MANUAL_ON;
            manual_event_int = TRUE;
            break;

        /* event - internal temperature regulation function on  */
        case TASK_MSG_ID__REGULATION_INT__ON:
            Outputs_OutputManualStatusInt = OUTPUT_MANUAL_REGULATION;
            manual_event_int = TRUE;
            break;


        /* event - internal temperature output timeout */
        case TASK_MSG_ID__TIMEOUT_OUTPUT_INT:
            if (Outputs_OutputManualStatusInt == OUTPUT_MANUAL_ON)
            {
                Outputs_OutputManualStatusInt = OUTPUT_MANUAL_OFF;
                manual_event_int = TRUE;
            }
            break;

        /* event - internal temperature regulation function off */
        case TASK_MSG_ID__REGULATION_INT__OFF:
            if (Outputs_OutputManualStatusInt == OUTPUT_MANUAL_REGULATION)
            {
                Outputs_OutputManualStatusInt = OUTPUT_MANUAL_OFF;
                manual_event_int = TRUE;
            }
            break;
    }

    if (manual_event_int)
    {
        Outputs_Status_OutputOut1.manual_status = Outputs_OutputManualStatusInt;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__012_OUTPUT_INFO, PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT1_STATUS);
    }


    /*=========================
     * external temperature
     *=========================*/
    switch (msg_identifier->id)
    {
        /* event - external temperature manual command off */
        case TASK_MSG_ID__MANUAL_EXT__OFF:
            Outputs_OutputManualStatusExt = OUTPUT_MANUAL_OFF;
            manual_event_ext = TRUE;
            break;

        /* event - external temperature manual command on  */
        case TASK_MSG_ID__MANUAL_EXT__ON:
            Outputs_OutputManualStatusExt = OUTPUT_MANUAL_ON;
            manual_event_ext = TRUE;
            break;

        /* event - external temperature regulation function on  */
        case TASK_MSG_ID__REGULATION_EXT__ON:
            Outputs_OutputManualStatusExt = OUTPUT_MANUAL_REGULATION;
            manual_event_ext = TRUE;
            break;


        /* event - external temperature output timeout */
        case TASK_MSG_ID__TIMEOUT_OUTPUT_EXT:
            if (Outputs_OutputManualStatusExt == OUTPUT_MANUAL_ON)
            {
                Outputs_OutputManualStatusExt = OUTPUT_MANUAL_OFF;
                manual_event_ext = TRUE;
            }
            break;

        /* event - external temperature regulation function off */
        case TASK_MSG_ID__REGULATION_EXT__OFF:
            if (Outputs_OutputManualStatusExt == OUTPUT_MANUAL_REGULATION)
            {
                Outputs_OutputManualStatusExt = OUTPUT_MANUAL_OFF;
                manual_event_ext = TRUE;
            }
            break;
    }

    if (manual_event_ext)
    {
        Outputs_Status_OutputOut2.manual_status = Outputs_OutputManualStatusExt;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__012_OUTPUT_INFO, PROGRAM_FLASH__FLASH_ID__012_OUTPUT_INFO__OUTPUT_OUT2_STATUS);
    }



    /*-----------------------------------------------------------------------------
     * Update functions status ("regulation", "antifrost", "chrono")
     *-----------------------------------------------------------------------------*/

    function_status_event_int = FALSE;
    function_status_event_ext = FALSE;

    /*=========================
     * internal temperature
     *=========================*/
    switch (msg_identifier->id)
    {
        /* event - internal temperature regulation function on  */
        case TASK_MSG_ID__REGULATION_INT__ON:
            /* extract the parameters */
            temperature_reg_int = (s16)(msg_identifier->param1);

            Outputs_FRegulationInt     = TRUE;
            Outputs_FRegulationIntTemp = temperature_reg_int;

            function_status_event_int = TRUE;
            break;

        /* event - internal temperature regulation function off */
        case TASK_MSG_ID__REGULATION_INT__OFF:
            Outputs_FRegulationInt     = FALSE;
            Outputs_FRegulationIntTemp = 0;

            function_status_event_int = TRUE;
            break;


        /* event - internal temperature antifrost  function off */
        case TASK_MSG_ID__ANTIFROST_INT__OFF:
            Outputs_FAntifrostInt     = FALSE;
            Outputs_FAntifrostIntTemp = 0;

            function_status_event_int = TRUE;
            break;

        /* event - internal temperature antifrost  function on  */
        case TASK_MSG_ID__ANTIFROST_INT__ON:
            /* extract the parameters */
            temperature_reg_int = (s16)(msg_identifier->param1);

            Outputs_FAntifrostInt     = TRUE;
            Outputs_FAntifrostIntTemp = temperature_reg_int;

            function_status_event_int = TRUE;
            break;


        /* event - internal temperature chrono     function off */
        case TASK_MSG_ID__CHRONO_INT__OFF:
            Outputs_FChronoInt     = FALSE;
            Outputs_FChronoIntTemp = 0;

            function_status_event_int = TRUE;
            break;

        /* event - internal temperature chrono     function on  */
        case TASK_MSG_ID__CHRONO_INT__ON:
            /* extract the parameters */
            temperature_reg_int = (s16)(msg_identifier->param1);

            Outputs_FChronoInt     = TRUE;
            Outputs_FChronoIntTemp = temperature_reg_int;

            function_status_event_int = TRUE;
            break;
    }


    /*=========================
     * external temperature
     *=========================*/
    switch (msg_identifier->id)
    {
        /* event - external temperature regulation function on  */
        case TASK_MSG_ID__REGULATION_EXT__ON:
            temperature_reg_ext = (s16)(msg_identifier->param1);

            Outputs_FRegulationExt     = TRUE;
            Outputs_FRegulationExtTemp = temperature_reg_ext;

            function_status_event_ext = TRUE;
            break;

        /* event - external temperature regulation function off */
        case TASK_MSG_ID__REGULATION_EXT__OFF:
            Outputs_FRegulationExt     = FALSE;
            Outputs_FRegulationExtTemp = 0;

            function_status_event_ext = TRUE;
            break;


        /* event - external temperature antifrost  function off */
        case TASK_MSG_ID__ANTIFROST_EXT__OFF:
            Outputs_FAntifrostExt     = FALSE;
            Outputs_FAntifrostExtTemp = 0;

            function_status_event_ext = TRUE;
            break;

        /* event - external temperature antifrost  function on  */
        case TASK_MSG_ID__ANTIFROST_EXT__ON:
            /* extract the parameters */
            temperature_reg_ext = (s16)(msg_identifier->param1);

            Outputs_FAntifrostExt     = TRUE;
            Outputs_FAntifrostExtTemp = temperature_reg_ext;

            function_status_event_ext = TRUE;
            break;


        /* event - external temperature chrono     function off */
        case TASK_MSG_ID__CHRONO_EXT__OFF:
            Outputs_FChronoExt     = FALSE;
            Outputs_FChronoExtTemp = 0;

            function_status_event_ext = TRUE;
            break;

        /* event - external temperature chrono     function on  */
        case TASK_MSG_ID__CHRONO_EXT__ON:
            /* extract the parameters */
            temperature_reg_ext = (s16)(msg_identifier->param1);

            Outputs_FChronoExt     = TRUE;
            Outputs_FChronoExtTemp = temperature_reg_ext;

            function_status_event_ext = TRUE;
            break;
    }



    /*-----------------------------------------------------------------------------
     * Set the output status (off, on, regulation)
     *-----------------------------------------------------------------------------*/

    /*=========================
     * internal temperature
     *=========================*/
    if (manual_event_int || function_status_event_int)
    {
        switch (Outputs_OutputManualStatusInt)
        {
            /* output manual status: output off */
            case OUTPUT_MANUAL_OFF:
                /*** Note: it is not necessary to check "function_status_event_int" ***/

                /* stop timer for internal temperature output (if it is enabled) */
                if (Outputs_TimerOutputInt)
                {
                    Outputs_TimerOutputInt = FALSE;
                    Outputs_TimerOutputInt_Stop();
                    Outputs_TimerOutputInt_Backup_Stop();
                }

                if (Outputs_FRegulationInt || Outputs_FAntifrostInt || Outputs_FChronoInt)
                {
                    /* start temperature regulation */
                    result = Outputs_RegulationTemperatureInt(&temperature_reg_int);
                    if (result)
                        result = Regulation_RegulationTemperatureInt_Regulation(temperature_reg_int);
                    else
                        result = Regulation_RegulationTemperatureInt_Off();
                }
                else
                {
                    /* disable OUT1 */
                    result = Regulation_RegulationTemperatureInt_Off();
                }
                break;


            /* output manual status: output on */
            case OUTPUT_MANUAL_ON:
                if (function_status_event_int)
                {
                    // NOTHING TO DO
                }
                else
                {
                    /* stop timer for internal temperature output (if it is enabled) */
                    if (Outputs_TimerOutputInt)
                    {
                        Outputs_TimerOutputInt = FALSE;
                        Outputs_TimerOutputInt_Stop();
                        Outputs_TimerOutputInt_Backup_Stop();
                    }

                    /* enable OUT1 */
                    result = Regulation_RegulationTemperatureInt_On();

                    /* start timer for internal temperature output (if activation time is > 0) */
                    if (Outputs_Config_TimerOutputInt.activation_time_value > 0)
                    {
                        /* calculate activation time in ms */
                        if      (Outputs_Config_TimerOutputInt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value          ) * 1000;
                        else if (Outputs_Config_TimerOutputInt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value *      60) * 1000;
                        else if (Outputs_Config_TimerOutputInt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_int_ms = ((u32)Outputs_Config_TimerOutputInt.activation_time_value          ) * 1000;

                        /* start timer for internal temperature output */
                        Outputs_TimerOutputInt = TRUE;
                        Outputs_TimerOutputInt_Start(activation_time_value_int_ms, FALSE);

                        /* start timer for internal temperature output backup */
                        Outputs_TimerOutputInt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP, TRUE);
                    }
                }
                break;


            /* output manual status: output regulation ("regulation" function) */
            case OUTPUT_MANUAL_REGULATION:
                /*** Note: it is not necessary to check "function_status_event_int" ***/

                /* stop timer for internal temperature output (if it is enabled) */
                if (Outputs_TimerOutputInt)
                {
                    Outputs_TimerOutputInt = FALSE;
                    Outputs_TimerOutputInt_Stop();
                    Outputs_TimerOutputInt_Backup_Stop();
                }

                /* start temperature regulation */
                result = Outputs_RegulationTemperatureInt(&temperature_reg_int);
                if (result)
                    result = Regulation_RegulationTemperatureInt_Regulation(temperature_reg_int);
                else
                    result = Regulation_RegulationTemperatureInt_Off();
                break;
        }
    }


    /*=========================
     * external temperature
     *=========================*/
    if (manual_event_ext || function_status_event_ext)
    {
        switch (Outputs_OutputManualStatusExt)
        {
            /* output manual status: output off */
            case OUTPUT_MANUAL_OFF:
                /*** Note: it is not necessary to check "function_status_event_ext" ***/

                /* stop timer for internal temperature output (if it is enabled) */
                if (Outputs_TimerOutputExt)
                {
                    Outputs_TimerOutputExt = FALSE;
                    Outputs_TimerOutputExt_Stop();
                    Outputs_TimerOutputExt_Backup_Stop();
                }

                if (Outputs_FRegulationExt || Outputs_FAntifrostExt || Outputs_FChronoExt)
                {
                    /* start temperature regulation */
                    result = Outputs_RegulationTemperatureExt(&temperature_reg_ext);
                    if (result)
                        result = Regulation_RegulationTemperatureExt_Regulation(temperature_reg_ext);
                    else
                        result = Regulation_RegulationTemperatureExt_Off();
                }
                else
                {
                    /* disable OUT1 */
                    result = Regulation_RegulationTemperatureExt_Off();
                }
                break;


            /* output manual status: output on */
            case OUTPUT_MANUAL_ON:
                if (function_status_event_ext)
                {
                    // NOTHING TO DO
                }
                else
                {
                    /* stop timer for internal temperature output (if it is enabled) */
                    if (Outputs_TimerOutputExt)
                    {
                        Outputs_TimerOutputExt = FALSE;
                        Outputs_TimerOutputExt_Stop();
                        Outputs_TimerOutputExt_Backup_Stop();
                    }

                    /* enable OUT1 */
                    result = Regulation_RegulationTemperatureExt_On();

                    /* start timer for internal temperature output (if activation time is > 0) */
                    if (Outputs_Config_TimerOutputExt.activation_time_value > 0)
                    {
                        /* calculate activation time in ms */
                        if      (Outputs_Config_TimerOutputExt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__SECOND)
                            activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value          ) * 1000;
                        else if (Outputs_Config_TimerOutputExt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE)
                            activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value *      60) * 1000;
                        else if (Outputs_Config_TimerOutputExt.activation_time_unit == OUTPUTS__ACTIVATION_TIME_UNIT__HOUR  )
                            activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value * 60 * 60) * 1000;
                        else
                            activation_time_value_ext_ms = ((u32)Outputs_Config_TimerOutputExt.activation_time_value          ) * 1000;

                        /* start timer for internal temperature output */
                        Outputs_TimerOutputExt = TRUE;
                        Outputs_TimerOutputExt_Start(activation_time_value_ext_ms, FALSE);

                        /* start timer for internal temperature output backup */
                        Outputs_TimerOutputExt_Backup_Start(TIME_MS_ELAPSED_TIME_BACKUP, TRUE);
                    }
                }
                break;


            /* output manual status: output regulation ("regulation" function) */
            case OUTPUT_MANUAL_REGULATION:
                /*** Note: it is not necessary to check "function_status_event_ext" ***/

                /* stop timer for internal temperature output (if it is enabled) */
                if (Outputs_TimerOutputExt)
                {
                    Outputs_TimerOutputExt = FALSE;
                    Outputs_TimerOutputExt_Stop();
                    Outputs_TimerOutputExt_Backup_Stop();
                }

                /* start temperature regulation */
                result = Outputs_RegulationTemperatureExt(&temperature_reg_ext);
                if (result)
                    result = Regulation_RegulationTemperatureExt_Regulation(temperature_reg_ext);
                else
                    result = Regulation_RegulationTemperatureExt_Off();
                break;
        }
    }



    if (
         /* output manual status */
         (Outputs_OutputManualStatusInt != old_output_manual_status_int) ||
         (Outputs_OutputManualStatusExt != old_output_manual_status_ext) ||

         /* internal/external antifrost function status */
         (Outputs_FAntifrostInt         != old_f_antifrost_int         ) ||
         (Outputs_FAntifrostIntTemp     != old_f_antifrost_int_temp    ) ||
         (Outputs_FAntifrostExt         != old_f_antifrost_ext         ) ||
         (Outputs_FAntifrostExtTemp     != old_f_antifrost_ext_temp    ) ||

         /* internal/external regulation function status */
         (Outputs_FRegulationInt        != old_f_regulation_int        ) ||
         (Outputs_FRegulationIntTemp    != old_f_regulation_int_temp   ) ||
         (Outputs_FRegulationExt        != old_f_regulation_ext        ) ||
         (Outputs_FRegulationExtTemp    != old_f_regulation_ext_temp   ) ||

         /* internal/external chrono     function status */
         (Outputs_FChronoInt            != old_f_chrono_int            ) ||
         (Outputs_FChronoIntTemp        != old_f_chrono_int_temp       ) ||
         (Outputs_FChronoExt            != old_f_chrono_ext            ) ||
         (Outputs_FChronoExtTemp        != old_f_chrono_ext_temp       ) ||

         /* internal/external temperature output timer status */
         (Outputs_TimerOutputInt        != old_timer_output_int        ) ||
         (Outputs_TimerOutputExt        != old_timer_output_ext        )
       )
    {
        Outputs_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Outputs_AdlCallback_Timer_TimerOutputInt
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Outputs_AdlCallback_Timer_TimerOutputInt(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUTPUT INT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_OUTPUT_INT;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Outputs_AdlCallback_Timer_TimerOutputExt
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Outputs_AdlCallback_Timer_TimerOutputExt(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUTPUT EXT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_OUTPUT_EXT;

    err = ql_rtos_event_send(Boot_TaskRef_Outputs, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Outputs_AdlCallback_Timer_TimerOutputInt_Backup
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Outputs_AdlCallback_Timer_TimerOutputInt_Backup(void *context)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUTPUT INT BACKUP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Outputs_TimerOutputInt_Info.status)
    {
        if (Outputs_TimerOutputInt_Info.time_remaining >= TIME_MS_ELAPSED_TIME_BACKUP)
            Outputs_TimerOutputInt_Info.time_remaining -= TIME_MS_ELAPSED_TIME_BACKUP;
        else
            Outputs_TimerOutputInt_Info.time_remaining  = 0;

        Outputs_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Outputs_AdlCallback_Timer_TimerOutputExt_Backup
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Outputs_AdlCallback_Timer_TimerOutputExt_Backup(void *context)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OUTPUTS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUTPUT EXT BACKUP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Outputs_TimerOutputExt_Info.status)
    {
        if (Outputs_TimerOutputExt_Info.time_remaining >= TIME_MS_ELAPSED_TIME_BACKUP)
            Outputs_TimerOutputExt_Info.time_remaining -= TIME_MS_ELAPSED_TIME_BACKUP;
        else
            Outputs_TimerOutputExt_Info.time_remaining  = 0;

        Outputs_UpdateVarCrc();
    }
}
