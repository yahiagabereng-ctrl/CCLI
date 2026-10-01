/*=============================================================================
 * File       :  DOTA_DRVGPIO.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - GPIOs management
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DOTA_DRVGPIO_H__


#define __DOTA_DRVGPIO_H__




/*=============================================================================
 * DEFINES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Number of GPIOs
 *-----------------------------------------------------------------------------*/
/* number of digital inputs and digital outputs */
#define DOTA_DRVGPIO__NUM_INPUTS               5       // number of digital inputs
#define DOTA_DRVGPIO__NUM_OUTPUTS              10      // number of digital outputs

/* number of GPIOs to be used (inputs and outpus) */
#define DOTA_DRVGPIO__NUM_GPIOS                (DOTA_DRVGPIO__NUM_INPUTS + DOTA_DRVGPIO__NUM_OUTPUTS)


/*-----------------------------------------------------------------------------
 * digital inputs and outputs
 *-----------------------------------------------------------------------------*/
/* digital inputs */
#define DOTA_DRVGPIO__IN__PWR_OK                0       // GPIO3  - "PWR_OK"        input
#define DOTA_DRVGPIO__IN__KEY_OUT_1             1       // GPIO9  - "KEY_OUT_1"     input
#define DOTA_DRVGPIO__IN__KEY_OUT_2             2       // GPIO10 - "KEY_OUT_2"     input
#define DOTA_DRVGPIO__IN__IN1                   3       // GPIO0  - "IN1"           input
#define DOTA_DRVGPIO__IN__IN2                   4       // GPIO1  - "IN2"           input

/* digital outputs */
#define DOTA_DRVGPIO__OUT__OUT_1                0       // GPIO14 - "OUT_1"         output
#define DOTA_DRVGPIO__OUT__OUT_2                1       // GPIO15 - "OUT_2"         output
#define DOTA_DRVGPIO__OUT__LED_GSM_GREEN        2       // GPIO11 - "LED_GSM_GREEN" output
#define DOTA_DRVGPIO__OUT__CHARGE               3       // GPIO2  - "CHARGE"        output
#define DOTA_DRVGPIO__OUT__VOLT_1               4       // GPIO20 - "VOLT_1"        output
#define DOTA_DRVGPIO__OUT__VOLT_2               5       // GPIO19 - "VOLT_2"        output
#define DOTA_DRVGPIO__OUT__SHUNT_1              6       // GPIO21 - "SHUNT_1"       output
#define DOTA_DRVGPIO__OUT__SHUNT_2              7       // GPIO5  - "SHUNT_2"       output
#define DOTA_DRVGPIO__OUT__PULL_1               8       // GPIO8  - "PULL_1"        output
#define DOTA_DRVGPIO__OUT__PULL_2               9       // GPIO17 - "PULL_2"        output


/*-----------------------------------------------------------------------------
 * digital input states
 *-----------------------------------------------------------------------------*/
/* GPIO19 - "PWR_OK"   input */
#define DOTA_DRVGPIO__IN__PWR_OK__LOW           0       // GPIO3  - "PWR_OK"         input  low
#define DOTA_DRVGPIO__IN__PWR_OK__HIGH          1       // GPIO3  - "PWR_OK"         input  high

/* GPIO17 - "KEY_OUT1" input */
#define DOTA_DRVGPIO__IN__KEY_OUT1__LOW         0       // GPIO9  - "KEY_OUT1"       input  low
#define DOTA_DRVGPIO__IN__KEY_OUT1__HIGH        1       // GPIO9  - "KEY_OUT1"       input  high

/* GPIO16 - "KEY_OUT2" input */
#define DOTA_DRVGPIO__IN__KEY_OUT2__LOW         0       // GPIO10 - "KEY_OUT2"       input  low
#define DOTA_DRVGPIO__IN__KEY_OUT2__HIGH        1       // GPIO10 - "KEY_OUT2"       input  high

