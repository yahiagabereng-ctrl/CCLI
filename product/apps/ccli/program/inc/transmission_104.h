/*=============================================================================
 * File       :  TRANSMISSION_104.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - transmission IEC 60870-5-104 (Eth_B, operator)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __TRANSMISSION_104_H__


#define __TRANSMISSION_104_H__




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
/* start the CS104 server when enabled in the yaml */
void Transmission104_Init(void);

/* stop the CS104 server */
void Transmission104_Stop(void);

/* task: publish TotW */
void Transmission104_PublishTask(u32 tick);

/* task: CS104 server tick (periodic / spontaneous ASDUs) */
void Transmission104_Task(u32 tick);

/* task: P4-04 comms-loss fallback and operator ASDU audit */
void Transmission104_EventTask(u32 tick);

#ifdef __cplusplus
}
#endif




#endif
