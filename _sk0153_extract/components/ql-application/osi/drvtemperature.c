/*=============================================================================
 * File       :  DRVTEMPERATURE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - temperature driver
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "adc.h"
#include "boot.h"
#include "debug_my.h"
#include "drvtemperature.h"
#include "fw_config.h"
#include "startup.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* task message IDs */
#define TASK_MSG_ID__TIMEOUT_TEMP_MEASUREMENT    (12100 | (QL_COMPONENT_APP_START << 16))    /* event - timeout temperature measurement                                 */
#define TASK_MSG_ID__OUT1_ON                     (12101 | (QL_COMPONENT_APP_START << 16))    /* event - output OUT1 on                                                  */
#define TASK_MSG_ID__OUT1_OFF                    (12102 | (QL_COMPONENT_APP_START << 16))    /* event - output OUT1 off                                                 */
#define TASK_MSG_ID__OUT2_ON                     (12103 | (QL_COMPONENT_APP_START << 16))    /* event - output OUT2 on                                                  */
#define TASK_MSG_ID__OUT2_OFF                    (12104 | (QL_COMPONENT_APP_START << 16))    /* event - output OUT2 off                                                 */
#define TASK_MSG_ID__TIMEOUT_OUT1_INCREASE       (12105 | (QL_COMPONENT_APP_START << 16))    /* event - timeout increase temperature offset for enabling of output OUT1 */
#define TASK_MSG_ID__TIMEOUT_OUT1_DECREASE       (12106 | (QL_COMPONENT_APP_START << 16))    /* event - timeout decrease temperature offset for enabling of output OUT1 */
#define TASK_MSG_ID__TIMEOUT_OUT2_INCREASE       (12107 | (QL_COMPONENT_APP_START << 16))    /* event - timeout increase temperature offset for enabling of output OUT2 */
#define TASK_MSG_ID__TIMEOUT_OUT2_DECREASE       (12108 | (QL_COMPONENT_APP_START << 16))    /* event - timeout decrease temperature offset for enabling of output OUT2 */

/* timeout values (ms) */
#define TIME_MS__TEMP_MEASUREMENT_PERIOD         (6 * 1000L)   /* periodic temperature measurement period (ms) */

/* max number of registered signals of new available temperatures */
#define NUM_MAX_SIGNALS_NEW_TEMPERATURES         8

/* max number of registered signals of new available adc values */
#define NUM_MAX_SIGNALS_NEW_ADC_VALUES           8

/* number of consecutive ADC readings */
#define NUMBER_OF_ADC_READINGS                   6

/* delay between 2 consecutive ADC readings (ms) */
#define DELAY_BEETWEEN_ADC_READINGS              100

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                  200

/*---------------------------------------------------------------------------
 * Temperature measurement
 *---------------------------------------------------------------------------*/
/* resistor value (Ohm) */
#if (                                                                          \
      (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_THEORETIC       ) ||  \
      (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_REAL            )     \
    )
#define RESISTOR__R                              ( 4640.0)
#endif
#if (                                                                          \
      (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_THEORETIC       ) ||  \
      (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_REAL            ) ||  \
      (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_MURATA_THEORETIC)     \
    )
#define RESISTOR__R                              (47000.0)
#endif

/* voltage values (V) */
#define VOLTAGE__VDD                             (3.0  )
#define VOLTAGE__ADC_FS                          (4.096)

/* sensor resistor values (Ohm) */
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_THEORETIC       )
#define SENSOR_RESISTOR__SHORT_CIRCUITED         100       // sensor short-circuited (if <= the value)
#define SENSOR_RESISTOR__PLUS_110                240       // +110.0 �C
#define SENSOR_RESISTOR__PLUS_105                273       // +105.0 �C
#define SENSOR_RESISTOR__PLUS_55                 1410      //  +55.0 �C
#define SENSOR_RESISTOR__MINUS_20                45253     //  -20.0 �C
#define SENSOR_RESISTOR__MINUS_25                59253     //  -25.0 �C
#define SENSOR_RESISTOR__DISCONNECTED            70000     // sensor disconnected    (if >= the value)
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_REAL            )
#define SENSOR_RESISTOR__SHORT_CIRCUITED         100       // sensor short-circuited (if <= the value)
#define SENSOR_RESISTOR__PLUS_110                240       // +110.0 �C
#define SENSOR_RESISTOR__PLUS_105                273       // +105.0 �C
#define SENSOR_RESISTOR__PLUS_55                 1409      //  +55.0 �C
#define SENSOR_RESISTOR__MINUS_20                41942     //  -20.0 �C
#define SENSOR_RESISTOR__MINUS_25                59253     //  -25.0 �C
#define SENSOR_RESISTOR__DISCONNECTED            70000     // sensor disconnected    (if >= the value)
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_THEORETIC       )
#define SENSOR_RESISTOR__SHORT_CIRCUITED         200       // sensor short-circuited (if <= the value)
#define SENSOR_RESISTOR__PLUS_110                500       // +110.0 �C
#define SENSOR_RESISTOR__PLUS_105                586       // +105.0 �C
#define SENSOR_RESISTOR__PLUS_55                 2962      //  +55.0 �C
#define SENSOR_RESISTOR__MINUS_20                88462     //  -20.0 �C
#define SENSOR_RESISTOR__MINUS_25                114462    //  -25.0 �C
#define SENSOR_RESISTOR__DISCONNECTED            150000    // sensor disconnected    (if >= the value)
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_REAL            )
#define SENSOR_RESISTOR__SHORT_CIRCUITED         200       // sensor short-circuited (if <= the value)
#define SENSOR_RESISTOR__PLUS_110                500       // +110.0 �C
#define SENSOR_RESISTOR__PLUS_105                586       // +105.0 �C
#define SENSOR_RESISTOR__PLUS_55                 2962      //  +55.0 �C
#define SENSOR_RESISTOR__MINUS_20                88462     //  -20.0 �C
#define SENSOR_RESISTOR__MINUS_25                114462    //  -25.0 �C
#define SENSOR_RESISTOR__DISCONNECTED            150000    // sensor disconnected    (if >= the value)
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_MURATA_THEORETIC)
#define SENSOR_RESISTOR__SHORT_CIRCUITED         200       // sensor short-circuited (if <= the value)
#define SENSOR_RESISTOR__PLUS_110                758       // +110.0 �C
#define SENSOR_RESISTOR__PLUS_105                858       // +105.0 �C
#define SENSOR_RESISTOR__PLUS_55                 3535      //  +55.0 �C
#define SENSOR_RESISTOR__MINUS_20                68237     //  -20.0 �C
#define SENSOR_RESISTOR__MINUS_25                87559     //  -25.0 �C
#define SENSOR_RESISTOR__MINUS_40                195652    //  -40.0 �C
#define SENSOR_RESISTOR__DISCONNECTED            250000    // sensor disconnected    (if >= the value)
#endif

/* size of NTC table */
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_THEORETIC       )
#define TABLE_NTC_SIZE                           28
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_REAL            )
#define TABLE_NTC_SIZE                           28
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_THEORETIC       )
#define TABLE_NTC_SIZE                           28
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_REAL            )
#define TABLE_NTC_SIZE                           28
#endif
#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_MURATA_THEORETIC)
#define TABLE_NTC_SIZE                           34
#endif

/* size of offset table */
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
#define TABLE_OFFSET_OUT_1_SIZE                  14
#define TABLE_OFFSET_OUT_2_SIZE                  14
#endif
#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
#define TABLE_OFFSET_OUT_1_SIZE                  4
#define TABLE_OFFSET_OUT_2_SIZE                  4
#endif

/* step of increase/decrease of temperature when output is enabled (/10 �C) */
#define STEP_OFFSET_OUT_1                        1
#define STEP_OFFSET_OUT_2                        1

/* parameters for temperature compensation */
#define X1                                       (84.0)    // (/10 �C)
#define X2                                       (106.0)   // (/10 �C)
#define Y1                                       (22.0)    // (/10 �C)
#define Y2                                       (11.0)    // (/10 �C)

#define TEMPERATURE_OFFSET                       (Y2)      // (/10 �C)




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* interpolation table for NTC */
typedef struct
{
    const u32 resistor      [TABLE_NTC_SIZE         ];    // NTC         (Ohm)
    const s16 temperature   [TABLE_NTC_SIZE         ];    // temperature (/10 �C)
} TABLE_NTC;


/* table for internal temperature offset for enabling of output OUT1 */
typedef struct
{
    const u32 times_increase[TABLE_OFFSET_OUT_1_SIZE];    // times for 0.1�C increase of temperature when output is enabled  (sec)
    const u32 times_decrease[TABLE_OFFSET_OUT_1_SIZE];    // times for 0.1�C decrease of temperature when output is disabled (sec)
} TABLE_TEMPERATURE_OFFSET_FOR_OUT1;


/* table for internal temperature offset for enabling of output OUT2 */
typedef struct
{
    const u32 times_increase[TABLE_OFFSET_OUT_2_SIZE];    // times for 0.1�C increase of temperature when output is enabled  (sec)
    const u32 times_decrease[TABLE_OFFSET_OUT_2_SIZE];    // times for 0.1�C decrease of temperature when output is disabled (sec)
} TABLE_TEMPERATURE_OFFSET_FOR_OUT2;


/* new available temperature signals */
typedef struct
{
    void (*ptr_function)(u8, s16, u8, s16);   /* pointer to the function to be called when there are new available temperatures */
                    //   |    |   |    |
                    //   |    |   |    |________  new external temperature (/10 �C)
                    //   |    |   |_____________  new external sensor status
                    //   |    |_________________  new internal temperature (/10 �C)
                    //   |______________________  new internal sensor status
} SIGNAL_NEW_TEMPERATURES;


/* new available adc value signals */
typedef struct
{
    void (*ptr_function)(u16, u16);   /* pointer to the function to be called when there are new available temperatures */
                    //    |    |
                    //    |    |________  new external adc value (mV)
                    //    |_____________  new internal adc value (mV)
} SIGNAL_NEW_ADC_VALUES;


/* timer info */
typedef struct
{
    bool status;                                          // timer status
    bool periodic;                                        // periodic indication
    u32  time_value;                                      // timeout value  (ms)
    u32  time_remaining;                                  // remaining time (ms)
    u32  time_now;                                        // 
} TIMER_INFO;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/

/* coefficients of interpolation table for NTC */

#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_THEORETIC)
/* 4K7 theoretic */
static const TABLE_NTC                                DrvTemperature_Table_Ntc =
{
    /* NTC (Ohm) */
    {
      //     1       2       3       4       5        6       7       8       9      10
           240,    273,    316,    368,    430,     503,    591,    698,    827,    983,   // 10
          1175,   1410,   1701,   2061,   2512,    3077,   3791,   4700,   5864,   7364,   // 20
          9314,  11868,  15242,  19737,  25781,   33990,  45253,  59253,                   // 30
    },

    /* temperature (/ 10 �C) */
    {
      //     1       2       3       4       5        6       7       8       9      10
          1100,   1050,   1000,    950,    900,     850,    800,    750,    700,    650,   // 10
           600,    550,    500,    450,    400,     350,    300,    250,    200,    150,   // 20
           100,     50,      0,    -50,   -100,    -150,   -200,   -250,                   // 30
    }
};
#endif


#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__4K7_REAL)
/* 4K7 real */
static const TABLE_NTC                                DrvTemperature_Table_Ntc =
{
    /* NTC (Ohm) */
    {
      //     1       2       3       4       5        6       7       8       9      10
           240,    273,    316,    368,    430,     503,    591,    698,    827,    983,   // 10
          1176,   1409,   1699,   2057,   2505,    3061,   3769,   4667,   5803,   7268,   // 20
          9172,  11632,  14833,  19058,  24689,   32050,  41942,  59253,                   // 30
    },

    /* temperature (/ 10 �C) */
    {
      //     1       2       3       4       5        6       7       8       9      10
          1100,   1050,   1000,    950,    900,     850,    800,    750,    700,    650,   // 10
           600,    550,    500,    450,    400,     350,    300,    250,    200,    150,   // 20
           100,     50,      0,    -50,   -100,    -150,   -200,   -250,                   // 30
    }
};
#endif


