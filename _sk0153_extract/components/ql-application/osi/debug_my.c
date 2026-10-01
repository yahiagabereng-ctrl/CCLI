/*=============================================================================
 * File       :  DEBUG.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - debug utilities
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "at_debug.h"
#include "clock.h"
#include "debug_my.h"
#include "fw_config.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* max buffer debug string length */
#define MAX_LENGTH_BUFFER_DEBUG_STRING  600

/* max strings name length */
#define LEN_MAX_TRACE_LEVEL_NAME        17                                      /* max length of trace level name */
#define LEN_MAX_FILE_NAME               25                                      /* max length of file        name */
#define LEN_MAX_FUNCTION_NAME           45                                      /* max length of function    name */
#define LEN_MAX_TASK_NAME               15                                      /* max length of task        name */

/* max strings length (the numeric constants depend on the source code below) */
#define LEN_MAX_STRING_TIME             (     23                       + 3  )   /* max length of time             string */
#define LEN_MAX_STRING_TRACE_LEVEL      ( 5 + LEN_MAX_TRACE_LEVEL_NAME + 3  )   /* max length of trace level name string */
#define LEN_MAX_STRING_HEADER           (     LEN_MAX_FILE_NAME        + 3 +    /* max length of header           string */  \
                                              11                       + 3 +                                                 \
                                              LEN_MAX_FUNCTION_NAME    + 3 +                                                 \
                                              LEN_MAX_TASK_NAME        + 3  )

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING         (MAX_LENGTH_BUFFER_DEBUG_STRING - (LEN_MAX_STRING_TIME + LEN_MAX_STRING_TRACE_LEVEL + LEN_MAX_STRING_HEADER))




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug port */
static       u32    Debug_DebugPort;

/* trace levels states and types */
static       bool   Debug_TraceLevelsState[DEBUG_TRACE_LEVELS_NUMBER];   // trace levels state (disabled / enabled)
static       bool   Debug_TraceLevelsType [DEBUG_TRACE_LEVELS_NUMBER];   // trace levels type  (low      / high   )

/* trace info to be printed */
static       bool   Debug_TraceInfoTime;                                 // indication if the time             has to be printed
static       bool   Debug_TraceInfoTraceLevelName;                       // indication if the trace level name has to be printed
static       bool   Debug_TraceInfoFileName;                             // indication if the file name        has to be printed
static       bool   Debug_TraceInfoLineNumber;                           // indication if the line             has to be printed
static       bool   Debug_TraceInfoFunctionName;                         // indication if the function         has to be printed
static       bool   Debug_TraceInfoTaskName;                             // indication if the task name        has to be printed

