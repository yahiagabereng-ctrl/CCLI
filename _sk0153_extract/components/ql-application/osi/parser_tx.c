/*=============================================================================
 * File       :  PARSER_TX.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - transmitted command parser
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
#include "alarm.h"
#include "boot.h"
#include "commands.h"
#include "debug_my.h"
#include "id.h"
#include "messages.h"
#include "parser_tx.h"
#include "phone.h"
#include "queue_my.h"
#include "reset.h"
#include "sms_answer_event.h"
#include "status.h"
#include "strings_my.h"
#include "synchronize.h"
#include "typedef.h"
#include "variables.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* new line */
//#define NEW_LINE   "\n"       // (    <LF>:      0x0A)
#define NEW_LINE     "\r\n"     // (<CR><LF>: 0x0D 0x0A)




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* build text */
void ParserTx_BuildSmsFwUpgrade      (PARSER_TX__FW_UPGRADE_RECORD        *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsAlarm          (PARSER_TX__ALARM_RECORD             *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsAnswer      (PARSER_TX__SMS_ANSWER_RECORD        *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsAnswerToCrs (PARSER_TX__SMS_ANSWER_TO_CRS_RECORD *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsForward     (PARSER_TX__SMS_FORWARD_RECORD       *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsAutosynchronize(PARSER_TX__AUTOSYNCHRONIZE_RECORD   *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsAnswerEvent (PARSER_TX__SMS_ANSWER_EVENT_RECORD  *ptr_record, ascii *ptr_text);




/*=============================================================================
 * Function   : ParserTx_BuildSmsFwUpgrade
 *
 * Description: build the "FW upgrade" SMS
 *
 *              Sintax:
 *                  "<date> <time> "<aaa_string>""
 *
 *                  - <date>     : date (dd/mm/yyyy)
 *                  - <time>     : time (hh:mm:ss)
 *
 *                  - <aaa_string>: aaa description
 *
 *              Example:
 *                    "28/09/2011 09:15:31 - "aaaaa""
 *
 * Input      : - ptr_record: pointer to FW upgrade record
 *              - ptr_text  : pointer to save ths SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsFwUpgrade(PARSER_TX__FW_UPGRADE_RECORD *ptr_record, ascii *ptr_text)
{
    /* SMS text string */
    static ascii        sms_text[160 + 1];

    /* ID */
           const ascii *ptr_id_fw_revision;

    /* language strings */
                 u16    id_string;
                 ascii *ptr_string_label_1;
                 ascii *ptr_string_label_2;
                 ascii *ptr_string_label_3;
                 ascii *ptr_string_value_1;
                 ascii *ptr_string_value_2;
                 ascii *ptr_string_value_3;
                 ascii *ptr_string_value_4;
                 ascii *ptr_string_value_5;
                 ascii *ptr_string_value_6;
                 ascii *ptr_string_value_7;
                 ascii *ptr_string_value_8;
                 ascii *ptr_string_value_9;
                 ascii *ptr_string_value_10;
                 ascii *ptr_string_value_11;

    static       ascii  string_fw_upgrade_result    [30 + 1];
    static       ascii  string_fw_upgrade_error_code[30 + 1];


    /* get the identifier */
    ptr_id_fw_revision = Id_GetIdFWRevision();


    id_string           = STRINGS__FW_UPGRADE__LABEL__FIRMWARE_UPGRADE_RESULT;          // "Firmware upgrade result"
    ptr_string_label_1  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__LABEL__ERROR;                            // "Error"
    ptr_string_label_2  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__LABEL__ACTUAL_VERSION;                   // "Actual version"
    ptr_string_label_3  = Strings_GetString(id_string);


    id_string           = STRINGS__FW_UPGRADE__VALUE__RESULT__UNKNOWN;                  // "unknown"
    ptr_string_value_1  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__RESULT__FAILED;                   // "failed"
    ptr_string_value_2  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__RESULT__EXECUTED;                 // "executed"
    ptr_string_value_3  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__UNKNOWN;                   // "unknown"
    ptr_string_value_4  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__NO_ERROR;                  // "no error"
    ptr_string_value_5  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__OTHER_ERROR;               // "other error"
    ptr_string_value_6  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__NO_GSM_REGISTRATION;       // "no GSM network registration"
    ptr_string_value_7  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__NO_APN_CONNECTION;         // "no GPRS APN connection"
    ptr_string_value_8  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_SERVER_CONNECTION;  // "no FTP server connection"
    ptr_string_value_9  = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FTP_FILE_DOWNLOAD;      // "no FTP file download"
    ptr_string_value_10 = Strings_GetString(id_string);

    id_string           = STRINGS__FW_UPGRADE__VALUE__ERROR__NO_FILE_INSTALLATION;      // "no file installation"
    ptr_string_value_11 = Strings_GetString(id_string);



    /* FW upgrade result     */
    if       (ptr_record->result == RESET__DOTA_RESULT__UNKNOWN)                          /* DOTA result - unknown */
    {
        strncpy(string_fw_upgrade_result, ptr_string_value_1, 30);
        string_fw_upgrade_result[30] = 0x00;
    }
    else if (ptr_record->result == RESET__DOTA_RESULT__ERROR   )                          /* DOTA result - error   */
    {
        strncpy(string_fw_upgrade_result, ptr_string_value_2, 30);
        string_fw_upgrade_result[30] = 0x00;
    }
    else if (ptr_record->result == RESET__DOTA_RESULT__SUCCESS )                          /* DOTA result - success */
    {
        strncpy(string_fw_upgrade_result, ptr_string_value_3, 30);
        string_fw_upgrade_result[30] = 0x00;
    }

    /* FW upgrade error code */
    if      (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__UNKNOWN             )   /* DOTA result error code - unknown                     */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_4 , 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__NO_ERROR            )   /* DOTA result error code - no error                    */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_5 , 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__OTHER_ERROR         )   /* DOTA result error code - other error                 */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_6 , 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__NO_GSM_REG          )   /* DOTA result error code - no GSM network registration */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_7 , 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__NO_GPRS_APN         )   /* DOTA result error code - no GPRS APN   connection    */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_8 , 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__NO_FTP_SERVER       )   /* DOTA result error code - no FTP server connection    */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_9 , 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__NO_FTP_FILE_DOWNLOAD)   /* DOTA result error code - no FTP file download        */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_10, 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }
    else if (ptr_record->error_code == RESET__DOTA_RESULT__ERROR__NO_FILE_INSTALL     )   /* DOTA result error code - no file install             */
    {
        strncpy(string_fw_upgrade_error_code, ptr_string_value_11, 30);
        string_fw_upgrade_error_code[30] = 0x00;
    }


    /* build the SMS text */
    if (ptr_record->result == RESET__DOTA_RESULT__SUCCESS)
    {
        /* DOTA result - success */
        snprintf(sms_text,
                 sizeof(sms_text),

               //"%02d/%02d/%04d %02d:%02d:%02d - "

                 "%s: %s."       NEW_LINE
                 "%s: \"%s\".",

               //ptr_record->time.day,      // day
               //ptr_record->time.month,    // month
               //ptr_record->time.year,     // year

               //ptr_record->time.hour,     // hour
               //ptr_record->time.minute,   // minute
               //ptr_record->time.second,   // second

                 ptr_string_label_1,
                 string_fw_upgrade_result,

                 ptr_string_label_3,
                 ptr_id_fw_revision);
    }
    else
    {
        /* DOTA result - unknown */
        /* DOTA result - error   */
        snprintf(sms_text,
                 sizeof(sms_text),

               //"%02d/%02d/%04d %02d:%02d:%02d - "

                 "%s: %s."       NEW_LINE
                 "%s: %s."       NEW_LINE
                 "%s: \"%s\".",

               //ptr_record->time.day,      // day
               //ptr_record->time.month,    // month
               //ptr_record->time.year,     // year

               //ptr_record->time.hour,     // hour
               //ptr_record->time.minute,   // minute
               //ptr_record->time.second,   // second

                 ptr_string_label_1,
                 string_fw_upgrade_result,

                 ptr_string_label_2,
                 string_fw_upgrade_error_code,

                 ptr_string_label_3,
                 ptr_id_fw_revision);
    }

    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, sms_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* copy the SMS text */
    strncpy(ptr_text, sms_text, 160);
    ptr_text[160] = 0x00;
}




