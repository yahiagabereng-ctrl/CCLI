/*=============================================================================
 * File       :  DOTA_MAIN.C
 *
 * Project    :  SK0153 - LINKY DIN 4G
 * Description:  DOTA - main
 *
 * Copyright Shitek Technology SRL (c) 2010
 *=============================================================================*/




/*=============================================================================
 * NOTES
 *=============================================================================*/
/*
 * ----------------------------------------------------------------------------
 *   DOTA
 * ----------------------------------------------------------------------------
 *  1) start
 *  2) read "DOTA phase" from Flash Objects
 *     if "DOTA phase" is "DOTA phase 1"
 *         jump to "DOTA phase 1" (DOTA     phase)
 *     else
 *         jump to "DOTA phase 2" (cleaning phase)
 *
 *
 * ----------------------------------------------------------------------------
 *   DOTA phase 1  (DOTA phase)
 * ----------------------------------------------------------------------------
 *  1) start
 *  2) read "number of DOTA phase 1 attempts" from Flash Objects
 *  3) increase "number of DOTA phase 1 attempts"
 *     if "number of DOTA phase 1 attempts" is not exausted
 *         4) format A&D memory                                        (*)
 *         5) wait for GSM network registration                        (*) (**)
 *         6) GPRS connection                                          (*)
 *         7) FTP  connection                                          (*)
 *         8  FTP  download (download file and store it in A&D memory) (*)
 *         9) store in Flash Objects flags for DOTA phase 2            (*)
 *        10) install file (stored in A&D memory)                      (*)  -->  it causes reset (DOTA phase 2 starts)
 *
 *        (*)  if any error or insuccess          , reset (DOTA phase 1 restarts)
 *        (**) if timeout GSM network registration, reset (DOTA phase 1 restarts)
 *     else
 *         4) store in Flash Objects the DOTA result
 *         5) store in Flash Objects flags for DOTA phase 2
 *         6) reset (DOTA phase 2 starts)
 *
 *
 * ----------------------------------------------------------------------------
 *   DOTA phase 2  (cleaning phase)
 * ----------------------------------------------------------------------------
 *  1) start
 *  2) verify if the DOTA result is already stored in Flash Objects
 *     if the DOTA result is not already stored in Flash Objects
 *         verify "init type" and store the DOTA result in Flash Objects
 *  3) format A&D
 *  4) store in Flash Objects the DOTA result for Program mode
 *  5) store in Flash Objects flags           for Program mode
 *  6) reset (Program mode starts)
 */




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* API      includes */
#include "ql_api_dev.h"
#include "ql_api_fota.h"
#include "ql_api_nw.h"
#include "ql_api_osi.h"
#include "ql_api_sim.h"
#include "ql_api_virt_at.h"
#include "ql_api_voice_call.h"
#include "ql_fs.h"
#include "ql_power.h"

/* user     includes */
#include "at_debug.h"
#include "boot_flash.h"
#include "boot.h"
#include "debug_my.h"
#include "dota_drvgpio.h"
#include "dota_flash.h"
#include "dota_ftp.h"
#include "dota_gprs.h"
#include "dota_main.h"
#include "program_flash.h"
#include "reset.h"
#include "typedef.h"
#include "utility.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* timeout (ms) */
#define TIME_MS__PERIOD__WDT_REARM                                (4 * 1000L)         /* time period for re-arm Application Watchdog (ms) */

/* default SIM PIN */
#define DEFAULT_PIN                                               "1234"

/* SIM card EF-CCID (Card Identification) */
#define LEN_MAX_EF_CCID                                           25

/* max debug string length */
#define MAX_LENGTH_DEBUG_STRING                                   200


/*-----------------------------------------------------------------------------
 * OS
 *-----------------------------------------------------------------------------*/
/* task message IDs */
#define DOTA__TASK_MSG_ID__RING_VOICE                             (10300 | (QL_COMPONENT_APP_START << 16))


/*-----------------------------------------------------------------------------
 * AT commands
 *-----------------------------------------------------------------------------*/
/* PIN SIM */
#define AT_COM_QPINC                                              "AT+QPINC=\"SC\"\r"  // get the number of remaining valid attempts for PIN 1

/* AT answer rx status */
#define AT_ANSWER_STATUS_RX__WAIT_START_CR                        0                    // AT answer rx status - waiting for start <CR>
#define AT_ANSWER_STATUS_RX__WAIT_START_LF                        1                    // AT answer rx status - waiting for start <LF>
#define AT_ANSWER_STATUS_RX__WAIT_DATA                            2                    // AT answer rx status - waiting for a data byte
#define AT_ANSWER_STATUS_RX__WAIT_STOP_LF                         3                    // AT answer rx status - waiting for stop  <LF>

/* answer  buffer size (bytes) */
#define SIZE_BUFFER_ANSWER                                        20


/*-----------------------------------------------------------------------------
 * DOTA phase 1  (DOTA phase)
 *-----------------------------------------------------------------------------*/
#define DOTA_PHASE_1__TIME_MS__PERIOD__GSM_NET_REG_POLLING        (      5 * 1000L)   /* time period for polling GSM network registration (ms) */
#define DOTA_PHASE_1__TIME_MS__WAIT_GSM_NET_REG                   (10 * 60 * 1000L)   /* time        for waiting network registration     (ms) */
//#define DOTA_PHASE_1__TIME_MS__RESET                              (60 * 60 * 1000L)   /* time for periodic module reset                   (ms) */

/* number of DOTA attempts */
#define DOTA_PHASE_1__NUMBER_OF_ATTEMPTS                          3


/*-----------------------------------------------------------------------------
 * DOTA phase 2  (cleaning  phase)
 *-----------------------------------------------------------------------------*/
#define DOTA_PHASE_2__TIME_MS__PERIOD__CHECK_AND_FORMAT_POLLING   (      5 * 1000L)   /* time period for checking A&D format end (ms) */




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* DOTA phase                        (actual value and default value) */
static       DOTA_MAIN__DOTA_PHASE                DotaMain_DotaPhase;
static const DOTA_MAIN__DOTA_PHASE                DotaMain_DotaPhaseDefault =
{
    DOTA_MAIN__DOTA_PHASE_1,
};

/* DOTA phase 1 - number of attempts (actual value and default value) */
static       DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS DotaMain_DotaPhase1NumOfAttempts;
static const DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS DotaMain_DotaPhase1NumOfAttemptsDefault =
{
    0,
};

/* DOTA result                       (actual value and default value) */
static       DOTA_MAIN__DOTA_RESULT               DotaMain_DotaResult;
static const DOTA_MAIN__DOTA_RESULT               DotaMain_DotaResultDefault =
{
    FALSE,
    DOTA_MAIN__DOTA_RESULT__UNKNOWN,
    DOTA_MAIN__DOTA_RESULT__ERROR__UNKNOWN,
};

/* DOTA phone number                 (actual value and default value) */
static       DOTA_MAIN__DOTA_PHONE_NUMBER         DotaMain_DotaPhoneNumber;
static const DOTA_MAIN__DOTA_PHONE_NUMBER         DotaMain_DotaPhoneNumberDefault =
{
    "",
};

