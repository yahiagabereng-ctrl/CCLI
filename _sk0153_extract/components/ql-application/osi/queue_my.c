/*=============================================================================
 * File       :  QUEUE.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  PROGRAM - transmitting queues
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*===========================================================================
 * INCLUDES
 *===========================================================================*/
/* user     includes */
#include "debug_my.h"
#include "drvtemperature.h"
#include "program_flash.h"   //@@@
#include "queue_my.h"
#include "reset.h"
#include "sms_answer_event.h"
#include "startup.h"
#include "typedef.h"
#include "user_gsm_retx.h"
#include "utility.h"




/*===========================================================================
 * DEFINES
 *===========================================================================*/
/* queue descriptions */
#define QUEUE_DESCRIPTION__FW_UPGRADE_QUEUE             "FW UPGRADE QUEUE      "    // FW upgrade        queue
#define QUEUE_DESCRIPTION__ALARM_QUEUE                  "ALARM QUEUE           "    // alarm             queue
#define QUEUE_DESCRIPTION__SMS_ANSWER_QUEUE             "SMS ANSWER QUEUE      "    // SMS answer        queue
#define QUEUE_DESCRIPTION__SMS_ANSWER_TO_CRS_QUEUE      "SMS ANSWER TO CRS QUEUE"   // SMS answer to CRS queue
#define QUEUE_DESCRIPTION__SMS_FORWARD_QUEUE            "SMS FORWARD QUEUE     "    // SMS forward       queue
#define QUEUE_DESCRIPTION__AUTOSYNCHRONIZE_QUEUE        "AUTOSYNCHRONIZE QUEUE "    // autosynchronize   queue
#define QUEUE_DESCRIPTION__SMS_ANSWER_EVENT_QUEUE       "SMS ANSWER EVENT QUEUE"    // SMS answer event  queue
#define QUEUE_DESCRIPTION__FILE_STATUS_LOG_QUEUE        "FILE STATUS LOG QUEUE "    // file status log   queue
#define QUEUE_DESCRIPTION__FILE_ALARM_QUEUE             "FILE ALARM QUEUE      "    // file alarm        queue

/* debug string length */
#define MAX_LENGTH_DEBUG_STRING                         200




/*===========================================================================
 * VARIABLES
 *===========================================================================*/
/* queues (FIFO) */
static       QUEUE__FW_UPGRADE_QUEUE        Queue_QueueFwUpgrade;                                                          // FW upgrade       queue  (stored in flash)
static       QUEUE__ALARM_QUEUE             Queue_QueueAlarm;                                                  // = ...;   // alarm            queue
static       QUEUE__SMS_ANSWER_QUEUE        Queue_QueueSmsAnswer;                                              // = ...;   // SMS answer       queue
static       QUEUE__SMS_ANSWER_TO_CRS_QUEUE Queue_QueueSmsAnswerToCrs;                                                     // SMS answer to CRS queue
static       QUEUE__SMS_FORWARD_QUEUE       Queue_QueueSmsForward;                                             // = ...;   // SMS forward      queue
static       QUEUE__AUTOSYNCHRONIZE_QUEUE   Queue_QueueAutosynchronize;                                        // = ...;   // autosynchronize  queue
static       QUEUE__SMS_ANSWER_EVENT_QUEUE  Queue_QueueSmsAnswerEvent;                                         // = ...;   // SMS answer event queue
static       QUEUE__FILE_STATUS_LOG_QUEUE   Queue_QueueFileStatusLog;                                                      // file status log  queue  (stored in flash)
static       QUEUE__FILE_ALARM_QUEUE        Queue_QueueFileAlarm;                                                          // file alarm       queue  (stored in flash)

/* CRC variables */
static       u16                            Queue_VarCrc;                                                      // = 0x0000;

/* FW upgrade queue default */
static const QUEUE__FW_UPGRADE_QUEUE        Queue_QueueFwUpgradeDefault =
{
    0,  // start   pointer (SP)
    0,  // reading pointer (RP)
    0,  // writing pointer (WP)

    // queue buffer
    {
    //     SMS DOTA phone number
    //     |      year           hour         week_day
    //     |      |     month    |  minute    |
    //     |      |     |  day   |  |  second |     FW upgrade result             FW upgrade error code
    //     |      |     |  |     |  |  |      |     |                             |
    //     |      |     |  |     |  |  |      |     |                             |
        {  "",   {2000, 1, 1,    0, 0, 0,     6},   RESET__DOTA_RESULT__UNKNOWN,  RESET__DOTA_RESULT__ERROR__UNKNOWN   },   //  1
        {  "",   {2000, 1, 1,    0, 0, 0,     6},   RESET__DOTA_RESULT__UNKNOWN,  RESET__DOTA_RESULT__ERROR__UNKNOWN   },   //  2
        {  "",   {2000, 1, 1,    0, 0, 0,     6},   RESET__DOTA_RESULT__UNKNOWN,  RESET__DOTA_RESULT__ERROR__UNKNOWN   },   //  3
        {  "",   {2000, 1, 1,    0, 0, 0,     6},   RESET__DOTA_RESULT__UNKNOWN,  RESET__DOTA_RESULT__ERROR__UNKNOWN   },   //  4

        {  "",   {2000, 1, 1,    0, 0, 0,     6},   RESET__DOTA_RESULT__UNKNOWN,  RESET__DOTA_RESULT__ERROR__UNKNOWN   },   //  5
    }
};


/* SMS answer to CRS queue default */
static const QUEUE__SMS_ANSWER_TO_CRS_QUEUE Queue_QueueSmsAnswerToCrsDefault =
{
    0,  // start   pointer (SP)
    0,  // reading pointer (RP)
    0,  // writing pointer (WP)

    // queue buffer
    {
    //     SMS recipient phone number
    //     |
    //     |
    //     |
    //     |      year           hour         week_day
    //     |      |     month    |  minute    |
    //     |      |     |  day   |  |  second |
    //     |      |     |  |     |  |  |      |
    //     |      |     |  |     |  |  |      |      answer_text
    //     |      |     |  |     |  |  |      |      |
        {  "",   {2000, 1, 1,    0, 0, 0,     6},    ""   },   // 1
        {  "",   {2000, 1, 1,    0, 0, 0,     6},    ""   },   // 2
        {  "",   {2000, 1, 1,    0, 0, 0,     6},    ""   },   // 3
        {  "",   {2000, 1, 1,    0, 0, 0,     6},    ""   },   // 4

        {  "",   {2000, 1, 1,    0, 0, 0,     6},    ""   },   // 5
    }
};


/* file status log queue default */
static const QUEUE__FILE_STATUS_LOG_QUEUE   Queue_QueueFileStatusLogDefault =
{
    0,  // start   pointer (SP)
    0,  // reading pointer (RP)
    0,  // writing pointer (WP)

    // queue buffer
    {
    //                    time             log file name                   start time                            stop time
    //                     |                    |   log file version            |                                    |
    //                     |                    |     |                         |                                    |
    //                     |                    |     |  log file number        |                                    |
    //     ________________|________________    |     |    |    ________________|________________    ________________|________________
    //     |                               |    |     |    |    |                               |    |                               |
    //     | year           hour         week_day     |    |    | year           hour         week_day year           hour         week
    //     | |     month    |  minute    | |    |     |    |    | |     month    |  minute    | |    | |     month    |  minute    | |
    //     | |     |  day   |  |  second | |    |     |    |    | |     |  day   |  |  second | |    | |     |  day   |  |  second | |
    //     | |     |  |     |  |  |      | |    |     |    |    | |     |  |     |  |  |      | |    | |     |  |     |  |  |      | |
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  1
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  2
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  3
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  4
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  5
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  6
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  7
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  8
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  9
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  10

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  11
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  12
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  13
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  14
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  15
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  16
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  17
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  18
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  19
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  20

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  21
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  22
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  23
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  24
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  25
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  26
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  27
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  28
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  29
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  30

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  31
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  32
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  33
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  34
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  35
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  36
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  37
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  38
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  39
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  40

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  41
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  42
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  43
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  44
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  45
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  46
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  47
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  48

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },  },   //  49
    }
};




