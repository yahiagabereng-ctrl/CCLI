/*=============================================================================
 * File       :  QUEUE_MY.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - queues (deferred work between program modules)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __QUEUE_MY_H__


#define __QUEUE_MY_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* queue identifiers */
#define QUEUE__QUEUE_ID__MAIN_MESSAGE      0        /* Main task message IDs        */
#define QUEUE__QUEUE_ID__FLASH_BACKUP      1        /* program_flash backup requests */
#define QUEUE__NUM_QUEUES                  2

/* max records per queue */
#define QUEUE__MAX_NUM_RECORDS             32




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init (all queues empty) */
void Queue_Init(void);

/* put / get one record (FIFO); FALSE when full / empty / bad queue ID */
bool Queue_PutRecord(u8 queue_id, u32  record);
bool Queue_GetRecord(u8 queue_id, u32 *ptr_record);

/* number of queued records */
u32  Queue_NumRecords(u8 queue_id);

/* status variables */
void Queue_UpdateVarCrc(void);
bool Queue_VerifyVarCrc(void);

#ifdef __cplusplus
}
#endif




#endif
