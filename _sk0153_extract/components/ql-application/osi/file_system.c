/*=============================================================================
 * File       :  FILE_SYSTEM.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - file system
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDE
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* API      includes */
#include "ql_fs.h"

/* user     includes */
#include "clock.h"
#include "debug_my.h"
#include "file_system.h"
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* header file name */
#define HEADER_FILE_NAME__UNKNOWN            "EASY__X"            // header file name - unknown
#define HEADER_FILE_NAME__LOG                "EASY__L"            // header file name - log   file
#define HEADER_FILE_NAME__ALARM              "EASY__A"            // header file name - alarm file

/* status file strings */
#define STATUS_FILE_STRING__NULL             ""                   // status file - null
#define STATUS_FILE_STRING__BUILD            "(build)"            // status file - "build"
#define STATUS_FILE_STRING__CLOSE            "(close)"            // status file - "close"
#define STATUS_FILE_STRING__SEND             "(send)"             // status file - "send"
#define STATUS_FILE_STRING__SENT             "(sent)"             // status file - "sent"

/* limits on the stored files */
#define LIMITS__MAX_NUMBER_OF_FILES__LOG     48                   // maximum number     of stored files         - log
#define LIMITS__MAX_NUMBER_OF_FILES__ALARM   12                   // maximum number     of stored files         - alarm
#define LIMITS__MAX_TOTAL_SIZE_OF_FILES      (1 * 1024 * 1024L)   // maximum total size of stored files (bytes) - IT MUST TO BE LESS THAN THE FILE SYSTEM SIZE (AT+WOPEN=9)

/* maximum number of wildcard terms in the pattern to limit recursion */
#define FIND_RECURS                          2

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING              200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static ascii FileSystem_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*===========================================================================
 * FUNCTION PROTOTYPES
 *===========================================================================*/
/* format */
       bool   FileSystem_Format(void);                                                                                                // ?   SI                log_status         parser_rx

/* info (file system) */
       bool   FileSystem_FileSystem_Total(u32 *ptr_size);
       bool   FileSystem_FileSystem_Free (u32 *ptr_size);
static bool   FileSystem_FileSystem_Used (u32 *ptr_size);

/* info (file size) */
       bool   FileSystem_FileSize_FileName(ascii *ptr_file_name, s32 *ptr_size);                                                      // OK  SI                                   smtp

/* info (num of files) */
static bool   FileSystem_NumOfFiles_All_Type(u8 file_type,   u32 *ptr_num_files);                                                     // ?   SI  file_system   log_status
       bool   FileSystem_NumOfFiles_Status  (u8 file_status, u32 *ptr_num_files);                                                     // ?   SI  file_system

/* info (file oldest) */
       bool   FileSystem_FileOldest_Type(u8 file_type, ascii *ptr_file_name);                                                         // ?   SI                log_status

/* info */
       bool   FileSystem_FileExists(ascii *ptr_file_name);                                                                            // OK  SI                                   transmission_mail
       bool   FileSystem_FreeSpaceExists(u8 file_type);                                                                               // XX  SI                log_status

/* delete file */
       bool   FileSystem_DeleteFiles_FileName(ascii *ptr_file_name);                                                                  // OK  SI                log_status  alarm

/* file "build" */
       bool   FileSystem_FileBuild_Create      (u8 file_type, CLOCK__TIME *ptr_time_start, CLOCK__TIME *ptr_time_stop);               //     SI                log_status  alarm
       bool   FileSystem_FileBuild_Exist       (u8 file_type);                                                                        //     SI  file_system   log_status
static bool   FileSystem_FileBuild_GetFileName (u8 file_type, ascii *ptr_file_name);                                                  //     SI  file_system
       bool   FileSystem_FileBuild_AppendString(u8 file_type, ascii *ptr_string);                                                     //     SI                log_status  alarm
       bool   FileSystem_FileBuild_ChangeStatus(u8 file_type, u8 new_file_status);                                                    //     SI                log_status  alarm

/* file "close" */
       bool   FileSystem_FileClose_GetFileName (u8 file_type, ascii *ptr_file_name);                                                  //     SI  file_system   log_status  alarm
       bool   FileSystem_FileClose_GetTimeStart(u8 file_type, CLOCK__TIME *ptr_time_start);                                           //     SI                log_status  alarm
       bool   FileSystem_FileClose_GetTimeStop (u8 file_type, CLOCK__TIME *ptr_time_stop);                                            //     SI                log_status  alarm
       bool   FileSystem_FileClose_ChangeStatus(u8 file_type, u8 new_file_status);                                                    //     SI                log_status  alarm

/* file */
       bool   FileSystem_File_Read(ascii *ptr_file_name, u32 num_bytes, u32 start_pointer, u8 *ptr_data);                             // OK  SI                                    smtp

/* file name */
static ascii *FileSystem_FileName_Build(u8 file_type, CLOCK__TIME *ptr_time_start, CLOCK__TIME *ptr_time_stop, u8 file_status);       // XX  SI  file_system

