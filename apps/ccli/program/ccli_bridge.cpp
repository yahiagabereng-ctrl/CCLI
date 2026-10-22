/*=============================================================================
 * File       :  CCLI_BRIDGE.CPP
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - C boundary into the C++ core / adapters / services
 *
 * Owns the runtime objects (config, O.14 store, Modbus, PF2, MMS, 104, GOOSE)
 * for the single-threaded Main supervisor. Wire behaviour stays in the
 * services / adapters; this file only adapts types.
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/
#include "ccli_tools.hpp"

#include "svc_goose.hpp"
#include "svc_iec104.hpp"
#include "svc_mms.hpp"
#include "svc_modbus.hpp"
#include "svc_pf2.hpp"

#include "cci/hal/platform.hpp"
#include "cci/hal/time.hpp"
#include "config/ccli_config.hpp"
#include "dso/dso_phase1.hpp"
#include "event/event_store.hpp"
#include "event/link_monitor.hpp"
#include "policy/net_policy.hpp"
#include "service/service_supervision.hpp"
#include "version/ccli_version.hpp"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unistd.h>

#include "ccli_bridge.h"

namespace {

struct Runtime {
    cci::core::CcliConfig        cfg{};
    cci::core::EventLogConfig    elog_cfg{};
    cci::core::Pf2Config         pf2_cfg{};
    cci::core::dso::DsoPhase1ApplyResult dso_apply{};
    std::string                  version_full;

    std::unique_ptr<cci::core::MeasurementStore>   measurements;
    std::unique_ptr<cci::core::EventStore>         events;
    std::unique_ptr<cci::services::ModbusService>  modbus;
    std::unique_ptr<cci::services::Pf2Service>     pf2;
    std::unique_ptr<cci::services::MmsService>     mms;
    std::unique_ptr<cci::services::Iec104Service>  iec104;
    std::unique_ptr<cci::services::GooseService>   goose;

    cci::adapters::DsoLiveCommand last_dso_cmd{};
    bool                          dso_cmd_seen{false};
    std::int64_t                  last_mb_trace_ms{-1};

    std::vector<std::string> watch_ifaces;
    std::vector<std::string> last_iface_state;
};

std::atomic<bool> g_running{true};
Runtime           g_rt;

void copy_cstr(char* dst, std::size_t dst_len, const std::string& src) {
    if (dst == nullptr || dst_len == 0) {
        return;
    }
    std::snprintf(dst, dst_len, "%s", src.c_str());
}

void append_event(const std::string& type, const std::string& detail) {
    if (g_rt.events) {
        g_rt.events->append({cci::hal::now().epoch_ms, type, detail, ""});
    }
}

void apply_dso_live_command(const cci::adapters::DsoLiveCommand& dso_cmd) {
    auto& cfg = g_rt.cfg;
    auto& pf2 = *g_rt.pf2;
    auto& modbus = *g_rt.modbus;

    if (dso_cmd.dirty) {
        const bool use_or_profile = g_rt.mms->operating_rule_fallback_latched();
        cci::core::Pf2Config live_cfg = pf2.config();
        live_cfg.use_dso_mock = true;
        const auto live = cci::core::dso::apply_live_dso_commands_to_pf2(
            live_cfg, cfg.plant, cfg.dso, dso_cmd.wlim_active, dso_cmd.wmax_spt_pct,
            dso_cmd.wsd_active, dso_cmd.wspt_pct, use_or_profile);
        if (live.applied) {
            pf2.set_thresholds(live.pf2_threshold_kw, live.pf2_release_kw);
            if (use_or_profile) {
                std::cerr << "mms→pf2: O.13.1.2 Operating Rule (O.11 min) — enter="
                          << live.pf2_threshold_kw << " release=" << live.pf2_release_kw
                          << " kW\n";
                append_event("mms", "operating_rule_autonomous");
            } else {
                std::cerr << "mms→pf2: Wlim=" << (dso_cmd.wlim_active ? "on" : "off") << "@"
                          << dso_cmd.wmax_spt_pct << "% WSd=" << (dso_cmd.wsd_active ? "on" : "off")
                          << "@" << dso_cmd.wspt_pct << "% p_wlim=" << live.derived.p_wlim_kw
                          << " p_wsd=" << live.derived.p_wsd_kw
                          << " p_eff=" << live.derived.p_effective_export_kw
                          << " kW → enter=" << live.pf2_threshold_kw
                          << " release=" << live.pf2_release_kw << " kW\n";
                append_event("mms", dso_cmd.wsd_active    ? "wsd_update"
                                    : dso_cmd.wlim_active ? "wlim_on"
                                                          : "wlim_off");
            }
        }
    }

    if (!dso_cmd.reactive_dirty) {
        return;
    }
    const double p_kw = g_rt.measurements->snapshot().p_kw;

    if (dso_cmd.pfsp_dirty) {
        const auto q_pfsp = cci::core::dso::apply_live_pfsp_command(
            cfg.plant, cfg.dso, dso_cmd.pfsp_active, dso_cmd.pfsp_cosphi,
            dso_cmd.pfsp_generation, p_kw);
        const int mod_val = dso_cmd.pfsp_active ? 1 : 5;
        if (q_pfsp.applied) {
            const std::string base = "pfsp_operate mod=" + std::to_string(mod_val) +
                                     " cosphi=" + std::to_string(dso_cmd.pfsp_cosphi) +
                                     " p_kw=" + std::to_string(p_kw) + " q_kvar=" +
                                     std::to_string(q_pfsp.derived.q_target_kvar);
            if (modbus.write_reactive_kvar(q_pfsp.derived.q_target_kvar)) {
                std::cerr << "mms→plant: PFSP on cosφ=" << dso_cmd.pfsp_cosphi << " P=" << p_kw
                          << " kW → Q=" << q_pfsp.derived.q_target_kvar
                          << " kvar [O.9.1.1 P5-R02]\n";
                append_event("mms", base + " result=ok");
            } else {
                append_event("mms", base + " result=plant_write_fail");
            }
        } else {
            append_event("mms", "pfsp_operate mod=5 cosphi=" +
                                    std::to_string(dso_cmd.pfsp_cosphi) +
                                    " q_kvar=0 result=cleared");
        }
        return;
    }

    const auto q_cmd = cci::core::dso::apply_live_varsd_command(
        cfg.plant, cfg.dso, dso_cmd.varsd_active, dso_cmd.vartgt_spt_pct);
    if (q_cmd.applied) {
        const int mod_val = dso_cmd.varsd_active ? 1 : 5;
        const std::string base = "varsd_operate mod=" + std::to_string(mod_val) +
                                 " pct=" + std::to_string(dso_cmd.vartgt_spt_pct) +
                                 " q_kvar=" + std::to_string(q_cmd.derived.q_target_kvar);
        if (modbus.write_reactive_kvar(q_cmd.derived.q_target_kvar)) {
            std::cerr << "mms→plant: VArSd on @" << dso_cmd.vartgt_spt_pct
                      << "% Smax → Q=" << q_cmd.derived.q_target_kvar
                      << " kvar [O.9.1.4 P5-R01]\n";
            append_event("mms", base + " result=ok");
        } else {
            std::cerr << "mms→plant: VArSd Q write FAILED q=" << q_cmd.derived.q_target_kvar
                      << " kvar\n";
            append_event("mms", base + " result=plant_write_fail");
        }
    } else {
        std::cerr << "mms→plant: VArSd off — plant Q command cleared [O.9.1.4]\n";
        append_event("mms", "varsd_operate mod=5 pct=" +
                                std::to_string(dso_cmd.vartgt_spt_pct) +
                                " q_kvar=0 result=cleared");
    }
}

}  // namespace

extern "C" {

/*-----------------------------------------------------------------------------
 * One-shot command line modes
 *-----------------------------------------------------------------------------*/