/* debug string */
static       ascii  Debug_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/* trace level names */
static const ascii *Debug_TraceLevelName[DEBUG_TRACE_LEVELS_NUMBER] =
{
    DEBUG_TRACE_LEVEL_NAME_LEVEL_1,                                      // level 1
    DEBUG_TRACE_LEVEL_NAME_LEVEL_2,                                      // level 2
    DEBUG_TRACE_LEVEL_NAME_LEVEL_3,                                      // level 3
    DEBUG_TRACE_LEVEL_NAME_LEVEL_4,                                      // level 4
    DEBUG_TRACE_LEVEL_NAME_LEVEL_5,                                      // level 5
    DEBUG_TRACE_LEVEL_NAME_LEVEL_6,                                      // level 6
    DEBUG_TRACE_LEVEL_NAME_LEVEL_7,                                      // level 7
    DEBUG_TRACE_LEVEL_NAME_LEVEL_8,                                      // level 8

    DEBUG_TRACE_LEVEL_NAME_LEVEL_9,                                      // level 9
    DEBUG_TRACE_LEVEL_NAME_LEVEL_10,                                     // level 10
    DEBUG_TRACE_LEVEL_NAME_LEVEL_11,                                     // level 11
    DEBUG_TRACE_LEVEL_NAME_LEVEL_12,                                     // level 12
    DEBUG_TRACE_LEVEL_NAME_LEVEL_13,                                     // level 13
    DEBUG_TRACE_LEVEL_NAME_LEVEL_14,                                     // level 14
    DEBUG_TRACE_LEVEL_NAME_LEVEL_15,                                     // level 15
    DEBUG_TRACE_LEVEL_NAME_LEVEL_16,                                     // level 16

    DEBUG_TRACE_LEVEL_NAME_LEVEL_17,                                     // level 17
    DEBUG_TRACE_LEVEL_NAME_LEVEL_18,                                     // level 18
    DEBUG_TRACE_LEVEL_NAME_LEVEL_19,                                     // level 19
    DEBUG_TRACE_LEVEL_NAME_LEVEL_20,                                     // level 20
    DEBUG_TRACE_LEVEL_NAME_LEVEL_21,                                     // level 21
    DEBUG_TRACE_LEVEL_NAME_LEVEL_22,                                     // level 22
    DEBUG_TRACE_LEVEL_NAME_LEVEL_23,                                     // level 23
    DEBUG_TRACE_LEVEL_NAME_LEVEL_24,                                     // level 24

    DEBUG_TRACE_LEVEL_NAME_LEVEL_25,                                     // level 25
    DEBUG_TRACE_LEVEL_NAME_LEVEL_26,                                     // level 26
    DEBUG_TRACE_LEVEL_NAME_LEVEL_27,                                     // level 27
    DEBUG_TRACE_LEVEL_NAME_LEVEL_28,                                     // level 28
    DEBUG_TRACE_LEVEL_NAME_LEVEL_29,                                     // level 29
    DEBUG_TRACE_LEVEL_NAME_LEVEL_30,                                     // level 30
    DEBUG_TRACE_LEVEL_NAME_LEVEL_31,                                     // level 31
    DEBUG_TRACE_LEVEL_NAME_LEVEL_32,                                     // level 32

    DEBUG_TRACE_LEVEL_NAME_LEVEL_33,                                     // level 33
    DEBUG_TRACE_LEVEL_NAME_LEVEL_34,                                     // level 34
    DEBUG_TRACE_LEVEL_NAME_LEVEL_35,                                     // level 35
    DEBUG_TRACE_LEVEL_NAME_LEVEL_36,                                     // level 36
    DEBUG_TRACE_LEVEL_NAME_LEVEL_37,                                     // level 37
    DEBUG_TRACE_LEVEL_NAME_LEVEL_38,                                     // level 38
    DEBUG_TRACE_LEVEL_NAME_LEVEL_39,                                     // level 39
    DEBUG_TRACE_LEVEL_NAME_LEVEL_40,                                     // level 40

    DEBUG_TRACE_LEVEL_NAME_LEVEL_41,                                     // level 41
    DEBUG_TRACE_LEVEL_NAME_LEVEL_42,                                     // level 42
    DEBUG_TRACE_LEVEL_NAME_LEVEL_43,                                     // level 43
    DEBUG_TRACE_LEVEL_NAME_LEVEL_44,                                     // level 44
    DEBUG_TRACE_LEVEL_NAME_LEVEL_45,                                     // level 45
    DEBUG_TRACE_LEVEL_NAME_LEVEL_46,                                     // level 46
    DEBUG_TRACE_LEVEL_NAME_LEVEL_47,                                     // level 47
    DEBUG_TRACE_LEVEL_NAME_LEVEL_48,                                     // level 48

    DEBUG_TRACE_LEVEL_NAME_LEVEL_49,                                     // level 49
    DEBUG_TRACE_LEVEL_NAME_LEVEL_50,                                     // level 50
    DEBUG_TRACE_LEVEL_NAME_LEVEL_51,                                     // level 51
    DEBUG_TRACE_LEVEL_NAME_LEVEL_52,                                     // level 52
    DEBUG_TRACE_LEVEL_NAME_LEVEL_53,                                     // level 53
    DEBUG_TRACE_LEVEL_NAME_LEVEL_54,                                     // level 54
    DEBUG_TRACE_LEVEL_NAME_LEVEL_55,                                     // level 55
    DEBUG_TRACE_LEVEL_NAME_LEVEL_56,                                     // level 56

    DEBUG_TRACE_LEVEL_NAME_LEVEL_57,                                     // level 57
    DEBUG_TRACE_LEVEL_NAME_LEVEL_58,                                     // level 58
    DEBUG_TRACE_LEVEL_NAME_LEVEL_59,                                     // level 59
    DEBUG_TRACE_LEVEL_NAME_LEVEL_60,                                     // level 60
    DEBUG_TRACE_LEVEL_NAME_LEVEL_61,                                     // level 61
    DEBUG_TRACE_LEVEL_NAME_LEVEL_62,                                     // level 62
    DEBUG_TRACE_LEVEL_NAME_LEVEL_63,                                     // level 63
    DEBUG_TRACE_LEVEL_NAME_LEVEL_64,                                     // level 64
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
             void   Debug_Init(void);

/* UART debug */
             void   Debug_SendDebugString1(u8     trace_level,
                                           u8     trace_type,
                                           ascii *debug_string,
                                           ascii *file_name,
                                           u32    line_number,
                                           ascii *function_name,
                                           u32    delay);
             void   Debug_SendDebugString2(u8     trace_level,
                                           u8     trace_type,
                                           ascii *debug_string,
                                           ascii *file_name,
                                           u32    line_number,
                                           ascii *function_name,
                                           u32    delay);

/* send debug string */
static       void   Debug_SendString(u16 port, ascii *string);

/* info */
       const ascii *Debug_TraceLevelsNameGet(u8 level);

/* set/get variables value */
             void   Debug_DebugPortSet              (u32 value);
             u32    Debug_DebugPortGet              (void);
             void   Debug_TraceLevelsStateSet       (u8 level, bool value);
             bool   Debug_TraceLevelsStateGet       (u8 level);
             void   Debug_TraceLevelsTypeSet        (u8 level, bool value);
             bool   Debug_TraceLevelsTypeGet        (u8 level);
             void   Debug_TraceInfoTimeSet          (bool value);
             bool   Debug_TraceInfoTimeGet          (void);
             void   Debug_TraceInfoTraceLevelNameSet(bool value);
             bool   Debug_TraceInfoTraceLevelNameGet(void);
             void   Debug_TraceInfoFileNameSet      (bool value);
             bool   Debug_TraceInfoFileNameGet      (void);
             void   Debug_TraceInfoLineNumberSet    (bool value);
             bool   Debug_TraceInfoLineNumberGet    (void);
             void   Debug_TraceInfoFunctionNameSet  (bool value);
             bool   Debug_TraceInfoFunctionNameGet  (void);
             void   Debug_TraceInfoTaskNameSet      (bool value);
             bool   Debug_TraceInfoTaskNameGet      (void);




/*===========================================================================
 * Function    : Debug_Init
 *
 * Description : init
 * Input       : -
 * Output      : -
 *===========================================================================*/
void Debug_Init(void)
{
    u8 i;


    /* debug port */
  //Debug_DebugPort = DEBUG__DEBUG_PORT_USB_AT;
    Debug_DebugPort = DEBUG__DEBUG_PORT_USB_AP_LOG;


    /* trace levels */
    for (i = 0; i < DEBUG_TRACE_LEVELS_NUMBER; i++)
    {
        Debug_TraceLevelsState[i] = FALSE;
        Debug_TraceLevelsType [i] = FALSE;
    }


    /* trace levels */
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_BOOT              - 1] = TRUE;     // level 1
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_DOTA              - 1] = TRUE;     // level 2
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_DEBUG             - 1] = TRUE;     // level 3
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_AT_DEBUG          - 1] = TRUE;     // level 4
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_TERMINAL          - 1] = TRUE;     // level 5
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_UTILITY           - 1] = TRUE;     // level 6
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_STARTUP           - 1] = TRUE;     // level 7
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_MAIN              - 1] = TRUE;     // level 8

  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_PHONE             - 1] = TRUE;     // level 9
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_PARSER            - 1] = TRUE;     // level 10
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_TEST              - 1] = TRUE;     // level 11
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_DRVGPIO           - 1] = FALSE;    // level 12
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_FLASH             - 1] = TRUE;     // level 13
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_CLOCK             - 1] = TRUE;     // level 14
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_USER_SYNCHRONIZE  - 1] = TRUE;     // level 15
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_USER_RETX         - 1] = TRUE;     // level 16

  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ADC               - 1] = TRUE;     // level 17
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_DRV_TEMPERATURE   - 1] = TRUE;     // level 18
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_DRV_BATTERY       - 1] = TRUE;     // level 19
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_SYNCHRONIZE       - 1] = TRUE;     // level 20
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ALARM             - 1] = TRUE;     // level 21
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_STATUS            - 1] = TRUE;     // level 22
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_TRANSMISSION_SMS  - 1] = TRUE;     // level 23
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS - 1] = TRUE;     // level 24

  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ENERGY            - 1] = TRUE;     // level 25
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_RESET             - 1] = TRUE;     // level 26
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_LED               - 1] = FALSE;    // level 27
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ALARM_TMAX        - 1] = TRUE;     // level 28
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ALARM_TMIN        - 1] = TRUE;     // level 29
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ALARM_INPUT       - 1] = TRUE;     // level 30
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ALARM_POWER       - 1] = TRUE;     // level 31
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_F_REGULATION      - 1] = TRUE;     // level 32

  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_F_CHRONO          - 1] = TRUE;     // level 33
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_F_ANTIFROST       - 1] = TRUE;     // level 34
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_CHARGER           - 1] = TRUE;     // level 35
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_DRVADCEXT         - 1] = TRUE;     // level 36
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_ADCEXT            - 1] = TRUE;     // level 37
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_PHONEBOOK         - 1] = TRUE;     // level 38
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_INPUTS            - 1] = TRUE;     // level 39
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_UI_LED            - 1] = TRUE;     // level 40

  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_REGULATION        - 1] = TRUE;     // level 41
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_OUTPUTS           - 1] = TRUE;     // level 42
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_CREDIT            - 1] = TRUE;     // level 43
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_RTC_ALARM         - 1] = TRUE;     // level 44
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_CALENDAR          - 1] = TRUE;     // level 45
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_QUEUE             - 1] = TRUE;     // level 46
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_INPUT_EVENT       - 1] = TRUE;     // level 47
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_SMS_ANSWER_EVENT  - 1] = TRUE;     // level 48

  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_FV_ACTION         - 1] = TRUE;     // level 49
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_OTHER             - 1] = TRUE;     // level 50
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_SMTP              - 1] = TRUE;     // level 51
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_LOG_STATUS        - 1] = TRUE;     // level 52
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_FILE_SYSTEM       - 1] = TRUE;     // level 53
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_IRQ               - 1] = TRUE;     // level 54
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_COUNTERS          - 1] = TRUE;     // level 55
  //Debug_TraceLevelsState[DEBUG_TRACE_LEVEL_HTTP              - 1] = TRUE;     // level 56


    /* trace info to be printed */
    Debug_TraceInfoTime           = TRUE;
    Debug_TraceInfoTraceLevelName = TRUE;
    Debug_TraceInfoFileName       = FALSE;
    Debug_TraceInfoLineNumber     = FALSE;
    Debug_TraceInfoFunctionName   = TRUE;
    Debug_TraceInfoTaskName       = FALSE;
}