/* utility */
static int    FileSystem_PatternMatch(const ascii* pat, const ascii* nam, unsigned int skip, unsigned int recur);




/*===========================================================================
 * Function   : FileSystem_Format
 *
 * Description: format the file system
 * Input      : -
 * Output     : - FALSE: file system not formatted (fail)
 *              - TRUE : file system     formatted
 *===========================================================================*/
bool FileSystem_Format(void)
{
#ifdef FUNCTION_IMPLEMENTED
    int res_int;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "Format the file system...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    // formats the first volume (A)
    res_int = adl_fsFormat(0);
    if (res_int != ADL_FS_NO_ERROR)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "adl_fsFormat ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "adl_fsFormat OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


#endif
    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileSystem_Total
 *
 * Description: get the total size (bytes) of the file system
 * Input      : - ptr_size: pointer to save the requested size (bytes)
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileSystem_Total(u32 *ptr_size)
{
    s64 fs_space;


    // gets the free diskspace
    fs_space = ql_fs_total_size("UFS:");
    if (fs_space < 0)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fs_total_size ERROR: %lld", fs_space);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        *ptr_size = 0;
        return FALSE;
    }

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fs_total_size OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    *ptr_size = (u32)fs_space;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileSystem_Free
 *
 * Description: get the free size (bytes) of the file system
 * Input      : - ptr_size: pointer to save the requested size (bytes)
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileSystem_Free(u32 *ptr_size)
{
    s64 fs_space;


    // gets the free diskspace
    fs_space = ql_fs_free_size("UFS:");
    if (fs_space < 0)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fs_free_size ERROR: %lld", fs_space);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        *ptr_size = 0;
        return FALSE;
    }

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fs_free_size OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    *ptr_size = (u32)fs_space;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileSystem_Used
 *
 * Description: get the used size (bytes) of the file system
 * Input      : - ptr_size: pointer to save the requested size (bytes)
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
static bool FileSystem_FileSystem_Used(u32 *ptr_size)
{
    u32  size_total;
    u32  size_free;

    bool result_1;
    bool result_2;


    /* get the file system info */
    result_1 = FileSystem_FileSystem_Total(&size_total);
    result_2 = FileSystem_FileSystem_Free (&size_free );

    if ((!result_1) || (!result_2))
    {
        *ptr_size = 0;
        return FALSE;
    }


    *ptr_size = (size_total - size_free);

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileSize_FileName
 *
 * Description: get the file size (bytes) of a specified file
 * Input      : - ptr_file_name: file name
 * Input      : - ptr_size     : pointer to save the requested size (bytes)
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileSize_FileName(ascii *ptr_file_name, s32 *ptr_size)
{
    struct stat fs_stat;
           int  res_int;


    /* get status information on the file */
    res_int = ql_stat(ptr_file_name, &fs_stat);
    if (res_int != QL_FILE_OK)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_stat ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_stat OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* file size in bytes (long) */
    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "filesize : %ld", fs_stat.st_size);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    if (res_int != QL_FILE_OK)
    {
        *ptr_size = 0;
        return FALSE;
    }


    *ptr_size = fs_stat.st_size;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_NumOfFiles_All_Type
 *
 * Description: get the number of files stored in the file system of the specified file type
 * Input      : - file_type    : file type
 *              - ptr_num_files: pointer to save the requested number of files
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
static bool FileSystem_NumOfFiles_All_Type(u8 file_type, u32 *ptr_num_files)
{
    QDIR    *dir;
    qdirent *entry;
    int      res_int;

    ascii    path[10 + 1];

    u8       num_of_files;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(path, HEADER_FILE_NAME__LOG"*.*");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(path, HEADER_FILE_NAME__ALARM"*.*");
            break;

        // file type - unknown
        default:
            strcpy(path, HEADER_FILE_NAME__UNKNOWN"*.*");
            break;
    }


    num_of_files = 0;

    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(path, entry->d_name, 0, FIND_RECURS);
            if (res_int)
                num_of_files++;
        }

        ql_closedir(dir);
    }


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Number of files (file_type %d): %d", file_type, num_of_files);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    *ptr_num_files = num_of_files;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_NumOfFiles_Status
 *
 * Description: get the number of files with a specified status stored in the file system
 * Input      : - file_status  : file status
 *                                  - FILE_SYSTEM__STATUS_FILE__NULL
 *                                  - FILE_SYSTEM__STATUS_FILE__BUILD
 *                                  - FILE_SYSTEM__STATUS_FILE__CLOSE
 *                                  - FILE_SYSTEM__STATUS_FILE__SEND
 *                                  - FILE_SYSTEM__STATUS_FILE__SENT
 *              - ptr_num_files: pointer to save the requested number of files
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_NumOfFiles_Status(u8 file_status, u32 *ptr_num_files)
{
    QDIR    *dir;
    qdirent *entry;
    int      res_int;

    ascii    path[12 + 1];

    u32      num_of_files;


    /* status file */
    switch (file_status)
    {
        // status file - null
        case FILE_SYSTEM__STATUS_FILE__NULL:
            strcpy(path, "*"STATUS_FILE_STRING__NULL".csv");
            break;

        // status file - "build"
        case FILE_SYSTEM__STATUS_FILE__BUILD:
            strcpy(path, "*"STATUS_FILE_STRING__BUILD".csv");
            break;

        // status file - "close"
        case FILE_SYSTEM__STATUS_FILE__CLOSE:
            strcpy(path, "*"STATUS_FILE_STRING__CLOSE".csv");
            break;

        // status file - "send"
        case FILE_SYSTEM__STATUS_FILE__SEND:
            strcpy(path, "*"STATUS_FILE_STRING__SEND".csv");
            break;

        // status file - "sent"
        case FILE_SYSTEM__STATUS_FILE__SENT:
            strcpy(path, "*"STATUS_FILE_STRING__SENT".csv");
            break;

        // status file - unknown
        default:
            return FALSE;
    }


    num_of_files = 0;

    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(path, entry->d_name, 0, FIND_RECURS);
            if (res_int)
                num_of_files++;
        }

        ql_closedir(dir);
    }


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Number of files (file status %d): %ld", file_status, num_of_files);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    *ptr_num_files = num_of_files;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileOldest_Type
 *
 * Description: find the oldest file of the specified file type
 * Input      : - file_type    : file type
 *              - ptr_file_name: pointer to save the file name of the oldest file
 * Output     : - FALSE: oldest file not found
 *              - TRUE : oldest file     found
 *===========================================================================*/
