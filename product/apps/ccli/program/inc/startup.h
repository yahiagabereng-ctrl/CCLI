/*=============================================================================
 * File       :  STARTUP.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - startup (bring-up order, startup type, recover)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __STARTUP_H__


#define __STARTUP_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"

#include "commands.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* startup type */
#define STARTUP__STARTUP_TYPE__UNKNOWN            0
#define STARTUP__STARTUP_TYPE__POWER_ON           1      /* previous run shut down cleanly    */
#define STARTUP__STARTUP_TYPE__REBOOT_INTERNAL    2      /* previous run crashed (tombstone)  */
#define STARTUP__STARTUP_TYPE__REBOOT_UNCLEAN     3      /* stale running marker (kill / PSU) */




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* startup: full bring-up in the lab-validated order; FALSE on fatal error */
bool Startup_TaskStartup(const COMMANDS__OPTIONS *ptr_options);

/* startup type (STARTUP__STARTUP_TYPE__*) */
u8   Startup_GetStartupType(void);

/* TRUE when retained status passed the CRC check on a reboot startup */
bool Startup_GetRecoverStatus(void);

#ifdef __cplusplus
}
#endif




#endif
