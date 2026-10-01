/*=============================================================================
 * File       :  COUNTERS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - counters manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "clock.h"
#include "counters.h"
#include "debug_my.h"
#include "fw_config.h"
#include "program_flash.h"
#include "rtc_alarm.h"
#include "startup.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * NOTES
 *===========================================================================*/
/*
 * AT+SMS="COUNTERBANDS C1 12345 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 1 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 2 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 3 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 4 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 5 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 6 S1 F3 00:00 S2 F2 07:00 S3 F3 23:00 S4 OFF S5 OFF S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 7 S1 F3 00:00 S2 OFF S3 OFF S4 OFF S5 OFF S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C1 F S1 F3 00:00 S2 OFF S3 OFF S4 OFF S5 OFF S6 OFF S7 OFF"
 *
 * AT+SMS="COUNTERBANDS C2 12345 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 1 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 2 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 3 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 4 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 5 S1 F3 00:00 S2 F2 07:00 S3 F1 08:00 S4 F2 19:00 S5 F3 23:00 S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 6 S1 F3 00:00 S2 F2 07:00 S3 F3 23:00 S4 OFF S5 OFF S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 7 S1 F3 00:00 S2 OFF S3 OFF S4 OFF S5 OFF S6 OFF S7 OFF"
 * AT+SMS="COUNTERBANDS C2 F S1 F3 00:00 S2 OFF S3 OFF S4 OFF S5 OFF S6 OFF S7 OFF"
 */




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* task message ID */
#define EVENT_COUNTER_C1_IMPULSE           (13100 | (QL_COMPONENT_APP_START << 16))  // counter C1 impulse
#define EVENT_COUNTER_C2_IMPULSE           (13101 | (QL_COMPONENT_APP_START << 16))  // counter C2 impulse
#define EVENT_TIMEOUT_COUNTERS_BACKUP      (13102 | (QL_COMPONENT_APP_START << 16))  // timeout counters backup
#define EVENT_MAIN_POWER_SUPPLY_PROBLEM    (13103 | (QL_COMPONENT_APP_START << 16))  // main power supply problem
#define EVENT_INPUT_C1_BAND                (13104 | (QL_COMPONENT_APP_START << 16))  // input C1 band
#define EVENT_INPUT_C2_BAND                (13105 | (QL_COMPONENT_APP_START << 16))  // input C2 band
#define EVENT_CLOCK_CHANGED                (13106 | (QL_COMPONENT_APP_START << 16))  // clock changed

/* time (ms) */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TIME_MS__COUNTERS_BACKUP_PERIOD    (20 * 60 * 1000L)
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TIME_MS__COUNTERS_BACKUP_PERIOD    ( 1 * 60 * 1000L)
#endif

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING            200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* C1 counters (total, F1, F2, F3) */
static       u32                              Counters_InputC1;     // = 0;
static       u32                              Counters_InputC1F1;   // = 0;
static       u32                              Counters_InputC1F2;   // = 0;
static       u32                              Counters_InputC1F3;   // = 0;

/* C2 counters (total, F1, F2, F3) */
static       u32                              Counters_InputC2;     // = 0;
static       u32                              Counters_InputC2F1;   // = 0;
static       u32                              Counters_InputC2F2;   // = 0;
static       u32                              Counters_InputC2F3;   // = 0;

/* CRC variables */
static       u16                              Counters_VarCrc;      // = 0x0000;

/* current C1/C2 counters band type */
static       u8                               Counters_InputC1BandType = COUNTERS__BAND_TYPE__F1;
static       u8                               Counters_InputC2BandType = COUNTERS__BAND_TYPE__F1;

/* debug string */
static       ascii                            Counters_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* status - input C1 */
static       COUNTERS__STATUS__INPUT_C1       Counters_Status_InputC1;
static const COUNTERS__STATUS__INPUT_C1       Counters_Status_InputC1Default =
{
    0,         /* counter C1            */

    0,         /* counter C1  - band F1 */
    0,         /* counter C1  - band F2 */
    0,         /* counter C1  - band F3 */
};


/* status - input C2 */
static       COUNTERS__STATUS__INPUT_C2       Counters_Status_InputC2;
static const COUNTERS__STATUS__INPUT_C2       Counters_Status_InputC2Default =
{
    0,         /* counter C2            */

    0,         /* counter C2  - band F1 */
    0,         /* counter C2  - band F2 */
    0,         /* counter C2  - band F3 */
};


/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - input C1 */
static       COUNTERS__CONFIG__INPUT_C1       Counters_Config_InputC1;
static const COUNTERS__CONFIG__INPUT_C1       Counters_Config_InputC1Default =
{
    FALSE,     /* input C1 enabled status */
};


/* configuration - input C2 */
static       COUNTERS__CONFIG__INPUT_C2       Counters_Config_InputC2;
static const COUNTERS__CONFIG__INPUT_C2       Counters_Config_InputC2Default =
{
    FALSE,     /* input C2 enabled status */
};


