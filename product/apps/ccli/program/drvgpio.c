/*=============================================================================
 * File       :  DRVGPIO.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DRIVER - GPIO (indexed DI/DO with polarity and boot safe-state)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "drvgpio.h"

#include "cci/hal/gpio_ll.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* active copy of the channel configuration */
static DrvGpioConfig DrvGpio_GpioConfig;

/* init indication */
static bool          DrvGpio_Initialized = false;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
static int  DrvGpio_OutputLevel (const DrvGpioChannelCfg *ch, bool logical_on);
static bool DrvGpio_InputLogical(const DrvGpioChannelCfg *ch, int raw_level);




/*=============================================================================
 * Function   : DrvGpio_DefaultLabConfig
 *
 * Description: default lab configuration
 * Input      : -
 * Output     : default configuration
 *=============================================================================*/
DrvGpioConfig DrvGpio_DefaultLabConfig(void)
{
    DrvGpioConfig cfg;

    /* io.di_permissive: gpio 27, active_low */
    cfg.inputs[DRVGPIO__IN__PERMISSIVE].line        = 27;
    cfg.inputs[DRVGPIO__IN__PERMISSIVE].active_high = false;

    /* io.do_curtail: gpio 5, active_low (typical opto relay board) */
    cfg.outputs[DRVGPIO__OUT__CURTAIL].line         = 5;
    cfg.outputs[DRVGPIO__OUT__CURTAIL].active_high  = false;

    return cfg;
}




/*=============================================================================
 * Function   : DrvGpio_Init
 *
 * Description: init HAL and channels; all outputs forced to logical OFF
 * Input      : cfg - channel configuration
 * Output     : TRUE on success
 *=============================================================================*/
bool DrvGpio_Init(const DrvGpioConfig *cfg)
{
    if (cfg == 0)
    {
        return false;
    }
    DrvGpio_GpioConfig = *cfg;

    if (!cci_gpio_ll_init())
    {
        return false;
    }

    /* configure inputs */
    for (int i = 0; i < DRVGPIO__NUM_INPUTS; ++i)
    {
        if (!cci_gpio_ll_config_input(DrvGpio_GpioConfig.inputs[i].line))
        {
            return false;
        }
    }

    /* configure outputs; boot to logical OFF (safe / de-energized) */
    for (int o = 0; o < DRVGPIO__NUM_OUTPUTS; ++o)
    {
        const int safe_level = DrvGpio_OutputLevel(&DrvGpio_GpioConfig.outputs[o], false);
        if (!cci_gpio_ll_config_output(DrvGpio_GpioConfig.outputs[o].line, safe_level))
        {
            return false;
        }
    }

    DrvGpio_Initialized = true;
    return true;
}




/*=============================================================================
 * Function   : DrvGpio_ReadInput
 *
 * Description: read a logical input state (polarity applied)
 * Input      : input_id - DRVGPIO__IN__*
 * Output     : state    - logical state; TRUE on success
 *=============================================================================*/
bool DrvGpio_ReadInput(int input_id, bool *state)
{
    if (!DrvGpio_Initialized || state == 0)
    {
        return false;
    }
    if (input_id < 0 || input_id >= DRVGPIO__NUM_INPUTS)
    {
        return false;
    }
    int raw = 0;
    if (!cci_gpio_ll_read(DrvGpio_GpioConfig.inputs[input_id].line, &raw))
    {
        return false;
    }
    *state = DrvGpio_InputLogical(&DrvGpio_GpioConfig.inputs[input_id], raw);
    return true;
}




/*=============================================================================
 * Function   : DrvGpio_SetOutput
 *
 * Description: set a logical output state (polarity applied)
 * Input      : output_id - DRVGPIO__OUT__*
 *              state     - logical state
 * Output     : TRUE on success
 *=============================================================================*/
bool DrvGpio_SetOutput(int output_id, bool state)
{
    if (!DrvGpio_Initialized)
    {
        return false;
    }
    if (output_id < 0 || output_id >= DRVGPIO__NUM_OUTPUTS)
    {
        return false;
    }
    const int level = DrvGpio_OutputLevel(&DrvGpio_GpioConfig.outputs[output_id], state);
    return cci_gpio_ll_write(DrvGpio_GpioConfig.outputs[output_id].line, level);
}




/*=============================================================================
 * Function   : DrvGpio_SafeState
 *
 * Description: force every output to logical OFF (de-energized)
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvGpio_SafeState(void)
{
    if (!DrvGpio_Initialized)
    {
        return;
    }
    for (int o = 0; o < DRVGPIO__NUM_OUTPUTS; ++o)
    {
        const int safe_level = DrvGpio_OutputLevel(&DrvGpio_GpioConfig.outputs[o], false);
        (void)cci_gpio_ll_write(DrvGpio_GpioConfig.outputs[o].line, safe_level);
    }
}




/*=============================================================================
 * Function   : DrvGpio_UpdateVarCrc / DrvGpio_VerifyVarCrc
 *
 * Description: status variables CRC (retained state across restart)
 *              TODO(ccli-product-sw-freeze): back with program_flash
 * Input      : -
 * Output     : VerifyVarCrc - TRUE when retained state is valid
 *=============================================================================*/
void DrvGpio_UpdateVarCrc(void)
{
}

bool DrvGpio_VerifyVarCrc(void)
{
    return true;
}




/*=============================================================================
 * Function   : DrvGpio_OutputLevel
 *
 * Description: map a logical output state to a raw level given polarity
 * Input      : ch         - channel configuration
 *              logical_on - logical state
 * Output     : raw level (0/1)
 *=============================================================================*/
static int DrvGpio_OutputLevel(const DrvGpioChannelCfg *ch, bool logical_on)
{
    if (ch->active_high)
    {
        return logical_on ? 1 : 0;
    }
    return logical_on ? 0 : 1;
}




/*=============================================================================
 * Function   : DrvGpio_InputLogical
 *
 * Description: map a raw input level to a logical state given polarity
 * Input      : ch        - channel configuration
 *              raw_level - raw level
 * Output     : logical state
 *=============================================================================*/
static bool DrvGpio_InputLogical(const DrvGpioChannelCfg *ch, int raw_level)
{
    if (ch->active_high)
    {
        return raw_level != 0;
    }
    return raw_level == 0;
}
