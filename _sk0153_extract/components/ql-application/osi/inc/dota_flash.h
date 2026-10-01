/*===========================================================================
 * File       :  DOTA_FLASH.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - backup flash manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *==========================================================================*/




#ifndef __DOTA_FLASH_H__


#define __DOTA_FLASH_H__




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
#define DOTA_FLASH__HANDLE_STRING__000_DOTA                    "DOTA - 000 - DOTA"     /* 000 - DOTA */


/*---------------------------------------------------------------------------
 * Data size (bytes)
 *---------------------------------------------------------------------------*/
#define DOTA_FLASH__DATA_SIZE__PHASE                           sizeof(DOTA_MAIN__DOTA_PHASE               )
#define DOTA_FLASH__DATA_SIZE__PHASE_1_ATTEMPTS                sizeof(DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS)
#define DOTA_FLASH__DATA_SIZE__PHASE_1_GPRS_APN_PARAMETERS     sizeof(DOTA_GPRS__GPRS_APN_PARAMETERS      )
#define DOTA_FLASH__DATA_SIZE__PHASE_1_GPRS_DNS_PARAMETERS     sizeof(DOTA_GPRS__GPRS_DNS_PARAMETERS      )
#define DOTA_FLASH__DATA_SIZE__PHASE_1_FTP_PARAMETERS          sizeof(DOTA_FTP__FTP_PARAMETERS            )
#define DOTA_FLASH__DATA_SIZE__DOTA_RESULT                     sizeof(DOTA_MAIN__DOTA_RESULT              )
#define DOTA_FLASH__DATA_SIZE__DOTA_PHONE_NUMBER               sizeof(DOTA_MAIN__DOTA_PHONE_NUMBER        )


/*---------------------------------------------------------------------------
 * File offset
 *---------------------------------------------------------------------------*/
/* IMPORTANT - Add new definitions at the end because current data Flash addresses must not be changed */
#define DOTA_FLASH__FILE_OFFSET__PHASE                         0
#define DOTA_FLASH__FILE_OFFSET__PHASE_1_ATTEMPTS              (DOTA_FLASH__FILE_OFFSET__PHASE                       + DOTA_FLASH__DATA_SIZE__PHASE                       + 2)
#define DOTA_FLASH__FILE_OFFSET__PHASE_1_GPRS_APN_PARAMETERS   (DOTA_FLASH__FILE_OFFSET__PHASE_1_ATTEMPTS            + DOTA_FLASH__DATA_SIZE__PHASE_1_ATTEMPTS            + 2)
#define DOTA_FLASH__FILE_OFFSET__PHASE_1_GPRS_DNS_PARAMETERS   (DOTA_FLASH__FILE_OFFSET__PHASE_1_GPRS_APN_PARAMETERS + DOTA_FLASH__DATA_SIZE__PHASE_1_GPRS_APN_PARAMETERS + 2)
#define DOTA_FLASH__FILE_OFFSET__PHASE_1_FTP_PARAMETERS        (DOTA_FLASH__FILE_OFFSET__PHASE_1_GPRS_DNS_PARAMETERS + DOTA_FLASH__DATA_SIZE__PHASE_1_GPRS_DNS_PARAMETERS + 2)
#define DOTA_FLASH__FILE_OFFSET__DOTA_RESULT                   (DOTA_FLASH__FILE_OFFSET__PHASE_1_FTP_PARAMETERS      + DOTA_FLASH__DATA_SIZE__PHASE_1_FTP_PARAMETERS      + 2)
#define DOTA_FLASH__FILE_OFFSET__DOTA_PHONE_NUMBER             (DOTA_FLASH__FILE_OFFSET__DOTA_RESULT                 + DOTA_FLASH__DATA_SIZE__DOTA_RESULT                 + 2)




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*---------------------------------------------------------------------------
 * Flash data
 *---------------------------------------------------------------------------*/
/* Flash object IDs */
typedef enum
{
    DOTA_FLASH__FLASH_ID__FIRST = 0,

    /*---------------------------------------------------------------------------
     * "000 - DOTA" - flash object IDs
     *---------------------------------------------------------------------------*/
    DOTA_FLASH__FLASH_ID__000_DOTA__FIRST                         = 0,
    DOTA_FLASH__FLASH_ID__000_DOTA__PHASE                         = DOTA_FLASH__FLASH_ID__000_DOTA__FIRST,              /* DOTA phase                         */
    DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS,                                                                   /* DOTA phase 1 - number of attempts  */
    DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS,                                                        /* DOTA phase 1 - GPRS APN parameters */
    DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS,                                                        /* DOTA phase 1 - GPRS DNS parameters */
    DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS,                                                             /* DOTA phase 1 - FTP      parameters */
    DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT,                                                                        /* DOTA result                        */
    DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER,                                                                  /* DOTA phone number                  */
    DOTA_FLASH__FLASH_ID__000_DOTA__LAST                          = DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER,

    DOTA_FLASH__FLASH_ID__LAST = DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER,
} DOTA_FLASH__FLASH_ID;

/* flash object handles indexes */
typedef enum
{
    DOTA_FLASH__HANDLE_INDEX__000_DOTA,  // 000 - Reset
} DOTA_FLASH__HANDLE_INDEX;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* flash init */
void DotaFlash_InitFlashAll (void);
bool DotaFlash_EraseFlashAll(void);

/* flash reading */
bool DotaFlash_ReadFlashAll(void);

/* request of single ID writing */
void DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX handle_index, DOTA_FLASH__FLASH_ID id_index);




#endif