/* configuration - input C1 bands */
static       COUNTERS__CONFIG__INPUT_C1_BANDS Counters_Config_InputC1Bands;
static const COUNTERS__CONFIG__INPUT_C1_BANDS Counters_Config_InputC1BandsDefault =
{
    /*
     *  F1 (green ):
     *    - Mon Tue Wed Thu Fri                    08:00-19:00
     *
     *  F2 (yellow):
     *    - Mon Tue Wed Thu Fri                    07:00-08:00
     *    - Mon Tue Wed Thu Fri                    19:00-23:00
     *    -                     Sat                07:00-23:00
     *
     *  F3 (red   ):
     *    - Mon Tue Wed Thu Fri Sat                00:00-07:00
     *    - Mon Tue Wed Thu Fri Sat                23:00-24:00
     *    -                         Sun            00:00-24:00
     *    -                             Festivity  00:00-24:00
     */

    /*
     *  Mon Tue Wed Thu Fri:
     *    - 00:00-07:00:  F3    (green )
     *    - 07:00-08:00:  F2    (yellow)
     *    - 08:00-19:00:  F1    (red   )
     *    - 19:00-23:00:  F2    (yellow)
     *    - 23:00-24:00:  F3    (green )
     *
     *  Sat:
     *    - 00:00-07:00:  F3    (green )
     *    - 07:00-23:00:  F2    (yellow)
     *    - 23:00-24:00:  F3    (green )
     *
     *  Sun:
     *    - 00:00-24:00:  F3    (green )
     *
     *  Festivity:
     *    - 00:00-24:00:  F3    (green )
     */

    FALSE,     /* input C1 bands enabled status (for week      days) */
    FALSE,     /* input C1 bands enabled status (for festivity days) */

    /* input C1 bands time (for week      days) */
    { //   band 1               band 2               band 3               band 4               band 5               band 6               band 7
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Mon
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Tue
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Wed
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Thu
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Fri
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Sat
        {  {TRUE , 3,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Sun
    },

    /* input C1 bands time (for festivity days) */
      //   band 1               band 2               band 3               band 4               band 5               band 6               band 7
        {  {TRUE , 3,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // festivity day

};


/* configuration - input C2 bands */
static       COUNTERS__CONFIG__INPUT_C2_BANDS Counters_Config_InputC2Bands;
static const COUNTERS__CONFIG__INPUT_C2_BANDS Counters_Config_InputC2BandsDefault =
{
    /*
     *  F1 (green ):
     *    - Mon Tue Wed Thu Fri                    08:00-19:00
     *
     *  F2 (yellow):
     *    - Mon Tue Wed Thu Fri                    07:00-08:00
     *    - Mon Tue Wed Thu Fri                    19:00-23:00
     *    -                     Sat                07:00-23:00
     *
     *  F3 (red   ):
     *    - Mon Tue Wed Thu Fri Sat                00:00-07:00
     *    - Mon Tue Wed Thu Fri Sat                23:00-24:00
     *    -                         Sun            00:00-24:00
     *    -                             Festivity  00:00-24:00
     */

    /*
     *  Mon Tue Wed Thu Fri:
     *    - 00:00-07:00:  F3    (green )
     *    - 07:00-08:00:  F2    (yellow)
     *    - 08:00-19:00:  F1    (red   )
     *    - 19:00-23:00:  F2    (yellow)
     *    - 23:00-24:00:  F3    (green )
     *
     *  Sat:
     *    - 00:00-07:00:  F3    (green )
     *    - 07:00-23:00:  F2    (yellow)
     *    - 23:00-24:00:  F3    (green )
     *
     *  Sun:
     *    - 00:00-24:00:  F3    (green )
     *
     *  Festivity:
     *    - 00:00-24:00:  F3    (green )
     */

    FALSE,     /* input C2 bands enabled status (for week      days) */
    FALSE,     /* input C2 bands enabled status (for festivity days) */

    /* input C2 bands time */
    { //   band 1               band 2               band 3               band 4               band 5               band 6               band 7
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Mon
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Tue
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Wed
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Thu
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 1,  8,  0},  {TRUE , 2, 19,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Fri
        {  {TRUE , 3,  0,  0},  {TRUE , 2,  7,  0},  {TRUE , 3, 23,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Sat
        {  {TRUE , 3,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // Sun
    },

    /* input C2 bands time (for festivity days) */
      //   band 1               band 2               band 3               band 4               band 5               band 6               band 7
        {  {TRUE , 3,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0},  {FALSE, 1,  0,  0}  },   // festivity day
};


/*---------------------------------------------------------------------------
 * OpenAT handlers
 *---------------------------------------------------------------------------*/
/* timer handler */
static       ql_timer_t                       Counters_TimerHandler_TimerCountersBackup;     /* timer for counter backup */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Counters_TaskCounters(void *argument);

/* status variables */
static void Counters_InitVar        (void);
static u16  Counters_CalculateVarCrc(void);
       void Counters_UpdateVarCrc   (void);
       bool Counters_VerifyVarCrc   (void);

/* events */
       void Counters_Event_CounterC1Impulse      (void);
       void Counters_Event_CounterC2Impulse      (void);
       void Counters_Event_MainPowerSupplyProblem(void);
       void Counters_Event_InputC1Band           (void);
       void Counters_Event_InputC2Band           (void);
       void Counters_Event_ClockChanged          (void);

/* new configuration */
       void Counters_NewConfigurationInputC1     (COUNTERS__CONFIG__INPUT_C1       *ptr_config_new, COUNTERS__CONFIG__INPUT_C1       *ptr_config_old);
       void Counters_NewConfigurationInputC2     (COUNTERS__CONFIG__INPUT_C2       *ptr_config_new, COUNTERS__CONFIG__INPUT_C2       *ptr_config_old);
       void Counters_NewConfigurationInputC1Bands(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_config_new, COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_config_old);
       void Counters_NewConfigurationInputC2Bands(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_config_new, COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_config_old);

/* get/set C1 counters */
       void Counters_InputC1_Get  (u32 *ptr_value);
       void Counters_InputC1_Set  (u32 *ptr_value);
       void Counters_InputC1F1_Get(u32 *ptr_value);
       void Counters_InputC1F1_Set(u32 *ptr_value);
       void Counters_InputC1F2_Get(u32 *ptr_value);
       void Counters_InputC1F2_Set(u32 *ptr_value);
       void Counters_InputC1F3_Get(u32 *ptr_value);
       void Counters_InputC1F3_Set(u32 *ptr_value);

/* get/set C2 counters */
       void Counters_InputC2_Get  (u32 *ptr_value);
       void Counters_InputC2_Set  (u32 *ptr_value);
       void Counters_InputC2F1_Get(u32 *ptr_value);
       void Counters_InputC2F1_Set(u32 *ptr_value);
       void Counters_InputC2F2_Get(u32 *ptr_value);
       void Counters_InputC2F2_Set(u32 *ptr_value);
       void Counters_InputC2F3_Get(u32 *ptr_value);
       void Counters_InputC2F3_Set(u32 *ptr_value);

/* update input C1/C2 band info */
static void Counters_UpdateInputC1BandInfo(void);
static void Counters_UpdateInputC2BandInfo(void);

/* find input C1/C2 band */
       u8   Counters_FindInputC1Band          (CLOCK__TIME *ptr_actual_rtc_time);
       u8   Counters_FindInputC2Band          (CLOCK__TIME *ptr_actual_rtc_time);

/* set next RTC alarm input C1/C2 band */
static void Counters_SetNewRtcAlarmInputC1Band(CLOCK__TIME *ptr_actual_rtc_time);
static void Counters_SetNewRtcAlarmInputC2Band(CLOCK__TIME *ptr_actual_rtc_time);

/* which counter band */
static bool Counters_WhichInputC1Band         (CLOCK__TIME *ptr_actual_rtc_time, u8 *ptr_band_weekday, u8 *ptr_band_index, u8 *ptr_band_type);
static bool Counters_WhichInputC2Band         (CLOCK__TIME *ptr_actual_rtc_time, u8 *ptr_band_weekday, u8 *ptr_band_index, u8 *ptr_band_type);

/* next  counter band */
static bool Counters_NextInputC1Band          (CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_counter_band_rtc_time);
static bool Counters_NextInputC2Band          (CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_counter_band_rtc_time);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "input C1" status */
       void Counters_Status_InputC1_GetDefault     (COUNTERS__STATUS__INPUT_C1       *ptr_data);
       void Counters_Status_InputC1_Get            (COUNTERS__STATUS__INPUT_C1       *ptr_data);
       void Counters_Status_InputC1_Set            (COUNTERS__STATUS__INPUT_C1       *ptr_data);

/* get/set "input C2" status */
       void Counters_Status_InputC2_GetDefault     (COUNTERS__STATUS__INPUT_C2       *ptr_data);
       void Counters_Status_InputC2_Get            (COUNTERS__STATUS__INPUT_C2       *ptr_data);
       void Counters_Status_InputC2_Set            (COUNTERS__STATUS__INPUT_C2       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "input C1" configuration */
       void Counters_Config_InputC1_GetDefault     (COUNTERS__CONFIG__INPUT_C1       *ptr_data);
       void Counters_Config_InputC1_Get            (COUNTERS__CONFIG__INPUT_C1       *ptr_data);
       void Counters_Config_InputC1_Set            (COUNTERS__CONFIG__INPUT_C1       *ptr_data);
       bool Counters_Config_InputC1_IsValid        (COUNTERS__CONFIG__INPUT_C1       *ptr_data);

/* get/set "input C2" configuration */
       void Counters_Config_InputC2_GetDefault     (COUNTERS__CONFIG__INPUT_C2       *ptr_data);
       void Counters_Config_InputC2_Get            (COUNTERS__CONFIG__INPUT_C2       *ptr_data);
       void Counters_Config_InputC2_Set            (COUNTERS__CONFIG__INPUT_C2       *ptr_data);
       bool Counters_Config_InputC2_IsValid        (COUNTERS__CONFIG__INPUT_C2       *ptr_data);

/* get/set "input C1 bands" configuration */
       void Counters_Config_InputC1Bands_GetDefault(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);
       void Counters_Config_InputC1Bands_Get       (COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);
       void Counters_Config_InputC1Bands_Set       (COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);
       bool Counters_Config_InputC1Bands_IsValid   (COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);

/* get/set "input C2 bands" configuration */
       void Counters_Config_InputC2Bands_GetDefault(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);
       void Counters_Config_InputC2Bands_Get       (COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);
       void Counters_Config_InputC2Bands_Set       (COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);
       bool Counters_Config_InputC2Bands_IsValid   (COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);


/*-----------------------------------------------------------------------------
 * action on timers
 *-----------------------------------------------------------------------------*/
static void Counters_TimerCounterBackup_Start(u32 time_value, bool periodic);
static void Counters_TimerCounterBackup_Stop (void);


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Counters_AdlCallback_Message_TaskMsg(u32 msg_identifier);

/* timer   callback functions */
static void Counters_AdlCallback_Timer_TimerCountersBackup(void *context);




/*===========================================================================
 * Function   : Counters_TaskCounters
 *
 * Description:
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_TaskCounters(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW , "TASK ENTRY POINT - COUNTERS - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* update input C1/C2 band info */
    Counters_UpdateInputC1BandInfo();
    Counters_UpdateInputC2BandInfo();


    /* internal status init */
    Counters_InitVar();


    /* creation of timer "TimerCountersBackup" */
    err = ql_rtos_timer_create(&Counters_TimerHandler_TimerCountersBackup, QL_TIMER_IN_SERVICE, Counters_AdlCallback_Timer_TimerCountersBackup, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* check for counters backup timer */
    if (Counters_Config_InputC1.enabled_status || Counters_Config_InputC2.enabled_status)
    {
        /* start counters backup timer */
        Counters_TimerCounterBackup_Start(TIME_MS__COUNTERS_BACKUP_PERIOD, TRUE);
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            Counters_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : Counters_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Counters_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* C1 counters (total, F1, F2, F3) */
        Counters_InputC1         = Counters_Status_InputC1.counter;
        Counters_InputC1F1       = Counters_Status_InputC1.counter_f1;
        Counters_InputC1F2       = Counters_Status_InputC1.counter_f2;
        Counters_InputC1F3       = Counters_Status_InputC1.counter_f3;

        /* C2 counters (total, F1, F2, F3) */
        Counters_InputC2         = Counters_Status_InputC2.counter;
        Counters_InputC2F1       = Counters_Status_InputC2.counter_f1;
        Counters_InputC2F2       = Counters_Status_InputC2.counter_f2;
        Counters_InputC2F3       = Counters_Status_InputC2.counter_f3;


        /**** update the variable CRC ****/
        Counters_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
   }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time
        // NO TIMER


        /**** other ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : Counters_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 Counters_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */

    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC1  , sizeof(Counters_InputC1  ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC1F1, sizeof(Counters_InputC1F1), crc);
    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC1F2, sizeof(Counters_InputC1F2), crc);
    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC1F3, sizeof(Counters_InputC1F3), crc);

    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC2  , sizeof(Counters_InputC2  ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC2F1, sizeof(Counters_InputC2F1), crc);
    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC2F2, sizeof(Counters_InputC2F2), crc);
    crc = Utility_CalculateCRC16((u8 *)&Counters_InputC2F3, sizeof(Counters_InputC2F3), crc);

    return crc;
}




/*=============================================================================
 * Function   : Counters_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Counters_UpdateVarCrc(void)
{
    Counters_VarCrc = Counters_CalculateVarCrc();
}




/*=============================================================================
 * Function   : Counters_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool Counters_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = Counters_CalculateVarCrc();

    if (Counters_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*===========================================================================
 * Function   : Counters_Event_CounterC1Impulse
 *
 * Description: signal the event counter C1 impulse
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_Event_CounterC1Impulse(void)
{
    ql_event_t event;
    QlOSStatus err;

    (void)err;


    event.id = EVENT_COUNTER_C1_IMPULSE;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
  //if (err != QL_OSI_SUCCESS)
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
  //else
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*===========================================================================
 * Function   : Counters_Event_CounterC2Impulse
 *
 * Description: signal the event counter C2 impulse
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_Event_CounterC2Impulse(void)
{
    ql_event_t event;
    QlOSStatus err;

    (void)err;


    event.id = EVENT_COUNTER_C2_IMPULSE;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
  //if (err != QL_OSI_SUCCESS)
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
  //else
  //{
  //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
  //}
}




/*===========================================================================
 * Function   : Counters_Event_MainPowerSupplyProblem
 *
 * Description: signal the event main power supply problem
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_Event_MainPowerSupplyProblem(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = EVENT_MAIN_POWER_SUPPLY_PROBLEM;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Counters_Event_InputC1Band
 *
 * Description: signal the event input C1 band
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_Event_InputC1Band(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = EVENT_INPUT_C1_BAND;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Counters_Event_InputC2Band
 *
 * Description: signal the event input C2 band
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_Event_InputC2Band(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = EVENT_INPUT_C2_BAND;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Counters_Event_ClockChanged
 *
 * Description: signal the event clock changed
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Counters_Event_ClockChanged(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = EVENT_CLOCK_CHANGED;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Counters_NewConfigurationInputC1
 *
 * Description: signal a new configuration of input counter C1
 * Input      : - ptr_config_new: pointer to new configuration
 *              - ptr_config_old: pointer to old configuration
 * Output     : -
 *=============================================================================*/
void Counters_NewConfigurationInputC1(COUNTERS__CONFIG__INPUT_C1 *ptr_config_new, COUNTERS__CONFIG__INPUT_C1 *ptr_config_old)
{
    if (ptr_config_old->enabled_status != ptr_config_new->enabled_status)
    {
        /* counter C1 activation or deactivation */

        Counters_InputC1   = 0;
        Counters_InputC1F1 = 0;
        Counters_InputC1F2 = 0;
        Counters_InputC1F3 = 0;
        Counters_UpdateVarCrc();

        Counters_Status_InputC1.counter    = Counters_InputC1;
        Counters_Status_InputC1.counter_f1 = Counters_InputC1F1;
        Counters_Status_InputC1.counter_f2 = Counters_InputC1F2;
        Counters_Status_InputC1.counter_f3 = Counters_InputC1F3;

        Counters_Status_InputC2.counter    = Counters_InputC2;
        Counters_Status_InputC2.counter_f1 = Counters_InputC2F1;
        Counters_Status_InputC2.counter_f2 = Counters_InputC2F2;
        Counters_Status_InputC2.counter_f3 = Counters_InputC2F3;

        /* backup */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1);
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2);


        /* check for counters backup timer */
        if (Counters_Config_InputC1.enabled_status || Counters_Config_InputC2.enabled_status)
        {
            /* restart counters backup timer */
            Counters_TimerCounterBackup_Stop();
            Counters_TimerCounterBackup_Start(TIME_MS__COUNTERS_BACKUP_PERIOD, TRUE);
        }
        else
        {
            /* stop counters backup timer */
            Counters_TimerCounterBackup_Stop();
        }
    }


    /* update input C1 band info */
    Counters_UpdateInputC1BandInfo();
}




/*=============================================================================
 * Function   : Counters_NewConfigurationInputC2
 *
 * Description: signal a new configuration of input counter C2
 * Input      : - ptr_config_new: pointer to new configuration
 *              - ptr_config_old: pointer to old configuration
 * Output     : -
 *=============================================================================*/
void Counters_NewConfigurationInputC2(COUNTERS__CONFIG__INPUT_C2 *ptr_config_new, COUNTERS__CONFIG__INPUT_C2 *ptr_config_old)
{
    if (ptr_config_old->enabled_status != ptr_config_new->enabled_status)
    {
        /* counter C2 activation or deactivation */

        Counters_InputC2   = 0;
        Counters_InputC2F1 = 0;
        Counters_InputC2F2 = 0;
        Counters_InputC2F3 = 0;
        Counters_UpdateVarCrc();

        Counters_Status_InputC1.counter    = Counters_InputC1;
        Counters_Status_InputC1.counter_f1 = Counters_InputC1F1;
        Counters_Status_InputC1.counter_f2 = Counters_InputC1F2;
        Counters_Status_InputC1.counter_f3 = Counters_InputC1F3;

        Counters_Status_InputC2.counter    = Counters_InputC2;
        Counters_Status_InputC2.counter_f1 = Counters_InputC2F1;
        Counters_Status_InputC2.counter_f2 = Counters_InputC2F2;
        Counters_Status_InputC2.counter_f3 = Counters_InputC2F3;

        /* backup */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1);
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2);


        /* check for counters backup timer */
        if (Counters_Config_InputC1.enabled_status || Counters_Config_InputC2.enabled_status)
        {
            /* restart counters backup timer */
            Counters_TimerCounterBackup_Stop();
            Counters_TimerCounterBackup_Start(TIME_MS__COUNTERS_BACKUP_PERIOD, TRUE);
        }
        else
        {
            /* stop counters backup timer */
            Counters_TimerCounterBackup_Stop();
        }
    }


    /* update input C2 band info */
    Counters_UpdateInputC2BandInfo();
}




/*=============================================================================
 * Function   : Counters_NewConfigurationInputC1Bands
 *
 * Description: signal a new configuration of input counter C1 bands
 * Input      : - ptr_config_new: pointer to new configuration
 *              - ptr_config_old: pointer to old configuration
 * Output     : -
 *=============================================================================*/
void Counters_NewConfigurationInputC1Bands(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_config_new, COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_config_old)
{
    if (ptr_config_old->enabled_status_week_days != ptr_config_new->enabled_status_week_days)
    {
        /* counter C1 bands activation or deactivation */

        Counters_InputC1   = 0;
        Counters_InputC1F1 = 0;
        Counters_InputC1F2 = 0;
        Counters_InputC1F3 = 0;
        Counters_UpdateVarCrc();

        Counters_Status_InputC1.counter    = Counters_InputC1;
        Counters_Status_InputC1.counter_f1 = Counters_InputC1F1;
        Counters_Status_InputC1.counter_f2 = Counters_InputC1F2;
        Counters_Status_InputC1.counter_f3 = Counters_InputC1F3;

        Counters_Status_InputC2.counter    = Counters_InputC2;
        Counters_Status_InputC2.counter_f1 = Counters_InputC2F1;
        Counters_Status_InputC2.counter_f2 = Counters_InputC2F2;
        Counters_Status_InputC2.counter_f3 = Counters_InputC2F3;

        /* backup */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1);
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2);
    }


    /* update input C1 band info */
    Counters_UpdateInputC1BandInfo();
}




