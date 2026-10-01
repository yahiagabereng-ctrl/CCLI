/*=============================================================================
 * File       :  DEBUG_MY.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - debug utilities
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DEBUG_MY_H__


#define __DEBUG_MY_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "ql_log.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* debug port */
#define DEBUG__DEBUG_PORT_NONE                   0                        // none
#define DEBUG__DEBUG_PORT_USB_AT                 1                        // USB AT port
#define DEBUG__DEBUG_PORT_USB_AP_LOG             2                        // USB AP LOG port

/* trace types (low or high) */
#define DEBUG_TRACE_TYPE_LOW                     0                        // type low
#define DEBUG_TRACE_TYPE_HIGH                    1                        // type high

/* number of trace levels */
#define DEBUG_TRACE_LEVELS_NUMBER                64

/* trace levels (valid range: 1-64) */
#define DEBUG_TRACE_LEVEL_BOOT                   1                        // level 1
#define DEBUG_TRACE_LEVEL_DOTA                   2                        // level 2
#define DEBUG_TRACE_LEVEL_DEBUG                  3                        // level 3
#define DEBUG_TRACE_LEVEL_AT_DEBUG               4                        // level 4
#define DEBUG_TRACE_LEVEL_TERMINAL               5                        // level 5
#define DEBUG_TRACE_LEVEL_UTILITY                6                        // level 6
#define DEBUG_TRACE_LEVEL_STARTUP                7                        // level 7
#define DEBUG_TRACE_LEVEL_MAIN                   8                        // level 8
#define DEBUG_TRACE_LEVEL_PHONE                  9                        // level 9
#define DEBUG_TRACE_LEVEL_PARSER                 10                       // level 10
#define DEBUG_TRACE_LEVEL_TEST                   11                       // level 11
#define DEBUG_TRACE_LEVEL_DRVGPIO                12                       // level 12
#define DEBUG_TRACE_LEVEL_FLASH                  13                       // level 13
#define DEBUG_TRACE_LEVEL_CLOCK                  14                       // level 14
#define DEBUG_TRACE_LEVEL_USER_SYNCHRONIZE       15                       // level 15
#define DEBUG_TRACE_LEVEL_USER_RETX              16                       // level 16
#define DEBUG_TRACE_LEVEL_ADC                    17                       // level 17
#define DEBUG_TRACE_LEVEL_DRV_TEMPERATURE        18                       // level 18
#define DEBUG_TRACE_LEVEL_DRV_BATTERY            19                       // level 19
#define DEBUG_TRACE_LEVEL_SYNCHRONIZE            20                       // level 20
#define DEBUG_TRACE_LEVEL_ALARM                  21                       // level 21
#define DEBUG_TRACE_LEVEL_STATUS                 22                       // level 22
#define DEBUG_TRACE_LEVEL_TRANSMISSION_SMS       23                       // level 23
#define DEBUG_TRACE_LEVEL_TRANSMISSION_GPRS      24                       // level 24
#define DEBUG_TRACE_LEVEL_ENERGY                 25                       // level 25
#define DEBUG_TRACE_LEVEL_RESET                  26                       // level 26
#define DEBUG_TRACE_LEVEL_LED                    27                       // level 27
#define DEBUG_TRACE_LEVEL_ALARM_TMAX             28                       // level 28
#define DEBUG_TRACE_LEVEL_ALARM_TMIN             29                       // level 29
#define DEBUG_TRACE_LEVEL_ALARM_INPUT            30                       // level 30
#define DEBUG_TRACE_LEVEL_ALARM_POWER            31                       // level 31
#define DEBUG_TRACE_LEVEL_F_REGULATION           32                       // level 32
#define DEBUG_TRACE_LEVEL_F_CHRONO               33                       // level 33
#define DEBUG_TRACE_LEVEL_F_ANTIFROST            34                       // level 34
#define DEBUG_TRACE_LEVEL_CHARGER                35                       // level 35
#define DEBUG_TRACE_LEVEL_DRVADCEXT              36                       // level 36
#define DEBUG_TRACE_LEVEL_ADCEXT                 37                       // level 37
#define DEBUG_TRACE_LEVEL_PHONEBOOK              38                       // level 38
#define DEBUG_TRACE_LEVEL_INPUTS                 39                       // level 39
#define DEBUG_TRACE_LEVEL_UI_LED                 40                       // level 40
#define DEBUG_TRACE_LEVEL_REGULATION             41                       // level 41
#define DEBUG_TRACE_LEVEL_OUTPUTS                42                       // level 42
#define DEBUG_TRACE_LEVEL_CREDIT                 43                       // level 43
#define DEBUG_TRACE_LEVEL_RTC_ALARM              44                       // level 44
#define DEBUG_TRACE_LEVEL_CALENDAR               45                       // level 45
#define DEBUG_TRACE_LEVEL_QUEUE                  46                       // level 46
#define DEBUG_TRACE_LEVEL_INPUT_EVENT            47                       // level 47
#define DEBUG_TRACE_LEVEL_SMS_ANSWER_EVENT       48                       // level 48
#define DEBUG_TRACE_LEVEL_FV_ACTION              49                       // level 49
#define DEBUG_TRACE_LEVEL_OTHER                  50                       // level 50
#define DEBUG_TRACE_LEVEL_SMTP                   51                       // level 51
#define DEBUG_TRACE_LEVEL_LOG_STATUS             52                       // level 52
#define DEBUG_TRACE_LEVEL_FILE_SYSTEM            53                       // level 53
#define DEBUG_TRACE_LEVEL_IRQ                    54                       // level 54
#define DEBUG_TRACE_LEVEL_COUNTERS               55                       // level 55
#define DEBUG_TRACE_LEVEL_HTTP                   56                       // level 56

