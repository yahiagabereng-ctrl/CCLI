/*=============================================================================
 * File       :  svc_io.c
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DI/DO service implementation.
 *=============================================================================*/
#include "svc_io.h"

#include "cci/hal/gpio_ll.h"
#include "drv_gpio.h"

#include <stdio.h>

static int s_annex_m_trip_gpio = 0;

bool svc_io_configure_annex_m_trip_monitor(int gpio_channel, bool enabled) {
    if (!enabled || gpio_channel < 1) {
        s_annex_m_trip_gpio = 0;
        return true;
    }
    s_annex_m_trip_gpio = gpio_channel;
    fprintf(stderr, "svc_io: annex_m_trip_monitor ch%d (O.11 inhibit DIO1) [P6-03]\n",
            s_annex_m_trip_gpio);
    return true;
}

bool svc_io_read_annex_m_trip_active(bool *active) {
    if (active == NULL) {
        return false;
    }
    if (s_annex_m_trip_gpio <= 0) {
        *active = false;
        return true;
    }
    int energized = 0;
    if (!cci_gpio_ll_read_relay(s_annex_m_trip_gpio, &energized)) {
        return false;
    }
    *active = energized != 0;
    return true;
}

bool svc_io_init_cfg(const DrvGpioConfig *cfg) {
    if (cfg == 0) {
        return false;
    }
    if (!DrvGpio_Init(cfg)) {
        fprintf(stderr,
                "svc_io: init failed (do_curtail=GPIO%d active_%s, di_permissive=GPIO%d "
                "active_%s)\n",
                cfg->outputs[DRVGPIO__OUT__CURTAIL].line,
                cfg->outputs[DRVGPIO__OUT__CURTAIL].active_high ? "high" : "low",
                cfg->inputs[DRVGPIO__IN__PERMISSIVE].line,
                cfg->inputs[DRVGPIO__IN__PERMISSIVE].active_high ? "high" : "low");
        return false;
    }
    /* Explicit safe state on start-up (belt-and-suspenders vs DrvGpio_Init). */
    DrvGpio_SafeState();
    fprintf(stderr, "svc_io: ready do_curtail=GPIO%d(active_%s) di_permissive=GPIO%d(active_%s)\n",
            cfg->outputs[DRVGPIO__OUT__CURTAIL].line,
            cfg->outputs[DRVGPIO__OUT__CURTAIL].active_high ? "high" : "low",
            cfg->inputs[DRVGPIO__IN__PERMISSIVE].line,
            cfg->inputs[DRVGPIO__IN__PERMISSIVE].active_high ? "high" : "low");
    return true;
}

bool svc_io_init(void) {
    const DrvGpioConfig cfg = DrvGpio_DefaultLabConfig();
    return svc_io_init_cfg(&cfg);
}

bool svc_io_apply_curtailment(bool active) {
    return DrvGpio_SetOutput(DRVGPIO__OUT__CURTAIL, active);
}

bool svc_io_read_permissive(bool *ok) {
    return DrvGpio_ReadInput(DRVGPIO__IN__PERMISSIVE, ok);
}

void svc_io_safe_state(void) {
    DrvGpio_SafeState();
}

void svc_io_shutdown(void) {
    DrvGpio_SafeState();
    cci_gpio_ll_shutdown();
}

void svc_io_log_status(const bool curtail_active, const bool permissive_ok,
                       const bool annex_m_trip_active) {
    if (s_annex_m_trip_gpio > 0) {
        fprintf(stderr, "io: curtail_do=%s permissive=%s annex_m_trip=%s\n",
                curtail_active ? "ON" : "OFF", permissive_ok ? "OK" : "BLOCKED",
                annex_m_trip_active ? "ACTIVE" : "off");
    } else {
        fprintf(stderr, "io: curtail_do=%s permissive=%s\n", curtail_active ? "ON" : "OFF",
                permissive_ok ? "OK" : "BLOCKED");
    }
}
