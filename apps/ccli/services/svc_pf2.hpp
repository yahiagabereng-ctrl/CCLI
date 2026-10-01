#pragma once

#include "event/event_ring.hpp"
#include "measurement/measurement_store.hpp"
#include "pf2/pf2_fsm.hpp"

namespace cci::services {

class Pf2Service {
public:
    Pf2Service(core::MeasurementStore& measurements, core::EventRing& events);
    Pf2Service(core::MeasurementStore& measurements, core::EventRing& events,
               core::Pf2Config cfg);

    void tick();

    // Last PF2 curtailment decision (drives DO via svc_io). Safe default: false.
    bool curtailment_active() const { return curtailment_active_; }

    const core::Pf2State& last_state() const { return last_state_; }

    void set_thresholds(double enter_kw, double release_kw) {
        fsm_.set_thresholds(enter_kw, release_kw);
    }

    const core::Pf2Config& config() const { return fsm_.config(); }

private:
    core::MeasurementStore& measurements_;
    core::EventRing& events_;
    core::Pf2Fsm fsm_;
    bool curtailment_active_{false};
    core::Pf2State last_state_{};
};

}  // namespace cci::services
