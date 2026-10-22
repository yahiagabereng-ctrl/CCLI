/*=============================================================================
 * File       :  PROGRAM_FLASH.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - flash objects (persisted configuration / status)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __PROGRAM_FLASH_H__


#define __PROGRAM_FLASH_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* handle indices */
#define PROGRAM_FLASH__HANDLE_INDEX__000_STARTUP                0
#define PROGRAM_FLASH__HANDLE_INDEX__001_REGULATION             1
#define PROGRAM_FLASH__HANDLE_INDEX__002_OUTPUTS                2
#define PROGRAM_FLASH__NUM_HANDLES                              3

/* flash IDs - handle 000 STARTUP */
#define PROGRAM_FLASH__FLASH_ID__000_STARTUP__STARTUP_TYPE      0

/* flash IDs - handle 001 REGULATION */
#define PROGRAM_FLASH__FLASH_ID__001_REGULATION__INTERLOCK      0

/* flash IDs - handle 002 OUTPUTS */
#define PROGRAM_FLASH__FLASH_ID__002_OUTPUTS__GPIO_CONFIG       0




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
void ProgramFlash_Init(void);

/* request the backup of one flash object (deferred to ProgramFlash_Task) */
bool ProgramFlash_RequestWriteBackup(u8 handle_index, u8 flash_id);

/* task: serve pending backup requests */
void ProgramFlash_Task(u32 tick);

/* number of backup requests served since boot */
u32  ProgramFlash_NumBackupsServed(void);

#ifdef __cplusplus
}
#endif




#endif
