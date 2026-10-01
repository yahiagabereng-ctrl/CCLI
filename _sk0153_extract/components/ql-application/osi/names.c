/*=============================================================================
 * File       :  NAMES.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - names
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <string.h>

/* user     includes */
#include "debug_my.h"
#include "fw_config.h"
#include "names.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
//#define MAX_LENGTH_DEBUG_STRING   200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* debug string */
//static ascii                             Names_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/

/* configuration - name OUT1 */
static       NAMES__CONFIG__NAME_OUT1    Names_Config_NamesOut1;
static const NAMES__CONFIG__NAME_OUT1    Names_Config_NamesOut1Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name OUT2 */
static       NAMES__CONFIG__NAME_OUT2    Names_Config_NamesOut2;
static const NAMES__CONFIG__NAME_OUT2    Names_Config_NamesOut2Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name IN1 */
static       NAMES__CONFIG__NAME_IN1     Names_Config_NamesIn1;
static const NAMES__CONFIG__NAME_IN1     Names_Config_NamesIn1Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name IN2 */
static       NAMES__CONFIG__NAME_IN2     Names_Config_NamesIn2;
static const NAMES__CONFIG__NAME_IN2     Names_Config_NamesIn2Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name IN3 */
static       NAMES__CONFIG__NAME_IN3     Names_Config_NamesIn3;
static const NAMES__CONFIG__NAME_IN3     Names_Config_NamesIn3Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name IN4 */
static       NAMES__CONFIG__NAME_IN4     Names_Config_NamesIn4;
static const NAMES__CONFIG__NAME_IN4     Names_Config_NamesIn4Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name TMIN */
static       NAMES__CONFIG__NAME_TMIN    Names_Config_NamesTmin;
static const NAMES__CONFIG__NAME_TMIN    Names_Config_NamesTminDefault =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name TMAX */
static       NAMES__CONFIG__NAME_TMAX    Names_Config_NamesTmax;
static const NAMES__CONFIG__NAME_TMAX    Names_Config_NamesTmaxDefault =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name TMINEXT */
static       NAMES__CONFIG__NAME_TMINEXT Names_Config_NamesTminExt;
static const NAMES__CONFIG__NAME_TMINEXT Names_Config_NamesTminExtDefault =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name TMAXEXT */
static       NAMES__CONFIG__NAME_TMAXEXT Names_Config_NamesTmaxExt;
static const NAMES__CONFIG__NAME_TMAXEXT Names_Config_NamesTmaxExtDefault =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name C1 */
static       NAMES__CONFIG__NAME_C1      Names_Config_NamesC1;
static const NAMES__CONFIG__NAME_C1      Names_Config_NamesC1Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};


/* configuration - name C2 */
static       NAMES__CONFIG__NAME_C2      Names_Config_NamesC2;
static const NAMES__CONFIG__NAME_C2      Names_Config_NamesC2Default =
{
 //  1     2     3     4     5       6     7     8     9     10
    {0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   0x00},
};




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




