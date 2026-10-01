/*=============================================================================
 * File       :  MESSAGES
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - messages
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
#include "messages.h"
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
//static ascii                                     Messages_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/

/* configuration - message IN1 */
static       MESSAGES__CONFIG__MESSAGE_IN1       Messages_Config_MessagesIn1;
static const MESSAGES__CONFIG__MESSAGE_IN1       Messages_Config_MessagesIn1Default =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN2 */
static       MESSAGES__CONFIG__MESSAGE_IN2       Messages_Config_MessagesIn2;
static const MESSAGES__CONFIG__MESSAGE_IN2       Messages_Config_MessagesIn2Default =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN3 */
static       MESSAGES__CONFIG__MESSAGE_IN3       Messages_Config_MessagesIn3;
static const MESSAGES__CONFIG__MESSAGE_IN3       Messages_Config_MessagesIn3Default =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN4 */
static       MESSAGES__CONFIG__MESSAGE_IN4       Messages_Config_MessagesIn4;
static const MESSAGES__CONFIG__MESSAGE_IN4       Messages_Config_MessagesIn4Default =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message TMIN */
static       MESSAGES__CONFIG__MESSAGE_TMIN      Messages_Config_MessagesTmin;
static const MESSAGES__CONFIG__MESSAGE_TMIN      Messages_Config_MessagesTminDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message TMAX */
static       MESSAGES__CONFIG__MESSAGE_TMAX      Messages_Config_MessagesTmax;
static const MESSAGES__CONFIG__MESSAGE_TMAX      Messages_Config_MessagesTmaxDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message TMINEXT */
static       MESSAGES__CONFIG__MESSAGE_TMINEXT   Messages_Config_MessagesTminExt;
static const MESSAGES__CONFIG__MESSAGE_TMINEXT   Messages_Config_MessagesTminExtDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - name TMAXEXT */
static       MESSAGES__CONFIG__MESSAGE_TMAXEXT   Messages_Config_MessagesTmaxExt;
static const MESSAGES__CONFIG__MESSAGE_TMAXEXT   Messages_Config_MessagesTmaxExtDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN1 R */
static       MESSAGES__CONFIG__MESSAGE_IN1_R     Messages_Config_MessagesIn1R;
static const MESSAGES__CONFIG__MESSAGE_IN1_R     Messages_Config_MessagesIn1RDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN2 R */
static       MESSAGES__CONFIG__MESSAGE_IN2_R     Messages_Config_MessagesIn2R;
static const MESSAGES__CONFIG__MESSAGE_IN2_R     Messages_Config_MessagesIn2RDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN3 R */
static       MESSAGES__CONFIG__MESSAGE_IN3_R     Messages_Config_MessagesIn3R;
static const MESSAGES__CONFIG__MESSAGE_IN3_R     Messages_Config_MessagesIn3RDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message IN4 R */
static       MESSAGES__CONFIG__MESSAGE_IN4_R     Messages_Config_MessagesIn4R;
static const MESSAGES__CONFIG__MESSAGE_IN4_R     Messages_Config_MessagesIn4RDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message TMIN R */
static       MESSAGES__CONFIG__MESSAGE_TMIN_R    Messages_Config_MessagesTminR;
static const MESSAGES__CONFIG__MESSAGE_TMIN_R    Messages_Config_MessagesTminRDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message TMAX R */
static       MESSAGES__CONFIG__MESSAGE_TMAX_R    Messages_Config_MessagesTmaxR;
static const MESSAGES__CONFIG__MESSAGE_TMAX_R    Messages_Config_MessagesTmaxRDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - message TMINEXT R */
static       MESSAGES__CONFIG__MESSAGE_TMINEXT_R Messages_Config_MessagesTminExtR;
static const MESSAGES__CONFIG__MESSAGE_TMINEXT_R Messages_Config_MessagesTminExtRDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};


/* configuration - name TMAXEXT R */
static       MESSAGES__CONFIG__MESSAGE_TMAXEXT_R Messages_Config_MessagesTmaxExtR;
static const MESSAGES__CONFIG__MESSAGE_TMAXEXT_R Messages_Config_MessagesTmaxExtRDefault =
{
    //  1     2     3     4     5       6     7     8     9     10
    {
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 10
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 20
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 30
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 40
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 50
        0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00,   // 60
        0x00
    }
};




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * get/set configuration
 *-----------------------------------------------------------------------------*/
