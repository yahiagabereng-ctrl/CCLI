/*=============================================================================
 * File       :  REGULATION.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - regulation (PF2 command -> permissive / Annex M -> DO)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __REGULATION_H__


#define __REGULATION_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* status - "curtailment interlock" */
typedef struct
{
    bool curtailment_commanded;       /* PF2 decision                         */
    bool permissive_ok;               /* permissive DI (or lab bypass)        */
    bool annex_m_trip_active;         /* Annex M trip relay energized         */
    bool do_curtail_on;               /* curtailment DO actually driven       */
} REGULATION__STATUS__INTERLOCK;




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init: first interlock evaluation and DO drive */
void Regulation_Init(void);

/* task: interlock evaluation and DO drive */
void Regulation_Task(u32 tick);

/* status */
void Regulation_Status_Get(REGULATION__STATUS__INTERLOCK *ptr_data);

/* status variables */
void Regulation_UpdateVarCrc(void);
bool Regulation_VerifyVarCrc(void);

#ifdef __cplusplus
}
#endif




#endif
