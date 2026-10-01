/*=============================================================================
 * File       :  TYPEDEF.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - data types
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __TYPEDEF_H__


#define __TYPEDEF_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include <stdbool.h>




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* true/false */
#ifdef FALSE
    #undef FALSE
#endif
#ifdef TRUE
    #undef TRUE
#endif
#define FALSE     0
#define TRUE      1

/* bits */
#define BIT_0       0x00000001
#define BIT_1       0x00000002
#define BIT_2       0x00000004
#define BIT_3       0x00000008
#define BIT_4       0x00000010
#define BIT_5       0x00000020
#define BIT_6       0x00000040
#define BIT_7       0x00000080
#define BIT_8       0x00000100
#define BIT_9       0x00000200
#define BIT_10      0x00000400
#define BIT_11      0x00000800
#define BIT_12      0x00001000
#define BIT_13      0x00002000
#define BIT_14      0x00004000
#define BIT_15      0x00008000
#define BIT_16      0x00010000
#define BIT_17      0x00020000
#define BIT_18      0x00040000
#define BIT_19      0x00080000
#define BIT_20      0x00100000
#define BIT_21      0x00200000
#define BIT_22      0x00400000
#define BIT_23      0x00800000
#define BIT_24      0x01000000
#define BIT_25      0x02000000
#define BIT_26      0x04000000
#define BIT_27      0x08000000
#define BIT_28      0x10000000
#define BIT_29      0x20000000
#define BIT_30      0x40000000
#define BIT_31      0x80000000




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Basic data types
 *-----------------------------------------------------------------------------*/
/* ASCII */
typedef                     char   ascii;

// integer 8 bits
typedef            unsigned char   u8;
typedef              signed char   s8;

// integer 16 bits
typedef      short unsigned int    u16;
typedef      short          int    s16;

// integer 32 bits
typedef      long  unsigned int    u32;
typedef      long           int    s32;

// integer 64 bits
typedef long long  unsigned int    u64;
typedef long long           int    s64;


/*-----------------------------------------------------------------------------
 * BYTE, WORD, DWORD types
 *-----------------------------------------------------------------------------*/
/* byte type */
typedef union
{
    /* bit access */
    struct
    {
        u8 bit_0: 1;
        u8 bit_1: 1;
        u8 bit_2: 1;
        u8 bit_3: 1;
        u8 bit_4: 1;
        u8 bit_5: 1;
        u8 bit_6: 1;
        u8 bit_7: 1;
    } bits;

    /* byte access */
    u8 byte;
} TYPE_BYTE;


/* word type */
typedef union
{
    /* bit access */
    struct
    {
        u8 bit_0 : 1;
        u8 bit_1 : 1;
        u8 bit_2 : 1;
        u8 bit_3 : 1;
        u8 bit_4 : 1;
        u8 bit_5 : 1;
        u8 bit_6 : 1;
        u8 bit_7 : 1;

        u8 bit_8 : 1;
        u8 bit_9 : 1;
        u8 bit_10: 1;
        u8 bit_11: 1;
        u8 bit_12: 1;
        u8 bit_13: 1;
        u8 bit_14: 1;
        u8 bit_15: 1;
    } bits;

    /* byte access */
    struct
    {
        u8 low;
        u8 high;
    } byte;

    /* word access */
    u16 word;
} TYPE_WORD;


/* double word type */
typedef union
{
    /* bit access */
    struct
    {
        u8 bit_0 : 1;
        u8 bit_1 : 1;
        u8 bit_2 : 1;
        u8 bit_3 : 1;
        u8 bit_4 : 1;
        u8 bit_5 : 1;
        u8 bit_6 : 1;
        u8 bit_7 : 1;

        u8 bit_8 : 1;
        u8 bit_9 : 1;
        u8 bit_10: 1;
        u8 bit_11: 1;
        u8 bit_12: 1;
        u8 bit_13: 1;
        u8 bit_14: 1;
        u8 bit_15: 1;

        u8 bit_16: 1;
        u8 bit_17: 1;
        u8 bit_18: 1;
        u8 bit_19: 1;
        u8 bit_20: 1;
        u8 bit_21: 1;
        u8 bit_22: 1;
        u8 bit_23: 1;

        u8 bit_24: 1;
        u8 bit_25: 1;
        u8 bit_26: 1;
        u8 bit_27: 1;
        u8 bit_28: 1;
        u8 bit_29: 1;
        u8 bit_30: 1;
        u8 bit_31: 1;
    } bits;

    /* byte access */
    struct
    {
        u8 low;
        u8 high;
        u8 upper_low;
        u8 upper_high;
    } byte;

    /* word access */
    struct
    {
        u16 low;
        u16 high;
    } word;

    /* dword access */
    u32 dword;
} TYPE_DWORD;




#endif
