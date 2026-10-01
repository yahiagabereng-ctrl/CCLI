/*=============================================================================
 * File       :  DEFAULT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - default
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DEFAULT_H__


#define __DEFAULT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
bool Default_DefaultCommand_Empty    (void);
bool Default_DefaultCommand_Phonebook(void);
bool Default_DefaultCommand_Commands (void);
bool Default_DefaultCommand_All      (void);

bool Default_DefaultResetKey(void);




#endif
