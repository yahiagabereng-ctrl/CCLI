/*=============================================================================
 * File       :  BOOT.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  BOOT - boot
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __BOOT_H__


#define __BOOT_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "ql_api_osi.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* boot modes */
#define BOOT__PROGRAM_MODE             0    // Program mode
#define BOOT__DOTA_MODE                1    // DOTA    mode




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* boot mode */
typedef unsigned char   BOOT__BOOT_MODE;


/* Task definition */
typedef struct
{
    void                 (*taskStart)(void *argument);  // pointer to task entry point
    unsigned int           stackSize;                   // number of bytes in task stack area
    char                  *taskName;                    // task name
    uint32                 event_count;                 // max event count
    APP_ThreadPriority_e   priority;                    // task priority
} adl_InitTasks_t;




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
extern ql_task_t Boot_TaskRef_Boot;
extern ql_task_t Boot_TaskRef_Startup;
extern ql_task_t Boot_TaskRef_Counters;
extern ql_task_t Boot_TaskRef_DrvGpio;
extern ql_task_t Boot_TaskRef_Led;
extern ql_task_t Boot_TaskRef_Main;
extern ql_task_t Boot_TaskRef_UiLed;
extern ql_task_t Boot_TaskRef_RtcAlarm;
extern ql_task_t Boot_TaskRef_Phone;
extern ql_task_t Boot_TaskRef_TransmissionMail;
extern ql_task_t Boot_TaskRef_TransmissionGprs;
extern ql_task_t Boot_TaskRef_TransmissionSms;
extern ql_task_t Boot_TaskRef_DrvTemperature;
extern ql_task_t Boot_TaskRef_Regulation;
extern ql_task_t Boot_TaskRef_Outputs;
extern ql_task_t Boot_TaskRef_FvAction;
extern ql_task_t Boot_TaskRef_FAntifrost;
extern ql_task_t Boot_TaskRef_FRegulation;
extern ql_task_t Boot_TaskRef_FChrono;
extern ql_task_t Boot_TaskRef_Energy;
extern ql_task_t Boot_TaskRef_AlarmTMax;
extern ql_task_t Boot_TaskRef_AlarmTMin;
extern ql_task_t Boot_TaskRef_AlarmInput;
extern ql_task_t Boot_TaskRef_AlarmPower;
extern ql_task_t Boot_TaskRef_Alarm;
extern ql_task_t Boot_TaskRef_LogStatus;
extern ql_task_t Boot_TaskRef_InputEvent;
extern ql_task_t Boot_TaskRef_Charger;
extern ql_task_t Boot_TaskRef_Synchronize;
extern ql_task_t Boot_TaskRef_Calendar;
extern ql_task_t Boot_TaskRef_ProgramFlash;
extern ql_task_t Boot_TaskRef_Dota;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Tasks
 *-----------------------------------------------------------------------------*/
/* Boot tasks */
void Boot_TaskBoot(void *argument);


/*---------------------------------------------------------------------------
 * Actions on phone
 *---------------------------------------------------------------------------*/
/* GPRS connection */
bool Boot_GprsConnectionStart(void);
bool Boot_GprsConnectionStop(void);
bool Boot_ClientHttpGetRxRequest(void);
bool Boot_ClientHttpGetTxRequest(void);
bool Boot_ClientHttpPostRequest(void);
bool Boot_ClientSmtpRequest(void);


/*-----------------------------------------------------------------------------
 * ???
 *-----------------------------------------------------------------------------*/
/* get/set boot mode */
void Boot_BootMode_GetDefault(BOOT__BOOT_MODE *ptr_data);
void Boot_BootMode_Get       (BOOT__BOOT_MODE *ptr_data);
void Boot_BootMode_Set       (BOOT__BOOT_MODE *ptr_data);

/* init */
void ql_sk_app_init(void);




#endif
