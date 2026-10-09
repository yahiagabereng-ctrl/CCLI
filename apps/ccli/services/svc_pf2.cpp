#include "svc_pf2.hpp"

#include "cci/hal/time.hpp"

namespace cci::services {

namespace {

core::Pf2Config lab_pf2_config() {
    core::Pf2Config cfg{};
    cfg.threshold_kw = 900.0;
    cfg.release_threshold_kw = 850.0;
    cfg.debounce_s = 30;
    cfg.min_on_s = 60;
    cfg.min_off_s = 30;
    cfg.stale_data_s = 10;
    return cfg;
}

}  // namespace

Pf2Service::Pf2Service(core::MeasurementStore& measurements, core::EventStore& events)
    : measurements_(measurements), events_(events), fsm_(lab_pf2_config()) {}

Pf2Service::Pf2Service(core::MeasurementStore& measurements, core::EventStore& events,
                       core::Pf2Config cfg)
    : measurements_(measurements), events_(events), fsm_(cfg) {}

void Pf2Service::tick() {
    const auto m = measurements_.snapshot();
    const auto now = cci::hal::now().epoch_ms;
    const auto st = fsm_.step(m, now);
    curtailment_active_ = st.curtailment_active;
    const bool log_pf2 = st.state == core::Pf2StateId::SafeState || st.curtailment_active;
    if (log_pf2 &&
        (st.state != last_state_.state || st.reason != last_state_.reason ||
         st.curtailment_active != last_state_.curtailment_active)) {
        events_.append({now, "pf2", st.reason});
    }
    last_state_ = st;
}

}  // namespace cci::services
