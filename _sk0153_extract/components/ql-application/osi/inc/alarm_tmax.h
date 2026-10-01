/*=============================================================================
 * File       :  ALARM_TMAX.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - maximum temperature alarm manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ALARM_TMAX_H__


#define __ALARM_TMAX_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* internal temperature status */
typedef struct
{
    u8   status_tmax_int;                          /* internal temperature status       */
    bool status_change_timer_int;                  /* indication of status change timer */
} ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT;


/* external temperature status */
typedef struct
{
    u8   status_tmax_ext;                          /* external temperature status */
    bool status_change_timer_ext;                  /* indication of status change timer */
} ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - maximum internal temperature alarm */
typedef struct
{
    bool alarm_enabled_status_tmax_int;            /* maximum internal temperature          alarm enabled status */
    bool alarm_enabled_status_tmax_int_restored;   /* maximum internal temperature restored alarm enabled status */
} ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT;


/* configuration - maximum external temperature alarm */
typedef struct
{
    bool alarm_enabled_status_tmax_ext;            /* maximum external temperature          alarm enabled status */
    bool alarm_enabled_status_tmax_ext_restored;   /* maximum external temperature restored alarm enabled status */
} ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT;


/* configuration - maximum internal temperature */
typedef struct
{
    s16  temperature_max_int;                      /* maximum internal temperature for alarm (/10 °C) */   // [-20.0 - +55.0 °C]
} ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT;


/* configuration - maximum external temperature */
typedef struct
{
    s16  temperature_max_ext;                      /* maximum external temperature for alarm (/10 °C) */   // [-20.0 - +55.0 °C]
} ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void AlarmTMax_TaskAlarmTMax(void *argument);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature status" */
void AlarmTMax_Status_TemperatureMaxInt_GetDefault      (ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       *ptr_data);
void AlarmTMax_Status_TemperatureMaxInt_Get             (ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       *ptr_data);
void AlarmTMax_Status_TemperatureMaxInt_Set             (ALARM_TMAX__STATUS__TEMPERATURE_MAX_INT       *ptr_data);

/* get/set "external temperature status" */
void AlarmTMax_Status_TemperatureMaxExt_GetDefault      (ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       *ptr_data);
void AlarmTMax_Status_TemperatureMaxExt_Get             (ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       *ptr_data);
void AlarmTMax_Status_TemperatureMaxExt_Set             (ALARM_TMAX__STATUS__TEMPERATURE_MAX_EXT       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configurations
 *-----------------------------------------------------------------------------*/
/* get/set "internal maximum temperature alarm" configuration */
void AlarmTMax_Config_AlarmTemperatureMaxInt_GetDefault (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);
void AlarmTMax_Config_AlarmTemperatureMaxInt_Get        (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);
void AlarmTMax_Config_AlarmTemperatureMaxInt_Set        (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);
bool AlarmTMax_Config_AlarmTemperatureMaxInt_IsValid    (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT *ptr_data);

/* get/set "external maximum temperature alarm" configuration */
void AlarmTMax_Config_AlarmTemperatureMaxExt_GetDefault (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);
void AlarmTMax_Config_AlarmTemperatureMaxExt_Get        (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);
void AlarmTMax_Config_AlarmTemperatureMaxExt_Set        (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);
bool AlarmTMax_Config_AlarmTemperatureMaxExt_IsValid    (ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT *ptr_data);

/* get/set "internal maximum temperature"       configuration */
void AlarmTMax_Config_TemperatureMaxInt_GetDefault      (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);
void AlarmTMax_Config_TemperatureMaxInt_Get             (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);
void AlarmTMax_Config_TemperatureMaxInt_Set             (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);
bool AlarmTMax_Config_TemperatureMaxInt_IsValid         (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT       *ptr_data);

/* get/set "external maximum temperature"       configuration */
void AlarmTMax_Config_TemperatureMaxExt_GetDefault      (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);
void AlarmTMax_Config_TemperatureMaxExt_Get             (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);
void AlarmTMax_Config_TemperatureMaxExt_Set             (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);
bool AlarmTMax_Config_TemperatureMaxExt_IsValid         (ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT       *ptr_data);




#endif