/* debug string */
static       ascii                                DotaMain_DebugString[MAX_LENGTH_DEBUG_STRING + 1];

/*-----------------------------------------------------------------------------
 *   DOTA phase 1  (DOTA phase)
 *-----------------------------------------------------------------------------*/
/* status flags */
static       bool                                 DotaMain_Phase1_SimFullInit       = FALSE;   /* indication of SIM full init  terminated */
static       bool                                 DotaMain_Phase1_NetworkRegistered = FALSE;   /* indication of GSM network registration  */
static       bool                                 DotaMain_Phase1_GprsConnection    = FALSE;   /* indication of GPRS connection           */
static       bool                                 DotaMain_Phase1_FtpFileDownload   = FALSE;   /* indication of FTP file download         */

/* SIM PIN attempts left */
static       u8                                   DotaMain_SimPinAttemptsLeft = 0;

/* SIM CCID */
static       ascii                                DotaMain_SimCcid[LEN_MAX_EF_CCID + 1];

/* auto mode flag */
static       bool                                 DotaMain_Phase1_AutoModeStatus = TRUE;

/*-----------------------------------------------------------------------------
 * Open AT handlers
 *-----------------------------------------------------------------------------*/
/* timer handlers */
static       ql_timer_t                           DotaMain_TimerHandler_WdtRearm;
static       ql_timer_t                           DotaMain_TimerHandler_WaitGsmNetReg;
static       ql_timer_t                           DotaMain_TimerHandler_PollGsmNetReg;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
/* task */
       void   Dota_TaskDota(void *argument);

/* DOTA phases */
static void   DotaMain_DotaPhase1(void);
static void   DotaMain_DotaPhase2(void);

/* set boot mode */
static void   DotaMain_SetDotaModePhase2(void);
static void   DotaMain_SetProgramMode   (void);

/* set DOTA result and DOTA phone number */
static void   DotaMain_SetDotaResult     (void);
static void   DotaMain_SetDotaPhoneNumber(void);

/* "CCID PIN" */
static void   DotaMain_CalculateCcidPin(ascii *ccid, ascii *ccid_pin);

/* auto mode (DOTA phase 1) */
       void   DotaMain_EnableAutoMode   (void);
       void   DotaMain_DisableAutoMode  (void);
       bool   DotaMain_GetAutoModeStatus(void);

/* errors */
       void   DotaMain_SystemError(void);
       void   DotaMain_ResetModule(void);

/* get/set DOTA phase */
       void   DotaMain_DotaPhase_GetDefault            (DOTA_MAIN__DOTA_PHASE                *ptr_data);
       void   DotaMain_DotaPhase_Get                   (DOTA_MAIN__DOTA_PHASE                *ptr_data);
       void   DotaMain_DotaPhase_Set                   (DOTA_MAIN__DOTA_PHASE                *ptr_data);

/* get/set DOTA phase 1 number of attempts */
       void   DotaMain_DotaPhase1NumAttempts_GetDefault(DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data);
       void   DotaMain_DotaPhase1NumAttempts_Get       (DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data);
       void   DotaMain_DotaPhase1NumAttempts_Set       (DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data);

/* get/set DOTA result */
       void   DotaMain_DotaResult_GetDefault           (DOTA_MAIN__DOTA_RESULT               *ptr_data);
       void   DotaMain_DotaResult_Get                  (DOTA_MAIN__DOTA_RESULT               *ptr_data);
       void   DotaMain_DotaResult_Set                  (DOTA_MAIN__DOTA_RESULT               *ptr_data);

/* get/set DOTA phone number */
       void   DotaMain_DotaPhoneNumber_GetDefault      (DOTA_MAIN__DOTA_PHONE_NUMBER         *ptr_data);
       void   DotaMain_DotaPhoneNumber_Get             (DOTA_MAIN__DOTA_PHONE_NUMBER         *ptr_data);
       void   DotaMain_DotaPhoneNumber_Set             (DOTA_MAIN__DOTA_PHONE_NUMBER         *ptr_data);


/*-----------------------------------------------------------------------------
 * Open-AT callback functions
 *-----------------------------------------------------------------------------*/
/* call        callback functions */
static void   DotaMain_AdlCallback_Service_Call(u8 sim, ql_vc_event_id_e event_id, void *ctx);

/* AT response callback functions */
static void   DotaMain_AdlCallback_AtResponse_QPINC(unsigned int ind_type, unsigned int size);

/* utility */
static ascii *DotaMain_GetArgString(ascii *dst, const ascii *src, u16 position);
static bool   DotaMain_StringRemoveCrLf(ascii *ptr_string, const u8 *ptr_bytes, u16 num_bytes);

/* timer       callback functions */
static void   DotaMain_AdlCallback_Timer_WdtRearm     (void *ptr_context);
static void   DotaMain_AdlCallback_Timer_WaitGsmNetReg(void *ptr_context);
static void   DotaMain_AdlCallback_Timer_PollGsmNetReg(void *ptr_context);




/*===========================================================================
 * Function   : Dota_TaskDota
 *
 * Description: task DOTA
 * Input      : -
 * Output     : -
 *===========================================================================*/
void Dota_TaskDota(void *argument)
{
    ql_event_t      event;
    QlOSStatus      err;

    ql_vc_errcode_e res_vc;


    /* init debug modules */
    Debug_Init();
    AtDebug_Init();


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "TASK ENTRY POINT - DOTA - START", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* init GPIOs */
    DotaDrvGpio_Init();


    /* start periodic timer for re-arm the Application Watchdog */
    err = ql_rtos_timer_create(&DotaMain_TimerHandler_WdtRearm, Boot_TaskRef_Dota, DotaMain_AdlCallback_Timer_WdtRearm, NULL);
    if (err != QL_OSI_SUCCESS)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_rtos_timer_create - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    err = ql_rtos_timer_start(DotaMain_TimerHandler_WdtRearm, TIME_MS__PERIOD__WDT_REARM, TRUE);
    if (err != QL_OSI_SUCCESS)
    {
        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_rtos_timer_start ERROR: %d", err);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_SystemError();
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_rtos_timer_start OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }


    //@@@
    //dota_phase = DOTA_MAIN__DOTA_PHASE_1;
    //dota_phase = DOTA_MAIN__DOTA_PHASE_2;


    /* check the DOTA phase (1 or 2) */
    if (DotaMain_DotaPhase == DOTA_MAIN__DOTA_PHASE_1)
    {
        /* DOTA phase 1 (DOTA phase) */
        DotaMain_DotaPhase1();
    }
    else
    {
        /* DOTA phase 2 (cleaning phase) */
        DotaMain_DotaPhase2();
    }


    for (;;)
    {
        err = ql_event_try_wait(&event);
        if (err == QL_OSI_SUCCESS)
        {
            /* debug */
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "CALLBACK     - MESSAGE     - TASK DOTA - msg identifier: %u", event.id);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            switch (event.id)
            {
                case DOTA__TASK_MSG_ID__RING_VOICE:
                    res_vc = ql_voice_call_end(0);
                    if (res_vc != QL_VC_SUCCESS)
                    {
                        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_voice_call_end ERROR: %d", res_vc);
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        DotaMain_SystemError();
                    }
                    else
                    {
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_voice_call_end OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }
                    break;


                default:
                    break;
            }
        }
    }
}




