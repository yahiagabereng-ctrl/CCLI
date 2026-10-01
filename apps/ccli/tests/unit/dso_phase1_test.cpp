#include "dso/dso_active_power.hpp"
#include "dso/dso_phase1.hpp"
#include "dso/dso_reactive_power.hpp"
#include "dso/plant_envelope.hpp"
#include "dso/regulation_map.hpp"

/**
 * Unit tests for core/dso/ — regulation master map R01, R03, R05, O.7.3.1, Phase1 apply.
 * Vectors documented in test_vectors/l2_dso_phase1.yaml.
 */
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

bool near_eq(const double a, const double b, const double eps = 0.11) {
    return std::fabs(a - b) <= eps;
}

}  // namespace

int main() {
    using namespace cci::core;
    using namespace cci::core::dso;

    // R01 / Eq (1): TR limits → S_calc ≈ 206.15 kVA
    const PlantEnvelope tr_plant = tr57126_table1_plant();
    const double s_calc = calc_smax_kva(tr_plant);
    if (!near_eq(s_calc, 206.15, 0.2)) {
        std::cerr << "R01 S_calc expected ~206.15 kVA, got " << s_calc << '\n';
        return EXIT_FAILURE;
    }
    if (!near_eq(smax_kva_effective(tr_plant), 210.0, 0.01)) {
        std::cerr << "R01 Smax_used expected 210 kVA\n";
        return EXIT_FAILURE;
    }

    // R03 / TR Table 1: WSd 20% → ~42 kW
    const DsoActivePowerCommands tr_dso = tr57126_table1_dso();
    const auto derived = derive_active_power_kw(tr_plant, tr_dso);
    if (!near_eq(derived.p_wsd_kw, 42.0, 0.01)) {
        std::cerr << "R03 p_wsd expected 42 kW, got " << derived.p_wsd_kw << '\n';
        return EXIT_FAILURE;
    }
    if (!near_eq(derived.p_effective_export_kw, 42.0, 0.01)) {
        std::cerr << "R05 p_effective expected 42 kW\n";
        return EXIT_FAILURE;
    }

    // O.11 example: sp 50%, lim 70% → 50%
    PlantEnvelope p{};
    p.smax_kva_authoritative = 100.0;
    DsoActivePowerCommands d{};
    d.wlim_active = true;
    d.wmax_spt_pct = 70.0;
    d.wsd_active = true;
    d.wspt_pct = 50.0;
    const auto o11a = derive_active_power_kw(p, d);
    if (!near_eq(o11a.p_effective_export_kw, 50.0, 0.01)) {
        std::cerr << "O.11 case A expected 50 kW\n";
        return EXIT_FAILURE;
    }

    // O.11 example: sp 80%, lim 70% → 70%
    d.wspt_pct = 80.0;
    const auto o11b = derive_active_power_kw(p, d);
    if (!near_eq(o11b.p_effective_export_kw, 70.0, 0.01)) {
        std::cerr << "O.11 case B expected 70 kW\n";
        return EXIT_FAILURE;
    }

    // R06 / Eq (8): ±5% band
    if (!settled_within_band_kw(41.0, 42.0, 0.05)) {
        std::cerr << "O.7.3.1 band check should pass for 41 vs 42 kW\n";
        return EXIT_FAILURE;
    }
    if (settled_within_band_kw(50.0, 42.0, 0.05)) {
        std::cerr << "O.7.3.1 band check should fail for 50 vs 42 kW\n";
        return EXIT_FAILURE;
    }

    // Phase 1 apply → pf2 threshold from TR profile
    Pf2Config cfg{};
    cfg.threshold_kw = 900.0;
    cfg.use_dso_mock = true;
    DsoConfig dso_cfg{};
    dso_cfg.enabled = true;
    dso_cfg.profile = DsoProfile::Tr57126Table1;
    dso_cfg.release_delta_kw = 5.0;
    const auto applied = apply_dso_to_pf2_config(cfg, PlantConfig{}, dso_cfg);
    if (!applied.applied || !near_eq(cfg.threshold_kw, 42.0, 0.01)) {
        std::cerr << "Phase1 apply expected threshold 42 kW\n";
        return EXIT_FAILURE;
    }
    if (!near_eq(cfg.release_threshold_kw, 37.0, 0.01)) {
        std::cerr << "Phase1 apply expected release 37 kW\n";
        return EXIT_FAILURE;
    }

    // R04 — w110 cap 90% on 100 kVA → 90 kW
    PlantEnvelope p100{};
    p100.smax_kva_authoritative = 100.0;
    DsoActivePowerCommands w110{};
    w110.w110_active = true;
    w110.w110_pct = 90.0;
    const auto r04 = derive_active_power_kw(p100, w110);
    if (!near_eq(r04.p_effective_export_kw, 90.0, 0.01)) {
        std::cerr << "R04 w110 expected 90 kW\n";
        return EXIT_FAILURE;
    }

    // R03 — negative WSptPct ignored (export 100% cap remains)
    DsoActivePowerCommands neg{};
    neg.wsd_active = true;
    neg.wspt_pct = -30.0;
    const auto r03neg = derive_active_power_kw(p100, neg);
    if (!near_eq(r03neg.p_effective_export_kw, 100.0, 0.01)) {
        std::cerr << "R03 negative wspt should not reduce export cap in P1\n";
        return EXIT_FAILURE;
    }

    // Custom yaml mock — full O.11 50/70/100 on 210 kVA → 105 kW
    PlantConfig pc{};
    pc.smax_kva = 210.0;
    DsoConfig dc{};
    dc.enabled = true;
    dc.profile = DsoProfile::Custom;
    dc.w110_active = true;
    dc.w110_pct = 100.0;
    dc.wlim_active = true;
    dc.wmax_spt_pct = 70.0;
    dc.wsd_active = true;
    dc.wspt_pct = 50.0;
    const auto mock = resolve_dso_mock_inputs(pc, dc);
    const auto full = derive_active_power_kw(mock.plant, mock.commands);
    if (!near_eq(full.p_effective_export_kw, 105.0, 0.5)) {
        std::cerr << "Custom O.11 mock expected 105 kW got " << full.p_effective_export_kw << '\n';
        return EXIT_FAILURE;
    }

    // Live MMS overlay: Wlim off, WSd on @ 20 % (Figura 2) → 42 kW on 210 kVA
    Pf2Config live_pf2{};
    live_pf2.use_dso_mock = true;
    live_pf2.threshold_kw = 900.0;
    DsoConfig live_dc = dc;
    live_dc.w110_active = false;
    live_dc.wlim_active = false;
    live_dc.wsd_active = false;
    const auto live = apply_live_dso_commands_to_pf2(live_pf2, pc, live_dc,
                                                     /*wlim*/ false, 0.0,
                                                     /*wsd*/ true, 20.0);
    if (!live.applied || !near_eq(live.pf2_threshold_kw, 42.0, 0.5)) {
        std::cerr << "Live WSd Figura2 expected enter~42 kW got " << live.pf2_threshold_kw
                  << '\n';
        return EXIT_FAILURE;
    }
    if (!near_eq(live.derived.p_wsd_kw, 42.0, 0.5)) {
        std::cerr << "Live WSd p_wsd expected 42 kW\n";
        return EXIT_FAILURE;
    }

    // P5-R01 — VArSd 10 % Smax on 210 kVA → 21 kVAr
    DsoReactivePowerCommands varsd{};
    varsd.varsd_active = true;
    varsd.vartgt_spt_pct = 10.0;
    const auto q_derived = derive_reactive_kvar(tr_plant, varsd);
    if (!near_eq(q_derived.q_target_kvar, 21.0, 0.01)) {
        std::cerr << "P5-R01 VArSd 10% expected 21 kvar got " << q_derived.q_target_kvar
                  << '\n';
        return EXIT_FAILURE;
    }
    const auto q_live = apply_live_varsd_command(pc, dc, true, 10.0);
    if (!q_live.applied || !near_eq(q_live.derived.q_target_kvar, 21.0, 0.5)) {
        std::cerr << "P5-R01 live apply expected 21 kvar\n";
        return EXIT_FAILURE;
    }
    if (!settled_within_band_kvar(20.0, 21.0, 0.05)) {
        std::cerr << "P5-R01 TsQ band check failed\n";
        return EXIT_FAILURE;
    }

    (void)kClauseO_8_2;
    (void)kClauseO_9_2_3;
    (void)kClauseTR57126;
    (void)kLabL04;

    std::cout << "dso_phase1_test: PASS (R01–R06 mock, O.11, Phase1 apply, live WSd, P5-R01)\n";
    return EXIT_SUCCESS;
}
