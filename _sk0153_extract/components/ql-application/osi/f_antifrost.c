/*=============================================================================
 * File       :  F_ANTIFROST.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - "antifrost" function
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "boot.h"
#include "debug_my.h"
#include "f_antifrost.h"
#include "fw_config.h"
#include "outputs.h"
#include "temperature.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* antifrost temperature (/10 �C) */
#define ANTIFROST_TEMPERATURE_MIN                   50    /* minimum antifrost temperature (/10 �C) */
#define ANTIFROST_TEMPERATURE_MAX                   180   /* maximum antifrost temperature (/10 �C) */

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                     200

/*-----------------------------------------------------------------------------
 * SEM states
 *-----------------------------------------------------------------------------*/
/* SEM states */
#define SEM_STATE__ANTIFROST_STATUS__OFF            0     /* SEM state - antifrost status - off */
#define SEM_STATE__ANTIFROST_STATUS__ON             1     /* SEM state - antifrost status - on  */

/* SEM state strings */
#define SEM_STATE_STRING__ANTIFROST_STATUS__OFF     "SEM_STATE__ANTIFROST_STATUS__OFF"
#define SEM_STATE_STRING__ANTIFROST_STATUS__ON      "SEM_STATE__ANTIFROST_STATUS__ON "


/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__ANTIFROST_INT__OFF               0     /* SEM event - internal antifrost - off */
#define SEM_EVENT__ANTIFROST_INT__ON                1     /* SEM event - internal antifrost - on  */
#define SEM_EVENT__ANTIFROST_EXT__OFF               2     /* SEM event - external antifrost - off */
#define SEM_EVENT__ANTIFROST_EXT__ON                3     /* SEM event - external antifrost - on  */

/* SEM events strings */
#define SEM_EVENT_STRING__ANTIFROST_INT__OFF        "SEM_EVENT__ANTIFROST_INT__OFF   "
#define SEM_EVENT_STRING__ANTIFROST_INT__ON         "SEM_EVENT__ANTIFROST_INT__ON    "
#define SEM_EVENT_STRING__ANTIFROST_EXT__OFF        "SEM_EVENT__ANTIFROST_EXT__OFF   "
#define SEM_EVENT_STRING__ANTIFROST_EXT__ON         "SEM_EVENT__ANTIFROST_EXT__ON    "


/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__ANTIFROST_INT__OFF             (11700 | (QL_COMPONENT_APP_START << 16))  /* event - internal antifrost - off           */
#define TASK_MSG_ID__ANTIFROST_INT__ON              (11701 | (QL_COMPONENT_APP_START << 16))  /* event - internal antifrost - on            */
#define TASK_MSG_ID__ANTIFROST_EXT__OFF             (11702 | (QL_COMPONENT_APP_START << 16))  /* event - external antifrost - off           */
#define TASK_MSG_ID__ANTIFROST_EXT__ON              (11703 | (QL_COMPONENT_APP_START << 16))  /* event - external antifrost - on            */




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - internal "antifrost function" */
static       F_ANTIFROST__CONFIG_F_ANTIFROST_INT FAntifrost_Config_FAntifrostInt;
static const F_ANTIFROST__CONFIG_F_ANTIFROST_INT FAntifrost_Config_FAntifrostIntDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,     /* function status       */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,    /* function status       */
#endif

    70,       /* antifrost temperature */
};


/* configuration - external "antifrost function" */
static       F_ANTIFROST__CONFIG_F_ANTIFROST_EXT FAntifrost_Config_FAntifrostExt;
static const F_ANTIFROST__CONFIG_F_ANTIFROST_EXT FAntifrost_Config_FAntifrostExtDefault =
{
#if defined(FW_CONFIG__VERSION__LSHW) || defined(FW_CONFIG__VERSION__LSHD)
    TRUE,     /* function status       */
#endif

#if defined(FW_CONFIG__VERSION__LFVW) || defined(FW_CONFIG__VERSION__LFVD)
    FALSE,    /* function status       */
#endif

    70,       /* antifrost temperature */
};