/*=============================================================================
 * Function   : DotaMain_DotaPhase1
 *
 * Description: DOTA phase 1 (cleaning phase)
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaMain_DotaPhase1(void)
{
    const char                 local_filename[] = "UFS:fota.pac";
          QFILE                local_file_fd;

          QlOSStatus           err;

          ql_sim_status_e      card_status;

          ql_sim_errcode_e     res_sim;
          ql_errcode_virt_at_e res_vat;

          u8                   counter;

    const char                 at_cmd_string[] = AT_COM_QPINC;
          int                  res_int;


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA phase 1 start", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* increase DOTA phase 1 attempts (and store it into flash) */
    DotaMain_DotaPhase1NumOfAttempts++;
    DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA, DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS);

    /* check the number of DOTA phase 1 attempts */
    if (DotaMain_DotaPhase1NumOfAttempts > DOTA_PHASE_1__NUMBER_OF_ATTEMPTS)
    {
        /* number of DOTA attempts exhausted */

        /* debug info */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA phase 1 attempts exhausted", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* store the DOTA result (=ERROR) */
        DotaMain_DotaResult.dota_result_stored = TRUE;
        DotaMain_DotaResult.dota_result        = DOTA_MAIN__DOTA_RESULT__ERROR;
        DotaMain_DotaResult.error_code         = DOTA_MAIN__DOTA_RESULT__ERROR__OTHER_ERROR;  //@@@
        DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA, DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT);

        /* set DOTA phase 2 */
        DotaMain_SetDotaModePhase2();

        /* reset the module (it restarts in DOTA phase 2 (cleaning phase) - the old firmware release is still present) */
        DotaMain_ResetModule();
    }
    else
    {
        /* number of DOTA attempts not exhausted */

        /* debug info */
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA phase 1 attempts not exhausted", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* debug info */
        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "DOTA phase 1 - Attempt Num. %d", DotaMain_DotaPhase1NumOfAttempts);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


        /* init variables */
        DotaMain_Phase1_SimFullInit       = FALSE;   /* indication of SIM full init  terminated */
        DotaMain_Phase1_NetworkRegistered = FALSE;   /* indication of GSM network registration  */
        DotaMain_Phase1_GprsConnection    = FALSE;   /* indication of GPRS connection           */
        DotaMain_Phase1_FtpFileDownload   = FALSE;   /* indication of FTP file download         */


        /*-----------------------------------------------------------------------------
         *  GSM services
         *-----------------------------------------------------------------------------*/
        counter = 0;
        while (1)
        {
            /* AT+CPIN? - check the SIM presence */
            res_sim = ql_sim_get_card_status(0, &card_status);
            if (res_sim != QL_SIM_SUCCESS)
            {
                card_status = QL_SIM_STATUS_UNKNOW;
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_card_status: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }

            if      (card_status == QL_SIM_STATUS_READY)
            {
                break;  // exit from while loop
            }
            else if (card_status == QL_SIM_STATUS_SIMPIN)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "QL_SIM_STATUS_SIMPIN", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* get the SIM CCID */

                /* send the "AT+CCID" command */
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "Send AT+CCID command...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                res_sim = ql_sim_get_iccid(0, DotaMain_SimCcid, LEN_MAX_EF_CCID);
                if (res_sim != QL_SIM_SUCCESS)
                {
                    snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_sim_get_iccid ERROR: %d", res_sim);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DotaMain_SimCcid[0] = 0x00;

                    DotaMain_ResetModule();
                }
                else
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_iccid OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    DotaMain_SimCcid[LEN_MAX_EF_CCID] = 0x00;

                    res_vat = ql_virt_at_open(QL_VIRT_AT_PORT_1, DotaMain_AdlCallback_AtResponse_QPINC);
                    if (res_vat != QL_VIRT_AT_SUCCESS)
                    {
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_virt_at_open: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }

                    /* read the number of PIN attempts left */
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "Send AT+QPINC command...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                    /* send the "AT+QPINC" command */
                    res_int = ql_virt_at_write(QL_VIRT_AT_PORT_1, (unsigned char *)at_cmd_string, sizeof(at_cmd_string) - 1);
                    if (res_int != sizeof(at_cmd_string) - 1)
                    {
                        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_virt_at_write ERROR: %d", res_int);
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                        DotaMain_SystemError();
                    }
                    else
                    {
                        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_virt_at_write OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    }
                }

                ql_rtos_task_sleep_ms(500L);

                break;  // exit from while loop
            }
            else if (card_status == QL_SIM_STATUS_SIMPUK)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "QL_SIM_STATUS_SIMPUK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DotaMain_ResetModule();

                break;  // exit from while loop
            }

            if (counter++ >= 60)
                break;  // exit from while loop

            ql_rtos_task_sleep_ms(500L);
        }

        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "AT+CPIN? ---> %d", card_status);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        /* waiting for SIM init */
        counter = 0;
        while (1)
        {
            res_sim = ql_sim_get_card_status(0, &card_status);
            if (res_sim != QL_SIM_SUCCESS)
            {
                card_status = QL_SIM_STATUS_UNKNOW;
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_sim_get_card_status: ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }

            if (card_status == QL_SIM_STATUS_READY)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "QL_SIM_STATUS_READY", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DotaMain_Phase1_SimFullInit = TRUE;  // SIM Full Init done

                break;  // exit from while loop
            }

            if (counter++ >= 60)
            {
                snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "QL_SIM_STATUS: %d", card_status);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DotaMain_ResetModule();

                break;  // exit from while loop
            }

            ql_rtos_task_sleep_ms(500L);
        }


        /* subscribe to call service */
        ql_voice_call_callback_register(DotaMain_AdlCallback_Service_Call);


        /*-----------------------------------------------------------------------------
         *  Timer
         *-----------------------------------------------------------------------------*/
        /* start periodic timer for polling GSM network registration */
        err = ql_rtos_timer_create(&DotaMain_TimerHandler_PollGsmNetReg, Boot_TaskRef_Dota, DotaMain_AdlCallback_Timer_PollGsmNetReg, NULL);
        if (err != QL_OSI_SUCCESS)
        {
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_rtos_timer_create ERROR: %d", err);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        err = ql_rtos_timer_start(DotaMain_TimerHandler_PollGsmNetReg, DOTA_PHASE_1__TIME_MS__PERIOD__GSM_NET_REG_POLLING, TRUE);
        if (err != QL_OSI_SUCCESS)
        {
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_rtos_timer_start ERROR: %d", err);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            DotaMain_SystemError();
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_rtos_timer_start OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }


        /* start timer for waiting GSM network registration */
        err = ql_rtos_timer_create(&DotaMain_TimerHandler_WaitGsmNetReg, Boot_TaskRef_Dota, DotaMain_AdlCallback_Timer_WaitGsmNetReg, NULL);
        if (err != QL_OSI_SUCCESS)
        {
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_rtos_timer_create ERROR: %d", err);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }
        err = ql_rtos_timer_start(DotaMain_TimerHandler_WaitGsmNetReg, DOTA_PHASE_1__TIME_MS__WAIT_GSM_NET_REG, FALSE);
        if (err != QL_OSI_SUCCESS)
        {
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_rtos_timer_start ERROR: %d", err);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            DotaMain_SystemError();
        }
        else
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_rtos_timer_start OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }


        /*-----------------------------------------------------------------------------
         *  A&D memory
         *-----------------------------------------------------------------------------*/
        /* start A&D formatting */
        local_file_fd = ql_fopen(local_filename, "wb+");
        if (local_file_fd <= 0)
        {
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_fopen ERROR: %d", local_file_fd);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            DotaMain_SystemError();
        }
        ql_fclose(local_file_fd);
    }
}