bool FileSystem_FileOldest_Type(u8 file_type, ascii *ptr_file_name)
{
    ascii    file_search_string[10 + 1];

    QDIR    *dir;
    qdirent *entry;

    time_t   file_time;
    time_t   file_time_oldest;

    ascii    file_name       [68 + 1];
    ascii    file_name_oldest[68 + 1];

    bool     oldest_file_found;

    int      res_int;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(file_search_string, HEADER_FILE_NAME__LOG"*.*");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(file_search_string, HEADER_FILE_NAME__ALARM"*.*");
            break;

        // file type - unknown
        default:
            strcpy(file_search_string, HEADER_FILE_NAME__UNKNOWN"*.*");
            break;
    }


    oldest_file_found = FALSE;

    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(file_search_string, entry->d_name, 0, FIND_RECURS);
            if (res_int)
            {
                /* build the file full file name (path + name) */
                strcpy (file_name, "UFS:");                                     /* copy the file path */
                strncat(file_name, entry->d_name, 68 - (sizeof("UFS:") - 1));   /* copy the file name */
                file_name[68] = 0x00;

                file_time = Clock_ConverTimeString4ToClockTime(file_name);

                if (!oldest_file_found)
                {
                    /* a possible oldest file not yet found */

                    oldest_file_found = TRUE;

                    /* save file time */
                    file_time_oldest = file_time;

                    /* save file name */
                    strcpy(file_name_oldest, file_name);
                }
                else
                {
                    /* a possible oldest file already found */

                    /* compare "current oldest" file time with "just found" file time */
                    if (file_time < file_time_oldest)
                    {
                        /* save file time */
                        file_time_oldest = file_time;

                        /* save file name */
                        strcpy(file_name_oldest, file_name);
                    }
                }
            }
        }

        ql_closedir(dir);
    }


    if (!oldest_file_found)
        return FALSE;


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Oldest file name (file type %d): \"%s\"", file_type, file_name_oldest);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Oldest file time (file type %d):"       , file_type);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    Clock_PrintTime3(DEBUG_TRACE_LEVEL_FILE_SYSTEM, 0, 0, &file_time_oldest);


    /* save file name */
    strncpy(ptr_file_name, file_name_oldest, 68);
    ptr_file_name[68] = 0x00;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileExists
 *
 * Description: check if a specified file exists
 * Input      : - ptr_file_name: file name
 * Output     : - FALSE: the specified file does not exist
 *              - TRUE : the specified file          exists
 *===========================================================================*/
