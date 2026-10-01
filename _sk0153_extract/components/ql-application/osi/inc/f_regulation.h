/*=============================================================================
 * File       :  F_REGULATION.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "regulation" function
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __F_REGULATION_H__


#define __F_REGULATION_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* status - "regulation" function status */
typedef struct
{
    bool status;          /* function status [FALSE/TRUE]                                          */
    s16  temperature;     /* temperature to be regulated (/10 °C) (valid only if "status" is TRUE) */
    u16  duration;        /* duration of regulation      (min)    (valid only if "status" is TRUE) */
    u16  remaining_time;  /* remaining time              (min)    (valid only if "status" is TRUE) */
} F_REGULATION__STATUS_REGULATION_INT;


/* status - "regulation" function status */
typedef struct
{
    bool status;          /* function status [FALSE/TRUE]                                          */
    s16  temperature;     /* temperature to be regulated (/10 °C) (valid only if "status" is TRUE) */
    u16  duration;        /* duration of regulation      (min)    (valid only if "status" is TRUE) */
    u16  remaining_time;  /* remaining time              (min)    (valid only if "status" is TRUE) */
} F_REGULATION__STATUS_REGULATION_EXT;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void FRegulation_TaskFRegulation(void *argument);

/* remaining time */
u32  FRegulation_RemainingTimeInt(void);
u32  FRegulation_RemainingTimeExt(void);

/* task events */
void FRegulation_NewTeleControlInt(F_REGULATION__STATUS_REGULATION_INT *ptr_function_status_new, F_REGULATION__STATUS_REGULATION_INT *ptr_function_status_old);
void FRegulation_NewTeleControlExt(F_REGULATION__STATUS_REGULATION_EXT *ptr_function_status_new, F_REGULATION__STATUS_REGULATION_EXT *ptr_function_status_old);

/* get/set internal "regulation function" status */
void FRegulation_Status_FRegulationInt_GetDefault(F_REGULATION__STATUS_REGULATION_INT *ptr_data);
void FRegulation_Status_FRegulationInt_Get       (F_REGULATION__STATUS_REGULATION_INT *ptr_data);
void FRegulation_Status_FRegulationInt_Set       (F_REGULATION__STATUS_REGULATION_INT *ptr_data);
bool FRegulation_Status_FRegulationInt_IsValid   (F_REGULATION__STATUS_REGULATION_INT *ptr_data);

/* get/set external "regulation function" status */
void FRegulation_Status_FRegulationExt_GetDefault(F_REGULATION__STATUS_REGULATION_EXT *ptr_data);
void FRegulation_Status_FRegulationExt_Get       (F_REGULATION__STATUS_REGULATION_EXT *ptr_data);
void FRegulation_Status_FRegulationExt_Set       (F_REGULATION__STATUS_REGULATION_EXT *ptr_data);
bool FRegulation_Status_FRegulationExt_IsValid   (F_REGULATION__STATUS_REGULATION_EXT *ptr_data);




#endif
