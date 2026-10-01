/*===========================================================================
 * File       :  DOTA_FLASH.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - backup flash manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *==========================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdlib.h>

/* API      includes */
#include "ql_fs.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "dota_flash.h"
#include "dota_ftp.h"
#include "dota_gprs.h"
#include "dota_main.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
#define DOTA_FLASH__FILE_PATH      "UFS:dota.bin"

/* maximum number of writing attempts to flash */
#define NUM_MAX_WRITING_BACKUP     3

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING    200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii    DotaFlash_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------
 * get default functions
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "DOTA_FLASH__FLASH_ID" enum definition */
static void (* const DotaFlash_Functions_GetDefault[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))DotaMain_DotaPhase_GetDefault,                       // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE
    (void (*)(u8 *))DotaMain_DotaPhase1NumAttempts_GetDefault,           // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS
    (void (*)(u8 *))DotaGprs_Config_ApnParameters_GetDefault,            // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS
    (void (*)(u8 *))DotaGprs_Config_DnsParameters_GetDefault,            // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS
    (void (*)(u8 *))DotaFtp_Config_FtpParameters_GetDefault,             // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS
    (void (*)(u8 *))DotaMain_DotaResult_GetDefault,                      // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT
    (void (*)(u8 *))DotaMain_DotaPhoneNumber_GetDefault,                 // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * get functions
 *---------------------------------------------------------*/
static void (* const DotaFlash_Functions_Get[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))DotaMain_DotaPhase_Get,                              // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE
    (void (*)(u8 *))DotaMain_DotaPhase1NumAttempts_Get,                  // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS
    (void (*)(u8 *))DotaGprs_Config_ApnParameters_Get,                   // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS
    (void (*)(u8 *))DotaGprs_Config_DnsParameters_Get,                   // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS
    (void (*)(u8 *))DotaFtp_Config_FtpParameters_Get,                    // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS
    (void (*)(u8 *))DotaMain_DotaResult_Get,                             // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT
    (void (*)(u8 *))DotaMain_DotaPhoneNumber_Get,                        // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * set functions
 *---------------------------------------------------------*/
