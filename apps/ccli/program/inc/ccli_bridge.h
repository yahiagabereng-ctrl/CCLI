/*=============================================================================
 * File       :  CCLI_BRIDGE.H
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - C boundary into the C++ core / adapters / services
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/



#ifndef __CCLI_BRIDGE_H__


#define __CCLI_BRIDGE_H__




/*=============================================================================
 * INCLUDES
 *=============================================================================*/
#include "typedef.h"




/*=============================================================================
 * DEFINES
 *=============================================================================*/
/* measurement quality */
#define CCLI_BRIDGE__QUALITY__INVALID         0
#define CCLI_BRIDGE__QUALITY__GOOD            1
#define CCLI_BRIDGE__QUALITY__STALE           2

/* max length of a Modbus error string */
#define CCLI_BRIDGE__MAX_LEN_MODBUS_ERROR     127

/* max length of a signal name */
#define CCLI_BRIDGE__MAX_LEN_SIGNUM_NAME      15




/*=============================================================================
 * DATA TYPES
 *=============================================================================*/
/* runtime configuration (subset of the yaml used by the C program modules) */
typedef struct
{
    bool loaded_from_file;            /* yaml found and parsed                   */
    bool lab_demo;                    /* simulator ramp (--lab-demo)             */

    s32  do_curtail_gpio;             /* curtailment DO channel                  */
    bool do_curtail_active_high;      /* curtailment DO polarity                 */
    s32  di_permissive_gpio;          /* permissive DI channel                   */
    bool di_permissive_active_high;   /* permissive DI polarity                  */
    bool permissive_bypass;           /* lab only: DI gate disabled              */

    bool annex_m_trip_enabled;        /* P6-03 Annex M trip monitor enabled      */
    s32  annex_m_trip_gpio;           /* P6-03 Annex M trip monitor channel      */

    bool mms_enabled;                 /* Eth_A IEC 61850 MMS server              */
    bool mms_tls_enabled;             /* 62351-3 TLS on MMS (product: required)  */
    s32  mms_tcp_port;                /* MMS TCP port (product: 3782, never 102) */
    bool iec104_enabled;              /* Eth_B IEC 60870-5-104 server            */
    bool iec104_tls_enabled;          /* 62351-3 TLS on 104 when 104 enabled     */
    bool goose_enabled;               /* plant LAN GOOSE subscriber              */
    s32  goose_timeout_s;             /* GOOSE link timeout (s), 0 = disabled    */

    bool modbus_trace;                /* per-exchange Modbus trace on stderr     */
} CCLI_BRIDGE__CONFIG;


/* crash / restart recovery information */
typedef struct
{
    bool  crash_recovered;
    bool  unclean_restart;
    s32   crash_signum;
    ascii signum_name[CCLI_BRIDGE__MAX_LEN_SIGNUM_NAME + 1];
} CCLI_BRIDGE__RECOVERY;


/* last Modbus exchange */
typedef struct
{
    bool  valid;
    bool  success;
    s64   exchange_ms;
    ascii error[CCLI_BRIDGE__MAX_LEN_MODBUS_ERROR + 1];
} CCLI_BRIDGE__MODBUS_EXCHANGE;


/* measurement snapshot */
typedef struct
{
    double p_kw;
    double q_kvar;
    double ppv_kv;                    /* PdCMMXU1.PPV: meter V_L1_L2 (modbus.reg_line_voltage) or plant.poc_ppv_kv */
    bool   ppv_from_meter;            /* true when ppv_kv came from the Modbus meter */
    u8     quality;                   /* CCLI_BRIDGE__QUALITY__* */
} CCLI_BRIDGE__MEASUREMENT;


/* supervisor status published to the bench status file */
typedef struct
{
    bool curtailment_commanded;
    bool permissive_ok;
    bool annex_m_trip_active;
    bool do_curtail_on;
    s32  modbus_link_state;           /* 0 unknown, 1 up, -1 down */
} CCLI_BRIDGE__STATUS;




#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * FUNCTION PROTOTYPES
 *=============================================================================*/

/*-----------------------------------------------------------------------------
 * One-shot command line modes (exit code returned)
 *-----------------------------------------------------------------------------*/
void CcliBridge_PrintVersionText(void);
void CcliBridge_PrintVersionJson(void);
void CcliBridge_PrintUsage      (void);

u32  CcliBridge_EventMaxCount   (void);
s32  CcliBridge_EventClear      (void);
s32  CcliBridge_EventWrapTest   (void);
s32  CcliBridge_EventDump       (const ascii *config_path, u32 count, bool json_out);