/*=============================================================================
 * Function   : Counters_NewConfigurationInputC2Bands
 *
 * Description: signal a new configuration of input counter C2 bands
 * Input      : - ptr_config_new: pointer to new configuration
 *              - ptr_config_old: pointer to old configuration
 * Output     : -
 *=============================================================================*/
void Counters_NewConfigurationInputC2Bands(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_config_new, COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_config_old)
{
    if (ptr_config_old->enabled_status_week_days != ptr_config_new->enabled_status_week_days)
    {
        /* counter C2 bands activation or deactivation */

        Counters_InputC2   = 0;
        Counters_InputC2F1 = 0;
        Counters_InputC2F2 = 0;
        Counters_InputC2F3 = 0;
        Counters_UpdateVarCrc();

        Counters_Status_InputC1.counter    = Counters_InputC1;
        Counters_Status_InputC1.counter_f1 = Counters_InputC1F1;
        Counters_Status_InputC1.counter_f2 = Counters_InputC1F2;
        Counters_Status_InputC1.counter_f3 = Counters_InputC1F3;

        Counters_Status_InputC2.counter    = Counters_InputC2;
        Counters_Status_InputC2.counter_f1 = Counters_InputC2F1;
        Counters_Status_InputC2.counter_f2 = Counters_InputC2F2;
        Counters_Status_InputC2.counter_f3 = Counters_InputC2F3;

        /* backup */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1);
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2);
    }


    /* update input C2 band info */
    Counters_UpdateInputC2BandInfo();
}