void CcliBridge_PrintVersionText(void) { cci::core::version::print_text(std::cout); }

void CcliBridge_PrintVersionJson(void) { cci::core::version::print_json(std::cout); }

void CcliBridge_PrintUsage(void) { cci::program::print_usage(); }

u32 CcliBridge_EventMaxCount(void) {
    return static_cast<u32>(cci::core::EventStore::kMaxEvents);
}

s32 CcliBridge_EventClear(void) {
    std::cerr << "O.14: user overwrite forbidden (P7-01) — no erase API\n";
    return cci::core::EventStore::user_overwrite_forbidden() ? 2 : 1;
}

s32 CcliBridge_EventWrapTest(void) { return cci::program::run_event_wrap_test(); }

s32 CcliBridge_EventDump(const ascii* config_path, u32 count, bool json_out) {
    return cci::program::run_event_dump(config_path, count, json_out);
}

s32 CcliBridge_BenchStatus(void) { return cci::program::run_bench_status_json(); }

s32 CcliBridge_BenchWriteConfig(const ascii* path) {
    return cci::program::run_bench_write_config(path);
}

s32 CcliBridge_RegulationCheck(const ascii* config_path, bool live, bool json_out,
                               s32 live_watch_s) {
    return cci::program::run_regulation_check(config_path, live, json_out, live_watch_s);
}