/*=============================================================================
 * Function   : DotaMain_DotaPhase2
 *
 * Description: DOTA phase 2 (cleaning phase)
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaMain_DotaPhase2(void)
{
    ql_fota_result_e  init_type;
    ql_errcode_fota_e result;


    /* debug info */
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA phase 2 start", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* verify if the DOTA result is already stored */
    if (!DotaMain_DotaResult.dota_result_stored)
    {
        /* verify init type */
        result = ql_fota_get_result(&init_type);
        if (result != QL_FOTA_SUCCESS)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_fota_get_result ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            init_type = QL_FOTA_STATUS_INVALID;
        }

        switch (init_type)
        {
            // Upgrade finished
            case QL_FOTA_FINISHED:
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA result: SUCCESS"                , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* store the DOTA result (= "SUCCESS") */
                DotaMain_DotaResult.dota_result_stored = TRUE;
                DotaMain_DotaResult.dota_result        = DOTA_MAIN__DOTA_RESULT__SUCCESS;
                DotaMain_DotaResult.error_code         = DOTA_MAIN__DOTA_RESULT__ERROR__NO_ERROR;
                break;

            // The latest FOTA package is detected, waiting for system upgrade
            case QL_FOTA_READY:
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA result: ERROR - NO FILE INSTALL", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* store the DOTA result (= "ERROR - NO FILE INSTALL") */
                DotaMain_DotaResult.dota_result_stored = TRUE;
                DotaMain_DotaResult.dota_result        = DOTA_MAIN__DOTA_RESULT__ERROR;
                DotaMain_DotaResult.error_code         = DOTA_MAIN__DOTA_RESULT__ERROR__NO_FILE_INSTALL;
                break;

            // The latest FOTA package is not detected
            // Invalid status
            // FOTA package verification failure
            case QL_FOTA_NOT_EXIST:
            case QL_FOTA_STATUS_INVALID:
            case QL_FOTA_PACK_CHECK_ERR:
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "DOTA result: ERROR - OTHER ERROR"    , __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                /* store the DOTA result (= "ERROR - OTHER ERROR") */
                DotaMain_DotaResult.dota_result_stored = TRUE;
                DotaMain_DotaResult.dota_result        = DOTA_MAIN__DOTA_RESULT__ERROR;
                DotaMain_DotaResult.error_code         = DOTA_MAIN__DOTA_RESULT__ERROR__OTHER_ERROR;
                break;
        }
        DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA, DOTA_FLASH__FLASH_ID__000_DOTA__DOTA_RESULT);
    }


    /* delete the upgrade package */
    ql_fota_file_reset(TRUE);


    /* store the DOTA result for PROGRAM mode */
    DotaMain_SetDotaResult();

    /* store the DOTA phone number for PROGRAM mode */
    DotaMain_SetDotaPhoneNumber();

    /* set PROGRAM mode */
    DotaMain_SetProgramMode();

    /* reset the module (it restarts in Program Mode) */
    DotaMain_ResetModule();
}




/*=============================================================================
 * Function   : DotaMain_SetDotaModePhase2
 *
 * Description: set DOTA mode phase 2
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaMain_SetDotaModePhase2(void)
{
    BOOT__BOOT_MODE boot_mode;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "Set the DOTA Mode - Phase 2...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* set "DOTA phase 1 number of attempts" to zero (and store it into flash) */
    DotaMain_DotaPhase1NumOfAttempts = 0;
    DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA     , DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS);

    /* set "DOTA phase" to DOTA phase 2              (and store it into flash) */
    DotaMain_DotaPhase = DOTA_MAIN__DOTA_PHASE_2;
    DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA     , DOTA_FLASH__FLASH_ID__000_DOTA__PHASE           );


    /* set "boot mode" to DOTA mode                  (and store it into flash) */
    boot_mode = BOOT__DOTA_MODE;
    Boot_BootMode_Set(&boot_mode);
    BootFlash_RequestWriteBackup(BOOT_FLASH__HANDLE_INDEX__000_BOOT_MODE, BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE  );
}




/*=============================================================================
 * Function   : DotaMain_SetProgramMode
 *
 * Description: set PROGRAM mode
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void DotaMain_SetProgramMode(void)
{
    BOOT__BOOT_MODE boot_mode;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "Set the PROGRAM Mode...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);


    /* set "DOTA phase 1 number of attempts" to zero (and store it into flash) */
    DotaMain_DotaPhase1NumOfAttempts = 0;
    DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA     , DOTA_FLASH__FLASH_ID__000_DOTA__PHASE_1_ATTEMPTS);

    /* set "DOTA phase" to DOTA phase 1              (and store it into flash) */
    DotaMain_DotaPhase = DOTA_MAIN__DOTA_PHASE_1;
    DotaFlash_RequestWriteBackup(DOTA_FLASH__HANDLE_INDEX__000_DOTA     , DOTA_FLASH__FLASH_ID__000_DOTA__PHASE           );


    /* set "boot mode" to PROGRAM mode               (and store it into flash) */
    boot_mode = BOOT__PROGRAM_MODE;
    Boot_BootMode_Set(&boot_mode);
    BootFlash_RequestWriteBackup(BOOT_FLASH__HANDLE_INDEX__000_BOOT_MODE, BOOT_FLASH__FLASH_ID__000_BOOT_MODE__BOOT_MODE  );
}




/*===========================================================================
 * Function    : DotaMain_SetDotaResult
 *
 * Description : store the DOTA result for PROGRAM mode
 * Input       : -
 * Output      : -
 *===========================================================================*/
