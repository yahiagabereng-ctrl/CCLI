/*=============================================================================
 * File       :  LOG_STATUS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "antifrost" function
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __LOG_STATUS_H__


#define __LOG_STATUS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - status log" */
typedef struct
{
    bool status;     /* enable status  */
    u16  period;     /* period   (min) */
    u16  duration;   /* duration (min) */
} LOG_STATUS__CONFIG__STATUS_LOG;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void LogStatus_TaskLogStatus(void *argument);

/* events */
void LogStatus_Event_LogStatusDuration(CLOCK__TIME *ptr_rtc_rime);
void LogStatus_Event_LogStatusPeriod  (CLOCK__TIME *ptr_rtc_rime);
void LogStatus_Event_NewConfiguration (void);
void LogStatus_Event_ForceTransmission(void);
void LogStatus_Event_ClockChanged     (void);
void LogStatus_Event_LogStatusFileSent(ascii *file_name);

/* log file */
void LogStatus_LogFile_AppendDataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2);

/* next log status storing time */
bool LogStatus_NextLogStatusStoringTime(CLOCK__TIME *ptr_actual_rtc_time, CLOCK__TIME *ptr_user_wakeup_rtc_time);

/* get/set "status log" configuration */
void LogStatus_Config_StatusLog_GetDefault(LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);
void LogStatus_Config_StatusLog_Get       (LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);
void LogStatus_Config_StatusLog_Set       (LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);
bool LogStatus_Config_StatusLog_IsValid   (LOG_STATUS__CONFIG__STATUS_LOG *ptr_data);




#endif
