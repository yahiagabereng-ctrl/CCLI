/*=============================================================================
 * File       :  STATUS.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - device status
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "clock.h"
#include "counters.h"
#include "debug_my.h"
#include "drvbattery.h"
#include "drvgpio.h"
#include "drvtemperature.h"
#include "regulation.h"
#include "status.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* device status */
static       STATUS__DEVICE_STATUS Status_DeviceStatus;
static const STATUS__DEVICE_STATUS Status_DeviceStatusUnknown =
{
    /* time */

    {
        /* date */
        2000,
        1,
        1,

        /* time */
        0,
        0,
        0,

        /* weekday */
        6,   // Saturday
    },


    /* device status */

    /* power supply */
    TRUE,                              /* presence of main power supply */
    FALSE,                             /* battery charge status         */
    0,                                 /* GSM module power supply (mV)  */

    /* temperatures */
    DRVTEMPERATURE__SENSOR__UNKNOWN,   /* internal sensor status        */
    DRVTEMPERATURE__SENSOR__UNKNOWN,   /* external sensor status        */
    0,                                 /* internal temperature (/10 �C) */
    0,                                 /* external temperature (/10 �C) */
    0,                                 /* internal ADC value            */
    0,                                 /* external ADC value            */

    /* digital inputs */
    TRUE,                              /* digital input  1 enabled status */
    TRUE,                              /* digital input  2 enabled status */
    FALSE,                             /* digital input  1                */
    FALSE,                             /* digital input  2                */
    FALSE,                             /* digital input  3                */
    FALSE,                             /* digital input  4                */

    /* counter inputs */
    FALSE,                             /* counter input  1 enabled       status   */
    FALSE,                             /* counter input  2 enabled       status   */
    FALSE,                             /* counter input  1 enabled bands status   */
    FALSE,                             /* counter input  2 enabled bands status   */
    COUNTERS__BAND_TYPE__F1,           /* counter input  1 band type (F1, F2, F3) */
    COUNTERS__BAND_TYPE__F1,           /* counter input  2 band type (F1, F2, F3) */
    0,                                 /* counter input  1                        */
    0,                                 /* counter input  2                        */
    0,                                 /* counter input  1 - band F1              */
    0,                                 /* counter input  1 - band F2              */
    0,                                 /* counter input  1 - band F3              */
    0,                                 /* counter input  2 - band F1              */
    0,                                 /* counter input  2 - band F2              */
    0,                                 /* counter input  2 - band F3              */

    /* digital outputs */
    FALSE,                             /* digital output 1 (         physical status) */
    FALSE,                             /* digital output 2 (         physical status) */
    FALSE,                             /* digital output 1 (expected logical  status) */
    FALSE,                             /* digital output 2 (expected logical  status) */
    FALSE,                             /* digital output 1 (         logical  status) */
    FALSE,                             /* digital output 2 (         logical  status) */
};


static       bool                  Status_DeviceStatusPresent = FALSE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
bool Status_ReadDeviceStatus(STATUS__DEVICE_STATUS *ptr_device_status);

bool Status_GetDeviceStatus (STATUS__DEVICE_STATUS *ptr_device_status);
bool Status_SetDeviceStatus (STATUS__DEVICE_STATUS *ptr_device_status);




/*===========================================================================
 * Function   : Status_ReadDeviceStatus
 *
 * Description: read the device status
 *                 - battery voltage
 *                 - internal temperature
 *                 - external temperature
 *                 - digital input  1
 *                 - digital input  2
 *                 - counter input  1
 *                 - counter input  2
 *                 - digital output 1 (physical status)
 *                 - digital output 2 (physical status)
 *                 - digital output 1 (logical  status)
 *                 - digital output 2 (logical  status)
 * Input      : - ptr_device_status: pointer to the device status read
 * Output     : - FALSE: device status not got
 *              - TRUE : device status     got
 *===========================================================================*/