s32 CcliBridge_GpioTest(void) { return cci::program::run_gpio_test(&g_running); }

void CcliBridge_GpioBlink(s32 count) { cci::program::gpio_blink_curtail(count, g_running); }

/*-----------------------------------------------------------------------------
 * Platform and service supervision
 *-----------------------------------------------------------------------------*/
bool CcliBridge_PlatformInit(void) { return cci::hal::platform_init(); }

void CcliBridge_Supervision_Install(void (*safe_state_hook)(void)) {
    cci::core::ServiceSupervision::set_safe_state_hook(safe_state_hook);
    cci::core::ServiceSupervision::install_handlers(&g_running);
}

bool CcliBridge_IsRunning(void) { return g_running.load(); }

void CcliBridge_Supervision_StartupCheck(CCLI_BRIDGE__RECOVERY* ptr_data) {
    const auto recovery = cci::core::ServiceSupervision::startup_check();
    ptr_data->crash_recovered = recovery.crash_recovered;
    ptr_data->unclean_restart = recovery.unclean_restart;
    ptr_data->crash_signum = recovery.crash_signum;
    copy_cstr(ptr_data->signum_name, sizeof(ptr_data->signum_name),
              cci::core::ServiceSupervision::signum_name(recovery.crash_signum));
}

void CcliBridge_Supervision_MarkRunning(void) {
    cci::core::ServiceSupervision::mark_running();
}

void CcliBridge_Supervision_MarkCleanShutdown(void) {
    cci::core::ServiceSupervision::mark_clean_shutdown();
}

void CcliBridge_Supervision_WatchdogPing(void) {
    cci::core::ServiceSupervision::procd_watchdog_ping();
}

u32 CcliBridge_Supervision_WatchdogPingS(void) {
    return static_cast<u32>(cci::core::ServiceSupervision::kDefaultWatchdogPingS);
}

void CcliBridge_SleepMs(u32 time_ms) { usleep(static_cast<useconds_t>(time_ms) * 1000U); }

s64 CcliBridge_NowMs(void) { return cci::hal::now().epoch_ms; }

/*-----------------------------------------------------------------------------
 * Configuration
 *-----------------------------------------------------------------------------*/