#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_THEORETIC)
/* 10K theoretic */
static const TABLE_NTC                                DrvTemperature_Table_Ntc =
{
    /* NTC (Ohm) */
    {
      //     1       2       3       4       5        6       7       8       9      10
           500,    586,    678,    786,    915,    1070,   1255,   1479,   1751,   2081,   // 10
          2473,   2962,   3572,   4332,   5272,    6463,   7965,   9883,  12309,  15441,   // 20
         19510,  24771,  31626,  40568,  52439,   67871,  88462, 114462,                   // 30
    },

    /* temperature (/ 10 �C) */
    {
      //     1       2       3       4       5        6       7       8       9      10
          1100,   1050,   1000,    950,    900,     850,    800,    750,    700,    650,   // 10
           600,    550,    500,    450,    400,     350,    300,    250,    200,    150,   // 20
           100,     50,      0,    -50,   -100,    -150,   -200,   -250,                   // 30
    }
};
#endif


#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_REAL)
/* 10K real */
static const TABLE_NTC                                DrvTemperature_Table_Ntc =
{
    /* NTC (Ohm) */
    {
      //     1       2       3       4       5        6       7       8       9      10
           500,    586,    678,    786,    915,    1070,   1255,   1479,   1751,   2081,   // 10
          2473,   2962,   3572,   4332,   5272,    6463,   7965,   9883,  12309,  15441,   // 20
         19510,  24771,  31626,  40568,  52439,   67871,  88462, 114462,                   // 30
    },

    /* temperature (/10 �C) */
    {
      //     1       2       3       4       5        6       7       8       9      10
          1100,   1050,   1000,    950,    900,     850,    800,    750,    700,    650,   // 10
           600,    550,    500,    450,    400,     350,    300,    250,    200,    150,   // 20
           100,     50,      0,    -50,   -100,    -150,   -200,   -250,                   // 30
    }
};
#endif


#if (FW_CONFIG__HW__NTC_MODEL == FW_CONFIG__HW__NTC_MODEL__10K_MURATA_THEORETIC)
/* 10K Murata theoretic */
static const TABLE_NTC                                DrvTemperature_Table_Ntc =
{
    /* NTC (Ohm) */
    {
      //     1       2       3       4       5        6       7       8       9      10
           531,    596,    672,    758,    858,     974,   1110,   1268,   1452,   1669,   // 10
          1925,   2228,   2586,   3014,   3535,    4161,   4917,   5834,   6948,   8315,   // 20
         10000,  12081,  14674,  17926,  22021,   27219,  33892,  42506,  53650,  68237,   // 30
         87559, 113347, 148717, 195652,                                                    // 40
    },

    /* temperature (/10 �C) */
    {
      //     1       2       3       4       5        6       7       8       9      10
          1250,   1200,   1150,   1100,   1050,    1000,    950,    900,    850,    800,   // 10
           750,    700,    650,    600,    550,     500,    450,    400,    350,    300,   // 20
           250,    200,    150,    100,     50,       0,    -50,   -100,   -150,   -200,   // 30
          -250,   -300,   -350,   -400,                                                    // 40
    }
};
#endif


#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__NORMAL)
/* table for internal temperature offset for enabling of output OUT 1 */
/* NOTE: every step is relative to a 0.1�C (= STEP_OFFSET_OUT_1) increase */
static const TABLE_TEMPERATURE_OFFSET_FOR_OUT1        DrvTemperature_Table_TemperatureIntOffsetForOut1 =
{
    /* times increase (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
            20,     20,     20,     30,     30,     60,     120,    120,     60,    240,   // 10
            60,     60,     60,     60,                                                    // 20
    },

    /* times decrease (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
            60,     60,     60,     30,     30,      60,     60,     30,     30,     60,   // 10
            60,     60,    180,     60,                                                    // 20
    },
};


/* table for internal temperature offset for enabling of output OUT 2 */
/* NOTE: every step is relative to a 0.1�C (= STEP_OFFSET_OUT_2) increase */
static const TABLE_TEMPERATURE_OFFSET_FOR_OUT2        DrvTemperature_Table_TemperatureIntOffsetForOut2 =
{
    /* times increase (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
            20,     20,     20,     30,     30,     60,     120,    120,     60,    240,   // 10
            60,     60,     60,     60,                                                    // 20
    },

    /* times decrease (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
            60,     60,     60,     30,     30,      60,     60,     30,     30,     60,   // 10
            60,     60,    180,     60,                                                    // 20
    },
};
#endif


#if (FW_CONFIG__TEST__MODE_1 == FW_CONFIG__TEST__MODE_1__FAST)
/* table for internal temperature offset for enabling of output OUT 1 */
/* NOTE: every step is relative to a 0.1�C (= STEP_OFFSET_OUT_1) increase */
static const TABLE_TEMPERATURE_OFFSET_FOR_OUT1        DrvTemperature_Table_TemperatureIntOffsetForOut1 =
{
    /* times increase (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
             4,      6,      8,     10,                                                    // 10
    },

    /* times decrease (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
            10,     12,     14,     16,                                                    // 10
    },
};


/* table for internal temperature offset for enabling of output OUT 2 */
/* NOTE: every step is relative to a 0.1�C (= STEP_OFFSET_OUT_2) increase */
static const TABLE_TEMPERATURE_OFFSET_FOR_OUT2        DrvTemperature_Table_TemperatureIntOffsetForOut2 =
{
    /* times increase (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
             5,      7,      9,     11,                                                    // 10
    },

    /* times decrease (sec) */
    {
      //     1       2       3       4       5        6       7       8       9      10
            11,     13,     15,     17,                                                    // 10
    },
};
#endif


/* sensor status */
static       u8                                       DrvTemperature_SensorStatusInt = DRVTEMPERATURE__SENSOR__UNKNOWN;
static       u8                                       DrvTemperature_SensorStatusExt = DRVTEMPERATURE__SENSOR__UNKNOWN;

/* internal/external temperatures (/10 �C) */
static       s16                                      DrvTemperature_TemperatureInt = 0;
static       s16                                      DrvTemperature_TemperatureExt = 0;

/* internal/external ADC value */
static       u16                                      DrvTemperature_AdcValueInt = 0;
static       u16                                      DrvTemperature_AdcValueExt = 0;

/* status of internal temperature compensation for enabling of output */
static       bool                                     DrvTemperature_Out1Increase;            // = FALSE
static       bool                                     DrvTemperature_Out1Decrease;            // = FALSE
static       bool                                     DrvTemperature_Out2Increase;            // = FALSE
static       bool                                     DrvTemperature_Out2Decrease;            // = FALSE

/* internal temperature table index for offset for enabling of output */
static       u8                                       DrvTemperature_IndexOffsetOut1;         // = 0
static       u8                                       DrvTemperature_IndexOffsetOut2;         // = 0

/* internal temperature offset for enabling of output (/10 �C) */
static       u8                                       DrvTemperature_TemperatureOffsetOut1;   // = 0
static       u8                                       DrvTemperature_TemperatureOffsetOut2;   // = 0

/* timer info */
static       TIMER_INFO                               DrvTemperature_TimerOut1Increase_Info;
static       TIMER_INFO                               DrvTemperature_TimerOut1Decrease_Info;
static       TIMER_INFO                               DrvTemperature_TimerOut2Increase_Info;
static       TIMER_INFO                               DrvTemperature_TimerOut2Decrease_Info;

/* CRC variables */
static       u16                                      DrvTemperature_VarCrc;                  // = 0x0000;


/* debug string */
static       ascii                                    DrvTemperature_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/* registered new available temperatures signals */
static       u8                                       DrvTemperature_NumberSignalsNewTemperatures = 0;                           /* number of registered new available temperatures signals */
static       SIGNAL_NEW_TEMPERATURES                  DrvTemperature_SignalsNewTemperatures[NUM_MAX_SIGNALS_NEW_TEMPERATURES];   /*           registered new available temperatures signals */

/* registered new available temperatures signals */
static       u8                                       DrvTemperature_NumberSignalsNewAdcValues = 0;                              /* number of registered new available ADC signals */
static       SIGNAL_NEW_ADC_VALUES                    DrvTemperature_SignalsNewAdcValues[NUM_MAX_SIGNALS_NEW_ADC_VALUES];        /*           registered new available ADC signals */


/*---------------------------------------------------------------------------
 * Configurations
 *---------------------------------------------------------------------------*/

/* configuration - "internal temperature calibration" */
static       DRVTEMPERATURE__CONFIG__CALIBRATION_INT  DrvTemperature_Config_CalibrationInt;
static const DRVTEMPERATURE__CONFIG__CALIBRATION_INT  DrvTemperature_Config_CalibrationIntDefault =
{
    0,      // calibration
};


/* configuration - "external temperature calibration" */
static       DRVTEMPERATURE__CONFIG__CALIBRATION_EXT  DrvTemperature_Config_CalibrationExt;
static const DRVTEMPERATURE__CONFIG__CALIBRATION_EXT  DrvTemperature_Config_CalibrationExtDefault =
{
    0,      // calibration
};


/* configuration - "level measurement mode" */
static       DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE DrvTemperature_Config_TemperatureMode;
static const DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE DrvTemperature_Config_TemperatureModeDefault =
{
    DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_TABLE,  // level measurement mode
};


/* configuration - "interpolation polynomial 1" */
/*---------------------------------------------------------------------------
 * Coefficients of interpolation polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
static       DRVTEMPERATURE__CONFIG__POLYNOMIAL_1     DrvTemperature_Config_Polynomial1;
static const DRVTEMPERATURE__CONFIG__POLYNOMIAL_1     DrvTemperature_Config_Polynomial1Default =
{
//    a6            a5            a4            a3            a2            a1            a0
    { 0.0,          0.0,          0.0,          0.0,          0.0,          1.0,          0.0 }   // y = x
  //{-4.3894E-05,  +1.9652E-03,  -3.3800E-02,  +3.2271E-01,  -1.7291E+00,  +5.5985E+00,   0.0 }
};


/* configuration - "interpolation polynomial 2" */
/*---------------------------------------------------------------------------
 * Coefficients of interpolation polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
static       DRVTEMPERATURE__CONFIG__POLYNOMIAL_2     DrvTemperature_Config_Polynomial2;
static const DRVTEMPERATURE__CONFIG__POLYNOMIAL_2     DrvTemperature_Config_Polynomial2Default =
{
//    a6            a5            a4            a3            a2            a1            a0
    { 0.0,          0.0,          0.0,          0.0,          0.0,          1.0,          0.0 }   // y = x
};


/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* timer handlers */
static       ql_timer_t                               DrvTemperature_TimerHandler_TimerTempMeasurement;    /* timer for periodic temperature measurement                        */
static       ql_timer_t                               DrvTemperature_TimerHandler_TimerOut1Increase;       /* timer for increase temperature offset for enabling of output OUT1 */
static       ql_timer_t                               DrvTemperature_TimerHandler_TimerOut1Decrease;       /* timer for decrease temperature offset for enabling of output OUT1 */
static       ql_timer_t                               DrvTemperature_TimerHandler_TimerOut2Increase;       /* timer for increase temperature offset for enabling of output OUT2 */
static       ql_timer_t                               DrvTemperature_TimerHandler_TimerOut2Decrease;       /* timer for decrease temperature offset for enabling of output OUT2 */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void  DrvTemperature_TaskDrvTemperature(void *argument);

/* status variables */
static void  DrvTemperature_InitVar        (void);
static u16   DrvTemperature_CalculateVarCrc(void);
       void  DrvTemperature_UpdateVarCrc   (void);
       bool  DrvTemperature_VerifyVarCrc   (void);

/* events */
       void  DrvTemperature_OutputOut1On (void);
       void  DrvTemperature_OutputOut1Off(void);
       void  DrvTemperature_OutputOut2On (void);
       void  DrvTemperature_OutputOut2Off(void);

/* get  internal/external temperature */
       bool  DrvTemperature_GetInternalTemperature  (u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);
       bool  DrvTemperature_GetExternalTemperature  (u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);

/* read internal/external temperature */
static bool  DrvTemperature_ReadInternalTemperature (u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);
static bool  DrvTemperature_ReadExternalTemperature (u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);
static bool  DrvTemperature_ReadInternalTemperature2(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);
static bool  DrvTemperature_ReadExternalTemperature2(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);

/* interpolation */
static float DrvTemperature_InterpolationPolynomial(float x);
static s16   DrvTemperature_InterpolationTableInt  (u32   x, TABLE_NTC *ptr_table_ntc, u8 size_table_ntc);

/* registered signals of new available temperatures */
       void  DrvTemperature_RegisterSignalNewTemperatures(void (*ptr_function)(u8, s16, u8, s16));

/* registered signals of new available ADC values */
       void  DrvTemperature_RegisterSignalNewAdcValues(void (*ptr_function)(u16, u16));

/*-----------------------------------------------------------------------------
 * action  on timers
 *-----------------------------------------------------------------------------*/
static void  DrvTemperature_TimerTempMeasurement_Start(u32 time_value, bool periodic);

static void  DrvTemperature_TimerOut1Increase_Start   (u32 time_value, bool periodic);
static u32   DrvTemperature_TimerOut1Increase_Stop    (void);

static void  DrvTemperature_TimerOut1Decrease_Start   (u32 time_value, bool periodic);
static u32   DrvTemperature_TimerOut1Decrease_Stop    (void);

static void  DrvTemperature_TimerOut2Increase_Start   (u32 time_value, bool periodic);
static u32   DrvTemperature_TimerOut2Increase_Stop    (void);

static void  DrvTemperature_TimerOut2Decrease_Start   (u32 time_value, bool periodic);
static u32   DrvTemperature_TimerOut2Decrease_Stop    (void);

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - "internal temperature calibration" */
       void  DrvTemperature_Config_CalibrationInt_GetDefault            (DRVTEMPERATURE__CONFIG__CALIBRATION_INT   *ptr_data);
       void  DrvTemperature_Config_CalibrationInt_Get                   (DRVTEMPERATURE__CONFIG__CALIBRATION_INT   *ptr_data);
       void  DrvTemperature_Config_CalibrationInt_Set                   (DRVTEMPERATURE__CONFIG__CALIBRATION_INT   *ptr_data);
       bool  DrvTemperature_Config_CalibrationInt_IsValid               (DRVTEMPERATURE__CONFIG__CALIBRATION_INT   *ptr_data);

/* configuration - "external temperature calibration" */
       void  DrvTemperature_Config_CalibrationExt_GetDefault            (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT   *ptr_data);
       void  DrvTemperature_Config_CalibrationExt_Get                   (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT   *ptr_data);
       void  DrvTemperature_Config_CalibrationExt_Set                   (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT   *ptr_data);
       bool  DrvTemperature_Config_CalibrationExt_IsValid               (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT   *ptr_data);

/* get/set "temperature measurement mode"     configuration */
       void  DrvTemperature_Config_TemperatureMeasurementMode_GetDefault(DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE  *ptr_data);
       void  DrvTemperature_Config_TemperatureMeasurementMode_Get       (DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE  *ptr_data);
       void  DrvTemperature_Config_TemperatureMeasurementMode_Set       (DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE  *ptr_data);
       bool  DrvTemperature_Config_TemperatureMeasurementMode_IsValid   (DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE  *ptr_data);

/* get/set "interpolation polynomial 1" configuration */
       void  DrvTemperature_Config_Polynomial1_GetDefault               (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1      *ptr_data);
       void  DrvTemperature_Config_Polynomial1_Get                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1      *ptr_data);
       void  DrvTemperature_Config_Polynomial1_Set                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1      *ptr_data);
       bool  DrvTemperature_Config_Polynomial1_IsValid                  (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1      *ptr_data);

/* get/set "interpolation polynomial 2" configuration */
       void  DrvTemperature_Config_Polynomial2_GetDefault               (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2      *ptr_data);
       void  DrvTemperature_Config_Polynomial2_Get                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2      *ptr_data);
       void  DrvTemperature_Config_Polynomial2_Set                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2      *ptr_data);
       bool  DrvTemperature_Config_Polynomial2_IsValid                  (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2      *ptr_data);

