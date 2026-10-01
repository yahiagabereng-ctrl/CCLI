/*=============================================================================
 * File       :  STATUS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - device status
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __STATUS_H__


#define __STATUS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* device status */
typedef struct
{
    /* time */

    CLOCK__TIME time;                     /* time */


    /* device status */

    /* power supply */
    bool        main_power_supply;        /* presence of main power supply [FALSE: not present, TRUE: present] */
    bool        battery_charge_status;    /* battery charge status         [FALSE: disabled   , TRUE: enabled] */
    u16         gsm_module_power_supply;  /* GSM module power supply (mV)                                      */

    /* temperatures */
    u8          sensor_status_int;        /* internal sensor status        */
    u8          sensor_status_ext;        /* external sensor status        */
    s16         temperature_int;          /* internal temperature (/10 �C) */
    s16         temperature_ext;          /* external temperature (/10 �C) */
    u16         adc_value_int;            /* internal ADC value            */
    u16         adc_value_ext;            /* external ADC value            */

    /* digital inputs */
    bool        in1_enabled_status;       /* digital input  1 enabled status */
    bool        in2_enabled_status;       /* digital input  2 enabled status */
    bool        in1;                      /* digital input  1                */
    bool        in2;                      /* digital input  2                */
    bool        in3;                      /* digital input  3                */
    bool        in4;                      /* digital input  4                */

    /* counter inputs */
    bool        c1_enabled_status;        /* counter input  1 enabled       status   */
    bool        c2_enabled_status;        /* counter input  2 enabled       status   */
    bool        c1_bands_enabled_status;  /* counter input  1 enabled bands status   */
    bool        c2_bands_enabled_status;  /* counter input  2 enabled bands status   */
    u8          c1_band_type;             /* counter input  1 band type (F1, F2, F3) */
    u8          c2_band_type;             /* counter input  2 band type (F1, F2, F3) */
    u32         c1;                       /* counter input  1                        */
    u32         c2;                       /* counter input  2                        */
    u32         c1_f1;                    /* counter input  1 - band F1              */
    u32         c1_f2;                    /* counter input  1 - band F2              */
    u32         c1_f3;                    /* counter input  1 - band F3              */
    u32         c2_f1;                    /* counter input  2 - band F1              */
    u32         c2_f2;                    /* counter input  2 - band F2              */
    u32         c2_f3;                    /* counter input  2 - band F3              */

    /* digital outputs */
    bool        out1_physical;            /* digital output 1 (         physical status) */
    bool        out2_physical;            /* digital output 2 (         physical status) */
    bool        out1_physical_expected;   /* digital output 1 (expected physical status) */
    bool        out2_physical_expected;   /* digital output 2 (expected physical status) */
    bool        out1_logical;             /* digital output 1 (         logical  status) */
    bool        out2_logical;             /* digital output 2 (         logical  status) */
} STATUS__DEVICE_STATUS;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
bool Status_ReadDeviceStatus(STATUS__DEVICE_STATUS *ptr_device_status);

bool Status_GetDeviceStatus (STATUS__DEVICE_STATUS *ptr_device_status);
bool Status_SetDeviceStatus (STATUS__DEVICE_STATUS *ptr_device_status);




#endif