bool CcliBridge_Config_Load(const ascii* config_path, bool lab_demo,
                            CCLI_BRIDGE__CONFIG* ptr_data) {
    (void)cci::core::lab_segmentation_policy();

    g_rt.cfg = cci::core::load_config(config_path);
    if (lab_demo) {
        g_rt.cfg.modbus.backend = cci::core::ModbusBackendKind::Simulator;
    }
    const auto& c = g_rt.cfg;
    std::cout << "ccli config " << (c.loaded_from_file ? "loaded from " : "defaults (no ")
              << config_path << (c.loaded_from_file ? "" : ")") << '\n';

    ptr_data->loaded_from_file = c.loaded_from_file;
    ptr_data->lab_demo = lab_demo;
    ptr_data->do_curtail_gpio = c.do_curtail.gpio;
    ptr_data->do_curtail_active_high = c.do_curtail.active_high;
    ptr_data->di_permissive_gpio = c.di_permissive.gpio;
    ptr_data->di_permissive_active_high = c.di_permissive.active_high;
    ptr_data->permissive_bypass = c.permissive_bypass;
    ptr_data->annex_m_trip_enabled = c.annex_m_trip_monitor.enabled;
    ptr_data->annex_m_trip_gpio = c.annex_m_trip_monitor.gpio;
    ptr_data->mms_enabled = c.mms.enabled;
    ptr_data->mms_tls_enabled = c.mms.tls.enabled;
    ptr_data->mms_tcp_port = c.mms.tcp_port;
    ptr_data->iec104_enabled = c.iec104.enabled;
    ptr_data->iec104_tls_enabled = c.iec104.tls.enabled;
    ptr_data->goose_enabled = c.goose.enabled;
    ptr_data->goose_timeout_s = c.goose.timeout_s;
    ptr_data->modbus_trace = c.modbus.trace;
    return c.loaded_from_file;
}

/*-----------------------------------------------------------------------------
 * O.14 event log
 *-----------------------------------------------------------------------------*/
void CcliBridge_Event_Open(void) {
    const auto& c = g_rt.cfg;
    g_rt.measurements = std::make_unique<cci::core::MeasurementStore>();
    g_rt.elog_cfg.enabled = c.event_log.enabled;
    g_rt.elog_cfg.path = c.event_log.path;
    g_rt.elog_cfg.syslog_enabled = c.event_log.syslog_enabled;
    g_rt.elog_cfg.syslog_host = c.event_log.syslog_host;
    g_rt.elog_cfg.syslog_port = c.event_log.syslog_port;
    g_rt.events = std::make_unique<cci::core::EventStore>(g_rt.elog_cfg);
    g_rt.version_full = cci::core::version::current().full;
}

void CcliBridge_Event_Append(const ascii* type, const ascii* detail) {
    append_event(type != nullptr ? type : "", detail != nullptr ? detail : "");
}

void CcliBridge_Event_RecordFirmwareBoot(void) {
    const auto fw =
        cci::core::EventStore::record_firmware_boot(g_rt.elog_cfg.path, g_rt.version_full);
    append_event("system", fw.detail);
}

void CcliBridge_Event_LogSegmentation(void) {
    const auto np = cci::core::lab_segmentation_policy();
    if (!cci::core::is_forward_allowed(np, cci::core::EthDomain::EthA,
                                       cci::core::EthDomain::EthB)) {
        append_event("net", "segmentation_deny:EthA->EthB");
    }
    if (!cci::core::is_forward_allowed(np, cci::core::EthDomain::EthB,
                                       cci::core::EthDomain::EthA)) {
        append_event("net", "segmentation_deny:EthB->EthA");
    }
}

const ascii* CcliBridge_Version_Full(void) { return g_rt.version_full.c_str(); }

/*-----------------------------------------------------------------------------
 * Runtime objects
 *-----------------------------------------------------------------------------*/
