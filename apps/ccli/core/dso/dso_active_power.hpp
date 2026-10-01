#pragma once

/**
 * @file dso_active_power.hpp
 * @brief Active-power DSO commands (O.9.2.1 / O.9.2.2 / O.9.2.3) and O.11 combination — Phase 1.
 *
 * Phase 1: values come from YAML or TR 57-126 preset, not from MMS.
 * Phase 3: populate DsoActivePowerCommands from Wlim / WSd ctlVal on Eth_A.
 *
 * Italian consolidated Allegato T:
 *   Tab. 84 — Wlim (WMaxSptPct)
 *   Tab. 85 — WSd (WSptPct)
 *
 * @see lab/PF2_REGULATION_PHASE1.md R02, R03, R05
 */

#include "dso/plant_envelope.hpp"

namespace cci::core::dso {

/**
 * Mock of Eth_A active-power related functions (export-focused Phase 1 bench).
 * Priority order when combining: see derive_active_power_kw() — aligns with O.11 examples
 * (min of simultaneous export caps/targets in kW).
 */
struct DsoActivePowerCommands {
    bool w110_active{false};   /**< O.9.2.1 — autonomous limit near 110 % Vn (no V input in P1) */
    double w110_pct{100.0};    /**< Export cap as % Smax when w110_active */

    bool wlim_active{false};   /**< O.9.2.2 — DSO slaved ceiling (DWMX1 / Wlim) */
    double wmax_spt_pct{0.0};  /**< WMaxSptPct [0..100] % Smax generation limit */

    bool wsd_active{false};    /**< O.9.2.3 — DSO modulation (DAGC1 / WSd) — Figura 2 s.p. W */
    double wspt_pct{0.0};      /**< WSptPct: + export, − import (% Smax); import not used in P1 FSM */
};

/** Numeric trace of derive_active_power_kw() for logs and tests. */
struct ActivePowerDerivation {
    double smax_kva_calc{0.0};       /**< Eq (1) from plant corners */
    double smax_kva_used{0.0};       /**< Authoritative or calc — p.u. base */
    double p_w110_kw{0.0};           /**< Component cap if w110_active */
    double p_wlim_kw{0.0};           /**< Component cap if wlim_active */
    double p_wsd_kw{0.0};            /**< Component cap if wsd_active (export + only in P1) */
    double p_effective_export_kw{0.0}; /**< O.11-style min() result used for Phase 1 PF2 threshold */
};

/**
 * Combines active P functions for **export** bench:
 *   1. Start from 100 % Smax (full capability).
 *   2. For each active function, p_cap = min(p_cap, pct_smax_to_kw(...)).
 *
 * Matches O.11 worked examples (50 % vs 70 % → 50 kW; 80 % vs 70 % → 70 kW on 100 kVA base).
 *
 * Limitations (Phase 1):
 *   - WSd negative (import) not mapped to FSM — only max(0, wspt_pct) for export mock.
 *   - Does not implement full O.11 priority table (reactive functions, MSD paths).
 */
ActivePowerDerivation derive_active_power_kw(const PlantEnvelope& plant,
                                             const DsoActivePowerCommands& dso);

/**
 * O.7.3.1 — "stable within ±5 % of expected" check (default tolerance 0.05).
 * Used in unit tests; production TsP ≤ 60 s timing is not measured in Phase 1.
 */
bool settled_within_band_kw(double p_meas_kw, double p_expected_kw, double tolerance = 0.05);

/** CEI TR 57-126 §6.2 Table 1 plant nameplate (200 kW, 50 kVAr, Smax 210 kVA). */
PlantEnvelope tr57126_table1_plant();

/** TR Figura 2 use case: Wlim off, WSd on, WSptPct default nominal 20 %. */
DsoActivePowerCommands tr57126_table1_dso();

}  // namespace cci::core::dso
