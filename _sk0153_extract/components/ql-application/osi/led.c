/*=============================================================================
 * File       :  LED.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - led manager
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 *-----------------------------------------------------------------------------
 * Led blink mode definition
 *-----------------------------------------------------------------------------
 * A led blink mode consists of a sequence of N consecutive blinks repeated M times.
 */




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* API      includes */
#include "ql_api_osi.h"

/* user     includes */
#include "typedef.h"
#include "boot.h"
#include "debug_my.h"
#include "drvgpio.h"
#include "led.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                   200

/*-----------------------------------------------------------------------------
 * SEM status
 *-----------------------------------------------------------------------------*/
/* SEM led status */
#define SEM_STATUS__LED_OFF                       0    /* led status - led off             */
#define SEM_STATUS__LED_ON                        1    /* led status - led on              */
#define SEM_STATUS__LED_BLINK_ON                  2    /* led status - led blink - led on  */
#define SEM_STATUS__LED_BLINK_OFF                 3    /* led status - led blink - led off */

/* SEM led status strings */
#define SEM_STATUS_STRING__LED_OFF                "SEM_STATUS__LED_OFF"
#define SEM_STATUS_STRING__LED_ON                 "SEM_STATUS__LED_ON"
#define SEM_STATUS_STRING__LED_BLINK_ON           "SEM_STATUS__LED_BLINK_ON"
#define SEM_STATUS_STRING__LED_BLINK_OFF          "SEM_STATUS__LED_BLINK_OFF"

/*-----------------------------------------------------------------------------
 * SEM events
 *-----------------------------------------------------------------------------*/
/* SEM events */
#define SEM_EVENT__LED_ON                         0    /* SEM event - led on                 */
#define SEM_EVENT__LED_OFF                        1    /* SEM event - led off                */
#define SEM_EVENT__LED_BLINK_START                2    /* SEM event - led blink start        */
#define SEM_EVENT__TIMEOUT_BLINK_DURATION         3    /* SEM event - timeout blink duration */

/* SEM events strings */
#define SEM_EVENT_STRING__LED_ON                  "SEM_EVENT__LED_ON"
#define SEM_EVENT_STRING__LED_OFF                 "SEM_EVENT__LED_OFF"
#define SEM_EVENT_STRING__LED_BLINK_START         "SEM_EVENT__LED_BLINK_START"
#define SEM_EVENT_STRING__TIMEOUT_BLINK_DURATION  "SEM_EVENT__TIMEOUT_BLINK_DURATION"

/*-----------------------------------------------------------------------------
 * task message IDs
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define TASK_MSG_ID__LED_ON                       0    /* led on                 */
#define TASK_MSG_ID__LED_OFF                      1    /* led off                */
#define TASK_MSG_ID__LED_BLINK_START              2    /* led blink start        */
#define TASK_MSG_ID__TIMEOUT_BLINK_DURATION       3    /* timeout blink duration */

/*-----------------------------------------------------------------------------
 * RTOS
 *-----------------------------------------------------------------------------*/
/* led mail size (bytes) */
#define LED__MAIL_SIZE                            15

/* message queue size */
#define MESSAGE_QUEUE_SIZE                        5




/*===========================================================================
 * DATA TYPES
 *===========================================================================*/
/* mail */
typedef struct
{
    u32 event;                   // message id
    u32 length_data;             // length of additional data
    u8  data[LED__MAIL_SIZE];    //           additional data
} LED__MAIL;




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* led id */
static const u8         Led_LedId    [LED__NUMBER_OF_LEDS] =
{
    LED__LED_GSM_GREEN,            /* led GSM green   */
};

/* led GPIO id */
static const u8         Led_LedGpioId[LED__NUMBER_OF_LEDS] =
{
    DRVGPIO__OUT__LED_GSM_GREEN,   /* GPIO15 - "LED_GSM_GREEN" output */
};

/* debug string */
static       ascii      Led_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* message queue handles */
static       ql_queue_t Led_MessageQueueHandler_MessageQueueEvents;

/* timer handlers */
static       ql_timer_t Led_TimerHandler_TimerBlinkDuration[LED__NUMBER_OF_LEDS];    // timers for blink duration




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void Led_TaskLed(void *argument);

