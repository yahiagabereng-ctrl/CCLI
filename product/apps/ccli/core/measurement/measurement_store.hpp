#pragma once

#include <cstdint>
#include <string>

namespace cci::core {

enum class DataQuality : std::uint8_t {
    Good,
    Stale,
    Invalid,
};

struct Measurement {
    double p_kw{0.0};
    double q_kvar{0.0};
    double pf{1.0};
    /** EMT432 V_L1_L2 (V) * line_voltage_scale -> kV for PPV.phsAB when has_ppv_kv. */
    double ppv_kv{0.0};
    bool has_ppv_kv{false};
    DataQuality quality{DataQuality::Invalid};
    std::int64_t timestamp_ms{0};
    std::string source;
};

class MeasurementStore {
public:
    void update(const Measurement& m);
    Measurement snapshot() const;

private:
    Measurement latest_{};
};

}  // namespace cci::core