bool FileSystem_FileExists(ascii *ptr_file_name)
{
    int res_int;


    /* search for the specified file */
    res_int = ql_file_exist(ptr_file_name);
    if (res_int != QL_FILE_OK)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_file_exist ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_file_exist OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FreeSpaceExists
 *
 * Description: check if there is free space in the file system for a new file
 * Input      : - file_type: file type
 * Output     : - FALSE: there is not free space in the file system
 *              - TRUE : there is     free space in the file system
 *===========================================================================*/
bool FileSystem_FreeSpaceExists(u8 file_type)
{
    u32  num_of_files;
    u32  num_of_files_max;

    u32  size_of_files;

    bool result;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            num_of_files_max = LIMITS__MAX_NUMBER_OF_FILES__LOG;
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            num_of_files_max = LIMITS__MAX_NUMBER_OF_FILES__ALARM;
            break;

        // file type - unknown
        default:
            num_of_files_max = 0;
            break;
    }


    /* get the number of stored files */
    result = FileSystem_NumOfFiles_All_Type(file_type, &num_of_files);
    if (!result)
        return FALSE;

    /* check the number of stored files */
    if (num_of_files >= num_of_files_max)
        return FALSE;


    /* get the used size of the file system (bytes) */
    result = FileSystem_FileSystem_Used(&size_of_files);
    if (!result)
        return FALSE;

    /* check the used size of the file system */
    if (size_of_files >= LIMITS__MAX_TOTAL_SIZE_OF_FILES)
        return FALSE;


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_DeleteFiles_FileName
 *
 * Description: delete a specified file
 * Input      : - ptr_file_name: file name of the file to be deleted
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_DeleteFiles_FileName(ascii *ptr_file_name)
{
    int res_int;


    /* deletes the specified file */
    res_int = ql_remove(ptr_file_name);
    if (res_int != QL_FILE_OK)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_remove ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_remove OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileBuild_Create
 *
 * Description: create a new "file build" (open and close the file, file size will be zero)
 * Input      : - file_type: file type
 *              - ptr_start: pointer to file start time (to build the file name)
 *              - ptr_stop : pointer to file stop  time (to build the file name)
 * Output     : - FALSE: "file build" not created (fail)
 *              - TRUE : "file build"     created
 *===========================================================================*/
bool FileSystem_FileBuild_Create(u8 file_type, CLOCK__TIME *ptr_time_start, CLOCK__TIME *ptr_time_stop)
{
    QFILE  file;

    ascii  file_name[68 + 1];
    ascii *ptr_file_name;

    int    res_int;


    /* build the file name */
    ptr_file_name = FileSystem_FileName_Build(file_type, ptr_time_start, ptr_time_stop, FILE_SYSTEM__STATUS_FILE__BUILD);

    strncpy(file_name, ptr_file_name, 68);
    file_name[68] = 0x00;


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Create a new \"file build\": \"%s\"", file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* create the file (open in append mode) */
    file = ql_fopen(file_name, "a");
    if (file > 0)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fopen OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fopen ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }


    /* close the created file (file size is zero) */
    res_int = ql_fclose(file);
    if (res_int != QL_FILE_OK)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fclose ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }

    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fclose OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileBuild_Exist
 *
 * Description:
 * Input      : - file_type: file type
 * Output     : - FALSE:
 *              - TRUE :
 *===========================================================================*/
bool FileSystem_FileBuild_Exist(u8 file_type)
{
    ascii    file_search_string[19 + 1];

    QDIR    *dir;
    qdirent *entry;

    bool     file_found;
    int      res_int;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(file_search_string, HEADER_FILE_NAME__LOG"*(build).csv");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(file_search_string, HEADER_FILE_NAME__ALARM"*(build).csv");
            break;

        // file type - unknown
        default:
            strcpy(file_search_string, HEADER_FILE_NAME__UNKNOWN"*(build).csv");
            break;
    }


    file_found = FALSE;

    /* search for the "file build" */
    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(file_search_string, entry->d_name, 0, FIND_RECURS);
            if (res_int)
            {
                file_found = TRUE;
                break;
            }
        }

        ql_closedir(dir);
    }


    return file_found;
}




/*===========================================================================
 * Function   : FileSystem_FileBuild_GetFileName
 *
 * Description: get the "file build" file name
 * Input      : - file_type    : file type
 *              - ptr_file_name: pointer to save the "file build" file name
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
static bool FileSystem_FileBuild_GetFileName(u8 file_type, ascii *ptr_file_name)
{
    ascii    file_search_string[19 + 1];

    QDIR    *dir;
    qdirent *entry;

    bool     file_found;
    int      res_int;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(file_search_string, HEADER_FILE_NAME__LOG"*(build).csv");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(file_search_string, HEADER_FILE_NAME__ALARM"*(build).csv");
            break;

        // file type - unknown
        default:
            strcpy(file_search_string, HEADER_FILE_NAME__UNKNOWN"*(build).csv");
            break;
    }


    file_found = FALSE;

    /* search for the "file build" */
    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(file_search_string, entry->d_name, 0, FIND_RECURS);
            if (res_int)
            {
                file_found = TRUE;

                strcpy(ptr_file_name, "UFS:");
                strncat(ptr_file_name, entry->d_name, 68 - (sizeof("UFS:") - 1));
                ptr_file_name[68] = 0x00;

                break;
            }
        }

        ql_closedir(dir);
    }


    return file_found;
}




/*===========================================================================
 * Function   : FileSystem_FileBuild_AppendString
 *
 * Description: append a string to the "file build"
 * Input      : - file_type : file type
 *              - ptr_string: string to be appended
 * Output     : - FALSE: string not appended (fail)
 *              - TRUE : string     appended
 *===========================================================================*/
