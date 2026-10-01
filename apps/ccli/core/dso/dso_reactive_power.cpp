#include "dso/dso_reactive_power.hpp"

#include <algorithm>
#include <cmath>

namespace cci::core::dso {

ReactivePowerDerivation derive_reactive_kvar(const PlantEnvelope& plant,
                                             const DsoReactivePowerCommands& cmd) {
    ReactivePowerDerivation out{};
    out.smax_kva_used = smax_kva_effective(plant);
    if (!cmd.varsd_active) {
        return out;
    }

    double q = pct_smax_to_kvar(cmd.vartgt_spt_pct, out.smax_kva_used);

    if (plant.q_ind_kvar > 0.0 || plant.q_cap_kvar > 0.0) {
        const double q_max_ind = plant.q_ind_kvar > 0.0 ? plant.q_ind_kvar : out.smax_kva_used;
        const double q_max_cap = plant.q_cap_kvar > 0.0 ? plant.q_cap_kvar : out.smax_kva_used;
        q = std::clamp(q, -q_max_cap, q_max_ind);
    }

    out.q_target_kvar = q;
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
