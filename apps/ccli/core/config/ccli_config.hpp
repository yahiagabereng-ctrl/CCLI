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
#include "event/event_store.hpp"



#include <cstdint>
#include <string>
#include <vector>



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

/** EMT432 holding-register commissioning (lab yaml modbus.commissioning). */
struct ModbusCommissioningConfig {
    /** Must match code kRevision in modbus_emt432_commissioning_revision.hpp (0 = ignore). */
    int    revision{5};
    bool   enabled{false};
    bool   apply_once{true};
    /** 40011 offset 10 — verify Rogowski mounted_version == 1. */
    bool   verify_mounted_version{true};
    /** 40232 offset 231 — RMW single phase + integrator ON. */
    bool   write_meas_1p_integrator{false};
    /** 40232 bit 15 — filtered measurement enabled (MR Measurement_Config). */
    bool   write_filtered_measurement{false};
    /** 40235 offset 234 — float32 Rogowski sensitivity [V/A]. */
    bool   write_rogowski_sensitivity{false};
    double rogowski_sensitivity_v_per_a{0.000085};
    /** 40234 — 0 = user TA (cert typical). */
    bool   verify_ta_chronos_register{true};
    bool   write_ta_chronos_register{false};
    /** 40237/40239/40241/40243/40245/40247 bench defaults (direct VT, no ripple cut). */
    bool   verify_transducer_bench_defaults{true};
    bool   write_transducer_bench_defaults{false};
    double ta_transducer_delay_deg{0.0};
    double tv_transducer_ratio{1.0};
    double tv_transducer_delay_deg{0.0};
    double minimum_voltage_ripple{0.0};
    /** 40245 — non-zero forces I to 0 below threshold (critical for Rogowski low current). */
    double minimum_current_ripple{0.0};
    /** 40247 — non-zero forces P/Q/S to 0 below threshold. */
    double minimum_power_ripple{0.0};
    /** 40256 — MR Energy_unit_factor; 4 = kWh. */
    bool   verify_energy_unit_kwh{true};
    bool   write_energy_unit_kwh{false};
    /** 40232 bit 9 — MR Energy saving enabled. */
    bool   write_energy_saving{false};
    /** After commissioning, FC3 read KWh1/KVARh1/KVAh1/KWh_SUM (lab log). */
    bool   log_energy_readings{true};
};

struct ModbusConfig {

    /** Optional second reader, used for an independent inverter RTU link. */
    bool enabled{false};

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

    /** Holding offset for line V (EMT432 1P V_L1_N @906, PLC 40907). -1 = use plant.poc_ppv_kv. Not @980 (L1-L2 peak). */
    int reg_line_voltage{-1};

    int reg_line_voltage_count{2};

    /** V to kV for MMS PPV (default 0.001). */
    double line_voltage_scale{0.001};

    /** Holding offset for I1 (EMT432 @922, PLC 40923). -1 = do not map A.phsA. */
    int reg_phase_a_current{-1};

    int reg_phase_a_current_count{2};

    /** A for MMS A.phsA (default 1.0). */
    double phase_a_current_scale{1.0};

    /** When true, poll EMT432 3φ PPV/A offsets (924–944); 1P leaves read ~0. */
    bool poll_pdc_3ph{false};

    int reg_phase_b_current{-1};
    int reg_phase_b_current_count{2};
    int reg_phase_c_current{-1};
    int reg_phase_c_current_count{2};
    int reg_ppv_phs_bc{-1};
    int reg_ppv_phs_ca{-1};
    int reg_active_power_l2{-1};
    int reg_active_power_l3{-1};
    int reg_reactive_power_l2{-1};
    int reg_reactive_power_l3{-1};

    /** PF1 float offset (EMT432 @956). Used when Q1 ≈ 0 for CMV ang.f. -1 = off. */
    int reg_power_factor{-1};
    int reg_power_factor_count{2};

    /** Multiply decoded P/Q before store (EMT432 P1/Q1 are W/VAR → use 0.001). */
    double power_scale{1.0};

    double reactive_scale{1.0};

    /** Chronos EMT432 TCP: float32 low register word first (Modbus CDAB). */
    bool float_word_swap{false};

    /** stderr log on each successful Modbus poll (lab bus trace). */
    bool trace{false};

    ModbusCommissioningConfig commissioning{};
};