bool FileSystem_FileBuild_AppendString(u8 file_type, ascii *ptr_string)
{
    ascii  file_name[68 + 1];

    QFILE  file;

    size_t len;
    int    res_int;
    bool   result;


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Append to the \"file build\" the string: %s", ptr_string);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* build the file name of the "file build" */
    result = FileSystem_FileBuild_GetFileName(file_type, file_name);
    if (!result)
        return FALSE;


    len = strlen(ptr_string);

    if (len > 0)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Open file (append mode): \"%s\"", file_name);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* open the "file build" in append mode */
        file = ql_fopen(file_name, "a");
        if (file > 0)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fopen OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fopen ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            return FALSE;
        }


        /* append the string to the file */
        res_int = ql_fwrite((void *)ptr_string, 1, len, file);

        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fwrite: %d bytes written", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        if (res_int != (int)len)
            return FALSE;


        /* close the "file build" */
        res_int = ql_fclose(file);
        if (res_int != QL_FILE_OK)
        {
            snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fclose ERROR: %d", res_int);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            return FALSE;
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fclose OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileBuild_ChangeStatus
 *
 * Description:
 * Input      : - file_type      : file type
 *              - new_file_status:
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileBuild_ChangeStatus(u8 file_type, u8 new_file_status)
{
    ascii file_name    [68 + 1];
    ascii new_file_name[68 + 1];

    bool  result;
    int   res_int;


    //@@@

    result = FileSystem_FileBuild_GetFileName(file_type, file_name);
    if (!result)
        return FALSE;


    // "UFS:EASY__L__2015.06.26_08.00.00__2015.06.26_10.00.00__(build).csv"
    // "UFS:EASY__A__2015.06.26_08.00.00__2015.06.26_10.00.00__(build).csv"
    //                                                          |
    //                                                          56

    strncpy(new_file_name, file_name, 68);
    new_file_name[68] = 0x00;

    new_file_name[56] = 'c';
    new_file_name[57] = 'l';
    new_file_name[58] = 'o';
    new_file_name[59] = 's';
    new_file_name[60] = 'e';

    new_file_name[61] = ')';

    new_file_name[62] = '.';

    new_file_name[63] = 'c';
    new_file_name[64] = 's';
    new_file_name[65] = 'v';

    new_file_name[66] = 0x00;


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Rename file - old file name: \"%s\", ", file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Rename file - new file name: \"%s\", ", new_file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    res_int = ql_rename(file_name, new_file_name);
    if (res_int != QL_FILE_OK)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_rename ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_rename OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_File_Read
 *
 * Description: read a block of bytes from a specified file
 * Input      : - ptr_file_name: file name
 *              - num_bytes    : number of bytes to be read [1-200000]
 *              - start_pointer: start pointer in the file
 *              - ptr_data     : pointer to save data read
 * Output     : - FALSE: bytes not read (fail)
 *              - TRUE : bytes     read
 *===========================================================================*/
bool FileSystem_File_Read(ascii *ptr_file_name, u32 num_bytes, u32 start_pointer, u8 *ptr_data)
{
    ascii file_name[68 + 1];

    QFILE file;
  //u32   file_size;

    int   res_int_1;
    int   res_int_2;


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Read from the \"file build\" %ld bytes starting from %ld", num_bytes, start_pointer);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (num_bytes > 200000)
        return FALSE;


    /* save the file name */
    strncpy(file_name, ptr_file_name, 68);
    file_name[68] = 0x00;


    //@@@ CONTROLLARE LA SIZE DEL FILE e FARE CONFRONTO CON num_bytes e start_pointer
  //file_size = ...
  //if ((start_pointer + num_bytes) > file_size)
  //    return FALSE;


    if (num_bytes > 0)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Open file (read mode): \"%s\", ", file_name);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* open the "file build" in read mode */
        file = ql_fopen(file_name, "r");
        if (file > 0)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fopen OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fopen ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            return FALSE;
        }


        /* moves the position into specified offset in the file */
        res_int_1 = ql_fseek(file, (long)start_pointer, QL_SEEK_SET);
        if (res_int_1 < 0)
        {
            snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fseek ERROR: %d", res_int_1);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fseek OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }


        /* read data from the file */
        res_int_1 = ql_fread((void *)ptr_data, 1, (size_t)num_bytes, file);

        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fread: %d bytes read", res_int_1);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* print the data read */
      //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "Data read:", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
      //Utility_PrintData(DEBUG_TRACE_LEVEL_FILE_SYSTEM, num_bytes, ptr_data);

        /* close the file */
        res_int_2 = ql_fclose(file);
        if (res_int_2 != QL_FILE_OK)
        {
            snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_fclose ERROR: %d", res_int_2);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            return FALSE;
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_fclose OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }

        if (res_int_1 != (int)num_bytes)
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileClose_GetFileName
 *
 * Description: get the "file close" file name
 * Input      : - file_type    : file type
 *              - ptr_file_name: pointer to save the "file close" file name
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileClose_GetFileName(u8 file_type, ascii *ptr_file_name)
{
    ascii    file_search_string[19 + 1];

    QDIR    *dir;
    qdirent *entry;

    bool     file_found;
    int      res_int;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(file_search_string, HEADER_FILE_NAME__LOG"*(close).csv");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(file_search_string, HEADER_FILE_NAME__ALARM"*(close).csv");
            break;

        // file type - unknown
        default:
            strcpy(file_search_string, HEADER_FILE_NAME__UNKNOWN"*(close).csv");
            break;
    }


    file_found = FALSE;

    /* search for the "file close" */
    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(file_search_string, entry->d_name, 0, FIND_RECURS);
            if (res_int)
            {
                file_found = TRUE;

                strcpy(ptr_file_name, "UFS:");
                strncat(ptr_file_name, entry->d_name, 68 - (sizeof("UFS:") - 1));
                ptr_file_name[68] = 0x00;

                break;
            }
        }

        ql_closedir(dir);
    }


    return file_found;
}




