/*=============================================================================
 * File       :  CLOCK.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - internal clock (RTC)
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __CLOCK_H__


#define __CLOCK_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include <time.h>
#include "typedef.h"




/*===========================================================================
 * DATA_TYPES
 *===========================================================================*/
/* internal clock time (RTC) */
typedef struct
{
    /* date */
    u16         year;             // year            (2000-2099)
    u8          month;            // month           (1-12)
    u8          day;              // day             (1-31)

    /* time */
    u8          hour;             // hour            (0-23)
    u8          minute;           // minute          (0-59)
    u8          second;           // second          (0-59)

    /* day of the week */
    u8          week_day;         // day of the week (1-7) (1: Monday, ..., 7: Sunday)
} CLOCK__TIME;


/* RTC backup time */
typedef struct
{
    CLOCK__TIME rtc_backup_time;  // RTC backup time
} CLOCK__RTC_BACKUP_TIME;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* set/get time */
bool   Clock_SetTime(CLOCK__TIME *ptr_time);
bool   Clock_GetTime(CLOCK__TIME *ptr_time);


/*-----------------------------------------------------------------------------
 * 'RTC time' utilities
 *-----------------------------------------------------------------------------*/
/* compare RTC times */
s8     Clock_CompareRtcTimes   (CLOCK__TIME *ptr_time_1, CLOCK__TIME *ptr_time_2);

/* calculate the RTC time difference */
s32    Clock_DifferenceRtcTimes(CLOCK__TIME *ptr_time_1, CLOCK__TIME *ptr_time_2);

/* add time offset */
void   Clock_AddOffset         (CLOCK__TIME *ptr_time  , CLOCK__TIME *ptr_time_offset, s32 offset);

/* calculate the nearest RTC time */
bool   Clock_NearestRtcTime(CLOCK__TIME *ptr_actual_rtc_time,
                            CLOCK__TIME *ptr_nearest_rtc_time,
                            u32          month_mask,
                            u8           week_mask,
                            u8           hour_time,
                            u8           min_time,
                            u32          offset_sec);

/* calculate next 'summer time' change time */
void   Clock_NextSummerTimeChange(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_summer_time_change_rtc_time);

/* is 'summer time' */
bool   Clock_IsSummerTime(CLOCK__TIME *ptr_rtc_time);


/*-----------------------------------------------------------------------------
 * 'time' utilities
 *-----------------------------------------------------------------------------*/
/* compare times */
s8     Clock_CompareTimes(u8 hour_1, u8 minute_1, u8 second_1,
                          u8 hour_2, u8 minute_2, u8 second_2);


/*-----------------------------------------------------------------------------
 * 'day' utilities
 *-----------------------------------------------------------------------------*/
/* calculate tomorrow  day */
void   Clock_TomorrowDay     (u16 today_year, u8 today_month, u8 today_day     ,                 u16 *ptr_tomorrow_year , u8 *ptr_tomorrow_month , u8 *ptr_tomorrow_day      );
void   Clock_TomorrowWeekDay (                                u8 today_week_day,                                                                   u8 *ptr_tomorrow_week_day );

/* calculate yesterday day */
void   Clock_YesterdayDay    (u16 today_year, u8 today_month, u8 today_day     ,                 u16 *ptr_yesterday_year, u8 *ptr_yesterday_month, u8 *ptr_yesterday_day     );
void   Clock_YesterdayWeekDay(                                u8 today_week_day,                                                                   u8 *ptr_yesterday_week_day);

/* calculate next      day */
void   Clock_NextDay         (u16 today_year, u8 today_month, u8 today_day     , u8 days_offset, u16 *ptr_next_year     , u8 *ptr_next_month     , u8 *ptr_next_day          );
void   Clock_NextWeekDay     (                                u8 today_week_day, u8 days_offset,                                                   u8 *ptr_next_week_day     );

/* calculate previous  day */
void   Clock_PreviousDay     (u16 today_year, u8 today_month, u8 today_day     , u8 days_offset, u16 *ptr_previous_year , u8 *ptr_previous_month , u8 *ptr_previous_day      );
void   Clock_PreviousWeekDay (                                u8 today_week_day, u8 days_offset,                                                   u8 *ptr_previous_week_day );

/* number of days in a month */
u8     Clock_DaysInAMonth(u8 month, u16 year);

/* weekday of a day */
u8     Clock_WeekDayOfADay(u8 day, u8 month, u16 year);

/* weekday string */
ascii *Clock_WeekDayString(u8 week_day);

/* festivity day */
bool   Clock_IsFestivityDay(u8 day, u8 month, u16 year);


/*-----------------------------------------------------------------------------
 * general utilities
 *-----------------------------------------------------------------------------*/
/* check time/date */
bool   Clock_IsValidClockTime(CLOCK__TIME *ptr_time);
bool   Clock_IsValidTime     (u8 hour, u8 minute, u8  second);
bool   Clock_IsValidDate     (u8 day , u8 month , u16 year  );

/* print time */
void   Clock_PrintTime1(u8 trace_level, u8 id, u32 delay, u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second);
void   Clock_PrintTime2(u8 trace_level, u8 id, u32 delay, CLOCK__TIME *ptr_time);
void   Clock_PrintTime3(u8 trace_level, u8 id, u32 delay, time_t *ptr_timestamp);


/*-----------------------------------------------------------------------------
 * format conversion
 *-----------------------------------------------------------------------------*/
/* format conversion */
bool   Clock_ConverTimeString1ToClockTime(ascii *time_string, CLOCK__TIME *ptr_time);
bool   Clock_ConverTimeString2ToClockTime(ascii *time_string, CLOCK__TIME *ptr_time);
bool   Clock_ConverTimeString3ToClockTime(ascii *time_string, time_t *ptr_unix_time);
time_t Clock_ConverTimeString4ToClockTime(ascii *time_string);


/*-----------------------------------------------------------------------------
 * RTC backup time
 *-----------------------------------------------------------------------------*/
/* get/set RTC backup time */
void   Clock_RtcBackupTime_GetDefault(CLOCK__RTC_BACKUP_TIME *ptr_data);
void   Clock_RtcBackupTime_Get       (CLOCK__RTC_BACKUP_TIME *ptr_data);
void   Clock_RtcBackupTime_Set       (CLOCK__RTC_BACKUP_TIME *ptr_data);




#endif