/*===========================================================================
 * Function   : Names_Config_NamesOut1_GetDefault
 *
 * Description: get the default "name OUT1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesOut1_GetDefault(NAMES__CONFIG__NAME_OUT1 *ptr_data)
{
    *ptr_data = Names_Config_NamesOut1Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut1_Get
 *
 * Description: get the "name OUT1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesOut1_Get(NAMES__CONFIG__NAME_OUT1 *ptr_data)
{
    *ptr_data = Names_Config_NamesOut1;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut1_Set
 *
 * Description: set the "name OUT1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesOut1_Set(NAMES__CONFIG__NAME_OUT1 *ptr_data)
{
    Names_Config_NamesOut1 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut1_IsValid
 *
 * Description: check if the "name OUT1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesOut1_IsValid(NAMES__CONFIG__NAME_OUT1 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_out1) > NAMES__LEN_MAX_NAME__OUT1)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__OUT1; i++)
    {
        if (ptr_data->name_out1[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut2_GetDefault
 *
 * Description: get the default "name OUT2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesOut2_GetDefault(NAMES__CONFIG__NAME_OUT2 *ptr_data)
{
    *ptr_data = Names_Config_NamesOut2Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut2_Get
 *
 * Description: get the "name OUT2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesOut2_Get(NAMES__CONFIG__NAME_OUT2 *ptr_data)
{
    *ptr_data = Names_Config_NamesOut2;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut2_Set
 *
 * Description: set the "name OUT2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesOut2_Set(NAMES__CONFIG__NAME_OUT2 *ptr_data)
{
    Names_Config_NamesOut2 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesOut2_IsValid
 *
 * Description: check if the "name OUT2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesOut2_IsValid(NAMES__CONFIG__NAME_OUT2 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_out2) > NAMES__LEN_MAX_NAME__OUT2)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__OUT2; i++)
    {
        if (ptr_data->name_out2[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn1_GetDefault
 *
 * Description: get the default "name IN1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn1_GetDefault(NAMES__CONFIG__NAME_IN1 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn1Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn1_Get
 *
 * Description: get the "name IN1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn1_Get(NAMES__CONFIG__NAME_IN1 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn1;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn1_Set
 *
 * Description: set the "name IN1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn1_Set(NAMES__CONFIG__NAME_IN1 *ptr_data)
{
    Names_Config_NamesIn1 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn1_IsValid
 *
 * Description: check if the "name IN1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesIn1_IsValid(NAMES__CONFIG__NAME_IN1 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_in1) > NAMES__LEN_MAX_NAME__IN1)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__IN1; i++)
    {
        if (ptr_data->name_in1[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn2_GetDefault
 *
 * Description: get the default "name IN2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn2_GetDefault(NAMES__CONFIG__NAME_IN2 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn2Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn2_Get
 *
 * Description: get the "name IN2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn2_Get(NAMES__CONFIG__NAME_IN2 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn2;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn2_Set
 *
 * Description: set the "name IN2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn2_Set(NAMES__CONFIG__NAME_IN2 *ptr_data)
{
    Names_Config_NamesIn2 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn2_IsValid
 *
 * Description: check if the "name IN2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesIn2_IsValid(NAMES__CONFIG__NAME_IN2 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_in2) > NAMES__LEN_MAX_NAME__IN2)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__IN2; i++)
    {
        if (ptr_data->name_in2[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn3_GetDefault
 *
 * Description: get the default "name IN3" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn3_GetDefault(NAMES__CONFIG__NAME_IN3 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn3Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn3_Get
 *
 * Description: get the "name IN3" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn3_Get(NAMES__CONFIG__NAME_IN3 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn3;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn3_Set
 *
 * Description: set the "name IN3" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn3_Set(NAMES__CONFIG__NAME_IN3 *ptr_data)
{
    Names_Config_NamesIn3 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn3_IsValid
 *
 * Description: check if the "name IN3" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesIn3_IsValid(NAMES__CONFIG__NAME_IN3 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_in3) > NAMES__LEN_MAX_NAME__IN3)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__IN3; i++)
    {
        if (ptr_data->name_in3[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn4_GetDefault
 *
 * Description: get the default "name IN4" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn4_GetDefault(NAMES__CONFIG__NAME_IN4 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn4Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn4_Get
 *
 * Description: get the "name IN4" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn4_Get(NAMES__CONFIG__NAME_IN4 *ptr_data)
{
    *ptr_data = Names_Config_NamesIn4;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn4_Set
 *
 * Description: set the "name IN4" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesIn4_Set(NAMES__CONFIG__NAME_IN4 *ptr_data)
{
    Names_Config_NamesIn4 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesIn4_IsValid
 *
 * Description: check if the "name IN4" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesIn4_IsValid(NAMES__CONFIG__NAME_IN4 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_in4) > NAMES__LEN_MAX_NAME__IN4)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__IN4; i++)
    {
        if (ptr_data->name_in4[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmin_GetDefault
 *
 * Description: get the default "name TMIN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmin_GetDefault(NAMES__CONFIG__NAME_TMIN *ptr_data)
{
    *ptr_data = Names_Config_NamesTminDefault;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmin_Get
 *
 * Description: get the "name TMIN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmin_Get(NAMES__CONFIG__NAME_TMIN *ptr_data)
{
    *ptr_data = Names_Config_NamesTmin;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmin_Set
 *
 * Description: set the "name TMIN" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmin_Set(NAMES__CONFIG__NAME_TMIN *ptr_data)
{
    Names_Config_NamesTmin = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmin_IsValid
 *
 * Description: check if the "name TMIN" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesTmin_IsValid(NAMES__CONFIG__NAME_TMIN *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_tmin) > NAMES__LEN_MAX_NAME__TMIN)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__TMIN; i++)
    {
        if (ptr_data->name_tmin[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmax_GetDefault
 *
 * Description: get the default "name TMAX" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmax_GetDefault(NAMES__CONFIG__NAME_TMAX *ptr_data)
{
    *ptr_data = Names_Config_NamesTmaxDefault;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmax_Get
 *
 * Description: get the "name TMAX" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmax_Get(NAMES__CONFIG__NAME_TMAX *ptr_data)
{
    *ptr_data = Names_Config_NamesTmax;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmax_Set
 *
 * Description: set the "name TMAX" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmax_Set(NAMES__CONFIG__NAME_TMAX *ptr_data)
{
    Names_Config_NamesTmax = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmax_IsValid
 *
 * Description: check if the "name TMAX" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesTmax_IsValid(NAMES__CONFIG__NAME_TMAX *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_tmax) > NAMES__LEN_MAX_NAME__TMAX)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__TMAX; i++)
    {
        if (ptr_data->name_tmax[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesTminExt_GetDefault
 *
 * Description: get the default "name TMINEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTminExt_GetDefault(NAMES__CONFIG__NAME_TMINEXT *ptr_data)
{
    *ptr_data = Names_Config_NamesTminExtDefault;
}




/*===========================================================================
 * Function   : Names_Config_NamesTminExt_Get
 *
 * Description: get the "name TMINEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTminExt_Get(NAMES__CONFIG__NAME_TMINEXT *ptr_data)
{
    *ptr_data = Names_Config_NamesTminExt;
}




/*===========================================================================
 * Function   : Names_Config_NamesTminExt_Set
 *
 * Description: set the "name TMINEXT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTminExt_Set(NAMES__CONFIG__NAME_TMINEXT *ptr_data)
{
    Names_Config_NamesTminExt = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesTminExt_IsValid
 *
 * Description: check if the "name TMINEXT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesTminExt_IsValid(NAMES__CONFIG__NAME_TMINEXT *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_tminext) > NAMES__LEN_MAX_NAME__TMINEXT)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__TMINEXT; i++)
    {
        if (ptr_data->name_tminext[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmaxExt_GetDefault
 *
 * Description: get the default "name TMAXEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmaxExt_GetDefault(NAMES__CONFIG__NAME_TMAXEXT *ptr_data)
{
    *ptr_data = Names_Config_NamesTmaxExtDefault;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmaxExt_Get
 *
 * Description: get the "name TMAXEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmaxExt_Get(NAMES__CONFIG__NAME_TMAXEXT *ptr_data)
{
    *ptr_data = Names_Config_NamesTmaxExt;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmaxExt_Set
 *
 * Description: set the "name TMAXEXT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesTmaxExt_Set(NAMES__CONFIG__NAME_TMAXEXT *ptr_data)
{
    Names_Config_NamesTmaxExt = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesTmaxExt_IsValid
 *
 * Description: check if the "name TMAXEXT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesTmaxExt_IsValid(NAMES__CONFIG__NAME_TMAXEXT *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_tmaxext) > NAMES__LEN_MAX_NAME__TMAXEXT)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__TMAXEXT; i++)
    {
        if (ptr_data->name_tmaxext[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesC1_GetDefault
 *
 * Description: get the default "name C1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesC1_GetDefault(NAMES__CONFIG__NAME_C1 *ptr_data)
{
    *ptr_data = Names_Config_NamesC1Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesC1_Get
 *
 * Description: get the "name C1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesC1_Get(NAMES__CONFIG__NAME_C1 *ptr_data)
{
    *ptr_data = Names_Config_NamesC1;
}




/*===========================================================================
 * Function   : Names_Config_NamesC1_Set
 *
 * Description: set the "name C1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesC1_Set(NAMES__CONFIG__NAME_C1 *ptr_data)
{
    Names_Config_NamesC1 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesC1_IsValid
 *
 * Description: check if the "name C1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesC1_IsValid(NAMES__CONFIG__NAME_C1 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_c1) > NAMES__LEN_MAX_NAME__C1)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__C1; i++)
    {
        if (ptr_data->name_c1[i] == ' ')
            return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : Names_Config_NamesC2_GetDefault
 *
 * Description: get the default "name C2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesC2_GetDefault(NAMES__CONFIG__NAME_C2 *ptr_data)
{
    *ptr_data = Names_Config_NamesC2Default;
}




/*===========================================================================
 * Function   : Names_Config_NamesC2_Get
 *
 * Description: get the "name C2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesC2_Get(NAMES__CONFIG__NAME_C2 *ptr_data)
{
    *ptr_data = Names_Config_NamesC2;
}




/*===========================================================================
 * Function   : Names_Config_NamesC2_Set
 *
 * Description: set the "name C2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Names_Config_NamesC2_Set(NAMES__CONFIG__NAME_C2 *ptr_data)
{
    Names_Config_NamesC2 = *ptr_data;
}




/*===========================================================================
 * Function   : Names_Config_NamesC2_IsValid
 *
 * Description: check if the "name C2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Names_Config_NamesC2_IsValid(NAMES__CONFIG__NAME_C2 *ptr_data)
{
    u8 i;


    if (strlen(ptr_data->name_c2) > NAMES__LEN_MAX_NAME__C2)
        return FALSE;

    for (i = 0; i < NAMES__LEN_MAX_NAME__C2; i++)
    {
        if (ptr_data->name_c2[i] == ' ')
            return FALSE;
    }

    return TRUE;
}
