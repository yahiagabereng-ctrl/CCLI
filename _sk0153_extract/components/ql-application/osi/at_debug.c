/*=============================================================================
 * File       :  AT_DEBUG.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - AT specific commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* API      includes */
#include "ql_api_osi.h"
#include "ql_api_virt_at.h"
#include "ql_uart.h"

/* user     includes */
#include "alarm_input.h"
#include "alarm_tmax.h"
#include "alarm_tmin.h"
#include "at_debug.h"
#include "boot.h"
#include "charger.h"
#include "clock.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "f_antifrost.h"
#include "f_regulation.h"
#include "forward.h"
#include "logs.h"
#include "parser_rx.h"
#include "password.h"
#include "phone.h"
#include "phonebook.h"
#include "program_flash.h"
#include "queue_my.h"
#include "regulation.h"
#include "report.h"
#include "reset.h"
#include "rtc_alarm.h"
#include "test.h"
#include "typedef.h"
#include "utility.h"

#ifdef FUNCTION_IMPLEMENTED
#include "terminal.h"
#include "boot_flash.h"
#include "dota_flash.h"
#include "dota_main.h"
#include "dota_gprs.h"
#include "dota_ftp.h"
#include "dota_and.h"
#include "main.h"
#endif




/*===========================================================================
 * NOTES
 *===========================================================================*/

/*
 * __________________________________________________________________________
 *
 *  AT+DEBUG:  trace levels management
 * __________________________________________________________________________
 *
 *
 * ---------------------------------------------------------------------------
 *  Action command
 * ---------------------------------------------------------------------------
 *  If <mode> = 0
 *    AT+DEBUG=<mode>,<level_mask>,[<type_mask>]
 *    OK
 *
 *  If <mode> = 1
 *    AT+DEBUG=<mode>,<level_ind>,[<type_ind>]
 *    OK
 *
 *  If <mode> = 2
 *    AT+DEBUG=<mode>,<level>,<state>,[<type>]
 *    OK
 *
 *  If <mode> = 3
 *    AT+DEBUG=<mode>,<level_name>,<state>,[<type>]
 *    OK
 *
 *  If <mode> = 4
 *    AT+DEBUG=<mode>,[<level>]
 *    +DEBUG: <level_mask>,<type_mask>,[<level>,<state>,<type>]
 *    +DEBUG: <level_ind>,<type_ind>
 *    OK
 *
 *  If <mode> = 5
 *    AT+DEBUG=<mode>,<info>,<state>
 *    OK
 *
 *  If <mode> = 6
 *    AT+DEBUG=<mode>,<port>,[<virtual_port>]
 *    OK
 *
 *
 * ---------------------------------------------------------------------------
 *  Read command
 * ---------------------------------------------------------------------------
 *  AT+DEBUG?
 *  +DEBUG: <mode>,<level_mask>,<type_mask>
 *  +DEBUG: <mode>,<level_ind>,<type_ind>
 *  OK
 *
 *
 * ---------------------------------------------------------------------------
 *  Test command
 * ---------------------------------------------------------------------------
 *  AT+DEBUG=?
 *  +DEBUG: (list of the supported <mode>s),(list of the supported <level_ind>s),(list of the supported <type_ind>s),
 *          (list of the supported <level>s),(list of the supported <state>s),(list of the supported <type>s),
 *          (list of the supported <port>s),(list of the supported <virtual_port>s),
 *  0: to set all trace levels in hex     format
 *  1: to set all trace levels in decimal format
 *  2: to set only one trace level (level number is specified)
 *  3: to set only one trace level (level name   is specified)
 *  4: to read the trace levels state
 *  5: set the info to be printed
 *  6: set the debug port
 *  OK
 *
 *
 * ---------------------------------------------------------------------------
 *  Parameters and Defined Values
 * ---------------------------------------------------------------------------
 *  <mode>:           requested operation
 *                    - 0:  set all trace levels in hex     format
 *                    - 1:  set all trace levels in decimal format
 *                    - 2:  set only one trace level (level number is specified)
 *                    - 3:  set only one trace level (level name   is specified)
 *                    - 4:  read the trace levels state
 *                    - 5:  set the info to be printed
 *                    - 6:  set the debug port
 *
 *  <level_mask>:     trace level mask
 *                    32 bits field hexadecimal string (lsb: level 1, msb: level 32) (default value: 0)
 *                      - bit set to 0: level disabled
 *                      - bit set to 1: level enabled
 *
 *  <type_mask>:      type level mask
 *                    32 bits field hexadecimal string (lsb: level 1, msb: level 32) (default value: 0)
 *                      - bit set to 0: low  level
 *                      - bit set to 1: high level
 *
 *  <level_ind>:      trace levels
 *                    32 bits field in decimal format  (lsb: level 1, msb: level 32) (default value: 0)
 *                      - bit set to 0: level disabled
 *                      - bit set to 1: level enabled
 *
 *  <type_ind>:       type levels
 *                    32 bits field in decimal format  (lsb: level 1, msb: level 32) (default value: 0)
 *                      - bit set to 0: low  level
 *                      - bit set to 1: high level
 *
 *  <level>:          trace level number
 *                    range: 1-32
 *
 *  <level_name>:     trace level string name
 *
 *  <state>:          enable state
 *                    - 0:  disabled   (default value)
 *                    - 1:  enabled
 *
 *  <type>:           trace type
 *                    - 0:  low  type (default value)
 *                    - 1:  high type
 *
 *  <info>:           info to be printed
 *                    - 0:  time
 *                    - 1:  trace level name
 *                    - 2:  file name
 *                    - 3:  line number
 *                    - 4:  function name
 *                    - 5:  task name
 *
 *  <port>:           debug port
 *                    - 0:  NONE (all ports)
 *                    - 1:  UART1
 *                    - 2:  UART2
 *                    - 3:  USB
 *                    - 4:  UART1     virtual base
 *                    - 5:  UART2     virtual base
 *                    - 6:  USB       virtual base
 *                    - 7:  BLUETOOTH virtual base
 *                    - 8:  GSM       virtual base
 *                    - 9:  GPRS      virtual base
 *
 *  <virtual_port>:   virtual port
 *                    range: 0-3
 *
 *
 * ---------------------------------------------------------------------------
 *  Examples
 * ---------------------------------------------------------------------------
 *  AT Command and answer                | Description
 * --------------------------------------|--------------------------------------
 *  AT+DEBUG=0,"0000000F"                | enable trace levels 0,1,2,3; all levels with the default type (low type)
 *  OK                                   |
 *  AT+DEBUG=1,15                        |
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=0,"0000000F","00000003"     | enable trace levels 0,1,2,3; levels 0,1 with high type, levels 2,3 with low type
 *  OK                                   |
 *  AT+DEBUG=1,15,3                      |
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG=2,23,0                      | disable trace level 23
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=2,23,1                      | enable  trace level 23 with the default type (low type)
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=2,23,1,0                    | enable  trace level 23 with low  type
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=2,23,1,1                    | enable  trace level 23 with high type
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG=3,"PHONE",0                 | disable trace level "PHONE"
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=3,"PHONE",1                 | enable  trace level "PHONE" with the default type (low type)
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=3,"PHONE",1,0               | enable  trace level "PHONE" with low  type
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=3,"PHONE",1,1               | enable  trace level "PHONE" with high type
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG=4                           | read the trace levels state
 *  +DEBUG: "0000000F","00000003"        |
 *  +DEBUG: 15,3                         |
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=4,23                        | read the trace level 23 state and type
 *  +DEBUG: "0000000F","00000003",23,0,0 |
 *  +DEBUG: 15,3,23,0,0                  |
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG=4,3,0                       | disable the printing of the line number info
 *  OK                                   |
 *                                       |
 *  AT+DEBUG=5,3,1                       | enable  the printing of the line number info
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG=6,1                         | set the debug port to UART1
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG?                            | read the debug settings
 *  +DEBUG: 0,"0000000F","00000003"      |
 *  +DEBUG: 1,15,3                       |
 *  +DEBUG: 4,1,1,0,0,0,0                |
 *  +DEBUG: 5,1                          |
 *  OK                                   |
 *                                       |
 *                                       |
 *  AT+DEBUG=?                           | read the info about the command
 *  +DEBUG: (0-4),(0-4294967295),(0-4294967295),(1-32),(0,1),(0,1),(0-9),(0-3)
 *  0: to set all trace levels in hex     format
 *  1: to set all trace levels in decimal format
 *  2: to set only one trace level (level number is specified)
 *  3: to set only one trace level (level name   is specified)
 *  4: to read the trace levels state    |
 *  5: set the info to be printed        |
 *  6: set the debug port                |
 *  OK                                   |
 */


/*
 * __________________________________________________________________________
 *
 *  AT+GPRS:  GPRS connection
 * __________________________________________________________________________
 *
 *
 * ---------------------------------------------------------------------------
 *  Action command
 * ---------------------------------------------------------------------------
 *    AT+GPRS=<mode>
 *    OK
 *
 *
 * ---------------------------------------------------------------------------
 *  Read command
 * ---------------------------------------------------------------------------
 *  AT+GPRS?
 *  OK
 *
 *
 * ---------------------------------------------------------------------------
 *  Test command
 * ---------------------------------------------------------------------------
 *  AT+GPRS=?
 *  +GPRS: (list of the supported <mode>s)
 *  0: to open  a GPRS connection
 *  1: to close a GPRS connection
 *  OK
 *
 *
 * ---------------------------------------------------------------------------
 *  Parameters and Defined Values
 * ---------------------------------------------------------------------------
 *  <mode>:           requested operation
 *                    - 0:  open  a GPRS connection
 *                    - 1:  close a GPRS connection
 *
 * ---------------------------------------------------------------------------
 *  Examples
 * ---------------------------------------------------------------------------
 *  AT Command and answer                | Description
 * --------------------------------------|--------------------------------------
 *  AT+GPRS=1                            | open  a GPRS connection
 *  OK                                   |
 *                                       |
 *  AT+GPRS=0                            | close a GPRS connection
 *  OK                                   |
 */




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* AT debug status */
/* - 0: AT debug not defined */
/* - 1: AT debug     defined */
//---------------------------------------------------------------------
#define AT_DEBUG_STATUS__BOOT_SET_MODE      0   //  1
//---------------------------------------------------------------------
#define AT_DEBUG_STATUS__DEBUG              1   //  2
#define AT_DEBUG_STATUS__OS_TIME            1   //  3
#define AT_DEBUG_STATUS__RTC_ALARM          1   //  4
#define AT_DEBUG_STATUS__RESET              0   //  5
#define AT_DEBUG_STATUS__SMS                1   //  6
#define AT_DEBUG_STATUS__MYCREG             0   //  7
#define AT_DEBUG_STATUS__MYCSQ              0   //  8
#define AT_DEBUG_STATUS__MYCGMS             0   //  9
#define AT_DEBUG_STATUS__GPI                1   // 10
#define AT_DEBUG_STATUS__GPO                0   // 11
#define AT_DEBUG_STATUS__QUEUE              1   // 12
#define AT_DEBUG_STATUS__LOG                1   // 13
#define AT_DEBUG_STATUS__FLASH              0   // 14
#define AT_DEBUG_STATUS__SETALARMINPUT      0   // 15
#define AT_DEBUG_STATUS__SETINPUT           0   // 16
#define AT_DEBUG_STATUS__SETALARMTMAX       0   // 17
#define AT_DEBUG_STATUS__SETTMAX            0   // 18
#define AT_DEBUG_STATUS__SETALARMTMIN       0   // 19
#define AT_DEBUG_STATUS__SETTMIN            0   // 20
#define AT_DEBUG_STATUS__SETPHONEBOOK       0   // 21
#define AT_DEBUG_STATUS__REGULATION         0   // 22
#define AT_DEBUG_STATUS__F_REGULATION       0   // 23
#define AT_DEBUG_STATUS__F_ANTIFROST        0   // 24
#define AT_DEBUG_STATUS__CHARGER            0   // 25
#define AT_DEBUG_STATUS__FS                 0   // 26
#define AT_DEBUG_STATUS__FILE_LOG           0   // 27
//---------------------------------------------------------------------
#define AT_DEBUG_STATUS__DOTA_SET_APN       0   // 28
#define AT_DEBUG_STATUS__DOTA_SET_DNS       0   // 29
#define AT_DEBUG_STATUS__DOTA_SET_FTP       0   // 30
#define AT_DEBUG_STATUS__DOTA_AUTO          0   // 31
#define AT_DEBUG_STATUS__DOTA_DO_AnD        0   // 32
#define AT_DEBUG_STATUS__DOTA_DO_GPRS       0   // 33
#define AT_DEBUG_STATUS__DOTA_DO_FTP        0   // 34
//---------------------------------------------------------------------

/* AT command types */
#define AT_CMD_TYPE_ACT                     0          /* Action command (without args) (ie. AT+TEST    ) */
#define AT_CMD_TYPE_ARGS                    1          /* Action command (with    args) (ie. AT+TEST=1,0) */
#define AT_CMD_TYPE_READ                    2          /* Read   command                (ie. AT+TEST?   ) */
#define AT_CMD_TYPE_TEST                    3          /* Test   command                (ie. AT+TEST=?  ) */

/* AT command types masks */
#define AT_CMD_TYPE_MASK_ACT                0x01       /* Action command (without args) (ie. AT+TEST    ) */
#define AT_CMD_TYPE_MASK_ARGS               0x02       /* Action command (with    args) (ie. AT+TEST=1,0) */
#define AT_CMD_TYPE_MASK_READ               0x04       /* Read   command                (ie. AT+TEST?   ) */
#define AT_CMD_TYPE_MASK_TEST               0x08       /* Test   command                (ie. AT+TEST=?  ) */

/* maximum string length */
#define MAX_LEN_AT_COMMAND                  170        /* maximum AT command          length */
#define MAX_LEN_AT_ANSWER                   180        /* maximum AT asnwer           length */
#define MAX_LEN_AT_COMMAND_ARG              (160 + 2)  /* maximum AT command argument length */   // +2 is for the 2 possible quote marks (") as parameter delimiter

/* maximum number of arguments for an AT command */
#define MAX_NUM_AT_COMMAND_ARGS             4

/* backspace sequence */
#define BACKSPACE                           '\x08'

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING             300




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
typedef enum
{
//--------------------------------------------------------------
#if (AT_DEBUG_STATUS__BOOT_SET_MODE   == 1)
    AT_DEBUG_STATUS__BOOT_SET_MODE,         //  1
#endif
//--------------------------------------------------------------
#if (AT_DEBUG_STATUS__DEBUG           == 1)
    AT_COMMAND__DEBUG,                      //  2
#endif
#if (AT_DEBUG_STATUS__OS_TIME         == 1)
    AT_COMMAND__OS_TIME,                    //  3
#endif
#if (AT_DEBUG_STATUS__RTC_ALARM       == 1)
    AT_COMMAND__RTC_ALARM,                  //  4
#endif
#if (AT_DEBUG_STATUS__RESET           == 1)
    AT_COMMAND__RESET,                      //  5
#endif
#if (AT_DEBUG_STATUS__SMS             == 1)
    AT_COMMAND__SMS,                        //  6
#endif
#if (AT_DEBUG_STATUS__MYCREG          == 1)
    AT_COMMAND__MYCREG,                     //  7
#endif
#if (AT_DEBUG_STATUS__MYCSQ           == 1)
    AT_COMMAND__MYCSQ,                      //  8
#endif
#if (AT_DEBUG_STATUS__MYCGMS          == 1)
    AT_COMMAND__MYCGMS,                     //  9
#endif
#if (AT_DEBUG_STATUS__GPI             == 1)
    AT_COMMAND__GPI,                        // 10
#endif
#if (AT_DEBUG_STATUS__GPO             == 1)
    AT_COMMAND__GPO,                        // 11
#endif
#if (AT_DEBUG_STATUS__QUEUE           == 1)
    AT_COMMAND__QUEUE,                      // 12
#endif
#if (AT_DEBUG_STATUS__LOG             == 1)
    AT_COMMAND__LOG,                        // 13
#endif
#if (AT_DEBUG_STATUS__FLASH           == 1)
    AT_COMMAND__FLASH,                      // 14
#endif
#if (AT_DEBUG_STATUS__SETALARMINPUT   == 1)
    AT_COMMAND__SETALARMINPUT,              // 15
#endif
#if (AT_DEBUG_STATUS__SETINPUT        == 1)
    AT_COMMAND__SETINPUT,                   // 16
#endif
#if (AT_DEBUG_STATUS__SETALARMTMAX    == 1)
    AT_COMMAND__SETALARMTMAX,               // 17
#endif
#if (AT_DEBUG_STATUS__SETTMAX         == 1)
    AT_COMMAND__SETTMAX,                    // 18
#endif
#if (AT_DEBUG_STATUS__SETALARMTMIN    == 1)
    AT_COMMAND__SETALARMTMIN,               // 19
#endif
#if (AT_DEBUG_STATUS__SETTMIN         == 1)
    AT_COMMAND__SETTMIN,                    // 20
#endif
#if (AT_DEBUG_STATUS__SETPHONEBOOK    == 1)
    AT_COMMAND__SETPHONEBOOK,               // 21
#endif
#if (AT_DEBUG_STATUS__REGULATION      == 1)
    AT_COMMAND__REGULATION,                 // 22
#endif
#if (AT_DEBUG_STATUS__F_REGULATION    == 1)
    AT_COMMAND__F_REGULATION,               // 23
#endif
#if (AT_DEBUG_STATUS__F_ANTIFROST     == 1)
    AT_COMMAND__F_ANTIFROST,                // 24
#endif
#if (AT_DEBUG_STATUS__CHARGER         == 1)
    AT_COMMAND__CHARGER,                    // 25
#endif
#if (AT_DEBUG_STATUS__FS              == 1)
    AT_COMMAND__FS,                         // 26
#endif
#if (AT_DEBUG_STATUS__FILE_LOG        == 1)
    AT_COMMAND__FILE_LOG,                   // 27
#endif
//--------------------------------------------------------------
#if (AT_DEBUG_STATUS__DOTA_SET_APN    == 1)
    AT_COMMAND__DOTA_SET_APN,               // 28
#endif
#if (AT_DEBUG_STATUS__DOTA_SET_DNS    == 1)
    AT_COMMAND__DOTA_SET_DNS,               // 29
#endif
#if (AT_DEBUG_STATUS__DOTA_SET_FTP    == 1)
    AT_COMMAND__DOTA_SET_FTP,               // 30
#endif
#if (AT_DEBUG_STATUS__DOTA_AUTO       == 1)
    AT_COMMAND__DOTA_AUTO,                  // 31
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_AnD     == 1)
    AT_COMMAND__DOTA_DO_AnD,                // 32
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_GPRS    == 1)
    AT_COMMAND__DOTA_DO_GPRS,               // 33
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_FTP     == 1)
    AT_COMMAND__DOTA_DO_FTP,                // 34
