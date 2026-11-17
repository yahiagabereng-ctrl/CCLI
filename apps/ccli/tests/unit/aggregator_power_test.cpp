#include "dso/dso_phase1.hpp"
#include "mms_adapter.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

static void check(bool ok, const char* reason) { if (!ok) throw std::runtime_error(reason); }
static bool near(double a, double b) { return std::abs(a-b) < .001; }

int main() {
    using namespace cci::core;
    using namespace cci::core::dso;
    try {
        PlantConfig plant{};
        plant.smax_kva = 100; plant.q_ind_kvar = 50; plant.q_cap_kvar = 40;
        DsoConfig cfg{};
        cfg.enabled = true; cfg.release_delta_kw = 5;
        Pf2Config pf{};
        auto p = apply_live_dso_commands_to_pf2(pf, plant, cfg, true, 70, false, 0, true, 50);
        check(near(p.pf2_threshold_kw, 50) && near(p.pf2_release_kw, 45), "Aggregator ceiling was not applied");
        p = apply_live_dso_commands_to_pf2(pf, plant, cfg, true, 70, false, 0, true, 90);
        check(near(p.pf2_threshold_kw, 70), "Aggregator relaxed the DSO ceiling");
        p = apply_live_dso_commands_to_pf2(pf, plant, cfg, true, 70, true, 30, true, 90);
        check(near(p.pf2_threshold_kw, 30), "Aggregator relaxed the DSO modulation cap");
        p = apply_live_dso_commands_to_pf2(pf, plant, cfg, false, 0, false, 0, true, 0);
        check(near(p.pf2_threshold_kw, 0), "Zero Aggregator ceiling was ignored");
        p = apply_live_dso_commands_to_pf2(pf, plant, cfg, true, 70, false, 0, false, 10);
        check(near(p.pf2_threshold_kw, 70), "Disabled Aggregator still constrained export");

        DsoConfig autonomous = cfg;
        autonomous.profile = DsoProfile::Tr57126Table1;
        p = apply_live_dso_commands_to_pf2(pf, plant, autonomous,
                                         false, 0, false, 0, true, 0, true);
        check(near(p.pf2_threshold_kw, 42) && near(p.pf2_release_kw, 37),
              "Operating Rule fallback was confused with the live Aggregator ceiling");

        auto q = select_live_reactive_command(plant, cfg, false, 0, false, 1, true, 20, true, 10);
        check(q.command.applied && near(q.command.derived.q_target_kvar, 10), "Aggregator Q was not applied");
        q = select_live_reactive_command(plant, cfg, false, 0, false, 1, true, 20, true, -100);
        check(near(q.command.derived.q_target_kvar, -40), "Aggregator Q exceeded capacitive envelope");
        q = select_live_reactive_command(plant, cfg, true, 20, false, 1, true, 20, true, -30);
        check(std::string(q.source) == "DSO_VArSd" && near(q.command.derived.q_target_kvar, 20), "Aggregator overrode DSO Q");
        q = select_live_reactive_command(plant, cfg, false, 0, true, -.8, true, 20, true, -30);
        check(std::string(q.source) == "DSO_PFSP" && near(q.command.derived.q_target_kvar, 15), "Aggregator overrode DSO PFSP");
        q = select_live_reactive_command(plant, cfg, false, 0, false, 1, true, 20, true, -30);
        check(std::string(q.source) == "AGGREGATOR_VArSa" && near(q.command.derived.q_target_kvar, -30), "DSO release did not resume Aggregator Q");
        q = select_live_reactive_command(plant, cfg, false, 0, false, 1, true, 20, false, -30);
        check(!q.command.applied && near(q.command.derived.q_target_kvar, 0), "Off did not clear Q selection");

        cci::adapters::DsoLiveCommand state{}, update{};
        state.wsd_active = true; state.wspt_pct = 20;
        state.varsd_active = true; state.vartgt_spt_pct = 15;
        update.valid = true; update.aggregator_dirty = true;
        update.wsa_active = true; update.wsa_spt_pct = 90;
        update.aggregator_reactive_dirty = true; update.varsa_active = true; update.varsa_spt_pct = -30;
        state.merge_update(update);
        check(state.wsd_active && near(state.wspt_pct, 20) && state.varsd_active,
              "Aggregator delta erased configured/previous DSO controls");
        update = {}; update.valid = true; update.dirty = true;
        state.merge_update(update);
        check(!state.wsd_active && state.wsa_active && state.varsa_active && state.varsd_active,
              "DSO active-power delta erased Aggregator or reactive controls");
        update = {}; update.valid = true; update.dirty = update.reactive_dirty = true;
        update.aggregator_dirty = update.aggregator_reactive_dirty = true;
        state.merge_update(update);
        check(!state.wsa_active && !state.varsa_active && !state.varsd_active, "Fallback delta did not clear both actors");
        cci::core::MmsConfig defaults{};
        check(!defaults.aggregator_plant_outputs_enabled, "Aggregator outputs enabled by default");
        state.wsa_active = state.varsa_active = true;
        state.aggregator_dirty = state.aggregator_reactive_dirty = true;
        state.wsd_active = state.varsd_active = true;
        state.dirty = state.reactive_dirty = true;
        const auto output = state.for_plant(false);
        check(!output.wsa_active && !output.varsa_active && !output.aggregator_dirty && !output.aggregator_reactive_dirty,
              "Disabled Aggregator reached a plant command");
        check(output.wsd_active && output.varsd_active && output.dirty && output.reactive_dirty,
              "Aggregator safety gate erased DSO state");
        std::cout << "Aggregator arbitration, output gate and state preservation passed\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