/* action on leds */
       void Led_LedOn        (u8  led_id);
       void Led_LedOff       (u8  led_id);
       void Led_LedBlinkStart(u8  led_id,
                              u8  blink_num,
                              u32 led_on,
                              u32 led_off,
                              u8  seq_num,
                              u32 seq_period);

/* action on timer */
static void Led_TimerBlinkDuration_Start(u8 led_id, u32 time_value, bool periodic);
static void Led_TimerBlinkDuration_Stop (u8 led_id);

/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* message callback functions */
static void Led_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data);

/* timer   callback functions */
static void Led_AdlCallback_Timer_TimerBlinkDuration(void *ptr_context);




/*=============================================================================
 * Function   : Led_TaskLed
 *
 * Description: led task
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Led_TaskLed(void *argument)
{
    LED__MAIL  message_received;
    QlOSStatus err;

    u8         i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - LED - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* init the leds status */
    for (i = 0; i < LED__NUMBER_OF_LEDS; i++)
    {
        DrvGpio_SetOutput(Led_LedGpioId[i], FALSE, 0);
    }


    /* creation of message queue "MessageQueueEvents" */
    err = ql_rtos_queue_create(&Led_MessageQueueHandler_MessageQueueEvents, sizeof(LED__MAIL), MESSAGE_QUEUE_SIZE);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_queue_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }

    /* creation of timer "TimerBlinkDuration" */
    for (i = 0; i < LED__NUMBER_OF_LEDS; i++)
    {
        err = ql_rtos_timer_create(&Led_TimerHandler_TimerBlinkDuration[i], QL_TIMER_IN_SERVICE, Led_AdlCallback_Timer_TimerBlinkDuration, (void *)&Led_LedId[i]);
        if (err != QL_OSI_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
    }


    for (;;)
    {
        err = ql_rtos_queue_wait(Led_MessageQueueHandler_MessageQueueEvents, (uint8 *)&message_received, sizeof(LED__MAIL), QL_WAIT_FOREVER);
        if (err == QL_OSI_SUCCESS)
        {
            /* process the received message */
            Led_AdlCallback_Message_TaskMsg(message_received.event, message_received.length_data, message_received.data);
        }
    }
}




/*=============================================================================
 * Function   : Led_LedOn
 *
 * Description: signal to turn on a led
 * Input      : - led_id: led to be turned on
 * Output     : -
 *=============================================================================*/
