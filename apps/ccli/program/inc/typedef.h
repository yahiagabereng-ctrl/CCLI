/*=============================================================================
 * File       :  TYPEDEF.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - base data types (SK0153 typedef.h layout)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __TYPEDEF_H__


#define __TYPEDEF_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef char     ascii;




/*=============================================================================
 * DEFINES
 *=============================================================================*/
#ifndef TRUE
#define TRUE  true
#endif

#ifndef FALSE
#define FALSE false
#endif




#endif
