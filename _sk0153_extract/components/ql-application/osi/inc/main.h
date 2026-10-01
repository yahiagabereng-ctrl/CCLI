/*=============================================================================
 * File       :  MAIN.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - main
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __MAIN_H__


#define __MAIN_H__




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* main task */
void Main_TaskMain(void *argument);

/* event signal */
void Main_EndOfSynchronize(void);

/* reset module */
void Main_ResetModule   (void);
void Main_RestartModule (void);
void Main_ShutdownModule(void);




#endif
