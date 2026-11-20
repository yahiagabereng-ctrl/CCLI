#include "svc_modbus.hpp"

#include "dso/dso_phase1.hpp"

#include "cci/hal/time.hpp"

#include <iostream>



namespace cci::services {



namespace {



adapters::ModbusPollConfig modbus_poll_from_config(const core::CcliConfig& app_cfg) {

    adapters::ModbusPollConfig out{};

    const auto& m = app_cfg.modbus;

    out.host = m.host;

    out.tcp_port = m.tcp_port;

    out.device = m.device;

    out.baud = m.baud;

    out.parity = m.parity;

    out.slave_id = m.slave_id;

    out.poll_ms = m.poll_ms;

    out.timeout_ms = m.timeout_ms;

    out.retries = m.retries;

    out.reg_active_power = m.reg_active_power;

    out.reg_active_power_count = m.reg_active_power_count;

    out.reg_reactive_power = m.reg_reactive_power;

    out.reg_reactive_power_count = m.reg_reactive_power_count;

    out.reg_line_voltage = m.reg_line_voltage;

    out.reg_line_voltage_count = m.reg_line_voltage_count;

    out.line_voltage_scale = m.line_voltage_scale;

    out.reg_phase_a_current = m.reg_phase_a_current;

    out.reg_phase_a_current_count = m.reg_phase_a_current_count;

    out.phase_a_current_scale = m.phase_a_current_scale;

    out.poll_pdc_3ph = m.poll_pdc_3ph;

    out.reg_phase_b_current = m.reg_phase_b_current;

    out.reg_phase_b_current_count = m.reg_phase_b_current_count;

    out.reg_phase_c_current = m.reg_phase_c_current;

    out.reg_phase_c_current_count = m.reg_phase_c_current_count;

    out.reg_ppv_phs_bc = m.reg_ppv_phs_bc;

    out.reg_ppv_phs_ca = m.reg_ppv_phs_ca;

    out.reg_active_power_l2 = m.reg_active_power_l2;

    out.reg_active_power_l3 = m.reg_active_power_l3;

    out.reg_reactive_power_l2 = m.reg_reactive_power_l2;

    out.reg_reactive_power_l3 = m.reg_reactive_power_l3;

    out.reg_power_factor = m.reg_power_factor;

    out.reg_power_factor_count = m.reg_power_factor_count;

    out.power_scale = m.power_scale;

    out.reactive_scale = m.reactive_scale;

    out.float_word_swap = m.float_word_swap;

    out.inverter_control = m.inverter_control;
    out.reg_p_limit_pct = m.reg_p_limit_pct;
    out.p_limit_gain = m.p_limit_gain;
    out.reg_power_on = m.reg_power_on;
    out.reg_shutdown = m.reg_shutdown;

    out.commissioning = m.commissioning;

    if (m.backend == core::ModbusBackendKind::Rtu) {

        out.backend = adapters::ModbusBackend::LibmodbusRtu;

    } else if (m.backend == core::ModbusBackendKind::Tcp) {

        out.backend = adapters::ModbusBackend::LibmodbusTcp;

    } else {

        out.backend = adapters::ModbusBackend::Simulator;

    }

    return out;

}



}  // namespace



ModbusService::ModbusService(core::MeasurementStore& measurements, const core::CcliConfig& app_cfg)

    : measurements_(measurements), adapter_(modbus_poll_from_config(app_cfg)) {}



void ModbusService::tick() {

    adapter_.poll(measurements_, cci::hal::now().epoch_ms);

}



void ModbusService::set_simulated_power_kw(const double p_kw) {

    adapter_.set_simulated_power_kw(p_kw);

}



bool ModbusService::write_reactive_kvar(const double q_kvar) {

    return adapter_.write_reactive_kvar(q_kvar);

}

bool ModbusService::command_active_limit(const bool wlim_active, const double wmax_spt_pct,
                                         const bool wsd_active, const double wspt_pct) {
    double pct = 100.0;
    if (wsd_active && wspt_pct >= 0.0) {
        pct = wspt_pct > 100.0 ? 100.0 : wspt_pct;
    } else if (wlim_active) {
        if (wmax_spt_pct < 0.0) {
            pct = 0.0;
        } else if (wmax_spt_pct > 100.0) {
            pct = 100.0;
        } else {
            pct = wmax_spt_pct;
        }
    }
    return adapter_.write_active_power_limit_pct(pct);
}

bool ModbusService::command_in_service(const bool in_service) {
    return adapter_.write_inverter_in_service(in_service);
}

void ModbusService::apply_startup_inverter_control(const core::CcliConfig& app_cfg) {
    if (!app_cfg.modbus.inverter_control) {
        return;
    }
    if (!command_in_service(app_cfg.modbus.inverter_in_service)) {
        std::cerr << "modbus: inverter run-state write failed\n";
        return;
    }
    if (!app_cfg.modbus.inverter_in_service || !app_cfg.dso.enabled || !app_cfg.pf2.use_dso_mock) {
        return;
    }
    const auto mock = core::dso::resolve_dso_mock_inputs(app_cfg.plant, app_cfg.dso);
    if (!command_active_limit(mock.commands.wlim_active, mock.commands.wmax_spt_pct,
                              mock.commands.wsd_active, mock.commands.wspt_pct)) {
        std::cerr << "modbus: inverter P limit write failed\n";
    }
}



}  // namespace cci::services


