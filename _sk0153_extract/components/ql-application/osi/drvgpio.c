/*=============================================================================
 * File       :  DRVGPIO.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - GPIOs management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 *  |-------------------------------|
 *  |        digital inputs         |
 *  |---------------|---------------|
 *  | signal        | HW version 00 |
 *  |---------------|---------------|
 *  | PWR_OK        | GPIO3         |
 *  | KEY_OUT_1     | GPIO9         |
 *  | KEY_OUT_2     | GPIO10        |
 *  | IN1           | GPIO0         |
 *  | IN2           | GPIO1         |
 *  |---------------|---------------|
 *
 *  |-------------------------------|
 *  |        digital outputs        |
 *  |---------------|---------------|
 *  | signal        | HW version 00 |
 *  |---------------|---------------|
 *  | OUT_1         | GPIO14        |
 *  | OUT_2         | GPIO15        |
 *  | LED_GSM_GREEN | GPIO11        |
 *  | CHARGE        | GPIO2         |
 *  | VOLT_1        | GPIO20        |
 *  | VOLT_2        | GPIO19        |
 *  | SHUNT_1       | GPIO21        |
 *  | SHUNT_2       | GPIO5         |
 *  | PULL_1        | GPIO8         |
 *  | PULL_2        | GPIO17        |
 *  |---------------|---------------|
 */




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_gpio.h"
#include "quec_pin_index.h"

/* user     includes */
#include "boot.h"
#include "counters.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "drvtemperature.h"
#include "energy.h"
#include "fw_config.h"
#include "startup.h"
#include "typedef.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
#define QUEC_PIN_NONE                           0xff

/* digital inputs polling time (ms) */
#define INPUTS_POLLING_TIME                     100

/* max number of registered signals of digital input status changes */
#define NUM_MAX_SIGNALS_DIGITAL_INPUTS          8

/* task message IDs */
#define DRVGPIO__TASK_MSG_ID__SET_OUTPUT        (13000 | (QL_COMPONENT_APP_START << 16))    // set output request

/* debug string length */
#define DEBUG_STRING_LENTGH                     200

/*-----------------------------------------------------------------------------
 * GPIOs labels
 *-----------------------------------------------------------------------------*/
/* GPIO digital input  labels */
#define GPIO_LABEL_INPUT__RST_IO                GPIO_12                 // GPIO12 - "RST_IO"        input    (pin 88 of EG915U)
#define GPIO_LABEL_INPUT__PWR_OK                GPIO_3                  // GPIO3  - "PWR_OK"        input    (pin 7   of EG915U)
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
#define GPIO_LABEL_INPUT__KEY_OUT_1             GPIO_9                  // GPIO9  - "KEY_OUT_1"     input    (pin 26  of EG915U)
#define GPIO_LABEL_INPUT__KEY_OUT_2             GPIO_10                 // GPIO10 - "KEY_OUT_2"     input    (pin 25  of EG915U)
#endif
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
#define GPIO_LABEL_INPUT__KEY_OUT_1             GPIO_10                 // GPIO10 - "KEY_OUT_1"     input    (pin 25  of EG915U)
#define GPIO_LABEL_INPUT__KEY_OUT_2             GPIO_9                  // GPIO9  - "KEY_OUT_2"     input    (pin 26  of EG915U)
#endif
#define GPIO_LABEL_INPUT__IN1                   GPIO_0                  // GPIO0  - "IN1"           input    (pin 4   of EG915U)
#define GPIO_LABEL_INPUT__IN2                   GPIO_1                  // GPIO1  - "IN2"           input    (pin 5   of EG915U)

/* GPIO digital output labels */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
#define GPIO_LABEL_OUTPUT__OUT_1                GPIO_14                 // GPIO14 - "OUT_1"         output   (pin 40  of EG915U)
#define GPIO_LABEL_OUTPUT__OUT_2                GPIO_15                 // GPIO15 - "OUT_2"         output   (pin 41  of EG915U)
#endif
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
#define GPIO_LABEL_OUTPUT__OUT_1                GPIO_15                 // GPIO15 - "OUT_1"         output   (pin 41  of EG915U)
#define GPIO_LABEL_OUTPUT__OUT_2                GPIO_14                 // GPIO14 - "OUT_2"         output   (pin 40  of EG915U)
#endif
#define GPIO_LABEL_OUTPUT__LED_GSM_GREEN        GPIO_11                 // GPIO11 - "LED_GSM_GREEN" output   (pin 64  of EG915U)
#define GPIO_LABEL_OUTPUT__CHARGE               GPIO_2                  // GPIO2  - "CHARGE"        output   (pin 6   of EG915U)
#define GPIO_LABEL_OUTPUT__VOLT_1               GPIO_20                 // GPIO20 - "VOLT_1"        output   (pin 28  of EG915U)
#define GPIO_LABEL_OUTPUT__VOLT_2               GPIO_19                 // GPIO19 - "VOLT_2"        output   (pin 36  of EG915U)
#define GPIO_LABEL_OUTPUT__SHUNT_1              GPIO_21                 // GPIO21 - "SHUNT_1"       output   (pin 27  of EG915U)
#define GPIO_LABEL_OUTPUT__SHUNT_2              GPIO_5                  // GPIO5  - "SHUNT_2"       output   (pin 20  of EG915U)
#define GPIO_LABEL_OUTPUT__PULL_1               GPIO_8                  // GPIO8  - "PULL_1"        output   (pin 104 of EG915U)
#define GPIO_LABEL_OUTPUT__PULL_2               GPIO_17                 // GPIO17 - "PULL_2"        output   (pin 114 of EG915U)

/*-----------------------------------------------------------------------------
 * GPIOs descriptions
 *-----------------------------------------------------------------------------*/
/* GPIO digital input  descriptions */
#define GPIO_DESCRIPTION_INPUT__RST_IO          "GPI__RST_IO   "        // GPIO12 - "RST_IO"        input    (pin 88 of EG915U)
#define GPIO_DESCRIPTION_INPUT__PWR_OK          "GPI__PWR_OK   "        // GPIO3  - "PWR_OK"        input    (pin 7   of EG915U)
#define GPIO_DESCRIPTION_INPUT__KEY_OUT_1       "GPI__KEY_OUT_1"        // GPIO9  - "KEY_OUT_1"     input    (pin 26  of EG915U)
#define GPIO_DESCRIPTION_INPUT__KEY_OUT_2       "GPI__KEY_OUT_2"        // GPIO10 - "KEY_OUT_2"     input    (pin 25  of EG915U)
#define GPIO_DESCRIPTION_INPUT__IN1             "GPI__IN1      "        // GPIO0  - "IN1"           input    (pin 4   of EG915U)
#define GPIO_DESCRIPTION_INPUT__IN2             "GPI__IN2      "        // GPIO1  - "IN2"           input    (pin 5   of EG915U)

