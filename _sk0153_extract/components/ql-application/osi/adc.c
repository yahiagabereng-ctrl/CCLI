/*=============================================================================
 * File       :  ADC.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - ADC driver
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * NOTES
 *===========================================================================*/

/*
 * Analog signals:
 * - ADC0: GSM voltage          (ADC 12 bits)
 * - ADC1: analog signal 1      (ADC 12 bits)
 * - ADC2: analog signal 2      (ADC 12 bits)
 * - ADC3: internal temperature (ADC 12 bits)
 *
 * ADC features:
 * - resolution: 12 bit
 * - range     : 0.0-2.0 V  (ADC1, ADC2, ADC3)
 *               3.2-4.8 V  (ADC0)
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_adc.h"
#include "ql_api_osi.h"

/* user     includes */
#include "adc.h"
#include "debug_my.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING   200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "ADC0 calibration" */
/*---------------------------------------------------------------------------
 * Coefficients of ADC0 calibration polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
static       ADC__CONFIG__ADC0_CALIBRATION Adc_Config_Adc0Calibration;
static const ADC__CONFIG__ADC0_CALIBRATION Adc_Config_Adc0CalibrationDefault =
{
    FALSE,
//    a6     a5     a4     a3     a2     a1     a0
    { 0.0,   0.0,   0.0,   0.0,   0.0,   1.0,   0.0 }   // y = x  (no calibration)
};


/* configuration - "ADC1 calibration" */
/*---------------------------------------------------------------------------
 * Coefficients of ADC1 calibration polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
static       ADC__CONFIG__ADC1_CALIBRATION Adc_Config_Adc1Calibration;
static const ADC__CONFIG__ADC1_CALIBRATION Adc_Config_Adc1CalibrationDefault =
{
    FALSE,
//    a6     a5     a4     a3     a2     a1     a0
    { 0.0,   0.0,   0.0,   0.0,   0.0,   1.0,   0.0 }   // y = x  (no calibration)
};

/* debug string */
static ascii Adc_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* read ADC channel */
bool Adc_ReadAdcChannel(int adc_channel_id, u8 num_samples, u32 delay_samples, u16 *ptr_adc_raw_value, u16 *ptr_adc_mv_value);

/* get/set "ADC0 calibration" configuration */
void Adc_Config_Adc0Calibration_GetDefault(ADC__CONFIG__ADC0_CALIBRATION *ptr_data);
void Adc_Config_Adc0Calibration_Get       (ADC__CONFIG__ADC0_CALIBRATION *ptr_data);
void Adc_Config_Adc0Calibration_Set       (ADC__CONFIG__ADC0_CALIBRATION *ptr_data);
bool Adc_Config_Adc0Calibration_IsValid   (ADC__CONFIG__ADC0_CALIBRATION *ptr_data);

/* get/set "ADC1 calibration" configuration */
void Adc_Config_Adc1Calibration_GetDefault(ADC__CONFIG__ADC1_CALIBRATION *ptr_data);
void Adc_Config_Adc1Calibration_Get       (ADC__CONFIG__ADC1_CALIBRATION *ptr_data);
void Adc_Config_Adc1Calibration_Set       (ADC__CONFIG__ADC1_CALIBRATION *ptr_data);
bool Adc_Config_Adc1Calibration_IsValid   (ADC__CONFIG__ADC1_CALIBRATION *ptr_data);




/*=============================================================================
 * Function   : Adc_ReadAdcChannel
 *
 * Description: read the ADC channel specified
 *              It does N consecutive readings, then calculate the average
 *              (it discards the minimum and maximum readings).
 * Input      : - adc_channel_id   : ADC channel ID (0-3)
 *              - num_samples      : number of     samples
 *              - delay_samples    : delay between samples (ms)
 *              - ptr_adc_raw_value: pointer to save the value of the ADC register
 *              - ptr_adc_mv_value : pointer to save the value of the ADC voltage (mV)
 * Output     : - FALSE: ADC channel not read (ADC channel ID uncorrect)
 *              - TRUE : ADC channel     read
 *=============================================================================*/
