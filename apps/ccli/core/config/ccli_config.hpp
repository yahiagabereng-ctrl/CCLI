/*=============================================================================

 * File       :  ccli_config.hpp

 *

 * Project    :  CCLI - CCI Central Plant Controller

 * Description:  Runtime configuration loaded from the deployed YAML file

 *               (/etc/ccli/lab.yaml). Replaces hardcoded PF2 thresholds and

 *               GPIO pin/polarity so asset-owner settings are honoured

 *               (IEC 62443 CR 3.6 configurable behaviour; CEI 0-16 traceable).

 *

 * The parser is intentionally minimal and tolerant: unknown keys are ignored

 * and any field not present in the file keeps its safe compiled default.

 *=============================================================================*/

#pragma once



#include "pf2/pf2_fsm.hpp"



#include <cstdint>
#include <string>



namespace cci::core {



struct IoLineCfg {

    int  gpio{0};

    bool active_high{false};

};



enum class ModbusBackendKind : std::uint8_t {

    Simulator,

    Rtu,

    Tcp,

};



struct ModbusConfig {

    ModbusBackendKind backend{ModbusBackendKind::Simulator};

    /** Modbus TCP peer (EMT432 on LAN3). Ignored for RTU. */
    std::string       host;

    int               tcp_port{502};

    std::string       device{"/dev/ttyS1"};

    int               baud{9600};

    char              parity{'N'};

    int               slave_id{1};

    int               poll_ms{1000};

    int               timeout_ms{2000};

    int               retries{3};

    /* Holding-register offset (libmodbus 0-based). 40001 -> 0 per simulator_map.yaml */

    int reg_active_power{0};

    int reg_active_power_count{2};

    int reg_reactive_power{2};

    int reg_reactive_power_count{2};

    /** Multiply decoded P/Q before store (EMT432 P1/Q1 are W/VAR → use 0.001). */
    double power_scale{1.0};

    double reactive_scale{1.0};