/*===========================================================================
 * Function   : Counters_InputC1_Get
 *
 * Description: get the C1 counter
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC1_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC1;
}




/*===========================================================================
 * Function   : Counters_InputC1_Set
 *
 * Description: set the C1 counter
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC1_Set(u32 *ptr_value)
{
    Counters_InputC1 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC1F1_Get
 *
 * Description: get the C1 counter band F1
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC1F1_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC1F1;
}




/*===========================================================================
 * Function   : Counters_InputC1F1_Set
 *
 * Description: set the C1 counter band F1
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC1F1_Set(u32 *ptr_value)
{
    Counters_InputC1F1 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC1F2_Get
 *
 * Description: get the C1 counter band F2
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC1F2_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC1F2;
}




/*===========================================================================
 * Function   : Counters_InputC1F2_Set
 *
 * Description: set the C1 counter band F2
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC1F2_Set(u32 *ptr_value)
{
    Counters_InputC1F2 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC1F3_Get
 *
 * Description: get the C1 counter band F3
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC1F3_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC1F3;
}




/*===========================================================================
 * Function   : Counters_InputC1F3_Set
 *
 * Description: set the C1 counter band F3
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC1F3_Set(u32 *ptr_value)
{
    Counters_InputC1F3 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC2_Get
 *
 * Description: get the C2 counter
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC2_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC2;
}




/*===========================================================================
 * Function   : Counters_InputC2_Set
 *
 * Description: set the C2 counter
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC2_Set(u32 *ptr_value)
{
    Counters_InputC2 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC2F1_Get
 *
 * Description: get the C2 counter band F1
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC2F1_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC2F1;
}




/*===========================================================================
 * Function   : Counters_InputC2F1_Set
 *
 * Description: set the C2 counter band F1
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC2F1_Set(u32 *ptr_value)
{
    Counters_InputC2F1 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC2F2_Get
 *
 * Description: get the C2 counter band F2
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC2F2_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC2F2;
}




/*===========================================================================
 * Function   : Counters_InputC2F2_Set
 *
 * Description: set the C2 counter band F2
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC2F2_Set(u32 *ptr_value)
{
    Counters_InputC2F2 = *ptr_value;
}




/*===========================================================================
 * Function   : Counters_InputC2F3_Get
 *
 * Description: get the C2 counter band F3
 * Input      : - ptr_data: pointer to store the value got
 * Output     : -
 *===========================================================================*/
void Counters_InputC2F3_Get(u32 *ptr_value)
{
    *ptr_value = Counters_InputC2F3;
}




/*===========================================================================
 * Function   : Counters_InputC2F3_Set
 *
 * Description: set the C2 counter band F3
 * Input      : - ptr_value: pointer to the value to set
 * Output     : -
 *===========================================================================*/
void Counters_InputC2F3_Set(u32 *ptr_value)
{
    Counters_InputC2F3 = *ptr_value;
}




/*=============================================================================
 * Function   : Counters_UpdateInputC1BandInfo
 *
 * Description: update input C1 band info
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Counters_UpdateInputC1BandInfo(void)
{
    CLOCK__TIME rtc_actual_time;
    bool        result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW , "Update input C1 Band info", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* get the actual time */
    result = Clock_GetTime(&rtc_actual_time);
    if (result)
    {
        /* find current input C1 Band type */
        Counters_InputC1BandType = Counters_FindInputC1Band(&rtc_actual_time);

        /* set next RTC alarm (for next input C1 band) */
        Counters_SetNewRtcAlarmInputC1Band(&rtc_actual_time);
    }
    else
    {
        Counters_InputC1BandType = COUNTERS__BAND_TYPE__F1;
    }
}




/*=============================================================================
 * Function   : Counters_UpdateInputC2BandInfo
 *
 * Description: update input C2 band info
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Counters_UpdateInputC2BandInfo(void)
{
    CLOCK__TIME rtc_actual_time;
    bool        result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW , "Update input C2 Band info", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* get the actual time */
    result = Clock_GetTime(&rtc_actual_time);
    if (result)
    {
        /* find current input C2 Band type */
        Counters_InputC2BandType = Counters_FindInputC2Band(&rtc_actual_time);

        /* set next RTC alarm (for next input C2 band) */
        Counters_SetNewRtcAlarmInputC2Band(&rtc_actual_time);
    }
    else
    {
        Counters_InputC2BandType = COUNTERS__BAND_TYPE__F1;
    }
}




/*=============================================================================
 * Function   : Counters_FindInputC1Band
 *
 * Description: determine the input C1 band (F1, F2, F3) of current time
 *              (if input C1 is enabled and input C1 bands are enabled)
 * Input      : - ptr_actual_rtc_time: pointer to current time
 * Output     : - input C1 band type (F1, F2, F3)
 *=============================================================================*/
u8 Counters_FindInputC1Band(CLOCK__TIME *ptr_actual_rtc_time)
{
    u8   band_weekday;
    u8   band_index;
    u8   band_type;

    bool result;


    /* determine the input C1 band (F1, F2, F3) of current time */
    result = Counters_WhichInputC1Band(ptr_actual_rtc_time, &band_weekday, &band_index, &band_type);
    if (!result)
        band_type = COUNTERS__BAND_TYPE__F1;

    return band_type;
}




/*=============================================================================
 * Function   : Counters_FindInputC2Band
 *
 * Description: determine the input C2 band (F1, F2, F3) of current time
 *              (if input C2 is enabled and input C2 bands are enabled)
 * Input      : - ptr_actual_rtc_time: pointer to current time
 * Output     : - input C2 band type (F1, F2, F3)
 *=============================================================================*/
u8 Counters_FindInputC2Band(CLOCK__TIME *ptr_actual_rtc_time)
{
    u8   band_weekday;
    u8   band_index;
    u8   band_type;

    bool result;


    /* determine the input C2 band (F1, F2, F3) of current time */
    result = Counters_WhichInputC2Band(ptr_actual_rtc_time, &band_weekday, &band_index, &band_type);
    if (!result)
        band_type = COUNTERS__BAND_TYPE__F1;

    return band_type;
}




/*=============================================================================
 * Function   : Counters_SetNewRtcAlarmInputC1Band
 *
 * Description: Set next RTC alarm (for next input C1 band)
 *              (if input C1 is enabled and input C1 bands are enabled)
 * Input      : - ptr_actual_rtc_time: pointer to current time
 * Output     : -
 *=============================================================================*/
static void Counters_SetNewRtcAlarmInputC1Band(CLOCK__TIME *ptr_actual_rtc_time)
{
    CLOCK__TIME next_band;

    bool        result;


    /* search next input C1 band */
    result = Counters_NextInputC1Band(ptr_actual_rtc_time, &next_band);
    if (result)
    {
        /* next input C1 band     found */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Set new RTC alarm for input C1 band", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* set RTC alarm to "next_c1_band" */
        result = RtcAlarm_RtcAlarmSet   (RTC_ALARM__ALARM_INPUT_C1_BAND, &next_band);
    }
    else
    {
        /* next input C1 band not found */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Delete RTC alarm for input C1 band" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* delete RTC alarm */
        result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_INPUT_C1_BAND);
    }
}




/*=============================================================================
 * Function   : Counters_SetNewRtcAlarmInputC2Band
 *
 *
 * Description: Set next RTC alarm (for next input C2 band)
 *              (if input C2 is enabled and input C2 bands are enabled)
 * Input      : - ptr_actual_rtc_time: pointer to current time
 * Output     : -
 *=============================================================================*/
static void Counters_SetNewRtcAlarmInputC2Band(CLOCK__TIME *ptr_actual_rtc_time)
{
    CLOCK__TIME next_band;

    bool        result;


    /* search next input C2 band */
    result = Counters_NextInputC2Band(ptr_actual_rtc_time, &next_band);
    if (result)
    {
        /* next input C2 band     found */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Set new RTC alarm for input C2 band", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* set RTC alarm to "next_c2_band" */
        result = RtcAlarm_RtcAlarmSet   (RTC_ALARM__ALARM_INPUT_C2_BAND, &next_band);
    }
    else
    {
        /* next input C2 band not found */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Delete RTC alarm for input C2 band" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* delete RTC alarm */
        result = RtcAlarm_RtcAlarmDelete(RTC_ALARM__ALARM_INPUT_C2_BAND);
    }
}




/*===========================================================================
 * Function   : Counters_WhichInputC1Band
 *
 * Description: determine the input C1 band (F1, F2, F3) of current time
 *              (if input C1 bands are enabled)
 * Input      : - ptr_actual_rtc_time: pointer to current time
 *              - ptr_band_weekday   : pointer to save the counter band weekday [0-7] (0: festivity, 1: Monday, ..., 7: Sunday)
 *              - ptr_band_index     : pointer to save the counter band index   [0 - (COUNTERS__NUM_BANDS_PER_DAY-1)]
 *              - ptr_band_type      : pointer to save the counter band type (F1, F2, F3)
 * Output     : - FALSE: input C1 bands disabled
 *              - TRUE : input C1 bands enabled
 *===========================================================================*/
