/*=============================================================================
 * File       :  DRVGPIO.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DRIVER - GPIO (indexed DI/DO with polarity and boot safe-state)
 *
 * Regulation notes:
 *   - Outputs default to logical OFF (de-energized) at init and on safe state
 *     (IEC 62443-4-2 CR 3.6 direction: deterministic outputs on fault).
 *   - Channel electrical polarity comes from the runtime yaml (io.*).
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __DRVGPIO_H__


#define __DRVGPIO_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include <stdbool.h>




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* digital input channel indices */
enum
{
    DRVGPIO__IN__PERMISSIVE = 0,      /* external enable / permissive contact */
    DRVGPIO__NUM_INPUTS
};

/* digital output channel indices */
enum
{
    DRVGPIO__OUT__CURTAIL = 0,        /* PF2 curtailment command output */
    DRVGPIO__NUM_OUTPUTS
};




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* per-channel electrical configuration */
typedef struct
{
    int  line;                        /* platform channel (ubus dido_v2 index on TG544) */
    bool active_high;                 /* true: logical ON -> HIGH; false: logical ON -> LOW */
} DrvGpioChannelCfg;

/* configuration - "GPIO channels" */
typedef struct
{
    DrvGpioChannelCfg inputs [DRVGPIO__NUM_INPUTS ];
    DrvGpioChannelCfg outputs[DRVGPIO__NUM_OUTPUTS];
} DrvGpioConfig;




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* default lab configuration (DO 5 active-low, DI 27 active-low) */
DrvGpioConfig DrvGpio_DefaultLabConfig(void);

/* init HAL and channels; all outputs forced to logical OFF (safe) */
bool DrvGpio_Init(const DrvGpioConfig *cfg);

/* read a logical input state (polarity applied): true = logical ON */
bool DrvGpio_ReadInput(int input_id, bool *state);

/* set a logical output state (polarity applied): true = logical ON */
bool DrvGpio_SetOutput(int output_id, bool state);

/* force every output to logical OFF (de-energized) */
void DrvGpio_SafeState(void);

/* status variables */
void DrvGpio_UpdateVarCrc(void);
bool DrvGpio_VerifyVarCrc(void);

#ifdef __cplusplus
}
#endif




#endif