/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void  DrvTemperature_AdlCallback_Message_TaskMsg(u32 msg_identifier);

/* timer   callback functions */
static void  DrvTemperature_AdlCallback_Timer_TimerTempMeasurement(void *ptr_context);
static void  DrvTemperature_AdlCallback_Timer_TimerOut1Increase   (void *ptr_context);
static void  DrvTemperature_AdlCallback_Timer_TimerOut1Decrease   (void *ptr_context);
static void  DrvTemperature_AdlCallback_Timer_TimerOut2Increase   (void *ptr_context);
static void  DrvTemperature_AdlCallback_Timer_TimerOut2Decrease   (void *ptr_context);




/*=============================================================================
 * Function   : DrvTemperature_TaskDrvTemperature
 *
 * Description: "driver temperature" function task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvTemperature_TaskDrvTemperature(void *argument)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - DRV_TEMPERATURE - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* creation of timer "TimerTempMeasurement" */
    err = ql_rtos_timer_create(&DrvTemperature_TimerHandler_TimerTempMeasurement, QL_TIMER_IN_SERVICE, DrvTemperature_AdlCallback_Timer_TimerTempMeasurement, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOut1Increase" */
    err = ql_rtos_timer_create(&DrvTemperature_TimerHandler_TimerOut1Increase, QL_TIMER_IN_SERVICE, DrvTemperature_AdlCallback_Timer_TimerOut1Increase, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOut1Decrease" */
    err = ql_rtos_timer_create(&DrvTemperature_TimerHandler_TimerOut1Decrease, QL_TIMER_IN_SERVICE, DrvTemperature_AdlCallback_Timer_TimerOut1Decrease, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOut2Increase" */
    err = ql_rtos_timer_create(&DrvTemperature_TimerHandler_TimerOut2Increase, QL_TIMER_IN_SERVICE, DrvTemperature_AdlCallback_Timer_TimerOut2Increase, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerOut2Decrease" */
    err = ql_rtos_timer_create(&DrvTemperature_TimerHandler_TimerOut2Decrease, QL_TIMER_IN_SERVICE, DrvTemperature_AdlCallback_Timer_TimerOut2Decrease, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* internal status init */
    DrvTemperature_InitVar();


    /* sensor status */
    DrvTemperature_SensorStatusInt = DRVTEMPERATURE__SENSOR__UNKNOWN;
    DrvTemperature_SensorStatusExt = DRVTEMPERATURE__SENSOR__UNKNOWN;

    /* internal/external temperatures (/10 �C) */
    DrvTemperature_TemperatureInt = 0;
    DrvTemperature_TemperatureExt = 0;

    /* internal/external ADC value */
    DrvTemperature_AdcValueInt = 0;
    DrvTemperature_AdcValueExt = 0;


    /* start the temperature measurement periodic timer */
    DrvTemperature_TimerTempMeasurement_Start(TIME_MS__TEMP_MEASUREMENT_PERIOD, TRUE);


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            DrvTemperature_AdlCallback_Message_TaskMsg(event.id);
        }
    }
}




/*=============================================================================
 * Function   : DrvTemperature_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_InitVar(void)
{
    bool recover_status;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* status of internal temperature compensation for enabling of output */
        DrvTemperature_Out1Increase = FALSE;
        DrvTemperature_Out1Decrease = FALSE;
        DrvTemperature_Out2Increase = FALSE;
        DrvTemperature_Out2Decrease = FALSE;

        /* internal temperature table index for offset for enabling of output */
        DrvTemperature_IndexOffsetOut1 = 0;
        DrvTemperature_IndexOffsetOut2 = 0;

        /* internal temperature offset for enabling of output (/10 �C) */
        DrvTemperature_TemperatureOffsetOut1 = 0;
        DrvTemperature_TemperatureOffsetOut2 = 0;

        /* timer info */
        DrvTemperature_TimerOut1Increase_Info.status         = FALSE;
        DrvTemperature_TimerOut1Increase_Info.periodic       = FALSE;
        DrvTemperature_TimerOut1Increase_Info.time_value     = 0;
        DrvTemperature_TimerOut1Increase_Info.time_remaining = 0;
        DrvTemperature_TimerOut1Increase_Info.time_now       = 0;

        DrvTemperature_TimerOut1Decrease_Info.status         = FALSE;
        DrvTemperature_TimerOut1Decrease_Info.periodic       = FALSE;
        DrvTemperature_TimerOut1Decrease_Info.time_value     = 0;
        DrvTemperature_TimerOut1Decrease_Info.time_remaining = 0;
        DrvTemperature_TimerOut1Decrease_Info.time_now       = 0;

        DrvTemperature_TimerOut2Increase_Info.status         = FALSE;
        DrvTemperature_TimerOut2Increase_Info.periodic       = FALSE;
        DrvTemperature_TimerOut2Increase_Info.time_value     = 0;
        DrvTemperature_TimerOut2Increase_Info.time_remaining = 0;
        DrvTemperature_TimerOut2Increase_Info.time_now       = 0;

        DrvTemperature_TimerOut2Decrease_Info.status         = FALSE;
        DrvTemperature_TimerOut2Decrease_Info.periodic       = FALSE;
        DrvTemperature_TimerOut2Decrease_Info.time_value     = 0;
        DrvTemperature_TimerOut2Decrease_Info.time_remaining = 0;
        DrvTemperature_TimerOut2Decrease_Info.time_now       = 0;


        /**** update the variable CRC ****/
        DrvTemperature_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        // NOTHING TO DO
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // start the active timers with the remaining time

        if (DrvTemperature_TimerOut1Increase_Info.status)
        {
            if (DrvTemperature_TimerOut1Increase_Info.periodic)
            {
                DrvTemperature_TimerOut1Increase_Start(DrvTemperature_TimerOut1Increase_Info.time_value, TRUE);
            }
            else
            {
                if (DrvTemperature_TimerOut1Increase_Info.time_remaining > 0)
                    DrvTemperature_TimerOut1Increase_Start(DrvTemperature_TimerOut1Increase_Info.time_remaining, FALSE);
            }
        }

        if (DrvTemperature_TimerOut1Decrease_Info.status)
        {
            if (DrvTemperature_TimerOut1Decrease_Info.periodic)
            {
                DrvTemperature_TimerOut1Decrease_Start(DrvTemperature_TimerOut1Decrease_Info.time_value, TRUE);
            }
            else
            {
                if (DrvTemperature_TimerOut1Decrease_Info.time_remaining > 0)
                    DrvTemperature_TimerOut1Decrease_Start(DrvTemperature_TimerOut1Decrease_Info.time_remaining, FALSE);
            }
        }

        if (DrvTemperature_TimerOut2Increase_Info.status)
        {
            if (DrvTemperature_TimerOut2Increase_Info.periodic)
            {
                DrvTemperature_TimerOut2Increase_Start(DrvTemperature_TimerOut2Increase_Info.time_value, TRUE);
            }
            else
            {
                if (DrvTemperature_TimerOut2Increase_Info.time_remaining > 0)
                    DrvTemperature_TimerOut2Increase_Start(DrvTemperature_TimerOut2Increase_Info.time_remaining, FALSE);
            }
        }

        if (DrvTemperature_TimerOut2Decrease_Info.status)
        {
            if (DrvTemperature_TimerOut2Decrease_Info.periodic)
            {
                DrvTemperature_TimerOut2Decrease_Start(DrvTemperature_TimerOut2Decrease_Info.time_value, TRUE);
            }
            else
            {
                if (DrvTemperature_TimerOut2Decrease_Info.time_remaining > 0)
                    DrvTemperature_TimerOut2Decrease_Start(DrvTemperature_TimerOut2Decrease_Info.time_remaining, FALSE);
            }
        }


        /**** other ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : DrvTemperature_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 DrvTemperature_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_Out1Increase          , sizeof(DrvTemperature_Out1Increase          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_Out1Decrease          , sizeof(DrvTemperature_Out1Decrease          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_Out2Increase          , sizeof(DrvTemperature_Out2Increase          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_Out2Decrease          , sizeof(DrvTemperature_Out2Decrease          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_IndexOffsetOut1       , sizeof(DrvTemperature_IndexOffsetOut1       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_IndexOffsetOut2       , sizeof(DrvTemperature_IndexOffsetOut2       ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_TemperatureOffsetOut1 , sizeof(DrvTemperature_TemperatureOffsetOut1 ), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_TemperatureOffsetOut2 , sizeof(DrvTemperature_TemperatureOffsetOut2 ), crc);

    /* timers info */
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_TimerOut1Increase_Info, sizeof(DrvTemperature_TimerOut1Increase_Info), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_TimerOut1Decrease_Info, sizeof(DrvTemperature_TimerOut1Decrease_Info), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_TimerOut2Increase_Info, sizeof(DrvTemperature_TimerOut2Increase_Info), crc);
    crc = Utility_CalculateCRC16((u8 *)&DrvTemperature_TimerOut2Decrease_Info, sizeof(DrvTemperature_TimerOut2Decrease_Info), crc);

    return crc;
}




