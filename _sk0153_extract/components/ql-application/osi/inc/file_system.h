/*=============================================================================
 * File       :  FILE_SYSTEM.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - file system
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __FILE_SYSTEM_H__


#define __FILE_SYSTEM_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"
#include "clock.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* file type */
#define FILE_SYSTEM__FILE_TYPE__LOG       0   // file type - log   file
#define FILE_SYSTEM__FILE_TYPE__ALARM     1   // file type - alarm file

/* status file */
#define FILE_SYSTEM__STATUS_FILE__NULL    0    // status file - null
#define FILE_SYSTEM__STATUS_FILE__BUILD   1    // status file - "build"
#define FILE_SYSTEM__STATUS_FILE__CLOSE   2    // status file - "close"
#define FILE_SYSTEM__STATUS_FILE__SEND    3    // status file - "send"
#define FILE_SYSTEM__STATUS_FILE__SENT    4    // status file - "sent"




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* format */
bool FileSystem_Format(void);

/* info (file system) */
bool FileSystem_FileSystem_Total(u32 *ptr_size);
bool FileSystem_FileSystem_Free (u32 *ptr_size);

/* info (file size) */
bool FileSystem_FileSize_FileName(ascii *ptr_file_name, s32 *ptr_size);

/* info (num of files) */
bool FileSystem_NumOfFiles_Status(u8 file_status, u32 *ptr_num_files);

/* info (file oldest) */
bool FileSystem_FileOldest_Type(u8 file_type, ascii *ptr_file_name);

/* info */
bool FileSystem_FileExists(ascii *ptr_file_name);
bool FileSystem_FreeSpaceExists(u8 file_type);

/* delete file */
bool FileSystem_DeleteFiles_FileName(ascii *ptr_file_name);

/* file "build" */
bool FileSystem_FileBuild_Create      (u8 file_type, CLOCK__TIME *ptr_time_start, CLOCK__TIME *ptr_time_stop);
bool FileSystem_FileBuild_Exist       (u8 file_type);
bool FileSystem_FileBuild_AppendString(u8 file_type, ascii *ptr_string);
bool FileSystem_FileBuild_ChangeStatus(u8 file_type, u8 new_file_status);

/* file "close" */
bool FileSystem_FileClose_GetFileName (u8 file_type, ascii       *ptr_file_name);
bool FileSystem_FileClose_GetTimeStart(u8 file_type, CLOCK__TIME *ptr_time_start);
bool FileSystem_FileClose_GetTimeStop (u8 file_type, CLOCK__TIME *ptr_time_stop);
bool FileSystem_FileClose_ChangeStatus(u8 file_type, u8 new_file_status);

/* file */
bool FileSystem_File_Read(ascii *ptr_file_name, u32 num_bytes, u32 start_pointer, u8 *ptr_data);




#endif
