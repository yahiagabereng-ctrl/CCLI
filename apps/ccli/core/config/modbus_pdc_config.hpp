#pragma once

#include "config/ccli_config.hpp"

namespace cci::core {

/** EMT432 RMS offsets (0-based from 40001) — see EMT432_RMS_REALTIME_HOLDING_SPEC.md */
inline void apply_modbus_pdc_defaults(ModbusConfig& cfg) {
    if (!cfg.poll_pdc_3ph) {
        return;
    }
    if (cfg.reg_phase_b_current < 0) {
        cfg.reg_phase_b_current = 924;
    }
    if (cfg.reg_phase_c_current < 0) {
        cfg.reg_phase_c_current = 926;
    }
    if (cfg.reg_ppv_phs_bc < 0) {
        cfg.reg_ppv_phs_bc = 916;
    }
    if (cfg.reg_ppv_phs_ca < 0) {
        cfg.reg_ppv_phs_ca = 918;
    }
    if (cfg.reg_active_power_l2 < 0) {
        cfg.reg_active_power_l2 = 934;
    }
    if (cfg.reg_active_power_l3 < 0) {
        cfg.reg_active_power_l3 = 936;
    }
    if (cfg.reg_reactive_power_l2 < 0) {
        cfg.reg_reactive_power_l2 = 942;
    }
    if (cfg.reg_reactive_power_l3 < 0) {
        cfg.reg_reactive_power_l3 = 944;
    }
    if (cfg.reg_power_factor < 0) {
        cfg.reg_power_factor = 956;
    }
    if (cfg.reg_phase_b_current_count < 2) {
        cfg.reg_phase_b_current_count = 2;
    }
    if (cfg.reg_phase_c_current_count < 2) {
        cfg.reg_phase_c_current_count = 2;
    }
}

}  // namespace cci::core
