#pragma once



#include "measurement/measurement_store.hpp"



#include <cstdint>

#include <optional>

#include <string>



namespace cci::adapters {



enum class ModbusBackend : std::uint8_t {

    Simulator,

    LibmodbusRtu,

    LibmodbusTcp,

};



/** Last master transaction (Phase 1: FC03 read holding only; no writes). */
struct ModbusExchangeRecord {
    bool        valid{false};
    int         function_code{3};
    int         slave_id{1};
    int         reg_start{0};       /**< libmodbus 0-based address */
    int         reg_count{0};
    std::string raw_regs_hex;
    double      p_kw{0.0};
    double      q_kvar{0.0};
    bool        success{false};
    std::string error;
    std::int64_t exchange_ms{0};
    std::string backend_label;
};

struct ModbusPollConfig {

    ModbusBackend backend{ModbusBackend::Simulator};

    std::string host;

    int         tcp_port{502};

    std::string device{"/dev/ttyS1"};

    int         baud{9600};

    char        parity{'N'};

    int         slave_id{1};

    int         poll_ms{1000};

    int         timeout_ms{2000};

    int         retries{3};

    int         reg_active_power{0};

    int         reg_active_power_count{2};

    int         reg_reactive_power{2};

    int         reg_reactive_power_count{2};

    double      power_scale{1.0};

    double      reactive_scale{1.0};

};



class ModbusAdapter {

public:

    explicit ModbusAdapter(ModbusPollConfig cfg = {});

    ~ModbusAdapter();



    ModbusAdapter(const ModbusAdapter&) = delete;

    ModbusAdapter& operator=(const ModbusAdapter&) = delete;



    bool poll(cci::core::MeasurementStore& store, std::int64_t now_ms);



    void set_simulated_power_kw(double p_kw);

    /** P5-R01 — lab plant Q command (simulator backend or FC16 RTU write). */
    bool write_reactive_kvar(double q_kvar);

    void inject_comm_fault(const std::string& fault_id);



    int crc_error_count() const { return crc_error_count_; }

    int timeout_count() const { return timeout_count_; }

    const ModbusExchangeRecord& last_exchange() const { return last_exchange_; }

    const ModbusPollConfig& poll_config() const { return cfg_; }



private:

    bool poll_simulator(cci::core::MeasurementStore& store, std::int64_t now_ms);

    bool poll_libmodbus(cci::core::MeasurementStore& store, std::int64_t now_ms);

    bool ensure_libmodbus_connected();

    void close_libmodbus();

    bool is_tcp_backend() const {
        return cfg_.backend == ModbusBackend::LibmodbusTcp;
    }

    void record_read_holding(std::int64_t now_ms, int start, int count, const std::uint16_t* regs,
                             int reg_count, bool success, const char* error);



    ModbusPollConfig cfg_;

    double           simulated_p_kw_{500.0};
    double           simulated_q_kvar_{50.0};
    bool             simulated_q_override_{false};

    std::optional<std::string> injected_fault_;

    int              crc_error_count_{0};

    int              timeout_count_{0};

    std::int64_t     last_poll_ms_{0};

    ModbusExchangeRecord last_exchange_{};



#ifdef CCLI_MODBUS_LIBMODBUS

    void* libmodbus_ctx_{nullptr}; /* modbus_t* — opaque to keep header free of modbus.h */

#endif

};



}  // namespace cci::adapters


