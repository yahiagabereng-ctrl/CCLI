/*=============================================================================
 * File       :  FILE_CSV.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - csv file
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __FILE_CSV_H__


#define __FILE_CSV_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* record types */
#define FILE_CSV__REC_TYPE__LOG      0
#define FILE_CSV__REC_TYPE__ALARM    1

/* list of CSV file versions */
#define FILE_CSV__FILE_VERSION_V1    1     /* CSV file version V1 */
#define FILE_CSV__FILE_VERSION_V2    2     /* CSV file version V2 */
#define FILE_CSV__FILE_VERSION_V3    3     /* CSV file version V3 */

/* CSV file version */
#define FILE_CSV__FILE_VERSION       FILE_CSV__FILE_VERSION_V3




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* build strings */
ascii *FileCsv_BuildString_Header    (void);
ascii *FileCsv_BuildString_DataRecord(u8 rec_type, u8 rec_subtype_1, u8 rec_subtype_2);




#endif
