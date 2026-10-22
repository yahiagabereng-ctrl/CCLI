/*=============================================================================
 * File       :  BOOT.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  BOOT - boot (command line, cooperative task table)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __BOOT_H__


#define __BOOT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"

#include "commands.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* cooperative task (run every period_ticks supervisor ticks, in table order) */
typedef struct
{
    const ascii *task_name;
    u32          period_ticks;
    void       (*task_entry)(u32 tick);
} BOOT__TASK_DEF;




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* boot: parse the command line, run one-shot modes; FALSE when the process must exit */
bool                  Boot_TaskBoot(s32 argc, ascii **argv, s32 *ptr_exit_code);

/* command line options */
const COMMANDS__OPTIONS *Boot_GetOptions(void);

/* cooperative task table */
u32                   Boot_NumTasks(void);
const BOOT__TASK_DEF *Boot_GetTaskDef(u32 index);

#ifdef __cplusplus
}
#endif




#endif
