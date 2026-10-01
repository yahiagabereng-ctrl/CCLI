/*=============================================================================
 * File       :  USER_GSM_RETX.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - user GSM - retransmission
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
#include "transmission_gprs.h"
#include "transmission_sms.h"
#include "typedef.h"
#include "user_gsm_retx.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* number of retransmissions */
#define NUMBER_OF_RETX            3

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING   200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* random configuration - "retx" */
static       USER_GSM_RETX__RANDOM_CONFIG__RETX UserGsmRetx_RandomConfig_Retx;
static const USER_GSM_RETX__RANDOM_CONFIG__RETX UserGsmRetx_RandomConfig_RetxDefault =
{
    FALSE,                 // random values generated
    0                      // random time offset (sec)
};


/* retransmission status */
static       USER_GSM_RETX__RETX_STATUS         UserGsmRetx_RetxStatus;
static const USER_GSM_RETX__RETX_STATUS         UserGsmRetx_RetxStatusDefault =
{
    FALSE,                 // status
    0                      // counter
};


/* debug string */
static       ascii                              UserGsmRetx_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "retransmission" random configuration */
       void UserGsmRetx_RandomConfig_Retx_GetDefault   (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
       void UserGsmRetx_RandomConfig_Retx_Get          (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
       void UserGsmRetx_RandomConfig_Retx_Set          (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
       bool UserGsmRetx_RandomConfig_Retx_IsValid      (USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data);
       void UserGsmRetx_RandomConfig_Retx_Generate     (void);
       bool UserGsmRetx_RandomConfig_Retx_IsGenerated  (void);

/* get/set retransmission status */
       void UserGsmRetx_RetransmissionStatus_GetDefault(USER_GSM_RETX__RETX_STATUS         *ptr_data);
       void UserGsmRetx_RetransmissionStatus_Get       (USER_GSM_RETX__RETX_STATUS         *ptr_data);
       void UserGsmRetx_RetransmissionStatus_Set       (USER_GSM_RETX__RETX_STATUS         *ptr_data);

/* the nearest user wakeup time */
       bool UserGsmRetx_NextWakeupTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time);

/* udpate data */
       void UserGsmRetx_UpdateData(void);

/* verify retransmission to be done */
static bool UserGsmRetx_RetransmissionToBeDone(void);

/* update retransmission status */
static void UserGsmRetx_UpdateRetransmissionStatus(void);

/* reset retransmission status */
       void UserGsmRetx_ResetRetransmissionStatus(void);

/* debug info */
       void UserGsmRetx_PrintRetransmissionStatus(void);




/*===========================================================================
 * Function   : UserGsmRetx_RandomConfig_Retx_GetDefault
 *
 * Description: get the default "retransmission" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RandomConfig_Retx_GetDefault(USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data)
{
    *ptr_data = UserGsmRetx_RandomConfig_RetxDefault;
}




/*===========================================================================
 * Function   : UserGsmRetx_RandomConfig_Retx_Get
 *
 * Description: get the "retransmission" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RandomConfig_Retx_Get(USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data)
{
    *ptr_data = UserGsmRetx_RandomConfig_Retx;
}




/*===========================================================================
 * Function   : UserGsmRetx_RandomConfig_Retx_Set
 *
 * Description: set the "retransmission" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RandomConfig_Retx_Set(USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data)
{
    UserGsmRetx_RandomConfig_Retx = *ptr_data;
}




/*===========================================================================
 * Function   : UserGsmRetx_RandomConfig_Retx_IsValid
 *
 * Description: check if the "retransmission" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool UserGsmRetx_RandomConfig_Retx_IsValid(USER_GSM_RETX__RANDOM_CONFIG__RETX *ptr_data)
{
    if (ptr_data->offset > (1 * 60 * 60))  // 1h (=3600s)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : UserGsmRetx_RandomConfig_Retx_Generate
 *
 * Description: generate the "retransmission" random configuration
 * Input      : -
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RandomConfig_Retx_Generate(void)
{
    u32 random_offset_sec;       /* random time offset (sec) */
    u32 random_offset_size_sec;


    /* generate a random time offset (sec) */
    random_offset_size_sec = (1 * 60 * 60) + 1;    // 1h (=3600s)
    random_offset_sec      = Utility_RandomInteger(0, random_offset_size_sec);
    if (random_offset_sec > (random_offset_size_sec - 1))
        random_offset_sec = (random_offset_size_sec - 1);

    UserGsmRetx_RandomConfig_Retx.values_generated = TRUE;               // random values generated
    UserGsmRetx_RandomConfig_Retx.offset           = random_offset_sec;  // random time offset (sec)

    snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "RETRANSMISSION - Random time offset generated: %ld", random_offset_sec);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__006_RANDOM_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__006_RANDOM_CONFIGURATIONS__RETRANSMISSION);
}