bool Adc_ReadAdcChannel(int adc_channel_id, u8 num_samples, u32 delay_samples, u16 *ptr_adc_raw_value, u16 *ptr_adc_mv_value)
{
    u32              adc_raw_value_avg;                           // average values (raw values)
    s32              adc_mv_value_avg;                            // average values (mV  values)

    int              adc_raw_values[ADC__MAX_NUMBER_OF_SAMPLES];  // single  values (raw values)
    int              adc_mv_values [ADC__MAX_NUMBER_OF_SAMPLES];  // single  values (mV  values)

    u8               min_index_raw;
    u8               min_index_mv;

    u8               max_index_raw;
    u8               max_index_mv;

    u8               num_sample_raw;
    u8               num_sample_mv;

    bool             sample_raw;
    bool             sample_mv;

    float            x;
    float            y;

    ql_errcode_adc_e res_s32;
    u8               i;


    /* check channel ID value */
    if (adc_channel_id >= 2)
    {
        *ptr_adc_raw_value = 0;
        *ptr_adc_mv_value  = 0;

        return FALSE;
    }

    /* check the number of samples */
    if (
         (num_samples == 0                         ) ||
         (num_samples >  ADC__MAX_NUMBER_OF_SAMPLES)
        )
    {
        return FALSE;
    }

    /* check the delay between samples */
    if (delay_samples > ADC__MAX_DELAY_SAMPLES)
        return FALSE;


    /* N consecutive analog readings with a delay between samples */
    for (i = 0; i < num_samples; i++)
    {
        /* read ADCx (raw value) */
        res_s32 = ql_adc_get_volt_raw(adc_channel_id, QL_ADC_SCALE_AUTO, &adc_raw_values[i]);
        if (res_s32 != QL_ADC_SUCCESS)
        {
            snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ql_adc_get_volt_raw ERROR: %d", res_s32);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
      //else
      //{
      //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, "ql_adc_get_volt_raw OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
      //}


        /* read ADCx (mV value) */
        res_s32 = ql_adc_get_volt(adc_channel_id, &adc_mv_values[i]);
        if (res_s32 != QL_ADC_SUCCESS)
        {
            snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ql_adc_get_volt ERROR: %d", res_s32);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
      //else
      //{
      //    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, "ql_adc_get_volt OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
      //}


        /* print the istantaneus values */
        snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ADC[%d][%d] - adc_raw_value: %d, adc_mv_value: %d", adc_channel_id, i, adc_raw_values[i], adc_mv_values[i]);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* delay beetween 2 consecutive ADC readings */
        if (delay_samples > 0)
            ql_rtos_task_sleep_ms(delay_samples);
    }


    /* find the index of the mininum and the maximum values */
    min_index_raw = 0;
    min_index_mv  = 0;
    max_index_raw = 0;
    max_index_mv  = 0;
    for (i = 0; i < num_samples; i++)
    {
        /* minimum value */
        if (adc_raw_values[i] < adc_raw_values[min_index_raw])
            min_index_raw = i;
        if (adc_mv_values [i] < adc_mv_values [min_index_mv ])
            min_index_mv  = i;

        /* maximum value */
        if (adc_raw_values[i] > adc_raw_values[max_index_raw])
            max_index_raw = i;
        if (adc_mv_values [i] > adc_mv_values [max_index_mv ])
            max_index_mv  = i;
    }

  //snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ADC[%d] - min_index_raw : %d, min_index_mv : %d", adc_channel_id, min_index_raw , min_index_mv);
  //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 100);

  //snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ADC[%d] - max_index_raw : %d, max_index_mv : %d", adc_channel_id, max_index_raw , max_index_mv);
  //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 100);


    /* calculus of average values (minimun and maximum values are not considered if number of samples is >= 4) */
    adc_raw_value_avg = 0;
    adc_mv_value_avg  = 0;
    num_sample_raw = 0;
    num_sample_mv  = 0;
    for (i = 0; i < num_samples; i++)
    {
        if (num_samples < 4)
        {
            sample_raw = TRUE;
            sample_mv  = TRUE;
        }
        else
        {
            if ((i != min_index_raw) && (i != max_index_raw))
                sample_raw = TRUE;
            else
                sample_raw = FALSE;

            if ((i != min_index_mv ) && (i != max_index_mv ))
                sample_mv  = TRUE;
            else
                sample_mv  = FALSE;
        }

        if (sample_raw)
        {
            adc_raw_value_avg += adc_raw_values[i];
            num_sample_raw++;
        }

        if (sample_mv)
        {
            adc_mv_value_avg  += adc_mv_values [i];
            num_sample_mv++;
        }
    }
    adc_raw_value_avg /= num_sample_raw;
    adc_mv_value_avg  /= num_sample_mv;
    if (adc_mv_value_avg < 0)
        adc_mv_value_avg = 0;

  //snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ADC[%d] - num_sample_raw: %d, num_sample_mv: %d", adc_channel_id, num_sample_raw, num_sample_mv);
  //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 100);


    /* print the average values (without ADC calibration) */
    snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ADC[%d] (no   calibration) - adc_raw_value_avg: %ld, adc_mv_value_avg: %ld", adc_channel_id, adc_raw_value_avg, adc_mv_value_avg);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* update the ADC value according to ADC calibration (not implemented for raw value) */
    if      (adc_channel_id == 0)
    {
        if (Adc_Config_Adc0Calibration.calibration)
        {
            x                = (float)adc_mv_value_avg;
            y                = Utility_InterpolationPolynomial(Adc_Config_Adc0Calibration.adc_coeff, ADC__ADC0_CALIBRATION_POLYNOMIAL_DEGREE, x);
            adc_mv_value_avg = (s32)y;
        }
    }
    else if (adc_channel_id == 1)
    {
        if (Adc_Config_Adc1Calibration.calibration)
        {
            x                = (float)adc_mv_value_avg;
            y                = Utility_InterpolationPolynomial(Adc_Config_Adc1Calibration.adc_coeff, ADC__ADC1_CALIBRATION_POLYNOMIAL_DEGREE, x);
            adc_mv_value_avg = (s32)y;
        }
    }
    if (adc_mv_value_avg < 0)
        adc_mv_value_avg = 0;


    /* print the average values (with ADC calibration) */
    snprintf(Adc_DebugString, sizeof(Adc_DebugString), "ADC[%d] (with calibration) - adc_raw_value_avg: %ld, adc_mv_value_avg: %ld", adc_channel_id, adc_raw_value_avg, adc_mv_value_avg);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_ADC, DEBUG_TRACE_TYPE_LOW, Adc_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* save the ADC values */
    *ptr_adc_raw_value = adc_raw_value_avg;
    *ptr_adc_mv_value  = adc_mv_value_avg;

    return TRUE;
}




/*===========================================================================
 * Function   : Adc_Config_Adc0Calibration_GetDefault
 *
 * Description: get the default "ADC0 calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Adc_Config_Adc0Calibration_GetDefault(ADC__CONFIG__ADC0_CALIBRATION *ptr_data)
{
    *ptr_data = Adc_Config_Adc0CalibrationDefault;
}




/*===========================================================================
 * Function   : Adc_Config_Adc0Calibration_Get
 *
 * Description: get the "ADC0 calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Adc_Config_Adc0Calibration_Get(ADC__CONFIG__ADC0_CALIBRATION *ptr_data)
{
    *ptr_data = Adc_Config_Adc0Calibration;
}




/*===========================================================================
 * Function   : Adc_Config_Adc0Calibration_Set
 *
 * Description: set the "ADC0 calibration" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Adc_Config_Adc0Calibration_Set(ADC__CONFIG__ADC0_CALIBRATION *ptr_data)
{
    Adc_Config_Adc0Calibration = *ptr_data;
}




/*===========================================================================
 * Function   : Adc_Config_Adc0Calibration_IsValid
 *
 * Description: check if the "ADC0 calibration" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Adc_Config_Adc0Calibration_IsValid(ADC__CONFIG__ADC0_CALIBRATION *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : Adc_Config_Adc1Calibration_GetDefault
 *
 * Description: get the default "ADC1 calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Adc_Config_Adc1Calibration_GetDefault(ADC__CONFIG__ADC1_CALIBRATION *ptr_data)
{
    *ptr_data = Adc_Config_Adc1CalibrationDefault;
}




/*===========================================================================
 * Function   : Adc_Config_Adc1Calibration_Get
 *
 * Description: get the "ADC1 calibration" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Adc_Config_Adc1Calibration_Get(ADC__CONFIG__ADC1_CALIBRATION *ptr_data)
{
    *ptr_data = Adc_Config_Adc1Calibration;
}




/*===========================================================================
 * Function   : Adc_Config_Adc1Calibration_Set
 *
 * Description: set the "ADC1 calibration" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Adc_Config_Adc1Calibration_Set(ADC__CONFIG__ADC1_CALIBRATION *ptr_data)
{
    Adc_Config_Adc1Calibration = *ptr_data;
}




/*===========================================================================
 * Function   : Adc_Config_Adc1Calibration_IsValid
 *
 * Description: check if the "ADC1 calibration" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Adc_Config_Adc1Calibration_IsValid(ADC__CONFIG__ADC1_CALIBRATION *ptr_data)
{
    return TRUE;
}
