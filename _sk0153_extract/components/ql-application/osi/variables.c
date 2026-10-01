/*=============================================================================
 * File       :  VARIABLES.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - variables
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>
#include <string.h>

/* user     includes */
#include "debug_my.h"
#include "drvtemperature.h"
#include "phone.h"
#include "pod.h"
#include "regulation.h"
#include "status.h"
#include "temperature.h"
#include "typedef.h"
#include "utility.h"
#include "variables.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING   200


/*-----------------------------------------------------------------------------
 * Variable strings
 *-----------------------------------------------------------------------------*/
/* POD */
#define LABEL_VAR__POD            "POD"          // POD

/* time */
#define LABEL_VAR__TIME           "TIME"         // actual time

/* power supply */
#define LABEL_VAR__POWER          "POWER"        // presence of main power supply

/* digital input/output status */
#define LABEL_VAR__IN1            "IN1"          // digital input  IN1   status
#define LABEL_VAR__IN2            "IN2"          // digital input  IN2   status
#define LABEL_VAR__C1             "C1"           // digital input  C1    value
#define LABEL_VAR__C2             "C2"           // digital input  C2    value
#define LABEL_VAR__C1_F1          "C1_F1"        // digital input  C1 F1 value
#define LABEL_VAR__C1_F2          "C1_F2"        // digital input  C1 F2 value
#define LABEL_VAR__C1_F3          "C1_F3"        // digital input  C1 F3 value
#define LABEL_VAR__C2_F1          "C2_F1"        // digital input  C2 F1 value
#define LABEL_VAR__C2_F2          "C2_F2"        // digital input  C2 F2 value
#define LABEL_VAR__C2_F3          "C2_F3"        // digital input  C2 F3 value
#define LABEL_VAR__OUT1           "OUT1"         // digital output OUT1  status
#define LABEL_VAR__OUT2           "OUT2"         // digital output OUT2  status

/* temperature */
#define LABEL_VAR__TINT           "TINT"         // internal temperature
#define LABEL_VAR__TEXT           "TEXT"         // external temperature
#define LABEL_VAR__ADC_INT        "ADC_INT"      // internal ADC value
#define LABEL_VAR__ADC_EXT        "ADC_EXT"      // external ADC value

/* network registration */
#define LABEL_VAR__CREG           "CREG"         // GSM  network registration status                 (AT+CREG?)
#define LABEL_VAR__CGREG          "CGREG"        // GPRS network registration status                 (AT+CGREG?)

/* GSM network operator */
#define LABEL_VAR__COPS0          "COPS0"        // GSM network operator - long  alphanumeric format (AT+COPS?)
#define LABEL_VAR__COPS1          "COPS1"        // GSM network operator - short alphanumeric format (AT+COPS?)
#define LABEL_VAR__COPS2          "COPS2"        // GSM network operator - numeric            format (AT+COPS?)

/* GSM signal quality */
#define LABEL_VAR__RSSI           "RSSI"         // RSSI (received signal strength)                  (AT+CSQ)
#define LABEL_VAR__BER            "BER"          // BER  (channel bit error rate  )                  (AT+CSQ)

/* GSM local information */
#define LABEL_VAR__MCC            "MCC"          // MCC (Mobile Country Code)
#define LABEL_VAR__MNC            "MNC"          // MNC (Mobile Network Code)
#define LABEL_VAR__LAC            "LAC"          // LAC (Location Area  Code)
#define LABEL_VAR__CI             "CI"           // CI  (Cell Identifier)

/* action result */
#define LABEL_VAR__RESETRESULT    "RESETRESULT"  // reset result




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* debug string */
static ascii Variables_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
       void Variables_BuildVariables(VARIABLES__VARIABLES_VALUES *ptr_variables_values, STATUS__DEVICE_STATUS *ptr_device_status, PHONE__PHONE_STATUS *ptr_phone_status, POD__CONFIG__POD *ptr_config_pod, u8 *ptr_reset_result);

       void Variables_DecodeVariablesString(ascii *ptr_string,                ascii *ptr_string_decoded,        VARIABLES__VARIABLES_VALUES *ptr_variables_values);

static void Variables_GetVariableValue     (ascii *ptr_string_variable_label, ascii *ptr_string_variable_value, VARIABLES__VARIABLES_VALUES *ptr_variables_values);




