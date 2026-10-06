#pragma once



/**

 * @file dso_phase1.hpp

 * @brief Bridge CEI equation layer → lab Pf2Config (Phase 1 only).

 */



#include "dso/dso_active_power.hpp"
#include "dso/dso_reactive_power.hpp"

#include "pf2/pf2_fsm.hpp"



namespace cci::core::dso {



struct DsoPhase1ApplyResult {

    ActivePowerDerivation derived{};

    bool applied{false};

    double pf2_threshold_kw{0.0};

    double pf2_release_kw{0.0};

};



struct DsoMockInputs {

    PlantEnvelope plant{};

    DsoActivePowerCommands commands{};

};



DsoMockInputs resolve_dso_mock_inputs(const PlantConfig& plant_cfg, const DsoConfig& dso_cfg);



DsoPhase1ApplyResult apply_dso_to_pf2_config(Pf2Config& cfg, const PlantConfig& plant_cfg,
                                             const DsoConfig& dso_cfg);

/**
 * Apply live Eth_A Wlim + WSd (MMS) onto PF2 thresholds.
 * Starts from yaml/TR mock, then overlays live active-power commands (O.11 min).
 */
DsoPhase1ApplyResult apply_live_dso_commands_to_pf2(Pf2Config& cfg,
                                                    const PlantConfig& plant_cfg,
                                                    const DsoConfig& dso_cfg,
                                                    bool wlim_active,
                                                    double wmax_spt_pct,
                                                    bool wsd_active,
                                                    double wspt_pct);

struct DsoReactiveApplyResult {
    ReactivePowerDerivation derived{};
    bool applied{false};
};

/** P5-R01 — live VArSd (O.9.1.4) → plant Q target kVAr. */
DsoReactiveApplyResult apply_live_varsd_command(const PlantConfig& plant_cfg,
                                                const DsoConfig& dso_cfg,
                                                bool varsd_active,
                                                double vartgt_spt_pct);

/** P5-R02 — live PFSP (O.9.1.1) → plant Q from cosφ × measured P. */
DsoReactiveApplyResult apply_live_pfsp_command(const PlantConfig& plant_cfg,
                                               const DsoConfig& dso_cfg,
                                               bool pfsp_active, double cosphi,
                                               bool generation_setpoint,
                                               double p_kw_measured);

}  // namespace cci::core::dso