/* get/set "message IN1"       configuration */
void Messages_Config_MessagesIn1_GetDefault     (MESSAGES__CONFIG__MESSAGE_IN1       *ptr_data);
void Messages_Config_MessagesIn1_Get            (MESSAGES__CONFIG__MESSAGE_IN1       *ptr_data);
void Messages_Config_MessagesIn1_Set            (MESSAGES__CONFIG__MESSAGE_IN1       *ptr_data);
bool Messages_Config_MessagesIn1_IsValid        (MESSAGES__CONFIG__MESSAGE_IN1       *ptr_data);

/* get/set "message IN2"       configuration */
void Messages_Config_MessagesIn2_GetDefault     (MESSAGES__CONFIG__MESSAGE_IN2       *ptr_data);
void Messages_Config_MessagesIn2_Get            (MESSAGES__CONFIG__MESSAGE_IN2       *ptr_data);
void Messages_Config_MessagesIn2_Set            (MESSAGES__CONFIG__MESSAGE_IN2       *ptr_data);
bool Messages_Config_MessagesIn2_IsValid        (MESSAGES__CONFIG__MESSAGE_IN2       *ptr_data);

/* get/set "message IN3"       configuration */
void Messages_Config_MessagesIn3_GetDefault     (MESSAGES__CONFIG__MESSAGE_IN3       *ptr_data);
void Messages_Config_MessagesIn3_Get            (MESSAGES__CONFIG__MESSAGE_IN3       *ptr_data);
void Messages_Config_MessagesIn3_Set            (MESSAGES__CONFIG__MESSAGE_IN3       *ptr_data);
bool Messages_Config_MessagesIn3_IsValid        (MESSAGES__CONFIG__MESSAGE_IN3       *ptr_data);

/* get/set "message IN4"       configuration */
void Messages_Config_MessagesIn4_GetDefault     (MESSAGES__CONFIG__MESSAGE_IN4       *ptr_data);
void Messages_Config_MessagesIn4_Get            (MESSAGES__CONFIG__MESSAGE_IN4       *ptr_data);
void Messages_Config_MessagesIn4_Set            (MESSAGES__CONFIG__MESSAGE_IN4       *ptr_data);
bool Messages_Config_MessagesIn4_IsValid        (MESSAGES__CONFIG__MESSAGE_IN4       *ptr_data);

/* get/set "message TMIN"      configuration */
void Messages_Config_MessagesTmin_GetDefault    (MESSAGES__CONFIG__MESSAGE_TMIN      *ptr_data);
void Messages_Config_MessagesTmin_Get           (MESSAGES__CONFIG__MESSAGE_TMIN      *ptr_data);
void Messages_Config_MessagesTmin_Set           (MESSAGES__CONFIG__MESSAGE_TMIN      *ptr_data);
bool Messages_Config_MessagesTmin_IsValid       (MESSAGES__CONFIG__MESSAGE_TMIN      *ptr_data);

/* get/set "message TMAX"      configuration */
void Messages_Config_MessagesTmax_GetDefault    (MESSAGES__CONFIG__MESSAGE_TMAX      *ptr_data);
void Messages_Config_MessagesTmax_Get           (MESSAGES__CONFIG__MESSAGE_TMAX      *ptr_data);
void Messages_Config_MessagesTmax_Set           (MESSAGES__CONFIG__MESSAGE_TMAX      *ptr_data);
bool Messages_Config_MessagesTmax_IsValid       (MESSAGES__CONFIG__MESSAGE_TMAX      *ptr_data);

/* get/set "message TMINEXT"   configuration */
void Messages_Config_MessagesTminExt_GetDefault (MESSAGES__CONFIG__MESSAGE_TMINEXT   *ptr_data);
void Messages_Config_MessagesTminExt_Get        (MESSAGES__CONFIG__MESSAGE_TMINEXT   *ptr_data);
void Messages_Config_MessagesTminExt_Set        (MESSAGES__CONFIG__MESSAGE_TMINEXT   *ptr_data);
bool Messages_Config_MessagesTminExt_IsValid    (MESSAGES__CONFIG__MESSAGE_TMINEXT   *ptr_data);

/* get/set "message TMAXEXT"   configuration */
void Messages_Config_MessagesTmaxExt_GetDefault (MESSAGES__CONFIG__MESSAGE_TMAXEXT   *ptr_data);
void Messages_Config_MessagesTmaxExt_Get        (MESSAGES__CONFIG__MESSAGE_TMAXEXT   *ptr_data);
void Messages_Config_MessagesTmaxExt_Set        (MESSAGES__CONFIG__MESSAGE_TMAXEXT   *ptr_data);
bool Messages_Config_MessagesTmaxExt_IsValid    (MESSAGES__CONFIG__MESSAGE_TMAXEXT   *ptr_data);