static void DotaMain_SetDotaResult(void)
{
    RESET__DOTA_RESULT dota_result;


    dota_result.dota_result_stored = DotaMain_DotaResult.dota_result_stored;


    switch (DotaMain_DotaResult.dota_result)
    {
        case DOTA_MAIN__DOTA_RESULT__UNKNOWN:
        default:
            dota_result.dota_result = RESET__DOTA_RESULT__UNKNOWN;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR:
            dota_result.dota_result = RESET__DOTA_RESULT__ERROR;
            break;

        case DOTA_MAIN__DOTA_RESULT__SUCCESS:
            dota_result.dota_result = RESET__DOTA_RESULT__SUCCESS;
            break;
    }


    switch (DotaMain_DotaResult.error_code)
    {
        case DOTA_MAIN__DOTA_RESULT__ERROR__UNKNOWN:
        default:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__UNKNOWN;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__NO_ERROR:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__NO_ERROR;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__OTHER_ERROR:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__OTHER_ERROR;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__NO_GSM_REG:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__NO_GSM_REG;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__NO_GPRS_APN:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__NO_GPRS_APN;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__NO_FTP_SERVER:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__NO_FTP_SERVER;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__NO_FTP_FILE_DOWNLOAD:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__NO_FTP_FILE_DOWNLOAD;
            break;

        case DOTA_MAIN__DOTA_RESULT__ERROR__NO_FILE_INSTALL:
            dota_result.error_code = RESET__DOTA_RESULT__ERROR__NO_FILE_INSTALL;
            break;
    }


    Reset_DotaResult_Set(&dota_result);

    /* NOTE: Don't use the "ProgramFlash_RequestWriteBackup" function because the task "Task_Flash" is not active in DOTA mode */
    ProgramFlash_BackupWrite(PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_RESULT);
}




/*===========================================================================
 * Function    : DotaMain_SetDotaPhoneNumber
 *
 * Description : store the DOTA phone number for PROGRAM mode
 * Input       : -
 * Output      : -
 *===========================================================================*/
static void DotaMain_SetDotaPhoneNumber(void)
{
    RESET__DOTA_PHONE_NUMBER dota_phone_number;


    strncpy(dota_phone_number.dota_phone_number, DotaMain_DotaPhoneNumber.dota_phone_number, RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER);
    dota_phone_number.dota_phone_number[RESET__DOTA_PHONE_NUMBER__MAX_LENGTH_PHONE_NUMBER] = 0x00;

    Reset_DotaPhoneNumber_Set(&dota_phone_number);

    /* NOTE: Don't use the "ProgramFlash_RequestWriteBackup" function because the task "ProgramFlash" is not active in DOTA mode */
    ProgramFlash_BackupWrite(PROGRAM_FLASH__FLASH_ID__000_RESET__DOTA_PHONE_NUMBER);
}




/*=============================================================================
 * Function   : DotaMain_CalculateCcidPin
 *
 * Description: build the "CCID PIN" (4 digits) using the SIM CCID
 *
 *              Example:
 *
 *                 CCID: 8939880342012645091
 *
 *                 1) 342012645091    (take last 12 digits of CCID string)
 *                 2) 3420 1264 5091  (build 3 blocks of 4 digits)
 *                 3) Sum the digits in the following way
 *                       3 + 1 + 5  =  9                9
 *                       4 + 2 + 0  =  6                6
 *                       2 + 6 + 9  =  17  -->  1 + 7 = 8
 *                       0 + 4 + 1  =  5                5
 *                 4) PIN = 9685  (4 digits)
 *
 *              If the CCID string has problems (not digit string or string length < 12),
 *              the default PIN ("1234") is used
 *
 * Input      : - ccid    : pointer to the          SIM CCID   string
 *              - ccid_pin: pointer to the save the "CCID PIN" string
 * Output     : - pointer to the "PIN CCID" string
 *=============================================================================*/
static void DotaMain_CalculateCcidPin(ascii *ccid, ascii *ccid_pin)
{
    u8   pin_digit_1;
    u8   pin_digit_2;
    u8   pin_digit_3;
    u8   pin_digit_4;

    u8   len_ccid;

    bool ccid_ok;


    len_ccid = (u8)strlen(ccid);

    if (
         (Utility_IsNumString(ccid)) &&
         (len_ccid >= 12           )
       )
    {
        ccid_ok = TRUE;
    }
    else
    {
        ccid_ok = FALSE;
    }


    if (ccid_ok)
    {
        /* CCID string is OK */

        /* sum the digits */
        pin_digit_1 = (ccid[len_ccid - 12    ] - '0')  +  (ccid[len_ccid - 12 + 4] - '0')  +  (ccid[len_ccid - 12 +  8] - '0');
        pin_digit_2 = (ccid[len_ccid - 12 + 1] - '0')  +  (ccid[len_ccid - 12 + 5] - '0')  +  (ccid[len_ccid - 12 +  9] - '0');
        pin_digit_3 = (ccid[len_ccid - 12 + 2] - '0')  +  (ccid[len_ccid - 12 + 6] - '0')  +  (ccid[len_ccid - 12 + 10] - '0');
        pin_digit_4 = (ccid[len_ccid - 12 + 3] - '0')  +  (ccid[len_ccid - 12 + 7] - '0')  +  (ccid[len_ccid - 12 + 11] - '0');

        /* if a result is >= 10, sum the 2 digits of the result (1st time) */
        if (pin_digit_1 >= 10)
            pin_digit_1 = (pin_digit_1 / 10) + (pin_digit_1 % 10);
        if (pin_digit_2 >= 10)
            pin_digit_2 = (pin_digit_2 / 10) + (pin_digit_2 % 10);
        if (pin_digit_3 >= 10)
            pin_digit_3 = (pin_digit_3 / 10) + (pin_digit_3 % 10);
        if (pin_digit_4 >= 10)
            pin_digit_4 = (pin_digit_4 / 10) + (pin_digit_4 % 10);

        /* if a result is >= 10, sum the 2 digits of the result (2nd time) */
        if (pin_digit_1 >= 10)
            pin_digit_1 = (pin_digit_1 / 10) + (pin_digit_1 % 10);
        if (pin_digit_2 >= 10)
            pin_digit_2 = (pin_digit_2 / 10) + (pin_digit_2 % 10);
        if (pin_digit_3 >= 10)
            pin_digit_3 = (pin_digit_3 / 10) + (pin_digit_3 % 10);
        if (pin_digit_4 >= 10)
            pin_digit_4 = (pin_digit_4 / 10) + (pin_digit_4 % 10);

        /* build the "CCID PIN" ASCII string */
        ccid_pin[0] = pin_digit_1 + '0';
        ccid_pin[1] = pin_digit_2 + '0';
        ccid_pin[2] = pin_digit_3 + '0';
        ccid_pin[3] = pin_digit_4 + '0';
        ccid_pin[4] = 0x00;
    }
    else
    {
        /* CCID string is not OK */

        /* build the default ASCII string ("1234") */
        strncpy(ccid_pin, DEFAULT_PIN, 4);
        ccid_pin[4] = 0x00;
    }


    snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "\"CCID PIN\": %s", ccid_pin);
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
}




