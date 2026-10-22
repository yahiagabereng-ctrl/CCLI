/*=============================================================================
 * File       :  TRANSMISSION_GOOSE.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - transmission IEC 61850 GOOSE (plant LAN subscriber)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "transmission_goose.h"

#include "alarm.h"
#include "ccli_bridge.h"
#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* link state */
#define TRANSMISSION_GOOSE__LINK_STATE__DOWN       (-1)
#define TRANSMISSION_GOOSE__LINK_STATE__UNKNOWN    ( 0)
#define TRANSMISSION_GOOSE__LINK_STATE__UP         ( 1)




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* link state */
static s32 TransmissionGoose_LinkState = TRANSMISSION_GOOSE__LINK_STATE__UNKNOWN;




/*=============================================================================
 * Function   : TransmissionGoose_Init
 *
 * Description: start the GOOSE subscriber when enabled in the yaml
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGoose_Init(void)
{
    TransmissionGoose_LinkState = TRANSMISSION_GOOSE__LINK_STATE__UNKNOWN;

    if (FwConfig_Get()->goose_enabled)
    {
        (void)CcliBridge_Goose_Start();
    }
    else
    {
        fprintf(stderr, "goose: disabled in config (plant Modbus path active)\n");
    }
}




/*=============================================================================
 * Function   : TransmissionGoose_Stop
 *
 * Description: stop the GOOSE subscriber
 * Input      : -
 * Output     : -
 *=============================================================================*/
void TransmissionGoose_Stop(void)
{
    CcliBridge_Goose_Stop();
}




/*=============================================================================
 * Function   : TransmissionGoose_LinkTask
 *
 * Description: link up / down on goose.timeout_s (first down after one timeout)
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void TransmissionGoose_LinkTask(u32 tick)
{
    const FW_CONFIG__RUNTIME *cfg = FwConfig_Get();
    s64                       now_ms;
    s64                       last_rx_ms;
    s64                       timeout_ms;
    bool                      heard;

    if (!cfg->goose_enabled || !CcliBridge_Goose_IsRunning() || (cfg->goose_timeout_s <= 0))
    {
        return;
    }

    now_ms     = CcliBridge_NowMs();
    last_rx_ms = CcliBridge_Goose_LastRxMs();
    timeout_ms = (s64)cfg->goose_timeout_s * 1000;
    heard      = (last_rx_ms > 0) && ((now_ms - last_rx_ms) <= timeout_ms);

    if (heard)
    {
        if (TransmissionGoose_LinkState <= TRANSMISSION_GOOSE__LINK_STATE__UNKNOWN)
        {
            Alarm_Log("goose", (TransmissionGoose_LinkState < TRANSMISSION_GOOSE__LINK_STATE__UNKNOWN) ? "link_recovered"
                                                                                                        : "link_up");
            TransmissionGoose_LinkState = TRANSMISSION_GOOSE__LINK_STATE__UP;
        }
    }
    else if ((TransmissionGoose_LinkState == TRANSMISSION_GOOSE__LINK_STATE__UP) ||
             ((TransmissionGoose_LinkState == TRANSMISSION_GOOSE__LINK_STATE__UNKNOWN) &&
              ((s64)tick * FW_CONFIG__MAIN__TICK_MS >= timeout_ms)))
    {
        Alarm_Log("goose", (last_rx_ms > 0) ? "link_down:timeout" : "link_down:no_rx");
        TransmissionGoose_LinkState = TRANSMISSION_GOOSE__LINK_STATE__DOWN;
    }
}
