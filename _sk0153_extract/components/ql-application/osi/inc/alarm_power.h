/*=============================================================================
 * File       :  ALARM_POWER.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - power alarm
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ALARM_POWER_H__


#define __ALARM_POWER_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - power alarm */
typedef struct
{
    bool alarm_enabled_status_power;            /* power          alarm enabled status */
    bool alarm_enabled_status_power_restored;   /* power restored alarm enabled status */
} ALARM_POWER__CONFIG__ALARM_POWER;


/* status - last power alarm */
typedef struct
{
    u8   last_power_alarm;   /* last power alarm */
} ALARM_POWER__STATUS__LAST_POWER_ALARM;





/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* alarm power task */
void AlarmPower_TaskAlarmPower(void *argument);

/* get/set "power alarm" configuration */
void AlarmPower_Config_AlarmPower_GetDefault    (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);
void AlarmPower_Config_AlarmPower_Get           (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);
void AlarmPower_Config_AlarmPower_Set           (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);
bool AlarmPower_Config_AlarmPower_IsValid       (ALARM_POWER__CONFIG__ALARM_POWER      *ptr_data);

/* get/set "last power alarm" */
void AlarmPower_Status_LastPowerAlarm_GetDefault(ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data);
void AlarmPower_Status_LastPowerAlarm_Get       (ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data);
void AlarmPower_Status_LastPowerAlarm_Set       (ALARM_POWER__STATUS__LAST_POWER_ALARM *ptr_data);




#endif
