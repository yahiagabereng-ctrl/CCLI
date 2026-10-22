#include "svc_goose.hpp"
#include "svc_iec104.hpp"
#include "svc_mms.hpp"
#include "svc_modbus.hpp"
#include "svc_pf2.hpp"
#include "svc_io.h"

#include "cci/hal/platform.hpp"
#include "cci/hal/time.hpp"
#include "config/ccli_config.hpp"
#include "dso/dso_phase1.hpp"
#include "drv_gpio.h"
#include "policy/net_policy.hpp"
#include "event/event_store.hpp"
#include "event/link_monitor.hpp"
#include "regulation/regulation_bench.hpp"
#include "service/service_supervision.hpp"
#include "version/ccli_version.hpp"

#include "modbus_adapter.hpp"

#include "ccli_tools.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <vector>
#include <csignal>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace cci::program;

namespace {

std::atomic<bool> g_running{true};

}  // namespace

int main(int argc, char** argv) {
    std::string config_path = "/etc/ccli/lab.yaml";
    bool gpio_test = false;
    bool lab_demo = false;
    bool regulation_check = false;
    bool regulation_live = false;
    bool regulation_json = false;
    int live_watch_sec = 0;
    bool bench_status = false;
    bool bench_write_config = false;
    bool event_dump = false;
    bool event_wrap_test = false;
    bool event_clear = false;
    bool event_dump_json = false;
    std::size_t event_dump_count = cci::core::EventStore::kMaxEvents;
    int gpio_blink = 0;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--version") {
            cci::core::version::print_text(std::cout);
            return 0;
        }
        if (arg == "--version-json") {
            cci::core::version::print_json(std::cout);
            return 0;
        }
        if (arg == "--help" || arg == "-h") {
            print_usage();
            return 0;
        }
        if (arg == "--regulation-check") {
            regulation_check = true;
            continue;
        }
        if (arg == "--live") {
            regulation_live = true;
            continue;
        }
        if (arg == "--json") {
            regulation_json = true;
            event_dump_json = true;
            continue;
        }
        if (arg == "--live-watch" && i + 1 < argc) {
            try {
                live_watch_sec = std::stoi(argv[++i]);
            } catch (const std::exception&) {
                live_watch_sec = 0;
            }
            if (live_watch_sec < 0) {
                live_watch_sec = 0;
            }
            continue;
        }
        if (arg == "--gpio-test") {
            gpio_test = true;
            continue;
        }
        if (arg == "--lab-demo") {
            lab_demo = true;
            continue;
        }
        if (arg == "--gpio-blink" && i + 1 < argc) {
            try {
                gpio_blink = std::stoi(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "ccli: invalid --gpio-blink value, ignoring\n";
                gpio_blink = 0;
            }
            if (gpio_blink < 0) {
                gpio_blink = 0;
            }
            continue;
        }
        if (arg == "--bench-status") {
            bench_status = true;
            continue;
        }
        if (arg == "--bench-write-config" && i + 1 < argc) {
            bench_write_config = true;
            config_path = argv[++i];
            continue;
        }
        if (arg == "--event-dump") {
            event_dump = true;
            continue;
        }
        if (arg == "--event-wrap-test") {
            event_wrap_test = true;
            continue;
        }
        if (arg == "--event-clear") {
            event_clear = true;
            continue;
        }
        if (arg == "--count" && i + 1 < argc) {
            try {
                event_dump_count = static_cast<std::size_t>(std::stoul(argv[++i]));
            } catch (const std::exception&) {
                event_dump_count = cci::core::EventStore::kMaxEvents;
            }
            continue;
        }
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
            continue;
        }
    }

    if (event_clear) {
        std::cerr << "O.14: user overwrite forbidden (P7-01) — no erase API\n";
        return cci::core::EventStore::user_overwrite_forbidden() ? 2 : 1;
    }
    if (event_wrap_test) {
        return run_event_wrap_test();
    }
    if (event_dump) {
        return run_event_dump(config_path, event_dump_count, event_dump_json);
    }
    if (bench_status) {
        return run_bench_status_json();
    }
    if (bench_write_config) {
        return run_bench_write_config(config_path);
    }
    if (regulation_check) {
        return run_regulation_check(config_path, regulation_live, regulation_json, live_watch_sec);
    }

    if (gpio_test) {
        return run_gpio_test(&g_running);
    }

    if (!cci::hal::platform_init()) {
        std::cerr << "platform_init failed\n";
        return 1;
    }

    const auto policy = cci::core::lab_segmentation_policy();
    (void)policy;

    // Load runtime configuration (PF2 thresholds + GPIO map) from the deployed
    // YAML. Falls back to safe compiled defaults if the file is missing.
    cci::core::CcliConfig app_cfg = cci::core::load_config(config_path);
    if (lab_demo) {
        app_cfg.modbus.backend = cci::core::ModbusBackendKind::Simulator;
    }
    std::cout << "ccli config " << (app_cfg.loaded_from_file ? "loaded from " : "defaults (no ")
              << config_path << (app_cfg.loaded_from_file ? "" : ")") << '\n';

    DrvGpioConfig gcfg{};
    gcfg.outputs[DRVGPIO__OUT__CURTAIL].line = app_cfg.do_curtail.gpio;
    gcfg.outputs[DRVGPIO__OUT__CURTAIL].active_high = app_cfg.do_curtail.active_high;
    gcfg.inputs[DRVGPIO__IN__PERMISSIVE].line = app_cfg.di_permissive.gpio;
    gcfg.inputs[DRVGPIO__IN__PERMISSIVE].active_high = app_cfg.di_permissive.active_high;

    // DI/DO stack: initialize to safe state (all outputs de-energized) before
    // any protocol activity. IEC 62443-4-2 CR 3.6 direction.
    if (!svc_io_init_cfg(&gcfg)) {
        std::cerr << "svc_io_init failed\n";
        return 1;
    }

    cci::core::ServiceSupervision::set_safe_state_hook(svc_io_safe_state);
    cci::core::ServiceSupervision::install_handlers(&g_running);

    if (app_cfg.permissive_bypass) {
        std::cerr << "io: WARNING permissive_bypass=on — DI gate disabled (lab only)\n";
    }

    svc_io_configure_annex_m_trip_monitor(app_cfg.annex_m_trip_monitor.gpio,
                                          app_cfg.annex_m_trip_monitor.enabled);

    if (gpio_blink > 0) {
        gpio_blink_curtail(gpio_blink, g_running);
    }

    cci::core::MeasurementStore measurements;
    cci::core::EventLogConfig elog_cfg;
    elog_cfg.enabled = app_cfg.event_log.enabled;
    elog_cfg.path = app_cfg.event_log.path;
    elog_cfg.syslog_enabled = app_cfg.event_log.syslog_enabled;
    elog_cfg.syslog_host = app_cfg.event_log.syslog_host;
    elog_cfg.syslog_port = app_cfg.event_log.syslog_port;
    cci::core::EventStore events(elog_cfg);
    const auto log_event = [&events](const std::string& type, const std::string& detail) {
        events.append({cci::hal::now().epoch_ms, type, detail, ""});
    };
    const auto boot_ver = cci::core::version::current();
    {
        const auto recovery = cci::core::ServiceSupervision::startup_check();
        if (recovery.crash_recovered) {
            const std::string sig =
                cci::core::ServiceSupervision::signum_name(recovery.crash_signum);
            log_event("security",
                      "crash_recovered:" + sig + " signum=" +
                          std::to_string(recovery.crash_signum));
            std::cerr << "security: previous crash recovered (" << sig << ")\n";
        }
        if (recovery.unclean_restart && !recovery.crash_recovered) {
            log_event("security", "unclean_restart:stale_running_marker");
            std::cerr << "security: unclean restart detected (stale running marker)\n";
        }
    }
    log_event("system", "power_on:" + boot_ver.full);
    {
        const auto fw = cci::core::EventStore::record_firmware_boot(elog_cfg.path, boot_ver.full);
        log_event("system", fw.detail);
    }
    log_event("system", "psu_unmonitored");
    log_event("security", "service_monitor:started");
    log_event("security", "tls_material:loaded_from_config");
    {
        const auto np = cci::core::lab_segmentation_policy();
        if (!cci::core::is_forward_allowed(np, cci::core::EthDomain::EthA,
                                           cci::core::EthDomain::EthB)) {
            log_event("net", "segmentation_deny:EthA->EthB");
        }
        if (!cci::core::is_forward_allowed(np, cci::core::EthDomain::EthB,
                                           cci::core::EthDomain::EthA)) {
            log_event("net", "segmentation_deny:EthB->EthA");
        }
    }
    cci::core::ServiceSupervision::mark_running();

    cci::services::ModbusService modbus(measurements, app_cfg);

    cci::core::Pf2Config pf2_cfg = app_cfg.pf2;

    /* Phase 1 CEI mock: optional rewrite of PF2 kW thresholds from O.8.2 / O.11 (core/dso/). */
    const auto dso_apply =
        cci::core::dso::apply_dso_to_pf2_config(pf2_cfg, app_cfg.plant, app_cfg.dso);
    if (dso_apply.applied) {
        std::cerr << "dso-phase1: O.8.2/O.11 mock — Smax_calc=" << dso_apply.derived.smax_kva_calc
                  << " Smax_used=" << dso_apply.derived.smax_kva_used << " kVA"
                  << " p_w110=" << dso_apply.derived.p_w110_kw
                  << " p_wlim=" << dso_apply.derived.p_wlim_kw
                  << " p_wsd=" << dso_apply.derived.p_wsd_kw
                  << " p_eff=" << dso_apply.derived.p_effective_export_kw
                  << " kW → pf2 enter=" << pf2_cfg.threshold_kw
                  << " release=" << pf2_cfg.release_threshold_kw << " kW\n";
        log_event("dso",
                  "polygon_seed smax_kva=" + std::to_string(dso_apply.derived.smax_kva_used) +
                      " enter_kw=" + std::to_string(pf2_cfg.threshold_kw));
    }

    cci::services::Pf2Service pf2(measurements, events, pf2_cfg);
    cci::services::MmsService mms;
    cci::services::Iec104Service iec104;
    cci::services::GooseService goose;

    modbus.tick();
    pf2.tick();
    // Curtailment DO is driven by the PF2 decision AND gated by the external
    // permissive input: no actuation unless the plant permissive allows it
    // (CCLI-VAL-001 §7 "output policy permits actuation").
    bool permissive0 = false;
    if (app_cfg.permissive_bypass) {
        permissive0 = true;
    } else {
        svc_io_read_permissive(&permissive0);
    }
    bool annex_m0 = false;
    if (app_cfg.annex_m_trip_monitor.enabled) {
        (void)svc_io_read_annex_m_trip_active(&annex_m0);
    }
    bool do_state = pf2.curtailment_active() && permissive0 && !annex_m0;
    svc_io_apply_curtailment(do_state);
    events.append({cci::hal::now().epoch_ms, "do",
                   do_state ? "curtail_on" : "curtail_off"});
    if (app_cfg.mms.enabled) {
        const cci::adapters::MmsAuditFn mms_audit =
            [&events](const std::string& detail) {
                events.append({cci::hal::now().epoch_ms, "mms", detail});
            };
        if (!mms.start(app_cfg.mms, mms_audit)) {
            std::cerr << "mms: FAILED to bind " << app_cfg.mms.bind_address << ":"
                      << app_cfg.mms.tcp_port
                      << (app_cfg.mms.tls.enabled ? " (TLS)" : "")
                      << " — disable TesPro IEC61850 LuCI or check LAN1 IP/certs\n";
        } else if (app_cfg.goose.publish_enabled) {
            mms.enable_goose_publishing(app_cfg.goose);
        }
    } else {
        std::cerr << "mms: disabled in config\n";
    }

    if (app_cfg.iec104.enabled) {
        if (!iec104.start(app_cfg.iec104)) {
            std::cerr << "iec104: FAILED to bind " << app_cfg.iec104.bind_address << ":"
                      << app_cfg.iec104.tcp_port << " — check LAN2 IP / port 2404 free\n";
        }
    } else {
        std::cerr << "iec104: disabled in config\n";
    }

    if (app_cfg.goose.enabled) {
        const auto log_goose = [&events](const std::string& cat, const std::string& detail) {
            events.append({cci::hal::now().epoch_ms, cat, detail});
        };
        if (!goose.start(app_cfg.goose, log_goose)) {
            std::cerr << "goose: FAILED to start on " << app_cfg.goose.interface
                      << " — check plant LAN / goCbRef / libiec61850 GOOSE build\n";
        }
    } else {
        std::cerr << "goose: disabled in config (plant Modbus path active)\n";
    }

    const auto ver = cci::core::version::current();
    std::cerr << "ccli " << ver.full << " (" << ver.git_sha << ") starting"
              << " config=" << config_path;
    if (lab_demo) {
        std::cerr << " lab-demo=on";
    }
    std::cerr << '\n';
    std::cout << "ccli " << ver.full << " started. config=" << config_path << '\n' << std::flush;

    std::int64_t loop_count = 0;
    const std::int64_t status_interval = 50; /* 50 * 100ms = 5s */
    const std::int64_t watchdog_interval =
        cci::core::ServiceSupervision::kDefaultWatchdogPingS * 10; /* 100ms loop */
    std::int64_t last_mb_trace_ms = -1;
    int modbus_link_state = 0; /* 0=unknown, 1=up, -1=down */
    cci::core::DataQuality last_meter_quality = cci::core::DataQuality::Invalid;
    bool permissive_tracked = false;
    bool last_permissive_di = false;
    bool last_annex_m_trip = annex_m0;
    cci::adapters::DsoLiveCommand last_dso_cmd{};
    bool dso_cmd_seen = false;
    int goose_link_state = 0; /* 0=unknown, 1=up, -1=down */
    const auto watch_ifaces = cci::core::split_watch_ifaces(app_cfg.event_log.watch_ifaces);
    std::vector<std::string> last_iface_state(watch_ifaces.size());

    while (g_running) {
        if (lab_demo) {
            /* Triangle wave 400..950 kW over ~120s to cross PF2 threshold (900 kW). */
            const auto t = loop_count / 10;
            const double phase = static_cast<double>(t % 120);
            const double p_kw = 400.0 + 550.0 * std::fabs(1.0 - std::fabs(phase / 60.0 - 1.0));
            modbus.set_simulated_power_kw(p_kw);
        }

        modbus.tick();
        pf2.tick();

        {
            const auto& mb = modbus.last_exchange();
            if (mb.valid) {
                if (mb.success) {
                    if (modbus_link_state <= 0) {
                        log_event("modbus",
                                  modbus_link_state < 0 ? "link_recovered" : "link_up");
                        modbus_link_state = 1;
                    }
                } else if (modbus_link_state >= 0) {
                    log_event("modbus", "link_down:" + mb.error);
                    modbus_link_state = -1;
                }
            }
        }

        {
            const cci::core::Measurement m = measurements.snapshot();
            if (m.quality != last_meter_quality) {
                if (m.quality == cci::core::DataQuality::Good) {
                    log_event("meter", last_meter_quality == cci::core::DataQuality::Stale
                                             ? "quality_recovered"
                                             : "quality_good");
                } else if (m.quality == cci::core::DataQuality::Stale) {
                    log_event("meter", "quality_stale");
                } else {
                    log_event("meter", "quality_invalid");
                }
                last_meter_quality = m.quality;
            }
            mms.update_tot_w_kw(m.p_kw);
            mms.update_totvar_kvar(m.q_kvar);
            mms.update_ppv_kv(app_cfg.plant.poc_ppv_kv);
            iec104.update_tot_w_kw(m.p_kw);
        }
        mms.refresh_time_quality();
        iec104.tick();

        /* Eth_A: DSO Wlim (O.9.2.2) + WSd (O.9.2.3 Figura 2 s.p. W) → PF2 */
        /* P3-05: no-comms → clear live Wlim/WSd (Operating Rule autonomous) */
        {
            const bool or_fallback_latched = mms.operating_rule_fallback_latched();
            if (mms.poll_comms_loss_fallback()) {
                std::cerr << "mms→pf2: P3-05 Operating Rule fallback (Eth_A no-comms)\n";
                events.append({cci::hal::now().epoch_ms, "mms", "comms_loss_fallback", ""});
                log_event("priority", "autonomous_after_eth_a_loss");
            }
            cci::adapters::DsoLiveCommand dso_cmd{};
            if (mms.poll_dso_live_command(dso_cmd) && dso_cmd.valid) {
                last_dso_cmd = dso_cmd;
                dso_cmd_seen = true;
                if (dso_cmd.dirty) {
                    cci::core::Pf2Config live_cfg = pf2.config();
                    live_cfg.use_dso_mock = true;
                    const bool use_or_profile =
                        or_fallback_latched || mms.operating_rule_fallback_latched();
                    const auto live = cci::core::dso::apply_live_dso_commands_to_pf2(
                        live_cfg, app_cfg.plant, app_cfg.dso, dso_cmd.wlim_active,
                        dso_cmd.wmax_spt_pct, dso_cmd.wsd_active, dso_cmd.wspt_pct,
                        use_or_profile);
                    if (live.applied) {
                        pf2.set_thresholds(live.pf2_threshold_kw, live.pf2_release_kw);
                        if (use_or_profile) {
                            std::cerr << "mms→pf2: O.13.1.2 Operating Rule (O.11 min) — Wlim active="
                                      << (live.derived.p_wlim_kw > 0.0 ? "yes" : "no")
                                      << " WMaxSptPct=" << (live.derived.p_wlim_kw > 0.0 ? "set" : "0")
                                      << "% WSd active=" << (live.derived.p_wsd_kw > 0.0 ? "yes" : "no")
                                      << " … enter=" << live.pf2_threshold_kw
                                      << " release=" << live.pf2_release_kw << " kW\n";
                        } else {
                            std::cerr << "mms→pf2: Wlim=" << (dso_cmd.wlim_active ? "on" : "off")
                                      << "@" << dso_cmd.wmax_spt_pct
                                      << "% WSd=" << (dso_cmd.wsd_active ? "on" : "off")
                                      << "@" << dso_cmd.wspt_pct
                                      << "% p_wlim=" << live.derived.p_wlim_kw
                                      << " p_wsd=" << live.derived.p_wsd_kw
                                      << " p_eff=" << live.derived.p_effective_export_kw
                                      << " kW → enter=" << live.pf2_threshold_kw
                                      << " release=" << live.pf2_release_kw << " kW\n";
                        }
                        events.append({cci::hal::now().epoch_ms, "mms",
                                       use_or_profile          ? "operating_rule_autonomous"
                                       : dso_cmd.wsd_active   ? "wsd_update"
                                       : dso_cmd.wlim_active ? "wlim_on"
                                                            : "wlim_off"});
                    }
                }
                if (dso_cmd.reactive_dirty) {
                    const auto m_snap = measurements.snapshot();
                    const double p_kw = m_snap.p_kw;

                    if (dso_cmd.pfsp_dirty) {
                        const auto q_pfsp = cci::core::dso::apply_live_pfsp_command(
                            app_cfg.plant, app_cfg.dso, dso_cmd.pfsp_active,
                            dso_cmd.pfsp_cosphi, dso_cmd.pfsp_generation, p_kw);
                        const int mod_val = dso_cmd.pfsp_active ? 1 : 5;
                        if (q_pfsp.applied) {
                            if (modbus.write_reactive_kvar(q_pfsp.derived.q_target_kvar)) {
                                std::cerr << "mms→plant: PFSP on cosφ="
                                          << dso_cmd.pfsp_cosphi << " P=" << p_kw
                                          << " kW → Q=" << q_pfsp.derived.q_target_kvar
                                          << " kvar [O.9.1.1 P5-R02]\n";
                                events.append({cci::hal::now().epoch_ms, "mms",
                                               "pfsp_operate mod=" + std::to_string(mod_val) +
                                                   " cosphi=" +
                                                   std::to_string(dso_cmd.pfsp_cosphi) +
                                                   " p_kw=" + std::to_string(p_kw) +
                                                   " q_kvar=" +
                                                   std::to_string(q_pfsp.derived.q_target_kvar) +
                                                   " result=ok"});
                            } else {
                                events.append({cci::hal::now().epoch_ms, "mms",
                                               "pfsp_operate mod=" + std::to_string(mod_val) +
                                                   " cosphi=" +
                                                   std::to_string(dso_cmd.pfsp_cosphi) +
                                                   " p_kw=" + std::to_string(p_kw) +
                                                   " q_kvar=" +
                                                   std::to_string(q_pfsp.derived.q_target_kvar) +
                                                   " result=plant_write_fail"});
                            }
                        } else {
                            events.append({cci::hal::now().epoch_ms, "mms",
                                           "pfsp_operate mod=5 cosphi=" +
                                               std::to_string(dso_cmd.pfsp_cosphi) +
                                               " q_kvar=0 result=cleared"});
                        }
                    }

                    if (!dso_cmd.pfsp_dirty) {
                        const auto q_cmd = cci::core::dso::apply_live_varsd_command(
                            app_cfg.plant, app_cfg.dso, dso_cmd.varsd_active,
                            dso_cmd.vartgt_spt_pct);
                        if (q_cmd.applied) {
                            const int mod_val = dso_cmd.varsd_active ? 1 : 5;
                            if (modbus.write_reactive_kvar(q_cmd.derived.q_target_kvar)) {
                                std::cerr << "mms→plant: VArSd on @" << dso_cmd.vartgt_spt_pct
                                          << "% Smax → Q=" << q_cmd.derived.q_target_kvar
                                          << " kvar [O.9.1.4 P5-R01]\n";
                                events.append({cci::hal::now().epoch_ms, "mms",
                                               "varsd_operate mod=" + std::to_string(mod_val) +
                                                   " pct=" +
                                                   std::to_string(dso_cmd.vartgt_spt_pct) +
                                                   " q_kvar=" +
                                                   std::to_string(q_cmd.derived.q_target_kvar) +
                                                   " result=ok"});
                            } else {
                                std::cerr << "mms→plant: VArSd Q write FAILED q="
                                          << q_cmd.derived.q_target_kvar << " kvar\n";
                                events.append({cci::hal::now().epoch_ms, "mms",
                                               "varsd_operate mod=" + std::to_string(mod_val) +
                                                   " pct=" +
                                                   std::to_string(dso_cmd.vartgt_spt_pct) +
                                                   " q_kvar=" +
                                                   std::to_string(q_cmd.derived.q_target_kvar) +
                                                   " result=plant_write_fail"});
                            }
                        } else {
                            std::cerr << "mms→plant: VArSd off — plant Q command cleared "
                                         "[O.9.1.4]\n";
                            events.append({cci::hal::now().epoch_ms, "mms",
                                           "varsd_operate mod=5 pct=" +
                                               std::to_string(dso_cmd.vartgt_spt_pct) +
                                               " q_kvar=0 result=cleared"});
                        }
                    }
                }
            }
        }

        if (iec104.poll_comms_loss_fallback()) {
            std::cerr << "iec104: P4-04 Eth_B comms-loss fallback\n";
            events.append({cci::hal::now().epoch_ms, "iec104", "comms_loss_fallback"});
        }

        {
            cci::adapters::Iec104Adapter::OperatorAuditEvent oa{};
            if (iec104.poll_operator_audit(oa) && oa.valid) {
                const std::string& detail =
                    oa.detail.empty()
                        ? (oa.rejected ? "operator_asdu_reject" : "operator_asdu_accept")
                        : oa.detail;
                std::cerr << "iec104: " << detail << "\n";
                events.append({cci::hal::now().epoch_ms, "iec104", detail});
            }
        }

        const bool commanded = pf2.curtailment_active();
        bool permissive = false;
        const bool permissive_ok =
            app_cfg.permissive_bypass || (svc_io_read_permissive(&permissive) && permissive);

        bool annex_m_trip = false;
        if (app_cfg.annex_m_trip_monitor.enabled) {
            (void)svc_io_read_annex_m_trip_active(&annex_m_trip);
            if (annex_m_trip != last_annex_m_trip) {
                log_event("annex_m",
                          annex_m_trip ? "trip_relay_active" : "trip_relay_cleared");
                last_annex_m_trip = annex_m_trip;
            }
        }

        if (!app_cfg.permissive_bypass) {
            bool permissive_di = false;
            if (svc_io_read_permissive(&permissive_di)) {
                if (!permissive_tracked || permissive_di != last_permissive_di) {
                    log_event("di", permissive_di ? "permissive_ok" : "permissive_blocked");
                    permissive_tracked = true;
                    last_permissive_di = permissive_di;
                }
            }
        }

        // Gate DO by permissive and Annex M trip (O.11 — no conflicting PF2 actuation).
        const bool new_do_state = commanded && permissive_ok && !annex_m_trip;
        if (new_do_state != do_state) {
            svc_io_apply_curtailment(new_do_state);
            const char* detail = new_do_state ? "curtail_on"
                                 : (annex_m_trip ? "curtail_blocked_annex_m"
                                 : (commanded ? "curtail_blocked_permissive" : "curtail_off"));
            events.append({cci::hal::now().epoch_ms, "do", detail});
            std::cerr << "io: DO " << detail << "\n";
            if (annex_m_trip && commanded) {
                events.append({cci::hal::now().epoch_ms, "annex_m", "teledistacco_inhibit"});
            }
            do_state = new_do_state;
        }

        if (app_cfg.goose.enabled && goose.is_running() && app_cfg.goose.timeout_s > 0 &&
            (loop_count % 10) == 0) {
            const auto now_ms = cci::hal::now().epoch_ms;
            const auto last_rx = goose.last_rx_ms();
            const std::int64_t timeout_ms =
                static_cast<std::int64_t>(app_cfg.goose.timeout_s) * 1000;
            const bool heard = last_rx > 0 && (now_ms - last_rx) <= timeout_ms;
            if (heard) {
                if (goose_link_state <= 0) {
                    log_event("goose", goose_link_state < 0 ? "link_recovered" : "link_up");
                    goose_link_state = 1;
                }
            } else if (goose_link_state == 1 ||
                       (goose_link_state == 0 && loop_count * 100 >= timeout_ms)) {
                log_event("goose", last_rx > 0 ? "link_down:timeout" : "link_down:no_rx");
                goose_link_state = -1;
            }
        }

        if ((loop_count % status_interval) == 0) {
            for (std::size_t i = 0; i < watch_ifaces.size(); ++i) {
                const std::string st = cci::core::read_iface_operstate(watch_ifaces[i]);
                if (st != last_iface_state[i]) {
                    const std::string prev = last_iface_state[i];
                    last_iface_state[i] = st;
                    if (!prev.empty() || st == "down" || st == "up") {
                        log_event("iface", watch_ifaces[i] + ":" + st);
                    }
                }
            }
            svc_io_log_status(do_state, permissive_ok, annex_m_trip);
            ZoneStatusSnapshot zs{};
            zs.mms_running = mms.is_running();
            zs.mms_clients = mms.client_count();
            zs.gnss_fix = mms.gnss_fix();
            zs.dso_cmd_seen = dso_cmd_seen;
            zs.dso_cmd = last_dso_cmd;
            zs.iec104_running = iec104.is_running();
            zs.iec104_clients = iec104.client_count();
            zs.modbus_link_state = modbus_link_state;
            zs.modbus_last = modbus.last_exchange();
            write_bench_status_file(kBenchStatusPath, app_cfg, measurements.snapshot(),
                                    pf2.last_state(), commanded, permissive_ok, annex_m_trip,
                                    do_state, dso_apply, zs);
        }

        if (app_cfg.modbus.trace) {
            const auto& ex = modbus.last_exchange();
            if (ex.valid && ex.exchange_ms != last_mb_trace_ms) {
                last_mb_trace_ms = ex.exchange_ms;
                const auto tr = modbus_trace_from(ex);
                const cci::core::Measurement m = measurements.snapshot();
                std::cerr << "modbus: " << tr.command;
                if (!tr.raw_regs_hex.empty()) {
                    std::cerr << " raw=" << tr.raw_regs_hex;
                }
                std::cerr << " P=" << m.p_kw << "kW pf2=" << pf2.last_state().reason
                          << " curtail=" << (commanded ? "yes" : "no")
                          << " DO=" << (do_state ? "ON" : "OFF") << "\n";
            }
        }

        if ((loop_count % watchdog_interval) == 0 && loop_count > 0) {
            cci::core::ServiceSupervision::procd_watchdog_ping();
        }

        ++loop_count;
        usleep(100000);
    }

    log_event("security", "service_monitor:shutdown");
    log_event("system", "power_off:shutdown");
    cci::core::ServiceSupervision::mark_clean_shutdown();
    svc_io_shutdown();
    goose.stop();
    iec104.stop();
    mms.stop();

    std::cout << "ccli stopping\n";
    return 0;
}
