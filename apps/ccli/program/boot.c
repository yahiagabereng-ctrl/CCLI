/*=============================================================================
 * File       :  BOOT.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  BOOT - boot (command line, cooperative task table)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "boot.h"

#include "alarm.h"
#include "f_pf2.h"
#include "fw_config.h"
#include "main.h"
#include "parser_modbus.h"
#include "program_flash.h"
#include "regulation.h"
#include "synchronize.h"
#include "transmission_104.h"
#include "transmission_goose.h"
#include "transmission_mms.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* command line options */
static COMMANDS__OPTIONS Boot_Options;

/*
 * Cooperative task table, one pass per supervisor tick (FW_CONFIG__MAIN__TICK_MS).
 * Order is behaviour: plant data -> PF2 -> publish -> DSO / operator commands ->
 * interlock -> monitors -> status. Keep it identical to the lab-validated loop.
 */
static const BOOT__TASK_DEF Boot_TaskDef[] =
{
    { "ParserModbus"       , 1                                  , ParserModbus_Task           },
    { "FPf2"               , 1                                  , FPf2_Task                   },
    { "ParserModbusLink"   , 1                                  , ParserModbus_LinkTask       },
    { "AlarmMeterQuality"  , 1                                  , Alarm_MeterQualityTask      },
    { "TransmissionMmsPub" , 1                                  , TransmissionMms_PublishTask },
    { "Transmission104Pub" , 1                                  , Transmission104_PublishTask },
    { "Synchronize"        , 1                                  , Synchronize_Task            },
    { "Transmission104"    , 1                                  , Transmission104_Task        },
    { "TransmissionMms"    , 1                                  , TransmissionMms_Task        },
    { "Transmission104Evt" , 1                                  , Transmission104_EventTask   },
    { "Regulation"         , 1                                  , Regulation_Task             },
    { "TransmissionGoose"  , FW_CONFIG__MAIN__GOOSE_PERIOD_TICKS , TransmissionGoose_LinkTask  },
    { "MainStatus"         , FW_CONFIG__MAIN__STATUS_PERIOD_TICKS, Main_StatusTask             },
    { "ParserModbusTrace"  , 1                                  , ParserModbus_TraceTask      },
    { "ProgramFlash"       , 1                                  , ProgramFlash_Task           },
    { "MainWatchdog"       , 1                                  , Main_WatchdogTask           },
};

#define BOOT__NUM_TASKS    (sizeof(Boot_TaskDef) / sizeof(Boot_TaskDef[0]))




/*=============================================================================
 * Function   : Boot_TaskBoot
 *
 * Description: parse the command line and run one-shot modes
 * Input      : argc, argv
 * Output     : ptr_exit_code - exit code; FALSE when the process must exit
 *=============================================================================*/
bool Boot_TaskBoot(s32 argc, ascii **argv, s32 *ptr_exit_code)
{
    Commands_Parse(argc, argv, &Boot_Options);

    if (Commands_RunOneShot(&Boot_Options, ptr_exit_code))
    {
        return false;
    }
    return true;
}




/*=============================================================================
 * Function   : Boot_GetOptions
 *
 * Description: command line options
 * Input      : -
 * Output     : options
 *=============================================================================*/
const COMMANDS__OPTIONS *Boot_GetOptions(void)
{
    return &Boot_Options;
}




/*=============================================================================
 * Function   : Boot_NumTasks / Boot_GetTaskDef
 *
 * Description: cooperative task table
 * Input      : index - task index
 * Output     : number of tasks / task definition (NULL when out of range)
 *=============================================================================*/
u32 Boot_NumTasks(void)
{
    return (u32)BOOT__NUM_TASKS;
}

const BOOT__TASK_DEF *Boot_GetTaskDef(u32 index)
{
    if (index >= BOOT__NUM_TASKS)
    {
        return 0;
    }
    return &Boot_TaskDef[index];
}