/*=============================================================================
 * Function   : DrvTemperature_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvTemperature_UpdateVarCrc(void)
{
    DrvTemperature_VarCrc = DrvTemperature_CalculateVarCrc();
}




/*=============================================================================
 * Function   : DrvTemperature_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool DrvTemperature_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = DrvTemperature_CalculateVarCrc();

    if (DrvTemperature_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*=============================================================================
 * Function   : DrvTemperature_OutputOut1On
 *
 * Description: signal output OUT1 on
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvTemperature_OutputOut1On(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__OUT1_ON;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_OutputOut1Off
 *
 * Description: signal output OUT1 off
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvTemperature_OutputOut1Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__OUT1_OFF;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_OutputOut2On
 *
 * Description: signal output OUT2 on
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvTemperature_OutputOut2On(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__OUT2_ON;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_OutputOut2Off
 *
 * Description: signal output OUT2 off
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DrvTemperature_OutputOut2Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__OUT2_OFF;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_GetInternalTemperature
 *
 * Description: get the actual internal temperature
 * Input      : - ptr_sensor_status: pointer to save the actual internal sensor status
 *              - ptr_temperature  : pointer to save the actual internal temperature (/10 �C)
 *              - ptr_adc_value    : pointer to save the actual internal ADC value
 * Output     : - FALSE: internal temperature not got
 *              - TRUE : internal temperature     got
 *=============================================================================*/
bool DrvTemperature_GetInternalTemperature(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value)
{
#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__NO )
    /* not temperature exchange */
    *ptr_sensor_status = DrvTemperature_SensorStatusInt;
    *ptr_temperature   = DrvTemperature_TemperatureInt;
    *ptr_adc_value     = DrvTemperature_AdcValueInt;
#endif

#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__YES)
    /*     temperature exchange */
    *ptr_sensor_status = DrvTemperature_SensorStatusExt;
    *ptr_temperature   = DrvTemperature_TemperatureExt;
    *ptr_adc_value     = DrvTemperature_AdcValueExt;
#endif


    return TRUE;
}




/*=============================================================================
 * Function   : DrvTemperature_GetExternalTemperature
 *
 * Description: get the actual external temperature
 * Input      : - ptr_sensor_status: pointer to save the actual external sensor status
 *              - ptr_temperature  : pointer to save the actual external temperature (/10 �C)
 *              - ptr_adc_value    : pointer to save the actual internal ADC value
 * Output     : - FALSE: external temperature not got
 *              - TRUE : external temperature     got
 *=============================================================================*/
bool DrvTemperature_GetExternalTemperature(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value)
{
#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__NO )
    /* not temperature exchange */
    *ptr_sensor_status = DrvTemperature_SensorStatusExt;
    *ptr_temperature   = DrvTemperature_TemperatureExt;
    *ptr_adc_value     = DrvTemperature_AdcValueExt;
#endif

#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__YES)
    /*     temperature exchange */
    *ptr_sensor_status = DrvTemperature_SensorStatusInt;
    *ptr_temperature   = DrvTemperature_TemperatureInt;
    *ptr_adc_value     = DrvTemperature_AdcValueInt;
#endif


    return TRUE;
}




/*=============================================================================
 * Function   : DrvTemperature_ReadInternalTemperature
 *
 * Description: read the actual internal temperature
 * Input      : - ptr_sensor_status: pointer to save the actual internal sensor status
 *              - ptr_temperature  : pointer to save the actual internal temperature (/10 �C)
 *              - ptr_adc_value    : pointer to save the actual internal ADC value
 * Output     : - FALSE: internal temperature not got
 *              - TRUE : internal temperature     got
 *=============================================================================*/
static bool DrvTemperature_ReadInternalTemperature(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value)
{
    bool result;


#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__NO )
    /* not temperature exchange */
    result = DrvTemperature_ReadInternalTemperature2(ptr_sensor_status, ptr_temperature, ptr_adc_value);
#endif

#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__YES)
    /*     temperature exchange */
    result = DrvTemperature_ReadExternalTemperature2(ptr_sensor_status, ptr_temperature, ptr_adc_value);
#endif


    return result;
}




/*=============================================================================
 * Function   : DrvTemperature_ReadExternalTemperature
 *
 * Description: read the actual external temperature
 * Input      : - ptr_sensor_status: pointer to save the actual external sensor status
 *              - ptr_temperature  : pointer to save the actual external temperature (/10 �C)
 *              - ptr_adc_value    : pointer to save the actual external ADC value
 * Output     : - FALSE: external temperature not got
 *              - TRUE : external temperature     got
 *=============================================================================*/
static bool DrvTemperature_ReadExternalTemperature(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value)
{
    bool result;


#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__NO )
    /* not temperature exchange */
    result = DrvTemperature_ReadExternalTemperature2(ptr_sensor_status, ptr_temperature, ptr_adc_value);
#endif

#if (FW_CONFIG__TEST__TEMP_EXCHANGE == FW_CONFIG__TEST__TEMP_EXCHANGE__YES)
    /*     temperature exchange */
    result = DrvTemperature_ReadInternalTemperature2(ptr_sensor_status, ptr_temperature, ptr_adc_value);
#endif


    return result;
}




/*=============================================================================
 * Function   : DrvTemperature_ReadInternalTemperature2
 *
 * Description: read the actual internal temperature
 * Input      : - ptr_sensor_status: pointer to save the actual internal sensor status
 *              - ptr_temperature  : pointer to save the actual internal temperature (/10 �C)
 *              - ptr_adc_value    : pointer to save the actual internal ADC value
 * Output     : - FALSE: internal temperature not got
 *              - TRUE : internal temperature     got
 *=============================================================================*/
static bool DrvTemperature_ReadInternalTemperature2(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value)
{
    /* sensor status */
    u8          sensor_status;

    /* temperatures (/10 �C) */
    s16         temperature;

    ascii       temp_string[10 + 1];

    u16         adc_raw_value;
    u16         adc_mv_value;
    float       R_ntc;

    TABLE_NTC  *ptr_table_ntc;
    u8          size_table_ntc;

    bool        result;

#if (FW_CONFIG__TEST__TEMP_COMPENSATION == FW_CONFIG__TEST__TEMP_COMPENSATION__YES)

#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
    s16         offset_1;
    s16         offset_2;
    s16         offset_3;
#endif

#endif


    /* read the NTC internal temperature signal (ADC0EXT) */
    result = Adc_ReadAdcChannel(ADC__CHANNEL_ID__ADC0__ANALOG_SIGNAL_1, NUMBER_OF_ADC_READINGS, DELAY_BEETWEEN_ADC_READINGS, &adc_raw_value, &adc_mv_value);
    if (!result)
    {
        sensor_status                  = DRVTEMPERATURE__SENSOR__UNKNOWN;
        temperature                    = 0;
        adc_mv_value                   = 0;

        DrvTemperature_SensorStatusInt = sensor_status;
        DrvTemperature_TemperatureInt  = temperature;
        DrvTemperature_AdcValueInt     = adc_mv_value;

        *ptr_sensor_status             = sensor_status;
        *ptr_temperature               = temperature;
        *ptr_adc_value                 = adc_mv_value;

        return FALSE;
    }


    /* print value of ADC internal value */
    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "ADC int value: %d", adc_mv_value);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* calculate the NTC internal temperature value (ohm) */
    /*
     *                  ADC * FS
     * R_ntc = R -----------------------
     *            4096 Vdd - (ADC * FS)
     */
    R_ntc = RESISTOR__R  *  (((float)adc_mv_value * VOLTAGE__ADC_FS)  /  ((4096.0 * VOLTAGE__VDD) - ((float)adc_mv_value * VOLTAGE__ADC_FS)));


    /* print value of NTC internal temperature (Ohm) */
    Utility_FloatToString(R_ntc, temp_string, 1);
    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "NTC int: %s (Ohm)", temp_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check if the sensor status */
    if      (R_ntc <= SENSOR_RESISTOR__SHORT_CIRCUITED)
    {
        /* internal sensor short-circuit */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Internal sensor short-circuited", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        sensor_status = DRVTEMPERATURE__SENSOR__OUT_OF_ORDER;
        temperature   = 0;
    }
    else if (R_ntc >= SENSOR_RESISTOR__DISCONNECTED)
    {
        /* internal sensor disconnected */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Internal sensor disconnected", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        sensor_status = DRVTEMPERATURE__SENSOR__DISCONNECTED;
        temperature   = -350;   // minimum temperature (/10 �C)
    }
    else
    {
        /* internal sensor connected */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Internal sensor connected", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* interpolation */
        if (DrvTemperature_Config_TemperatureMode.mode == DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_TABLE)
        {
            /* table      interpolation */

            ptr_table_ntc  = (TABLE_NTC *)&DrvTemperature_Table_Ntc;
            size_table_ntc = TABLE_NTC_SIZE;

            sensor_status = DRVTEMPERATURE__SENSOR__CONNECTED;
            temperature   =       DrvTemperature_InterpolationTableInt  ((u32)R_ntc, ptr_table_ntc, size_table_ntc);
        }
        else
        {
            /* polynomial interpolation */

            sensor_status = DRVTEMPERATURE__SENSOR__CONNECTED;
            temperature   = (s16)(DrvTemperature_InterpolationPolynomial(R_ntc) * 10.0);
        }


        /* print the internal temperature (without probe calibration) */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "internal temperature (no   calibration): %d (/10 �C)", temperature);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* update the internal temperature according to internal temperatures calibration */
        temperature += DrvTemperature_Config_CalibrationInt.calibration;

        /* print the internal temperature (with    probe calibration) */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "internal temperature (with calibration): %d (/10 �C)", temperature);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* update the internal temperature according to internal temperature compensation */
#if (FW_CONFIG__TEST__TEMP_COMPENSATION == FW_CONFIG__TEST__TEMP_COMPENSATION__YES)

#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LFVW)
        /* calculus of temperature compensation for constant contribute */
        if (temperature >= ((s16)X2))
            offset_1 = -((s16)TEMPERATURE_OFFSET);
        else
            offset_1 = -((s16)((((Y2 - Y1) / (X2 - X1)) * ((float)temperature - X1))  +  Y1));
        if (offset_1 < -47)
            offset_1 = -47;

        /* calculus of temperature compensation for enabling of output */
        if (DrvTemperature_TemperatureOffsetOut1 <= TABLE_OFFSET_OUT_1_SIZE)
            offset_2 = -((s16)(STEP_OFFSET_OUT_1 * DrvTemperature_TemperatureOffsetOut1));
        else
            offset_2 = 0;
        if (DrvTemperature_TemperatureOffsetOut2 <= TABLE_OFFSET_OUT_2_SIZE)
            offset_3 = -((s16)(STEP_OFFSET_OUT_2 * DrvTemperature_TemperatureOffsetOut2));
        else
            offset_3 = 0;

        /* temperature compensation */
        temperature += (offset_1 + offset_2 + offset_3);


        /* print the internal temperature offset  for constant contribute */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "internal temperatures offsets for constant contribute: %d (/10 �C)", offset_1);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* print the internal temperature offsets for enabling of output */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "internal temperatures offsets for outputs: %d, %d (/10 �C)", offset_2, offset_3);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* print the internal temperature (with offset for temperature compensation) */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "internal temperature (with offset for temperature compensation): %d (/10 �C)", temperature);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
