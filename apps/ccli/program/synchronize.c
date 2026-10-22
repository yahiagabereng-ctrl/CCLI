/*=============================================================================
 * File       :  SYNCHRONIZE.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - synchronize (GNSS / time quality for the data model)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* user     includes */
#include "synchronize.h"

#include "ccli_bridge.h"
#include "main.h"




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* first GNSS fix seen */
static bool Synchronize_Synchronized = FALSE;




/*=============================================================================
 * Function   : Synchronize_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Synchronize_Init(void)
{
    Synchronize_Synchronized = FALSE;
}




/*=============================================================================
 * Function   : Synchronize_Task
 *
 * Description: refresh MMS time quality; signal Main once on first GNSS fix
 * Input      : tick - supervisor tick
 * Output     : -
 *=============================================================================*/
void Synchronize_Task(u32 tick)
{
    (void)tick;

    CcliBridge_Mms_RefreshTimeQuality();

    if (!Synchronize_Synchronized && CcliBridge_Mms_GnssFix())
    {
        Synchronize_Synchronized = TRUE;
        Main_EndOfSynchronize();
    }
}




/*=============================================================================
 * Function   : Synchronize_IsSynchronized
 *
 * Description: TRUE once the first GNSS fix has been seen
 * Input      : -
 * Output     : synchronized indication
 *=============================================================================*/
bool Synchronize_IsSynchronized(void)
{
    return Synchronize_Synchronized;
}
