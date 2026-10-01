/*=============================================================================
 * File       :  DRVGPIO.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - GPIOs management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DRVGPIO_H__


#define __DRVGPIO_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Number of GPIOs
 *-----------------------------------------------------------------------------*/
/* number of digital inputs and digital outputs */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
#define DRV_GPIO__NUM_INPUTS               6       // number of digital inputs
#define DRV_GPIO__NUM_OUTPUTS              10      // number of digital outputs
#endif
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
#define DRV_GPIO__NUM_INPUTS               5       // number of digital inputs
#define DRV_GPIO__NUM_OUTPUTS              10      // number of digital outputs
#endif

/* number of GPIOs to be used (inputs and outputs) */
#define DRV_GPIO__NUM_GPIOS                (DRV_GPIO__NUM_INPUTS + DRV_GPIO__NUM_OUTPUTS)


/*-----------------------------------------------------------------------------
 * digital inputs and outputs
 *-----------------------------------------------------------------------------*/
/* digital inputs */
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
#define DRVGPIO__IN__RST_IO                0       // GPIO12 - "RST_IO"        input
#define DRVGPIO__IN__PWR_OK                1       // GPIO3  - "PWR_OK"        input
#define DRVGPIO__IN__KEY_OUT_1             2       // GPIO9  - "KEY_OUT_1"     input
#define DRVGPIO__IN__KEY_OUT_2             3       // GPIO10 - "KEY_OUT_2"     input
#define DRVGPIO__IN__IN1                   4       // GPIO0  - "IN1"           input
#define DRVGPIO__IN__IN2                   5       // GPIO1  - "IN2"           input
#define DRVGPIO__IN__IN3                   6       // ADC0   - "IN3"           input
#define DRVGPIO__IN__IN4                   7       // ADC1   - "IN4"           input
#endif
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
#define DRVGPIO__IN__PWR_OK                0       // GPIO3  - "PWR_OK"        input
#define DRVGPIO__IN__KEY_OUT_1             1       // GPIO9  - "KEY_OUT_1"     input
#define DRVGPIO__IN__KEY_OUT_2             2       // GPIO10 - "KEY_OUT_2"     input
#define DRVGPIO__IN__IN1                   3       // GPIO0  - "IN1"           input
#define DRVGPIO__IN__IN2                   4       // GPIO1  - "IN2"           input
#define DRVGPIO__IN__IN3                   5       // ADC0   - "IN3"           input
#define DRVGPIO__IN__IN4                   6       // ADC1   - "IN4"           input
#endif

/* digital outputs */
#define DRVGPIO__OUT__OUT_1                0       // GPIO14 - "OUT_1"         output
#define DRVGPIO__OUT__OUT_2                1       // GPIO15 - "OUT_2"         output
#define DRVGPIO__OUT__LED_GSM_GREEN        2       // GPIO11 - "LED_GSM_GREEN" output
#define DRVGPIO__OUT__CHARGE               3       // GPIO2  - "CHARGE"        output
#define DRVGPIO__OUT__VOLT_1               4       // GPIO20 - "VOLT_1"        output
#define DRVGPIO__OUT__VOLT_2               5       // GPIO19 - "VOLT_2"        output
#define DRVGPIO__OUT__SHUNT_1              6       // GPIO21 - "SHUNT_1"       output
#define DRVGPIO__OUT__SHUNT_2              7       // GPIO5  - "SHUNT_2"       output
#define DRVGPIO__OUT__PULL_1               8       // GPIO8  - "PULL_1"        output
#define DRVGPIO__OUT__PULL_2               9       // GPIO17 - "PULL_2"        output


/*-----------------------------------------------------------------------------
 * digital input states
 *-----------------------------------------------------------------------------*/
/* GPIO12 - "RST_IO"   input */
#define DRVGPIO__IN__RST_IO__LOW           0       // GPIO12 - "RST_IO"         input  low
#define DRVGPIO__IN__RST_IO__HIGH          1       // GPIO12 - "RST_IO"         input  high

/* GPIO3  - "PWR_OK"   input */
#define DRVGPIO__IN__PWR_OK__LOW           0       // GPIO3  - "PWR_OK"         input  low
#define DRVGPIO__IN__PWR_OK__HIGH          1       // GPIO3  - "PWR_OK"         input  high

/* GPIO9  - "KEY_OUT1" input */
#define DRVGPIO__IN__KEY_OUT1__LOW         0       // GPIO9  - "KEY_OUT1"       input  low
#define DRVGPIO__IN__KEY_OUT1__HIGH        1       // GPIO9  - "KEY_OUT1"       input  high

/* GPIO10 - "KEY_OUT2" input */
#define DRVGPIO__IN__KEY_OUT2__LOW         0       // GPIO10 - "KEY_OUT2"       input  low
#define DRVGPIO__IN__KEY_OUT2__HIGH        1       // GPIO10 - "KEY_OUT2"       input  high

