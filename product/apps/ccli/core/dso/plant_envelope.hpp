#pragma once

/**
 * @file plant_envelope.hpp
 * @brief CEI 0-16 O.8.2 — plant polygon reference and % Smax conversions (Phase 1 core).
 *
 * All Annex T active-power set-points and limits are expressed as **percentage of Smax**
 * (apparent power reference at PdC). This header provides:
 *   - Eq (1): compute Smax from declared P/Q capability corners
 *   - Eq (2): convert a percentage to an approximate kW command (cos φ ≈ 1 engineering check)
 *
 * Authoritative Smax from the Operating Rule (DPCC3.PdC_VA.VARtg) may differ slightly from
 * Eq (1) — e.g. TR 57-126 Table 1 lists 210 kVA while Eq (1) from 200 kW / 50 kVAr ≈ 206 kVA.
 * Use smax_kva_authoritative when set (> 0).
 *
 * @see lab/PF2_REGULATION_PHASE1.md R01
 * @see knowledge-base/08-engineering/CCI_Annex_O_Extract.md §6
 */

#include <cmath>

namespace cci::core::dso {

/** O.8.2 — producer-declared limits at PdC (kW / kVAr). Carloads excluded per norm. */
struct PlantEnvelope {
    double p_imm_kw{0.0};   /**< Pfed — max active power export (immittance) */
    double p_ass_kw{0.0};   /**< Pass — max active power import (absorption) */
    double q_ind_kvar{0.0}; /**< Max inductive reactive capability */
    double q_cap_kvar{0.0}; /**< Max capacitive reactive capability */
    /**
     * Operating Rule / MMS nameplate Smax (kVA). When > 0, used as p.u. base instead of Eq (1).
     * Maps to IEC 61850 DPCC3.PdC_VA.VARtg in product Phase 3+.
     */
    double smax_kva_authoritative{0.0};
};

/**
 * Eq (1) — CEI 0-16 O.8.2 polygon apparent-power reference:
 *   Smax = sqrt( max(Pimm², Pass²) + max(Qind², Qcap²) )
 */
inline double calc_smax_kva(const PlantEnvelope& p) {
    const double p_axis = std::max(p.p_imm_kw, p.p_ass_kw);
    const double q_axis = std::max(p.q_ind_kvar, p.q_cap_kvar);
    return std::sqrt(p_axis * p_axis + q_axis * q_axis);
}

/** Returns authoritative Smax if configured, otherwise Eq (1). */
inline double smax_kva_effective(const PlantEnvelope& p) {
    if (p.smax_kva_authoritative > 0.0) {
        return p.smax_kva_authoritative;
    }
    return calc_smax_kva(p);
}

/**
 * Eq (2) — Annex T percentage of Smax to active power (kW).
 * Valid as a bench approximation when power factor ≈ 1 at PdC; not a substitute for signed
 * complex power at low cos φ.
 */
inline double pct_smax_to_kw(const double pct, const double smax_kva) {
    return (pct / 100.0) * smax_kva;
}

/** Signed % Smax → reactive power (kVAr) — O.9.1.4 VArTgtSptPct bench form. */
inline double pct_smax_to_kvar(const double pct, const double smax_kva) {
    return (pct / 100.0) * smax_kva;
}

}  // namespace cci::core::dso