/*=============================================================================
 * Function   : ParserTx_BuildSmsAlarm
 *
 * Description: build the "ALARM" SMS
 *
 *              Sintax:
 *                  "<date> <time> "<alarm_string>""
 *
 *                  - <date>        : date (dd/mm/yyyy)
 *                  - <time>        : time (hh:mm:ss)
 *
 *                  - <alarm_string>: alarm description
 *
 *              Example:
 *                    "28/09/2011 09:15:31 - "BATTERY ALMOST DISCHARGED""
 *
 * Input      : - ptr_record: pointer to alarm record
 *              - ptr_text  : pointer to save ths SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsAlarm(PARSER_TX__ALARM_RECORD *ptr_record, ascii *ptr_text)
{
    static MESSAGES__CONFIG__MESSAGE_IN1       config_message_in1;
    static MESSAGES__CONFIG__MESSAGE_IN2       config_message_in2;
    static MESSAGES__CONFIG__MESSAGE_IN3       config_message_in3;
    static MESSAGES__CONFIG__MESSAGE_IN4       config_message_in4;
    static MESSAGES__CONFIG__MESSAGE_TMIN      config_message_tmin;
    static MESSAGES__CONFIG__MESSAGE_TMAX      config_message_tmax;
    static MESSAGES__CONFIG__MESSAGE_TMINEXT   config_message_tminext;
    static MESSAGES__CONFIG__MESSAGE_TMAXEXT   config_message_tmaxext;

    static MESSAGES__CONFIG__MESSAGE_IN1_R     config_message_in1_r;
    static MESSAGES__CONFIG__MESSAGE_IN2_R     config_message_in2_r;
    static MESSAGES__CONFIG__MESSAGE_IN3_R     config_message_in3_r;
    static MESSAGES__CONFIG__MESSAGE_IN4_R     config_message_in4_r;
    static MESSAGES__CONFIG__MESSAGE_TMIN_R    config_message_tmin_r;
    static MESSAGES__CONFIG__MESSAGE_TMAX_R    config_message_tmax_r;
    static MESSAGES__CONFIG__MESSAGE_TMINEXT_R config_message_tminext_r;
    static MESSAGES__CONFIG__MESSAGE_TMAXEXT_R config_message_tmaxext_r;


    /* SMS text string */
    static ascii                               sms_text[160 + 1];

    /* alarm strings */
           ascii                              *string_alarm_descr;


    /* convert alarm ID to ASCII string */
    string_alarm_descr = Alarm_AlarmLongString(ptr_record->alarm_data.alarm_id);


    /* check if there is a message defined for the alarm */
    if      (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_INT_MIN         )
    {
        Messages_Config_MessagesTmin_Get   (&config_message_tmin   );
        if (strlen(config_message_tmin.message_tmin          ) > 0)
            string_alarm_descr = config_message_tmin.message_tmin;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_EXT_MIN         )
    {
        Messages_Config_MessagesTminExt_Get(&config_message_tminext);
        if (strlen(config_message_tminext.message_tminext    ) > 0)
            string_alarm_descr = config_message_tminext.message_tminext;
    }

    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_INT_MIN_RESTORED)
    {
        Messages_Config_MessagesTminR_Get   (&config_message_tmin_r   );
        if (strlen(config_message_tmin_r.message_tmin_r      ) > 0)
            string_alarm_descr = config_message_tmin_r.message_tmin_r;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_EXT_MIN_RESTORED)
    {
        Messages_Config_MessagesTminExtR_Get(&config_message_tminext_r);
        if (strlen(config_message_tminext_r.message_tminext_r) > 0)
            string_alarm_descr = config_message_tminext_r.message_tminext_r;
    }


    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_INT_MAX         )
    {
        Messages_Config_MessagesTmax_Get   (&config_message_tmax   );
        if (strlen(config_message_tmax.message_tmax          ) > 0)
            string_alarm_descr = config_message_tmax.message_tmax;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_EXT_MAX         )
    {
        Messages_Config_MessagesTmaxExt_Get(&config_message_tmaxext);
        if (strlen(config_message_tmaxext.message_tmaxext    ) > 0)
            string_alarm_descr = config_message_tmaxext.message_tmaxext;
    }

    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_INT_MAX_RESTORED)
    {
        Messages_Config_MessagesTmaxR_Get   (&config_message_tmax_r   );
        if (strlen(config_message_tmax_r.message_tmax_r      ) > 0)
            string_alarm_descr = config_message_tmax_r.message_tmax_r;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__T_EXT_MAX_RESTORED)
    {
        Messages_Config_MessagesTmaxExtR_Get(&config_message_tmaxext_r);
        if (strlen(config_message_tmaxext_r.message_tmaxext_r) > 0)
            string_alarm_descr = config_message_tmaxext_r.message_tmaxext_r;
    }


    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN1               )
    {
        Messages_Config_MessagesIn1_Get    (&config_message_in1    );
        if (strlen(config_message_in1.message_in1            ) > 0)
            string_alarm_descr = config_message_in1.message_in1;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN2               )
    {
        Messages_Config_MessagesIn2_Get    (&config_message_in2    );
        if (strlen(config_message_in2.message_in2            ) > 0)
            string_alarm_descr = config_message_in2.message_in2;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN3               )
    {
        Messages_Config_MessagesIn3_Get    (&config_message_in3    );
        if (strlen(config_message_in3.message_in3            ) > 0)
            string_alarm_descr = config_message_in3.message_in3;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN4               )
    {
        Messages_Config_MessagesIn4_Get    (&config_message_in4    );
        if (strlen(config_message_in4.message_in4            ) > 0)
            string_alarm_descr = config_message_in4.message_in4;
    }

    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN1_RESTORED      )
    {
        Messages_Config_MessagesIn1R_Get    (&config_message_in1_r    );
        if (strlen(config_message_in1_r.message_in1_r        ) > 0)
            string_alarm_descr = config_message_in1_r.message_in1_r;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN2_RESTORED      )
    {
        Messages_Config_MessagesIn2R_Get    (&config_message_in2_r    );
        if (strlen(config_message_in2_r.message_in2_r        ) > 0)
            string_alarm_descr = config_message_in2_r.message_in2_r;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN3_RESTORED      )
    {
        Messages_Config_MessagesIn3R_Get    (&config_message_in3_r    );
        if (strlen(config_message_in3_r.message_in3_r        ) > 0)
            string_alarm_descr = config_message_in3_r.message_in3_r;
    }
    else if (ptr_record->alarm_data.alarm_id == ALARM__ALARM_ID__IN4_RESTORED      )
    {
        Messages_Config_MessagesIn4R_Get    (&config_message_in4_r    );
        if (strlen(config_message_in4_r.message_in4_r        ) > 0)
            string_alarm_descr = config_message_in4_r.message_in4_r;
    }


    /* build the SMS text */
    snprintf(sms_text,
             sizeof(sms_text),

           //"%02d/%02d/%04d %02d:%02d:%02d - "

           //"\"%.80s\"",
             "%.80s",

           //ptr_record->device_status.time.day,      // day
           //ptr_record->device_status.time.month,    // month
           //ptr_record->device_status.time.year,     // year

           //ptr_record->device_status.time.hour,     // hour
           //ptr_record->device_status.time.minute,   // minute
           //ptr_record->device_status.time.second,   // second

             string_alarm_descr);                     // alarm description


    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, sms_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* copy the SMS text */
    strncpy(ptr_text, sms_text, 160);
    ptr_text[160] = 0x00;
}