/*===========================================================================
 * Function   : FileSystem_FileClose_GetTimeStart
 *
 * Description: get the "file close" time start
 * Input      : - file_type     : file type
 *              - ptr_time_start: pointer to save the "file close" time start
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileClose_GetTimeStart(u8 file_type, CLOCK__TIME *ptr_time_start)
{
    ascii        file_search_string[19 + 1];

    QDIR        *dir;
    qdirent     *entry;

    ascii        time_start_string[19 + 1];
    CLOCK__TIME  time_start;

    bool         file_found;
    int          res_int;
    u8           i;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(file_search_string, HEADER_FILE_NAME__LOG"*(close).csv");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(file_search_string, HEADER_FILE_NAME__ALARM"*(close).csv");
            break;

        // file type - unknown
        default:
            strcpy(file_search_string, HEADER_FILE_NAME__UNKNOWN"*(close).csv");
            break;
    }


    file_found = FALSE;

    /* search for the "file close" */
    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(file_search_string, entry->d_name, 0, FIND_RECURS);
            if (res_int)
            {
                file_found = TRUE;

                // "EASY__L__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
                // "EASY__A__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
                //           |
                //           9

                /* extract the time start string from the file name */
                for (i = 0; i < 19; i++)
                    time_start_string[i] = entry->d_name[9 + i];
                time_start_string[19] = 0x00;

                break;
            }
        }

        ql_closedir(dir);
    }

    if (!file_found)
        return FALSE;


    time_start.year     = ((u16)(time_start_string[ 0] - '0') * 1000) +
                          ((u16)(time_start_string[ 1] - '0') *  100) +
                          ((u16)(time_start_string[ 2] - '0') *   10) +
                          ((u16)(time_start_string[ 3] - '0') *    1);
    time_start.month    = (     (time_start_string[ 5] - '0') *   10) +
                          (     (time_start_string[ 6] - '0') *    1);
    time_start.day      = (     (time_start_string[ 8] - '0') *   10) +
                          (     (time_start_string[ 9] - '0') *    1);
    time_start.hour     = (     (time_start_string[11] - '0') *   10) +
                          (     (time_start_string[12] - '0') *    1);
    time_start.minute   = (     (time_start_string[14] - '0') *   10) +
                          (     (time_start_string[15] - '0') *    1);
    time_start.second   = (     (time_start_string[17] - '0') *   10) +
                          (     (time_start_string[18] - '0') *    1);
    time_start.week_day = Clock_WeekDayOfADay(time_start.day, time_start.month, time_start.year);


    *ptr_time_start = time_start;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileClose_GetTimeStop
 *
 * Description: get the "file close" time stop
 * Input      : - file_type    : file type
 *              - ptr_time_stop: pointer to save the "file close" time stop
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileClose_GetTimeStop(u8 file_type, CLOCK__TIME *ptr_time_stop)
{
    ascii        file_search_string[19 + 1];

    QDIR        *dir;
    qdirent     *entry;

    ascii        time_stop_string[19 + 1];
    CLOCK__TIME  time_stop;

    bool         file_found;
    int          res_int;
    u8           i;


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            strcpy(file_search_string, HEADER_FILE_NAME__LOG"*(close).csv");
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            strcpy(file_search_string, HEADER_FILE_NAME__ALARM"*(close).csv");
            break;

        // file type - unknown
        default:
            strcpy(file_search_string, HEADER_FILE_NAME__UNKNOWN"*(close).csv");
            break;
    }


    file_found = FALSE;

    /* search for the "file close" */
    dir = ql_opendir("UFS:");
    if (dir)
    {
        for (;;)
        {
            /* Get a directory item */
            entry = ql_readdir(dir);

            /* Terminate if any error or end of directory */
            if (!entry || !entry->d_name[0])
                break;

            /* Test for the file name */
            res_int = FileSystem_PatternMatch(file_search_string, entry->d_name, 0, FIND_RECURS);
            if (res_int)
            {
                file_found = TRUE;

                // "EASY__L__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
                // "EASY__A__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
                //                                |
                //                                30

                /* extract the time start string from the file name */
                for (i = 0; i < 19; i++)
                    time_stop_string[i] = entry->d_name[30 + i];
                time_stop_string[19] = 0x00;

                break;
            }
        }

        ql_closedir(dir);
    }

    if (!file_found)
        return FALSE;


    time_stop.year     = ((u16)(time_stop_string[ 0] - '0') * 1000) +
                         ((u16)(time_stop_string[ 1] - '0') *  100) +
                         ((u16)(time_stop_string[ 2] - '0') *   10) +
                         ((u16)(time_stop_string[ 3] - '0') *    1);
    time_stop.month    = (     (time_stop_string[ 5] - '0') *   10) +
                         (     (time_stop_string[ 6] - '0') *    1);
    time_stop.day      = (     (time_stop_string[ 8] - '0') *   10) +
                         (     (time_stop_string[ 9] - '0') *    1);
    time_stop.hour     = (     (time_stop_string[11] - '0') *   10) +
                         (     (time_stop_string[12] - '0') *    1);
    time_stop.minute   = (     (time_stop_string[14] - '0') *   10) +
                         (     (time_stop_string[15] - '0') *    1);
    time_stop.second   = (     (time_stop_string[17] - '0') *   10) +
                         (     (time_stop_string[18] - '0') *    1);
    time_stop.week_day = Clock_WeekDayOfADay(time_stop.day, time_stop.month, time_stop.year);


    *ptr_time_stop = time_stop;

    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileClose_ChangeStatus
 *
 * Description:
 * Input      : - file_type      : file type
 *              - new_file_status:
 * Output     : - FALSE: error
 *              - TRUE : success
 *===========================================================================*/
