#pragma once



#include "config/ccli_config.hpp"

#include "measurement/measurement_store.hpp"

#include "modbus_adapter.hpp"



namespace cci::services {



class ModbusService {

public:

    ModbusService(core::MeasurementStore& measurements, const core::CcliConfig& app_cfg);



    void tick();

    void set_simulated_power_kw(double p_kw);

    /** P5-R01 — lab plant Q command (FC16 or simulator backend). */
    bool write_reactive_kvar(double q_kvar);

    /** FC06 percent derating. WSd percent wins over Wlim. 100% when both are off. */
    bool command_active_limit(bool wlim_active, double wmax_spt_pct, bool wsd_active,
                              double wspt_pct);

    /** FC06 40200 power-on or 40201 shutdown. */
    bool command_in_service(bool in_service);

    /** One-shot at process start when modbus.inverter_control is set. */
    void apply_startup_inverter_control(const core::CcliConfig& app_cfg);

    const adapters::ModbusExchangeRecord& last_exchange() const { return adapter_.last_exchange(); }

    int timeout_count() const { return adapter_.timeout_count(); }

    const adapters::ModbusPollConfig& poll_config() const { return adapter_.poll_config(); }

private:

    core::MeasurementStore& measurements_;

    adapters::ModbusAdapter adapter_;

};



}  // namespace cci::services


