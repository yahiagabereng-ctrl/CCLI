/*=============================================================================
 * File       :  OUT_KEY.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - digital output keys
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* standard includes */
#include <stdlib.h>

/* user     includes */
#include "debug_my.h"
#include "drvgpio.h"
#include "out_key.h"
#include "outputs.h"
#include "regulation.h"
#include "typedef.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING    200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* configuration - buttons */
static       OUT_KEY__CONFIG__BUTTONS OutKey_Config_Buttons;
static const OUT_KEY__CONFIG__BUTTONS OutKey_Config_ButtonsDefault =
{
    TRUE,    /* status */
};

/* "output keys" block indication */
static       bool                     OutKey_OutKeysBlocked = FALSE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
       void OutKey_Init(void);

/* get/set "buttons" configuration */
       void OutKey_Config_Buttons_GetDefault(OUT_KEY__CONFIG__BUTTONS *ptr_data);
       void OutKey_Config_Buttons_Get       (OUT_KEY__CONFIG__BUTTONS *ptr_data);
       void OutKey_Config_Buttons_Set       (OUT_KEY__CONFIG__BUTTONS *ptr_data);
       bool OutKey_Config_Buttons_IsValid   (OUT_KEY__CONFIG__BUTTONS *ptr_data);

/* "output keys" block/unblock */
       void OutKey_OutKeysBlock  (void);
       void OutKey_OutKeysUnblock(void);

/* "output key" input event */
static void OutKey_KeyOut1Event(u8 input_id, bool input_status);
static void OutKey_KeyOut2Event(u8 input_id, bool input_status);




/*===========================================================================
 * Function   : OutKey_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *===========================================================================*/
void OutKey_Init(void)
{
    /* "output keys" block indication */
    OutKey_OutKeysBlocked = FALSE;

    /* register the function for signals of digital input event */
    DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__KEY_OUT_1, OutKey_KeyOut1Event);
    DrvGpio_RegisterSignalDigitalInputEvent(DRVGPIO__IN__KEY_OUT_2, OutKey_KeyOut2Event);
}




/*===========================================================================
 * Function   : OutKey_Config_Buttons_GetDefault
 *
 * Description: get the default "buttons" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void OutKey_Config_Buttons_GetDefault(OUT_KEY__CONFIG__BUTTONS *ptr_data)
{
    *ptr_data = OutKey_Config_ButtonsDefault;
}




/*===========================================================================
 * Function   : OutKey_Config_Buttons_Get
 *
 * Description: get the "buttons" configuration
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void OutKey_Config_Buttons_Get(OUT_KEY__CONFIG__BUTTONS *ptr_data)
{
    *ptr_data = OutKey_Config_Buttons;
}




/*===========================================================================
 * Function   : OutKey_Config_Buttons_Set
 *
 * Description: set the "buttons" configuration
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void OutKey_Config_Buttons_Set(OUT_KEY__CONFIG__BUTTONS *ptr_data)
{
    OutKey_Config_Buttons = *ptr_data;
}




/*===========================================================================
 * Function   : OutKey_Config_Buttons_IsValid
 *
 * Description: check if the "buttons" configuration is valid
 * Input      : - ptr_data: pointer to the values to be checked
 * Output     : - FALSE: configuration is not valid
 *              - TRUE : configuration is     valid
 *===========================================================================*/
bool OutKey_Config_Buttons_IsValid(OUT_KEY__CONFIG__BUTTONS *ptr_data)
{
    return TRUE;
}




/*===========================================================================
 * Function   : OutKey_OutKeysUnblock
 *
 * Description: signal "output keys" block
 * Input      : -
 * Output     : -
 *===========================================================================*/
void OutKey_OutKeysBlock(void)
{
    OutKey_OutKeysBlocked = TRUE;
}



/*===========================================================================
 * Function   : OutKey_OutKeysUnblock
 *
 * Description: signal "output keys" unblock
 * Input      : -
 * Output     : -
 *===========================================================================*/
void OutKey_OutKeysUnblock(void)
{
    OutKey_OutKeysBlocked = FALSE;
}




/*===========================================================================
 * Function   : OutKey_KeyOut1Event
 *
 * Description: signal new "KEY_OUT_1" input event
 * Input      : - input_id    : input ID
 *              - input_status: input status
 * Output     : -
 *===========================================================================*/
static void OutKey_KeyOut1Event(u8 input_id, bool input_status)
{
    bool out1;
    bool result;

    (void)result;


    if (input_id != DRVGPIO__IN__KEY_OUT_1)
        return;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "New \"KEY_OUT_1\" input event", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (!OutKey_Config_Buttons.status)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "Key disabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return;
    }

    if (OutKey_OutKeysBlocked)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "Key blocked" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return;
    }


    if (!input_status)
    {
        /* OUT1 key pression */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "OUT1 key pression", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* get digital output OUT1 logical status */
        out1 = Regulation_StatusLogicalOutputInt();

        /* invert the digital output OUT1 logical status */
        if (out1)
        {
            /* OUT1 is activated */

            /* disable output 1 */
            result = Outputs_Event_ManualInt_Off();
        }
        else
        {
            /* OUT1 is disactivated */

            /* enable  output 1 */
            result = Outputs_Event_ManualInt_On();
        }
    }
    else
    {
        /* OUT1 key release */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "OUT1 key release", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : OutKey_KeyOut2Event
 *
 * Description: signal new "KEY_OUT_2" input event
 * Input      : - input_id    : input ID
 *              - input_status: input status
 * Output     : -
 *===========================================================================*/
static void OutKey_KeyOut2Event(u8 input_id, bool input_status)
{
    bool out2;
    bool result;

    (void)result;


    if (input_id != DRVGPIO__IN__KEY_OUT_2)
        return;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "New \"KEY_OUT_2\" input event", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    if (!OutKey_Config_Buttons.status)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "Key disabled", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return;
    }

    if (OutKey_OutKeysBlocked)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "Key blocked" , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        return;
    }


    if (!input_status)
    {
        /* OUT2 key pression */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "OUT2 key pression", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* get digital output OUT2 logical status */
        out2 = Regulation_StatusLogicalOutputExt();

        /* invert the digital output OUT2 logical status */
        if (out2)
        {
            /* OUT2 is activated */

            /* disable output 2 */
            result = Outputs_Event_ManualExt_Off();
        }
        else
        {
            /* OUT2 is disactivated */

            /* enable  output 2 */
            result = Outputs_Event_ManualExt_On();
        }
    }
    else
    {
        /* OUT2 key release */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_OTHER, DEBUG_TRACE_TYPE_LOW, "OUT2 key release", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
