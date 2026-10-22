/*=============================================================================
 * File       :  ALARM.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - alarm (O.14 event log entry point, meter quality)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __ALARM_H__


#define __ALARM_H__




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
void Alarm_Init(void);

/* append one O.14 event (type = O.14 source, detail = free text) */
void Alarm_Log(const ascii *type, const ascii *detail);

/* task: meter data quality transitions */
void Alarm_MeterQualityTask(u32 tick);

#ifdef __cplusplus
}
#endif




#endif