/*=============================================================================
 * Function   : DotaMain_EnableAutoMode
 *
 * Description: enable auto mode
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DotaMain_EnableAutoMode(void)
{
    DotaMain_Phase1_AutoModeStatus = TRUE;
}




/*=============================================================================
 * Function   : DotaMain_DisableAutoMode
 *
 * Description: disable auto mode
 * Input      : -
 * Output     : -
 *=============================================================================*/
void DotaMain_DisableAutoMode(void)
{
    DotaMain_Phase1_AutoModeStatus = FALSE;
}




/*=============================================================================
 * Function   : DotaMain_GetAutoModeStatus
 *
 * Description: return the auto mode status
 * Input      : -
 * Output     : - FALSE: auto mode disabled
 *              - TRUE : auto mode enabled
 *=============================================================================*/
bool DotaMain_GetAutoModeStatus(void)
{
    return DotaMain_Phase1_AutoModeStatus;
}




/*===========================================================================
 * Function    : DotaMain_SystemError
 *
 * Description : system error --> reset the module
 * Input       : -
 * Output      : -
 *===========================================================================*/
void DotaMain_SystemError(void)
{
    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "System Error", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    DotaMain_ResetModule();
}




/*===========================================================================
 * Function    : DotaMain_ResetModule
 *
 * Description : Reset the module sending "AT+CFUN=1" AT command
 * Input       : -
 * Output      : -
 *===========================================================================*/
