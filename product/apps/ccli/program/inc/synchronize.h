/*=============================================================================
 * File       :  SYNCHRONIZE.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - synchronize (GNSS / time quality for the data model)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __SYNCHRONIZE_H__


#define __SYNCHRONIZE_H__




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
/* init */
void Synchronize_Init(void);

/* task: refresh MMS time quality; signal Main once on first GNSS fix */
void Synchronize_Task(u32 tick);

/* TRUE once the first GNSS fix has been seen */
bool Synchronize_IsSynchronized(void);

#ifdef __cplusplus
}
#endif




#endif
