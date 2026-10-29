#include "svc_modbus.hpp"



#include "cci/hal/time.hpp"

#include <chrono>



namespace cci::services {



namespace {



adapters::ModbusPollConfig modbus_poll_from_config(const core::ModbusConfig& m) {

    adapters::ModbusPollConfig out{};

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

    : ModbusService(measurements, app_cfg.modbus) {}

ModbusService::ModbusService(core::MeasurementStore& measurements, const core::ModbusConfig& modbus_cfg)
    : measurements_(measurements), adapter_(modbus_poll_from_config(modbus_cfg)) {}

ModbusService::~ModbusService() {
    stop_background();
}

bool ModbusService::start_background() {
    if (worker_.joinable()) return true;
    stop_requested_.store(false);
    try {
        worker_ = std::thread([this]() {
            const int poll_ms = adapter_.poll_config().poll_ms > 0 ? adapter_.poll_config().poll_ms : 1;
            const auto interval = std::chrono::milliseconds(poll_ms);
            while (!stop_requested_.load()) {
                try {
                    tick();
                } catch (...) {
                    auto failed = measurements_.snapshot();
                    failed.quality = core::DataQuality::Invalid;
                    failed.timestamp_ms = cci::hal::now().epoch_ms;
                    failed.source = "modbus_exception";
                    measurements_.update(failed);
                }
                const auto deadline = std::chrono::steady_clock::now() + interval;
                while (!stop_requested_.load() && std::chrono::steady_clock::now() < deadline)
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        });
    } catch (...) {
        return false;
    }
    return true;
}

void ModbusService::stop_background() {
    stop_requested_.store(true);
    if (worker_.joinable()) worker_.join();
}



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


