/*=============================================================================
 * File       :  TRANSMISSION_MMS.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - transmission IEC 61850 MMS (Eth_A, DSO, 62351 TLS)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __TRANSMISSION_MMS_H__


#define __TRANSMISSION_MMS_H__




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
/* start the MMS server when enabled in the yaml */
void TransmissionMms_Init(void);

/* stop the MMS server */
void TransmissionMms_Stop(void);

/* task: publish TotW / TotVAr / PPV to the data model */
void TransmissionMms_PublishTask(u32 tick);

/* task: P3-05 comms-loss fallback and DSO live commands (Wlim / WSd / VArSd / PFSP) */
void TransmissionMms_Task(u32 tick);

#ifdef __cplusplus
}
#endif




#endif