/* get/set "message IN1 R"     configuration */
void Messages_Config_MessagesIn1R_GetDefault    (MESSAGES__CONFIG__MESSAGE_IN1_R     *ptr_data);
void Messages_Config_MessagesIn1R_Get           (MESSAGES__CONFIG__MESSAGE_IN1_R     *ptr_data);
void Messages_Config_MessagesIn1R_Set           (MESSAGES__CONFIG__MESSAGE_IN1_R     *ptr_data);
bool Messages_Config_MessagesIn1R_IsValid       (MESSAGES__CONFIG__MESSAGE_IN1_R     *ptr_data);

/* get/set "message IN2 R"     configuration */
void Messages_Config_MessagesIn2R_GetDefault    (MESSAGES__CONFIG__MESSAGE_IN2_R     *ptr_data);
void Messages_Config_MessagesIn2R_Get           (MESSAGES__CONFIG__MESSAGE_IN2_R     *ptr_data);
void Messages_Config_MessagesIn2R_Set           (MESSAGES__CONFIG__MESSAGE_IN2_R     *ptr_data);
bool Messages_Config_MessagesIn2R_IsValid       (MESSAGES__CONFIG__MESSAGE_IN2_R     *ptr_data);

/* get/set "message IN3 R"     configuration */
void Messages_Config_MessagesIn3R_GetDefault    (MESSAGES__CONFIG__MESSAGE_IN3_R     *ptr_data);
void Messages_Config_MessagesIn3R_Get           (MESSAGES__CONFIG__MESSAGE_IN3_R     *ptr_data);
void Messages_Config_MessagesIn3R_Set           (MESSAGES__CONFIG__MESSAGE_IN3_R     *ptr_data);
bool Messages_Config_MessagesIn3R_IsValid       (MESSAGES__CONFIG__MESSAGE_IN3_R     *ptr_data);

/* get/set "message IN4 R"     configuration */
void Messages_Config_MessagesIn4R_GetDefault    (MESSAGES__CONFIG__MESSAGE_IN4_R     *ptr_data);
void Messages_Config_MessagesIn4R_Get           (MESSAGES__CONFIG__MESSAGE_IN4_R     *ptr_data);
void Messages_Config_MessagesIn4R_Set           (MESSAGES__CONFIG__MESSAGE_IN4_R     *ptr_data);
bool Messages_Config_MessagesIn4R_IsValid       (MESSAGES__CONFIG__MESSAGE_IN4_R     *ptr_data);

/* get/set "message TMIN R"    configuration */
void Messages_Config_MessagesTminR_GetDefault   (MESSAGES__CONFIG__MESSAGE_TMIN_R    *ptr_data);
void Messages_Config_MessagesTminR_Get          (MESSAGES__CONFIG__MESSAGE_TMIN_R    *ptr_data);
void Messages_Config_MessagesTminR_Set          (MESSAGES__CONFIG__MESSAGE_TMIN_R    *ptr_data);
bool Messages_Config_MessagesTminR_IsValid      (MESSAGES__CONFIG__MESSAGE_TMIN_R    *ptr_data);

/* get/set "message TMAX R"    configuration */
void Messages_Config_MessagesTmaxR_GetDefault   (MESSAGES__CONFIG__MESSAGE_TMAX_R    *ptr_data);
void Messages_Config_MessagesTmaxR_Get          (MESSAGES__CONFIG__MESSAGE_TMAX_R    *ptr_data);
void Messages_Config_MessagesTmaxR_Set          (MESSAGES__CONFIG__MESSAGE_TMAX_R    *ptr_data);
bool Messages_Config_MessagesTmaxR_IsValid      (MESSAGES__CONFIG__MESSAGE_TMAX_R    *ptr_data);

/* get/set "message TMINEXT R" configuration */
void Messages_Config_MessagesTminExtR_GetDefault(MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data);
void Messages_Config_MessagesTminExtR_Get       (MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data);
void Messages_Config_MessagesTminExtR_Set       (MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data);
bool Messages_Config_MessagesTminExtR_IsValid   (MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data);

