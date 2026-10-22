#include "dso/dso_active_power.hpp"

#include <algorithm>
#include <cmath>

namespace cci::core::dso {

ActivePowerDerivation derive_active_power_kw(const PlantEnvelope& plant,
                                             const DsoActivePowerCommands& dso) {
    ActivePowerDerivation out{};
    out.smax_kva_calc = calc_smax_kva(plant);
    out.smax_kva_used = smax_kva_effective(plant);

    /* Full export capability before any slaved/autonomous function applies. */
    double p_cap = pct_smax_to_kw(100.0, out.smax_kva_used);

    if (dso.w110_active) {
        out.p_w110_kw = pct_smax_to_kw(dso.w110_pct, out.smax_kva_used);
        p_cap = std::min(p_cap, out.p_w110_kw);
    }

    if (dso.wlim_active) {
        out.p_wlim_kw = pct_smax_to_kw(dso.wmax_spt_pct, out.smax_kva_used);
        p_cap = std::min(p_cap, out.p_wlim_kw);
    }

    if (dso.wsd_active) {
        /*
         * Phase 1 lab maps WSd export set-point as a **ceiling** for the existing
         * "curtail when P > threshold" FSM — not full O.9.2.3 closed-loop tracking.
         * Import set-points (negative WSptPct) are ignored until storage/import FSM exists.
         */
        if (dso.wspt_pct >= 0.0) {
            out.p_wsd_kw = pct_smax_to_kw(dso.wspt_pct, out.smax_kva_used);
            p_cap = std::min(p_cap, out.p_wsd_kw);
        }
    }

    out.p_effective_export_kw = p_cap;
    return out;
}

bool settled_within_band_kw(const double p_meas_kw, const double p_expected_kw,
                             const double tolerance) {
    if (std::abs(p_expected_kw) < 1e-6) {
        return std::abs(p_meas_kw) < 1e-3;
    }
    return std::abs((p_meas_kw - p_expected_kw) / p_expected_kw) <= tolerance;
}

PlantEnvelope tr57126_table1_plant() {
    PlantEnvelope p{};
    p.p_imm_kw = 200.0;
    p.p_ass_kw = 200.0;
    p.q_ind_kvar = 50.0;
    p.q_cap_kvar = 50.0;
    p.smax_kva_authoritative = 210.0;
    return p;
}

DsoActivePowerCommands tr57126_table1_dso() {
    DsoActivePowerCommands d{};
    d.wlim_active = false;
    d.wsd_active = true;
    d.wspt_pct = 20.0;
    return d;
}

}  // namespace cci::core::dso