#endif
//--------------------------------------------------------------

    // do not move these 2 commands from here (final positions)
    AT_COMMAND__AT,
    AT_COMMAND__UKNOWN,

    NUM_AT_COMMANDS,    // number of defined (enabled) AT commands
} AT_COMMAND_AT;


typedef struct
{
    u16                type;                                                       // AT command type
    u8                 num_of_args;                                                // number of arguments
    ascii              args[MAX_NUM_AT_COMMAND_ARGS][MAX_LEN_AT_COMMAND_ARG + 1];  // list   of arguments
} AT_COMMAND_INFO;


/* callback function for AT command */
typedef void (*AT_COMMAND_HANDLER)(AT_COMMAND_INFO *ptr_at_command_rx);


/* AT command info */
typedef struct
{
    /* AT command name */
    ascii             *string_name;                                                // AT command name
    u8                 string_name_length;                                         // AT command name length

    /* command types allowed */
    u8                 cmd_types;                                                  // command types allowed (Action (with arguments), Action (without arguments), Test, Read)

    /* callback function handler */
    AT_COMMAND_HANDLER cmd_handler;                                                // handler of the callback function associated to the AT command
} AT_COMMAND_DEF;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
       void   AtDebug_Init(void);

/* callback */
static void   AtDebug_VirtAtNotifyCallback(unsigned int ind_type, unsigned int size);
static void   AtDebug_UartNotifyCallback(unsigned int ind_type, ql_uart_port_number_e port, unsigned int size);

/* rx data */
static void   AtDebug_DataRx(u16 len_data, u8 *ptr_data);

/* send string response */
       void   AtDebug_SendString(ascii *string_to_send);

/* info    AT command */
static bool   AtDebug_IsAtCommand(ascii *at_command_string, u8 *ptr_at_cmd_index, AT_COMMAND_INFO *ptr_at_cmd_info);

/* execute AT command */
static void   AtDebug_ExecuteAtCommand(u8 at_cmd_index, AT_COMMAND_INFO *ptr_at_cmd_info);

/* utility */
static ascii *AtDebug_GetArgString(ascii *dst, const ascii *src, u16 position);

/* send final OK/ERRROR answer */
static void   AtDebug_SendFinalOkErrorAnswer(bool is_error);

/* utility */
static void   AtDebug_ExtractArguments(AT_COMMAND_INFO *ptr_at_cmd_info, ascii *ptr_arg_strings[]);


/*-----------------------------------------------------------------------------
 *  AT specific command callback functions
 *-----------------------------------------------------------------------------*/
/* BOOT */
#if (AT_DEBUG_STATUS__BOOT_SET_MODE   == 1)
static void   AtDebug_Callback_AtCommand_BOOT_SET_MODE   (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif

/* PROGRAM */
#if (AT_DEBUG_STATUS__DEBUG           == 1)
static void   AtDebug_Callback_AtCommand_DEBUG           (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__OS_TIME         == 1)
static void   AtDebug_Callback_AtCommand_OS_TIME         (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__RTC_ALARM       == 1)
static void   AtDebug_Callback_AtCommand_RTC_ALARM       (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__RESET           == 1)
static void   AtDebug_Callback_AtCommand_RESET           (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SMS             == 1)
static void   AtDebug_Callback_AtCommand_SMS             (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__MYCREG          == 1)
static void   AtDebug_Callback_AtCommand_MYCREG          (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__MYCSQ           == 1)
static void   AtDebug_Callback_AtCommand_MYCSQ           (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__MYCGMS          == 1)
static void   AtDebug_Callback_AtCommand_MYCGMS          (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__GPI             == 1)
static void   AtDebug_Callback_AtCommand_GPI             (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__GPO             == 1)
static void   AtDebug_Callback_AtCommand_GPO             (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__QUEUE           == 1)
static void   AtDebug_Callback_AtCommand_QUEUE           (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__LOG             == 1)
static void   AtDebug_Callback_AtCommand_LOG             (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__FLASH           == 1)
static void   AtDebug_Callback_AtCommand_FLASH           (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETALARMINPUT   == 1)
static void   AtDebug_Callback_AtCommand_SETALARMINPUT   (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETINPUT        == 1)
static void   AtDebug_Callback_AtCommand_SETINPUT        (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETALARMTMAX    == 1)
static void   AtDebug_Callback_AtCommand_SETALARMTMAX    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETTMAX         == 1)
static void   AtDebug_Callback_AtCommand_SETTMAX         (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETALARMTMIN    == 1)
static void   AtDebug_Callback_AtCommand_SETALARMTMIN    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETTMIN         == 1)
static void   AtDebug_Callback_AtCommand_SETTMIN         (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__SETPHONEBOOK    == 1)
static void   AtDebug_Callback_AtCommand_SETPHONEBOOK    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__REGULATION      == 1)
static void   AtDebug_Callback_AtCommand_REGULATION      (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__F_REGULATION    == 1)
static void   AtDebug_Callback_AtCommand_F_REGULATION    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__F_ANTIFROST     == 1)
static void   AtDebug_Callback_AtCommand_F_ANTIFROST     (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__CHARGER         == 1)
static void   AtDebug_Callback_AtCommand_CHARGER         (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__FS              == 1)
static void   AtDebug_Callback_AtCommand_FS              (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__FILE_LOG        == 1)
static void   AtDebug_Callback_AtCommand_FILE_LOG        (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif

/* DOTA */
#if (AT_DEBUG_STATUS__DOTA_SET_APN    == 1)
static void   AtDebug_Callback_AtCommand_DOTA_SET_APN    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__DOTA_SET_DNS    == 1)
static void   AtDebug_Callback_AtCommand_DOTA_SET_DNS    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__DOTA_SET_FTP    == 1)
static void   AtDebug_Callback_AtCommand_DOTA_SET_FTP    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__DOTA_AUTO       == 1)
static void   AtDebug_Callback_AtCommand_DOTA_AUTO       (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_AnD     == 1)
static void   AtDebug_Callback_AtCommand_DOTA_DO_AnD     (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_GPRS    == 1)
static void   AtDebug_Callback_AtCommand_DOTA_DO_GPRS    (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_FTP     == 1)
static void   AtDebug_Callback_AtCommand_DOTA_DO_FTP     (AT_COMMAND_INFO *ptr_at_cmd_info);
#endif

// do not move these 2 commands from here (final positions)
static void   AtDebug_Callback_AtCommand_AT              (AT_COMMAND_INFO *ptr_at_cmd_info);
static void   AtDebug_Callback_AtCommand_Unknown         (AT_COMMAND_INFO *ptr_at_cmd_info);




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* AT command buffer */
static       ascii          AtDebug_ATCommand[MAX_LEN_AT_COMMAND + 1];

/* AT asnwer buffer */
/* NOTE: It is possible to use the same buffer for AT commands and AT answers */
/*       Pay attention if you change the 2 buffer sizes (MAX_LEN_AT_COMMAND and MAX_LEN_AT_ANSWER) *//*
static       ascii          AtDebug_ATAnswer [MAX_LEN_AT_ANSWER  + 1];*/
//#define                   AtDebug_ATAnswer    AtDebug_ATCommand

/* AT answers */
static const ascii          AtDebug_Answer_OK   [] = "\r\nOK\r\n";
static const ascii          AtDebug_Answer_Error[] = "\r\nERROR\r\n";

