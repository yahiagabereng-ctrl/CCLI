/*=============================================================================
 * File       :  MAIN.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - main (supervisor loop, messages, watchdog, status)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "main.h"

#include "alarm.h"
#include "boot.h"
#include "ccli_bridge.h"
#include "fw_config.h"
#include "outputs.h"
#include "parser_modbus.h"
#include "queue_my.h"
#include "regulation.h"
#include "startup.h"
#include "transmission_104.h"
#include "transmission_goose.h"
#include "transmission_mms.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
static void Main_ProcessMessage(u32 msg_identifier);




/*=============================================================================
 * Function   : main
 *
 * Description: process entry point (Boot -> Startup -> Main)
 * Input      : argc, argv
 * Output     : process exit code
 *=============================================================================*/
int main(int argc, char **argv)
{
    s32 exit_code = 0;

    if (!Boot_TaskBoot(argc, argv, &exit_code))
    {
        return exit_code;
    }

    if (!Startup_TaskStartup(Boot_GetOptions()))
    {
        return 1;
    }

    Main_TaskMain();
    Main_ShutdownModule();
    return 0;
}




/*=============================================================================
 * Function   : Main_TaskMain
 *
 * Description: cooperative supervisor loop (one Boot task table pass per tick)
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_TaskMain(void)
{
    u32 tick = 0;
    u32 msg_identifier;

    while (CcliBridge_IsRunning())
    {
        for (u32 i = 0; i < Boot_NumTasks(); ++i)
        {
            const BOOT__TASK_DEF *task = Boot_GetTaskDef(i);

            if ((tick % task->period_ticks) == 0)
            {
                task->task_entry(tick);
            }
        }

        while (Queue_GetRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, &msg_identifier))
        {
            Main_ProcessMessage(msg_identifier);
        }

        ++tick;
        CcliBridge_SleepMs(FW_CONFIG__MAIN__TICK_MS);
    }
}




/*=============================================================================
 * Function   : Main_EndOfSynchronize
 *
 * Description: signal the end of synchronize (first GNSS fix)
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_EndOfSynchronize(void)
{
    if (!Queue_PutRecord(QUEUE__QUEUE_ID__MAIN_MESSAGE, MAIN__TASK_MSG_ID__END_OF_SYNCHRONIZE))
    {
        fprintf(stderr, "main: message queue full (END_OF_SYNCHRONIZE dropped)\n");
    }
}




/*=============================================================================
 * Function   : Main_StatusTask
 *
 * Description: iface watch, status line, bench status file
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Main_StatusTask(u32 tick)
{
    REGULATION__STATUS__INTERLOCK interlock;
    CCLI_BRIDGE__STATUS           status;

    (void)tick;

    CcliBridge_Iface_WatchTask();

    Regulation_Status_Get(&interlock);
    Outputs_LogStatus(interlock.do_curtail_on, interlock.permissive_ok, interlock.annex_m_trip_active);

    status.curtailment_commanded = interlock.curtailment_commanded;
    status.permissive_ok         = interlock.permissive_ok;
    status.annex_m_trip_active   = interlock.annex_m_trip_active;
    status.do_curtail_on         = interlock.do_curtail_on;
    status.modbus_link_state     = ParserModbus_GetLinkState();
    CcliBridge_Status_Write(&status);
}




/*=============================================================================
 * Function   : Main_WatchdogTask
 *
 * Description: procd watchdog ping
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Main_WatchdogTask(u32 tick)
{
    const u32 interval_ticks = CcliBridge_Supervision_WatchdogPingS() * (1000 / FW_CONFIG__MAIN__TICK_MS);

    if ((tick > 0) && ((tick % interval_ticks) == 0))
    {
        CcliBridge_Supervision_WatchdogPing();
    }
}




/*=============================================================================
 * Function   : Main_ShutdownModule
 *
 * Description: orderly shutdown (outputs safe, servers stopped)
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Main_ShutdownModule(void)
{
    Alarm_Log("security", "service_monitor:shutdown");
    Alarm_Log("system",   "power_off:shutdown");
    CcliBridge_Supervision_MarkCleanShutdown();
    Outputs_Shutdown();
    TransmissionGoose_Stop();
    Transmission104_Stop();
    TransmissionMms_Stop();

    CcliBridge_PrintStopped();
    CcliBridge_Runtime_Destroy();
}




/*=============================================================================
 * Function   : Main_ProcessMessage
 *
 * Description: process one task message
 * Input      : msg_identifier - MAIN__TASK_MSG_ID__*
 * Output     : -
 *=============================================================================*/
static void Main_ProcessMessage(u32 msg_identifier)
{
    switch (msg_identifier)
    {
        case MAIN__TASK_MSG_ID__END_OF_SYNCHRONIZE:
            fprintf(stderr, "synchronize: GNSS fix acquired (end of synchronize)\n");
            break;

        default:
            break;
    }
}