#endif

#endif
    }


    DrvTemperature_SensorStatusInt = sensor_status;
    DrvTemperature_TemperatureInt  = temperature;
    DrvTemperature_AdcValueInt     = adc_mv_value;

    *ptr_sensor_status             = sensor_status;
    *ptr_temperature               = temperature;
    *ptr_adc_value                 = adc_mv_value;

    return TRUE;
}




/*=============================================================================
 * Function   : DrvTemperature_ReadExternalTemperature2
 *
 * Description: read the actual external temperature
 * Input      : - ptr_sensor_status: pointer to save the actual external sensor status
 *              - ptr_temperature  : pointer to save the actual external temperature (/10 �C)
 *              - ptr_adc_value    : pointer to save the actual internal ADC value
 * Output     : - FALSE: external temperature not got
 *              - TRUE : external temperature     got
 *=============================================================================*/
static bool DrvTemperature_ReadExternalTemperature2(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value)
{
    /* sensor status */
    u8          sensor_status;

    /* temperatures (/10 �C) */
    s16         temperature;

    ascii       temp_string[10 + 1];

    u16         adc_raw_value;
    u16         adc_mv_value;
    float       R_ntc;

    TABLE_NTC  *ptr_table_ntc;
    u8          size_table_ntc;

    bool        result;


    /* read the NTC external temperature signal (ADC1EXT) */
    result = Adc_ReadAdcChannel(ADC__CHANNEL_ID__ADC1__ANALOG_SIGNAL_2, NUMBER_OF_ADC_READINGS, DELAY_BEETWEEN_ADC_READINGS, &adc_raw_value, &adc_mv_value);
    if (!result)
    {
        sensor_status                  = DRVTEMPERATURE__SENSOR__UNKNOWN;
        temperature                    = 0;
        adc_mv_value                   = 0;

        DrvTemperature_SensorStatusExt = sensor_status;
        DrvTemperature_TemperatureExt  = temperature;
        DrvTemperature_AdcValueInt     = adc_mv_value;

        *ptr_sensor_status             = sensor_status;
        *ptr_temperature               = temperature;
        *ptr_adc_value                 = adc_mv_value;

        return FALSE;
    }


    /* print value of ADC external value */
    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "ADC ext value: %d", adc_mv_value);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* calculate the NTC external temperature value (ohm) */
    /*
     *                  ADC * FS
     * R_ntc = R -----------------------
     *            4096 Vdd - (ADC * FS)
     */
    R_ntc = RESISTOR__R  *  (((float)adc_mv_value * VOLTAGE__ADC_FS)  /  ((4096.0 * VOLTAGE__VDD) - ((float)adc_mv_value * VOLTAGE__ADC_FS)));


    /* print value of NTC external temperature (Ohm) */
    Utility_FloatToString(R_ntc, temp_string, 1);
    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "NTC ext: %s (Ohm)", temp_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check if the sensor status */
    if      (R_ntc <= SENSOR_RESISTOR__SHORT_CIRCUITED)
    {
        /* external sensor short-circuit */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "External sensor short-circuited", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        sensor_status = DRVTEMPERATURE__SENSOR__OUT_OF_ORDER;
        temperature   = 0;
    }
    else if (R_ntc >= SENSOR_RESISTOR__DISCONNECTED)
    {
        /* external sensor disconnected */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "External sensor disconnected", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        sensor_status = DRVTEMPERATURE__SENSOR__DISCONNECTED;
        temperature   = 0;
    }
    else
    {
        /* external sensor connected */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "External sensor connected", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* interpolation */
        if (DrvTemperature_Config_TemperatureMode.mode == DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_TABLE)
        {
            /* table      interpolation */

            ptr_table_ntc  = (TABLE_NTC *)&DrvTemperature_Table_Ntc;
            size_table_ntc = TABLE_NTC_SIZE;

            sensor_status = DRVTEMPERATURE__SENSOR__CONNECTED;
            temperature   =       DrvTemperature_InterpolationTableInt  ((u32)R_ntc, ptr_table_ntc, size_table_ntc);
        }
        else
        {
            /* polynomial interpolation */

            sensor_status = DRVTEMPERATURE__SENSOR__CONNECTED;
            temperature   = (s16)(DrvTemperature_InterpolationPolynomial(R_ntc) * 10.0);
        }


        /* print the external temperature (without probe calibration) */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "external temperature (no   calibration): %d (/10 �C)", temperature);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* update the external temperature according to external temperatures calibration */
        temperature += DrvTemperature_Config_CalibrationExt.calibration;

        /* print the external temperature (with    probe calibration) */
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "external temperature (with calibration): %d (/10 �C)", temperature);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    DrvTemperature_SensorStatusExt = sensor_status;
    DrvTemperature_TemperatureExt  = temperature;
    DrvTemperature_AdcValueExt     = adc_mv_value;

    *ptr_sensor_status             = sensor_status;
    *ptr_temperature               = temperature;
    *ptr_adc_value                 = adc_mv_value;

    return TRUE;
}




/*=============================================================================
 * Function   : DrvTemperature_InterpolationPolynomial
 *
 * Description:
 * Input      : - x:
 * Output     : -
 *=============================================================================*/
static float DrvTemperature_InterpolationPolynomial(float x)
{
    float y;
    float z;


    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Value x: %f", x);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* polynomial interpolation - polynomial 1 */
    y = Utility_InterpolationPolynomial((float *)DrvTemperature_Config_Polynomial1.poly_coeff, DRVTEMPERATURE__POLYNOMIAL_1_DEGREE, x);
    if (y <  -25.0)  //  -25.0 �C
        y =  -25.0;
    if (y > +105.0)  // +105.0 �C
        y = +105.0;


    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Value y: %f", y);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* polynomial interpolation - polynomial 2 */
    z = Utility_InterpolationPolynomial((float *)DrvTemperature_Config_Polynomial2.poly_coeff, DRVTEMPERATURE__POLYNOMIAL_2_DEGREE, y);
    if (z <  -25.0)  //  -25.0 �C
        z =  -25.0;
    if (z > +105.0)  // +105.0 �C
        z = +105.0;


    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Value z: %f", z);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return z;
}




/*=============================================================================
 * Function   : DrvTemperature_InterpolationTableInt
 *
 * Description:
 * Input      : - x             :
 *              - ptr_table_ntc :
 *              - size_table_ntc:
 * Output     : -
 *=============================================================================*/
static s16 DrvTemperature_InterpolationTableInt(u32 x, TABLE_NTC *ptr_table_ntc, u8 size_table_ntc)
{
    s16 y;


    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Value x: %ld", x);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* table interpolation */
    if (size_table_ntc > 0)
    {
        y = Utility_InterpolationTableInt((u32 *)ptr_table_ntc->resistor,
                                          (s16 *)ptr_table_ntc->temperature,
                                                 size_table_ntc,
                                                 x);
        if (y < ptr_table_ntc->temperature[size_table_ntc - 1])   // -40.0 �C
            y = ptr_table_ntc->temperature[size_table_ntc - 1];
        if (y > ptr_table_ntc->temperature[0                 ])   // 105.0 �C
            y = ptr_table_ntc->temperature[0                 ];
    }
    else
    {
        y = 0;
    }


    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Value y: %d", y);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return y;
}




/*=============================================================================
 * Function   : DrvTemperature_RegisterSignalNewTemperatures
 *
 * Description: register a function to be called when there are new available temperatures
 * Input      : - ptr_funcion: pointer to the function to be called
 * Output     : -
 *=============================================================================*/
void DrvTemperature_RegisterSignalNewTemperatures(void (*ptr_function)(u8, s16, u8, s16))
{
    if (DrvTemperature_NumberSignalsNewTemperatures < NUM_MAX_SIGNALS_NEW_TEMPERATURES)
    {
        DrvTemperature_SignalsNewTemperatures[DrvTemperature_NumberSignalsNewTemperatures].ptr_function = ptr_function;
        DrvTemperature_NumberSignalsNewTemperatures++;
    }
}




/*=============================================================================
 * Function   : DrvTemperature_RegisterSignalNewAdcValues
 *
 * Description: register a function to be called when there are new available ADC values
 * Input      : - ptr_funcion: pointer to the function to be called
 * Output     : -
 *=============================================================================*/
void DrvTemperature_RegisterSignalNewAdcValues(void (*ptr_function)(u16, u16))
{
    if (DrvTemperature_NumberSignalsNewAdcValues < NUM_MAX_SIGNALS_NEW_ADC_VALUES)
    {
        DrvTemperature_SignalsNewAdcValues[DrvTemperature_NumberSignalsNewAdcValues].ptr_function = ptr_function;
        DrvTemperature_NumberSignalsNewAdcValues++;
    }
}




/*=============================================================================
 * Function   : DrvTemperature_TimerTempMeasurement_Start
 *
 * Description: start the "temperature measurement" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_TimerTempMeasurement_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start \"temperature measurement\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(DrvTemperature_TimerHandler_TimerTempMeasurement, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut1Increase_Start
 *
 * Description: start the "OUT1 increase" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_TimerOut1Increase_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start \"OUT1 increase\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(DrvTemperature_TimerHandler_TimerOut1Increase, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        DrvTemperature_TimerOut1Increase_Info.status         = TRUE;
        DrvTemperature_TimerOut1Increase_Info.periodic       = periodic;
        DrvTemperature_TimerOut1Increase_Info.time_value     = time_value;
        DrvTemperature_TimerOut1Increase_Info.time_remaining = time_value;
        DrvTemperature_TimerOut1Increase_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut1Increase_Stop
 *
 * Description: stop the "OUT1 increase" timer
 * Input      : -
 * Output     : - remaining time (ms)
 *=============================================================================*/