static bool Counters_WhichInputC1Band(CLOCK__TIME *ptr_actual_rtc_time, u8 *ptr_band_weekday, u8 *ptr_band_index, u8 *ptr_band_type)
{
    u16  year;
    u8   month;
    u8   day;

    u8   hour;
    u8   minute;
    u8   second;

    u8   week_day;

    u8   band_weekday;
    u8   band_index;
    u8   band_type;

    bool is_festivity_day;
    s8   res_s8;
    u8   i;


    /* verify if it is disabled */
    if (!Counters_Config_InputC1.enabled_status)
        return FALSE;  // it is disabled
    if (!Counters_Config_InputC1Bands.enabled_status_week_days)
        return FALSE;  // it is disabled


    /* copy delayed RTC time */
    year     = ptr_actual_rtc_time->year;
    month    = ptr_actual_rtc_time->month;
    day      = ptr_actual_rtc_time->day;
    hour     = ptr_actual_rtc_time->hour;
    minute   = ptr_actual_rtc_time->minute;
    second   = ptr_actual_rtc_time->second;
    week_day = ptr_actual_rtc_time->week_day;


    /* check if current day is a festivity day */
    if (Counters_Config_InputC1Bands.enabled_status_festivity_days)
    {
        /* festivity for input C1 enabled */

        /* check if current day is a festivity day */
        is_festivity_day = Clock_IsFestivityDay(day, month, year);
    }
    else
    {
        /* festivity for input C1 disabled */

        is_festivity_day = FALSE;
    }


    if (is_festivity_day)
    {
        /* current day is festivity day and festivity is enabled */

        /* search band index for festivity day */
        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC1Bands.bands_festivity_days[i].status)
            {
                /* i-th band enabled */

                res_s8 = Clock_CompareTimes(hour,
                                            minute,
                                            second,
                                            Counters_Config_InputC1Bands.bands_festivity_days[i].start_hour,
                                            Counters_Config_InputC1Bands.bands_festivity_days[i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for festivity day */

                    break;
                }
            }
        }

        band_weekday = 0;  // festivity
        band_index   = i;
        band_type    = Counters_Config_InputC1Bands.bands_festivity_days[i].type;
    }
    else
    {
        /* current day is week      day or
         * current day is festivity day but festivity is disabled */

        /* search band index for current weekday */
        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC1Bands.bands_week_days[week_day - 1][i].status)
            {
                /* i-th band enabled */

                res_s8 = Clock_CompareTimes(hour,
                                            minute,
                                            second,
                                            Counters_Config_InputC1Bands.bands_week_days[week_day - 1][i].start_hour,
                                            Counters_Config_InputC1Bands.bands_week_days[week_day - 1][i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for current weekday */

                    break;
                }
            }
        }

        band_weekday = week_day;
        band_index   = i;
        band_type    = Counters_Config_InputC1Bands.bands_week_days[week_day - 1][i].type;
    }


    /* save info */
    *ptr_band_weekday = band_weekday;
    *ptr_band_index   = band_index;
    *ptr_band_type    = band_type;

    return TRUE;
}




/*===========================================================================
 * Function   : Counters_WhichInputC2Band
 *
 * Description: determine the input C2 band (F1, F2, F3) of current time
 *              (if input C2 bands are enabled)
 * Input      : - ptr_actual_rtc_time: pointer to current time
 *              - ptr_band_weekday   : pointer to save the counter band weekday [0-7] (0: festivity, 1: Monday, ..., 7: Sunday)
 *              - ptr_band_index     : pointer to save the counter band index   [0 - (COUNTERS__NUM_BANDS_PER_DAY-1)]
 *              - ptr_band_type      : pointer to save the counter band type (F1, F2, F3)
 * Output     : - FALSE: input C2 bands disabled
 *              - TRUE : input C2 bands enabled
 *===========================================================================*/
static bool Counters_WhichInputC2Band(CLOCK__TIME *ptr_actual_rtc_time, u8 *ptr_band_weekday, u8 *ptr_band_index, u8 *ptr_band_type)
{
    u16  year;
    u8   month;
    u8   day;

    u8   hour;
    u8   minute;
    u8   second;

    u8   week_day;

    u8   band_weekday;
    u8   band_index;
    u8   band_type;

    bool is_festivity_day;
    s8   res_s8;
    u8   i;


    /* verify if it is disabled */
    if (!Counters_Config_InputC2.enabled_status)
        return FALSE;  // it is disabled
    if (!Counters_Config_InputC2Bands.enabled_status_week_days)
        return FALSE;  // it is disabled


    /* copy delayed RTC time */
    year     = ptr_actual_rtc_time->year;
    month    = ptr_actual_rtc_time->month;
    day      = ptr_actual_rtc_time->day;
    hour     = ptr_actual_rtc_time->hour;
    minute   = ptr_actual_rtc_time->minute;
    second   = ptr_actual_rtc_time->second;
    week_day = ptr_actual_rtc_time->week_day;


    /* check if current day is a festivity day */
    if (Counters_Config_InputC2Bands.enabled_status_festivity_days)
    {
        /* festivity for input C2 enabled */

        /* check if current day is a festivity day */
        is_festivity_day = Clock_IsFestivityDay(day, month, year);
    }
    else
    {
        /* festivity for input C2 disabled */

        is_festivity_day = FALSE;
    }


    if (is_festivity_day)
    {
        /* current day is festivity day and festivity is enabled */

        /* search band index for festivity day */
        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC2Bands.bands_festivity_days[i].status)
            {
                /* i-th band enabled */

                res_s8 = Clock_CompareTimes(hour,
                                            minute,
                                            second,
                                            Counters_Config_InputC2Bands.bands_festivity_days[i].start_hour,
                                            Counters_Config_InputC2Bands.bands_festivity_days[i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for festivity day */

                    break;
                }
            }
        }

        band_weekday = 0;  // festivity
        band_index   = i;
        band_type    = Counters_Config_InputC2Bands.bands_festivity_days[i].type;
    }
    else
    {
        /* current day is week      day or
         * current day is festivity day but festivity is disabled */

        /* search band index for current weekday */
        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC2Bands.bands_week_days[week_day - 1][i].status)
            {
                /* i-th band enabled */

                res_s8 = Clock_CompareTimes(hour,
                                            minute,
                                            second,
                                            Counters_Config_InputC2Bands.bands_week_days[week_day - 1][i].start_hour,
                                            Counters_Config_InputC2Bands.bands_week_days[week_day - 1][i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for current weekday */

                    break;
                }
            }
        }

        band_weekday = week_day;
        band_index   = i;
        band_type    = Counters_Config_InputC2Bands.bands_week_days[week_day - 1][i].type;
    }


    /* save info */
    *ptr_band_weekday = band_weekday;
    *ptr_band_index   = band_index;
    *ptr_band_type    = band_type;

    return TRUE;
}




/*===========================================================================
 * Function   : Counters_NextInputC1Band
 *
 * Description: determine next input C1 band (F1, F2, F3) (start time)
 *              with reference to current time
 *              (if input C1 bands are enabled)
 * Input      : - ptr_actual_rtc_time           : pointer to current time
 *              - ptr_next_counter_band_rtc_time: pointer to save next input C1 band time (start time)
 * Output     : - FALSE: input C1 bands disabled
 *              - TRUE : input C1 bands enabled
 *===========================================================================*/