/* get/set "message TMAXEXT R" configuration */
void Messages_Config_MessagesTmaxExtR_GetDefault(MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data);
void Messages_Config_MessagesTmaxExtR_Get       (MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data);
void Messages_Config_MessagesTmaxExtR_Set       (MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data);
bool Messages_Config_MessagesTmaxExtR_IsValid   (MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data);




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1_GetDefault
 *
 * Description: get the default "message IN1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn1_GetDefault(MESSAGES__CONFIG__MESSAGE_IN1 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn1Default;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1_Get
 *
 * Description: get the "message IN1" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn1_Get(MESSAGES__CONFIG__MESSAGE_IN1 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn1;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1_Set
 *
 * Description: set the "message IN1" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn1_Set(MESSAGES__CONFIG__MESSAGE_IN1 *ptr_data)
{
    Messages_Config_MessagesIn1 = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1_IsValid
 *
 * Description: check if the "message IN1" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn1_IsValid(MESSAGES__CONFIG__MESSAGE_IN1 *ptr_data)
{
    if (strlen(ptr_data->message_in1) > MESSAGES__LEN_MAX_MESSAGE__IN1)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2_GetDefault
 *
 * Description: get the default "message IN2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn2_GetDefault(MESSAGES__CONFIG__MESSAGE_IN2 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn2Default;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2_Get
 *
 * Description: get the "message IN2" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn2_Get(MESSAGES__CONFIG__MESSAGE_IN2 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn2;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2_Set
 *
 * Description: set the "message IN2" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn2_Set(MESSAGES__CONFIG__MESSAGE_IN2 *ptr_data)
{
    Messages_Config_MessagesIn2 = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2_IsValid
 *
 * Description: check if the "message IN2" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn2_IsValid(MESSAGES__CONFIG__MESSAGE_IN2 *ptr_data)
{
    if (strlen(ptr_data->message_in2) > MESSAGES__LEN_MAX_MESSAGE__IN2)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3_GetDefault
 *
 * Description: get the default "message IN3" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn3_GetDefault(MESSAGES__CONFIG__MESSAGE_IN3 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn3Default;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3_Get
 *
 * Description: get the "message IN3" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn3_Get(MESSAGES__CONFIG__MESSAGE_IN3 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn3;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3_Set
 *
 * Description: set the "message IN3" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn3_Set(MESSAGES__CONFIG__MESSAGE_IN3 *ptr_data)
{
    Messages_Config_MessagesIn3 = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3_IsValid
 *
 * Description: check if the "message IN3" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn3_IsValid(MESSAGES__CONFIG__MESSAGE_IN3 *ptr_data)
{
    if (strlen(ptr_data->message_in3) > MESSAGES__LEN_MAX_MESSAGE__IN3)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4_GetDefault
 *
 * Description: get the default "message IN4" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn4_GetDefault(MESSAGES__CONFIG__MESSAGE_IN4 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn4Default;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4_Get
 *
 * Description: get the "message IN4" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn4_Get(MESSAGES__CONFIG__MESSAGE_IN4 *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn4;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4_Set
 *
 * Description: set the "message IN4" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn4_Set(MESSAGES__CONFIG__MESSAGE_IN4 *ptr_data)
{
    Messages_Config_MessagesIn4 = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4_IsValid
 *
 * Description: check if the "message IN4" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn4_IsValid(MESSAGES__CONFIG__MESSAGE_IN4 *ptr_data)
{
    if (strlen(ptr_data->message_in4) > MESSAGES__LEN_MAX_MESSAGE__IN4)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmin_GetDefault
 *
 * Description: get the default "message TMIN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmin_GetDefault(MESSAGES__CONFIG__MESSAGE_TMIN *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmin_Get
 *
 * Description: get the "message TMIN" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmin_Get(MESSAGES__CONFIG__MESSAGE_TMIN *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmin;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmin_Set
 *
 * Description: set the "message TMIN" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmin_Set(MESSAGES__CONFIG__MESSAGE_TMIN *ptr_data)
{
    Messages_Config_MessagesTmin = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmin_IsValid
 *
 * Description: check if the "message TMIN" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTmin_IsValid(MESSAGES__CONFIG__MESSAGE_TMIN *ptr_data)
{
    if (strlen(ptr_data->message_tmin) > MESSAGES__LEN_MAX_MESSAGE__TMIN)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmax_GetDefault
 *
 * Description: get the default "message TMAX" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmax_GetDefault(MESSAGES__CONFIG__MESSAGE_TMAX *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmax_Get
 *
 * Description: get the "message TMAX" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmax_Get(MESSAGES__CONFIG__MESSAGE_TMAX *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmax;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmax_Set
 *
 * Description: set the "message TMAX" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmax_Set(MESSAGES__CONFIG__MESSAGE_TMAX *ptr_data)
{
    Messages_Config_MessagesTmax = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmax_IsValid
 *
 * Description: check if the "message TMAX" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTmax_IsValid(MESSAGES__CONFIG__MESSAGE_TMAX *ptr_data)
{
    if (strlen(ptr_data->message_tmax) > MESSAGES__LEN_MAX_MESSAGE__TMAX)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExt_GetDefault
 *
 * Description: get the default "message TMINEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminExt_GetDefault(MESSAGES__CONFIG__MESSAGE_TMINEXT *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminExtDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExt_Get
 *
 * Description: get the "message TMINEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminExt_Get(MESSAGES__CONFIG__MESSAGE_TMINEXT *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminExt;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExt_Set
 *
 * Description: set the "message TMINEXT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminExt_Set(MESSAGES__CONFIG__MESSAGE_TMINEXT *ptr_data)
{
    Messages_Config_MessagesTminExt = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExt_IsValid
 *
 * Description: check if the "message TMINEXT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTminExt_IsValid(MESSAGES__CONFIG__MESSAGE_TMINEXT *ptr_data)
{
    if (strlen(ptr_data->message_tminext) > MESSAGES__LEN_MAX_MESSAGE__TMINEXT)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExt_GetDefault
 *
 * Description: get the default "message TMAXEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxExt_GetDefault(MESSAGES__CONFIG__MESSAGE_TMAXEXT *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxExtDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExt_Get
 *
 * Description: get the "message TMAXEXT" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxExt_Get(MESSAGES__CONFIG__MESSAGE_TMAXEXT *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxExt;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExt_Set
 *
 * Description: set the "message TMAXEXT" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxExt_Set(MESSAGES__CONFIG__MESSAGE_TMAXEXT *ptr_data)
{
    Messages_Config_MessagesTmaxExt = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExt_IsValid
 *
 * Description: check if the "message TMAXEXT" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTmaxExt_IsValid(MESSAGES__CONFIG__MESSAGE_TMAXEXT *ptr_data)
{
    if (strlen(ptr_data->message_tmaxext) > MESSAGES__LEN_MAX_MESSAGE__TMAXEXT)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1R_GetDefault
 *
 * Description: get the default "message IN1 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn1R_GetDefault(MESSAGES__CONFIG__MESSAGE_IN1_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn1RDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1R_Get
 *
 * Description: get the "message IN1 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn1R_Get(MESSAGES__CONFIG__MESSAGE_IN1_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn1R;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1R_Set
 *
 * Description: set the "message IN1 R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn1R_Set(MESSAGES__CONFIG__MESSAGE_IN1_R *ptr_data)
{
    Messages_Config_MessagesIn1R = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn1R_IsValid
 *
 * Description: check if the "message IN1 R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn1R_IsValid(MESSAGES__CONFIG__MESSAGE_IN1_R *ptr_data)
{
    if (strlen(ptr_data->message_in1_r) > MESSAGES__LEN_MAX_MESSAGE__IN1_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2R_GetDefault
 *
 * Description: get the default "message IN2 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn2R_GetDefault(MESSAGES__CONFIG__MESSAGE_IN2_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn2RDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2R_Get
 *
 * Description: get the "message IN2 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn2R_Get(MESSAGES__CONFIG__MESSAGE_IN2_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn2R;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2R_Set
 *
 * Description: set the "message IN2 R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn2R_Set(MESSAGES__CONFIG__MESSAGE_IN2_R *ptr_data)
{
    Messages_Config_MessagesIn2R = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn2R_IsValid
 *
 * Description: check if the "message IN2 R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn2R_IsValid(MESSAGES__CONFIG__MESSAGE_IN2_R *ptr_data)
{
    if (strlen(ptr_data->message_in2_r) > MESSAGES__LEN_MAX_MESSAGE__IN2_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3R_GetDefault
 *
 * Description: get the default "message IN3 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn3R_GetDefault(MESSAGES__CONFIG__MESSAGE_IN3_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn3RDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3R_Get
 *
 * Description: get the "message IN3 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn3R_Get(MESSAGES__CONFIG__MESSAGE_IN3_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn3R;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3R_Set
 *
 * Description: set the "message IN3 R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn3R_Set(MESSAGES__CONFIG__MESSAGE_IN3_R *ptr_data)
{
    Messages_Config_MessagesIn3R = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn3R_IsValid
 *
 * Description: check if the "message IN3 R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn3R_IsValid(MESSAGES__CONFIG__MESSAGE_IN3_R *ptr_data)
{
    if (strlen(ptr_data->message_in3_r) > MESSAGES__LEN_MAX_MESSAGE__IN3_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4R_GetDefault
 *
 * Description: get the default "message IN4 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn4R_GetDefault(MESSAGES__CONFIG__MESSAGE_IN4_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn4RDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4R_Get
 *
 * Description: get the "message IN4 R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn4R_Get(MESSAGES__CONFIG__MESSAGE_IN4_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesIn4R;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4R_Set
 *
 * Description: set the "message IN4 R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesIn4R_Set(MESSAGES__CONFIG__MESSAGE_IN4_R *ptr_data)
{
    Messages_Config_MessagesIn4R = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesIn4R_IsValid
 *
 * Description: check if the "message IN4 R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesIn4R_IsValid(MESSAGES__CONFIG__MESSAGE_IN4_R *ptr_data)
{
    if (strlen(ptr_data->message_in4_r) > MESSAGES__LEN_MAX_MESSAGE__IN4_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminR_GetDefault
 *
 * Description: get the default "message TMIN R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminR_GetDefault(MESSAGES__CONFIG__MESSAGE_TMIN_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminRDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminR_Get
 *
 * Description: get the "message TMIN R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminR_Get(MESSAGES__CONFIG__MESSAGE_TMIN_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminR;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminR_Set
 *
 * Description: set the "message TMIN R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminR_Set(MESSAGES__CONFIG__MESSAGE_TMIN_R *ptr_data)
{
    Messages_Config_MessagesTminR = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminR_IsValid
 *
 * Description: check if the "message TMIN R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTminR_IsValid(MESSAGES__CONFIG__MESSAGE_TMIN_R *ptr_data)
{
    if (strlen(ptr_data->message_tmin_r) > MESSAGES__LEN_MAX_MESSAGE__TMIN_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxR_GetDefault
 *
 * Description: get the default "message TMAX R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxR_GetDefault(MESSAGES__CONFIG__MESSAGE_TMAX_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxRDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxR_Get
 *
 * Description: get the "message TMAX R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxR_Get(MESSAGES__CONFIG__MESSAGE_TMAX_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxR;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxR_Set
 *
 * Description: set the "message TMAX R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxR_Set(MESSAGES__CONFIG__MESSAGE_TMAX_R *ptr_data)
{
    Messages_Config_MessagesTmaxR = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxR_IsValid
 *
 * Description: check if the "message TMAX R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTmaxR_IsValid(MESSAGES__CONFIG__MESSAGE_TMAX_R *ptr_data)
{
    if (strlen(ptr_data->message_tmax_r) > MESSAGES__LEN_MAX_MESSAGE__TMAX_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExtR_GetDefault
 *
 * Description: get the default "message TMINEXT R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminExtR_GetDefault(MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminExtRDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExtR_Get
 *
 * Description: get the "message TMINEXT R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminExtR_Get(MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTminExtR;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExtR_Set
 *
 * Description: set the "message TMINEXT R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTminExtR_Set(MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data)
{
    Messages_Config_MessagesTminExtR = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTminExtR_IsValid
 *
 * Description: check if the "message TMINEXT R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTminExtR_IsValid(MESSAGES__CONFIG__MESSAGE_TMINEXT_R *ptr_data)
{
    if (strlen(ptr_data->message_tminext_r) > MESSAGES__LEN_MAX_MESSAGE__TMINEXT_R)
        return FALSE;

    return TRUE;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExtR_GetDefault
 *
 * Description: get the default "message TMAXEXT R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxExtR_GetDefault(MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxExtRDefault;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExtR_Get
 *
 * Description: get the "message TMAXEXT R" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxExtR_Get(MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data)
{
    *ptr_data = Messages_Config_MessagesTmaxExtR;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExtR_Set
 *
 * Description: set the "message TMAXEXT R" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Messages_Config_MessagesTmaxExtR_Set(MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data)
{
    Messages_Config_MessagesTmaxExtR = *ptr_data;
}




/*===========================================================================
 * Function   : Messages_Config_MessagesTmaxExtR_IsValid
 *
 * Description: check if the "message TMAXEXT R" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool Messages_Config_MessagesTmaxExtR_IsValid(MESSAGES__CONFIG__MESSAGE_TMAXEXT_R *ptr_data)
{
    if (strlen(ptr_data->message_tmaxext_r) > MESSAGES__LEN_MAX_MESSAGE__TMAXEXT_R)
        return FALSE;

    return TRUE;
}