/* GPIO digital output descriptions */
#define GPIO_DESCRIPTION_OUTPUT__OUT_1          "GPO__OUT_1        "    // GPIO14 - "OUT_1"         output   (pin 40  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__OUT_2          "GPO__OUT_2        "    // GPIO15 - "OUT_2"         output   (pin 41  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__LED_GSM_GREEN  "GPO__LED_GSM_GREEN"    // GPIO11 - "LED_GSM_GREEN" output   (pin 64  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__CHARGE         "GPO__CHARGE       "    // GPIO2  - "CHARGE"        output   (pin 6   of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__VOLT_1         "GPO__VOLT_1       "    // GPIO20 - "VOLT_1"        output   (pin 28  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__VOLT_2         "GPO__VOLT_2       "    // GPIO19 - "VOLT_2"        output   (pin 36  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__SHUNT_1        "GPO__SHUNT_1      "    // GPIO21 - "SHUNT_1"       output   (pin 27  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__SHUNT_2        "GPO__SHUNT_2      "    // GPIO5  - "SHUNT_2"       output   (pin 20  of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__PULL_1         "GPO__PULL_1       "    // GPIO8  - "PULL_1"        output   (pin 104 of EG915U)
#define GPIO_DESCRIPTION_OUTPUT__PULL_2         "GPO__PULL_2       "    // GPIO17 - "PULL_2"        output   (pin 114 of EG915U)




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* input event signals */
typedef struct
{
    u8     input_id;                  /* input ID                                                                        */
    void (*ptr_function)(u8, bool);   /* pointer to the function to be called when there is a digital input state change */
    //                   |   |
    // input ID    ______|   |
    // input state __________|
} SIGNAL_INPUT_EVENT;

