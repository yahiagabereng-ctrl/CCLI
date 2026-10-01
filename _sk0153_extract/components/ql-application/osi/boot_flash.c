/*===========================================================================
 * File       :  BOOT_FLASH.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  BOOT - backup flash manager
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
#include "boot_flash.h"
#include "boot.h"
#include "debug_my.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
#define BOOT_FLASH__FILE_PATH      "UFS:boot.bin"

/* maximum number of writing attempts to flash */
#define NUM_MAX_WRITING_BACKUP     3

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING    60




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii    BootFlash_DebugString[MAX_LENGTH_DEBUG_STRING + 1];


/*---------------------------------------------------------
 * get default functions
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static void (* const BootFlash_Functions_GetDefault[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))Boot_BootMode_GetDefault,                    // BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * get functions
 *---------------------------------------------------------*/
static void (* const BootFlash_Functions_Get[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))Boot_BootMode_Get,                           // BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * set functions
 *---------------------------------------------------------*/
static void (* const BootFlash_Functions_Set[])(u8 *) =
{
    //---------------------------------------------------------
    (void (*)(u8 *))Boot_BootMode_Set,                           // BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * data address
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static const u16 BootFlash_FileOffset[] =
{
    //---------------------------------------------------------
    BOOT_FLASH__FILE_OFFSET__BOOT_MODE,                          // BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE
    //---------------------------------------------------------
};


/*---------------------------------------------------------
 * data size (bytes)
 *---------------------------------------------------------*/
/* IMPORTANT - The order must to be the same of "PROGRAM_FLASH__FLASH_ID" enum definition */
static const u16 BootFlash_DataSize[] =
{
    //---------------------------------------------------------
    BOOT_FLASH__DATA_SIZE__BOOT_MODE,                            // BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE
    //---------------------------------------------------------
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* flash init */
       void BootFlash_InitFlashAll(void);

/* flash reading */
       bool BootFlash_ReadFlashAll(void);
static bool BootFlash_ReadFlashId (BOOT_FLASH__FLASH_ID id_index);

/* request of single ID writing */
       void BootFlash_RequestWriteBackup(BOOT_FLASH__HANDLE_INDEX handle_index, BOOT_FLASH__FLASH_ID id_index);

/* action on a single ID */
static bool BootFlash_BackupWriteDefault(BOOT_FLASH__FLASH_ID id_index);
static bool BootFlash_BackupWrite       (BOOT_FLASH__FLASH_ID id_index);
static bool BootFlash_BackupRead        (BOOT_FLASH__FLASH_ID id_index);
static bool BootFlash_BackupVerify      (BOOT_FLASH__FLASH_ID id_index);

/* ID size info */
static u16  BootFlash_SizeBackup(BOOT_FLASH__FLASH_ID id_index, u16 *ptr_data_address, u16 *ptr_data_size);




/*===========================================================================
 * Function   : BootFlash_InitFlashAll
 *
 * Description: prepare the flash, in order:
 *                 - erase                  unsupported IDs
 *                 - create (write) missing   supported IDs (with default values)
 * Input      : -
 * Output     : -
 *===========================================================================*/
void BootFlash_InitFlashAll(void)
{
    QFILE                file;

    BOOT_FLASH__FLASH_ID id_index;

    bool                 result_1;
    bool                 result_2;


    file = ql_fopen(BOOT_FLASH__FILE_PATH, "r+");
    if (file <= 0)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "ql_fopen ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        file = ql_fopen(BOOT_FLASH__FILE_PATH, "w+");
        if (file <= 0)
            return;

        ql_fclose(file);

        /* write default values in Flash */
        result_1 = TRUE;

        for (id_index = BOOT_FLASH__FLASH_ID__FIRST; id_index <= BOOT_FLASH__FLASH_ID__LAST; id_index++)
        {
            result_2 = BootFlash_BackupWriteDefault(id_index);
            if (!result_2)
            {
                snprintf(BootFlash_DebugString, sizeof(BootFlash_DebugString), "BootFlash_BackupWriteDefault ERROR - id_index: %d", id_index);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, BootFlash_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                result_1 = FALSE;
            }
        }

        if (!result_1)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "BootFlash_InitFlashAll ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        ql_fclose(file);
    }
}




/*===========================================================================
 * Function   : BootFlash_ReadFlashAll
 *
 * Description: read all (supported) IDs from flash and copy them to RAM
 * Inputs     : -
 * Outputs    : -
 *===========================================================================*/
bool BootFlash_ReadFlashAll(void)
{
    BOOT_FLASH__FLASH_ID id_index;

    bool                 result_1;
    bool                 result_2;


    result_1 = TRUE;

    for (id_index = BOOT_FLASH__FLASH_ID__FIRST; id_index <= BOOT_FLASH__FLASH_ID__LAST; id_index++)
    {
        result_2 = BootFlash_ReadFlashId(id_index);
        if (!result_2)
            result_1 = FALSE;
    }

    return result_1;
}




/*===========================================================================
 * Function   : BootFlash_ReadFlashId
 *
 * Description: read an ID (if supported) from flash and copy it to RAM
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool BootFlash_ReadFlashId(BOOT_FLASH__FLASH_ID id_index)
{
    bool result;


    /* reading of the ID of the handle */
    result = BootFlash_BackupRead(id_index);
    if (!result)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "BootFlash_BackupRead ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return result;
}




