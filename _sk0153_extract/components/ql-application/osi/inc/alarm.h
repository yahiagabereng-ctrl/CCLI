/*=============================================================================
 * File       :  ALARM.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - alarms
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ALARM_H__


#define __ALARM_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "status.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* alarm IDs */
#define ALARM__ALARM_ID__T_INT_MIN                  1   // ID =   1 - internal temperature below minimum temperature
#define ALARM__ALARM_ID__T_EXT_MIN                  2   // ID =   2 - external temperature below minimum temperature
#define ALARM__ALARM_ID__T_INT_MIN_RESTORED         3   // ID =   3 - internal temperature below minimum temperature restored
#define ALARM__ALARM_ID__T_EXT_MIN_RESTORED         4   // ID =   4 - external temperature below minimum temperature restored
#define ALARM__ALARM_ID__T_INT_MAX                  5   // ID =   5 - internal temperature above maximum temperature
#define ALARM__ALARM_ID__T_EXT_MAX                  6   // ID =   6 - external temperature above maximum temperature
#define ALARM__ALARM_ID__T_INT_MAX_RESTORED         7   // ID =   7 - internal temperature above maximum temperature restored
#define ALARM__ALARM_ID__T_EXT_MAX_RESTORED         8   // ID =   8 - external temperature above maximum temperature restored
#define ALARM__ALARM_ID__IN1                        9   // ID =   9 - digital input IN1
#define ALARM__ALARM_ID__IN2                        10  // ID =  10 - digital input IN2
#define ALARM__ALARM_ID__IN3                        11  // ID =  11 - digital input IN3
#define ALARM__ALARM_ID__IN4                        12  // ID =  12 - digital input IN4
#define ALARM__ALARM_ID__IN1_RESTORED               13  // ID =  13 - digital input IN1 restored
#define ALARM__ALARM_ID__IN2_RESTORED               14  // ID =  14 - digital input IN2 restored
#define ALARM__ALARM_ID__IN3_RESTORED               15  // ID =  15 - digital input IN3 restored
#define ALARM__ALARM_ID__IN4_RESTORED               16  // ID =  16 - digital input IN4 restored
#define ALARM__ALARM_ID__MAIN_POWER                 17  // ID =  17 - main power
#define ALARM__ALARM_ID__MAIN_POWER_RESTORED        18  // ID =  18 - main power restored
#define ALARM__ALARM_ID__CREDIT_WARNING             19  // ID =  19 - credit warning

/* number of alarm IDs */
#define ALARM__NUM_OF_ALARM_IDS                     19  // number of           alarm IDs
#define ALARM__NUM_OF_ALARM_IDS_AVAILABLE           20  // number of available alarm IDs

/* alarm modes */
#define ALARM__ALARM_MODE__SMS                      0   // alarm mode - SMS
#define ALARM__ALARM_MODE__MAIL                     1   // alarm mode - mail




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* alarm data */
typedef struct
{
    u8                    alarm_id;                                     /* alarm type ID */
    u16                   alarm_par;                                    /* alarm parameter (optional) */
} ALARM__ALARM_DATA;


/* alarm */
typedef struct
{
    ALARM__ALARM_DATA     alarm_data;                                   /* alarm data    */
    STATUS__DEVICE_STATUS device_status;                                /* device status */
} ALARM__ALARM;


/* alarm counters */
typedef struct
{
    /* total   alarm counter */
    u32                   total  [ALARM__NUM_OF_ALARM_IDS_AVAILABLE];   /* total   alarm counter */

    /* yearly  alarm counter */
    u32                   yearly [ALARM__NUM_OF_ALARM_IDS_AVAILABLE];   /* yearly  alarm counter */

    /* monthly alarm counter */
    u32                   monthly[ALARM__NUM_OF_ALARM_IDS_AVAILABLE];   /* monthly alarm counter */
} ALARM__ALARM_COUNTERS;


/* configuration - "alarm mode" */
typedef struct
{
    u8 mode;    // alarm mode (0:
} ALARM__CONFIG__ALARM_MODE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void   Alarm_TaskAlarm(void *argument);

/* events */
bool   Alarm_Event_NewAlarm     (u8 alarm_type, u16 alarm_par);
bool   Alarm_Event_AlarmFileSent(ascii *file_name);

/* alarm file */
void   Alarm_AlarmFile_AppendDataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2);

/* alarm string */
ascii *Alarm_AlarmLongString (u8 alarm_type);
ascii *Alarm_AlarmShortString(u8 alarm_type);

/* reset alarm counters */
void   Alarm_AlarmCountersTotalReset  (void);
void   Alarm_AlarmCountersYearlyReset (void);
void   Alarm_AlarmCountersMonthlyReset(void);
void   Alarm_AlarmCountersReset       (void);
void   Alarm_AlarmCounterTotalReset   (u8 alarm_type);
void   Alarm_AlarmCounterYearlyReset  (u8 alarm_type);
void   Alarm_AlarmCounterMonthlyReset (u8 alarm_type);
void   Alarm_AlarmCounterReset        (u8 alarm_type);

/* get/set alarm counters */
void   Alarm_AlarmCounters_GetDefault(ALARM__ALARM_COUNTERS *ptr_data);
void   Alarm_AlarmCounters_Get       (ALARM__ALARM_COUNTERS *ptr_data);
void   Alarm_AlarmCounters_Set       (ALARM__ALARM_COUNTERS *ptr_data);

/* get/set "alarm mode" configuration */
void   Alarm_Config_AlarmMode_GetDefault(ALARM__CONFIG__ALARM_MODE *ptr_data);
void   Alarm_Config_AlarmMode_Get       (ALARM__CONFIG__ALARM_MODE *ptr_data);
void   Alarm_Config_AlarmMode_Set       (ALARM__CONFIG__ALARM_MODE *ptr_data);
bool   Alarm_Config_AlarmMode_IsValid   (ALARM__CONFIG__ALARM_MODE *ptr_data);




#endif