/*=============================================================================
 * Function   : ParserTx_BuildSmsSmsAnswer
 *
 * Description: build the "SMS answer" SMS
 *
 *              Sintax:
 *                  "..."
 *
 *              Example:
 *                    "..."
 *
 * Input      : - ptr_record: pointer to SMS answer record
 *              - ptr_text  : pointer to save ths SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsSmsAnswer(PARSER_TX__SMS_ANSWER_RECORD *ptr_record, ascii *ptr_text)
{
    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, ptr_record->answer_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* copy the SMS text */
    strncpy(ptr_text, ptr_record->answer_text, 160);
    ptr_text[160] = 0x00;
}




/*=============================================================================
 * Function   : ParserTx_BuildSmsSmsAnswerToCrs
 *
 * Description: build the "SMS answer to CRS" SMS
 *
 *              Sintax:
 *                  "... *<checksum>"
 *
 *                  - <checksum>: checksum
 *
 *              Example:
 *                    "..."
 *
 * Input      : - ptr_record: pointer to SMS answer record
 *              - ptr_text  : pointer to save the SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsSmsAnswerToCrs(PARSER_TX__SMS_ANSWER_TO_CRS_RECORD *ptr_record, ascii *ptr_text)
{
    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, ptr_record->answer_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* copy the SMS text */
    strncpy(ptr_text, ptr_record->answer_text, 160);
    ptr_text[160] = 0x00;
}