/* antifrost function status (internal and external) */
static       u8                                  FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__OFF;
static       u8                                  FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__OFF;


/* debug string */
static       ascii                               FAntifrost_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void FAntifrost_TaskFAntifrost(void *argument);

/* task events */
       void FAntifrost_NewConfigurationInt(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_config_new, F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_config_old);
       void FAntifrost_NewConfigurationExt(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_config_new, F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_config_old);

static bool FAntifrost_AntifrostTemperatureInt_On (s16 temperature);
static bool FAntifrost_AntifrostTemperatureInt_Off(void);

static bool FAntifrost_AntifrostTemperatureExt_On (s16 temperature);
static bool FAntifrost_AntifrostTemperatureExt_Off(void);

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
/* get/set internal "antifrost function" configuration */
       void FAntifrost__Config_FAntifrostInt_GetDefault(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);
       void FAntifrost__Config_FAntifrostInt_Get       (F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);
       void FAntifrost__Config_FAntifrostInt_Set       (F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);
       bool FAntifrost__Config_FAntifrostInt_IsValid   (F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data);

/* get/set external "antifrost function" configuration */
       void FAntifrost__Config_FAntifrostExt_GetDefault(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);
       void FAntifrost__Config_FAntifrostExt_Get       (F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);
       void FAntifrost__Config_FAntifrostExt_Set       (F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);
       bool FAntifrost__Config_FAntifrostExt_IsValid   (F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data);

/*-----------------------------------------------------------------------------
 * Open AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void FAntifrost_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier);




/*=============================================================================
 * Function   : FAntifrost_TaskFAntifrost
 *
 * Description: "antifrost" function task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void FAntifrost_TaskFAntifrost(void *argument)
{
    ql_event_t event;
    QlOSStatus err;

    bool       result;


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - F_ANTIFROST - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* internal antifrost function */
    if (FAntifrost_Config_FAntifrostInt.status)
    {
        /* internal "antifrost" function enabled */
        FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__ON;
        result = Outputs_Event_FAntifrostInt_On(FAntifrost_Config_FAntifrostInt.temperature);
    }
    else
    {
        /* internal "antifrost" function disabled */
        FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__OFF;
    }


    /* external antifrost function */
    if (FAntifrost_Config_FAntifrostExt.status)
    {
        /* external "antifrost" function enabled */
        FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__ON;
        result = Outputs_Event_FAntifrostExt_On(FAntifrost_Config_FAntifrostExt.temperature);
    }
    else
    {
        /* external "antifrost" function disabled */
        FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__OFF;
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received event */
            FAntifrost_AdlCallback_Message_TaskMsg(&event);
        }
    }
}




/*=============================================================================
 * Function   : FAntifrost_NewConfigurationInt
 *
 * Description: signal a new configuration of internal antifrost function
 * Input      : - ptr_config_new: pointer to new configuration
 *              - ptr_config_old: pointer to old configuration
 * Output     : -
 *=============================================================================*/
void FAntifrost_NewConfigurationInt(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_config_new, F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_config_old)
{
    bool result;

    (void)result;


    if      ((!ptr_config_old->status) && ( ptr_config_new->status))
    {
        /* internal antifrost function activation */
        result = FAntifrost_AntifrostTemperatureInt_On(ptr_config_new->temperature);
    }
    else if (( ptr_config_old->status) && (!ptr_config_new->status))
    {
        /* internal antifrost function disactivation */
        result = FAntifrost_AntifrostTemperatureInt_Off();
    }
    else if (( ptr_config_old->status) && ( ptr_config_new->status))
    {
        /* internal antifrost function parameters change */
        if (( ptr_config_old->temperature != ptr_config_new->temperature))
            result = FAntifrost_AntifrostTemperatureInt_On(ptr_config_new->temperature);
    }
}




/*=============================================================================
 * Function   : FAntifrost_NewConfigurationExt
 *
 * Description: signal a new configuration of external antifrost function
 * Input      : - ptr_config_new: pointer to new configuration
 *              - ptr_config_old: pointer to old configuration
 * Output     : -
 *=============================================================================*/