/* GPIO info */
typedef struct
{
    u8          pin_num;
    u8          default_func;
    u8          gpio_func;
    ql_GpioNum  gpio_num;
    ql_GpioDir  gpio_dir;
    ql_PullMode gpio_pull;
    ql_LvlMode  gpio_lvl;
} GPIO_INFO;




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* GPIOs config table 1 */
static GPIO_INFO          DrvGpio_GpioConfig[DRV_GPIO__NUM_GPIOS] =
{
    // digital inputs
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    {QUEC_PIN_DNAME_GPIO_12,    1, 0, GPIO_LABEL_INPUT__RST_IO,         GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO12 - "RST_IO"        input    (pin 88 of EG915U)
#endif
    {QUEC_PIN_DNAME_GPIO_3,     3, 0, GPIO_LABEL_INPUT__PWR_OK,         GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO3  - "PWR_OK"        input    (pin 7   of EG915U)
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    {QUEC_PIN_DNAME_GPIO_9,     1, 0, GPIO_LABEL_INPUT__KEY_OUT_1,      GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO9  - "KEY_OUT_1"     input    (pin 26  of EG915U)
    {QUEC_PIN_DNAME_GPIO_10,    1, 0, GPIO_LABEL_INPUT__KEY_OUT_2,      GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO10 - "KEY_OUT_2"     input    (pin 25  of EG915U)
#endif
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
    {QUEC_PIN_DNAME_GPIO_10,    1, 0, GPIO_LABEL_INPUT__KEY_OUT_1,      GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO10 - "KEY_OUT_1"     input    (pin 25  of EG915U)
    {QUEC_PIN_DNAME_GPIO_9,     1, 0, GPIO_LABEL_INPUT__KEY_OUT_2,      GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO9  - "KEY_OUT_2"     input    (pin 26  of EG915U)
#endif
    {QUEC_PIN_DNAME_GPIO_0,     3, 0, GPIO_LABEL_INPUT__IN1,            GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO0  - "IN1"           input    (pin 4   of EG915U)
    {QUEC_PIN_DNAME_GPIO_1,     3, 0, GPIO_LABEL_INPUT__IN2,            GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO1  - "IN2"           input    (pin 5   of EG915U)

    // digital outputs
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    {QUEC_PIN_DNAME_GPIO_14,    1, 0, GPIO_LABEL_OUTPUT__OUT_1,         GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO14 - "OUT_1"         output   (pin 40  of EG915U)
    {QUEC_PIN_DNAME_GPIO_15,    1, 0, GPIO_LABEL_OUTPUT__OUT_2,         GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO15 - "OUT_2"         output   (pin 41  of EG915U)
#endif
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
    {QUEC_PIN_DNAME_GPIO_15,    1, 0, GPIO_LABEL_OUTPUT__OUT_1,         GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO15 - "OUT_1"         output   (pin 41  of EG915U)
    {QUEC_PIN_DNAME_GPIO_14,    1, 0, GPIO_LABEL_OUTPUT__OUT_2,         GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO14 - "OUT_2"         output   (pin 40  of EG915U)
#endif
    {QUEC_PIN_DNAME_GPIO_11,    1, 0, GPIO_LABEL_OUTPUT__LED_GSM_GREEN, GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO11 - "LED_GSM_GREEN" output   (pin 64  of EG915U)
    {QUEC_PIN_DNAME_GPIO_2,     3, 0, GPIO_LABEL_OUTPUT__CHARGE,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO2  - "CHARGE"        output   (pin 6   of EG915U)
    {QUEC_PIN_DNAME_GPIO_20,    1, 0, GPIO_LABEL_OUTPUT__VOLT_1,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO20 - "VOLT_1"        output   (pin 28  of EG915U)
    {QUEC_PIN_DNAME_GPIO_19,    1, 0, GPIO_LABEL_OUTPUT__VOLT_2,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO19 - "VOLT_2"        output   (pin 36  of EG915U)
    {QUEC_PIN_DNAME_GPIO_21,    1, 0, GPIO_LABEL_OUTPUT__SHUNT_1,       GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO21 - "SHUNT_1"       output   (pin 27  of EG915U)
    {QUEC_PIN_DNAME_GPIO_5,     0, 0, GPIO_LABEL_OUTPUT__SHUNT_2,       GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO5  - "SHUNT_2"       output   (pin 20  of EG915U)
    {QUEC_PIN_DNAME_GPIO_8,     0, 0, GPIO_LABEL_OUTPUT__PULL_1,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_HIGH     },    // GPIO8  - "PULL_1"        output   (pin 104 of EG915U)
    {QUEC_PIN_DNAME_I2C_M1_SDA, 0, 4, GPIO_LABEL_OUTPUT__PULL_2,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_HIGH     },    // GPIO17 - "PULL_2"        output   (pin 114 of EG915U)
};

/* digital inputs               status (physical status) */
static bool               DrvGpio_StatusInputs[DRV_GPIO__NUM_INPUTS];   // = {FALSE, ..., FALSE};

/* digital output OUT1 and OUT2 status (physical status) */
static bool               DrvGpio_StatusOut1;                           // = FALSE;
static bool               DrvGpio_StatusOut2;                           // = FALSE;

/* charge status (physical status) */
static bool               DrvGpio_StatusCharge;                         // = FALSE;

/* CRC variables */
static u16                DrvGpio_VarCrc;                               // = 0x0000;

/* indication of input enabled (IN1 and IN2) */
static bool               DrvGpio_InputEnabled_In1 = TRUE;
static bool               DrvGpio_InputEnabled_In2 = TRUE;

/* registered input event signals */
static u8                 DrvGpio_NumberSignalsDigitalInputEvents = 0;                         /* number of registered input event signals */
static SIGNAL_INPUT_EVENT DrvGpio_SignalsDigitalInputEvents[NUM_MAX_SIGNALS_DIGITAL_INPUTS];   /*           registered input event signals */

/* debug string */
static ascii              DrvGpio_DebugString[DEBUG_STRING_LENTGH + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void   DrvGpio_TaskDrvGpio(void *argument);

/* status variables */
static void   DrvGpio_InitVar        (void);
static u16    DrvGpio_CalculateVarCrc(void);
       void   DrvGpio_UpdateVarCrc   (void);
       bool   DrvGpio_VerifyVarCrc   (void);


/*-----------------------------------------------------------------------------
 * Digital inputs
 *-----------------------------------------------------------------------------*/
/* read inputs */
       bool   DrvGpio_ReadInput (u8 input_id);

/* read inputs descriptions */
       ascii *DrvGpio_ReadInputDescription(u8 input_id);

/* registered signals of digital input event */
       void   DrvGpio_RegisterSignalDigitalInputEvent(u8 input_id, void (*ptr_function)(u8, bool));


/*-----------------------------------------------------------------------------
 * Digital outputs
 *-----------------------------------------------------------------------------*/
/* slow actions on outputs */
       void   DrvGpio_SetOutput (u8 output_id, bool state, u32 time_impulse);
static void   DrvGpio_SetOutput2(u8 output_id, bool state, u32 time_impulse);

/* fast actions on outputs */
       void   DrvGpio_SetOutputOut1Low        (void);
       void   DrvGpio_SetOutputOut1High       (void);

       void   DrvGpio_SetOutputOut2Low        (void);
       void   DrvGpio_SetOutputOut2High       (void);

       void   DrvGpio_SetOutputLedGsmGreenLow (void);
       void   DrvGpio_SetOutputLedGsmGreenHigh(void);

       void   DrvGpio_SetOutputChargeLow      (void);
       void   DrvGpio_SetOutputChargeHigh     (void);

       void   DrvGpio_SetOutputVolt1Low       (void);
       void   DrvGpio_SetOutputVolt1High      (void);

       void   DrvGpio_SetOutputVolt2Low       (void);
       void   DrvGpio_SetOutputVolt2High      (void);

       void   DrvGpio_SetOutputShunt1Low      (void);
       void   DrvGpio_SetOutputShunt1High     (void);

       void   DrvGpio_SetOutputShunt2Low      (void);
       void   DrvGpio_SetOutputShunt2High     (void);

       void   DrvGpio_SetOutputPull1Low       (void);
       void   DrvGpio_SetOutputPull1High      (void);

       void   DrvGpio_SetOutputPull2Low       (void);
       void   DrvGpio_SetOutputPull2High      (void);

/* fast actions on OUT1 and OUT2 outputs */
       void   DrvGpio_DisactivateOut1         (void);
       void   DrvGpio_ActivateOut1            (void);

       void   DrvGpio_DisactivateOut2         (void);
       void   DrvGpio_ActivateOut2            (void);

/* fast actions on CHARGE output */
       void   DrvGpio_DisactivateCharger      (void);
       void   DrvGpio_ActivateCharger         (void);

/* read outputs */
       bool   DrvGpio_ReadOutput(u8 output_id);

/* read OUT1/OUT2 digital outputs */
       bool   DrvGpio_ReadOut1(void);
       bool   DrvGpio_ReadOut2(void);

/* read outputs descriptions */
       ascii *DrvGpio_ReadOutputDescription(u8 output_id);


/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message     callback functions */
static void   DrvGpio_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);

/* GPIOs event callback functions */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
static void   DrvGpio_AdlCallback_RstIoEvent  (void *param);
#endif
static void   DrvGpio_AdlCallback_PwrOkEvent  (void *param);
static void   DrvGpio_AdlCallback_KeyOut1Event(void *param);
static void   DrvGpio_AdlCallback_KeyOut2Event(void *param);
static void   DrvGpio_AdlCallback_In1Event    (void *param);
static void   DrvGpio_AdlCallback_In2Event    (void *param);

/* interrupt callback functions */
static void   DrvGpio_AdlCallback_C1Event     (void *param);
static void   DrvGpio_AdlCallback_C2Event     (void *param);




/*=============================================================================
 * Function   : DrvGpio_TaskDrvGpio
 *
 * Description: task GPIOs
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_TaskDrvGpio(void *argument)
{
    ql_event_t                 event;
    QlOSStatus                 err;

    COUNTERS__CONFIG__INPUT_C1 config_input_c1;
    COUNTERS__CONFIG__INPUT_C2 config_input_c2;

    ENERGY__CONFIG__ADC_VALUE  config_adc_values;

    u8                         index;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - DRV_GPIO - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* get the configuration counter input C1 */
    /* get the configuration counter input C2 */
    Counters_Config_InputC1_Get(&config_input_c1);
    Counters_Config_InputC2_Get(&config_input_c2);

    /* check if IN1 and IN2 inputs are enabled */
    if (!config_input_c1.enabled_status)
        DrvGpio_InputEnabled_In1 = TRUE;
    else
        DrvGpio_InputEnabled_In1 = FALSE;
    if (!config_input_c2.enabled_status)
        DrvGpio_InputEnabled_In2 = TRUE;
    else
        DrvGpio_InputEnabled_In2 = FALSE;


    /* GPIO init */
    for (index = 0; index < DRV_GPIO__NUM_GPIOS; index++)
    {
        ql_pin_set_func(DrvGpio_GpioConfig[index].pin_num, DrvGpio_GpioConfig[index].gpio_func);

        ql_gpio_deinit(DrvGpio_GpioConfig[index].gpio_num);
        ql_gpio_init(DrvGpio_GpioConfig[index].gpio_num, DrvGpio_GpioConfig[index].gpio_dir, DrvGpio_GpioConfig[index].gpio_pull, DrvGpio_GpioConfig[index].gpio_lvl);
    }


    /* get the "ADC value" configuration */
    Energy_Config_AdcValue_Get(&config_adc_values);

    /* set digital outputs to init values */
    if (config_adc_values.alarm_enabled_status)
    {
        DrvGpio_SetOutputPull1Low();
        DrvGpio_SetOutputPull2Low();
    }


    /* subscribe GPIOs */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    ql_int_register(GPIO_LABEL_INPUT__RST_IO,    EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH, PULL_NONE, DrvGpio_AdlCallback_RstIoEvent,   NULL);
#endif
    ql_int_register(GPIO_LABEL_INPUT__PWR_OK,    EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH, PULL_NONE, DrvGpio_AdlCallback_PwrOkEvent,   NULL);
    ql_int_register(GPIO_LABEL_INPUT__KEY_OUT_1, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH, PULL_NONE, DrvGpio_AdlCallback_KeyOut1Event, NULL);
    ql_int_register(GPIO_LABEL_INPUT__KEY_OUT_2, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH, PULL_NONE, DrvGpio_AdlCallback_KeyOut2Event, NULL);

    if (DrvGpio_InputEnabled_In1)
        ql_int_register(GPIO_LABEL_INPUT__IN1, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH,   PULL_NONE, DrvGpio_AdlCallback_In1Event, NULL);
    else
        ql_int_register(GPIO_LABEL_INPUT__IN1, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_RISING, PULL_NONE, DrvGpio_AdlCallback_C1Event,  NULL);

    if (DrvGpio_InputEnabled_In2)
        ql_int_register(GPIO_LABEL_INPUT__IN2, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH,   PULL_NONE, DrvGpio_AdlCallback_In2Event, NULL);
    else
        ql_int_register(GPIO_LABEL_INPUT__IN2, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_RISING, PULL_NONE, DrvGpio_AdlCallback_C2Event,  NULL);


    /* IRQ enable */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    ql_int_enable(GPIO_LABEL_INPUT__RST_IO);
#endif
    ql_int_enable(GPIO_LABEL_INPUT__PWR_OK);
    ql_int_enable(GPIO_LABEL_INPUT__KEY_OUT_1);
    ql_int_enable(GPIO_LABEL_INPUT__KEY_OUT_2);
    ql_int_enable(GPIO_LABEL_INPUT__IN1);
    ql_int_enable(GPIO_LABEL_INPUT__IN2);


    /* internal status init */
    DrvGpio_InitVar();


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            DrvGpio_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DrvGpio_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* digital inputs               status (physical status) */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
        DrvGpio_StatusInputs[DRVGPIO__IN__RST_IO   ] = DrvGpio_ReadInput(DRVGPIO__IN__RST_IO   );    // GPIO12 - "RST_IO"    input
#endif
        DrvGpio_StatusInputs[DRVGPIO__IN__PWR_OK   ] = DrvGpio_ReadInput(DRVGPIO__IN__PWR_OK   );    // GPIO19 - "PWR_OK"    input
        DrvGpio_StatusInputs[DRVGPIO__IN__KEY_OUT_1] = DrvGpio_ReadInput(DRVGPIO__IN__KEY_OUT_1);    // GPIO17 - "KEY_OUT_1" input
        DrvGpio_StatusInputs[DRVGPIO__IN__KEY_OUT_2] = DrvGpio_ReadInput(DRVGPIO__IN__KEY_OUT_2);    // GPIO16 - "KEY_OUT_2" input
        DrvGpio_StatusInputs[DRVGPIO__IN__IN1      ] = DrvGpio_ReadInput(DRVGPIO__IN__IN1      );    // GPIO03 - "IN1"       input
        DrvGpio_StatusInputs[DRVGPIO__IN__IN2      ] = DrvGpio_ReadInput(DRVGPIO__IN__IN2      );    // GPIO25 - "IN2"       input

        /* digital output OUT1 and OUT2 status (physical status) */
        DrvGpio_StatusOut1 = FALSE;
        DrvGpio_StatusOut2 = FALSE;

        /* charge status (physical status) */
        DrvGpio_StatusCharge = FALSE;


        /**** update the variable CRC ****/
        DrvGpio_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** outputs ****/

        /* set digital outputs to init values */
      //DrvGpio_SetOutputOut1Low();
      //DrvGpio_SetOutputOut2Low();
        DrvGpio_SetOutputLedGsmGreenLow();
        DrvGpio_SetOutputChargeLow();

        /* disable output OUT1 */
        // NOTE: Commented because the HW REV01 leaves the OUT1 on at the startup if it was on when power supply has been removed
        //DrvGpio_DisactivateOut1();       /* disactivate digital output OUT1 */

        /* disable output OUT2 */
        // NOTE: Commented because the HW REV01 leaves the OUT2 on at the startup if it was on when power supply has been removed
        //DrvGpio_DisactivateOut2();       /* disactivate digital output OUT2 */

        /* disable charger */
        DrvGpio_DisactivateCharger();
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time
        // NO TIMER


        /**** outputs ****/

        if (DrvGpio_StatusOut1)
        {
            /* enable  output OUT1 */
            DrvGpio_ActivateOut1();          /* activate    digital output OUT1 */
        }
        else
        {
            /* disable output OUT1 */
            DrvGpio_DisactivateOut1();       /* disactivate digital output OUT1 */
        }

        if (DrvGpio_StatusOut2)
        {
            /* enable  output OUT2 */
            DrvGpio_ActivateOut2();          /* activate    digital output OUT2 */
        }
        else
        {
            /* disable output OUT2 */
            DrvGpio_DisactivateOut2();       /* disactivate digital output OUT2 */
        }

        if (DrvGpio_StatusCharge)
        {
            /* enable  charger */
            DrvGpio_ActivateCharger();
        }
        else
        {
            /* disable charger */
            DrvGpio_DisactivateCharger();
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 DrvGpio_CalculateVarCrc(void)
{
    u16 crc;
    u8  i;


    crc = 0x0000;

    /* variables */
    for (i = 0; i < DRV_GPIO__NUM_INPUTS; i++)
        crc = Utility_CalculateCRC16((u8 *)&DrvGpio_StatusInputs[i], sizeof(DrvGpio_StatusInputs[i]), crc);
    crc =     Utility_CalculateCRC16((u8 *)&DrvGpio_StatusOut1     , sizeof(DrvGpio_StatusOut1     ), crc);
    crc =     Utility_CalculateCRC16((u8 *)&DrvGpio_StatusOut2     , sizeof(DrvGpio_StatusOut2     ), crc);
    crc =     Utility_CalculateCRC16((u8 *)&DrvGpio_StatusCharge   , sizeof(DrvGpio_StatusCharge   ), crc);

    return crc;
}




/*=============================================================================
 * Function   : DrvGpio_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_UpdateVarCrc(void)
{
    DrvGpio_VarCrc = DrvGpio_CalculateVarCrc();
}




/*=============================================================================
 * Function   : DrvGpio_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool DrvGpio_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = DrvGpio_CalculateVarCrc();

    if (DrvGpio_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*=============================================================================
 * Function   : DrvGpio_ReadInput
 *
 * Description: read a digital input
 * Input      : - input_id: input ID to be read
 * Output     : - FALSE: the input is low
 *              - TRUE : the input is high
 *=============================================================================*/
bool DrvGpio_ReadInput(u8 input_id)
{
    ql_errcode_gpio err;

    ql_GpioNum      handle_input;
    ql_LvlMode      input_status;


    /* verify input ID */
    if (
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
         (input_id != DRVGPIO__IN__RST_IO   ) &&    // GPIO12 - "RST_IO"    input
#endif
         (input_id != DRVGPIO__IN__PWR_OK   ) &&    // GPIO19 - "PWR_OK"    input
         (input_id != DRVGPIO__IN__KEY_OUT_1) &&    // GPIO17 - "KEY_OUT_1" input
         (input_id != DRVGPIO__IN__KEY_OUT_2) &&    // GPIO16 - "KEY_OUT_2" input
         (input_id != DRVGPIO__IN__IN1      ) &&    // GPIO03 - "IN1"       input
         (input_id != DRVGPIO__IN__IN2      )       // GPIO25 - "IN2"       input
       )
    {
        return FALSE;
    }


    /* read the specified input */
    switch (input_id)
    {
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
        /* GPIO12 - "RST_IO"   input */
        case DRVGPIO__IN__RST_IO:
            handle_input = GPIO_LABEL_INPUT__RST_IO;
            break;
#endif

        /* GPIO19 - "PWR_OK"   input */
        case DRVGPIO__IN__PWR_OK:
            handle_input = GPIO_LABEL_INPUT__PWR_OK;
            break;

        /* GPIO17 - "KEY_OUT_1" input */
        case DRVGPIO__IN__KEY_OUT_1:
            handle_input = GPIO_LABEL_INPUT__KEY_OUT_1;
            break;

        /* GPIO16 - "KEY_OUT_2" input */
        case DRVGPIO__IN__KEY_OUT_2:
            handle_input = GPIO_LABEL_INPUT__KEY_OUT_2;
            break;

        /* GPIO03 - "IN1"      input */
        case DRVGPIO__IN__IN1:
            handle_input = GPIO_LABEL_INPUT__IN1;
            break;

        /* GPIO25 - "IN2"      input */
        /* unknown input */
        case DRVGPIO__IN__IN2:
        default:
            handle_input = GPIO_LABEL_INPUT__IN2;
            break;
    }
    err = ql_gpio_get_level(handle_input, &input_status);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_get_level - ERROR: %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_get_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    if (input_status == LVL_HIGH)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : DrvGpio_ReadInputDescription
 *
 * Description: read a digital input description
 * Input      : - input_id: input ID to be read
 * Output     : - pointer to input description
 *=============================================================================*/
ascii *DrvGpio_ReadInputDescription(u8 input_id)
{
    static const ascii *input_description_strings[] =
    {
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
        GPIO_DESCRIPTION_INPUT__RST_IO,
#endif
        GPIO_DESCRIPTION_INPUT__PWR_OK,
        GPIO_DESCRIPTION_INPUT__KEY_OUT_1,
        GPIO_DESCRIPTION_INPUT__KEY_OUT_2,
        GPIO_DESCRIPTION_INPUT__IN1,
        GPIO_DESCRIPTION_INPUT__IN2,
    };


    /* verify input ID */
    if (
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
         (input_id != DRVGPIO__IN__RST_IO   ) &&    // GPIO12 - "RST_IO"    input
#endif
         (input_id != DRVGPIO__IN__PWR_OK   ) &&    // GPIO19 - "PWR_OK"    input
         (input_id != DRVGPIO__IN__KEY_OUT_1) &&    // GPIO17 - "KEY_OUT_1" input
         (input_id != DRVGPIO__IN__KEY_OUT_2) &&    // GPIO16 - "KEY_OUT_2" input
         (input_id != DRVGPIO__IN__IN1      ) &&    // GPIO03 - "IN1"       input
         (input_id != DRVGPIO__IN__IN2      )       // GPIO25 - "IN2"       input
       )
    {
        return NULL;
    }


    return ((ascii *)input_description_strings[input_id]);
}




/*=============================================================================
 * Function   : DrvGpio_RegisterSignalDigitalInputEvent
 *
 * Description: register a function to be called when there is a digital input event
 * Input      : - input_id   : input ID
 *              - ptr_funcion: pointer to the function to be called
 * Output     : -
 *=============================================================================*/
void DrvGpio_RegisterSignalDigitalInputEvent(u8 input_id, void (*ptr_function)(u8, bool))
{
    if (DrvGpio_NumberSignalsDigitalInputEvents < NUM_MAX_SIGNALS_DIGITAL_INPUTS)
    {
        DrvGpio_SignalsDigitalInputEvents[DrvGpio_NumberSignalsDigitalInputEvents].input_id     = input_id;
        DrvGpio_SignalsDigitalInputEvents[DrvGpio_NumberSignalsDigitalInputEvents].ptr_function = ptr_function;
        DrvGpio_NumberSignalsDigitalInputEvents++;
    }
}




/*=============================================================================
 * Function   : DrvGpio_SetOutput
 *
 * Description: set an output
 * Input      : - output_id   : output id
 *              - state       : output state
 *              - time_impulse: time impulse (ms) (0= no impulse)
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutput(u8 output_id, bool state, u32 time_impulse)
{
    ql_event_t event;
    QlOSStatus err;


    /* verify output id */
    if (
         (output_id != DRVGPIO__OUT__OUT_1        ) &&    // GPIO21 - "OUT_1"         output
         (output_id != DRVGPIO__OUT__OUT_2        ) &&    // GPIO23 - "OUT_2"         output
         (output_id != DRVGPIO__OUT__LED_GSM_GREEN) &&    // GPIO15 - "LED_GSM_GREEN" output
         (output_id != DRVGPIO__OUT__CHARGE       ) &&    // GPIO24 - "CHARGE"        output
         (output_id != DRVGPIO__OUT__VOLT_1       ) &&    // GPIO20 - "VOLT_1"        output
         (output_id != DRVGPIO__OUT__VOLT_2       ) &&    // GPIO19 - "VOLT_2"        output
         (output_id != DRVGPIO__OUT__SHUNT_1      ) &&    // GPIO21 - "SHUNT_1"       output
         (output_id != DRVGPIO__OUT__SHUNT_2      ) &&    // GPIO5  - "SHUNT_2"       output
         (output_id != DRVGPIO__OUT__PULL_1       ) &&    // GPIO8  - "PULL_1"        output
         (output_id != DRVGPIO__OUT__PULL_2       )       // GPIO17 - "PULL_2"        output
       )
    {
        return;
    }


    event.id     = DRVGPIO__TASK_MSG_ID__SET_OUTPUT;
    event.param1 = output_id;
    event.param2 = state;
    event.param3 = time_impulse;

    err = ql_rtos_event_send(Boot_TaskRef_DrvGpio, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvGpio_SetOutput2
 *
 * Description: set an output
 * Input      : - output_id   : output id
 *              - state       : output state
 *              - time_impulse: time impulse (ms) (0= no impulse)  (not implemented)
 * Output     : -
 *=============================================================================*/
static void DrvGpio_SetOutput2(u8 output_id, bool state, u32 time_impulse)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    /* verify output id */
    if (
         (output_id != DRVGPIO__OUT__OUT_1        ) &&    // GPIO21 - "OUT_1"         output
         (output_id != DRVGPIO__OUT__OUT_2        ) &&    // GPIO23 - "OUT_2"         output
         (output_id != DRVGPIO__OUT__LED_GSM_GREEN) &&    // GPIO15 - "LED_GSM_GREEN" output
         (output_id != DRVGPIO__OUT__CHARGE       ) &&    // GPIO24 - "CHARGE"        output
         (output_id != DRVGPIO__OUT__VOLT_1       ) &&    // GPIO20 - "VOLT_1"        output
         (output_id != DRVGPIO__OUT__VOLT_2       ) &&    // GPIO19 - "VOLT_2"        output
         (output_id != DRVGPIO__OUT__SHUNT_1      ) &&    // GPIO21 - "SHUNT_1"       output
         (output_id != DRVGPIO__OUT__SHUNT_2      ) &&    // GPIO5  - "SHUNT_2"       output
         (output_id != DRVGPIO__OUT__PULL_1       ) &&    // GPIO8  - "PULL_1"        output
         (output_id != DRVGPIO__OUT__PULL_2       )       // GPIO17 - "PULL_2"        output
       )
    {
        return;
    }


    /* set the specified output */
    switch (output_id)
    {
        case DRVGPIO__OUT__OUT_1:
            handle_output = GPIO_LABEL_OUTPUT__OUT_1;
            break;

        case DRVGPIO__OUT__OUT_2:
            handle_output = GPIO_LABEL_OUTPUT__OUT_2;
            break;

        case DRVGPIO__OUT__LED_GSM_GREEN:
            handle_output = GPIO_LABEL_OUTPUT__LED_GSM_GREEN;
            break;

        case DRVGPIO__OUT__CHARGE:
            handle_output = GPIO_LABEL_OUTPUT__CHARGE;
            break;

        case DRVGPIO__OUT__VOLT_1:
            handle_output = GPIO_LABEL_OUTPUT__VOLT_1;
            break;

        case DRVGPIO__OUT__VOLT_2:
            handle_output = GPIO_LABEL_OUTPUT__VOLT_2;
            break;

        case DRVGPIO__OUT__SHUNT_1:
            handle_output = GPIO_LABEL_OUTPUT__SHUNT_1;
            break;

        case DRVGPIO__OUT__SHUNT_2:
            handle_output = GPIO_LABEL_OUTPUT__SHUNT_2;
            break;

        case DRVGPIO__OUT__PULL_1:
            handle_output = GPIO_LABEL_OUTPUT__PULL_1;
            break;

        case DRVGPIO__OUT__PULL_2:
        default:
            handle_output = GPIO_LABEL_OUTPUT__PULL_2;
            break;
    }
    err = ql_gpio_set_level(handle_output, state);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputOut1Low
 *
 * Description: set the "OUT_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputOut1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__OUT_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputOut1High
 *
 * Description: set the "OUT_1" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputOut1High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__OUT_1;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputOut2Low
 *
 * Description: set the "OUT_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputOut2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__OUT_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputOut2High
 *
 * Description: set the "OUT_2" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputOut2High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__OUT_2;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputLedGsmGreenLow
 *
 * Description: set the "LED_GSM_GREEN" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputLedGsmGreenLow(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__LED_GSM_GREEN;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputLedGsmGreenHigh
 *
 * Description: set the "LED_GSM_GREEN" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputLedGsmGreenHigh(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__LED_GSM_GREEN;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputChargeLow
 *
 * Description: set the "CHARGE" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputChargeLow(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__CHARGE;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputChargeHigh
 *
 * Description: set the "CHARGE" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputChargeHigh(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__CHARGE;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputVolt1Low
 *
 * Description: set the "VOLT_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputVolt1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__VOLT_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputVolt1High
 *
 * Description: set the "VOLT_1" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputVolt1High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__VOLT_1;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputVolt2Low
 *
 * Description: set the "VOLT_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputVolt2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__VOLT_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputVolt2High
 *
 * Description: set the "VOLT_2" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputVolt2High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__VOLT_2;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputShunt1Low
 *
 * Description: set the "SHUNT_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputShunt1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__SHUNT_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputShunt1High
 *
 * Description: set the "SHUNT_1" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputShunt1High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__SHUNT_1;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputShunt2Low
 *
 * Description: set the "SHUNT_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputShunt2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__SHUNT_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputShunt2High
 *
 * Description: set the "SHUNT_2" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputShunt2High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__SHUNT_2;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputPull1Low
 *
 * Description: set the "PULL_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputPull1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__PULL_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputPull1High
 *
 * Description: set the "PULL_1" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputPull1High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__PULL_1;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputPull2Low
 *
 * Description: set the "PULL_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputPull2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__PULL_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_SetOutputPull2High
 *
 * Description: set the "PULL_2" output high
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SetOutputPull2High(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__PULL_2;

    err = ql_gpio_set_level(handle_output, LVL_HIGH);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
  //else
  //{
  //    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - OK");
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*=============================================================================
 * Function   : DrvGpio_DisactivateOut1
 *
 * Description: disactivate digital output OUT1
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_DisactivateOut1(void)
{
    /* disactivate digital output OUT1 */
    DrvGpio_SetOutputOut1Low();


    DrvGpio_StatusOut1 = FALSE;

    DrvGpio_UpdateVarCrc();


    /* signal output OUT1 disactivation */
    DrvTemperature_OutputOut1Off();
}




/*=============================================================================
 * Function   : DrvGpio_ActivateOut1
 *
 * Description: activate digital output OUT1
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_ActivateOut1(void)
{
    /* activate digital output OUT1 */
    DrvGpio_SetOutputOut1High();


    DrvGpio_StatusOut1 = TRUE;

    DrvGpio_UpdateVarCrc();


    /* signal output OUT1 activation */
    DrvTemperature_OutputOut1On();
}




/*=============================================================================
 * Function   : DrvGpio_DisactivateOut2
 *
 * Description: disactivate digital output OUT2
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_DisactivateOut2(void)
{
    /* disactivate digital output OUT2 */
    DrvGpio_SetOutputOut2Low();


    DrvGpio_StatusOut2 = FALSE;

    DrvGpio_UpdateVarCrc();


    /* signal output OUT2 disactivation */
    DrvTemperature_OutputOut2Off();
}




/*=============================================================================
 * Function   : DrvGpio_ActivateOut2
 *
 * Description: activate digital output OUT2
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_ActivateOut2(void)
{
    /* activate digital output OUT2 */
    DrvGpio_SetOutputOut2High();


    DrvGpio_StatusOut2 = TRUE;

    DrvGpio_UpdateVarCrc();


    /* signal output OUT2 activation */
    DrvTemperature_OutputOut2On();
}




/*=============================================================================
 * Function   : DrvGpio_DisactivateCharger
 *
 * Description: disactivate charger
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_DisactivateCharger(void)
{
    /* disactivate charge output */
    DrvGpio_SetOutputChargeLow();

    DrvGpio_StatusCharge = FALSE;

    DrvGpio_UpdateVarCrc();
}




/*=============================================================================
 * Function   : DrvGpio_ActivateCharger
 *
 * Description: activate charger
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_ActivateCharger(void)
{
    /* activate charge output */
    DrvGpio_SetOutputChargeHigh();

    DrvGpio_StatusCharge = TRUE;

    DrvGpio_UpdateVarCrc();
}




/*=============================================================================
 * Function   : DrvGpio_ReadOutput
 *
 * Description: read a digital output
 * Input      : - output_id: output ID to be read
 * Output     : - FALSE: the output is low
 *              - TRUE : the output is high
 *=============================================================================*/
bool DrvGpio_ReadOutput(u8 output_id)
{
    ql_errcode_gpio err;

    ql_GpioNum      handle_output;
    ql_LvlMode      output_state;


    /* verify output ID */
    if (
         (output_id != DRVGPIO__OUT__OUT_1        ) &&    // GPIO21 - "OUT_1"         output
         (output_id != DRVGPIO__OUT__OUT_2        ) &&    // GPIO23 - "OUT_2"         output
         (output_id != DRVGPIO__OUT__LED_GSM_GREEN) &&    // GPIO15 - "LED_GSM_GREEN" output
         (output_id != DRVGPIO__OUT__CHARGE       ) &&    // GPIO24 - "CHARGE"        output
         (output_id != DRVGPIO__OUT__VOLT_1       ) &&    // GPIO20 - "VOLT_1"        output
         (output_id != DRVGPIO__OUT__VOLT_2       ) &&    // GPIO19 - "VOLT_2"        output
         (output_id != DRVGPIO__OUT__SHUNT_1      ) &&    // GPIO21 - "SHUNT_1"       output
         (output_id != DRVGPIO__OUT__SHUNT_2      ) &&    // GPIO5  - "SHUNT_2"       output
         (output_id != DRVGPIO__OUT__PULL_1       ) &&    // GPIO8  - "PULL_1"        output
         (output_id != DRVGPIO__OUT__PULL_2       )       // GPIO17 - "PULL_2"        output
       )
    {
        return FALSE;
    }


    /* read the specified input */
    switch (output_id)
    {
        case DRVGPIO__OUT__OUT_1:
            handle_output = GPIO_LABEL_OUTPUT__OUT_1;
            break;

        case DRVGPIO__OUT__OUT_2:
            handle_output = GPIO_LABEL_OUTPUT__OUT_2;
            break;

        case DRVGPIO__OUT__LED_GSM_GREEN:
            handle_output = GPIO_LABEL_OUTPUT__LED_GSM_GREEN;
            break;

        case DRVGPIO__OUT__CHARGE:
            handle_output = GPIO_LABEL_OUTPUT__CHARGE;
            break;

        case DRVGPIO__OUT__VOLT_1:
            handle_output = GPIO_LABEL_OUTPUT__VOLT_1;
            break;

        case DRVGPIO__OUT__VOLT_2:
            handle_output = GPIO_LABEL_OUTPUT__VOLT_2;
            break;

        case DRVGPIO__OUT__SHUNT_1:
            handle_output = GPIO_LABEL_OUTPUT__SHUNT_1;
            break;

        case DRVGPIO__OUT__SHUNT_2:
            handle_output = GPIO_LABEL_OUTPUT__SHUNT_2;
            break;

        case DRVGPIO__OUT__PULL_1:
            handle_output = GPIO_LABEL_OUTPUT__PULL_1;
            break;

        case DRVGPIO__OUT__PULL_2:
        default:
            handle_output = GPIO_LABEL_OUTPUT__PULL_2;
            break;
    }
    err = ql_gpio_get_level(handle_output, &output_state);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "QL FUNCTION - ql_gpio_get_level - ERROR: %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_get_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return ((bool)output_state);
}




/*=============================================================================
 * Function   : DrvGpio_ReadOut1
 *
 * Description: read the OUT1 digital output
 * Input      : -
 * Output     : - OUT1 digital output status
 *=============================================================================*/
bool DrvGpio_ReadOut1(void)
{
    return DrvGpio_StatusOut1;
}




/*=============================================================================
 * Function   : DrvGpio_ReadOut2
 *
 * Description: read the OUT2 digital output
 * Input      : -
 * Output     : - OUT2 digital output status
 *=============================================================================*/
bool DrvGpio_ReadOut2(void)
{
    return DrvGpio_StatusOut2;
}




/*=============================================================================
 * Function   : DrvGpio_ReadOutputDescription
 *
 * Description: read a digital output description
 * Input      : - output_id: output ID to be read
 * Output     : - pointer to output description
 *=============================================================================*/
ascii *DrvGpio_ReadOutputDescription(u8 output_id)
{
    static const ascii *output_description_strings[] =
    {
        GPIO_DESCRIPTION_OUTPUT__OUT_1,
        GPIO_DESCRIPTION_OUTPUT__OUT_2,
        GPIO_DESCRIPTION_OUTPUT__LED_GSM_GREEN,
        GPIO_DESCRIPTION_OUTPUT__CHARGE,
        GPIO_DESCRIPTION_OUTPUT__VOLT_1,
        GPIO_DESCRIPTION_OUTPUT__VOLT_2,
        GPIO_DESCRIPTION_OUTPUT__SHUNT_1,
        GPIO_DESCRIPTION_OUTPUT__SHUNT_2,
        GPIO_DESCRIPTION_OUTPUT__PULL_1,
        GPIO_DESCRIPTION_OUTPUT__PULL_2,
    };


    /* verify output id */
    if (
         (output_id != DRVGPIO__OUT__OUT_1        ) &&    // GPIO21 - "OUT_1"         output
         (output_id != DRVGPIO__OUT__OUT_2        ) &&    // GPIO23 - "OUT_2"         output
         (output_id != DRVGPIO__OUT__LED_GSM_GREEN) &&    // GPIO15 - "LED_GSM_GREEN" output
         (output_id != DRVGPIO__OUT__CHARGE       ) &&    // GPIO24 - "CHARGE"        output
         (output_id != DRVGPIO__OUT__VOLT_1       ) &&    // GPIO20 - "VOLT_1"        output
         (output_id != DRVGPIO__OUT__VOLT_2       ) &&    // GPIO19 - "VOLT_2"        output
         (output_id != DRVGPIO__OUT__SHUNT_1      ) &&    // GPIO21 - "SHUNT_1"       output
         (output_id != DRVGPIO__OUT__SHUNT_2      ) &&    // GPIO5  - "SHUNT_2"       output
         (output_id != DRVGPIO__OUT__PULL_1       ) &&    // GPIO8  - "PULL_1"        output
         (output_id != DRVGPIO__OUT__PULL_2       )       // GPIO17 - "PULL_2"        output
       )
    {
        return NULL;
    }


    return ((ascii *)output_description_strings[output_id]);
}




/*===========================================================================
 * Function   : DrvGpio_AdlCallback_Message_TaskMsg
 *
 * Description: - task main message callback
 * Input      : - msg_identifier:
 * Output     : -
 *===========================================================================*/
static void DrvGpio_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
    u8   output_id;
    bool state;
    u32  time_impulse;


    snprintf(DrvGpio_DebugString, sizeof(DrvGpio_DebugString), "CALLBACK     - MESSAGE     - TASK DRVGPIO - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (msg_identifier->id)
    {
        /* set ouput request */
        case DRVGPIO__TASK_MSG_ID__SET_OUTPUT:
            /* extract the parameters */
            output_id    = msg_identifier->param1;
            state        = msg_identifier->param2;
            time_impulse = msg_identifier->param3;

            /* check the parameters values */
            if (
                 (output_id != DRVGPIO__OUT__OUT_1        ) &&    // GPIO21 - "OUT_1"         output
                 (output_id != DRVGPIO__OUT__OUT_2        ) &&    // GPIO23 - "OUT_2"         output
                 (output_id != DRVGPIO__OUT__LED_GSM_GREEN) &&    // GPIO15 - "LED_GSM_GREEN" output
                 (output_id != DRVGPIO__OUT__CHARGE       ) &&    // GPIO24 - "CHARGE"        output
                 (output_id != DRVGPIO__OUT__VOLT_1       ) &&    // GPIO20 - "VOLT_1"        output
                 (output_id != DRVGPIO__OUT__VOLT_2       ) &&    // GPIO19 - "VOLT_2"        output
                 (output_id != DRVGPIO__OUT__SHUNT_1      ) &&    // GPIO21 - "SHUNT_1"       output
                 (output_id != DRVGPIO__OUT__SHUNT_2      ) &&    // GPIO5  - "SHUNT_2"       output
                 (output_id != DRVGPIO__OUT__PULL_1       ) &&    // GPIO8  - "PULL_1"        output
                 (output_id != DRVGPIO__OUT__PULL_2       )       // GPIO17 - "PULL_2"        output
               )
            {
                return;
            }

            /* set the output */
            DrvGpio_SetOutput2(output_id, state, time_impulse);
            break;


        /* unknown message identifier */
        default:
            return;
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_RstIoEvent
 *
 * Description: GPIO event callback function 1
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
static void DrvGpio_AdlCallback_RstIoEvent(void *param)
{
    bool            input_status;
    u8              i;

    ql_LvlMode      gpio_level;


    ql_gpio_get_level(GPIO_LABEL_INPUT__RST_IO, &gpio_level);

    /* level field definition */
    if (gpio_level == LVL_HIGH)
        input_status = TRUE;
    else
        input_status = FALSE;

    /* verify if the digital input status is changed */
    if (input_status != DrvGpio_StatusInputs[DRVGPIO__IN__RST_IO])
    {
        /* the digital input status is changed */

        /* update the digital input status */
        DrvGpio_StatusInputs[DRVGPIO__IN__RST_IO] = input_status;

        DrvGpio_UpdateVarCrc();

        /* signal the digital input state change to the registered applications */
        for (i = 0; i < DrvGpio_NumberSignalsDigitalInputEvents; i++)
        {
            if (DrvGpio_SignalsDigitalInputEvents[i].input_id == DRVGPIO__IN__RST_IO)
                DrvGpio_SignalsDigitalInputEvents[i].ptr_function(DRVGPIO__IN__RST_IO, input_status);
        }
    }
}
#endif




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_PwrOkEvent
 *
 * Description: GPIO event callback function 2
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_PwrOkEvent(void *param)
{
    bool            input_status;
    u8              i;

    ql_LvlMode      gpio_level;


    ql_gpio_get_level(GPIO_LABEL_INPUT__PWR_OK, &gpio_level);

    /* level field definition */
    if (gpio_level == LVL_HIGH)
        input_status = TRUE;
    else
        input_status = FALSE;

    /* verify if the digital input status is changed */
    if (input_status != DrvGpio_StatusInputs[DRVGPIO__IN__PWR_OK])
    {
        /* the digital input status is changed */

        /* update the digital input status */
        DrvGpio_StatusInputs[DRVGPIO__IN__PWR_OK] = input_status;

        DrvGpio_UpdateVarCrc();

        /* signal the digital input state change to the registered applications */
        for (i = 0; i < DrvGpio_NumberSignalsDigitalInputEvents; i++)
        {
            if (DrvGpio_SignalsDigitalInputEvents[i].input_id == DRVGPIO__IN__PWR_OK)
                DrvGpio_SignalsDigitalInputEvents[i].ptr_function(DRVGPIO__IN__PWR_OK, input_status);
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_KeyOut1Event
 *
 * Description: GPIO event callback function 3
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_KeyOut1Event(void *param)
{
    bool            input_status;
    u8              i;

    ql_LvlMode      gpio_level;


    ql_gpio_get_level(GPIO_LABEL_INPUT__KEY_OUT_1, &gpio_level);

    /* level field definition */
    if (gpio_level == LVL_HIGH)
        input_status = TRUE;
    else
        input_status = FALSE;

    /* verify if the digital input status is changed */
    if (input_status != DrvGpio_StatusInputs[DRVGPIO__IN__KEY_OUT_1])
    {
        /* the digital input status is changed */

        /* update the digital input status */
        DrvGpio_StatusInputs[DRVGPIO__IN__KEY_OUT_1] = input_status;

        DrvGpio_UpdateVarCrc();

        /* signal the digital input state change to the registered applications */
        for (i = 0; i < DrvGpio_NumberSignalsDigitalInputEvents; i++)
        {
            if (DrvGpio_SignalsDigitalInputEvents[i].input_id == DRVGPIO__IN__KEY_OUT_1)
                DrvGpio_SignalsDigitalInputEvents[i].ptr_function(DRVGPIO__IN__KEY_OUT_1, input_status);
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_In1Event
 *
 * Description: GPIO event callback function 5
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_In1Event(void *param)
{
    bool            input_status;
    u8              i;

    ql_LvlMode      gpio_level;


    ql_gpio_get_level(GPIO_LABEL_INPUT__IN1, &gpio_level);

    /* level field definition */
    if (gpio_level == LVL_HIGH)
        input_status = TRUE;
    else
        input_status = FALSE;

    /* verify if the digital input status is changed */
    if (input_status != DrvGpio_StatusInputs[DRVGPIO__IN__IN1])
    {
        /* the digital input status is changed */

        /* update the digital input status */
        DrvGpio_StatusInputs[DRVGPIO__IN__IN1] = input_status;

        DrvGpio_UpdateVarCrc();

        /* signal the digital input state change to the registered applications */
        for (i = 0; i < DrvGpio_NumberSignalsDigitalInputEvents; i++)
        {
            if (DrvGpio_SignalsDigitalInputEvents[i].input_id == DRVGPIO__IN__IN1)
                DrvGpio_SignalsDigitalInputEvents[i].ptr_function(DRVGPIO__IN__IN1, input_status);
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_In2Event
 *
 * Description: GPIO event callback function 6
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_In2Event(void *param)
{
    bool            input_status;
    u8              i;

    ql_LvlMode      gpio_level;


    ql_gpio_get_level(GPIO_LABEL_INPUT__IN2, &gpio_level);

    /* level field definition */
    if (gpio_level == LVL_HIGH)
        input_status = TRUE;
    else
        input_status = FALSE;

    /* verify if the digital input status is changed */
    if (input_status != DrvGpio_StatusInputs[DRVGPIO__IN__IN2])
    {
        /* the digital input status is changed */

        /* update the digital input status */
        DrvGpio_StatusInputs[DRVGPIO__IN__IN2] = input_status;

        DrvGpio_UpdateVarCrc();

        /* signal the digital input state change to the registered applications */
        for (i = 0; i < DrvGpio_NumberSignalsDigitalInputEvents; i++)
        {
            if (DrvGpio_SignalsDigitalInputEvents[i].input_id == DRVGPIO__IN__IN2)
                DrvGpio_SignalsDigitalInputEvents[i].ptr_function(DRVGPIO__IN__IN2, input_status);
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_KeyOut2Event
 *
 * Description: GPIO event callback function 4
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_KeyOut2Event(void *param)
{
    bool            input_status;
    u8              i;

    ql_LvlMode      gpio_level;


    ql_gpio_get_level(GPIO_LABEL_INPUT__KEY_OUT_2, &gpio_level);

    /* level field definition */
    if (gpio_level == LVL_HIGH)
        input_status = TRUE;
    else
        input_status = FALSE;

    /* verify if the digital input status is changed */
    if (input_status != DrvGpio_StatusInputs[DRVGPIO__IN__KEY_OUT_2])
    {
        /* the digital input status is changed */

        /* update the digital input status */
        DrvGpio_StatusInputs[DRVGPIO__IN__KEY_OUT_2] = input_status;

        DrvGpio_UpdateVarCrc();

        /* signal the digital input state change to the registered applications */
        for (i = 0; i < DrvGpio_NumberSignalsDigitalInputEvents; i++)
        {
            if (DrvGpio_SignalsDigitalInputEvents[i].input_id == DRVGPIO__IN__KEY_OUT_2)
                DrvGpio_SignalsDigitalInputEvents[i].ptr_function(DRVGPIO__IN__KEY_OUT_2, input_status);
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_C1Event
 *
 * Description: GPIO event callback function 4
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_C1Event(void *param)
{
    ql_LvlMode      c1_status;
    ql_errcode_gpio err;


    /* read C1 status */
    err = ql_gpio_get_level(GPIO_LABEL_INPUT__IN1, &c1_status);
    if (err == QL_GPIO_SUCCESS)
    {
        if (c1_status == LVL_HIGH)
        {
            /* C1 is still high */

            /* signal the counter input C1 impulse */
            Counters_Event_CounterC1Impulse();
        }
    }
}




/*=============================================================================
 * Function   : DrvGpio_AdlCallback_C2Event
 *
 * Description: GPIO event callback function 4
 * Input      : - param: pointer to the event table
 * Output     : -
 *=============================================================================*/
static void DrvGpio_AdlCallback_C2Event(void *param)
{
    ql_LvlMode      c2_status;
    ql_errcode_gpio err;


    /* read C2 status */
    err = ql_gpio_get_level(GPIO_LABEL_INPUT__IN2, &c2_status);
    if (err == QL_GPIO_SUCCESS)
    {
        if (c2_status == LVL_HIGH)
        {
            /* C2 is still high */

            /* signal the counter input C2 impulse */
            Counters_Event_CounterC2Impulse();
        }
    }
}
