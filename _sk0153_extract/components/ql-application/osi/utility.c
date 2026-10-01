/*=============================================================================
 * File       :  UTILITY.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - utilities
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* API      includes */
#include "ql_api_dev.h"
#include "ql_api_rtc.h"
#include "ql_adc.h"

/* user     includes */
#include "debug_my.h"
#include "phone.h"   // for random generator utility
#include "typedef.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* CRC16 polynomial (x^16 + x^15 + x^2 + 1) */
#define CRC16_POLYNOMIAL          0x8003

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING   300




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* base64 input */
typedef struct
{
    u8    a1;
    u8    a2;
    u8    a3;
} BASE64_INPUT;

/* base64 output */
typedef struct
{
    ascii b1;
    ascii b2;
    ascii b3;
    ascii b4;
} BASE64_OUTPUT;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
static       ascii Utility_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/* CRC16 table */
/* It was calculated using the function "Utility_InitCRC16Table" */
static const u16   Utility_Crc16Table[256] =
{
    0x0000,    0x8003,    0x8005,    0x0006,    0x8009,    0x000A,    0x000C,    0x800F,    0x8011,    0x0012,    0x0014,    0x8017,    0x0018,    0x801B,    0x801D,    0x001E,
    0x8021,    0x0022,    0x0024,    0x8027,    0x0028,    0x802B,    0x802D,    0x002E,    0x0030,    0x8033,    0x8035,    0x0036,    0x8039,    0x003A,    0x003C,    0x803F,
    0x8041,    0x0042,    0x0044,    0x8047,    0x0048,    0x804B,    0x804D,    0x004E,    0x0050,    0x8053,    0x8055,    0x0056,    0x8059,    0x005A,    0x005C,    0x805F,
    0x0060,    0x8063,    0x8065,    0x0066,    0x8069,    0x006A,    0x006C,    0x806F,    0x8071,    0x0072,    0x0074,    0x8077,    0x0078,    0x807B,    0x807D,    0x007E,
    0x8081,    0x0082,    0x0084,    0x8087,    0x0088,    0x808B,    0x808D,    0x008E,    0x0090,    0x8093,    0x8095,    0x0096,    0x8099,    0x009A,    0x009C,    0x809F,
    0x00A0,    0x80A3,    0x80A5,    0x00A6,    0x80A9,    0x00AA,    0x00AC,    0x80AF,    0x80B1,    0x00B2,    0x00B4,    0x80B7,    0x00B8,    0x80BB,    0x80BD,    0x00BE,
    0x00C0,    0x80C3,    0x80C5,    0x00C6,    0x80C9,    0x00CA,    0x00CC,    0x80CF,    0x80D1,    0x00D2,    0x00D4,    0x80D7,    0x00D8,    0x80DB,    0x80DD,    0x00DE,
    0x80E1,    0x00E2,    0x00E4,    0x80E7,    0x00E8,    0x80EB,    0x80ED,    0x00EE,    0x00F0,    0x80F3,    0x80F5,    0x00F6,    0x80F9,    0x00FA,    0x00FC,    0x80FF,
    0x8101,    0x0102,    0x0104,    0x8107,    0x0108,    0x810B,    0x810D,    0x010E,    0x0110,    0x8113,    0x8115,    0x0116,    0x8119,    0x011A,    0x011C,    0x811F,
    0x0120,    0x8123,    0x8125,    0x0126,    0x8129,    0x012A,    0x012C,    0x812F,    0x8131,    0x0132,    0x0134,    0x8137,    0x0138,    0x813B,    0x813D,    0x013E,
    0x0140,    0x8143,    0x8145,    0x0146,    0x8149,    0x014A,    0x014C,    0x814F,    0x8151,    0x0152,    0x0154,    0x8157,    0x0158,    0x815B,    0x815D,    0x015E,
    0x8161,    0x0162,    0x0164,    0x8167,    0x0168,    0x816B,    0x816D,    0x016E,    0x0170,    0x8173,    0x8175,    0x0176,    0x8179,    0x017A,    0x017C,    0x817F,
    0x0180,    0x8183,    0x8185,    0x0186,    0x8189,    0x018A,    0x018C,    0x818F,    0x8191,    0x0192,    0x0194,    0x8197,    0x0198,    0x819B,    0x819D,    0x019E,
    0x81A1,    0x01A2,    0x01A4,    0x81A7,    0x01A8,    0x81AB,    0x81AD,    0x01AE,    0x01B0,    0x81B3,    0x81B5,    0x01B6,    0x81B9,    0x01BA,    0x01BC,    0x81BF,
    0x81C1,    0x01C2,    0x01C4,    0x81C7,    0x01C8,    0x81CB,    0x81CD,    0x01CE,    0x01D0,    0x81D3,    0x81D5,    0x01D6,    0x81D9,    0x01DA,    0x01DC,    0x81DF,
    0x01E0,    0x81E3,    0x81E5,    0x01E6,    0x81E9,    0x01EA,    0x01EC,    0x81EF,    0x81F1,    0x01F2,    0x01F4,    0x81F7,    0x01F8,    0x81FB,    0x81FD,    0x01FE
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * CRC, checksum
 *-----------------------------------------------------------------------------*/
/* CRC16 */
     //void   Utility_InitCRC16Table(void);
       u16    Utility_CalculateCRC16(u8 *ptr_data, u16 len_data, u16 crc_init);

/* checksum */
       u8     Utility_CalculateChecksum(u8 *ptr_data, u16 len_data, u8 checksum_init);


/*-----------------------------------------------------------------------------
 * Format conversion
 *-----------------------------------------------------------------------------*/
/* format conversion */
       u8     Utility_BoolToString  (bool   bool_value     , ascii *ptr_string);
       u8     Utility_U32ToString   (u32    u32_value      , ascii *ptr_string);
       u8     Utility_FloatToString (float  float_value    , ascii *ptr_string, u8 precision);
       u8     Utility_DoubleToString(double double_value   , ascii *ptr_string, u8 precision);
       u8     Utility_S16ExpNToASCII(s16    s16_exp_n_value, ascii *ptr_string, s8 n, u8 num_digits);
       u8     Utility_S32ExpNToASCII(s32    s32_exp_n_value, ascii *ptr_string, s8 n, u8 num_digits);

       u32    Utility_HexToInt (ascii msb, ascii lsb);
       u32    Utility_Hexatoi32(const ascii *src);

       void   Utility_ByteToHex(u8 byte, ascii *ptr_msb, ascii *ptr_lsb);


/*-----------------------------------------------------------------------------
 * Base64
 *-----------------------------------------------------------------------------*/
       void   Utility_Integer6BitsToBase64            (u8  data_in, ascii *ptr_data_out);
       void   Utility_Integer12BitsToBase64           (u16 data_in, ascii *ptr_data_out);
       void   Utility_Integer24BitsToBase64           (u32 data_in, ascii *ptr_data_out);
       void   Utility_Integer32BitsToBase64           (u32 data_in, ascii *ptr_data_out);
       void   Utility_Integer60BitsToBase64           (u64 data_in, ascii *ptr_data_out);
       void   Utility_Integer8BitsToBase64WithPadding (u32 data_in, ascii *ptr_data_out);
       void   Utility_Integer16BitsToBase64WithPadding(u32 data_in, ascii *ptr_data_out);
       void   Utility_Integer24BitsToBase64WithPadding(u32 data_in, ascii *ptr_data_out);
static void   Utility_1ByteToBase64                   (BASE64_INPUT input, BASE64_OUTPUT *ptr_output);
static void   Utility_2BytesToBase64                  (BASE64_INPUT input, BASE64_OUTPUT *ptr_output);
static void   Utility_3BytesToBase64                  (BASE64_INPUT input, BASE64_OUTPUT *ptr_output);
static ascii  Utility_ByteToBase64                    (u8 c);


/*-----------------------------------------------------------------------------
 * Interpolation
 *-----------------------------------------------------------------------------*/
/* polynomial interpolation */
       float  Utility_InterpolationPolynomial(float *ptr_poly_coeff, u8 degree, float x);

/* table interpolation */
       float  Utility_InterpolationTableFloat(float *ptr_table_coeff_x, float *ptr_table_coeff_y, u8 table_coeff_size, float x);
       s16    Utility_InterpolationTableInt  (u32   *ptr_table_coeff_x, s16   *ptr_table_coeff_y, u8 table_coeff_size, u32   x);

/* table quantization */
       u8     Utility_QuantizationTableFloat (float *ptr_table_coeff_x, u8 table_coeff_size, float x);
       u8     Utility_QuantizationTableInt   (u32   *ptr_table_coeff_x, u8 table_coeff_size, u32   x);


/*-----------------------------------------------------------------------------
 * Strings
 *-----------------------------------------------------------------------------*/
       bool   Utility_IsNumString   (const ascii *ptr_string);
       bool   Utility_IsHexaString  (const ascii *ptr_string);
       bool   Utility_IsPhoneString (ascii *ptr_string);
       bool   Utility_IsFloat1String(ascii *ptr_string, u8 max_num_digits_decimal_part);
       bool   Utility_IsFloat2String(ascii *ptr_string, u8 max_num_digits_decimal_part);
       bool   Utility_CheckString   (ascii *ptr_string, u8 string_type, bool string_quoted, u8 string_min_length, u8 string_max_length, u8 max_num_digits_decimal_part);
       ascii *Utility_Strtok        (ascii *ptr_string,       ascii  delimiter);
       ascii *Utility_Strtok2       (ascii *ptr_string, const ascii *delimiters, ascii *ptr_delimiter);
       void   Utility_ReverseString (ascii *ptr_string);
       bool   Utility_Strcmp        (ascii *ptr_string_1, ascii *ptr_string_2);


/*-----------------------------------------------------------------------------
 * Random
 *-----------------------------------------------------------------------------*/
static u32    Utility_RandInit                (void);
       u32    Utility_RandomInteger           (u32 range_start, u32 range_size);
       void   Utility_RandomAlphanumericString(u8 string_len, ascii *random_string);


/*-----------------------------------------------------------------------------
 * Debug
 *-----------------------------------------------------------------------------*/
       void   Utility_PrintData(u8 trace_level, u32 len_data, u8 *ptr_data);




/*===========================================================================
 * Function   : Utility_InitCRC16Table
 *
 * Description: init the CRC16 table
 * Input      : -
 * Output     : -
 *===========================================================================*/
//void Utility_InitCRC16Table(void)
//{
//    u32 crc;
//    u32 i;
//    u32 j;
//
//
//    for (i = 0; i < 256; i++)
//    {
//        crc = (i << 8);
//        for (j = 0; j < 8; j++)
//            crc = (crc << 1) ^ ((crc & 0x8000) ? CRC16_POLYNOMIAL : 0);
//
//        Utility_Crc16Table[i] = crc & 0xFFFF;
//    }
//}




/*===========================================================================
 * Function   : Utility_CalculateCRC16
 *
 * Description: calculate the CRC16 on a block of data
 * Input      : - ptr_data: pointer to data
 *              - len_data: length of  data
 *              - crc_init: initial CRC16 value
 * Output     : CRC16 calculated
 *===========================================================================*/
u16 Utility_CalculateCRC16(u8 *ptr_data, u16 len_data, u16 crc_init)
{
    u16 crc;
    u16 i;


    crc = crc_init;

    for (i = 0; i < len_data; i++)
        crc = Utility_Crc16Table[(crc >> 8) & 0xFF] ^ (crc << 8) ^ *ptr_data++;

    return crc;
}




/*===========================================================================
 * Function   : Utility_CalculateChecksum
 *
 * Description: calculate the checksum on a block of data
 *              (LSB of the 1's complement of the sum of the data block bytes)
 * Input      : - ptr_data     : pointer to data
 *              - len_data     : length of  data
 *              - checksum_init: initial checksum value
 * Output     : calculated checksum
 *===========================================================================*/
u8 Utility_CalculateChecksum(u8 *ptr_data, u16 len_data, u8 checksum_init)
{
    u8  checksum;
    u16 i;


    checksum = ~checksum_init;

    for (i = 0; i < len_data; i++)
        checksum += *ptr_data++;
    checksum = ~checksum;

    return checksum;
}




/*===========================================================================
 * Function   : Utility_BoolToString
 *
 * Description: convert a boolean to ASCII string
 * Input      : - bool_value: boolean to be converted
 *              - ptr_string: pointer to store the ASCII string
 * Output     : length of the obtained ASCII string
 *===========================================================================*/
u8 Utility_BoolToString(bool bool_value, ascii *ptr_string)
{
    if (bool_value == TRUE)
    {
        ptr_string[0] = 'T';
        ptr_string[1] = 'R';
        ptr_string[2] = 'U';
        ptr_string[3] = 'E';
        ptr_string[4] = '\0';

        return 4;
    }
    else
    {
        ptr_string[0] = 'F';
        ptr_string[1] = 'A';
        ptr_string[2] = 'L';
        ptr_string[3] = 'S';
        ptr_string[4] = 'E';
        ptr_string[5] = '\0';

        return 5;
    }
}




/*===========================================================================
 * Function   : Utility_U32ToString
 *
 * Description: convert a u32 number to ASCII string
 * Input      : - u32_value : u32 number to be converted
 *              - ptr_string: pointer to store the ASCII string
 * Output     : length of the obtained ASCII string
 *===========================================================================*/
u8 Utility_U32ToString(u32 u32_value, ascii *ptr_string)
{
    u8 num_digits;
    u8 digit;
    u8 buffer[11];
    u8 i;


    num_digits = 0;
    if (u32_value == 0)
    {
        *(ptr_string + num_digits) = '0';
        num_digits++;
    }
    else
    {
        while (u32_value != 0)
        {
            digit   = u32_value % 10;
            u32_value /= 10;
            buffer[num_digits] = digit + '0';
            num_digits++;
        }

        for (i = 0; i < num_digits; i++)
            *(ptr_string + i) = buffer[num_digits - 1 - i];
    }

    *(ptr_string + num_digits) = 0x00;

    return (strlen(ptr_string));
}




/*=============================================================================
 * Function   : Utility_FloatToString
 *
 * Description: convert a float number to ASCII string
 * Input      : - float_value: float number to be converted
 *              - ptr_string : pointer to store the ASCII string
 *              - precision  : number of digits for decimal part (0, ..., 6)
 * Output     : length of the obtained ASCII string
 *=============================================================================*/
u8 Utility_FloatToString(float float_value, ascii *ptr_string, u8 precision)
{
    sprintf(ptr_string, "%.*f", precision, float_value);

    return (strlen(ptr_string));


//    float precision_factor;
//    float float_frac_part;
//
//    s32   int_part;
//    u32   frac_part;
//
//  //u32 precision_factor;
//  //s32 d;
//
//
//    switch (precision)
//    {
//        /* decimal precision: 0 digits */
//        case 0:
//            precision_factor = 1;
//            break;
//
//        /* decimal precision: 1 digit  */
//        case 1:
//            precision_factor = 10;
//            break;
//
//        /* decimal precision: 2 digits */
//        case 2:
//            precision_factor = 100;
//            break;
//
//        /* decimal precision: 3 digits */
//        case 3:
//            precision_factor = 1000;
//            break;
//
//        /* decimal precision: 4 digits */
//        case 4:
//            precision_factor = 10000;
//            break;
//
//        /* decimal precision: 5 digits */
//        case 5:
//            precision_factor = 100000;
//            break;
//
//        /* decimal precision: 6 digits */
//        default:
//        case 6:
//            precision_factor = 1000000;
//            break;
//    }
//
//
//    int_part        = (s32)float_value;
//    float_frac_part = fabs(float_value - (float)int_part);
//    frac_part       = (u32)trunc(float_frac_part * (float)precision_factor);
//
//    //int_part  = (s32)float_value;
//    //d         =            ((s32)float_value *        precision_factor);
//    //frac_part = labs((s32)((     float_value * (float)precision_factor) - (float)d));
//
//
//    if (precision == 0)
//        sprintf(ptr_string, "%ld"      , int_part);
//    else
//        sprintf(ptr_string, "%ld.%0*ld", int_part, precision, frac_part);
//
//
//    return (strlen(ptr_string));
}




/*===========================================================================
 * Function   : Utility_DoubleToString
 *
 * Description: convert a double number to ASCII string
 * Input      : - double_value: double number to be converted
 *              - ptr_string  : pointer to store the ASCII string
 *              - precision  : number of digits for decimal part (0, ..., 6)
 * Output     : length of the obtained ASCII string
 *===========================================================================*/
u8 Utility_DoubleToString(double double_value, ascii *ptr_string, u8 precision)
{
    sprintf(ptr_string, "%.*f", precision, double_value);

    return (strlen(ptr_string));


//    double precision_factor;
//    double double_frac_part;
//
//    s32    int_part;
//    u32    frac_part;
//
//  //u32 precision_factor;
//  //s32 d;
//
//
//    switch (precision)
//    {
//        /* decimal precision: 0 digits */
//        case 0:
//            precision_factor = 1;
//            break;
//
//        /* decimal precision: 1 digit  */
//        case 1:
//            precision_factor = 10;
//            break;
//
//        /* decimal precision: 2 digits */
//        case 2:
//            precision_factor = 100;
//            break;
//
//        /* decimal precision: 3 digits */
//        case 3:
//            precision_factor = 1000;
//            break;
//
//        /* decimal precision: 4 digits */
//        case 4:
//            precision_factor = 10000;
//            break;
//
//        /* decimal precision: 5 digits */
//        case 5:
//            precision_factor = 100000;
//            break;
//
//        /* decimal precision: 6 digits */
//        default:
//        case 6:
//            precision_factor = 1000000;
//            break;
//    }
//
//
//    int_part         = (s32)double_value;
//    double_frac_part = fabs(double_value - (double)int_part);
//    frac_part        = (u32)trunc(double_frac_part * (double)precision_factor);
//
//    //int_part  = (s32)double_value;
//    //d         =            ((s32)double_value *         precision_factor);
//    //frac_part = labs((s32)((     double_value * (double)precision_factor) - (double)d));
//
//
//    if (precision == 0)
//        sprintf(ptr_string, "%ld"      , int_part);
//    else
//        sprintf(ptr_string, "%ld.%0*ld", int_part, precision, frac_part);
//
//
//    return (strlen(ptr_string));
}




/*=============================================================================
 * Function   : Utility_HexToInt
 *
 * Description: convert 2 characters in hex format to int format
 *              For example:
 *                    Utility_ConvertHexToInt('A', '3') returns 0xA3 (=163)
 * Input      : - msb: Most Significative Byte
 *              - lsb: Less Significative Byte
 * Output     : -
 *=============================================================================*/
u32 Utility_HexToInt(ascii msb, ascii lsb)
{
    ascii buffer[3];
    u32   val;


    buffer[0] = msb;
    buffer[1] = lsb;
    buffer[2] = 0x00;

    val = strtol((const ascii*)buffer, NULL, 16);

    return val;
}




/*=============================================================================
 * Function   : Utility_Hexatoi32
 *
 * Description: convert a string in hex format to 32 bits unsigned int format
 *              For example:
 *                    Utility_ConvertHexToInt("13A") returns 0x0000013A (=314)
 * Input      : - src: hex string to be converted
 * Output     : - result of the conversion
 *=============================================================================*/
u32 Utility_Hexatoi32(const ascii *src)
{
    const ascii *ptr;
          u8     len;
          u8     hex_digit_char;
          u8     hex_digit_int;
          u32    val;
          u32    base;
          u8     i;


    if (!Utility_IsHexaString(src))
        return 0;

    len = (u8)strlen(src);
    if (len > 8)
        len = 8;

    ptr = src;

    val  = 0;
    base = 1;

    for (i = len; i > 0; i--)
    {
        hex_digit_char = *(ptr + i - 1);
        hex_digit_int  = (hex_digit_char >= 'a')   ?   (hex_digit_char - 'a' + 10)   :   ((hex_digit_char >= 'A')  ?  (hex_digit_char - 'A' + 10)  :  (hex_digit_char - '0'));

        val += (u32)hex_digit_int * base;

        base *= 16;
    }

    return val;
}




/*=============================================================================
 * Function   : Utility_ByteToHex
 *
 * Description: concert one byte to the 2 correspondig hex digits in ASCII format
 *                 Example: 0xA6  --->  'A' '6'
 * Input      : - byte   : byte to be converted
 *              - ptr_msb: pointer to save the most significative character
 *              - ptr_lsb: pointer to save the less significative character
 * Output     : -
 *=============================================================================*/
void Utility_ByteToHex(u8 byte, ascii *ptr_msb, ascii *ptr_lsb)
{
    u8 msn;
    u8 lsn;


    msn = (byte >> 4) & 0x0F;
    lsn = (byte     ) & 0x0F;

    *ptr_msb = (msn > 9) ? (msn - 10 + 'A') : (msn + '0');
    *ptr_lsb = (lsn > 9) ? (lsn - 10 + 'A') : (lsn + '0');
}




/*=============================================================================
 * Function   : Utility_Integer6BitsToBase64
 *
 * Description: convert a 6-bits unsigned integer to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer6BitsToBase64(u8 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = 0x00;
    input.a2 = 0x00;
    input.a3 = data_in & 0x3F;    // only 6 bits

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[0] = output.b4;


    ptr_data_out[1] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer12BitsToBase64
 *
 * Description: convert a 12-bits unsigned integer to the base64 ASCII representation
 * Input      : - data_in     :
 *              - ptr_data_out:
 * Output     : -
 *=============================================================================*/
void Utility_Integer12BitsToBase64(u16 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = 0x00;
    input.a2 = (u8)((data_in >> 8) & 0x000F);   // only 4 bits
    input.a3 = (u8)((data_in     ) & 0x00FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[0] = output.b3;
    ptr_data_out[1] = output.b4;


    ptr_data_out[2] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer24BitsToBase64
 *
 * Description: convert a 24-bits unsigned integer to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer24BitsToBase64(u32 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = (u8)((data_in >> 16) & 0x000000FF);
    input.a2 = (u8)((data_in >>  8) & 0x000000FF);
    input.a3 = (u8)((data_in      ) & 0x000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[0] = output.b1;
    ptr_data_out[1] = output.b2;
    ptr_data_out[2] = output.b3;
    ptr_data_out[3] = output.b4;


    ptr_data_out[4] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer32BitsToBase64
 *
 * Description: convert a 32-bits unsigned integer to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer32BitsToBase64(u32 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = 0x00;
    input.a2 = 0x00;
    input.a3 = (u8)((data_in >> 24) & 0x000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[0] = output.b3;
    ptr_data_out[1] = output.b4;


    input.a1 = (u8)((data_in >> 16) & 0x000000FF);
    input.a2 = (u8)((data_in >>  8) & 0x000000FF);
    input.a3 = (u8)((data_in      ) & 0x000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[2] = output.b1;
    ptr_data_out[3] = output.b2;
    ptr_data_out[4] = output.b3;
    ptr_data_out[5] = output.b4;


    ptr_data_out[6] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer60BitsToBase64
 *
 * Description: convert a 60-bits unsigned integer to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer60BitsToBase64(u64 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = 0x00;
    input.a2 = (u8)((data_in >> 58) & 0x000000000000000F);   // only 4 bits
    input.a3 = (u8)((data_in >> 48) & 0x00000000000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[ 0] = output.b3;
    ptr_data_out[ 1] = output.b4;


    input.a1 = (u8)((data_in >> 40) & 0x00000000000000FF);
    input.a2 = (u8)((data_in >> 32) & 0x00000000000000FF);
    input.a3 = (u8)((data_in >> 24) & 0x00000000000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[ 2] = output.b1;
    ptr_data_out[ 3] = output.b2;
    ptr_data_out[ 4] = output.b3;
    ptr_data_out[ 5] = output.b4;


    input.a1 = (u8)((data_in >> 16) & 0x00000000000000FF);
    input.a2 = (u8)((data_in >>  8) & 0x00000000000000FF);
    input.a3 = (u8)((data_in      ) & 0x00000000000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[ 6] = output.b1;
    ptr_data_out[ 7] = output.b2;
    ptr_data_out[ 8] = output.b3;
    ptr_data_out[ 9] = output.b4;


    ptr_data_out[10] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer8BitsToBase64WithPadding
 *
 * Description: convert a 8-bits unsigned integer to the base64 ASCII representation
 *              with the padding characters
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer8BitsToBase64WithPadding(u32 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = (u8)((data_in >> 16) & 0x000000FF);
    input.a2 = 0x00;
    input.a3 = 0x00;

    Utility_1ByteToBase64(input, &output);

    ptr_data_out[0] = output.b1;
    ptr_data_out[1] = output.b2;
    ptr_data_out[2] = output.b3;
    ptr_data_out[3] = output.b4;


    ptr_data_out[4] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer16BitsToBase64WithPadding
 *
 * Description: convert a 16-bits unsigned integer to the base64 ASCII representation
 *              with the padding characters
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer16BitsToBase64WithPadding(u32 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = (u8)((data_in >> 16) & 0x000000FF);
    input.a2 = (u8)((data_in >>  8) & 0x000000FF);
    input.a3 = 0x00;

    Utility_2BytesToBase64(input, &output);

    ptr_data_out[0] = output.b1;
    ptr_data_out[1] = output.b2;
    ptr_data_out[2] = output.b3;
    ptr_data_out[3] = output.b4;


    ptr_data_out[4] = 0x00;
}




/*=============================================================================
 * Function   : Utility_Integer24BitsToBase64WithPadding
 *
 * Description: convert a 24-bits unsigned integer to the base64 ASCII representation
 *              with the padding characters
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
void Utility_Integer24BitsToBase64WithPadding(u32 data_in, ascii *ptr_data_out)
{
    BASE64_INPUT  input;
    BASE64_OUTPUT output;


    input.a1 = (u8)((data_in >> 16) & 0x000000FF);
    input.a2 = (u8)((data_in >>  8) & 0x000000FF);
    input.a3 = (u8)((data_in      ) & 0x000000FF);

    Utility_3BytesToBase64(input, &output);

    ptr_data_out[0] = output.b1;
    ptr_data_out[1] = output.b2;
    ptr_data_out[2] = output.b3;
    ptr_data_out[3] = output.b4;


    ptr_data_out[4] = 0x00;
}




/*=============================================================================
 * Function   : Utility_1ByteToBase64
 *
 * Description: convert a 1-byte to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
static void Utility_1ByteToBase64(BASE64_INPUT input, BASE64_OUTPUT *ptr_output)
{
    u32 num;
    u8  c;


    input.a2 = 0x00;
    input.a3 = 0x00;

    num = ((u32)input.a1 << 16) |
          ((u32)input.a2 <<  8) |
          ((u32)input.a3      );


    ptr_output->b4 = '=';  // padding character

    ptr_output->b3 = '=';  // padding character

    c = (u8)((num >> 12) & 0x0000003F);
    ptr_output->b2 = Utility_ByteToBase64(c);

    c = (u8)((num >> 18) & 0x0000003F);
    ptr_output->b1 = Utility_ByteToBase64(c);
}




/*=============================================================================
 * Function   : Utility_2BytesToBase64
 *
 * Description: convert a 2-bytes to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
static void Utility_2BytesToBase64(BASE64_INPUT input, BASE64_OUTPUT *ptr_output)
{
    u32 num;
    u8  c;


    input.a3 = 0x00;

    num = ((u32)input.a1 << 16) |
          ((u32)input.a2 <<  8) |
          ((u32)input.a3      );


    ptr_output->b4 = '=';  // padding character

    c = (u8)((num >>  6) & 0x0000003F);
    ptr_output->b3 = Utility_ByteToBase64(c);

    c = (u8)((num >> 12) & 0x0000003F);
    ptr_output->b2 = Utility_ByteToBase64(c);

    c = (u8)((num >> 18) & 0x0000003F);
    ptr_output->b1 = Utility_ByteToBase64(c);
}




/*=============================================================================
 * Function   : Utility_3BytesToBase64
 *
 * Description: convert a 3-bytes to the base64 ASCII representation
 * Input      : - data_in     :                 data to be converted
 *              - ptr_data_out: pointer to save data       converted
 * Output     : -
 *=============================================================================*/
static void Utility_3BytesToBase64(BASE64_INPUT input, BASE64_OUTPUT *ptr_output)
{
    u32 num;
    u8  c;


    num = ((u32)input.a1 << 16) |
          ((u32)input.a2 <<  8) |
          ((u32)input.a3      );


    c = (u8)((num      ) & 0x0000003F);
    ptr_output->b4 = Utility_ByteToBase64(c);

    c = (u8)((num >>  6) & 0x0000003F);
    ptr_output->b3 = Utility_ByteToBase64(c);

    c = (u8)((num >> 12) & 0x0000003F);
    ptr_output->b2 = Utility_ByteToBase64(c);

    c = (u8)((num >> 18) & 0x0000003F);
    ptr_output->b1 = Utility_ByteToBase64(c);
}




/*=============================================================================
 * Function   : Utility_ByteToBase64
 *
 * Description: convert a 6-bits unsigned integer to the relative base64 ASCII
 *              character
 *              http://it.wikipedia.org/wiki/Base64
 *              https://www.base64encode.org/
 * Input      : - c: 6-bits unsigned integer to be converted
 * Output     : - base64 ASCII character output
 *=============================================================================*/
static ascii Utility_ByteToBase64(u8 c)
{
    if      (c >= 64)
        return ('?');              // ERROR
    else if (c == 63)
        return ('/');              // =47 =0x2F
    else if (c == 62)
        return ('+');              // =43 =0x2B
    else if (c >= 52)
        return (c - (52  - '0'));  // -4
    else if (c >= 26)
        return (c + ('a' - 26 ));  // +71
    else
        return (c + ('A' -  0) );  // +65
}




/*=============================================================================
 * Function   : Utility_InterpolationPolynomial
 *
 * Description: It finds the "y" value corresponding to a specified "x" value
 *              based on interpolation polynomial.
 *              It simply calculates the value of the polynomial for the specified value.
 *              It works with floatig point (float -->  single precision)
 *
 *              y = (a_n * x^n) + (a_n-1 * x^(n-1)) + ... + (a_1 * x^1) + a_0
 *
 * Input      : - ptr_poly_coeff: pointer to the interpolation polynomial coefficients
 *              - degree        : polynomial degree
 *              - x             : specified value
 * Output     : - result of the polynomial interpolation
 *=============================================================================*/
float Utility_InterpolationPolynomial(float *ptr_poly_coeff, u8 degree, float x)
{
    double x_exp_i;
    double y;
    u8     i;


    y = 0.0;

    for (i = 0; i < (degree + 1); i++)
    {
        x_exp_i  = pow((double)x, (double)(degree - i));
        y       += ((double)ptr_poly_coeff[i] * x_exp_i);   // + (a_i * x^i)
    }

    return ((float)y);
}




/*=============================================================================
 * Function   : Utility_InterpolationTableFloat
 *
 * Description: It finds the "y" value corresponding to a specified "x" value based on
 *              a curve defined by 2 tables of coefficients ("x" and "y").
 *              The 2 tables define a broken line.
 *              The coefficients in the 2 tables have to be sorted.
 *              So it does an interpolation based on these 2 tables of coefficients.
 *              It works with floatig point (float -->  single precision).
 *
 * Input      : - ptr_table_coeff_x: pointer to the "x" coefficients table
 *            : - ptr_table_coeff_y: pointer to the "y" coefficients table
 *              - table_coeff_size : size of the "x" and "y" coefficients tables
 *              - x                : specified value
 * Output     : - result of the table interpolation
 *=============================================================================*/
float Utility_InterpolationTableFloat(float *ptr_table_coeff_x, float *ptr_table_coeff_y, u8 table_coeff_size, float x)
{
    float x1;
    float x2;

    float y1;
    float y2;

    float y;

    u8    i;


    if (table_coeff_size == 0)
        return x;


    /* compare with the first "x" element of the table */
    if (x <= ptr_table_coeff_x[0])
    {
        /* x is smaller than or equal to the first "x" coefficient of the table */
        y = ptr_table_coeff_y[0];
        return y;
    }


    /* compare with the last "x" element of the table */
    if (x >= ptr_table_coeff_x[table_coeff_size - 1])
    {
        /* x is greater than or equal to the last "x" coefficient of the table */
        y = ptr_table_coeff_y[table_coeff_size - 1];
        return y;
    }


    /* find the interval of the "x" table where x stays */
    for (i = 0; i < (table_coeff_size - 1); i++)
    {
        if      (x == ptr_table_coeff_x[i])
        {
            /* x is equal to the i-th coefficient of the "x" table */
            y = ptr_table_coeff_y[i];
            return y;
        }
        else if (x <  ptr_table_coeff_x[i + 1])
        {
            /* x stays between the i-th and the i+1-th coefficients of the "x" table */
            break;
        }
    }


    /* values of the 2 extremes of the segment of the broken line where the x stays */
    x1 = ptr_table_coeff_x[i    ];
    x2 = ptr_table_coeff_x[i + 1];
    y1 = ptr_table_coeff_y[i    ];
    y2 = ptr_table_coeff_y[i + 1];


    /* calculate the y value for the x value for that segment */
    /*
     *   y  - y1     x  - x1                [(x2 * y1) - (x1 * y2)] + [x * (y2 - y1)]
     *  --------- = ---------   --->   x = -------------------------------------------
     *   y2 - y1     x2 - x1                               x2 - x1
     */
    y  = (x2 * y1) - (y2 * x1);
    y += (x * (y2 - y1));
    y /= (x2 - x1);


    return y;
}




/*=============================================================================
 * Function   : Utility_InterpolationTableInt
 *
 * Description: It finds the "y" value corresponding to a specified "x" value based on
 *              a curve defined by 2 tables of coefficients ("x" and "y").
 *              The 2 tables define a broken line.
 *              The coefficients in the 2 tables have to be sorted.
 *              So it does an interpolation based on these 2 tables of coefficients.
 *              It works with integer.
 *
 * Input      : - ptr_table_coeff_x: pointer to the "x" coefficients table
 *            : - ptr_table_coeff_y: pointer to the "y" coefficients table
 *              - table_coeff_size : size of the "x" and "y" coefficients tables
 *              - x                : specified value
 * Output     : - result of the table interpolation
 *=============================================================================*/
s16 Utility_InterpolationTableInt(u32 *ptr_table_coeff_x, s16 *ptr_table_coeff_y, u8 table_coeff_size, u32 x)
{
    s32 x1;
    s32 x2;

    s32 y1;
    s32 y2;

    s32 temp_s32;

    s16 y;

    u8  i;


    if (table_coeff_size == 0)
        return x;


    /* compare with the first "x" element of the table */
    if (x <= ptr_table_coeff_x[0])
    {
        /* x is smaller than or equal to the first "x" coefficient of the table */
        y = ptr_table_coeff_y[0];
        return y;
    }


    /* compare with the last "x" element of the table */
    if (x >= ptr_table_coeff_x[table_coeff_size - 1])
    {
        /* x is greater than or equal to the last "x" coefficient of the table */
        y = ptr_table_coeff_y[table_coeff_size - 1];
        return y;
    }


    /* find the interval of the "x" table where x stays */
    for (i = 0; i < (table_coeff_size - 1); i++)
    {
        if      (x == ptr_table_coeff_x[i])
        {
            /* x is equal to the i-th coefficient of the "x" table */
            y = ptr_table_coeff_y[i];
            return y;
        }
        else if (x <  ptr_table_coeff_x[i + 1])
        {
            /* x stays between the i-th and the i+1-th coefficients of the "x" table */
            break;
        }
    }


    /* values of the 2 extremes of the segment of the broken line where the x stays */
    x1 = (s32)ptr_table_coeff_x[i    ];
    x2 = (s32)ptr_table_coeff_x[i + 1];
    y1 = (s32)ptr_table_coeff_y[i    ];
    y2 = (s32)ptr_table_coeff_y[i + 1];

    /* calculate the y value for the x value for that segment */
    /*
     *   y  - y1     x  - x1                [(x2 * y1) - (x1 * y2)] + [x * (y2 - y1)]
     *  --------- = ---------   --->   x = -------------------------------------------
     *   y2 - y1     x2 - x1                               x2 - x1
     */
    temp_s32  = (x2 * y1) - (x1 * y2);
    temp_s32 += ((s32)x * (y2 - y1));
    temp_s32 /= (x2 - x1);

    y = (s16)temp_s32;

    return y;
}




/*=============================================================================
 * Function   : Utility_QuantizationTableFloat
 *
 * Description: It finds the "xq" value obtained with a quantization based on
 *              a table of coefficients ("x").
 *              The coefficients in the table have to be sorted.
 *              It works with floatig point (float -->  single precision).
 *
 * Input      : - ptr_table_coeff_x: pointer to the "x" coefficients table
 *              - table_coeff_size : size of    the "x" coefficients table
 *              - x                : specified value
 * Output     : - index of the coefficent of the table that is the result of the quantization
 *=============================================================================*/
u8 Utility_QuantizationTableFloat(float *ptr_table_coeff_x, u8 table_coeff_size, float x)
{
    u8 i;


    if (table_coeff_size == 0)
        return 0;


        /* compare with the first "x" element of the table */
    if (x <= ptr_table_coeff_x[0])
    {
        /* x is smaller than or equal to the first "x" coefficient of the table */
        return 0;
    }


    /* compare with the last "x" element of the table */
    if (x >= ptr_table_coeff_x[table_coeff_size - 1])
    {
        /* x is greater than or equal to the last "x" coefficient of the table */
        return (table_coeff_size - 1);
    }


    /* find the interval of the "x" table where x stays */
    for (i = 0; i < (table_coeff_size - 1); i++)
    {
        if      (x == ptr_table_coeff_x[i])
        {
            /* x is equal to the i-th coefficient of the "x" table */
            return i;
        }
        else if (x <  ptr_table_coeff_x[i + 1])
        {
            /* x stays between the i-th and the i+1-th coefficients of the "x" table */

            /* calculate which is the nearest coefficient between i-th and the i+1-th coefficients */
            if ((x - ptr_table_coeff_x[i])  <=  (ptr_table_coeff_x[i + 1] - x))
                return (i    );   // i-th   coefficient is the nearest
            else
                return (i + 1);   // i+1-th coefficient is the nearest
        }
    }


    return 0;
}




/*=============================================================================
 * Function   : Utility_QuantizationTableInt
 *
 * Description: It finds the "xq" value obtained with a quantization based on
 *              a table of coefficients ("x").
 *              The coefficients in the table have to be sorted.
 *              It works with integer.
 *
 * Input      : - ptr_table_coeff_x: pointer to the "x" coefficients table
 *              - table_coeff_size : size of    the "x" coefficients table
 *              - x                : specified value
 * Output     : - index of the coefficent of the table that is the result of the quantization
 *=============================================================================*/
u8 Utility_QuantizationTableInt(u32 *ptr_table_coeff_x, u8 table_coeff_size, u32 x)
{
    u8 i;


    /* compare with the first "x" element of the table */
    if (x <= ptr_table_coeff_x[0])
    {
        /* x is smaller than or equal to the first "x" coefficient of the table */
        return 0;
    }


    /* compare with the last "x" element of the table */
    if (x >= ptr_table_coeff_x[table_coeff_size - 1])
    {
        /* x is greater than or equal to the last "x" coefficient of the table */
        return (table_coeff_size - 1);
    }


    /* find the interval of the "x" table where x stays */
    for (i = 0; i < (table_coeff_size - 1); i++)
    {
        if      (x == ptr_table_coeff_x[i])
        {
            /* x is equal to the i-th coefficient of the "x" table */
            return i;
        }
        else if (x <  ptr_table_coeff_x[i + 1])
        {
            /* x stays between the i-th and the i+1-th coefficients of the "x" table */

            /* calculate which is the nearest coefficient between i-th and the i+1-th coefficients */
            if ((x - ptr_table_coeff_x[i])  <=  (ptr_table_coeff_x[i + 1] - x))
                return (i    );   // i-th   coefficient is the nearest
            else
                return (i + 1);   // i+1-th coefficient is the nearest
        }
    }


    return 0;
}




/*=============================================================================
 * Function   : Utility_S16ExpNToASCII
 *
 * Description: convert an integer number with sign (s16) expressed in
 *              [ x(10^n) ] format to the corrispondent ASCII string.
 *
 *              The integer number is expressed in [ x(10^n) ] format.
 *              Example:
 *                12345
 *                    (n = +3)  -->  12345000
 *                    (n = +2)  -->   1234500
 *                    (n = +1)  -->    123450
 *                    (n =  0)  -->     12345
 *                    (n = -1)  -->      1234.5
 *                    (n = -2)  -->       123.45
 *                    (n = -3)  -->        12.345
 *               -12345
 *                    (n = +3)  --> -12345000
 *                    (n = +2)  -->  -1234500
 *                    (n = +1)  -->   -123450
 *                    (n =  0)  -->    -12345
 *                    (n = -1)  -->     -1234.5
 *                    (n = -2)  -->      -123.45
 *                    (n = -3)  -->       -12.345
 *
 *              If "n" is negative, the fractional part in the ASCII string is composed
 *              by "num_digits" digits.
 *              Example (with num_digits = 3):
 *                    |---------|-------------------------------------------------------|
 *                    |         |                           n                           |
 *                    |         |-------------|-------------|-------------|-------------|
 *                    |         |       0     |      -1     |      -2     |      -3     |
 *                    |---------|-------------|-------------|-------------|-------------|
 *                    |      1  |     "1.000" |     "0.100" |     "0.010" |     "0.001" |
 *                    |     12  |    "12.000" |     "1.200" |     "0.120" |     "0.012" |
 *                    |    123  |   "123.000" |    "12.300" |     "1.230" |     "0.123" |
 *                    |   1234  |  "1234.000" |   "123.400" |    "12.340" |     "1.234" |
 *                    |  12345  | "12345.000" |  "1234.500" |   "123.450" |    "12.345" |
 *                    |---------|-------------|-------------|-------------|-------------|
 *
 * Input      : - s16_exp_n_value: number (s16) to be converted
 *              - ptr_string     : pointer to save the ASCII string
 *              - n              : exponent of 10 (-3, -2, -1, 0, +1, +2, +3)
 *              - num_digits     : number of digits of fractional part in ASCII string (0-3) (only for "n" negative)
 * Output     : - length of the obtained ASCII string
 *=============================================================================*/
u8 Utility_S16ExpNToASCII(s16 s16_exp_n_value, ascii *ptr_string, s8 n, u8 num_digits)  //32
{
    /* absolute values */
    u16 u16_exp_n_value_abs;    //32
    u8  n_abs;

    /* interger and fractional parts */
    u16 int_part;   //32
    u16 frac_part;

    /* 10 factor (depending on 'n' value) */
    u16 factor_10;


    /* check "n" (-3, -2, -1, 0, +1, +2, +3) */
    if ((n < -3) || (n > +3))
    {
        ptr_string[0] = 0x00;
        return 0;
    }

    /* check "num_digits" (0-3) */
    if (num_digits > 3)
    {
        ptr_string[0] = 0x00;
        return 0;
    }


    /* calculate the "factor_10" (depending on "n") */
    n_abs = abs(n);
    if      (n_abs == 0)
        factor_10 = 1;
    else if (n_abs == 1)
        factor_10 = 10;
    else if (n_abs == 2)
        factor_10 = 100;
    else if (n_abs == 3)
        factor_10 = 1000;


    /* calculate the integer part and the fractional part */
    u16_exp_n_value_abs = abs(s16_exp_n_value);
    if (n >= 0)
    {
        /* "n" positive */
        int_part  = u16_exp_n_value_abs * factor_10;
        frac_part = 0;
    }
    else
    {
        /* "n" negative */
        int_part  = u16_exp_n_value_abs / factor_10;
        frac_part = u16_exp_n_value_abs % factor_10;
    }


    /* build the string */
    if (s16_exp_n_value >= 0)
    {
        /* positive number or equal to zero */
        if (n >= 0)
        {
            /* "n" positive or zero */
            sprintf(ptr_string, "%d", int_part);
        }
        else
        {
            /* "n" negative */
            if      (num_digits == 0)
                sprintf(ptr_string, "%d"     , int_part           );
            else if (num_digits == 1)
                sprintf(ptr_string, "%d.%01d", int_part, frac_part);
            else if (num_digits == 2)
                sprintf(ptr_string, "%d.%02d", int_part, frac_part);
            else
                sprintf(ptr_string, "%d.%03d", int_part, frac_part);
        }
    }
    else
    {
        /* negative number */
        if (n >= 0)
        {
            /* "n" positive or zero */
            sprintf(ptr_string, "-%d", int_part);
        }
        else
        {
            /* "n" negative */
            if      (num_digits == 0)
                sprintf(ptr_string, "-%d"     , int_part           );
            else if (num_digits == 1)
                sprintf(ptr_string, "-%d.%01d", int_part, frac_part);
            else if (num_digits == 2)
                sprintf(ptr_string, "-%d.%02d", int_part, frac_part);
            else
                sprintf(ptr_string, "-%d.%03d", int_part, frac_part);
        }
    }


    return (strlen(ptr_string));
}




/*=============================================================================
 * Function   : Utility_S32ExpNToASCII
 *
 * Description: convert an integer number with sign (s32) expressed in
 *              [ x(10^n) ] format to the corrispondent ASCII string.
 *
 *              The integer number is expressed in [ x(10^n) ] format.
 *              Example:
 *                12345
 *                    (n = +3)  -->  12345000
 *                    (n = +2)  -->   1234500
 *                    (n = +1)  -->    123450
 *                    (n =  0)  -->     12345
 *                    (n = -1)  -->      1234.5
 *                    (n = -2)  -->       123.45
 *                    (n = -3)  -->        12.345
 *               -12345
 *                    (n = +3)  --> -12345000
 *                    (n = +2)  -->  -1234500
 *                    (n = +1)  -->   -123450
 *                    (n =  0)  -->    -12345
 *                    (n = -1)  -->     -1234.5
 *                    (n = -2)  -->      -123.45
 *                    (n = -3)  -->       -12.345
 *
 *              If "n" is negative, the fractional part in the ASCII string is composed
 *              by "num_digits" digits.
 *              Example (with num_digits = 3):
 *                    |---------|-------------------------------------------------------|
 *                    |         |                           n                           |
 *                    |         |-------------|-------------|-------------|-------------|
 *                    |         |       0     |      -1     |      -2     |      -3     |
 *                    |---------|-------------|-------------|-------------|-------------|
 *                    |      1  |     "1.000" |     "0.100" |     "0.010" |     "0.001" |
 *                    |     12  |    "12.000" |     "1.200" |     "0.120" |     "0.012" |
 *                    |    123  |   "123.000" |    "12.300" |     "1.230" |     "0.123" |
 *                    |   1234  |  "1234.000" |   "123.400" |    "12.340" |     "1.234" |
 *                    |  12345  | "12345.000" |  "1234.500" |   "123.450" |    "12.345" |
 *                    |---------|-------------|-------------|-------------|-------------|
 *
 * Input      : - s32_exp_n_value: number (s32) to be converted
 *              - ptr_string     : pointer to save the ASCII string
 *              - n              : exponent of 10 (-3, -2, -1, 0, +1, +2, +3)
 *              - num_digits     : number of digits of fractional part in ASCII string (0-3) (only for "n" negative)
 * Output     : - length of the obtained ASCII string
 *=============================================================================*/
u8 Utility_S32ExpNToASCII(s32 s32_exp_n_value, ascii *ptr_string, s8 n, u8 num_digits)
{
    /* absolute values */
    u32 u32_exp_n_value_abs;
    u8  n_abs;

    /* interger and fractional parts */
    u32 int_part;
    u32 frac_part;

    /* 10 factor (depending on 'n' value) */
    u16 factor_10;


    /* check "n" (-3, -2, -1, 0, +1, +2, +3) */
    if ((n < -3) || (n > +3))
    {
        ptr_string[0] = 0x00;
        return 0;
    }

    /* check "num_digits" (0-3) */
    if (num_digits > 3)
    {
        ptr_string[0] = 0x00;
        return 0;
    }


    /* calculate the "factor_10" (depending on "n") */
    n_abs = abs(n);
    if      (n_abs == 0)
        factor_10 = 1;
    else if (n_abs == 1)
        factor_10 = 10;
    else if (n_abs == 2)
        factor_10 = 100;
    else if (n_abs == 3)
        factor_10 = 1000;


    /* calculate the integer part and the fractional part */
    u32_exp_n_value_abs = abs(s32_exp_n_value);
    if (n >= 0)
    {
        /* "n" positive */
        int_part  = u32_exp_n_value_abs * factor_10;
        frac_part = 0;
    }
    else
    {
        /* "n" negative */
        int_part  = u32_exp_n_value_abs / factor_10;
        frac_part = u32_exp_n_value_abs % factor_10;
    }


    /* build the string */
    if (s32_exp_n_value >= 0)
    {
        /* positive number or equal to zero */
        if (n >= 0)
        {
            /* "n" positive or zero */
            sprintf(ptr_string, "%ld", int_part);
        }
        else
        {
            /* "n" negative */
            if      (num_digits == 0)
                sprintf(ptr_string, "%ld"      , int_part           );
            else if (num_digits == 1)
                sprintf(ptr_string, "%ld.%01ld", int_part, frac_part);
            else if (num_digits == 2)
                sprintf(ptr_string, "%ld.%02ld", int_part, frac_part);
            else
                sprintf(ptr_string, "%ld.%03ld", int_part, frac_part);
        }
    }
    else
    {
        /* negative number */
        if (n >= 0)
        {
            /* "n" positive or zero */
            sprintf(ptr_string, "-%ld", int_part);
        }
        else
        {
            /* "n" negative */
            if      (num_digits == 0)
                sprintf(ptr_string, "-%ld"      , int_part           );
            else if (num_digits == 1)
                sprintf(ptr_string, "-%ld.%01ld", int_part, frac_part);
            else if (num_digits == 2)
                sprintf(ptr_string, "-%ld.%02ld", int_part, frac_part);
            else
                sprintf(ptr_string, "-%ld.%03ld", int_part, frac_part);
        }
    }


    return (strlen(ptr_string));
}




/*=============================================================================
 * Function   : Utility_IsNumString
 *
 * Description: verify if a string is a numeric string
 * Input      : - ptr_string: pointer to the string to be verified
 * Output     : - FALSE: the string is not a numeric string
 *              - TRUE : the string is     a numeric string
 *=============================================================================*/
bool Utility_IsNumString(const ascii *ptr_string)
{
    u8 character;


    if (( ptr_string) == NULL)
        return FALSE;

    if ((*ptr_string) == '\0')
        return FALSE;


    while ((character = *ptr_string++) != '\0')
    {
        if (!isdigit(character))
            return FALSE;
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Utility_IsHexaString
 *
 * Description: verify if a string is a hexadecimal string
 * Input      : - ptr_string: pointer to the string to be verified
 * Output     : - FALSE: the string is not a hexadecimal string
 *              - TRUE : the string is     a hexadecimal string
 *=============================================================================*/
bool Utility_IsHexaString(const ascii *ptr_string)
{
    u8 character;


    if (( ptr_string) == NULL)
        return FALSE;

    if ((*ptr_string) == '\0')
        return FALSE;


    while ((character = *ptr_string++) != '\0')
    {
        if (!isxdigit(character))
            return FALSE;
    }


    return TRUE;
}




/*===========================================================================
 * Function   : Utility_IsPhoneString
 *
 * Description: verify if the string is a valid phone number
 *              (national or international format)
 * Input      : - ptr_string: pointer to the string to be checked
 * Output     : - FALSE: the string is not a valid phone number (national or international format)
 *              - TRUE : the string is     a valid phone number (national or international format)
 *===========================================================================*/
bool Utility_IsPhoneString(ascii *ptr_string)
{
    u8 character;


    if (( ptr_string) == NULL)
        return FALSE;

    if ((*ptr_string) == '\0')
        return FALSE;


    character = *ptr_string;
    if (character == '+')
        ptr_string++;


    while ((character = *ptr_string++) != '\0')
    {
        if (!isdigit(character))
            return FALSE;
    }


    return TRUE;
}




/*=============================================================================
 * Function   : Utility_IsFloat1String
 *
 * Description: check if the string is a float number
 *                  [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$
 * Input      : - ptr_string                 : pointer to the string to be checked
 *              - max_num_digits_decimal_part: max number of digits of decimal part
 * Output     : - FALSE: the string is not a float number
 *              - TRUE : the string is     a float number
 *=============================================================================*/
bool Utility_IsFloat1String(ascii *ptr_string, u8 max_num_digits_decimal_part)
{
    bool  seen_digit;
    bool  seen_dot;
    bool  seen_exp;
    bool  just_seen_exp;

    ascii c;
    u8    num_digits_decimal_part;
    u8    i;


    seen_digit    = FALSE;
    seen_dot      = FALSE;
    seen_exp      = FALSE;
    just_seen_exp = FALSE;

    num_digits_decimal_part = 0;


    for (i = 0; i < strlen(ptr_string); i++)
    {
        c = ptr_string[i];

        /* digit (0-9) */
        if ((c >= '0') && (c <= '9'))
        {
            seen_digit = TRUE;

            if ((seen_dot) && (!seen_exp))
                num_digits_decimal_part++;

            continue;
        }

        /* minus or plus (-,+) */
        if (((c == '-') || (c=='+'))  &&  ((i == 0) || just_seen_exp))
        {
            continue;
        }

        /* dot (.) */
        if ((c == '.') && (!seen_dot))
        {
            seen_dot = TRUE;
            continue;
        }

        /* exp (e,E) */
        just_seen_exp = FALSE;
        if (((c == 'e') || (c == 'E'))  &&  (!seen_exp))
        {
            seen_exp      = TRUE;
            just_seen_exp = TRUE;
            continue;
        }

        return FALSE;
    }


    if (!seen_digit)
        return FALSE;

    if (num_digits_decimal_part > max_num_digits_decimal_part)
        return FALSE;

    return TRUE;
}




/*=============================================================================
 * Function   : Utility_IsFloat2String
 *
 * Description: check if the string is a float number (comma allowed)
 *                  [-+]?[0-9]*\[.,]?[0-9]+([eE][-+]?[0-9]+)?$
 * Input      : - ptr_string                 : pointer to the string to be checked
 *              - max_num_digits_decimal_part: max number of digits of decimal part
 * Output     : - FALSE: the string is not a float number
 *              - TRUE : the string is     a float number
 *=============================================================================*/
bool Utility_IsFloat2String(ascii *ptr_string, u8 max_num_digits_decimal_part)
{
    bool  seen_digit;
    bool  seen_dot;
    bool  seen_comma;
    bool  seen_exp;
    bool  just_seen_exp;

    ascii c;
    u8    num_digits_decimal_part;
    u8    i;


    seen_digit    = FALSE;
    seen_dot      = FALSE;
    seen_comma    = FALSE;
    seen_exp      = FALSE;
    just_seen_exp = FALSE;

    num_digits_decimal_part = 0;


    for (i = 0; i < strlen(ptr_string); i++)
    {
        c = ptr_string[i];

        /* digit (0-9) */
        if ((c >= '0') && (c <= '9'))
        {
            seen_digit = TRUE;

            if (((seen_dot) || (seen_comma)) && (!seen_exp))
                num_digits_decimal_part++;

            continue;
        }

        /* minus or plus (-,+) */
        if (((c == '-') || (c=='+'))  &&  ((i == 0) || just_seen_exp))
        {
            continue;
        }

        /* dot (.) */
        if ((c == '.') && (!seen_dot)
        )
        {
            seen_dot = TRUE;
            continue;
        }

        /* comma (,) */
        if ((c == ',') && (!seen_comma))
        {
            seen_comma = TRUE;
            continue;
        }

        /* exp (e,E) */
        just_seen_exp = FALSE;
        if (((c == 'e') || (c == 'E'))  &&  (!seen_exp))
        {
            seen_exp      = TRUE;
            just_seen_exp = TRUE;
            continue;
        }

        return FALSE;
    }


    if (!seen_digit)
        return FALSE;

    if (num_digits_decimal_part > max_num_digits_decimal_part)
        return FALSE;

    return TRUE;
}




/*=============================================================================
 * Function   : Utility_CheckString
 *
 * Description: check if a string satisfies the specified requirements:
 *                  - string type
 *                  - string min length
 *                  - string max length
 * Input      : - ptr_string                 : pointer to the string to be checked
 *              - string_type                : string type       expected
 *              - string_quoted              : quoted string     expected
 *              - string_min_length          : min string length expected (=0   if there is no limit)
 *              - string_max_length          : max string length expected (=255 if there is no limit)
 *              - max_num_digits_decimal_part: max number of digits of decimal part (valid only for "floating point" string type)
 * Output     : - FALSE: string requirements not satisfied
 *              - TRUE : string requirements     satisfied
 *=============================================================================*/
bool Utility_CheckString(ascii *ptr_string, u8 string_type, bool string_quoted, u8 string_min_length, u8 string_max_length, u8 max_num_digits_decimal_part)
{
    static u8   string_copy[160 + 1];

           u8   len_string;
           u8   len_string_no_quoted;

           bool result;


    /* check the string length */
    len_string = (u8)strlen(ptr_string);
    if (
         (len_string < string_min_length) ||
         (len_string > string_max_length)
       )
    {
        return FALSE;
    }
    if (len_string > 160)
        return FALSE;



    /* check the quoted string */
    if (string_quoted)
    {
        if ((ptr_string[0] != '"') || (ptr_string[len_string - 1] != '"'))
            return FALSE;

        /* copy the string removing the 2 quote characters */
        strncpy((ascii *)string_copy, ptr_string + 1, 160);
        string_copy[160] = 0x00;
        string_copy[len_string - 2] = 0x00;

        len_string_no_quoted = len_string - 2;
    }
    else
    {
        /* copy the string */
        strncpy((ascii *)string_copy, ptr_string    , 160);
        string_copy[160] = 0x00;

        len_string_no_quoted = len_string;
    }


    /* check the string characters */
    switch (string_type)
    {
        /* string type: date (format dd/mm/yyyy) */
        case UTILITY__STRING__DATE_1:
            if (len_string_no_quoted == 10)
            {
                if (
                     (!isdigit(string_copy[0])      ) ||
                     (!isdigit(string_copy[1])      ) ||
                     (         string_copy[2] != '/') ||
                     (!isdigit(string_copy[3])      ) ||
                     (!isdigit(string_copy[4])      ) ||
                     (         string_copy[5] != '/') ||
                     (!isdigit(string_copy[6])      ) ||
                     (!isdigit(string_copy[7])      ) ||
                     (!isdigit(string_copy[8])      ) ||
                     (!isdigit(string_copy[9])      )
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: date (format dd.mm.yyyy) */
        case UTILITY__STRING__DATE_2:
            if (len_string_no_quoted == 10)
            {
                if (
                     (!isdigit(string_copy[0])      ) ||
                     (!isdigit(string_copy[1])      ) ||
                     (         string_copy[2] != '.') ||
                     (!isdigit(string_copy[3])      ) ||
                     (!isdigit(string_copy[4])      ) ||
                     (         string_copy[5] != '.') ||
                     (!isdigit(string_copy[6])      ) ||
                     (!isdigit(string_copy[7])      ) ||
                     (!isdigit(string_copy[8])      ) ||
                     (!isdigit(string_copy[9])      )
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: time (format hh:mm:ss) */
        case UTILITY__STRING__TIME_1:
            if (len_string_no_quoted == 8)
            {
                if (
                     (!isdigit(string_copy[0])      ) ||
                     (!isdigit(string_copy[1])      ) ||
                     (         string_copy[2] != ':') ||
                     (!isdigit(string_copy[3])      ) ||
                     (!isdigit(string_copy[4])      ) ||
                     (         string_copy[5] != ':') ||
                     (!isdigit(string_copy[6])      ) ||
                     (!isdigit(string_copy[7])      )
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: time (format hh.mm.ss) */
        case UTILITY__STRING__TIME_2:
            if (len_string_no_quoted == 8)
            {
                if (
                     (!isdigit(string_copy[0])      ) ||
                     (!isdigit(string_copy[1])      ) ||
                     (         string_copy[2] != '.') ||
                     (!isdigit(string_copy[3])      ) ||
                     (!isdigit(string_copy[4])      ) ||
                     (         string_copy[5] != '.') ||
                     (!isdigit(string_copy[6])      ) ||
                     (!isdigit(string_copy[7])      )
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: time (format hh:mm) */
        case UTILITY__STRING__TIME_3:
            if (len_string_no_quoted == 5)
            {
                if (
                     (!isdigit(string_copy[0])      ) ||
                     (!isdigit(string_copy[1])      ) ||
                     (         string_copy[2] != ':') ||
                     (!isdigit(string_copy[3])      ) ||
                     (!isdigit(string_copy[4])      )
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: time (format hh.mm) */
        case UTILITY__STRING__TIME_4:
            if (len_string_no_quoted == 5)
            {
                if (
                     (!isdigit(string_copy[0])      ) ||
                     (!isdigit(string_copy[1])      ) ||
                     (         string_copy[2] != '.') ||
                     (!isdigit(string_copy[3])      ) ||
                     (!isdigit(string_copy[4])      )
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: boolean */
        case UTILITY__STRING__BOOLEAN:
            if (len_string_no_quoted == 1)
            {
                if (
                     (string_copy[0] != 'T') &&
                     (string_copy[0] != 'F') &&
                     (string_copy[0] != 't') &&
                     (string_copy[0] != 'f')
                   )
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                return FALSE;
            }
            break;


        /* string type: phone number */
        case UTILITY__STRING__PHONE_NUMBER:
            if (len_string_no_quoted > 0)
            {
                result = Utility_IsPhoneString((ascii *)string_copy);
                return result;
            }
            else
            {
                if (string_min_length == 0)
                    return TRUE;
                else
                    return FALSE;
            }
            break;


        /* string type: text */
        case UTILITY__STRING__TEXT:
            return TRUE;


        /* string type: unsigned integer number */
        case UTILITY__STRING__UINT_NUMBER:
            if (len_string_no_quoted > 0)
            {
                result = Utility_IsNumString((ascii *)string_copy);
                return result;
            }
            else
            {
                if (string_min_length == 0)
                    return TRUE;
                else
                    return FALSE;
            }
            break;


        /* string type: signed integer number */
        case UTILITY__STRING__SINT_NUMBER:
            if (len_string_no_quoted > 0)
            {
                if (
                     (!isdigit(string_copy[0])) &&
                     (
                       (string_copy[0] != '-') &&
                       (string_copy[0] != '+')
                     )
                   )
                {
                    return FALSE;
                }

                if      (string_copy[0] == '-')
                    result = Utility_IsNumString((ascii *)(string_copy + 1));
                else if (string_copy[0] == '+')
                    result = Utility_IsNumString((ascii *)(string_copy + 1));
                else
                    result = Utility_IsNumString((ascii *)(string_copy    ));

                return result;
            }
            else
            {
                if (string_min_length == 0)
                    return TRUE;
                else
                    return FALSE;
            }
            break;


        /* string type: hex number */
        case UTILITY__STRING__HEX_NUMBER:
            if (len_string_no_quoted > 0)
            {
                result = Utility_IsHexaString((ascii *)string_copy);
                return result;
            }
            else
            {
                if (string_min_length == 0)
                    return TRUE;
                else
                    return FALSE;
            }
            break;


        /* string type: floating number */
        case UTILITY__STRING__FLOAT_1_NUMBER:
            if (len_string_no_quoted > 0)
            {
                result = Utility_IsFloat1String((ascii *)string_copy, max_num_digits_decimal_part);
                return result;
            }
            else
            {
                if (string_min_length == 0)
                    return TRUE;
                else
                    return FALSE;
            }
            return TRUE;


        /* string type: floating number (comma allowed) */
        case UTILITY__STRING__FLOAT_2_NUMBER:
            if (len_string_no_quoted > 0)
            {
                result = Utility_IsFloat2String((ascii *)string_copy, max_num_digits_decimal_part);
                return result;
            }
            else
            {
                if (string_min_length == 0)
                    return TRUE;
                else
                    return FALSE;
            }
            return TRUE;


        /* string type: unknown */
        default:
            return FALSE;
    }
}




/*===========================================================================
 * Function   : Utility_Strtok
 *
 * Description: "strtok" function
 * Input      : - ptr_string: pointer to the string
 *              - delimiter : token delimiter character
 * Output     : - pointer to next token found
 *              - NULL if there are no tokens left to retrieve
 *===========================================================================*/
ascii *Utility_Strtok(ascii *ptr_string, ascii delimiter)
{
    static ascii *ptr_start_string = NULL;
    static u8     len_string       = 0;
    static u8     num_char_parsed  = 0;

           ascii *ptr_start_token;
           ascii *ptr_char_string;


    /* check if it is the first function call */
    if (ptr_string != NULL)
    {
        /* first function call */
        ptr_start_string = ptr_string;
        len_string       = strlen(ptr_string);
        num_char_parsed  = 0;
    }


    /* check if all the string has been parsed */
    if (num_char_parsed < len_string)
    {
        /* not all the string has been parsed */

        /* find next token */
        ptr_start_token = ptr_start_string + num_char_parsed;
        ptr_char_string = ptr_start_string + num_char_parsed;

        while ((num_char_parsed < len_string) && ((*ptr_char_string) != delimiter))
        {
            ptr_char_string++;
            num_char_parsed++;
        }

        /* put the end of string for the token found */
        *ptr_char_string = 0x00;

        /* parse the delimiter character */
        ptr_char_string++;
        num_char_parsed++;

        return ptr_start_token;
    }
    else
    {
        /* all the string has been parsed */

        return NULL;
    }
}




/*===========================================================================
 * Function   : Utility_Strtok2
 *
 * Description:
 * Input      : - ptr_string   : pointer to the string
 *              - delimiters   : token delimiter characters
 *              - ptr_delimiter: pointer to save the delimiter found for this token
 * Output     : - pointer to next token found
 *              - NULL if there are no tokens left to retrieve
 *===========================================================================*/
ascii *Utility_Strtok2(ascii *ptr_string, const ascii *delimiters, ascii *ptr_delimiter)
{
    static ascii *ptr_start_string = NULL;
    static u8     len_string       = 0;
    static u8     num_char_parsed  = 0;

           ascii *ptr_start_token;
           ascii *ptr_char_string;

           u8     number_of_delimiters;

           bool   delimiter_found;
           u8     i;


    /* calculate the number of delimiters */
    number_of_delimiters = (u8)strlen(delimiters);
    if (number_of_delimiters == 0)
        return NULL;


    /* check if it is the first function call */
    if (ptr_string != NULL)
    {
        /* first function call */
        ptr_start_string = ptr_string;
        len_string       = strlen(ptr_string);
        num_char_parsed  = 0;
    }


    /* check if all the string has been parsed */
    if (num_char_parsed < len_string)
    {
        /* not all the string has been parsed */

        /* find next token */
        ptr_start_token = ptr_start_string + num_char_parsed;
        ptr_char_string = ptr_start_string + num_char_parsed;
        *ptr_delimiter  = 0x00;
        delimiter_found = FALSE;
        while ((num_char_parsed < len_string) && (!delimiter_found))
        {
            for (i = 0; i < number_of_delimiters; i++)
            {
                if ((*ptr_char_string) == delimiters[i])
                {
                    *ptr_delimiter  = delimiters[i];
                    delimiter_found = TRUE;
                    break;
                }
            }

            if (!delimiter_found)
            {
                ptr_char_string++;
                num_char_parsed++;
            }
        }

        /* put the end of string for the token found */
        *ptr_char_string = 0x00;

        /* parse the delimiter character */
        ptr_char_string++;
        num_char_parsed++;

        return ptr_start_token;
    }
    else
    {
        /* all the string has been parsed */

        *ptr_delimiter = 0x00;
        return NULL;
    }
}




/*=============================================================================
 * Function   : Utility_ReverseString
 *
 * Description: reverse a string
 * Input      : - ptr_string: pointer to the string
 * Output     : -
 *=============================================================================*/
void Utility_ReverseString(ascii *ptr_string)
{
    u8    len_string;
    ascii temp;

    u8    i;
    u8    j;


    len_string = (u8)strlen(ptr_string);
    if (len_string <= 1)
        return;

    for (i = 0, j = len_string - 1;  i < (len_string / 2); i++, j--)
    {
        temp          = ptr_string[i];
        ptr_string[i] = ptr_string[j];
        ptr_string[j] = temp;
    }
}




/*===========================================================================
 * Function   : Utility_Strcmp
 *
 * Description: compare 2 strings
 * Input      : - ptr_string_1: string 1 to be compared
 *              - ptr_string_2: string 2 to be compared
 * Output     : - FALSE: the 2 strings are not identic
 *              - TRUE : the 2 strings are     identic
 *===========================================================================*/
bool Utility_Strcmp(ascii *ptr_string_1, ascii *ptr_string_2)
{
    u8 len_string_1;
    u8 len_string_2;


    if (ptr_string_1 == NULL)
        return FALSE;
    if (ptr_string_2 == NULL)
        return FALSE;

    len_string_1 = (u8)strlen(ptr_string_1);
    len_string_2 = (u8)strlen(ptr_string_2);

    if (
         (len_string_1  == len_string_2) &&
         (strncmp(ptr_string_1, ptr_string_2, len_string_1) == 0)
       )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




/*===========================================================================
 * Function   : Utility_RandInit
 *
 * Description: generate a random 32 bit unsigned integer number
 *              The seed is calculated on:
 *                  - RTC time
 *                  - OS  time
 *                  - Phone IMEI
 *                  - ADC values (ADC0, ADC1, ADC2, ADC3)
 * Input      : -
 * Output     : - random u32 integer number
 *===========================================================================*/
static u32 Utility_RandInit(void)
{
    u32              seed;

    ascii            imei[PHONE__LEN_MAX_IMEI + 1];            /* phone IDs */

    ql_rtc_time_t    rtc_time = {0, 0, 0, 1, 1, 2000, 6};      /* RTC time      */
    s64              os_time_ms;                               /* OS  time (ms) */

    int              adc0_raw_value;                           /* ADC0 raw value */
    int              adc1_raw_value;                           /* ADC1 raw value */

    ql_errcode_adc_e res_adc;
    ql_errcode_rtc_e res_rtc;

    u8               i;


    /* get RTC time */
    res_rtc = ql_rtc_get_time(&rtc_time);
    if (res_rtc != QL_RTC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UTILITY, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtc_get_time - ERROR"  , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* get OS time (ms) */
    os_time_ms = ql_rtos_up_time_ms();

    /* get the phone IDs */
    memset(imei, 0x00, PHONE__LEN_MAX_IMEI + 1);
    (void)ql_dev_get_imei(imei, PHONE__LEN_MAX_IMEI, 0);

    /* read ADC0 (raw value) */
    res_adc = ql_adc_get_volt_raw(0, QL_ADC_SCALE_AUTO, &adc0_raw_value);
    if (res_adc != QL_ADC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UTILITY, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_adc_get_volt_raw - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* read ADC1 (raw value) */
    res_adc = ql_adc_get_volt_raw(1, QL_ADC_SCALE_AUTO, &adc1_raw_value);
    if (res_adc != QL_ADC_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_UTILITY, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_adc_get_volt_raw - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    /* initialize random seed */

    seed  = 123456;

    seed += (u32) rtc_time.tm_year;
    seed += (u32) rtc_time.tm_mon;
    seed += (u32) rtc_time.tm_mday;
    seed -= (u32) rtc_time.tm_hour;
    seed *= (u32)(rtc_time.tm_min + 1);
    seed *= (u32)(rtc_time.tm_sec + 1);
    seed += (u32) rtc_time.tm_wday;

    for (i = 0; i < PHONE__LEN_MAX_IMEI; i++)
        seed += (u32)(imei[i]);

    seed += (u32) adc0_raw_value;
    seed -= (u32) adc1_raw_value;

    seed += (u32) os_time_ms;


    return seed;
}




/*===========================================================================
 * Function   : Utility_RandomInteger
 *
 * Description: generate a random u32 integer number
 *              The seed is calculated on:
 *                  - RTC time
 *                  - OS  time
 *                  - Phone IMEI
 *                  - ADC values (ADC0, ADC1, ADC2, ADC3)
 * Input      : - range_start: range start for the random value
 *              - range_size : range size  for the random value
 * Output     : random u32 integer number
 *===========================================================================*/
u32 Utility_RandomInteger(u32 range_start, u32 range_size)
{
    u32 seed;
    u32 random_value;
    u32 random_value_in_range;


    /* check the range size value */
    if (range_start > 0)
    {
        if (range_size > (ULONG_MAX - range_start + 1))
            range_size = ULONG_MAX - range_start + 1;
    }


    /* initialize random seed */
    seed  = Utility_RandInit();


    /* generate a random value in the range [0 --- RAND_MAX] */
    random_value = rand_r((unsigned int *)&seed);

    /* generate a random value in the range [range_start --- (range_start + range_size - 1)] */
    random_value_in_range = range_start + (random_value % range_size);


    return random_value_in_range;
}




/*=============================================================================
 * Function   : Utility_RandomAlphanumericString
 *
 * Description: generate a random alphanumeric string
 * Input      : - string_len: length of the string to be generated (maximum 40)
 * Output     : -
 *=============================================================================*/
void Utility_RandomAlphanumericString(u8 string_len, ascii *random_string)
{
    const ascii alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";   // 26 + 26 + 10 = 62 characters

          u8    alphabet_index;
          u8    i;

          u32   seed;
          u32   random_value;


    /* check maximum string length */
    if (string_len > 40)
        string_len = 40;


    /* initialize random seed */
    seed  = Utility_RandInit();


    /* generate the character of the random string */
    for (i = 0; i < string_len; i++)
    {
        /* i-th character */

        /* generate a random value in the range [0 --- RAND_MAX] */
        random_value = rand_r((unsigned int *)&seed);

        /* generate a random value in the range [0 --- (62 - 1)] */
        alphabet_index = (u8)(random_value % 62);

        /* random index for the alphabet */
        if (alphabet_index > (62 - 1))
            alphabet_index = i;  // maximum value for "i" is (40-1) that is less than (62-1)

        /* extract the random character from the alphabet and save it */
        random_string[i] = alphabet[alphabet_index];
    }


    /* print the random string */
  //snprintf(Utility_DebugString, sizeof(Utility_DebugString), "Random alphanumeric string: \"%s\"", random_string);
  //Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, Utility_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* end of string */
    random_string[string_len] = 0x00;
}




/*=============================================================================
 * Function   : Utility_PrintData
 *
 * Description: print a block of data for debug
 * Input      : - trace_level: trace level
 *              - len_data   : length of the block of the data to be printed
 *              - ptr_data   :               block of the data to be printed
 * Output     : -
 *=============================================================================*/
void Utility_PrintData(u8 trace_level, u32 len_data, u8 *ptr_data)
{
    static ascii  temp_string[(2 * 128) + 1];

           u8    *ptr_data_temp;

           u32    n;
           u32    r;

           u8     byte;

           ascii *ptr_msb;
           ascii *ptr_lsb;

           u8     i;
           u8     j;


    ptr_data_temp = ptr_data;

    snprintf(Utility_DebugString, sizeof(Utility_DebugString), "DATA(---) len_data: %ld", len_data);
    Debug_SendDebugString1(trace_level, DEBUG_TRACE_TYPE_LOW, Utility_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 50);

    n = len_data / 128;
    r = len_data % 128;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < 128; j++)
        {
            byte = *(ptr_data_temp + j);

            ptr_msb = temp_string + ((2 * j)    );
            ptr_lsb = temp_string + ((2 * j) + 1);

            Utility_ByteToHex(byte, ptr_msb, ptr_lsb);
        }
        temp_string[(2 * 128)] = 0x00;

        snprintf(Utility_DebugString, sizeof(Utility_DebugString), "DATA(%03d): %s", i, temp_string);
        Debug_SendDebugString1(trace_level, DEBUG_TRACE_TYPE_LOW, Utility_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 50);

        ptr_data_temp += 128;
    }

    if (r > 0)
    {
        for (j = 0; j < r; j++)
        {
            byte = *(ptr_data_temp + j);

            ptr_msb = temp_string + ((2 * j)    );
            ptr_lsb = temp_string + ((2 * j) + 1);

            Utility_ByteToHex(byte, ptr_msb, ptr_lsb);
        }
        temp_string[(2 * r)] = 0x00;

        snprintf(Utility_DebugString, sizeof(Utility_DebugString), "DATA(%03d): %s", i, temp_string);
        Debug_SendDebugString1(trace_level, DEBUG_TRACE_TYPE_LOW, Utility_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 50);
    }
}
