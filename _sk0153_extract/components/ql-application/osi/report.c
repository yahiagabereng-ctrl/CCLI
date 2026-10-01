/*=============================================================================
 * File       :  REPORT.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - report
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "report.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - "report" */
static       REPORT__CONFIG__REPORT Report_Config_Report;
static const REPORT__CONFIG__REPORT Report_Config_ReportDefault =
{
    TRUE
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* get/set "report" configuration */
void Report_Config_Report_GetDefault(REPORT__CONFIG__REPORT *ptr_data);
void Report_Config_Report_Get       (REPORT__CONFIG__REPORT *ptr_data);
void Report_Config_Report_Set       (REPORT__CONFIG__REPORT *ptr_data);
bool Report_Config_Report_IsValid   (REPORT__CONFIG__REPORT *ptr_data);




/*===========================================================================
 * Function   : Report_Config_Report_GetDefault
 *
 * Description: get the default "report" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Report_Config_Report_GetDefault(REPORT__CONFIG__REPORT *ptr_data)
{
    *ptr_data = Report_Config_ReportDefault;
}




/*===========================================================================
 * Function   : Report_Config_Report_Get
 *
 * Description: get the "report" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Report_Config_Report_Get(REPORT__CONFIG__REPORT *ptr_data)
{
    *ptr_data = Report_Config_Report;
}




/*===========================================================================
 * Function   : Report_Config_Report_Set
 *
 * Description: set the "report" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Report_Config_Report_Set(REPORT__CONFIG__REPORT *ptr_data)
{
    Report_Config_Report = *ptr_data;
}




/*===========================================================================
 * Function   : Report_Config_Report_IsValid
 *
 * Description: check if the "report" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Report_Config_Report_IsValid(REPORT__CONFIG__REPORT *ptr_data)
{
    return TRUE;
}
