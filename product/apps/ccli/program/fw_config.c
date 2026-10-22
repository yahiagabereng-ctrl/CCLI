/*=============================================================================
 * File       :  FW_CONFIG.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - firmware configuration (compile-time + loaded yaml)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "fw_config.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* loaded runtime configuration */
static FW_CONFIG__RUNTIME FwConfig_Runtime;




/*=============================================================================
 * Function   : FwConfig_Load
 *
 * Description: load the runtime yaml
 * Input      : config_path - yaml path
 *              lab_demo    - force the Modbus simulator backend
 * Output     : TRUE when loaded from file
 *=============================================================================*/
bool FwConfig_Load(const ascii *config_path, bool lab_demo)
{
    memset(&FwConfig_Runtime, 0, sizeof(FwConfig_Runtime));
    return CcliBridge_Config_Load(config_path, lab_demo, &FwConfig_Runtime);
}




/*=============================================================================
 * Function   : FwConfig_Get
 *
 * Description: get the loaded runtime configuration
 * Input      : -
 * Output     : runtime configuration
 *=============================================================================*/
const FW_CONFIG__RUNTIME *FwConfig_Get(void)
{
    return &FwConfig_Runtime;
}




/*=============================================================================
 * Function   : FwConfig_GetDrvGpioConfig
 *
 * Description: DI/DO channel map from the loaded configuration
 * Input      : -
 * Output     : ptr_data - channel configuration
 *=============================================================================*/
void FwConfig_GetDrvGpioConfig(DrvGpioConfig *ptr_data)
{
    memset(ptr_data, 0, sizeof(*ptr_data));
    ptr_data->outputs[DRVGPIO__OUT__CURTAIL  ].line        = FwConfig_Runtime.do_curtail_gpio;
    ptr_data->outputs[DRVGPIO__OUT__CURTAIL  ].active_high = FwConfig_Runtime.do_curtail_active_high;
    ptr_data->inputs [DRVGPIO__IN__PERMISSIVE].line        = FwConfig_Runtime.di_permissive_gpio;
    ptr_data->inputs [DRVGPIO__IN__PERMISSIVE].active_high = FwConfig_Runtime.di_permissive_active_high;
}
