/*=============================================================================
 * File       :  CHARGER.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - battery charger
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "charger.h"
#include "debug_my.h"
#include "drvbattery.h"
#include "drvgpio.h"
#include "fw_config.h"
#include "startup.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* time (ms) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TIME_MS__CHARGE_ON                      ((( 1 * 60) + 30) * 1000L)
#define TIME_MS__CHARGE_OFF                     (((13 * 60) + 30) * 1000L)
#define TIME_MS__CHARGE_OFF_AFTER_POWER_ON      ((( 1 * 60)     ) * 1000L)
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TIME_MS__CHARGE_ON                      ((( 0 * 60) + 15) * 1000L)
#define TIME_MS__CHARGE_OFF                     ((( 2 * 60)     ) * 1000L)
#define TIME_MS__CHARGE_OFF_AFTER_POWER_ON      ((( 0 * 60) + 30) * 1000L)
#endif

/* number of battery voltage readings */
#define NUMBER_OF_BATTERY_READINGS              4

/* voltage difference on GSM module power supply with/without charger (mV) */
#define VOLTAGE_DIFFERENCE                      50

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                 200

/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__POWER_OFF                    0                    /* SEM state - power off              */
#define SEM_STATE__POWER_ON_CHARGE_OFF          1                    /* SEM state - power on  - charge off */
#define SEM_STATE__POWER_ON_CHARGE_ON           2                    /* SEM state - power on  - charge on  */

/* SEM state strings */
#define SEM_STATE_STRING__POWER_OFF             "SEM_STATE__POWER_OFF"
#define SEM_STATE_STRING__POWER_ON_CHARGE_OFF   "SEM_STATE__POWER_ON_CHARGE_OFF"
#define SEM_STATE_STRING__POWER_ON_CHARGE_ON    "SEM_STATE__POWER_ON_CHARGE_ON"

/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__POWER_INTERRUPTION           0                    /* SEM event - main power supply interruption */
#define SEM_EVENT__POWER_RETURN                 1                    /* SEM event - main power supply return       */
#define SEM_EVENT__TIMEOUT_CHARGE_OFF           2                    /* SEM event - timeout charge off             */
#define SEM_EVENT__TIMEOUT_CHARGE_ON            3                    /* SEM event - timeout charge on              */

