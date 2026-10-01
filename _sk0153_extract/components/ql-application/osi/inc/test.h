/*=============================================================================
 * File       :  TEST.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  TEST - test
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __TEST_H__


#define __TEST_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
void Test_PutRecord_FwUpgrade      (u8 num_records);
void Test_PutRecord_Alarm          (u8 num_records);
void Test_PutRecord_SmsAnswer      (u8 num_records);
void Test_PutRecord_SmsForward     (u8 num_records);
void Test_PutRecord_Autosynchronize(u8 num_records);
void Test_PutRecord_SmsAnswerEvent (u8 num_records);
void Test_PutRecord_FileStatusLog  (u8 num_records, const ascii *file_name);
void Test_PutRecord_FileAlarm      (u8 num_records, const ascii *file_name);

void Test_PutRecord_File_StatusLog (u8 num_records);




#endif
