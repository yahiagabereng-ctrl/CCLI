/*=============================================================================
 * File       :  ALARM_TMIN.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - minimum temperature alarm manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ALARM_TMIN_H__


#define __ALARM_TMIN_H__




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
    u8   status_tmin_int;                          /* internal temperature status       */
    bool status_change_timer_int;                  /* indication of status change timer */
} ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT;


/* external temperature status */
typedef struct
{
    u8   status_tmin_ext;                          /* external temperature status       */
    bool status_change_timer_ext;                  /* indication of status change timer */
} ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - minimum internal temperature alarm */
typedef struct
{
    bool alarm_enabled_status_tmin_int;            /* minimum internal temperature          alarm enabled status */
    bool alarm_enabled_status_tmin_int_restored;   /* minimum internal temperature restored alarm enabled status */
} ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT;


/* configuration - minimum external temperature alarm */
typedef struct
{
    bool alarm_enabled_status_tmin_ext;            /* minimum external temperature          alarm enabled status */
    bool alarm_enabled_status_tmin_ext_restored;   /* minimum external temperature restored alarm enabled status */
} ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT;


/* configuration - minimum internal temperature */
typedef struct
{
    s16  temperature_min_int;                      /* minimum internal temperature for alarm (/10 °C) */   // [-20.0 - +55.0 °C]
} ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT;


/* configuration - minimum external temperature */
typedef struct
{
    s16  temperature_min_ext;                      /* minimum external temperature for alarm (/10 °C) */   // [-20.0 - +55.0 °C]
} ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void AlarmTMin_TaskAlarmTMin(void *argument);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "internal temperature status" */
void AlarmTMin_Status_TemperatureMinInt_GetDefault      (ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       *ptr_data);
void AlarmTMin_Status_TemperatureMinInt_Get             (ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       *ptr_data);
void AlarmTMin_Status_TemperatureMinInt_Set             (ALARM_TMIN__STATUS__TEMPERATURE_MIN_INT       *ptr_data);

/* get/set "external temperature status" */
void AlarmTMin_Status_TemperatureMinExt_GetDefault      (ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       *ptr_data);
void AlarmTMin_Status_TemperatureMinExt_Get             (ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       *ptr_data);
void AlarmTMin_Status_TemperatureMinExt_Set             (ALARM_TMIN__STATUS__TEMPERATURE_MIN_EXT       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configurations
 *-----------------------------------------------------------------------------*/
/* get/set "internal minimum temperature alarm" configuration */
void AlarmTMin_Config_AlarmTemperatureMinInt_GetDefault (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);
void AlarmTMin_Config_AlarmTemperatureMinInt_Get        (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);
void AlarmTMin_Config_AlarmTemperatureMinInt_Set        (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);
bool AlarmTMin_Config_AlarmTemperatureMinInt_IsValid    (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT *ptr_data);

/* get/set "external minimum temperature alarm" configuration */
void AlarmTMin_Config_AlarmTemperatureMinExt_GetDefault (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);
void AlarmTMin_Config_AlarmTemperatureMinExt_Get        (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);
void AlarmTMin_Config_AlarmTemperatureMinExt_Set        (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);
bool AlarmTMin_Config_AlarmTemperatureMinExt_IsValid    (ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT *ptr_data);

/* get/set "internal minimum temperature"       configuration */
void AlarmTMin_Config_TemperatureMinInt_GetDefault      (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);
void AlarmTMin_Config_TemperatureMinInt_Get             (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);
void AlarmTMin_Config_TemperatureMinInt_Set             (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);
bool AlarmTMin_Config_TemperatureMinInt_IsValid         (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT       *ptr_data);

/* get/set "external minimum temperature"       configuration */
void AlarmTMin_Config_TemperatureMinExt_GetDefault      (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);
void AlarmTMin_Config_TemperatureMinExt_Get             (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);
void AlarmTMin_Config_TemperatureMinExt_Set             (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);
bool AlarmTMin_Config_TemperatureMinExt_IsValid         (ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT       *ptr_data);




#endif
