#include "modbus_adapter.hpp"

#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>



#ifdef CCLI_MODBUS_LIBMODBUS

#include <modbus/modbus.h>

#endif



namespace cci::adapters {



namespace {



void float32_to_regs_be(const double value, std::uint16_t& hi, std::uint16_t& lo) {
    float f = static_cast<float>(value);
    std::uint32_t raw = 0;
    static_assert(sizeof(float) == sizeof(std::uint32_t), "float32 size");
    std::memcpy(&raw, &f, sizeof(raw));
    hi = static_cast<std::uint16_t>((raw >> 16U) & 0xFFFFU);
    lo = static_cast<std::uint16_t>(raw & 0xFFFFU);
}

double regs_to_float32_be(const std::uint16_t hi, const std::uint16_t lo) {

    const std::uint32_t raw = (static_cast<std::uint32_t>(hi) << 16U) | lo;

    float               value = 0.0F;

    static_assert(sizeof(float) == sizeof(std::uint32_t), "float32 size");

    std::memcpy(&value, &raw, sizeof(value));

    return static_cast<double>(value);

}

double regs_to_float32_pair(const std::uint16_t reg0, const std::uint16_t reg1,
                            const bool word_swap) {
    if (word_swap) {
        return regs_to_float32_be(reg1, reg0);
    }
    return regs_to_float32_be(reg0, reg1);
}

std::string regs_hex_string(const std::uint16_t* regs, const int n) {
    std::ostringstream os;
    for (int i = 0; i < n; ++i) {
        if (i > 0) {
            os << ' ';
        }
        os << "0x" << std::hex << std::setw(4) << std::setfill('0') << regs[i] << std::dec;
    }
    return os.str();
}

int read_block_start(const ModbusPollConfig& cfg) {
    const int p_end = cfg.reg_active_power + cfg.reg_active_power_count;
    const int q_end = cfg.reg_reactive_power + cfg.reg_reactive_power_count;
    const int start = cfg.reg_active_power < cfg.reg_reactive_power ? cfg.reg_active_power
                                                                  : cfg.reg_reactive_power;
    const int end = p_end > q_end ? p_end : q_end;
    return start;
}

int read_block_count(const ModbusPollConfig& cfg) {
    const int start = read_block_start(cfg);
    const int p_end = cfg.reg_active_power + cfg.reg_active_power_count;
    const int q_end = cfg.reg_reactive_power + cfg.reg_reactive_power_count;
    const int end = p_end > q_end ? p_end : q_end;
    return end - start;
}

}  // namespace

void ModbusAdapter::record_read_holding(const std::int64_t now_ms, const int start,
                                        const int count, const std::uint16_t* regs,
                                        const int reg_count, const bool success,
                                        const char* error) {
    last_exchange_.valid = true;
    last_exchange_.function_code = 3;
    last_exchange_.slave_id = cfg_.slave_id;
    last_exchange_.reg_start = start;
    last_exchange_.reg_count = count;
    last_exchange_.success = success;
    last_exchange_.error = error ? error : "";
    last_exchange_.exchange_ms = now_ms;
    switch (cfg_.backend) {
        case ModbusBackend::LibmodbusRtu:
            last_exchange_.backend_label = "rtu";
            break;
        case ModbusBackend::LibmodbusTcp:
            last_exchange_.backend_label = "tcp";
            break;
        default:
            last_exchange_.backend_label = "simulator";
            break;
    }
    if (regs != nullptr && reg_count > 0) {
        last_exchange_.raw_regs_hex = regs_hex_string(regs, reg_count);
    } else {
        last_exchange_.raw_regs_hex.clear();
    }
    last_exchange_.p_kw = 0.0;
    last_exchange_.q_kvar = 0.0;
    if (success && regs != nullptr && reg_count > 0) {
        const int p_idx = cfg_.reg_active_power - start;
        const int q_idx = cfg_.reg_reactive_power - start;
        if (cfg_.reg_active_power_count >= 2 && (p_idx + 1) < reg_count) {
            last_exchange_.p_kw =
                regs_to_float32_pair(regs[p_idx], regs[p_idx + 1], cfg_.float_word_swap);
        } else if (cfg_.reg_active_power_count == 1 && p_idx >= 0 && p_idx < reg_count) {
            last_exchange_.p_kw = static_cast<double>(regs[p_idx]);
        }
        if (cfg_.reg_reactive_power_count >= 2 && (q_idx + 1) < reg_count) {
            last_exchange_.q_kvar =
                regs_to_float32_pair(regs[q_idx], regs[q_idx + 1], cfg_.float_word_swap);
        } else if (cfg_.reg_reactive_power_count == 1 && q_idx >= 0 && q_idx < reg_count) {
            last_exchange_.q_kvar = static_cast<double>(regs[q_idx]);
        }
        last_exchange_.p_kw *= cfg_.power_scale;
        last_exchange_.q_kvar *= cfg_.reactive_scale;
    }
}

ModbusAdapter::ModbusAdapter(ModbusPollConfig cfg) : cfg_(std::move(cfg)) {}



ModbusAdapter::~ModbusAdapter() {

    close_libmodbus();

}



void ModbusAdapter::set_simulated_power_kw(const double p_kw) {

    simulated_p_kw_ = p_kw;

    injected_fault_.reset();

}

bool ModbusAdapter::write_reactive_kvar(const double q_kvar) {
    if (cfg_.backend == ModbusBackend::Simulator) {
        simulated_q_kvar_ = q_kvar;
        simulated_q_override_ = true;
        return true;
    }

#ifdef CCLI_MODBUS_LIBMODBUS
    if (cfg_.backend != ModbusBackend::LibmodbusRtu) {
        return false;
    }
    if (cfg_.reg_reactive_power_count < 2) {
        return false;
    }
    if (!ensure_libmodbus_connected()) {
        return false;
    }
    std::uint16_t hi = 0;
    std::uint16_t lo = 0;
    float32_to_regs_be(q_kvar, hi, lo);
    const std::uint16_t regs[2] = {hi, lo};
    if (modbus_write_registers(static_cast<modbus_t*>(libmodbus_ctx_), cfg_.reg_reactive_power, 2,
                               regs) == -1) {
        std::fprintf(stderr, "modbus: FC16 write Q @ reg %d failed\n",
                     cfg_.reg_reactive_power);
        return false;
    }
    std::fprintf(stderr, "modbus: FC16 write Q=%.2f kvar @ holding %d\n", q_kvar,
                 40001 + cfg_.reg_reactive_power);
    return true;
#else
    (void)q_kvar;
    return false;
#endif
}



void ModbusAdapter::inject_comm_fault(const std::string& fault_id) {

    injected_fault_ = fault_id;

}



bool ModbusAdapter::poll(cci::core::MeasurementStore& store, const std::int64_t now_ms) {

    if (last_poll_ms_ > 0 && (now_ms - last_poll_ms_) < cfg_.poll_ms) {

        return true;

    }

    last_poll_ms_ = now_ms;



    switch (cfg_.backend) {

        case ModbusBackend::Simulator:

            return poll_simulator(store, now_ms);

        case ModbusBackend::LibmodbusRtu:
        case ModbusBackend::LibmodbusTcp:

            return poll_libmodbus(store, now_ms);

    }

    return false;

}



bool ModbusAdapter::poll_simulator(cci::core::MeasurementStore& store, const std::int64_t now_ms) {

    if (injected_fault_.has_value()) {

        const auto& fault = *injected_fault_;

        if (fault == "L1-C-01" || fault == "timeout") {

            ++timeout_count_;

            const int start = read_block_start(cfg_);
            const int count = read_block_count(cfg_);
            record_read_holding(now_ms, start, count, nullptr, 0, false, "timeout");

            cci::core::Measurement m{};

            m.quality = cci::core::DataQuality::Stale;

            m.timestamp_ms = now_ms - (cfg_.timeout_ms + 1);

            m.source = "modbus_sim";

            store.update(m);

            return false;

        }

        if (fault == "L1-C-02" || fault == "exception") {

            cci::core::Measurement m{};

            m.quality = cci::core::DataQuality::Invalid;

            m.timestamp_ms = now_ms;

            m.source = "modbus_sim";

            store.update(m);

            return false;

        }

        if (fault == "L1-C-03" || fault == "bad_crc") {

            ++crc_error_count_;

            return false;

        }

    }



    cci::core::Measurement m{};

    m.p_kw = simulated_p_kw_;

    m.q_kvar =
        simulated_q_override_ ? simulated_q_kvar_ : (simulated_p_kw_ * 0.1);

    m.pf = 0.98;

    m.quality = cci::core::DataQuality::Good;

    m.timestamp_ms = now_ms;

    m.source = "modbus_sim";

    const int start = read_block_start(cfg_);
    const int count = read_block_count(cfg_);
    std::uint16_t regs[32]{};
    if (cfg_.reg_active_power_count >= 2 && count >= 2) {
        const int p_idx = cfg_.reg_active_power - start;
        float pf = static_cast<float>(simulated_p_kw_);
        std::uint32_t raw = 0;
        std::memcpy(&raw, &pf, sizeof(raw));
        if (p_idx >= 0 && (p_idx + 1) < count) {
            regs[p_idx] = static_cast<std::uint16_t>((raw >> 16U) & 0xFFFFU);
            regs[p_idx + 1] = static_cast<std::uint16_t>(raw & 0xFFFFU);
        }
    }
    record_read_holding(now_ms, start, count, regs, count, true, nullptr);

    store.update(m);

    return true;

}



void ModbusAdapter::close_libmodbus() {

#ifdef CCLI_MODBUS_LIBMODBUS

    if (libmodbus_ctx_ != nullptr) {

        modbus_close(static_cast<modbus_t*>(libmodbus_ctx_));

        modbus_free(static_cast<modbus_t*>(libmodbus_ctx_));

        libmodbus_ctx_ = nullptr;

    }

#endif

}



bool ModbusAdapter::ensure_libmodbus_connected() {

#ifdef CCLI_MODBUS_LIBMODBUS

    if (libmodbus_ctx_ != nullptr) {

        return true;

    }

    modbus_t* ctx = nullptr;

    if (cfg_.backend == ModbusBackend::LibmodbusTcp) {

        if (cfg_.host.empty()) {

            return false;

        }

        ctx = modbus_new_tcp(cfg_.host.c_str(), cfg_.tcp_port);

    } else {

        if (cfg_.device.empty()) {

            return false;

        }

        ctx = modbus_new_rtu(cfg_.device.c_str(), cfg_.baud, cfg_.parity, 8, 1);

    }

    if (ctx == nullptr) {

        return false;

    }



    modbus_set_slave(ctx, cfg_.slave_id);



    const int timeout_sec = cfg_.timeout_ms / 1000;

    const int timeout_usec = (cfg_.timeout_ms % 1000) * 1000;

    modbus_set_response_timeout(ctx, timeout_sec, timeout_usec);

    if (cfg_.backend == ModbusBackend::LibmodbusRtu) {

        modbus_set_byte_timeout(ctx, timeout_sec, timeout_usec);

    }



    if (modbus_connect(ctx) != 0) {

        modbus_free(ctx);

        return false;

    }



    libmodbus_ctx_ = ctx;

    return true;

#else

    (void)cfg_;

    return false;

#endif

}



bool ModbusAdapter::poll_libmodbus(cci::core::MeasurementStore& store, const std::int64_t now_ms) {

#ifdef CCLI_MODBUS_LIBMODBUS

    const char* source = is_tcp_backend() ? "modbus_tcp" : "modbus_rtu";

    if (!ensure_libmodbus_connected()) {

        ++timeout_count_;

        const int start = read_block_start(cfg_);
        const int count = read_block_count(cfg_);
        record_read_holding(now_ms, start, count, nullptr, 0, false, "connect_fail");

        cci::core::Measurement m{};

        m.quality = cci::core::DataQuality::Stale;

        m.timestamp_ms = now_ms - (cfg_.timeout_ms + 1);

        m.source = source;

        store.update(m);

        return false;

    }



    modbus_t* ctx = static_cast<modbus_t*>(libmodbus_ctx_);

    const int start = read_block_start(cfg_);

    const int reg_count = read_block_count(cfg_);



    std::uint16_t regs[32]{};

    if (reg_count <= 0 || reg_count > static_cast<int>(sizeof(regs) / sizeof(regs[0]))) {

        record_read_holding(now_ms, start, reg_count, nullptr, 0, false, "bad_map");

        return false;

    }



    const int rc = modbus_read_registers(ctx, start, reg_count, regs);

    if (rc < 0) {

        ++timeout_count_;

        record_read_holding(now_ms, start, reg_count, nullptr, 0, false, "read_fail");

        close_libmodbus();

        cci::core::Measurement m{};

        m.quality = cci::core::DataQuality::Stale;

        m.timestamp_ms = now_ms - (cfg_.timeout_ms + 1);

        m.source = source;

        store.update(m);

        return false;

    }



    record_read_holding(now_ms, start, reg_count, regs, reg_count, true, nullptr);

    const int p_idx = cfg_.reg_active_power - start;

    const int q_idx = cfg_.reg_reactive_power - start;



    cci::core::Measurement m{};

    if (cfg_.reg_active_power_count >= 2 && (p_idx + 1) < reg_count) {

        m.p_kw = regs_to_float32_pair(regs[p_idx], regs[p_idx + 1], cfg_.float_word_swap);

    } else if (cfg_.reg_active_power_count == 1 && p_idx < reg_count) {

        m.p_kw = static_cast<double>(regs[p_idx]);

    }



    if (cfg_.reg_reactive_power_count >= 2 && (q_idx + 1) < reg_count) {

        m.q_kvar = regs_to_float32_pair(regs[q_idx], regs[q_idx + 1], cfg_.float_word_swap);

    } else if (cfg_.reg_reactive_power_count == 1 && q_idx < reg_count) {

        m.q_kvar = static_cast<double>(regs[q_idx]);

    } else {

        m.q_kvar = m.p_kw * 0.1;

    }

    m.p_kw *= cfg_.power_scale;

    m.q_kvar *= cfg_.reactive_scale;



    m.pf = 0.98;

    m.quality = cci::core::DataQuality::Good;

    m.timestamp_ms = now_ms;

    m.source = source;

    store.update(m);

    return true;

#else

    (void)store;

    (void)now_ms;

    return poll_simulator(store, now_ms);

#endif

}



}  // namespace cci::adapters


