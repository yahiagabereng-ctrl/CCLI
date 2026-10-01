/*=============================================================================
 * File       :  ALARM_INPUT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - digital inputs alarm manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __ALARM_INPUT_H__


#define __ALARM_INPUT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* input activation time unit */
#define ALARM_INPUT__ACTIVATION_TIME_UNIT__SECOND    0         /* seconds */
#define ALARM_INPUT__ACTIVATION_TIME_UNIT__MINUTE    1         /* minutes */
#define ALARM_INPUT__ACTIVATION_TIME_UNIT__HOUR      2         /* hours   */

/* input activation status */
#define ALARM_INPUT__ACTIVATION_STATUS__CLOSE        (FALSE)   /* close */
#define ALARM_INPUT__ACTIVATION_STATUS__OPEN         (TRUE )   /* open  */




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Status
 *-----------------------------------------------------------------------------*/
/* input 1 status */
typedef struct
{
    bool status_input;                          /* input status                      */
    bool status_change_timer;                   /* indication of status change timer */
} ALARM_INPUT__STATUS__INPUT_IN1;


/* input 2 status */
typedef struct
{
    bool status_input;                          /* input status                      */
    bool status_change_timer;                   /* indication of status change timer */
} ALARM_INPUT__STATUS__INPUT_IN2;


/* input 3 status */
typedef struct
{
    bool status_input;                          /* input status                      */
    bool status_change_timer;                   /* indication of status change timer */
} ALARM_INPUT__STATUS__INPUT_IN3;


/* input 4 status */
typedef struct
{
    bool status_input;                          /* input status                      */
    bool status_change_timer;                   /* indication of status change timer */
} ALARM_INPUT__STATUS__INPUT_IN4;


/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - IN1 digital input alarm */
typedef struct
{
    bool alarm_enabled_status_input;            /* digital input          alarm enabled status */
    bool alarm_enabled_status_input_restored;   /* digital input restored alarm enabled status */
} ALARM_INPUT__CONFIG__ALARM_INPUT_IN1;


/* configuration - IN2 digital input alarm */
typedef struct
{
    bool alarm_enabled_status_input;            /* digital input          alarm enabled status */
    bool alarm_enabled_status_input_restored;   /* digital input restored alarm enabled status */
} ALARM_INPUT__CONFIG__ALARM_INPUT_IN2;


/* configuration - IN3 digital input alarm */
typedef struct
{
    bool alarm_enabled_status_input;            /* digital input          alarm enabled status */
    bool alarm_enabled_status_input_restored;   /* digital input restored alarm enabled status */
} ALARM_INPUT__CONFIG__ALARM_INPUT_IN3;


/* configuration - IN4 digital input alarm */
typedef struct
{
    bool alarm_enabled_status_input;            /* digital input          alarm enabled status */
    bool alarm_enabled_status_input_restored;   /* digital input restored alarm enabled status */
} ALARM_INPUT__CONFIG__ALARM_INPUT_IN4;


/* configuration - IN1 digital input */
typedef struct
{
    bool activation_status;                     /* input activation status     */   // [open/close]
    u8   activation_time_value;                 /* input activation time value */   // [1-255]
    u8   activation_time_unit;                  /* input activation time unit  */   // [second/minute/hour]
} ALARM_INPUT__CONFIG__INPUT_IN1;


/* configuration - IN2 digital input */
typedef struct
{
    bool activation_status;                     /* input activation status     */   // [open/close]
    u8   activation_time_value;                 /* input activation time value */   // [1-255]
    u8   activation_time_unit;                  /* input activation time unit  */   // [second/minute/hour]
} ALARM_INPUT__CONFIG__INPUT_IN2;


/* configuration - IN3 digital input */
typedef struct
{
    bool activation_status;                     /* input activation status     */   // [open/close]
    u8   activation_time_value;                 /* input activation time value */   // [1-255]
    u8   activation_time_unit;                  /* input activation time unit  */   // [second/minute/hour]
} ALARM_INPUT__CONFIG__INPUT_IN3;


/* configuration - IN4 digital input */
typedef struct
{
    bool activation_status;                     /* input activation status     */   // [open/close]
    u8   activation_time_value;                 /* input activation time value */   // [1-255]
    u8   activation_time_unit;                  /* input activation time unit  */   // [second/minute/hour]
} ALARM_INPUT__CONFIG__INPUT_IN4;


/* configuration - IN3 digital input type */
typedef struct
{
    bool signal_type;                           /* signal type [FALSE: analog signal, TRUE: digital signal] */
} ALARM_INPUT__CONFIG__INPUT_IN3_TYPE;


/* configuration - IN4 digital input type */
typedef struct
{
    bool signal_type;                           /* signal type [0,1] */
} ALARM_INPUT__CONFIG__INPUT_IN4_TYPE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void AlarmInput_TaskAlarmInput(void *argument);

/* new signal type input configuration */
void AlarmInput_NewConfigSignalTypeIn3(ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_config_new, ALARM_INPUT__CONFIG__INPUT_IN3_TYPE *ptr_config_old);
void AlarmInput_NewConfigSignalTypeIn4(ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_config_new, ALARM_INPUT__CONFIG__INPUT_IN4_TYPE *ptr_config_old);


