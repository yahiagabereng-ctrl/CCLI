/*=============================================================================
 * File       :  FILE_CSV.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - csv file
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdio.h>
#include <string.h>

/* user     includes */
#include "credit.h"
#include "debug_my.h"
#include "drvtemperature.h"
#include "file_csv.h"
#include "phone.h"
#include "status.h"
#include "temperature.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING    214




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static ascii FileCsv_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* build strings */
ascii *FileCsv_BuildString_Header    (void);
ascii *FileCsv_BuildString_DataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2);




/*===========================================================================
 * Function   : FileCsv_BuildString_Header
 *
 * Description: build the header string (terminated with <CR><LF>) for the csv file
 * Input      : -
 * Output     : - pointer to the header string built
 *===========================================================================*/
ascii *FileCsv_BuildString_Header(void)
{
    static ascii header_string[214 + 1];


    // "REC_TYPE;REC_SUBTYPE_1;REC_SUBTYPE_2;DATE;TIME;C1;C1_F1;C1_F2;C1_F3;C1_BAND_TYPE;C2;C2_F1;C2_F2;C2_F3;C2_BAND_TYPE;IN1;IN2;OUT1;OUT2;TINT;TEXT;ADC_INT;ADC_EXT;CREG;CGREG;RSSI;BER;POWER;CREDIT;VERSION;CHECKSUM\r\n"

    strncpy(header_string,
            "REC_TYPE;"
            "REC_SUBTYPE_1;"
            "REC_SUBTYPE_2;"
            "DATE;"
            "TIME;"
            "C1;"
            "C1_F1;"
            "C1_F2;"
            "C1_F3;"
            "C1_BAND_TYPE;"
            "C2;"
            "C2_F1;"
            "C2_F2;"
            "C2_F3;"
            "C2_BAND_TYPE;"
            "IN1;"
            "IN2;"
            "OUT1;"
            "OUT2;"
            "TINT;"
            "TEXT;"
            "ADC_INT;"
            "ADC_EXT;"
            "CREG;"
            "CGREG;"
            "RSSI;"
            "BER;"
            "POWER;"
            "CREDIT;"
            "VERSION;"
            "CHECKSUM"
            "\r\n",
            214);
    header_string[214] = 0x00;

    snprintf(FileCsv_DebugString, sizeof(FileCsv_DebugString), "%s", header_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, FileCsv_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return header_string;
}




/*===========================================================================
 * Function   : FileCsv_BuildString_DataRecord
 *
 * Description: build a data record string (terminated with <CR><LF>) for the csv file
 * Input      : - rec_type     : record type
 *              - rec_subtype_1: record subtype 1
 *              - rec_subtype_2: record subtype 2
 * Output     : - pointer to the data record string built
 *===========================================================================*/
ascii *FileCsv_BuildString_DataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2)
{
    /* device status */
    static STATUS__DEVICE_STATUS            device_status;

    /* phone  status */
    static PHONE__PHONE_STATUS              phone_status;

    /* "credit" configuration */
    static CREDIT__CONFIG__CREDIT           config_credit;

    /* available credit */
    static CREDIT__STATUS__AVAILABLE_CREDIT available_credit;

    static ascii                            string_type               [  5 + 1];
    static ascii                            string_subtype_1          [  5 + 1];
    static ascii                            string_subtype_2          [  5 + 1];

    static ascii                            string_c1                 [ 12 + 1];
    static ascii                            string_c1_f1              [ 12 + 1];
    static ascii                            string_c1_f2              [ 12 + 1];
    static ascii                            string_c1_f3              [ 12 + 1];
    static ascii                            string_c1_band_type       [  4 + 1];

    static ascii                            string_c2                 [ 12 + 1];
    static ascii                            string_c2_f1              [ 12 + 1];
    static ascii                            string_c2_f2              [ 12 + 1];
    static ascii                            string_c2_f3              [ 12 + 1];
    static ascii                            string_c2_band_type       [  4 + 1];

    static ascii                            string_in1                [  3 + 1];
    static ascii                            string_in2                [  3 + 1];

    static ascii                            string_out1               [  1 + 1];
    static ascii                            string_out2               [  1 + 1];

    static ascii                            string_temperature_int    [  8 + 1];
    static ascii                            string_temperature_ext    [  8 + 1];

    static ascii                            string_adc_value_int      [  5 + 1];
    static ascii                            string_adc_value_ext      [  5 + 1];

    static ascii                            string_power              [  3 + 1];
    static ascii                            string_credit             [  5 + 1];

    static ascii                            string_record             [160 + 1];

           u8                               len_string_record;

           u8                               checksum;

           u8                               nibble_h;
           u8                               nibble_l;

           u8                               hex_digit_h;
           u8                               hex_digit_l;


    // "REC_TYPE;REC_SUBTYPE_1;REC_SUBTYPE_2;DATE;TIME;C1;C1_F1;C1_F2;C1_F3;C1_BAND_TYPE;C2;C2_F1;C2_F2;C2_F3;C2_BAND_TYPE;IN1;IN2;OUT1;OUT2;TINT;TEXT;ADC_INT;ADC_EXT;CREG;CGREG;RSSI;BER;POWER;CREDIT;VERSION;CHECKSUM\r\n"
    // "L;-;-;10/06/2015;12:00:00;59861;59000;800;61;F1;-;-;-;-;-;-;1;0;0;21.5;-3.1;895;124;1;0;18;0;1;120;1;C2\r\n"
    // "A;6;-;10/06/2015;12:00:00;59861;59000;800;61;F1;-;-;-;-;-;-;1;0;0;21.5;-3.1;895;124;1;0;18;0;1;120;1;C2\r\n"


    /* read the device status */
    Status_ReadDeviceStatus(&device_status);

    /* get the phone status */
    Phone_PhoneStatus_Get(&phone_status);

    /* get the credit info */
    Credit_Config_Credit_Get         (&config_credit   );
    Credit_Status_AvailableCredit_Get(&available_credit);


    /* REC_TYPE */
    if      (rec_type == FILE_CSV__REC_TYPE__LOG  )
        sprintf(string_type, "L");
    else if (rec_type == FILE_CSV__REC_TYPE__ALARM)
        sprintf(string_type, "A");
    else
        sprintf(string_type, "?");

//@@@
    /* REC_SUBTYPE_1 */
    if      (rec_type == FILE_CSV__REC_TYPE__LOG  )
        sprintf(string_subtype_1, "-"                );
    else if (rec_type == FILE_CSV__REC_TYPE__ALARM)
        sprintf(string_subtype_1, "%d", rec_subtype_1);
    else
        sprintf(string_subtype_1, "?"                );

//@@@
    /* REC_SUBTYPE_2 */
    if      (rec_type == FILE_CSV__REC_TYPE__LOG  )
        sprintf(string_subtype_2, "-"                );
    else if (rec_type == FILE_CSV__REC_TYPE__ALARM)
        sprintf(string_subtype_2, "%d", rec_subtype_2);
    else
        sprintf(string_subtype_2, "?"                );


    /* C1 */
    if ((device_status.c1_enabled_status))
        sprintf(string_c1          , "%ld", device_status.c1          );
    else
        sprintf(string_c1          , "%s" , "-"                       );

    /* C1_F1 */
    if ((device_status.c1_enabled_status) && (device_status.c1_bands_enabled_status))
        sprintf(string_c1_f1       , "%ld", device_status.c1_f1       );
    else
        sprintf(string_c1_f1       , "%s" , "-"                       );

    /* C1_F2 */
    if ((device_status.c1_enabled_status) && (device_status.c1_bands_enabled_status))
        sprintf(string_c1_f2       , "%ld", device_status.c1_f2       );
    else
        sprintf(string_c1_f2       , "%s" , "-"                       );

    /* C1_F3 */
    if ((device_status.c1_enabled_status) && (device_status.c1_bands_enabled_status))
        sprintf(string_c1_f3       , "%ld", device_status.c1_f3       );
    else
        sprintf(string_c1_f3       , "%s" , "-"                       );

    /* C1_BAND_TYPE */
    if ((device_status.c1_enabled_status) && (device_status.c1_bands_enabled_status))
        sprintf(string_c1_band_type, "F%d", device_status.c1_band_type);
    else
        sprintf(string_c1_band_type, "%s" , "-"                       );


    /* C2 */
    if ((device_status.c2_enabled_status))
        sprintf(string_c2          , "%ld", device_status.c2          );
    else
        sprintf(string_c2          , "%s" , "-"                       );

    /* C2_F1 */
    if ((device_status.c2_enabled_status) && (device_status.c2_bands_enabled_status))
        sprintf(string_c2_f1       , "%ld", device_status.c2_f1       );
    else
        sprintf(string_c2_f1       , "%s" , "-"                       );

    /* C2_F2 */
    if ((device_status.c2_enabled_status) && (device_status.c2_bands_enabled_status))
        sprintf(string_c2_f2       , "%ld", device_status.c2_f2       );
    else
        sprintf(string_c2_f2       , "%s" , "-"                       );

    /* C2_F3 */
    if ((device_status.c2_enabled_status) && (device_status.c2_bands_enabled_status))
        sprintf(string_c2_f3       , "%ld", device_status.c2_f3       );
    else
        sprintf(string_c2_f3       , "%s" , "-"                       );

    /* C2_BAND_TYPE */
    if ((device_status.c2_enabled_status) && (device_status.c2_bands_enabled_status))
        sprintf(string_c2_band_type, "F%d", device_status.c2_band_type);
    else
        sprintf(string_c2_band_type, "%s" , "-"                       );


    /* IN1 */
    if (device_status.in1_enabled_status)
    {
        if (device_status.in1)
            string_in1[0] = '1';
        else
            string_in1[0] = '0';
    }
    else
    {
        string_in1[0] = '-';
    }
    string_in1[1] = 0x00;

    /* IN2 */
    if (device_status.in2_enabled_status)
    {
        if (device_status.in2)
            string_in2[0] = '1';
        else
            string_in2[0] = '0';
    }
    else
    {
        string_in2[0] = '-';
    }
    string_in2[1] = 0x00;


    /* OUT1 */
    if (device_status.out1_physical_expected)
        string_out1[0] = '1';
    else
        string_out1[0] = '0';
    string_out1[1] = 0x00;

    /* OUT2 */
    if (device_status.out2_physical_expected)
        string_out2[0] = '1';
    else
        string_out2[0] = '0';
    string_out2[1] = 0x00;


    /* TINT */
    if (device_status.sensor_status_int == DRVTEMPERATURE__SENSOR__CONNECTED)
    {
        if (Temperature_IsValidTemperature(device_status.temperature_int))
        {
            Utility_S16ExpNToASCII(device_status.temperature_int, string_temperature_int, -1, 1);       // sensor status - connected (temperature in     valid range)
        }
        else
        {
            strncpy(string_temperature_int, "?", (sizeof(string_temperature_int) - 1));              // sensor status - connected (temperature out of valid range)
            string_temperature_int[(sizeof(string_temperature_int) - 1)] = 0x00;
        }
    }
    else
    {
        strncpy(string_temperature_int, "?", (sizeof(string_temperature_int) - 1));                  // sensor status - unknown or disconnected or out of order
        string_temperature_int[(sizeof(string_temperature_int) - 1)] = 0x00;
    }

    /* TEXT */
    if (device_status.sensor_status_ext == DRVTEMPERATURE__SENSOR__CONNECTED)
    {
        if (Temperature_IsValidTemperature(device_status.temperature_ext))
        {
            Utility_S16ExpNToASCII(device_status.temperature_ext, string_temperature_ext, -1, 1);       // sensor status - connected (temperature in     valid range)
        }
        else
        {
            strncpy(string_temperature_ext, "?", (sizeof(string_temperature_ext) - 1));              // sensor status - connected (temperature out of valid range)
            string_temperature_ext[(sizeof(string_temperature_ext) - 1)] = 0x00;
        }
    }
    else
    {
        strncpy(string_temperature_ext, "?", (sizeof(string_temperature_ext) - 1));                  // sensor status - unknown or disconnected or out of order
        string_temperature_ext[(sizeof(string_temperature_ext) - 1)] = 0x00;
    }


    /* ADC_INT */
    if (device_status.sensor_status_int != DRVTEMPERATURE__SENSOR__UNKNOWN)
    {
        sprintf(string_adc_value_int, "%d", device_status.adc_value_int);                            // sensor status - not unknown
    }
    else
    {
        strncpy(string_adc_value_int, "?", (sizeof(string_adc_value_int) - 1));                      // sensor status -     unknown
        string_adc_value_int[(sizeof(string_adc_value_int) - 1)] = 0x00;
    }

    /* ADC_EXT */
    if (device_status.sensor_status_ext != DRVTEMPERATURE__SENSOR__UNKNOWN)
    {
        sprintf(string_adc_value_ext, "%d", device_status.adc_value_ext);                            // sensor status - not unknown
    }
    else
    {
        strncpy(string_adc_value_ext, "?", (sizeof(string_adc_value_ext) - 1));                      // sensor status -     unknown
        string_adc_value_ext[(sizeof(string_adc_value_ext) - 1)] = 0x00;
    }


    /* POWER */
    if (device_status.main_power_supply)
        string_power[0] = '1';
    else
        string_power[0] = '0';
    string_power[1] = 0x00;


    /* CREDIT */
    if (config_credit.status)
    {
        /* credit enabled */
        sprintf(string_credit, "%d", available_credit.available_credit);
    }
    else
    {
        /* credit disabled */
        strncpy(string_credit, "-", 4);
        string_credit[4] = 0x00;
    }


    /* build the record string */
    snprintf(string_record,
             sizeof(string_record) - 4,

             "%s;%s;%s;%02d/%02d/%04d;%02d:%02d:%02d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d;%d;%d;%d;%s;%s;%d;",

             string_type,
             string_subtype_1,
             string_subtype_2,

             device_status.time.day,     // day
             device_status.time.month,   // month
             device_status.time.year,    // year
             device_status.time.hour,    // hour
             device_status.time.minute,  // minute
             device_status.time.second,  // second

             string_c1,
             string_c1_f1,
             string_c1_f2,
             string_c1_f3,
             string_c1_band_type,

             string_c2,
             string_c2_f1,
             string_c2_f2,
             string_c2_f3,
             string_c2_band_type,

             string_in1,
             string_in2,

             string_out1,
             string_out2,

             string_temperature_int,
             string_temperature_ext,

             string_adc_value_int,
             string_adc_value_ext,

             phone_status.creg,
             phone_status.cgreg,
             phone_status.rssi,
             phone_status.ber,

             string_power,
             string_credit,

             FILE_CSV__FILE_VERSION);


    /* CHECKSUM */

    /* calculate the checksum */
    len_string_record = strlen(string_record);
    checksum = Utility_CalculateChecksum((u8 *)string_record, len_string_record, 0x00);

    /* calculate the 2 hex digits in ASCII format */
    nibble_h = (checksum >> 4) & 0x0F;
    nibble_l = (checksum     ) & 0x0F;
    if (nibble_h <= 9)
        hex_digit_h = '0'+ (nibble_h     );
    else
        hex_digit_h = 'A'+ (nibble_h - 10);
    if (nibble_l <= 9)
        hex_digit_l = '0'+ (nibble_l     );
    else
        hex_digit_l = 'A'+ (nibble_l - 10);

    /* add the checksum */
    string_record[len_string_record    ] = hex_digit_h;
    string_record[len_string_record + 1] = hex_digit_l;


    /* <CR><LF> */
    string_record[len_string_record + 2] = '\r';
    string_record[len_string_record + 3] = '\n';

    /* end of string */
    string_record[len_string_record + 4] = 0x00;


    snprintf(FileCsv_DebugString, sizeof(FileCsv_DebugString), "%s", string_record);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, FileCsv_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return string_record;
}
