/*=============================================================================
 * File       :  MAIN.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - main (supervisor loop, messages, watchdog, status)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __MAIN_H__


#define __MAIN_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* task message IDs */
#define MAIN__TASK_MSG_ID__END_OF_SYNCHRONIZE    1




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* main task: cooperative supervisor loop until SIGTERM / SIGINT */
void Main_TaskMain(void);

/* event signal */
void Main_EndOfSynchronize(void);

/* periodic tasks (registered in the Boot task table) */
void Main_StatusTask  (u32 tick);
void Main_WatchdogTask(u32 tick);

/* orderly shutdown (outputs safe, servers stopped) */
void Main_ShutdownModule(void);

#ifdef __cplusplus
}
#endif




#endif