void CcliBridge_Runtime_Create(void) {
    auto& c = g_rt.cfg;
    g_rt.modbus = std::make_unique<cci::services::ModbusService>(*g_rt.measurements, c);

    /* Phase 1 CEI mock: optional rewrite of PF2 kW thresholds from O.8.2 / O.11 (core/dso/). */
    g_rt.pf2_cfg = c.pf2;
    g_rt.dso_apply = cci::core::dso::apply_dso_to_pf2_config(g_rt.pf2_cfg, c.plant, c.dso);
    const auto& d = g_rt.dso_apply;
    if (d.applied) {
        std::cerr << "dso-phase1: O.8.2/O.11 mock — Smax_calc=" << d.derived.smax_kva_calc
                  << " Smax_used=" << d.derived.smax_kva_used << " kVA"
                  << " p_w110=" << d.derived.p_w110_kw << " p_wlim=" << d.derived.p_wlim_kw
                  << " p_wsd=" << d.derived.p_wsd_kw
                  << " p_eff=" << d.derived.p_effective_export_kw
                  << " kW → pf2 enter=" << g_rt.pf2_cfg.threshold_kw
                  << " release=" << g_rt.pf2_cfg.release_threshold_kw << " kW\n";
        append_event("dso", "polygon_seed smax_kva=" + std::to_string(d.derived.smax_kva_used) +
                                " enter_kw=" + std::to_string(g_rt.pf2_cfg.threshold_kw));
    }

    g_rt.pf2 = std::make_unique<cci::services::Pf2Service>(*g_rt.measurements, *g_rt.events,
                                                           g_rt.pf2_cfg);
    g_rt.mms = std::make_unique<cci::services::MmsService>();
    g_rt.iec104 = std::make_unique<cci::services::Iec104Service>();
    g_rt.goose = std::make_unique<cci::services::GooseService>();

    g_rt.watch_ifaces = cci::core::split_watch_ifaces(c.event_log.watch_ifaces);
    g_rt.last_iface_state.assign(g_rt.watch_ifaces.size(), std::string());
}

void CcliBridge_Runtime_Destroy(void) {
    g_rt.goose.reset();
    g_rt.iec104.reset();
    g_rt.mms.reset();
    g_rt.pf2.reset();
    g_rt.modbus.reset();
    g_rt.events.reset();
    g_rt.measurements.reset();
}

void CcliBridge_PrintStartedBanner(const ascii* config_path, bool lab_demo) {
    const auto ver = cci::core::version::current();
    std::cerr << "ccli " << ver.full << " (" << ver.git_sha << ") starting"
              << " config=" << config_path;
    if (lab_demo) {
        std::cerr << " lab-demo=on";
    }
    std::cerr << '\n';
    std::cout << "ccli " << ver.full << " started. config=" << config_path << '\n' << std::flush;
}

void CcliBridge_PrintStopped(void) { std::cout << "ccli stopping\n"; }

/* Modbus master */
void CcliBridge_Modbus_Tick(void) { g_rt.modbus->tick(); }

void CcliBridge_Modbus_SetSimulatedPowerKw(double p_kw) {
    g_rt.modbus->set_simulated_power_kw(p_kw);
}

void CcliBridge_Modbus_LastExchange(CCLI_BRIDGE__MODBUS_EXCHANGE* ptr_data) {
    const auto& ex = g_rt.modbus->last_exchange();
    ptr_data->valid = ex.valid;
    ptr_data->success = ex.success;
    ptr_data->exchange_ms = ex.exchange_ms;
    copy_cstr(ptr_data->error, sizeof(ptr_data->error), ex.error);
}

void CcliBridge_Modbus_Trace(bool curtailment_commanded, bool do_curtail_on) {
    const auto& ex = g_rt.modbus->last_exchange();
    if (!ex.valid || ex.exchange_ms == g_rt.last_mb_trace_ms) {
        return;
    }
    g_rt.last_mb_trace_ms = ex.exchange_ms;
    const auto tr = cci::program::modbus_trace_from(ex);
    const cci::core::Measurement m = g_rt.measurements->snapshot();
    std::cerr << "modbus: " << tr.command;
    if (!tr.raw_regs_hex.empty()) {
        std::cerr << " raw=" << tr.raw_regs_hex;
    }
    std::cerr << " P=" << m.p_kw << "kW pf2=" << g_rt.pf2->last_state().reason
              << " curtail=" << (curtailment_commanded ? "yes" : "no")
              << " DO=" << (do_curtail_on ? "ON" : "OFF") << "\n";
}

