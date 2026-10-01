/*=============================================================================
 * File       :  CLOCK.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - internal clock (RTC)
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 * The internal clock time (RTC) can be regulated in one of the following ways:
 *   - external input:
 *       - a received "SET TIME" command (received SMS or connection with a PC)
 *       - from the GSM network (if it is available)
 *       - from the GPS time    (if it is available)
 *   - automatic:
 *       - with a REALIGN procedure (SMS sent to itself)
 *       - with Internet time       (GPRS connection)
 */

/*
 * The RTC of Sierra Wireless modules starts from "Saturday 01/01/2000 00:00:00"
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* API      includes */
#include "ql_api_rtc.h"

/* user     includes */
#include "typedef.h"
#include "clock.h"
#include "debug_my.h"
#include "language.h"
#include "strings_my.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* maximum number of festivity days */
#define MAX_NUM_OF_FESTIVITY_DAYS_FIXED     15    /* maximum number of festivity days (with fixed    day in the year) */
#define MAX_NUM_OF_FESTIVITY_DAYS_VARIABLE  100   /* maximum number of festivity days (with variable day in the year) */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING             200




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* day of the year */
typedef struct
{
    u8              day;                                                 /* day   (1-31) */
    u8              month;                                               /* month (1-12) */
} DAY_OF_THE_YEAR;


/* day */
typedef struct
{
    u8              day;                                                 /* day   (1-31)      */
    u8              month;                                               /* month (1-12)      */
    u16             year;                                                /* year  (2000-2099) */
} DAY;


/* festivity days (with fixed    day in the year) */
typedef struct
{
    DAY_OF_THE_YEAR festivity_days[MAX_NUM_OF_FESTIVITY_DAYS_FIXED   ];  /* festivity days (with fixed    day in the year) */
} FESTIVITY_DAYS_FIXED_IN_THE_YEAR;


/* festivity days (with variable day in the year) */
typedef struct
{
    DAY             festivity_days[MAX_NUM_OF_FESTIVITY_DAYS_VARIABLE];  /* festivity days (with variable day in the year) */
} FESTIVITY_DAYS_VARIABLE_IN_THE_YEAR;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* RTC backup time */
static       CLOCK__RTC_BACKUP_TIME Clock_RtcBackupTime;
static const CLOCK__RTC_BACKUP_TIME Clock_RtcBackupTimeDefault =
{
    {
        /* date */
        2000,
        1,
        1,

        /* time */
        0,
        0,
        0,

        /* day of the week */
        6    // Saturday
    }
};


/* number of days in the months */                     // ---  Jan Feb Mar  Apr May Jun  Jul Aug Sep  Oct Nov Dec
static const u8    Clock_DaysInMonthsForNormalYear[13] = { 0,  31, 28, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31};
static const u8    Clock_DaysInMonthsForLeapYear  [13] = { 0,  31, 29, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31};


/* festivity days (with fixed    day in the year) (for Italy) */
static const FESTIVITY_DAYS_FIXED_IN_THE_YEAR    Clock_FestivityDaysFixedInTheYear =
{
    {
    //   day  month
        {  1,  1       },   //   1 - 01/01/yyyy
        {  6,  1       },   //   2 - 06/01/yyyy
      //{  ?,  ?       },   //       ??/??/yyyy  (Monday after Easter, variable day in the year)
        { 25,  4       },   //   3 - 25/04/yyyy
        {  1,  5       },   //   4 - 01/05/yyyy
        {  2,  6       },   //   5 - 02/06/yyyy
        { 15,  8       },   //   6 - 15/08/yyyy
        {  1, 11       },   //   7 - 01/11/yyyy
        {  8, 12       },   //   8 - 08/12/yyyy
        { 25, 12       },   //   9 - 25/12/yyyy
        { 26, 12       },   //  10 - 26/12/yyyy

        {  0,  0       },   //  11 - not used
        {  0,  0       },   //  12 - not used
        {  0,  0       },   //  13 - not used
        {  0,  0       },   //  14 - not used
        {  0,  0       },   //  15 - not used
    }
};


/* festivity days (with variable day in the year) (for Italy) */
static const FESTIVITY_DAYS_VARIABLE_IN_THE_YEAR Clock_FestivityDaysVariableInTheYear =
{
    {
    //   day month year
        {24,  4,   2000},   //   1 - Monday after Easter, year 2000
        {16,  4,   2001},   //   2 - Monday after Easter, year 2001
        { 1,  4,   2002},   //   3 - Monday after Easter, year 2002
        {21,  4,   2003},   //   4 - Monday after Easter, year 2003
        {12,  4,   2004},   //   5 - Monday after Easter, year 2004
        {28,  3,   2005},   //   6 - Monday after Easter, year 2005
        {17,  4,   2006},   //   7 - Monday after Easter, year 2006
        { 9,  4,   2007},   //   8 - Monday after Easter, year 2007
        {24,  3,   2008},   //   9 - Monday after Easter, year 2008
        {13,  4,   2009},   //  10 - Monday after Easter, year 2009

        { 5,  4,   2010},   //  11 - Monday after Easter, year 2010
        {25,  4,   2011},   //  12 - Monday after Easter, year 2011
        { 9,  4,   2012},   //  13 - Monday after Easter, year 2012
        { 1,  4,   2013},   //  14 - Monday after Easter, year 2013
        {21,  4,   2014},   //  15 - Monday after Easter, year 2014
        { 6,  4,   2015},   //  16 - Monday after Easter, year 2015
        {28,  3,   2016},   //  17 - Monday after Easter, year 2016
        {17,  4,   2017},   //  18 - Monday after Easter, year 2017
        { 2,  4,   2018},   //  19 - Monday after Easter, year 2018
        {22,  4,   2019},   //  20 - Monday after Easter, year 2019

        {13,  4,   2020},   //  21 - Monday after Easter, year 2020
        { 5,  4,   2021},   //  22 - Monday after Easter, year 2021
        {18,  4,   2022},   //  23 - Monday after Easter, year 2022
        {10,  4,   2023},   //  24 - Monday after Easter, year 2023
        { 1,  4,   2024},   //  25 - Monday after Easter, year 2024
        {21,  4,   2025},   //  26 - Monday after Easter, year 2025
        { 6,  4,   2026},   //  27 - Monday after Easter, year 2026
        {29,  3,   2027},   //  28 - Monday after Easter, year 2027
        {17,  4,   2028},   //  29 - Monday after Easter, year 2028
        { 2,  4,   2029},   //  30 - Monday after Easter, year 2029

        {22,  4,   2030},   //  31 - Monday after Easter, year 2030
        {14,  4,   2031},   //  32 - Monday after Easter, year 2031
        {29,  3,   2032},   //  33 - Monday after Easter, year 2032
        {18,  4,   2033},   //  34 - Monday after Easter, year 2033
        {10,  4,   2034},   //  35 - Monday after Easter, year 2034
        {26,  3,   2035},   //  36 - Monday after Easter, year 2035
        {14,  4,   2036},   //  37 - Monday after Easter, year 2036
        { 6,  4,   2037},   //  38 - Monday after Easter, year 2037
        {26,  4,   2038},   //  39 - Monday after Easter, year 2038
        {11,  4,   2039},   //  40 - Monday after Easter, year 2039

        { 2,  4,   2040},   //  41 - Monday after Easter, year 2040
        {22,  4,   2041},   //  42 - Monday after Easter, year 2041
        { 7,  4,   2042},   //  43 - Monday after Easter, year 2042
        {30,  3,   2043},   //  44 - Monday after Easter, year 2043
        {18,  4,   2044},   //  45 - Monday after Easter, year 2044
        {10,  4,   2045},   //  46 - Monday after Easter, year 2045
        {26,  3,   2046},   //  47 - Monday after Easter, year 2046
        {15,  4,   2047},   //  48 - Monday after Easter, year 2047
        { 6,  4,   2048},   //  49 - Monday after Easter, year 2048
        {19,  4,   2049},   //  50 - Monday after Easter, year 2049

        {11,  4,   2050},   //  51 - Monday after Easter, year 2050
        { 3,  4,   2051},   //  52 - Monday after Easter, year 2051
        {22,  4,   2052},   //  53 - Monday after Easter, year 2052
        { 7,  4,   2053},   //  54 - Monday after Easter, year 2053
        {30,  3,   2054},   //  55 - Monday after Easter, year 2054
        {19,  4,   2055},   //  56 - Monday after Easter, year 2055
        { 3,  4,   2056},   //  57 - Monday after Easter, year 2056
        {23,  4,   2057},   //  58 - Monday after Easter, year 2057
        {15,  4,   2058},   //  59 - Monday after Easter, year 2058
        {31,  3,   2059},   //  60 - Monday after Easter, year 2059

        {19,  4,   2060},   //  61 - Monday after Easter, year 2060
        {11,  4,   2061},   //  62 - Monday after Easter, year 2061
        {27,  3,   2062},   //  63 - Monday after Easter, year 2062
        {16,  4,   2063},   //  64 - Monday after Easter, year 2063
        { 7,  4,   2064},   //  65 - Monday after Easter, year 2064
        {30,  3,   2065},   //  66 - Monday after Easter, year 2065
        {12,  4,   2066},   //  67 - Monday after Easter, year 2066
        { 4,  4,   2067},   //  68 - Monday after Easter, year 2067
        {23,  4,   2068},   //  69 - Monday after Easter, year 2068
        {15,  4,   2069},   //  70 - Monday after Easter, year 2069

        {31,  3,   2070},   //  71 - Monday after Easter, year 2070
        {20,  4,   2071},   //  72 - Monday after Easter, year 2071
        {11,  4,   2072},   //  73 - Monday after Easter, year 2072
        {27,  3,   2073},   //  74 - Monday after Easter, year 2073
        {16,  4,   2074},   //  75 - Monday after Easter, year 2074
        { 8,  4,   2075},   //  76 - Monday after Easter, year 2075
        {20,  4,   2076},   //  77 - Monday after Easter, year 2076
        {12,  4,   2077},   //  78 - Monday after Easter, year 2077
        { 4,  4,   2078},   //  79 - Monday after Easter, year 2078
        {24,  4,   2079},   //  80 - Monday after Easter, year 2079

        { 8,  4,   2080},   //  81 - Monday after Easter, year 2080
        {31,  3,   2081},   //  82 - Monday after Easter, year 2081
        {20,  4,   2082},   //  83 - Monday after Easter, year 2082
        { 5,  4,   2083},   //  84 - Monday after Easter, year 2083
        {27,  3,   2084},   //  85 - Monday after Easter, year 2084
        {16,  4,   2085},   //  86 - Monday after Easter, year 2085
        { 1,  4,   2086},   //  87 - Monday after Easter, year 2086
        {21,  4,   2087},   //  88 - Monday after Easter, year 2087
        {12,  4,   2088},   //  89 - Monday after Easter, year 2088
        { 4,  4,   2089},   //  90 - Monday after Easter, year 2089

        {17,  4,   2090},   //  91 - Monday after Easter, year 2090
        { 9,  4,   2091},   //  92 - Monday after Easter, year 2091
        {31,  3,   2092},   //  93 - Monday after Easter, year 2092
        {13,  4,   2093},   //  94 - Monday after Easter, year 2093
        { 5,  4,   2094},   //  95 - Monday after Easter, year 2094
        {25,  4,   2095},   //  96 - Monday after Easter, year 2095
        {16,  4,   2096},   //  97 - Monday after Easter, year 2096
        { 1,  4,   2097},   //  98 - Monday after Easter, year 2097
        {21,  4,   2098},   //  99 - Monday after Easter, year 2098
        {13,  4,   2099},   // 100 - Monday after Easter, year 2099
    }
};


