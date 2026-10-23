#pragma once

#include <algorithm>
#include <cmath>

namespace cci::core {

/** PdCMMXU1 PPV (DEL) + A (WYE) magnitudes and angles for MMS CMV leaves. */
struct PdcMmxuPhasors {
    bool valid{false};

    double ppv_ab_kv{0.0};
    double ppv_bc_kv{0.0};
    double ppv_ca_kv{0.0};
    double ppv_ab_ang_deg{0.0};
    double ppv_bc_ang_deg{0.0};
    double ppv_ca_ang_deg{0.0};

    double a_phs_a{0.0};
    double a_phs_b{0.0};
    double a_phs_c{0.0};
    double a_phs_a_ang_deg{0.0};
    double a_phs_b_ang_deg{0.0};
    double a_phs_c_ang_deg{0.0};
};

inline double pdc_current_angle_deg_from_pq(const double p_w, const double q_var) {
    constexpr double k_eps = 1.0;
    if (std::fabs(p_w) < k_eps && std::fabs(q_var) < k_eps) {
        return 0.0;
    }
    return std::atan2(q_var, p_w) * (180.0 / M_PI);
}

/** When EMT432 Q1 float is ~0 (incl. −0), use PF1 (cos φ) for a visible lab angle. */
inline double pdc_current_angle_deg_from_pq_or_pf(const double p_w, const double q_var,
                                                    const double pf1) {
    constexpr double k_q_eps_var = 5.0;
    constexpr double k_p_eps_w = 50.0;
    if (std::fabs(q_var) >= k_q_eps_var) {
        return pdc_current_angle_deg_from_pq(p_w, q_var);
    }
    if (std::fabs(p_w) < k_p_eps_w || pf1 <= 0.0 || pf1 > 1.05) {
        return pdc_current_angle_deg_from_pq(p_w, q_var);
    }
    const double pf = std::clamp(std::fabs(pf1), 0.0, 1.0);
    double phi_deg = std::acos(pf) * (180.0 / M_PI);
    if (p_w < 0.0) {
        phi_deg = -phi_deg;
    }
    return phi_deg;
}

inline bool pdc_emt432_three_phase_active(const double i2_a, const double i3_a,
                                          const double v_bc_kv) {
    constexpr double k_i = 0.05;
    constexpr double k_v = 0.01;
    return std::fabs(i2_a) > k_i || std::fabs(i3_a) > k_i || std::fabs(v_bc_kv) > k_v;
}

/** DEL voltage angles: reference AB=0°; nominal −120°/120° when 3φ active. */
inline void pdc_fill_del_voltage_angles(const bool three_phase_active, PdcMmxuPhasors& p) {
    p.ppv_ab_ang_deg = 0.0;
    if (three_phase_active) {
        p.ppv_bc_ang_deg = -120.0;
        p.ppv_ca_ang_deg = 120.0;
    } else {
        p.ppv_bc_ang_deg = 0.0;
        p.ppv_ca_ang_deg = 0.0;
    }
}

}  // namespace cci::core
