/*=============================================================================
 * File       :  PARSER_RX.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - received command parser
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __PARSER_RX_H__


#define __PARSER_RX_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
#define PARSER_RX__NUM_MAX_ANSWERS    26




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* received command text parser */
bool ParserRx_ParseCommandText(bool command_not_to_exe, CLOCK__TIME *ptr_time_tx, ascii *ptr_phone_number, bool is_adm, bool is_crs, ascii *ptr_command_text, bool *ptr_report, bool *ptr_command_executed, u8 *ptr_num_answers, ascii **ptr_answer_text, u8 max_size_answer_text);




#endif