void Led_LedOn(u8 led_id)
{
    LED__MAIL  message_to_be_sent;
    QlOSStatus err;

    u8         i;


    if (led_id != LED__LED_GSM_GREEN)
    {
        return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__LED_ON;
    message_to_be_sent.length_data = 1;
    message_to_be_sent.data[0]     = led_id;
    for (i = 1; i < LED__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(Led_MessageQueueHandler_MessageQueueEvents, sizeof(LED__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Led_LedOff
 *
 * Description: signal to turn off a led
 * Input      : - led_id: led to be turned off
 * Output     : -
 *=============================================================================*/
void Led_LedOff(u8 led_id)
{
    LED__MAIL  message_to_be_sent;
    QlOSStatus err;

    u8         i;


    if (led_id != LED__LED_GSM_GREEN)
    {
        return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__LED_OFF;
    message_to_be_sent.length_data = 1;
    message_to_be_sent.data[0]     = led_id;
    for (i = 1; i < LED__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(Led_MessageQueueHandler_MessageQueueEvents, sizeof(LED__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Led_PlayBlinkMode
 *
 * Description: signal to start to blink a led
 * Input      : - led_id    : led to be started to blink
 *              - blink_num : blinks   - number of blinks
 *              - led_on    : blinks   - duration of led on                 (ms)
 *              - led_off   : blinks   - duration of led off between blinks (ms)
 *              - seq_num   : sequence - number of sequence                 (0: forever)
 *              - seq_period: sequence - sequence period                    (ms)
 * Output     : -
 *=============================================================================*/
void Led_LedBlinkStart(u8  led_id,
                       u8  blink_num,
                       u32 led_on,
                       u32 led_off,
                       u8  seq_num,
                       u32 seq_period)
{
    LED__MAIL  message_to_be_sent;
    QlOSStatus err;


    if (led_id != LED__LED_GSM_GREEN)
    {
        return;
    }


    message_to_be_sent.event       = TASK_MSG_ID__LED_BLINK_START;
    message_to_be_sent.length_data = 15;
    message_to_be_sent.data[ 0]    =       led_id;
    message_to_be_sent.data[ 1]    =       blink_num;
    message_to_be_sent.data[ 2]    = (u8)((led_on     >> 24) & 0x000000FF);
    message_to_be_sent.data[ 3]    = (u8)((led_on     >> 16) & 0x000000FF);
    message_to_be_sent.data[ 4]    = (u8)((led_on     >>  8) & 0x000000FF);
    message_to_be_sent.data[ 5]    = (u8)((led_on          ) & 0x000000FF);
    message_to_be_sent.data[ 6]    = (u8)((led_off    >> 24) & 0x000000FF);
    message_to_be_sent.data[ 7]    = (u8)((led_off    >> 16) & 0x000000FF);
    message_to_be_sent.data[ 8]    = (u8)((led_off    >>  8) & 0x000000FF);
    message_to_be_sent.data[ 9]    = (u8)((led_off         ) & 0x000000FF);
    message_to_be_sent.data[10]    =       seq_num;
    message_to_be_sent.data[11]    = (u8)((seq_period >> 24) & 0x000000FF);
    message_to_be_sent.data[12]    = (u8)((seq_period >> 16) & 0x000000FF);
    message_to_be_sent.data[13]    = (u8)((seq_period >>  8) & 0x000000FF);
    message_to_be_sent.data[14]    = (u8)((seq_period      ) & 0x000000FF);

    err = ql_rtos_queue_release(Led_MessageQueueHandler_MessageQueueEvents, sizeof(LED__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL_OSI_SUCCESS ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL_OSI_SUCCESS OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Led_TimerBlinkDuration_Start
 *
 * Description: start the "blink duration" timer
 * Input      : - led_id    : led ID
 *              - time_value: timeout value (ms)
 *              - periodic  : timer type indication
 *                            - FALSE: one shot timer
 *                            - TRUE : periodic timer
 * Output     : -
 *=============================================================================*/
static void Led_TimerBlinkDuration_Start(u8 led_id, u32 time_value, bool periodic)
{
    QlOSStatus err;


    if (led_id != LED__LED_GSM_GREEN)
    {
        return;
    }


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "Start \"blink duration\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* start the timer */
    err = ql_rtos_timer_start(Led_TimerHandler_TimerBlinkDuration[led_id], time_value, periodic);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_start - OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*=============================================================================
 * Function   : Led_TimerBlinkDuration_Stop
 *
 * Description: stop the "blink duration" timer
 * Input      : - led_id: led ID
 * Output     : -
 *=============================================================================*/
static void Led_TimerBlinkDuration_Stop(u8 led_id)
{
    QlOSStatus err;


    if (led_id != LED__LED_GSM_GREEN)
    {
        return;
    }


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "Stop \"blink duration\" timer", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    /* stop the timer (if enabled) */
    err = ql_rtos_timer_stop(Led_TimerHandler_TimerBlinkDuration[led_id]);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(Led_DebugString, sizeof(Led_DebugString), "QL FUNCTION - ql_rtos_timer_stop ERROR %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, Led_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_stop - OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : Led_AdlCallback_Message_TaskMsg
 *
 * Description: message callback function for task
 * Inputs     : -
 * Outputs    : -
 *===========================================================================*/
static void Led_AdlCallback_Message_TaskMsg(u32 msg_identifier, u32 length, u8 *ptr_data)
{
    /* SEM status and SEM event */
    static       u8     sem_status[LED__NUMBER_OF_LEDS] = {SEM_STATUS__LED_OFF};
                 u32    sem_event;

    /* led ID */
                 u8     led_id;

    /* number of blinks and sequences played */
    static       u8     num_blinks_played   [LED__NUMBER_OF_LEDS] = {0};
    static       u8     num_sequences_played[LED__NUMBER_OF_LEDS] = {0};

    /* blink mode parameters */
    static       u8     blink_num  [LED__NUMBER_OF_LEDS];
    static       u32    led_on     [LED__NUMBER_OF_LEDS];
    static       u32    led_off    [LED__NUMBER_OF_LEDS];
    static       u8     seq_num    [LED__NUMBER_OF_LEDS];
    static       u32    seq_period [LED__NUMBER_OF_LEDS];

    static       u32    seq_silence[LED__NUMBER_OF_LEDS];


    /* SEM status strings */
    static const ascii *sem_status_strings[] =
    {
        SEM_STATUS_STRING__LED_OFF,
        SEM_STATUS_STRING__LED_ON,
        SEM_STATUS_STRING__LED_BLINK_ON,
        SEM_STATUS_STRING__LED_BLINK_OFF,
    };

    /* SEM event strings */
    static const ascii *sem_event_strings[] =
    {
        SEM_EVENT_STRING__LED_ON,
        SEM_EVENT_STRING__LED_OFF,
        SEM_EVENT_STRING__LED_BLINK_START,
        SEM_EVENT_STRING__TIMEOUT_BLINK_DURATION,
    };


    snprintf(Led_DebugString, sizeof(Led_DebugString), "CALLBACK     - MESSAGE     - TASK LED - msg identifier: %lu, length: %lu", msg_identifier, length);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, Led_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* check the data length */
    switch (msg_identifier)
    {
        /* led blink start */
        case TASK_MSG_ID__LED_BLINK_START:
            if (length != 15)
                return;
            break;

        /* led on                 */
        /* led off                */
        /* timeout blink duration */
        case TASK_MSG_ID__LED_ON:
        case TASK_MSG_ID__LED_OFF:
        case TASK_MSG_ID__TIMEOUT_BLINK_DURATION:
            if (length != 1)
                return;
            break;

        /* other events or unknown event */
        default:
            return;
    }


    switch (msg_identifier)
    {
        case TASK_MSG_ID__LED_ON:
            sem_event = SEM_EVENT__LED_ON;
            break;

        case TASK_MSG_ID__LED_OFF:
            sem_event = SEM_EVENT__LED_OFF;
            break;

        case TASK_MSG_ID__LED_BLINK_START:
            sem_event = SEM_EVENT__LED_BLINK_START;
            break;

        case TASK_MSG_ID__TIMEOUT_BLINK_DURATION:
            sem_event = SEM_EVENT__TIMEOUT_BLINK_DURATION;
            break;

        default:
            return;
    }


    /* extract led ID */
    led_id = *ptr_data;


    snprintf(Led_DebugString, sizeof(Led_DebugString), "led_id: %d - SEM status: %s", led_id, sem_status_strings[sem_status[led_id]]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, Led_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    snprintf(Led_DebugString, sizeof(Led_DebugString), "led_id: %d - SEM event : %s", led_id, sem_event_strings [sem_event         ]);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, Led_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    switch (sem_event)
    {
        /* SEM event - led on */
        case SEM_EVENT__LED_ON:
            Led_TimerBlinkDuration_Stop(led_id);
            DrvGpio_SetOutput(Led_LedGpioId[led_id], TRUE , 0);
            sem_status[led_id] = SEM_STATUS__LED_ON;
            break;



        /* SEM event - led off */
        case SEM_EVENT__LED_OFF:
            Led_TimerBlinkDuration_Stop(led_id);
            DrvGpio_SetOutput(Led_LedGpioId[led_id], FALSE, 0);
            sem_status[led_id] = SEM_STATUS__LED_OFF;
            break;



        /* SEM event - led blink start */
        case SEM_EVENT__LED_BLINK_START:
            blink_num [led_id] =       *(ptr_data +  1);
            led_on    [led_id] = ((u32)*(ptr_data +  2) << 24) |
                                 ((u32)*(ptr_data +  3) << 16) |
                                 ((u32)*(ptr_data +  4) <<  8) |
                                 ((u32)*(ptr_data +  5)      );
            led_off   [led_id] = ((u32)*(ptr_data +  6) << 24) |
                                 ((u32)*(ptr_data +  7) << 16) |
                                 ((u32)*(ptr_data +  8) <<  8) |
                                 ((u32)*(ptr_data +  9)      );
            seq_num   [led_id] =       *(ptr_data + 10);
            seq_period[led_id] = ((u32)*(ptr_data + 11) << 24) |
                                 ((u32)*(ptr_data + 12) << 16) |
                                 ((u32)*(ptr_data + 13) <<  8) |
                                 ((u32)*(ptr_data + 14)      );

            if (seq_period[led_id]  >  (((blink_num[led_id] - 1) * (led_on[led_id] + led_off[led_id])) + led_on[led_id]))
                seq_silence[led_id] = seq_period[led_id] - (((blink_num[led_id] - 1) * (led_on[led_id] + led_off[led_id])) + led_on[led_id]);
            else
                seq_silence[led_id] = 100;

            snprintf(Led_DebugString,
                     sizeof(Led_DebugString),
                     "led_id: %d - blink_num: %d, led_on: %ld, led_off: %ld, seq_num: %d, seq_period: %ld",
                     led_id,
                     blink_num [led_id],
                     led_on    [led_id],
                     led_off   [led_id],
                     seq_num   [led_id],
                     seq_period[led_id]);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, Led_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            if ((blink_num[led_id] > 0) && (led_on[led_id] > 0))
            {
                /* parameters consistent */

                Led_TimerBlinkDuration_Stop(led_id);

                num_sequences_played[led_id] = 0;
                num_blinks_played   [led_id] = 0;

                /* start a blink on the led */
                DrvGpio_SetOutput(Led_LedGpioId[led_id], TRUE , 0);

                /* start the blink duration timer */
                Led_TimerBlinkDuration_Start(led_id, led_on[led_id], FALSE);

                sem_status[led_id] = SEM_STATUS__LED_BLINK_ON;
            }
            else
            {
                /* parameters not consistent */

                // NOTHING TO DO
            }
            break;



        /* SEM event - timeout blink duration */
        case SEM_EVENT__TIMEOUT_BLINK_DURATION:
            if (sem_status[led_id] == SEM_STATUS__LED_BLINK_ON)
            {
                /* led status: led blink - led on */

                /* stop to blink on the led */
                DrvGpio_SetOutput(Led_LedGpioId[led_id], FALSE, 0);

                num_blinks_played[led_id]++;

                if (num_blinks_played[led_id] < blink_num[led_id])
                {
                    /* blink sequence not yet terminated */

                    /* start the blink duration timer (for led_off) */
                    Led_TimerBlinkDuration_Start(led_id, led_off[led_id], FALSE);

                    sem_status[led_id] = SEM_STATUS__LED_BLINK_OFF;
                }
                else
                {
                    /* blink sequence terminated */

                    num_sequences_played[led_id]++;
                    num_blinks_played   [led_id] = 0;

                    if (
                         (seq_num             [led_id] == 0              ) ||
                         (num_sequences_played[led_id] <  seq_num[led_id])
                       )
                    {
                        /* sequence not yet terminated */

                        /* start the blink duration timer (for seq_silence) */
                        Led_TimerBlinkDuration_Start(led_id, seq_silence[led_id], FALSE);

                        sem_status[led_id] = SEM_STATUS__LED_BLINK_OFF;
                    }
                    else
                    {
                        /* sequence terminated */

                        sem_status[led_id] = SEM_STATUS__LED_OFF;
                    }
                }
            }
            else
            {
                /* led status: led blink - led off */

                /* start to blink on the led */
                DrvGpio_SetOutput(Led_LedGpioId[led_id], TRUE, 0);

                /* start the blink duration timer */
                Led_TimerBlinkDuration_Start(led_id, led_on[led_id], FALSE);

                sem_status[led_id] = SEM_STATUS__LED_BLINK_ON;
            }
            break;



        /* other events or unknown event */
        default:
            break;
    }
}




/*=============================================================================
 * Function   : Led_AdlCallback_Timer_TimerBlinkDuration
 *
 * Description: timer for blink duration
 * Input      : - ptr_context:
 * Output     : -
 *=============================================================================*/
static void Led_AdlCallback_Timer_TimerBlinkDuration(void *ptr_context)
{
    LED__MAIL  message_to_be_sent;
    QlOSStatus err;

    u8         led_id;
    u8         i;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER       - BLINK DURATION", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    led_id = *((u8 *)ptr_context);


    message_to_be_sent.event       = TASK_MSG_ID__TIMEOUT_BLINK_DURATION;
    message_to_be_sent.length_data = 1;
    message_to_be_sent.data[0]     = led_id;
    for (i = 1; i < LED__MAIL_SIZE; i++)
    {
        message_to_be_sent.data[i] = 0x00;
    }

    err = ql_rtos_queue_release(Led_MessageQueueHandler_MessageQueueEvents, sizeof(LED__MAIL), (uint8 *)&message_to_be_sent, QL_NO_WAIT);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_LED, DEBUG_TRACE_TYPE_LOW, "ql_rtos_queue_release OK"   , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}
