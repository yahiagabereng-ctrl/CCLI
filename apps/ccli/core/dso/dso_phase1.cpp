#include "dso/dso_phase1.hpp"

#include <algorithm>

namespace cci::core::dso {

namespace {

PlantEnvelope plant_from_config(const PlantConfig& c) {
    PlantEnvelope p{};
    p.p_imm_kw = c.p_imm_kw;
    p.p_ass_kw = c.p_ass_kw;
    p.q_ind_kvar = c.q_ind_kvar;
    p.q_cap_kvar = c.q_cap_kvar;
    p.smax_kva_authoritative = c.smax_kva;
    return p;
}

DsoActivePowerCommands dso_from_config(const DsoConfig& c) {
    DsoActivePowerCommands d{};
    d.w110_active = c.w110_active;
    d.w110_pct = c.w110_pct;
    d.wlim_active = c.wlim_active;
    d.wmax_spt_pct = c.wmax_spt_pct;
    d.wsd_active = c.wsd_active;
    d.wspt_pct = c.wspt_pct;
    return d;
}

}  // namespace

DsoMockInputs resolve_dso_mock_inputs(const PlantConfig& plant_cfg, const DsoConfig& dso_cfg) {
    DsoMockInputs out{};
    if (dso_cfg.profile == DsoProfile::Tr57126Table1) {
        out.plant = tr57126_table1_plant();
        out.commands = tr57126_table1_dso();
    } else {
        out.plant = plant_from_config(plant_cfg);
        out.commands = dso_from_config(dso_cfg);
    }
    return out;
}

DsoPhase1ApplyResult apply_dso_to_pf2_config(Pf2Config& cfg, const PlantConfig& plant_cfg,
                                             const DsoConfig& dso_cfg) {
    DsoPhase1ApplyResult result{};

    if (!dso_cfg.enabled || !cfg.use_dso_mock) {
        result.pf2_threshold_kw = cfg.threshold_kw;
        result.pf2_release_kw = cfg.release_threshold_kw;
        return result;
    }

    const DsoMockInputs mock = resolve_dso_mock_inputs(plant_cfg, dso_cfg);

    result.derived = derive_active_power_kw(mock.plant, mock.commands);
    result.applied = true;
    result.pf2_threshold_kw = result.derived.p_effective_export_kw;

    const double delta = dso_cfg.release_delta_kw > 0.0 ? dso_cfg.release_delta_kw : 50.0;
    result.pf2_release_kw = std::max(0.0, result.pf2_threshold_kw - delta);

    cfg.threshold_kw = result.pf2_threshold_kw;
    cfg.release_threshold_kw = result.pf2_release_kw;

    /* Pf2Fsm ctor also clamps release if release >= enter; keep yaml-derived pair consistent. */
    if (cfg.release_threshold_kw >= cfg.threshold_kw && cfg.threshold_kw > 0.0) {
        cfg.release_threshold_kw = std::max(0.0, cfg.threshold_kw - delta);
    }

    return result;
}

DsoPhase1ApplyResult apply_live_dso_commands_to_pf2(Pf2Config& cfg,
                                                    const PlantConfig& plant_cfg,
                                                    const DsoConfig& dso_cfg,
                                                    bool wlim_active,
                                                    double wmax_spt_pct,
                                                    bool wsd_active,
                                                    double wspt_pct) {
    DsoPhase1ApplyResult result{};

    if (!dso_cfg.enabled || !cfg.use_dso_mock) {
        if (!dso_cfg.enabled) {
            result.pf2_threshold_kw = cfg.threshold_kw;
            result.pf2_release_kw = cfg.release_threshold_kw;
            return result;
        }
    }

    DsoMockInputs mock = resolve_dso_mock_inputs(plant_cfg, dso_cfg);
    mock.commands.wlim_active = wlim_active;
    mock.commands.wmax_spt_pct = wmax_spt_pct;
    mock.commands.wsd_active = wsd_active;
    mock.commands.wspt_pct = wspt_pct;

    result.derived = derive_active_power_kw(mock.plant, mock.commands);
    result.applied = true;
    result.pf2_threshold_kw = result.derived.p_effective_export_kw;

    const double delta = dso_cfg.release_delta_kw > 0.0 ? dso_cfg.release_delta_kw : 5.0;
    result.pf2_release_kw = std::max(0.0, result.pf2_threshold_kw - delta);

    cfg.threshold_kw = result.pf2_threshold_kw;
    cfg.release_threshold_kw = result.pf2_release_kw;
    if (cfg.release_threshold_kw >= cfg.threshold_kw && cfg.threshold_kw > 0.0) {
        cfg.release_threshold_kw = std::max(0.0, cfg.threshold_kw - delta);
    }

    return result;
}

DsoReactiveApplyResult apply_live_varsd_command(const PlantConfig& plant_cfg,
                                                const DsoConfig& dso_cfg,
                                                const bool varsd_active,
                                                const double vartgt_spt_pct) {
    DsoReactiveApplyResult result{};
    if (!dso_cfg.enabled) {
        return result;
    }

    DsoMockInputs mock = resolve_dso_mock_inputs(plant_cfg, dso_cfg);
    DsoReactivePowerCommands reactive{};
    reactive.varsd_active = varsd_active;
    reactive.vartgt_spt_pct = vartgt_spt_pct;

    result.derived = derive_reactive_kvar(mock.plant, reactive);
    result.applied = varsd_active;
    return result;
}

DsoReactiveApplyResult apply_live_pfsp_command(const PlantConfig& plant_cfg,
                                               const DsoConfig& dso_cfg,
                                               const bool pfsp_active,
                                               const double cosphi,
                                               const bool generation_setpoint,
                                               const double p_kw_measured) {
    DsoReactiveApplyResult result{};
    if (!dso_cfg.enabled) {
        return result;
    }

    DsoMockInputs mock = resolve_dso_mock_inputs(plant_cfg, dso_cfg);
    PfspCommand pfsp{};
    pfsp.active = pfsp_active;
    pfsp.cosphi = cosphi;
    pfsp.generation = generation_setpoint;

    result.derived = derive_pfsp_kvar(mock.plant, p_kw_measured, pfsp);
    result.applied = pfsp_active;
    return result;
}

}  // namespace cci::core::dso
