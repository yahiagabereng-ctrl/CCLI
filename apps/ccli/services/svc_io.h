/*=============================================================================
 * File       :  svc_io.h
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  DI/DO service - thin manager between PF2 and the drv_gpio
 *               driver (SK0153 outputs.c role, curtailment slice only).
 *
 * C API, callable from the C++ orchestrator (ccli_main.cpp) via extern "C".
 *=============================================================================*/
#ifndef CCI_SVC_IO_H
#define CCI_SVC_IO_H

#include <stdbool.h>

#include "drv_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the DI/DO stack with the default lab config. All outputs are
 * forced to safe (de-energized) before returning. Returns true on success. */
bool svc_io_init(void);

/* Initialize the DI/DO stack with an explicit config (pins/polarity from the
 * runtime config file). All outputs forced to safe before returning. */
bool svc_io_init_cfg(const DrvGpioConfig *cfg);

/* Drive the PF2 curtailment output: active = ON, else OFF. */
bool svc_io_apply_curtailment(bool active);

/* Read the external permissive digital input: *ok = true when permitted. */
bool svc_io_read_permissive(bool *ok);

/* Force all outputs to safe state (de-energized). Call on fault / shutdown. */
void svc_io_safe_state(void);

/* Release GPIO lines (call before exit so another instance can start). */
void svc_io_shutdown(void);

/* Log one status line to stderr (procd -> logread). */
void svc_io_log_status(bool curtail_active, bool permissive_ok);

#ifdef __cplusplus
}
#endif

#endif /* CCI_SVC_IO_H */
