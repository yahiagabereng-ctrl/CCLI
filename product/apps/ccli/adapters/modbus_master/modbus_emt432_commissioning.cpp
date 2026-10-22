#include "modbus_emt432_commissioning.hpp"

#include "modbus_emt432_commissioning_revision.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#ifdef CCLI_MODBUS_LIBMODBUS
#include <modbus/modbus.h>
#endif

namespace cci::adapters {

namespace {

constexpr int kOffMounted = 10;    // 40011
constexpr int kOffMeasCfg = 231;   // 40232
constexpr int kOffTaChronos = 233; // 40234
constexpr int kOffTaSens = 234;    // 40235
constexpr int kOffTaDelay = 236;   // 40237
constexpr int kOffTvRatio = 238;   // 40239
constexpr int kOffTvDelay = 240;   // 40241
constexpr int kOffMinVRipple = 242;  // 40243
constexpr int kOffMinIRipple = 244;  // 40245
constexpr int kOffMinPRipple = 246;  // 40247
constexpr int kOffEnergyUnit = 255;  // 40256
constexpr int kOffKWh1 = 715;        // 40716
constexpr int kOffKWhSum = 728;      // 40729
constexpr int kOffKvarh1 = 764;      // 40765
constexpr int kOffKvah1 = 844;       // 40845

constexpr std::uint16_t kEnergyUnitKwh = 4U;

constexpr double kSensTolerance = 1e-8;
constexpr double kFloatTolerance = 1e-5;

bool f32_near(const double a, const double b) {
    return std::fabs(a - b) <= kFloatTolerance;
}

std::uint16_t apply_1p_rogowski(std::uint16_t reg) {
    reg = static_cast<std::uint16_t>(reg & ~0x6U);
    reg = static_cast<std::uint16_t>(reg | (1U << 13));
    return reg;
}

std::uint16_t apply_filtered_measurement(std::uint16_t reg) {
    return static_cast<std::uint16_t>(reg | (1U << 15));
}

std::uint16_t apply_energy_saving(std::uint16_t reg) {
    return static_cast<std::uint16_t>(reg | (1U << 9));
}

bool energy_saving_ok(std::uint16_t reg) {
    return ((reg >> 9) & 1) != 0;
}

bool meas_cfg_ok(std::uint16_t reg) {
    const int mode = (reg >> 1) & 0x3;
    const bool integrator = ((reg >> 13) & 1) != 0;
    return mode == 0 && integrator;
}

bool filtered_measurement_ok(std::uint16_t reg) {
    return ((reg >> 15) & 1) != 0;
}

std::uint16_t apply_meas_cfg_rmw(std::uint16_t reg, const core::ModbusCommissioningConfig& cfg) {
    if (cfg.write_meas_1p_integrator) {
        reg = apply_1p_rogowski(reg);
    }
    if (cfg.write_filtered_measurement) {
        reg = apply_filtered_measurement(reg);
    }
    if (cfg.write_energy_saving) {
        reg = apply_energy_saving(reg);
    }
    return reg;
}

bool meas_cfg_targets_met(std::uint16_t reg, const core::ModbusCommissioningConfig& cfg) {
    if (cfg.write_meas_1p_integrator && !meas_cfg_ok(reg)) {
        return false;
    }
    if (cfg.write_filtered_measurement && !filtered_measurement_ok(reg)) {
        return false;
    }
    if (cfg.write_energy_saving && !energy_saving_ok(reg)) {
        return false;
    }
    return true;
}

void float32_to_regs(const double value, std::uint16_t& r0, std::uint16_t& r1, const bool word_swap) {
    float f = static_cast<float>(value);
    std::uint32_t raw = 0;
    std::memcpy(&raw, &f, sizeof(raw));
    const std::uint16_t hi = static_cast<std::uint16_t>((raw >> 16U) & 0xFFFFU);
    const std::uint16_t lo = static_cast<std::uint16_t>(raw & 0xFFFFU);
    if (word_swap) {
        r0 = lo;
        r1 = hi;
    } else {
        r0 = hi;
        r1 = lo;
    }
}

double regs_to_float32(const std::uint16_t r0, const std::uint16_t r1, const bool word_swap) {
    std::uint16_t hi = r0;
    std::uint16_t lo = r1;
    if (word_swap) {
        hi = r1;
        lo = r0;
    }
    const std::uint32_t raw =
        (static_cast<std::uint32_t>(hi) << 16U) | static_cast<std::uint32_t>(lo);
    float value = 0.0F;
    std::memcpy(&value, &raw, sizeof(value));
    return static_cast<double>(value);
}

#ifdef CCLI_MODBUS_LIBMODBUS

bool read_u16(modbus_t* ctx, int offset, std::uint16_t& out) {
    std::uint16_t reg = 0;
    if (modbus_read_registers(ctx, offset, 1, &reg) != 1) {
        return false;
    }
    out = reg;
    return true;
}

bool write_u16(modbus_t* ctx, int offset, std::uint16_t value) {
    return modbus_write_register(ctx, offset, value) != -1;
}

bool read_f32(modbus_t* ctx, int offset, double& out, bool word_swap) {
    std::uint16_t regs[2]{};
    if (modbus_read_registers(ctx, offset, 2, regs) != 2) {
        return false;
    }
    out = regs_to_float32(regs[0], regs[1], word_swap);
    return true;
}

bool write_f32(modbus_t* ctx, int offset, double value, bool word_swap) {
    std::uint16_t r0 = 0;
    std::uint16_t r1 = 0;
    float32_to_regs(value, r0, r1, word_swap);
    const std::uint16_t regs[2] = {r0, r1};
    return modbus_write_registers(ctx, offset, 2, regs) != -1;
}

std::int64_t regs_to_i64_be(const std::uint16_t* regs) {
    std::uint64_t raw = 0;
    for (int i = 0; i < 4; ++i) {
        raw = (raw << 16U) | static_cast<std::uint64_t>(regs[i] & 0xFFFFU);
    }
    return static_cast<std::int64_t>(raw);
}

bool read_i64_wh_tenth(modbus_t* ctx, int offset, std::int64_t& out) {
    std::uint16_t regs[4]{};
    if (modbus_read_registers(ctx, offset, 4, regs) != 4) {
        return false;
    }
    out = regs_to_i64_be(regs);
    return true;
}

#endif

}  // namespace

bool emt432_apply_commissioning(void* libmodbus_ctx, const core::ModbusCommissioningConfig& cfg,
                                const bool float_word_swap) {
    if (!cfg.enabled) {
        return true;
    }

    using emt432_commissioning::kDocumentId;
    using emt432_commissioning::kRevision;
    using emt432_commissioning::kRevisionStr;

    if (cfg.revision != 0 && cfg.revision != kRevision) {
        std::fprintf(stderr,
                     "modbus: EMT432 commissioning ABORT %s rev %s — yaml revision %d mismatch "
                     "(expected %d)\n",
                     kDocumentId, kRevisionStr, cfg.revision, kRevision);
        return false;
    }

#ifndef CCLI_MODBUS_LIBMODBUS
    (void)libmodbus_ctx;
    (void)float_word_swap;
    std::fprintf(stderr, "modbus: commissioning skipped (libmodbus not built)\n");
    return false;
#else
    auto* ctx = static_cast<modbus_t*>(libmodbus_ctx);
    if (ctx == nullptr) {
        return false;
    }

    bool ok = true;
    std::fprintf(stderr, "modbus: EMT432 commissioning start %s rev %s\n", kDocumentId,
                 kRevisionStr);

    if (cfg.verify_mounted_version) {
        std::uint16_t mounted = 0;
        if (!read_u16(ctx, kOffMounted, mounted)) {
            std::fprintf(stderr, "modbus: FAIL read 40011 mounted_version\n");
            ok = false;
        } else if (mounted != 1U) {
            std::fprintf(stderr,
                         "modbus: FAIL 40011 mounted_version=%u (expected 1 Rogowski)\n",
                         static_cast<unsigned>(mounted));
            ok = false;
        } else {
            std::fprintf(stderr, "modbus: PASS 40011 mounted_version=1\n");
        }
    }

    if (cfg.write_meas_1p_integrator || cfg.write_filtered_measurement ||
        cfg.write_energy_saving) {
        std::uint16_t meas = 0;
        if (!read_u16(ctx, kOffMeasCfg, meas)) {
            std::fprintf(stderr, "modbus: FAIL read 40232 Measurement_Config\n");
            ok = false;
        } else if (meas_cfg_targets_met(meas, cfg)) {
            std::fprintf(stderr,
                         "modbus: SKIP 40232 Measurement_Config already OK raw=%u (0x%x)\n",
                         static_cast<unsigned>(meas), static_cast<unsigned>(meas));
        } else {
            const std::uint16_t next = apply_meas_cfg_rmw(meas, cfg);
            if (!write_u16(ctx, kOffMeasCfg, next)) {
                std::fprintf(stderr, "modbus: FAIL write 40232 Measurement_Config\n");
                ok = false;
            } else {
                std::fprintf(stderr,
                             "modbus: WROTE 40232 Measurement_Config raw=%u -> %u (0x%x)\n",
                             static_cast<unsigned>(meas), static_cast<unsigned>(next),
                             static_cast<unsigned>(next));
            }
        }
    }

    {
        std::uint16_t meas = 0;
        if (!read_u16(ctx, kOffMeasCfg, meas)) {
            std::fprintf(stderr, "modbus: FAIL 40232 verify read\n");
            ok = false;
        } else {
            if (!meas_cfg_ok(meas)) {
                std::fprintf(stderr,
                             "modbus: FAIL 40232 verify mode/integrator raw=%u (0x%x)\n",
                             static_cast<unsigned>(meas), static_cast<unsigned>(meas));
                ok = false;
            } else {
                std::fprintf(stderr, "modbus: PASS 40232 1P+integrator ON\n");
            }
            if (cfg.write_filtered_measurement && !filtered_measurement_ok(meas)) {
                std::fprintf(stderr,
                             "modbus: FAIL 40232 verify filtered measurement (bit15) raw=%u "
                             "(0x%x)\n",
                             static_cast<unsigned>(meas), static_cast<unsigned>(meas));
                ok = false;
            } else if (cfg.write_filtered_measurement) {
                std::fprintf(stderr, "modbus: PASS 40232 filtered measurement ON (bit15)\n");
            }
            if (cfg.write_energy_saving && !energy_saving_ok(meas)) {
                std::fprintf(stderr,
                             "modbus: FAIL 40232 verify energy saving (bit9) raw=%u (0x%x)\n",
                             static_cast<unsigned>(meas), static_cast<unsigned>(meas));
                ok = false;
            } else if (cfg.write_energy_saving) {
                std::fprintf(stderr, "modbus: PASS 40232 energy saving ON (bit9)\n");
            }
        }
    }

    if (cfg.verify_energy_unit_kwh || cfg.write_energy_unit_kwh) {
        std::uint16_t eu = 0;
        if (!read_u16(ctx, kOffEnergyUnit, eu)) {
            std::fprintf(stderr, "modbus: FAIL read 40256 Energy_unit_factor\n");
            ok = false;
        } else if (cfg.write_energy_unit_kwh && eu != kEnergyUnitKwh) {
            if (!write_u16(ctx, kOffEnergyUnit, kEnergyUnitKwh)) {
                std::fprintf(stderr, "modbus: FAIL write 40256 Energy_unit_factor=4 (kWh)\n");
                ok = false;
            } else {
                std::fprintf(stderr, "modbus: WROTE 40256 Energy_unit_factor=4 (kWh)\n");
                eu = kEnergyUnitKwh;
            }
        }
        if (cfg.verify_energy_unit_kwh && eu != kEnergyUnitKwh) {
            std::fprintf(stderr,
                         "modbus: FAIL 40256 Energy_unit_factor=%u (expected 4 kWh)\n",
                         static_cast<unsigned>(eu));
            ok = false;
        } else if (cfg.verify_energy_unit_kwh) {
            std::fprintf(stderr, "modbus: PASS 40256 Energy_unit_factor=4 (kWh)\n");
        }
    }

    if (cfg.verify_ta_chronos_register || cfg.write_ta_chronos_register) {
        std::uint16_t ta_ch = 0;
        if (!read_u16(ctx, kOffTaChronos, ta_ch)) {
            std::fprintf(stderr, "modbus: FAIL read 40234 TA_Chronos_register\n");
            ok = false;
        } else if (cfg.write_ta_chronos_register && ta_ch != 0U) {
            if (!write_u16(ctx, kOffTaChronos, 0)) {
                std::fprintf(stderr, "modbus: FAIL write 40234 TA_Chronos_register=0\n");
                ok = false;
            } else {
                std::fprintf(stderr, "modbus: WROTE 40234 TA_Chronos_register 0 (user TA)\n");
                ta_ch = 0;
            }
        }
        if (cfg.verify_ta_chronos_register && ta_ch != 0U) {
            std::fprintf(stderr,
                         "modbus: FAIL 40234 TA_Chronos_register=%u (expected 0 user)\n",
                         static_cast<unsigned>(ta_ch));
            ok = false;
        } else if (cfg.verify_ta_chronos_register) {
            std::fprintf(stderr, "modbus: PASS 40234 TA_Chronos_register=0\n");
        }
    }

    auto verify_write_f32 = [&](int offset, const char* label, double target, bool do_write) {
        double val = 0.0;
        if (!read_f32(ctx, offset, val, float_word_swap)) {
            std::fprintf(stderr, "modbus: FAIL read %s\n", label);
            ok = false;
            return;
        }
        if (f32_near(val, target)) {
            std::fprintf(stderr, "modbus: PASS %s=%g\n", label, val);
            return;
        }
        if (do_write) {
            if (!write_f32(ctx, offset, target, float_word_swap)) {
                std::fprintf(stderr, "modbus: FAIL write %s=%g\n", label, target);
                ok = false;
                return;
            }
            std::fprintf(stderr, "modbus: WROTE %s=%g (was %g)\n", label, target, val);
            val = target;
        }
        if (!f32_near(val, target)) {
            std::fprintf(stderr, "modbus: FAIL %s=%g (expected %g)\n", label, val, target);
            ok = false;
        }
    };

    if (cfg.verify_transducer_bench_defaults || cfg.write_transducer_bench_defaults) {
        const bool wr = cfg.write_transducer_bench_defaults;
        verify_write_f32(kOffTaDelay, "40237 TA_Transducer_delay [deg]", cfg.ta_transducer_delay_deg,
                         wr);
        verify_write_f32(kOffTvRatio, "40239 TV_Transducer_ratio", cfg.tv_transducer_ratio, wr);
        verify_write_f32(kOffTvDelay, "40241 TV_Transducer_delay [deg]", cfg.tv_transducer_delay_deg,
                         wr);
        verify_write_f32(kOffMinVRipple, "40243 minimum_voltage_ripple", cfg.minimum_voltage_ripple,
                         wr);
        verify_write_f32(kOffMinIRipple, "40245 minimum_current_ripple", cfg.minimum_current_ripple,
                         wr);
        verify_write_f32(kOffMinPRipple, "40247 minimum_power_ripple", cfg.minimum_power_ripple, wr);
    }

    if (cfg.write_rogowski_sensitivity) {
        double sens = 0.0;
        const double target = cfg.rogowski_sensitivity_v_per_a;
        if (read_f32(ctx, kOffTaSens, sens, float_word_swap) &&
            std::fabs(sens - target) <= kSensTolerance) {
            std::fprintf(stderr, "modbus: SKIP 40235 sensitivity already %g V/A\n", sens);
        } else if (!write_f32(ctx, kOffTaSens, target, float_word_swap)) {
            std::fprintf(stderr, "modbus: FAIL write 40235 TA sensitivity\n");
            ok = false;
        } else {
            std::fprintf(stderr, "modbus: WROTE 40235 sensitivity=%g V/A\n", target);
        }

        if (!read_f32(ctx, kOffTaSens, sens, float_word_swap) ||
            std::fabs(sens - target) > kSensTolerance) {
            std::fprintf(stderr, "modbus: FAIL 40235 verify read=%g expected=%g\n", sens, target);
            ok = false;
        } else {
            std::fprintf(stderr, "modbus: PASS 40235 sensitivity=%g V/A\n", sens);
        }
    }

    if (cfg.log_energy_readings) {
        auto log_e = [&](int offset, const char* label) {
            std::int64_t raw = 0;
            if (!read_i64_wh_tenth(ctx, offset, raw)) {
                std::fprintf(stderr, "modbus: READ energy %s @ offset %d failed\n", label, offset);
                return;
            }
            const double kwh = static_cast<double>(raw) / 10000.0;
            std::fprintf(stderr, "modbus: READ energy %s raw=%lld (%.4f kWh)\n",
                         label, static_cast<long long>(raw), kwh);
        };
        log_e(kOffKWh1, "KWh1");
        log_e(kOffKWhSum, "KWh_SUM");
        log_e(kOffKvarh1, "KVARh1");
        log_e(kOffKvah1, "KVAh1");
    }

    std::fprintf(stderr, "modbus: EMT432 commissioning %s\n", ok ? "PASS" : "FAIL");
    return ok;
#endif
}

}  // namespace cci::adapters