static void (* const DotaFlash_Functions_Set[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))DotaMain_DotaPhase_Set,                              // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE
    (void (*)(u8 *))DotaMain_DotaPhase1NumAttempts_Set,                  // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS
    (void (*)(u8 *))DotaGprs_Config_ApnParameters_Set,                   // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS
    (void (*)(u8 *))DotaGprs_Config_DnsParameters_Set,                   // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS
    (void (*)(u8 *))DotaFtp_Config_FtpParameters_Set,                    // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS
    (void (*)(u8 *))DotaMain_DotaResult_Set,                             // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT
    (void (*)(u8 *))DotaMain_DotaPhoneNumber_Set,                        // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * data address
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "DOTA_FLASH__FLASH_ID" enum definition */
static const u16 DotaFlash_FileOffset[] =
{
    //---------------------------------------------------------
    DOTA_FLASH__FILE_OFFSET__PHASE,                                      // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE
    DOTA_FLASH__FILE_OFFSET__PHASE_1_ATTEMPTS,                           // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS
    DOTA_FLASH__FILE_OFFSET__PHASE_1_GPRS_APN_PARAMETERS,                // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS
    DOTA_FLASH__FILE_OFFSET__PHASE_1_GPRS_DNS_PARAMETERS,                // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS
    DOTA_FLASH__FILE_OFFSET__PHASE_1_FTP_PARAMETERS,                     // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS
    DOTA_FLASH__FILE_OFFSET__DOTA_RESULT,                                // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT
    DOTA_FLASH__FILE_OFFSET__DOTA_PHONE_NUMBER,                          // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * data size (bytes)
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static const u16 DotaFlash_DataSize[] =
{
    //---------------------------------------------------------
    DOTA_FLASH__DATA_SIZE__PHASE,                                        // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE
    DOTA_FLASH__DATA_SIZE__PHASE_1_ATTEMPTS,                             // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS
    DOTA_FLASH__DATA_SIZE__PHASE_1_GPRS_APN_PARAMETERS,                  // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_APN_PARAMETERS
    DOTA_FLASH__DATA_SIZE__PHASE_1_GPRS_DNS_PARAMETERS,                  // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_GPRS_DNS_PARAMETERS
    DOTA_FLASH__DATA_SIZE__PHASE_1_FTP_PARAMETERS,                       // DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_FTP_PARAMETERS
    DOTA_FLASH__DATA_SIZE__DOTA_RESULT,                                  // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT
    DOTA_FLASH__DATA_SIZE__DOTA_PHONE_NUMBER,                            // DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_PHONE_NUMBER
    //---------------------------------------------------------
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* flash init */
       void DotaFlash_InitFlashAll (void);
       bool DotaFlash_EraseFlashAll(void);

/* flash reading */
       bool DotaFlash_ReadFlashAll(void);
static bool DotaFlash_ReadFlashId (DOTA_FLASH__FLASH_ID id_index);

/* request of single ID writing */
       void DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX handle_index, DOTA_FLASH__FLASH_ID id_index);

/* action on a single ID */
static bool DotaFlash_BackupWriteDefault(DOTA_FLASH__FLASH_ID id_index);
static bool DotaFlash_BackupWrite       (DOTA_FLASH__FLASH_ID id_index);
static bool DotaFlash_BackupRead        (DOTA_FLASH__FLASH_ID id_index);
static bool DotaFlash_BackupVerify      (DOTA_FLASH__FLASH_ID id_index);

/* ID size info */
static bool DotaFlash_SizeBackup(DOTA_FLASH__FLASH_ID id_index, u16 *ptr_data_address, u16 *ptr_data_size);




/*===========================================================================
 * Function   : DotaFlash_InitFlashAll
 *
 * Description: prepare the flash, in order:
 *                 - erase                  unsupported IDs
 *                 - create (write) missing   supported IDs (with default values)
 * Input      : -
 * Output     : -
 *===========================================================================*/
void DotaFlash_InitFlashAll(void)
{
    QFILE file;
    bool  result;


    file = ql_fopen(DOTA_FLASH__FILE_PATH, "r+");
    if (file <= 0)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "ql_fopen ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        result = DotaFlash_EraseFlashAll();
        if (!result)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "DotaFlash_InitFlashAll ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        ql_fclose(file);
    }
}




/*===========================================================================
 * Function   : DotaFlash_EraseFlashAll
 *
 * Description: erase the flash, in order:
 *                 - erase all IDs
 * Input      : -
 * Output     : -
 *===========================================================================*/
bool DotaFlash_EraseFlashAll(void)
{
    DOTA_FLASH__FLASH_ID id_index;

    QFILE                file;

    bool                 result_1;
    bool                 result_2;


    file = ql_fopen(DOTA_FLASH__FILE_PATH, "w+");
    if (file <= 0)
        return FALSE;

    ql_fclose(file);

    /* write default values in Flash */
    result_1 = TRUE;

    for (id_index = DOTA_FLASH__FLASH_ID__FIRST; id_index <= DOTA_FLASH__FLASH_ID__LAST; id_index++)
    {
        result_2 = DotaFlash_BackupWriteDefault(id_index);
        if (!result_2)
        {
            snprintf(DotaFlash_DebugString, sizeof(DotaFlash_DebugString), "DotaFlash_BackupWriteDefault ERROR - id_index: %d", id_index);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, DotaFlash_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            result_1 = FALSE;
        }
    }

    return result_1;
}




/*===========================================================================
 * Function   : DotaFlash_ReadFlashAll
 *
 * Description: read all (supported) IDs from flash and copy them to RAM
 * Inputs     : -
 * Outputs    : -
 *===========================================================================*/