    /** stderr log on each successful Modbus poll (lab bus trace). */
    bool trace{false};
};

/** Phase 3 — IEC 61850 MMS on Eth_A (LAN1). */
struct MmsTlsConfig {
    bool        enabled{false};
    std::string own_key;     /**< PEM/DER private key path */
    std::string own_cert;    /**< Server TLS certificate (mbedtls-parseable CN subject) */
    /** 62351-4 G.2/G.6.2 MMS auth cert; defaults to own_cert when empty. */
    std::string acse_cert;
    /** G.2 E2E auth private key; defaults to own_key when empty. */
    std::string acse_key;
    std::string ca_cert;     /**< Trust anchor / root CA */
    /** TLS allow-list PEM; use acse_client_cert for AARQ cert DER match when set. */
    std::string client_cert;
    std::string acse_client_cert;
    /** P3-08 / 62351-8: VIEWER role — associate+read, no Wlim write. */
    std::string viewer_cert;
    std::string acse_viewer_cert;
    /** P3-09 / T.3.3.4.9: PEM CRL path (lab); EST/SCEP product follow-up. */
    std::string crl_path;
    /**
     * Optional: allow-listed but CRL-revoked cert (P3-09 negative).
     * Must be listed so rejection is from CRL, not unknown-cert.
     */
    std::string revoked_cert;
    /** Validate peer chain against ca_cert (lab PKI). */
    bool chain_validation{true};
};

/**
 * Phase 4 — IEC 60870-5-104 on Eth_B (LAN2) only.
 * O.13.1.1.1 operator port · 62351-3 TLS (P4-03) · not Annex T.
 */
struct Iec104TlsConfig {
    bool        enabled{false};
    std::string own_key;
    std::string own_cert;
    std::string ca_cert;
    std::string client_cert;
};

/**
 * Operating Rule profile for Operatore Abilitato (Eth_B 104).
 * Annex O O.8.3 monitor quantities; commands gated by mode.
 */
struct Iec104OperatorRule {
    /** monitor_only | command — lab default monitor_only. */
    std::string mode{"monitor_only"};
    /** Master command/set-point ASDUs (C_SC/C_SE/…). false = reject + log. */
    bool allow_commands{false};
    /** Allow C_IC_NA_1 general interrogation (supervision). */
    bool allow_gi{true};
    /** Allow C_CS_NA_1 clock sync from operator master. */
    bool allow_clock_sync{false};
    /** Publish TotW at ioa_tot_w (parent Iec104Config). */
    bool monitor_tot_w{true};
    /** Publish TotVAr at ioa_tot_var when enabled (P5). */
    bool monitor_tot_var{false};
    int  ioa_tot_var{1002};
    /** Publish PPV at ioa_ppv when enabled (meter GAP until P5-M08). */
    bool monitor_ppv{false};
    int  ioa_ppv{1003};
    /** O.10.3.1 MSD active-power set-point IOA (C_SE_NC_1) when command mode. */
    int  ioa_msd_sp{3001};
};

struct Iec104Config {
    bool        enabled{false};
    /** Must be Eth_B (lab 192.168.1.130); never 0.0.0.0 or Eth_A. */
    std::string bind_address{"192.168.1.130"};
    int         tcp_port{2404};
    /** P4-04: seconds after last active STARTDT before fallback. 0 = disabled. */
    int         comms_loss_fallback_s{60};
    /** ASDU common address (station). */
    int         common_address{1};
    /** Lab IOA for TotW analogue (M_ME_NC_1). */
    int         ioa_tot_w{1001};
    /** Cyclic monitor interval (seconds). */
    int         periodic_s{4};
    Iec104OperatorRule operator_rule{};
    Iec104TlsConfig tls{};
};

/** Annex T / plant LAN — IEC 61850 GOOSE (L2), separate from DSO MMS on Eth_A. */
struct GooseConfig {
    bool        enabled{false};
    /** Linux interface for plant GOOSE (e.g. eth0, lan3). Not Eth_A DSO bind. */
    std::string interface{"eth0"};
    /** Subscribe: GoCB ref in MMS notation (Annex T Type 1 plant path). */
    std::string subscribe_go_cb_ref;
    /** 0 = accept any APPID matching goCbRef. */
    uint16_t    subscribe_app_id{0};
    /** Optional dst MAC filter (hex "01:0c:cd:01:00:01" or empty). */
    std::string subscribe_dst_mac;
    /** Enable IedServer integrated GOOSE publisher (GoCBs in CID/.cfg). */
    bool        publish_enabled{false};
    uint16_t    publish_app_id{0x0001};
    std::string publish_dst_mac{"01:0c:cd:01:00:01"};
    uint16_t    publish_vlan_id{0};
    int         publish_interval_ms{1000};
};

struct MmsConfig {
    bool        enabled{false};
    std::string bind_address{"192.168.10.1"};
    /** Cleartext lab default 102; Annex T TLS T-profile default 3782. */
    int         tcp_port{102};
    /**
     * P3-05 / O.9.2.2: seconds after last MMS client disconnect before
     * Operating Rule autonomous fallback (clear live Wlim). 0 = disabled.
     */
    int         comms_loss_fallback_s{60};
    /**
     * O.7.3.3 / Eq (9): minimum seconds between processed external set-point
     * writes (WMaxSptPct, WSptPct, VArTgtSptPct). Faster writes are rejected.
     * Normative default 3; 0 = disable gate (lab only).
     */
    int         setpoint_min_interval_s{3};
    /**
     * P3-11: poll TesPro `sim-manager gnss_get_position` for TimeQuality.
     * 0 = disable GNSS poll (TimeQuality stays clockNotSynchronized).
     */
    int         gnss_poll_s{15};
    /** Legacy: step CLOCK_REALTIME from NMEA. Prefer chrony; default off. */
    bool        gnss_discipline_clock{false};
    /** Read chronyc tracking for ±100 ms TimeQuality (T.3.3.4.5). */
    bool        chrony_poll{true};
    /**
     * libiec61850 dynamic model from genconfig .cfg (full CID).
     * Empty → legacy 9-LN hand-built MVP in mms_adapter.cpp.
     */
    std::string model_cfg_path;
    MmsTlsConfig tls{};
};

struct CcliConfig {

    Pf2Config   pf2{};

    PlantConfig plant{};

    DsoConfig   dso{};

    ModbusConfig modbus{};

    MmsConfig   mms{};

    GooseConfig goose{};

    Iec104Config iec104{};

    IoLineCfg   do_curtail{5, false};

    IoLineCfg   di_permissive{27, false};

    /** P6-03: monitor externally driven trip relay on ubus channel (e.g. DIO3). */
    struct {
        bool enabled{false};
        int  gpio{3};
    } annex_m_trip_monitor{};

    /** P7-01: O.14 rolling event log (2048, append-only file). */
    struct {
        bool        enabled{true};
        std::string path{"/var/lib/ccli/events.jsonl"};
    } event_log{};

    /* Lab-only: skip permissive DI gate when GD32 input is faulted (NOT for production). */
    bool        permissive_bypass{false};

    bool        loaded_from_file{false};

};



CcliConfig load_config(const std::string& path);



}  // namespace cci::core


