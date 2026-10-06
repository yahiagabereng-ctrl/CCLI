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
#include "regulation/regulation_bench.hpp"
#include "service/service_supervision.hpp"
#include "version/ccli_version.hpp"

#include "modbus_adapter.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <string>
#include <unistd.h>

namespace {

constexpr const char* kBenchStatusPath = "/var/run/ccli-status.json";

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

/** Read-only zone view for the lab dashboard (DSO Eth_A · operator Eth_B · plant). */
struct ZoneStatusSnapshot {
    bool                          mms_running{false};
    int                           mms_clients{0};
    bool                          gnss_fix{false};
    bool                          dso_cmd_seen{false};
    cci::adapters::DsoLiveCommand dso_cmd{};
    bool                          iec104_running{false};
    int                           iec104_clients{0};
    int                           modbus_link_state{0}; /* 0=unknown, 1=up, -1=down */
    cci::adapters::ModbusExchangeRecord modbus_last{};
};

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
        << ", \"published\": " << (orule.monitor_tot_w ? "true" : "false")
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

std::atomic<bool> g_running{true};

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
              << "       ccli --event-wrap-test   Verify 2048-event ring wrap (P7-01)\n";
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

void capture_modbus_log(const cci::adapters::ModbusExchangeRecord& ex,
                        std::vector<cci::core::regulation::ModbusLiveTrace>& log,
                        std::int64_t& last_logged_ms) {
    if (!ex.valid || ex.exchange_ms == 0 || ex.exchange_ms == last_logged_ms) {
        return;
    }
    last_logged_ms = ex.exchange_ms;
    log.push_back(modbus_trace_from(ex));
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
            cci::core::EventRing events;
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

void gpio_blink_curtail(int count) {
    std::cerr << "gpio-blink: " << count << " cycles on DO (active-low relay: LOW=click)\n";
    for (int i = 0; i < count && g_running; ++i) {
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

int run_gpio_test() {
    if (!cci::hal::platform_init()) {
        std::cerr << "platform_init failed\n";
        return 1;
    }
    if (!svc_io_init()) {
        std::cerr << "svc_io_init failed\n";
        return 1;
    }

    cci::core::ServiceSupervision::set_safe_state_hook(svc_io_safe_state);
    cci::core::ServiceSupervision::install_handlers(&g_running);

    std::cerr << "gpio-test: toggling curtail DO every 1s; reading permissive DI\n";

    bool on = false;
    while (g_running) {
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

}  // namespace

int main(int argc, char** argv) {
    std::string config_path = "config/lab_pi.yaml";
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
        return run_gpio_test();
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
        gpio_blink_curtail(gpio_blink);
    }

    cci::core::MeasurementStore measurements;
    cci::core::EventStore events(
        {app_cfg.event_log.enabled, app_cfg.event_log.path});
    const auto log_event = [&events](const std::string& type, const std::string& detail) {
        events.append({cci::hal::now().epoch_ms, type, detail});
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
    log_event("system", "firmware_boot:" + boot_ver.full);
    log_event("system", "psu_unmonitored");
    log_event("security", "service_monitor:started");
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

    cci::services::Pf2Service pf2(measurements, events.ring(), pf2_cfg);
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
            if (mms.poll_comms_loss_fallback()) {
                std::cerr << "mms→pf2: P3-05 Operating Rule fallback (Eth_A no-comms)\n";
                events.append({cci::hal::now().epoch_ms, "mms", "comms_loss_fallback"});
            }
            cci::adapters::DsoLiveCommand dso_cmd{};
            if (mms.poll_dso_live_command(dso_cmd) && dso_cmd.valid) {
                last_dso_cmd = dso_cmd;
                dso_cmd_seen = true;
                if (dso_cmd.dirty) {
                    cci::core::Pf2Config live_cfg = pf2.config();
                    live_cfg.use_dso_mock = true;
                    const auto live = cci::core::dso::apply_live_dso_commands_to_pf2(
                        live_cfg, app_cfg.plant, app_cfg.dso, dso_cmd.wlim_active,
                        dso_cmd.wmax_spt_pct, dso_cmd.wsd_active, dso_cmd.wspt_pct);
                    if (live.applied) {
                        pf2.set_thresholds(live.pf2_threshold_kw, live.pf2_release_kw);
                        std::cerr << "mms→pf2: Wlim=" << (dso_cmd.wlim_active ? "on" : "off")
                                  << "@" << dso_cmd.wmax_spt_pct
                                  << "% WSd=" << (dso_cmd.wsd_active ? "on" : "off")
                                  << "@" << dso_cmd.wspt_pct
                                  << "% p_wlim=" << live.derived.p_wlim_kw
                                  << " p_wsd=" << live.derived.p_wsd_kw
                                  << " p_eff=" << live.derived.p_effective_export_kw
                                  << " kW → enter=" << live.pf2_threshold_kw
                                  << " release=" << live.pf2_release_kw << " kW\n";
                        events.append({cci::hal::now().epoch_ms, "mms",
                                       dso_cmd.wsd_active   ? "wsd_update"
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

        if ((loop_count % status_interval) == 0) {
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