/* measurement store */
void CcliBridge_Measurement_Get(CCLI_BRIDGE__MEASUREMENT* ptr_data) {
    const cci::core::Measurement m = g_rt.measurements->snapshot();
    ptr_data->p_kw = m.p_kw;
    ptr_data->q_kvar = m.q_kvar;
    /* P5-PPV: meter line voltage when modbus.reg_line_voltage is set, else plant constant. */
    ptr_data->ppv_from_meter = m.has_ppv_kv;
    ptr_data->ppv_kv = m.has_ppv_kv ? m.ppv_kv : g_rt.cfg.plant.poc_ppv_kv;
    switch (m.quality) {
        case cci::core::DataQuality::Good:
            ptr_data->quality = CCLI_BRIDGE__QUALITY__GOOD;
            break;
        case cci::core::DataQuality::Stale:
            ptr_data->quality = CCLI_BRIDGE__QUALITY__STALE;
            break;
        default:
            ptr_data->quality = CCLI_BRIDGE__QUALITY__INVALID;
            break;
    }
}

/* PF2 FSM */
void CcliBridge_Pf2_Tick(void) { g_rt.pf2->tick(); }

bool CcliBridge_Pf2_CurtailmentActive(void) { return g_rt.pf2->curtailment_active(); }

/* Eth_A IEC 61850 MMS */
bool CcliBridge_Mms_Start(void) {
    const auto& c = g_rt.cfg;
    cci::core::EventStore* events = g_rt.events.get();
    const cci::adapters::MmsAuditFn mms_audit = [events](const std::string& detail) {
        events->append({cci::hal::now().epoch_ms, "mms", detail});
    };
    if (!g_rt.mms->start(c.mms, mms_audit)) {
        std::cerr << "mms: FAILED to bind " << c.mms.bind_address << ":" << c.mms.tcp_port
                  << (c.mms.tls.enabled ? " (TLS)" : "")
                  << " — disable TesPro IEC61850 LuCI or check LAN1 IP/certs\n";
        return false;
    }
    if (c.goose.publish_enabled) {
        g_rt.mms->enable_goose_publishing(c.goose);
    }
    return true;
}

void CcliBridge_Mms_Stop(void) { g_rt.mms->stop(); }

void CcliBridge_Mms_PublishMeasurement(double p_kw, double q_kvar, double ppv_kv) {
    g_rt.mms->update_tot_w_kw(p_kw);
    g_rt.mms->update_totvar_kvar(q_kvar);
    g_rt.mms->update_ppv_kv(ppv_kv);
}

void CcliBridge_Mms_RefreshTimeQuality(void) { g_rt.mms->refresh_time_quality(); }

bool CcliBridge_Mms_GnssFix(void) { return g_rt.mms->gnss_fix(); }

bool CcliBridge_Mms_PollCommsLossFallback(void) { return g_rt.mms->poll_comms_loss_fallback(); }

void CcliBridge_Mms_PollAndApplyDsoCommand(void) {
    cci::adapters::DsoLiveCommand dso_cmd{};
    if (!g_rt.mms->poll_dso_live_command(dso_cmd) || !dso_cmd.valid) {
        return;
    }
    g_rt.last_dso_cmd = dso_cmd;
    g_rt.dso_cmd_seen = true;
    apply_dso_live_command(dso_cmd);
}