bool DotaFlash_ReadFlashAll(void)
{
    DOTA_FLASH__FLASH_ID id_index;

    bool                 result_1;
    bool                 result_2;


    result_1 = TRUE;

    for (id_index = DOTA_FLASH__FLASH_ID__FIRST; id_index <= DOTA_FLASH__FLASH_ID__LAST; id_index++)
    {
        result_2 = DotaFlash_ReadFlashId(id_index);
        if (!result_2)
            result_1 = FALSE;
    }

    return result_1;
}




/*===========================================================================
 * Function   : DotaFlash_ReadFlashId
 *
 * Description: read an ID (if supported) from flash and copy it to RAM
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool DotaFlash_ReadFlashId(DOTA_FLASH__FLASH_ID id_index)
{
    bool result;


    /* reading of the ID of the handle */
    result = DotaFlash_BackupRead(id_index);
    if (!result)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "DotaFlash_BackupRead ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return result;
}




/*===========================================================================
 * Function   : DotaFlash_RequestWriteBackup
 *
 * Description: signal a request of writing of an ID of a specified handle
 * Inputs     : - handle_index: handle index
 *              - id_index    : ID     index
 * Outputs    : -
 *===========================================================================*/
void DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX handle_index, DOTA_FLASH__FLASH_ID id_index)
{
    u8   num_writings;

    bool result_1;
    bool result_2;


    /* backup writing */
    num_writings = 0;
    do
    {
        /* writing */
        result_1 = DotaFlash_BackupWrite (id_index);
        if (!result_1)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "DotaFlash_BackupWrite ERROR" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        else
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "DotaFlash_BackupWrite OK"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* verify */
        result_2 = DotaFlash_BackupVerify(id_index);
        if (!result_2)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "DotaFlash_BackupVerify ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        else
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "DotaFlash_BackupVerify OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        num_writings++;
    } while (((!result_1) || (!result_2)) && (num_writings < NUM_MAX_WRITING_BACKUP));
}




