/*=============================================================================
 * File       :  COMMANDS.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - commands (command line / commissioning one-shot modes)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* user     includes */
#include "commands.h"

#include "ccli_bridge.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* command handler: returns TRUE to stop parsing (immediate commands) */
typedef bool (*COMMANDS__HANDLER)(COMMANDS__OPTIONS *ptr_options, const ascii *arg);

/* command definition */
typedef struct
{
    const ascii       *string_name;
    bool               has_arg;
    COMMANDS__HANDLER  cmd_handler;
} COMMANDS__DEF;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
static bool Commands_ParseInt(const ascii *arg, s32 *ptr_value);

static bool Commands_Callback_Version         (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_VersionJson     (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_Help            (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_RegulationCheck (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_Live            (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_Json            (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_LiveWatch       (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_GpioTest        (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_LabDemo         (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_GpioBlink       (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_BenchStatus     (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_BenchWriteConfig(COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_EventDump       (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_EventWrapTest   (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_EventClear      (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_Count           (COMMANDS__OPTIONS *ptr_options, const ascii *arg);
static bool Commands_Callback_Config          (COMMANDS__OPTIONS *ptr_options, const ascii *arg);




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* command definitions (order MUST match COMMANDS__CMD) */
static const COMMANDS__DEF Commands_CommandsDef[COMMANDS__NUM_COMMANDS] =
{
    { "--version"           , false, Commands_Callback_Version          },
    { "--version-json"      , false, Commands_Callback_VersionJson      },
    { "--help"              , false, Commands_Callback_Help             },
    { "-h"                  , false, Commands_Callback_Help             },
    { "--regulation-check"  , false, Commands_Callback_RegulationCheck  },
    { "--live"              , false, Commands_Callback_Live             },
    { "--json"              , false, Commands_Callback_Json             },
    { "--live-watch"        , true , Commands_Callback_LiveWatch        },
    { "--gpio-test"         , false, Commands_Callback_GpioTest         },
    { "--lab-demo"          , false, Commands_Callback_LabDemo          },
    { "--gpio-blink"        , true , Commands_Callback_GpioBlink        },
    { "--bench-status"      , false, Commands_Callback_BenchStatus      },
    { "--bench-write-config", true , Commands_Callback_BenchWriteConfig },
    { "--event-dump"        , false, Commands_Callback_EventDump        },
    { "--event-wrap-test"   , false, Commands_Callback_EventWrapTest    },
    { "--event-clear"       , false, Commands_Callback_EventClear       },
    { "--count"             , true , Commands_Callback_Count            },
    { "--config"            , true , Commands_Callback_Config           },
};




/*=============================================================================
 * Function   : Commands_Parse
 *
 * Description: parse the command line (unknown arguments are ignored; an
 *              argument-taking command without its argument is ignored)
 * Input      : argc, argv
 * Output     : ptr_options - options
 *=============================================================================*/
void Commands_Parse(s32 argc, ascii **argv, COMMANDS__OPTIONS *ptr_options)
{
    memset(ptr_options, 0, sizeof(*ptr_options));
    snprintf(ptr_options->config_path, sizeof(ptr_options->config_path), "%s", FW_CONFIG__DEFAULT_CONFIG_PATH);
    ptr_options->event_dump_count = CcliBridge_EventMaxCount();

    for (s32 i = 1; i < argc; ++i)
    {
        for (s32 c = 0; c < COMMANDS__NUM_COMMANDS; ++c)
        {
            const COMMANDS__DEF *def = &Commands_CommandsDef[c];
            const ascii         *arg = 0;

            if (strcmp(argv[i], def->string_name) != 0)
            {
                continue;
            }
            if (def->has_arg)
            {
                if ((i + 1) >= argc)
                {
                    break;
                }
                arg = argv[++i];
            }
            if (def->cmd_handler(ptr_options, arg))
            {
                return;
            }
            break;
        }
    }
}




/*=============================================================================
 * Function   : Commands_RunOneShot
 *
 * Description: run a one-shot mode (precedence: version/help, event-clear,
 *              event-wrap-test, event-dump, bench-status, bench-write-config,
 *              regulation-check, gpio-test)
 * Input      : ptr_options   - options
 * Output     : ptr_exit_code - process exit code; FALSE when the daemon must start
 *=============================================================================*/
bool Commands_RunOneShot(const COMMANDS__OPTIONS *ptr_options, s32 *ptr_exit_code)
{
    *ptr_exit_code = 0;

    if (ptr_options->version_text)
    {
        CcliBridge_PrintVersionText();
        return true;
    }
    if (ptr_options->version_json)
    {
        CcliBridge_PrintVersionJson();
        return true;
    }
    if (ptr_options->help)
    {
        CcliBridge_PrintUsage();
        return true;
    }
    if (ptr_options->event_clear)
    {
        *ptr_exit_code = CcliBridge_EventClear();
        return true;
    }
    if (ptr_options->event_wrap_test)
    {
        *ptr_exit_code = CcliBridge_EventWrapTest();
        return true;
    }
    if (ptr_options->event_dump)
    {
        *ptr_exit_code = CcliBridge_EventDump(ptr_options->config_path, ptr_options->event_dump_count,
                                              ptr_options->json_out);
        return true;
    }
    if (ptr_options->bench_status)
    {
        *ptr_exit_code = CcliBridge_BenchStatus();
        return true;
    }
    if (ptr_options->bench_write_config)
    {
        *ptr_exit_code = CcliBridge_BenchWriteConfig(ptr_options->config_path);
        return true;
    }
    if (ptr_options->regulation_check)
    {
        *ptr_exit_code = CcliBridge_RegulationCheck(ptr_options->config_path, ptr_options->regulation_live,
                                                    ptr_options->json_out, ptr_options->live_watch_s);
        return true;
    }
    if (ptr_options->gpio_test)
    {
        *ptr_exit_code = CcliBridge_GpioTest();
        return true;
    }
    return false;
}




/*=============================================================================
 * Function   : Commands_ParseInt
 *
 * Description: parse a leading decimal integer (trailing text ignored)
 * Input      : arg       - text
 * Output     : ptr_value - value; FALSE when no digits or out of range
 *=============================================================================*/
static bool Commands_ParseInt(const ascii *arg, s32 *ptr_value)
{
    ascii *end = 0;
    long   value;

    errno = 0;
    value = strtol(arg, &end, 10);
    if ((end == arg) || (errno == ERANGE) || (value > INT_MAX) || (value < INT_MIN))
    {
        return false;
    }
    *ptr_value = (s32)value;
    return true;
}




/*=============================================================================
 * Command callbacks
 *=============================================================================*/
static bool Commands_Callback_Version(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->version_text = true;
    return true;
}

static bool Commands_Callback_VersionJson(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->version_json = true;
    return true;
}

static bool Commands_Callback_Help(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->help = true;
    return true;
}

static bool Commands_Callback_RegulationCheck(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->regulation_check = true;
    return false;
}

static bool Commands_Callback_Live(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->regulation_live = true;
    return false;
}

static bool Commands_Callback_Json(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->json_out = true;
    return false;
}

static bool Commands_Callback_LiveWatch(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    s32 value = 0;

    if (!Commands_ParseInt(arg, &value) || (value < 0))
    {
        value = 0;
    }
    ptr_options->live_watch_s = value;
    return false;
}

static bool Commands_Callback_GpioTest(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->gpio_test = true;
    return false;
}

static bool Commands_Callback_LabDemo(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->lab_demo = true;
    return false;
}

static bool Commands_Callback_GpioBlink(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    s32 value = 0;

    if (!Commands_ParseInt(arg, &value))
    {
        fprintf(stderr, "ccli: invalid --gpio-blink value, ignoring\n");
        value = 0;
    }
    ptr_options->gpio_blink = (value < 0) ? 0 : value;
    return false;
}

static bool Commands_Callback_BenchStatus(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->bench_status = true;
    return false;
}

static bool Commands_Callback_BenchWriteConfig(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    ptr_options->bench_write_config = true;
    snprintf(ptr_options->config_path, sizeof(ptr_options->config_path), "%s", arg);
    return false;
}

static bool Commands_Callback_EventDump(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->event_dump = true;
    return false;
}

static bool Commands_Callback_EventWrapTest(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->event_wrap_test = true;
    return false;
}

static bool Commands_Callback_EventClear(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    (void)arg;
    ptr_options->event_clear = true;
    return false;
}

static bool Commands_Callback_Count(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    ascii         *end = 0;
    unsigned long  value;

    errno = 0;
    value = strtoul(arg, &end, 10);
    if ((end == arg) || (errno == ERANGE) || (value > UINT32_MAX))
    {
        ptr_options->event_dump_count = CcliBridge_EventMaxCount();
    }
    else
    {
        ptr_options->event_dump_count = (u32)value;
    }
    return false;
}

static bool Commands_Callback_Config(COMMANDS__OPTIONS *ptr_options, const ascii *arg)
{
    snprintf(ptr_options->config_path, sizeof(ptr_options->config_path), "%s", arg);
    return false;
}