/*===========================================================================
 * Function    : Debug_SendDebugString1
 *
 * Description : - send a debug string to the debug UART
 * Input       : - trace_level  : trace level (1-40)
 *               - trace_type   : trace type (low or high)
 *               - debug_string : debug string to be sent
 *               - file_name    : source file name string (where this function was called)
 *               - line_number  : line number      string (where this function was called)
 *               - function_name: function    name string (where this function was called)
 *               - delay        : final delay (ms)
 *
 *               Format of the debug message (complete format):
 *
 *                 Date      | Time    |                | Source file name     | Source line  | Source function name                    | Open AT task name| Debug message
 *                           |         |                |                      | number       |                                         |                  |
 *                 --------------------------------------------------------------------------------------------------------------------------------------------------------------
 *                 20/01/2011 10:25:32 - (03) PHONE     - ..\src\phone.c       - line:  1168 - Phone_AdlCallback_AtResponse_CREG        - Task Phone       - SMS sent
 *                 20/01/2011 10:25:35 - (04) PARSER    - ..\src\parser.c      - line:   325 - Parser_ParseCommandText                  - Task Parser      - Parser command OK
 * Output      : -
 *===========================================================================*/
void Debug_SendDebugString1(u8     trace_level,
                            u8     trace_type,
                            ascii *debug_string,
                            ascii *file_name,
                            u32    line_number,
                            ascii *function_name,
                            u32    delay)
{
          CLOCK__TIME       current_time;

          ql_task_status_t  task_status;
          QlOSStatus        err;

    const ascii            *task_name = "---";

          ascii             trace_level_string [ 25 + 1];
          ascii             current_time_string[ 32 + 1];
          ascii             header_string      [120 + 1];
          ascii             temp_string        [ 60 + 1];

          u32               len_debug_string;


    /* verify the trace level */
    if ((trace_level == 0) || (trace_level > DEBUG_TRACE_LEVELS_NUMBER))
        return;

    /* verify the trace type */
    if ((trace_type != DEBUG_TRACE_TYPE_LOW) && (trace_type != DEBUG_TRACE_TYPE_HIGH))
        return;


    /* verify if the debug trace level is enabled */
    if (!Debug_TraceLevelsState[trace_level - 1])
        return;

    /* verify if the high debug trace type is enabled (only for high trace type) */
    if (trace_type == DEBUG_TRACE_TYPE_HIGH)
    {
        if (!Debug_TraceLevelsType[trace_level - 1])
            return;
    }


    /* calculate the debug string length */
    len_debug_string = strlen(debug_string);

    /* check the debug string length */
  //if (len_debug_string == 0)
  //    return;


    if (len_debug_string > MAX_LENGTH_BUFFER_DEBUG_STRING)
        debug_string[MAX_LENGTH_BUFFER_DEBUG_STRING] = 0x00;


    /* get current time and convert it to string */
    Clock_GetTime(&current_time);

    /* get the active task ID and convert the active task name to string */
    err = ql_rtos_task_get_status(NULL, &task_status);
    if (err == QL_OSI_SUCCESS)
        task_name = task_status.pcTaskName;


    /* build the current time string */
    current_time_string[0] = 0x00;
    if (Debug_TraceInfoTime)
    {
        sprintf(current_time_string,
                "%.3s %02d/%02d/%04d %02d:%02d:%02d - ",
                Clock_WeekDayString(Clock_WeekDayOfADay(current_time.day, current_time.month, current_time.year)),
                current_time.day,
                current_time.month,
                current_time.year,
                current_time.hour,
                current_time.minute,
                current_time.second);
        current_time_string[26] = 0x00;
    }

    /* build the trace level string */
    trace_level_string[0] = 0x00;
    if (Debug_TraceInfoTraceLevelName)
    {
        snprintf(trace_level_string,
                 sizeof(trace_level_string),
                 "[%02d] %-*.*s - ",
                 trace_level,
                 LEN_MAX_TRACE_LEVEL_NAME,
                 LEN_MAX_TRACE_LEVEL_NAME,
                 Debug_TraceLevelName[trace_level - 1]);
    }

    /* build the header string */
    header_string[0] = 0x00;
    if (Debug_TraceInfoFileName)
    {
        sprintf(temp_string, "%-*.*s - ", LEN_MAX_FILE_NAME, LEN_MAX_FILE_NAME, file_name);
        strcat(header_string, temp_string);
    }
    if (Debug_TraceInfoLineNumber)
    {
        snprintf(temp_string, 15, "line: %5ld - ", line_number);
        strcat(header_string, temp_string);
    }
    if (Debug_TraceInfoFunctionName)
    {
        sprintf(temp_string, "%-*.*s - ", LEN_MAX_FUNCTION_NAME, LEN_MAX_FUNCTION_NAME, function_name);
        strcat(header_string, temp_string);
    }
    if (Debug_TraceInfoTaskName)
    {
        sprintf(temp_string, "%-*.*s - ", LEN_MAX_TASK_NAME, LEN_MAX_TASK_NAME, task_name);
        strcat(header_string, temp_string);
    }


    /* build the complete debug string */
    sprintf(Debug_DebugString,
            "%s%s%s%s\r\n",
            current_time_string,
            trace_level_string,
            header_string,
            debug_string);


    /* send the complete debug string to the debug UART port */
    Debug_SendString(Debug_DebugPort, Debug_DebugString);


    /* delay */
    if (delay > 0)
        ql_rtos_task_sleep_ms(delay);
}




