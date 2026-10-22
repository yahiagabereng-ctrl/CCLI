/*=============================================================================
 * File       :  TRANSMISSION_GOOSE.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - transmission IEC 61850 GOOSE (plant LAN subscriber)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __TRANSMISSION_GOOSE_H__


#define __TRANSMISSION_GOOSE_H__




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
/* start the GOOSE subscriber when enabled in the yaml */
void TransmissionGoose_Init(void);

/* stop the GOOSE subscriber */
void TransmissionGoose_Stop(void);

/* task: link up / down on goose.timeout_s */
void TransmissionGoose_LinkTask(u32 tick);

#ifdef __cplusplus
}
#endif




#endif
