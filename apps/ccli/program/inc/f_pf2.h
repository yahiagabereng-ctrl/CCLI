/*=============================================================================
 * File       :  F_PF2.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - function PF2 (CEI 0-16 export limitation FSM)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __F_PF2_H__


#define __F_PF2_H__




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
/* task: one PF2 FSM step on the latest measurement */
void FPf2_Task(u32 tick);

/* last PF2 curtailment decision (safe default: FALSE) */
bool FPf2_CurtailmentActive(void);

#ifdef __cplusplus
}
#endif




#endif