/*===========================================================================
 * Function    : Debug_SendDebugString2
 *
 * Description : - send a debug string to the debug UART
 * Input       : - trace_level  : trace level (1-40)
 *               - trace_type   : trace type (low or high)
 *               - debug_string : debug string to be sent
 *               - file_name    : source file name string (where this function was called)
 *               - line_number  : line number      string (where this function was called)
 *               - function_name: function    name string (where this function was called)
 *               - delay        : final delay (ms)
 *
 *               Format of the debug message:
 *
 * Output      : -
 *===========================================================================*/
void Debug_SendDebugString2(u8     trace_level,
                            u8     trace_type,
                            ascii *debug_string,
                            ascii *file_name,
                            u32    line_number,
                            ascii *function_name,
                            u32    delay)
{
          CLOCK__TIME       current_time;

          ql_task_status_t  task_status;
          QlOSStatus        err;

    const ascii            *task_name = "---";

          ascii             trace_level_string [ 25 + 1];
          ascii             current_time_string[ 32 + 1];
          ascii             temp_string        [100 + 1];

          u32               len_debug_string;


    /* verify trace level */
    if ((trace_level == 0) || (trace_level > DEBUG_TRACE_LEVELS_NUMBER))
        return;

    /* verify the trace type */
    if ((trace_type != DEBUG_TRACE_TYPE_LOW) && (trace_type != DEBUG_TRACE_TYPE_HIGH))
        return;


    /* verify if the debug trace level is enabled */
    if (!Debug_TraceLevelsState[trace_level - 1])
        return;

    /* verify if the high debug trace type is enabled (only for high trace type) */
    if (trace_type == DEBUG_TRACE_TYPE_HIGH)
    {
        if (!Debug_TraceLevelsType[trace_level - 1])
            return;
    }


    /* calculate the debug string length */
    len_debug_string = strlen(debug_string);

    /* check the debug string length */
    if (len_debug_string == 0)
        return;
    if (len_debug_string > (sizeof(temp_string) - sizeof(current_time_string) - sizeof(trace_level_string) - 11))
        debug_string[sizeof(temp_string) - sizeof(current_time_string) - sizeof(trace_level_string) - 11] = 0x00;


    /* get current time and convert it to string */
    Clock_GetTime(&current_time);

    current_time_string[0] = 0x00;
    if (Debug_TraceInfoTime)
    {
        sprintf(current_time_string,
                "%.3s %02d/%02d/%04d %02d:%02d:%02d - ",
                Clock_WeekDayString(Clock_WeekDayOfADay(current_time.day, current_time.month, current_time.year)),
                current_time.day,
                current_time.month,
                current_time.year,
                current_time.hour,
                current_time.minute,
                current_time.second);
        current_time_string[26] = 0x00;
    }

    /* build the trace level string */
    trace_level_string[0] = 0x00;
    if (Debug_TraceInfoTraceLevelName)
    {
        snprintf(trace_level_string,
                 sizeof(trace_level_string),
                 "[%02d] %-*.*s - ",
                 trace_level,
                 LEN_MAX_TRACE_LEVEL_NAME,
                 LEN_MAX_TRACE_LEVEL_NAME,
                 Debug_TraceLevelName[trace_level - 1]);
    }

    /* get the active task ID and convert the active task name to string */
    err = ql_rtos_task_get_status(NULL, &task_status);
    if (err == QL_OSI_SUCCESS)
        task_name = task_status.pcTaskName;


    /* send the complete debug string to the debug UART port */
    Debug_SendString(Debug_DebugPort, "----------------------------------------------------------\r\n");

    /* file name */
    if (Debug_TraceInfoFileName)
    {
        sprintf(temp_string, "%s%sFile    : %s\r\n", current_time_string, trace_level_string, file_name    );
        Debug_SendString(Debug_DebugPort, temp_string);
    }

    /* line number */
    if (Debug_TraceInfoLineNumber)
    {
        sprintf(temp_string, "%s%sLine    : %ld\r\n", current_time_string, trace_level_string, line_number  );
        Debug_SendString(Debug_DebugPort, temp_string);
    }

    /* function name */
    if (Debug_TraceInfoFunctionName)
    {
        sprintf(temp_string, "%s%sFunction: %s\r\n", current_time_string, trace_level_string, function_name);
        Debug_SendString(Debug_DebugPort, temp_string);
    }

    /* task name */
    if (Debug_TraceInfoTaskName)
    {
        sprintf(temp_string, "%s%sTask    : %s\r\n", current_time_string, trace_level_string, task_name    );
        Debug_SendString(Debug_DebugPort, temp_string);
    }

    /* debug string */
    sprintf(temp_string, "%s%sMessage : %s\r\n", current_time_string, trace_level_string, debug_string);
    Debug_SendString(Debug_DebugPort, temp_string);

    Debug_SendString(Debug_DebugPort, "----------------------------------------------------------\r\n");


    /* delay */
    if (delay > 0)
        ql_rtos_task_sleep_ms(delay);
}




