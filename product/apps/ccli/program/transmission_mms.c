/*=============================================================================
 * File       :  TRANSMISSION_MMS.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - transmission IEC 61850 MMS (Eth_A, DSO, 62351 TLS)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "transmission_mms.h"

#include "alarm.h"
#include "ccli_bridge.h"
#include "fw_config.h"




/*=============================================================================
 * Function   : TransmissionMms_Init
 *
 * Description: start the MMS server when enabled in the yaml
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionMms_Init(void)
{
    if (FwConfig_Get()->mms_enabled)
    {
        (void)CcliBridge_Mms_Start();
    }
    else
    {
        fprintf(stderr, "mms: disabled in config\n");
    }
}




/*=============================================================================
 * Function   : TransmissionMms_Stop
 *
 * Description: stop the MMS server
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionMms_Stop(void)
{
    CcliBridge_Mms_Stop();
}




/*=============================================================================
 * Function   : TransmissionMms_PublishTask
 *
 * Description: publish TotW / TotVAr / PPV to the data model
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void TransmissionMms_PublishTask(u32 tick)
{
    CCLI_BRIDGE__MEASUREMENT measurement;

    (void)tick;

    CcliBridge_Measurement_Get(&measurement);
    CcliBridge_Mms_PublishMeasurement(measurement.p_kw, measurement.q_kvar, measurement.ppv_kv);
}




/*=============================================================================
 * Function   : TransmissionMms_Task
 *
 * Description: P3-05 comms-loss fallback, then DSO live commands -> PF2 / plant Q
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void TransmissionMms_Task(u32 tick)
{
    (void)tick;

    if (CcliBridge_Mms_PollCommsLossFallback())
    {
        fprintf(stderr, "mms\xE2\x86\x92pf2: P3-05 Operating Rule fallback (Eth_A no-comms)\n");
        Alarm_Log("mms", "comms_loss_fallback");
        Alarm_Log("priority", "autonomous_after_eth_a_loss");
    }

    CcliBridge_Mms_PollAndApplyDsoCommand();
}
