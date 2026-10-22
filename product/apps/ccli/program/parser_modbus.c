/*=============================================================================
 * File       :  PARSER_MODBUS.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - Modbus master (plant meter poll, link state, trace)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <math.h>
#include <stdio.h>

/* user     includes */
#include "parser_modbus.h"

#include "alarm.h"
#include "ccli_bridge.h"
#include "fw_config.h"
#include "regulation.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* lab demo: triangle wave 400..950 kW over ~120 s (crosses the 900 kW PF2 threshold) */
#define PARSER_MODBUS__LAB_DEMO__P_MIN_KW     400.0
#define PARSER_MODBUS__LAB_DEMO__P_SPAN_KW    550.0
#define PARSER_MODBUS__LAB_DEMO__PERIOD_S     120

/* max length of a link event detail */
#define MAX_LENGTH_LINK_DETAIL                200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* link state */
static s32 ParserModbus_LinkState = PARSER_MODBUS__LINK_STATE__UNKNOWN;




/*=============================================================================
 * Function   : ParserModbus_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void ParserModbus_Init(void)
{
    ParserModbus_LinkState = PARSER_MODBUS__LINK_STATE__UNKNOWN;
}




/*=============================================================================
 * Function   : ParserModbus_Task
 *
 * Description: poll the plant meter (lab demo: simulated ramp first)
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void ParserModbus_Task(u32 tick)
{
    if (FwConfig_Get()->lab_demo)
    {
        const u32    t     = tick / (1000 / FW_CONFIG__MAIN__TICK_MS);
        const double phase = (double)(t % PARSER_MODBUS__LAB_DEMO__PERIOD_S);
        const double half  = PARSER_MODBUS__LAB_DEMO__PERIOD_S / 2.0;
        const double p_kw  = PARSER_MODBUS__LAB_DEMO__P_MIN_KW +
                             PARSER_MODBUS__LAB_DEMO__P_SPAN_KW * fabs(1.0 - fabs(phase / half - 1.0));

        CcliBridge_Modbus_SetSimulatedPowerKw(p_kw);
    }

    CcliBridge_Modbus_Tick();
}




/*=============================================================================
 * Function   : ParserModbus_LinkTask
 *
 * Description: log link up / down / recovered on the last exchange
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void ParserModbus_LinkTask(u32 tick)
{
    CCLI_BRIDGE__MODBUS_EXCHANGE exchange;
    ascii                        detail[MAX_LENGTH_LINK_DETAIL + 1];

    (void)tick;

    CcliBridge_Modbus_LastExchange(&exchange);
    if (!exchange.valid)
    {
        return;
    }

    if (exchange.success)
    {
        if (ParserModbus_LinkState <= PARSER_MODBUS__LINK_STATE__UNKNOWN)
        {
            Alarm_Log("modbus", (ParserModbus_LinkState < PARSER_MODBUS__LINK_STATE__UNKNOWN) ? "link_recovered"
                                                                                               : "link_up");
            ParserModbus_LinkState = PARSER_MODBUS__LINK_STATE__UP;
        }
    }
    else if (ParserModbus_LinkState >= PARSER_MODBUS__LINK_STATE__UNKNOWN)
    {
        snprintf(detail, sizeof(detail), "link_down:%s", exchange.error);
        Alarm_Log("modbus", detail);
        ParserModbus_LinkState = PARSER_MODBUS__LINK_STATE__DOWN;
    }
}




/*=============================================================================
 * Function   : ParserModbus_TraceTask
 *
 * Description: per-exchange trace on stderr when modbus.trace is set
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void ParserModbus_TraceTask(u32 tick)
{
    REGULATION__STATUS__INTERLOCK status;

    (void)tick;

    if (!FwConfig_Get()->modbus_trace)
    {
        return;
    }
    Regulation_Status_Get(&status);
    CcliBridge_Modbus_Trace(status.curtailment_commanded, status.do_curtail_on);
}




/*=============================================================================
 * Function   : ParserModbus_GetLinkState
 *
 * Description: link state
 * Input      : -
 * Output     : PARSER_MODBUS__LINK_STATE__*
 *=============================================================================*/
s32 ParserModbus_GetLinkState(void)
{
    return ParserModbus_LinkState;
}
