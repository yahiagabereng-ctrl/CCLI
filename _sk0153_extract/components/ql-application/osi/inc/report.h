/*=============================================================================
 * File       :  REPORT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - report
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __REPORT_H__


#define __REPORT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - "report" */
typedef struct
{
    bool status;   /* report status */
} REPORT__CONFIG__REPORT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "report" configuration */
void Report_Config_Report_GetDefault(REPORT__CONFIG__REPORT *ptr_data);
void Report_Config_Report_Get       (REPORT__CONFIG__REPORT *ptr_data);
void Report_Config_Report_Set       (REPORT__CONFIG__REPORT *ptr_data);
bool Report_Config_Report_IsValid   (REPORT__CONFIG__REPORT *ptr_data);




#endif