/*===========================================================================
 * Function   : UserGsmRetx_RandomConfig_Retx_IsGenerated
 *
 * Description: verify if the "retransmission" random configuration is
 *              already generated
 * Input      : -
 * Output     : - FALSE: random configuration not generated
 *              - TRUE : random configuration     generated
 *===========================================================================*/
bool UserGsmRetx_RandomConfig_Retx_IsGenerated(void)
{
    return UserGsmRetx_RandomConfig_Retx.values_generated;
}




/*===========================================================================
 * Function   : UserGsmRetx_RetransmissionStatus_GetDefault
 *
 * Description: get the default retransmission status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RetransmissionStatus_GetDefault(USER_GSM_RETX__RETX_STATUS *ptr_data)
{
    *ptr_data = UserGsmRetx_RetxStatusDefault;
}




/*===========================================================================
 * Function   : UserGsmRetx_RetransmissionStatus_Get
 *
 * Description: get the retransmission status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RetransmissionStatus_Get(USER_GSM_RETX__RETX_STATUS *ptr_data)
{
    *ptr_data = UserGsmRetx_RetxStatus;
}




/*===========================================================================
 * Function   : UserGsmRetx_RetransmissionStatus_Set
 *
 * Description: set the retransmission status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_RetransmissionStatus_Set(USER_GSM_RETX__RETX_STATUS *ptr_data)
{
    UserGsmRetx_RetxStatus = *ptr_data;
}




/*===========================================================================
 * Function   : UserGsmRetx_NextWakeupTime
 *
 * Description: determine the nearest user wakeup RTC time
 *              (03:00:00, 09:00:00, 15:00:00, 21:00:00)
 * Input      : - ptr_actual_rtc_time     : pointer to      actual                  RTC time
 *              - ptr_user_wakeup_rtc_time: pointer to save the nearest user wakeup RTC time
 * Output     : - FALSE: the nearest user wakeup RTC time not available
 *              - TRUE : the nearest user wakeup RTC time     available
 *===========================================================================*/
