/*=============================================================================
 * File       :  ALARM_INPUT.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - alarm input (permissive DI, Annex M trip relay)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __ALARM_INPUT_H__


#define __ALARM_INPUT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init with the boot-time Annex M trip state */
void AlarmInput_Init(bool annex_m_trip_active);

/* log transitions */
void AlarmInput_TrackAnnexM    (bool annex_m_trip_active);
void AlarmInput_TrackPermissive(bool permissive_di);

#ifdef __cplusplus
}
#endif




#endif