static bool Counters_NextInputC1Band(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_counter_band_rtc_time)
{
    CLOCK__TIME actual_rtc_time;

    u16         year;
    u8          month;
    u8          day;

    u8          week_day;

    u8          hour;
    u8          minute;

    bool        last_band_found;
    u8          last_band_index;

    u8          band_index;

    bool        is_festivity_day;
    s8          res_s8;
    u8          i;


    /* verify if it is disabled */
    if (!Counters_Config_InputC1.enabled_status)
        return FALSE;  // it is disabled
    if (!Counters_Config_InputC1Bands.enabled_status_week_days)
        return FALSE;  // it is disabled


    /* copy actual RTC time */
    actual_rtc_time = *ptr_actual_rtc_time;


    /* check if current day is a festivity day */
    if (Counters_Config_InputC1Bands.enabled_status_festivity_days)
    {
        /* festivity for input C1 enabled */

        /* check if current day is a festivity day */
        is_festivity_day = Clock_IsFestivityDay(actual_rtc_time.day, actual_rtc_time.month, actual_rtc_time.year);
    }
    else
    {
        /* festivity for input C1 disabled */

        is_festivity_day = FALSE;
    }


    if (is_festivity_day)
    {
        /* current day is festivity day and festivity is enabled */

        /* search band index for festivity day */

        last_band_found = FALSE;
        last_band_index = COUNTERS__NUM_BANDS_PER_DAY - 1;

        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC1Bands.bands_festivity_days[i].status)
            {
                /* i-th band enabled */

                if (!last_band_found)
                {
                    last_band_found = TRUE;
                    last_band_index = i;
                }

                res_s8 = Clock_CompareTimes(actual_rtc_time.hour,
                                            actual_rtc_time.minute,
                                            actual_rtc_time.second,
                                            Counters_Config_InputC1Bands.bands_festivity_days[i].start_hour,
                                            Counters_Config_InputC1Bands.bands_festivity_days[i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for current weekday */
                    break;
                }
            }
        }

        if (!last_band_found)
        {
            last_band_found = TRUE;
            last_band_index = 0;
        }

        if (i != last_band_index)
        {
            /* not last band for current weekday */

            band_index = i + 1;

            day      = actual_rtc_time.day;
            month    = actual_rtc_time.month;
            year     = actual_rtc_time.year;

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = Counters_Config_InputC1Bands.bands_festivity_days[band_index].start_hour;
            minute   = Counters_Config_InputC1Bands.bands_festivity_days[band_index].start_minute;
        }
        else
        {
            /*     last band for current weekday */

            band_index = 0;

            Clock_TomorrowDay(actual_rtc_time.year, actual_rtc_time.month, actual_rtc_time.day,
                              &year               , &month               , &day);

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = 0;
            minute   = 0;
        }
    }
    else
    {
        /* current day is week      day or
           current day is festivity day but festivity is disabled */

        /* search band index for current weekday */

        last_band_found = FALSE;
        last_band_index = COUNTERS__NUM_BANDS_PER_DAY - 1;

        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC1Bands.bands_week_days[actual_rtc_time.week_day - 1][i].status)
            {
                /* i-th band enabled */

                if (!last_band_found)
                {
                    last_band_found = TRUE;
                    last_band_index = i;
                }

                res_s8 = Clock_CompareTimes(actual_rtc_time.hour,
                                            actual_rtc_time.minute,
                                            actual_rtc_time.second,
                                            Counters_Config_InputC1Bands.bands_week_days[actual_rtc_time.week_day - 1][i].start_hour,
                                            Counters_Config_InputC1Bands.bands_week_days[actual_rtc_time.week_day - 1][i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for current weekday */
                    break;
                }
            }
        }

        if (!last_band_found)
        {
            last_band_found = TRUE;
            last_band_index = 0;
        }

        if (i != last_band_index)
        {
            /* not last band for current weekday */

            band_index = i + 1;

            day      = actual_rtc_time.day;
            month    = actual_rtc_time.month;
            year     = actual_rtc_time.year;

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = Counters_Config_InputC1Bands.bands_week_days[week_day - 1][band_index].start_hour;
            minute   = Counters_Config_InputC1Bands.bands_week_days[week_day - 1][band_index].start_minute;
        }
        else
        {
            /*     last band for current weekday */

            band_index = 0;

            Clock_TomorrowDay(actual_rtc_time.year, actual_rtc_time.month, actual_rtc_time.day,
                              &year               , &month               , &day);

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = 0;
            minute   = 0;
        }
    }


    /* save next input C1 band */

    ptr_next_counter_band_rtc_time->second   = 0;
    ptr_next_counter_band_rtc_time->minute   = minute;
    ptr_next_counter_band_rtc_time->hour     = hour;

    ptr_next_counter_band_rtc_time->day      = day;
    ptr_next_counter_band_rtc_time->month    = month;
    ptr_next_counter_band_rtc_time->year     = year;

    ptr_next_counter_band_rtc_time->week_day = week_day;


    return TRUE;
}




/*===========================================================================
 * Function   : Counters_NextInputC2Band
 *
 * Description: determine next input C2 band (F1, F2, F3) (start time)
 *              with reference to current time
 *              (if input C2 bands are enabled)
 * Input      : - ptr_actual_rtc_time           : pointer to current time
 *              - ptr_next_counter_band_rtc_time: pointer to save next input C2 band time (start time)
 * Output     : - FALSE: input C2 bands disabled
 *              - TRUE : input C2 bands enabled
 *===========================================================================*/
static bool Counters_NextInputC2Band(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_next_counter_band_rtc_time)
{
    CLOCK__TIME actual_rtc_time;

    u16         year;
    u8          month;
    u8          day;

    u8          week_day;

    u8          hour;
    u8          minute;

    bool        last_band_found;
    u8          last_band_index;

    u8          band_index;

    bool        is_festivity_day;
    s8          res_s8;
    u8          i;


    /* verify if it is disabled */
    if (!Counters_Config_InputC2.enabled_status)
        return FALSE;  // it is disabled
    if (!Counters_Config_InputC2Bands.enabled_status_week_days)
        return FALSE;  // it is disabled


    /* copy actual RTC time */
    actual_rtc_time = *ptr_actual_rtc_time;


    /* check if current day is a festivity day */
    if (Counters_Config_InputC2Bands.enabled_status_festivity_days)
    {
        /* festivity for input C2 enabled */

        /* check if current day is a festivity day */
        is_festivity_day = Clock_IsFestivityDay(actual_rtc_time.day, actual_rtc_time.month, actual_rtc_time.year);
    }
    else
    {
        /* festivity for input C2 disabled */

        is_festivity_day = FALSE;
    }


    if (is_festivity_day)
    {
        /* current day is festivity day and festivity is enabled */

        /* search band index for festivity day */

        last_band_found = FALSE;
        last_band_index = COUNTERS__NUM_BANDS_PER_DAY - 1;

        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC2Bands.bands_festivity_days[i].status)
            {
                /* i-th band enabled */

                if (!last_band_found)
                {
                    last_band_found = TRUE;
                    last_band_index = i;
                }

                res_s8 = Clock_CompareTimes(actual_rtc_time.hour,
                                            actual_rtc_time.minute,
                                            actual_rtc_time.second,
                                            Counters_Config_InputC2Bands.bands_festivity_days[i].start_hour,
                                            Counters_Config_InputC2Bands.bands_festivity_days[i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for current weekday */
                    break;
                }
            }
        }

        if (!last_band_found)
        {
            last_band_found = TRUE;
            last_band_index = 0;
        }

        if (i != last_band_index)
        {
            /* not last band for current weekday */

            band_index = i + 1;

            day      = actual_rtc_time.day;
            month    = actual_rtc_time.month;
            year     = actual_rtc_time.year;

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = Counters_Config_InputC2Bands.bands_festivity_days[band_index].start_hour;
            minute   = Counters_Config_InputC2Bands.bands_festivity_days[band_index].start_minute;
        }
        else
        {
            /*     last band for current weekday */

            band_index = 0;

            Clock_TomorrowDay(actual_rtc_time.year, actual_rtc_time.month, actual_rtc_time.day,
                              &year               , &month               , &day);

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = 0;
            minute   = 0;
        }
    }
    else
    {
        /* current day is week      day or
           current day is festivity day but festivity is disabled */

        /* search band index for current weekday */

        last_band_found = FALSE;
        last_band_index = COUNTERS__NUM_BANDS_PER_DAY - 1;

        for (i = (COUNTERS__NUM_BANDS_PER_DAY - 1); i > 0; i--)
        {
            if (Counters_Config_InputC2Bands.bands_week_days[actual_rtc_time.week_day - 1][i].status)
            {
                /* i-th band enabled */

                if (!last_band_found)
                {
                    last_band_found = TRUE;
                    last_band_index = i;
                }

                res_s8 = Clock_CompareTimes(actual_rtc_time.hour,
                                            actual_rtc_time.minute,
                                            actual_rtc_time.second,
                                            Counters_Config_InputC2Bands.bands_week_days[actual_rtc_time.week_day - 1][i].start_hour,
                                            Counters_Config_InputC2Bands.bands_week_days[actual_rtc_time.week_day - 1][i].start_minute,
                                            0);
                if ((res_s8 == +1) || (res_s8 == 0))
                {
                    /* current time  >=  i-th configured band start time for current weekday */
                    break;
                }
            }
        }

        if (!last_band_found)
        {
            last_band_found = TRUE;
            last_band_index = 0;
        }

        if (i != last_band_index)
        {
            /* not last band for current weekday */

            band_index = i + 1;

            day      = actual_rtc_time.day;
            month    = actual_rtc_time.month;
            year     = actual_rtc_time.year;

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = Counters_Config_InputC2Bands.bands_week_days[week_day - 1][band_index].start_hour;
            minute   = Counters_Config_InputC2Bands.bands_week_days[week_day - 1][band_index].start_minute;
        }
        else
        {
            /*     last band for current weekday */

            band_index = 0;

            Clock_TomorrowDay(actual_rtc_time.year, actual_rtc_time.month, actual_rtc_time.day,
                              &year               , &month               , &day);

            week_day = Clock_WeekDayOfADay(day, month, year);

            hour     = 0;
            minute   = 0;
        }
    }


    /* save next input C2 band */

    ptr_next_counter_band_rtc_time->second   = 0;
    ptr_next_counter_band_rtc_time->minute   = minute;
    ptr_next_counter_band_rtc_time->hour     = hour;

    ptr_next_counter_band_rtc_time->day      = day;
    ptr_next_counter_band_rtc_time->month    = month;
    ptr_next_counter_band_rtc_time->year     = year;

    ptr_next_counter_band_rtc_time->week_day = week_day;


    return TRUE;
}




