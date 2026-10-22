/*=============================================================================
 * File       :  PARSER_MODBUS.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - Modbus master (plant meter poll, link state, trace)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __PARSER_MODBUS_H__


#define __PARSER_MODBUS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* link state */
#define PARSER_MODBUS__LINK_STATE__DOWN       (-1)
#define PARSER_MODBUS__LINK_STATE__UNKNOWN    ( 0)
#define PARSER_MODBUS__LINK_STATE__UP         ( 1)




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
void ParserModbus_Init(void);

/* task: poll the plant meter (lab demo: simulated ramp) */
void ParserModbus_Task(u32 tick);

/* task: log link up / down / recovered */
void ParserModbus_LinkTask(u32 tick);

/* task: per-exchange trace on stderr (modbus.trace) */
void ParserModbus_TraceTask(u32 tick);

/* link state (PARSER_MODBUS__LINK_STATE__*) */
s32  ParserModbus_GetLinkState(void);

#ifdef __cplusplus
}
#endif




#endif
