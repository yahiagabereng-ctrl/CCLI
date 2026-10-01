/*=============================================================================
 * File       :  USER_GSM_SYNC.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - user GSM - time synchronize
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "clock.h"
#include "debug_my.h"
#include "program_flash.h"
#include "typedef.h"
#include "user_gsm_sync.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING      200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* random configuration - "synchronize" */
static       USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE UserGsmSync_RandomConfig_Synchronize;
static const USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE UserGsmSync_RandomConfig_SynchronizeDefault =
{
    FALSE,        // random values generated
    0             // random time offset (sec)
};


/* debug string */
static       ascii                                     UserGsmSync_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "synchronize" random configuration */
void UserGsmSync_RandomConfig_Synchronize_GetDefault (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
void UserGsmSync_RandomConfig_Synchronize_Get        (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
void UserGsmSync_RandomConfig_Synchronize_Set        (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
bool UserGsmSync_RandomConfig_Synchronize_IsValid    (USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data);
void UserGsmSync_RandomConfig_Synchronize_Generate   (void);
bool UserGsmSync_RandomConfig_Synchronize_IsGenerated(void);

/* the nearest user wakeup time */
bool UserGsmSync_NextWakeupTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time);

/* update data */
void UserGsmSync_UpdateData(void);




/*===========================================================================
 * Function   : UserGsmSync_RandomConfig_Synchronize_GetDefault
 *
 * Description: get the default "synchronize" random configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void UserGsmSync_RandomConfig_Synchronize_GetDefault(USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data)
{
    *ptr_data = UserGsmSync_RandomConfig_SynchronizeDefault;
}




/*===========================================================================
 * Function   : UserGsmSync_RandomConfig_Synchronize_Get
 *
 * Description: get the "synchronize" random configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void UserGsmSync_RandomConfig_Synchronize_Get(USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data)
{
    *ptr_data = UserGsmSync_RandomConfig_Synchronize;
}




/*===========================================================================
 * Function   : UserGsmSync_RandomConfig_Synchronize_Set
 *
 * Description: set the "synchronize" random configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void UserGsmSync_RandomConfig_Synchronize_Set(USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data)
{
    UserGsmSync_RandomConfig_Synchronize = *ptr_data;
}




/*===========================================================================
 * Function   : UserGsmSync_RandomConfig_Synchronize_IsValid
 *
 * Description: check if the "synchronize" random configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool UserGsmSync_RandomConfig_Synchronize_IsValid(USER_GSM_SYNC__RANDOM_CONFIG__SYNCHRONIZE *ptr_data)
{
    if (ptr_data->offset > (12 * 60 * 60))  // 12h (=43200s)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : UserGsmSync_RandomConfig_Synchronize_Generate
 *
 * Description: generate the "synchronize" random configuration
 * Input      : -
 * Output     : -
 *===========================================================================*/
void UserGsmSync_RandomConfig_Synchronize_Generate(void)
{
    u32 random_offset_sec;       /* random time offset (sec) */
    u32 random_offset_size_sec;


    /* generate a random time offset (sec) */
    random_offset_size_sec = (12 * 60 * 60) + 1;    // 12h (=43200s)
    random_offset_sec      = Utility_RandomInteger(0, random_offset_size_sec);
    if (random_offset_sec > (random_offset_size_sec - 1))
        random_offset_sec = (random_offset_size_sec - 1);

    UserGsmSync_RandomConfig_Synchronize.values_generated = TRUE;               // random values generated
    UserGsmSync_RandomConfig_Synchronize.offset           = random_offset_sec;  // random time offset (sec)

    snprintf(UserGsmSync_DebugString, sizeof(UserGsmSync_DebugString), "SYNCHRONIZE - Random time offset generated: %ld", random_offset_sec);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_SYNCHRONIZE, DEBUG_TRACE_TYPE_LOW, UserGsmSync_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__006_RANDOM_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__SYNCHRONIZE);
}




/*===========================================================================
 * Function   : UserGsmSync_RandomConfig_Synchronize_IsGenerated
 *
 * Description: verify if the "synchronize" random configuration is
 *              already generated
 * Input      : -
 * Output     : - FALSE: random configuration not generated
 *              - TRUE : random configuration     generated
 *===========================================================================*/
bool UserGsmSync_RandomConfig_Synchronize_IsGenerated(void)
{
    return UserGsmSync_RandomConfig_Synchronize.values_generated;
}




/*===========================================================================
 * Function   : UserGsmSync_NextWakeupTime
 *
 * Description: determine the nearest user wakeup RTC time
 *              It is the nearest between:
 *                 - 02:00:00 of the 1st day of a month
 *                 - 'Summer time' change time
 *                        'Summer time' is active beetween:
 *                        - 02:00:00 of last sunday of March
 *                        - 03:00:00 of last sunday of October
 * Input      : - ptr_actual_rtc_time     : pointer to      actual                  RTC time
 *              - ptr_user_wakeup_rtc_time: pointer to save the nearest user wakeup RTC time
 * Output     : - FALSE: the nearest user wakeup RTC time not available
 *              - TRUE : the nearest user wakeup RTC time     available
 *===========================================================================*/
bool UserGsmSync_NextWakeupTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time)
{
    CLOCK__TIME time_1;
    CLOCK__TIME time_2;
    CLOCK__TIME time;

    u8          hour;
    u8          day;
    u8          month;
    u16         year;

    s8          result;


    /*-------------------------------------------------------------------
     * 02:00:00 of the 1st day of the actual or next month
     *-------------------------------------------------------------------*/
    month = ptr_actual_rtc_time->month;
    year  = ptr_actual_rtc_time->year;
    day   = ptr_actual_rtc_time->day;
    hour  = ptr_actual_rtc_time->hour;

    if (
         ((day == 0)               ) ||  // day = 0  --->  it is not possible
         ((day == 1) && (hour >= 2)) ||
         ((day >  1)               )
       )
    {
        month++;
        if (month > 12)
        {
            month = 1;
            year++;
        }
    }

    time_1.second   = 0;
    time_1.minute   = 0;
    time_1.hour     = 2;

    time_1.day      = 1;
    time_1.month    = month;
    time_1.year     = year;

    time_1.week_day = Clock_WeekDayOfADay(1, month, year);


    /*-------------------------------------------------------------------
     * Nearest change of summer time
     *-------------------------------------------------------------------*/
    Clock_NextSummerTimeChange(ptr_actual_rtc_time, &time_2);


    /*-------------------------------------------------------------------
     * Compare time_1 and time_2
     *-------------------------------------------------------------------*/
    result = Clock_CompareRtcTimes(&time_1, &time_2);

    /* save the nearest user wakeup RTC time */
    if (result == -1)
        time = time_1;
    else
        time = time_2;


    *ptr_user_wakeup_rtc_time = time;

    return TRUE;
}




/*===========================================================================
 * Function   : UserGsmSync_UpdateData
 *
 * Description: update data
 * Input      : -
 * Output     : -
 *===========================================================================*/
void UserGsmSync_UpdateData(void)
{
    // NOTHING TO DO

    return;
}
