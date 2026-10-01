/*===========================================================================
 * File       :  BOOT_FLASH.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  BOOT - backup flash manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *==========================================================================*/




#ifndef __BOOT_FLASH_H__


#define __BOOT_FLASH_H__




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/

/*---------------------------------------------------------------------------
 * Handler definition
 *---------------------------------------------------------------------------*/

/*
 * Note: It is not  allowed to delete or modify the values defined below
 *       It is only allowed to append new values (new definitions)
 */

/* flash object handles strings */
#define BOOT_FLASH__HANDLE_STRING__000_BOOT_MODE          "BOOT - 000 - Boot mode"     /* 000 - BOOT mode */


/*---------------------------------------------------------------------------
 * Data size (bytes)
 *---------------------------------------------------------------------------*/
#define BOOT_FLASH__DATA_SIZE__BOOT_MODE                  sizeof(BOOT__BOOT_MODE)


/*---------------------------------------------------------------------------
 * File offset
 *---------------------------------------------------------------------------*/
/* IMPORTANT - Add new definitions at the end because current data Flash addresses must not be changed */
#define BOOT_FLASH__FILE_OFFSET__BOOT_MODE                0




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*---------------------------------------------------------------------------
 * Flash data
 *---------------------------------------------------------------------------*/
/* Flash object IDs */
typedef enum
{
    BOOT_FLASH__FLASH_ID__FIRST = 0,

    /*---------------------------------------------------------------------------
     * "000 - BOOT mode" - flash object IDs
     *---------------------------------------------------------------------------*/
    BOOT_FLASH__FLASH_ID__000_BOOT_MODE__FIRST       = 0,
    BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE   = BOOT_FLASH__FLASH_ID__000_BOOT_MODE__FIRST,       /* boot mode */
    BOOT_FLASH__FLASH_ID__000_BOOT_MODE__LAST        = BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE,

    BOOT_FLASH__FLASH_ID__LAST = BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE,
} BOOT_FLASH__FLASH_ID;

/* flash object handles indexes */
typedef enum
{
    BOOT_FLASH__HANDLE_INDEX__000_BOOT_MODE,   /* 000 - BOOT mode */
} BOOT_FLASH__HANDLE_INDEX;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* flash init */
void BootFlash_InitFlashAll(void);

/* flash reading */
bool BootFlash_ReadFlashAll(void);

/* request of single ID writing */
void BootFlash_RequestWriteBackup(BOOT_FLASH__HANDLE_INDEX handle_index, BOOT_FLASH__FLASH_ID id_index);




#endif
