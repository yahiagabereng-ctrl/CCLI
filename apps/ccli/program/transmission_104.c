/*=============================================================================
 * File       :  TRANSMISSION_104.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - transmission IEC 60870-5-104 (Eth_B, operator)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "transmission_104.h"

#include "alarm.h"
#include "ccli_bridge.h"
#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* max length of an operator audit detail */
#define MAX_LENGTH_AUDIT_DETAIL    255




/*=============================================================================
 * Function   : Transmission104_Init
 *
 * Description: start the CS104 server when enabled in the yaml
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Transmission104_Init(void)
{
    if (FwConfig_Get()->iec104_enabled)
    {
        (void)CcliBridge_Iec104_Start();
    }
    else
    {
        fprintf(stderr, "iec104: disabled in config\n");
    }
}




/*=============================================================================
 * Function   : Transmission104_Stop
 *
 * Description: stop the CS104 server
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Transmission104_Stop(void)
{
    CcliBridge_Iec104_Stop();
}




/*=============================================================================
 * Function   : Transmission104_PublishTask
 *
 * Description: publish TotW
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Transmission104_PublishTask(u32 tick)
{
    CCLI_BRIDGE__MEASUREMENT measurement;

    (void)tick;

    CcliBridge_Measurement_Get(&measurement);
    CcliBridge_Iec104_UpdateTotWKw(measurement.p_kw);
}




/*=============================================================================
 * Function   : Transmission104_Task
 *
 * Description: CS104 server tick
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Transmission104_Task(u32 tick)
{
    (void)tick;

    CcliBridge_Iec104_Tick();
}




/*=============================================================================
 * Function   : Transmission104_EventTask
 *
 * Description: P4-04 comms-loss fallback and operator ASDU audit
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Transmission104_EventTask(u32 tick)
{
    ascii detail[MAX_LENGTH_AUDIT_DETAIL + 1];

    (void)tick;

    if (CcliBridge_Iec104_PollCommsLossFallback())
    {
        fprintf(stderr, "iec104: P4-04 Eth_B comms-loss fallback\n");
        Alarm_Log("iec104", "comms_loss_fallback");
    }

    if (CcliBridge_Iec104_PollOperatorAudit(detail, sizeof(detail)))
    {
        fprintf(stderr, "iec104: %s\n", detail);
        Alarm_Log("iec104", detail);
    }
}
