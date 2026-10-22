/*=============================================================================
 * File       :  OUTPUTS.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - outputs (PF2 curtailment DO, permissive DI, Annex M trip)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "outputs.h"

#include "cci/hal/gpio_ll.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* Annex M trip relay channel (0 = monitor disabled) */
static int Outputs_AnnexMTripGpio = 0;




/*=============================================================================
 * Function   : Outputs_Init
 *
 * Description: init the DI/DO stack; outputs forced to safe state
 * Input      : cfg - channel configuration
 * Output     : TRUE on success
 *=============================================================================*/
bool Outputs_Init(const DrvGpioConfig *cfg)
{
    if (cfg == 0)
    {
        return false;
    }
    if (!DrvGpio_Init(cfg))
    {
        fprintf(stderr,
                "svc_io: init failed (do_curtail=GPIO%d active_%s, di_permissive=GPIO%d "
                "active_%s)\n",
                cfg->outputs[DRVGPIO__OUT__CURTAIL].line,
                cfg->outputs[DRVGPIO__OUT__CURTAIL].active_high ? "high" : "low",
                cfg->inputs[DRVGPIO__IN__PERMISSIVE].line,
                cfg->inputs[DRVGPIO__IN__PERMISSIVE].active_high ? "high" : "low");
        return false;
    }

    /* explicit safe state on start-up (belt-and-suspenders vs DrvGpio_Init) */
    DrvGpio_SafeState();
    fprintf(stderr, "svc_io: ready do_curtail=GPIO%d(active_%s) di_permissive=GPIO%d(active_%s)\n",
            cfg->outputs[DRVGPIO__OUT__CURTAIL].line,
            cfg->outputs[DRVGPIO__OUT__CURTAIL].active_high ? "high" : "low",
            cfg->inputs[DRVGPIO__IN__PERMISSIVE].line,
            cfg->inputs[DRVGPIO__IN__PERMISSIVE].active_high ? "high" : "low");
    return true;
}




/*=============================================================================
 * Function   : Outputs_InitDefault
 *
 * Description: init the DI/DO stack with the default lab configuration
 * Input      : -
 * Output     : TRUE on success
 *=============================================================================*/
bool Outputs_InitDefault(void)
{
    const DrvGpioConfig cfg = DrvGpio_DefaultLabConfig();
    return Outputs_Init(&cfg);
}




/*=============================================================================
 * Function   : Outputs_ApplyCurtailment / Outputs_ReadPermissive
 *
 * Description: drive the curtailment DO / read the permissive DI
 * Input      : active - curtailment ON
 * Output     : ok     - permissive state; TRUE on success
 *=============================================================================*/
bool Outputs_ApplyCurtailment(bool active)
{
    return DrvGpio_SetOutput(DRVGPIO__OUT__CURTAIL, active);
}

bool Outputs_ReadPermissive(bool *ok)
{
    return DrvGpio_ReadInput(DRVGPIO__IN__PERMISSIVE, ok);
}




/*=============================================================================
 * Function   : Outputs_ConfigureAnnexMTripMonitor
 *
 * Description: P6-03 monitor of the trip relay driven on a ubus channel
 * Input      : gpio_channel - channel; enabled - monitor enabled
 * Output     : TRUE
 *=============================================================================*/
bool Outputs_ConfigureAnnexMTripMonitor(int gpio_channel, bool enabled)
{
    if (!enabled || gpio_channel < 1)
    {
        Outputs_AnnexMTripGpio = 0;
        return true;
    }
    Outputs_AnnexMTripGpio = gpio_channel;
    fprintf(stderr, "svc_io: annex_m_trip_monitor ch%d (O.11 inhibit DIO1) [P6-03]\n",
            Outputs_AnnexMTripGpio);
    return true;
}




/*=============================================================================
 * Function   : Outputs_ReadAnnexMTripActive
 *
 * Description: TRUE when the Annex M trip relay is energized
 * Input      : -
 * Output     : active - trip state; TRUE on success
 *=============================================================================*/
bool Outputs_ReadAnnexMTripActive(bool *active)
{
    if (active == 0)
    {
        return false;
    }
    if (Outputs_AnnexMTripGpio <= 0)
    {
        *active = false;
        return true;
    }
    int energized = 0;
    if (!cci_gpio_ll_read_relay(Outputs_AnnexMTripGpio, &energized))
    {
        return false;
    }
    *active = energized != 0;
    return true;
}




/*=============================================================================
 * Function   : Outputs_GateCurtailment
 *
 * Description: O.11 interlock - no conflicting PF2 actuation
 * Input      : commanded, permissive_ok, annex_m_trip_active
 * Output     : curtailment DO state
 *=============================================================================*/
bool Outputs_GateCurtailment(bool commanded, bool permissive_ok, bool annex_m_trip_active)
{
    return commanded && permissive_ok && !annex_m_trip_active;
}




/*=============================================================================
 * Function   : Outputs_CurtailDetail
 *
 * Description: O.14 "do" event detail for a DO transition
 * Input      : do_on, commanded, annex_m_trip_active
 * Output     : detail string
 *=============================================================================*/
const char *Outputs_CurtailDetail(bool do_on, bool commanded, bool annex_m_trip_active)
{
    if (do_on)
    {
        return "curtail_on";
    }
    if (annex_m_trip_active)
    {
        return "curtail_blocked_annex_m";
    }
    return commanded ? "curtail_blocked_permissive" : "curtail_off";
}




/*=============================================================================
 * Function   : Outputs_SafeState / Outputs_Shutdown
 *
 * Description: force outputs safe / release GPIO lines before exit
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Outputs_SafeState(void)
{
    DrvGpio_SafeState();
}

void Outputs_Shutdown(void)
{
    DrvGpio_SafeState();
    cci_gpio_ll_shutdown();
}




/*=============================================================================
 * Function   : Outputs_LogStatus
 *
 * Description: one status line on stderr
 * Input      : curtail_active, permissive_ok, annex_m_trip_active
 * Output     : -
 *=============================================================================*/
void Outputs_LogStatus(bool curtail_active, bool permissive_ok, bool annex_m_trip_active)
{
    if (Outputs_AnnexMTripGpio > 0)
    {
        fprintf(stderr, "io: curtail_do=%s permissive=%s annex_m_trip=%s\n",
                curtail_active ? "ON" : "OFF", permissive_ok ? "OK" : "BLOCKED",
                annex_m_trip_active ? "ACTIVE" : "off");
    }
    else
    {
        fprintf(stderr, "io: curtail_do=%s permissive=%s\n", curtail_active ? "ON" : "OFF",
                permissive_ok ? "OK" : "BLOCKED");
    }
}




/*=============================================================================
 * Function   : Outputs_UpdateVarCrc / Outputs_VerifyVarCrc
 *
 * Description: status variables CRC (retained state across restart)
 *              TODO(ccli-product-sw-freeze): back with program_flash
 * Input      : -
 * Output     : VerifyVarCrc - TRUE when retained state is valid
 *=============================================================================*/
void Outputs_UpdateVarCrc(void)
{
}

bool Outputs_VerifyVarCrc(void)
{
    return true;
}
