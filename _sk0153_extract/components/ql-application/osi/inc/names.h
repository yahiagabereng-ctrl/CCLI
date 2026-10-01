/*=============================================================================
 * File       :  NAMES.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - names
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __NAMES_H__


#define __NAMES_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* maximum name string lengths */
#define NAMES__LEN_MAX_NAME__OUT1     10     /* OUT1    */
#define NAMES__LEN_MAX_NAME__OUT2     10     /* OUT2    */
#define NAMES__LEN_MAX_NAME__IN1      10     /* IN1     */
#define NAMES__LEN_MAX_NAME__IN2      10     /* IN2     */
#define NAMES__LEN_MAX_NAME__IN3      10     /* IN3     */
#define NAMES__LEN_MAX_NAME__IN4      10     /* IN4     */
#define NAMES__LEN_MAX_NAME__TMIN     10     /* TMIN    */
#define NAMES__LEN_MAX_NAME__TMAX     10     /* TMAX    */
#define NAMES__LEN_MAX_NAME__TMINEXT  10     /* TMINEXT */
#define NAMES__LEN_MAX_NAME__TMAXEXT  10     /* TMAXEXT */
#define NAMES__LEN_MAX_NAME__C1       10     /* C1      */
#define NAMES__LEN_MAX_NAME__C2       10     /* C2      */




/*=============================================================================
 * DATA TYPE
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - name OUT1 */
typedef struct
{
    ascii name_out1   [NAMES__LEN_MAX_NAME__OUT1    + 1];
} NAMES__CONFIG__NAME_OUT1;


/* configuration - name OUT2 */
typedef struct
{
    ascii name_out2   [NAMES__LEN_MAX_NAME__OUT2    + 1];
} NAMES__CONFIG__NAME_OUT2;


/* configuration - name IN1 */
typedef struct
{
    ascii name_in1    [NAMES__LEN_MAX_NAME__IN1     + 1];
} NAMES__CONFIG__NAME_IN1;


/* configuration - name IN2 */
typedef struct
{
    ascii name_in2    [NAMES__LEN_MAX_NAME__IN2     + 1];
} NAMES__CONFIG__NAME_IN2;


/* configuration - name IN3 */
typedef struct
{
    ascii name_in3    [NAMES__LEN_MAX_NAME__IN3     + 1];
} NAMES__CONFIG__NAME_IN3;


/* configuration - name IN4 */
typedef struct
{
    ascii name_in4    [NAMES__LEN_MAX_NAME__IN4     + 1];
} NAMES__CONFIG__NAME_IN4;


/* configuration - name TMIN */
typedef struct
{
    ascii name_tmin   [NAMES__LEN_MAX_NAME__TMIN    + 1];
} NAMES__CONFIG__NAME_TMIN;


/* configuration - name TMAX */
typedef struct
{
    ascii name_tmax   [NAMES__LEN_MAX_NAME__TMAX    + 1];
} NAMES__CONFIG__NAME_TMAX;

/* configuration - name TMINEXT */
typedef struct
{
    ascii name_tminext[NAMES__LEN_MAX_NAME__TMINEXT + 1];
} NAMES__CONFIG__NAME_TMINEXT;


/* configuration - name TMAXEXT */
typedef struct
{
    ascii name_tmaxext[NAMES__LEN_MAX_NAME__TMAXEXT + 1];
} NAMES__CONFIG__NAME_TMAXEXT;


/* configuration - name C1 */
typedef struct
{
    ascii name_c1     [NAMES__LEN_MAX_NAME__C1      + 1];
} NAMES__CONFIG__NAME_C1;


/* configuration - name C2 */
typedef struct
{
    ascii name_c2     [NAMES__LEN_MAX_NAME__C2      + 1];
} NAMES__CONFIG__NAME_C2;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "name OUT1"    configuration */
void Names_Config_NamesOut1_GetDefault   (NAMES__CONFIG__NAME_OUT1    *ptr_data);
void Names_Config_NamesOut1_Get          (NAMES__CONFIG__NAME_OUT1    *ptr_data);
void Names_Config_NamesOut1_Set          (NAMES__CONFIG__NAME_OUT1    *ptr_data);
bool Names_Config_NamesOut1_IsValid      (NAMES__CONFIG__NAME_OUT1    *ptr_data);

/* get/set "name OUT2"    configuration */
void Names_Config_NamesOut2_GetDefault   (NAMES__CONFIG__NAME_OUT2    *ptr_data);
void Names_Config_NamesOut2_Get          (NAMES__CONFIG__NAME_OUT2    *ptr_data);
void Names_Config_NamesOut2_Set          (NAMES__CONFIG__NAME_OUT2    *ptr_data);
bool Names_Config_NamesOut2_IsValid      (NAMES__CONFIG__NAME_OUT2    *ptr_data);