/* header strings of the trace levels */
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_1           "BOOT"                   // level 1
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_2           "DOTA"                   // level 2
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_3           "DEBUG"                  // level 3
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_4           "AT_DEBUG"               // level 4
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_5           "TERMINAL"               // level 5
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_6           "UTILITY"                // level 6
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_7           "STARTUP"                // level 7
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_8           "MAIN"                   // level 8

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_9           "PHONE"                  // level 9
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_10          "PARSER"                 // level 10
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_11          "TEST"                   // level 11
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_12          "DRVGPIO"                // level 12
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_13          "FLASH"                  // level 13
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_14          "CLOCK"                  // level 14
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_15          "USER_SYNCHRONIZE"       // level 15
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_16          "USER_RETX"              // level 16

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_17          "ADC"                    // level 17
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_18          "DRV_TEMPERATURE"        // level 18
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_19          "DRV_BATTERY"            // level 19
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_20          "SYNCHRONIZE"            // level 20
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_21          "ALARM"                  // level 21
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_22          "STATUS"                 // level 22
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_23          "TRANSMISSION_SMS"       // level 23
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_24          "TRANSMISSION_GPRS"      // level 24

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_25          "TRANSMISSION_MAIL"      // level 25
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_26          "RESET"                  // level 26
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_27          "LED"                    // level 27
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_28          "ALARM_TMAX"             // level 28
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_29          "ALARM_TMIN"             // level 29
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_30          "ALARM_INPUT"            // level 30
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_31          "ALARM_POWER"            // level 31
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_32          "F_REGULATION"           // level 32

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_33          "F_CHRONO"               // level 33
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_34          "F_ANTIFROST"            // level 34
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_35          "CHARGER"                // level 35
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_36          "DRVADCEXT"              // level 36
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_37          "ADCEXT"                 // level 37
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_38          "PHONEBOOK"              // level 38
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_39          "INPUTS"                 // level 39
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_40          "UI_LED"                 // level 40

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_41          "REGULATION"             // level 41
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_42          "OUTPUTS"                // level 42
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_43          "CREDIT"                 // level 43
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_44          "RTC_ALARM"              // level 44
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_45          "CALENDAR"               // level 45
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_46          "QUEUE"                  // level 46
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_47          "INPUT_EVENT"            // level 47
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_48          "SMS_ANSWER_EVENT"       // level 48

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_49          "FV_ACTION"              // level 49
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_50          "OTHER"                  // level 50
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_51          "SMTP"                   // level 51
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_52          "LOG_STATUS"             // level 52
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_53          "FILE_SYSTEM"            // level 53
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_54          "IRQ"                    // level 54
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_55          "COUNTERS"               // level 55
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_56          "HTTP"                   // level 56

#define DEBUG_TRACE_LEVEL_NAME_LEVEL_57          "???"                    // level 57
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_58          "???"                    // level 58
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_59          "???"                    // level 59
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_60          "???"                    // level 60
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_61          "???"                    // level 61
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_62          "???"                    // level 62
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_63          "???"                    // level 63
#define DEBUG_TRACE_LEVEL_NAME_LEVEL_64          "???"                    // level 64




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
      void Debug_Init(void);

/* UART debug */
      void Debug_SendDebugString1(u8     trace_level,
                                  u8     trace_type,
                                  ascii *debug_string,
                                  ascii *file_name,
                                  u32    line_number,
                                  ascii *function_name,
                                  u32    delay);
      void Debug_SendDebugString2(u8     trace_level,
                                  u8     trace_type,
                                  ascii *debug_string,
                                  ascii *file_name,
                                  u32    line_number,
                                  ascii *function_name,
                                  u32    delay);

/* info */
const ascii *Debug_TraceLevelsNameGet(u8 level);

/* set/get variables value */
      void Debug_DebugPortSet              (u32 value);
      u32  Debug_DebugPortGet              (void);
      void Debug_TraceLevelsStateSet       (u8 level, bool value);
      bool Debug_TraceLevelsStateGet       (u8 level);
      void Debug_TraceLevelsTypeSet        (u8 level, bool value);
      bool Debug_TraceLevelsTypeGet        (u8 level);
      void Debug_TraceInfoTimeSet          (bool value);
      bool Debug_TraceInfoTimeGet          (void);
      void Debug_TraceInfoTraceLevelNameSet(bool value);
      bool Debug_TraceInfoTraceLevelNameGet(void);
      void Debug_TraceInfoFileNameSet      (bool value);
      bool Debug_TraceInfoFileNameGet      (void);
      void Debug_TraceInfoLineNumberSet    (bool value);
      bool Debug_TraceInfoLineNumberGet    (void);
      void Debug_TraceInfoFunctionNameSet  (bool value);
      bool Debug_TraceInfoFunctionNameGet  (void);
      void Debug_TraceInfoTaskNameSet      (bool value);
      bool Debug_TraceInfoTaskNameGet      (void);




#endif
