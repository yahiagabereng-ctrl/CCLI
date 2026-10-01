/*=============================================================================
 * File       :  COUNTERS.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - counters manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __COUNTERS_H__


#define __COUNTERS_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* counters band types */
#define COUNTERS__BAND_TYPE__F1        1   // band type F1 (more expensive)
#define COUNTERS__BAND_TYPE__F2        2   // band type F2
#define COUNTERS__BAND_TYPE__F3        3   // band type F3 (less expensive)

/* maximum number of counters bands per day */
#define COUNTERS__NUM_BANDS_PER_DAY    7




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/* counter band */
typedef struct
{
    bool                   status;                                                /* band enable status        */
    u8                     type;                                                  /* band type (F1, F2, F3)    */
    u8                     start_hour;                                            /* start start hour   [0-23] */
    u8                     start_minute;                                          /* start start minute [0-59] */
} COUNTERS__COUNTER_BAND;


/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* input C1 */
typedef struct
{
    u32                    counter;                                               /* counter C1           */

    u32                    counter_f1;                                            /* counter C1 - band F1 */
    u32                    counter_f2;                                            /* counter C1 - band F2 */
    u32                    counter_f3;                                            /* counter C1 - band F3 */
} COUNTERS__STATUS__INPUT_C1;


/* input C2 */
typedef struct
{
    u32                    counter;                                               /* counter C2           */

    u32                    counter_f1;                                            /* counter C2 - band F1 */
    u32                    counter_f2;                                            /* counter C2 - band F2 */
    u32                    counter_f3;                                            /* counter C2 - band F3 */
} COUNTERS__STATUS__INPUT_C2;


/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* configuration - input C1 */
typedef struct
{
    bool                   enabled_status;                                        /* input C1 enabled status */
} COUNTERS__CONFIG__INPUT_C1;


/* configuration - input C2 */
typedef struct
{
    bool                   enabled_status;                                        /* input C2 enabled status */
} COUNTERS__CONFIG__INPUT_C2;


/* configuration - input C1 bands */
typedef struct
{
    bool                   enabled_status_week_days;                              /* input C1 bands enabled status (for week      days) */
    bool                   enabled_status_festivity_days;                         /* input C1 bands enabled status (for festivity days) */
    COUNTERS__COUNTER_BAND bands_week_days     [7][COUNTERS__NUM_BANDS_PER_DAY];  /* input C1 bands time           (for week      days) */
    COUNTERS__COUNTER_BAND bands_festivity_days   [COUNTERS__NUM_BANDS_PER_DAY];  /* input C1 bands time           (for festivity days) */
} COUNTERS__CONFIG__INPUT_C1_BANDS;


/* configuration - input C2 bands */
typedef struct
{
    bool                   enabled_status_week_days;                              /* input C2 bands enabled status (for week      days) */
    bool                   enabled_status_festivity_days;                         /* input C2 bands enabled status (for festivity days) */
    COUNTERS__COUNTER_BAND bands_week_days     [7][COUNTERS__NUM_BANDS_PER_DAY];  /* input C1 bands time           (for week      days) */
    COUNTERS__COUNTER_BAND bands_festivity_days   [COUNTERS__NUM_BANDS_PER_DAY];  /* input C1 bands time           (for festivity days) */
} COUNTERS__CONFIG__INPUT_C2_BANDS;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Counters_TaskCounters(void *argument);

/* status variables */
void Counters_UpdateVarCrc(void);
bool Counters_VerifyVarCrc(void);

/* events */
void Counters_Event_CounterC1Impulse      (void);
void Counters_Event_CounterC2Impulse      (void);
void Counters_Event_MainPowerSupplyProblem(void);
void Counters_Event_InputC1Band           (void);
void Counters_Event_InputC2Band           (void);
void Counters_Event_ClockChanged          (void);

/* new configuration */
void Counters_NewConfigurationInputC1     (COUNTERS__CONFIG__INPUT_C1       *ptr_config_new, COUNTERS__CONFIG__INPUT_C1       *ptr_config_old);
void Counters_NewConfigurationInputC2     (COUNTERS__CONFIG__INPUT_C2       *ptr_config_new, COUNTERS__CONFIG__INPUT_C2       *ptr_config_old);
void Counters_NewConfigurationInputC1Bands(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_config_new, COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_config_old);
void Counters_NewConfigurationInputC2Bands(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_config_new, COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_config_old);