bool Status_ReadDeviceStatus(STATUS__DEVICE_STATUS *ptr_device_status)
{
    /* configuration - C1 counter input */
    /* configuration - C2 counter input */
           COUNTERS__CONFIG__INPUT_C1       config_input_c1;
           COUNTERS__CONFIG__INPUT_C2       config_input_c2;

    /* configuration - C1 counter input bands */
    /* configuration - C2 counter input bands */
    static COUNTERS__CONFIG__INPUT_C1_BANDS config_input_c1_bands;
    static COUNTERS__CONFIG__INPUT_C2_BANDS config_input_c2_bands;

    /* C1 counter input */
    /* C2 counter input */
           u32                              input_c1_counter;
           u32                              input_c2_counter;
           u32                              input_c1_counter_f1;
           u32                              input_c1_counter_f2;
           u32                              input_c1_counter_f3;
           u32                              input_c2_counter_f1;
           u32                              input_c2_counter_f2;
           u32                              input_c2_counter_f3;

    /* counter inputs */
           CLOCK__TIME                      time;

           bool                             main_power_supply;
           u16                              gsm_module_power_supply;

           u8                               sensor_status_int;
           u8                               sensor_status_ext;
           s16                              temperature_int;
           s16                              temperature_ext;
           u16                              adc_value_int;
           u16                              adc_value_ext;

           bool                             in1_enabled_status;
           bool                             in2_enabled_status;
           bool                             in1;
           bool                             in2;
           bool                             in3;
           bool                             in4;

           bool                             c1_enabled_status;
           bool                             c2_enabled_status;
           bool                             c1_bands_enabled_status;
           bool                             c2_bands_enabled_status;
           u8                               c1_band_type;
           u8                               c2_band_type;
           u32                              c1;
           u32                              c2;
           u32                              c1_f1;
           u32                              c1_f2;
           u32                              c1_f3;
           u32                              c2_f1;
           u32                              c2_f2;
           u32                              c2_f3;

           bool                             out1_physical;
           bool                             out2_physical;
           bool                             out1_physical_expected;
           bool                             out2_physical_expected;
           bool                             out1_logical;
           bool                             out2_logical;

           u8                               band_type_c1;
           u8                               band_type_c2;

           bool                             error;
           bool                             result;


    error = FALSE;


    /* get input C1 configuration */
    /* get input C2 configuration */
    Counters_Config_InputC1_Get(&config_input_c1);
    Counters_Config_InputC2_Get(&config_input_c2);

    /* get input C1 bands configuration */
    /* get input C2 bands configuration */
    Counters_Config_InputC1Bands_Get(&config_input_c1_bands);
    Counters_Config_InputC2Bands_Get(&config_input_c2_bands);


    /* get the time */
    result = Clock_GetTime(&time);
    if (!result)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STATUS, DEBUG_TRACE_TYPE_LOW, "Clock_GetTime ERROR"                        , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        error = TRUE;
    }


    /* get battery voltage */
    main_power_supply = DrvGpio_ReadInput(DRVGPIO__IN__PWR_OK);

    /* get GSM module power supply */
    result = DrvBattery_GetBatteryVoltage(&gsm_module_power_supply);
    if (!result)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STATUS, DEBUG_TRACE_TYPE_LOW, "DrvBattery_GetBatteryVoltage ERROR"         , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        error = TRUE;
    }


    /* get internal temperature */
    result = DrvTemperature_GetInternalTemperature(&sensor_status_int, &temperature_int, &adc_value_int);
    if (!result)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STATUS, DEBUG_TRACE_TYPE_LOW, "DrvTemperature_GetInternalTemperature ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        error = TRUE;
    }

    /* get external temperature */
    result = DrvTemperature_GetExternalTemperature(&sensor_status_ext, &temperature_ext, &adc_value_ext);
    if (!result)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_STATUS, DEBUG_TRACE_TYPE_LOW, "DrvTemperature_GetExternalTemperature ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        error = TRUE;
    }


    /* get digital inputs IN1 status */
    if (!config_input_c1.enabled_status)
    {
        /* get digital inputs IN1 status */
        in1_enabled_status = TRUE;
        in1                = DrvGpio_ReadInput(DRVGPIO__IN__IN1);
    }
    else
    {
        in1_enabled_status = FALSE;
        in1                = TRUE;
    }

    /* get digital inputs IN2 status */
    if (!config_input_c2.enabled_status)
    {
        /* get digital inputs IN2 status */
        in2_enabled_status = TRUE;
        in2                = DrvGpio_ReadInput(DRVGPIO__IN__IN2);
    }
    else
    {
        in2_enabled_status = FALSE;
        in2                = TRUE;
    }

    /* get digital inputs IN3 status */
    if (sensor_status_int == DRVTEMPERATURE__SENSOR__OUT_OF_ORDER)
        in3 = FALSE;
    else
        in3 = TRUE;

    /* get digital inputs IN4 status */
    if (sensor_status_ext == DRVTEMPERATURE__SENSOR__OUT_OF_ORDER)
        in4 = FALSE;
    else
        in4 = TRUE;


    /* get counter inputs C1 status */
    if (config_input_c1.enabled_status)
    {
        if (config_input_c1_bands.enabled_status_week_days)
        {
            band_type_c1 = Counters_FindInputC1Band(&time);

            /* get counter inputs C1 value */
            Counters_InputC1_Get  (&input_c1_counter   );
            Counters_InputC1F1_Get(&input_c1_counter_f1);
            Counters_InputC1F2_Get(&input_c1_counter_f2);
            Counters_InputC1F3_Get(&input_c1_counter_f3);
            c1_enabled_status       = TRUE;
            c1_bands_enabled_status = TRUE;
            c1_band_type            = band_type_c1;
            c1                      = input_c1_counter;
            c1_f1                   = input_c1_counter_f1;
            c1_f2                   = input_c1_counter_f2;
            c1_f3                   = input_c1_counter_f3;
        }
        else
        {
            /* get counter inputs C1 value */
            Counters_InputC1_Get  (&input_c1_counter   );
            c1_enabled_status       = TRUE;
            c1_bands_enabled_status = FALSE;
            c1_band_type            = COUNTERS__BAND_TYPE__F1;
            c1                      = input_c1_counter;
            c1_f1                   = 0;
            c1_f2                   = 0;
            c1_f3                   = 0;
        }
    }
    else
    {
        c1_enabled_status       = FALSE;
        c1_bands_enabled_status = FALSE;
        c1_band_type            = COUNTERS__BAND_TYPE__F1;
        c1                      = 0;
        c1_f1                   = 0;
        c1_f2                   = 0;
        c1_f3                   = 0;
    }

    /* get counter inputs C2 status */
    if (config_input_c2.enabled_status)
    {
        if (config_input_c2_bands.enabled_status_week_days)
        {
            band_type_c2 = Counters_FindInputC2Band(&time);

            /* get counter inputs C2 value */
            Counters_InputC2_Get  (&input_c2_counter   );
            Counters_InputC2F1_Get(&input_c2_counter_f1);
            Counters_InputC2F2_Get(&input_c2_counter_f2);
            Counters_InputC2F3_Get(&input_c2_counter_f3);
            c2_enabled_status       = TRUE;
            c2_bands_enabled_status = TRUE;
            c2_band_type            = band_type_c2;
            c2                      = input_c2_counter;
            c2_f1                   = input_c2_counter_f1;
            c2_f2                   = input_c2_counter_f2;
            c2_f3                   = input_c2_counter_f3;
        }
        else
        {
            /* get counter inputs C2 value */
            Counters_InputC2_Get  (&input_c2_counter   );
            c2_enabled_status       = TRUE;
            c2_bands_enabled_status = FALSE;
            c2_band_type            = COUNTERS__BAND_TYPE__F1;
            c2                      = input_c2_counter;
            c2_f1                   = 0;
            c2_f2                   = 0;
            c2_f3                   = 0;
        }
    }
    else
    {
        c2_enabled_status       = FALSE;
        c2_bands_enabled_status = FALSE;
        c2_band_type            = COUNTERS__BAND_TYPE__F1;
        c2                      = 0;
        c2_f1                   = 0;
        c2_f2                   = 0;
        c2_f3                   = 0;
    }


    /* get digital outputs OUT1 and OUT2 status (         physical status) */
    out1_physical          =  DrvGpio_ReadOut1();
    out2_physical          =  DrvGpio_ReadOut2();

    /* get digital outputs OUT1 and OUT2 status (expected physical status) */
    out1_physical_expected =  Regulation_StatusPhysicalExpectedOutputInt();
    out2_physical_expected =  Regulation_StatusPhysicalExpectedOutputExt();

    /* get digital outputs OUT1 and OUT2 status (         logical  status) */
    out1_logical           =  Regulation_StatusLogicalOutputInt();
    out2_logical           =  Regulation_StatusLogicalOutputExt();


    if (error)
    {
        Status_DeviceStatus        = Status_DeviceStatusUnknown;
        Status_DeviceStatusPresent = FALSE;

        *ptr_device_status         = Status_DeviceStatus;

        return FALSE;
    }


    /* save the device status */
    Status_DeviceStatus.time                    = time;
    Status_DeviceStatus.main_power_supply       = main_power_supply;
    Status_DeviceStatus.gsm_module_power_supply = gsm_module_power_supply;
    Status_DeviceStatus.sensor_status_int       = sensor_status_int;
    Status_DeviceStatus.sensor_status_ext       = sensor_status_ext;
    Status_DeviceStatus.temperature_int         = temperature_int;
    Status_DeviceStatus.temperature_ext         = temperature_ext;
    Status_DeviceStatus.adc_value_int           = adc_value_int;
    Status_DeviceStatus.adc_value_ext           = adc_value_ext;
    Status_DeviceStatus.in1_enabled_status      = in1_enabled_status;
    Status_DeviceStatus.in2_enabled_status      = in2_enabled_status;
    Status_DeviceStatus.in1                     = in1;
    Status_DeviceStatus.in2                     = in2;
    Status_DeviceStatus.in3                     = in3;
    Status_DeviceStatus.in4                     = in4;
    Status_DeviceStatus.c1_enabled_status       = c1_enabled_status;
    Status_DeviceStatus.c2_enabled_status       = c2_enabled_status;
    Status_DeviceStatus.c1_bands_enabled_status = c1_bands_enabled_status;
    Status_DeviceStatus.c2_bands_enabled_status = c2_bands_enabled_status;
    Status_DeviceStatus.c1_band_type            = c1_band_type;
    Status_DeviceStatus.c2_band_type            = c2_band_type;
    Status_DeviceStatus.c1                      = c1;
    Status_DeviceStatus.c2                      = c2;
    Status_DeviceStatus.c1_f1                   = c1_f1;
    Status_DeviceStatus.c1_f2                   = c1_f2;
    Status_DeviceStatus.c1_f3                   = c1_f3;
    Status_DeviceStatus.c2_f1                   = c2_f1;
    Status_DeviceStatus.c2_f2                   = c2_f2;
    Status_DeviceStatus.c2_f3                   = c2_f3;
    Status_DeviceStatus.out1_physical           = out1_physical;
    Status_DeviceStatus.out2_physical           = out2_physical;
    Status_DeviceStatus.out1_physical_expected  = out1_physical_expected;
    Status_DeviceStatus.out2_physical_expected  = out2_physical_expected;
    Status_DeviceStatus.out1_logical            = out1_logical;
    Status_DeviceStatus.out2_logical            = out2_logical;

    Status_DeviceStatusPresent = TRUE;


    *ptr_device_status = Status_DeviceStatus;

    return TRUE;
}




/*===========================================================================
 * Function   : Status_GetDeviceStatus
 *
 * Description: get the device status
 * Input      : - ptr_device_status: pointer to the device status read
 * Output     : - FALSE: device status not read
 *              - TRUE : device status     read
 *===========================================================================*/
bool Status_GetDeviceStatus(STATUS__DEVICE_STATUS *ptr_device_status)
{
    if (!Status_DeviceStatusPresent)
    {
        Status_DeviceStatus = Status_DeviceStatusUnknown;

        *ptr_device_status = Status_DeviceStatus;

        return FALSE;
    }


    *ptr_device_status = Status_DeviceStatus;

    return TRUE;
}




/*===========================================================================
 * Function   : Status_SetDeviceStatus
 *
 * Description: set the device status
 * Input      : - ptr_device_status: pointer to the device status to be set
 * Output     : - FALSE: device status not set
 *              - TRUE : device status     set
 *===========================================================================*/
bool Status_SetDeviceStatus(STATUS__DEVICE_STATUS *ptr_device_status)
{
    Status_DeviceStatus = *ptr_device_status;

    Status_DeviceStatusPresent = TRUE;

    return TRUE;
}