/*===========================================================================
 * Function   : Counters_Status_InputC1_GetDefault
 *
 * Description: get the default "input C1" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Status_InputC1_GetDefault(COUNTERS__STATUS__INPUT_C1 *ptr_data)
{
    *ptr_data = Counters_Status_InputC1Default;
}




/*===========================================================================
 * Function   : Counters_Status_InputC1_Get
 *
 * Description: get the "input C1" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Status_InputC1_Get(COUNTERS__STATUS__INPUT_C1 *ptr_data)
{
    *ptr_data = Counters_Status_InputC1;
}




/*===========================================================================
 * Function   : Counters_Status_InputC1_Set
 *
 * Description: set the "input C1" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Counters_Status_InputC1_Set(COUNTERS__STATUS__INPUT_C1 *ptr_data)
{
    Counters_Status_InputC1 = *ptr_data;
}




/*===========================================================================
 * Function   : Counters_Status_InputC2_GetDefault
 *
 * Description: get the default "input C2" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Status_InputC2_GetDefault(COUNTERS__STATUS__INPUT_C2 *ptr_data)
{
    *ptr_data = Counters_Status_InputC2Default;
}




/*===========================================================================
 * Function   : Counters_Status_InputC2_Get
 *
 * Description: get the "input C2" status
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Status_InputC2_Get(COUNTERS__STATUS__INPUT_C2 *ptr_data)
{
    *ptr_data = Counters_Status_InputC2;
}




/*===========================================================================
 * Function   : Counters_Status_InputC2_Set
 *
 * Description: set the "input C2" status
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Counters_Status_InputC2_Set(COUNTERS__STATUS__INPUT_C2 *ptr_data)
{
    Counters_Status_InputC2 = *ptr_data;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1_GetDefault
 *
 * Description: get the default "input C1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC1_GetDefault(COUNTERS__CONFIG__INPUT_C1 *ptr_data)
{
    *ptr_data = Counters_Config_InputC1Default;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1_Get
 *
 * Description: get the "input C1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC1_Get(COUNTERS__CONFIG__INPUT_C1 *ptr_data)
{
    *ptr_data = Counters_Config_InputC1;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1_Set
 *
 * Description: set the "input C1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC1_Set(COUNTERS__CONFIG__INPUT_C1 *ptr_data)
{
    Counters_Config_InputC1 = *ptr_data;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1_IsValid
 *
 * Description: check if the "input C1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Counters_Config_InputC1_IsValid(COUNTERS__CONFIG__INPUT_C1 *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2_GetDefault
 *
 * Description: get the default "input C2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC2_GetDefault(COUNTERS__CONFIG__INPUT_C2 *ptr_data)
{
    *ptr_data = Counters_Config_InputC2Default;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2_Get
 *
 * Description: get the "input C2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC2_Get(COUNTERS__CONFIG__INPUT_C2 *ptr_data)
{
    *ptr_data = Counters_Config_InputC2;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2_Set
 *
 * Description: set the "input C2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC2_Set(COUNTERS__CONFIG__INPUT_C2 *ptr_data)
{
    Counters_Config_InputC2 = *ptr_data;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2_IsValid
 *
 * Description: check if the "input C2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Counters_Config_InputC2_IsValid(COUNTERS__CONFIG__INPUT_C2 *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1Bands_GetDefault
 *
 * Description: get the default "input C1 bands" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC1Bands_GetDefault(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data)
{
    *ptr_data = Counters_Config_InputC1BandsDefault;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1Bands_Get
 *
 * Description: get the "input C1 bands" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC1Bands_Get(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data)
{
    *ptr_data = Counters_Config_InputC1Bands;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1Bands_Set
 *
 * Description: set the "input C1 bands" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC1Bands_Set(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data)
{
    Counters_Config_InputC1Bands = *ptr_data;
}




/*===========================================================================
 * Function   : Counters_Config_InputC1Bands_IsValid
 *
 * Description: check if the "input C1 bands" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Counters_Config_InputC1Bands_IsValid(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data)
{
    s8 res_s8;
    u8 i;
    u8 j;


    /*--------------------------------------------------------------
     * Check the parameters values
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < COUNTERS__NUM_BANDS_PER_DAY; j++)
        {
            /* band type (F1, F2, F3) */
            if (
                 (ptr_data->bands_week_days     [i][j].type != COUNTERS__BAND_TYPE__F1) &&
                 (ptr_data->bands_week_days     [i][j].type != COUNTERS__BAND_TYPE__F2) &&
                 (ptr_data->bands_week_days     [i][j].type != COUNTERS__BAND_TYPE__F3)
               )
            {
                return FALSE;
            }

            /* band start hour   [0-23] */
            if (ptr_data->bands_week_days     [i][j].start_hour   > 23)
                return FALSE;

            /* band start minute [0-59] */
            if (ptr_data->bands_week_days     [i][j].start_minute > 59)
                return FALSE;
        }
    }

    /* festivity days */
        for (j = 0; j < COUNTERS__NUM_BANDS_PER_DAY; j++)
        {
            /* band type (F1, F2, F3) */
            if (
                 (ptr_data->bands_festivity_days   [j].type != COUNTERS__BAND_TYPE__F1) &&
                 (ptr_data->bands_festivity_days   [j].type != COUNTERS__BAND_TYPE__F2) &&
                 (ptr_data->bands_festivity_days   [j].type != COUNTERS__BAND_TYPE__F3)
               )
            {
                return FALSE;
            }

            /* band start hour   [0-23] */
            if (ptr_data->bands_festivity_days   [j].start_hour   > 23)
                return FALSE;

            /* band start minute [0-59] */
            if (ptr_data->bands_festivity_days   [j].start_minute > 59)
                return FALSE;
        }


    /*--------------------------------------------------------------
     * Check first band enable status (it must to be enabled)
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        if (!(ptr_data->bands_week_days     [i][0].status))
        {
            /* first band disabled */
            return FALSE;
        }
    }

    /* festivity days */
        if (!(ptr_data->bands_festivity_days   [0].status))
        {
            /* first band disabled */
            return FALSE;
        }


    /*--------------------------------------------------------------
     * Check first band start time (it must to be 00:00)
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        if (
             (ptr_data->bands_week_days     [i][0].start_hour   != 0) ||
             (ptr_data->bands_week_days     [i][0].start_minute != 0)
           )
        {
            /* first band start time different from 00:00 */
            return FALSE;
        }
    }

    /* festivity days */
        if (
             (ptr_data->bands_festivity_days   [0].start_hour   != 0) ||
             (ptr_data->bands_festivity_days   [0].start_minute != 0)
           )
        {
            /* first band start time different from 00:00 */
            return FALSE;
        }


    /*--------------------------------------------------------------
     * Check relation between j-th band enable status and (j+1)-th band enable status
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (!(ptr_data->bands_week_days     [i][j    ].status)) &&
                 ( (ptr_data->bands_week_days     [i][j + 1].status))
               )
            {
                /* j-th band disabled and (j+1)-th band enabled */
                return FALSE;
            }
        }
    }

    /* festivity days */
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (!(ptr_data->bands_festivity_days   [j    ].status)) &&
                 ( (ptr_data->bands_festivity_days   [j + 1].status))
               )
            {
                /* j-th band disabled and (j+1)-th band enabled */
                return FALSE;
            }
        }


    /*--------------------------------------------------------------
     * Check relation between j-th band start time and (j+1)-th band start time
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (ptr_data->bands_week_days     [i][j    ].status) &&
                 (ptr_data->bands_week_days     [i][j + 1].status)
               )
            {
                /* j-th band and (j+1)-th band enabled */
                res_s8 = Clock_CompareTimes(ptr_data->bands_week_days     [i][j    ].start_hour,
                                            ptr_data->bands_week_days     [i][j    ].start_minute,
                                            0,
                                            ptr_data->bands_week_days     [i][j + 1].start_hour,
                                            ptr_data->bands_week_days     [i][j + 1].start_minute,
                                            0);
                if ((res_s8 == 0) || (res_s8 == +1))
                {
                    /* j-th band start time  >=  (j+1)-th band start time */
                    return FALSE;
                }
            }
        }
    }

    /* festivity days */
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (ptr_data->bands_festivity_days   [j    ].status) &&
                 (ptr_data->bands_festivity_days   [j + 1].status)
               )
            {
                /* j-th band and (j+1)-th band enabled */
                res_s8 = Clock_CompareTimes(ptr_data->bands_festivity_days   [j    ].start_hour,
                                            ptr_data->bands_festivity_days   [j    ].start_minute,
                                            0,
                                            ptr_data->bands_festivity_days   [j + 1].start_hour,
                                            ptr_data->bands_festivity_days   [j + 1].start_minute,
                                            0);
                if ((res_s8 == 0) || (res_s8 == +1))
                {
                    /* j-th band start time  >=  (j+1)-th band start time */
                    return FALSE;
                }
            }
        }


    return TRUE;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2Bands_GetDefault
 *
 * Description: get the default "input C2 bands" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC2Bands_GetDefault(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data)
{
    *ptr_data = Counters_Config_InputC2BandsDefault;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2Bands_Get
 *
 * Description: get the "input C2 bands" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC2Bands_Get(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data)
{
    *ptr_data = Counters_Config_InputC2Bands;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2Bands_Set
 *
 * Description: set the "input C2 bands" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Counters_Config_InputC2Bands_Set(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data)
{
    Counters_Config_InputC2Bands = *ptr_data;
}




/*===========================================================================
 * Function   : Counters_Config_InputC2Bands_IsValid
 *
 * Description: check if the "input C2 bands" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Counters_Config_InputC2Bands_IsValid(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data)
{
    s8 res_s8;
    u8 i;
    u8 j;


    /*--------------------------------------------------------------
     * Check the parameters values
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < COUNTERS__NUM_BANDS_PER_DAY; j++)
        {
            /* band type (F1, F2, F3) */
            if (
                 (ptr_data->bands_week_days     [i][j].type != COUNTERS__BAND_TYPE__F1) &&
                 (ptr_data->bands_week_days     [i][j].type != COUNTERS__BAND_TYPE__F2) &&
                 (ptr_data->bands_week_days     [i][j].type != COUNTERS__BAND_TYPE__F3)
               )
            {
                return FALSE;
            }

            /* band start hour   [0-23] */
            if (ptr_data->bands_week_days     [i][j].start_hour   > 23)
                return FALSE;

            /* band start minute [0-59] */
            if (ptr_data->bands_week_days     [i][j].start_minute > 59)
                return FALSE;
        }
    }

    /* festivity days */
        for (j = 0; j < COUNTERS__NUM_BANDS_PER_DAY; j++)
        {
            /* band type (F1, F2, F3) */
            if (
                 (ptr_data->bands_festivity_days   [j].type != COUNTERS__BAND_TYPE__F1) &&
                 (ptr_data->bands_festivity_days   [j].type != COUNTERS__BAND_TYPE__F2) &&
                 (ptr_data->bands_festivity_days   [j].type != COUNTERS__BAND_TYPE__F3)
               )
            {
                return FALSE;
            }

            /* band start hour   [0-23] */
            if (ptr_data->bands_festivity_days   [j].start_hour   > 23)
                return FALSE;

            /* band start minute [0-59] */
            if (ptr_data->bands_festivity_days   [j].start_minute > 59)
                return FALSE;
        }


    /*--------------------------------------------------------------
     * Check first band enable status (it must to be enabled)
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        if (!(ptr_data->bands_week_days     [i][0].status))
        {
            /* first band disabled */
            return FALSE;
        }
    }

    /* festivity days */
        if (!(ptr_data->bands_festivity_days   [0].status))
        {
            /* first band disabled */
            return FALSE;
        }


    /*--------------------------------------------------------------
     * Check first band start time (it must to be 00:00)
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        if (
             (ptr_data->bands_week_days     [i][0].start_hour   != 0) ||
             (ptr_data->bands_week_days     [i][0].start_minute != 0)
           )
        {
            /* first band start time different from 00:00 */
            return FALSE;
        }
    }

    /* festivity days */
        if (
             (ptr_data->bands_festivity_days   [0].start_hour   != 0) ||
             (ptr_data->bands_festivity_days   [0].start_minute != 0)
           )
        {
            /* first band start time different from 00:00 */
            return FALSE;
        }


    /*--------------------------------------------------------------
     * Check relation between j-th band enable status and (j+1)-th band enable status
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (!(ptr_data->bands_week_days     [i][j    ].status)) &&
                 ( (ptr_data->bands_week_days     [i][j + 1].status))
               )
            {
                /* j-th band disabled and (j+1)-th band enabled */
                return FALSE;
            }
        }
    }

    /* festivity days */
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (!(ptr_data->bands_festivity_days   [j    ].status)) &&
                 ( (ptr_data->bands_festivity_days   [j + 1].status))
               )
            {
                /* j-th band disabled and (j+1)-th band enabled */
                return FALSE;
            }
        }


    /*--------------------------------------------------------------
     * Check relation between j-th band start time and (j+1)-th band start time
     *--------------------------------------------------------------*/
    /* week days */
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (ptr_data->bands_week_days     [i][j    ].status) &&
                 (ptr_data->bands_week_days     [i][j + 1].status)
               )
            {
                /* j-th band and (j+1)-th band enabled */
                res_s8 = Clock_CompareTimes(ptr_data->bands_week_days     [i][j    ].start_hour,
                                            ptr_data->bands_week_days     [i][j    ].start_minute,
                                            0,
                                            ptr_data->bands_week_days     [i][j + 1].start_hour,
                                            ptr_data->bands_week_days     [i][j + 1].start_minute,
                                            0);
                if ((res_s8 == 0) || (res_s8 == +1))
                {
                    /* j-th band start time  >=  (j+1)-th band start time */
                    return FALSE;
                }
            }
        }
    }

    /* festivity days */
        for (j = 0; j < (COUNTERS__NUM_BANDS_PER_DAY - 1); j++)
        {
            if (
                 (ptr_data->bands_festivity_days   [j    ].status) &&
                 (ptr_data->bands_festivity_days   [j + 1].status)
               )
            {
                /* j-th band and (j+1)-th band enabled */
                res_s8 = Clock_CompareTimes(ptr_data->bands_festivity_days   [j    ].start_hour,
                                            ptr_data->bands_festivity_days   [j    ].start_minute,
                                            0,
                                            ptr_data->bands_festivity_days   [j + 1].start_hour,
                                            ptr_data->bands_festivity_days   [j + 1].start_minute,
                                            0);
                if ((res_s8 == 0) || (res_s8 == +1))
                {
                    /* j-th band start time  >=  (j+1)-th band start time */
                    return FALSE;
                }
            }
        }


    return TRUE;
}