/* file alarm queue default */
static const QUEUE__FILE_ALARM_QUEUE        Queue_QueueFileAlarmDefault =
{
    0,  // start   pointer (SP)
    0,  // reading pointer (RP)
    0,  // writing pointer (WP)

    // queue buffer
    {
    //                    time             alarm file name                 start time                            stop time                alarm type ID
    //                     |                    |   alarm file version          |                                    |                    |    alarm parameter
    //                     |                    |     |                         |                                    |                    |    |
    //                     |                    |     |  alarm file number      |                                    |                    |    |
    //     ________________|________________    |     |    |    ________________|________________    ________________|________________    |    |
    //     |                               |    |     |    |    |                               |    |                               |    |    |
    //     | year           hour         week_day     |    |    | year           hour         week_day year           hour         week   |    |
    //     | |     month    |  minute    | |    |     |    |    | |     month    |  minute    | |    | |     month    |  minute    | |    |    |
    //     | |     |  day   |  |  second | |    |     |    |    | |     |  day   |  |  second | |    | |     |  day   |  |  second | |    |    |
    //     | |     |  |     |  |  |      | |    |     |    |    | |     |  |     |  |  |      | |    | |     |  |     |  |  |      | |    |    |
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  1
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  2
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  3
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  4
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  5
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  6
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  7
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  8
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  9
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  10

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  11
        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  12

        {  { 2000, 1, 1,    0, 0, 0,     6 },   "",   0,   0,   { 2000, 1, 1,    0, 0, 0,     6 },   { 2000, 1, 1,    0, 0, 0,     6 },   0,   0  },   //  13
    }
};


/* debug string */
//static       ascii                         Queue_DebugString[MAX_LENGTH_DEBUG_STRING + 1];




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* init */
       void   Queue_Init(void);

/* status variables */
static void   Queue_InitVar        (void);
static u16    Queue_CalculateVarCrc(void);
       void   Queue_UpdateVarCrc   (void);
       bool   Queue_VerifyVarCrc   (void);


/*-----------------------------------------------------------------------------
 * Read queue description
 *-----------------------------------------------------------------------------*/
/* FW upgrade        queue - read queue description */
       ascii *Queue_FwUpgrade_ReadQueueDescription      (void);

/* alarm             queue - read queue description */
       ascii *Queue_Alarm_ReadQueueDescription          (void);

/* SMS answer        queue - read queue description */
       ascii *Queue_SmsAnswer_ReadQueueDescription      (void);

/* SMS answer to CRS queue - read queue description */
       ascii *Queue_SmsAnswerToCrs_ReadQueueDescription (void);

/* SMS forward       queue - read queue description */
       ascii *Queue_SmsForward_ReadQueueDescription     (void);

/* autosynchronize   queue - read queue description */
       ascii *Queue_Autosynchronize_ReadQueueDescription(void);

/* SMS answer event  queue - read queue description */
       ascii *Queue_SmsAnswerEvent_ReadQueueDescription (void);

/* file status log   queue - read queue description */
       ascii *Queue_FileStatusLog_ReadQueueDescription   (void);

/* file alarm        queue - read queue description */
       ascii *Queue_FileAlarm_ReadQueueDescription       (void);


/*-----------------------------------------------------------------------------
 * Put/get records
 *-----------------------------------------------------------------------------*/
/* FW upgrade       queue - get/set queue */
       bool   Queue_FwUpgrade_PutRecord      (QUEUE__FW_UPGRADE_QUEUE_RECORD        *ptr_record);
       bool   Queue_FwUpgrade_GetRecord      (QUEUE__FW_UPGRADE_QUEUE_RECORD        *ptr_record);

/* alarm            queue - put/get */
       bool   Queue_Alarm_PutRecord          (QUEUE__ALARM_QUEUE_RECORD             *ptr_record);
       bool   Queue_Alarm_GetRecord          (QUEUE__ALARM_QUEUE_RECORD             *ptr_record);

/* SMS answer       queue - put/get */
       bool   Queue_SmsAnswer_PutRecord      (QUEUE__SMS_ANSWER_QUEUE_RECORD        *ptr_record);
       bool   Queue_SmsAnswer_GetRecord      (QUEUE__SMS_ANSWER_QUEUE_RECORD        *ptr_record);

/* SMS answer to CRS   queue - put/get */
       bool   Queue_SmsAnswerToCrs_PutRecord (QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_record);
       bool   Queue_SmsAnswerToCrs_GetRecord (QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_record);

/* SMS forward      queue - put/get */
       bool   Queue_SmsForward_PutRecord     (QUEUE__SMS_FORWARD_QUEUE_RECORD       *ptr_record);
       bool   Queue_SmsForward_GetRecord     (QUEUE__SMS_FORWARD_QUEUE_RECORD       *ptr_record);

/* autosynchronize  queue - get/set queue */
       bool   Queue_Autosynchronize_PutRecord(QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD   *ptr_record);
       bool   Queue_Autosynchronize_GetRecord(QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD   *ptr_record);

/* SMS answer event queue - get/set queue */
       bool   Queue_SmsAnswerEvent_PutRecord (QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD  *ptr_record);
       bool   Queue_SmsAnswerEvent_GetRecord (QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD  *ptr_record);

/* file status log  queue - get/set queue */
       bool   Queue_FileStatusLog_PutRecord  (QUEUE__FILE_STATUS_LOG_QUEUE_RECORD   *ptr_record);
       bool   Queue_FileStatusLog_GetRecord  (QUEUE__FILE_STATUS_LOG_QUEUE_RECORD   *ptr_record);

/* file alarm       queue - get/set queue */
       bool   Queue_FileAlarm_PutRecord      (QUEUE__FILE_ALARM_QUEUE_RECORD        *ptr_record);
       bool   Queue_FileAlarm_GetRecord      (QUEUE__FILE_ALARM_QUEUE_RECORD        *ptr_record);


/*-----------------------------------------------------------------------------
 * Delete/recover records
 *-----------------------------------------------------------------------------*/
/* FW upgrade      queue - delete/recover */
       void   Queue_FwUpgrade_DeleteRecords          (void);
       void   Queue_FwUpgrade_DeleteRecordsGot       (void);
       void   Queue_FwUpgrade_RecoverRecordsGot      (void);

/* alarm           queue - delete/recover */
       void   Queue_Alarm_DeleteRecords              (void);
       void   Queue_Alarm_DeleteRecordsGot           (void);
       void   Queue_Alarm_RecoverRecordsGot          (void);

/* SMS answer      queue - delete/recover */
       void   Queue_SmsAnswer_DeleteRecords          (void);
       void   Queue_SmsAnswer_DeleteRecordsGot       (void);
       void   Queue_SmsAnswer_RecoverRecordsGot      (void);