/*===========================================================================
 * Function   : BootFlash_RequestWriteBackup
 *
 * Description: signal a request of writing of an ID of a specified handle
 * Inputs     : - handle_index: handle index
 *              - id_index    : ID     index
 * Outputs    : -
 *===========================================================================*/
void BootFlash_RequestWriteBackup(BOOT_FLASH__HANDLE_INDEX handle_index, BOOT_FLASH__FLASH_ID id_index)
{
    u8   num_writings;

    bool result_1;
    bool result_2;


    /* backup writing */
    num_writings = 0;
    do
    {
        /* writing */
        result_1 = BootFlash_BackupWrite (id_index);
        if (!result_1)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "BootFlash_BackupWrite ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* verify */
        result_2 = BootFlash_BackupVerify(id_index);
        if (!result_2)
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FLASH, DEBUG_TRACE_TYPE_LOW, "BootFlash_BackupVerify ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        num_writings++;
    } while (((!result_1) || (!result_2)) && (num_writings < NUM_MAX_WRITING_BACKUP));
}




/*===========================================================================
 * Function   : BootFlash_BackupWriteDefault
 *
 * Description: write to backup the default values of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool BootFlash_BackupWriteDefault(BOOT_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;

    int    res_int;


    /* get data info (data address, data size) */
    BootFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(BOOT_FLASH__FILE_PATH, "r+");
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
    if (BootFlash_Functions_GetDefault[id_index])
        BootFlash_Functions_GetDefault[id_index](ptr_data);


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
 * Function   : BootFlash_BackupWrite
 *
 * Description: write to backup the values of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool BootFlash_BackupWrite(BOOT_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;

    int    res_int;


    /* get data info (data address, data size) */
    BootFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(BOOT_FLASH__FILE_PATH, "r+");
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
    if (BootFlash_Functions_Get[id_index])
        BootFlash_Functions_Get[id_index](ptr_data);


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
 * Function   : BootFlash_BackupRead
 *
 * Description: read from backup the backup values of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool BootFlash_BackupRead(BOOT_FLASH__FLASH_ID id_index)
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
    BootFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(BOOT_FLASH__FILE_PATH, "r+");
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
         (BootFlash_Functions_GetDefault[id_index]) &&
         (BootFlash_Functions_Set       [id_index])
       )
    {
        if (!crc_ok)
            BootFlash_Functions_GetDefault[id_index](ptr_data);
        BootFlash_Functions_Set           [id_index](ptr_data);
    }


    /* free allocated memory */
    if (ptr_data)
        free(ptr_data);


    return crc_ok;
}




/*===========================================================================
 * Function   : BootFlash_BackupVerify
 *
 * Description: verify the CRC verifica del CRC of an specified ID of an specified handle
 * Inputs     : - id_index: ID index
 * Outputs    : -
 *===========================================================================*/
static bool BootFlash_BackupVerify(BOOT_FLASH__FLASH_ID id_index)
{
    QFILE  file;

    u16    data_address;
    u16    data_size;
    u8    *ptr_data;

    u16    crc_calcolated;
    u16    crc_read;

    int    res_int;


    /* data size calculus */
    BootFlash_SizeBackup(id_index, &data_address, &data_size);
    if (data_size == 0)
        return FALSE;

    file = ql_fopen(BOOT_FLASH__FILE_PATH, "r+");
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
 * Function   : BootFlash_SizeBackup
 *
 * Description: get the data info (data address, data size) of the specified data ID
 * Inputs     : - id_index        : data ID
 *              - ptr_data_address: pointer to save data address
 *              - ptr_data_size   : pointer to save data size (bytes)
 * Outputs    : - FALSE: data info not got (the specified data ID does not exist)
 *              - TRUE : data info     got
 *===========================================================================*/
static u16 BootFlash_SizeBackup(BOOT_FLASH__FLASH_ID id_index, u16 *ptr_data_address, u16 *ptr_data_size)
{
    *ptr_data_address = BootFlash_FileOffset[id_index];
    *ptr_data_size    = BootFlash_DataSize  [id_index];

    return TRUE;
}