/* debug string */
static       ascii Clock_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




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


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* AT response callback functions */
static time_t Clock_RtcConvSecTime(ql_rtc_time_t *rtc_time);




/*===========================================================================
 * Function   : Clock_SetTime
 *
 * Description: set the time (internal clock --> RTC)
 * Input      : - ptr_time: pointer to the time to be set
 * Output     : - FALSE: time not set (bad values)
 *              - TRUE : time     set
 *===========================================================================*/
bool Clock_SetTime(CLOCK__TIME *ptr_time)
{
    ql_rtc_time_t    new_rtc_time;
    ql_errcode_rtc_e res_rtc;


    if (
         /* date */
         ((ptr_time->year   <  2000) || (ptr_time->year   >  2099)) ||
         ((ptr_time->month  ==    0) || (ptr_time->month  >    12)) ||
         ((ptr_time->day    ==    0) || (ptr_time->day    >    31)) ||

         /* time */
         (                              (ptr_time->hour   >    59)) ||
         (                              (ptr_time->minute >    59)) ||
         (                              (ptr_time->second >    59))
       )
    {
        return FALSE;
    }


    /* date */
    new_rtc_time.tm_year =  ptr_time->year;                                       // year            (four digits)
    new_rtc_time.tm_mon  =  ptr_time->month;                                      // month           (1-12)
    new_rtc_time.tm_mday =  ptr_time->day;                                        // day             (1-31)

    /* time */
    new_rtc_time.tm_hour =  ptr_time->hour;                                       // hour            (0-23)
    new_rtc_time.tm_min  =  ptr_time->minute;                                     // minute          (0-59)
    new_rtc_time.tm_sec  =  ptr_time->second;                                     // second          (0-59)

    new_rtc_time.tm_wday = (ptr_time->week_day == 7) ? 0 : ptr_time->week_day;    // day of the week (0-6)


    /* set the RTC time (with ql_rtc_set_time function) */
    res_rtc = ql_rtc_set_time(&new_rtc_time);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CLOCK, DEBUG_TRACE_TYPE_LOW , "ql_rtc_set_time ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Clock_GetTime
 *
 * Description: get the time (internal clock --> RTC)
 * Input      : - ptr_time: pointer to save the time got
 * Output     : - FALSE: time not got (not available)
 *              - TRUE : time     got
 *===========================================================================*/
bool Clock_GetTime(CLOCK__TIME *ptr_time)
{
    ql_rtc_time_t    current_rtc_time;
    ql_errcode_rtc_e res_rtc;


    /* get the RTC time */
    res_rtc = ql_rtc_get_time(&current_rtc_time);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_CLOCK, DEBUG_TRACE_TYPE_LOW , "ql_rtc_get_time ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* date */
        ptr_time->year     = 2000;
        ptr_time->month    = 1;
        ptr_time->day      = 1;

        /* time */
        ptr_time->hour     = 0;
        ptr_time->minute   = 0;
        ptr_time->second   = 0;

        /* day of the week */
        ptr_time->week_day = 6;  // Saturday

        return FALSE;
    }


    /* date */
    ptr_time->year     =  current_rtc_time.tm_year;
    ptr_time->month    =  current_rtc_time.tm_mon;
    ptr_time->day      =  current_rtc_time.tm_mday;

    /* time */
    ptr_time->hour     =  current_rtc_time.tm_hour;
    ptr_time->minute   =  current_rtc_time.tm_min;
    ptr_time->second   =  current_rtc_time.tm_sec;

    /* day of the week */
    ptr_time->week_day = (current_rtc_time.tm_wday == 0) ? 7 : current_rtc_time.tm_wday;
  //ptr_time->week_day = Clock_WeekDayOfADay(current_rtc_time.tm_mday, current_rtc_time.tm_mon, current_rtc_time.tm_year);


    return TRUE;
}




/*===========================================================================
 * Function   : Clock_CompareRtcTimes
 *
 * Description: compare 2 RTC times
 * Input      : - ptr_time_1: pointer to RTC time 1 to be compared
 *            : - ptr_time_2: pointer to RTC time 2 to be compared
 * Output     : - -1: RTC time 1 is previous   to RTC time 2
 *              -  0: RTC time 1 is equal      to RTC time 2
 *              - +1: RTC time 1 is subsequent to RTC time 2
 *===========================================================================*/
s8 Clock_CompareRtcTimes(CLOCK__TIME *ptr_time_1, CLOCK__TIME *ptr_time_2)
{
    ql_rtc_time_t rtc_time_1;
    ql_rtc_time_t rtc_time_2;

    time_t        rtc_time_stamp_1;
    time_t        rtc_time_stamp_2;


    /* RTC time 1 */

    rtc_time_1.tm_year =  ptr_time_1->year;
    rtc_time_1.tm_mon  =  ptr_time_1->month;
    rtc_time_1.tm_mday =  ptr_time_1->day;

    rtc_time_1.tm_hour =  ptr_time_1->hour;
    rtc_time_1.tm_min  =  ptr_time_1->minute;
    rtc_time_1.tm_sec  =  ptr_time_1->second;

    rtc_time_1.tm_wday = (ptr_time_1->week_day == 7) ? 0 : ptr_time_1->week_day;


    /* RTC time 2 */

    rtc_time_2.tm_year =  ptr_time_2->year;
    rtc_time_2.tm_mon  =  ptr_time_2->month;
    rtc_time_2.tm_mday =  ptr_time_2->day;

    rtc_time_2.tm_hour =  ptr_time_2->hour;
    rtc_time_2.tm_min  =  ptr_time_2->minute;
    rtc_time_2.tm_sec  =  ptr_time_2->second;

    rtc_time_2.tm_wday = (ptr_time_2->week_day == 7) ? 0 : ptr_time_2->week_day;


    /* convert RTC time 1 to RTC time stamp 1 */
    rtc_time_stamp_1 = Clock_RtcConvSecTime(&rtc_time_1);

    /* convert RTC time 2 to RTC time stamp 2 */
    rtc_time_stamp_2 = Clock_RtcConvSecTime(&rtc_time_2);


    /* compare RTC time stamp 1 with the RTC time stamp 2 */
    if      (rtc_time_stamp_1 < rtc_time_stamp_2)
        return (-1);   // time 1 is previous   to time 2
    else if (rtc_time_stamp_1 > rtc_time_stamp_2)
        return (+1);   // time 1 is subsequent to time 2
    else
        return ( 0);   // time 1 is equal      to time 2    ('SecondFracPart' is zero for both)
}




/*===========================================================================
 * Function   : Clock_DifferenceRtcTimes
 *
 * Description: calculate the time difference (in seconds) between 2 RTC times
 * Input      : - ptr_time_1: pointer to RTC time 1
 *            : - ptr_time_2: pointer to RTC time 2
 * Output     : - time difference (in seconds)
 *                  - < 0: RTC time 1 is previous   to RTC time 2
 *                  - = 0: RTC time 1 is equal      to RTC time 2
 *                  - > 0: RTC time 1 is subsequent to RTC time 2
 *===========================================================================*/
