/*=============================================================================
 * File       :  OUTPUTS.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - outputs (PF2 curtailment DO, permissive DI, Annex M trip)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __OUTPUTS_H__


#define __OUTPUTS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include <stdbool.h>

#include "drvgpio.h"




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init with an explicit / default configuration; outputs forced safe */
bool Outputs_Init       (const DrvGpioConfig *cfg);
bool Outputs_InitDefault(void);

/* curtailment DO / permissive DI */
bool Outputs_ApplyCurtailment(bool active);
bool Outputs_ReadPermissive  (bool *ok);

/* P6-03 Annex M trip relay monitor */
bool Outputs_ConfigureAnnexMTripMonitor(int gpio_channel, bool enabled);
bool Outputs_ReadAnnexMTripActive      (bool *active);

/* curtailment interlock: PF2 command AND permissive AND NOT Annex M trip */
bool         Outputs_GateCurtailment (bool commanded, bool permissive_ok, bool annex_m_trip_active);
const char  *Outputs_CurtailDetail   (bool do_on, bool commanded, bool annex_m_trip_active);

/* safe state / release */
void Outputs_SafeState(void);
void Outputs_Shutdown (void);

/* one status line on stderr (procd -> logread) */
void Outputs_LogStatus(bool curtail_active, bool permissive_ok, bool annex_m_trip_active);

/* status variables */
void Outputs_UpdateVarCrc(void);
bool Outputs_VerifyVarCrc(void);

#ifdef __cplusplus
}
#endif




#endif