/*===========================================================================
 * Function   : DotaFlash_BackupWriteDefault
 *
 * Description: write to backup the default values of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool DotaFlash_BackupWriteDefault(DOTA_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;

    int    res_int;


    /* get data info (data address, data size) */
    DotaFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(DOTA_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        
        ql_fclose(file);
        return FALSE;
    }


    /* get the variables from RAM */
    if (DotaFlash_Functions_GetDefault[id_index])
        DotaFlash_Functions_GetDefault[id_index](ptr_data);


    if (data_size == 0)
    {
        /* free allocated memory */
        if (ptr_data)
            free(ptr_data);

        return FALSE;
    }


    /* CRC calculus */
    crc_calcolated = Utility_CalculateCRC16(ptr_data, data_size, 0);
    *(ptr_data + data_size    ) = (u8)((crc_calcolated >> 8) & 0x00FF);
    *(ptr_data + data_size + 1) = (u8)((crc_calcolated     ) & 0x00FF);


    /* writing to flash (data + CRC) */
    res_int = ql_fwrite((void *)ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* free allocated memory */
    if (ptr_data)
        free(ptr_data);

    if (res_int != (int)(data_size + 2))
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_fwrite - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : DotaFlash_BackupWrite
 *
 * Description: write to backup the values of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool DotaFlash_BackupWrite(DOTA_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;

    int    res_int;


    /* get data info (data address, data size) */
    DotaFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(DOTA_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        
        ql_fclose(file);
        return FALSE;
    }


    /* get the variables from RAM */
    if (DotaFlash_Functions_Get[id_index])
        DotaFlash_Functions_Get[id_index](ptr_data);


    if (data_size == 0)
    {
        /* free memory allocation */
        if (ptr_data)
            free(ptr_data);

        return FALSE;
    }


    /* CRC calculus */
    crc_calcolated = Utility_CalculateCRC16(ptr_data, data_size, 0);
    *(ptr_data + data_size    ) = (u8)((crc_calcolated >> 8) & 0x00FF);
    *(ptr_data + data_size + 1) = (u8)((crc_calcolated     ) & 0x00FF);


    /* writing to flash (data + CRC) */
    res_int = ql_fwrite((void *)ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* free memory allocation */
    if (ptr_data)
        free(ptr_data);

    if (res_int != (int)(data_size + 2))
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_fwrite - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : DotaFlash_BackupRead
 *
 * Description: read from backup the backup values of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool DotaFlash_BackupRead(DOTA_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;
    u16    crc_read;
    bool   crc_ok;

    int    res_int;


    /* get data info (data address, data size) */
    DotaFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(DOTA_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        
        ql_fclose(file);
        return FALSE;
    }


    /* reading from flash objects (data + CRC) */
    res_int = ql_fread((void *)ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* CRC verify */
    if (res_int == (int)(data_size + 2))
    {
        crc_read       = (((u16)(*(ptr_data + data_size    ))) << 8) |
                         (((u16)(*(ptr_data + data_size + 1)))     );
        crc_calcolated = Utility_CalculateCRC16(ptr_data, data_size, 0);

        if (crc_calcolated == crc_read)
            crc_ok = TRUE;
        else
            crc_ok = FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_fread - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        crc_ok = FALSE;
    }


    /* set the variables in RAM */
    if (
         (DotaFlash_Functions_GetDefault[id_index]) &&
         (DotaFlash_Functions_Set       [id_index])
       )
    {
        if (!crc_ok)
            DotaFlash_Functions_GetDefault[id_index](ptr_data);
        DotaFlash_Functions_Set           [id_index](ptr_data);
    }


    /* free allocated memory */
    if (ptr_data)
        free(ptr_data);


    return crc_ok;
}




/*===========================================================================
 * Function   : DotaFlash_BackupVerify
 *
 * Description: verify the CRC verifica del CRC of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool DotaFlash_BackupVerify(DOTA_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;
    u16    crc_read;

    int    res_int;


    /* data size calculus */
    DotaFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(DOTA_FLASH__FILE_PATH, "r+");
    if (file <= 0)
        return FALSE;

    res_int = ql_fseek(file, (long)data_address, QL_SEEK_SET);
    if (res_int < 0)
    {
        ql_fclose(file);
        return FALSE;
    }

    /* memory allocation (data + CRC) */
    ptr_data = (u8 *)malloc(data_size + 2);
    if (ptr_data == NULL)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        
        ql_fclose(file);
        return FALSE;
    }


    /* reading from flash objects (data + CRC) */
    res_int = ql_fread((void *)ptr_data, 1, data_size + 2, file);
    ql_fclose(file);


    /* verify CRC */
    crc_read       = (((u16)(*(ptr_data + data_size    ))) << 8) |
                     (((u16)(*(ptr_data + data_size + 1)))     );
    crc_calcolated = Utility_CalculateCRC16(ptr_data, data_size, 0);


    /* free allocated memory */
    if (ptr_data)
        free(ptr_data);


    if (crc_calcolated == crc_read)
        return TRUE;
    else
        return FALSE;
}




/*===========================================================================
 * Function   : DotaFlash_SizeBackup
 *
 * Description: get the data info (data address, data size) of the specified data ID
 * Inputs     : - id_index        : data ID
 *              - ptr_data_address: pointer to save data address
 *              - ptr_data_size   : pointer to save data size (bytes)
 * Outputs    : - FALSE: data info not got (the specified data ID does not exist)
 *              - TRUE : data info     got
 *===========================================================================*/
static bool DotaFlash_SizeBackup(DOTA_FLASH__FLASH_ID id_index, u16 *ptr_data_address, u16 *ptr_data_size)
{
    *ptr_data_address = DotaFlash_FileOffset[id_index];
    *ptr_data_size    = DotaFlash_DataSize  [id_index];

    return TRUE;
}