/** Phase 3 — IEC 61850 MMS on Eth_A (LAN1). */
struct MmsE2eConfig {
    bool configuration_valid{true};
    bool enabled{false};
    bool allow_compatibility{false};
    std::string certificate, private_key, trust_anchors, crls;
    std::vector<std::string> certificate_path;
    std::string ap_title;
    unsigned ae_qualifier{0};
    std::string signature_algorithm{"rsa-sha256"};
    unsigned rekey_seconds{900};
};

struct TlsTimingConfig {
    int revocation_check_s{3600}; // 1..86400; required local CRL re-evaluation
    int renegotiation_s{43200};   // 600..86400; TLS 1.2 full handshake
    int resumption_s{3600};       // 1..7200; monotonic session cache lifetime (one-hour default)
    bool allow_sha384{false};     // optional TLS/certificate signature hash
    bool allow_sha512{false};     // optional TLS/certificate signature hash
    bool configuration_valid{true};
};

struct MmsTlsConfig : TlsTimingConfig {
    bool        enabled{false};
    std::string own_key;     /**< PEM/DER private key path */
    std::string own_cert;    /**< Server TLS certificate (mbedtls-parseable CN subject) */
    /** Optional additional server identity for TLS suites requiring another key type. */
    std::string additional_own_key;
    std::string additional_own_cert;
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
    /** Required PEM CRL bundle for TLS peer revocation checks. */
    std::string crl_path;
    /**
     * Optional: allow-listed but CRL-revoked cert (P3-09 negative).
     * Must be listed so rejection is from CRL, not unknown-cert.
     */
    std::string revoked_cert;
    /** Additional TLS peer pin; application roles still require the commissioning policy. */
    std::string additional_client_cert;
    /** Repeatable tls_allowed_client_cert entries; pins do not grant application roles. */
    std::vector<std::string> allowed_client_certs;
    /** Validate peer chain against ca_cert (lab PKI). */
    bool chain_validation{true};
};

/**
 * Phase 4 — IEC 60870-5-104 on Eth_B (LAN2) only.
 * O.13.1.1.1 operator port · 62351-3 TLS (P4-03) · not Annex T.
 */
struct Iec104TlsConfig : TlsTimingConfig {
    bool        enabled{false};
    std::string own_key;
    std::string own_cert;
    std::string ca_cert;
    std::string client_cert;
    /** Repeatable exact TLS peer-certificate allowlist entries. */
    std::vector<std::string> allowed_client_certs;
    /** Required PEM CRL bundle for TLS peer revocation checks. */
    std::string crl_path;
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
    /** O.14 plant GOOSE comms: log link_down after this many seconds without RX. 0 = off. */
    int         timeout_s{10};
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
    /** Reserved for a validated signed P/Q plant backend; false keeps Aggregator commands in memory. */
    bool        aggregator_plant_outputs_enabled{false};
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
    /** IEC role PolicySet; local permission library and scopes share its directory. */
    std::string rbac_policy_path;
    /** Enable Profile A only after a valid commissioning policy is loaded. */
    bool rbac_profile_a{false};
    /** Pinned device-owner signing certificate; independent of TLS credentials. */
    std::string rbac_owner_cert;
    /** Durable policy anti-rollback state; provision directory on persistent storage. */
    std::string rbac_activation_state;
    /** Identity validation CA/CRL; CA falls back to the existing TLS CA path. */
    std::string rbac_identity_ca;
    std::string rbac_identity_crl;
    MmsE2eConfig e2e{};
    MmsTlsConfig tls{};
};

struct CcliConfig {

    Pf2Config   pf2{};

    PlantConfig plant{};

    DsoConfig   dso{};

    ModbusConfig modbus{};

    /** Independent inverter reader; does not replace the POC meter poller. */
    ModbusConfig inverter_modbus{};

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

    /** P7-01 / P7-03: O.14 rolling event log (2048, append-only file + optional syslog). */
    struct : EventLogConfig {
        /** Space/comma-separated ifaces for O.14 physical-link (e.g. "lan1 lan2"). */
        std::string watch_ifaces;
    } event_log{};

    /* Lab-only: skip permissive DI gate when GD32 input is faulted (NOT for production). */
    bool        permissive_bypass{false};

    bool        loaded_from_file{false};

};



CcliConfig load_config(const std::string& path);



}  // namespace cci::core


