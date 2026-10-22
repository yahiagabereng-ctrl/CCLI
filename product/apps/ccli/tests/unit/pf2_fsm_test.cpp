#include "pf2/pf2_fsm.hpp"

#include "measurement/measurement_store.hpp"

#include <cstdlib>
#include <iostream>

namespace {

bool expect_no_curtail(cci::core::Pf2Fsm& fsm, cci::core::Measurement& m, const std::int64_t t_ms,
                       const char* label) {
    m.timestamp_ms = t_ms;  // fresh sample each tick (real poll refreshes stamp)
    const auto st = fsm.step(m, t_ms);
    if (st.curtailment_active) {
        std::cerr << label << ": expected no curtailment\n";
        return false;
    }
    return true;
}

bool expect_curtail(cci::core::Pf2Fsm& fsm, cci::core::Measurement& m, const std::int64_t t_ms,
                    const char* label) {
    m.timestamp_ms = t_ms;  // fresh sample each tick (real poll refreshes stamp)
    const auto st = fsm.step(m, t_ms);
    if (!st.curtailment_active) {
        std::cerr << label << ": expected curtailment active\n";
        return false;
    }
    return true;
}

cci::core::Measurement good_measurement(const double p_kw, const std::int64_t t_ms) {
    cci::core::Measurement m{};
    m.p_kw = p_kw;
    m.quality = cci::core::DataQuality::Good;
    m.timestamp_ms = t_ms;
    return m;
}

}  // namespace

int main() {
    cci::core::Pf2Config cfg{};
    cfg.threshold_kw = 900.0;
    cfg.release_threshold_kw = 850.0;
    cfg.debounce_s = 30;
    cfg.min_on_s = 60;
    cfg.min_off_s = 30;
    cfg.stale_data_s = 10;

    cci::core::Pf2Fsm fsm(cfg);

    // L1-P-01: P = 500 kW -> no curtailment
    auto m = good_measurement(500, 0);
    if (!expect_no_curtail(fsm, m, 0, "L1-P-01")) {
        return EXIT_FAILURE;
    }

    // L1-P-02: P = 700 kW -> no curtailment
    m = good_measurement(700, 30'000);
    if (!expect_no_curtail(fsm, m, 30'000, "L1-P-02")) {
        return EXIT_FAILURE;
    }

    // L1-P-03: P = 950 kW -> curtail after debounce (30 s). Debounce is measured
    // from the first over-threshold evaluation, so step at t0, then before and
    // after the 30 s window.
    m = good_measurement(950, 60'000);
    if (!expect_no_curtail(fsm, m, 60'000, "L1-P-03 t0")) {
        return EXIT_FAILURE;
    }
    if (!expect_no_curtail(fsm, m, 89'000, "L1-P-03 pre-debounce")) {
        return EXIT_FAILURE;
    }
    if (!expect_curtail(fsm, m, 91'000, "L1-P-03")) {
        return EXIT_FAILURE;
    }

    // L1-P-04: P = 1000 kW -> remains active (no command storm / state flip)
    m = good_measurement(1000, 120'000);
    if (!expect_curtail(fsm, m, 120'000, "L1-P-04")) {
        return EXIT_FAILURE;
    }

    // Hysteresis: drop to 870 kW stays curtailed (above release 850)
    m = good_measurement(870, 130'000);
    if (!expect_curtail(fsm, m, 130'000, "hysteresis hold")) {
        return EXIT_FAILURE;
    }

    // Min-on: drop below release (800 < 850). Must hold curtailment until both
    // the release debounce (30 s) AND minimum on-time (60 s from start ~91 s)
    // are satisfied, then release.
    m = good_measurement(800, 140'000);
    if (!expect_curtail(fsm, m, 140'000, "min-on hold t0")) {
        return EXIT_FAILURE;
    }
    if (!expect_curtail(fsm, m, 160'000, "min-on hold (debounce not met)")) {
        return EXIT_FAILURE;
    }
    if (!expect_no_curtail(fsm, m, 175'000, "release after min-on + debounce")) {
        return EXIT_FAILURE;
    }

    // L1-C-01: stale/invalid quality -> safe state
    m = good_measurement(500, 200'000);
    m.quality = cci::core::DataQuality::Stale;
    const auto safe = fsm.step(m, 200'000);
    if (safe.state != cci::core::Pf2StateId::SafeState) {
        std::cerr << "L1-C-01: expected safe state on stale quality\n";
        return EXIT_FAILURE;
    }

    // Timestamp guard: Good quality but no timestamp must be treated as stale.
    cci::core::Measurement no_ts{};
    no_ts.p_kw = 950;
    no_ts.quality = cci::core::DataQuality::Good;
    no_ts.timestamp_ms = 0;
    const auto guarded = fsm.step(no_ts, 210'000);
    if (guarded.state != cci::core::Pf2StateId::SafeState) {
        std::cerr << "timestamp guard: expected safe state on zero timestamp\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
