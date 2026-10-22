/*=============================================================================
 * File       :  FW_CONFIG.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - firmware configuration (compile-time + loaded yaml)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __FW_CONFIG_H__


#define __FW_CONFIG_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"

#include "ccli_bridge.h"
#include "drvgpio.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* default runtime yaml (procd passes --config from UCI ccli.main.config_file) */
#define FW_CONFIG__DEFAULT_CONFIG_PATH          "/etc/ccli/lab.yaml"

/* max length of the runtime yaml path */
#define FW_CONFIG__MAX_LEN_CONFIG_PATH          255

/* supervisor tick (ms) */
#define FW_CONFIG__MAIN__TICK_MS                100

/* periodic tasks (in supervisor ticks) */
#define FW_CONFIG__MAIN__STATUS_PERIOD_TICKS    50      /* 5 s  - status line + bench file */
#define FW_CONFIG__MAIN__GOOSE_PERIOD_TICKS     10      /* 1 s  - GOOSE link monitor       */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* runtime configuration */
typedef CCLI_BRIDGE__CONFIG FW_CONFIG__RUNTIME;




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* load the runtime yaml (safe compiled defaults if missing) */
bool                      FwConfig_Load(const ascii *config_path, bool lab_demo);

/* get the loaded runtime configuration */
const FW_CONFIG__RUNTIME *FwConfig_Get(void);

/* DI/DO channel map from the loaded configuration */
void                      FwConfig_GetDrvGpioConfig(DrvGpioConfig *ptr_data);

#ifdef __cplusplus
}
#endif




#endif