/*=============================================================================
 * Function   : Variables_BuildVariables
 *
 * Description: build the variables
 * Input      : - ptr_variables_values: pointer to save the variables values
 *              - ptr_device_status   : pointer to device status
 *              - ptr_phone_status    : pointer to phone  status
 *              - ptr_config_pod      : pointer to "POD" configuration
 *              - ptr_reset_result    : pointer to reset result
 * Output     : -
 *=============================================================================*/
void Variables_BuildVariables(VARIABLES__VARIABLES_VALUES *ptr_variables_values, STATUS__DEVICE_STATUS *ptr_device_status, PHONE__PHONE_STATUS *ptr_phone_status, POD__CONFIG__POD *ptr_config_pod, u8 *ptr_reset_result)
{
    /* device status */
    static STATUS__DEVICE_STATUS                  device_status;

    /* phone  status */
    static PHONE__PHONE_STATUS                    phone_status;

    /* configurations */
    static POD__CONFIG__POD                       config_pod;

    /* reset result */
    static u8                                     reset_result;

    /* "active status OUT1" and "active status OUT2" configurations */
    static REGULATION__CONFIG__ACTIVE_STATUS_OUT1 config_active_status_out1;
    static REGULATION__CONFIG__ACTIVE_STATUS_OUT2 config_active_status_out2;

           bool                                   result;
           u8                                     i;

    (void)result;


    /* read the device status */
    result = Status_ReadDeviceStatus(&device_status);

    /* get the "POD" configuration */
    Pod_Config_Pod_Get(&config_pod);

    /* get the phone status */
    Phone_PhoneStatus_Get(&phone_status);

    /* reset result */
    reset_result = *ptr_reset_result;


    /* get the "active status OUT1" configuration */
    /* get the "active status OUT2" configuration */
    Regulation_Config_ActiveStatusOut1_Get(&config_active_status_out1);
    Regulation_Config_ActiveStatusOut2_Get(&config_active_status_out2);


    /* POD */
    strncpy(ptr_variables_values->pod, config_pod.pod, POD__MAX_LENGTH_POD);          // POD
    ptr_variables_values->pod[POD__MAX_LENGTH_POD] = 0x00;

    /* time */
    ptr_variables_values->time              =  device_status.time;                       // actual time

    /* power supply */
    ptr_variables_values->power             =  device_status.main_power_supply;          // presence of main power supply

    /* digital input/output status */
    ptr_variables_values->in1_enabled_status =  device_status.in1_enabled_status;         // digital input  IN1  enabled status
    ptr_variables_values->in2_enabled_status =  device_status.in2_enabled_status;         // digital input  IN2  enabled status
    ptr_variables_values->in1                =  device_status.in1;                        // digital input  IN1          status
    ptr_variables_values->in2                =  device_status.in2;                        // digital input  IN2          status
    ptr_variables_values->c1_enabled_status  =  device_status.c1_enabled_status;          // counter input  C1   enabled status
    ptr_variables_values->c2_enabled_status  =  device_status.c2_enabled_status;          // counter input  C2   enabled status
    ptr_variables_values->c1                 =  device_status.c1;                         // counter input  C1
    ptr_variables_values->c2                 =  device_status.c2;                         // counter input  C2
    if (config_active_status_out1.active_status)
        ptr_variables_values->out1           =  device_status.out1_physical;              // digital output OUT1         status (consider the activation status)
    else
        ptr_variables_values->out1           = !device_status.out1_physical;              // digital output OUT1         status (consider the activation status)
    if (config_active_status_out2.active_status)
        ptr_variables_values->out2           =  device_status.out2_physical;              // digital output OUT2         status (consider the activation status)
    else
        ptr_variables_values->out2           = !device_status.out2_physical;              // digital output OUT2         status (consider the activation status)

    /* temperature */
    ptr_variables_values->sensor_status_int  =  device_status.sensor_status_int;          // internal sensor status
    ptr_variables_values->sensor_status_ext  =  device_status.sensor_status_ext;          // external sensor status
    ptr_variables_values->tint               =  device_status.temperature_int;            // internal temperature
    ptr_variables_values->text               =  device_status.temperature_ext;            // external temperature
    ptr_variables_values->adc_int            =  device_status.adc_value_int;              // internal ADC value
    ptr_variables_values->adc_ext            =  device_status.adc_value_ext;              // external ADC value

    /* network registration */
    ptr_variables_values->creg               =  phone_status.creg;                        // GSM  network registration status                 (AT+CREG?)
    ptr_variables_values->cgreg              =  phone_status.cgreg;                       // GPRS network registration status                 (AT+CGREG?)

    /* GSM network operator */
    for (i = 0; i < (32 + 1); i++)
        ptr_variables_values->cops0[i]       =  phone_status.operator_long_string [i];    // GSM network operator - long  alphanumeric format (AT+COPS?)
    for (i = 0; i < (10 + 1); i++)
        ptr_variables_values->cops1[i]       =  phone_status.operator_short_string[i];    // GSM network operator - short alphanumeric format (AT+COPS?)
    for (i = 0; i < ( 6 + 1); i++)
        ptr_variables_values->cops2[i]       =  phone_status.operator_numeric     [i];    // GSM network operator - numeric            format (AT+COPS?)

    /* GSM signal quality */
    ptr_variables_values->rssi               =  phone_status.rssi;                        // RSSI (received signal strength)                  (AT+CSQ)
    ptr_variables_values->ber                =  phone_status.ber;                         // BER  (channel bit error rate  )                  (AT+CSQ)

    /* GSM local information */
    for (i = 0; i < ( 3 + 1); i++)
        ptr_variables_values->mcc[i]         =  phone_status.mcc[i];                      // MCC (Mobile Country Code)
    for (i = 0; i < ( 3 + 1); i++)
        ptr_variables_values->mnc[i]         =  phone_status.mnc[i];                      // MNC (Mobile Network Code)
    for (i = 0; i < ( 4 + 1); i++)
        ptr_variables_values->lac[i]         =  phone_status.lac[i];                      // LAC (Location Area  Code)
    for (i = 0; i < ( 4 + 1); i++)
        ptr_variables_values->ci [i]         =  phone_status.ci [i];                      // CI  (Cell Identifier)

    /* action result */
    ptr_variables_values->resetresult        =  reset_result;                             // reset result
}