/* get/set C1 counters */
void Counters_InputC1_Get  (u32 *ptr_value);
void Counters_InputC1_Set  (u32 *ptr_value);
void Counters_InputC1F1_Get(u32 *ptr_value);
void Counters_InputC1F1_Set(u32 *ptr_value);
void Counters_InputC1F2_Get(u32 *ptr_value);
void Counters_InputC1F2_Set(u32 *ptr_value);
void Counters_InputC1F3_Get(u32 *ptr_value);
void Counters_InputC1F3_Set(u32 *ptr_value);

/* get/set C2 counters */
void Counters_InputC2_Get  (u32 *ptr_value);
void Counters_InputC2_Set  (u32 *ptr_value);
void Counters_InputC2F1_Get(u32 *ptr_value);
void Counters_InputC2F1_Set(u32 *ptr_value);
void Counters_InputC2F2_Get(u32 *ptr_value);
void Counters_InputC2F2_Set(u32 *ptr_value);
void Counters_InputC2F3_Get(u32 *ptr_value);
void Counters_InputC2F3_Set(u32 *ptr_value);

/* find input C1/C2 band */
u8   Counters_FindInputC1Band(CLOCK__TIME *ptr_actual_rtc_time);
u8   Counters_FindInputC2Band(CLOCK__TIME *ptr_actual_rtc_time);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "counter C1" status */
void Counters_Status_InputC1_GetDefault     (COUNTERS__STATUS__INPUT_C1       *ptr_data);
void Counters_Status_InputC1_Get            (COUNTERS__STATUS__INPUT_C1       *ptr_data);
void Counters_Status_InputC1_Set            (COUNTERS__STATUS__INPUT_C1       *ptr_data);

/* get/set "counter C2" status */
void Counters_Status_InputC2_GetDefault     (COUNTERS__STATUS__INPUT_C2       *ptr_data);
void Counters_Status_InputC2_Get            (COUNTERS__STATUS__INPUT_C2       *ptr_data);
void Counters_Status_InputC2_Set            (COUNTERS__STATUS__INPUT_C2       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "input C1" configuration */
void Counters_Config_InputC1_GetDefault     (COUNTERS__CONFIG__INPUT_C1       *ptr_data);
void Counters_Config_InputC1_Get            (COUNTERS__CONFIG__INPUT_C1       *ptr_data);
void Counters_Config_InputC1_Set            (COUNTERS__CONFIG__INPUT_C1       *ptr_data);
bool Counters_Config_InputC1_IsValid        (COUNTERS__CONFIG__INPUT_C1       *ptr_data);

/* get/set "input C2" configuration */
void Counters_Config_InputC2_GetDefault     (COUNTERS__CONFIG__INPUT_C2       *ptr_data);
void Counters_Config_InputC2_Get            (COUNTERS__CONFIG__INPUT_C2       *ptr_data);
void Counters_Config_InputC2_Set            (COUNTERS__CONFIG__INPUT_C2       *ptr_data);
bool Counters_Config_InputC2_IsValid        (COUNTERS__CONFIG__INPUT_C2       *ptr_data);


/* get/set "input C1 bands" configuration */
void Counters_Config_InputC1Bands_GetDefault(COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);
void Counters_Config_InputC1Bands_Get       (COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);
void Counters_Config_InputC1Bands_Set       (COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);
bool Counters_Config_InputC1Bands_IsValid   (COUNTERS__CONFIG__INPUT_C1_BANDS *ptr_data);

/* get/set "input C2 bands" configuration */
void Counters_Config_InputC2Bands_GetDefault(COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);
void Counters_Config_InputC2Bands_Get       (COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);
void Counters_Config_InputC2Bands_Set       (COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);
bool Counters_Config_InputC2Bands_IsValid   (COUNTERS__CONFIG__INPUT_C2_BANDS *ptr_data);




#endif