/* SEM events strings */
#define SEM_EVENT_STRING__POWER_INTERRUPTION    "SEM_EVENT__POWER_INTERRUPTION"
#define SEM_EVENT_STRING__POWER_RETURN          "SEM_EVENT__POWER_RETURN"
#define SEM_EVENT_STRING__TIMEOUT_CHARGE_OFF    "SEM_EVENT__TIMEOUT_CHARGE_OFF"
#define SEM_EVENT_STRING__TIMEOUT_CHARGE_ON     "SEM_EVENT__TIMEOUT_CHARGE_ON"

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__POWER_INTERRUPTION         (10700 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__POWER_RETURN               (10701 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_CHARGE_OFF         (10702 | (QL_COMPONENT_APP_START << 16))
#define TASK_MSG_ID__TIMEOUT_CHARGE_ON          (10703 | (QL_COMPONENT_APP_START << 16))




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
static u8         Charger_SemState;                    // = SEM_STATE__POWER_ON_CHARGE_OFF;

/* timeout for charger (ms) */
static u32        Charger_TimeChargeOn;                // = TIME_MS__CHARGE_ON;
static u32        Charger_TimeChargeOff;               // = TIME_MS__CHARGE_OFF;
static u32        Charger_TimeChargeOffAfterPowerOn;   // = TIME_MS__CHARGE_OFF_AFTER_POWER_ON;

/* timer info */
static TIMER_INFO Charger_TimerChargeOff_Info;
static TIMER_INFO Charger_TimerChargeOn_Info;

/* CRC variables */
static u16        Charger_VarCrc;                      // = 0x0000;

/* debug string */
static ascii      Charger_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static ql_timer_t Charger_TimerHandler_TimerChargeOff;      /* timer for charge off */
static ql_timer_t Charger_TimerHandler_TimerChargeOn;       /* timer for charge on  */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Charger_TaskCharger(void *argument);

/* status variables */
static void Charger_InitVar        (void);
static u16  Charger_CalculateVarCrc(void);
       void Charger_UpdateVarCrc   (void);
       bool Charger_VerifyVarCrc   (void);

/* task events */
static void Charger_PowerOkInputEvent(u8 input_id, bool input_status);

/*-----------------------------------------------------------------------------
 * set/get charger parameters
 *-----------------------------------------------------------------------------*/
      void Charger_GetTimeChargeOn                 (u32 *ptr_timeout_value);
      void Charger_SetTimeChargeOn                 (u32 *ptr_timeout_value);
      bool Charger_IsValidTimeChargeOn             (u32 *ptr_timeout_value);

      void Charger_GetTimeChargeOff                (u32 *ptr_timeout_value);
      void Charger_SetTimeChargeOff                (u32 *ptr_timeout_value);
      bool Charger_IsValidTimeChargeOff            (u32 *ptr_timeout_value);

      void Charger_GetTimeChargeOffAfterPowerOn    (u32 *ptr_timeout_value);
      void Charger_SetTimeChargeOffAfterPowerOn    (u32 *ptr_timeout_value);
      bool Charger_IsValidTimeChargeOffAfterPowerOn(u32 *ptr_timeout_value);

/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void Charger_TimerChargeOff_Start  (u32 time_value, bool periodic);
static void Charger_TimerChargeOff_Stop   (void);

static void Charger_TimerChargeOn_Start   (u32 time_value, bool periodic);
static void Charger_TimerChargeOn_Stop    (void);

/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Charger_AdlCallback_Message_TaskMsg(u32 msg_identifier);

/* timer   callback functions */
static void Charger_AdlCallback_Timer_TimerChargeOff(void *context);
static void Charger_AdlCallback_Timer_TimerChargeOn (void *context);




/*===========================================================================
 * Function   : Charger_TaskCharger
 *
 * Description: battery charger task
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Charger_TaskCharger(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - CHARGER - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* creation of timer "TimerChargeOff" */
    err = ql_rtos_timer_create(&Charger_TimerHandler_TimerChargeOff, QL_TIMER_IN_SERVICE, Charger_AdlCallback_Timer_TimerChargeOff, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerChargeOn" */
    err = ql_rtos_timer_create(&Charger_TimerHandler_TimerChargeOn, QL_TIMER_IN_SERVICE, Charger_AdlCallback_Timer_TimerChargeOn, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* register the function for signals of digital input event */
    DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__PWR_OK, Charger_PowerOkInputEvent);


    /* internal status init */
    Charger_InitVar();


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Charger_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : Charger_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Charger_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        Charger_SemState = SEM_STATE__POWER_ON_CHARGE_OFF;

        /* timeout for charger */
        Charger_TimeChargeOn              = TIME_MS__CHARGE_ON;
        Charger_TimeChargeOff             = TIME_MS__CHARGE_OFF;
        Charger_TimeChargeOffAfterPowerOn = TIME_MS__CHARGE_OFF_AFTER_POWER_ON;

        /* timer info */
        Charger_TimerChargeOff_Info.status         = FALSE;
        Charger_TimerChargeOff_Info.periodic       = FALSE;
        Charger_TimerChargeOff_Info.time_value     = 0;
        Charger_TimerChargeOff_Info.time_remaining = 0;

        /* timer info */
        Charger_TimerChargeOn_Info.status          = FALSE;
        Charger_TimerChargeOn_Info.periodic        = FALSE;
        Charger_TimerChargeOn_Info.time_value      = 0;
        Charger_TimerChargeOn_Info.time_remaining  = 0;


        /**** update the variable CRC ****/
        Charger_UpdateVarCrc();


        /**** timer ****/

        /* start timer charge off */
      //Charger_TimerChargeOff_Start(TIME_MS__CHARGE_OFF_AFTER_POWER_ON, FALSE);
        Charger_TimerChargeOff_Start(Charger_TimeChargeOffAfterPowerOn , FALSE);


        /**** outputs ****/

        /* disable charger */
        DrvGpio_DisactivateCharger();    /* disactivate charger */
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time

        if (Charger_TimerChargeOff_Info.status)
        {
            if (Charger_TimerChargeOff_Info.periodic)
            {
                Charger_TimerChargeOff_Start(Charger_TimerChargeOff_Info.time_value, TRUE);
            }
            else
            {
                if (Charger_TimerChargeOff_Info.time_remaining > 0)
                    Charger_TimerChargeOff_Start(Charger_TimerChargeOff_Info.time_remaining, FALSE);
            }
        }

        if (Charger_TimerChargeOn_Info.status)
        {
            if (Charger_TimerChargeOn_Info.periodic)
            {
                Charger_TimerChargeOn_Start(Charger_TimerChargeOn_Info.time_value, TRUE);
            }
            else
            {
                if (Charger_TimerChargeOn_Info.time_remaining > 0)
                    Charger_TimerChargeOn_Start(Charger_TimerChargeOn_Info.time_remaining, FALSE);
            }
        }


        /**** outputs ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : Charger_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 Charger_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&Charger_SemState           , sizeof(Charger_SemState           ), crc);

    /* timers info */
    crc = Utility_CalculateCRC16((u8 *)&Charger_TimerChargeOff_Info, sizeof(Charger_TimerChargeOff_Info), crc);
    crc = Utility_CalculateCRC16((u8 *)&Charger_TimerChargeOn_Info , sizeof(Charger_TimerChargeOn_Info ), crc);

    return crc;
}




/*=============================================================================
 * Function   : Charger_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Charger_UpdateVarCrc(void)
{
    Charger_VarCrc = Charger_CalculateVarCrc();
}




/*=============================================================================
 * Function   : Charger_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool Charger_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = Charger_CalculateVarCrc();

    if (Charger_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : Charger_PowerOkInputEvent
 *
 * Description: signal new PWR_OK input event
 * Input      : - input_id    : input ID
 *              - input_status: input status
 * Output     : -
 *===========================================================================*/
static void Charger_PowerOkInputEvent(u8 input_id, bool input_status)
{
    ql_event_t event;
    QlOSStatus err;


    if (input_id != DRVGPIO__IN__PWR_OK)
        return;


    if (input_status)
        event.id = TASK_MSG_ID__POWER_RETURN;
    else
        event.id = TASK_MSG_ID__POWER_INTERRUPTION;

    err = ql_rtos_event_send(Boot_TaskRef_Charger, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Charger_GetTimeChargeOn
 *
 * Description: get the "time charge on" value
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Charger_GetTimeChargeOn(u32 *ptr_data)
{
    *ptr_data = Charger_TimeChargeOn;
}




/*===========================================================================
 * Function   : Charger_SetTimeChargeOn
 *
 * Description: set the "time charge on" value
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Charger_SetTimeChargeOn(u32 *ptr_data)
{
    Charger_TimeChargeOn = *ptr_data;

    Charger_UpdateVarCrc();
}




/*===========================================================================
 * Function   : Charger_IsValidTimeChargeOn
 *
 * Description: check if the "time charge on" value is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Charger_IsValidTimeChargeOn(u32 *ptr_data)
{
    if ((*ptr_data) == 0)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Charger_GetTimeChargeOff
 *
 * Description: get the "time charge on" value
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Charger_GetTimeChargeOff(u32 *ptr_data)
{
    *ptr_data = Charger_TimeChargeOff;
}




/*===========================================================================
 * Function   : Charger_SetTimeChargeOff
 *
 * Description: set the "time charge on" value
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Charger_SetTimeChargeOff(u32 *ptr_data)
{
    Charger_TimeChargeOff = *ptr_data;

    Charger_UpdateVarCrc();
}




/*===========================================================================
 * Function   : Charger_IsValidTimeChargeOff
 *
 * Description: check if the "time charge on" value is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Charger_IsValidTimeChargeOff(u32 *ptr_data)
{
    if ((*ptr_data) == 0)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Charger_GetTimeChargeOffAfterPowerOn
 *
 * Description: get the "time charge on" value
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Charger_GetTimeChargeOffAfterPowerOn(u32 *ptr_data)
{
    *ptr_data = Charger_TimeChargeOffAfterPowerOn;
}




/*===========================================================================
 * Function   : Charger_SetTimeChargeOffAfterPowerOn
 *
 * Description: set the "time charge on" value
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Charger_SetTimeChargeOffAfterPowerOn(u32 *ptr_data)
{
    Charger_TimeChargeOffAfterPowerOn = *ptr_data;

    Charger_UpdateVarCrc();
}




/*===========================================================================
 * Function   : Charger_IsValidTimeChargeOffAfterPowerOn
 *
 * Description: check if the "time charge on" value is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Charger_IsValidTimeChargeOffAfterPowerOn(u32 *ptr_data)
{
    if ((*ptr_data) == 0)
        return FALSE;

    return TRUE;
}




/*=============================================================================
 * Function   : Charger_TimerChargedOff_Start
 *
 * Description: start the "charge off" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Charger_TimerChargeOff_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Start \"charge off\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Charger_TimerHandler_TimerChargeOff, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Charger_TimerChargeOff_Info.status         = TRUE;
        Charger_TimerChargeOff_Info.periodic       = periodic;
        Charger_TimerChargeOff_Info.time_value     = time_value;
        Charger_TimerChargeOff_Info.time_remaining = time_value;

        Charger_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Charger_TimerChargeOff_Stop
 *
 * Description: stop the "charge off" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Charger_TimerChargeOff_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Stop \"charge off\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Charger_TimerHandler_TimerChargeOff);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Charger_DebugString, sizeof(Charger_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, Charger_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Charger_TimerChargeOff_Info.status         = FALSE;
        Charger_TimerChargeOff_Info.periodic       = FALSE;
        Charger_TimerChargeOff_Info.time_value     = 0;
        Charger_TimerChargeOff_Info.time_remaining = 0;

        Charger_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Charger_TimerChargeOn_Start
 *
 * Description: start the "charge on" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Charger_TimerChargeOn_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Start \"charge on\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Charger_TimerHandler_TimerChargeOn, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Charger_TimerChargeOn_Info.status         = TRUE;
        Charger_TimerChargeOn_Info.periodic       = periodic;
        Charger_TimerChargeOn_Info.time_value     = time_value;
        Charger_TimerChargeOn_Info.time_remaining = time_value;

        Charger_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Charger_TimerChargeOn_Stop
 *
 * Description: stop the "charge on" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Charger_TimerChargeOn_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Stop \"charge on\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Charger_TimerHandler_TimerChargeOn);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Charger_DebugString, sizeof(Charger_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, Charger_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        Charger_TimerChargeOn_Info.status         = FALSE;
        Charger_TimerChargeOn_Info.periodic       = FALSE;
        Charger_TimerChargeOn_Info.time_value     = 0;
        Charger_TimerChargeOn_Info.time_remaining = 0;

        Charger_UpdateVarCrc();
    }
}




/*===========================================================================
 * Function   : Charger_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data      :
 * Output     : -
 *===========================================================================*/
static void Charger_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
                 u8     old_sem_state;

                 u32    sem_event;

                 u16    gsm_module_power_supply_1;

                 bool   result_1;

    static const ascii *sem_state_strings[] =
    {
        SEM_STATE_STRING__POWER_OFF,
        SEM_STATE_STRING__POWER_ON_CHARGE_OFF,
        SEM_STATE_STRING__POWER_ON_CHARGE_ON,
    };

    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__POWER_INTERRUPTION,
        SEM_EVENT_STRING__POWER_RETURN,
        SEM_EVENT_STRING__TIMEOUT_CHARGE_OFF,
        SEM_EVENT_STRING__TIMEOUT_CHARGE_ON,
    };


    /* debug */
    snprintf(Charger_DebugString, sizeof(Charger_DebugString), "CALLBACK     - MESSAGE     - TASK CHARGER - msg identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, Charger_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier)
    {
        case TASK_MSG_ID__POWER_INTERRUPTION:
            sem_event = SEM_EVENT__POWER_INTERRUPTION;
            break;

        case TASK_MSG_ID__POWER_RETURN:
            sem_event = SEM_EVENT__POWER_RETURN;
            break;

        case TASK_MSG_ID__TIMEOUT_CHARGE_OFF:
            sem_event = SEM_EVENT__TIMEOUT_CHARGE_OFF;
            break;

        case TASK_MSG_ID__TIMEOUT_CHARGE_ON:
            sem_event = SEM_EVENT__TIMEOUT_CHARGE_ON;
            break;

        default:
            return;
    }


    snprintf(Charger_DebugString, sizeof(Charger_DebugString), "SEM state: %s", sem_state_strings[Charger_SemState]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, Charger_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Charger_DebugString, sizeof(Charger_DebugString), "SEM event: %s", sem_event_strings[sem_event]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, Charger_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    old_sem_state = Charger_SemState;



    switch (Charger_SemState)
    {
        /* SEM state - power off */
        case SEM_STATE__POWER_OFF:
            switch (sem_event)
            {
                /* SEM event - main power supply return */
                case SEM_EVENT__POWER_RETURN:
                    /* start timer charge off */
                  //Charger_TimerChargeOff_Start(TIME_MS__CHARGE_OFF_AFTER_POWER_ON, FALSE);
                    Charger_TimerChargeOff_Start(Charger_TimeChargeOffAfterPowerOn , FALSE);

                    Charger_SemState = SEM_STATE__POWER_ON_CHARGE_OFF;
                    break;


                /* SEM event - main power supply interruption */
                /* SEM event - timeout charge off             */
                /* SEM event - timeout charge on              */
                case SEM_EVENT__POWER_INTERRUPTION:
                case SEM_EVENT__TIMEOUT_CHARGE_OFF:
                case SEM_EVENT__TIMEOUT_CHARGE_ON:
                    // NOTHING TO DO
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - power on - charge off */
        case SEM_STATE__POWER_ON_CHARGE_OFF:
            switch (sem_event)
            {
                /* SEM event - main power supply interruption */
                case SEM_EVENT__POWER_INTERRUPTION:
                    /* stop timer charge off */
                    Charger_TimerChargeOff_Stop();

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Disable the charger", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* disable the charger */
                    DrvGpio_DisactivateCharger();

                    Charger_SemState = SEM_STATE__POWER_OFF;
                    break;


                /* SEM event - timeout charge off */
                case SEM_EVENT__TIMEOUT_CHARGE_OFF:
                    /* get GSM module power supply (before enabling the charger) */
                    result_1 = DrvBattery_GetBatteryVoltage(&gsm_module_power_supply_1);
                    if (!result_1)
                    {
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "DrvBattery_GetBatteryVoltage ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Enable the charger", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* enable the charger */
                    DrvGpio_ActivateCharger();

                    /* start timer charge on */
                  //Charger_TimerChargeOn_Start(TIME_MS__CHARGE_ON  , FALSE);
                    Charger_TimerChargeOn_Start(Charger_TimeChargeOn, FALSE);

                    Charger_SemState = SEM_STATE__POWER_ON_CHARGE_ON;
                    break;


                /* SEM event - main power supply return */
                /* SEM event - timeout charge on        */
                case SEM_EVENT__POWER_RETURN:
                case SEM_EVENT__TIMEOUT_CHARGE_ON:
                    // NOTHING TO DO
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - power on - charge  on */
        case SEM_STATE__POWER_ON_CHARGE_ON:
            switch (sem_event)
            {
                /* SEM event - main power supply interruption */
                case SEM_EVENT__POWER_INTERRUPTION:
                    /* stop timer charge on */
                    Charger_TimerChargeOn_Stop();

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Disable the charger", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* disable the charger */
                    DrvGpio_DisactivateCharger();

                    Charger_SemState = SEM_STATE__POWER_OFF;
                    break;


                /* SEM event - timeout charge on  */
                case SEM_EVENT__TIMEOUT_CHARGE_ON:
                    /* start timer charge off */
                  //Charger_TimerChargeOff_Start(TIME_MS__CHARGE_OFF  , FALSE);
                    Charger_TimerChargeOff_Start(Charger_TimeChargeOff, FALSE);

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Disable the charger", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* disable the charger */
                    DrvGpio_DisactivateCharger();

                    Charger_SemState = SEM_STATE__POWER_ON_CHARGE_OFF;
                    break;


                /* SEM event - main power supply return */
                /* SEM event - timeout charge off       */
                case SEM_EVENT__POWER_RETURN:
                case SEM_EVENT__TIMEOUT_CHARGE_OFF:
                    // NOTHING TO DO
                    break;


                /* event - other events */
                default:
                    break;
            }
            break;



        /* SEM state - unknown state */
        default:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "Disable the charger", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* disable the charger */
            DrvGpio_DisactivateCharger();

            Charger_SemState = SEM_STATE__POWER_OFF;
            break;
    }



    if (
         (Charger_SemState != old_sem_state)
       )
    {
        Charger_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : Charger_AdlCallback_Timer_TimerChargeOff
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Charger_AdlCallback_Timer_TimerChargeOff(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - CHARGE OFF", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Charger_TimerChargeOff_Info.periodic)
    {
      //Charger_TimerChargeOff_Info.status         = TRUE;
      //Charger_TimerChargeOff_Info.periodic       = TRUE;
      //Charger_TimerChargeOff_Info.time_value     = time_value;
        Charger_TimerChargeOff_Info.time_remaining = Charger_TimerChargeOff_Info.time_value;

        Charger_UpdateVarCrc();
    }
    else
    {
        Charger_TimerChargeOff_Info.status         = FALSE;
        Charger_TimerChargeOff_Info.periodic       = FALSE;
        Charger_TimerChargeOff_Info.time_value     = 0;
        Charger_TimerChargeOff_Info.time_remaining = 0;

        Charger_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_CHARGE_OFF;

    err = ql_rtos_event_send(Boot_TaskRef_Charger, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Charger_AdlCallback_Timer_TimerChargeOn
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Charger_AdlCallback_Timer_TimerChargeOn(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - CHARGE ON", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (Charger_TimerChargeOn_Info.periodic)
    {
      //Charger_TimerChargeOn_Info.status         = TRUE;
      //Charger_TimerChargeOn_Info.periodic       = TRUE;
      //Charger_TimerChargeOn_Info.time_value     = time_value;
        Charger_TimerChargeOn_Info.time_remaining = Charger_TimerChargeOn_Info.time_value;

        Charger_UpdateVarCrc();
    }
    else
    {
        Charger_TimerChargeOn_Info.status         = FALSE;
        Charger_TimerChargeOn_Info.periodic       = FALSE;
        Charger_TimerChargeOn_Info.time_value     = 0;
        Charger_TimerChargeOn_Info.time_remaining = 0;

        Charger_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_CHARGE_ON;

    err = ql_rtos_event_send(Boot_TaskRef_Charger, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CHARGER, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
