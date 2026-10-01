/*=============================================================================
 * File       :  drv_gpio.c
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DI/DO driver implementation (SK0153 drvgpio.c style).
 *=============================================================================*/
#include "drv_gpio.h"

#include "cci/hal/gpio_ll.h"

/* Active copy of the channel configuration (SK0153: DrvGpio_GpioConfig[]). */
static DrvGpioConfig s_cfg;
static bool          s_initialized = false;

/* Map a logical output state to a raw electrical level given polarity. */
static int output_level_for(const DrvGpioChannelCfg *ch, bool logical_on) {
    if (ch->active_high) {
        return logical_on ? 1 : 0;
    }
    return logical_on ? 0 : 1;
}

/* Map a raw input level to a logical state given polarity. */
static bool input_logical_for(const DrvGpioChannelCfg *ch, int raw_level) {
    if (ch->active_high) {
        return raw_level != 0;
    }
    return raw_level == 0;
}

DrvGpioConfig DrvGpio_DefaultLabConfig(void) {
    DrvGpioConfig cfg;

    /* config/lab_pi.yaml -> io.di_permissive: gpio 27, active_low: true */
    cfg.inputs[DRVGPIO__IN__PERMISSIVE].line = 27;
    cfg.inputs[DRVGPIO__IN__PERMISSIVE].active_high = false;

    /* config/lab_pi.yaml -> io.do_curtail: gpio 5, active_low (typical opto relay board) */
    cfg.outputs[DRVGPIO__OUT__CURTAIL].line = 5;
    cfg.outputs[DRVGPIO__OUT__CURTAIL].active_high = false;

    return cfg;
}

bool DrvGpio_Init(const DrvGpioConfig *cfg) {
    if (cfg == 0) {
        return false;
    }
    s_cfg = *cfg;

    if (!cci_gpio_ll_init()) {
        return false;
    }

    /* Configure inputs. */
    for (int i = 0; i < DRVGPIO__NUM_INPUTS; ++i) {
        if (!cci_gpio_ll_config_input(s_cfg.inputs[i].line)) {
            return false;
        }
    }

    /* Configure outputs; boot to logical OFF (safe / de-energized). */
    for (int o = 0; o < DRVGPIO__NUM_OUTPUTS; ++o) {
        const int safe_level = output_level_for(&s_cfg.outputs[o], false);
        if (!cci_gpio_ll_config_output(s_cfg.outputs[o].line, safe_level)) {
            return false;
        }
    }

    s_initialized = true;
    return true;
}

bool DrvGpio_ReadInput(int input_id, bool *state) {
    if (!s_initialized || state == 0) {
        return false;
    }
    if (input_id < 0 || input_id >= DRVGPIO__NUM_INPUTS) {
        return false;
    }
    int raw = 0;
    if (!cci_gpio_ll_read(s_cfg.inputs[input_id].line, &raw)) {
        return false;
    }
    *state = input_logical_for(&s_cfg.inputs[input_id], raw);
    return true;
}

bool DrvGpio_SetOutput(int output_id, bool state) {
    if (!s_initialized) {
        return false;
    }
    if (output_id < 0 || output_id >= DRVGPIO__NUM_OUTPUTS) {
        return false;
    }
    const int level = output_level_for(&s_cfg.outputs[output_id], state);
    return cci_gpio_ll_write(s_cfg.outputs[output_id].line, level);
}

void DrvGpio_SafeState(void) {
    if (!s_initialized) {
        return;
    }
    for (int o = 0; o < DRVGPIO__NUM_OUTPUTS; ++o) {
        const int safe_level = output_level_for(&s_cfg.outputs[o], false);
        (void)cci_gpio_ll_write(s_cfg.outputs[o].line, safe_level);
    }
}
