/*=============================================================================
 * File       :  MESSAGES.H
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - messages
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




#ifndef __MESSAGES_H__


#define __MESSAGES_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* maximum message string lengths */

#define MESSAGES__LEN_MAX_MESSAGE__IN1        60     /* IN1       */
#define MESSAGES__LEN_MAX_MESSAGE__IN2        60     /* IN2       */
#define MESSAGES__LEN_MAX_MESSAGE__IN3        60     /* IN3       */
#define MESSAGES__LEN_MAX_MESSAGE__IN4        60     /* IN4       */
#define MESSAGES__LEN_MAX_MESSAGE__TMIN       60     /* TMIN      */
#define MESSAGES__LEN_MAX_MESSAGE__TMAX       60     /* TMAX      */
#define MESSAGES__LEN_MAX_MESSAGE__TMINEXT    60     /* TMINEXT   */
#define MESSAGES__LEN_MAX_MESSAGE__TMAXEXT    60     /* TMAXEXT   */

#define MESSAGES__LEN_MAX_MESSAGE__IN1_R      60     /* IN1 R     */
#define MESSAGES__LEN_MAX_MESSAGE__IN2_R      60     /* IN2 R     */
#define MESSAGES__LEN_MAX_MESSAGE__IN3_R      60     /* IN3 R     */
#define MESSAGES__LEN_MAX_MESSAGE__IN4_R      60     /* IN4 R     */
#define MESSAGES__LEN_MAX_MESSAGE__TMIN_R     60     /* TMIN R    */
#define MESSAGES__LEN_MAX_MESSAGE__TMAX_R     60     /* TMAX R    */
#define MESSAGES__LEN_MAX_MESSAGE__TMINEXT_R  60     /* TMINEXT R */
#define MESSAGES__LEN_MAX_MESSAGE__TMAXEXT_R  60     /* TMAXEXT R */




/*=============================================================================
 * DATA TYPE
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * Configurations
 *-----------------------------------------------------------------------------*/
/* configuration - message IN1 */
typedef struct
{
    ascii message_in1      [MESSAGES__LEN_MAX_MESSAGE__IN1        + 1];
} MESSAGES__CONFIG__MESSAGE_IN1;


/* configuration - message IN2 */
typedef struct
{
    ascii message_in2     [MESSAGES__LEN_MAX_MESSAGE__IN2         + 1];
} MESSAGES__CONFIG__MESSAGE_IN2;


/* configuration - message IN3 */
typedef struct
{
    ascii message_in3     [MESSAGES__LEN_MAX_MESSAGE__IN3         + 1];
} MESSAGES__CONFIG__MESSAGE_IN3;


/* configuration - message IN4 */
typedef struct
{
    ascii message_in4     [MESSAGES__LEN_MAX_MESSAGE__IN4         + 1];
} MESSAGES__CONFIG__MESSAGE_IN4;


/* configuration - message TMIN */
typedef struct
{
    ascii message_tmin     [MESSAGES__LEN_MAX_MESSAGE__TMIN       + 1];
} MESSAGES__CONFIG__MESSAGE_TMIN;


/* configuration - message TMAX */
typedef struct
{
    ascii message_tmax     [MESSAGES__LEN_MAX_MESSAGE__TMAX       + 1];
} MESSAGES__CONFIG__MESSAGE_TMAX;

/* configuration - message TMINEXT */
typedef struct
{
    ascii message_tminext  [MESSAGES__LEN_MAX_MESSAGE__TMINEXT    + 1];
} MESSAGES__CONFIG__MESSAGE_TMINEXT;


/* configuration - message TMAXEXT */
typedef struct
{
    ascii message_tmaxext  [MESSAGES__LEN_MAX_MESSAGE__TMAXEXT    + 1];
} MESSAGES__CONFIG__MESSAGE_TMAXEXT;



/* configuration - message IN1 R */
typedef struct
{
    ascii message_in1_r    [MESSAGES__LEN_MAX_MESSAGE__IN1_R     + 1];
} MESSAGES__CONFIG__MESSAGE_IN1_R;


/* configuration - message IN2 R */
typedef struct
{
    ascii message_in2_r    [MESSAGES__LEN_MAX_MESSAGE__IN2_R     + 1];
} MESSAGES__CONFIG__MESSAGE_IN2_R;


/* configuration - message IN3 R */
typedef struct
{
    ascii message_in3_r    [MESSAGES__LEN_MAX_MESSAGE__IN3_R     + 1];
} MESSAGES__CONFIG__MESSAGE_IN3_R;


/* configuration - message IN4 R */
typedef struct
{
    ascii message_in4_r    [MESSAGES__LEN_MAX_MESSAGE__IN4_R     + 1];
} MESSAGES__CONFIG__MESSAGE_IN4_R;


/* configuration - message TMIN R */
typedef struct
{
    ascii message_tmin_r   [MESSAGES__LEN_MAX_MESSAGE__TMIN_R    + 1];
} MESSAGES__CONFIG__MESSAGE_TMIN_R;


/* configuration - message TMAX R */
typedef struct
{
    ascii message_tmax_r   [MESSAGES__LEN_MAX_MESSAGE__TMAX_R    + 1];
} MESSAGES__CONFIG__MESSAGE_TMAX_R;

/* configuration - message TMINEXT R */
typedef struct
{
    ascii message_tminext_r[MESSAGES__LEN_MAX_MESSAGE__TMINEXT_R + 1];
} MESSAGES__CONFIG__MESSAGE_TMINEXT_R;


/* configuration - message TMAXEXTR */
typedef struct
{
    ascii message_tmaxext_r[MESSAGES__LEN_MAX_MESSAGE__TMAXEXT_R + 1];
} MESSAGES__CONFIG__MESSAGE_TMAXEXT_R;




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




#endif
