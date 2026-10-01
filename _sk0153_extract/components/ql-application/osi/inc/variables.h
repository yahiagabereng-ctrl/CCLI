/*=============================================================================
 * File       :  VARIABLES.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - variables
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __VARIABLES_H__


#define __VARIABLES_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"
#include "phone.h"
#include "pod.h"
#include "status.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* variables values */
typedef struct
{
    /* POD */
    ascii       pod[POD__MAX_LENGTH_POD + 1];  // POD

    /* time */
    CLOCK__TIME time;                          // actual time

    /* power supply */
    bool        power;                         // presence of main power supply

    /* digital input/output status */
    bool        in1_enabled_status;            // digital input  IN1      enabled status
    bool        in2_enabled_status;            // digital input  IN2      enabled status
    bool        in1;                           // digital input  IN1              status
    bool        in2;                           // digital input  IN2              status
    bool        c1_enabled_status;             // counter input  C1       enabled status
    bool        c2_enabled_status;             // counter input  C2       enabled status
    bool        c1_bands_enabled_status;       // counter input  C1 bands enabled status
    bool        c2_bands_enabled_status;       // counter input  C2 bands enabled status
    u32         c1;                            // counter input  C1
    u32         c2;                            // counter input  C2
    u32         c1_f1;                         // counter input  C1 F1
    u32         c1_f2;                         // counter input  C1 F3
    u32         c1_f3;                         // counter input  C1 F3
    u32         c2_f1;                         // counter input  C2 F1
    u32         c2_f2;                         // counter input  C2 F2
    u32         c2_f3;                         // counter input  C2 F3
    bool        out1;                          // digital output OUT1             status (consider the activation status)
    bool        out2;                          // digital output OUT2             status (consider the activation status)

    /* temperature */
    u8          sensor_status_int;             // internal sensor status
    u8          sensor_status_ext;             // external sensor status
    s16         tint;                          // internal temperature
    s16         text;                          // external temperature
    u16         adc_int;                       // internal ADC value
    u16         adc_ext;                       // external ADC value

    /* network registration */
    u8          creg;                          // GSM  network registration status                 (AT+CREG?)
    u8          cgreg;                         // GPRS network registration status                 (AT+CGREG?)

    /* GSM network operator */
    ascii       cops0[32 + 1];                 // GSM network operator - long  alphanumeric format (AT+COPS?)
    ascii       cops1[10 + 1];                 // GSM network operator - short alphanumeric format (AT+COPS?)
    ascii       cops2[ 6 + 1];                 // GSM network operator - numeric            format (AT+COPS?)

    /* GSM signal quality */
    u8          rssi;                          // RSSI (received signal strength)                  (AT+CSQ)
    u8          ber;                           // BER  (channel bit error rate  )                  (AT+CSQ)

    /* GSM local information */
    ascii       mcc[3 + 1];                    // MCC (Mobile Country Code)
    ascii       mnc[3 + 1];                    // MNC (Mobile Network Code)
    ascii       lac[4 + 1];                    // LAC (Location Area  Code)
    ascii       ci [4 + 1];                    // CI  (Cell Identifier)

    /* action result */
    u8          resetresult;                   // reset result
} VARIABLES__VARIABLES_VALUES;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
void Variables_BuildVariables(VARIABLES__VARIABLES_VALUES *ptr_variables_values, STATUS__DEVICE_STATUS *ptr_device_status, PHONE__PHONE_STATUS *ptr_phone_status, POD__CONFIG__POD *ptr_config_pod, u8 *ptr_reset_result);

void Variables_DecodeVariablesString(ascii *ptr_string, ascii *ptr_string_decoded, VARIABLES__VARIABLES_VALUES *ptr_variables_values);




#endif