/* AT specific commmand table */
static const AT_COMMAND_DEF AtDebug_AtCommandsDef[NUM_AT_COMMANDS] =
{
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
  //  AT command               Callback function handler
  //     name
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------

  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
  //  BOOT
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
#if (AT_DEBUG_STATUS__BOOT_SET_MODE   == 1)
    { "+BOOT_SET_MODE"   ,     (sizeof("+BOOT_SET_MODE"  ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_BOOT_SET_MODE   },  // to set Boot Mode
#endif

  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
  //  PROGRAM
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
    /* DEBUG configuration */
#if (AT_DEBUG_STATUS__DEBUG           == 1)
    { "+DEBUG"           ,     (sizeof("+DEBUG"          ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DEBUG           },  // to set Debug parameters
#endif

    /* reset */
#if (AT_DEBUG_STATUS__OS_TIME         == 1)
    { "+OS_TIME"         ,     (sizeof("+OS_TIME"        ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_OS_TIME         },  // to read the OS time
#endif

    /* RTC alarm */
#if (AT_DEBUG_STATUS__RTC_ALARM       == 1)
    { "+RTC_ALARM"       ,     (sizeof("+RTC_ALARM"      ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_RTC_ALARM       },  // to read the RTC alarms
#endif

    /* reset */
#if (AT_DEBUG_STATUS__RESET           == 1)
    { "+RESET"           ,     (sizeof("+RESET"          ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_RESET           },  // to clear the power-on reset type
#endif

    /* SMS */
#if (AT_DEBUG_STATUS__SMS             == 1)
    { "+SMS"             ,     (sizeof("+SMS"            ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SMS             },  // to send a command usually sent by SMS
#endif

    /* MY GSM */
#if (AT_DEBUG_STATUS__MYCREG          == 1)
    { "+MYCREG"          ,     (sizeof("+MYCREG"         ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_MYCREG          },  // to create my AT+CREG answer
#endif
#if (AT_DEBUG_STATUS__MYCSQ           == 1)
    { "+MYCSQ"           ,     (sizeof("+MYCSQ"          ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_MYCSQ           },  // to create my AT+CSQ  answer
#endif
#if (AT_DEBUG_STATUS__MYCGMS          == 1)
    { "+MYCGMS"          ,     (sizeof("+MYCGMS"         ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_MYCGMS          },  // to create my AT+CGMS answer
#endif

    /* GPIOs */
#if (AT_DEBUG_STATUS__GPI             == 1)
    { "+GPI"             ,     (sizeof("+GPI"            ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_GPI             },  // to read the digital inputs  status
#endif
#if (AT_DEBUG_STATUS__GPO             == 1)
    { "+GPO"             ,     (sizeof("+GPO"            ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_GPO             },  // to read the digital outputs status
#endif

    /* queue/log */
#if (AT_DEBUG_STATUS__QUEUE           == 1)
    { "+QUEUE"           ,     (sizeof("+QUEUE"          ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_QUEUE           },  // to print the tx queue status
#endif
#if (AT_DEBUG_STATUS__LOG             == 1)
    { "+LOG"             ,     (sizeof("+LOG"            ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_LOG             },  // to print the tx log   status
#endif

    /* flash */
#if (AT_DEBUG_STATUS__FLASH           == 1)
    { "+FLASH"           ,     (sizeof("+FLASH"          ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_FLASH           },  // to print the flash objcts info
#endif

    /* configurations */
#if (AT_DEBUG_STATUS__SETALARMINPUT   == 1)
    { "+SETALARMINPUT"   ,     (sizeof("+SETALARMINPUT"  ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETALARMINPUT   },  // to set the alarm input
#endif
#if (AT_DEBUG_STATUS__SETINPUT        == 1)
    { "+SETINPUT"        ,     (sizeof("+SETINPUT"       ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETINPUT        },  // to set the       input
#endif
#if (AT_DEBUG_STATUS__SETALARMTMAX    == 1)
    { "+SETALARMTMAX"    ,     (sizeof("+SETALARMTMAX"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETALARMTMAX    },  // to set the alarm tmax
#endif
#if (AT_DEBUG_STATUS__SETTMAX         == 1)
    { "+SETTMAX"         ,     (sizeof("+SETTMAX"        ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETTMAX         },  // to set the       tmax
#endif
#if (AT_DEBUG_STATUS__SETALARMTMIN    == 1)
    { "+SETALARMTMIN"    ,     (sizeof("+SETALARMTMIN"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETALARMTMIN    },  // to set the alarm tmin
#endif
#if (AT_DEBUG_STATUS__SETTMIN         == 1)
    { "+SETTMIN"         ,     (sizeof("+SETTMIN"        ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETTMIN         },  // to set the       tmin
#endif
#if (AT_DEBUG_STATUS__SETPHONEBOOK    == 1)
    { "+SETPHONEBOOK"    ,     (sizeof("+SETPHONEBOOK"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_SETPHONEBOOK    },  // to set the       tmin
#endif

    /* actions */
#if (AT_DEBUG_STATUS__REGULATION      == 1)
    { "+REGULATION"      ,     (sizeof("+REGULATION"     ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_REGULATION      },  // action on regulation
#endif
#if (AT_DEBUG_STATUS__F_REGULATION    == 1)
    { "+F_REGULATION"    ,     (sizeof("+F_REGULATION"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_F_REGULATION    },  // action on regulation function
#endif
#if (AT_DEBUG_STATUS__F_ANTIFROST     == 1)
    { "+F_ANTIFROST"     ,     (sizeof("+F_ANTIFROST"    ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_F_ANTIFROST     },  // action on antifrost  function
#endif

    /* charger */
#if (AT_DEBUG_STATUS__CHARGER         == 1)
    { "+CHARGER"         ,     (sizeof("+CHARGER"        ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_CHARGER         },  // to set charger parameter
#endif

    /* file system */
#if (AT_DEBUG_STATUS__FS              == 1)
    { "+FS"              ,     (sizeof("+FS"             ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_FS              },  // to manage the file system
#endif

    /* file log */
#if (AT_DEBUG_STATUS__FILE_LOG        == 1)
    { "+FILE_LOG"        ,     (sizeof("+FILE_LOG"       ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_FILE_LOG        },  // to ...
#endif


  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
  //  DOTA
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------
    /* parameters configuration */
#if (AT_DEBUG_STATUS__DOTA_SET_APN    == 1)
    { "+DOTA_SET_APN"    ,     (sizeof("+DOTA_SET_APN"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_SET_APN    },  // to set APN parameters
#endif
#if (AT_DEBUG_STATUS__DOTA_SET_DNS    == 1)
    { "+DOTA_SET_DNS"    ,     (sizeof("+DOTA_SET_DNS"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_SET_DNS    },  // to set DNS parameters
#endif
#if (AT_DEBUG_STATUS__DOTA_SET_FTP    == 1)
    { "+DOTA_SET_FTP"    ,     (sizeof("+DOTA_SET_FTP"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_SET_FTP    },  // to set FTP parameters
#endif

    /* auto mode */
#if (AT_DEBUG_STATUS__DOTA_AUTO       == 1)
    { "+DOTA_AUTO"       ,     (sizeof("+DOTA_AUTO"      ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_AUTO       },  // to enable/disable auto mode
#endif

    /* do DOTA actions */
#if (AT_DEBUG_STATUS__DOTA_DO_AnD     == 1)
    { "+DOTA_DO_AnD"     ,     (sizeof("+DOTA_DO_AnD"    ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_DO_AnD     },  // to do the installation of the Application from A&D space
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_GPRS    == 1)
    { "+DOTA_DO_GPRS"    ,     (sizeof("+DOTA_DO_GPRS"   ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_DO_GPRS    },  // to do a GPRS connection (start/stop)
#endif
#if (AT_DEBUG_STATUS__DOTA_DO_FTP     == 1)
    { "+DOTA_DO_FTP"     ,     (sizeof("+DOTA_DO_FTP"    ) - 1),    (                       AT_CMD_TYPE_MASK_ARGS | AT_CMD_TYPE_MASK_READ |  AT_CMD_TYPE_MASK_TEST),    AtDebug_Callback_AtCommand_DOTA_DO_FTP     },  // to do a FTP  connection (start/stop)
#endif
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // do not move these 2 commands from here (final positions)
    { ""                 ,     (sizeof(""                ) - 1),    (0x00                                                                                         ),    AtDebug_Callback_AtCommand_AT              },  //   - "AT" command
    { ""                 ,     (sizeof(""                ) - 1),    (0x00                                                                                         ),    AtDebug_Callback_AtCommand_Unknown         },  //   - unknown command
  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
};

/* debug string */
static       ascii          AtDebug_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*===========================================================================
 * Function    : AtDebug_Init
 *
 * Description : init
 * Input       : -
 * Output      : -
 *===========================================================================*/
void AtDebug_Init(void)
{
    ql_virt_at_open(QL_VIRT_AT_PORT_0, AtDebug_VirtAtNotifyCallback);

    /* subscribe the AT specific commands */
    ql_uart_open(QL_USB_PORT_AT);
    ql_uart_register_cb(QL_USB_PORT_AT, AtDebug_UartNotifyCallback);
}




/*=============================================================================
 * Function   : AtDebug_VirtAtNotifyCallback
 *
 * Description:
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void AtDebug_VirtAtNotifyCallback(unsigned int ind_type, unsigned int size)
{
    ql_uart_errcode_e    res_uart;
    ql_uart_tx_status_e  tx_status;

    u8                  *ptr_data;
    int                  len_data_read;


    if (QUEC_VIRT_AT_RX_RECV_DATA_IND == ind_type)
    {
        ptr_data = (u8 *)malloc(size);
        if (ptr_data == NULL)
            return;

        memset(ptr_data, 0, size);

        len_data_read = ql_virt_at_read(QL_VIRT_AT_PORT_0, ptr_data, size);
        if (len_data_read != (int)size)
        {
            /* free allocated memory */
            free(ptr_data);
            return;
        }

        /* forward command answer to USB AT port */
        ql_uart_write(QL_USB_PORT_AT, ptr_data, size);

        /* wait for FIFO data transmission to complete */
        while (1)
        {
            res_uart = ql_uart_get_tx_fifo_status(QL_USB_PORT_AT, &tx_status);
            if (res_uart)
                break;

            if (tx_status == QL_UART_TX_COMPLETE)
                break;
        }

        /* free allocated memory */
        free(ptr_data);
    }
}




/*===========================================================================
 * Function   : AtDebug_UartNotifyCallback
 *
 * Description:  
 * Input      : -
 * Output     : -
 *===========================================================================*/
static void AtDebug_UartNotifyCallback(unsigned int ind_type, ql_uart_port_number_e port, unsigned int size)
{
    u8  *ptr_data;

    int  len_data;
    int  len_data_read;


    if (
         (QUEC_UART_RX_OVERFLOW_IND  == ind_type) ||
         (QUEC_UART_RX_RECV_DATA_IND == ind_type)
       )
    {
        if (size == 0)
            return;

        ptr_data = (u8 *)malloc(size);
        if (ptr_data == NULL)
        {
            return;
        }

        memset(ptr_data, 0, size);

        len_data = (int)size;
        while (len_data > 0)
        {
            len_data_read = ql_uart_read(QL_USB_PORT_AT, ptr_data, len_data);
            if ((len_data_read <= 0) || (len_data_read > len_data))
                break;

            len_data -= len_data_read;
        }

        /* pass the received byte to the AT command parser */
        AtDebug_DataRx(size, ptr_data);

        /* free allocated memory */
        free(ptr_data);
    }
}




/*===========================================================================
 * Function   : AtDebug_DataRx
 *
 * Description: Scan the received data form the indicated channel (IR port or
 *              GSM module)
 * Input      : - channel : rx channel
 *                            - 0: IR port
 *                            - 1: GSM module
 *              - len_data:            received data length
 *              - ptr_data: pointer to received data
 * Output     : -
 *===========================================================================*/
static void AtDebug_DataRx(u16 len_data, u8 *ptr_data)
{
    ascii           *rec_string;

    u16              at_last_length;
    u16              len_at_command;

    AT_COMMAND_INFO *ptr_at_cmd_info;
    u8               at_cmd_index;

    bool             result;
    u16              i;


    /* allocate memory for the received string */
    rec_string = (ascii *)malloc(len_data);
    if (rec_string == NULL)
        return;

    /* copy the received string */
    memcpy((void *)rec_string, (void *)ptr_data, (u32)len_data);


    /* calculate the partial AT command length (already received) */
    at_last_length = (u16)strlen(AtDebug_ATCommand);


    /* scan the received string */
    for (i = 0; i < len_data; i++)
    {
        len_at_command = (u16)strlen(AtDebug_ATCommand);

        switch (len_at_command)
        {
            /* len_at_command == 0 */
            case 0:
                /* the string is empty, search for 'a' or 'A' */
                if (
                     (rec_string[i] == 'a') ||
                     (rec_string[i] == 'A')
                   )
                {
                    /* first 'a' or 'A' received */
                    AtDebug_ATCommand[0] = rec_string[i];
                    AtDebug_ATCommand[1] = 0x00;
                }
                else
                {
                    // ignore the character
                }
                break;



            /* len_at_command == 1 */
            case 1:
                /* the string is with only 'a' or 'A', search for 't' or 'T' */
                if (
                     ((AtDebug_ATCommand[0] == 'a') && (rec_string[i] == 't')) ||
                     ((AtDebug_ATCommand[0] == 'A') && (rec_string[i] == 'T'))
                   )
                {
                    /* 't' or 'T' (after 'a' or 'A') received */
                    AtDebug_ATCommand[1] = rec_string[i];
                    AtDebug_ATCommand[2] = 0x00;
                }
                else
                {
                    /* reset the AT command */
                    AtDebug_ATCommand[0] = 0x00;
                    at_last_length          = 0;
                }
                break;



            /* len_at_command >= 2 */
            /* AT mode: echo each char until <CR> */
            default:
                switch (rec_string[i])
                {
                    /* backspace received */
                    case BACKSPACE:
                        /* check if at least one character has been received besides "AT" */
                        if (len_at_command > 2)
                        {
                            /* at least one character has been received besides "AT" */

                            if (len_at_command <= at_last_length)
                            {
                                /* something to erase on remote echo */
                                at_last_length--;
                            }

                            /* erase last character */
                            AtDebug_ATCommand[len_at_command - 1] = 0x00;
                        }
                        break;


                    /* <CR> received */
                    case '\r':
                        /* <CR> received (an AT command has been received) */

                        AtDebug_ATCommand[len_at_command] = 0x00;

                        /* allocate memory for the AT command parameters info */
                        ptr_at_cmd_info = (AT_COMMAND_INFO *)malloc(sizeof(AT_COMMAND_INFO) + len_at_command + 1);
                        if (ptr_at_cmd_info == NULL)
                            return;

                        if (ptr_at_cmd_info)
                        {
                            /* check if the received AT command is a AT specific command */
                            result = AtDebug_IsAtCommand(AtDebug_ATCommand, &at_cmd_index, ptr_at_cmd_info);
                            if (result)
                            {
                                /* the received AT command is     a AT specific command */

                                /* echo command */
                                AtDebug_SendString(AtDebug_ATCommand);

                                /* execute the AT command */
                                AtDebug_ExecuteAtCommand(at_cmd_index, ptr_at_cmd_info);
                            }
                            else
                            {
                                /* the received AT command is not a AT specific command */

                                if (len_at_command == 2)
                                {
                                    /* echo command */
                                    AtDebug_SendString(AtDebug_ATCommand);

                                    /* execute the "AT" command */
                                    AtDebug_ExecuteAtCommand(NUM_AT_COMMANDS - 2, NULL);
                                }
                                else
                                {
                                    AtDebug_ATCommand[len_at_command++] = '\r';
                                    AtDebug_ATCommand[len_at_command  ] = 0x00;

                                    ql_virt_at_write(QL_VIRT_AT_PORT_0, (unsigned char *)AtDebug_ATCommand, len_at_command);
                                }
                            }

                            /* release memory for the AT command parameters info */
                            if (ptr_at_cmd_info)
                                free((void *)ptr_at_cmd_info);
                        }

                        /* reset the AT command */
                        AtDebug_ATCommand[0] = 0x00;
                        at_last_length          = 0;
                        break;


                    /* other character received (no backspace, no <CR> */
                    default:
                        /* add the character to the AT command */
                        if (len_at_command < MAX_LEN_AT_COMMAND)
                        {
                            AtDebug_ATCommand[len_at_command    ] = rec_string[i];
                            AtDebug_ATCommand[len_at_command + 1] = 0x00;
                        }
                        break;
                }
                break;
        }
    }


    /* release memory for the received string */
    if (rec_string)
        free((void *)rec_string);
}




/*=============================================================================
 * Function   : AtDebug_SendString
 *
 * Description: send a string response to a port
 * Input      : - string_to_send: string to be sent
 * Output     : -
 *=============================================================================*/
void AtDebug_SendString(ascii *string_to_send)
{
    ql_uart_errcode_e   res_uart;
    ql_uart_tx_status_e tx_status;

    unsigned int        len_string;


    len_string = strlen(string_to_send);
    if (len_string > 0)
    {
        ql_uart_write(QL_USB_PORT_AT, (u8 *)string_to_send, len_string);

        /* wait for FIFO data transmission to complete */
        while (1)
        {
            res_uart = ql_uart_get_tx_fifo_status(QL_USB_PORT_AT, &tx_status);
            if (res_uart)
                break;

            if (tx_status == QL_UART_TX_COMPLETE)
                break;
        }
    }
}




/*===========================================================================
 * Function    : AtDebug_IsAtCommand
 *
 * Description : verify if an AT command string is a defined AT command.
 *               If it is a defined AT command, build the corresponding AT command info
 * Input       : - at_command_string: AT command string to verify
 *               - ptr_at_cmd_index : pointer to where to save the corresponding AT command index
 *               - ptr_at_cmd_info  : pointer to where to save the corresponding AT command info
 * Output      : - FALSE: it is NOT a defined AT command
 *               - TRUE : it is     a defined AT command
 *===========================================================================*/
static bool AtDebug_IsAtCommand(ascii *at_command_string, u8 *ptr_at_cmd_index, AT_COMMAND_INFO *ptr_at_cmd_info)
{
    u8     at_cmd_length;
    u8     at_cmd_arguments_length;

    ascii  first_char_after_header;
    ascii  second_char_after_header;

    u16    at_cmd_type;
    u8     num_at_cmd_arguments;

    ascii *get_para_result;
    bool   command_ok;
    u8     i;
    u8     j;


    command_ok = FALSE;


    /* calculate the AT command length */
    at_cmd_length = (u8)strlen(at_command_string);


    /* check the minimum AT command length */
    if (at_cmd_length <= 2)
        return FALSE;


    /* convert the initial part of AT command string to upper case */
    for (i = 0; i < at_cmd_length; i++)
    {
        if ((at_command_string[i] == '=') || (at_command_string[i] == '?'))
            break;

        at_command_string[i] = toupper((u8)at_command_string[i]);
    }


    /* check all the defined AT command */
    for (i = 0; i < NUM_AT_COMMANDS; i++)
    {
        /* compare the AT command name (case insensitive) */
        if (!strncmp(at_command_string + 2, AtDebug_AtCommandsDef[i].string_name, AtDebug_AtCommandsDef[i].string_name_length))
        {
            /* AT command name found */

            /* command arguments length (all is after "AT+<command_name>") */
            at_cmd_arguments_length = at_cmd_length - (2 + AtDebug_AtCommandsDef[i].string_name_length);

            /* first character */
            if (at_cmd_arguments_length >= 1)
                first_char_after_header  = *(at_command_string + 2 + AtDebug_AtCommandsDef[i].string_name_length   );
            else
                first_char_after_header  = 0x00;

            /* second character */
            if (at_cmd_arguments_length >= 2)
                second_char_after_header = *(at_command_string + 2 + AtDebug_AtCommandsDef[i].string_name_length + 1);
            else
                second_char_after_header = 0x00;

            num_at_cmd_arguments = 0;

            switch (at_cmd_arguments_length)
            {
                /* == 0 */
                case 0:
                    /* Action (without arguments) command type */
                    if (AtDebug_AtCommandsDef[i].cmd_types & AT_CMD_TYPE_MASK_ACT)
                    {
                        /* Action (without arguments) command type allowed */
                        at_cmd_type = AT_CMD_TYPE_ACT;
                        command_ok = TRUE;
                    }
                    else
                    {
                        /* Action (without arguments) command type not allowed */
                        command_ok = FALSE;
                    }
                    break;


                /* == 1 */
                case 1:
                    if (first_char_after_header == '?')
                    {
                        /* Read command type */
                        if (AtDebug_AtCommandsDef[i].cmd_types & AT_CMD_TYPE_MASK_READ)
                        {
                            /* Read command type allowed */
                            at_cmd_type = AT_CMD_TYPE_READ;
                            command_ok = TRUE;
                        }
                        else
                        {
                            /* Read command type not allowed */
                            command_ok = FALSE;
                        }
                    }
                    else
                    {
                        /* unknown command */
                        command_ok = FALSE;
                    }
                    break;


                /* >= 2 */
                default:
                    if (first_char_after_header == '=')
                    {
                        if (
                            (at_cmd_arguments_length  == 2  ) &&
                            (second_char_after_header == '?')
                           )
                        {
                            /* Test command type */
                            if (AtDebug_AtCommandsDef[i].cmd_types & AT_CMD_TYPE_MASK_TEST)
                            {
                                /* Test command type allowed */
                                at_cmd_type = AT_CMD_TYPE_TEST;
                                command_ok = TRUE;
                            }
                            else
                            {
                                /* Test command type not allowed */
                                command_ok = FALSE;
                            }
                        }
                        else
                        {
                            if (AtDebug_AtCommandsDef[i].cmd_types & AT_CMD_TYPE_MASK_ARGS)
                            {
                                /* Action (with arguments) command type allowed */

                                /* extract the arguments and check the number of the arguments (maximum MAX_NUM_AT_COMMAND_ARGS arguments) */
                                num_at_cmd_arguments = 0;
                                for (j = 1; j <= MAX_NUM_AT_COMMAND_ARGS; j++)
                                {
                                    /* extract the j-th argument string */
                                    get_para_result = AtDebug_GetArgString(ptr_at_cmd_info->args[j - 1], (const ascii *)at_command_string, j);
                                    if (get_para_result != NULL)
                                    {
                                        /* the j-th argument exists */

                                        /* increment the number of arguments found */
                                        num_at_cmd_arguments++;
                                    }
                                    else
                                    {
                                        /* the j-th argument does not exist */

                                        ptr_at_cmd_info->args[j - 1][0] = 0x00;

                                        if (at_command_string[at_cmd_length - 1] == ',')
                                        {
                                            /* the AT command string is terminated by a comma character (',') */
                                            command_ok = FALSE;
                                        }
                                        break;
                                    }
                                }

                                at_cmd_type = AT_CMD_TYPE_ARGS;
                                command_ok = TRUE;
                            }
                            else
                            {
                                /* Action (with arguments) command type not allowed */
                                command_ok = FALSE;
                            }
                        }
                    }
                    else
                    {
                        /* unknown command */
                        command_ok = FALSE;
                    }
                    break;
            }

            /* exit from "for" (if command found) */
            if (command_ok)
                break;
        }
    }


    /* parse the AT command (if it is ok) */
    if (command_ok)
    {
        /* save the AT command index found */
        *ptr_at_cmd_index = i;

        /* save the AT command parameters info */
        if (ptr_at_cmd_info)
        {
            ptr_at_cmd_info->type        = at_cmd_type;             // type
            ptr_at_cmd_info->num_of_args = num_at_cmd_arguments;    // number of arguments
        }
    }


    if (command_ok)
        return TRUE;
    else
        return FALSE;
}




/*=============================================================================
 * Function   : AtDebug_ExecuteAtCommand
 *
 * Description: execute a defined AT command
 * Input      : - at_cmd_index   : defined AT command index   to be executed
 *              - ptr_at_cmd_info: pointer to AT command info to be executed
 * Output     : -
 *=============================================================================*/
static void AtDebug_ExecuteAtCommand(u8 at_cmd_index, AT_COMMAND_INFO *ptr_at_cmd_info)
{
    if (at_cmd_index >= NUM_AT_COMMANDS)
        return;

    /* call the callback function associated to the command */
    AtDebug_AtCommandsDef[at_cmd_index].cmd_handler(ptr_at_cmd_info);
}




/*=============================================================================
 * Function   : AtDebug_GetArgString
 *
 * Description: If src is a string formatted as an AT response (for example "+RESP:P1,P2,P3") or
 *              as an AT command (for example "AT+CMD=P1,P2,P3"), the function copies the
 *              parameter at position offset (starting from 1) if it is present in the src buffer, and
 *              returns a pointer on dst. It returns NULL otherwise.
 *              if the parameter is a quoted string (for example "AT+CMD=P1,"P2",P3"),
 *              the 2 quotes are removed.
 * Input      : - dst     : destination string
 *              - src     : source      string
 *              - position: position of the parameter to be found (starting from 1)
 * Output     : - pointer to dst string if the parameter is     found
 *              - NULL                  if the parameter is not found
 *=============================================================================*/
static ascii *AtDebug_GetArgString(ascii *dst, const ascii *src, u16 position)
{
    u16    len_src;
    u16    len_src_temp;

    ascii *token;
    ascii *remainder;
    u16    len_token;
    u8     num_arg;
    u8     i;


    /* calculate the length of src string */
    len_src = (u16)strlen(src);
    if (len_src > MAX_LEN_AT_COMMAND)
        return NULL;

    len_token = 0;

    num_arg = 0;
    len_src_temp = 0;

    token = strtok_r((ascii *)src, "=:", &remainder);
    if (token != NULL)
    {
        len_token     = strlen(token);
        len_src_temp += len_token;
        if (len_src_temp < len_src)
        {
            token[len_token] = '=';   // put the delimiter removed by strtok (put '=' in both cases)
            len_src_temp++;
        }
    }

    if (   (                         (sizeof("AT+SMS") - 1)  == len_token)     &&
           (strncmp(token, "AT+SMS", (sizeof("AT+SMS") - 1)) == 0        )   )
    {
        token = strchr(src, '=');
        if (token != NULL)
        {
            token++;
            len_token = strlen(token);

            if (position == 1)
            {
                if (len_token <= MAX_LEN_AT_COMMAND_ARG)
                {
                    if (
                         (token[0            ] == '"') &&
                         (token[len_token - 1] == '"')
                       )
                    {
                        /* quoted string */
                        for (i = 0; i < (len_token - 2); i++)
                            dst[i] = token[i + 1];
                        dst[len_token - 2] = 0x00;

                        return dst;
                    }
                    else
                    {
                        /* no quoted string */
                        return NULL;
                    }
                }
                else
                {
                    return NULL;
                }
            }
        }
    }
    else
    {
        while (token != NULL)
        {
            token = strtok_r(NULL, ",", &remainder);
            if (token != NULL)
            {
                len_token     = strlen(token);
                len_src_temp += len_token;
                if (len_src_temp < len_src)
                {
                    token[len_token] = ',';   // put the delimiter removed by strtok
                    len_src_temp++;
                }

                num_arg++;
                if (num_arg == position)
                {
                    if (len_token <= MAX_LEN_AT_COMMAND_ARG)
                    {
                        if (
                             (token[0            ] == '"') &&
                             (token[len_token - 1] == '"')
                           )
                        {
                            /* quoted string */
                            for (i = 0; i < (len_token - 2); i++)
                                dst[i] = token[i + 1];
                            dst[len_token - 2] = 0x00;
                        }
                        else
                        {
                            /* no quoted string */
                            for (i = 0; i < len_token; i++)
                                dst[i] = token[i];
                            dst[len_token] = 0x00;
                        }

                        return dst;
                    }
                    else
                    {
                        return NULL;
                    }
                }
            }
        }
    }


    return NULL;
}




/*=============================================================================
 * Function   : AtDebug_SendFinalOkErrorAnswer
 *
 * Description: send final "OK" / "ERROR" answer
 * Input      : - is_error: error indication
 * Output     : -
 *=============================================================================*/
static void AtDebug_SendFinalOkErrorAnswer(bool is_error)
{
    if (!is_error)
    {
        // "OK"
        AtDebug_SendString((ascii *)AtDebug_Answer_OK   );
    }
    else
    {
        // "ERROR"
        AtDebug_SendString((ascii *)AtDebug_Answer_Error);
    }
}




/*=============================================================================
 * Function   : AtDebug_ExtractArguments
 *
 * Description: extract the arguments strings
 *              - ptr_at_cmd_info: pointer to where the corresponding AT command info are saved
 *              - ptr_arg_strings: pointers array to save the extracted arguments strings
 * Output     : -
 *=============================================================================*/
static void AtDebug_ExtractArguments(AT_COMMAND_INFO *ptr_at_cmd_info, ascii *ptr_arg_strings[])
{
    u8 i;


    /* extract the arguments */
    for (i = 0; i < ptr_at_cmd_info->num_of_args; i++)
        ptr_arg_strings[i] = ptr_at_cmd_info->args[i];
}




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_BOOT_SET_MODE
 *
 * Description: callback function for AT+BOOT_SET_MODE command
 * Input      : - ptr_at_cmd_info:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__BOOT_SET_MODE == 1)
static void AtDebug_Callback_AtCommand_BOOT_SET_MODE(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii           *ptr_arg_string[1];

    ascii            answer_string[30 + 1];

    BOOT__BOOT_MODE  boot_mode;

    bool             is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+BOOT_SET_MODE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <boot_mode> parameter */
            boot_mode = atoi(ptr_arg_string[0]);

            if (
                 (boot_mode != 0) &&
                 (boot_mode != 1) &&
                 (boot_mode != 2) &&
                 (boot_mode != 3)
               )
            {
                is_error = TRUE;
                break;
            }

            /* configure the Boot mode */
            Boot_BootMode_Set(&boot_mode);

            /* request the backup to flash objects */
            BootFlash_RequestWriteBackup(BOOT_FLASH__HANDLE_INDEX__000_BOOT_MODE, BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE);
            break;


        // Read command (AT+BOOT_SET_MODE?)
        case AT_CMD_TYPE_READ:
            /* get the Boot mode */
            Boot_BootMode_Get(&boot_mode);

            snprintf(answer_string, sizeof(answer_string), "\r\n+BOOT_SET_MODE: %d\r\n", boot_mode);
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+BOOT_SET_MODE=?)
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DEBUG
 *
 * Description: callback function for AT+DEBUG command
 * Input      : - ptr_at_cmd_info:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DEBUG == 1)
static void AtDebug_Callback_AtCommand_DEBUG(AT_COMMAND_INFO *ptr_at_cmd_info)
{
          ascii *ptr_arg_string[4];

          ascii  answer_string[61 + 1];

          u8     mode;

          u32    level_mask;
          u32    type_mask;

          u32    level_ind;
          u32    type_ind;

          u32    level;
          u32    state;
          u32    type;
          u32    info;
          u32    port;

    const ascii *level_name;
    const ascii *level_name_ith;

          u32    debug_port;

          u8     state_0;
          u8     state_1;
          u8     state_2;
          u8     state_3;
          u8     state_4;
          u8     state_5;

          bool   is_error;
          u8     i;


    /* debug info */
  //Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DEBUG", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if ((ptr_at_cmd_info->num_of_args == 0) || (ptr_at_cmd_info->num_of_args > 4))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <mode> parameter */
            mode = atoi(ptr_arg_string[0]);

            switch (mode)
            {
                // <mode> = 0
                // AT+DEBUG=<mode>,<level_mask>,[<type_mask>]
                case 0:
                    /* check the number of the parameters */
                    if ((ptr_at_cmd_info->num_of_args != 2) && (ptr_at_cmd_info->num_of_args != 3))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <level_mask> parameter */
                    if (!Utility_IsHexaString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    if (strlen(ptr_arg_string[1]) != 8)
                    {
                        is_error = TRUE;
                        break;
                    }
                    level_mask = Utility_Hexatoi32(ptr_arg_string[1]);

                    /* <type_mask> parameter (optional) */
                    if (ptr_at_cmd_info->num_of_args == 3)
                    {
                        if (!Utility_IsHexaString(ptr_arg_string[2]))
                        {
                            is_error = TRUE;
                            break;
                        }
                        if (strlen(ptr_arg_string[2]) != 8)
                        {
                            is_error = TRUE;
                            break;
                        }
                        type_mask = Utility_Hexatoi32(ptr_arg_string[2]);
                    }
                    else
                    {
                        type_mask = 0x00000000;
                    }

                    /* save new settings */
                    for (i = 0; i < DEBUG_TRACE_LEVELS_NUMBER; i++)
                    {
                        Debug_TraceLevelsStateSet(i + 1, (bool)((level_mask >> i) & 0x00000001));
                        Debug_TraceLevelsTypeSet (i + 1, (bool)((type_mask  >> i) & 0x00000001));
                    }
                    break;


                // <mode> = 1
                // AT+DEBUG=<mode>,<level_ind>,[<type_ind>]
                case 1:
                    /* check the number of the parameters */
                    if ((ptr_at_cmd_info->num_of_args != 2) && (ptr_at_cmd_info->num_of_args != 3))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <level_ind> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    level_ind = (u32)atoi(ptr_arg_string[1]);

                    /* <type_ind> parameter (optional) */
                    if (ptr_at_cmd_info->num_of_args == 3)
                    {
                        if (!Utility_IsNumString(ptr_arg_string[2]))
                        {
                            is_error = TRUE;
                            break;
                        }
                        type_ind = (u32)atoi(ptr_arg_string[2]);
                    }
                    else
                    {
                        type_ind = 0;
                    }

                    /* save new settings */
                    for (i = 0; i < DEBUG_TRACE_LEVELS_NUMBER; i++)
                    {
                        Debug_TraceLevelsStateSet(i + 1, (bool)((level_ind >> i) & 0x00000001));
                        Debug_TraceLevelsTypeSet (i + 1, (bool)((type_ind  >> i) & 0x00000001));
                    }
                    break;


                // <mode> = 2
                // AT+DEBUG=<mode>,<level>,<state>,[<type>]
                case 2:
                    /* check the number of the parameters */
                    if ((ptr_at_cmd_info->num_of_args != 3) && (ptr_at_cmd_info->num_of_args != 4))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <level> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    level = (u32)atoi(ptr_arg_string[1]);
                    if ((level == 0) || (level > 32))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <state> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[2]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    state = (u32)atoi(ptr_arg_string[2]);
                    if ((state != 0) && (state != 1))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <type> parameter (optional) */
                    if (ptr_at_cmd_info->num_of_args == 4)
                    {
                        if (!Utility_IsNumString(ptr_arg_string[3]))
                        {
                            is_error = TRUE;
                            break;
                        }
                        type = (u32)atoi(ptr_arg_string[3]);
                        if ((type != 0) && (type != 1))
                        {
                            is_error = TRUE;
                            break;
                        }
                    }
                    else
                    {
                        type = 0;
                    }

                    /* save new settings */
                    Debug_TraceLevelsStateSet(level, (bool)state);
                    Debug_TraceLevelsTypeSet (level, (bool)type );
                    break;


                // <mode> = 3
                // AT+DEBUG=<mode>,<level_name>,<state>,[<type>]
                case 3:
                    /* check the number of the parameters */
                    if ((ptr_at_cmd_info->num_of_args != 3) && (ptr_at_cmd_info->num_of_args != 4))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <level_name> parameter */
                    level_name = ptr_arg_string[1];
                    if (strlen(level_name) == 0)
                    {
                        is_error = TRUE;
                        break;
                    }
                    for (i = 0; i < DEBUG_TRACE_LEVELS_NUMBER; i++)
                    {
                        level_name_ith = Debug_TraceLevelsNameGet(i + 1);
                        if (!strcmp(level_name_ith, level_name))
                            break;
                    }
                    if (i >= DEBUG_TRACE_LEVELS_NUMBER)
                    {
                        is_error = TRUE;
                        break;
                    }
                    level = i + 1;

                    /* <state> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[2]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    state = (u32)atoi(ptr_arg_string[2]);
                    if ((state != 0) && (state != 1))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <type> parameter (optional) */
                    if (ptr_at_cmd_info->num_of_args == 4)
                    {
                        if (!Utility_IsNumString(ptr_arg_string[3]))
                        {
                            is_error = TRUE;
                            break;
                        }
                        type = (u32)atoi(ptr_arg_string[3]);
                        if ((type != 0) && (type != 1))
                        {
                            is_error = TRUE;
                            break;
                        }
                    }
                    else
                    {
                        type = 0;
                    }

                    /* save new settings */
                    Debug_TraceLevelsStateSet(level, (bool)state);
                    Debug_TraceLevelsTypeSet (level, (bool)type );
                    break;


                // <mode> = 4
                // AT+DEBUG=<mode>,[<level>]
                case 4:
                    /* check the number of the parameters */
                    if ((ptr_at_cmd_info->num_of_args != 1) && (ptr_at_cmd_info->num_of_args != 2))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <level> parameter (optional) */
                    if (ptr_at_cmd_info->num_of_args == 2)
                    {
                        if (!Utility_IsNumString(ptr_arg_string[1]))
                        {
                            is_error = TRUE;
                            break;
                        }
                        level = (u32)atoi(ptr_arg_string[1]);
                        if ((level == 0) || (level > 32))
                        {
                            is_error = TRUE;
                            break;
                        }
                    }
                    else
                    {
                        level = 0;
                    }

                    // +DEBUG: <level_mask>,<type_mask>,[<level>,<state>,<type>]
                    level_mask = 0x00000000;
                    type_mask  = 0x00000000;
                    for (i = 0; i < DEBUG_TRACE_LEVELS_NUMBER; i++)
                    {
                        if (Debug_TraceLevelsStateGet(i + 1))
                            level_mask |= (1 << i);
                        if (Debug_TraceLevelsTypeGet (i + 1))
                            type_mask  |= (1 << i);
                    }
                    if (level == 0)
                    {
                        /* no level specified */
                        snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: \"%08lx\",\"%08lx\"\r\n", level_mask, type_mask);
                    }
                    else
                    {
                        /* level is specified */
                        if (Debug_TraceLevelsStateGet(level))
                            state = 1;
                        else
                            state = 0;
                        if (Debug_TraceLevelsTypeGet (level))
                            type  = 1;
                        else
                            type  = 0;
                        snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: \"%08lx\",\"%08lx\",%ld,%ld,%ld\r\n", level_mask, type_mask, level, state, type);
                    }
                    AtDebug_SendString(answer_string);

                    // +DEBUG: <level_ind>,<type_ind>
                    level_ind = level_mask;
                    type_ind  = type_mask;
                    if (level == 0)
                    {
                        /* no level specified */
                        snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: %ld,%ld\r\n", level_ind , type_ind);
                    }
                    else
                    {
                        /* level is specified */
                        if (Debug_TraceLevelsStateGet(level))
                            state = 1;
                        else
                            state = 0;
                        if (Debug_TraceLevelsTypeGet (level))
                            type  = 1;
                        else
                            type  = 0;
                        snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: %ld,%ld,%ld,%ld,%ld\r\n", level_ind , type_ind, level, state, type);
                    }
                    AtDebug_SendString(answer_string);
                    break;


                // <mode> = 5
                // AT+DEBUG=<mode>,<info>,<state>
                case 5:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 3)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <info> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    info = (u32)atoi(ptr_arg_string[1]);
                    if (info > 5)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <state> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[2]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    state = (u32)atoi(ptr_arg_string[2]);
                    if ((state != 0) && (state != 1))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* save new settings */
                    switch (info)
                    {
                        case 0:
                            Debug_TraceInfoTimeSet          ((bool)state);
                            break;

                        case 1:
                            Debug_TraceInfoTraceLevelNameSet((bool)state);
                            break;

                        case 2:
                            Debug_TraceInfoFileNameSet      ((bool)state);
                            break;

                        case 3:
                            Debug_TraceInfoLineNumberSet    ((bool)state);
                            break;

                        case 4:
                            Debug_TraceInfoFunctionNameSet  ((bool)state);
                            break;

                        case 5:
                            Debug_TraceInfoTaskNameSet      ((bool)state);
                            break;
                    }
                    break;


                // <mode> = 6
                // AT+DEBUG=<mode>,<port>
                case 6:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 2)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <port> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    port = (u32)atoi(ptr_arg_string[1]);
                    if (port > 4)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* save new settings */
                    switch (port)
                    {
                        case 0:
                        default:
                            debug_port = DEBUG__DEBUG_PORT_NONE;
                            break;

                        case 1:
                            debug_port = DEBUG__DEBUG_PORT_USB_AT;
                            break;

                        case 2:
                            debug_port = DEBUG__DEBUG_PORT_USB_AP_LOG;
                            break;
                    }
                    Debug_DebugPortSet(debug_port);
                    break;


                // <mode> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+DEBUG?)
        case AT_CMD_TYPE_READ:
            // +DEBUG: <mode>,<level_mask>,<type_mask>
            level_mask = 0x00000000;
            type_mask  = 0x00000000;
            for (i = 0; i < DEBUG_TRACE_LEVELS_NUMBER; i++)
            {
                if (Debug_TraceLevelsStateGet(i + 1))
                    level_mask |= (1 << i);
                if (Debug_TraceLevelsTypeGet (i + 1))
                    type_mask  |= (1 << i);
            }
            snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: 0,\"%08lx\",\"%08lx\"\r\n", level_mask, type_mask);
            AtDebug_SendString(answer_string);


            // +DEBUG: <mode>,<level_ind>,<type_ind>
            level_ind = level_mask;
            type_ind  = type_mask;
            snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: 1,%ld,%ld\r\n", level_ind , type_ind );
            AtDebug_SendString(answer_string);


            // +DEBUG: <mode>,<state_0>,<state_1>,<state_2>,<state_3>,<state_4>,<state_5>
            if (Debug_TraceInfoTimeGet          ())
                state_0 = 1;
            else
                state_0 = 0;
            if (Debug_TraceInfoTraceLevelNameGet())
                state_1 = 1;
            else
                state_1 = 0;
            if (Debug_TraceInfoFileNameGet      ())
                state_2 = 1;
            else
                state_2 = 0;
            if (Debug_TraceInfoLineNumberGet    ())
                state_3 = 1;
            else
                state_3 = 0;
            if (Debug_TraceInfoFunctionNameGet  ())
                state_4 = 1;
            else
                state_4 = 0;
            if (Debug_TraceInfoTaskNameGet      ())
                state_5 = 1;
            else
                state_5 = 0;
            snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: 4,%d,%d,%d,%d,%d,%d\r\n", state_0, state_1, state_2, state_3, state_4, state_5);
            AtDebug_SendString(answer_string);


            // +DEBUG: <mode>,<port>
            debug_port = Debug_DebugPortGet();
            switch (debug_port)
            {
                case DEBUG__DEBUG_PORT_NONE:
                    port = 0;
                    break;

                case DEBUG__DEBUG_PORT_USB_AT:
                    port = 1;
                    break;

                case DEBUG__DEBUG_PORT_USB_AP_LOG:
                    port = 2;
                    break;

                default:
                    port = 255;
                    break;
            }
            snprintf(answer_string, sizeof(answer_string), "\r\n+DEBUG: 6,%ld\r\n", port);
            AtDebug_SendString(answer_string);
            break;



        // Test command (AT+DEBUG=?)
        case AT_CMD_TYPE_TEST:
            // +DEBUG: (list of the supported <mode>s),(list of the supported <level_ind>s),(list of the supported <type_ind>s),
            //         (list of the supported <level>s),(list of the supported <state>s),(list of the supported <type>s),
            //         (list of the supported <port>s),(list of the supported <virtual_port>s),
          //adl_atSendResponse(ADL_AT_PORT_TYPE(ptr_params->Port, ADL_AT_INT), "\r\n+DEBUG: (0-4),(0-4294967295),(0-4294967295),(1-32),(0,1),(0,1),(0-9),(0-3)\r\n");
            AtDebug_SendString("\r\n+DEBUG: (0-4),(0-4294967295),(0-4294967295),(1-32),(0,1),(0,1),(0-9),(0-3)\r\n");

            // 0: to set all trace levels in hex     format
            // 1: to set all trace levels in decimal format
            // 2: to set only one trace level (level number is specified)
            // 3: to set only one trace level (level name   is specified)
            // 4: to read the trace levels state
            // 5: set the info to be printed
            // 6: set the debug port
            AtDebug_SendString("\r\n0: to set all trace levels in hex     format\r\n"              );
            AtDebug_SendString("\r\n1: to set all trace levels in decimal format\r\n"              );
            AtDebug_SendString("\r\n2: to set only one trace level (level number is specified)\r\n");
            AtDebug_SendString("\r\n3: to set only one trace level (level name   is specified)\r\n");
            AtDebug_SendString("\r\n4: to read the trace levels state\r\n"                         );
            AtDebug_SendString("\r\n5: set the info to be printed\r\n"                             );
            AtDebug_SendString("\r\n6: set the debug port"                                         );
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_OS_TIME
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__OS_TIME == 1)
static void AtDebug_Callback_AtCommand_OS_TIME(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii answer_string[30 + 1];

    s64   os_time;

    bool  is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+OS_TIME", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            is_error = TRUE;
            break;


        // Read command (AT+OS_TIME?)
        case AT_CMD_TYPE_READ:
            /* get OS time (ms) */
            os_time = ql_rtos_up_time_ms();

            snprintf(answer_string, sizeof(answer_string), "\r\n+OS_TIME: %lld\r\n", os_time);
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+OS_TIME=?)
        case AT_CMD_TYPE_TEST:
            is_error = TRUE;
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_RTC_ALARM
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__RTC_ALARM == 1)
static void AtDebug_Callback_AtCommand_RTC_ALARM(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                 answer_string[90 + 1];

    RTC_ALARM__RTC_ALARM  rtc_alarm;

    ascii                *rtc_alarm__description;

    bool                  is_error;
    bool                  result;
    u8                    i;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+RTC_ALARM", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            is_error = TRUE;
            break;


        // Read command (AT+RTC_ALARM?)
        case AT_CMD_TYPE_READ:
            /* get RTC alarms */
            for (i = 0; i < RTC_ALARM__MAX_NUM_OF_RTC_ALARMS; i++)
            {
                /* get 'i' index RTC alarm info */
                result = RtcAlarm_GetRtcAlarms(i, &rtc_alarm);

                /* get 'i' index RTC alarm description */
                rtc_alarm__description = RtcAlarm_ReadRtcAlarmDescription(i);

                if (rtc_alarm.status)
                {
                    /* 'i' index RTC alarm enabled */
                    snprintf(answer_string,
                             sizeof(answer_string),

                             "\r\n+RTC_ALARM: %d,\"%s\",%d,\"%.3s %02d/%02d/%04d %02d:%02d:%02d\"\r\n",

                             i + 1,
                             rtc_alarm__description,
                             rtc_alarm.status,

                             Clock_WeekDayString(Clock_WeekDayOfADay(rtc_alarm.rtc_alarm_time.day, rtc_alarm.rtc_alarm_time.month, rtc_alarm.rtc_alarm_time.year)),
                             rtc_alarm.rtc_alarm_time.day,
                             rtc_alarm.rtc_alarm_time.month,
                             rtc_alarm.rtc_alarm_time.year,
                             rtc_alarm.rtc_alarm_time.hour,
                             rtc_alarm.rtc_alarm_time.minute,
                             rtc_alarm.rtc_alarm_time.second);
                }
                else
                {
                    /* 'i' index RTC alarm disabled */
                    snprintf(answer_string,
                             sizeof(answer_string),

                             "\r\n+RTC_ALARM: %d,\"%s\",%d\r\n",

                             i + 1,
                             rtc_alarm__description,
                             rtc_alarm.status);
                }

                AtDebug_SendString(answer_string);
            }


            /* get nearest RTC alarm */
            RtcAlarm_GetNearestRtcAlarm(&rtc_alarm);

            /* get nearest RTC alarm description */
            rtc_alarm__description = RtcAlarm_ReadRtcAlarmDescription(RTC_ALARM__ALARM_NEAREST);

            if (rtc_alarm.status)
            {
                /* nearest RTC alarm enabled */
                snprintf(answer_string,
                         sizeof(answer_string),

                         "\r\n+RTC_ALARM: 0,\"%s\",%d,\"%.3s %02d/%02d/%04d %02d:%02d:%02d\"\r\n",

                         rtc_alarm__description,
                         rtc_alarm.status,

                         Clock_WeekDayString(Clock_WeekDayOfADay(rtc_alarm.rtc_alarm_time.day, rtc_alarm.rtc_alarm_time.month, rtc_alarm.rtc_alarm_time.year)),
                         rtc_alarm.rtc_alarm_time.day,
                         rtc_alarm.rtc_alarm_time.month,
                         rtc_alarm.rtc_alarm_time.year,
                         rtc_alarm.rtc_alarm_time.hour,
                         rtc_alarm.rtc_alarm_time.minute,
                         rtc_alarm.rtc_alarm_time.second);
            }
            else
            {
                /* nearest RTC alarm disabled */
                snprintf(answer_string,
                         sizeof(answer_string),

                         "\r\n+RTC_ALARM: 0,%d\r\n",

                         rtc_alarm.status);
            }

            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+RTC_ALARM=?)
        case AT_CMD_TYPE_TEST:
            is_error = TRUE;
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_RESET
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__RESET == 1)
static void AtDebug_Callback_AtCommand_RESET(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                      *ptr_arg_string[1];

    u8                          action;

  //RESET__POWER_ON_RESET_TYPE  poweron_reset_type;

    bool                        is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+RESET", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            action = atoi(ptr_arg_string[0]);

            /* verify the <action> parameter */
            if (action != 0)
            {
                is_error = TRUE;
                break;
            }

            /* reset for watchdog */
            Main_ResetModule();

            /* clear the power-on reset type */
          //poweron_reset_type = RESET__POWERON_RESET_TYPE__UNKNOWN;
          //Reset_PowerOnResetType_Set(&poweron_reset_type);

            /* request the backup to flash objects */
          //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_RESET, PROGRAM_FLASH__FLASH_ID__000_RESET__POWERON_RESET_TYPE);
            break;



        // Read command (AT+RESET?)
        // Test command (AT+RESET=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SMS
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SMS == 1)
static void AtDebug_Callback_AtCommand_SMS(AT_COMMAND_INFO *ptr_at_cmd_info)
{
           ascii                    *ptr_arg_string[1];

    static ascii                     command_string                                [160      + 1];

    static ascii                     answer_string                                 [160 + 12 + 1];
    static ascii                    *ptr_parser_strings[PARSER_RX__NUM_MAX_ANSWERS];
    static ascii                     parser_strings    [PARSER_RX__NUM_MAX_ANSWERS][160      + 1];

           u8                        len_command;
           ascii                     character;
           bool                      in_escape;

           u8                        num_answers;

    static LOGS__RX_COMMAND_RECORD   rx_command_record;

           REPORT__CONFIG__REPORT    config_report;

           CLOCK__TIME               command_time_tx;
           CLOCK__TIME               current_time;
           CLOCK__TIME               time;

    const  ascii                    *ptr_command;

           bool                      report;
           bool                      command_valid;
           bool                      answer;

           bool                      parser_result;
           bool                      is_error;
           bool                      result;
           u8                        i;
           u8                        j;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SMS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <sms_cmd_text> parameter */
            ptr_command = ptr_arg_string[0];

            /* manage possible escape characters */
            len_command = (u8)strlen(ptr_command);
            in_escape = FALSE;
            j   = 0;

            for (i = 0; i < len_command; i++)
            {
                character = *(ptr_command + i);

                if (!in_escape)
                {
                    if (character != '^')
                        command_string[j++] = character;
                    else
                        in_escape = TRUE;
                }
                else
                {
                    if      (character == '^')
                        command_string[j++] = character;
                    else if (character == '$')
                        command_string[j++] = '"';
                    else
                        command_string[j++] = character;
                    in_escape = FALSE;
                }
            }
            command_string[j] = 0x00;

            snprintf(AtDebug_DebugString, sizeof(AtDebug_DebugString), "command_string: \"%s\"", command_string);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, AtDebug_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* verify the max length of the command string received form UART */
            if (strlen(command_string) > 160)
            {
                is_error = TRUE;
                break;
            }

            /* get the actual time (command time) */
            result = Clock_GetTime(&command_time_tx);

            /* parser of the command received from UART */
            for (i = 0; i < PARSER_RX__NUM_MAX_ANSWERS; i++)
            {
                parser_strings    [i][0] = 0x00;
                ptr_parser_strings[i]    = &parser_strings[i][0];
            }
            parser_result = ParserRx_ParseCommandText(FALSE, &command_time_tx, "", TRUE, FALSE, command_string, &report, &command_valid, &num_answers, ptr_parser_strings, 160);

            if (num_answers > PARSER_RX__NUM_MAX_ANSWERS)
                num_answers = PARSER_RX__NUM_MAX_ANSWERS;;

            /* send answer string to UART */
            if (parser_result)
            {
                /* parser OK */

                /*------------------------------------------------------------------
                 * Verify if the answer has to be transmitted
                 *------------------------------------------------------------------*/
                if (report)
                {
                    /* get the report configuration */
                    Report_Config_Report_Get(&config_report);
                    if (config_report.status)
                    {
                        /* report enabled */
                        answer = TRUE;
                    }
                    else
                    {
                        /* report disabled */
                        answer = FALSE;
                    }
                }
                else
                {
                    answer = TRUE;
                }

                if (answer)
                {
                    for (i = 0; i < num_answers; i++)
                    {
                        strncpy(parser_strings[i], ptr_parser_strings[i], 160);
                        parser_strings[i][160] = 0x00;
                        if (strlen(parser_strings[i]) > 0)
                        {
                            snprintf(answer_string, sizeof(answer_string), "\r\n+SMS: %d,\"%s\"\r\n", i + 1, parser_strings[i]);
                        }
                        else
                        {
                            snprintf(answer_string, sizeof(answer_string), "\r\n+SMS: %d,\"\"\r\n"  , i + 1                   );
                        }
                        AtDebug_SendString(answer_string);
                    }
                }
                else
                {
                    AtDebug_SendString("\r\n+SMS: \"\"\r\n");
                }


                /*------------------------------------------------------------------
                 * Build and put the rx command record for the rx command log
                 *------------------------------------------------------------------*/
                if (command_valid)
                {
                    /* get actual time */
                    result = Clock_GetTime(&current_time);
                    if (result)
                    {
                        /* current time     available */
                        time.year     = current_time.year;
                        time.month    = current_time.month;
                        time.day      = current_time.day;
                        time.hour     = current_time.hour;
                        time.minute   = current_time.minute;
                        time.second   = current_time.second;
                        time.week_day = Clock_WeekDayOfADay(time.day, time.month, time.year);
                    }
                    else
                    {
                        /* current time not available */
                        time.year     = 2000;
                        time.month    = 1;
                        time.day      = 1;
                        time.hour     = 0;
                        time.minute   = 0;
                        time.second   = 0;
                        time.week_day = 6;  // Saturday
                    }

                    /* build the rx command record */
                    rx_command_record.command_type = LOGS__RX_COMMAND__COMMAND_TYPE__PC;
                    rx_command_record.time         = time;
                    rx_command_record.command_exe  = TRUE;
                    strncpy(rx_command_record.name        , ""         ,  14);
                    strncpy(rx_command_record.phone_number, ""         ,  20);
                    strncpy(rx_command_record.text        , ptr_command, 160);
                    rx_command_record.name        [ 14] = 0x00;
                    rx_command_record.phone_number[ 20] = 0x00;
                    rx_command_record.text        [160] = 0x00;

                    /* put the rx command record in the rx command log */
                    result = Logs_RxCommmand_PutRecord(&rx_command_record);

                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "Created a rx commmand log record", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
            }
            else
            {
                /* parser error */
                AtDebug_SendString("\r\n+SMS: ERROR\r\n");
            }
            break;



        // Read command (AT+SMS?)
        // Test command (AT+SMS=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_MYCREG
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__MYCREG == 1)
static void AtDebug_Callback_AtCommand_MYCREG(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[2];

    ascii  answer_string[20 + 1];

    u8     status;
    u8     value;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+MYCREG", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if ((ptr_at_cmd_info->num_of_args == 0) || (ptr_at_cmd_info->num_of_args > 2))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            if (!Utility_IsNumString(ptr_arg_string[0]))
            {
                is_error = TRUE;
                break;
            }
            status = atoi(ptr_arg_string[0]);

            switch (status)
            {
                // <status> = 0
                // AT+MYCREG=<status>
                case 0:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 1)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* disable "MYCREG" */
                    Phone_MyGsm_MyCreg_Disable();
                    break;


                // <status> = 1
                // AT+MYCREG=<status>,<value>
                case 1:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 2)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <value> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    value = (u8)atoi(ptr_arg_string[1]);

                    /* verify the <value> parameter */
                    if (value > 5)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* enable "MYCREG" */
                    Phone_MyGsm_MyCreg_Enable(value);
                    break;


                // <mode> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+MYCREG?)
        case AT_CMD_TYPE_READ:
            /* get the "My CREG" status */
            Phone_MyGsm_MyCreg_GetStatus(&status, &value);

            if (status == 0)
                snprintf(answer_string, sizeof(answer_string), "\r\n+MYCREG: %d\r\n"   , status       );
            else
                snprintf(answer_string, sizeof(answer_string), "\r\n+MYCREG: %d,%d\r\n", status, value);
            AtDebug_SendString(answer_string);
            break;



        // Test command (AT+MYCREG=?)
        case AT_CMD_TYPE_TEST:
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_MYCSQ
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__MYCSQ == 1)
static void AtDebug_Callback_AtCommand_MYCSQ(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[3];

    ascii  answer_string[20 + 1];

    u8     status;
    u8     value_rssi;
    u8     value_ber;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+MYCSQ", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if ((ptr_at_cmd_info->num_of_args == 0) || (ptr_at_cmd_info->num_of_args > 3))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            if (!Utility_IsNumString(ptr_arg_string[0]))
            {
                is_error = TRUE;
                break;
            }
            status = atoi(ptr_arg_string[0]);

            switch (status)
            {
                // <status> = 0
                // AT+MYCREG=<status>
                case 0:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 1)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* disable "MYCSQ" */
                    Phone_MyGsm_MyCsq_Disable();
                    break;


                // <status> = 1
                // AT+MYCSQ=<status>,<value_rssi>,<value_ber>
                case 1:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 3)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <value_rssi> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    value_rssi = (u8)atoi(ptr_arg_string[1]);

                    /* <value_ber>  parameter */
                    if (!Utility_IsNumString(ptr_arg_string[2]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    value_ber  = (u8)atoi(ptr_arg_string[2]);

                    /* verify the <value_rssi> parameter */
                    if ((value_rssi > 31) && (value_rssi != 99))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* verify the <value_ber> parameter */
                    if ((value_ber  >  7) && (value_ber  != 99))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* enable "MYCSQ" */
                    Phone_MyGsm_MyCsq_Enable(value_rssi, value_ber);
                    break;


                // <mode> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+MYCSQ?)
        case AT_CMD_TYPE_READ:
            /* get the "My CSQ" status */
            Phone_MyGsm_MyCsq_GetStatus(&status, &value_rssi, &value_ber);

            if (status == 0)
                snprintf(answer_string, sizeof(answer_string), "\r\n+MYCSQ: %d\r\n"      , status                       );
            else
                snprintf(answer_string, sizeof(answer_string), "\r\n+MYCSQ: %d,%d,%d\r\n", status, value_rssi, value_ber);
            AtDebug_SendString(answer_string);
            break;



        // Test command (AT+MYCSQ=?)
        case AT_CMD_TYPE_TEST:
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_MYCGMS
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__MYCGMS == 1)
static void AtDebug_Callback_AtCommand_MYCGMS(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[2];

    ascii  answer_string[20 + 1];

    u8     status;
    u8     value;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+MYCGMS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if ((ptr_at_cmd_info->num_of_args == 0) || (ptr_at_cmd_info->num_of_args > 2))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            if (!Utility_IsNumString(ptr_arg_string[0]))
            {
                is_error = TRUE;
                break;
            }
            status = atoi(ptr_arg_string[0]);

            switch (status)
            {
                // <status> = 0
                // AT+MYCGMS=<status>
                case 0:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 1)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* disable "MYCMGS" */
                    Phone_MyGsm_MyCgms_Disable();
                    break;


                // <status> = 1
                // AT+MYCGMS=<status>,<value>
                case 1:
                    /* check the number of the parameters */
                    if (ptr_at_cmd_info->num_of_args != 2)
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* <value> parameter */
                    if (!Utility_IsNumString(ptr_arg_string[1]))
                    {
                        is_error = TRUE;
                        break;
                    }
                    value = (u8)atoi(ptr_arg_string[1]);

                    /* verify the <value> parameter */
                    if ((value != 0) && (value != 1))
                    {
                        is_error = TRUE;
                        break;
                    }

                    /* enable "MYCMGS" */
                    Phone_MyGsm_MyCgms_Enable((bool)value);
                    break;


                // <mode> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+MYCGMS?)
        case AT_CMD_TYPE_READ:
            /* get the "My CGMS" status */
            Phone_MyGsm_MyCgms_GetStatus(&status, (bool *)&value);

            if (value)
                value = 1;
            else
                value = 0;

            if (status == 0)
                snprintf(answer_string, sizeof(answer_string), "\r\n+MYCGMS: %d\r\n"   , status       );
            else
                snprintf(answer_string, sizeof(answer_string), "\r\n+MYCGMS: %d,%d\r\n", status, value);
            AtDebug_SendString(answer_string);
            break;



        // Test command (AT+MYCGMS=?)
        case AT_CMD_TYPE_TEST:
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_GPI
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__GPI == 1)
static void AtDebug_Callback_AtCommand_GPI(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii  answer_string[50 + 1];

    bool   input_status;
    ascii *input_description;

    bool   is_error;
    u8     i;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+GPI", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            break;


        // Read command (AT+GPI?)
        case AT_CMD_TYPE_READ:
            /* digital inputs */
            for (i = 0; i < DRV_GPIO__NUM_INPUTS; i++)
            {
                input_status      = DrvGpio_ReadInput           (i);
                input_description = DrvGpio_ReadInputDescription(i);

                snprintf(answer_string, sizeof(answer_string), "\r\n+GPI: %d,\"%s\",%d\r\n", i, input_description, input_status);
                AtDebug_SendString(answer_string);
            }
            break;


        // Test command (AT+GPI=?)
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_GPO
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__GPO == 1)
static void AtDebug_Callback_AtCommand_GPO(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[2];

    ascii  answer_string[50 + 1];

    u8     output_id;
    bool   output_status;
    ascii *output_description;

    bool   is_error;
    u8     i;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+GPO", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            output_id     = atoi(ptr_arg_string[0]);
            output_status = atoi(ptr_arg_string[1]);

            /* verify parameters value */
            if (
                 (output_id != 0) &&
                 (output_id != 1) &&
                 (output_id != 2) &&
                 (output_id != 3)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (output_status != 0) &&
                 (output_status != 1)
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the output */
            DrvGpio_SetOutput(output_id, output_status, 0);
            break;


        // Read command (AT+GPO?)
        case AT_CMD_TYPE_READ:
            /* digital inputs */
            for (i = 0; i < DRV_GPIO__NUM_OUTPUTS; i++)
            {
                output_status      = DrvGpio_ReadOutput           (i);
                output_description = DrvGpio_ReadOutputDescription(i);

                snprintf(answer_string, sizeof(answer_string), "\r\n+GPO: %d,\"%s\",%d\r\n", i, output_description, output_status);
                AtDebug_SendString(answer_string);
            }
            break;


        // Test command (AT+GPO=?)
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_QUEUE
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__QUEUE == 1)
static void AtDebug_Callback_AtCommand_QUEUE(AT_COMMAND_INFO *ptr_at_cmd_info)
{
          ascii *ptr_arg_string[3];

          ascii  answer_string[40 + 1];

          ascii *queue_1_description;
          ascii *queue_2_description;
          ascii *queue_3_description;
          ascii *queue_4_description;
          ascii *queue_5_description;
          ascii *queue_6_description;
          ascii *queue_7_description;
          ascii *queue_8_description;
          ascii *queue_9_description;

          u8     num_of_record_1;
          u8     num_of_record_2;
          u8     num_of_record_3;
          u8     num_of_record_4;
          u8     num_of_record_5;
          u8     num_of_record_6;
          u8     num_of_record_7;
          u8     num_of_record_8;
          u8     num_of_record_9;

          u8     num_total_of_record_1;
          u8     num_total_of_record_2;
          u8     num_total_of_record_3;
          u8     num_total_of_record_4;
          u8     num_total_of_record_5;
          u8     num_total_of_record_6;
          u8     num_total_of_record_7;
          u8     num_total_of_record_8;
          u8     num_total_of_record_9;

          u8     queue_id;
          u8     num_of_records;
    const ascii *file_name;

          bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+QUEUE", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 3)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            queue_id       = atoi(ptr_arg_string[0]);
            num_of_records = atoi(ptr_arg_string[1]);
            file_name      =      ptr_arg_string[2];

            /* verify parameters value */
            if (
                 (queue_id == 0) ||
                 (queue_id >  9)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (num_of_records == 0 ) ||
                 (num_of_records >  20)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (queue_id == 8) ||
                 (queue_id == 9)
               )
            {
                if (strlen(file_name) == 0)
                {
                    is_error = TRUE;
                    break;
                }
            }


            /* add the records in the queue */
            switch (queue_id)
            {
                case 1:
                    Test_PutRecord_FwUpgrade      (num_of_records);
                    break;

                case 2:
                    Test_PutRecord_Alarm          (num_of_records);
                    break;

                case 3:
                    Test_PutRecord_SmsAnswer      (num_of_records);
                    break;

                case 4:
                default:
                    // NOTHING TO DO
                    break;

                case 5:
                    Test_PutRecord_SmsForward     (num_of_records);
                    break;

                case 6:
                    Test_PutRecord_Autosynchronize(num_of_records);
                    break;

                case 7:
                    Test_PutRecord_SmsAnswerEvent (num_of_records);
                    break;

                case 8:
                    Test_PutRecord_FileStatusLog  (num_of_records, file_name);
                    break;

                case 9:
                    Test_PutRecord_FileAlarm      (num_of_records, file_name);
                    break;
            }
            break;


        // Read command (AT+QUEUE?)
        case AT_CMD_TYPE_READ:
            num_of_record_1 = Queue_FwUpgrade_NumRecords      ();
            num_of_record_2 = Queue_Alarm_NumRecords          ();
            num_of_record_3 = Queue_SmsAnswer_NumRecords      ();
            num_of_record_4 = Queue_SmsAnswerToCrs_NumRecords ();
            num_of_record_5 = Queue_SmsForward_NumRecords     ();
            num_of_record_6 = Queue_Autosynchronize_NumRecords();
            num_of_record_7 = Queue_SmsAnswerEvent_NumRecords ();
            num_of_record_8 = Queue_FileStatusLog_NumRecords  ();
            num_of_record_9 = Queue_FileAlarm_NumRecords      ();

            num_total_of_record_1 = QUEUE__QUEUE_SIZE__FW_UPGRADE;
            num_total_of_record_2 = QUEUE__QUEUE_SIZE__ALARM;
            num_total_of_record_3 = QUEUE__QUEUE_SIZE__SMS_ANSWER;
            num_total_of_record_4 = QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS;
            num_total_of_record_5 = QUEUE__QUEUE_SIZE__SMS_FORWARD;
            num_total_of_record_6 = QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE;
            num_total_of_record_7 = QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT;
            num_total_of_record_8 = QUEUE__QUEUE_SIZE__FILE_STATUS_LOG;
            num_total_of_record_9 = QUEUE__QUEUE_SIZE__FILE_ALARM;

            queue_1_description = Queue_FwUpgrade_ReadQueueDescription();
            queue_2_description = Queue_Alarm_ReadQueueDescription();
            queue_3_description = Queue_SmsAnswer_ReadQueueDescription();
            queue_4_description = Queue_SmsAnswerToCrs_ReadQueueDescription();
            queue_5_description = Queue_SmsForward_ReadQueueDescription();
            queue_6_description = Queue_Autosynchronize_ReadQueueDescription();
            queue_7_description = Queue_SmsAnswerEvent_ReadQueueDescription();
            queue_8_description = Queue_FileStatusLog_ReadQueueDescription();
            queue_9_description = Queue_FileAlarm_ReadQueueDescription();

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 1,\"%s\",%d,%d\r\n" , queue_1_description , num_of_record_1, num_total_of_record_1);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 2,\"%s\",%d,%d\r\n" , queue_2_description , num_of_record_2, num_total_of_record_2);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 3,\"%s\",%d,%d\r\n" , queue_3_description , num_of_record_3, num_total_of_record_3);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 4,\"%s\",%d,%d\r\n" , queue_4_description , num_of_record_4, num_total_of_record_4);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 5,\"%s\",%d,%d\r\n" , queue_5_description , num_of_record_5, num_total_of_record_5);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 6,\"%s\",%d,%d\r\n" , queue_6_description , num_of_record_6, num_total_of_record_6);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 7,\"%s\",%d,%d\r\n" , queue_7_description , num_of_record_7, num_total_of_record_7);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 8,\"%s\",%d,%d\r\n" , queue_8_description , num_of_record_8, num_total_of_record_8);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+QUEUE: 9,\"%s\",%d,%d\r\n" , queue_9_description , num_of_record_9, num_total_of_record_9);
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+QUEUE=?)
        case AT_CMD_TYPE_TEST:
            is_error = TRUE;
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_LOG
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__LOG == 1)
static void AtDebug_Callback_AtCommand_LOG(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii  answer_string[40 + 1];

    ascii *log_1_description;

    u8     num_of_record_1;

    u8     num_total_of_record_1;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+LOG", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            is_error = TRUE;
            break;


        // Read command (AT+LOG?)
        case AT_CMD_TYPE_READ:
            num_of_record_1 = Logs_RxCommmand_NumRecords();

            num_total_of_record_1 = LOGS__LOG_SIZE__RX_COMMAND;

            log_1_description  = Logs_RxCommmand_ReadLogDescription();

            snprintf(answer_string, sizeof(answer_string), "\r\n+LOG: 1,\"%s\",%d,%d\r\n" , log_1_description , num_of_record_1 , num_total_of_record_1);
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+LOG=?)
        case AT_CMD_TYPE_TEST:
            is_error = TRUE;
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_FLASH
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__FLASH == 1)
static void AtDebug_Callback_AtCommand_FLASH(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[3];

    ascii  answer_string[30 + 1];

    u8     handle_index;
    u16    start_id;
    u16    end_id;

    u16    num_max_id;
    ascii *handle;

    u32    free_mem;
    s32    used_size;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+FLASH", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 3)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            handle_index = atoi(ptr_arg_string[0]);
            start_id     = atoi(ptr_arg_string[1]);
            end_id       = atoi(ptr_arg_string[2]);

            if (handle_index == 0xFF)
                handle     = NULL;
            else
                handle     = ProgramFlash_HandleString   (handle_index);

            if (end_id       == 0xFF)
                num_max_id = ADL_FLH_ALL_IDS;
            else
                num_max_id = ProgramFlash_HandleNumMaxIds(handle_index);

            /* verify parameters value */
            if (start_id >= num_max_id)
            {
                is_error = TRUE;
                break;
            }
            if (end_id   >= num_max_id)
            {
                is_error = TRUE;
                break;
            }
            if (start_id >  end_id    )
            {
                is_error = TRUE;
                break;
            }

            /* get flash objects used size */
            used_size = adl_flhGetUsedSize(handle, start_id, end_id);

            /* get flash objects free size */
            free_mem  = adl_flhGetFreeMem();

            if (used_size >= 0)
                snprintf(answer_string, sizeof(answer_string), "\r\n+FLASH: %ld,%ld\r\n", used_size, free_mem);
            else
                snprintf(answer_string, sizeof(answer_string), "\r\n+FLASH: ???,%ld\r\n",            free_mem);
            AtDebug_SendString(answer_string);
            break;


        // Read command (AT+FLASH?)
        // Test command (AT+FLASH=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETALARMINPUT
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETALARMINPUT == 1)
static void AtDebug_Callback_AtCommand_SETALARMINPUT(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                                *ptr_arg_string[2];

    ascii                                 answer_string[30 + 1];

    /* configuration - IN1 digital input alarm */
    /* configuration - IN2 digital input alarm */
    ALARM_INPUT__CONFIG__ALARM_INPUT_IN1  config_alarm_input_in1;
    ALARM_INPUT__CONFIG__ALARM_INPUT_IN2  config_alarm_input_in2;

    u8                                    input_id;
    u8                                    alarm_enabled_status_input;

    bool                                  is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETALARMINPUT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            input_id                   = atoi(ptr_arg_string[0]);
            alarm_enabled_status_input = atoi(ptr_arg_string[1]);

            /* verify parameters value */
            if (
                 (input_id != 1) &&
                 (input_id != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (alarm_enabled_status_input != 0) &&
                 (alarm_enabled_status_input != 1)
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the configuration */
            if (input_id == 1)
            {
                config_alarm_input_in1.alarm_enabled_status_input = (bool)alarm_enabled_status_input;
                AlarmInput_Config_AlarmInputIn1_Set(&config_alarm_input_in1);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_ENABLE_ALARM);
            }
            else
            {
                config_alarm_input_in2.alarm_enabled_status_input = (bool)alarm_enabled_status_input;
                AlarmInput_Config_AlarmInputIn2_Set(&config_alarm_input_in2);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_ENABLE_ALARM);
            }
            break;


        // Read command (AT+SETALARMINPUT?)
        // Test command (AT+SETALARMINPUT=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the configuration */
            AlarmInput_Config_AlarmInputIn1_Get(&config_alarm_input_in1);
            AlarmInput_Config_AlarmInputIn2_Get(&config_alarm_input_in2);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETALARMINPUT: 1,%d\r\n", config_alarm_input_in1.alarm_enabled_status_input);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETALARMINPUT: 2,%d\r\n", config_alarm_input_in2.alarm_enabled_status_input);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETINPUT
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETINPUT == 1)
static void AtDebug_Callback_AtCommand_SETINPUT(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                          *ptr_arg_string[4];

    ascii                           answer_string[30 + 1];

    /* configuration - IN1 digital input */
    /* configuration - IN2 digital input */
    ALARM_INPUT__CONFIG__INPUT_IN1  config_input_in1;
    ALARM_INPUT__CONFIG__INPUT_IN2  config_input_in2;

    u8                              input_id;
    u8                              activation_status;
    u8                              activation_time_unit;
    u8                              activation_time_value;

    bool                            is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETINPUT", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 4)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            input_id              = atoi(ptr_arg_string[0]);
            activation_status     = atoi(ptr_arg_string[1]);
            activation_time_unit  = atoi(ptr_arg_string[2]);
            activation_time_value = atoi(ptr_arg_string[3]);

            /* verify parameters value */
            if (
                 (input_id != 1) &&
                 (input_id != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (activation_status != 0) &&
                 (activation_status != 1)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND) &&
                 (activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE) &&
                 (activation_time_unit != ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR  )
               )
            {
                is_error = TRUE;
                break;
            }
            if (activation_time_value == 0)
            {
                is_error = TRUE;
                break;
            }

            /* set the configuration */
            if (input_id == 1)
            {
                config_input_in1.activation_status     = (bool)activation_status;
                config_input_in1.activation_time_unit  =       activation_time_unit;
                config_input_in1.activation_time_value =       activation_time_value;
                AlarmInput_Config_InputIn1_Set(&config_input_in1);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN1_INPUT_ALARM);
            }
            else
            {
                config_input_in2.activation_status     = (bool)activation_status;
                config_input_in2.activation_time_unit  =       activation_time_unit;
                config_input_in2.activation_time_value =       activation_time_value;
                AlarmInput_Config_InputIn2_Set(&config_input_in2);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__INPUT_IN2_INPUT_ALARM);
            }
            break;


        // Read command (AT+SETINPUT?)
        // Test command (AT+SETINPUT=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the configuration */
            AlarmInput_Config_InputIn1_Get(&config_input_in1);
            AlarmInput_Config_InputIn2_Get(&config_input_in2);

            snprintf(answer_string,
                     sizeof(answer_string),
                     "\r\n+SETINPUT: 1,%d,%d,%d\r\n",
                     config_input_in1.activation_status,
                     config_input_in1.activation_time_unit,
                     config_input_in1.activation_time_value);
            AtDebug_SendString(answer_string);

            snprintf(answer_string,
                     sizeof(answer_string),
                     "\r\n+SETINPUT: 2,%d,%d,%d\r\n",
                     config_input_in2.activation_status,
                     config_input_in2.activation_time_unit,
                     config_input_in2.activation_time_value);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETALARMTMAX
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETALARMTMAX == 1)
static void AtDebug_Callback_AtCommand_SETALARMTMAX(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                                         *ptr_arg_string[2];

    ascii                                          answer_string[30 + 1];

    /* configuration - maximum internal temperature alarm */
    /* configuration - maximum external temperature alarm */
    ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_INT  config_alarm_temperature_max_int;
    ALARM_TMAX__CONFIG__ALARM_TEMPERATURE_MAX_EXT  config_alarm_temperature_max_ext;

    u8                                             temperature;
    u8                                             alarm_enabled_status_tmax;

    bool                                           is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETALARMTMAX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature               = atoi(ptr_arg_string[0]);
            alarm_enabled_status_tmax = atoi(ptr_arg_string[1]);

            /* verify parameters value */
            if (
                 (temperature != 1) &&
                 (temperature != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (alarm_enabled_status_tmax != 0) &&
                 (alarm_enabled_status_tmax != 1)
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the configuration */
            if (temperature == 1)
            {
                config_alarm_temperature_max_int.alarm_enabled_status_tmax_int = alarm_enabled_status_tmax;
                AlarmTMax_Config_AlarmTemperatureMaxInt_Set(&config_alarm_temperature_max_int);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_ENABLE_ALARM);
            }
            else
            {
                config_alarm_temperature_max_ext.alarm_enabled_status_tmax_ext = alarm_enabled_status_tmax;
                AlarmTMax_Config_AlarmTemperatureMaxExt_Set(&config_alarm_temperature_max_ext);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_ENABLE_ALARM);
            }
            break;


        // Read command (AT+SETALARMTMAX?)
        // Test command (AT+SETALARMTMAX=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the configuration */
            AlarmTMax_Config_AlarmTemperatureMaxInt_Get(&config_alarm_temperature_max_int);
            AlarmTMax_Config_AlarmTemperatureMaxExt_Get(&config_alarm_temperature_max_ext);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETALARMTMAX: 1,%d\r\n", config_alarm_temperature_max_int.alarm_enabled_status_tmax_int);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETALARMTMAX: 2,%d\r\n", config_alarm_temperature_max_ext.alarm_enabled_status_tmax_ext);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETTMAX
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETTMAX == 1)
static void AtDebug_Callback_AtCommand_SETTMAX(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                                   *ptr_arg_string[2];

    ascii                                    answer_string[30 + 1];

    /* configuration - maximum internal temperature */
    /* configuration - maximum external temperature */
    ALARM_TMAX__CONFIG__TEMPERATURE_MAX_INT  config_temperature_max_int;
    ALARM_TMAX__CONFIG__TEMPERATURE_MAX_EXT  config_temperature_max_ext;

    u8                                       temperature;
    s16                                      temperature_max;

    bool                                     is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETTMAX", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature     = atoi(ptr_arg_string[0]);
            temperature_max = atoi(ptr_arg_string[1]);

            /* verify parameters value */
            if (
                 (temperature != 1) &&
                 (temperature != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (temperature_max < -200) ||
                 (temperature_max >  550)
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the configuration */
            if (temperature == 1)
            {
                config_temperature_max_int.temperature_max_int = temperature_max;            /* maximum internal temperature for alarm (/10 �C) */   // [-20.0 - +55.0 �C]
                AlarmTMax_Config_TemperatureMaxInt_Set(&config_temperature_max_int);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_INT_TEMPERATURE_ALARM);
            }
            else
            {
                config_temperature_max_ext.temperature_max_ext = temperature_max;
                AlarmTMax_Config_TemperatureMaxExt_Set(&config_temperature_max_ext);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MAX_EXT_TEMPERATURE_ALARM);
            }
            break;


        // Read command (AT+SETTMAX?)
        // Test command (AT+SETTMAX=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the configuration */
            AlarmTMax_Config_TemperatureMaxInt_Get(&config_temperature_max_int);
            AlarmTMax_Config_TemperatureMaxExt_Get(&config_temperature_max_ext);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETTMAX: 1,%d\r\n", config_temperature_max_int.temperature_max_int);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETTMAX: 2,%d\r\n", config_temperature_max_ext.temperature_max_ext);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETALARMTMIN
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETALARMTMIN == 1)
static void AtDebug_Callback_AtCommand_SETALARMTMIN(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                                         *ptr_arg_string[2];

    ascii                                          answer_string[30 + 1];

    /* configuration - minimum internal temperature alarm */
    /* configuration - minimum external temperature alarm */
    ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_INT  config_alarm_temperature_min_int;
    ALARM_TMIN__CONFIG__ALARM_TEMPERATURE_MIN_EXT  config_alarm_temperature_min_ext;

    u8                                             temperature;
    u8                                             alarm_enabled_status_tmin;

    bool                                           is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETALARMTMIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature               = atoi(ptr_arg_string[0]);
            alarm_enabled_status_tmin = atoi(ptr_arg_string[1]);

            /* verify parameters value */
            if (
                 (temperature != 1) &&
                 (temperature != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (alarm_enabled_status_tmin != 0) &&
                 (alarm_enabled_status_tmin != 1)
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the configuration */
            if (temperature == 1)
            {
                config_alarm_temperature_min_int.alarm_enabled_status_tmin_int = alarm_enabled_status_tmin;
                AlarmTMin_Config_AlarmTemperatureMinInt_Set(&config_alarm_temperature_min_int);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_ENABLE_ALARM);
            }
            else
            {
                config_alarm_temperature_min_ext.alarm_enabled_status_tmin_ext = alarm_enabled_status_tmin;
                AlarmTMin_Config_AlarmTemperatureMinExt_Set(&config_alarm_temperature_min_ext);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_ENABLE_ALARM);
            }
            break;


        // Read command (AT+SETALARMTMIN?)
        // Test command (AT+SETALARMTMIN=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the configuration */
            AlarmTMin_Config_AlarmTemperatureMinInt_Get(&config_alarm_temperature_min_int);
            AlarmTMin_Config_AlarmTemperatureMinExt_Get(&config_alarm_temperature_min_ext);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETALARMTMIN: 1,%d\r\n", config_alarm_temperature_min_int.alarm_enabled_status_tmin_int);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETALARMTMIN: 2,%d\r\n", config_alarm_temperature_min_ext.alarm_enabled_status_tmin_ext);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETTMIN
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETTMIN == 1)
static void AtDebug_Callback_AtCommand_SETTMIN(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii                                   *ptr_arg_string[2];

    ascii                                    answer_string[30 + 1];

    /* configuration - minimun internal temperature */
    /* configuration - minimum external temperature */
    ALARM_TMIN__CONFIG__TEMPERATURE_MIN_INT  config_temperature_min_int;
    ALARM_TMIN__CONFIG__TEMPERATURE_MIN_EXT  config_temperature_min_ext;

    u8                                       temperature;
    s16                                      temperature_min;

    bool                                     is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETTMIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature     = atoi(ptr_arg_string[0]);
            temperature_min = atoi(ptr_arg_string[1]);

            /* verify parameters value */
            if (
                 (temperature != 1) &&
                 (temperature != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (temperature_min < -200) ||
                 (temperature_min >  550)
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the configuration */
            if (temperature == 1)
            {
                config_temperature_min_int.temperature_min_int = temperature_min;
                AlarmTMin_Config_TemperatureMinInt_Set(&config_temperature_min_int);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM);
            }
            else
            {
                config_temperature_min_ext.temperature_min_ext = temperature_min;
                AlarmTMin_Config_TemperatureMinExt_Set(&config_temperature_min_ext);
                ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_EXT_TEMPERATURE_ALARM);
            }
            break;


        // Read command (AT+SETTMIN?)
        // Test command (AT+SETTMIN=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the configuration */
            AlarmTMin_Config_TemperatureMinInt_Get(&config_temperature_min_int);
            AlarmTMin_Config_TemperatureMinExt_Get(&config_temperature_min_ext);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETTMIN: 1,%d\r\n", config_temperature_min_int.temperature_min_int);
            AtDebug_SendString(answer_string);

            snprintf(answer_string, sizeof(answer_string), "\r\n+SETTMIN: 2,%d\r\n", config_temperature_min_ext.temperature_min_ext);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_SETPHONEBOOK
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__SETPHONEBOOK == 1)
static void AtDebug_Callback_AtCommand_SETPHONEBOOK(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[2];

  //ascii  answer_string[30 + 1];

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+SETPHONEBOOK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

          //temperature     = atoi(ptr_arg_string[0]);
          //temperature_min = atoi(ptr_arg_string[1]);

            /* verify parameters value */
          //if (
          //     (temperature != 1) &&
          //     (temperature != 2)
          //   )
          //{
          //    is_error = TRUE;
          //    break;
          //}

            /* set the configuration */
          //if (temperature == 1)
          //{
          //    config_temperature_min_int.temperature_min_int = temperature_min;
          //    AlarmTMin_Config_TemperatureMinInt_Set(&config_temperature_min_int);
          //    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__001_CONFIGURATIONS, PROGRAM_FLASH__FLASH_ID__001_CONFIGURATIONS__T_MIN_INT_TEMPERATURE_ALARM);
          //}
            break;


        // Read command (AT+SETPHONEBOOK?)
        // Test command (AT+SETPHONEBOOK=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            Phonebook_PrintPhonebook();
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_REGULATION
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__REGULATION == 1)
static void AtDebug_Callback_AtCommand_REGULATION(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[3];

    u8     temperature_id;
    u8     mode;
    s16    temperature;

    bool   is_error;
    bool   result;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+REGULATION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if ((ptr_at_cmd_info->num_of_args != 2) && (ptr_at_cmd_info->num_of_args != 3))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature_id  = atoi(ptr_arg_string[0]);
            mode            = atoi(ptr_arg_string[1]);
            if (ptr_at_cmd_info->num_of_args == 3)
                temperature = atoi(ptr_arg_string[2]);
            else
                temperature = 0;

            /* verify parameters value */
            if (
                 (temperature_id != 1) &&
                 (temperature_id != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (mode != 0) &&
                 (mode != 1) &&
                 (mode != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (mode == 2)
            {
                if (ptr_at_cmd_info->num_of_args != 3)
                {
                    is_error = TRUE;
                    break;
                }
            }
            if (
                 (temperature < -200) ||
                 (temperature >  550)
               )
            {
                is_error = TRUE;
                break;
            }

            /* action on regulation */
            if (temperature_id == 1)
            {
                /* internal temperature */
                if      (mode == 0)
                    result = Regulation_RegulationTemperatureInt_Off();
                else if (mode == 1)
                    result = Regulation_RegulationTemperatureInt_On();
                else if (mode == 2)
                    result = Regulation_RegulationTemperatureInt_Regulation(temperature);
            }
            else
            {
                /* external temperature */
                if      (mode == 0)
                    result = Regulation_RegulationTemperatureExt_Off();
                else if (mode == 1)
                    result = Regulation_RegulationTemperatureExt_On();
                else if (mode == 2)
                    result = Regulation_RegulationTemperatureExt_Regulation(temperature);
            }
            break;


        // Read command (AT+REGULATION?)
        // Test command (AT+REGULATION=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_F_REGULATION
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__F_REGULATION == 1)
static void AtDebug_Callback_AtCommand_F_REGULATION(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[4];

    u8     temperature_id;
    u8     action;
    s16    temperature;
    u16    duration;

    bool   is_error;
  //bool   result;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+F_REGULATION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if ((ptr_at_cmd_info->num_of_args != 2) && (ptr_at_cmd_info->num_of_args != 4))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature_id  = atoi(ptr_arg_string[0]);
            action          = atoi(ptr_arg_string[1]);
            if (ptr_at_cmd_info->num_of_args == 4)
            {
                temperature = atoi(ptr_arg_string[2]);
                duration    = atoi(ptr_arg_string[3]);
            }
            else
            {
                temperature = 0;
                duration    = 1;
            }

            /* verify parameters value */
            if (
                 (temperature_id != 1) &&
                 (temperature_id != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (action != 0) &&
                 (action != 1)
               )
            {
                is_error = TRUE;
                break;
            }
            if (action == 1)
            {
                if (ptr_at_cmd_info->num_of_args != 4)
                {
                    is_error = TRUE;
                    break;
                }
            }
            if (
                 (temperature < -200) ||
                 (temperature >  550)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (duration == 0 ) ||
                 (duration >  48)
               )
            {
                is_error = TRUE;
                break;
            }

            /* action on regulation */
            if (temperature_id == 1)
            {
                /* internal temperature */
                //if (action == 0)
                //    result = FRegulation_RegulationTemperatureInt_Stop();
                //else
                //    result = FRegulation_RegulationTemperatureInt_Start(temperature, duration);

            }
            else
            {
                /* external temperature */
                //if (action == 0)
                //    result = FRegulation_RegulationTemperatureExt_Stop();
                //else
                //    result = FRegulation_RegulationTemperatureExt_Start(temperature, duration);
            }
            break;


        // Read command (AT+F_REGULATION?)
        // Test command (AT+F_REGULATION=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_F_ANTIFROST
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__F_ANTIFROST == 1)
static void AtDebug_Callback_AtCommand_F_ANTIFROST(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[3];

    u8     temperature_id;
    u8     action;
    s16    temperature;

    bool   is_error;
  //bool   result;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+F_ANTIFROST", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if ((ptr_at_cmd_info->num_of_args != 2) && (ptr_at_cmd_info->num_of_args != 3))
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            temperature_id  = atoi(ptr_arg_string[0]);
            action          = atoi(ptr_arg_string[1]);
            if (ptr_at_cmd_info->num_of_args == 3)
                temperature = atoi(ptr_arg_string[2]);
            else
                temperature = 0;

            /* verify parameters value */
            if (
                 (temperature_id != 1) &&
                 (temperature_id != 2)
               )
            {
                is_error = TRUE;
                break;
            }
            if (
                 (action != 0) &&
                 (action != 1)
               )
            {
                is_error = TRUE;
                break;
            }
            if (action == 1)
            {
                if (ptr_at_cmd_info->num_of_args != 3)
                {
                    is_error = TRUE;
                    break;
                }
            }
            if (
                 (temperature < -200) ||
                 (temperature >  550)
               )
            {
                is_error = TRUE;
                break;
            }

            /* action on regulation */
            if (temperature_id == 1)
            {
                /* internal temperature */
                //if (action == 0)
                //    result = FAntifrost_AntifrostTemperatureInt_Off();
                //else
                //    result = FAntifrost_AntifrostTemperatureInt_On(temperature);
            }
            else
            {
                /* external temperature */
                //if (action == 0)
                //    result = FAntifrost_AntifrostTemperatureExt_Off();
                //else
                //    result = FAntifrost_AntifrostTemperatureExt_On(temperature);
            }
            break;


        // Read command (AT+F_ANTIFROST?)
        // Test command (AT+F_ANTIFROST=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_CHARGER
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__CHARGER == 1)
static void AtDebug_Callback_AtCommand_CHARGER(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[3];

    ascii  answer_string[40 + 1];

    u32    time_charge_on;
    u32    time_charge_off;
    u32    time_charge_off_after_power_on;

    u32    time_charge_on_ms;
    u32    time_charge_off_ms;
    u32    time_charge_off_after_power_on_ms;

    bool   is_error;
  //bool   result;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+CHARGER", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 3)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            time_charge_on                 = atoi(ptr_arg_string[0]);
            time_charge_off                = atoi(ptr_arg_string[1]);
            time_charge_off_after_power_on = atoi(ptr_arg_string[2]);

            /* verify parameters value */
            if (
                 (time_charge_on                 == 0) ||
                 (time_charge_off                == 0) ||
                 (time_charge_off_after_power_on == 0)
               )
            {
                is_error = TRUE;
                break;
            }

            time_charge_on_ms                 = time_charge_on                 * 1000L;
            time_charge_off_ms                = time_charge_off                * 1000L;
            time_charge_off_after_power_on_ms = time_charge_off_after_power_on * 1000L;

            /* check the charger parameters */
            if (
                 (!Charger_IsValidTimeChargeOn             (&time_charge_on_ms                )) ||
                 (!Charger_IsValidTimeChargeOff            (&time_charge_off_ms               )) ||
                 (!Charger_IsValidTimeChargeOffAfterPowerOn(&time_charge_off_after_power_on_ms))
               )
            {
                is_error = TRUE;
                break;
            }

            /* set the charger parameters */
            Charger_SetTimeChargeOn             (&time_charge_on_ms                );
            Charger_SetTimeChargeOff            (&time_charge_off_ms               );
            Charger_SetTimeChargeOffAfterPowerOn(&time_charge_off_after_power_on_ms);
            break;


        // Read command (AT+CHARGER?)
        // Test command (AT+CHARGER=?)
        case AT_CMD_TYPE_READ:
        case AT_CMD_TYPE_TEST:
            /* get the charger parameters */
            Charger_GetTimeChargeOn             (&time_charge_on_ms                );
            Charger_GetTimeChargeOff            (&time_charge_off_ms               );
            Charger_GetTimeChargeOffAfterPowerOn(&time_charge_off_after_power_on_ms);

            time_charge_on                 = time_charge_on_ms                 / 1000L;
            time_charge_off                = time_charge_off_ms                / 1000L;
            time_charge_off_after_power_on = time_charge_off_after_power_on_ms / 1000L;

            snprintf(answer_string, sizeof(answer_string), "\r\n+CHARGER: %ld,%ld,%ld\r\n", time_charge_on, time_charge_off, time_charge_off_after_power_on);
            AtDebug_SendString(answer_string);
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_FS
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__FS == 1)
static void AtDebug_Callback_AtCommand_FS(AT_COMMAND_INFO *ptr_at_cmd_info)
{
           ascii *ptr_arg_string[3];

    static ascii  answer_string[128 + 1];

    static bool   fs_init_done = FALSE;

           u8     mode;
           u8     par_1;
           u8     par_2;

           bool   is_error;
         //bool   result;
           int    res_int;

           enum
           {
               CMD_INIT = 0,         //  0
               CMD_CHDIR,            //  1
               CMD_DIR,              //  2
               CMD_RMDIR,            //  3
               CMD_MKDIR,            //  4
               CMD_DEL,              //  5
               CMD_PUT,              //  6
               CMD_FORMAT,           //  7
               CMD_GET,              //  8
               CMD_INSTALL,          //  9
               CMD_UNLOCK,           // 10
               CMD_LOCK,             // 11
               CMD_CHANGE_PASSWORD,  // 12
               // extra commands
               CMD_CREATE_TREE = 0x20,
           };


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+FS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (
                 (ptr_at_cmd_info->num_of_args == 0) ||
                 (ptr_at_cmd_info->num_of_args >  3)
               )
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            mode  = atoi(ptr_arg_string[0]);
            par_1 = atoi(ptr_arg_string[1]);
            par_2 = atoi(ptr_arg_string[2]);

            /* verify parameters value */
            if (
                 (mode > 12)
               )
            {
                is_error = TRUE;
                break;
            }

            /* execute the command depending on <mode> */
            switch (mode)
            {
                case CMD_INIT:
                    if (!fs_init_done)
                    {
                        FsMisc_NorFsInit();
                        fs_init_done = TRUE;
                    }
                    break;


                // Parse Directory
                case CMD_DIR:
                    if (ptr_at_cmd_info->num_of_args == 2)
                    {
                        if (FsMisc_Cmd_Dir((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1)) < 0)
                        {
                            AtDebug_SendString("\r\nERROR: Dir Error\r\n");
                            return;
                        }
                    }
                    else
                    {
                        if (FsMisc_Cmd_Dir(""                                             ) < 0)
                        {
                            AtDebug_SendString("\r\nERROR: Dir Error\r\n");
                            return;
                        }
                    }
                    break;


                // Get File
                case CMD_GET:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        AtDebug_SendString("\r\nERROR: Missing Name\r\n");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nGet content from %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if (FsMisc_Cmd_GetFile((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1)) < 0)
                    {
                        AtDebug_SendString("\r\nERROR: Get file Error\r\n");
                        return;
                    }
                    break;


                case CMD_CHDIR:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        AtDebug_SendString("\r\nERROR: Missing path\r\n");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nChange Directory to %s",(ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if (adl_fsChdir((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1)))
                    {
                        AtDebug_SendString("\r\nERROR: Bad Path\r\n");
                        return;
                    }
                    break;


                // Put File
                case CMD_PUT:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        adl_atSendResponse(ADL_AT_RSP, "\r\nMissing Name");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nPut content to %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    FsMisc_Cmd_PutFile(ptr_at_cmd_info->ParaList, ptr_at_cmd_info->num_of_args);
                    break;


                case CMD_DEL:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        adl_atSendResponse(ADL_AT_RSP, "\r\nERROR: No File Name provided");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nDeleting file %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if (adl_fsDelete((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1)))
                    {
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, "\r\nERROR: Impossible to delete\r\n");
                        return;
                    }
                    break;


                case CMD_MKDIR:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        adl_atSendResponse(ADL_AT_RSP, "\r\nERROR: No Directory Name provided");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nCreating directory %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if ((res_int = adl_fsMkdir((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1))) != ADL_FS_NO_ERROR)
                    {
                        snprintf(answer_string, sizeof(answer_string), "\r\nERROR: Impossible to create, error %d\r\n", res_int);
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                        return;
                    }
                    break;


                case CMD_RMDIR:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, "\r\nERROR: No Directory Name provided");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nRemoving directory %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if ((res_int = adl_fsRmdir((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1))) != ADL_FS_NO_ERROR)
                    {
                        snprintf(answer_string, sizeof(answer_string), "\r\nERROR: Impossible to remove, error %d\r\n", res_int);
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                        return;
                    }
                    break;


                case CMD_FORMAT:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        snprintf(answer_string, sizeof(answer_string), "\r\nERROR: Bad param number");
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nFormating drive %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if ((res_int = adl_fsFormat(atoi((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1)))) != ADL_FS_NO_ERROR)
                    {
                        snprintf(answer_string, sizeof(answer_string),"\r\nERROR: Impossible to format, error %d\r\n", res_int);
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                        return;
                    }
                    break;


                case CMD_INSTALL:
                    if (ptr_at_cmd_info->num_of_args < 2)
                    {
                        AtDebug_SendString("\r\nERROR: No File Name provided\r\n");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nInstalling file %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    if (adl_fsInstall((ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1)))
                    {
                        adl_atSendResponse(ADL_AT_RSP, "\r\nERROR: Impossible to install\r\n");
                        return;
                    }
                    break;


                case CMD_UNLOCK:
                    /*s32 fs_api_unlock(const char *path, char *pwd, u32 pwd_length, u32 *access_mask)*/
                    if (ptr_at_cmd_info->num_of_args < 3)
                    {
                        AtDebug_SendString("\r\nERROR: Unlock : Not enougth parameters provided\r\n");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nUnlock %s", (ascii *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    res_int = FsMisc_Cmd_Unlock(ptr_at_cmd_info->ParaList, ptr_at_cmd_info->num_of_args);
                    if (res_int & 0x80000000)
                        /* error */
                        snprintf(answer_string, sizeof(answer_string), "\r\n Error = %d\r\n" , (res_int & ~0x80000000));
                    else
                        snprintf(answer_string, sizeof(answer_string), "\r\n Access = %d\r\n",  res_int               );
                    AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                    break;


                case CMD_LOCK:
                    /* s32 fs_api_lock(const char *path, char *cur_pwd, u32 cur_pwd_length, u32 access_mask) */
                    if (ptr_at_cmd_info->num_of_args < 4)
                    {
                        adl_atSendResponse(ADL_AT_RSP, "\r\nERROR: Lock : Not enough parameters provided\r\n");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string), "\r\nLock %s", (char *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    res_int = FsMisc_Cmd_Lock(ptr_at_cmd_info->ParaList, ptr_at_cmd_info->num_of_args);
                    if (res_int)
                    {
                        snprintf(answer_string, sizeof(answer_string), "\r\nError = %d\r\n", res_int);
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                    }
                    else
                    {
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, "\r\nDone\r\n");
                    }
                    break;


                case CMD_CHANGE_PASSWORD:
                    /* s32 fs_api_changePassword(const char *path, char *cur_pwd, u32 cur_pwd_length, char *new_pwd, u32 new_pwd_length) */
                    if (ptr_at_cmd_info->num_of_args < 4)
                    {
                        adl_atSendResponse(ADL_AT_RSP, "\r\nERROR: Change password : Not enough parameters provided\r\n");
                        return;
                    }

                    snprintf(answer_string, sizeof(answer_string),"\r\nChange Password in %s", (char *)wm_lstGetItem(ptr_at_cmd_info->ParaList, 1));
                    AtDebug_SendString(answer_string);

                    res_int = FsMisc_Cmd_ChangePassword(ptr_at_cmd_info->ParaList, ptr_at_cmd_info->num_of_args);
                    if (res_int)
                    {
                        snprintf(answer_string, sizeof(answer_string), "\r\nError = %d\r\n", res_int);
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                    }
                    else
                    {
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, "\r\nDone\r\n");
                    }
                    break;


                case CMD_CREATE_TREE:
                    if ((res_int = FsMisc_CreateFileTree()) < 0)
                    {
                        snprintf(answer_string, sizeof(answer_string), "ERROR: Error %d while creating File Tree\r\n", res_int);
                        AtDebug_SendString(ptr_at_cmd_info->Port, ADL_AT_RSP, answer_string);
                        return;
                    }
                    break;


                default:
                    AtDebug_SendString("\r\nERROR: Bad Command\r\n");
                    return;
            }
            break;



        // Read command (AT+FS?)
        case AT_CMD_TYPE_READ:
            break;



        // Test command (AT+FS=?)
        case AT_CMD_TYPE_TEST:
            AtDebug_SendString("\r\n+FS=<mode>[,<par_1>][,<par_2>]\r\n");
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_FILE_LOG
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__FILE_LOG == 1)
static void AtDebug_Callback_AtCommand_FILE_LOG(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[1];

    ascii  answer_string[40 + 1];

    u8     num_of_records;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+FILE_LOG", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            num_of_records = atoi(ptr_arg_string[0]);

            /* verify parameters value */
            if (
                 (num_of_records == 0 ) ||
                 (num_of_records >  20)
               )
            {
                is_error = TRUE;
                break;
            }

            /* add the records in the file status log */
            Test_PutRecord_File_StatusLog(num_of_records);
            break;


        // Read command (AT+QUEUE?)
        case AT_CMD_TYPE_READ:
            is_error = TRUE;
            break;


        // Test command (AT+QUEUE=?)
        case AT_CMD_TYPE_TEST:
            is_error = TRUE;
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_SET_APN
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_SET_APN == 1)
static void AtDebug_Callback_AtCommand_DOTA_SET_APN(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    static DOTA_GPRS__GPRS_APN_PARAMETERS gprs_apn_parameters;

    static ascii                          answer_string[30 + 40 + 1];

           ascii                         *ptr_arg_string[3];

           ascii                         *ptr_apn_server;
           ascii                         *ptr_apn_user_name;
           ascii                         *ptr_apn_password;

           bool                           is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_SET_APN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 3)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            ptr_apn_server    = ptr_arg_string[0];
            ptr_apn_user_name = ptr_arg_string[1];
            ptr_apn_password  = ptr_arg_string[2];

            /* verify the max length of the parameter strings received form UART */
            if (
                 ((strlen(ptr_apn_server   ) > 40)) ||
                 ((strlen(ptr_apn_user_name) > 40)) ||
                 ((strlen(ptr_apn_password ) > 40))
               )
            {
                is_error = TRUE;
                break;
            }

            /* configure the APN GPRS parameters */
            strncpy(gprs_apn_parameters.apn_server   , ptr_apn_server   , DOTA_GPRS__MAX_LENGTH_GPRS_APN_SERVER   );
            strncpy(gprs_apn_parameters.apn_user_name, ptr_apn_user_name, DOTA_GPRS__MAX_LENGTH_GPRS_APN_USER_NAME);
            strncpy(gprs_apn_parameters.apn_password , ptr_apn_password , DOTA_GPRS__MAX_LENGTH_GPRS_APN_PASSWORD );
            gprs_apn_parameters.apn_server   [DOTA_GPRS__MAX_LENGTH_GPRS_APN_SERVER   ] = 0x00;
            gprs_apn_parameters.apn_user_name[DOTA_GPRS__MAX_LENGTH_GPRS_APN_USER_NAME] = 0x00;
            gprs_apn_parameters.apn_password [DOTA_GPRS__MAX_LENGTH_GPRS_APN_PASSWORD ] = 0x00;
            DotaGprs_Config_ApnParameters_Set(&gprs_apn_parameters);

            /* request the backup to flash objects */
            DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA, DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS);
            break;


        // Read command (AT+DOTA_SET_APN?)
        case AT_CMD_TYPE_READ:
            /* get the APN parameters */
            DotaGprs_Config_ApnParameters_Get(&gprs_apn_parameters);

            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_APN: SERVER    - \"%s\"\r\n", gprs_apn_parameters.apn_server   );
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_APN: USER NAME - \"%s\"\r\n", gprs_apn_parameters.apn_user_name);
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_APN: PASSWORD  - \"%s\"\r\n", gprs_apn_parameters.apn_password );
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+DOTA_SET_APN=?)
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_SET_DNS
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_SET_DNS == 1)
static void AtDebug_Callback_AtCommand_DOTA_SET_DNS(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    static DOTA_GPRS__GPRS_DNS_PARAMETERS gprs_dns_parameters;

    static ascii                          answer_string[30 + 20 + 1];

           ascii                          *ptr_arg_string[2];

           ascii                          *ptr_dns1;
           ascii                          *ptr_dns2;

           bool                            is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_SET_DNS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 2)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            ptr_dns1 = ptr_arg_string[0];
            ptr_dns2 = ptr_arg_string[1];

            /* verify the max length of the parameter strings received form UART */
            if (
                 ((strlen(ptr_dns1) > 20)) ||
                 ((strlen(ptr_dns2) > 20))
               )
            {
                is_error = TRUE;
                break;
            }

            /* configure the DNS parameters */
            strncpy(gprs_dns_parameters.dns_1, ptr_dns1, DOTA_GPRS__MAX_LENGTH_GPRS_DNS_1);
            strncpy(gprs_dns_parameters.dns_2, ptr_dns2, DOTA_GPRS__MAX_LENGTH_GPRS_DNS_2);
            gprs_dns_parameters.dns_1[DOTA_GPRS__MAX_LENGTH_GPRS_DNS_1] = 0x00;
            gprs_dns_parameters.dns_2[DOTA_GPRS__MAX_LENGTH_GPRS_DNS_2] = 0x00;
            DotaGprs_Config_DnsParameters_Set(&gprs_dns_parameters);

            /* request the backup to flash objects */
            DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA, DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS);
            break;


        // Read command (AT+DOTA_SET_DNS?)
        case AT_CMD_TYPE_READ:
            /* get the DNS parameters */
            DotaGprs_Config_DnsParameters_Get(&gprs_dns_parameters);

            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_DNS: DNS 1 - \"%s\"\r\n", gprs_dns_parameters.dns_1);
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_DNS: DNS 2 - \"%s\"\r\n", gprs_dns_parameters.dns_2);
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+DOTA_SET_DNS=?)
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_SET_FTP
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_SET_FTP == 1)
static void AtDebug_Callback_AtCommand_DOTA_SET_FTP(AT_COMMAND_INFO *ptr_at_cmd_info)
{
           ascii                   *ptr_arg_string[7];

    static ascii                    answer_string[30 + 40 + 1];

    static DOTA_FTP__FTP_PARAMETERS ftp_parameters;

           u8                       ftp_mode;
           u16                      ftp_port;

           ascii                   *ptr_ftp_server;
           ascii                   *ptr_ftp_user_name;
           ascii                   *ptr_ftp_password;
           ascii                   *ptr_ftp_file_path;
           ascii                   *ptr_ftp_file_name;

           bool                     is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_SET_FTP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 7)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            ftp_mode          = atoi(ptr_arg_string[0]);
            ftp_port          = atoi(ptr_arg_string[1]);
            ptr_ftp_server    =      ptr_arg_string[2];
            ptr_ftp_user_name =      ptr_arg_string[3];
            ptr_ftp_password  =      ptr_arg_string[4];
            ptr_ftp_file_path =      ptr_arg_string[5];
            ptr_ftp_file_name =      ptr_arg_string[6];

            /* verify <ftp_mode> parameter */
            if (
                 (ftp_mode != 0) &&
                 (ftp_mode != 1)
               )
            {
                is_error = TRUE;
                break;
            }

            /* verify the max length of the parameter strings received form UART */
            if (
                 ((strlen(ptr_ftp_server   ) > 40)) ||
                 ((strlen(ptr_ftp_user_name) > 40)) ||
                 ((strlen(ptr_ftp_password ) > 40)) ||
                 ((strlen(ptr_ftp_file_path) > 40)) ||
                 ((strlen(ptr_ftp_file_name) > 40))
               )
            {
                is_error = TRUE;
                break;
            }

            /* configure the FTP parameters */
            ftp_parameters.ftp_mode = ftp_mode;
            ftp_parameters.ftp_port = ftp_port;
            strncpy(ftp_parameters.ftp_server   , ptr_ftp_server   , DOTA_FTP__MAX_LENGTH_FTP_SERVER   );
            strncpy(ftp_parameters.ftp_user_name, ptr_ftp_user_name, DOTA_FTP__MAX_LENGTH_FTP_USER_NAME);
            strncpy(ftp_parameters.ftp_password , ptr_ftp_password , DOTA_FTP__MAX_LENGTH_FTP_PASSWORD );
            strncpy(ftp_parameters.ftp_file_path, ptr_ftp_file_path, DOTA_FTP__MAX_LENGTH_FTP_FILE_PATH);
            strncpy(ftp_parameters.ftp_file_name, ptr_ftp_file_name, DOTA_FTP__MAX_LENGTH_FTP_FILE_NAME);
            ftp_parameters.ftp_server   [DOTA_FTP__MAX_LENGTH_FTP_SERVER   ] = 0x00;
            ftp_parameters.ftp_user_name[DOTA_FTP__MAX_LENGTH_FTP_USER_NAME] = 0x00;
            ftp_parameters.ftp_password [DOTA_FTP__MAX_LENGTH_FTP_PASSWORD ] = 0x00;
            ftp_parameters.ftp_file_path[DOTA_FTP__MAX_LENGTH_FTP_FILE_PATH] = 0x00;
            ftp_parameters.ftp_file_name[DOTA_FTP__MAX_LENGTH_FTP_FILE_NAME] = 0x00;
            DotaFtp_Config_FtpParameters_Set(&ftp_parameters);

            /* request the backup to flash objects */
            DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA, DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS);
            break;


        // Read command (AT+DOTA_SET_FTP?)
        case AT_CMD_TYPE_READ:
            /* get the FTP parameters */
            DotaFtp_Config_FtpParameters_Get(&ftp_parameters);

            if (ftp_parameters.ftp_mode == DOTA_FTP__FTP_MODE_PASSIVE)
                AtDebug_SendString("\r\n+DOTA_SET_FTP: MODE      - PASSIVE\r\n");
            else
                AtDebug_SendString("\r\n+DOTA_SET_FTP: MODE      - ACTIVE\r\n" );

            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_FTP: PORT      - %d\r\n"    , ftp_parameters.ftp_port     );
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_FTP: SERVER    - \"%s\"\r\n", ftp_parameters.ftp_server   );
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_FTP: USER NAME - \"%s\"\r\n", ftp_parameters.ftp_user_name);
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_FTP: PASSWORD  - \"%s\"\r\n", ftp_parameters.ftp_password );
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_FTP: FILE PATH - \"%s\"\r\n", ftp_parameters.ftp_file_path);
            AtDebug_SendString(answer_string);
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_SET_FTP: FILE NAME - \"%s\"\r\n", ftp_parameters.ftp_file_name);
            AtDebug_SendString(answer_string);
            break;


        // Test command (AT+DOTA_SET_FTP=?)
        case AT_CMD_TYPE_TEST:
            break;


        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_AUTO
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_AUTO == 1)
static void AtDebug_Callback_AtCommand_DOTA_AUTO(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[1];

    ascii  answer_string[20 + 1];

    u8     mode;
    bool   auto_mode_status;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_AUTO", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <mode> parameter */
            mode = atoi(ptr_arg_string[0]);

            /* <mode> parameter */
            switch (mode)
            {
                // <mode> = 0
                case 0:
                    /* disable auto mode */
                    DotaMain_DisableAutoMode();
                    break;

                // <mode> = 1
                case 1:
                    /* enable  auto mode */
                    DotaMain_EnableAutoMode();
                    break;

                // <mode> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+DOTA_AUTO?)
        case AT_CMD_TYPE_READ:
            if (DotaMain_GetAutoModeStatus())
                auto_mode_status = 1;
            else
                auto_mode_status = 0;
            snprintf(answer_string, sizeof(answer_string), "\r\n+DOTA_AUTO: %d\r\n", auto_mode_status);
            AtDebug_SendString(answer_string);
            break;



        // Test command (AT+DOTA_AUTO=?)
        case AT_CMD_TYPE_TEST:
            AtDebug_SendString("\r\n+DOTA_AUTO: (0-1)\r\n");
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_DO_AnD
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_DO_AnD == 1)
static void AtDebug_Callback_AtCommand_DOTA_DO_AnD(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[1];

    u8     command;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_DO_AnD", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <command> parameter */
            command = atoi(ptr_arg_string[0]);

            /* <command> parameter */
            switch (command)
            {
                // <commmand> = 1
                case 1:
                    DotaAnD_AnDInstall();
                    break;


                // <command> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+DOTA_DO_AnD?)
        case AT_CMD_TYPE_READ:
            break;



        // Test command (AT+DOTA_DO_AnD=?)
        case AT_CMD_TYPE_TEST:
            AtDebug_SendString("\r\n+DOTA_DO_AnD: (1)\r\n");
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_DO_GPRS
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_DO_GPRS == 1)
static void AtDebug_Callback_AtCommand_DOTA_DO_GPRS(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[1];

    u8     command;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_DO_GPRS", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <command> parameter */
            command = atoi(ptr_arg_string[0]);

            /* <command> parameter */
            switch (command)
            {
                // <commmand> = 0
                case 0:
                    /* stop  the GPRS connection */
                    DotaGprs_GprsStopConnection();
                    break;

                // <commmand> = 1
                case 1:
                    /* start the GPRS connection */
                    DotaGprs_GprsStartConnection();
                    break;

                // <command> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+DOTA_DO_GPRS?)
        case AT_CMD_TYPE_READ:
            break;



        // Test command (AT+DOTA_DO_GPRS=?)
        case AT_CMD_TYPE_TEST:
            AtDebug_SendString("\r\n+DOTA_DO_GPRS: (0-1)\r\n");
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_DOTA_DO_FTP
 *
 * Description: callback function for
 * Input      : - params:
 * Output     : -
 *=============================================================================*/
#if (AT_DEBUG_STATUS__DOTA_DO_FTP == 1)
static void AtDebug_Callback_AtCommand_DOTA_DO_FTP(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    ascii *ptr_arg_string[1];

    u8     command;

    bool   is_error;


    /* debug info */
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_AT_DEBUG, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SPECIFIC COMMAND - AT+DOTA_DO_FTP", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    is_error = FALSE;


    switch (ptr_at_cmd_info->type)
    {
        // Action command
        case AT_CMD_TYPE_ARGS:
            /* check the number of the parameters */
            if (ptr_at_cmd_info->num_of_args != 1)
            {
                is_error = TRUE;
                break;
            }

            /* extract the parameters */
            AtDebug_ExtractArguments(ptr_at_cmd_info, ptr_arg_string);

            /* <command> parameter */
            command = atoi(ptr_arg_string[0]);

            /* <command> parameter */
            switch (command)
            {
                // <commmand> = 0
                case 0:
                    /* stop  the FTP connection */
                    DotaFtp_FtpStopConnection();
                    break;


                // <commmand> = 1
                case 1:
                    /* start the FTP connection */
                    DotaFtp_FtpStartConnection();
                    break;


                // <command> = other value
                default:
                    is_error = TRUE;
                    break;
            }
            break;



        // Read command (AT+DOTA_DO_FTP?)
        case AT_CMD_TYPE_READ:
            break;



        // Test command (AT+DOTA_DO_FTP=?)
        case AT_CMD_TYPE_TEST:
            AtDebug_SendString("\r\n+DOTA_DO_FTP: (0-1)\r\n");
            break;



        // Other command type
        default:
            is_error = TRUE;
            break;
    }


    AtDebug_SendFinalOkErrorAnswer(is_error);
}
#endif




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_AT
 *
 * Description: callback function for AT command "AT"
 * Input      : - ptr_at_cmd_info: pointer to AT command info (not used)
 * Output     : -
 *=============================================================================*/
static void AtDebug_Callback_AtCommand_AT(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    // OK
    AtDebug_SendString((ascii *)AtDebug_Answer_OK);
}




/*=============================================================================
 * Function   : AtDebug_Callback_AtCommand_Unknown
 *
 * Description: callback function for an unknown command
 * Input      : - ptr_at_cmd_info: pointer to AT command info (not used)
 * Output     : -
 *=============================================================================*/
static void AtDebug_Callback_AtCommand_Unknown(AT_COMMAND_INFO *ptr_at_cmd_info)
{
    // ERROR
    AtDebug_SendString((ascii *)AtDebug_Answer_Error);
}