/* GPIO0  - "IN1"      input */
#define DRVGPIO__IN__IN1__LOW              0       // GPIO0  - "IN1"            input  low
#define DRVGPIO__IN__IN1__HIGH             1       // GPIO0  - "IN1"            input  high

/* GPIO1  - "IN2"      input */
#define DRVGPIO__IN__IN2__LOW              0       // GPIO1  - "IN2"            input  low
#define DRVGPIO__IN__IN2__HIGH             1       // GPIO1  - "IN2"            input  high


/*-----------------------------------------------------------------------------
 * digital outputs states
 *-----------------------------------------------------------------------------*/
/* GPIO14 - "OUT_1"         output */
#define DRVGPIO__OUT__OUT_1__LOW           FALSE   // GPIO14 - "OUT_1"         output low
#define DRVGPIO__OUT__OUT_1__HIGH          TRUE    // GPIO14 - "OUT_1"         output high

/* GPIO15 - "OUT_2"         output */
#define DRVGPIO__OUT__OUT_2__LOW           FALSE   // GPIO15 - "OUT_2"         output low
#define DRVGPIO__OUT__OUT_2__HIGH          TRUE    // GPIO15 - "OUT_2"         output high

/* GPIO11 - "LED_GSM_GREEN" output */
#define DRVGPIO__OUT__LED_GSM_GREEN__LOW   FALSE   // GPIO11 - "LED_GSM_GREEN" output low
#define DRVGPIO__OUT__LED_GSM_GREEN__HIGH  TRUE    // GPIO11 - "LED_GSM_GREEN" output high

/* GPIO2  - "CHARGE"        output */
#define DRVGPIO__OUT__CHARGE__LOW          FALSE   // GPIO2  - "CHARGE"        output low
#define DRVGPIO__OUT__CHARGE__HIGH         TRUE    // GPIO2  - "CHARGE"        output high

/* GPIO20 - "VOLT_1"        output */
#define DRVGPIO__OUT__VOLT_1__LOW          FALSE   // GPIO20 - "VOLT_1"        output low
#define DRVGPIO__OUT__VOLT_1__HIGH         TRUE    // GPIO20 - "VOLT_1"        output high

/* GPIO19 - "VOLT_2"        output */
#define DRVGPIO__OUT__VOLT_2__LOW          FALSE   // GPIO19 - "VOLT_2"        output low
#define DRVGPIO__OUT__VOLT_2__HIGH         TRUE    // GPIO19 - "VOLT_2"        output high

/* GPIO21 - "SHUNT_1"       output */
#define DRVGPIO__OUT__SHUNT_1__LOW         FALSE   // GPIO21 - "SHUNT_1"       output low
#define DRVGPIO__OUT__SHUNT_1__HIGH        TRUE    // GPIO21 - "SHUNT_1"       output high

/* GPIO5  - "SHUNT_2"       output */
#define DRVGPIO__OUT__SHUNT_2__LOW         FALSE   // GPIO5  - "SHUNT_2"       output low
#define DRVGPIO__OUT__SHUNT_2__HIGH        TRUE    // GPIO5  - "SHUNT_2"       output high

/* GPIO8  - "PULL_1"        output */
#define DRVGPIO__OUT__PULL_1__LOW          FALSE   // GPIO8  - "PULL_1"        output low
#define DRVGPIO__OUT__PULL_1__HIGH         TRUE    // GPIO8  - "PULL_1"        output high

/* GPIO17 - "PULL_2"        output */
#define DRVGPIO__OUT__PULL_2__LOW          FALSE   // GPIO17 - "PULL_2"        output low
#define DRVGPIO__OUT__PULL_2__HIGH         TRUE    // GPIO17 - "PULL_2"        output high




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void   DrvGpio_TaskDrvGpio(void *argument);

/* status variables */
void   DrvGpio_UpdateVarCrc(void);
bool   DrvGpio_VerifyVarCrc(void);

/*-----------------------------------------------------------------------------
 * Digital inputs
 *-----------------------------------------------------------------------------*/
/* read inputs */
bool   DrvGpio_ReadInput(u8 input_id);

/* read inputs descriptions */
ascii *DrvGpio_ReadInputDescription(u8 input_id);

/* registered signals of digital input event */
void   DrvGpio_RegisterSignalDigitalInputEvent(u8 input_id, void (*ptr_function)(u8, bool));

/*-----------------------------------------------------------------------------
 * Digital outputs
 *-----------------------------------------------------------------------------*/
/* slow actions on outputs */
void   DrvGpio_SetOutput(u8 output_id, bool state, u32 time_impulse);

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




#endif
