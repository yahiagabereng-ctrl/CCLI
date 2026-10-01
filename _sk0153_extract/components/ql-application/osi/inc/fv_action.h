/*=============================================================================
 * File       :  FV_ACTION.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - FV actions
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __FV_ACTION_H__


#define __FV_ACTION_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPE
 *=============================================================================*/
/* configuration - "autorestore" */
typedef struct
{
    bool status;     /* enable status               [OFF/ON] */
    u8   wait;       /* wait for autorestore (hour) [0-96  ] */
} FV_ACTION__CONFIG__AUTORESTORE;


/* configuration - "autoreconnection" */
typedef struct
{
    bool status;     /* enable status        [OFF/ON] */
    u16  delay;      /* delay for OUT2 (sec) [0- 240] */
    u16  time;       /* time  for OUT2 (sec) [0-3600] */
} FV_ACTION__CONFIG__AUTORECONNECTION;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void FvAction_TaskFvAction(void *argument);

/* status variables */
void FvAction_UpdateVarCrc(void);
bool FvAction_VerifyVarCrc(void);

/* task events */
void FvAction_RequestDetachment(ascii *sms_phone_number);
void FvAction_RequestRestore   (ascii *sms_phone_number);

/* get/set "autorestore" configuration */
void FvAction_Config_AutoRestore_GetDefault     (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);
void FvAction_Config_AutoRestore_Get            (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);
void FvAction_Config_AutoRestore_Set            (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);
bool FvAction_Config_AutoRestore_IsValid        (FV_ACTION__CONFIG__AUTORESTORE      *ptr_data);

/* get/set "autoreconnection" configuration */
void FvAction_Config_AutoReconnection_GetDefault(FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);
void FvAction_Config_AutoReconnection_Get       (FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);
void FvAction_Config_AutoReconnection_Set       (FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);
bool FvAction_Config_AutoReconnection_IsValid   (FV_ACTION__CONFIG__AUTORECONNECTION *ptr_data);




#endif
