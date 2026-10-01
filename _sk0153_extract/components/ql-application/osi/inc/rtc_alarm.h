/*=============================================================================
 * File       :  RTC_ALARM.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - RTC alarm
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __RTC_ALARM_H__


#define __RTC_ALARM_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* maximum number of RTC alarms */
#define RTC_ALARM__MAX_NUM_OF_RTC_ALARMS       10

/* defined RTC alarms */
#define RTC_ALARM__ALARM_F_CHRONO_INT          0    /* internal chrono function */
#define RTC_ALARM__ALARM_F_CHRONO_EXT          1    /* external chrono function */
#define RTC_ALARM__ALARM_SUMMER_TIME           2    /* summer time change       */
#define RTC_ALARM__ALARM_AUTOSYNCHRONIZE       3    /* clock autosychronize     */
#define RTC_ALARM__ALARM_LOG_STATUS_DURATION   4    /* log status duration      */   // Note: "RTC_ALARM__ALARM_LOG_STATUS_DURATION" must to be before than "RTC_ALARM__ALARM_LOG_STATUS_PERIOD"
#define RTC_ALARM__ALARM_LOG_STATUS_PERIOD     5    /* log status period        */
#define RTC_ALARM__ALARM_INPUT_C1_BAND         6    /* input C1 band            */
#define RTC_ALARM__ALARM_INPUT_C2_BAND         7    /* input C2 band            */
#define RTC_ALARM__ALARM_NEAREST               100  /* nearest alarm            */




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* RTC alarm */
typedef struct
{
    bool        status;           // RTC alarm enabled status [FALSE/TRUE]
    CLOCK__TIME rtc_alarm_time;   // RTC alarm time
} RTC_ALARM__RTC_ALARM;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void RtcAlarm_TaskRtcAlarm(void *argument);

/* status variables */
void RtcAlarm_UpdateVarCrc(void);
bool RtcAlarm_VerifyVarCrc(void);

/* info */
bool RtcAlarm_GetRtcAlarms      (u8 rtc_alarm_id, RTC_ALARM__RTC_ALARM *ptr_rtc_alarm);
bool RtcAlarm_GetNearestRtcAlarm(                 RTC_ALARM__RTC_ALARM *ptr_rtc_alarm);

/* action on RTC alarms */
bool RtcAlarm_RtcAlarmSet   (u8 rtc_alarm_id, CLOCK__TIME *ptr_rtc_alarm_time);
bool RtcAlarm_RtcAlarmDelete(u8 rtc_alarm_id);

/* read RTC alarm description */
ascii *RtcAlarm_ReadRtcAlarmDescription(u8 rtc_alarm_id);




#endif
