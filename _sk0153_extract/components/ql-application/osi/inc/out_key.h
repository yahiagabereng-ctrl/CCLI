/*=============================================================================
 * File       :  OUT_KEY.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - digital output keys
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __OUT_KEY_H__


#define __OUT_KEY_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* configuration - buttons */
typedef struct
{
    bool status;    /* status */
} OUT_KEY__CONFIG__BUTTONS;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
void OutKey_Init(void);

/* get/set "buttons" configuration */
void OutKey_Config_Buttons_GetDefault(OUT_KEY__CONFIG__BUTTONS *ptr_data);
void OutKey_Config_Buttons_Get       (OUT_KEY__CONFIG__BUTTONS *ptr_data);
void OutKey_Config_Buttons_Set       (OUT_KEY__CONFIG__BUTTONS *ptr_data);
bool OutKey_Config_Buttons_IsValid   (OUT_KEY__CONFIG__BUTTONS *ptr_data);

/* "output keys" block/unblock */
void OutKey_OutKeysBlock  (void);
void OutKey_OutKeysUnblock(void);




#endif
