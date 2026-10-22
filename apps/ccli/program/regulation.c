/*=============================================================================
 * File       :  REGULATION.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - regulation (PF2 command -> permissive / Annex M -> DO)
 *
 * Curtailment DO = PF2 command AND permissive DI AND NOT Annex M trip
 * (CCLI-VAL-001 s.7 "output policy permits actuation"; O.11 no conflicting
 * PF2 actuation while the Annex M teledistacco relay is active).
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "regulation.h"

#include "alarm.h"
#include "alarm_input.h"
#include "f_pf2.h"
#include "fw_config.h"
#include "outputs.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* interlock status */
static REGULATION__STATUS__INTERLOCK Regulation_Status;




/*=============================================================================
 * Function   : Regulation_Init
 *
 * Description: first interlock evaluation and DO drive (after first PF2 step)
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Regulation_Init(void)
{
    const FW_CONFIG__RUNTIME *cfg = FwConfig_Get();
    bool                      permissive = FALSE;
    bool                      annex_m_trip = FALSE;

    if (cfg->permissive_bypass)
    {
        permissive = TRUE;
    }
    else
    {
        (void)Outputs_ReadPermissive(&permissive);
    }

    if (cfg->annex_m_trip_enabled)
    {
        (void)Outputs_ReadAnnexMTripActive(&annex_m_trip);
    }

    Regulation_Status.curtailment_commanded = FPf2_CurtailmentActive();
    Regulation_Status.permissive_ok         = permissive;
    Regulation_Status.annex_m_trip_active   = annex_m_trip;
    Regulation_Status.do_curtail_on         = Outputs_GateCurtailment(Regulation_Status.curtailment_commanded,
                                                                      permissive, annex_m_trip);

    (void)Outputs_ApplyCurtailment(Regulation_Status.do_curtail_on);
    Alarm_Log("do", Regulation_Status.do_curtail_on ? "curtail_on" : "curtail_off");

    AlarmInput_Init(annex_m_trip);
}




/*=============================================================================
 * Function   : Regulation_Task
 *
 * Description: interlock evaluation and DO drive
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Regulation_Task(u32 tick)
{
    const FW_CONFIG__RUNTIME *cfg = FwConfig_Get();
    bool                      commanded;
    bool                      permissive = FALSE;
    bool                      permissive_ok;
    bool                      permissive_di = FALSE;
    bool                      annex_m_trip = FALSE;
    bool                      do_on;

    (void)tick;

    commanded     = FPf2_CurtailmentActive();
    permissive_ok = cfg->permissive_bypass || (Outputs_ReadPermissive(&permissive) && permissive);

    if (cfg->annex_m_trip_enabled)
    {
        (void)Outputs_ReadAnnexMTripActive(&annex_m_trip);
        AlarmInput_TrackAnnexM(annex_m_trip);
    }

    if (!cfg->permissive_bypass)
    {
        if (Outputs_ReadPermissive(&permissive_di))
        {
            AlarmInput_TrackPermissive(permissive_di);
        }
    }

    do_on = Outputs_GateCurtailment(commanded, permissive_ok, annex_m_trip);
    if (do_on != Regulation_Status.do_curtail_on)
    {
        const char *detail = Outputs_CurtailDetail(do_on, commanded, annex_m_trip);

        (void)Outputs_ApplyCurtailment(do_on);
        Alarm_Log("do", detail);
        fprintf(stderr, "io: DO %s\n", detail);
        if (annex_m_trip && commanded)
        {
            Alarm_Log("annex_m", "teledistacco_inhibit");
        }
        Regulation_Status.do_curtail_on = do_on;
    }

    Regulation_Status.curtailment_commanded = commanded;
    Regulation_Status.permissive_ok         = permissive_ok;
    Regulation_Status.annex_m_trip_active   = annex_m_trip;
}




/*=============================================================================
 * Function   : Regulation_Status_Get
 *
 * Description: get the interlock status
 * Input      : -
 * Output     : ptr_data - status
 *=============================================================================*/
void Regulation_Status_Get(REGULATION__STATUS__INTERLOCK *ptr_data)
{
    *ptr_data = Regulation_Status;
}




/*=============================================================================
 * Function   : Regulation_UpdateVarCrc / Regulation_VerifyVarCrc
 *
 * Description: status variables CRC (retained state across restart)
 *              TODO(ccli-product-sw-freeze): back with program_flash
 * Input      : -
 * Output     : VerifyVarCrc - TRUE when retained state is valid
 *=============================================================================*/
void Regulation_UpdateVarCrc(void)
{
}

bool Regulation_VerifyVarCrc(void)
{
    return true;
}