/*=============================================================================
 * Function   : ParserTx_BuildSmsSmsForward
 *
 * Description: build the "SMS forward" SMS
 *
 *              Sintax:
 *                  "... *<checksum>"
 *
 *                  - <checksum>: checksum
 *
 *              Example:
 *                    "..."
 *
 * Input      : - ptr_record: pointer to SMS answer record
 *              - ptr_text  : pointer to save ths SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsSmsForward(PARSER_TX__SMS_FORWARD_RECORD *ptr_record, ascii *ptr_text)
{
    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, ptr_record->forward_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* copy the SMS text */
    strncpy(ptr_text, ptr_record->forward_text, 160);
    ptr_text[160] = 0x00;
}




/*=============================================================================
 * Function   : ParserTx_BuildSmsAutosynchronize
 *
 * Description: build the "AUTOSYNCHRONIZE" SMS
 *
 *              Sintax:
 *                  "AUTOSYNCHRONIZE IMEI=<imei> IMSI=<imsi> CCID=<ef_ccid> NSYNC=<n_sync>"
 *
 *                  - <imei>         : phone IMEI    (quoted string)
 *                  - <imsi>         : SIM IMSI      (quoted string)
 *                  - <ef_ccid>      : SIM CCID      (quoted string)
 *
 *                  - <n_sync>       : autosynchronize SMS counter
 *
 *              Example:
 *                    "AUTOSYNCHRONIZE IMEI="123456789012345" IMSI="123456789012345" CCID="12345678901234567890" NSYNC=123"
 *
 * Input      : - ptr_record: pointer to autosynchronize record
 *              - ptr_text  : pointer to save ths SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsAutosynchronize(PARSER_TX__AUTOSYNCHRONIZE_RECORD *ptr_record, ascii *ptr_text)
{
    /* SMS text string */
    static ascii                 sms_text[111 + 1];

    /* phone info */
    static PHONE__PHONE_ID       phone_id;
    static PHONE__PHONE_ACTIVITY phone_activity;

    /* SIM info */
    static PHONE__SIM_ID         sim_id;


    /* get the phone IDs           */
    /* get the phone activity info */
    /* get the SIM   IDs           */
    Phone_PhoneId_Get      (&phone_id      );
    Phone_PhoneActivity_Get(&phone_activity);
    Phone_SimId_Get        (&sim_id        );


    /* build the SMS text */
    snprintf(sms_text,
             sizeof(sms_text),

             "AUTOSYNCHRONIZE"

             " IMEI=\"%.15s\""
             " IMSI=\"%.15s\""
             " CCID=\"%.25s\""

             " NSYNC=%d",

             phone_id.imei,                 // IMEI
             sim_id.imsi,                   // IMSI    SIM card
             sim_id.ef_ccid,                // EF-CCID SIM card

             ptr_record->sms_counter);      // autosynchronize SMS counter


    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, sms_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* copy the SMS text */
    strncpy(ptr_text, sms_text, 160);
    ptr_text[160] = 0x00;
}




