/*=============================================================================
 * File       :  F_CHRONO.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "chrono-thermostat" function
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __F_CHRONO_H__


#define __F_CHRONO_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* number of chrono windows in a day */
#define CHRONO__NUM_OF_WINDOWS      3




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* chrono window */
typedef struct
{
    /* window status */
    bool                    status;                                     /* window status   [FALSE/TRUE] */

    /* temperature to be regulated */
    s16                     temperature;                                /* temperature to be regulated (/10 °C) [0.0-32.0 °C] */    // (valid only if "status" is TRUE)

    /* window start time */
    u8                      start_hour;                                 /* window start hour                    [0-23]        */    // (valid only if "status" is TRUE)
    u8                      start_minute;                               /* window start minute                  [0-59]        */    // (valid only if "status" is TRUE)

    /* window stop  time */
    u8                      stop_hour;                                  /* window stop  hour                    [0-23]        */    // (valid only if "status" is TRUE)
    u8                      stop_minute;                                /* window stop  minute                  [0-59]        */    // (valid only if "status" is TRUE)
} F_CHRONO__CHRONO_WINDOW;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - internal chrono function */
typedef struct
{
    bool                    status;                                     /* chrono function status [FALSE/TRUE] */
    F_CHRONO__CHRONO_WINDOW chrono_windows[7][CHRONO__NUM_OF_WINDOWS];  /* chrono windows                      */
} F_CHRONO__CONFIG_F_CHRONO_INT;


/* configuration - external chrono function */
typedef struct
{
    bool                    status;                                     /* chrono function status [FALSE/TRUE] */
    F_CHRONO__CHRONO_WINDOW chrono_windows[7][CHRONO__NUM_OF_WINDOWS];  /* chrono windows                      */
} F_CHRONO__CONFIG_F_CHRONO_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void FChrono_TaskFChrono(void *argument);

/* task event */
void FChrono_ChronoEventInt(void);
void FChrono_ChronoEventExt(void);


/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set "internal chrono function" configuration */
void FChrono__Config_FChronoInt_GetDefault(F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);
void FChrono__Config_FChronoInt_Get       (F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);
void FChrono__Config_FChronoInt_Set       (F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);
bool FChrono__Config_FChronoInt_IsValid   (F_CHRONO__CONFIG_F_CHRONO_INT *ptr_data);

/* get/set "external chrono function" configuration */
void FChrono__Config_FChronoExt_GetDefault(F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);
void FChrono__Config_FChronoExt_Get       (F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);
void FChrono__Config_FChronoExt_Set       (F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);
bool FChrono__Config_FChronoExt_IsValid   (F_CHRONO__CONFIG_F_CHRONO_EXT *ptr_data);




#endif
