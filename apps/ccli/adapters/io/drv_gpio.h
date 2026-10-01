/*=============================================================================
 * File       :  drv_gpio.h
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DI/DO driver - indexed digital inputs/outputs with polarity
 *               and boot safe-state. SK0153 drvgpio.c style, ported to a
 *               portable C HAL (cci_gpio_ll_*) for Pi / OpenWrt.
 *
 * Regulation notes:
 *   - Outputs default to logical OFF (de-energized) at init and on safe state
 *     (IEC 62443-4-2 CR 3.6 direction: deterministic outputs on fault).
 *   - Channel electrical polarity comes from config (config/lab_pi.yaml).
 *=============================================================================*/
#ifndef CCI_DRV_GPIO_H
#define CCI_DRV_GPIO_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Digital input channel indices (SK0153: DRVGPIO__IN__*)
 *---------------------------------------------------------------------------*/
enum {
    DRVGPIO__IN__PERMISSIVE = 0, /* external enable / permissive contact */
    DRVGPIO__NUM_INPUTS
};

/*-----------------------------------------------------------------------------
 * Digital output channel indices (SK0153: DRVGPIO__OUT__*)
 *---------------------------------------------------------------------------*/
enum {
    DRVGPIO__OUT__CURTAIL = 0, /* PF2 curtailment command output */
    DRVGPIO__NUM_OUTPUTS
};

/*-----------------------------------------------------------------------------
 * Per-channel electrical configuration (SK0153: DrvGpio_GpioConfig[])
 *---------------------------------------------------------------------------*/
typedef struct {
    int  line;        /* platform GPIO line: BCM pin (Pi) / gpiochip line (TG-524) */
    bool active_high; /* true: logical ON -> line HIGH; false: logical ON -> LOW */
} DrvGpioChannelCfg;

typedef struct {
    DrvGpioChannelCfg inputs[DRVGPIO__NUM_INPUTS];
    DrvGpioChannelCfg outputs[DRVGPIO__NUM_OUTPUTS];
} DrvGpioConfig;

/* Default lab config, mirrors config/lab_pi.yaml (do_curtail=5 active-high,
 * di_permissive=27 active-low). A YAML loader can replace this later. */
DrvGpioConfig DrvGpio_DefaultLabConfig(void);

/* Initialize the low-level HAL and all channels. All outputs are forced to
 * logical OFF (safe). Returns true on success. */
bool DrvGpio_Init(const DrvGpioConfig *cfg);

/* Read a logical input state (polarity applied): true = logical ON. */
bool DrvGpio_ReadInput(int input_id, bool *state);

/* Set a logical output state (polarity applied): true = logical ON. */
bool DrvGpio_SetOutput(int output_id, bool state);

/* Force every output to logical OFF (de-energized). 62443-4-2 CR 3.6. */
void DrvGpio_SafeState(void);

#ifdef __cplusplus
}
#endif

#endif /* CCI_DRV_GPIO_H */
