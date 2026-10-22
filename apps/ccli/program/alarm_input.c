/*=============================================================================
 * File       :  ALARM_INPUT.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - alarm input (permissive DI, Annex M trip relay)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "alarm_input.h"

#include "alarm.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* last Annex M trip relay state */
static bool AlarmInput_LastAnnexMTrip     = FALSE;

/* last permissive DI state (valid once tracked) */
static bool AlarmInput_PermissiveTracked  = FALSE;
static bool AlarmInput_LastPermissiveDi   = FALSE;




/*=============================================================================
 * Function   : AlarmInput_Init
 *
 * Description: init with the boot-time Annex M trip state
 * Input      : annex_m_trip_active - trip relay state at boot
 * Output     : -
 *=============================================================================*/
void AlarmInput_Init(bool annex_m_trip_active)
{
    AlarmInput_LastAnnexMTrip    = annex_m_trip_active;
    AlarmInput_PermissiveTracked = FALSE;
    AlarmInput_LastPermissiveDi  = FALSE;
}




/*=============================================================================
 * Function   : AlarmInput_TrackAnnexM
 *
 * Description: log Annex M trip relay transitions (P6-03)
 * Input      : annex_m_trip_active - trip relay state
 * Output     : -
 *=============================================================================*/
void AlarmInput_TrackAnnexM(bool annex_m_trip_active)
{
    if (annex_m_trip_active == AlarmInput_LastAnnexMTrip)
    {
        return;
    }
    Alarm_Log("annex_m", annex_m_trip_active ? "trip_relay_active" : "trip_relay_cleared");
    AlarmInput_LastAnnexMTrip = annex_m_trip_active;
}




/*=============================================================================
 * Function   : AlarmInput_TrackPermissive
 *
 * Description: log permissive DI transitions (first read always logged)
 * Input      : permissive_di - DI state
 * Output     : -
 *=============================================================================*/
void AlarmInput_TrackPermissive(bool permissive_di)
{
    if (AlarmInput_PermissiveTracked && (permissive_di == AlarmInput_LastPermissiveDi))
    {
        return;
    }
    Alarm_Log("di", permissive_di ? "permissive_ok" : "permissive_blocked");
    AlarmInput_PermissiveTracked = TRUE;
    AlarmInput_LastPermissiveDi  = permissive_di;
}
