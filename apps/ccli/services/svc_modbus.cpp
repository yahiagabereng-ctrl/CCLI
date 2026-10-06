#include "svc_modbus.hpp"



#include "cci/hal/time.hpp"



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

    out.power_scale = m.power_scale;

    out.reactive_scale = m.reactive_scale;

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



}  // namespace cci::services