/* SMS answer to CRS   queue - delete/recover */
       void   Queue_SmsAnswerToCrs_DeleteRecords     (void);
       void   Queue_SmsAnswerToCrs_DeleteRecordsGot  (void);
       void   Queue_SmsAnswerToCrs_RecoverRecordsGot (void);

/* SMS forward     queue - delete/recover */
       void   Queue_SmsForward_DeleteRecords         (void);
       void   Queue_SmsForward_DeleteRecordsGot      (void);
       void   Queue_SmsForward_RecoverRecordsGot     (void);

/* autosynchronize queue - delete/recover */
       void   Queue_Autosynchronize_DeleteRecords    (void);
       void   Queue_Autosynchronize_DeleteRecordsGot (void);
       void   Queue_Autosynchronize_RecoverRecordsGot(void);

/* SMS answer event queue - delete/recover */
       void   Queue_SmsAnswerEvent_DeleteRecords     (void);
       void   Queue_SmsAnswerEvent_DeleteRecordsGot  (void);
       void   Queue_SmsAnswerEvent_RecoverRecordsGot (void);

/* file status log  queue - delete/recover */
       void   Queue_FileStatusLog_DeleteRecords      (void);
       void   Queue_FileStatusLog_DeleteRecordsGot   (void);
       void   Queue_FileStatusLog_RecoverRecordsGot  (void);

/* file alarm       queue - delete/recover */
       void   Queue_FileAlarm_DeleteRecords          (void);
       void   Queue_FileAlarm_DeleteRecordsGot       (void);
       void   Queue_FileAlarm_RecoverRecordsGot      (void);


/*-----------------------------------------------------------------------------
 * Number of records
 *-----------------------------------------------------------------------------*/
/* FW upgrade      queue - number of records */
       u8     Queue_FwUpgrade_NumRecords      (void);

/* alarm           queue - number of records */
       u8     Queue_Alarm_NumRecords          (void);

/* SMS answer      queue - number of records */
       u8     Queue_SmsAnswer_NumRecords      (void);

/* SMS answer to CRS   queue - number of records */
       u8     Queue_SmsAnswerToCrs_NumRecords (void);

/* SMS forward     queue - number of records */
       u8     Queue_SmsForward_NumRecords     (void);

/* autosynchronize queue - number of records */
       u8     Queue_Autosynchronize_NumRecords(void);

/* SMS answer event queue - number of records */
       u8     Queue_SmsAnswerEvent_NumRecords (void);

/* file status log  queue - number of records */
       u8     Queue_FileStatusLog_NumRecords  (void);

/* file alarm       queue - number of records */
       u8     Queue_FileAlarm_NumRecords      (void);


/*-----------------------------------------------------------------------------
 * Get/set queue
 *-----------------------------------------------------------------------------*/
/* FW upgrade      queue - get/set queue */
       void   Queue_FwUpgradeQueue_GetDefault     (QUEUE__FW_UPGRADE_QUEUE        *ptr_data);
       void   Queue_FwUpgradeQueue_Get            (QUEUE__FW_UPGRADE_QUEUE        *ptr_data);
       void   Queue_FwUpgradeQueue_Set            (QUEUE__FW_UPGRADE_QUEUE        *ptr_data);

/* SMS answer to CRS   queue - get/set queue */
       void   Queue_SmsAnswerToCrsQueue_GetDefault(QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data);
       void   Queue_SmsAnswerToCrsQueue_Get       (QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data);
       void   Queue_SmsAnswerToCrsQueue_Set       (QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data);

/* file status log queue - get/set queue */
       void   Queue_FileStatusLogQueue_GetDefault (QUEUE__FILE_STATUS_LOG_QUEUE   *ptr_data);
       void   Queue_FileStatusLogQueue_Get        (QUEUE__FILE_STATUS_LOG_QUEUE   *ptr_data);
       void   Queue_FileStatusLogQueue_Set        (QUEUE__FILE_STATUS_LOG_QUEUE   *ptr_data);

/* file alarm       queue - get/set queue */
       void   Queue_FileAlarmQueue_GetDefault     (QUEUE__FILE_ALARM_QUEUE        *ptr_data);
       void   Queue_FileAlarmQueue_Get            (QUEUE__FILE_ALARM_QUEUE        *ptr_data);
       void   Queue_FileAlarmQueue_Set            (QUEUE__FILE_ALARM_QUEUE        *ptr_data);



/*=============================================================================
 * Function   : Queue_Init
 *
 * Description: init
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Init(void)
{
    /* internal status init */
    Queue_InitVar();
}




