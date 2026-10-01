/*=============================================================================
 * File       :  UTILITY.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - utilities
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __UTILITY_H__


#define __UTILITY_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
#define UTILITY__STRING__DATE_1          0    /* string type: date (format dd/mm/yyyy)                */
#define UTILITY__STRING__DATE_2          1    /* string type: date (format dd.mm.yyyy)                */
#define UTILITY__STRING__TIME_1          2    /* string type: time (format hh:mm:ss)                  */
#define UTILITY__STRING__TIME_2          3    /* string type: time (format hh.mm.ss)                  */
#define UTILITY__STRING__TIME_3          4    /* string type: time (format hh:mm   )                  */
#define UTILITY__STRING__TIME_4          5    /* string type: time (format hh.mm   )                  */
#define UTILITY__STRING__BOOLEAN         6    /* string type: boolean                                 */
#define UTILITY__STRING__PHONE_NUMBER    7    /* string type: phone            number                 */
#define UTILITY__STRING__TEXT            8    /* string type: text                                    */
#define UTILITY__STRING__UINT_NUMBER     9    /* string type: unsigned integer number                 */
#define UTILITY__STRING__SINT_NUMBER     10   /* string type: signed   integer number                 */
#define UTILITY__STRING__HEX_NUMBER      11   /* string type: hex              number                 */
#define UTILITY__STRING__FLOAT_1_NUMBER  12   /* string type: floating         number                 */
#define UTILITY__STRING__FLOAT_2_NUMBER  13   /* string type: floating         number (comma allowed) */




/*=============================================================================
 * DATA_TYPES
 *=============================================================================*/
/* data type */
typedef union
{
    u8    date;
    u8    time;
    ascii text;
} UTILITY__DATA_TYPE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * CRC, checksum
 *-----------------------------------------------------------------------------*/
/* CRC16 */
void   Utility_InitCRC16Table(void);
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
bool   Utility_CheckString   (ascii *ptr_string, u8 string_type, bool string_quoted, u8 string_min_lenght, u8 string_max_lenght, u8 num_digits_decimal_part);
ascii *Utility_Strtok        (ascii *ptr_string,       ascii  delimiter );
ascii *Utility_Strtok2       (ascii *ptr_string, const ascii *delimiters, ascii *ptr_delimiter);
bool   Utility_Strcmp        (ascii *ptr_string_1, ascii *ptr_string_2);


/*-----------------------------------------------------------------------------
 * Random
 *-----------------------------------------------------------------------------*/
u32    Utility_RandomInteger           (u32 range_start, u32 range_size);
void   Utility_RandomAlphanumericString(u8 string_len, ascii *random_string);


/*-----------------------------------------------------------------------------
 * Debug
 *-----------------------------------------------------------------------------*/
void   Utility_PrintData(u8 trace_level, u32 len_data, u8 *ptr_data);




#endif