void FAntifrost_NewConfigurationExt(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_config_new, F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_config_old)
{
    bool result;

    (void)result;


    if      ((!ptr_config_old->status) && ( ptr_config_new->status))
    {
        /* internal antifrost function activation */
        result = FAntifrost_AntifrostTemperatureExt_On(ptr_config_new->temperature);
    }
    else if (( ptr_config_old->status) && (!ptr_config_new->status))
    {
        /* internal antifrost function disactivation */
        result = FAntifrost_AntifrostTemperatureExt_Off();
    }
    else if (( ptr_config_old->status) && ( ptr_config_new->status))
    {
        /* internal antifrost function parameters change */
        if (( ptr_config_old->temperature != ptr_config_new->temperature))
            result = FAntifrost_AntifrostTemperatureExt_On(ptr_config_new->temperature);
    }
}




/*=============================================================================
 * Function   : FAntifrost_AntifrostTemperatureInt_On
 *
 * Description: signal the internal temperature antifrost on
 * Input      : - temperature: antifrost temperature (/10 �C)
 * Output     : - FALSE: antifrost not started
 *              - TRUE : antifrost     started
 *=============================================================================*/
bool FAntifrost_AntifrostTemperatureInt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;

    if (
         (temperature < ANTIFROST_TEMPERATURE_MIN) ||
         (temperature > ANTIFROST_TEMPERATURE_MAX)
       )
    {
        return FALSE;
    }


    event.id     = TASK_MSG_ID__ANTIFROST_INT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_FAntifrost, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : FAntifrost_AntifrostTemperatureInt_Off
 *
 * Description: signal the internal temperature antifrost off
 * Input      : -
 * Output     : - FALSE: antifrost not stopped
 *              - TRUE : antifrost     stopped
 *=============================================================================*/
bool FAntifrost_AntifrostTemperatureInt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__ANTIFROST_INT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_FAntifrost, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*=============================================================================
 * Function   : FAntifrost_AntifrostTemperatureExt_On
 *
 * Description: signal the external temperature antifrost on
 * Input      : - temperature: antifrost temperature (/10 �C)
 * Output     : - FALSE: antifrost not started
 *              - TRUE : antifrost     started
 *=============================================================================*/
