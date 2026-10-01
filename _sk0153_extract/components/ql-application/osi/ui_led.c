/*=============================================================================
 * File       :  UI_LED.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - Led User Interface
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 * *** GSM leds indication ***
 *
 * |----------|-----------------------------------|-----------|--------------|
 * | Priority |              Status               |   RSSI    |  Green led   |
 * |----------|-----------------------------------|-----------|--------------|
 * |    1     | SIM problem                       |    ---    | blink mode 1 |
 * |----------|-----------------------------------|-----------|--------------|
 * |    2     | no registered                     |    ---    |     off      |
 * |----------|-----------------------------------|-----------|--------------|
 * |    3     | registered with    SMS tx problem |    ---    | blink mode 2 |
 * |----------|-----------------------------------|-----------|--------------|
 * |          |                                   |    low    | blink fast   |
 * |    4     | registered without SMS tx problem |  medium   | blink medium |
 * |          |                                   |    good   | blink slow   |
 * |          |                                   | excellent |      on      |
 * |----------|-----------------------------------|-----------|--------------|
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "default.h"
#include "drvgpio.h"
#include "fw_config.h"
#include "led.h"
#include "startup.h"
#include "typedef.h"
#include "ui_led.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* task message IDs */
#define TASK_MSG_ID__GSM_NOT_REGISTERED       (12700 | (QL_COMPONENT_APP_START << 16))     // event - GSM module not registered
#define TASK_MSG_ID__GSM_REGISTERED           (12701 | (QL_COMPONENT_APP_START << 16))     // event - GSM module     registered
#define TASK_MSG_ID__SIM_PROBLEM_START        (12702 | (QL_COMPONENT_APP_START << 16))     // event - SIM problem             start
#define TASK_MSG_ID__SIM_PROBLEM_STOP         (12703 | (QL_COMPONENT_APP_START << 16))     // event - SIM problem             stop
#define TASK_MSG_ID__GSM_REG_DENIED_START     (12704 | (QL_COMPONENT_APP_START << 16))     // event - GSM registration denied start
#define TASK_MSG_ID__GSM_REG_DENIED_STOP      (12705 | (QL_COMPONENT_APP_START << 16))     // event - GSM registration denied stop
#define TASK_MSG_ID__SMS_TX_FAIL_START        (12706 | (QL_COMPONENT_APP_START << 16))     // event - SMS tx fail             start
#define TASK_MSG_ID__SMS_TX_FAIL_STOP         (12707 | (QL_COMPONENT_APP_START << 16))     // event - SMS tx fail             stop

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING               200

/*---------------------------------------------------------------------------
 * led blink timeout (ms)
 *---------------------------------------------------------------------------*/
/* time for led blink "SIM problem" (ms) */
#define TIME_MS__SIM_PROBLEM__TIME_LED_ON     (200L)
#define TIME_MS__SIM_PROBLEM__TIME_LED_OFF    (200L)

/* time for led blink "SMS tx fail" (ms) */
#define TIME_MS__SMS_TX_FAIL__TIME_LED_ON     (500L)
#define TIME_MS__SMS_TX_FAIL__TIME_LED_OFF    (500L)


/* time for led blink (excellent GSM network signal) (ms) */
// led on (no blink)

/* time for led blink (good      GSM network signal) (ms) */
#define TIME_MS__RSSI_GOOD__TIME_LED_ON       (500L)
#define TIME_MS__RSSI_GOOD__TIME_LED_OFF      (500L)

/* time for led blink (medium    GSM network signal) (ms) */
#define TIME_MS__RSSI_MEDIUM__TIME_LED_ON     (300L)
#define TIME_MS__RSSI_MEDIUM__TIME_LED_OFF    (300L)

/* time for led blink (low       GSM network signal) (ms) */
#define TIME_MS__RSSI_LOW__TIME_LED_ON        (100L)
#define TIME_MS__RSSI_LOW__TIME_LED_OFF       (100L)




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static ascii     UiLed_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void UiLed_TaskUiLed(void *argument);

