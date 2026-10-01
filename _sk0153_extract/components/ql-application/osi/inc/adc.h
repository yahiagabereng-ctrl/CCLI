/*=============================================================================
 * File       :  ADC.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - ADC driver
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ADC_H__


#define __ADC_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* ADC channel IDs */
#define ADC__CHANNEL_ID__ADC0__ANALOG_SIGNAL_1   0     /* ADC0: analog signal 1      (ADC 12 bits) */
#define ADC__CHANNEL_ID__ADC1__ANALOG_SIGNAL_2   1     /* ADC1: analog signal 2      (ADC 12 bits) */
#define ADC__CHANNEL_ID__ADC2__GSM_VOLTAGE       2     /* ADC2: GSM voltage          (ADC 12 bits) */

/* calibration polynomial degree */
#define ADC__ADC0_CALIBRATION_POLYNOMIAL_DEGREE  6     /* ADC0 calibration polynomial */   /* 6th degree polynomial */
#define ADC__ADC1_CALIBRATION_POLYNOMIAL_DEGREE  6     /* ADC1 calibration polynomial */   /* 6th degree polynomial */

/* maximum number of       consecutive ADC readings      */
#define ADC__MAX_NUMBER_OF_SAMPLES               10

/* maximum delay between 2 consecutive ADC readings (ms) */
#define ADC__MAX_DELAY_SAMPLES                   1000




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "ADC0 calibration" */
/*---------------------------------------------------------------------------
 * Coefficients of ADC0 calibration polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
typedef struct
{
    bool  calibration;
    float adc_coeff[ADC__ADC0_CALIBRATION_POLYNOMIAL_DEGREE + 1];
} ADC__CONFIG__ADC0_CALIBRATION;


/* configuration - "ADC1 calibration" */
/*---------------------------------------------------------------------------
 * Coefficients of ADC1 calibration polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
typedef struct
{
    bool  calibration;
    float adc_coeff[ADC__ADC1_CALIBRATION_POLYNOMIAL_DEGREE + 1];
} ADC__CONFIG__ADC1_CALIBRATION;




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




#endif
