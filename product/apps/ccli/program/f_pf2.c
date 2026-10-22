/*=============================================================================
 * File       :  F_PF2.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - function PF2 (CEI 0-16 export limitation FSM)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "f_pf2.h"

#include "ccli_bridge.h"




/*=============================================================================
 * Function   : FPf2_Task
 *
 * Description: one PF2 FSM step on the latest measurement (core/pf2)
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void FPf2_Task(u32 tick)
{
    (void)tick;

    CcliBridge_Pf2_Tick();
}




/*=============================================================================
 * Function   : FPf2_CurtailmentActive
 *
 * Description: last PF2 curtailment decision
 * Input      : -
 * Output     : TRUE when curtailment is commanded
 *=============================================================================*/
bool FPf2_CurtailmentActive(void)
{
    return CcliBridge_Pf2_CurtailmentActive();
}
