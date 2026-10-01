/*=============================================================================
 * File       :  AT_DEBUG.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - AT specificis commands
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __AT_DEBUG_H__


#define __AT_DEBUG_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
void AtDebug_Init(void);

/* send string response */
void AtDebug_SendString(ascii *string_to_send);




#endif
