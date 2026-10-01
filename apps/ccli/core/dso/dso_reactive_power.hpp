#pragma once

/**
 * @file dso_reactive_power.hpp
 * @brief Reactive-power DSO commands (O.9.1.4 / T Table 88) — P5-R01.
 *
 * VArSdDVAR1.VArTgtSptPct: signed % of Smax at PdC → target Q (kVAr).
 * Plant actuation (Modbus / inverter) is adapter responsibility.
 *
 * @see lab/CCI_Reactive_Roadmap_Gap_Report.md P5-R01
 */

#include "dso/plant_envelope.hpp"

namespace cci::core::dso {

/** O.9.1.4 — direct Q set-point (DVAR prefix VArSd). */
struct DsoReactivePowerCommands {
    bool varsd_active{false};
    double vartgt_spt_pct{0.0}; /**< Signed % Smax — inductive + / capacitive − */
};

struct ReactivePowerDerivation {
    double smax_kva_used{0.0};
    double q_target_kvar{0.0}; /**< Commanded Q when varsd_active */
};

/**
 * O.9.1.4 bench: Q_sp ≈ (pct / 100) × Smax, clamped to declared Q envelope.
 * Inactive → q_target_kvar = 0 (no plant command).
 */
ReactivePowerDerivation derive_reactive_kvar(const PlantEnvelope& plant,
                                             const DsoReactivePowerCommands& cmd);

/** O.7.3.1 reactive band (±5 % default). */
bool settled_within_band_kvar(double q_meas_kvar, double q_expected_kvar,
                              double tolerance = 0.05);

}  // namespace cci::core::dso