/*-----------------------------------------------------------------------------
 * get/set status
 *-----------------------------------------------------------------------------*/
/* get/set "input 2 status" */
void AlarmInput_Status_InputIn1_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN1       *ptr_data);
void AlarmInput_Status_InputIn1_Get            (ALARM_INPUT__STATUS__INPUT_IN1       *ptr_data);
void AlarmInput_Status_InputIn1_Set            (ALARM_INPUT__STATUS__INPUT_IN1       *ptr_data);

/* get/set "input 2 status" */
void AlarmInput_Status_InputIn2_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN2       *ptr_data);
void AlarmInput_Status_InputIn2_Get            (ALARM_INPUT__STATUS__INPUT_IN2       *ptr_data);
void AlarmInput_Status_InputIn2_Set            (ALARM_INPUT__STATUS__INPUT_IN2       *ptr_data);

/* get/set "input 3 status" */
void AlarmInput_Status_InputIn3_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN3       *ptr_data);
void AlarmInput_Status_InputIn3_Get            (ALARM_INPUT__STATUS__INPUT_IN3       *ptr_data);
void AlarmInput_Status_InputIn3_Set            (ALARM_INPUT__STATUS__INPUT_IN3       *ptr_data);

/* get/set "input 4 status" */
void AlarmInput_Status_InputIn4_GetDefault     (ALARM_INPUT__STATUS__INPUT_IN4       *ptr_data);
void AlarmInput_Status_InputIn4_Get            (ALARM_INPUT__STATUS__INPUT_IN4       *ptr_data);
void AlarmInput_Status_InputIn4_Set            (ALARM_INPUT__STATUS__INPUT_IN4       *ptr_data);


/*-----------------------------------------------------------------------------
 * get/set configurations
 *-----------------------------------------------------------------------------*/
/* get/set "IN1 digital input alarm" configuration */
void AlarmInput_Config_AlarmInputIn1_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);
void AlarmInput_Config_AlarmInputIn1_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);
void AlarmInput_Config_AlarmInputIn1_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);
bool AlarmInput_Config_AlarmInputIn1_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN1 *ptr_data);

/* get/set "IN2 digital input alarm" configuration */
void AlarmInput_Config_AlarmInputIn2_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);
void AlarmInput_Config_AlarmInputIn2_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);
void AlarmInput_Config_AlarmInputIn2_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);
bool AlarmInput_Config_AlarmInputIn2_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN2 *ptr_data);

/* get/set "IN3 digital input alarm" configuration */
void AlarmInput_Config_AlarmInputIn3_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);
void AlarmInput_Config_AlarmInputIn3_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);
void AlarmInput_Config_AlarmInputIn3_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);
bool AlarmInput_Config_AlarmInputIn3_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN3 *ptr_data);

/* get/set "IN4 digital input alarm" configuration */
void AlarmInput_Config_AlarmInputIn4_GetDefault(ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);
void AlarmInput_Config_AlarmInputIn4_Get       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);
void AlarmInput_Config_AlarmInputIn4_Set       (ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);
bool AlarmInput_Config_AlarmInputIn4_IsValid   (ALARM_INPUT__CONFIG__ALARM_INPUT_IN4 *ptr_data);

/* get/set "IN1 digital input"       configuration */
void AlarmInput_Config_InputIn1_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);
void AlarmInput_Config_InputIn1_Get            (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);
void AlarmInput_Config_InputIn1_Set            (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);
bool AlarmInput_Config_InputIn1_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN1       *ptr_data);

/* get/set "IN2 digital input"       configuration */
void AlarmInput_Config_InputIn2_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);
void AlarmInput_Config_InputIn2_Get            (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);
void AlarmInput_Config_InputIn2_Set            (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);
bool AlarmInput_Config_InputIn2_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN2       *ptr_data);

/* get/set "IN3 digital input"       configuration */
void AlarmInput_Config_InputIn3_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);
void AlarmInput_Config_InputIn3_Get            (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);
void AlarmInput_Config_InputIn3_Set            (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);
bool AlarmInput_Config_InputIn3_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN3       *ptr_data);

/* get/set "IN4 digital input"       configuration */
void AlarmInput_Config_InputIn4_GetDefault     (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);
void AlarmInput_Config_InputIn4_Get            (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);
void AlarmInput_Config_InputIn4_Set            (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);
bool AlarmInput_Config_InputIn4_IsValid        (ALARM_INPUT__CONFIG__INPUT_IN4       *ptr_data);

/* get/set "IN3 digital input type"  configuration */
void AlarmInput_Config_InputIn3Type_GetDefault (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);
void AlarmInput_Config_InputIn3Type_Get        (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);
void AlarmInput_Config_InputIn3Type_Set        (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);
bool AlarmInput_Config_InputIn3Type_IsValid    (ALARM_INPUT__CONFIG__INPUT_IN3_TYPE  *ptr_data);

/* get/set "IN4 digital input type"  configuration */
void AlarmInput_Config_InputIn4Type_GetDefault (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);
void AlarmInput_Config_InputIn4Type_Get        (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);
void AlarmInput_Config_InputIn4Type_Set        (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);
bool AlarmInput_Config_InputIn4Type_IsValid    (ALARM_INPUT__CONFIG__INPUT_IN4_TYPE  *ptr_data);




#endif