bool FileSystem_FileClose_ChangeStatus(u8 file_type, u8 new_file_status)
{
    ascii file_name    [68 + 1];
    ascii new_file_name[68 + 1];

    bool  result;
    int   res_int;


    //@@@

    result = FileSystem_FileClose_GetFileName(file_type, file_name);
    if (!result)
        return FALSE;


    // "UFS:EASY__L__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
    // "UFS:EASY__A__2015.06.26_08.00.00__2015.06.26_10.00.00__(close).csv"
    //                                                          |
    //                                                          56

    strncpy(new_file_name, file_name, 68);
    new_file_name[68] = 0x00;

    new_file_name[56] = 's';
    new_file_name[57] = 'e';
    new_file_name[58] = 'n';
    new_file_name[59] = 'd';

    new_file_name[60] = ')';

    new_file_name[61] = '.';

    new_file_name[62] = 'c';
    new_file_name[63] = 's';
    new_file_name[64] = 'v';

    new_file_name[65] = 0x00;


    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Rename file - old file name: \"%s\", ", file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "Rename file - new file name: \"%s\", ", new_file_name);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    res_int = ql_rename(file_name, new_file_name);
    if (res_int != QL_FILE_OK)
    {
        snprintf(FileSystem_DebugString, sizeof(FileSystem_DebugString), "ql_rename ERROR: %d", res_int);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, FileSystem_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        return FALSE;
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "ql_rename OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FileSystem_FileName_Build
 *
 * Description: build the file name string
 * Input      : - file_type     : file type
 *              - ptr_time_start: pointer to file start time
 *              - ptr_time_stop : pointer to file stop time
 *              - file_status   :            file status
 * Output     : - pointer to save the string built
 *===========================================================================*/
static ascii *FileSystem_FileName_Build(u8 file_type, CLOCK__TIME *ptr_time_start, CLOCK__TIME *ptr_time_stop, u8 file_status)
{
           CLOCK__TIME time_start;
           CLOCK__TIME time_stop;

    static ascii       file_name[68 + 1];

           ascii      *ptr_header;
           ascii      *ptr_file_status;

           int         res_int;
           bool        result;


    // UFS:EASY__L__2015.05.06_04.00.00__2015.05.06_08.00.00.csv
    // UFS:EASY__L__2015.05.06_04.00.00__2015.05.06_08.00.00__(build).csv
    // UFS:EASY__L__2015.05.06_04.00.00__2015.05.06_08.00.00__(close).csv
    // UFS:EASY__L__2015.05.06_04.00.00__2015.05.06_08.00.00__(send).csv
    // UFS:EASY__L__2015.05.06_04.00.00__2015.05.06_08.00.00__(sent).csv

    // UFS:EASY__A__2015.05.06_04.00.00__2015.05.06_08.00.00.csv
    // UFS:EASY__A__2015.05.06_04.00.00__2015.05.06_08.00.00__(build).csv
    // UFS:EASY__A__2015.05.06_04.00.00__2015.05.06_08.00.00__(close).csv
    // UFS:EASY__A__2015.05.06_04.00.00__2015.05.06_08.00.00__(send).csv
    // UFS:EASY__A__2015.05.06_04.00.00__2015.05.06_08.00.00__(sent).csv


    time_start = *ptr_time_start;
    time_stop  = *ptr_time_stop;

    result = Clock_IsValidClockTime(&time_start);
    if (!result)
    {
        time_start.year     = 2000;
        time_start.month    = 1;
        time_start.day      = 1;
        time_start.hour     = 0;
        time_start.minute   = 0;
        time_start.second   = 0;
        time_start.week_day = 6;  // Saturday
    }

    result = Clock_IsValidClockTime(&time_stop);
    if (!result)
    {
        time_stop.year      = 2000;
        time_stop.month     = 1;
        time_stop.day       = 1;
        time_stop.hour      = 0;
        time_stop.minute    = 0;
        time_stop.second    = 0;
        time_stop.week_day  = 6;  // Saturday
    }


    switch (file_type)
    {
        // file type - log   file
        case FILE_SYSTEM__FILE_TYPE__LOG:
            ptr_header = HEADER_FILE_NAME__LOG;
            break;

        // file type - alarm file
        case FILE_SYSTEM__FILE_TYPE__ALARM:
            ptr_header = HEADER_FILE_NAME__ALARM;
            break;

        // file type - unknown
        default:
            ptr_header = HEADER_FILE_NAME__UNKNOWN;
            break;
    }


    /* status file */
    switch (file_status)
    {
        // status file - null
        case FILE_SYSTEM__STATUS_FILE__NULL:
            ptr_file_status = STATUS_FILE_STRING__NULL;
            break;

        // status file - "build"
        case FILE_SYSTEM__STATUS_FILE__BUILD:
            ptr_file_status = STATUS_FILE_STRING__BUILD;
            break;

        // status file - "close"
        case FILE_SYSTEM__STATUS_FILE__CLOSE:
            ptr_file_status = STATUS_FILE_STRING__CLOSE;
            break;

        // status file - "send"
        case FILE_SYSTEM__STATUS_FILE__SEND:
            ptr_file_status = STATUS_FILE_STRING__SEND;
            break;

        // status file - "sent"
        case FILE_SYSTEM__STATUS_FILE__SENT:
            ptr_file_status = STATUS_FILE_STRING__SENT;
            break;

        // status file - unknown
        default:
            ptr_file_status = "XXX";
            break;
    }

    res_int = snprintf(file_name, 68 + 1,
                       "UFS:%s__%04d.%02d.%02d_%02d.%02d.%02d__%04d.%02d.%02d_%02d.%02d.%02d__%s.csv",

                       ptr_header,

                       time_start.year,
                       time_start.month,
                       time_start.day,
                       time_start.hour,
                       time_start.minute,
                       time_start.second,

                       time_stop.year,
                       time_stop.month,
                       time_stop.day,
                       time_stop.hour,
                       time_stop.minute,
                       time_stop.second,

                       ptr_file_status);
    if (res_int > 68)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_FILE_SYSTEM, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - snprintf - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return file_name;
}




/*===========================================================================
 * Function   : FileSystem_PatternMatch
 *
 * Description: get the number of files stored in the file system of the specified file type
 * Input      : - pat  : matching pattern
 *              - nam  : string to be tested
 *              - skip : number of pre-skip chars (number of ?s, b8:infinite (* specified))
 *              - recur: recursion count
 * Output     : - 0: mismatched
 *              - 1: matched
 *===========================================================================*/
static int FileSystem_PatternMatch(const ascii* pat, const ascii* nam, unsigned int skip, unsigned int recur)
{
    const ascii        *pptr;
    const ascii        *nptr;

          u32           pchr;
          u32           nchr;

          unsigned int  sk;


    /* pre-skip name chars */
    while ((skip & 0xFF) != 0)
    {
        /* branch mismatched if less name chars */
        if (*nam++ == '\0')
            return 0;    

        skip--;
    }

    /* matched? (short circuit) */
    if (*pat == '\0' && skip)
        return 1;

    /* retry until end of name if infinite search is specified */
    do
    {
        /* top of pattern and name to match */
        pptr = pat;
        nptr = nam;

        for (;;)
        {
            if (*pptr == '\?' || *pptr == '*')
            {
                /* wildcard term */

                if (recur == 0)
                {
                    /* too many wildcard terms */
                    return 0;
                }

                /* analyze the wildcard term */
                sk = 0;
                do
                {
                    if (*pptr++ == '\?')
                        sk++;
                    else
                        sk |= 0x100;
                } while (*pptr == '\?' || *pptr == '*');

                /* test new branch (recursive call) */
                if (FileSystem_PatternMatch(pptr, nptr, sk, recur - 1))
                    return 1;

                nchr = *nptr;

                /* branch mismatched */
                break;
            }

            /* get a pattern char */
            /* get a name    char */
            pchr = *pptr++;
            nchr = *nptr++;

            if (pchr != nchr)
            {
                /* branch mismatched */
                break;
            }

            if (pchr == 0)
            {
                /* branch matched at end of both strings */
                return 1;
            }
        }

        nam++;
    } while (skip && nchr);

    return 0;
}