static u32 DrvTemperature_TimerOut1Increase_Stop(void)
{
    QlOSStatus err;
    u32        remaining_time_ms = 0;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Stop \"OUT1 increase\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(DrvTemperature_TimerHandler_TimerOut1Increase);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        remaining_time_ms = ql_rtos_get_system_tick() - DrvTemperature_TimerOut1Increase_Info.time_now;

        DrvTemperature_TimerOut1Increase_Info.status         = FALSE;
        DrvTemperature_TimerOut1Increase_Info.periodic       = FALSE;
        DrvTemperature_TimerOut1Increase_Info.time_value     = 0;
        DrvTemperature_TimerOut1Increase_Info.time_remaining = 0;
        DrvTemperature_TimerOut1Increase_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    return remaining_time_ms;
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut1Decrease_Start
 *
 * Description: start the "OUT1 decrease" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_TimerOut1Decrease_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start \"OUT1 decrease\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(DrvTemperature_TimerHandler_TimerOut1Decrease, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        DrvTemperature_TimerOut1Decrease_Info.status         = TRUE;
        DrvTemperature_TimerOut1Decrease_Info.periodic       = periodic;
        DrvTemperature_TimerOut1Decrease_Info.time_value     = time_value;
        DrvTemperature_TimerOut1Decrease_Info.time_remaining = time_value;
        DrvTemperature_TimerOut1Decrease_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut1Decrease_Stop
 *
 * Description: stop the "OUT1 decrease" timer
 * Input      : -
 * Output     : - remaining time (ms)
 *=============================================================================*/
static u32 DrvTemperature_TimerOut1Decrease_Stop(void)
{
    QlOSStatus err;
    u32        remaining_time_ms = 0;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Stop \"OUT1 decrease\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(DrvTemperature_TimerHandler_TimerOut1Decrease);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        remaining_time_ms = ql_rtos_get_system_tick() - DrvTemperature_TimerOut1Decrease_Info.time_now;

        DrvTemperature_TimerOut1Decrease_Info.status         = FALSE;
        DrvTemperature_TimerOut1Decrease_Info.periodic       = FALSE;
        DrvTemperature_TimerOut1Decrease_Info.time_value     = 0;
        DrvTemperature_TimerOut1Decrease_Info.time_remaining = 0;
        DrvTemperature_TimerOut1Decrease_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    return remaining_time_ms;
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut2Increase_Start
 *
 * Description: start the "OUT2 increase" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_TimerOut2Increase_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start \"OUT2 increase\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(DrvTemperature_TimerHandler_TimerOut2Increase, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        DrvTemperature_TimerOut2Increase_Info.status         = TRUE;
        DrvTemperature_TimerOut2Increase_Info.periodic       = periodic;
        DrvTemperature_TimerOut2Increase_Info.time_value     = time_value;
        DrvTemperature_TimerOut2Increase_Info.time_remaining = time_value;
        DrvTemperature_TimerOut2Increase_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut2Increase_Stop
 *
 * Description: stop the "OUT2 increase" timer
 * Input      : -
 * Output     : - remaining time (ms)
 *=============================================================================*/
static u32 DrvTemperature_TimerOut2Increase_Stop(void)
{
    QlOSStatus err;
    u32        remaining_time_ms = 0;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Stop \"OUT2 increase\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(DrvTemperature_TimerHandler_TimerOut2Increase);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        remaining_time_ms = ql_rtos_get_system_tick() - DrvTemperature_TimerOut2Increase_Info.time_now;

        DrvTemperature_TimerOut2Increase_Info.status         = FALSE;
        DrvTemperature_TimerOut2Increase_Info.periodic       = FALSE;
        DrvTemperature_TimerOut2Increase_Info.time_value     = 0;
        DrvTemperature_TimerOut2Increase_Info.time_remaining = 0;
        DrvTemperature_TimerOut2Increase_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    return remaining_time_ms;
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut2Decrease_Start
 *
 * Description: start the "OUT2 decrease" timer
 * Input      : - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_TimerOut2Decrease_Start(u32 time_value, bool periodic)
{
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Start \"OUT2 decrease\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(DrvTemperature_TimerHandler_TimerOut2Decrease, time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        DrvTemperature_TimerOut2Decrease_Info.status         = TRUE;
        DrvTemperature_TimerOut2Decrease_Info.periodic       = periodic;
        DrvTemperature_TimerOut2Decrease_Info.time_value     = time_value;
        DrvTemperature_TimerOut2Decrease_Info.time_remaining = time_value;
        DrvTemperature_TimerOut2Decrease_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : DrvTemperature_TimerOut2Decrease_Stop
 *
 * Description: stop the "OUT2 decrease" timer
 * Input      : -
 * Output     : - remaining time (ms)
 *=============================================================================*/
static u32 DrvTemperature_TimerOut2Decrease_Stop(void)
{
    QlOSStatus err;
    u32        remaining_time_ms = 0;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Stop \"OUT2 decrease\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(DrvTemperature_TimerHandler_TimerOut2Decrease);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    if (err == QL_OSI_SUCCESS)
    {
        remaining_time_ms = ql_rtos_get_system_tick() - DrvTemperature_TimerOut2Decrease_Info.time_now;

        DrvTemperature_TimerOut2Decrease_Info.status         = FALSE;
        DrvTemperature_TimerOut2Decrease_Info.periodic       = FALSE;
        DrvTemperature_TimerOut2Decrease_Info.time_value     = 0;
        DrvTemperature_TimerOut2Decrease_Info.time_remaining = 0;
        DrvTemperature_TimerOut2Decrease_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    return remaining_time_ms;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationInt_GetDefault
 *
 * Description: get the default "internal temperature calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_CalibrationInt_GetDefault(DRVTEMPERATURE__CONFIG__CALIBRATION_INT *ptr_data)
{
    *ptr_data = DrvTemperature_Config_CalibrationIntDefault;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationInt_Get
 *
 * Description: get the "internal temperature calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_CalibrationInt_Get(DRVTEMPERATURE__CONFIG__CALIBRATION_INT *ptr_data)
{
    *ptr_data = DrvTemperature_Config_CalibrationInt;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationInt_Set
 *
 * Description: set the "internal temperature calibration" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_CalibrationInt_Set(DRVTEMPERATURE__CONFIG__CALIBRATION_INT *ptr_data)
{
    DrvTemperature_Config_CalibrationInt = *ptr_data;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationInt_IsValid
 *
 * Description: check if the "internal temperature calibration" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DrvTemperature_Config_CalibrationInt_IsValid(DRVTEMPERATURE__CONFIG__CALIBRATION_INT *ptr_data)
{
    if (
         (ptr_data->calibration < -50) ||
         (ptr_data->calibration > +50)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationExt_GetDefault
 *
 * Description: get the default "external temperature calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_CalibrationExt_GetDefault(DRVTEMPERATURE__CONFIG__CALIBRATION_EXT *ptr_data)
{
    *ptr_data = DrvTemperature_Config_CalibrationExtDefault;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationExt_Get
 *
 * Description: get the "external temperature calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_CalibrationExt_Get(DRVTEMPERATURE__CONFIG__CALIBRATION_EXT *ptr_data)
{
    *ptr_data = DrvTemperature_Config_CalibrationExt;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationExt_Set
 *
 * Description: set the "external temperature calibration" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_CalibrationExt_Set(DRVTEMPERATURE__CONFIG__CALIBRATION_EXT *ptr_data)
{
    DrvTemperature_Config_CalibrationExt = *ptr_data;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_CalibrationExt_IsValid
 *
 * Description: check if the "external temperature calibration" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DrvTemperature_Config_CalibrationExt_IsValid(DRVTEMPERATURE__CONFIG__CALIBRATION_EXT *ptr_data)
{
    if (
         (ptr_data->calibration < -50) ||
         (ptr_data->calibration > +50)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_TemperatureMeasurementMode_GetDefault
 *
 * Description: get the default "temperature measurement mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_TemperatureMeasurementMode_GetDefault(DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data)
{
    *ptr_data = DrvTemperature_Config_TemperatureModeDefault;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_TemperatureMeasurementMode_Get
 *
 * Description: get the "temperature measurement mode" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_TemperatureMeasurementMode_Get(DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data)
{
    *ptr_data = DrvTemperature_Config_TemperatureMode;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_TemperatureMeasurementMode_Set
 *
 * Description: set the "temperature measurement mode" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_TemperatureMeasurementMode_Set(DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data)
{
    DrvTemperature_Config_TemperatureMode = *ptr_data;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_TemperatureMeasurementMode_IsValid
 *
 * Description: check if the "temperature measurement mode" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DrvTemperature_Config_TemperatureMeasurementMode_IsValid(DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data)
{
    if (
         (ptr_data->mode != DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_TABLE     ) &&
         (ptr_data->mode != DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_POLYNOMIAL)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial1_GetDefault
 *
 * Description: get the default "interpolation polynomial 1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_Polynomial1_GetDefault(DRVTEMPERATURE__CONFIG__POLYNOMIAL_1 *ptr_data)
{
    *ptr_data = DrvTemperature_Config_Polynomial1Default;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial1_Get
 *
 * Description: get the "interpolation polynomial 1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_Polynomial1_Get(DRVTEMPERATURE__CONFIG__POLYNOMIAL_1 *ptr_data)
{
    *ptr_data = DrvTemperature_Config_Polynomial1;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial1_Set
 *
 * Description: set the "interpolation polynomial 1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_Polynomial1_Set(DRVTEMPERATURE__CONFIG__POLYNOMIAL_1 *ptr_data)
{
    DrvTemperature_Config_Polynomial1 = *ptr_data;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial1_IsValid
 *
 * Description: check if the "interpolation polynomial 1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DrvTemperature_Config_Polynomial1_IsValid(DRVTEMPERATURE__CONFIG__POLYNOMIAL_1 *ptr_data)
{
    // EVERY VALUE IS VALID

    return TRUE;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial2_GetDefault
 *
 * Description: get the default "interpolation polynomial 2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_Polynomial2_GetDefault(DRVTEMPERATURE__CONFIG__POLYNOMIAL_2 *ptr_data)
{
    *ptr_data = DrvTemperature_Config_Polynomial2Default;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial2_Get
 *
 * Description: get the "interpolation polynomial 2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_Polynomial2_Get(DRVTEMPERATURE__CONFIG__POLYNOMIAL_2 *ptr_data)
{
    *ptr_data = DrvTemperature_Config_Polynomial2;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial2_Set
 *
 * Description: set the "interpolation polynomial 2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DrvTemperature_Config_Polynomial2_Set(DRVTEMPERATURE__CONFIG__POLYNOMIAL_2 *ptr_data)
{
    DrvTemperature_Config_Polynomial2 = *ptr_data;
}




/*===========================================================================
 * Function   : DrvTemperature_Config_Polynomial2_IsValid
 *
 * Description: check if the "interpolation polynomial 2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool DrvTemperature_Config_Polynomial2_IsValid(DRVTEMPERATURE__CONFIG__POLYNOMIAL_2 *ptr_data)
{
    // EVERY VALUE IS VALID

    return TRUE;
}




/*===========================================================================
 * Function   : DrvTemperature_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - msg_identifier:
 * Output     : -
 *===========================================================================*/
static void DrvTemperature_AdlCallback_Message_TaskMsg(u32 msg_identifier)
{
    /* status of internal temperature compensation for enabling of output */
    u8    old_out1_increase;
    u8    old_out1_decrease;
    u8    old_out2_increase;
    u8    old_out2_decrease;

    /* internal temperature table index for offset for enabling of output */
    u8    old_index_offset_out1;
    u8    old_index_offset_out2;

    /* internal temperature offset for enabling of output (/10 �C) */
    u8    old_temperature_offset_out1;
    u8    old_temperature_offset_out2;

    u8    new_sensor_status_int;
    u8    new_sensor_status_ext;

    s16   new_temperature_int;
    s16   new_temperature_ext;

    u16   new_adc_value_int;
    u16   new_adc_value_ext;

    u32   remaining_time_ms;
    u32   elapsed_time_ms;

    u32   time_increase_value_ms;
    u32   time_decrease_value_ms;

    float elapsed_time_factor;

    u8    i;


    /* debug */
    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "CALLBACK     - MESSAGE     - TASK DRV TEMPERATURE - msg identifier: %lu", msg_identifier);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* status of internal temperature compensation for enabling of output */
    old_out1_increase           = DrvTemperature_Out1Increase;
    old_out1_decrease           = DrvTemperature_Out1Decrease;
    old_out2_increase           = DrvTemperature_Out2Increase;
    old_out2_decrease           = DrvTemperature_Out2Decrease;

    /* internal temperature table index for offset for enabling of output */
    old_index_offset_out1       = DrvTemperature_IndexOffsetOut1;
    old_index_offset_out2       = DrvTemperature_IndexOffsetOut2;

    /* internal temperature offset for enabling of output (/10 �C) */
    old_temperature_offset_out1 = DrvTemperature_TemperatureOffsetOut1;
    old_temperature_offset_out2 = DrvTemperature_TemperatureOffsetOut2;



    switch (msg_identifier)
    {
        /* event - timeout temperature measurement */
        case TASK_MSG_ID__TIMEOUT_TEMP_MEASUREMENT:
            /* read actual internal and external temperatures */
            DrvTemperature_ReadInternalTemperature(&new_sensor_status_int, &new_temperature_int, &new_adc_value_int);
            DrvTemperature_ReadExternalTemperature(&new_sensor_status_ext, &new_temperature_ext, &new_adc_value_ext);


            DrvTemperature_SensorStatusInt = new_sensor_status_int;
            DrvTemperature_SensorStatusExt = new_sensor_status_ext;

            DrvTemperature_TemperatureInt  = new_temperature_int;
            DrvTemperature_TemperatureExt  = new_temperature_ext;

            DrvTemperature_AdcValueInt = new_adc_value_int;
            DrvTemperature_AdcValueExt = new_adc_value_ext;


            snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Int. sensor status: %d,  Int. temperature: %d (/10 �C),  Int. ADC value: %d", new_sensor_status_int, new_temperature_int, new_adc_value_int);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "Ext. sensor status: %d,  Ext. temperature: %d (/10 �C),  Ext. ADC value: %d", new_sensor_status_ext, new_temperature_ext, new_adc_value_ext);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


            /* signal the new available temperatures to the registered applications */
            for (i = 0; i < DrvTemperature_NumberSignalsNewTemperatures; i++)
                DrvTemperature_SignalsNewTemperatures[i].ptr_function(new_sensor_status_int, new_temperature_int, new_sensor_status_ext, new_temperature_ext);

            /* signal the new available ADC values to the registered applications */
            for (i = 0; i < DrvTemperature_NumberSignalsNewAdcValues; i++)
                DrvTemperature_SignalsNewAdcValues[i].ptr_function(new_adc_value_int, new_adc_value_ext);
            break;



        /* event - output OUT1 on */
        case TASK_MSG_ID__OUT1_ON:
            if (DrvTemperature_IndexOffsetOut1 > (TABLE_OFFSET_OUT_1_SIZE - 1))
                DrvTemperature_IndexOffsetOut1 = 0;

            if      (
                      (!DrvTemperature_Out1Increase) &&
                      (!DrvTemperature_Out1Decrease)
                    )
            {
                /* output compensation not in progress */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT1 compensation not in progress", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                if (DrvTemperature_IndexOffsetOut1 == 0)
                {
                    elapsed_time_factor = 1.0;

                    /* calculation of time for increase temperature offset */
                    time_increase_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut1.times_increase[DrvTemperature_IndexOffsetOut1] * 1000L)));

                    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT1 - index: %d, time_increase_value_ms: %ld", DrvTemperature_IndexOffsetOut1, time_increase_value_ms);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DrvTemperature_Out1Increase = TRUE;
                    DrvTemperature_Out1Decrease = FALSE;

                    /* start timer for increase temperature offset for enabling of output */
                    DrvTemperature_TimerOut1Increase_Start(time_increase_value_ms, FALSE);
                }
            }
            else if (
                      (!DrvTemperature_Out1Increase) &&
                      ( DrvTemperature_Out1Decrease)
                    )
            {
                /* output compensation in progress (decrease) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT1 compensation in progress - decrease", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut1Increase_Stop();

                remaining_time_ms      = DrvTemperature_TimerOut1Decrease_Stop();
                time_decrease_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut1.times_decrease[DrvTemperature_IndexOffsetOut1] * 1000L;

                if (remaining_time_ms <= time_decrease_value_ms)
                    elapsed_time_ms = time_decrease_value_ms - remaining_time_ms;
                else
                    elapsed_time_ms = time_decrease_value_ms;

                if (elapsed_time_ms < time_decrease_value_ms)
                    elapsed_time_factor = ((float)elapsed_time_ms) / ((float)time_decrease_value_ms);
                else
                    elapsed_time_factor = 1.0;

                /* calculation of time for increase temperature offset */
                time_increase_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut1.times_increase[DrvTemperature_IndexOffsetOut1] * 1000L)));

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT1 - index: %d, time_increase_value_ms: %ld", DrvTemperature_IndexOffsetOut1, time_increase_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_Out1Increase = TRUE;
                DrvTemperature_Out1Decrease = FALSE;

                /* start timer for increase temperature offset for enabling of output */
                DrvTemperature_TimerOut1Increase_Start(time_increase_value_ms, FALSE);
            }
            else if (
                      ( DrvTemperature_Out1Increase) &&
                      (!DrvTemperature_Out1Decrease)
                    )
            {
                /* output compensation in progress (increase) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT1 compensation in progress - increase", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                // NOTHING TO DO
            }
            else
            {
                /* output compensation anomaly */

                // NOTHING TO DO

                DrvTemperature_Out1Increase = FALSE;
                DrvTemperature_Out1Decrease = FALSE;

                DrvTemperature_IndexOffsetOut1 = 0;
            }
            break;


        /* event - output OUT1 off */
        case TASK_MSG_ID__OUT1_OFF:
            if (DrvTemperature_IndexOffsetOut1 > (TABLE_OFFSET_OUT_1_SIZE - 1))
                DrvTemperature_IndexOffsetOut1 = TABLE_OFFSET_OUT_1_SIZE - 1;

            if      (
                      (!DrvTemperature_Out1Increase) &&
                      (!DrvTemperature_Out1Decrease)
                    )
            {
                /* output compensation not in progress */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT1 compensation not in progress", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                if (DrvTemperature_IndexOffsetOut1 == TABLE_OFFSET_OUT_1_SIZE - 1)
                {
                    elapsed_time_factor = 1.0;

                    /* calculation of time for decrease temperature offset */
                    time_decrease_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut1.times_decrease[DrvTemperature_IndexOffsetOut1] * 1000L)));

                    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT1 - index: %d, time_decrease_value_ms: %ld", DrvTemperature_IndexOffsetOut1, time_decrease_value_ms);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DrvTemperature_Out1Increase = FALSE;
                    DrvTemperature_Out1Decrease = TRUE;

                    /* start timer for decrease temperature offset for enabling of output */
                    DrvTemperature_TimerOut1Decrease_Start(time_decrease_value_ms, FALSE);
                }
            }
            else if (
                      ( DrvTemperature_Out1Increase) &&
                      (!DrvTemperature_Out1Decrease)
                    )
            {
                /* output compensation in progress (increase) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT1 compensation in progress - increase", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut1Decrease_Stop();

                remaining_time_ms      = DrvTemperature_TimerOut1Increase_Stop();
                time_increase_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut1.times_increase[DrvTemperature_IndexOffsetOut1] * 1000L;

                if (remaining_time_ms <= time_increase_value_ms)
                    elapsed_time_ms = time_increase_value_ms - remaining_time_ms;
                else
                    elapsed_time_ms = time_increase_value_ms;

                if (elapsed_time_ms <= time_increase_value_ms)
                    elapsed_time_factor = ((float)elapsed_time_ms) / ((float)time_increase_value_ms);
                else
                    elapsed_time_factor = 1.0;

                /* calculation of time for decrease temperature offset */
                time_decrease_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut1.times_decrease[DrvTemperature_IndexOffsetOut1] * 1000L)));

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT1 - index: %d, time_decrease_value_ms: %ld", DrvTemperature_IndexOffsetOut1, time_decrease_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_Out1Increase = FALSE;
                DrvTemperature_Out1Decrease = TRUE;

                /* start timer for decrease temperature offset for enabling of output */
                DrvTemperature_TimerOut1Decrease_Start(time_decrease_value_ms, FALSE);
            }
            else if (
                      (!DrvTemperature_Out1Increase) &&
                      ( DrvTemperature_Out1Decrease)
                    )
            {
                /* output compensation in progress (decrease) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT1 compensation in progress - decrease", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                // NOTHING TO DO
            }
            else
            {
                /* output compensation anomaly */

                // NOTHING TO DO

                DrvTemperature_Out1Increase = FALSE;
                DrvTemperature_Out1Decrease = FALSE;

                DrvTemperature_IndexOffsetOut1 = 0;
            }
            break;



        /* event - output OUT2 on */
        case TASK_MSG_ID__OUT2_ON:
            if (DrvTemperature_IndexOffsetOut2 > (TABLE_OFFSET_OUT_2_SIZE - 1))
                DrvTemperature_IndexOffsetOut2 = 0;

            if      (
                      (!DrvTemperature_Out2Increase) &&
                      (!DrvTemperature_Out2Decrease)
                    )
            {
                /* output compensation not in progress */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT2 compensation not in progress", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                if (DrvTemperature_IndexOffsetOut2 == 0)
                {
                    elapsed_time_factor = 1.0;

                    /* calculation of time for increase temperature offset */
                    time_increase_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut2.times_increase[DrvTemperature_IndexOffsetOut2] * 1000L)));

                    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT2 - index: %d, time_increase_value_ms: %ld", DrvTemperature_IndexOffsetOut2, time_increase_value_ms);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DrvTemperature_Out2Increase = TRUE;
                    DrvTemperature_Out2Decrease = FALSE;

                    /* start timer for increase temperature offset for enabling of output */
                    DrvTemperature_TimerOut2Increase_Start(time_increase_value_ms, FALSE);
                }
            }
            else if (
                      (!DrvTemperature_Out2Increase) &&
                      ( DrvTemperature_Out2Decrease)
                    )
            {
                /* output compensation in progress (decrease) */

                if (DrvTemperature_IndexOffsetOut2 == 0)
                    DrvTemperature_IndexOffsetOut2 = 1;

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT2 compensation in progress - decrease", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut2Increase_Stop();

                remaining_time_ms      = DrvTemperature_TimerOut2Decrease_Stop();
                time_decrease_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut2.times_decrease[DrvTemperature_IndexOffsetOut2] * 1000L;

                if (remaining_time_ms <= time_decrease_value_ms)
                    elapsed_time_ms = time_decrease_value_ms - remaining_time_ms;
                else
                    elapsed_time_ms = time_decrease_value_ms;

                if (elapsed_time_ms < time_decrease_value_ms)
                    elapsed_time_factor = ((float)elapsed_time_ms) / ((float)time_decrease_value_ms);
                else
                    elapsed_time_factor = 1.0;

                /* calculation of time for increase temperature offset */
                time_increase_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut2.times_increase[DrvTemperature_IndexOffsetOut2] * 1000L)));

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT2 - index: %d, time_increase_value_ms: %ld", DrvTemperature_IndexOffsetOut2, time_increase_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_Out2Increase = TRUE;
                DrvTemperature_Out2Decrease = FALSE;

                /* start timer for increase temperature offset for enabling of output */
                DrvTemperature_TimerOut2Increase_Start(time_increase_value_ms, FALSE);
            }
            else if (
                      ( DrvTemperature_Out2Increase) &&
                      (!DrvTemperature_Out2Decrease)
                    )
            {
                /* output compensation in progress (increase) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT2 compensation in progress - increase", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                // NOTHING TO DO
            }
            else
            {
                /* output compensation anomaly */

                // NOTHING TO DO

                DrvTemperature_Out2Increase = FALSE;
                DrvTemperature_Out2Decrease = FALSE;

                DrvTemperature_IndexOffsetOut2 = 0;
            }
            break;


        /* event - output OUT2 off */
        case TASK_MSG_ID__OUT2_OFF:
            if (DrvTemperature_IndexOffsetOut2 > (TABLE_OFFSET_OUT_2_SIZE - 1))
                DrvTemperature_IndexOffsetOut2 = TABLE_OFFSET_OUT_2_SIZE - 1;

            if      (
                      (!DrvTemperature_Out2Increase) &&
                      (!DrvTemperature_Out2Decrease)
                    )
            {
                /* output compensation not in progress */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT2 compensation not in progress", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                if (DrvTemperature_IndexOffsetOut2 == TABLE_OFFSET_OUT_2_SIZE - 1)
                {
                    elapsed_time_factor = 1.0;

                    /* calculation of time for decrease temperature offset */
                    time_decrease_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut2.times_decrease[DrvTemperature_IndexOffsetOut2] * 1000L)));

                    snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT2 - index: %d, time_decrease_value_ms: %ld", DrvTemperature_IndexOffsetOut2, time_decrease_value_ms);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DrvTemperature_Out2Increase = FALSE;
                    DrvTemperature_Out2Decrease = TRUE;

                    /* start timer for decrease temperature offset for enabling of output */
                    DrvTemperature_TimerOut2Decrease_Start(time_decrease_value_ms, FALSE);
                }
            }
            else if (
                      ( DrvTemperature_Out2Increase) &&
                      (!DrvTemperature_Out2Decrease)
                    )
            {
                /* output compensation in progress (increase) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT2 compensation in progress - increase", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut2Decrease_Stop();

                remaining_time_ms      = DrvTemperature_TimerOut2Increase_Stop();
                time_increase_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut2.times_increase[DrvTemperature_IndexOffsetOut2] * 1000L;

                if (remaining_time_ms <= time_increase_value_ms)
                    elapsed_time_ms = time_increase_value_ms - remaining_time_ms;
                else
                    elapsed_time_ms = time_increase_value_ms;

                if (elapsed_time_ms <= time_increase_value_ms)
                    elapsed_time_factor = ((float)elapsed_time_ms) / ((float)time_increase_value_ms);
                else
                    elapsed_time_factor = 1.0;

                /* calculation of time for decrease temperature offset */
                time_decrease_value_ms = (u32)(elapsed_time_factor * ((float)(DrvTemperature_Table_TemperatureIntOffsetForOut2.times_decrease[DrvTemperature_IndexOffsetOut2] * 1000L)));

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT2 - index: %d, time_decrease_value_ms: %ld", DrvTemperature_IndexOffsetOut2, time_decrease_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_Out2Increase = FALSE;
                DrvTemperature_Out2Decrease = TRUE;

                /* start timer for decrease temperature offset for enabling of output */
                DrvTemperature_TimerOut2Decrease_Start(time_decrease_value_ms, FALSE);
            }
            else if (
                      (!DrvTemperature_Out2Increase) &&
                      ( DrvTemperature_Out2Decrease)
                    )
            {
                /* output compensation in progress (decrease) */

                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "Output OUT2 compensation in progress - decrease", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                // NOTHING TO DO
            }
            else
            {
                /* output compensation anomaly */

                // NOTHING TO DO

                DrvTemperature_Out2Increase = FALSE;
                DrvTemperature_Out2Decrease = FALSE;

                DrvTemperature_IndexOffsetOut2 = 0;
            }
            break;



        /* event - timeout increase temperature offset for enabling of output OUT1 */
        case TASK_MSG_ID__TIMEOUT_OUT1_INCREASE:
            if      (DrvTemperature_IndexOffsetOut1 < (TABLE_OFFSET_OUT_1_SIZE - 1))
            {
                /* maximum index not yet reached */

                DrvTemperature_IndexOffsetOut1++;
                DrvTemperature_TemperatureOffsetOut1 = DrvTemperature_IndexOffsetOut1;

                time_increase_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut1.times_increase[DrvTemperature_IndexOffsetOut1] * 1000L;

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT1 - index: %d, time_increase_value_ms: %ld", DrvTemperature_IndexOffsetOut1, time_increase_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut1Increase_Start(time_increase_value_ms, FALSE);
            }
            else if (DrvTemperature_IndexOffsetOut1 == (TABLE_OFFSET_OUT_1_SIZE - 1))
            {
                /* maximum index         reached */

                DrvTemperature_TemperatureOffsetOut1 = TABLE_OFFSET_OUT_1_SIZE;

                DrvTemperature_Out1Increase = FALSE;
                DrvTemperature_Out1Decrease = FALSE;
            }
            break;


        /* event - timeout decrease temperature offset for enabling of output OUT1 */
        case TASK_MSG_ID__TIMEOUT_OUT1_DECREASE:
            if      (DrvTemperature_IndexOffsetOut1 > 0)
            {
                /* minimum index not yet reached */

                DrvTemperature_IndexOffsetOut1--;
                DrvTemperature_TemperatureOffsetOut1 = DrvTemperature_IndexOffsetOut1 + 1;

                time_decrease_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut1.times_decrease[DrvTemperature_IndexOffsetOut1] * 1000L;

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT1 - index: %d, time_decrease_value_ms: %ld", DrvTemperature_IndexOffsetOut1, time_decrease_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut1Decrease_Start(time_decrease_value_ms, FALSE);
            }
            else if (DrvTemperature_IndexOffsetOut1 == 0)
            {
                /* minimum index         reached */

                DrvTemperature_TemperatureOffsetOut1 = 0;

                DrvTemperature_Out1Increase = FALSE;
                DrvTemperature_Out1Decrease = FALSE;
            }
            break;



        /* event - timeout increase temperature offset for enabling of output OUT2 */
        case TASK_MSG_ID__TIMEOUT_OUT2_INCREASE:
            if      (DrvTemperature_IndexOffsetOut2 < (TABLE_OFFSET_OUT_2_SIZE - 1))
            {
                /* maximum index not yet reached */

                DrvTemperature_IndexOffsetOut2++;
                DrvTemperature_TemperatureOffsetOut2++;

                time_increase_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut2.times_increase[DrvTemperature_IndexOffsetOut2] * 1000L;

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT2 - index: %d, time_increase_value_ms: %ld", DrvTemperature_IndexOffsetOut2, time_increase_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut2Increase_Start(time_increase_value_ms, FALSE);
            }
            else if (DrvTemperature_IndexOffsetOut2 == (TABLE_OFFSET_OUT_2_SIZE - 1))
            {
                /* maximum index         reached */

                DrvTemperature_TemperatureOffsetOut2++;

                DrvTemperature_Out2Increase = FALSE;
                DrvTemperature_Out2Decrease = FALSE;
            }
            break;


        /* event - timeout decrease temperature offset for enabling of output OUT2 */
        case TASK_MSG_ID__TIMEOUT_OUT2_DECREASE:
            if      (DrvTemperature_IndexOffsetOut2 > 0)
            {
                /* minimum index not yet reached */

                DrvTemperature_IndexOffsetOut2--;
                DrvTemperature_TemperatureOffsetOut2 = DrvTemperature_IndexOffsetOut2 + 1;

                time_decrease_value_ms = DrvTemperature_Table_TemperatureIntOffsetForOut2.times_decrease[DrvTemperature_IndexOffsetOut2] * 1000L;

                snprintf(DrvTemperature_DebugString, sizeof(DrvTemperature_DebugString), "OUT2 - index: %d, time_decrease_value_ms: %ld", DrvTemperature_IndexOffsetOut2, time_decrease_value_ms);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, DrvTemperature_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DrvTemperature_TimerOut2Decrease_Start(time_decrease_value_ms, FALSE);
            }
            else if (DrvTemperature_IndexOffsetOut2 == 0)
            {
                /* minimum index         reached */

                DrvTemperature_TemperatureOffsetOut2 = 0;

                DrvTemperature_Out2Increase = FALSE;
                DrvTemperature_Out2Decrease = FALSE;
            }
            break;
    }



    if (
         /* status of internal temperature compensation for enabling of output */
         (DrvTemperature_Out1Increase          != old_out1_increase          ) ||
         (DrvTemperature_Out1Decrease          != old_out1_decrease          ) ||
         (DrvTemperature_Out2Increase          != old_out2_increase          ) ||
         (DrvTemperature_Out2Decrease          != old_out2_decrease          ) ||

         /* internal temperature table index for offset for enabling of output */
         (DrvTemperature_IndexOffsetOut1       != old_index_offset_out1      ) ||
         (DrvTemperature_IndexOffsetOut2       != old_index_offset_out2      ) ||

         /* internal temperature offset for enabling of output (/10 �C) */
         (DrvTemperature_TemperatureOffsetOut1 != old_temperature_offset_out1) ||
         (DrvTemperature_TemperatureOffsetOut2 != old_temperature_offset_out2)
       )
    {
        DrvTemperature_UpdateVarCrc();
    }
}




/*=============================================================================
 * Function   : DrvTemperature_AdlCallback_Timer_TimerTempMeasurement
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_AdlCallback_Timer_TimerTempMeasurement(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - DRV TEMPERATURE MEASUREMENT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    event.id = TASK_MSG_ID__TIMEOUT_TEMP_MEASUREMENT;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_AdlCallback_Timer_TimerOut1Increase
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_AdlCallback_Timer_TimerOut1Increase(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUT1 INCREASE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);



    if (DrvTemperature_TimerOut1Increase_Info.periodic)
    {
      //DrvTemperature_TimerOut1Increase_Info.status         = TRUE;
      //DrvTemperature_TimerOut1Increase_Info.periodic       = TRUE;
      //DrvTemperature_TimerOut1Increase_Info.time_value     = time_value;
        DrvTemperature_TimerOut1Increase_Info.time_remaining = DrvTemperature_TimerOut1Increase_Info.time_value;
        DrvTemperature_TimerOut1Increase_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
    else
    {
        DrvTemperature_TimerOut1Increase_Info.status         = FALSE;
        DrvTemperature_TimerOut1Increase_Info.periodic       = FALSE;
        DrvTemperature_TimerOut1Increase_Info.time_value     = 0;
        DrvTemperature_TimerOut1Increase_Info.time_remaining = 0;
        DrvTemperature_TimerOut1Increase_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_OUT1_INCREASE;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_AdlCallback_Timer_TimerOut1Decrease
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_AdlCallback_Timer_TimerOut1Decrease(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUT1 DECREASE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (DrvTemperature_TimerOut1Decrease_Info.periodic)
    {
      //DrvTemperature_TimerOut1Decrease_Info.status         = TRUE;
      //DrvTemperature_TimerOut1Decrease_Info.periodic       = TRUE;
      //DrvTemperature_TimerOut1Decrease_Info.time_value     = time_value;
        DrvTemperature_TimerOut1Decrease_Info.time_remaining = DrvTemperature_TimerOut1Decrease_Info.time_value;
        DrvTemperature_TimerOut1Decrease_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
    else
    {
        DrvTemperature_TimerOut1Decrease_Info.status         = FALSE;
        DrvTemperature_TimerOut1Decrease_Info.periodic       = FALSE;
        DrvTemperature_TimerOut1Decrease_Info.time_value     = 0;
        DrvTemperature_TimerOut1Decrease_Info.time_remaining = 0;
        DrvTemperature_TimerOut1Decrease_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_OUT1_DECREASE;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_AdlCallback_Timer_TimerOut2Increase
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_AdlCallback_Timer_TimerOut2Increase(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUT2 INCREASE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (DrvTemperature_TimerOut2Increase_Info.periodic)
    {
      //DrvTemperature_TimerOut2Increase_Info.status         = TRUE;
      //DrvTemperature_TimerOut2Increase_Info.periodic       = TRUE;
      //DrvTemperature_TimerOut2Increase_Info.time_value     = time_value;
        DrvTemperature_TimerOut2Increase_Info.time_remaining = DrvTemperature_TimerOut2Increase_Info.time_value;
        DrvTemperature_TimerOut2Increase_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
    else
    {
        DrvTemperature_TimerOut2Increase_Info.status         = FALSE;
        DrvTemperature_TimerOut2Increase_Info.periodic       = FALSE;
        DrvTemperature_TimerOut2Increase_Info.time_value     = 0;
        DrvTemperature_TimerOut2Increase_Info.time_remaining = 0;
        DrvTemperature_TimerOut2Increase_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_OUT2_INCREASE;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : DrvTemperature_AdlCallback_Timer_TimerOut2Decrease
 *
 * Description:
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void DrvTemperature_AdlCallback_Timer_TimerOut2Decrease(void *ptr_context)
{
    ql_event_t event;
    QlOSStatus err;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - OUT2 DECREASE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (DrvTemperature_TimerOut2Decrease_Info.periodic)
    {
      //DrvTemperature_TimerOut2Decrease_Info.status         = TRUE;
      //DrvTemperature_TimerOut2Decrease_Info.periodic       = TRUE;
      //DrvTemperature_TimerOut2Decrease_Info.time_value     = time_value;
        DrvTemperature_TimerOut2Decrease_Info.time_remaining = DrvTemperature_TimerOut2Decrease_Info.time_value;
        DrvTemperature_TimerOut2Decrease_Info.time_now       = ql_rtos_get_system_tick();

        DrvTemperature_UpdateVarCrc();
    }
    else
    {
        DrvTemperature_TimerOut2Decrease_Info.status         = FALSE;
        DrvTemperature_TimerOut2Decrease_Info.periodic       = FALSE;
        DrvTemperature_TimerOut2Decrease_Info.time_value     = 0;
        DrvTemperature_TimerOut2Decrease_Info.time_remaining = 0;
        DrvTemperature_TimerOut2Decrease_Info.time_now       = 0;

        DrvTemperature_UpdateVarCrc();
    }


    event.id = TASK_MSG_ID__TIMEOUT_OUT2_DECREASE;

    err = ql_rtos_event_send(Boot_TaskRef_DrvTemperature, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DRV_TEMPERATURE, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
