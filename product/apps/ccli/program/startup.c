/*=============================================================================
 * File       :  STARTUP.C
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - startup (bring-up order, startup type, recover)
 *
 * Bring-up order (IEC 62443-4-2 CR 3.6 direction): DI/DO forced safe before
 * any protocol activity; O.14 log opened before the first PF2 step; servers
 * started last.
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
/* standard includes */
#include <stdio.h>

/* user     includes */
#include "startup.h"

#include "alarm.h"
#include "ccli_bridge.h"
#include "drvgpio.h"
#include "f_pf2.h"
#include "fw_config.h"
#include "outputs.h"
#include "parser_modbus.h"
#include "program_flash.h"
#include "queue_my.h"
#include "regulation.h"
#include "synchronize.h"
#include "transmission_104.h"
#include "transmission_goose.h"
#include "transmission_mms.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* max length of an event detail */
#define MAX_LENGTH_EVENT_DETAIL    200




/*=============================================================================
 * VARIABLES
 *=============================================================================*/
/* startup type */
static u8   Startup_StartupType   = STARTUP__STARTUP_TYPE__UNKNOWN;

/* recover status indication */
static bool Startup_RecoverStatus = FALSE;




/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/
static void Startup_RecoverManagement(void);




/*=============================================================================
 * Function   : Startup_TaskStartup
 *
 * Description: full bring-up in the lab-validated order
 * Input      : ptr_options - command line options
 * Output     : FALSE on fatal error (process exit code 1)
 *=============================================================================*/
bool Startup_TaskStartup(const COMMANDS__OPTIONS *ptr_options)
{
    const FW_CONFIG__RUNTIME *cfg;
    DrvGpioConfig             gpio_cfg;
    ascii                     detail[MAX_LENGTH_EVENT_DETAIL + 1];

    /*----------------------------------------------------------------
     * Platform and configuration
     *----------------------------------------------------------------*/
    if (!CcliBridge_PlatformInit())
    {
        fprintf(stderr, "platform_init failed\n");
        return false;
    }

    (void)FwConfig_Load(ptr_options->config_path, ptr_options->lab_demo);
    cfg = FwConfig_Get();

    /* Product cyber gate (O.15 / T.3.3.4 / 62351-3): Eth_A MMS must be TLS :3782.
     * Cleartext :102 is lab TSP only and must not start in this tree. */
    if (cfg->mms_enabled && (!cfg->mms_tls_enabled || cfg->mms_tcp_port == 102))
    {
        fprintf(stderr,
                "startup: REFUSE — product MMS requires tls_enabled and tcp_port!=102 "
                "(got tls=%s port=%d). Use product_tr400.yaml / *_5ln.yaml.\n",
                cfg->mms_tls_enabled ? "true" : "false", (int)cfg->mms_tcp_port);
        return false;
    }
    if (cfg->iec104_enabled && !cfg->iec104_tls_enabled)
    {
        fprintf(stderr,
                "startup: REFUSE — product IEC 104 requires tls_enabled on Eth_B :2404\n");
        return false;
    }

    Queue_Init();
    ProgramFlash_Init();


    /*----------------------------------------------------------------
     * DI/DO stack: safe state before any protocol activity
     *----------------------------------------------------------------*/
    FwConfig_GetDrvGpioConfig(&gpio_cfg);
    if (!Outputs_Init(&gpio_cfg))
    {
        fprintf(stderr, "svc_io_init failed\n");
        return false;
    }

    CcliBridge_Supervision_Install(Outputs_SafeState);

    if (cfg->permissive_bypass)
    {
        fprintf(stderr, "io: WARNING permissive_bypass=on \xE2\x80\x94 DI gate disabled (lab only)\n");
    }

    (void)Outputs_ConfigureAnnexMTripMonitor(cfg->annex_m_trip_gpio, cfg->annex_m_trip_enabled);

    if (ptr_options->gpio_blink > 0)
    {
        CcliBridge_GpioBlink(ptr_options->gpio_blink);
    }


    /*----------------------------------------------------------------
     * O.14 event log, startup type, recover
     *----------------------------------------------------------------*/
    CcliBridge_Event_Open();
    Alarm_Init();
    Startup_RecoverManagement();

    snprintf(detail, sizeof(detail), "power_on:%s", CcliBridge_Version_Full());
    Alarm_Log("system", detail);
    CcliBridge_Event_RecordFirmwareBoot();
    Alarm_Log("system",   "psu_unmonitored");
    Alarm_Log("security", "service_monitor:started");
    Alarm_Log("security", "tls_material:loaded_from_config");
    CcliBridge_Event_LogSegmentation();
    CcliBridge_Supervision_MarkRunning();

    (void)ProgramFlash_RequestWriteBackup(PROGRAM_FLASH__HANDLE_INDEX__000_STARTUP,
                                          PROGRAM_FLASH__FLASH_ID__000_STARTUP__STARTUP_TYPE);


    /*----------------------------------------------------------------
     * Runtime objects, first PF2 step, first DO drive
     *----------------------------------------------------------------*/
    CcliBridge_Runtime_Create();
    ParserModbus_Init();
    Synchronize_Init();

    CcliBridge_Modbus_Tick();
    FPf2_Task(0);
    Regulation_Init();


    /*----------------------------------------------------------------
     * Servers (Eth_A MMS, Eth_B 104, plant LAN GOOSE)
     *----------------------------------------------------------------*/
    TransmissionMms_Init();
    Transmission104_Init();
    TransmissionGoose_Init();

    CcliBridge_PrintStartedBanner(ptr_options->config_path, ptr_options->lab_demo);
    return true;
}




