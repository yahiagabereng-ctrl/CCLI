/*=============================================================================
 * File       :  FW_CONFIG.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - FW configuration
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
#include "fw_config.h"
#include "typedef.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* time jump */
static       FW_CONFIG__ID_MODEL FwConfig_IdModel;
static const FW_CONFIG__ID_MODEL FwConfig_IdModelDefault =
{
#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LSHW)
    FW_CONFIG__TYPE__THERMOSTAT,
#endif
#if defined(FW_CONFIG__VERSION__LFVD) || defined(FW_CONFIG__VERSION__LFVW)
    FW_CONFIG__TYPE__PHOTOVOLTAIC,
#endif

#if defined(FW_CONFIG__VERSION__LSHD) || defined(FW_CONFIG__VERSION__LFVD)
    FW_CONFIG__HARDWARE__DIN,
#endif
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    FW_CONFIG__HARDWARE__WALL,
#endif
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set time jump */
void FwConfig_IdModel_GetDefault(FW_CONFIG__ID_MODEL *ptr_data);
void FwConfig_IdModel_Get       (FW_CONFIG__ID_MODEL *ptr_data);
void FwConfig_IdModel_Set       (FW_CONFIG__ID_MODEL *ptr_data);
bool FwConfig_IdModel_IsValid   (FW_CONFIG__ID_MODEL *ptr_data);




/*===========================================================================
 * Function   :   FwConfig_IdModel_GetDefault
 *
 * Description:   get the default "model identifier" configuration
 * Input      :   - ptr_data: pointer to store the values got
 * Output     :   -
 *===========================================================================*/
void FwConfig_IdModel_GetDefault(FW_CONFIG__ID_MODEL *ptr_data)
{
    *ptr_data = FwConfig_IdModelDefault;
}




/*===========================================================================
 * Function   :   FwConfig_IdModel_Get
 *
 * Description:   get the "model identifier" configuration
 * Input      :   - ptr_data: pointer to store the values got
 * Output     :   -
 *===========================================================================*/
void FwConfig_IdModel_Get(FW_CONFIG__ID_MODEL *ptr_data)
{
    *ptr_data = FwConfig_IdModel;
}




/*===========================================================================
 * Function   :   FwConfig_IdModel_Set
 *
 * Description:   set the "model identifier" configuration
 * Input      :   - ptr_data: pointer to the values to set
 * Output     :   -
 *===========================================================================*/
void FwConfig_IdModel_Set(FW_CONFIG__ID_MODEL *ptr_data)
{
    FwConfig_IdModel = *ptr_data;
}




/*===========================================================================
 * Function   : FwConfig_IdModel_IsValid
 *
 * Description: check if the "model identifier" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FwConfig_IdModel_IsValid(FW_CONFIG__ID_MODEL *ptr_data)
{
    return TRUE;
}
