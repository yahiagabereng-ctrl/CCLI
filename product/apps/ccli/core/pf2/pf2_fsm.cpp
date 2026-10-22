#include "pf2/pf2_fsm.hpp"

namespace cci::core {

Pf2Fsm::Pf2Fsm(Pf2Config cfg) : cfg_(cfg) {
    if (cfg_.release_threshold_kw >= cfg_.threshold_kw) {
        cfg_.release_threshold_kw = cfg_.threshold_kw - 50.0;
    }
    state_.state = Pf2StateId::Normal;
    state_.reason = "init";
}

void Pf2Fsm::set_thresholds(double enter_kw, double release_kw) {
    cfg_.threshold_kw = enter_kw;
    cfg_.release_threshold_kw = release_kw;
    if (cfg_.release_threshold_kw >= cfg_.threshold_kw && cfg_.threshold_kw > 0.0) {
        cfg_.release_threshold_kw = cfg_.threshold_kw - 5.0;
        if (cfg_.release_threshold_kw < 0.0) {
            cfg_.release_threshold_kw = 0.0;
        }
    }
}

bool Pf2Fsm::is_stale(const Measurement& m, const std::int64_t now_ms) const {
    if (m.quality != DataQuality::Good) {
        return true;
    }
    // A Good sample with no valid timestamp cannot be age-verified, so it must
    // not be trusted to drive curtailment (R-PF2-001: stale data -> safe state).
    if (m.timestamp_ms <= 0) {
        return true;
    }
    return (now_ms - m.timestamp_ms) > (static_cast<std::int64_t>(cfg_.stale_data_s) * 1000);
}

void Pf2Fsm::enter_safe_state(const std::string& reason) {
    state_.state = Pf2StateId::SafeState;
    state_.reason = reason;
    state_.curtailment_active = false;
    over_threshold_since_ms_ = -1;
    under_release_since_ms_ = -1;
}

Pf2State Pf2Fsm::step(const Measurement& m, const std::int64_t now_ms) {
    if (is_stale(m, now_ms)) {
        const auto reason = m.quality != DataQuality::Good ? "stale_or_invalid" : "stale_data_timeout";
        enter_safe_state(reason);
        return state_;
    }

    if (state_.state == Pf2StateId::SafeState && m.quality == DataQuality::Good) {
        state_.state = Pf2StateId::Normal;
        state_.reason = "recovered";
    }

    const bool above_enter = m.p_kw > cfg_.threshold_kw;
    const bool above_release = m.p_kw > cfg_.release_threshold_kw;

    if (state_.curtailment_active || state_.state == Pf2StateId::CurtailActive) {
        if (!above_release) {
            if (under_release_since_ms_ < 0) {
                under_release_since_ms_ = now_ms;
            }
            const auto elapsed_s = (now_ms - under_release_since_ms_) / 1000;
            const auto on_time_s =
                curtailment_started_ms_ >= 0 ? (now_ms - curtailment_started_ms_) / 1000 : cfg_.min_on_s;
            const auto off_ok = curtailment_cleared_ms_ < 0 ||
                                (now_ms - curtailment_cleared_ms_) / 1000 >= cfg_.min_off_s;
            if (elapsed_s >= cfg_.debounce_s && on_time_s >= cfg_.min_on_s && off_ok) {
                state_.state = Pf2StateId::Normal;
                state_.reason = "released";
                state_.curtailment_active = false;
                curtailment_cleared_ms_ = now_ms;
                curtailment_started_ms_ = -1;
                over_threshold_since_ms_ = -1;
                under_release_since_ms_ = -1;
            } else {
                state_.state = Pf2StateId::CurtailActive;
                state_.reason = "hysteresis_hold";
                state_.curtailment_active = true;
            }
        } else {
            under_release_since_ms_ = -1;
            state_.state = Pf2StateId::CurtailActive;
            state_.reason = "p_above_threshold";
            state_.curtailment_active = true;
        }
        return state_;
    }

    if (above_enter) {
        if (over_threshold_since_ms_ < 0) {
            over_threshold_since_ms_ = now_ms;
        }
        const auto elapsed_s = (now_ms - over_threshold_since_ms_) / 1000;
        if (elapsed_s >= cfg_.debounce_s) {
            state_.state = Pf2StateId::CurtailActive;
            state_.reason = "p_above_threshold";
            state_.curtailment_active = true;
            if (curtailment_started_ms_ < 0) {
                curtailment_started_ms_ = now_ms;
            }
        } else {
            state_.state = Pf2StateId::CurtailPending;
            state_.reason = "debounce";
            state_.curtailment_active = false;
        }
    } else {
        over_threshold_since_ms_ = -1;
        state_.state = Pf2StateId::Normal;
        state_.reason = "ok";
        state_.curtailment_active = false;
    }

    return state_;
}

}  // namespace cci::core