/*=============================================================================
 * Function   : Startup_GetStartupType
 *
 * Description: startup type
 * Input      : -
 * Output     : STARTUP__STARTUP_TYPE__*
 *=============================================================================*/
u8 Startup_GetStartupType(void)
{
    return Startup_StartupType;
}




/*=============================================================================
 * Function   : Startup_GetRecoverStatus
 *
 * Description: recover status indication
 * Input      : -
 * Output     : TRUE when retained status is valid
 *=============================================================================*/
bool Startup_GetRecoverStatus(void)
{
    return Startup_RecoverStatus;
}




/*=============================================================================
 * Function   : Startup_RecoverManagement
 *
 * Description: startup type from the supervision tombstone / running marker,
 *              O.14 security events, retained status CRC check
 * Input      : -
 * Output     : -
 *=============================================================================*/
static void Startup_RecoverManagement(void)
{
    CCLI_BRIDGE__RECOVERY recovery;
    ascii                 detail[MAX_LENGTH_EVENT_DETAIL + 1];
    bool                  recover_status;

    CcliBridge_Supervision_StartupCheck(&recovery);

    if (recovery.crash_recovered)
    {
        Startup_StartupType = STARTUP__STARTUP_TYPE__REBOOT_INTERNAL;
        snprintf(detail, sizeof(detail), "crash_recovered:%s signum=%d",
                 recovery.signum_name, (int)recovery.crash_signum);
        Alarm_Log("security", detail);
        fprintf(stderr, "security: previous crash recovered (%s)\n", recovery.signum_name);
    }
    else if (recovery.unclean_restart)
    {
        Startup_StartupType = STARTUP__STARTUP_TYPE__REBOOT_UNCLEAN;
        Alarm_Log("security", "unclean_restart:stale_running_marker");
        fprintf(stderr, "security: unclean restart detected (stale running marker)\n");
    }
    else
    {
        Startup_StartupType = STARTUP__STARTUP_TYPE__POWER_ON;
    }

    switch (Startup_StartupType)
    {
        /* startup type - reboot (crash / unclean) */
        case STARTUP__STARTUP_TYPE__REBOOT_INTERNAL:
        case STARTUP__STARTUP_TYPE__REBOOT_UNCLEAN:
            recover_status = TRUE;
            break;

        /* startup type - power-on / unknown */
        case STARTUP__STARTUP_TYPE__POWER_ON:
        case STARTUP__STARTUP_TYPE__UNKNOWN:
        default:
            recover_status = FALSE;
            break;
    }

    if (recover_status)
    {
        recover_status = DrvGpio_VerifyVarCrc()    &&
                         Outputs_VerifyVarCrc()    &&
                         Regulation_VerifyVarCrc() &&
                         Queue_VerifyVarCrc();
    }

    /* outputs always restart from safe state; retained status is informative until program_flash persists it */
    Startup_RecoverStatus = recover_status;
}
