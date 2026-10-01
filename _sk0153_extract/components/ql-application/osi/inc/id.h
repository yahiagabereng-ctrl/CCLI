/*=============================================================================
 * File       :  ID.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - identifiers
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ID_H__


#define __ID_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "fw_config.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* maximum serial number string length */
#define ID__MAX_LENGTH_SERIAL_NUMBER    15

/* serial number string length */
#define ID__LENGTH_SERIAL_NUMBER        12




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - "serial number" */
typedef struct
{
    ascii serial_number[ID__MAX_LENGTH_SERIAL_NUMBER + 1];   // serial number
} ID__CONFIG__SERIAL_NUMBER;




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* identifiers strings */
#if defined(FW_CONFIG__VERSION__LSHW)
    #define ID__ID_MANUFACTURER    "Shitek Technology s.r.l."   /* identifier string: manufacturer      */
    #define ID__ID_MODEL           "SK0148 - LINKY"             /* identifier string: product model     */
    #define ID__ID_FW_REVISION     "1.7.0"                      /* identifier string: firmware revision */
  //#define ID__ID_FW_REVISION     "1.7.0 (d)"                  /* identifier string: firmware revision */
#endif


#if defined(FW_CONFIG__VERSION__LFVW)
    #define ID__ID_MANUFACTURER    "Shitek Technology s.r.l."   /* identifier string: manufacturer      */
    #define ID__ID_MODEL           "SK0148 - LINKY FV"          /* identifier string: product model     */
    #define ID__ID_FW_REVISION     "1.7.0"                      /* identifier string: firmware revision */
  //#define ID__ID_FW_REVISION     "1.7.0 (d)"                  /* identifier string: firmware revision */
#endif


#if defined(FW_CONFIG__VERSION__LSHD)
    #define ID__ID_MANUFACTURER    "Shitek Technology s.r.l."   /* identifier string: manufacturer      */
    #define ID__ID_MODEL           "SK0153 - LINKY"             /* identifier string: product model     */
    #define ID__ID_FW_REVISION     "1.7.0"                      /* identifier string: firmware revision */
  //#define ID__ID_FW_REVISION     "1.7.0 (d)"                  /* identifier string: firmware revision */
#endif


#if defined(FW_CONFIG__VERSION__LFVD)
    #define ID__ID_MANUFACTURER    "Shitek Technology s.r.l."   /* identifier string: manufacturer      */
    #define ID__ID_MODEL           "SK0153 - LINKY FV"          /* identifier string: product model     */
    #define ID__ID_FW_REVISION     "1.7.0"                      /* identifier string: firmware revision */
  //#define ID__ID_FW_REVISION     "1.7.0 (d)"                  /* identifier string: firmware revision */
#endif




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
const ascii *Id_GetIdManufacturer(void);
const ascii *Id_GetIdModel       (void);
const ascii *Id_GetIdFWRevision  (void);

/* get/set "serial number" configuration */
      void   Id_Config_SerialNumber_GetDefault(ID__CONFIG__SERIAL_NUMBER *ptr_data);
      void   Id_Config_SerialNumber_Get       (ID__CONFIG__SERIAL_NUMBER *ptr_data);
      void   Id_Config_SerialNumber_Set       (ID__CONFIG__SERIAL_NUMBER *ptr_data);
      bool   Id_Config_SerialNumber_IsValid   (ID__CONFIG__SERIAL_NUMBER *ptr_data);




#endif
