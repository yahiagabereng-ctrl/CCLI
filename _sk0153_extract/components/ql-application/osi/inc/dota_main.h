/*=============================================================================
 * File       :  DOTA_MAIN.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - main
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __DOTA_MAIN_H__


#define __DOTA_MAIN_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* DOTA phases */
#define DOTA_MAIN__DOTA_PHASE_1                              1      /* DOTA phase 1 (DOTA     phase) */
#define DOTA_MAIN__DOTA_PHASE_2                              2      /* DOTA phase 2 (cleaning phase) */

/* DOTA result */
#define DOTA_MAIN__DOTA_RESULT__UNKNOWN                      0      /* DOTA result - unknown */
#define DOTA_MAIN__DOTA_RESULT__ERROR                        1      /* DOTA result - error   */
#define DOTA_MAIN__DOTA_RESULT__SUCCESS                      2      /* DOTA result - success */

/* DOTA result - error codes */
#define DOTA_MAIN__DOTA_RESULT__ERROR__UNKNOWN               0      /* DOTA result error code - unknown                     */
#define DOTA_MAIN__DOTA_RESULT__ERROR__NO_ERROR              1      /* DOTA result error code - no error                    */
#define DOTA_MAIN__DOTA_RESULT__ERROR__OTHER_ERROR           2      /* DOTA result error code - other error                 */
#define DOTA_MAIN__DOTA_RESULT__ERROR__NO_GSM_REG            3      /* DOTA result error code - no GSM network registration */
#define DOTA_MAIN__DOTA_RESULT__ERROR__NO_GPRS_APN           4      /* DOTA result error code - no GPRS APN   connection    */
#define DOTA_MAIN__DOTA_RESULT__ERROR__NO_FTP_SERVER         5      /* DOTA result error code - no FTP server connection    */
#define DOTA_MAIN__DOTA_RESULT__ERROR__NO_FTP_FILE_DOWNLOAD  6      /* DOTA result error code - no FTP file download        */
#define DOTA_MAIN__DOTA_RESULT__ERROR__NO_FILE_INSTALL       7      /* DOTA result error code - no file install             */

/* DOTA phone number - maximum phone number length */
#define DOTA_MAIN__MAX_LENGTH_PHONE_NUMBER                   20




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* DOTA phase */
typedef u8   DOTA_MAIN__DOTA_PHASE;

/* DOTA phase 1 - number of attempts */
typedef u8   DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS;

/* DOTA result */
typedef struct
{
    bool  dota_result_stored;                                         // DOTA result stored [FALSE, TRUE]
    u8    dota_result;                                                // DOTA result
    u8    error_code;                                                 // DOTA result error code
} DOTA_MAIN__DOTA_RESULT;

/* DOTA phone number */
typedef struct
{
    ascii dota_phone_number[DOTA_MAIN__MAX_LENGTH_PHONE_NUMBER + 1];  // DOTA phone number
} DOTA_MAIN__DOTA_PHONE_NUMBER;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
void Dota_TaskDota(void *argument);

/* auto mode */
void DotaMain_EnableAutoMode   (void);
void DotaMain_DisableAutoMode  (void);
bool DotaMain_GetAutoModeStatus(void);

/* errors */
void DotaMain_SystemError(void);
void DotaMain_ResetModule(void);

/* get/set DOTA phase */
void DotaMain_DotaPhase_GetDefault            (DOTA_MAIN__DOTA_PHASE                *ptr_data);
void DotaMain_DotaPhase_Get                   (DOTA_MAIN__DOTA_PHASE                *ptr_data);
void DotaMain_DotaPhase_Set                   (DOTA_MAIN__DOTA_PHASE                *ptr_data);

/* get/set DOTA phase 1 number of attempts */
void DotaMain_DotaPhase1NumAttempts_GetDefault(DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data);
void DotaMain_DotaPhase1NumAttempts_Get       (DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data);
void DotaMain_DotaPhase1NumAttempts_Set       (DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data);

/* get/set DOTA result */
void DotaMain_DotaResult_GetDefault           (DOTA_MAIN__DOTA_RESULT               *ptr_data);
void DotaMain_DotaResult_Get                  (DOTA_MAIN__DOTA_RESULT               *ptr_data);
void DotaMain_DotaResult_Set                  (DOTA_MAIN__DOTA_RESULT               *ptr_data);

/* get/set DOTA phone number */
void DotaMain_DotaPhoneNumber_GetDefault      (DOTA_MAIN__DOTA_PHONE_NUMBER         *ptr_data);
void DotaMain_DotaPhoneNumber_Get             (DOTA_MAIN__DOTA_PHONE_NUMBER         *ptr_data);
void DotaMain_DotaPhoneNumber_Set             (DOTA_MAIN__DOTA_PHONE_NUMBER         *ptr_data);




#endif
