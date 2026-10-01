/*=============================================================================
 * File       :  PARSER_TX.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - transmitted command parser
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __PARSER_TX_H__


#define __PARSER_TX_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "queue_my.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
typedef QUEUE__FW_UPGRADE_QUEUE_RECORD        PARSER_TX__FW_UPGRADE_RECORD;         /* FW upgrade       record */
typedef QUEUE__ALARM_QUEUE_RECORD             PARSER_TX__ALARM_RECORD;              /* alarm            record */
typedef QUEUE__SMS_ANSWER_QUEUE_RECORD        PARSER_TX__SMS_ANSWER_RECORD;         /* SMS answer       record */
typedef QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD PARSER_TX__SMS_ANSWER_TO_CRS_RECORD;  /* SMS answer to CRS record */
typedef QUEUE__SMS_FORWARD_QUEUE_RECORD       PARSER_TX__SMS_FORWARD_RECORD;        /* SMS forward      record */
typedef QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD   PARSER_TX__AUTOSYNCHRONIZE_RECORD;    /* autosynchronize  record */
typedef QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD  PARSER_TX__SMS_ANSWER_EVENT_RECORD;   /* SMS answer event record */




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
void ParserTx_BuildSmsFwUpgrade      (PARSER_TX__FW_UPGRADE_RECORD        *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsAlarm          (PARSER_TX__ALARM_RECORD             *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsAnswer      (PARSER_TX__SMS_ANSWER_RECORD        *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsAnswerToCrs (PARSER_TX__SMS_ANSWER_TO_CRS_RECORD *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsForward     (PARSER_TX__SMS_FORWARD_RECORD       *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsAutosynchronize(PARSER_TX__AUTOSYNCHRONIZE_RECORD   *ptr_record, ascii *ptr_text);
void ParserTx_BuildSmsSmsAnswerEvent (PARSER_TX__SMS_ANSWER_EVENT_RECORD  *ptr_record, ascii *ptr_text);




#endif