/* led startup */
static void UiLed_StartupNormal      (void);
static void UiLed_StartupResetDefault(void);

/* task event */
       void UiLed_SignalSimProblemStart  (void);
       void UiLed_SignalSimProblemStop   (void);

       void UiLed_SignalGsmRegDeniedStart(void);
       void UiLed_SignalGsmRegDeniedStop (void);

       void UiLed_SignalSmsTxFailStart   (void);
       void UiLed_SignalSmsTxFailStop    (void);

       void UiLed_SignalGsmNotRegistered (void);
       void UiLed_SignalGsmRegistered    (u8 rssi, u8 ber);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void UiLed_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);




/*=============================================================================
 * Function   : UiLed_TaskUiLed
 *
 * Description: UI led task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_TaskUiLed(void *argument)
{
    ql_event_t event;
    QlOSStatus err;

    bool       recover_status;

    bool       startup_normal;
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    bool       rst_io_input;
#endif

    bool       result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - UI_LED - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* init led at normal startup */
    startup_normal = TRUE;

#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    /* read the RST_IO signal in order to verify the startup type (normal or reset default) */
    rst_io_input = DrvGpio_ReadInput(DRVGPIO__IN__RST_IO);
    if (rst_io_input)
        startup_normal = TRUE;
    else
        startup_normal = FALSE;


    /* set some configurations to default values */
    if (!startup_normal)
        result = Default_DefaultResetKey();
#endif


    /* init delay */
    ql_rtos_task_sleep_ms(500L);


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* led startup */
        if (startup_normal)
            UiLed_StartupNormal();
        else
            UiLed_StartupResetDefault();
    }


    /* fixed green indication (no GSM registration) */
    Led_LedOff(LED__LED_GSM_GREEN);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            UiLed_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : UiLed_StartupNormal
 *
 * Description: init led at normal startup
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void UiLed_StartupNormal(void)
{
    u8 i;


    for (i = 0; i < 3; i++)
    {
        /* green led on */
        DrvGpio_SetOutputLedGsmGreenHigh();

        ql_rtos_task_sleep_ms(500L);


        /* green led off */
        DrvGpio_SetOutputLedGsmGreenLow();

        ql_rtos_task_sleep_ms(250L);
    }


    ql_rtos_task_sleep_ms(2000L);
}




/*=============================================================================
 * Function   : UiLed_StartupResetDefault
 *
 * Description: init led at reset default startup
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void UiLed_StartupResetDefault(void)
{
    u8 i;


    /* GSM leds blink */
    for (i = 0; i < 7; i++)
    {
        /* green led on */
        DrvGpio_SetOutputLedGsmGreenHigh();

        ql_rtos_task_sleep_ms(200L);

        /* leds off */
        DrvGpio_SetOutputLedGsmGreenLow();

        ql_rtos_task_sleep_ms(100L);
    }


    ql_rtos_task_sleep_ms(2000L);
}




/*=============================================================================
 * Function   : UiLed_SignalSimProblemStart
 *
 * Description: signal SIM problem start
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalSimProblemStart(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__SIM_PROBLEM_START;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalSimProblemStop
 *
 * Description: signal SIM problem stop
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalSimProblemStop(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__SIM_PROBLEM_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalGsmRegDeniedStart
 *
 * Description: signal GSM registration start
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalGsmRegDeniedStart(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_REG_DENIED_START;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalGsmRegDeniedStop
 *
 * Description: signal GSM registration stop
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalGsmRegDeniedStop(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_REG_DENIED_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalSmsTxFailStart
 *
 * Description: signal SMS tx fail start
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalSmsTxFailStart(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__SMS_TX_FAIL_START;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalSmsTxFailStop
 *
 * Description: signal SMS tx fail stop
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalSmsTxFailStop(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__SMS_TX_FAIL_STOP;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalGsmNotRegistered
 *
 * Description: signal GSM module not registered
 * Input      : -
 * Output     : -
 *=============================================================================*/