/*=============================================================================
 * Function   : Variables_DecodeVariablesString
 *
 * Description: decode a command/answer string replacing the possible variable labels
 *              with the respective variable values
 * Input      : - ptr_string          : pointer to      the         command/answer string to be decoded
 *              - ptr_string_decoded  : pointer to save the decoded command/answer string
 *              - ptr_variables_values: pointer to variables values
 * Output     : -
 *=============================================================================*/
void Variables_DecodeVariablesString(ascii *ptr_string, ascii *ptr_string_decoded, VARIABLES__VARIABLES_VALUES *ptr_variables_values)
{
    static VARIABLES__VARIABLES_VALUES variables_values;

    static ascii                       string_decoded        [160 + 1];
    static ascii                       string_variable_label [160 + 1];
    static ascii                       string_variable_value [160 + 1];

           bool                        in_variable_label;
           u8                          variable_label_index_start;
           u8                          variable_label_index_stop;
           u8                          variable_label_len;

           ascii                       character;

           u8                          len_string;
           u8                          index;

           bool                        result;
           u8                          i;

    (void)result;


    snprintf(Variables_DebugString, sizeof(Variables_DebugString), "String to be decoded: \"%s\"", ptr_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, Variables_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);


    /* init */

    variables_values = *ptr_variables_values;

    string_decoded        [0] = 0x00;
    string_variable_label [0] = 0x00;
    string_variable_value [0] = 0x00;

    in_variable_label = FALSE;

    len_string = (u8)strlen(ptr_string);
    index      = 0;


    /* search for possible variables in the string */
    for (i = 0; i < len_string; i++)
    {
        /* extract the i-th character */
        character = *(ptr_string + i);


        /* check if we are inside a variable (between two '#' characters) */
        if (!in_variable_label)
        {
            /* we are outside a variable (between two '#' characters) */

            /* check if i-th character is '#' */
            if (character == '#')
            {
                /* i-th character is '#' (start of a variable) */

                in_variable_label          = TRUE;
                variable_label_index_start = i;

                string_variable_label[0] = 0x00;
            }
            else
            {
                /* i-th character is not '#' */

                /* copy the i-th character in the decoded string */
                if (index < 160)
                {
                    string_decoded[index    ] = character;
                    string_decoded[index + 1] = 0x00;
                    index++;
                }
            }
        }
        else
        {
            /* we are inside a variable (between two '#' characters) */

            /* check if i-th character is '#' */
            if (character == '#')
            {
                /* i-th character is '#' (end of a variable) */

                in_variable_label         = FALSE;
                variable_label_index_stop = i;

                if (variable_label_index_stop > variable_label_index_start)
                    variable_label_len = variable_label_index_stop - variable_label_index_start + 1 - 2;   // -2  -->  two '#' characters
                else
                    variable_label_len = 0;

                if (variable_label_len > 0)
                {
                    /* variable length is greater than zero (--> #variable_label#) */

                    /* get the variable value */
                    Variables_GetVariableValue(string_variable_label, string_variable_value, &variables_values);

                    /* remove the two '#' characters */
                    // NOTHING TO DO

                    /* replace the variable label with its variable value */
                    strncat(string_decoded, string_variable_value, 160);
                    string_decoded[160] = 0x00;

                    index += strlen(string_variable_value);
                }
                else
                {
                    /* variable length is equal to zero (--> ##) */

                    /* remove the two '#' characters and add one '#' character */
                    string_decoded[index    ] = '#';
                    string_decoded[index + 1] = 0x00;
                    index++;
                }
            }
            else
            {
                /* i-th character is not '#' */

                /* copy the i-th character in the variable label string */
                string_variable_label[i - variable_label_index_start - 1] = character;
                string_variable_label[i - variable_label_index_start    ] = 0x00;
            }
        }
    }


    snprintf(Variables_DebugString, sizeof(Variables_DebugString), "String decoded: \"%s\"", string_decoded);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, Variables_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);

    /* save the decoded string */
    strncpy(ptr_string_decoded, string_decoded, 160);
}




