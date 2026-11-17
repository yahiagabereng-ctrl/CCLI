// On-device component harness: the real MMS adapter, no controller/plant I/O.
#include "mms_adapter.hpp"
#include "iec104_adapter.hpp"
#include "event/event_store.hpp"
#include "dso/dso_phase1.hpp"
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>

namespace {
volatile std::sig_atomic_t running = 1;
void stop(int) { running = 0; }
bool test_path(const std::string& path) {
#if defined(CCLI_HOST_DUAL_TEST)
    return !path.empty() && path.find("..") == std::string::npos;
#else
    return path.rfind("/etc/ccli/rbac-test-20261016/", 0) == 0 && path.find("..") == std::string::npos;
#endif
}
}
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: mms-rbac-device-server CONFIG DURATION_SECONDS\n";
        return 2;
    }
    const int duration = std::atoi(argv[2]);
    auto cfg = cci::core::load_config(argv[1]);
    if (!cfg.loaded_from_file || duration < 1 || duration > 900 ||
#if defined(CCLI_HOST_DUAL_TEST)
        cfg.mms.tcp_port != 3784 || cfg.mms.bind_address != "127.0.0.1" ||
#else
        cfg.mms.tcp_port != 3783 ||
#endif
        !cfg.mms.rbac_profile_a ||
        !cfg.event_log.enabled || !test_path(cfg.event_log.path) ||
        !test_path(cfg.mms.rbac_activation_state) || !test_path(cfg.mms.rbac_policy_path) ||
        !cfg.mms.model_cfg_path.empty()) {
        std::cerr << "refusing configuration outside isolated test port/state/log/package or MVP model\n";
        return 2;
    }
    if (cfg.iec104.enabled && (cfg.iec104.tcp_port != 2405 ||
        cfg.iec104.bind_address != "192.168.0.1" || !cfg.iec104.tls.enabled ||
        cfg.iec104.operator_rule.mode != "monitor_only" || cfg.iec104.operator_rule.allow_commands ||
        cfg.iec104.operator_rule.allow_clock_sync)) {
        std::cerr << "refusing IEC 104 outside isolated TLS monitoring port\n";
        return 2;
    }
    cfg.mms.gnss_poll_s = 0;
    cfg.mms.gnss_discipline_clock = false;
    cfg.mms.chrony_poll = false;
    cci::core::EventLogConfig log;
    log.enabled = true;
    log.path = cfg.event_log.path;
    cci::core::EventStore events(log);
    cci::adapters::MmsAdapter server;
    const auto audit = [&events](const std::string& detail) {
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        events.append({now, "mms", detail});
        std::cerr << detail << '\n';
    };
    if (!server.start(cfg.mms, audit)) return 1;
    cci::adapters::Iec104Adapter iec104;
    if (cfg.iec104.enabled && !iec104.start(cfg.iec104)) return 1;
    std::signal(SIGTERM, stop);
    std::signal(SIGINT, stop);
    std::cerr << "ISOLATED_RBAC_TEST_READY port=" << cfg.mms.tcp_port << " synthetic_measurements=true plant_io=false\n";
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(duration);
    unsigned sample = 0;
    cci::adapters::DsoLiveCommand effective{};
    const auto initial = cci::core::dso::resolve_dso_mock_inputs(cfg.plant, cfg.dso);
    effective.wlim_active = cfg.dso.enabled && initial.commands.wlim_active;
    effective.wmax_spt_pct = initial.commands.wmax_spt_pct;
    effective.wsd_active = cfg.dso.enabled && initial.commands.wsd_active;
    effective.wspt_pct = initial.commands.wspt_pct;
    while (running && std::chrono::steady_clock::now() < deadline) {
        if (cfg.iec104.enabled) {
            iec104.update_tot_w_kw(12.0);
            iec104.tick();
            std::string detail;
            while (iec104.poll_security_event(detail)) audit("IEC104 " + detail);
        }
        server.update_tot_w_kw(12.0 + (sample++ % 2));
        server.update_totvar_kvar(2.0);
        server.update_ppv_kv(0.4);
        server.poll_comms_loss_fallback();
        cci::adapters::DsoLiveCommand update{};
        if (server.poll_dso_live_command(update)) {
            effective.merge_update(update);
            cci::core::Pf2Config pf{};
            const auto power = cci::core::dso::apply_live_dso_commands_to_pf2(pf, cfg.plant, cfg.dso,
                effective.wlim_active, effective.wmax_spt_pct, effective.wsd_active, effective.wspt_pct,
                effective.wsa_active, effective.wsa_spt_pct);
            const auto reactive = cci::core::dso::select_live_reactive_command(cfg.plant, cfg.dso,
                effective.varsd_active, effective.vartgt_spt_pct, effective.pfsp_active,
                effective.pfsp_cosphi, effective.pfsp_generation, 12.0,
                effective.varsa_active, effective.varsa_spt_pct);
            audit("POWER_COMMAND p_eff_kw=" + std::to_string(power.pf2_threshold_kw) +
                " q_target_kvar=" + std::to_string(reactive.command.derived.q_target_kvar) +
                " q_source=" + reactive.source + " plant_io=false");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    server.stop();
    iec104.stop();
    std::cerr << "ISOLATED_RBAC_TEST_STOPPED\n";
    return 0;
}