void UiLed_SignalGsmNotRegistered(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__GSM_NOT_REGISTERED;

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : UiLed_SignalGsmRegistered
 *
 * Description: signal GSM module registered
 * Input      : - rssi: RSSI
 *              - ber : BER
 * Output     : -
 *=============================================================================*/
void UiLed_SignalGsmRegistered(u8 rssi, u8 ber)
{
    ql_event_t event;
    QlOSStatus err;


    event.id     = TASK_MSG_ID__GSM_REGISTERED;
    event.param1 = rssi;
    event.param2 = ber;  // not used

    err = ql_rtos_event_send(Boot_TaskRef_UiLed, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : UiLed_AdlCallback_Message_TaskMsg
 *
 * Description: message callback function for task
 * Inputs     : - ptr_data:
 * Outputs    : -
 *===========================================================================*/
static void UiLed_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    static bool status_change  = FALSE;

    static bool sim_problem    = FALSE;
    static bool gsm_reg_denied = FALSE;
    static bool not_registered = TRUE;
    static bool sms_tx_fail    = FALSE;

    static u8   gsm_signal_old = 0xFF;
           u8   gsm_signal = 0;

           u8   rssi;


    snprintf(UiLed_DebugString, sizeof(UiLed_DebugString), "CALLBACK     - MESSAGE     - TASK UI_LED - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, UiLed_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    status_change = FALSE;

    switch (msg_identifier->id)
    {
        /* event - SIM problem start */
        case TASK_MSG_ID__SIM_PROBLEM_START:
            if (!sim_problem)
            {
                sim_problem    = TRUE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - SIM problem stop */
        case TASK_MSG_ID__SIM_PROBLEM_STOP:
            if (sim_problem)
            {
                sim_problem    = FALSE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - GSM registration denied start */
        case TASK_MSG_ID__GSM_REG_DENIED_START:
            if (!gsm_reg_denied)
            {
                gsm_reg_denied = TRUE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - GSM registration denied stop */
        case TASK_MSG_ID__GSM_REG_DENIED_STOP:
            if (gsm_reg_denied)
            {
                gsm_reg_denied = FALSE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - SMS tx fail start */
        case TASK_MSG_ID__SMS_TX_FAIL_START:
            if (!sms_tx_fail)
            {
                sms_tx_fail    = TRUE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - SMS tx fail stop */
        case TASK_MSG_ID__SMS_TX_FAIL_STOP:
            if (sms_tx_fail)
            {
                sms_tx_fail    = FALSE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - GSM module not registered */
        case TASK_MSG_ID__GSM_NOT_REGISTERED:
            if (!not_registered)
            {
                not_registered = TRUE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            break;


        /* event - GSM module registered */
        case TASK_MSG_ID__GSM_REGISTERED:
            if (not_registered)
            {
                not_registered = FALSE;
                gsm_signal_old = 0xFF;
                status_change  = TRUE;
            }
            else
            {
                /* extract the parameters */
                rssi = msg_identifier->param1;

                if      (rssi <   8)
                {
                    /* low       GSM network signal */
                    gsm_signal = 0;
                }
                else if (rssi <  16)
                {
                    /* medium    GSM network signal */
                    gsm_signal = 1;
                }
                else if (rssi <  24)
                {
                    /* good      GSM network signal */
                    gsm_signal = 2;
                }
                else if (rssi <= 31)
                {
                    /* excellent GSM network signal */
                    gsm_signal = 3;
                }
                else
                {
                    /* unknown    GSM network signal */
                    gsm_signal = 4;
                }

                if (gsm_signal != gsm_signal_old)
                {
                    /* GSM signal is changed */

                    gsm_signal_old = gsm_signal;
                    status_change  = TRUE;
                }
            }
            break;


        /* other events or unknown event */
        default:
            return;
    }


    snprintf(UiLed_DebugString,
             sizeof(UiLed_DebugString),
             "status_change: %d;   sim_problem: %d; gsm_reg_denied: %d, not_registered: %d; sms_tx_fail: %d;   gsm_signal: %d; gsm_signal_old: %d",
             status_change,
             sim_problem,
             gsm_reg_denied,
             not_registered,
             sms_tx_fail,
             gsm_signal,
             gsm_signal_old);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UI_LED, DEBUG_TRACE_TYPE_LOW, UiLed_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* verify if status is changed */
    if (status_change)
    {
        /* status is changed */

        /* leds off */
        Led_LedOff(LED__LED_GSM_GREEN);

        if (sim_problem || gsm_reg_denied)
        {
            /* SIM problem or GSM registration denied */

            /* fast blink green indication */
            Led_LedBlinkStart(LED__LED_GSM_GREEN, 5, TIME_MS__SIM_PROBLEM__TIME_LED_ON, TIME_MS__SIM_PROBLEM__TIME_LED_OFF, 0, 5000);
        }
        else
        {
            /* SIM OK and GSM registration not denied */
            if (not_registered)
            {
                /* not registered */

                /* no green indication */
                Led_LedOff(LED__LED_GSM_GREEN);
            }
            else
            {
                /* registered */
                if (sms_tx_fail)
                {
                    /* SMS tx fail */

                    /* slow blink green indication */
                    Led_LedBlinkStart(LED__LED_GSM_GREEN, 3, TIME_MS__SMS_TX_FAIL__TIME_LED_ON, TIME_MS__SMS_TX_FAIL__TIME_LED_OFF, 0, 5000);
                }
                else
                {
                    /* SMS tx OK */

                    if      (gsm_signal == 0)
                    {
                        /* low       GSM network signal */

                        /* fast blink green indication */
                        Led_LedBlinkStart(LED__LED_GSM_GREEN, 1, TIME_MS__RSSI_LOW__TIME_LED_ON   , TIME_MS__RSSI_LOW__TIME_LED_OFF   , 0, (TIME_MS__RSSI_LOW__TIME_LED_ON    + TIME_MS__RSSI_LOW__TIME_LED_OFF   ));
                    }
                    else if (gsm_signal == 1)
                    {
                        /* medium    GSM network signal */

                        /* slow blink green indication */
                        Led_LedBlinkStart(LED__LED_GSM_GREEN, 1, TIME_MS__RSSI_MEDIUM__TIME_LED_ON, TIME_MS__RSSI_MEDIUM__TIME_LED_OFF, 0, (TIME_MS__RSSI_MEDIUM__TIME_LED_ON + TIME_MS__RSSI_MEDIUM__TIME_LED_OFF));
                    }
                    else if (gsm_signal == 2)
                    {
                        /* good      GSM network signal */

                        /* slow blink green indication */
                        Led_LedBlinkStart(LED__LED_GSM_GREEN, 1, TIME_MS__RSSI_GOOD__TIME_LED_ON  , TIME_MS__RSSI_GOOD__TIME_LED_OFF  , 0, (TIME_MS__RSSI_GOOD__TIME_LED_ON   + TIME_MS__RSSI_GOOD__TIME_LED_OFF  ));
                    }
                    else if (gsm_signal == 3)
                    {
                        /* excellent GSM network signal */

                        /* fixed green indication */
                        Led_LedOn        (LED__LED_GSM_GREEN);
                    }
                    else
                    {
                        /* unknown GSM network signal */

                        /* fast blink green indication */
                        Led_LedBlinkStart(LED__LED_GSM_GREEN, 1, TIME_MS__RSSI_LOW__TIME_LED_ON   , TIME_MS__RSSI_LOW__TIME_LED_OFF   , 0, (TIME_MS__RSSI_LOW__TIME_LED_ON    + TIME_MS__RSSI_LOW__TIME_LED_OFF   ));
                    }
                }
            }
        }
    }
}
