/*=============================================================================
 * File       :  DRVTEMPERATURE.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - temperature driver
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DRVTEMPERATURE_H__


#define __DRVTEMPERATURE_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* sensor status */
#define DRVTEMPERATURE__SENSOR__UNKNOWN                             0     /* sensor status - unknown      */
#define DRVTEMPERATURE__SENSOR__DISCONNECTED                        1     /* sensor status - disconnected */
#define DRVTEMPERATURE__SENSOR__OUT_OF_ORDER                        2     /* sensor status - out of order */
#define DRVTEMPERATURE__SENSOR__CONNECTED                           3     /* sensor status - connected    */

/* interpolation polynomial degree */
#define DRVTEMPERATURE__POLYNOMIAL_1_DEGREE                         6     /* interpolation polynomial 1 */   /* 6th degree polynomial */
#define DRVTEMPERATURE__POLYNOMIAL_2_DEGREE                         6     /* interpolation polynomial 2 */   /* 6th degree polynomial */

/* level measurement modes */
#define DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_TABLE       0     /* temperature measurement mode - interpolation table      */
#define DRVTEMPERATURE__TEMPERATURE_MODE__INTERPOLATION_POLYNOMIAL  1     /* temperature measurement mode - interpolation polynomial */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - "internal temperature calibration" */
typedef struct
{
    s16   calibration;   /* calibration (/10 �C) [-5.0-+5.0 �C] */
} DRVTEMPERATURE__CONFIG__CALIBRATION_INT;


/* configuration - "external temperature calibration" */
typedef struct
{
    s16   calibration;   /* calibration (/10 �C) [-5.0-+5.0 �C] */
} DRVTEMPERATURE__CONFIG__CALIBRATION_EXT;


/* configuration - "temperature mode" */
typedef struct
{
    u8    mode;          /* temperature mode */
} DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE;


/* configuration - "interpolation polynomial 1" */
/*---------------------------------------------------------------------------
 * Coefficients of interpolation polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
typedef struct
{
    float poly_coeff[DRVTEMPERATURE__POLYNOMIAL_1_DEGREE + 1];
} DRVTEMPERATURE__CONFIG__POLYNOMIAL_1;


/* configuration - "interpolation polynomial 2" */
/*---------------------------------------------------------------------------
 * Coefficients of interpolation polynomial (a_n, a_n-1, .., a_1, a_0)
 * y = (a_n * x^n)  +  (a_n-1 * x^(n-1))  +  ...  +  (a_1 * x^1)  +  a_0
 *---------------------------------------------------------------------------*/
typedef struct
{
    float poly_coeff[DRVTEMPERATURE__POLYNOMIAL_2_DEGREE + 1];
} DRVTEMPERATURE__CONFIG__POLYNOMIAL_2;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void DrvTemperature_TaskDrvTemperature(void *argument);

/* status variables */
void DrvTemperature_UpdateVarCrc(void);
bool DrvTemperature_VerifyVarCrc(void);

/* events */
void DrvTemperature_OutputOut1On (void);
void DrvTemperature_OutputOut1Off(void);
void DrvTemperature_OutputOut2On (void);
void DrvTemperature_OutputOut2Off(void);

/* get internal/external temperature */
bool DrvTemperature_GetInternalTemperature(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);
bool DrvTemperature_GetExternalTemperature(u8 *ptr_sensor_status, s16 *ptr_temperature, u16 *ptr_adc_value);

/* registered signals of new available temperatures */
void DrvTemperature_RegisterSignalNewTemperatures(void (*ptr_function)(u8, s16, u8, s16));

/* registered signals of new available ADC values */
void DrvTemperature_RegisterSignalNewAdcValues(void (*ptr_function)(u16, u16));

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - "internal temperature calibration" */
void DrvTemperature_Config_CalibrationInt_GetDefault            (DRVTEMPERATURE__CONFIG__CALIBRATION_INT  *ptr_data);
void DrvTemperature_Config_CalibrationInt_Get                   (DRVTEMPERATURE__CONFIG__CALIBRATION_INT  *ptr_data);
void DrvTemperature_Config_CalibrationInt_Set                   (DRVTEMPERATURE__CONFIG__CALIBRATION_INT  *ptr_data);
bool DrvTemperature_Config_CalibrationInt_IsValid               (DRVTEMPERATURE__CONFIG__CALIBRATION_INT  *ptr_data);

/* configuration - "external temperature calibration" */
void DrvTemperature_Config_CalibrationExt_GetDefault            (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT  *ptr_data);
void DrvTemperature_Config_CalibrationExt_Get                   (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT  *ptr_data);
void DrvTemperature_Config_CalibrationExt_Set                   (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT  *ptr_data);
bool DrvTemperature_Config_CalibrationExt_IsValid               (DRVTEMPERATURE__CONFIG__CALIBRATION_EXT  *ptr_data);

/* get/set "temperature measurement mode"     configuration */
void DrvTemperature_Config_TemperatureMeasurementMode_GetDefault(DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data);
void DrvTemperature_Config_TemperatureMeasurementMode_Get       (DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data);
void DrvTemperature_Config_TemperatureMeasurementMode_Set       (DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data);
bool DrvTemperature_Config_TemperatureMeasurementMode_IsValid   (DRVTEMPERATURE__CONFIG__TEMPERATURE_MODE *ptr_data);

/* get/set "interpolation polynomial 1" configuration */
void DrvTemperature_Config_Polynomial1_GetDefault               (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1     *ptr_data);
void DrvTemperature_Config_Polynomial1_Get                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1     *ptr_data);
void DrvTemperature_Config_Polynomial1_Set                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1     *ptr_data);
bool DrvTemperature_Config_Polynomial1_IsValid                  (DRVTEMPERATURE__CONFIG__POLYNOMIAL_1     *ptr_data);

/* get/set "interpolation polynomial 2" configuration */
void DrvTemperature_Config_Polynomial2_GetDefault               (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2     *ptr_data);
void DrvTemperature_Config_Polynomial2_Get                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2     *ptr_data);
void DrvTemperature_Config_Polynomial2_Set                      (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2     *ptr_data);
bool DrvTemperature_Config_Polynomial2_IsValid                  (DRVTEMPERATURE__CONFIG__POLYNOMIAL_2     *ptr_data);




#endif
