/*=============================================================================
 * File       :  DOTA_DRVGPIO.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - GPIOs management
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
#include "ql_gpio.h"
#include "quec_pin_index.h"

/* user     includes */
#include "debug_my.h"
#include "dota_drvgpio.h"
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* debug string length */
#define DEBUG_STRING_LENTGH                     200

/*-----------------------------------------------------------------------------
 * GPIOs labels
 *-----------------------------------------------------------------------------*/
/* GPIO digital input  labels */
#define GPIO_LABEL_INPUT__PWR_OK                GPIO_3                  // GPIO3  - "PWR_OK"        input    (pin 7  of EG915U)
#define GPIO_LABEL_INPUT__KEY_OUT_1             GPIO_9                  // GPIO9  - "KEY_OUT_1"     input    (pin 26 of EG915U)
#define GPIO_LABEL_INPUT__KEY_OUT_2             GPIO_10                 // GPIO10 - "KEY_OUT_2"     input    (pin 25 of EG915U)
#define GPIO_LABEL_INPUT__IN1                   GPIO_0                  // GPIO0  - "IN1"           input    (pin 4  of EG915U)
#define GPIO_LABEL_INPUT__IN2                   GPIO_1                  // GPIO1  - "IN2"           input    (pin 5  of EG915U)

/* GPIO digital output labels */
#define GPIO_LABEL_OUTPUT__OUT_1                GPIO_14                 // GPIO14 - "OUT_1"         output   (pin 40 of EG915U)
#define GPIO_LABEL_OUTPUT__OUT_2                GPIO_15                 // GPIO15 - "OUT_2"         output   (pin 41 of EG915U)
#define GPIO_LABEL_OUTPUT__LED_GSM_GREEN        GPIO_11                 // GPIO11 - "LED_GSM_GREEN" output   (pin 64 of EG915U)
#define GPIO_LABEL_OUTPUT__CHARGE               GPIO_2                  // GPIO2  - "CHARGE"        output   (pin 6  of EG915U)
#define GPIO_LABEL_OUTPUT__VOLT_1               GPIO_20                 // GPIO20 - "VOLT_1"        output   (pin 28  of EG915U)
#define GPIO_LABEL_OUTPUT__VOLT_2               GPIO_19                 // GPIO19 - "VOLT_2"        output   (pin 36  of EG915U)
#define GPIO_LABEL_OUTPUT__SHUNT_1              GPIO_21                 // GPIO21 - "SHUNT_1"       output   (pin 27  of EG915U)
#define GPIO_LABEL_OUTPUT__SHUNT_2              GPIO_5                  // GPIO5  - "SHUNT_2"       output   (pin 20  of EG915U)
#define GPIO_LABEL_OUTPUT__PULL_1               GPIO_8                  // GPIO8  - "PULL_1"        output   (pin 104 of EG915U)
#define GPIO_LABEL_OUTPUT__PULL_2               GPIO_17                 // GPIO17 - "PULL_2"        output   (pin 114 of EG915U)




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
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
/* GPIOs config table */
static GPIO_INFO DotaDrvGpio_GpioConfig[DOTA_DRVGPIO__NUM_GPIOS] =
{
    // digital inputs
    {QUEC_PIN_DNAME_GPIO_3,     3, 0, GPIO_LABEL_INPUT__PWR_OK,         GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO3  - "PWR_OK"        input    (pin 7   of EG915U)
    {QUEC_PIN_DNAME_GPIO_9,     1, 0, GPIO_LABEL_INPUT__KEY_OUT_1,      GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO9  - "KEY_OUT_1"     input    (pin 26  of EG915U)
    {QUEC_PIN_DNAME_GPIO_10,    1, 0, GPIO_LABEL_INPUT__KEY_OUT_2,      GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO10 - "KEY_OUT_2"     input    (pin 25  of EG915U)
    {QUEC_PIN_DNAME_GPIO_0,     3, 0, GPIO_LABEL_INPUT__IN1,            GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO0  - "IN1"           input    (pin 4   of EG915U)
    {QUEC_PIN_DNAME_GPIO_1,     3, 0, GPIO_LABEL_INPUT__IN2,            GPIO_INPUT,  PULL_NONE,     QUEC_PIN_NONE},    // GPIO1  - "IN2"           input    (pin 5   of EG915U)

    // digital outputs
    {QUEC_PIN_DNAME_GPIO_14,    1, 0, GPIO_LABEL_OUTPUT__OUT_1,         GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO14 - "OUT_1"         output   (pin 40  of EG915U)
    {QUEC_PIN_DNAME_GPIO_15,    1, 0, GPIO_LABEL_OUTPUT__OUT_2,         GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO15 - "OUT_2"         output   (pin 41  of EG915U)
    {QUEC_PIN_DNAME_GPIO_11,    1, 0, GPIO_LABEL_OUTPUT__LED_GSM_GREEN, GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO11 - "LED_GSM_GREEN" output   (pin 64  of EG915U)
    {QUEC_PIN_DNAME_GPIO_2,     3, 0, GPIO_LABEL_OUTPUT__CHARGE,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO2  - "CHARGE"        output   (pin 6   of EG915U)
    {QUEC_PIN_DNAME_GPIO_20,    1, 0, GPIO_LABEL_OUTPUT__VOLT_1,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO20 - "VOLT_1"        output   (pin 28  of EG915U)
    {QUEC_PIN_DNAME_GPIO_19,    1, 0, GPIO_LABEL_OUTPUT__VOLT_2,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO19 - "VOLT_2"        output   (pin 36  of EG915U)
    {QUEC_PIN_DNAME_GPIO_21,    1, 0, GPIO_LABEL_OUTPUT__SHUNT_1,       GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO21 - "SHUNT_1"       output   (pin 27  of EG915U)
    {QUEC_PIN_DNAME_GPIO_5,     0, 0, GPIO_LABEL_OUTPUT__SHUNT_2,       GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO5  - "SHUNT_2"       output   (pin 20  of EG915U)
    {QUEC_PIN_DNAME_GPIO_8,     0, 0, GPIO_LABEL_OUTPUT__PULL_1,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO8  - "PULL_1"        output   (pin 104 of EG915U)
    {QUEC_PIN_DNAME_I2C_M1_SDA, 0, 4, GPIO_LABEL_OUTPUT__PULL_2,        GPIO_OUTPUT, QUEC_PIN_NONE, LVL_LOW      },    // GPIO17 - "PULL_2"        output   (pin 114 of EG915U)
};


/* debug string */
static ascii     DotaDrvGpio_DebugString[DEBUG_STRING_LENTGH + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
       void DotaDrvGpio_Init(void);

/* fast actions on outputs */
static void DotaDrvGpio_SetOutputOut1Low       (void);
static void DotaDrvGpio_SetOutputOut2Low       (void);
static void DotaDrvGpio_SetOutputLedGsmGreenLow(void);
static void DotaDrvGpio_SetOutputChargeLow     (void);
static void DotaDrvGpio_SetOutputVolt1Low      (void);
static void DotaDrvGpio_SetOutputVolt2Low      (void);
static void DotaDrvGpio_SetOutputShunt1Low     (void);
static void DotaDrvGpio_SetOutputShunt2Low     (void);
static void DotaDrvGpio_SetOutputPull1Low      (void);
static void DotaDrvGpio_SetOutputPull2Low      (void);




/*=============================================================================
 * Function   : DotaDrvGpio_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DotaDrvGpio_Init(void)
{
    u8 index;


    /* GPIO init */
    for (index = 0; index < DOTA_DRVGPIO__NUM_GPIOS; index++)
    {
        ql_pin_set_func(DotaDrvGpio_GpioConfig[index].pin_num, DotaDrvGpio_GpioConfig[index].gpio_func);

        ql_gpio_deinit(DotaDrvGpio_GpioConfig[index].gpio_num);
        ql_gpio_init(DotaDrvGpio_GpioConfig[index].gpio_num, DotaDrvGpio_GpioConfig[index].gpio_dir, DotaDrvGpio_GpioConfig[index].gpio_pull, DotaDrvGpio_GpioConfig[index].gpio_lvl);
    }


    /* set digital outputs to init values */
    DotaDrvGpio_SetOutputOut1Low();
    DotaDrvGpio_SetOutputOut2Low();
    DotaDrvGpio_SetOutputLedGsmGreenLow();
    DotaDrvGpio_SetOutputChargeLow();
    DotaDrvGpio_SetOutputVolt1Low();
    DotaDrvGpio_SetOutputVolt2Low();
    DotaDrvGpio_SetOutputShunt1Low();
    DotaDrvGpio_SetOutputShunt2Low();
    DotaDrvGpio_SetOutputPull1Low();
    DotaDrvGpio_SetOutputPull2Low();
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputOut1Low
 *
 * Description: set the "OUT_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputOut1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__OUT_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputOut2Low
 *
 * Description: set the "OUT_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputOut2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__OUT_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputLedGsmGreenLow
 *
 * Description: set the "LED_GSM_GREEN" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputLedGsmGreenLow(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__LED_GSM_GREEN;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputChargeLow
 *
 * Description: set the "CHARGE" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputChargeLow(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__CHARGE;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputVolt1Low
 *
 * Description: set the "VOLT_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputVolt1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__VOLT_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputVolt2Low
 *
 * Description: set the "VOLT_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputVolt2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__VOLT_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputShunt1Low
 *
 * Description: set the "SHUNT_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputShunt1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__SHUNT_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputShunt2Low
 *
 * Description: set the "SHUNT_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputShunt2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__SHUNT_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputPull1Low
 *
 * Description: set the "PULL_1" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputPull1Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__PULL_1;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DotaDrvGpio_SetOutputPull2Low
 *
 * Description: set the "PULL_2" output low
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaDrvGpio_SetOutputPull2Low(void)
{
    ql_GpioNum      handle_output;
    ql_errcode_gpio err;


    handle_output = GPIO_LABEL_OUTPUT__PULL_2;

    err = ql_gpio_set_level(handle_output, LVL_LOW);
    if (err != QL_GPIO_SUCCESS)
    {
        snprintf(DotaDrvGpio_DebugString, sizeof(DotaDrvGpio_DebugString), "QL FUNCTION - ql_gpio_set_level - ERROR: %d %lx", err, (u32)handle_output);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, DotaDrvGpio_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRVGPIO, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_gpio_set_level - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