void DotaMain_ResetModule(void)
{
    ql_errcode_power res_pm;


    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "Reset module...", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    res_pm = ql_power_reset(RESET_NORMAL);
    if (res_pm != QL_POWER_RESET_SUCCESS)
    {
        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_power_reset ERROR: %d", res_pm);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        while(1);  // cause reset for Application Watchdog
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_power_reset OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function   : DotaMain_DotaPhase_GetDefault
 *
 * Description: get the default DOTA phase
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhase_GetDefault(DOTA_MAIN__DOTA_PHASE *ptr_data)
{
    *ptr_data = DotaMain_DotaPhaseDefault;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhase_Get
 *
 * Description: get the DOTA phase
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhase_Get(DOTA_MAIN__DOTA_PHASE *ptr_data)
{
    *ptr_data = DotaMain_DotaPhase;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhase_Set
 *
 * Description: set the DOTA phase
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhase_Set(DOTA_MAIN__DOTA_PHASE *ptr_data)
{
    DotaMain_DotaPhase = *ptr_data;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhase1NumAttempts_GetDefault
 *
 * Description: get the default DOTA phase 1 number of attempts
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhase1NumAttempts_GetDefault(DOTA_MAIN__DOTA_PHASE *ptr_data)
{
    *ptr_data = DotaMain_DotaPhase1NumOfAttemptsDefault;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhase1NumAttempts_Get
 *
 * Description: get the DOTA phase 1 number of attempts
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhase1NumAttempts_Get(DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data)
{
    *ptr_data = DotaMain_DotaPhase1NumOfAttempts;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhase1NumAttempts_Set
 *
 * Description: set the DOTA phase 1 number of attempts
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhase1NumAttempts_Set(DOTA_MAIN__DOTA_PHASE_1_NUM_ATTEMPTS *ptr_data)
{
    DotaMain_DotaPhase1NumOfAttempts = *ptr_data;
}




/*===========================================================================
 * Function   : DotaMain_DotaResult_GetDefault
 *
 * Description: get the default DOTA result
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaResult_GetDefault(DOTA_MAIN__DOTA_RESULT *ptr_data)
{
    *ptr_data = DotaMain_DotaResultDefault;
}




/*===========================================================================
 * Function   : DotaMain_DotaResult_Get
 *
 * Description: get the DOTA result
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaResult_Get(DOTA_MAIN__DOTA_RESULT *ptr_data)
{
    *ptr_data = DotaMain_DotaResult;
}




/*===========================================================================
 * Function   : DotaMain_DotaResult_Set
 *
 * Description: set the DOTA result
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaResult_Set(DOTA_MAIN__DOTA_RESULT *ptr_data)
{
    DotaMain_DotaResult = *ptr_data;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhoneNumber_GetDefault
 *
 * Description: get the default DOTA phone number
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhoneNumber_GetDefault(DOTA_MAIN__DOTA_PHONE_NUMBER *ptr_data)
{
    *ptr_data = DotaMain_DotaPhoneNumberDefault;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhoneNumber_Get
 *
 * Description: get the DOTA phone number
 * Input      : - ptr_data: pointer to store the values got
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhoneNumber_Get(DOTA_MAIN__DOTA_PHONE_NUMBER *ptr_data)
{
    *ptr_data = DotaMain_DotaPhoneNumber;
}




/*===========================================================================
 * Function   : DotaMain_DotaPhoneNumber_Set
 *
 * Description: set the DOTA phone number
 * Input      : - ptr_data: pointer to the values to set
 * Output     : -
 *===========================================================================*/
void DotaMain_DotaPhoneNumber_Set(DOTA_MAIN__DOTA_PHONE_NUMBER *ptr_data)
{
    DotaMain_DotaPhoneNumber = *ptr_data;
}




/*===========================================================================
 * Function   : DotaMain_AdlCallback_Service_Call
 *
 * Description:
 * Input      : - event  :
 *              - call_id:
 * Output     : - to or not to forward the event
 *===========================================================================*/
static void DotaMain_AdlCallback_Service_Call(u8 sim, ql_vc_event_id_e event_id, void *ctx)
{
    ql_event_t event;
    QlOSStatus err;


    switch (event_id)
    {
        // voice phone call ringing
        case QL_VC_RING_IND:
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - SERVICE CALL - QL_VC_RING_IND", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* signal incoming call */
            event.id = DOTA__TASK_MSG_ID__RING_VOICE;

            err = ql_rtos_event_send(Boot_TaskRef_Dota, &event);
            if (err != QL_OSI_SUCCESS)
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_rtos_event_send ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            }
            break;


        // Unknown event
        default:
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "CALLBACK     - SERVICE CALL - event_id: %d", event_id);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            break;
    }
}




/*=============================================================================
 * Function   : DotaMain_AdlCallback_AtResponse_QPINC
 *
 * Description: callback function for response to "+QPINC" AT command
 * Input      : - params: information about AT response
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
static void DotaMain_AdlCallback_AtResponse_QPINC(unsigned int ind_type, unsigned int size)
{
    u8                       *ptr_data;
    int                       res_int;

    ql_sim_verify_pin_info_s  pin;
    ql_sim_errcode_e          res_sim;

    ascii                     qpinc_string[SIZE_BUFFER_ANSWER + 1];

    ascii                     pin_string[2 + 1] = {0x00, 0x00, 0x00};
    ascii                     puk_string[2 + 1] = {0x00, 0x00, 0x00};

    u8                        pin_attempts_left;
    u8                        puk_attempts_left;

    bool                      first_attempt_done;


    if (ind_type == QUEC_VIRT_AT_RX_RECV_DATA_IND)
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - AT RESPONSE - AT+QPINC", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        ptr_data = (u8 *)malloc(size + 1);
        if (ptr_data == NULL)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "LIB FUNCTION - malloc - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
            return;
        }

        memset(ptr_data, 0, size + 1);

        res_int = ql_virt_at_read(QL_VIRT_AT_PORT_1, ptr_data, size);
        if (res_int != (int)size)
        {
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "QL FUNCTION - ql_virt_at_read - ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            /* free allocated memory */
            free(ptr_data);
            return;
        }

        if (  (                                              (sizeof(AT_COM_QPINC) - 1)  == size)  &&
              (strncmp((const char *)ptr_data, AT_COM_QPINC, (sizeof(AT_COM_QPINC) - 1)) == 0   )  )
        {
            /* echo */

            /* free allocated memory */
            free(ptr_data);
            return;
        }

        memset(qpinc_string, 0, SIZE_BUFFER_ANSWER + 1);
        DotaMain_StringRemoveCrLf(qpinc_string, ptr_data, size);

        /* free allocated memory */
        free(ptr_data);

        if (strncmp(qpinc_string, "+QPINC:", strlen("+QPINC:")) == 0)
        {
            /* +QPINC: "SC",<PIN_counter>,<PUK_counter> */

            DotaMain_GetArgString(pin_string, qpinc_string, 2);
            DotaMain_GetArgString(puk_string, qpinc_string, 3);

            pin_attempts_left = (u8)atoi(pin_string);
            puk_attempts_left = (u8)atoi(puk_string);

            DotaMain_SimPinAttemptsLeft = pin_attempts_left;

            /* debug info */
            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "AT+QPINC ---> +QPINC: \"SC\",%d,%d", pin_attempts_left, puk_attempts_left);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
        }


        if (strncmp(qpinc_string, "OK", (sizeof("OK") - 1)) == 0)
        {
            /* OK */

            /* debug info */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "AT+QPINC ---> OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "SIM PIN attempts left: %d", DotaMain_SimPinAttemptsLeft);
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            first_attempt_done = FALSE;
            while (1)
            {
                if (DotaMain_SimPinAttemptsLeft < 2)
                {
                    /* minus than 2 PIN attempts left */
                    DotaMain_ResetModule();
                }

                if (DotaMain_SimPinAttemptsLeft > 0)
                    DotaMain_SimPinAttemptsLeft--;

                if (first_attempt_done)
                {
                    /* try to enter the "default PIN" (2nd pin ATTEMPT) */
                    strcpy((char *)(pin.pin_value), DEFAULT_PIN);
                }
                else
                {
                    first_attempt_done = TRUE;

                    /* try to enter the "CCID PIN" (1st PIN attempts) */
                    DotaMain_CalculateCcidPin(DotaMain_SimCcid, (char *)(pin.pin_value));
                }

                res_sim = ql_sim_verify_pin(0, &pin);
                if (res_sim != QL_SIM_SUCCESS)
                {
                    snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_sim_verify_pin ERROR: %d", res_sim);
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                }
                else
                {
                    Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_sim_verify_pin OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                    break;
                }
            }
        }


        if (
             (strncmp(qpinc_string, "ERROR"      , (sizeof("ERROR"      ) - 1)) == 0) ||
             (strncmp(qpinc_string, "+CME ERROR:", (sizeof("+CME ERROR:") - 1)) == 0)
           )
        {
            /* ERROR, +CME ERROR: */

            /* debug info */
            Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "AT+QPINC ---> ERROR", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

            DotaMain_ResetModule();
        }
    }
}




/*=============================================================================
 * Function   : DotaMain_GetArgString
 *
 * Description: If src is a string formatted as an AT response (for example "+RESP:P1,P2,P3") or
 *              as an AT command (for example "AT+CMD=P1,P2,P3"), the function copies the
 *              parameter at position offset (starting from 1) if it is present in the src buffer, and
 *              returns a pointer on dst. It returns NULL otherwise.
 *              if the parameter is a quoted string (for example "AT+CMD=P1,"P2",P3"),
 *              the 2 quotes are removed.
 * Input      : - dst     : destination string
 *              - src     : source      string
 *              - position: position of the parameter to be found (starting from 1)
 * Output     : - pointer to dst string if the parameter is     found
 *              - NULL                  if the parameter is not found
 *=============================================================================*/
static ascii *DotaMain_GetArgString(ascii *dst, const ascii *src, u16 position)
{
    u16    len_src;
    u16    len_src_temp;

    ascii *token;
    ascii *remainder;
    u16    len_token;
    u8     num_arg;
    u8     i;


    /* calculate the length of src string */
    len_src = (u16)strlen(src);

    num_arg = 0;
    len_src_temp = 0;

    token = strtok_r((ascii *)src, "=:", &remainder);
    if (token != NULL)
    {
        len_token     = strlen(token);
        len_src_temp += len_token;
        if (len_src_temp < len_src)
        {
            token[len_token] = '=';   // put the delimiter removed by strtok (put '=' in both cases)
            len_src_temp++;
        }
    }

    while (token != NULL)
    {
        token = strtok_r(NULL, ",", &remainder);
        if (token != NULL)
        {
            len_token     = strlen(token);
            len_src_temp += len_token;
            if (len_src_temp < len_src)
            {
                token[len_token] = ',';   // put the delimiter removed by strtok
                len_src_temp++;
            }

            num_arg++;
            if (num_arg == position)
            {
                if (len_token <= 2)
                {
                    if (
                         (token[0            ] == '"') &&
                         (token[len_token - 1] == '"')
                       )
                    {
                        /* quoted string */
                        for (i = 0; i < (len_token - 2); i++)
                            dst[i] = token[i + 1];
                        dst[len_token - 2] = 0x00;
                    }
                    else
                    {
                        /* no quoted string */
                        for (i = 0; i < len_token; i++)
                            dst[i] = token[i];
                        dst[len_token] = 0x00;
                    }

                    return dst;
                }
                else
                {
                    return NULL;
                }
            }
        }
    }


    return NULL;
}




/*=============================================================================
 * Function   : DotaMain_StringRemoveCrLf
 *
 * Description:
 * Input      : - params:
 * Output     : - FALSE:
 *              - TRUE :
 *=============================================================================*/
static bool DotaMain_StringRemoveCrLf(ascii *ptr_string, const u8 *ptr_bytes, u16 num_bytes)
{
    u8  byte_rx;
    u8  at_answer_status_rx;

    u8  at_answer_buffer[SIZE_BUFFER_ANSWER];
    u8  at_answer_buffer_index;

    u8  i;
    u8  j;


    at_answer_buffer_index = 0;
    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;

    /* check if a possible AT answer or a prompt from the GSM module has been received */
    for (i = 0; i < num_bytes; i++)
    {
        byte_rx = *ptr_bytes++;

        /* check if a possible AT answer from the GSM module has been received */
        switch (at_answer_status_rx)
        {
            // AT answer rx status - waiting for start <CR>
            case AT_ANSWER_STATUS_RX__WAIT_START_CR:
                /* check if a <CR> has been received */
                if (byte_rx == '\r')
                {
                    /* <CR> received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_LF;
                }
                else
                {
                    /* other byte received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                }
                break;


            // AT answer rx status - waiting for start <LF>
            case AT_ANSWER_STATUS_RX__WAIT_START_LF:
                /* check if a <LF> has been received */
                if      (byte_rx == '\n')
                {
                    /* <LF> received */
                    at_answer_buffer_index = 0;
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_DATA;
                }
                else if (byte_rx == '\r')
                {
                    /* <CR> received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_LF;
                }
                else
                {
                    /* other byte received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                }
                break;


            // AT answer rx status - waiting for a data byte
            case AT_ANSWER_STATUS_RX__WAIT_DATA:
                /* check if a <CR> has been received */
                if (byte_rx == '\r')
                {
                    /* <CR> received */
                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_STOP_LF;
                }
                else
                {
                    /* other byte received */

                    /* save the received data byte */
                    if (at_answer_buffer_index < SIZE_BUFFER_ANSWER)
                        at_answer_buffer[at_answer_buffer_index++] = byte_rx;
                }
                break;


            // AT answer rx status - waiting for stop  <LF>
            case AT_ANSWER_STATUS_RX__WAIT_STOP_LF:
                /* check if a <LF> has been received */
                if      (byte_rx == '\n')
                {
                    /* <LF> received */

                    /* copy all the received bytes (except <LF> and <CR> bytes) */
                    for (j = 0; j < at_answer_buffer_index; j++)
                        *(ptr_string + j) = at_answer_buffer[j];

                    /* terminate the string with NULL */
                    *(ptr_string + at_answer_buffer_index) = '\0';

                    return TRUE;
                }
                else if (byte_rx == '\r')
                {
                    /* <CR> received */

                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_LF;
                }
                else
                {
                    /* other byte received */

                    at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                }
                break;


            // AT answer rx status - unknown
            default:
                at_answer_status_rx = AT_ANSWER_STATUS_RX__WAIT_START_CR;
                break;
        }
    }


    return FALSE;
}




/*===========================================================================
 * Function    : DotaMain_AdlCallback_Timer_WdtRearm
 *
 * Description :
 * Input       : - ptr_context:
 * Output      : -
 *===========================================================================*/
static void DotaMain_AdlCallback_Timer_WdtRearm(void *ptr_context)
{
    ql_errcode_dev_e result;


    /* re-arm the Application Watchdog */
    result = ql_dev_feed_wdt();
    if (result != QL_DEV_SUCCESS)
    {
        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_dev_feed_wdt ERROR: %d", result);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        DotaMain_SystemError();
    }
    else
    {
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_dev_feed_wdt OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
}




/*===========================================================================
 * Function    : DotaMain_AdlCallback_Timer_WaitGsmNetReg
 *
 * Description :
 * Input       : - ptr_context:
 * Output      : -
 *===========================================================================*/
static void DotaMain_AdlCallback_Timer_WaitGsmNetReg(void *ptr_context)
{
    Debug_SendDebugString2(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "CALLBACK     - TIMER - DotaMain_TimerHandler_WaitGsmNetReg", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

    DotaMain_ResetModule();
}




/*===========================================================================
 * Function    : DotaMain_AdlCallback_Timer_PollGsmNetReg
 *
 * Description :
 * Input       : - ptr_context:
 * Output      : -
 *===========================================================================*/
static void DotaMain_AdlCallback_Timer_PollGsmNetReg(void *ptr_context)
{
    const char                    local_filename[] = "UFS:fota.pac";

          ql_nw_reg_status_info_s reg_info;
          ql_nw_errcode_e         res_nw;

          ql_errcode_fota_e       res_fota;

          QlOSStatus              err;

          bool                    result;


    /* send tha AT command "AT+CREG?" */
    res_nw = ql_nw_get_reg_status(0, &reg_info);
    if (res_nw != QL_NW_SUCCESS)
    {
        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_nw_get_reg_status ERROR: %d", res_nw);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
    }
    else
    {
        /* debug info */
        snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "AT+CGREG? ---> +CGREG: 0,%d", reg_info.data_reg.state);
        Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

        switch (reg_info.data_reg.state)
        {
            // not registered, ME is not currently searching for a new operator
            // not registered, ME currently searching for a new operator
            // registration denied
            // unknown
            case QL_NW_REG_STATE_NOT_REGISTERED:
            case QL_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING:
            case QL_NW_REG_STATE_DENIED:
            case QL_NW_REG_STATE_UNKNOWN:
                // not registered in network
                DotaMain_Phase1_NetworkRegistered = FALSE;
                break;

            // registered, home network
            // registered, roaming
            case QL_NW_REG_STATE_HOME_NETWORK:
            case QL_NW_REG_STATE_ROAMING:
                // registered in network
                DotaMain_Phase1_NetworkRegistered = TRUE;

                if (DotaMain_Phase1_SimFullInit)
                {
                    /* SIM full init and A&D formatting terminated */
                    if (!DotaMain_Phase1_GprsConnection)
                    {
                        if (DotaMain_Phase1_AutoModeStatus)
                        {
                            /* stop timer for waiting GSM network registration */
                            err = ql_rtos_timer_stop(DotaMain_TimerHandler_WaitGsmNetReg);
                            if (err != QL_OSI_SUCCESS)
                            {
                                snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_rtos_timer_stop ERROR: %d", err);
                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                                DotaMain_SystemError();
                            }
                            else
                            {
                                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_rtos_timer_stop OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);
                            }

                            /* start the GPRS connection */
                            result = DotaGprs_GprsStartConnection();
                            if (!result)
                                DotaMain_ResetModule();

                            DotaMain_Phase1_GprsConnection = TRUE;
                        }
                    }
                }
                break;

            // unknown <stat> value
            default:
                DotaMain_Phase1_NetworkRegistered = FALSE;
                break;
        }
    }


    /* verify if the FTP file download is terminated */
    DotaMain_Phase1_FtpFileDownload = DotaFtp_FtpFileDownloadDone();
    if (DotaMain_Phase1_FtpFileDownload)
    {
        /* FTP file download terminated */

        if (DotaMain_Phase1_AutoModeStatus)
        {
            /* set DOTA phase 2 */
            DotaMain_SetDotaModePhase2();

            /* install the application */
            res_fota = ql_fota_image_verify((char *)local_filename);
            if (res_fota != QL_FOTA_SUCCESS)
            {
                snprintf(DotaMain_DebugString, sizeof(DotaMain_DebugString), "ql_fota_image_verify ERROR: %d", res_fota);
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, DotaMain_DebugString, __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                DotaMain_SystemError();
            }
            else
            {
                Debug_SendDebugString1(DEBUG_TRACE_LEVEL_DOTA, DEBUG_TRACE_TYPE_LOW, "ql_fota_image_verify OK", __FILE__, __LINE__, (ascii *)__FUNCTION__, 0);

                ql_rtos_task_sleep_ms(5000);

                DotaMain_ResetModule();
            }
        }
    }
}
