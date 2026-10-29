#pragma once



#include "config/ccli_config.hpp"

#include "measurement/measurement_store.hpp"

#include "modbus_adapter.hpp"

#include <atomic>
#include <thread>



namespace cci::services {



class ModbusService {

public:

    ModbusService(core::MeasurementStore& measurements, const core::CcliConfig& app_cfg);
    ModbusService(core::MeasurementStore& measurements, const core::ModbusConfig& modbus_cfg);
    ~ModbusService();

    bool start_background();
    void stop_background();



    void tick();

    void set_simulated_power_kw(double p_kw);

    /** P5-R01 — lab plant Q command (FC16 or simulator backend). */
    bool write_reactive_kvar(double q_kvar);

    const adapters::ModbusExchangeRecord& last_exchange() const { return adapter_.last_exchange(); }

    int timeout_count() const { return adapter_.timeout_count(); }

    const adapters::ModbusPollConfig& poll_config() const { return adapter_.poll_config(); }

private:

    core::MeasurementStore& measurements_;

    adapters::ModbusAdapter adapter_;
    std::atomic<bool> stop_requested_{false};
    std::thread worker_;

};



}  // namespace cci::services