/*=============================================================================
 * Function   : Variables_GetVariableValue
 *
 * Description: get the variable value string associated to a variable label string
 * Input      : - ptr_string_variable_label: pointer to      the variable label string
 *            : - ptr_string_variable_value: pointer to save the variable value string associated to the variable label string
 *              - ptr_variables_values     : pointer to variables values
 * Output     : -
 *=============================================================================*/
static void Variables_GetVariableValue(ascii *ptr_string_variable_label, ascii *ptr_string_variable_value, VARIABLES__VARIABLES_VALUES *ptr_variables_values)
{
    u8    len_string_variable_label;

    bool  string_label_pod;
    bool  string_label_time;
    bool  string_label_power;
    bool  string_label_in1;
    bool  string_label_in2;
    bool  string_label_c1;
    bool  string_label_c2;
    bool  string_label_c1_f1;
    bool  string_label_c1_f2;
    bool  string_label_c1_f3;
    bool  string_label_c2_f1;
    bool  string_label_c2_f2;
    bool  string_label_c2_f3;
    bool  string_label_out1;
    bool  string_label_out2;
    bool  string_label_tint;
    bool  string_label_text;
    bool  string_label_adc_int;
    bool  string_label_adc_ext;
    bool  string_label_creg;
    bool  string_label_cgreg;
    bool  string_label_cops0;
    bool  string_label_cops1;
    bool  string_label_cops2;
    bool  string_label_rssi;
    bool  string_label_ber;
    bool  string_label_mcc;
    bool  string_label_mnc;
    bool  string_label_lac;
    bool  string_label_ci;
    bool  string_label_resetresult;


    snprintf(Variables_DebugString, sizeof(Variables_DebugString), "Variable label: \"%s\"", ptr_string_variable_label);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_HIGH, Variables_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);


    string_label_pod         = FALSE;
    string_label_time        = FALSE;
    string_label_power       = FALSE;
    string_label_in1         = FALSE;
    string_label_in2         = FALSE;
    string_label_c1          = FALSE;
    string_label_c2          = FALSE;
    string_label_c1_f1       = FALSE;
    string_label_c1_f2       = FALSE;
    string_label_c1_f3       = FALSE;
    string_label_c2_f1       = FALSE;
    string_label_c2_f2       = FALSE;
    string_label_c2_f3       = FALSE;
    string_label_out1        = FALSE;
    string_label_out2        = FALSE;
    string_label_tint        = FALSE;
    string_label_text        = FALSE;
    string_label_adc_int     = FALSE;
    string_label_adc_ext     = FALSE;
    string_label_creg        = FALSE;
    string_label_cgreg       = FALSE;
    string_label_cops0       = FALSE;
    string_label_cops1       = FALSE;
    string_label_cops2       = FALSE;
    string_label_rssi        = FALSE;
    string_label_ber         = FALSE;
    string_label_mcc         = FALSE;
    string_label_mnc         = FALSE;
    string_label_lac         = FALSE;
    string_label_ci          = FALSE;
    string_label_resetresult = FALSE;


    len_string_variable_label = (u8)strlen(ptr_string_variable_label);


    /* "POD"        variable */
    if      ( (                                                           (sizeof(LABEL_VAR__POD        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__POD        , (sizeof(LABEL_VAR__POD        ) - 1)) == 0                        ) )
        string_label_pod          = TRUE;

    /* "TIME"       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__TIME       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__TIME       , (sizeof(LABEL_VAR__TIME       ) - 1)) == 0                        ) )
         string_label_time        = TRUE;

    /* "POWER"      variable */
    else if ( (                                                           (sizeof(LABEL_VAR__POWER      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__POWER      , (sizeof(LABEL_VAR__POWER      ) - 1)) == 0                        ) )
         string_label_power       = TRUE;

    /* "IN1         variable */
    else if ( (                                                           (sizeof(LABEL_VAR__IN1        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__IN1        , (sizeof(LABEL_VAR__IN1        ) - 1)) == 0                        ) )
         string_label_in1         = TRUE;

    /* "IN2         variable */
    else if ( (                                                           (sizeof(LABEL_VAR__IN2        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__IN2        , (sizeof(LABEL_VAR__IN2        ) - 1)) == 0                        ) )
         string_label_in2         = TRUE;

    /* "C1          variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C1         ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C1         , (sizeof(LABEL_VAR__C1         ) - 1)) == 0                        ) )
         string_label_c1          = TRUE;

    /* "C2          variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C2         ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C2         , (sizeof(LABEL_VAR__C2         ) - 1)) == 0                        ) )
         string_label_c2          = TRUE;

    /* "C1 F1       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C1_F1      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C1_F1      , (sizeof(LABEL_VAR__C1_F1      ) - 1)) == 0                        ) )
         string_label_c1_f1       = TRUE;

    /* "C1 F3       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C1_F2      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C1_F2      , (sizeof(LABEL_VAR__C1_F2      ) - 1)) == 0                        ) )
         string_label_c1_f2       = TRUE;

    /* "C1 F3       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C1_F3      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C1_F3      , (sizeof(LABEL_VAR__C1_F3      ) - 1)) == 0                        ) )
         string_label_c1_f3       = TRUE;

    /* "C2 F1       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C2_F1      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C2_F1      , (sizeof(LABEL_VAR__C2_F1      ) - 1)) == 0                        ) )
         string_label_c2_f1       = TRUE;

    /* "C2 F3       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C2_F2      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C2_F2      , (sizeof(LABEL_VAR__C2_F2      ) - 1)) == 0                        ) )
         string_label_c2_f2       = TRUE;

    /* "C2 F3       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__C2_F3      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__C2_F3      , (sizeof(LABEL_VAR__C2_F3      ) - 1)) == 0                        ) )
         string_label_c2_f3       = TRUE;

    /* "OUT1        variable */
    else if ( (                                                           (sizeof(LABEL_VAR__OUT1       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__OUT1       , (sizeof(LABEL_VAR__OUT1       ) - 1)) == 0                        ) )
         string_label_out1        = TRUE;

    /* "OUT2        variable */
    else if ( (                                                           (sizeof(LABEL_VAR__OUT2       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__OUT2       , (sizeof(LABEL_VAR__OUT2       ) - 1)) == 0                        ) )
         string_label_out2        = TRUE;

    /* "TINT        variable */
    else if ( (                                                           (sizeof(LABEL_VAR__TINT       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__TINT       , (sizeof(LABEL_VAR__TINT       ) - 1)) == 0                        ) )
         string_label_tint        = TRUE;

    /* "TEXT        variable */
    else if ( (                                                           (sizeof(LABEL_VAR__TEXT       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__TEXT       , (sizeof(LABEL_VAR__TEXT       ) - 1)) == 0                        ) )
         string_label_text        = TRUE;

    /* "ADC_INT     variable */
    else if ( (                                                           (sizeof(LABEL_VAR__ADC_INT    ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__ADC_INT    , (sizeof(LABEL_VAR__ADC_INT    ) - 1)) == 0                        ) )
         string_label_adc_int     = TRUE;

    /* "ADC_EXT     variable */
    else if ( (                                                           (sizeof(LABEL_VAR__ADC_EXT    ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__ADC_EXT    , (sizeof(LABEL_VAR__ADC_EXT    ) - 1)) == 0                        ) )
         string_label_adc_ext     = TRUE;

    /* "CREG        variable */
    else if ( (                                                           (sizeof(LABEL_VAR__CREG       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__CREG       , (sizeof(LABEL_VAR__CREG       ) - 1)) == 0                        ) )
         string_label_creg        = TRUE;

    /* "CGREG       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__CGREG      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__CGREG      , (sizeof(LABEL_VAR__CGREG      ) - 1)) == 0                        ) )
         string_label_cgreg       = TRUE;

    /* "COPS0       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__COPS0      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__COPS0      , (sizeof(LABEL_VAR__COPS0      ) - 1)) == 0                        ) )
         string_label_cops0       = TRUE;

    /* "COPS1       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__COPS1      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__COPS1      , (sizeof(LABEL_VAR__COPS1      ) - 1)) == 0                        ) )
         string_label_cops1       = TRUE;

    /* "COPS2       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__COPS2      ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__COPS2      , (sizeof(LABEL_VAR__COPS2      ) - 1)) == 0                        ) )
         string_label_cops2       = TRUE;

    /* "RSSI"       variable */
    else if ( (                                                           (sizeof(LABEL_VAR__RSSI       ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__RSSI       , (sizeof(LABEL_VAR__RSSI       ) - 1)) == 0                        ) )
         string_label_rssi        = TRUE;

    /* "BER         variable */
    else if ( (                                                           (sizeof(LABEL_VAR__BER        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__BER        , (sizeof(LABEL_VAR__BER        ) - 1)) == 0                        ) )
         string_label_ber         = TRUE;

    /* "MCC         variable */
    else if ( (                                                           (sizeof(LABEL_VAR__MCC        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__MCC        , (sizeof(LABEL_VAR__MCC        ) - 1)) == 0                        ) )
         string_label_mcc         = TRUE;

    /* "MNC         variable */
    else if ( (                                                           (sizeof(LABEL_VAR__MNC        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__MNC        , (sizeof(LABEL_VAR__MNC        ) - 1)) == 0                        ) )
         string_label_mnc         = TRUE;

    /* "LAC         variable */
    else if ( (                                                           (sizeof(LABEL_VAR__LAC        ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__LAC        , (sizeof(LABEL_VAR__LAC        ) - 1)) == 0                        ) )
         string_label_lac         = TRUE;

    /* "CI          variable */
    else if ( (                                                           (sizeof(LABEL_VAR__CI         ) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__CI         , (sizeof(LABEL_VAR__CI         ) - 1)) == 0                        ) )
         string_label_ci          = TRUE;

    /* "RESETRESULT variable */
    else if ( (                                                           (sizeof(LABEL_VAR__RESETRESULT) - 1)  == len_string_variable_label) &&
              (strncmp(ptr_string_variable_label, LABEL_VAR__RESETRESULT, (sizeof(LABEL_VAR__RESETRESULT) - 1)) == 0                        ) )
         string_label_resetresult = TRUE;


    /* save empty string for the variable value string */
    *ptr_string_variable_value = 0x00;


    /* "POD" variable */
    if      (string_label_pod        )
    {
        /* save the found string for the variable value string */
        strncpy(ptr_string_variable_value, ptr_variables_values->pod, 160);
    }


    /* "TIME"       variable */
    else if (string_label_time       )
    {
        /* save the found string for the variable value string */
        sprintf(ptr_string_variable_value,
                "%.3s %02d/%02d/%04d %02d:%02d:%02d",

                /* weekday */
                Clock_WeekDayString(ptr_variables_values->time.week_day),

                /* date */
                ptr_variables_values->time.day,
                ptr_variables_values->time.month,
                ptr_variables_values->time.year,

                /* time */
                ptr_variables_values->time.hour,
                ptr_variables_values->time.minute,
                ptr_variables_values->time.second);
    }


    /* "POWER"      variable */
    else if (string_label_power      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->power)
            strncpy(ptr_string_variable_value, "1", 160);
        else
            strncpy(ptr_string_variable_value, "0", 160);
    }


    /* "IN1         variable */
    else if (string_label_in1        )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->in1_enabled_status)
        {
            if (ptr_variables_values->in1)
                strncpy(ptr_string_variable_value, "0", 160);
            else
                strncpy(ptr_string_variable_value, "1", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "IN2         variable */
    else if (string_label_in2        )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->in2_enabled_status)
        {
            if (ptr_variables_values->in2)
                strncpy(ptr_string_variable_value, "0", 160);
            else
                strncpy(ptr_string_variable_value, "1", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "C1          variable */
    else if (string_label_c1         )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c1_enabled_status)
            sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c1);
        else
            strncpy(ptr_string_variable_value, "---", 160);
    }


    /* "C2          variable */
    else if (string_label_c2         )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c2_enabled_status)
            sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c2);
        else
            strncpy(ptr_string_variable_value, "---", 160);
    }


    /* "C1 F1       variable */
    else if (string_label_c1_f1      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c1_enabled_status)
        {
            if (ptr_variables_values->c1_bands_enabled_status)
                sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c1_f1);
            else
                strncpy(ptr_string_variable_value, "---", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "C1 F2       variable */
    else if (string_label_c1_f2      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c1_enabled_status)
        {
            if (ptr_variables_values->c1_bands_enabled_status)
                sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c1_f2);
            else
                strncpy(ptr_string_variable_value, "---", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "C1 F3       variable */
    else if (string_label_c1_f3      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c1_enabled_status)
        {
            if (ptr_variables_values->c1_bands_enabled_status)
                sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c1_f3);
            else
                strncpy(ptr_string_variable_value, "---", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "C2 F1       variable */
    else if (string_label_c2_f1      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c2_enabled_status)
        {
            if (ptr_variables_values->c2_bands_enabled_status)
                sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c2_f1);
            else
                strncpy(ptr_string_variable_value, "---", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "C2 F2       variable */
    else if (string_label_c2_f2      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c2_enabled_status)
        {
            if (ptr_variables_values->c2_bands_enabled_status)
                sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c2_f2);
            else
                strncpy(ptr_string_variable_value, "---", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "C2 F3       variable */
    else if (string_label_c2_f3      )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->c2_enabled_status)
        {
            if (ptr_variables_values->c2_bands_enabled_status)
                sprintf(ptr_string_variable_value, "%ld", ptr_variables_values->c2_f3);
            else
                strncpy(ptr_string_variable_value, "---", 160);
        }
        else
        {
            strncpy(ptr_string_variable_value, "---", 160);
        }
    }


    /* "OUT1        variable */
    else if (string_label_out1       )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->out1)
            strncpy(ptr_string_variable_value, "1", 160);
        else
            strncpy(ptr_string_variable_value, "0", 160);
    }


    /* "OUT2        variable */
    else if (string_label_out2       )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->out2)
            strncpy(ptr_string_variable_value, "1", 160);
        else
            strncpy(ptr_string_variable_value, "0", 160);
    }


    /* "TINT        variable */
    else if (string_label_tint       )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->sensor_status_int == DRVTEMPERATURE__SENSOR__CONNECTED)
        {
            if (Temperature_IsValidTemperature(ptr_variables_values->tint))
            {
                Utility_S16ExpNToASCII(ptr_variables_values->tint, ptr_string_variable_value, -1, 1);    // sensor status - connected (temperature in     valid range)
            }
            else
            {
                strncpy(ptr_string_variable_value, "ERR", 160);                                       // sensor status - connected (temperature out of valid range)
            }
        }
        else
        {
            strncpy(ptr_string_variable_value, "ERR", 160);                                           // sensor status - unknown or disconnected or out of order
        }
    }


    /* "TEXT        variable */
    else if (string_label_text       )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->sensor_status_ext == DRVTEMPERATURE__SENSOR__CONNECTED)
        {
            if (Temperature_IsValidTemperature(ptr_variables_values->text))
            {
                Utility_S16ExpNToASCII(ptr_variables_values->text, ptr_string_variable_value, -1, 1);    // sensor status - connected (temperature in     valid range)
            }
            else
            {
                strncpy(ptr_string_variable_value, "ERR", 160);                                       // sensor status - connected (temperature out of valid range)
            }
        }
        else
        {
            strncpy(ptr_string_variable_value, "ERR", 160);                                           // sensor status - unknown or disconnected or out of order
        }
    }


    /* "ADC_INT     variable */
    else if (string_label_adc_int    )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->sensor_status_int != DRVTEMPERATURE__SENSOR__UNKNOWN)
        {
            sprintf(ptr_string_variable_value, "%d", ptr_variables_values->adc_int);                  // sensor status -  not unknown
        }
        else
        {
            strncpy(ptr_string_variable_value, "ERR", 160);                                           // sensor status -     unknown
        }
    }



    /* "ADC_EXT     variable */
    else if (string_label_adc_ext    )
    {
        /* save the found string for the variable value string */
        if (ptr_variables_values->sensor_status_ext != DRVTEMPERATURE__SENSOR__UNKNOWN)
        {
            sprintf(ptr_string_variable_value, "%d", ptr_variables_values->adc_ext);                  // sensor status -  not unknown
        }
        else
        {
            strncpy(ptr_string_variable_value, "ERR", 160);                                           // sensor status -     unknown
        }
    }


    /* "CREG        variable */
    else if (string_label_creg        )
    {
        /* save the found string for the variable value string */
        sprintf(ptr_string_variable_value, "%d", ptr_variables_values->creg);
    }


    /* "CGREG       variable */
    else if (string_label_cgreg      )
    {
        /* save the found string for the variable value string */
        sprintf(ptr_string_variable_value, "%d", ptr_variables_values->cgreg);
    }


    /* "COPS0       variable */
    else if (string_label_cops0      )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->cops0) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->cops0, 160);
        else
            strncpy(ptr_string_variable_value, "???"                      , 160);
    }


    /* "COPS1       variable */
    else if (string_label_cops1      )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->cops1) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->cops1, 160);
        else
            strncpy(ptr_string_variable_value, "???"                      , 160);
    }


    /* "COPS2       variable */
    else if (string_label_cops2      )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->cops2) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->cops2, 160);
        else
            strncpy(ptr_string_variable_value, "???"                      , 160);
    }


    /* "RSSI"       variable */
    else if (string_label_rssi       )
    {
        /* save the found string for the variable value string */
        sprintf(ptr_string_variable_value, "%d", ptr_variables_values->rssi);
    }


    /* "BER         variable */
    else if (string_label_ber        )
    {
        /* save the found string for the variable value string */
        sprintf(ptr_string_variable_value, "%d", ptr_variables_values->ber);
    }


    /* "MCC         variable */
    else if (string_label_mcc        )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->mcc) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->mcc, 160);
        else
            strncpy(ptr_string_variable_value, "???"                    , 160);
    }


    /* "MNC         variable */
    else if (string_label_mnc        )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->mnc) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->mnc, 160);
        else
            strncpy(ptr_string_variable_value, "???"                    , 160);
    }


    /* "LAC         variable */
    else if (string_label_lac        )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->lac) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->lac, 160);
        else
            strncpy(ptr_string_variable_value, "???"                    , 160);
    }


    /* "CI          variable */
    else if (string_label_ci         )
    {
        /* save the found string for the variable value string */
        if (strlen(ptr_variables_values->ci) > 0)
            strncpy(ptr_string_variable_value, ptr_variables_values->ci , 160);
        else
            strncpy(ptr_string_variable_value, "???"                    , 160);
    }


    /* "RESETRESULT variable */
    else if (string_label_resetresult)
    {
        /* save the found string for the variable value string */
        strncpy(ptr_string_variable_value, "OK", 160);
    }


    snprintf(Variables_DebugString, sizeof(Variables_DebugString), "Variable value: \"%s\"", ptr_string_variable_value);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_HIGH, Variables_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);
}
