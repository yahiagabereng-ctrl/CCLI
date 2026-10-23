#include "modbus_pdc_decode.hpp"

#include "modbus_emt432_mms_map_revision.hpp"

#include <cmath>
#include <cstring>

namespace cci::adapters {

namespace {

double regs_to_float32_pair(const std::uint16_t reg0, const std::uint16_t reg1,
                            const bool word_swap) {
    const auto be = [](std::uint16_t hi, std::uint16_t lo) {
        const std::uint32_t raw = (static_cast<std::uint32_t>(hi) << 16U) | lo;
        float value = 0.0F;
        static_assert(sizeof(float) == sizeof(std::uint32_t));
        std::memcpy(&value, &raw, sizeof(value));
        return static_cast<double>(value);
    };
    if (word_swap) {
        return be(reg1, reg0);
    }
    return be(reg0, reg1);
}

double decode_float_at(const ModbusPollConfig& cfg, const int reg_offset, const int start,
                       const std::uint16_t* regs, const int reg_count, const int reg_count_cfg,
                       const double scale) {
    if (reg_offset < 0) {
        return 0.0;
    }
    const int idx = reg_offset - start;
    if (reg_count_cfg >= 2 && (idx + 1) < reg_count) {
        return regs_to_float32_pair(regs[idx], regs[idx + 1], cfg.float_word_swap) * scale;
    }
    if (reg_count_cfg == 1 && idx >= 0 && idx < reg_count) {
        return static_cast<double>(regs[idx]) * scale;
    }
    return 0.0;
}

double decode_float_w(const ModbusPollConfig& cfg, const int reg_offset, const int start,
                      const std::uint16_t* regs, const int reg_count) {
    return decode_float_at(cfg, reg_offset, start, regs, reg_count, 2, 1.0);
}

}  // namespace

void decode_pdc_mmxu_from_regs(const ModbusPollConfig& cfg, const int start,
                               const std::uint16_t* regs, const int reg_count,
                               cci::core::PdcMmxuPhasors& out) {
    (void)emt432_mms_map::kRevision;
    out = cci::core::PdcMmxuPhasors{};

    const bool map_a = cfg.reg_phase_a_current >= 0;
    const bool map_extra = cfg.poll_pdc_3ph;
    if (!map_a && !map_extra) {
        return;
    }

    if (cfg.reg_line_voltage >= 0) {
        out.ppv_ab_kv = decode_float_at(cfg, cfg.reg_line_voltage, start, regs, reg_count,
                                        cfg.reg_line_voltage_count, cfg.line_voltage_scale);
    }

    if (map_extra) {
        out.ppv_bc_kv = decode_float_at(cfg, cfg.reg_ppv_phs_bc, start, regs, reg_count, 2,
                                        cfg.line_voltage_scale);
        out.ppv_ca_kv = decode_float_at(cfg, cfg.reg_ppv_phs_ca, start, regs, reg_count, 2,
                                        cfg.line_voltage_scale);
    }

    if (map_a) {
        out.a_phs_a = decode_float_at(cfg, cfg.reg_phase_a_current, start, regs, reg_count,
                                      cfg.reg_phase_a_current_count, cfg.phase_a_current_scale);
    }
    if (map_extra) {
        out.a_phs_b = decode_float_at(cfg, cfg.reg_phase_b_current, start, regs, reg_count,
                                      cfg.reg_phase_b_current_count, cfg.phase_a_current_scale);
        out.a_phs_c = decode_float_at(cfg, cfg.reg_phase_c_current, start, regs, reg_count,
                                      cfg.reg_phase_c_current_count, cfg.phase_a_current_scale);
    }

    const double p1_w = decode_float_w(cfg, cfg.reg_active_power, start, regs, reg_count);
    const double q1_var = decode_float_w(cfg, cfg.reg_reactive_power, start, regs, reg_count);
    const double p2_w =
        map_extra ? decode_float_w(cfg, cfg.reg_active_power_l2, start, regs, reg_count) : 0.0;
    const double q2_var =
        map_extra ? decode_float_w(cfg, cfg.reg_reactive_power_l2, start, regs, reg_count) : 0.0;
    const double p3_w =
        map_extra ? decode_float_w(cfg, cfg.reg_active_power_l3, start, regs, reg_count) : 0.0;
    const double q3_var =
        map_extra ? decode_float_w(cfg, cfg.reg_reactive_power_l3, start, regs, reg_count) : 0.0;

    const double pf1 =
        cfg.reg_power_factor >= 0
            ? decode_float_w(cfg, cfg.reg_power_factor, start, regs, reg_count)
            : 0.0;
    out.a_phs_a_ang_deg = cfg.reg_power_factor >= 0
                              ? cci::core::pdc_current_angle_deg_from_pq_or_pf(p1_w, q1_var,
                                                                               pf1)
                              : cci::core::pdc_current_angle_deg_from_pq(p1_w, q1_var);
    if (map_extra) {
        out.a_phs_b_ang_deg = cci::core::pdc_current_angle_deg_from_pq(p2_w, q2_var);
        out.a_phs_c_ang_deg = cci::core::pdc_current_angle_deg_from_pq(p3_w, q3_var);
    } else {
        out.a_phs_b_ang_deg = 0.0;
        out.a_phs_c_ang_deg = 0.0;
    }

    const bool three_phase = map_extra && cci::core::pdc_emt432_three_phase_active(
                                               out.a_phs_b, out.a_phs_c, out.ppv_bc_kv);
    cci::core::pdc_fill_del_voltage_angles(three_phase, out);
    /* 1P bench: PPV.phsAB angle tracks current φ (meter has no V–I phasor reg). */
    if (!three_phase) {
        out.ppv_ab_ang_deg = out.a_phs_a_ang_deg;
    }

    out.valid = map_a || map_extra;
}

}  // namespace cci::adapters