/*=============================================================================
 * Function   : ParserTx_BuildSmsSmsAnswerEvent
 *
 * Description: build the "SMS answer event" SMS
 *
 *              Sintax:
 *                  "..."
 *
 *              Example:
 *                    "..."
 *
 * Input      : - ptr_record: pointer to SMS answer record
 *              - ptr_text  : pointer to save ths SMS text
 * Output     : -
 *=============================================================================*/
void ParserTx_BuildSmsSmsAnswerEvent(PARSER_TX__SMS_ANSWER_EVENT_RECORD *ptr_record, ascii *ptr_text)
{
    /* SMS text string */
    static ascii                                sms_text[160 + 1];

    static COMMANDS__CONFIG__ANSWER_DETACHMENT  config_answer_detachment;
    static COMMANDS__CONFIG__ANSWER_RESTORE     config_answer_restore;
    static COMMANDS__CONFIG__ANSWER_STATUS      config_answer_status;
    static COMMANDS__CONFIG__ANSWER_RESET       config_answer_reset;

    static ascii                                string_configured_answer_decoded[160 + 1];

           ascii                               *ptr_string_configured_answer;


    switch (ptr_record->answer_type)
    {
        // SMS answer event - answer event for DETACHMENT command
        case SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, "Build DETACHMENT answer...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);

            /* get the "answer DETACHMENT" configuration */
            Commands_Config_AnswerDetachment_Get(&config_answer_detachment);

            ptr_string_configured_answer = config_answer_detachment.answer;
            break;


        // SMS answer event - answer event for RESTORE    command
        case SMS_ANSWER_EVENT__ANSWER_TYPE__RESTORE:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, "Build RESTORE answer..."   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);

            /* get the "answer RESTORE"    configuration */
            Commands_Config_AnswerRestore_Get   (&config_answer_restore   );

            ptr_string_configured_answer = config_answer_restore.answer;
            break;


        // SMS answer event - answer event for STATUS     command
        case SMS_ANSWER_EVENT__ANSWER_TYPE__STATUS:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, "Build STATUS answer..."    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);

            /* get the "answer STATUS"     configuration */
            Commands_Config_AnswerStatus_Get    (&config_answer_status    );

            ptr_string_configured_answer = config_answer_status.answer;
            break;


        // SMS answer event - answer event for RESET      command
        case SMS_ANSWER_EVENT__ANSWER_TYPE__RESET:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, "Build RESET answer..."     , __FILE__, __LINE__, (ascii *)__FUNCTION__, 40);

            /* get the "answer RESET"      configuration */
            Commands_Config_AnswerReset_Get     (&config_answer_reset     );

            ptr_string_configured_answer = config_answer_reset.answer;
            break;


        /* SMS answer event not found */
        default:
            ptr_string_configured_answer = NULL;
            break;
    }


    /* decode the configured answer string replacing the possible variable labels with their respective variable values */
    Variables_DecodeVariablesString(ptr_string_configured_answer, string_configured_answer_decoded, &ptr_record->variables_values);


    strncpy(sms_text, string_configured_answer_decoded, 160);
    ptr_text[160] = 0x00;

    /* print the SMS text */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_PARSER, DEBUG_TRACE_TYPE_LOW, sms_text, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* copy the SMS text */
    strncpy(ptr_text, sms_text, 160);
    ptr_text[160] = 0x00;
}