/* get/set "name IN1"     configuration */
void Names_Config_NamesIn1_GetDefault    (NAMES__CONFIG__NAME_IN1     *ptr_data);
void Names_Config_NamesIn1_Get           (NAMES__CONFIG__NAME_IN1     *ptr_data);
void Names_Config_NamesIn1_Set           (NAMES__CONFIG__NAME_IN1     *ptr_data);
bool Names_Config_NamesIn1_IsValid       (NAMES__CONFIG__NAME_IN1     *ptr_data);

/* get/set "name IN2"     configuration */
void Names_Config_NamesIn2_GetDefault    (NAMES__CONFIG__NAME_IN2     *ptr_data);
void Names_Config_NamesIn2_Get           (NAMES__CONFIG__NAME_IN2     *ptr_data);
void Names_Config_NamesIn2_Set           (NAMES__CONFIG__NAME_IN2     *ptr_data);
bool Names_Config_NamesIn2_IsValid       (NAMES__CONFIG__NAME_IN2     *ptr_data);

/* get/set "name IN3"     configuration */
void Names_Config_NamesIn3_GetDefault    (NAMES__CONFIG__NAME_IN3     *ptr_data);
void Names_Config_NamesIn3_Get           (NAMES__CONFIG__NAME_IN3     *ptr_data);
void Names_Config_NamesIn3_Set           (NAMES__CONFIG__NAME_IN3     *ptr_data);
bool Names_Config_NamesIn3_IsValid       (NAMES__CONFIG__NAME_IN3     *ptr_data);

/* get/set "name IN4"     configuration */
void Names_Config_NamesIn4_GetDefault    (NAMES__CONFIG__NAME_IN4     *ptr_data);
void Names_Config_NamesIn4_Get           (NAMES__CONFIG__NAME_IN4     *ptr_data);
void Names_Config_NamesIn4_Set           (NAMES__CONFIG__NAME_IN4     *ptr_data);
bool Names_Config_NamesIn4_IsValid       (NAMES__CONFIG__NAME_IN4     *ptr_data);

/* get/set "name TMIN"    configuration */
void Names_Config_NamesTmin_GetDefault   (NAMES__CONFIG__NAME_TMIN    *ptr_data);
void Names_Config_NamesTmin_Get          (NAMES__CONFIG__NAME_TMIN    *ptr_data);
void Names_Config_NamesTmin_Set          (NAMES__CONFIG__NAME_TMIN    *ptr_data);
bool Names_Config_NamesTmin_IsValid      (NAMES__CONFIG__NAME_TMIN    *ptr_data);

/* get/set "name TMAX"    configuration */
void Names_Config_NamesTmax_GetDefault   (NAMES__CONFIG__NAME_TMAX    *ptr_data);
void Names_Config_NamesTmax_Get          (NAMES__CONFIG__NAME_TMAX    *ptr_data);
void Names_Config_NamesTmax_Set          (NAMES__CONFIG__NAME_TMAX    *ptr_data);
bool Names_Config_NamesTmax_IsValid      (NAMES__CONFIG__NAME_TMAX    *ptr_data);

/* get/set "name TMINEXT" configuration */
void Names_Config_NamesTminExt_GetDefault(NAMES__CONFIG__NAME_TMINEXT *ptr_data);
void Names_Config_NamesTminExt_Get       (NAMES__CONFIG__NAME_TMINEXT *ptr_data);
void Names_Config_NamesTminExt_Set       (NAMES__CONFIG__NAME_TMINEXT *ptr_data);
bool Names_Config_NamesTminExt_IsValid   (NAMES__CONFIG__NAME_TMINEXT *ptr_data);

/* get/set "name TMAXEXT" configuration */
void Names_Config_NamesTmaxExt_GetDefault(NAMES__CONFIG__NAME_TMAXEXT *ptr_data);
void Names_Config_NamesTmaxExt_Get       (NAMES__CONFIG__NAME_TMAXEXT *ptr_data);
void Names_Config_NamesTmaxExt_Set       (NAMES__CONFIG__NAME_TMAXEXT *ptr_data);
bool Names_Config_NamesTmaxExt_IsValid   (NAMES__CONFIG__NAME_TMAXEXT *ptr_data);

/* get/set "name C1"      configuration */
void Names_Config_NamesC1_GetDefault     (NAMES__CONFIG__NAME_C1      *ptr_data);
void Names_Config_NamesC1_Get            (NAMES__CONFIG__NAME_C1      *ptr_data);
void Names_Config_NamesC1_Set            (NAMES__CONFIG__NAME_C1      *ptr_data);
bool Names_Config_NamesC1_IsValid        (NAMES__CONFIG__NAME_C1      *ptr_data);

/* get/set "name C2"      configuration */
void Names_Config_NamesC2_GetDefault     (NAMES__CONFIG__NAME_C2      *ptr_data);
void Names_Config_NamesC2_Get            (NAMES__CONFIG__NAME_C2      *ptr_data);
void Names_Config_NamesC2_Set            (NAMES__CONFIG__NAME_C2      *ptr_data);
bool Names_Config_NamesC2_IsValid        (NAMES__CONFIG__NAME_C2      *ptr_data);




#endif