s32 Clock_DifferenceRtcTimes(CLOCK__TIME *ptr_time_1, CLOCK__TIME *ptr_time_2)
{
    ql_rtc_time_t rtc_time_time_1;
    ql_rtc_time_t rtc_time_time_2;

    time_t        rtc_time_stamp_time_1;
    time_t        rtc_time_stamp_time_2;

    s32           diff_time_sec;


    /* build RTC time structs */

    rtc_time_time_1.tm_year =  ptr_time_1->year;
    rtc_time_time_1.tm_mon  =  ptr_time_1->month;
    rtc_time_time_1.tm_mday =  ptr_time_1->day;
    rtc_time_time_1.tm_wday = (ptr_time_1->week_day == 7) ? 0 : ptr_time_1->week_day;
    rtc_time_time_1.tm_hour =  ptr_time_1->hour;
    rtc_time_time_1.tm_min  =  ptr_time_1->minute;
    rtc_time_time_1.tm_sec  =  ptr_time_1->second;

    rtc_time_time_2.tm_year =  ptr_time_2->year;
    rtc_time_time_2.tm_mon  =  ptr_time_2->month;
    rtc_time_time_2.tm_mday =  ptr_time_2->day;
    rtc_time_time_2.tm_wday = (ptr_time_2->week_day == 7) ? 0 : ptr_time_2->week_day;
    rtc_time_time_2.tm_hour =  ptr_time_2->hour;
    rtc_time_time_2.tm_min  =  ptr_time_2->minute;
    rtc_time_time_2.tm_sec  =  ptr_time_2->second;


    /* convert RTC time to RTC time stamp */
    rtc_time_stamp_time_1 = Clock_RtcConvSecTime(&rtc_time_time_1);
    rtc_time_stamp_time_2 = Clock_RtcConvSecTime(&rtc_time_time_2);


    /* calculate the difference time */
    if (rtc_time_stamp_time_1 >= rtc_time_stamp_time_2)
        diff_time_sec = ( (s32)(rtc_time_stamp_time_1 - rtc_time_stamp_time_2));
    else
        diff_time_sec = (-(s32)(rtc_time_stamp_time_2 - rtc_time_stamp_time_1));


    return diff_time_sec;
}




/*===========================================================================
 * Function   : Clock_AddOffset
 *
 * Description: add a signed offset (sec) to a RTC time
 * Input      : - ptr_time       : pointer to RTC time
 *            : - ptr_time_offset: pointer to RTC time with the offset
 *            : - offset         : signed offset to be added (sec)
 * Output     : -
 *===========================================================================*/
void Clock_AddOffset(CLOCK__TIME *ptr_time, CLOCK__TIME *ptr_time_offset, s32 offset)
{
    ql_rtc_time_t rtc_time_time_1;
    struct tm     rtc_time_time_2;

    time_t        rtc_time_stamp_time;


    /* check if offset is equal to zero */
    if (offset == 0)
    {
        *ptr_time_offset = *ptr_time;
        return;
    }


    /* build RTC time structs */
    rtc_time_time_1.tm_year =  ptr_time->year;
    rtc_time_time_1.tm_mon  =  ptr_time->month;
    rtc_time_time_1.tm_mday =  ptr_time->day;
    rtc_time_time_1.tm_wday = (ptr_time->week_day == 7) ? 0 : ptr_time->week_day;
    rtc_time_time_1.tm_hour =  ptr_time->hour;
    rtc_time_time_1.tm_min  =  ptr_time->minute;
    rtc_time_time_1.tm_sec  =  ptr_time->second;


    /* convert RTC time to RTC time stamp */
    rtc_time_stamp_time = Clock_RtcConvSecTime(&rtc_time_time_1);


    /* add the offset */
    if (offset > 0)
        rtc_time_stamp_time += (u32)    (offset);
    else
        rtc_time_stamp_time -= (u32)labs(offset);


    /* convert RTC time stamp to RTC time */
    (void)gmtime_r((const time_t *)&rtc_time_stamp_time, &rtc_time_time_2);


    /* save the time with the offset */
    ptr_time_offset->year     =  rtc_time_time_2.tm_year + 1900;
    ptr_time_offset->month    =  rtc_time_time_2.tm_mon  + 1;
    ptr_time_offset->day      =  rtc_time_time_2.tm_mday;
    ptr_time_offset->week_day = (rtc_time_time_2.tm_wday == 0) ? 7 : rtc_time_time_2.tm_wday;
    ptr_time_offset->hour     =  rtc_time_time_2.tm_hour;
    ptr_time_offset->minute   =  rtc_time_time_2.tm_min;
    ptr_time_offset->second   =  rtc_time_time_2.tm_sec;
}




/*===========================================================================
 * Function   : Clock_NearestRtcTime
 *
 * Description: calculate the nearest RTC time depending on
 *                - the month mask
 *                - the week  mask
 *                - the time of the day
 * Input      : - ptr_actual_rtc_time : pointer to      actual      RTC time
 *            : - ptr_nearest_rtc_time: pointer to save the nearest RTC time
 *              - month_mask          : binary mask for month days for ...
 *              - week_mask           : binary mask for week  days for ...
 *              - hour_time           : hour    of time of the day for ... [0-23]
 *              - min_time            : minutes of time of the day for ... [0-59]
 *              - offset_sec          : fixed offset to be added (sec)     [0-86400]  (not used)
 * Output     : - FALSE: no nearest RTC time (no flagged days)
 *              - TRUE :    nearest RTC time calculated
 *===========================================================================*/