/*=============================================================================
 * Function   : Counters_TimerCounterBackup_Start
 *
 * Description: start the "counters backup" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Counters_TimerCounterBackup_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Start \"counters backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Counters_TimerHandler_TimerCountersBackup, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Counters_TimerCounterBackup_Stop
 *
 * Description: stop the "counters backup" timer
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Counters_TimerCounterBackup_Stop(void)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "Stop \"counters backup\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Counters_TimerHandler_TimerCountersBackup);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Counters_DebugString, sizeof(Counters_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, Counters_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Counters_AdlCallback_Message_TaskMsg
 *
 * Description: - task counters callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void Counters_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    switch (msg_identifier)
    {
        /* counter C1 impulse */
        case EVENT_COUNTER_C1_IMPULSE:
            /* increase C1 input counter */
            if (!Counters_Config_InputC1Bands.enabled_status_week_days)
            {
                /* input C1 bands disabled */

                Counters_InputC1++;
            }
            else
            {
                /* input C1 bands enabled */

                Counters_InputC1++;

                switch (Counters_InputC1BandType)
                {
                    /* band type F1 (more expensive) */
                    case COUNTERS__BAND_TYPE__F1:
                    default:
                        Counters_InputC1F1++;
                        break;

                    /* band type F2 */
                    case COUNTERS__BAND_TYPE__F2:
                        Counters_InputC1F2++;
                        break;

                    /* band type F3 (less expensive) */
                    case COUNTERS__BAND_TYPE__F3:
                        Counters_InputC1F3++;
                        break;
                }
            }

            Counters_UpdateVarCrc();
            break;


        /* counter C2 impulse */
        case EVENT_COUNTER_C2_IMPULSE:
            /* increase C2 input counter */
            if (!Counters_Config_InputC2Bands.enabled_status_week_days)
            {
                /* input C2 bands disabled */

                Counters_InputC2++;
            }
            else
            {
                /* input C2 bands enabled */

                Counters_InputC2++;

                switch (Counters_InputC2BandType)
                {
                    /* band type F1 (more expensive) */
                    case COUNTERS__BAND_TYPE__F1:
                    default:
                        Counters_InputC2F1++;
                        break;

                    /* band type F2 */
                    case COUNTERS__BAND_TYPE__F2:
                        Counters_InputC2F2++;
                        break;

                    /* band type F3 (less expensive) */
                    case COUNTERS__BAND_TYPE__F3:
                        Counters_InputC2F3++;
                        break;
                }
            }

            Counters_UpdateVarCrc();
            break;


        /* timeout counters backup   */
        /* main power supply problem */
        case EVENT_TIMEOUT_COUNTERS_BACKUP:
        case EVENT_MAIN_POWER_SUPPLY_PROBLEM:
            Counters_Status_InputC1.counter    = Counters_InputC1;
            Counters_Status_InputC1.counter_f1 = Counters_InputC1F1;
            Counters_Status_InputC1.counter_f2 = Counters_InputC1F2;
            Counters_Status_InputC1.counter_f3 = Counters_InputC1F3;

            Counters_Status_InputC2.counter    = Counters_InputC2;
            Counters_Status_InputC2.counter_f1 = Counters_InputC2F1;
            Counters_Status_InputC2.counter_f2 = Counters_InputC2F2;
            Counters_Status_InputC2.counter_f3 = Counters_InputC2F3;

            /* backup */
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C1);
            ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__016_COUNTERS, PROGRAM_FLASH__FLASH_ID__016_COUNTERS__COUNTER_C2);
            break;


        /* input C1 band */
        case EVENT_INPUT_C1_BAND:
            /* update input C1 band info */
            Counters_UpdateInputC1BandInfo();
            break;


        /* input C2 band */
        case EVENT_INPUT_C2_BAND:
            /* update input C2 band info */
            Counters_UpdateInputC2BandInfo();
            break;


        /* clock changed */
        case EVENT_CLOCK_CHANGED:
            /* update input C1/C2 band info */
            Counters_UpdateInputC1BandInfo();
            Counters_UpdateInputC2BandInfo();
            break;


        default:
            break;
    }
}




/*=============================================================================
 * Function   : Counters_AdlCallback_Timer_TimerCountersBackup
 *
 * Description:
 * Input      : - context:
 * Output     : -
 *=============================================================================*/
static void Counters_AdlCallback_Timer_TimerCountersBackup(void *context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - BACKUP COUNTERS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = EVENT_TIMEOUT_COUNTERS_BACKUP;

    err = ql_rtos_event_send(Boot_TaskRef_Counters, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_COUNTERS, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
