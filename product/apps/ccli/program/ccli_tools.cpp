/*=============================================================================
 * File       :  CCLI_TOOLS.CPP
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - one-shot CLI tools and bench status file (C++)
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/
#include "ccli_tools.hpp"

#include "svc_io.h"
#include "svc_modbus.hpp"
#include "svc_pf2.hpp"

#include "cci/hal/platform.hpp"
#include "event/event_store.hpp"
#include "service/service_supervision.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>

namespace cci::program {

namespace {

std::string quality_string(const cci::core::DataQuality q) {
    switch (q) {
        case cci::core::DataQuality::Good:
            return "good";
        case cci::core::DataQuality::Stale:
            return "stale";
        default:
            return "invalid";
    }
}

std::string json_escape_str(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (const char c : s) {
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(c) >= 0x20) {
                    out += c;
                }
                break;
        }
    }
    return out;
}

const char* modbus_backend_string(const cci::core::ModbusBackendKind k) {
    switch (k) {
        case cci::core::ModbusBackendKind::Rtu:
            return "rtu";
        case cci::core::ModbusBackendKind::Tcp:
            return "tcp";
        default:
            return "simulator";
    }
}

const char* link_state_string(const int s) {
    return s > 0 ? "up" : (s < 0 ? "down" : "unknown");
}

void capture_modbus_log(const cci::adapters::ModbusExchangeRecord& ex,
                        std::vector<cci::core::regulation::ModbusLiveTrace>& log,
                        std::int64_t& last_logged_ms) {
    if (!ex.valid || ex.exchange_ms == 0 || ex.exchange_ms == last_logged_ms) {
        return;
    }
    last_logged_ms = ex.exchange_ms;
    log.push_back(modbus_trace_from(ex));
}

}  // namespace

void write_bench_status_file(const std::string& path, const cci::core::CcliConfig& cfg,
                             const cci::core::Measurement& m, const cci::core::Pf2State& pf2_st,
                             const bool curtailment_commanded, const bool permissive_ok,
                             const bool annex_m_trip_active, const bool do_curtail_on,
                             const cci::core::dso::DsoPhase1ApplyResult& dso_apply,
                             const ZoneStatusSnapshot& z) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return;
    }
    const auto& d = dso_apply.derived;
    const auto& c = z.dso_cmd;
    const auto& orule = cfg.iec104.operator_rule;
    const auto& mb = z.modbus_last;
    out << "{\n"
        << "  \"ts_ms\": " << m.timestamp_ms << ",\n"
        << "  \"p_kw\": " << m.p_kw << ",\n"
        << "  \"q_kvar\": " << m.q_kvar << ",\n"
        << "  \"quality\": \"" << quality_string(m.quality) << "\",\n"
        << "  \"pf2_reason\": \"" << pf2_st.reason << "\",\n"
        << "  \"curtailment_commanded\": " << (curtailment_commanded ? "true" : "false")
        << ",\n"
        << "  \"permissive_ok\": " << (permissive_ok ? "true" : "false") << ",\n"
        << "  \"annex_m_trip_active\": " << (annex_m_trip_active ? "true" : "false")
        << ",\n"
        << "  \"do_curtail_on\": " << (do_curtail_on ? "true" : "false") << ",\n"
        << "  \"dso_applied\": " << (dso_apply.applied ? "true" : "false") << ",\n"
        << "  \"derived\": {\n"
        << "    \"smax_calc_kva\": " << d.smax_kva_calc << ",\n"
        << "    \"smax_used_kva\": " << d.smax_kva_used << ",\n"
        << "    \"p_wsd_kw\": " << d.p_wsd_kw << ",\n"
        << "    \"p_wlim_kw\": " << d.p_wlim_kw << ",\n"
        << "    \"p_w110_kw\": " << d.p_w110_kw << ",\n"
        << "    \"p_effective_kw\": " << d.p_effective_export_kw << ",\n"
        << "    \"pf2_enter_kw\": " << dso_apply.pf2_threshold_kw << ",\n"
        << "    \"pf2_release_kw\": " << dso_apply.pf2_release_kw << "\n"
        << "  },\n"
        << "  \"pf2_debounce_s\": " << cfg.pf2.debounce_s << ",\n"
        << "  \"stale_data_s\": " << cfg.pf2.stale_data_s << ",\n"
        << "  \"zones\": {\n"
        << "    \"dso\": {\n"
        << "      \"enabled\": " << (cfg.mms.enabled ? "true" : "false") << ",\n"
        << "      \"running\": " << (z.mms_running ? "true" : "false") << ",\n"
        << "      \"bind\": \"" << json_escape_str(cfg.mms.bind_address) << "\",\n"
        << "      \"port\": " << cfg.mms.tcp_port << ",\n"
        << "      \"tls\": " << (cfg.mms.tls.enabled ? "true" : "false") << ",\n"
        << "      \"clients\": " << z.mms_clients << ",\n"
        << "      \"gnss_fix\": " << (z.gnss_fix ? "true" : "false") << ",\n"
        << "      \"comms_loss_fallback_s\": " << cfg.mms.comms_loss_fallback_s << ",\n"
        << "      \"setpoint_min_interval_s\": " << cfg.mms.setpoint_min_interval_s << ",\n"
        << "      \"cmd_seen\": " << (z.dso_cmd_seen ? "true" : "false") << ",\n"
        << "      \"wlim_active\": " << (c.wlim_active ? "true" : "false") << ",\n"
        << "      \"wlim_pct\": " << c.wmax_spt_pct << ",\n"
        << "      \"wsd_active\": " << (c.wsd_active ? "true" : "false") << ",\n"
        << "      \"wsd_pct\": " << c.wspt_pct << ",\n"
        << "      \"varsd_active\": " << (c.varsd_active ? "true" : "false") << ",\n"
        << "      \"varsd_pct\": " << c.vartgt_spt_pct << ",\n"
        << "      \"pfsp_active\": " << (c.pfsp_active ? "true" : "false") << ",\n"
        << "      \"pfsp_cosphi\": " << c.pfsp_cosphi << ",\n"
        << "      \"pfsp_generation\": " << (c.pfsp_generation ? "true" : "false") << ",\n"
        << "      \"ppv_kv\": " << cfg.plant.poc_ppv_kv << "\n"
        << "    },\n"
        << "    \"operator\": {\n"
        << "      \"enabled\": " << (cfg.iec104.enabled ? "true" : "false") << ",\n"
        << "      \"running\": " << (z.iec104_running ? "true" : "false") << ",\n"
        << "      \"bind\": \"" << json_escape_str(cfg.iec104.bind_address) << "\",\n"
        << "      \"port\": " << cfg.iec104.tcp_port << ",\n"
        << "      \"tls\": " << (cfg.iec104.tls.enabled ? "true" : "false") << ",\n"
        << "      \"clients\": " << z.iec104_clients << ",\n"
        << "      \"common_address\": " << cfg.iec104.common_address << ",\n"
        << "      \"periodic_s\": " << cfg.iec104.periodic_s << ",\n"
        << "      \"comms_loss_fallback_s\": " << cfg.iec104.comms_loss_fallback_s << ",\n"
        << "      \"mode\": \"" << json_escape_str(orule.mode) << "\",\n"
        << "      \"allow_commands\": " << (orule.allow_commands ? "true" : "false") << ",\n"
        << "      \"allow_gi\": " << (orule.allow_gi ? "true" : "false") << ",\n"
        << "      \"allow_clock_sync\": " << (orule.allow_clock_sync ? "true" : "false") << ",\n"
        << "      \"ioa\": [\n"
        << "        {\"name\": \"TotW\", \"ioa\": " << cfg.iec104.ioa_tot_w
        << ", \"enabled\": " << (orule.monitor_tot_w ? "true" : "false")
        << ", \"published\": "
        << ((orule.monitor_tot_w && z.iec104_running) ? "true" : "false")
        << ", \"value\": " << m.p_kw << ", \"unit\": \"kW\"},\n"
        << "        {\"name\": \"TotVAr\", \"ioa\": " << orule.ioa_tot_var
        << ", \"enabled\": " << (orule.monitor_tot_var ? "true" : "false")
        << ", \"published\": false, \"value\": " << m.q_kvar << ", \"unit\": \"kVAr\"},\n"
        << "        {\"name\": \"PPV\", \"ioa\": " << orule.ioa_ppv
        << ", \"enabled\": " << (orule.monitor_ppv ? "true" : "false")
        << ", \"published\": false, \"value\": " << cfg.plant.poc_ppv_kv
        << ", \"unit\": \"kV\"}\n"
        << "      ]\n"
        << "    },\n"
        << "    \"plant\": {\n"
        << "      \"modbus_backend\": \"" << modbus_backend_string(cfg.modbus.backend) << "\",\n"
        << "      \"modbus_device\": \"" << json_escape_str(cfg.modbus.device) << "\",\n"
        << "      \"modbus_host\": \"" << json_escape_str(cfg.modbus.host) << "\",\n"
        << "      \"modbus_tcp_port\": " << cfg.modbus.tcp_port << ",\n"
        << "      \"modbus_baud\": " << cfg.modbus.baud << ",\n"
        << "      \"modbus_slave_id\": " << cfg.modbus.slave_id << ",\n"
        << "      \"poll_ms\": " << cfg.modbus.poll_ms << ",\n"
        << "      \"link\": \"" << link_state_string(z.modbus_link_state) << "\",\n"
        << "      \"last_exchange_ms\": " << mb.exchange_ms << ",\n"
        << "      \"last_success\": " << (mb.success ? "true" : "false") << ",\n"
        << "      \"last_error\": \"" << json_escape_str(mb.error) << "\",\n"
        << "      \"goose_enabled\": " << (cfg.goose.enabled ? "true" : "false") << ",\n"
        << "      \"goose_interface\": \"" << json_escape_str(cfg.goose.interface) << "\",\n"
        << "      \"goose_publish\": " << (cfg.goose.publish_enabled ? "true" : "false") << "\n"
        << "    }\n"
        << "  }\n"
        << "}\n";
}

int run_bench_status_json() {
    std::ifstream in(kBenchStatusPath);
    if (!in) {
        std::cout << "{\"error\":\"ccli not running or status file missing\","
                     "\"path\":\""
                  << kBenchStatusPath << "\"}\n";
        return 1;
    }
    std::cout << in.rdbuf();
    return 0;
}

int run_bench_write_config(const std::string& path) {
    std::ostringstream buf;
    buf << std::cin.rdbuf();
    const std::string yaml = buf.str();
    if (yaml.size() < 20) {
        std::cerr << "bench-write-config: empty or too short\n";
        return 1;
    }
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        std::cerr << "bench-write-config: cannot write " << path << "\n";
        return 1;
    }
    out << yaml;
    std::cout << "{\"ok\":true,\"path\":\"" << path << "\",\"bytes\":" << yaml.size() << "}\n";
    return 0;
}

void print_usage() {
    std::cout << "ccli — CCI lab application (PF2 / Modbus / MMS scaffold)\n"
              << "Usage: ccli [--config PATH] [--version] [--version-json]\n"
              << "       ccli --gpio-test     Toggle DO5 / read DI27 (lab wiring check)\n"
              << "       ccli --gpio-blink N  Blink DO N times at start (relay wiring check)\n"
              << "       ccli --lab-demo      Ramp sim power to exercise PF2 -> DO5\n"
              << "       ccli --regulation-check [--live] [--live-watch SEC] [--json]\n"
              << "       ccli --bench-status --json   Read /var/run/ccli-status.json (daemon)\n"
              << "       ccli --bench-write-config PATH   Write lab yaml from stdin (lab UI)\n"
              << "       ccli --event-dump [--count N] [--json]   O.14 event log (P7-01/02)\n"
              << "       ccli --event-wrap-test   Verify 2048-event ring wrap (P7-01)\n"
              << "       ccli --event-clear     Rejected (O.14 user cannot overwrite)\n";
}

int run_event_dump(const std::string& config_path, std::size_t count, bool json_out) {
    const cci::core::CcliConfig cfg = cci::core::load_config(config_path);
    if (count == 0 || count > cci::core::EventStore::kMaxEvents) {
        count = cci::core::EventStore::kMaxEvents;
    }
    return cci::core::EventStore::dump_to_stream(std::cout, cfg.event_log.path, count, json_out);
}

int run_event_wrap_test() {
    const std::string path = "/tmp/ccli-event-wrap-test.jsonl";
    std::remove(path.c_str());
    const bool ok = cci::core::EventStore::run_wrap_test(path);
    std::cout << (ok ? "P7-01 wrap test: PASS (2048 events, oldest dropped)\n"
                     : "P7-01 wrap test: FAIL\n");
    std::remove(path.c_str());
    return ok ? 0 : 1;
}

cci::core::regulation::ModbusLiveTrace modbus_trace_from(
    const cci::adapters::ModbusExchangeRecord& ex) {
    cci::core::regulation::ModbusLiveTrace t{};
    if (!ex.valid) {
        return t;
    }
    t.valid = true;
    t.function_code = ex.function_code;
    t.slave_id = ex.slave_id;
    t.reg_start = ex.reg_start;
    t.reg_count = ex.reg_count;
    t.raw_regs_hex = ex.raw_regs_hex;
    t.p_kw = ex.p_kw;
    t.q_kvar = ex.q_kvar;
    t.success = ex.success;
    t.error = ex.error;
    t.exchange_ms = ex.exchange_ms;
    t.backend = ex.backend_label;
    std::ostringstream cmd;
    cmd << "FC" << t.function_code << " ReadHoldingRegisters"
        << " unit=" << t.slave_id << " addr=" << (40001 + t.reg_start) << " qty=" << t.reg_count
        << " -> P=" << ex.p_kw << "kW Q=" << ex.q_kvar << "kvar";
    if (!ex.success) {
        cmd << " ERR=" << ex.error;
    }
    t.command = cmd.str();
    return t;
}

int run_regulation_check(const std::string& config_path, bool live, bool json_out,
                         int live_watch_sec) {
    if (!cci::hal::platform_init()) {
        std::cerr << "regulation-check: platform_init failed\n";
        return 1;
    }

    const cci::core::CcliConfig app_cfg = cci::core::load_config(config_path);
    cci::core::regulation::RegulationReport report =
        cci::core::regulation::build_regulation_report(app_cfg, config_path);

    if (live) {
        cci::core::regulation::LiveSnapshot snap{};
        snap.valid = true;
        snap.permissive_bypass = app_cfg.permissive_bypass;

        DrvGpioConfig gcfg{};
        gcfg.outputs[DRVGPIO__OUT__CURTAIL].line = app_cfg.do_curtail.gpio;
        gcfg.outputs[DRVGPIO__OUT__CURTAIL].active_high = app_cfg.do_curtail.active_high;
        gcfg.inputs[DRVGPIO__IN__PERMISSIVE].line = app_cfg.di_permissive.gpio;
        gcfg.inputs[DRVGPIO__IN__PERMISSIVE].active_high = app_cfg.di_permissive.active_high;

        if (svc_io_init_cfg(&gcfg)) {
            cci::core::MeasurementStore measurements;
            cci::core::EventLogConfig elog_cfg;
            elog_cfg.enabled = app_cfg.event_log.enabled;
            elog_cfg.path = app_cfg.event_log.path;
            cci::core::EventStore events(elog_cfg);
            cci::services::ModbusService modbus(measurements, app_cfg);
            cci::core::Pf2Config pf2_cfg = app_cfg.pf2;
            const auto dso_apply =
                cci::core::dso::apply_dso_to_pf2_config(pf2_cfg, app_cfg.plant, app_cfg.dso);
            cci::services::Pf2Service pf2(measurements, events, pf2_cfg);

            snap.p_effective_export_kw = dso_apply.derived.p_effective_export_kw;
            snap.pf2_enter_kw = pf2_cfg.threshold_kw;
            snap.pf2_release_kw = pf2_cfg.release_threshold_kw;
            snap.modbus_poll_ms = app_cfg.modbus.poll_ms;

            std::int64_t last_logged_ms = -1;
            const int default_watch =
                std::max(4, (app_cfg.modbus.poll_ms + 999) / 1000);
            const int watch_s = live_watch_sec > 0 ? live_watch_sec : default_watch;
            const int step_ms = 100;
            const int iterations = (watch_s * 1000) / step_ms;

            for (int i = 0; i < iterations; ++i) {
                modbus.tick();
                pf2.tick();
                capture_modbus_log(modbus.last_exchange(), snap.modbus_log, last_logged_ms);
                usleep(static_cast<useconds_t>(step_ms * 1000));
            }

            const cci::core::Measurement m = measurements.snapshot();
            snap.p_kw = m.p_kw;
            snap.quality = m.quality;
            snap.pf2_state = pf2.last_state();
            snap.curtailment_commanded = pf2.curtailment_active();
            snap.modbus_last = modbus_trace_from(modbus.last_exchange());
            snap.modbus_timeout_count = modbus.timeout_count();

            bool perm = false;
            if (app_cfg.permissive_bypass) {
                snap.permissive_ok = true;
            } else {
                (void)svc_io_read_permissive(&perm);
                snap.permissive_ok = perm;
            }
            snap.do_curtail_on = snap.curtailment_commanded && snap.permissive_ok;
            svc_io_shutdown();
        } else {
            std::cerr << "regulation-check: svc_io_init failed (live IO skipped)\n";
        }

        cci::core::regulation::apply_live_snapshot(report, snap, app_cfg);
    }

    if (json_out) {
        cci::core::regulation::print_regulation_report_json(report, std::cout);
    } else {
        cci::core::regulation::print_regulation_report_text(report, std::cout);
    }
    return 0;
}

void gpio_blink_curtail(int count, const std::atomic<bool>& running) {
    std::cerr << "gpio-blink: " << count << " cycles on DO (active-low relay: LOW=click)\n";
    for (int i = 0; i < count && running; ++i) {
        if (!svc_io_apply_curtailment(true)) {
            std::cerr << "gpio-blink: apply_curtailment(true) failed\n";
            return;
        }
        usleep(400000);
        if (!svc_io_apply_curtailment(false)) {
            std::cerr << "gpio-blink: apply_curtailment(false) failed\n";
            return;
        }
        usleep(400000);
    }
    std::cerr << "gpio-blink: done\n";
}

int run_gpio_test(std::atomic<bool>* running) {
    if (!cci::hal::platform_init()) {
        std::cerr << "platform_init failed\n";
        return 1;
    }
    if (!svc_io_init()) {
        std::cerr << "svc_io_init failed\n";
        return 1;
    }

    cci::core::ServiceSupervision::set_safe_state_hook(svc_io_safe_state);
    cci::core::ServiceSupervision::install_handlers(running);

    std::cerr << "gpio-test: toggling curtail DO every 1s; reading permissive DI\n";

    bool on = false;
    while (*running) {
        on = !on;
        if (!svc_io_apply_curtailment(on)) {
            std::cerr << "gpio-test: apply_curtailment failed\n";
            return 1;
        }
        bool permissive = false;
        if (!svc_io_read_permissive(&permissive)) {
            std::cerr << "gpio-test: read_permissive failed\n";
            return 1;
        }
        svc_io_log_status(on, permissive, false);
        usleep(1000000);
    }

    svc_io_shutdown();
    return 0;
}

}  // namespace cci::program