bool UserGsmRetx_NextWakeupTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time)
{
    /* next     time without offset */
    u16         next_time_no_offset__year;
    u8          next_time_no_offset__month;
    u8          next_time_no_offset__day;
    u8          next_time_no_offset__hour;
    u8          next_time_no_offset__minute;
    u8          next_time_no_offset__second;

    /* previous time without offset */
    u16         previous_time_no_offset__year;
    u8          previous_time_no_offset__month;
    u8          previous_time_no_offset__day;
    u8          previous_time_no_offset__hour;
    u8          previous_time_no_offset__minute;
    u8          previous_time_no_offset__second;

    u8          last_day;

    u32         random_offset_sec;       /* random time offset (sec) */
    u32         random_offset_size_sec;

    CLOCK__TIME rtc_next_time_no_offset;
    CLOCK__TIME rtc_previous_time_no_offset;

    CLOCK__TIME rtc_next_time_with_offset;
    CLOCK__TIME rtc_previous_time_with_offset;

    bool        result_bool;
    s8          result_s8;


    /* verify if a retransmission has to be done */
    result_bool = UserGsmRetx_RetransmissionToBeDone();
    if (!result_bool)
        return FALSE;  // a retransmission has not to be done


    /*-----------------------------------------------------------------------
     * Generate a random time offset (sec)
     *-----------------------------------------------------------------------*/
    if (UserGsmRetx_RandomConfig_Retx.values_generated)
    {
        random_offset_size_sec = 60 * 60;    // 1 hour
        random_offset_sec      = UserGsmRetx_RandomConfig_Retx.offset;
        if (random_offset_sec > (random_offset_size_sec - 1))
            random_offset_sec = (random_offset_size_sec - 1);
    }
    else
    {
        random_offset_sec = 0;
    }

    snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "Random time offset: %ld", random_offset_sec);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /*-----------------------------------------------------------------------
     * Calculate next time (without offset)
     *-----------------------------------------------------------------------*/

    next_time_no_offset__year   = ptr_actual_rtc_time->year;
    next_time_no_offset__month  = ptr_actual_rtc_time->month;
    next_time_no_offset__day    = ptr_actual_rtc_time->day;
    next_time_no_offset__hour   = ptr_actual_rtc_time->hour;
    next_time_no_offset__minute = 0;
    next_time_no_offset__second = 0;

    if      (next_time_no_offset__hour <  3)
    {
        next_time_no_offset__hour  = 3;
    }
    else if (next_time_no_offset__hour <  9)
    {
        next_time_no_offset__hour = 9;
    }
    else if (next_time_no_offset__hour < 15)
    {
        next_time_no_offset__hour = 15;
    }
    else if (next_time_no_offset__hour < 21)
    {
        next_time_no_offset__hour = 21;
    }
    else
    {
        next_time_no_offset__hour = 3;

        last_day = Clock_DaysInAMonth(next_time_no_offset__month, next_time_no_offset__year);

        next_time_no_offset__day++;
        if (next_time_no_offset__day > last_day)
        {
            next_time_no_offset__day = 1;
            next_time_no_offset__month++;
            if (next_time_no_offset__month > 12)
            {
                next_time_no_offset__month = 1;
                next_time_no_offset__year++;
            }
        }
    }


    /*-----------------------------------------------------------------------
     * Calculate previous time (without offset)
     *-----------------------------------------------------------------------*/

    previous_time_no_offset__year   = ptr_actual_rtc_time->year;
    previous_time_no_offset__month  = ptr_actual_rtc_time->month;
    previous_time_no_offset__day    = ptr_actual_rtc_time->day;
    previous_time_no_offset__hour   = ptr_actual_rtc_time->hour;
    previous_time_no_offset__minute = 0;
    previous_time_no_offset__second = 0;

    if      (previous_time_no_offset__hour <  3)
    {
        previous_time_no_offset__hour = 21;

        if (previous_time_no_offset__day > 1)
        {
            previous_time_no_offset__day--;
        }
        else
        {
            last_day = Clock_DaysInAMonth(previous_time_no_offset__month, previous_time_no_offset__year);

            previous_time_no_offset__day = last_day;
            if (previous_time_no_offset__month > 1)
            {
                previous_time_no_offset__month--;
            }
            else
            {
                previous_time_no_offset__month = 12;
                previous_time_no_offset__year--;
            }
        }
    }
    else if (previous_time_no_offset__hour <  9)
    {
        previous_time_no_offset__hour = 3;
    }
    else if (previous_time_no_offset__hour < 15)
    {
        previous_time_no_offset__hour = 9;
    }
    else if (previous_time_no_offset__hour < 21)
    {
        previous_time_no_offset__hour = 15;
    }
    else
    {
        previous_time_no_offset__hour = 21;
    }


    /*-----------------------------------------------------------------------
     * Add the random offset
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

    if (random_offset_sec > 0)
    {
        Clock_AddOffset(&rtc_next_time_no_offset    , &rtc_next_time_with_offset    , (s32)random_offset_sec);
        Clock_AddOffset(&rtc_previous_time_no_offset, &rtc_previous_time_with_offset, (s32)random_offset_sec);
    }

    /* print debug info */
    //Clock_PrintTime2(1, 50, &rtc_next_time_no_offset    );
    //Clock_PrintTime2(2, 50, &rtc_previous_time_no_offset);
    //if (random_offset_sec > 0)
    //{
    //    Clock_PrintTime2(3, 50, &rtc_next_time_with_offset    );
    //    Clock_PrintTime2(4, 50, &rtc_previous_time_with_offset);
    //}
    //Clock_PrintTime2(5, 50, ptr_actual_rtc_time);


    /*-----------------------------------------------------------------------
     * Determine and save the result
     *-----------------------------------------------------------------------*/
    if (random_offset_sec == 0)
    {
        /* offset to be added     equal to zero */

        *ptr_user_wakeup_rtc_time = rtc_next_time_no_offset;
    }
    else
    {
        /* offset to be added not equal to zero */

        result_s8 = Clock_CompareRtcTimes(ptr_actual_rtc_time, &rtc_previous_time_with_offset);
        if (result_s8 == -1)
        {
            /* "actual time" is     previous to "previous time without offset" */
            *ptr_user_wakeup_rtc_time = rtc_previous_time_with_offset;
        }
        else
        {
            /* "actual time" is not previous to "previous time without offset" */
            *ptr_user_wakeup_rtc_time = rtc_next_time_with_offset;
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : UserGsmRetx_UpdateData
 *
 * Description: update data
 * Input      : -
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_UpdateData(void)
{
    UserGsmRetx_UpdateRetransmissionStatus();
}




/*===========================================================================
 * Function   : UserGsmRetx_RetransmissionToBeDone
 *
 * Description: verify if a retransmission has to be done
 * Input      : -
 * Output     : - FALSE: retransmission has not to be done
 *              - TRUE : retransmission has     to be done
 *===========================================================================*/
static bool UserGsmRetx_RetransmissionToBeDone(void)
{
    bool result_1;
    bool result_2;


    snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "Retransmission status - status: %d; counter: %d", UserGsmRetx_RetxStatus.status, UserGsmRetx_RetxStatus.counter);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* verify if there are data to be transmitted */
    result_1 = TransmissionSms_ExistDataToBeTransmitted();
    result_2 = TransmissionGprs_ExistDataToBeTransmitted();
    if (!(result_1 || result_2))
    {
        /* there are not data to be transmitted */
        return FALSE;
    }
    else
    {
        /* there are     data to be transmitted */

        /* verify if a retransmission is in progress */
        if (!UserGsmRetx_RetxStatus.status)
        {
            /* a retransmission not in progress */
            return TRUE;
        }
        else
        {
            /* a retransmission     in progress */

            /* verify if the number of transmission windows is terminated */
            if (UserGsmRetx_RetxStatus.counter < NUMBER_OF_RETX)
            {
                /* number of transmission windows not terminated */
                return TRUE;
            }
            else
            {
                /* number of transmission windows     terminated */
                return FALSE;
            }
        }
    }
}




/*===========================================================================
 * Function   : UserGsmRetx_UpdateRetransmissionStatus
 *
 * Description: Updates the retransmission status info (stored in flash objects)
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void UserGsmRetx_UpdateRetransmissionStatus(void)
{
    bool result_1;
    bool result_2;


    snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "Old retransmission status - status: %d; counter: %d", UserGsmRetx_RetxStatus.status, UserGsmRetx_RetxStatus.counter);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* verify if there are data to be transmitted */
    result_1 = TransmissionSms_ExistDataToBeTransmitted();
    result_2 = TransmissionGprs_ExistDataToBeTransmitted();
    if (!(result_1 || result_2))
    {
        /* there are not data to be transmitted */

        UserGsmRetx_RetxStatus.status  = FALSE;
        UserGsmRetx_RetxStatus.counter = 0;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION, PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS);
    }
    else
    {
        /* there are     data to be transmitted */

        /* verify if the number of transmission windows is terminated */
        if (!UserGsmRetx_RetxStatus.status)
        {
            /* retransmission not in progress */

            UserGsmRetx_RetxStatus.status  = TRUE;
            UserGsmRetx_RetxStatus.counter = 0;

            /* request the backup to flash objects */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION, PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS);
        }
        else
        {
            /* retransmission     in progress */

            if (UserGsmRetx_RetxStatus.counter < NUMBER_OF_RETX)
            {
                /* number of transmission windows not terminated */

                UserGsmRetx_RetxStatus.status = TRUE;
                UserGsmRetx_RetxStatus.counter++;

                /* request the backup to flash objects */
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION, PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS);
            }
            else
            {
                /* number of transmission windows     terminated */

                // The retransmission status info are left inalterated (={TRUE, NUMBER_OF_RETX})
            }
        }
    }


    snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "New retransmission status - status: %d; counter: %d", UserGsmRetx_RetxStatus.status, UserGsmRetx_RetxStatus.counter);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}




/*===========================================================================
 * Function   : UserGsmRetx_ResetRetransmissionStatus
 *
 * Description: reset retransmission status
 * Input      : -
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_ResetRetransmissionStatus(void)
{
    UserGsmRetx_RetxStatus.status  = FALSE;
    UserGsmRetx_RetxStatus.counter = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__008_RETRANSMISSION, PROGRAM_FLASH__FLASH_ID__008_RETRANSMISSION__RETX_STATUS);
}




/*===========================================================================
 * Function   : UserGsmRetx_PrintRetransmissionStatus
 *
 * Description: print the retransmission status
 * Input      : -
 * Output     : -
 *===========================================================================*/
void UserGsmRetx_PrintRetransmissionStatus(void)
{
    if (UserGsmRetx_RetxStatus.status)
    {
        snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "Retransmission status - status: ENABLED; counter: %d" , UserGsmRetx_RetxStatus.counter);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 50);
    }
    else
    {
        snprintf(UserGsmRetx_DebugString, sizeof(UserGsmRetx_DebugString), "Retransmission status - status: DISABLED; counter: %d", UserGsmRetx_RetxStatus.counter);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_USER_RETX, DEBUG_TRACE_TYPE_LOW, UserGsmRetx_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 50);
    }
}