/* GPIO03 - "IN1"      input */
#define DOTA_DRVGPIO__IN__IN1__LOW              0       // GPIO0  - "IN1"            input  low
#define DOTA_DRVGPIO__IN__IN1__HIGH             1       // GPIO0  - "IN1"            input  high

/* GPIO25 - "IN2"      input */
#define DOTA_DRVGPIO__IN__IN2__LOW              0       // GPIO1  - "IN2"            input  low
#define DOTA_DRVGPIO__IN__IN2__HIGH             1       // GPIO1  - "IN2"            input  high


/*-----------------------------------------------------------------------------
 * digital outputs states
 *-----------------------------------------------------------------------------*/
/* GPIO21 - "OUT_1"         output */
#define DOTA_DRVGPIO__OUT__OUT_1__LOW           FALSE   // GPIO14  - "OUT_1"         output low
#define DOTA_DRVGPIO__OUT__OUT_1__HIGH          TRUE    // GPIO14  - "OUT_1"         output high

/* GPIO23 - "OUT_2"         output */
#define DOTA_DRVGPIO__OUT__OUT_2__LOW           FALSE   // GPIO15  - "OUT_2"         output low
#define DOTA_DRVGPIO__OUT__OUT_2__HIGH          TRUE    // GPIO15  - "OUT_2"         output high

/* GPIO15 - "LED_GSM_GREEN" output */
#define DOTA_DRVGPIO__OUT__LED_GSM_GREEN__LOW   FALSE   // GPIO11  - "LED_GSM_GREEN" output low
#define DOTA_DRVGPIO__OUT__LED_GSM_GREEN__HIGH  TRUE    // GPIO11  - "LED_GSM_GREEN" output high

/* GPIO24 - "CHARGE"        output */
#define DOTA_DRVGPIO__OUT__CHARGE__LOW          FALSE   // GPIO2   - "CHARGE"        output low
#define DOTA_DRVGPIO__OUT__CHARGE__HIGH         TRUE    // GPIO2   - "CHARGE"        output high

/* GPIO20 - "VOLT_1"        output */
#define DOTA_DRVGPIO__OUT__VOLT_1__LOW          FALSE   // GPIO20 - "VOLT_1"        output low
#define DOTA_DRVGPIO__OUT__VOLT_1__HIGH         TRUE    // GPIO20 - "VOLT_1"        output high

/* GPIO19 - "VOLT_2"        output */
#define DOTA_DRVGPIO__OUT__VOLT_2__LOW          FALSE   // GPIO19 - "VOLT_2"        output low
#define DOTA_DRVGPIO__OUT__VOLT_2__HIGH         TRUE    // GPIO19 - "VOLT_2"        output high

/* GPIO21 - "SHUNT_1"       output */
#define DOTA_DRVGPIO__OUT__SHUNT_1__LOW         FALSE   // GPIO21 - "SHUNT_1"       output low
#define DOTA_DRVGPIO__OUT__SHUNT_1__HIGH        TRUE    // GPIO21 - "SHUNT_1"       output high

/* GPIO5  - "SHUNT_2"       output */
#define DOTA_DRVGPIO__OUT__SHUNT_2__LOW         FALSE   // GPIO5  - "SHUNT_2"       output low
#define DOTA_DRVGPIO__OUT__SHUNT_2__HIGH        TRUE    // GPIO5  - "SHUNT_2"       output high

/* GPIO8  - "PULL_1"        output */
#define DOTA_DRVGPIO__OUT__PULL_1__LOW          FALSE   // GPIO8  - "PULL_1"        output low
#define DOTA_DRVGPIO__OUT__PULL_1__HIGH         TRUE    // GPIO8  - "PULL_1"        output high

/* GPIO17 - "PULL_2"        output */
#define DOTA_DRVGPIO__OUT__PULL_2__LOW          FALSE   // GPIO17 - "PULL_2"        output low
#define DOTA_DRVGPIO__OUT__PULL_2__HIGH         TRUE    // GPIO17 - "PULL_2"        output high




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
void DotaDrvGpio_Init(void);




#endif