bool Clock_NearestRtcTime(CLOCK__TIME *ptr_actual_rtc_time,
                          CLOCK__TIME *ptr_nearest_rtc_time,
                          u32          month_mask,
                          u8           week_mask,
                          u8           hour_time,
                          u8           min_time,
                          u32          offset_sec)
{
    /* previous day */
    u16         previous_day__year;
    u8          previous_day__month;
    u8          previous_day__day;
    u8          previous_day__week_day;

    /* next     day */
    u16         next_day__year;
    u8          next_day__month;
    u8          next_day__day;
    u8          next_day__week_day;


    /* next time without offset - date */
    u16         next_time_no_offset__year;
    u8          next_time_no_offset__month;
    u8          next_time_no_offset__day;

    /* next time without offset - time */
    u8          next_time_no_offset__hour;
    u8          next_time_no_offset__minute;
    u8          next_time_no_offset__second;


    /* previous time without offset - date */
    u16         previous_time_no_offset__year;
    u8          previous_time_no_offset__month;
    u8          previous_time_no_offset__day;

    /* previous time without offset - time */
    u8          previous_time_no_offset__hour;
    u8          previous_time_no_offset__minute;
    u8          previous_time_no_offset__second;


    CLOCK__TIME rtc_next_time_no_offset;
    CLOCK__TIME rtc_previous_time_no_offset;

    CLOCK__TIME rtc_next_time_with_offset;
    CLOCK__TIME rtc_previous_time_with_offset;


    s8          result;
    u8          i;


    /*-----------------------------------------------------------------------
     * Verify if month mask and week mask are with all flag bits cleared
     *-----------------------------------------------------------------------*/
    if (
         (!(month_mask & 0x7FFFFFFF)) &&
         (!(week_mask  & 0x7F      ))
       )
    {
        return FALSE;   // month mask and week mask are with all flag bits cleared
    }


    /*-----------------------------------------------------------------------
     * Look for the next time (without offset)
     *-----------------------------------------------------------------------*/
    /* verify if the next time without offset stays in today */
    if (
         /* verify if today is a flagged day */
         (
           ((month_mask >> (ptr_actual_rtc_time->day      - 1)) & 0x00000001) ||
           ((week_mask  >> (ptr_actual_rtc_time->week_day - 1)) & 0x01      )
         )
         &&
         /* verify if time of the day is not yet elapsed */
         (
           (
             (ptr_actual_rtc_time->hour   <  hour_time)      // actual hour lower
           )
              ||
           (
             (ptr_actual_rtc_time->hour   == hour_time) &&   // same hour and
             (ptr_actual_rtc_time->minute <  min_time )      // actual minutes lower
           )
         )
       )
    {
        /*
         * today is a flagged day
         *   AND
         * time of the day not yet elapsed
         */

        /* next time without offset - date: today */
        next_time_no_offset__year     = ptr_actual_rtc_time->year;
        next_time_no_offset__month    = ptr_actual_rtc_time->month;
        next_time_no_offset__day      = ptr_actual_rtc_time->day;

        /* next time without offset - time: time of the day configured */
        next_time_no_offset__hour     = hour_time;
        next_time_no_offset__minute   = min_time;
        next_time_no_offset__second   = 0;
    }
    else
    {
        /*
         * today is not a flagged day
         *   OR
         * time of the day yet elapsed
         */

        /* check the successive days for 2 months (2x31 days) */

        next_day__year     = ptr_actual_rtc_time->year;
        next_day__month    = ptr_actual_rtc_time->month;
        next_day__day      = ptr_actual_rtc_time->day;
        next_day__week_day = ptr_actual_rtc_time->week_day;

        for (i = 0; i < (2 * 31); i++)
        {
            previous_day__year     = next_day__year;
            previous_day__month    = next_day__month;
            previous_day__day      = next_day__day;
            previous_day__week_day = next_day__week_day;

            /* calculate next day */
            Clock_TomorrowDay    ( previous_day__year,  previous_day__month,  previous_day__day,
                                   &next_day__year    , &next_day__month    , &next_day__day);
            Clock_TomorrowWeekDay( previous_day__week_day,
                                   &next_day__week_day);


            /* verify if next day is a flagged day */
            if (
                 ((month_mask >> (next_day__day      - 1)) & 0x00000001) ||
                 ((week_mask  >> (next_day__week_day - 1)) & 0x01      )
               )
            {
                /* next day is a flagged day */

                /* next time without offset - date */
                next_time_no_offset__year     = next_day__year;
                next_time_no_offset__month    = next_day__month;
                next_time_no_offset__day      = next_day__day;

                /* next time without offset - time: time of the day configured */
                next_time_no_offset__hour     = hour_time;
                next_time_no_offset__minute   = min_time;
                next_time_no_offset__second   = 0;

                break;
            }
        }

        if (i == (2 * 31))
            return FALSE;   // no flagged days in the 2 successive months (2x31 days)
    }


    /*-----------------------------------------------------------------------
     * Look for the previous time (without offset)
     *-----------------------------------------------------------------------*/
    /* verify if the previous time without offset stays in today */
    if (
         /* verify if today is a flagged day */
         (
           ((month_mask >> (ptr_actual_rtc_time->day      - 1)) & 0x00000001) ||
           ((week_mask  >> (ptr_actual_rtc_time->week_day - 1)) & 0x01      )
         )
         &&
         /* verify if time of the day is already elapsed */
         (
           (
             (ptr_actual_rtc_time->hour   >  hour_time)      // actual hour higher
           )
              ||
           (
             (ptr_actual_rtc_time->hour   == hour_time) &&   // same hour and
             (ptr_actual_rtc_time->minute >= min_time )      // actual minutes higher or equal
           )
         )
       )
    {
        /*
         * today is a flagged day
         *   AND
         * time of the day already elapsed
         */

        /* previous time without offset - date: today */
        previous_time_no_offset__year     = ptr_actual_rtc_time->year;
        previous_time_no_offset__month    = ptr_actual_rtc_time->month;
        previous_time_no_offset__day      = ptr_actual_rtc_time->day;

        /* previous time without offset - time: time of the day configured */
        previous_time_no_offset__hour     = hour_time;
        previous_time_no_offset__minute   = min_time;
        previous_time_no_offset__second   = 0;
    }
    else
    {
        /*
         * today is not a flagged day
         *   OR
         * time of the day already elapsed
         */

        /* check the previous days for 2 months (2x31 days) */

        previous_day__year     = ptr_actual_rtc_time->year;
        previous_day__month    = ptr_actual_rtc_time->month;
        previous_day__day      = ptr_actual_rtc_time->day;
        previous_day__week_day = ptr_actual_rtc_time->week_day;

        for (i = 0; i < (2 * 31); i++)
        {
            next_day__year     = previous_day__year;
            next_day__month    = previous_day__month;
            next_day__day      = previous_day__day;
            next_day__week_day = previous_day__week_day;

            /* calculate previous day */
            Clock_YesterdayDay    ( next_day__year    ,  next_day__month    ,  next_day__day,
                                   &previous_day__year, &previous_day__month, &previous_day__day);
            Clock_YesterdayWeekDay( next_day__week_day,
                                   &previous_day__week_day);


            /* verify if previous day is a flagged day */
            if (
                 ((month_mask >> (previous_day__day      - 1)) & 0x00000001) ||
                 ((week_mask  >> (previous_day__week_day - 1)) & 0x01      )
               )
            {
                /* previous day is a flagged day */

                /* previous time without offset - date */
                previous_time_no_offset__year     = previous_day__year;
                previous_time_no_offset__month    = previous_day__month;
                previous_time_no_offset__day      = previous_day__day;

                /* previous time without offset - time: time of the day configured */
                previous_time_no_offset__hour     = hour_time;
                previous_time_no_offset__minute   = min_time;
                previous_time_no_offset__second   = 0;

                break;
            }
        }

        if (i == (2 * 31))
            return FALSE;   // no flagged days in the 2 previous months (2x31 days)
    }


    /*-----------------------------------------------------------------------
     * Add the offset
     *-----------------------------------------------------------------------*/
    rtc_next_time_no_offset.year         = next_time_no_offset__year;
    rtc_next_time_no_offset.month        = next_time_no_offset__month;
    rtc_next_time_no_offset.day          = next_time_no_offset__day;
    rtc_next_time_no_offset.hour         = next_time_no_offset__hour;
    rtc_next_time_no_offset.minute       = next_time_no_offset__minute;
    rtc_next_time_no_offset.second       = next_time_no_offset__second;
    rtc_next_time_no_offset.week_day     = Clock_WeekDayOfADay(next_time_no_offset__day    , next_time_no_offset__month    , next_time_no_offset__year    );

    rtc_previous_time_no_offset.year     = previous_time_no_offset__year;
    rtc_previous_time_no_offset.month    = previous_time_no_offset__month;
    rtc_previous_time_no_offset.day      = previous_time_no_offset__day;
    rtc_previous_time_no_offset.hour     = previous_time_no_offset__hour;
    rtc_previous_time_no_offset.minute   = previous_time_no_offset__minute;
    rtc_previous_time_no_offset.second   = previous_time_no_offset__second;
    rtc_previous_time_no_offset.week_day = Clock_WeekDayOfADay(previous_time_no_offset__day, previous_time_no_offset__month, previous_time_no_offset__year);

    if (offset_sec > 0)
    {
        Clock_AddOffset(&rtc_next_time_no_offset    , &rtc_next_time_with_offset    , (s32)offset_sec);
        Clock_AddOffset(&rtc_previous_time_no_offset, &rtc_previous_time_with_offset, (s32)offset_sec);
    }


    /*-----------------------------------------------------------------------
     * Determine and save the result
     *-----------------------------------------------------------------------*/
    if (offset_sec == 0)
    {
        /* offset to be added     equal to zero */

        *ptr_nearest_rtc_time = rtc_next_time_no_offset;
    }
    else
    {
        /* offset to be added not equal to zero */

        result = Clock_CompareRtcTimes(ptr_actual_rtc_time, &rtc_previous_time_with_offset);
        if (result == -1)
        {
            /* "actual time" is     previous to "previous time without offset" */
            *ptr_nearest_rtc_time = rtc_previous_time_with_offset;
        }
        else
        {
            /* "actual time" is not previous to "previous time without offset" */
            *ptr_nearest_rtc_time = rtc_next_time_with_offset;
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Clock_NextSummerTimeChange
 *
 * Description: determine next 'summer time' change RTC time
 *              'Summer time' active between (Italian time):
 *                 - 02:00:00 of last sunday of March
 *                 - 03:00:00 of last sunday of October
 * Input      : - ptr_actual_rtc_time     : pointer to      actual                    RTC time
 *              - ptr_user_wakeup_rtc_time: pointer to save next 'summer time' change RTC time
 * Output     : -
 *===========================================================================*/
void Clock_NextSummerTimeChange(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_summer_time_change_rtc_time)
{
    u8   day;
    u8   month;
    u16  year;

    u8   week_day;

    u8   hour;

    u8   day_of_last_sunday_march;
    u8   day_of_last_sunday_october;
    u8   day_of_last_sunday_march_next_year;

    bool last_sunday_march;
    bool last_sunday_october;
    bool last_sunday_march_next_year;


    day      = ptr_actual_rtc_time->day;
    month    = ptr_actual_rtc_time->month;
    year     = ptr_actual_rtc_time->year;

    week_day = ptr_actual_rtc_time->week_day;

    hour     = ptr_actual_rtc_time->hour;


    last_sunday_march           = FALSE;
    last_sunday_october         = FALSE;
    last_sunday_march_next_year = FALSE;


    /* calculus of the day of last sunday of March   of this year */
    week_day = Clock_WeekDayOfADay(31,  3, year   );
    if (week_day == 7)
        day_of_last_sunday_march           = 31;
    else
        day_of_last_sunday_march           = 31 - week_day;

    /* calculus of the day of last sunday of October of this year */
    week_day = Clock_WeekDayOfADay(31, 10, year   );
    if (week_day == 7)
        day_of_last_sunday_october         = 31;
    else
        day_of_last_sunday_october         = 31 - week_day;

    /* calculus of the day of last sunday of March   of next year */
    week_day = Clock_WeekDayOfADay(31, 3, year + 1);
    if (week_day == 7)
        day_of_last_sunday_march_next_year = 31;
    else
        day_of_last_sunday_march_next_year = 31 - week_day;


    /* comparation of the actual month with relationship to March and October */

    /*-------------------------------------------------------------------
     * The actual month is before March
     *-------------------------------------------------------------------*/
    if      (month < 3)
    {
        /* the actual month is before March */

        /* last sunday of March of this year */
        last_sunday_march = TRUE;
    }


    /*-------------------------------------------------------------------
     * The actual month is after October
     *-------------------------------------------------------------------*/
    else if (month > 10)
    {
        /* the actual month is after October */

        /* last sunday of March of next year */
        last_sunday_march_next_year = TRUE;
    }


    /*-------------------------------------------------------------------
     * The actual month is between March and October
     *-------------------------------------------------------------------*/
    else if ((month > 3) && (month < 10))
    {
        /* the actual month is between March and October */

        /* last sunday of October of this year */
        last_sunday_october = TRUE;
    }


    /*-------------------------------------------------------------------
     * The actual month is March
     *-------------------------------------------------------------------*/
    else if  (month == 3)
    {
        /* the actual month is March */

        /* comparation of the actual day with relationship to last sunday of the month (March) */
        if      (day < day_of_last_sunday_march)
        {
            /* the actual day is before last sunday of the month (March) */

            /* last sunday of March of this year */
            last_sunday_march   = TRUE;
        }
        else if (day > day_of_last_sunday_march)
        {
            /* the actual day is after  last sunday of the month (March) */

            /* last sunday of October of this year */
            last_sunday_october = TRUE;
        }
        else
        {
            /* the actual day is        last sunday of the month (March) */

            /* comparation of the actual time with relationship to "02:00:00" */
            if (hour < 2)   // 02:00:00
            {
                /* the actual time is before "02:00:00" */

                /* last sunday of March of this year */
                last_sunday_march   = TRUE;
            }
            else
            {
                /* the actual time is after  "02:00:00" */

                /* last sunday of October of this year */
                last_sunday_october = TRUE;
            }
        }
    }


    /*-------------------------------------------------------------------
     * The actual month is October
     *-------------------------------------------------------------------*/
    else if  (month == 10)
    {
        /* the actual month is October */

        /* comparation of the actual day with relationship to last sunday of the month (October) */
        if      (day < day_of_last_sunday_october)
        {
            /* the actual day is before last sunday of the month (October) */

            /* last sunday of October of this year */
            last_sunday_october         = TRUE;
        }
        else if (day > day_of_last_sunday_october)
        {
            /* the actual day is after  last sunday of the month (October) */

            /* last sunday of March of next year */
            last_sunday_march_next_year = TRUE;
        }
        else
        {
            /* the actual day is        last sunday of the month (October) */

            /* comparation of the actual time with relationship to "03:00:00" */
            if (hour < 3)   // 03:00:00
            {
                /* the actual time is before "03:00:00" */

                /* last sunday of October of this year */
                last_sunday_october         = TRUE;
            }
            else
            {
                /* the actual time is after  "03:00:00" */

                /* last sunday of March of next year */
                last_sunday_march_next_year = TRUE;
            }
        }
    }


    if (last_sunday_march)
    {
        day   = day_of_last_sunday_march;
        month = 3;
      //year  = year;

        hour  = 2;  // 02:00:00
    }

    if (last_sunday_october)
    {
        day   = day_of_last_sunday_october;
        month = 10;
      //year  = year;

        hour  = 3;  // 03:00:00
    }

    if (last_sunday_march_next_year)
    {
        day   = day_of_last_sunday_march_next_year;
        month = 3;
        year++;

        hour  = 2;  // 02:00:00
    }


    /* save next 'summer time' change RTC time */

    ptr_summer_time_change_rtc_time->second   = 0;
    ptr_summer_time_change_rtc_time->minute   = 0;
    ptr_summer_time_change_rtc_time->hour     = hour;

    ptr_summer_time_change_rtc_time->day      = day;
    ptr_summer_time_change_rtc_time->month    = month;
    ptr_summer_time_change_rtc_time->year     = year;

    ptr_summer_time_change_rtc_time->week_day = Clock_WeekDayOfADay(day, month, year);
}




/*===========================================================================
 * Function   : Clock_IsSummerTime
 *
 * Description: verify if a RTC time stays in 'summer time'
 *              'Summer time' active between (Italian time):
 *                 - 02:00:00 of last sunday of March
 *                 - 03:00:00 of last sunday of October
 * Input      : - ptr_rtc_time: pointer to RTC time
 * Output     : - FALSE: RTC time does not stay in summer time
 *              - TRUE : RTC time stays         in summer time
 *===========================================================================*/
bool Clock_IsSummerTime(CLOCK__TIME *ptr_rtc_time)
{
    CLOCK__TIME summer_time_change_rtc_time;


    Clock_NextSummerTimeChange(ptr_rtc_time, &summer_time_change_rtc_time);

    if (summer_time_change_rtc_time.month == 10)  // next change in October
        return TRUE;    // Summer time
    else
        return FALSE;   // Winter time
}




/*===========================================================================
 * Function   : Clock_CompareTimes
 *
 * Description: compare 2 times
 * Input      : - hour_1  : hour   of time 1
 *              - minute_1: minute of time 1
 *              - second_1: second of time 1
 *              - hour_2  : hour   of time 2
 *              - minute_2: minute of time 2
 *              - second_2: second of time 2
 * Output     : - -1: time 1 is previous   to time 2
 *              -  0: time 1 is equal      to time 2
 *              - +1: time 1 is subsequent to time 2
 *===========================================================================*/
s8 Clock_CompareTimes(u8 hour_1, u8 minute_1, u8 second_1,
                      u8 hour_2, u8 minute_2, u8 second_2)
{
    if (hour_1 < hour_2)
        return (-1);

    if (hour_1 > hour_2)
        return (+1);


    if (minute_1 < minute_2)
        return (-1);

    if (minute_1 > minute_2)
        return (+1);


    if (second_1 < second_2)
        return (-1);

    if (second_1 > second_2)
        return (+1);


    return (0);
}




/*===========================================================================
 * Function   : Clock_TomorrowDay
 *
 * Description: calculate tomorrow day
 * Input      : - today_year        :                 today    - year
 *              - today_month       :                 today    - month
 *              - today_day         :                 today    - day
 *              - ptr_tomorrow_year : pointer to save tomorrow - year
 *              - ptr_tomorrow_month: pointer to save tomorrow - month
 *              - ptr_tomorrow_day  : pointer to save tomorrow - day
 * Output     : -
 *===========================================================================*/
void Clock_TomorrowDay(u16  today_year       , u8  today_month       , u8  today_day,
                       u16 *ptr_tomorrow_year, u8 *ptr_tomorrow_month, u8 *ptr_tomorrow_day)
{
    u16 tomorrow_year;
    u8  tomorrow_month;
    u8  tomorrow_day;

    u8  last_day;


    /* calculate last day of the month */
    if (today_month <= 12)
    {
        if (today_year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
            last_day = (u8)Clock_DaysInMonthsForNormalYear[today_month];  // normal year
        else
            last_day = (u8)Clock_DaysInMonthsForLeapYear  [today_month];  // leap   year
    }
    else
    {
        last_day = 31;
    }


    tomorrow_year  = today_year;
    tomorrow_month = today_month;
    tomorrow_day   = today_day;


    /* increase the day */
    tomorrow_day++;
    if (tomorrow_day > last_day)
    {
        /* increase the month */
        tomorrow_month++;
        tomorrow_day   = 1;
        if (tomorrow_month > 12)
        {
            /* increase the year */
            tomorrow_year++;
            tomorrow_month = 1;
        }
    }


    /* save the result */
    *ptr_tomorrow_year  = tomorrow_year;
    *ptr_tomorrow_month = tomorrow_month;
    *ptr_tomorrow_day   = tomorrow_day;
}




/*===========================================================================
 * Function   : Clock_TomorrowWeekDay
 *
 * Description: calculate tomorrow week day
 * Input      : - today_week_day       :                 today    - week day
 *              - ptr_tomorrow_week_day: pointer to save tomorrow - week day
 * Output     : -
 *===========================================================================*/
void Clock_TomorrowWeekDay(u8  today_week_day,
                       u8 *ptr_tomorrow_week_day)
{
    if (today_week_day < 7)
    {
        /* today is not Sunday */
        *ptr_tomorrow_week_day = today_week_day + 1;
    }
    else
    {
        /* today is     Sunday */
        *ptr_tomorrow_week_day = 1;
    }
}




/*===========================================================================
 * Function   : Clock_YesterdayDay
 *
 * Description: calculate yesterday day
 * Input      : - today_year         :                 today     - year
 *              - today_month        :                 today     - month
 *              - today_day          :                 today     - day
 *              - ptr_yesterday_year : pointer to save yesterday - year
 *              - ptr_yesterday_month: pointer to save yesterday - month
 *              - ptr_yesterday_month: pointer to save yesterday - day
 * Output     : -
 *===========================================================================*/
void Clock_YesterdayDay(u16  today_year        , u8  today_month        , u8  today_day,
                        u16 *ptr_yesterday_year, u8 *ptr_yesterday_month, u8 *ptr_yesterday_day)
{
    u16 yesterday_year;
    u8  yesterday_month;
    u8  yesterday_day;

    u8  last_day;


    /* calculate last day of the previous month */
    if (today_month <= 12)
    {
        if (today_month > 1)
        {
            /* actual month is not January */
            if (today_year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
                last_day = (u8)Clock_DaysInMonthsForNormalYear[today_month - 1];  // normal year
            else
                last_day = (u8)Clock_DaysInMonthsForLeapYear  [today_month - 1];  // leap   year
        }
        else
        {
            /* actual month is January */
            last_day = 31;
        }
    }
    else
    {
        last_day = 31;
    }


    yesterday_year  = today_year;
    yesterday_month = today_month;
    yesterday_day   = today_day;


    if (yesterday_day > 1)
    {
        /* today is not day 1st */

        /* decrease the day */
        yesterday_day--;
    }
    else
    {
        /* today is day 1st */

        yesterday_day = last_day;

        if (yesterday_month > 1)
        {
            /* today is not January */

            /* decrease the month */
            yesterday_month--;
        }
        else
        {
            /* today is January */

            yesterday_month = 12;

            /* decrease the year */
            yesterday_year--;
        }
    }


    /* save the result */
    *ptr_yesterday_year  = yesterday_year;
    *ptr_yesterday_month = yesterday_month;
    *ptr_yesterday_day   = yesterday_day;
}




/*===========================================================================
 * Function   : Clock_YesterdayWeekDay
 *
 * Description: calculate yesterday week day
 * Input      : - today_week_day        :                 today     - week day
 *              - ptr_yesterday_week_day: pointer to save yesterday - week day
 * Output     : -
 *===========================================================================*/
void Clock_YesterdayWeekDay(u8   today_week_day,
                            u8  *ptr_yesterday_week_day)
{
    if (today_week_day > 1)
    {
        /* today is not Monday */
        *ptr_yesterday_week_day = today_week_day - 1;
    }
    else
    {
        /* today is     Monday */
        *ptr_yesterday_week_day = 7;
    }
}




/*===========================================================================
 * Function   : Clock_NextDay
 *
 * Description: calculate next day
 * Input      : - today_year    :                 today    - year
 *              - today_month   :                 today    - month
 *              - today_day     :                 today    - day
 *              - days_offset   : number of days offset [0-7]
 *              - ptr_next_year : pointer to save next day - year
 *              - ptr_next_month: pointer to save next day - month
 *              - ptr_next_day  : pointer to save next day - day
 * Output     : -
 *===========================================================================*/
void Clock_NextDay(u16 today_year, u8 today_month, u8 today_day, u8 days_offset, u16 *ptr_next_year, u8 *ptr_next_month, u8 *ptr_next_day)
{
    u16 next_year;
    u8  next_month;
    u8  next_day;

    u8  last_day;


    /* verify "days_offset" value */
    if (days_offset > 7)
        days_offset = 7;


    /* calculate last day of the month */
    if (today_month <= 12)
    {
        if (today_year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
            last_day = (u8)Clock_DaysInMonthsForNormalYear[today_month];  // normal year
        else
            last_day = (u8)Clock_DaysInMonthsForLeapYear  [today_month];  // leap   year
    }
    else
    {
        last_day = 31;
    }


    next_year  = today_year;
    next_month = today_month;
    next_day   = today_day;


    /* increase the day */
    next_day += days_offset;
    if (next_day > last_day)
    {
        /* increase the month */
        next_month++;
        next_day = next_day - last_day;
        if (next_month > 12)
        {
            /* increase the year */
            next_year++;
            next_month = 1;
        }
    }


    /* save the result */
    *ptr_next_year  = next_year;
    *ptr_next_month = next_month;
    *ptr_next_day   = next_day;
}




/*===========================================================================
 * Function   : Clock_NextWeekDay
 *
 * Description: calculate next week day
 * Input      : - today_week_day   :                 today    - week day
 *              - days_offset      : number of days offset [0-7]
 *              - ptr_next_week_day: pointer to save next day - week day
 * Output     : -
 *===========================================================================*/
void Clock_NextWeekDay(u8 today_week_day, u8 days_offset, u8 *ptr_next_week_day)
{
    u8 next_week_day;


    /* verify "days_offset" value */
    if (days_offset > 7)
        days_offset = 7;


    /* calculate next day */
    next_week_day = (today_week_day + days_offset) % 7;
    if (next_week_day == 0)
        next_week_day = 7;


    /* save the result */
    *ptr_next_week_day = next_week_day;
}




/*===========================================================================
 * Function   : Clock_PreviousDay
 *
 * Description: calculate previous day
 * Input      : - today_year        :                 today        - year
 *              - today_month       :                 today        - month
 *              - today_day         :                 today        - day
 *              - days_offset       : number of days offset [0-7]
 *              - ptr_previous_year : pointer to save previous day - year
 *              - ptr_previous_month: pointer to save previous day - month
 *              - ptr_previous_month: pointer to save previous day - day
 * Output     : -
 *===========================================================================*/
void Clock_PreviousDay(u16 today_year, u8 today_month, u8 today_day, u8 days_offset, u16 *ptr_previous_year, u8 *ptr_previous_month, u8 *ptr_previous_day)
{
    u16 previous_year;
    u8  previous_month;
    u8  previous_day;

    u8  last_day;


    /* verify "days_offset" value */
    if (days_offset > 7)
        days_offset = 7;


    /* calculate last day of the previous month */
    if (today_month <= 12)
    {
        if (today_month > 1)
        {
            /* actual month is not January */
            if (today_year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
                last_day = (u8)Clock_DaysInMonthsForNormalYear[today_month - 1];  // normal year
            else
                last_day = (u8)Clock_DaysInMonthsForLeapYear  [today_month - 1];  // leap   year
        }
        else
        {
            /* actual month is January */
            last_day = 31;
        }
    }
    else
    {
        last_day = 31;
    }


    previous_year  = today_year;
    previous_month = today_month;
    previous_day   = today_day;


    if (previous_day > days_offset)
    {
        /* today is     bigger than than the number of day offset */

        /* decrease the day */
        previous_day -= days_offset;
    }
    else
    {
        /* today is not bigger than than the number of day offset */

        previous_day = last_day - (days_offset - previous_day);

        if (previous_month > 1)
        {
            /* today is not January */

            /* decrease the month */
            previous_month--;
        }
        else
        {
            /* today is January */

            previous_month = 12;

            /* decrease the year */
            previous_year--;
        }
    }


    /* save the result */
    *ptr_previous_year  = previous_year;
    *ptr_previous_month = previous_month;
    *ptr_previous_day   = previous_day;
}




/*===========================================================================
 * Function   : Clock_PreviousWeekDay
 *
 * Description: calculate previous week day
 * Input      : - today_week_day       :                 today        - week day
 *              - days_offset          : number of days offset [0-7]
 *              - ptr_previous_week_day: pointer to save previous day - week day
 * Output     : -
 *===========================================================================*/
void Clock_PreviousWeekDay(u8 today_week_day, u8 days_offset, u8 *ptr_previous_week_day)
{
    u8 previous_week_day;



    /* verify "days_offset" value */
    if (days_offset > 7)
        days_offset = 7;


    /* calculate previous day */
    previous_week_day = (7 + today_week_day - days_offset) % 7;
    if (previous_week_day == 0)
        previous_week_day = 7;


    /* save the result */
    *ptr_previous_week_day = previous_week_day;
}




/*===========================================================================
 * Function   : Clock_DaysInAMonth
 *
 * Description: return the number of days in a month
 * Input      : - month: month
 *              - year : year
 * Output     : -
 *===========================================================================*/
u8 Clock_DaysInAMonth(u8 month, u16 year)
{
    u8 last_day;


    if (year < 2000)
        return 31;

    if ((month == 0) || (month > 12))
        return 31;


    if (year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
        last_day = (u8)Clock_DaysInMonthsForNormalYear[month];  // normal year
    else
        last_day = (u8)Clock_DaysInMonthsForLeapYear  [month];  // leap   year


    return last_day;
}




/*===========================================================================
 * Function   : Clock_WeekDayOfADay
 *
 * Description: return the weekday of a day
 * Input      : - day  : day
 *              - month: month
 *              - year : year
 * Output     : - weekday of a day (1-7)
 *===========================================================================*/
u8 Clock_WeekDayOfADay(u8 day, u8 month, u16 year)
{
    CLOCK__TIME time_1;
    CLOCK__TIME time_2;

    u8          last_day;

    s32         diff_time;
    u32         num_of_days;

    u8          week_day;


    if (year < 2000)
        return 1;

    if ((month == 0) || (month > 12))
        return 1;

    if (year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
        last_day = (u8)Clock_DaysInMonthsForNormalYear[month];  // normal year
    else
        last_day = (u8)Clock_DaysInMonthsForLeapYear  [month];  // leap   year
    if ((day   == 0) || (day > last_day))
        return 1;


    /* calculate the number of days of difference between 00:00:00 of the specified day and 00:00:00 of "Saturday 01/01/2000" */

    /* specified day, 00:00:00 */
    time_1.year     = year;
    time_1.month    = month;
    time_1.day      = day;
    time_1.hour     = 0;
    time_1.minute   = 0;
    time_1.second   = 0;
  //time_1.week_day = 0;

    /* Saturday 01/01/2000, 00:00:00 */
    time_2.year     = 2000;
    time_2.month    = 1;
    time_2.day      = 1;
    time_2.hour     = 0;
    time_2.minute   = 0;
    time_2.second   = 0;
  //time_2.week_day = 6;

    diff_time = Clock_DifferenceRtcTimes(&time_1, &time_2);  // in seconds

    num_of_days = (u32)diff_time / (24 * 60 * 60);


    /* calculate the weekday of the specified day */
    week_day = (u8)((6 + num_of_days) % 7);
    if (week_day == 0)
        week_day = 7;


    return week_day;
}




/*===========================================================================
 * Function   : Clock_WeekDayString
 *
 * Description: return the weekday string
 * Input      : - week_day: weekday (1-7)
 * Output     : - weekday string
 *===========================================================================*/
ascii *Clock_WeekDayString(u8 week_day)
{
                 LANGUAGE__CONFIG__LANGUAGE  language_configuration;
                 LANGUAGE__LANGUAGE          language;
                 ascii                      *weekday_string;
                 u16                         id_string;


    /* weekday strings */
    static const u16                         week_day_strings[7] =
    {
        STRINGS__WEEKDAY__MON,   // "Mon"
        STRINGS__WEEKDAY__TUE,   // "Tue"
        STRINGS__WEEKDAY__WED,   // "Wed"
        STRINGS__WEEKDAY__THU,   // "Thu"
        STRINGS__WEEKDAY__FRI,   // "Fri"
        STRINGS__WEEKDAY__SAT,   // "Sat"
        STRINGS__WEEKDAY__SUN,   // "Sun"
    };


    /* verify weekday */
    if ((week_day == 0) || (week_day > 7))
    {
        weekday_string = "???";
        return weekday_string;
    }


    /* get language configuration */
    Language_Config_Language_Get(&language_configuration);

    language = language_configuration.language;

    /* verify language configuration */
    if (language >= LANGUAGE__NUMBERS_OF_LANGUAGES)
        language = LANGUAGE__LANGUAGE_ENGLISH;


    /* extract the weekday string */
    id_string      = week_day_strings[week_day - 1];
    weekday_string = Strings_GetString(id_string);

    return weekday_string;
}




/*===========================================================================
 * Function   : Clock_IsFestivityDay
 *
 * Description: check if a specified day is a festivity day
 * Input      : - day  : day
 *              - month: month
 *              - year : year
 * Output     : - FALSE; the specified day is not a festivity day
 *              - TRUE ; the specified day is     a festivity day
 *===========================================================================*/
bool Clock_IsFestivityDay(u8 day, u8 month, u16 year)
{
    bool result;
    u8   i;


    /* check if the specified day is a valid date */
    result = Clock_IsValidDate(day, month, year);
    if (!result)
        return FALSE;


    /* check festivity days (with fixed    day in the year) */
    for (i = 0; i < MAX_NUM_OF_FESTIVITY_DAYS_FIXED;    i++)
    {
        if (
             (day   == Clock_FestivityDaysFixedInTheYear.festivity_days   [i].day  ) &&
             (month == Clock_FestivityDaysFixedInTheYear.festivity_days   [i].month)
           )
        {
            return TRUE;
        }
    }


    /* check festivity days (with variable day in the year) */
    for (i = 0; i < MAX_NUM_OF_FESTIVITY_DAYS_VARIABLE; i++)
    {
        if (
             (day   == Clock_FestivityDaysVariableInTheYear.festivity_days[i].day  ) &&
             (month == Clock_FestivityDaysVariableInTheYear.festivity_days[i].month) &&
             (year  == Clock_FestivityDaysVariableInTheYear.festivity_days[i].year )
           )
        {
            return TRUE;
        }
    }


    return FALSE;
}




/*===========================================================================
 * Function   : Clock_IsValidClockTime
 *
 * Description: verify if a specified clock time (time/date/weekday) is a
 *              valid clock time
 * Input      : - ptr_time: pointer to the time to be verified
 * Output     : - FALSE: the time is not a valid time
 *              - TRUE : the time is     a valid time
 *===========================================================================*/
bool Clock_IsValidClockTime(CLOCK__TIME *ptr_time)
{
    u8   week_day;
    bool result;


    /* check the time */
    result = Clock_IsValidTime    (ptr_time->hour, ptr_time->minute, ptr_time->second);
    if (!result)
        return FALSE;

    /* check the date */
    result = Clock_IsValidDate    (ptr_time->day , ptr_time->month , ptr_time->year  );
    if (!result)
        return FALSE;

    /* check the weekday */
    week_day = Clock_WeekDayOfADay(ptr_time->day , ptr_time->month , ptr_time->year  );
    if (week_day != ptr_time->week_day)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Clock_IsValidTime
 *
 * Description: verify if a specified time is a valid time
 * Input      : - hour  : hour   of the time to be verified
 *              - minute: minute of the time to be verified
 *              - second: second of the time to be verified
 * Output     : - FALSE: the time is not a valid time
 *              - TRUE : the time is     a valid time
 *===========================================================================*/
bool Clock_IsValidTime(u8 hour, u8 minute, u8 second)
{
    if (hour   >= 24)
        return FALSE;

    if (minute >= 60)
        return FALSE;

    if (second >= 60)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : Clock_IsValidDate
 *
 * Description: verify if a specified date is a valid date
 * Input      : - day  : day   of the date to be verified
 *              - month: month of the date to be verified
 *              - year : year  of the date to be verified
 * Output     : - FALSE: the date is not a valid date
 *              - TRUE : the date is     a valid date
 *===========================================================================*/
bool Clock_IsValidDate(u8 day, u8 month, u16 year)
{
    u8 last_day;


    if ((year  <  2000) || (year  > 2099))
        return FALSE;

    if ((month == 0   ) || (month > 12  ))
        return FALSE;


    if (year % 4)  // the years of the beginning of the centuries are not managed (it works fine until 2099)
        last_day = (u8)Clock_DaysInMonthsForNormalYear[month];  // normal year
    else
        last_day = (u8)Clock_DaysInMonthsForLeapYear  [month];  // leap   year


    if ((day   == 0   ) || (day   > last_day))
        return FALSE;


    return TRUE;
}




/*=============================================================================
 * Function   : Clock_PrintTime
 *
 * Description: Print the RTC time indicated
 * Input      : - trace_level: trace level
 *              - id         : identification
 *              - delay      : final delay (ms)
 *              - year       : year   (2000-2099)
 *              - month      : month  (1-12)
 *              - day        : day    (1-31)
 *              - hour       : hour   (0-23)
 *              - minute     : minute (0-59)
 *              - second     : second (0-59)
 * Output     : -
 *=============================================================================*/
void Clock_PrintTime1(u8 trace_level, u8 id, u32 delay, u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second)
{
    snprintf(Clock_DebugString,
             sizeof(Clock_DebugString),
             "Time[%d]:  %02d/%02d/%04d %02d:%02d:%02d",
             id,
             day,
             month,
             year,
             hour,
             minute,
             second);

    Debug_SendDebugString1(trace_level, DEBUG_TRACE_TYPE_LOW, Clock_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, delay);
}




/*=============================================================================
 * Function   : Clock_PrintTime2
 *
 * Description: Print the RTC time indicated
 * Input      : - trace_level: trace level
 *              - id         : identification
 *              - delay      : final delay (ms)
 *              - ptr_time   : pounter to time to be printed
 * Output     : -
 *=============================================================================*/
void Clock_PrintTime2(u8 trace_level, u8 id, u32 delay, CLOCK__TIME *ptr_time)
{
    snprintf(Clock_DebugString,
             sizeof(Clock_DebugString),
             "Time[%d]:  %.3s %02d/%02d/%04d %02d:%02d:%02d",
             id,
             Clock_WeekDayString(ptr_time->week_day),
             ptr_time->day,
             ptr_time->month,
             ptr_time->year,
             ptr_time->hour,
             ptr_time->minute,
             ptr_time->second);

    Debug_SendDebugString1(trace_level, DEBUG_TRACE_TYPE_LOW, Clock_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, delay);
}




/*=============================================================================
 * Function   : Clock_PrintTime3
 *
 * Description: Print the RTC time indicated
 * Input      : - trace_level  : trace level
 *              - id           : identification
 *              - delay        : final delay (ms)
 *              - ptr_timestamp: pounter to time to be printed
 * Output     : -
 *=============================================================================*/
void Clock_PrintTime3(u8 trace_level, u8 id, u32 delay, time_t *ptr_timestamp)
{
    struct tm tm_time;


    /* convert RTC time stamp to RTC time */
    (void)gmtime_r(ptr_timestamp, &tm_time);

    snprintf(Clock_DebugString,
             sizeof(Clock_DebugString),
             "Time[%d]:  %.3s %02d/%02d/%04d %02d:%02d:%02d",
             id,
             Clock_WeekDayString((tm_time.tm_wday == 0) ? 7 : tm_time.tm_wday),
             tm_time.tm_mday,
             tm_time.tm_mon + 1,
             tm_time.tm_year + 1900,
             tm_time.tm_hour,
             tm_time.tm_min,
             tm_time.tm_sec);

    Debug_SendDebugString1(trace_level, DEBUG_TRACE_TYPE_LOW, Clock_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, delay);
}




/*=============================================================================
 * Function   : Clock_ConverTimeString1ToClockTime
 *
 * Description: convert a time string (format: "Tue, 13 Jan 2015 15:42:26 GMT")
 *              to a clock time
 * Input      : - time_string: time string to be converted
 *              - ptr_time   : pointer to save the converted time
 * Output     : - FALSE: time string not converted (string not valid)
 *              - TRUE : time string     converted
 *=============================================================================*/
bool Clock_ConverTimeString1ToClockTime(ascii *time_string, CLOCK__TIME *ptr_time)
{
    static const ascii      *month_strings   [12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static const ascii      *week_day_strings[ 7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

                 CLOCK__TIME time;

                 ascii       month_string   [3 + 1];
                 ascii       week_day_string[3 + 1];

                 u16         year;
                 u16         month;
                 u16         day;

                 u16         hour;
                 u16         minute;
                 u16         second;

                 u16         week_day;

                 int         num_parameters;
                 bool        result;
                 u8          i;


    // "Tue, 13 Jan 2015 15:42:26 GMT"

    month_string   [0] = 0x00;
    week_day_string[0] = 0x00;

    /* extract the values from the time string */
    num_parameters = sscanf(time_string, "%3s, %2hd %3s %4hd %2hd:%2hd:%2hd GMT", week_day_string, &day, month_string, &year, &hour, &minute, &second);
    if (num_parameters != 7)
        return FALSE;


    /* convert month string to month index */
    for (i = 0; i < 12; i++)
    {
        if (!strncmp(month_strings   [i], month_string   , 3))
            break;
    }
    if (i < 12)
        month = i + 1;
    else
        return FALSE;


    /* convert week day string to week day index */
    for (i = 0; i <  7; i++)
    {
        if (!strncmp(week_day_strings[i], week_day_string, 3))
            break;
    }
    if (i <  7)
        week_day = i + 1;   // 1: Monday, ..., 7: Sunday
    else
        return FALSE;


    /* save the converted time */
    time.year     =     year;
    time.month    = (u8)month;
    time.day      = (u8)day;
    time.hour     = (u8)hour;
    time.minute   = (u8)minute;
    time.second   = (u8)second;
    time.week_day = (u8)week_day;


    /* check the converted time */
    result = Clock_IsValidClockTime(&time);
    if (!result)
        return FALSE;


    /* save the converted time */
    *ptr_time = time;

    return TRUE;
}




/*=============================================================================
 * Function   : Clock_ConverTimeString2ToClockTime
 *
 * Description: convert a time string (format: "Tue, 13 Jan 2015 15:42:26 +0100")
 *              to a clock time
 * Input      : - time_string: time string to be converted
 *              - ptr_time   : pointer to save the converted time
 * Output     : - FALSE: time string not converted (string not valid)
 *              - TRUE : time string     converted
 *=============================================================================*/
bool Clock_ConverTimeString2ToClockTime(ascii *time_string, CLOCK__TIME *ptr_time)
{
    static const ascii      *month_strings   [12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    static const ascii      *week_day_strings[ 7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

                 CLOCK__TIME time;

                 ascii       month_string   [3 + 1];
                 ascii       week_day_string[3 + 1];

                 u16         year;
                 u16         month;
                 u16         day;

                 u16         hour;
                 u16         minute;
                 u16         second;

                 u16         week_day;

                 ascii       time_zone_sign;
                 u16         time_zone_offset;

                 int         num_parameters;
                 bool        result;
                 u8          i;


    // "Tue, 13 Jan 2015 15:42:26 +0100"

    month_string   [0] = 0x00;
    week_day_string[0] = 0x00;

    /* extract the values from the time string */
    num_parameters = sscanf(time_string, "%3s, %2hd %3s %4hd %2hd:%2hd:%2hd %c%4hd", week_day_string, &day, month_string, &year, &hour, &minute, &second, &time_zone_sign, &time_zone_offset);
    if (num_parameters != 9)
        return FALSE;


    /* check time zone sign character */
    if (
         (time_zone_sign != '+') &&
         (time_zone_sign != '-')
       )
    {
        return FALSE;
    }


    /* convert month string to month index */
    for (i = 0; i < 12; i++)
    {
        if (!strncmp(month_strings   [i], month_string   , 3))
            break;
    }
    if (i < 12)
        month = i + 1;
    else
        return FALSE;


    /* convert week day string to week day index */
    for (i = 0; i <  7; i++)
    {
        if (!strncmp(week_day_strings[i], week_day_string, 3))
            break;
    }
    if (i <  7)
        week_day = i + 1;   // 1: Monday, ..., 7: Sunday
    else
        return FALSE;


    /* save the converted time */
    time.year     =     year;
    time.month    = (u8)month;
    time.day      = (u8)day;
    time.hour     = (u8)hour;
    time.minute   = (u8)minute;
    time.second   = (u8)second;
    time.week_day = (u8)week_day;


    /* check the converted time */
    result = Clock_IsValidClockTime(&time);
    if (!result)
        return FALSE;


    /* save the converted time */
    *ptr_time = time;

    return TRUE;
}




/*=============================================================================
 * Function   : Clock_ConverTimeString3ToClockTime
 *
 * Description: convert a time string (format: "15/01/13 15:42:26 +01 0")
 *              to a clock time
 * Input      : - time_string: time string to be converted
 *              - ptr_time   : pointer to save the converted time
 * Output     : - FALSE: time string not converted (string not valid)
 *              - TRUE : time string     converted
 *=============================================================================*/
bool Clock_ConverTimeString3ToClockTime(ascii *time_string, time_t *ptr_unix_time)
{
          ascii         *token;
          ascii         *remainder;

          long           temp_value[7];
    const ascii          delimiter[] = "// ::  ";

          ql_rtc_time_t  time;
          time_t         time_stamp;

          long           offset;
          u8             i;


    // "15/01/13 15:42:26 +01 0"

    /* extract the values from the time string */
    token = time_string;
    for (i = 0; i < 7; i++)
    {
        if (token == NULL)
            return FALSE;

        temp_value[i] = strtol(token, &remainder, 10);
        if (*remainder != delimiter[i])
            return FALSE;

        token = remainder + 1;
    }

    /* save the converted time */
    time.tm_year = temp_value[0] + 2000;
    time.tm_mon  = temp_value[1];
    time.tm_mday = temp_value[2];
    time.tm_hour = temp_value[3];
    time.tm_min  = temp_value[4];
    time.tm_sec  = temp_value[5];
    time.tm_wday = 0;


    /* convert RTC time to RTC time stamp */
    time_stamp = Clock_RtcConvSecTime(&time);


    /* check if offset is equal to zero */
    offset = temp_value[6] * 900;
    if (offset == 0)
    {
        *ptr_unix_time = time_stamp;
        return TRUE;
    }

    /* add the offset */
    if (offset > 0)
        time_stamp +=     (offset);
    else
        time_stamp -= labs(offset);


    /* save the converted time */
    *ptr_unix_time = time_stamp;
    return TRUE;
}




/*=============================================================================
 * Function   : Clock_ConverTimeString4ToClockTime
 *
 * Description: convert a time string (format: "2015.06.26_08.00.00")
 *              to a unix time
 * Input      : - time_string: time string to be converted
 * Output     : - converted unix time
 *=============================================================================*/
time_t Clock_ConverTimeString4ToClockTime(ascii *time_string)
{
    u16           year;
    u8            month;
    u8            day;

    u8            hour;
    u8            minute;
    u8            second;

    ql_rtc_time_t time;
    time_t        time_stamp;


    // "UFS:EASY__L__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
    // "UFS:EASY__A__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
    //               |
    //               13

    /* extract the time from the file name */
    year     = ((u16)(time_string[13] - '0') * 1000) +
               ((u16)(time_string[14] - '0') *  100) +
               ((u16)(time_string[15] - '0') *   10) +
               ((u16)(time_string[16] - '0') *    1);
    month    = (     (time_string[18] - '0') *   10) +
               (     (time_string[19] - '0') *    1);
    day      = (     (time_string[21] - '0') *   10) +
               (     (time_string[22] - '0') *    1);
    hour     = (     (time_string[24] - '0') *   10) +
               (     (time_string[25] - '0') *    1);
    minute   = (     (time_string[27] - '0') *   10) +
               (     (time_string[28] - '0') *    1);
    second   = (     (time_string[30] - '0') *   10) +
               (     (time_string[31] - '0') *    1);

    /* save the converted time */
    time.tm_year = year;
    time.tm_mon  = month;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min  = minute;
    time.tm_sec  = second;
    time.tm_wday = 0;

    /* convert RTC time to RTC time stamp */
    time_stamp = Clock_RtcConvSecTime(&time);

    return time_stamp;
}




/*===========================================================================
 * Function   : Clock_RtcBackupTime_GetDefault
 *
 * Description: get the default RTC backup time
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Clock_RtcBackupTime_GetDefault(CLOCK__RTC_BACKUP_TIME *ptr_data)
{
    *ptr_data = Clock_RtcBackupTimeDefault;
}




/*===========================================================================
 * Function   : Clock_RtcBackupTime_Get
 *
 * Description: get the RTC backup time
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Clock_RtcBackupTime_Get(CLOCK__RTC_BACKUP_TIME *ptr_data)
{
    *ptr_data = Clock_RtcBackupTime;
}




/*===========================================================================
 * Function   : Clock_RtcBackupTime_Set
 *
 * Description: set the RTC backup time
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Clock_RtcBackupTime_Set(CLOCK__RTC_BACKUP_TIME *ptr_data)
{
    Clock_RtcBackupTime = *ptr_data;
}




/*===========================================================================
 * Function   : Clock_RtcConvSecTime
 *
 * Description: convert time to arithmetic representation
 * Input      : - rtc_time: pointer to time represented as ql_rtc_time_t
 * Output     : - time converted to a time_t value
 *===========================================================================*/
static time_t Clock_RtcConvSecTime(ql_rtc_time_t *rtc_time)
{
    int mon = rtc_time->tm_mon;
    int year = rtc_time->tm_year;
    int64_t days, hours;
    time_t tim;


    mon -= 2;
    if (0 >= (int)mon) 
    {    
        // 1..12 -> 11,12,1..10
        mon += 12;    
        
        //Puts Feb last since it has leap day
        year -= 1;
    }
    days = (unsigned long)(year / 4 - year / 100 + year / 400 + 367 * mon / 12 + rtc_time->tm_mday) + year * 365 - 719499;

    hours = (days * 24) + rtc_time->tm_hour;

    tim = (hours * 60 + rtc_time->tm_min) * 60 + rtc_time->tm_sec;

    return tim;
}