/*=============================================================================
 * Function   : Queue_InitVar
 *
 * Description: init the variables and the module status
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Queue_InitVar(void)
{
    bool recover_status;

    u8   i;
    u8   j;


    /* get the recover status */
    recover_status = Startup_GetRecoverStatus();
    if (!recover_status)
    {
        /* startup without recover of the internal status (variables with default   values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_QUEUE, DEBUG_TRACE_TYPE_LOW, "Start with default values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/

        /* FW upgrade       queue */    // stored in flash
      //Queue_QueueFwUpgrade.ptr_start = 0;    // start   pointer (SP)
      //Queue_QueueFwUpgrade.ptr_rd    = 0;    // reading pointer (RP)
      //Queue_QueueFwUpgrade.ptr_wr    = 0;    // writing pointer (WP)
      //for (i = 0; i < (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1); i++)
      //{
      //    /* phone number */
      //    Queue_QueueFwUpgrade.records [i].sms_phone_number[0]                     = 0x00;                                        /* SMS DOTA phone number */
      //
      //    /* time */
      //    Queue_QueueFwUpgrade.records [i].time.year                               = 2000;
      //    Queue_QueueFwUpgrade.records [i].time.month                              = 1;
      //    Queue_QueueFwUpgrade.records [i].time.day                                = 1;
      //    Queue_QueueFwUpgrade.records [i].time.hour                               = 0;
      //    Queue_QueueFwUpgrade.records [i].time.minute                             = 0;
      //    Queue_QueueFwUpgrade.records [i].time.second                             = 0;
      //    Queue_QueueFwUpgrade.records [i].time.week_day                           = 6;
      //
      //    /* FW upgrade result */
      //    Queue_QueueFwUpgrade.records [i].result                                  = RESET__DOTA_RESULT__UNKNOWN;                 /* FW upgrade result     */
      //    Queue_QueueFwUpgrade.records [i].error_code                              = RESET__DOTA_RESULT__ERROR__UNKNOWN;          /* FW upgrade error code */
      //}


        /* alarm            queue */
        Queue_QueueAlarm.ptr_start = 0;    // start   pointer (SP)
        Queue_QueueAlarm.ptr_rd    = 0;    // reading pointer (RP)
        Queue_QueueAlarm.ptr_wr    = 0;    // writing pointer (WP)
        for (i = 0; i < (QUEUE__QUEUE_SIZE__ALARM + 1); i++)
        {
            /* alarm data */
            Queue_QueueAlarm.records     [i].alarm_data.alarm_id                     = 0;                                           /* alarm type ID */
            Queue_QueueAlarm.records     [i].alarm_data.alarm_par                    = 0;                                           /* alarm parameter (optional) */

            /* device status */

            /* time */
            Queue_QueueAlarm.records [i].device_status.time.year                     = 2000;
            Queue_QueueAlarm.records [i].device_status.time.month                    = 1;
            Queue_QueueAlarm.records [i].device_status.time.day                      = 1;
            Queue_QueueAlarm.records [i].device_status.time.hour                     = 0;
            Queue_QueueAlarm.records [i].device_status.time.minute                   = 0;
            Queue_QueueAlarm.records [i].device_status.time.second                   = 0;
            Queue_QueueAlarm.records [i].device_status.time.week_day                 = 6;

             /* power supply */
            Queue_QueueAlarm.records     [i].device_status.main_power_supply         = TRUE;                                        /* presence of main power supply */
            Queue_QueueAlarm.records     [i].device_status.battery_charge_status     = FALSE;                                       /* battery charge status         */
            Queue_QueueAlarm.records     [i].device_status.gsm_module_power_supply   = 3800;                                        /* GSM module power supply (mV)  */

            /* temperatures */
            Queue_QueueAlarm.records     [i].device_status.sensor_status_int         = DRVTEMPERATURE__SENSOR__UNKNOWN;             /* internal sensor status        */
            Queue_QueueAlarm.records     [i].device_status.sensor_status_ext         = DRVTEMPERATURE__SENSOR__UNKNOWN;             /* external sensor status        */
            Queue_QueueAlarm.records     [i].device_status.temperature_int           = 0;                                           /* internal temperature (/10 °C) */
            Queue_QueueAlarm.records     [i].device_status.temperature_ext           = 0;                                           /* external temperature (/10 °C) */
            Queue_QueueAlarm.records     [i].device_status.adc_value_int             = 0;                                           /* internal ADC value            */
            Queue_QueueAlarm.records     [i].device_status.adc_value_ext             = 0;                                           /* external ADC value            */

            /* digital inputs */
            Queue_QueueAlarm.records     [i].device_status.in1                       = FALSE;                                       /* digital input  1 */
            Queue_QueueAlarm.records     [i].device_status.in2                       = FALSE;                                       /* digital input  2 */

            /* digital outputs */
            Queue_QueueAlarm.records     [i].device_status.out1_physical             = FALSE;                                       /* digital output 1 (         physical status) */
            Queue_QueueAlarm.records     [i].device_status.out2_physical             = FALSE;                                       /* digital output 2 (         physical status) */
            Queue_QueueAlarm.records     [i].device_status.out1_physical_expected    = FALSE;                                       /* digital output 1 (expected physical status) */
            Queue_QueueAlarm.records     [i].device_status.out2_physical_expected    = FALSE;                                       /* digital output 2 (expected physical status) */
            Queue_QueueAlarm.records     [i].device_status.out1_logical              = FALSE;                                       /* digital output 1 (         logical  status) */
            Queue_QueueAlarm.records     [i].device_status.out2_logical              = FALSE;                                       /* digital output 2 (         logical  status) */
        }


        /* SMS answer       queue */
        Queue_QueueSmsAnswer.ptr_start = 0;    // start   pointer (SP)
        Queue_QueueSmsAnswer.ptr_rd    = 0;    // reading pointer (RP)
        Queue_QueueSmsAnswer.ptr_wr    = 0;    // writing pointer (WP)
        for (i = 0; i < (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1); i++)
        {
            /* phone number */
            Queue_QueueSmsAnswer.records [i].sms_phone_number[0] = 0x00;                                                            /* SMS recipient phone number */

            /* time */
            Queue_QueueSmsAnswer.records [i].time.year                               = 2000;
            Queue_QueueSmsAnswer.records [i].time.month                              = 1;
            Queue_QueueSmsAnswer.records [i].time.day                                = 1;
            Queue_QueueSmsAnswer.records [i].time.hour                               = 0;
            Queue_QueueSmsAnswer.records [i].time.minute                             = 0;
            Queue_QueueSmsAnswer.records [i].time.second                             = 0;
            Queue_QueueSmsAnswer.records [i].time.week_day                           = 6;

            /* text */
            Queue_QueueSmsAnswer.records [i].answer_text[0]                          = 0x00;                                        /* answer text */
        }


        /* SMS forward      queue */
        Queue_QueueSmsForward.ptr_start = 0;    // start   pointer (SP)
        Queue_QueueSmsForward.ptr_rd    = 0;    // reading pointer (RP)
        Queue_QueueSmsForward.ptr_wr    = 0;    // writing pointer (WP)
        for (i = 0; i < (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1); i++)
        {
            /* time */
            Queue_QueueSmsForward.records[i].time.year                               = 2000;
            Queue_QueueSmsForward.records[i].time.month                              = 1;
            Queue_QueueSmsForward.records[i].time.day                                = 1;
            Queue_QueueSmsForward.records[i].time.hour                               = 0;
            Queue_QueueSmsForward.records[i].time.minute                             = 0;
            Queue_QueueSmsForward.records[i].time.second                             = 0;
            Queue_QueueSmsForward.records[i].time.week_day                           = 6;

            /* text */
            Queue_QueueSmsForward.records[i].forward_text[0]                         = 0x00;                                        /* forward text */
        }


        /* autosynchronize  queue */
        Queue_QueueAutosynchronize.ptr_start = 0;    // start   pointer (SP)
        Queue_QueueAutosynchronize.ptr_rd    = 0;    // reading pointer (RP)
        Queue_QueueAutosynchronize.ptr_wr    = 0;    // writing pointer (WP)
        for (i = 0; i < QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE; i++)
        {
            Queue_QueueAutosynchronize.records[i].sms_counter                        = 0;                                           /* synchronize SMS counter */
        }


        /* SMS answer event queue */
        Queue_QueueSmsAnswerEvent.ptr_start = 0;    // start   pointer (SP)
        Queue_QueueSmsAnswerEvent.ptr_rd    = 0;    // reading pointer (RP)
        Queue_QueueSmsAnswerEvent.ptr_wr    = 0;    // writing pointer (WP)
        for (i = 0; i < (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1); i++)
        {
            /* phone number */
            Queue_QueueSmsAnswerEvent.records [i].sms_phone_number[0]                = 0x00;                                        /* SMS recipient phone number */

            /* variables values */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.pod[0]            = 0x00;                                        /* POD */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.year         = 2000;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.month        = 1;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.day          = 1;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.hour         = 0;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.minute       = 0;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.second       = 0;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.time.week_day     = 6;
            Queue_QueueSmsAnswerEvent.records [i].variables_values.power             = TRUE;                                        /* presence of main power supply */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.in1               = FALSE;                                       /* digital input  IN1  status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.in2               = FALSE;                                       /* digital input  IN2  status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.out1              = FALSE;                                       /* digital output OUT1 status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.out2              = FALSE;                                       /* digital output OUT2 status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.sensor_status_int = DRVTEMPERATURE__SENSOR__UNKNOWN;             /* internal sensor status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.sensor_status_ext = DRVTEMPERATURE__SENSOR__UNKNOWN;             /* external sensor status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.tint              = 0;                                           /* internal temperature */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.text              = 0;                                           /* external temperature */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.creg              = 0;                                           /* GSM  network registration status */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.cgreg             = 0;                                           /* GPRS network registration status */
            for (j = 0; j < (32 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.cops0[j]      = 0;                                           /* GSM network operator - long  alphanumeric format */
            for (j = 0; j < (10 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.cops1[j]      = 0;                                           /* GSM network operator - short alphanumeric format */
            for (j = 0; j < ( 6 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.cops2[j]      = 0;                                           /* GSM network operator - numeric            format */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.rssi              = 99;                                          /* RSSI (received signal strength) */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.ber               = 99;                                          /* BER  (channel bit error rate  ) */
            for (j = 0; j < ( 3 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.mcc[j]        = 0;                                           /* MCC (Mobile Country Code) */
            for (j = 0; j < ( 3 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.mnc[j]        = 0;                                           /* MNC (Mobile Network Code) */
            for (j = 0; j < ( 4 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.lac[j]        = 0;                                           /* LAC (Location Area  Code) */
            for (j = 0; j < ( 4 + 1); j++)
                Queue_QueueSmsAnswerEvent.records [i].variables_values.ci [j]        = 0;                                           /* CI  (Cell Identifier) */
            Queue_QueueSmsAnswerEvent.records [i].variables_values.resetresult       = 0;                                           /* reset result */

            /* answer type */
            Queue_QueueSmsAnswerEvent.records [i].answer_type                        = SMS_ANSWER_EVENT__ANSWER_TYPE__DETACHMENT;   /* answer type */
        }


        /* status log       queue */    // stored in flash
      //Queue_QueueFileStatusLog.ptr_start = 0;    // start   pointer (SP)
      //Queue_QueueFileStatusLog.ptr_rd    = 0;    // reading pointer (RP)
      //Queue_QueueFileStatusLog.ptr_wr    = 0;    // writing pointer (WP)
      //for (i = 0; i < (QUEUE__QUEUE_SIZE__LOG_STATUS + 1); i++)
      //{
      //    /* time */
      //    Queue_QueueFileStatusLog.records [i].time.year                           = 2000;
      //    Queue_QueueFileStatusLog.records [i].time.month                          = 1;
      //    Queue_QueueFileStatusLog.records [i].time.day                            = 1;
      //    Queue_QueueFileStatusLog.records [i].time.hour                           = 0;
      //    Queue_QueueFileStatusLog.records [i].time.minute                         = 0;
      //    Queue_QueueFileStatusLog.records [i].time.second                         = 0;
      //    Queue_QueueFileStatusLog.records [i].time.week_day                       = 6;
      //
      //    /* log file name */
      //    for (j = 0; j < (60 + 1); j++)
      //        Queue_QueueFileStatusLog.records [i].log_file.[j]                    = 0x00;                                        /* log file name */
      //}


        /**** update the variable CRC ****/
        Queue_UpdateVarCrc();


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        // NOTHING TO DO
    }
    else
    {
        /* startup with    recover of the internal status (variables with recovered values) */

        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_QUEUE, DEBUG_TRACE_TYPE_LOW, "Start with recovered values", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /**** variables init ****/
        // NOTHING TO DO


        /**** update the variable CRC ****/
        // NOTHING TO DO


        /**** timer ****/
        // NOTHING TO DO


        /**** other ****/
        // NOTHING TO DO
    }
}




/*=============================================================================
 * Function   : Queue_CalculateVarCrc
 *
 * Description: calculate the variables CRC
 * Input      : -
 * Output     : - variables CRC calculated
 *=============================================================================*/
static u16 Queue_CalculateVarCrc(void)
{
    u16 crc;


    crc = 0x0000;

    /* variables */
  //crc = Utility_CalculateCRC16((u8 *)&Queue_QueueFwUpgrade      , sizeof(Queue_QueueFwUpgrade      ), crc);   // stored in flash
    crc = Utility_CalculateCRC16((u8 *)&Queue_QueueAlarm          , sizeof(Queue_QueueAlarm          ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Queue_QueueSmsAnswer      , sizeof(Queue_QueueSmsAnswer      ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Queue_QueueSmsForward     , sizeof(Queue_QueueSmsForward     ), crc);
    crc = Utility_CalculateCRC16((u8 *)&Queue_QueueAutosynchronize, sizeof(Queue_QueueAutosynchronize), crc);
    crc = Utility_CalculateCRC16((u8 *)&Queue_QueueSmsAnswerEvent , sizeof(Queue_QueueSmsAnswerEvent ), crc);
  //crc = Utility_CalculateCRC16((u8 *)&Queue_QueueFileStatusLog  , sizeof(Queue_QueueFileStatusLog  ), crc);   // stored in flash
  //crc = Utility_CalculateCRC16((u8 *)&Queue_QueueFileAlarm      , sizeof(Queue_QueueFileAlarm      ), crc);   // stored in flash

    return crc;
}




/*=============================================================================
 * Function   : Queue_UpdateVarCrc
 *
 * Description: update the variables CRC
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_UpdateVarCrc(void)
{
    Queue_VarCrc = Queue_CalculateVarCrc();
}




/*=============================================================================
 * Function   : Queue_VerifyVarCrc
 *
 * Description: verify the variables CRC
 * Input      : -
 * Output     : - FALSE: variables CRC not correct
 *              - TRUE : variables CRC     correct
 *=============================================================================*/
bool Queue_VerifyVarCrc(void)
{
    u16  crc_calculated;
    bool crc_ok;


    crc_calculated = Queue_CalculateVarCrc();

    if (Queue_VarCrc == crc_calculated)
        crc_ok = TRUE;
    else
        crc_ok = FALSE;

    if (!crc_ok)
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_QUEUE, DEBUG_TRACE_TYPE_LOW, "Variables CRC not correct", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    return crc_ok;
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_ReadQueueDescription
 *
 * Description: read the FW upgrade queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_FwUpgrade_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__FW_UPGRADE_QUEUE);
}




/*=============================================================================
 * Function   : Queue_Alarm_ReadQueueDescription
 *
 * Description: read the alarm queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_Alarm_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__ALARM_QUEUE);
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_ReadQueueDescription
 *
 * Description: read the SMS answer queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_SmsAnswer_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__SMS_ANSWER_QUEUE);
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_ReadQueueDescription
 *
 * Description: read the SMS answer to CRS queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_SmsAnswerToCrs_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__SMS_ANSWER_TO_CRS_QUEUE);
}




/*=============================================================================
 * Function   : Queue_SmsForward_ReadQueueDescription
 *
 * Description: read the SMS forward queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_SmsForward_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__SMS_FORWARD_QUEUE);
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_ReadQueueDescription
 *
 * Description: read the autosynchronize queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_Autosynchronize_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__AUTOSYNCHRONIZE_QUEUE);
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_ReadQueueDescription
 *
 * Description: read the SMS answer event queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_SmsAnswerEvent_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__SMS_ANSWER_EVENT_QUEUE);
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_ReadQueueDescription
 *
 * Description: read the file status log queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_FileStatusLog_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__FILE_STATUS_LOG_QUEUE);
}




/*=============================================================================
 * Function   : Queue_FileAlarm_ReadQueueDescription
 *
 * Description: read the file alarm queue description
 * Input      : -
 * Output     : - pointer to queue description
 *=============================================================================*/
ascii *Queue_FileAlarm_ReadQueueDescription(void)
{
    return ((ascii *)QUEUE_DESCRIPTION__FILE_ALARM_QUEUE);
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_PutRecord
 *
 * Description: put a record in the FW upgrade queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_FwUpgrade_PutRecord(QUEUE__FW_UPGRADE_QUEUE_RECORD *ptr_record)
{
    QUEUE__FW_UPGRADE_QUEUE_RECORD *ptr_records;

    u8                             *ptr_ptr_start;
    u8                             *ptr_ptr_rd;
    u8                             *ptr_ptr_wr;


    ptr_records   = &Queue_QueueFwUpgrade.records[0];

    ptr_ptr_start = &Queue_QueueFwUpgrade.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFwUpgrade.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFwUpgrade.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE);  //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_GetRecord
 *
 * Description: get the older record not already read present in the FW upgrade queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_FwUpgrade_GetRecord(QUEUE__FW_UPGRADE_QUEUE_RECORD *ptr_record)
{
    QUEUE__FW_UPGRADE_QUEUE_RECORD *ptr_records;

    u8                             *ptr_ptr_rd;
    u8                             *ptr_ptr_wr;


    ptr_records   = &Queue_QueueFwUpgrade.records[0];

    ptr_ptr_rd    = &Queue_QueueFwUpgrade.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFwUpgrade.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_Alarm_PutRecord
 *
 * Description: put a record in the alarm queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_Alarm_PutRecord(QUEUE__ALARM_QUEUE_RECORD *ptr_record)
{
    QUEUE__ALARM_QUEUE_RECORD *ptr_records;

    u8                        *ptr_ptr_start;
    u8                        *ptr_ptr_rd;
    u8                        *ptr_ptr_wr;


    ptr_records   = &Queue_QueueAlarm.records[0];

    ptr_ptr_start = &Queue_QueueAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAlarm.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueAlarm.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__ALARM + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__ALARM + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__ALARM + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__ALARM + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__ALARM_QUEUE);  //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_Alarm_GetRecord
 *
 * Description: get the older record not already read present in the alarm queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_Alarm_GetRecord(QUEUE__ALARM_QUEUE_RECORD *ptr_record)
{
    QUEUE__ALARM_QUEUE_RECORD *ptr_records;

    u8                        *ptr_ptr_rd;
    u8                        *ptr_ptr_wr;


    ptr_records   = &Queue_QueueAlarm.records[0];

    ptr_ptr_rd    = &Queue_QueueAlarm.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueAlarm.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__ALARM + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__ALARM_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_PutRecord
 *
 * Description: put a record in the SMS answer queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_SmsAnswer_PutRecord(QUEUE__SMS_ANSWER_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_ANSWER_QUEUE_RECORD *ptr_records;

    u8                             *ptr_ptr_start;
    u8                             *ptr_ptr_rd;
    u8                             *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsAnswer.records[0];

    ptr_ptr_start = &Queue_QueueSmsAnswer.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswer.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswer.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);   //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_GetRecord
 *
 * Description: get the older record not already read present in the SMS answer queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_SmsAnswer_GetRecord(QUEUE__SMS_ANSWER_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_ANSWER_QUEUE_RECORD *ptr_records;

    u8                             *ptr_ptr_rd;
    u8                             *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsAnswer.records[0];

    ptr_ptr_rd    = &Queue_QueueSmsAnswer.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswer.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_PutRecord
 *
 * Description: put a record in the SMS answer to CRS queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record     put
 *=============================================================================*/
bool Queue_SmsAnswerToCrs_PutRecord(QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_records;

    u8                                    *ptr_ptr_start;
    u8                                    *ptr_ptr_rd;
    u8                                    *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsAnswerToCrs.records[0];

    ptr_ptr_start = &Queue_QueueSmsAnswerToCrs.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerToCrs.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswerToCrs.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1))
        *ptr_ptr_wr = 0;


    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_GetRecord
 *
 * Description: get the older record not already read present in the SMS answer to CRS queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_SmsAnswerToCrs_GetRecord(QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_ANSWER_TO_CRS_QUEUE_RECORD *ptr_records;

    u8                                    *ptr_ptr_rd;
    u8                                    *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsAnswerToCrs.records[0];

    ptr_ptr_rd    = &Queue_QueueSmsAnswerToCrs.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswerToCrs.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1))
            *ptr_ptr_rd = 0;

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_SmsForward_PutRecord
 *
 * Description: put a record in the SMS forward queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_SmsForward_PutRecord(QUEUE__SMS_FORWARD_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_FORWARD_QUEUE_RECORD *ptr_records;

    u8                              *ptr_ptr_start;
    u8                              *ptr_ptr_rd;
    u8                              *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsForward.records[0];

    ptr_ptr_start = &Queue_QueueSmsForward.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsForward.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsForward.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_FORWARD_QUEUE);   //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_SmsForward_GetRecord
 *
 * Description: get the older record not already read present in the SMS forward queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_SmsForward_GetRecord(QUEUE__SMS_FORWARD_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_FORWARD_QUEUE_RECORD *ptr_records;

    u8                              *ptr_ptr_rd;
    u8                              *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsForward.records[0];

    ptr_ptr_rd    = &Queue_QueueSmsForward.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsForward.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_FORWARD_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_PutRecord
 *
 * Description: put a record in the autosynchronize queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_Autosynchronize_PutRecord(QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *ptr_record)
{
    QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *ptr_records;

    u8                                  *ptr_ptr_start;
    u8                                  *ptr_ptr_rd;
    u8                                  *ptr_ptr_wr;


    ptr_records   = &Queue_QueueAutosynchronize.records[0];

    ptr_ptr_start = &Queue_QueueAutosynchronize.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAutosynchronize.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueAutosynchronize.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__AUTOSYNCHRONIZE_QUEUE);   //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_GetRecord
 *
 * Description: get the older record not already read present in the autosynchronize queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_Autosynchronize_GetRecord(QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *ptr_record)
{
    QUEUE__AUTOSYNCHRONIZE_QUEUE_RECORD *ptr_records;

    u8                                  *ptr_ptr_rd;
    u8                                  *ptr_ptr_wr;


    ptr_records   = &Queue_QueueAutosynchronize.records[0];

    ptr_ptr_rd    = &Queue_QueueAutosynchronize.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueAutosynchronize.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__AUTOSYNCHRONIZE_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_PutRecord
 *
 * Description: put a record in the SMS answer event queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_SmsAnswerEvent_PutRecord(QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD *ptr_records;

    u8                                   *ptr_ptr_start;
    u8                                   *ptr_ptr_rd;
    u8                                   *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsAnswerEvent.records[0];

    ptr_ptr_start = &Queue_QueueSmsAnswerEvent.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerEvent.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswerEvent.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_EVENT_QUEUE);   //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_GetRecord
 *
 * Description: get the older record not already read present in the SMS answer event queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_SmsAnswerEvent_GetRecord(QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD *ptr_record)
{
    QUEUE__SMS_ANSWER_EVENT_QUEUE_RECORD *ptr_records;

    u8                                   *ptr_ptr_rd;
    u8                                   *ptr_ptr_wr;


    ptr_records   = &Queue_QueueSmsAnswerEvent.records[0];

    ptr_ptr_rd    = &Queue_QueueSmsAnswerEvent.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswerEvent.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_EVENT_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_PutRecord
 *
 * Description: put a record in the file status log queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_FileStatusLog_PutRecord(QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *ptr_record)
{
    QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *ptr_records;

    u8                                  *ptr_ptr_start;
    u8                                  *ptr_ptr_rd;
    u8                                  *ptr_ptr_wr;


    ptr_records   = &Queue_QueueFileStatusLog.records[0];

    ptr_ptr_start = &Queue_QueueFileStatusLog.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileStatusLog.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFileStatusLog.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__FILE_STATUS_LOG + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__FILE_STATUS_LOG + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__FILE_STATUS_LOG + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__FILE_STATUS_LOG + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_GetRecord
 *
 * Description: get the older record not already read present in the file status log queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_FileStatusLog_GetRecord(QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *ptr_record)
{
    QUEUE__FILE_STATUS_LOG_QUEUE_RECORD *ptr_records;

    u8                                  *ptr_ptr_rd;
    u8                                  *ptr_ptr_wr;


    ptr_records   = &Queue_QueueFileStatusLog.records[0];

    ptr_ptr_rd    = &Queue_QueueFileStatusLog.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFileStatusLog.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__FILE_STATUS_LOG + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_FileAlarm_PutRecord
 *
 * Description: put a record in the file alarm queue
 *              If the queue is full, delete the oldest record in the queue
 * Input      : - ptr_record: pointer to the record to be put in the queue
 * Output     : - FALSE: record not put
 *              - TRUE : record    put
 *=============================================================================*/
bool Queue_FileAlarm_PutRecord(QUEUE__FILE_ALARM_QUEUE_RECORD *ptr_record)
{
    QUEUE__FILE_ALARM_QUEUE_RECORD *ptr_records;

    u8                             *ptr_ptr_start;
    u8                             *ptr_ptr_rd;
    u8                             *ptr_ptr_wr;


    ptr_records   = &Queue_QueueFileAlarm.records[0];

    ptr_ptr_start = &Queue_QueueFileAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileAlarm.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFileAlarm.ptr_wr;


    /* verify if the queue is full */
    if ((((*ptr_ptr_wr) + 1) % (QUEUE__QUEUE_SIZE__FILE_ALARM + 1)) == (*ptr_ptr_start))
    {
        /* queue full */

        /* delete the older record present in the queue */
        /* (update the queue start pointer and possibly the queue reading pointer) */
        if ((*ptr_ptr_start) == (*ptr_ptr_rd))
        {
            if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__FILE_ALARM + 1))
                *ptr_ptr_rd = 0;
        }
        if (++(*ptr_ptr_start) >= (QUEUE__QUEUE_SIZE__FILE_ALARM + 1))
            *ptr_ptr_start = 0;
    }


    /* put the record in the queue */
    *(ptr_records + (*ptr_ptr_wr)) = *ptr_record;

    /* update the queue writing pointer */
    if (++(*ptr_ptr_wr) >= (QUEUE__QUEUE_SIZE__FILE_ALARM + 1))
        *ptr_ptr_wr = 0;


    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();

    /* reset retransmission status */
    UserGsmRetx_ResetRetransmissionStatus();


    return TRUE;
}




/*=============================================================================
 * Function   : Queue_FileAlarm_GetRecord
 *
 * Description: get the older record not already read present in the file alarm queue
 * Input      : - ptr_record: pointer to store the record got from the queue
 * Output     : - FALSE: record not got (there aren't record not already read in the queue)
 *              - TRUE : record     got
 *=============================================================================*/
bool Queue_FileAlarm_GetRecord(QUEUE__FILE_ALARM_QUEUE_RECORD *ptr_record)
{
    QUEUE__FILE_ALARM_QUEUE_RECORD *ptr_records;

    u8                             *ptr_ptr_rd;
    u8                             *ptr_ptr_wr;


    ptr_records   = &Queue_QueueFileAlarm.records[0];

    ptr_ptr_rd    = &Queue_QueueFileAlarm.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFileAlarm.ptr_wr;


    /* verify if there are record not already read in the queue */
    if ((*ptr_ptr_rd) != (*ptr_ptr_wr))
    {
        /* there are record not already read in the queue */

        *ptr_record = *(ptr_records + (*ptr_ptr_rd));

        /* update the record queue reading pointer */
        if (++(*ptr_ptr_rd) >= (QUEUE__QUEUE_SIZE__FILE_ALARM + 1))
            *ptr_ptr_rd = 0;

        /* request the backup to flash objects */
        ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE);   //@@@

        Queue_UpdateVarCrc();

        return TRUE;
    }
    else
    {
        /* there aren't record not already read in the queue */

        return FALSE;
    }
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_DeleteRecords
 *
 * Description: delete permanently the records in the FW upgrade queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FwUpgrade_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueFwUpgrade.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFwUpgrade.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFwUpgrade.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the FW upgrade queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FwUpgrade_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueFwUpgrade.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFwUpgrade.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_RecoverRecordsGot
 *
 * Description: recover the records already got in the FW upgrade queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FwUpgrade_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueFwUpgrade.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFwUpgrade.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FW_UPGRADE_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_Alarm_DeleteRecords
 *
 * Description: delete permanently the records in the alarm queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Alarm_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAlarm.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueAlarm.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_Alarm_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the alarm queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Alarm_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAlarm.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_Alarm_RecoverRecordsGot
 *
 * Description: recover the records already got in the alarm queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Alarm_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAlarm.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_DeleteRecords
 *
 * Description: delete permanently the records in the SMS answer queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswer_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueSmsAnswer.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswer.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswer.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the SMS answer queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswer_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsAnswer.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswer.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_RecoverRecordsGot
 *
 * Description: recover the records already got in the SMS answer queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswer_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsAnswer.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswer.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_DeleteRecords
 *
 * Description: delete permanently the records in the SMS answer queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswerToCrs_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueSmsAnswerToCrs.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerToCrs.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswerToCrs.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the SMS answer queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswerToCrs_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsAnswerToCrs.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerToCrs.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_RecoverRecordsGot
 *
 * Description: recover the records already got in the SMS answer queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswerToCrs_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsAnswerToCrs.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerToCrs.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsForward_DeleteRecords
 *
 * Description: delete permanently the records in the SMS forward queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsForward_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueSmsForward.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsForward.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsForward.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_FORWARD_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsForward_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the SMS forward queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsForward_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsForward.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsForward.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_FORWARD_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsForward_RecoverRecordsGot
 *
 * Description: recover the records already got in the SMS forward queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsForward_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsForward.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsForward.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_FORWARD_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_DeleteRecords
 *
 * Description: delete permanently the records in the autosynchronize queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Autosynchronize_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueAutosynchronize.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAutosynchronize.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueAutosynchronize.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__AUTOSYNCHRONIZE_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the autosynchronize queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Autosynchronize_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueAutosynchronize.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAutosynchronize.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__AUTOSYNCHRONIZE_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_RecoverRecordsGot
 *
 * Description: recover the records already got in the autosynchronize queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_Autosynchronize_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueAutosynchronize.ptr_start;
    ptr_ptr_rd    = &Queue_QueueAutosynchronize.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__AUTOSYNCHRONIZE_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_DeleteRecords
 *
 * Description: delete permanently the records in the SMS answer event queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswerEvent_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueSmsAnswerEvent.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerEvent.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueSmsAnswerEvent.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_EVENT_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the SMS answer event queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswerEvent_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsAnswerEvent.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerEvent.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_EVENT_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_RecoverRecordsGot
 *
 * Description: recover the records already got in the SMS answer event queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_SmsAnswerEvent_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueSmsAnswerEvent.ptr_start;
    ptr_ptr_rd    = &Queue_QueueSmsAnswerEvent.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    //ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__SMS_ANSWER_EVENT_QUEUE);  //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_DeleteRecords
 *
 * Description: delete permanently the records in the file status log queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FileStatusLog_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueFileStatusLog.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileStatusLog.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFileStatusLog.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the file status log queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FileStatusLog_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueFileStatusLog.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileStatusLog.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_RecoverRecordsGot
 *
 * Description: recover the records already got in the file status log queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FileStatusLog_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueFileStatusLog.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileStatusLog.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_STATUS_LOG_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FileAlarm_DeleteRecords
 *
 * Description: delete permanently the records in the file alarm queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FileAlarm_DeleteRecords(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;
    u8 *ptr_ptr_wr;


    ptr_ptr_start = &Queue_QueueFileAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileAlarm.ptr_rd;
    ptr_ptr_wr    = &Queue_QueueFileAlarm.ptr_wr;

    /* update the queue pointers */
    *ptr_ptr_start = 0;
    *ptr_ptr_rd    = 0;
    *ptr_ptr_wr    = 0;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FileAlarm_DeleteRecordsGot
 *
 * Description: delete permanently the records already got in the file alarm queue
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FileAlarm_DeleteRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueFileAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileAlarm.ptr_rd;

    /* update the queue start pointer */
    *ptr_ptr_start = *ptr_ptr_rd;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FileAlarm_RecoverRecordsGot
 *
 * Description: recover the records already got in the file alarm queue (they become "not got")
 * Input      : -
 * Output     : -
 *=============================================================================*/
void Queue_FileAlarm_RecoverRecordsGot(void)
{
    u8 *ptr_ptr_start;
    u8 *ptr_ptr_rd;


    ptr_ptr_start = &Queue_QueueFileAlarm.ptr_start;
    ptr_ptr_rd    = &Queue_QueueFileAlarm.ptr_rd;

    /* update the queue reading pointer */
    *ptr_ptr_rd = *ptr_ptr_start;

    /* request the backup to flash objects */
    ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__003_QUEUE, PROGRAM_FLASH__FLASH_ID__003_QUEUE__FILE_ALARM_QUEUE);   //@@@

    Queue_UpdateVarCrc();
}




/*=============================================================================
 * Function   : Queue_FwUpgrade_NumRecords
 *
 * Description: return the number of records in the FW upgrade queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_FwUpgrade_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueFwUpgrade.ptr_start;
    ptr_wr    = Queue_QueueFwUpgrade.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                       (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__FW_UPGRADE + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_Alarm_NumRecords
 *
 * Description: return the number of records in the alarm queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_Alarm_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueAlarm.ptr_start;
    ptr_wr    = Queue_QueueAlarm.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                  (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__ALARM + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_SmsAnswer_NumRecords
 *
 * Description: return the number of records in the SMS answer queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_SmsAnswer_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueSmsAnswer.ptr_start;
    ptr_wr    = Queue_QueueSmsAnswer.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                       (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__SMS_ANSWER + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_SmsAnswerToCrs_NumRecords
 *
 * Description: return the number of records in the SMS answer to CRS queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_SmsAnswerToCrs_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueSmsAnswerToCrs.ptr_start;
    ptr_wr    = Queue_QueueSmsAnswerToCrs.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                              (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__SMS_ANSWER_TO_CRS + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_SmsForward_NumRecords
 *
 * Description: return the number of records in the SMS forward queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_SmsForward_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueSmsForward.ptr_start;
    ptr_wr    = Queue_QueueSmsForward.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                        (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__SMS_FORWARD + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_Autosynchronize_NumRecords
 *
 * Description: return the number of records in the autosynchronize queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_Autosynchronize_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueAutosynchronize.ptr_start;
    ptr_wr    = Queue_QueueAutosynchronize.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                            (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__AUTOSYNCHRONIZE + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_SmsAnswerEvent_NumRecords
 *
 * Description: return the number of records in the SMS answer event queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_SmsAnswerEvent_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueSmsAnswerEvent.ptr_start;
    ptr_wr    = Queue_QueueSmsAnswerEvent.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                             (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__SMS_ANSWER_EVENT + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_FileStatusLog_NumRecords
 *
 * Description: return the number of records in the file status log queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_FileStatusLog_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueFileStatusLog.ptr_start;
    ptr_wr    = Queue_QueueFileStatusLog.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                            (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__FILE_STATUS_LOG + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*=============================================================================
 * Function   : Queue_FileAlarm_NumRecords
 *
 * Description: return the number of records in the file alarm queue
 * Input      : -
 * Output     : - number of records
 *=============================================================================*/
u8 Queue_FileAlarm_NumRecords(void)
{
    u8 num_records;

    u8 ptr_start;
    u8 ptr_wr;


    ptr_start = Queue_QueueFileAlarm.ptr_start;
    ptr_wr    = Queue_QueueFileAlarm.ptr_wr;

    if (ptr_wr >= ptr_start)
        num_records =                                       (ptr_wr    - ptr_start);
    else
        num_records = (QUEUE__QUEUE_SIZE__FILE_ALARM + 1) - (ptr_start - ptr_wr   );

    return num_records;
}




/*===========================================================================
 * Function   : Queue_FwUpgradeQueue_GetDefault
 *
 * Description: get the default FW upgrade queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_FwUpgradeQueue_GetDefault(QUEUE__FW_UPGRADE_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueFwUpgradeDefault;
}




/*===========================================================================
 * Function   : Queue_FwUpgradeQueue_Get
 *
 * Description: get the FW upgrade queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_FwUpgradeQueue_Get(QUEUE__FW_UPGRADE_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueFwUpgrade;
}




/*===========================================================================
 * Function   : Queue_FwUpgradeQueue_Set
 *
 * Description: set the FW upgrade queue
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Queue_FwUpgradeQueue_Set(QUEUE__FW_UPGRADE_QUEUE *ptr_data)
{
    Queue_QueueFwUpgrade = *ptr_data;
}




/*===========================================================================
 * Function   : Queue_SmsAnswerToCrsQueue_GetDefault
 *
 * Description: get the default SMS answer to SMS queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_SmsAnswerToCrsQueue_GetDefault(QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueSmsAnswerToCrsDefault;
}




/*===========================================================================
 * Function   : Queue_SmsAnswerToCrsQueue_Get
 *
 * Description: get the SMS answer to SMS queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_SmsAnswerToCrsQueue_Get(QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueSmsAnswerToCrs;
}




/*===========================================================================
 * Function   : Queue_SmsAnswerToCrsQueue_Set
 *
 * Description: set the SMS answer to SMS queue
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Queue_SmsAnswerToCrsQueue_Set(QUEUE__SMS_ANSWER_TO_CRS_QUEUE *ptr_data)
{
    Queue_QueueSmsAnswerToCrs = *ptr_data;
}




/*===========================================================================
 * Function   : Queue_FileStatusLogQueue_GetDefault
 *
 * Description: get the default file status log queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_FileStatusLogQueue_GetDefault(QUEUE__FILE_STATUS_LOG_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueFileStatusLogDefault;
}




/*===========================================================================
 * Function   : Queue_FileStatusLogQueue_Get
 *
 * Description: get the file status log queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_FileStatusLogQueue_Get(QUEUE__FILE_STATUS_LOG_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueFileStatusLog;
}




/*===========================================================================
 * Function   : Queue_FileStatusLogQueue_Set
 *
 * Description: set the file status log queue
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Queue_FileStatusLogQueue_Set(QUEUE__FILE_STATUS_LOG_QUEUE *ptr_data)
{
    Queue_QueueFileStatusLog = *ptr_data;
}




/*===========================================================================
 * Function   : Queue_FileAlarmQueue_GetDefault
 *
 * Description: get the default file alarm queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_FileAlarmQueue_GetDefault(QUEUE__FILE_ALARM_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueFileAlarmDefault;
}




/*===========================================================================
 * Function   : Queue_FileAlarmQueue_Get
 *
 * Description: get the file alarm queue
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void Queue_FileAlarmQueue_Get(QUEUE__FILE_ALARM_QUEUE *ptr_data)
{
    *ptr_data = Queue_QueueFileAlarm;
}




/*===========================================================================
 * Function   : Queue_FileAlarmQueue_Set
 *
 * Description: set the file alarm queue
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void Queue_FileAlarmQueue_Set(QUEUE__FILE_ALARM_QUEUE *ptr_data)
{
    Queue_QueueFileAlarm = *ptr_data;
}