bool FAntifrost_AntifrostTemperatureExt_On(s16 temperature)
{
    ql_event_t event;
    QlOSStatus err;


    if (!Temperature_IsValidTemperature(temperature))
        return FALSE;

    if (
         (temperature < ANTIFROST_TEMPERATURE_MIN) ||
         (temperature > ANTIFROST_TEMPERATURE_MAX)
       )
    {
        return FALSE;
    }


    event.id     = TASK_MSG_ID__ANTIFROST_EXT__ON;
    event.param1 = temperature;

    err = ql_rtos_event_send(Boot_TaskRef_FAntifrost, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    return TRUE;
}




/*=============================================================================
 * Function   : FAntifrost_AntifrostTemperatureExt_Off
 *
 * Description: signal the external temperature antifrost off
 * Input      : -
 * Output     : - FALSE: antifrost not stopped
 *              - TRUE : antifrost     stopped
 *=============================================================================*/
bool FAntifrost_AntifrostTemperatureExt_Off(void)
{
    ql_event_t event;
    QlOSStatus err;


    event.id = TASK_MSG_ID__ANTIFROST_EXT__OFF;

    err = ql_rtos_event_send(Boot_TaskRef_FAntifrost, &event);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    return TRUE;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostInt_GetDefault
 *
 * Description: get the default internal "antifrost function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FAntifrost__Config_FAntifrostInt_GetDefault(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data)
{
    *ptr_data = FAntifrost_Config_FAntifrostIntDefault;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostInt_Get
 *
 * Description: get the internal "antifrost function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FAntifrost__Config_FAntifrostInt_Get(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data)
{
    *ptr_data = FAntifrost_Config_FAntifrostInt;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostInt_Set
 *
 * Description: set the internal "antifrost function" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FAntifrost__Config_FAntifrostInt_Set(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data)
{
    FAntifrost_Config_FAntifrostInt = *ptr_data;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostInt_IsValid
 *
 * Description: check if the internal "antifrost function" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FAntifrost__Config_FAntifrostInt_IsValid(F_ANTIFROST__CONFIG_F_ANTIFROST_INT *ptr_data)
{
    if (
         (ptr_data->temperature < ANTIFROST_TEMPERATURE_MIN) ||
         (ptr_data->temperature > ANTIFROST_TEMPERATURE_MAX)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostExt_GetDefault
 *
 * Description: get the default external "antifrost function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FAntifrost__Config_FAntifrostExt_GetDefault(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data)
{
    *ptr_data = FAntifrost_Config_FAntifrostExtDefault;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostExt_Get
 *
 * Description: get the external "antifrost function" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void FAntifrost__Config_FAntifrostExt_Get(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data)
{
    *ptr_data = FAntifrost_Config_FAntifrostExt;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostExt_Set
 *
 * Description: set the external "antifrost function" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void FAntifrost__Config_FAntifrostExt_Set(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data)
{
    FAntifrost_Config_FAntifrostExt = *ptr_data;
}




/*===========================================================================
 * Function   : FAntifrost__Config_FAntifrostExt_IsValid
 *
 * Description: check if the external "antifrost function" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool FAntifrost__Config_FAntifrostExt_IsValid(F_ANTIFROST__CONFIG_F_ANTIFROST_EXT *ptr_data)
{
    if (
         (ptr_data->temperature < ANTIFROST_TEMPERATURE_MIN) ||
         (ptr_data->temperature > ANTIFROST_TEMPERATURE_MAX)
       )
    {
        return FALSE;
    }

    return TRUE;
}




/*===========================================================================
 * Function   : FAntifrost_AdlCallback_Message_TaskMsg
 *
 * Description: - task message callback
 * Input      : - ptr_data:
 * Output     : -
 *===========================================================================*/
static void FAntifrost_AdlCallback_Message_TaskMsg(ql_event_t *msg_identifier)
{
                 u32    sem_event;

                 s16    temperature_reg_int;
                 s16    temperature_reg_ext;

                 bool   result;

    static const ascii *sem_state_strings[] =
    {
        SEM_STATE_STRING__ANTIFROST_STATUS__OFF,
        SEM_STATE_STRING__ANTIFROST_STATUS__ON,
    };

    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__ANTIFROST_INT__OFF,
        SEM_EVENT_STRING__ANTIFROST_INT__ON,
        SEM_EVENT_STRING__ANTIFROST_EXT__OFF,
        SEM_EVENT_STRING__ANTIFROST_EXT__ON,
    };


    /* avoid compiler warning unused-but-set-variable */
    (void)result;


    /* debug */
    snprintf(FAntifrost_DebugString, sizeof(FAntifrost_DebugString), "CALLBACK     - MESSAGE     - TASK F_ANTIFROST - msg identifier: %u", msg_identifier->id);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, FAntifrost_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    //sem_event = msg_identifier;
    switch (msg_identifier->id)
    {
        case TASK_MSG_ID__ANTIFROST_INT__OFF:
            sem_event = SEM_EVENT__ANTIFROST_INT__OFF;
            break;

        case TASK_MSG_ID__ANTIFROST_INT__ON:
            sem_event = SEM_EVENT__ANTIFROST_INT__ON;
            break;

        case TASK_MSG_ID__ANTIFROST_EXT__OFF:
            sem_event = SEM_EVENT__ANTIFROST_EXT__OFF;
            break;

        case TASK_MSG_ID__ANTIFROST_EXT__ON:
            sem_event = SEM_EVENT__ANTIFROST_EXT__ON;
            break;

        default:
            return;
    }



    snprintf(FAntifrost_DebugString, sizeof(FAntifrost_DebugString), "SEM state int: %s", sem_state_strings[FAntifrost_FAntifrostStatusInt]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, FAntifrost_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FAntifrost_DebugString, sizeof(FAntifrost_DebugString), "SEM state ext: %s", sem_state_strings[FAntifrost_FAntifrostStatusExt]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, FAntifrost_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(FAntifrost_DebugString, sizeof(FAntifrost_DebugString), "SEM event    : %s", sem_event_strings[sem_event                     ]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_F_ANTIFROST, DEBUG_TRACE_TYPE_LOW, FAntifrost_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);



    /*---------------------------------------------------------------------------
    * Internal antifrost
    *---------------------------------------------------------------------------*/
    switch (FAntifrost_FAntifrostStatusInt)
    {
        /*----------------------------------------------
        * SEM state - antifrost status - off
        *----------------------------------------------*/
        case SEM_STATE__ANTIFROST_STATUS__OFF:
            switch (sem_event)
            {
                /* event - internal antifrost - on */
                case SEM_EVENT__ANTIFROST_INT__ON:
                    /* extract the parameters */
                    temperature_reg_int = (s16)msg_identifier->param1;

                    result = Outputs_Event_FAntifrostInt_On(temperature_reg_int);

                    FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__ON;
                    break;


                /* event - internal antifrost - off */
                case SEM_EVENT__ANTIFROST_INT__OFF:
                    // NOTHING TO DO
                    break;


                // unknown event
                default:
                    break;
            }
            break;



        /*----------------------------------------------
        * SEM state - antifrost status - on
        *----------------------------------------------*/
        case SEM_STATE__ANTIFROST_STATUS__ON:
            switch (sem_event)
            {
                /* event - internal antifrost - on */
                case SEM_EVENT__ANTIFROST_INT__ON:
                    /* extract the parameters */
                    temperature_reg_int = (s16)msg_identifier->param1;

                    result = Outputs_Event_FAntifrostInt_On(temperature_reg_int);

                    FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__ON;
                    break;


                /* event - internal antifrost - off */
                case SEM_EVENT__ANTIFROST_INT__OFF:
                    result = Outputs_Event_FAntifrostInt_Off();

                    FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__OFF;
                    break;


                // unknown event
                default:
                    break;
            }
            break;



        /*----------------------------------------------
        * antifrost status - unknown
        *----------------------------------------------*/
        default:
            FAntifrost_FAntifrostStatusInt = SEM_STATE__ANTIFROST_STATUS__OFF;
            break;
    }




    /*---------------------------------------------------------------------------
    * External antifrost
    *---------------------------------------------------------------------------*/
    switch (FAntifrost_FAntifrostStatusExt)
    {
        /*----------------------------------------------
        * SEM state - antifrost status - off
        *----------------------------------------------*/
        case SEM_STATE__ANTIFROST_STATUS__OFF:
            switch (sem_event)
            {
                /* event - external antifrost - on */
                case SEM_EVENT__ANTIFROST_EXT__ON:
                    /* extract the parameters */
                    temperature_reg_ext = (s16)msg_identifier->param1;

                    result = Outputs_Event_FAntifrostExt_On(temperature_reg_ext);

                    FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__ON;
                    break;


                /* event - external antifrost - off */
                case SEM_EVENT__ANTIFROST_EXT__OFF:
                    // NOTHING TO DO
                    break;


                // unknown event
                default:
                    break;
            }
            break;



        /*----------------------------------------------
        * SEM state - antifrost status - on
        *----------------------------------------------*/
        case SEM_STATE__ANTIFROST_STATUS__ON:
            switch (sem_event)
            {
                /* event - external antifrost - on */
                case SEM_EVENT__ANTIFROST_EXT__ON:
                    /* extract the parameters */
                    temperature_reg_ext = (s16)msg_identifier->param1;

                    result = Outputs_Event_FAntifrostExt_On(temperature_reg_ext);

                    FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__ON;
                    break;


                /* event - external antifrost - off */
                case SEM_EVENT__ANTIFROST_EXT__OFF:
                    result = Outputs_Event_FAntifrostExt_Off();

                    FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__OFF;
                    break;


                // unknown event
                default:
                    break;
            }
            break;



        /*----------------------------------------------
        * antifrost status - unknown
        *----------------------------------------------*/
        default:
            FAntifrost_FAntifrostStatusExt = SEM_STATE__ANTIFROST_STATUS__OFF;
            break;
    }
}