/*=============================================================================
 * Function   : Debug_SendString
 *
 * Description: send a debug string to a port
 * Input      : - port  : port to be used
 *              - string: debug string
 * Output     : -
 *=============================================================================*/
static void Debug_SendString(u16 port, ascii *string)
{
    switch (port)
    {
        // none
        case DEBUG__DEBUG_PORT_NONE:
            // NOTHING TO DO
            break;

        // USB AT port
        case DEBUG__DEBUG_PORT_USB_AT:
            AtDebug_SendString(string);
            break;

        // USB AP LOG port
        case DEBUG__DEBUG_PORT_USB_AP_LOG:
            QL_LOG_PRINTF_TAG(QL_LOG_LEVEL_INFO, QL_LOG_TAG_OPEN, "%s", string);
            break;
    }
}




/*=============================================================================
 * Function   : Debug_TraceLevelsNameGet
 *
 * Description: get the trace lavel name of the level specified
 * Input      : - level: trace level to be got
 * Output     : - trace lavel name of the level specified
 *=============================================================================*/
const ascii *Debug_TraceLevelsNameGet(u8 level)
{
    if ((level == 0) || (level > DEBUG_TRACE_LEVELS_NUMBER))
        return NULL;

    return Debug_TraceLevelName[level - 1];
}




