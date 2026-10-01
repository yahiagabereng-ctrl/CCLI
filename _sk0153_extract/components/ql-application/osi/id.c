/*=============================================================================
 * File       :  ID.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - identifiers
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDE
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "id.h"
#include "typedef.h"




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* identifiers strings */
static const ascii                     Id_IdManufacturer[] = ID__ID_MANUFACTURER;     /* identifier string: manufacturer      */
static const ascii                     Id_IdModel       [] = ID__ID_MODEL;            /* identifier string: product model     */
static const ascii                     Id_IdFW_Revision [] = ID__ID_FW_REVISION;      /* identifier string: firmware revision */

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - "serial number" */
static       ID__CONFIG__SERIAL_NUMBER Id_Config_SerialNumber;
static const ID__CONFIG__SERIAL_NUMBER Id_Config_SerialNumberDefault =
{
    "------------"   // serial number
};




/*===========================================================================
 * FUNCTION PROTOTYPES
 *===========================================================================*/
const ascii *Id_GetIdManufacturer(void);
const ascii *Id_GetIdModel       (void);
const ascii *Id_GetIdFWRevision  (void);

/* get/set "serial number" configuration */
      void   Id_Config_SerialNumber_GetDefault(ID__CONFIG__SERIAL_NUMBER *ptr_data);
      void   Id_Config_SerialNumber_Get       (ID__CONFIG__SERIAL_NUMBER *ptr_data);
      void   Id_Config_SerialNumber_Set       (ID__CONFIG__SERIAL_NUMBER *ptr_data);
      bool   Id_Config_SerialNumber_IsValid   (ID__CONFIG__SERIAL_NUMBER *ptr_data);




/*===========================================================================
 * Function    : Id_GetIdManufacturer
 *
 * Description : return the manufacturer identifier string
 * Input       : -
 * Output      : - pointer to the manufacturer identifier string
 *===========================================================================*/
const ascii *Id_GetIdManufacturer(void)
{
    return Id_IdManufacturer;
}




/*===========================================================================
 * Function    : Id_GetIdModel
 *
 * Description : return the product model identifier string
 * Input       : -
 * Output      : - pointer to the product model identifier string
 *===========================================================================*/
const ascii *Id_GetIdModel(void)
{
    return Id_IdModel;
}




/*===========================================================================
 * Function    : Id_GetIdFWRevision
 *
 * Description : return the firmware revision identifier string
 * Input       : -
 * Output      : - pointer to the firmware revision identifier string
 *===========================================================================*/
const ascii *Id_GetIdFWRevision(void)
{
    return Id_IdFW_Revision;
}




/*===========================================================================
 * Function   : Id_Config_SerialNumber_GetDefault
 *
 * Description: get the default "serial number" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Id_Config_SerialNumber_GetDefault(ID__CONFIG__SERIAL_NUMBER *ptr_data)
{
    *ptr_data = Id_Config_SerialNumberDefault;
}




/*===========================================================================
 * Function   : Id_Config_SerialNumber_Get
 *
 * Description: get the "serial number" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Id_Config_SerialNumber_Get(ID__CONFIG__SERIAL_NUMBER *ptr_data)
{
    *ptr_data = Id_Config_SerialNumber;
}




/*===========================================================================
 * Function   : Id_Config_SerialNumber_Set
 *
 * Description: set the "serial number" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Id_Config_SerialNumber_Set(ID__CONFIG__SERIAL_NUMBER *ptr_data)
{
    Id_Config_SerialNumber = *ptr_data;
}




/*===========================================================================
 * Function   : Id_Config_SerialNumber_IsValid
 *
 * Description: check if the "serial number" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Id_Config_SerialNumber_IsValid(ID__CONFIG__SERIAL_NUMBER *ptr_data)
{
    if (strlen(ptr_data->serial_number) >  ID__MAX_LENGTH_SERIAL_NUMBER)
        return FALSE;

    if (strlen(ptr_data->serial_number) != ID__LENGTH_SERIAL_NUMBER    )
        return FALSE;


    return TRUE;
}