s32  CcliBridge_BenchStatus     (void);
s32  CcliBridge_BenchWriteConfig(const ascii *path);

s32  CcliBridge_RegulationCheck (const ascii *config_path, bool live, bool json_out, s32 live_watch_s);
s32  CcliBridge_GpioTest        (void);

/* relay wiring check at daemon start (--gpio-blink N) */
void CcliBridge_GpioBlink       (s32 count);

/*-----------------------------------------------------------------------------
 * Platform and service supervision
 *-----------------------------------------------------------------------------*/
bool CcliBridge_PlatformInit                 (void);
void CcliBridge_Supervision_Install          (void (*safe_state_hook)(void));
bool CcliBridge_IsRunning                    (void);
void CcliBridge_Supervision_StartupCheck     (CCLI_BRIDGE__RECOVERY *ptr_data);
void CcliBridge_Supervision_MarkRunning      (void);
void CcliBridge_Supervision_MarkCleanShutdown(void);
void CcliBridge_Supervision_WatchdogPing     (void);
u32  CcliBridge_Supervision_WatchdogPingS    (void);
void CcliBridge_SleepMs                      (u32 time_ms);
s64  CcliBridge_NowMs                        (void);

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
bool CcliBridge_Config_Load(const ascii *config_path, bool lab_demo, CCLI_BRIDGE__CONFIG *ptr_data);

/*-----------------------------------------------------------------------------
 * O.14 event log
 *-----------------------------------------------------------------------------*/
void         CcliBridge_Event_Open              (void);
void         CcliBridge_Event_Append            (const ascii *type, const ascii *detail);
void         CcliBridge_Event_RecordFirmwareBoot(void);
void         CcliBridge_Event_LogSegmentation   (void);
const ascii *CcliBridge_Version_Full            (void);

/*-----------------------------------------------------------------------------
 * Runtime objects (Modbus, PF2, MMS, 104, GOOSE)
 *-----------------------------------------------------------------------------*/
void CcliBridge_Runtime_Create (void);
void CcliBridge_Runtime_Destroy(void);
void CcliBridge_PrintStartedBanner(const ascii *config_path, bool lab_demo);
void CcliBridge_PrintStopped      (void);

/* Modbus master (plant meter) */
void CcliBridge_Modbus_Tick              (void);
void CcliBridge_Modbus_SetSimulatedPowerKw(double p_kw);
void CcliBridge_Modbus_LastExchange      (CCLI_BRIDGE__MODBUS_EXCHANGE *ptr_data);
void CcliBridge_Modbus_Trace             (bool curtailment_commanded, bool do_curtail_on);

/* measurement store */
void CcliBridge_Measurement_Get(CCLI_BRIDGE__MEASUREMENT *ptr_data);

/* PF2 FSM */
void CcliBridge_Pf2_Tick              (void);
bool CcliBridge_Pf2_CurtailmentActive (void);

/* Eth_A IEC 61850 MMS */
bool CcliBridge_Mms_Start                 (void);
void CcliBridge_Mms_Stop                  (void);
void CcliBridge_Mms_PublishMeasurement    (double p_kw, double q_kvar, double ppv_kv);
void CcliBridge_Mms_RefreshTimeQuality    (void);
bool CcliBridge_Mms_GnssFix               (void);
bool CcliBridge_Mms_PollCommsLossFallback (void);
void CcliBridge_Mms_PollAndApplyDsoCommand(void);

/* Eth_B IEC 60870-5-104 */
bool CcliBridge_Iec104_Start                (void);
void CcliBridge_Iec104_Stop                 (void);
void CcliBridge_Iec104_UpdateTotWKw         (double p_kw);
void CcliBridge_Iec104_Tick                 (void);
bool CcliBridge_Iec104_PollCommsLossFallback(void);
bool CcliBridge_Iec104_PollOperatorAudit    (ascii *detail, u32 max_len);

/* plant LAN GOOSE */
bool CcliBridge_Goose_Start    (void);
void CcliBridge_Goose_Stop     (void);
bool CcliBridge_Goose_IsRunning(void);
s64  CcliBridge_Goose_LastRxMs (void);

/* network interfaces watch (O.14 iface events) */
void CcliBridge_Iface_WatchTask(void);

/* bench status file (/var/run/ccli-status.json) */
void CcliBridge_Status_Write(const CCLI_BRIDGE__STATUS *ptr_data);

#ifdef __cplusplus
}
#endif




#endif