/*=============================================================================
 * Function   : Debug_DebugPortSet
 *
 * Description: set the debug port
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_DebugPortSet(u32 value)
{
    if (
         (value != DEBUG__DEBUG_PORT_NONE      ) &&
         (value != DEBUG__DEBUG_PORT_USB_AT    ) &&
         (value != DEBUG__DEBUG_PORT_USB_AP_LOG)
       )
    {
       return;
    }

    Debug_DebugPort = value;
}




/*=============================================================================
 * Function   : Debug_DebugPortGet
 *
 * Description: get the debug port value
 * Input      : -
 * Output     : - debug port value
 *=============================================================================*/
u32 Debug_DebugPortGet(void)
{
    return Debug_DebugPort;
}




/*=============================================================================
 * Function   : Debug_TraceLevelsStateSet
 *
 * Description: set the trace lavel state of the level specified
 * Input      : - level: trace level to be set
 *              - value: value       to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceLevelsStateSet(u8 level, bool value)
{
    if ((level == 0) || (level > DEBUG_TRACE_LEVELS_NUMBER))
        return;

    Debug_TraceLevelsState[level - 1] = value;
}




/*=============================================================================
 * Function   : Debug_TraceLevelsStateGet
 *
 * Description: get the trace lavel state of the level specified
 * Input      : - level: trace level to be got
 * Output     : - trace lavel state of the level specified
 *=============================================================================*/
