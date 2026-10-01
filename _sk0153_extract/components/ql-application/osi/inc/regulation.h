/*=============================================================================
 * File       :  REGULATION.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - temperature regulation
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __REGULATION_H__


#define __REGULATION_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - "active status OUT1" */
typedef struct
{
    bool active_status;     /* active status [FALSE: relay de-energized, TRUE: relay energized] */
} REGULATION__CONFIG__ACTIVE_STATUS_OUT1;


/* configuration - "active status OUT2" */
typedef struct
{
    bool active_status;     /* active status [0,1] */
} REGULATION__CONFIG__ACTIVE_STATUS_OUT2;


/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* internal temperature status */
typedef struct
{
    u8   status_t_int;                    /* internal temperature status             */
    bool status_change_timer_start_int;   /* indication of status change start timer */
    bool status_change_timer_stop_int;    /* indication of status change stop  timer */
} REGULATION__STATUS__TEMPERATURE_INT;


/* external temperature status */
typedef struct
{
    u8   status_t_ext;                    /* external temperature status             */
    bool status_change_timer_start_ext;   /* indication of status change start timer */
    bool status_change_timer_stop_ext;    /* indication of status change stop  timer */
} REGULATION__STATUS__TEMPERATURE_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Regulation_TaskRegulation(void *argument);

/* status variables */
void Regulation_UpdateVarCrc(void);
bool Regulation_VerifyVarCrc(void);

/* outputs status (logical status) */
bool Regulation_StatusLogicalOutputInt(void);
bool Regulation_StatusLogicalOutputExt(void);

/* outputs status (expected physical status) */
bool Regulation_StatusPhysicalExpectedOutputInt(void);
bool Regulation_StatusPhysicalExpectedOutputExt(void);


/*-----------------------------------------------------------------------------
 * task events
 *-----------------------------------------------------------------------------*/
/* internal temperature regulation */
bool Regulation_RegulationTemperatureInt_Off       (void);
bool Regulation_RegulationTemperatureInt_On        (void);
bool Regulation_RegulationTemperatureInt_Regulation(s16 temperature);

/* external temperature regulation */
bool Regulation_RegulationTemperatureExt_Off       (void);
bool Regulation_RegulationTemperatureExt_On        (void);
bool Regulation_RegulationTemperatureExt_Regulation(s16 temperature);

/* outputs block/unblock */
void Regulation_OutputsBlock  (void);
void Regulation_OutputsUnblock(void);

/* new active status output configuration */
void Regulation_NewConfigActiveStatusOut1(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_config_new, REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_config_old);
void Regulation_NewConfigActiveStatusOut2(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_config_new, REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_config_old);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "active status OUT1" configuration */
void Regulation_Config_ActiveStatusOut1_GetDefault(REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);
void Regulation_Config_ActiveStatusOut1_Get       (REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);
void Regulation_Config_ActiveStatusOut1_Set       (REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);
bool Regulation_Config_ActiveStatusOut1_IsValid   (REGULATION__CONFIG__ACTIVE_STATUS_OUT1 *ptr_data);

/* get/set "active status OUT2" configuration */
void Regulation_Config_ActiveStatusOut2_GetDefault(REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);
void Regulation_Config_ActiveStatusOut2_Get       (REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);
void Regulation_Config_ActiveStatusOut2_Set       (REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);
bool Regulation_Config_ActiveStatusOut2_IsValid   (REGULATION__CONFIG__ACTIVE_STATUS_OUT2 *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature status" */
void Regulation_Status_TemperatureInt_GetDefault(REGULATION__STATUS__TEMPERATURE_INT *ptr_data);
void Regulation_Status_TemperatureInt_Get       (REGULATION__STATUS__TEMPERATURE_INT *ptr_data);
void Regulation_Status_TemperatureInt_Set       (REGULATION__STATUS__TEMPERATURE_INT *ptr_data);

/* get/set "external temperature status" */
void Regulation_Status_TemperatureExt_GetDefault(REGULATION__STATUS__TEMPERATURE_EXT *ptr_data);
void Regulation_Status_TemperatureExt_Get       (REGULATION__STATUS__TEMPERATURE_EXT *ptr_data);
void Regulation_Status_TemperatureExt_Set       (REGULATION__STATUS__TEMPERATURE_EXT *ptr_data);




#endif
