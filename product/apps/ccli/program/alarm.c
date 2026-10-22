/*=============================================================================
 * File       :  ALARM.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - alarm (O.14 event log entry point, meter quality)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "alarm.h"

#include "ccli_bridge.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* last meter data quality (CCLI_BRIDGE__QUALITY__*) */
static u8 Alarm_LastMeterQuality = CCLI_BRIDGE__QUALITY__INVALID;




/*=============================================================================
 * Function   : Alarm_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Alarm_Init(void)
{
    Alarm_LastMeterQuality = CCLI_BRIDGE__QUALITY__INVALID;
}




/*=============================================================================
 * Function   : Alarm_Log
 *
 * Description: append one O.14 event
 * Input      : type   - O.14 source
 *              detail - free text
 * Output     : -
 *=============================================================================*/
void Alarm_Log(const ascii *type, const ascii *detail)
{
    CcliBridge_Event_Append(type, detail);
}




/*=============================================================================
 * Function   : Alarm_MeterQualityTask
 *
 * Description: log meter data quality transitions
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Alarm_MeterQualityTask(u32 tick)
{
    CCLI_BRIDGE__MEASUREMENT measurement;

    (void)tick;

    CcliBridge_Measurement_Get(&measurement);
    if (measurement.quality == Alarm_LastMeterQuality)
    {
        return;
    }

    if (measurement.quality == CCLI_BRIDGE__QUALITY__GOOD)
    {
        Alarm_Log("meter", (Alarm_LastMeterQuality == CCLI_BRIDGE__QUALITY__STALE) ? "quality_recovered"
                                                                                  : "quality_good");
    }
    else if (measurement.quality == CCLI_BRIDGE__QUALITY__STALE)
    {
        Alarm_Log("meter", "quality_stale");
    }
    else
    {
        Alarm_Log("meter", "quality_invalid");
    }
    Alarm_LastMeterQuality = measurement.quality;
}