bool Debug_TraceLevelsStateGet(u8 level)
{
    if ((level == 0) || (level > DEBUG_TRACE_LEVELS_NUMBER))
        return FALSE;

    return Debug_TraceLevelsState[level - 1];
}




/*=============================================================================
 * Function   : Debug_TraceLevelsTypeSet
 *
 * Description: set the trace lavel type of the level specified
 * Input      : - level: trace level to be set
 *              - value: value       to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceLevelsTypeSet(u8 level, bool value)
{
    if ((level == 0) || (level > DEBUG_TRACE_LEVELS_NUMBER))
        return;

    Debug_TraceLevelsType[level - 1] = value;
}




/*=============================================================================
 * Function   : Debug_TraceLevelsTypeGet
 *
 * Description: get the trace lavel type of the level specified
 * Input      : - level: trace level to be got
 * Output     : - trace lavel type of the level specified
 *=============================================================================*/
bool Debug_TraceLevelsTypeGet(u8 level)
{
    if ((level == 0) || (level > DEBUG_TRACE_LEVELS_NUMBER))
        return FALSE;

    return Debug_TraceLevelsType[level - 1];
}




/*=============================================================================
 * Function   : Debug_TraceInfoTimeSet
 *
 * Description: set the indication flag for time printing
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceInfoTimeSet(bool value)
{
    Debug_TraceInfoTime = value;
}




/*=============================================================================
 * Function   : Debug_TraceInfoTimeGet
 *
 * Description: get the indication flag for time printing
 * Input      : -
 * Output     : - indication flag for time printing
 *=============================================================================*/
