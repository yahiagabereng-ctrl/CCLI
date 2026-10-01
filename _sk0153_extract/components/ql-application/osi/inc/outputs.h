/*=============================================================================
 * File       :  OUTPUTS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - digital outputs manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __OUTPUTS_H__


#define __OUTPUTS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* output activation time unit */
#define OUTPUTS__ACTIVATION_TIME_UNIT__SECOND    0    /* seconds */
#define OUTPUTS__ACTIVATION_TIME_UNIT__MINUTE    1    /* minutes */
#define OUTPUTS__ACTIVATION_TIME_UNIT__HOUR      2    /* hours   */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - internal timer output */
typedef struct
{
    u8   activation_time_value;    /* output activation time value */   // [0-255]
    u8   activation_time_unit;     /* output activation time unit  */   // [second/minute/hour]
} OUTPUTS__CONFIG__TIMER_OUTPUT_INT;


/* configuration - external timer output */
typedef struct
{
    u8   activation_time_value;    /* output activation time value */   // [0-255]
    u8   activation_time_unit;     /* output activation time unit  */   // [second/minute/hour]
} OUTPUTS__CONFIG__TIMER_OUTPUT_EXT;


/* status - indication of OUT1 output manual status */
typedef struct
{
    bool manual_status;            /* indication of OUT1 output manual status */
} OUTPUTS__STATUS__OUTPUT_OUT1;


/* status - indication of OUT2 output manual status */
typedef struct
{
    bool manual_status;            /* indication of OUT2 output manual status */
} OUTPUTS__STATUS__OUTPUT_OUT2;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Outputs_TaskOutputs(void *argument);

/* status variables */
void Outputs_UpdateVarCrc(void);
bool Outputs_VerifyVarCrc(void);

/* get/set "internal timer output" configuration */
void Outputs_Config_TimerOutputInt_GetDefault(OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);
void Outputs_Config_TimerOutputInt_Get       (OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);
void Outputs_Config_TimerOutputInt_Set       (OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);
bool Outputs_Config_TimerOutputInt_IsValid   (OUTPUTS__CONFIG__TIMER_OUTPUT_INT *ptr_data);

/* get/set "external timer output" configuration */
void Outputs_Config_TimerOutputExt_GetDefault(OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);
void Outputs_Config_TimerOutputExt_Get       (OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);
void Outputs_Config_TimerOutputExt_Set       (OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);
bool Outputs_Config_TimerOutputExt_IsValid   (OUTPUTS__CONFIG__TIMER_OUTPUT_EXT *ptr_data);

/* get/set "indication of OUT1 output manual status" configuration */
void Outputs_Status_OutputOut1_GetDefault    (OUTPUTS__STATUS__OUTPUT_OUT1      *ptr_data);
void Outputs_Status_OutputOut1_Get           (OUTPUTS__STATUS__OUTPUT_OUT1      *ptr_data);
void Outputs_Status_OutputOut1_Set           (OUTPUTS__STATUS__OUTPUT_OUT1      *ptr_data);

/* get/set "indication of OUT2 output manual status" configuration */
void Outputs_Status_OutputOut2_GetDefault    (OUTPUTS__STATUS__OUTPUT_OUT2      *ptr_data);
void Outputs_Status_OutputOut2_Get           (OUTPUTS__STATUS__OUTPUT_OUT2      *ptr_data);
void Outputs_Status_OutputOut2_Set           (OUTPUTS__STATUS__OUTPUT_OUT2      *ptr_data);

/* task events */

/* internal temperature */
bool Outputs_Event_ManualInt_Off     (void);
bool Outputs_Event_ManualInt_On      (void);
bool Outputs_Event_FAntifrostInt_Off (void);
bool Outputs_Event_FAntifrostInt_On  (s16 temperature);
bool Outputs_Event_FRegulationInt_Off(void);
bool Outputs_Event_FRegulationInt_On (s16 temperature);
bool Outputs_Event_FChronoInt_Off    (void);
bool Outputs_Event_FChronoInt_On     (s16 temperature);

/* external temperature */
bool Outputs_Event_ManualExt_Off     (void);
bool Outputs_Event_ManualExt_On      (void);
bool Outputs_Event_FAntifrostExt_Off (void);
bool Outputs_Event_FAntifrostExt_On  (s16 temperature);
bool Outputs_Event_FRegulationExt_Off(void);
bool Outputs_Event_FRegulationExt_On (s16 temperature);
bool Outputs_Event_FChronoExt_Off    (void);
bool Outputs_Event_FChronoExt_On     (s16 temperature);




#endif
