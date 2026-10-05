#pragma once

/**
 * @file setpoint_gate.hpp
 * @brief O.7.3.3 "External Set Point Dynamics" — Eq (9) minimum spacing gate.
 *
 * Annex O: minimum interval between *processed* external set-point updates is 3 s;
 * commands arriving faster are rejected. The first set-point is always accepted.
 *
 * Draw.io: Flowcharts/DSO_P_limit O_9_2_2.drawio ("Δt = t_now − t_last ... Δt ≥ 3 s").
 * Master map: R07 (regulation_map.hpp). Header-only, no platform dependency, unit-tested.
 *
 * Scope decision: one gate per MMS server (global across WMaxSptPct / WSptPct /
 * VArTgtSptPct) — Annex O speaks of plant set-point dynamics, not per-object.
 * Activation (Mod) writes are not set-points and are not gated.
 */

#include <cstdint>

namespace cci::core::dso {

/** Normative default — O.7.3.3. */
constexpr std::uint32_t kSetpointMinIntervalMsO733 = 3000U;

/**
 * Pure spacing rule: accept when no previous processed set-point exists or
 * (now − last) ≥ min_interval. A min_interval of 0 disables the gate.
 */
constexpr bool setpoint_spacing_ok(const bool has_last, const std::uint64_t last_ms,
                                   const std::uint64_t now_ms,
                                   const std::uint32_t min_interval_ms) {
    if (min_interval_ms == 0U || !has_last) {
        return true;
    }
    if (now_ms < last_ms) {
        /* Clock went backwards (step) — fail safe: accept and re-base. */
        return true;
    }
    return (now_ms - last_ms) >= static_cast<std::uint64_t>(min_interval_ms);
}

/** Stateful gate — call accept() once per external set-point write. */
class SetpointSpacingGate {
public:
    explicit SetpointSpacingGate(const std::uint32_t min_interval_ms = kSetpointMinIntervalMsO733)
        : min_interval_ms_(min_interval_ms) {}

    /** True → set-point may be processed; the timestamp is recorded. False → reject. */
    bool accept(const std::uint64_t now_ms) {
        if (!setpoint_spacing_ok(has_last_, last_ms_, now_ms, min_interval_ms_)) {
            return false;
        }
        has_last_ = true;
        last_ms_ = now_ms;
        return true;
    }

    /** Milliseconds still to wait before the next set-point is accepted (0 = ready). */
    std::uint64_t remaining_ms(const std::uint64_t now_ms) const {
        if (setpoint_spacing_ok(has_last_, last_ms_, now_ms, min_interval_ms_)) {
            return 0U;
        }
        return static_cast<std::uint64_t>(min_interval_ms_) - (now_ms - last_ms_);
    }

    void reset() {
        has_last_ = false;
        last_ms_ = 0U;
    }

    std::uint32_t min_interval_ms() const { return min_interval_ms_; }
    bool enabled() const { return min_interval_ms_ > 0U; }
    bool has_last() const { return has_last_; }
    std::uint64_t last_ms() const { return last_ms_; }

private:
    std::uint32_t min_interval_ms_;
    bool has_last_{false};
    std::uint64_t last_ms_{0U};
};

}  // namespace cci::core::dso