bool Debug_TraceInfoTimeGet(void)
{
    return Debug_TraceInfoTime;
}




/*=============================================================================
 * Function   : Debug_TraceInfoTraceLevelNameSet
 *
 * Description: set the indication flag for trace level name printing
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceInfoTraceLevelNameSet(bool value)
{
    Debug_TraceInfoTraceLevelName = value;
}




/*=============================================================================
 * Function   : Debug_TraceInfoTraceLevelNameGet
 *
 * Description: get the indication flag for trace level name printing
 * Input      : -
 * Output     : - indication flag for trace level name printing
 *=============================================================================*/
bool Debug_TraceInfoTraceLevelNameGet(void)
{
    return Debug_TraceInfoTraceLevelName;
}




/*=============================================================================
 * Function   : Debug_TraceInfoFileNameSet
 *
 * Description: set the indication flag for file name printing
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceInfoFileNameSet(bool value)
{
    Debug_TraceInfoFileName = value;
}




/*=============================================================================
 * Function   : Debug_TraceInfoFileNameGet
 *
 * Description: get the indication flag for file name printing
 * Input      : -
 * Output     : - indication flag for file name printing
 *=============================================================================*/
bool Debug_TraceInfoFileNameGet(void)
{
    return Debug_TraceInfoFileName;
}




/*=============================================================================
 * Function   : Debug_TraceInfoLineNumberSet
 *
 * Description: set the indication flag for line number printing
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceInfoLineNumberSet(bool value)
{
    Debug_TraceInfoLineNumber = value;
}




/*=============================================================================
 * Function   : Debug_TraceInfoLineNumberGet
 *
 * Description: get the indication flag for line number printing
 * Input      : -
 * Output     : - indication flag for line number printing
 *=============================================================================*/
bool Debug_TraceInfoLineNumberGet(void)
{
    return Debug_TraceInfoLineNumber;
}




/*=============================================================================
 * Function   : Debug_TraceInfoFunctionNameSet
 *
 * Description: set the indication flag for function name printing
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceInfoFunctionNameSet(bool value)
{
    Debug_TraceInfoFunctionName = value;
}




/*=============================================================================
 * Function   : Debug_TraceInfoFunctionNameGet
 *
 * Description: get the indication flag for function name printing
 * Input      : -
 * Output     : - indication flag for function name printing
 *=============================================================================*/
bool Debug_TraceInfoFunctionNameGet(void)
{
    return Debug_TraceInfoFunctionName;
}




/*=============================================================================
 * Function   : Debug_TraceInfoTaskNameSet
 *
 * Description: set the indication flag for task name printing
 * Input      : - value: value to be set
 * Output     : -
 *=============================================================================*/
void Debug_TraceInfoTaskNameSet(bool value)
{
    Debug_TraceInfoTaskName = value;
}




/*=============================================================================
 * Function   : Debug_TraceInfoTaskNameGet
 *
 * Description: get the indication flag for task name printing
 * Input      : -
 * Output     : - indication flag for task name printing
 *=============================================================================*/
bool Debug_TraceInfoTaskNameGet(void)
{
    return Debug_TraceInfoTaskName;
}
