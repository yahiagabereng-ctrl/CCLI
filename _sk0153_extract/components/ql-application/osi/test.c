/*=============================================================================
 * File       :  TEST.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  TEST - Test
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "alarm.h"
#include "clock.h"
#include "file_csv.h"
#include "log_status.h"
#include "phone.h"
#include "pod.h"
#include "program_flash.h"
#include "queue_my.h"
#include "reset.h"
#include "sms_answer_event.h"
#include "status.h"
#include "synchronize.h"
#include "test.h"
#include "typedef.h"
#include "variables.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* tests */
void Test_PutRecord_FwUpgrade      (u8 num_records);
void Test_PutRecord_Alarm          (u8 num_records);
void Test_PutRecord_SmsAnswer      (u8 num_records);
void Test_PutRecord_SmsForward     (u8 num_records);
void Test_PutRecord_Autosynchronize(u8 num_records);
void Test_PutRecord_SmsAnswerEvent (u8 num_records);
void Test_PutRecord_FileStatusLog  (u8 num_records, const ascii *file_name);
void Test_PutRecord_FileAlarm      (u8 num_records, const ascii *file_name);

void Test_PutRecord_File_StatusLog (u8 num_records);




/*=============================================================================
 * Function   : Test_PutRecord_FwUpgrade
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_FwUpgrade(u8 num_records)
{
    QUEUE__FW_UPGRADE_QUEUE_RECORD record;
    CLOCK__TIME                    time;
    RESET__DOTA_RESULT             dota_result;
    RESET__DOTA_PHONE_NUMBER       dota_phone_number;
    bool                           result;
    u8                             i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        /* get DOTA result       */
        /* get DOTA phone number */
        Reset_DotaResult_Get     (&dota_result      );
        Reset_DotaPhoneNumber_Get(&dota_phone_number);

        /* get the actual time */
        result = Clock_GetTime(&time);

        /* build the FW upgrade record */
        strncpy(record.sms_phone_number, dota_phone_number.dota_phone_number, 20);  /* SMS DOTA phone number */
        record.sms_phone_number[20] = 0x00;
        record.time       = time;                                                      /* time                  */
        record.result     = dota_result.dota_result;                                   /* FW upgrade result     */
        record.error_code = dota_result.error_code;                                    /* FW upgrade error code */

        /* put the FW upgrade record in the FW upgrade queue */
        result = Queue_FwUpgrade_PutRecord(&record);

        /* request the backup to flash objects */
        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE);   //@@@
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_Alarm
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_Alarm(u8 num_records)
{
    static ALARM__ALARM          alarm;
    static STATUS__DEVICE_STATUS device_status;

           u8                    alarm_type;
           u8                    alarm_par;

           bool                  result;
           u8                    i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        alarm_type = ALARM__ALARM_ID__IN1;
        alarm_par  = 0;

        /* alarm type ID */
        alarm.alarm_data.alarm_id  = alarm_type;   // alarm type ID

        /* alarm parameters */
        alarm.alarm_data.alarm_par = alarm_par;    // alarm parameter (optional)


        /* get device status */
        result = Status_GetDeviceStatus(&device_status);

        /* alarm device status */
        alarm.device_status = device_status;

        /* put the new alarm in the alarm queue */
        result = Queue_Alarm_PutRecord((QUEUE__ALARM_QUEUE_RECORD *)&alarm);
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_SmsAnswer
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_SmsAnswer(u8 num_records)
{
    QUEUE__SMS_ANSWER_QUEUE_RECORD record_sms_answer;
    CLOCK__TIME                    time;
    bool                           result;
    u8                             i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        /* get the actual time */
        result = Clock_GetTime(&time);

        /* build the SMS answer record */
        record_sms_answer.time = time;                                       /* time                       */
        strncpy(record_sms_answer.sms_phone_number, "+391111"    ,  20);     /* SMS recipient phone number */
        strncpy(record_sms_answer.answer_text     , "answer text", 160);     /* answer text                */
        record_sms_answer.sms_phone_number[ 20] = 0x00;
        record_sms_answer.answer_text     [160] = 0x00;

        /* put the SMS answer record in the SMS answer queue */
        result = Queue_SmsAnswer_PutRecord(&record_sms_answer);
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_SmsForward
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_SmsForward(u8 num_records)
{
    QUEUE__SMS_FORWARD_QUEUE_RECORD record_sms_forward;
    CLOCK__TIME                     time;
    bool                            result;
    u8                              i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        /* get the actual time */
        result = Clock_GetTime(&time);

        /* build the SMS forward record */
        record_sms_forward.time = time;                                    /* time         */
        strncpy(record_sms_forward.forward_text, "forward text", 160);     /* forward text */
        record_sms_forward.forward_text[160] = 0x00;

        /* put the SMS forward record in the SMS forward queue */
        result = Queue_SmsForward_PutRecord(&record_sms_forward);
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_Autosynchronize
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_Autosynchronize(u8 num_records)
{
    SYNCHRONIZE__SMS_COUNTER synchronize_sms_counter;
    bool                     result;
    u8                       i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        /* get    the autosynchronize SMS counter */
        Synchronize_SmsCounter_Get(&synchronize_sms_counter);

        /* update the autosynchronize SMS counter */
        synchronize_sms_counter.sms_counter++;
        Synchronize_SmsCounter_Set(&synchronize_sms_counter);

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__005_SYNCHRONIZE, PROGRAM_FLASH__FLASH_ID__005_SYNCHRONIZE__SMS_COUNTER);

        /* put a record in the autosynchronize transmission queue */
        result = Queue_Autosynchronize_PutRecord((QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *)&synchronize_sms_counter);
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_SmsAnswerEvent
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_SmsAnswerEvent(u8 num_records)
{
    static SMS_ANSWER_EVENT__SMS_ANSWER_EVENT sms_answer_event;
    static VARIABLES__VARIABLES_VALUES        variables_values;
    static STATUS__DEVICE_STATUS              device_status;
    static POD__CONFIG__POD                   config_pod;
    static PHONE__PHONE_STATUS                phone_status;
    static u8                                 reset_result;
           u8                                 answer_type;
           bool                               result;
           u8                                 i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        answer_type = SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT;

        /* build the variables */
        Variables_BuildVariables(&variables_values, &device_status, &phone_status, &config_pod, &reset_result);


        /* SMS recipient phone number */
        strncpy(sms_answer_event.sms_phone_number, "+391111", 20);
        sms_answer_event.sms_phone_number[20] = 0x00;

        /* variables values */
        sms_answer_event.variables_values = variables_values;

        /* answer type */
        sms_answer_event.answer_type = answer_type;


        /* put the SMS answer event record in the SMS answer event queue */
        result = Queue_SmsAnswerEvent_PutRecord((QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD *)&sms_answer_event);

    }
}




/*=============================================================================
 * Function   : Test_PutRecord_FileStatusLog
 *
 * Description:
 * Input      : - num_records:
 *              - file_name  :
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_FileStatusLog(u8 num_records, const ascii *file_name)
{
    static QUEUE__FILE_STATUS_LOG_QUEUE_RECORD file_status_loq_queue_record;

           CLOCK__TIME                         current_time;
           CLOCK__TIME                         start_time;
           CLOCK__TIME                         stop_time;

           bool                                result;
           u8                                  i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        /* get the current time */
        result = Clock_GetTime(&current_time);
        result = Clock_GetTime(&start_time  );
        result = Clock_GetTime(&stop_time   );


        strncpy(file_status_loq_queue_record.log_file_name, file_name, 68);   /* log file name */
        file_status_loq_queue_record.log_file_name[68] = 0x00;

        file_status_loq_queue_record.time       = current_time;                  /* time             */
        file_status_loq_queue_record.v          = 1;                             /* log file version */
        file_status_loq_queue_record.n          = 0;                             /* log file number  */
        file_status_loq_queue_record.start_time = start_time;                    /* start time       */
        file_status_loq_queue_record.stop_time  = stop_time;                     /* stop  time       */

        /* put a status log file in the file log status queue */
        result = Queue_FileStatusLog_PutRecord((QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *)&file_status_loq_queue_record);

        /* request the backup to flash objects */
      //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_FileAlarm
 *
 * Description:
 * Input      : - num_records:
 *              - file_name  :
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_FileAlarm(u8 num_records, const ascii *file_name)
{
    static QUEUE__FILE_ALARM_QUEUE_RECORD file_alarm_queue_record;

           CLOCK__TIME                    current_time;
           CLOCK__TIME                    start_time;
           CLOCK__TIME                    stop_time;

           bool                           result;
           u8                             i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;

    for (i = 0; i < num_records; i++)
    {
        /* get the current time */
        result = Clock_GetTime(&current_time);
        result = Clock_GetTime(&start_time  );
        result = Clock_GetTime(&stop_time   );


        strncpy(file_alarm_queue_record.alarm_file_name, file_name, 68); /* log file name */
        file_alarm_queue_record.alarm_file_name[68] = 0x00;

        file_alarm_queue_record.time       = current_time;                  /* time             */
        file_alarm_queue_record.v          = 1;                             /* log file version */
        file_alarm_queue_record.n          = 0;                             /* log file number  */
        file_alarm_queue_record.start_time = start_time;                    /* start time       */
        file_alarm_queue_record.stop_time  = stop_time;                     /* stop  time       */

        /* put a status log file in the file alarm queue */
        result = Queue_FileStatusLog_PutRecord((QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *)&file_alarm_queue_record);

        /* request the backup to flash objects */
      //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE);   //@@@
    }
}




/*=============================================================================
 * Function   : Test_PutRecord_File_StatusLog
 *
 * Description:
 * Input      : - num_records:
 * Output     : -
 *=============================================================================*/
void Test_PutRecord_File_StatusLog(u8 num_records)
{
    u8 i;


    for (i = 0; i < num_records; i++)
    {
        LogStatus_LogFile_AppendDataRecord(FILE_CSV__REC_TYPE__LOG, 0, 0);
    }
}
