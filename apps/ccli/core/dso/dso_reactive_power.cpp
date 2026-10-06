#include "dso/dso_reactive_power.hpp"

#include <algorithm>
#include <cmath>

namespace cci::core::dso {

namespace {

double clamp_q_to_envelope(const PlantEnvelope& plant, const double smax_kva,
                           const double q_kvar) {
    if (plant.q_ind_kvar > 0.0 || plant.q_cap_kvar > 0.0) {
        const double q_max_ind = plant.q_ind_kvar > 0.0 ? plant.q_ind_kvar : smax_kva;
        const double q_max_cap = plant.q_cap_kvar > 0.0 ? plant.q_cap_kvar : smax_kva;
        return std::clamp(q_kvar, -q_max_cap, q_max_ind);
    }
    return q_kvar;
}

}  // namespace

ReactivePowerDerivation derive_reactive_kvar(const PlantEnvelope& plant,
                                             const DsoReactivePowerCommands& cmd) {
    ReactivePowerDerivation out{};
    out.smax_kva_used = smax_kva_effective(plant);
    if (!cmd.varsd_active) {
        return out;
    }

    double q = pct_smax_to_kvar(cmd.vartgt_spt_pct, out.smax_kva_used);
    out.q_target_kvar = clamp_q_to_envelope(plant, out.smax_kva_used, q);
    return out;
}

ReactivePowerDerivation derive_pfsp_kvar(const PlantEnvelope& plant,
                                         const double p_kw_measured,
                                         const PfspCommand& cmd) {
    ReactivePowerDerivation out{};
    out.smax_kva_used = smax_kva_effective(plant);
    if (!cmd.active || std::abs(p_kw_measured) < 1e-3) {
        return out;
    }

    /*
     * Eq (12) — direct cosφ: use |cosφ| for magnitude; CEI gen range [-1,0] stores
     * signed cosφ on PFGnTgtSpt, load range [0,1] on PFLodTgtSpt.
     */
    const double cos_mag = std::clamp(std::abs(cmd.cosphi), 0.001, 1.0);
    const double phi = std::acos(cos_mag);
    double q = std::abs(p_kw_measured) * std::tan(phi);

    /* Lab convention: inductive +Q at export (matches VArSd / Modbus slave sign). */
    if (cmd.generation && cmd.cosphi > 0.0) {
        q = -q;
    } else if (!cmd.generation && cmd.cosphi < 0.0) {
        q = -q;
    }

    out.q_target_kvar = clamp_q_to_envelope(plant, out.smax_kva_used, q);
    return out;
}

bool settled_within_band_kvar(const double q_meas_kvar, const double q_expected_kvar,
                              const double tolerance) {
    if (q_expected_kvar == 0.0) {
        return std::fabs(q_meas_kvar) <= tolerance;
    }
    const double band = std::fabs(q_expected_kvar) * tolerance;
    return std::fabs(q_meas_kvar - q_expected_kvar) <= band;
}

}  // namespace cci::core::dso
