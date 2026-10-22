/*=============================================================================
 * File       :  svc_io.c
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DI/DO service - compatibility forwarders to program/outputs.c.
 *=============================================================================*/
#include "svc_io.h"

#include "outputs.h"

bool svc_io_configure_annex_m_trip_monitor(int gpio_channel, bool enabled) {
    return Outputs_ConfigureAnnexMTripMonitor(gpio_channel, enabled);
}

bool svc_io_read_annex_m_trip_active(bool *active) {
    return Outputs_ReadAnnexMTripActive(active);
}

bool svc_io_init_cfg(const DrvGpioConfig *cfg) {
    return Outputs_Init(cfg);
}

bool svc_io_init(void) {
    return Outputs_InitDefault();
}

bool svc_io_apply_curtailment(bool active) {
    return Outputs_ApplyCurtailment(active);
}

bool svc_io_read_permissive(bool *ok) {
    return Outputs_ReadPermissive(ok);
}

void svc_io_safe_state(void) {
    Outputs_SafeState();
}

void svc_io_shutdown(void) {
    Outputs_Shutdown();
}

void svc_io_log_status(const bool curtail_active, const bool permissive_ok,
                       const bool annex_m_trip_active) {
    Outputs_LogStatus(curtail_active, permissive_ok, annex_m_trip_active);
}
