/*=============================================================================
 * File       :  COMMANDS.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - commands (command line / commissioning one-shot modes)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __COMMANDS_H__


#define __COMMANDS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"

#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* commands (order MUST match Commands_CommandsDef[]) */
typedef enum
{
    COMMANDS__CMD__VERSION = 0,
    COMMANDS__CMD__VERSION_JSON,
    COMMANDS__CMD__HELP,
    COMMANDS__CMD__HELP_SHORT,
    COMMANDS__CMD__REGULATION_CHECK,
    COMMANDS__CMD__LIVE,
    COMMANDS__CMD__JSON,
    COMMANDS__CMD__LIVE_WATCH,
    COMMANDS__CMD__GPIO_TEST,
    COMMANDS__CMD__LAB_DEMO,
    COMMANDS__CMD__GPIO_BLINK,
    COMMANDS__CMD__BENCH_STATUS,
    COMMANDS__CMD__BENCH_WRITE_CONFIG,
    COMMANDS__CMD__EVENT_DUMP,
    COMMANDS__CMD__EVENT_WRAP_TEST,
    COMMANDS__CMD__EVENT_CLEAR,
    COMMANDS__CMD__COUNT,
    COMMANDS__CMD__CONFIG,
    COMMANDS__NUM_COMMANDS
} COMMANDS__CMD;




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* command line options */
typedef struct
{
    ascii config_path[FW_CONFIG__MAX_LEN_CONFIG_PATH + 1];

    bool  version_text;               /* --version          (immediate) */
    bool  version_json;               /* --version-json     (immediate) */
    bool  help;                       /* --help / -h        (immediate) */

    bool  regulation_check;
    bool  regulation_live;
    bool  json_out;
    s32   live_watch_s;

    bool  gpio_test;
    bool  lab_demo;
    s32   gpio_blink;

    bool  bench_status;
    bool  bench_write_config;

    bool  event_dump;
    bool  event_wrap_test;
    bool  event_clear;
    u32   event_dump_count;
} COMMANDS__OPTIONS;




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* parse the command line (unknown arguments are ignored) */
void Commands_Parse(s32 argc, ascii **argv, COMMANDS__OPTIONS *ptr_options);

/* run a one-shot mode; FALSE when the daemon must start */
bool Commands_RunOneShot(const COMMANDS__OPTIONS *ptr_options, s32 *ptr_exit_code);

#ifdef __cplusplus
}
#endif




#endif