/* Eth_B IEC 60870-5-104 */
bool CcliBridge_Iec104_Start(void) {
    const auto& c = g_rt.cfg;
    if (!g_rt.iec104->start(c.iec104)) {
        std::cerr << "iec104: FAILED to bind " << c.iec104.bind_address << ":"
                  << c.iec104.tcp_port << " — check LAN2 IP / port 2404 free\n";
        return false;
    }
    return true;
}

void CcliBridge_Iec104_Stop(void) { g_rt.iec104->stop(); }

void CcliBridge_Iec104_UpdateTotWKw(double p_kw) { g_rt.iec104->update_tot_w_kw(p_kw); }

void CcliBridge_Iec104_Tick(void) { g_rt.iec104->tick(); }

bool CcliBridge_Iec104_PollCommsLossFallback(void) {
    return g_rt.iec104->poll_comms_loss_fallback();
}

bool CcliBridge_Iec104_PollOperatorAudit(ascii* detail, u32 max_len) {
    cci::adapters::Iec104Adapter::OperatorAuditEvent oa{};
    if (!g_rt.iec104->poll_operator_audit(oa) || !oa.valid) {
        return false;
    }
    const std::string text =
        oa.detail.empty() ? (oa.rejected ? "operator_asdu_reject" : "operator_asdu_accept")
                          : oa.detail;
    copy_cstr(detail, max_len, text);
    return true;
}

/* plant LAN GOOSE */
bool CcliBridge_Goose_Start(void) {
    const auto& c = g_rt.cfg;
    cci::core::EventStore* events = g_rt.events.get();
    const auto log_goose = [events](const std::string& cat, const std::string& detail) {
        events->append({cci::hal::now().epoch_ms, cat, detail});
    };
    if (!g_rt.goose->start(c.goose, log_goose)) {
        std::cerr << "goose: FAILED to start on " << c.goose.interface
                  << " — check plant LAN / goCbRef / libiec61850 GOOSE build\n";
        return false;
    }
    return true;
}

void CcliBridge_Goose_Stop(void) { g_rt.goose->stop(); }

bool CcliBridge_Goose_IsRunning(void) { return g_rt.goose->is_running(); }

s64 CcliBridge_Goose_LastRxMs(void) { return g_rt.goose->last_rx_ms(); }

/* network interfaces watch */
void CcliBridge_Iface_WatchTask(void) {
    for (std::size_t i = 0; i < g_rt.watch_ifaces.size(); ++i) {
        const std::string st = cci::core::read_iface_operstate(g_rt.watch_ifaces[i]);
        if (st != g_rt.last_iface_state[i]) {
            const std::string prev = g_rt.last_iface_state[i];
            g_rt.last_iface_state[i] = st;
            if (!prev.empty() || st == "down" || st == "up") {
                append_event("iface", g_rt.watch_ifaces[i] + ":" + st);
            }
        }
    }
}

/* bench status file */
void CcliBridge_Status_Write(const CCLI_BRIDGE__STATUS* ptr_data) {
    cci::program::ZoneStatusSnapshot zs{};
    zs.mms_running = g_rt.mms->is_running();
    zs.mms_clients = g_rt.mms->client_count();
    zs.gnss_fix = g_rt.mms->gnss_fix();
    zs.dso_cmd_seen = g_rt.dso_cmd_seen;
    zs.dso_cmd = g_rt.last_dso_cmd;
    zs.iec104_running = g_rt.iec104->is_running();
    zs.iec104_clients = g_rt.iec104->client_count();
    zs.modbus_link_state = ptr_data->modbus_link_state;
    zs.modbus_last = g_rt.modbus->last_exchange();
    cci::program::write_bench_status_file(
        cci::program::kBenchStatusPath, g_rt.cfg, g_rt.measurements->snapshot(),
        g_rt.pf2->last_state(), ptr_data->curtailment_commanded, ptr_data->permissive_ok,
        ptr_data->annex_m_trip_active, ptr_data->do_curtail_on, g_rt.dso_apply, zs);
}

}  // extern "C"
