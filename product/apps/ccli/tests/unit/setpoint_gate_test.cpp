#include "dso/setpoint_gate.hpp"
#include "dso/regulation_map.hpp"

/**
 * Unit test — R07 / O.7.3.3 / Eq (9): minimum 3 s between processed external set-points.
 * Vectors: first write accepted; +2999 ms rejected; +3000 ms accepted; gate off accepts all;
 * clock step backwards fails safe (accept + re-base).
 */
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {

int fail(const char* what) {
    std::cerr << "setpoint_gate_test FAIL: " << what << '\n';
    return EXIT_FAILURE;
}

}  // namespace

int main() {
    using namespace cci::core::dso;

    if (kSetpointMinIntervalMsO733 != 3000U) {
        return fail("normative default must be 3000 ms");
    }

    SetpointSpacingGate gate;  // 3 s
    if (!gate.enabled() || gate.has_last()) {
        return fail("fresh gate state");
    }
    if (!gate.accept(10'000)) {
        return fail("first set-point must be accepted");
    }
    if (gate.accept(12'999)) {
        return fail("2999 ms after last must be rejected");
    }
    if (gate.remaining_ms(12'999) != 1U) {
        return fail("remaining_ms at 2999 ms should be 1");
    }
    if (gate.last_ms() != 10'000) {
        return fail("rejected write must not re-base t_last");
    }
    if (!gate.accept(13'000)) {
        return fail("exactly 3000 ms after last must be accepted");
    }
    if (gate.accept(13'001)) {
        return fail("burst after accepted write must be rejected");
    }
    if (gate.remaining_ms(16'000) != 0U) {
        return fail("remaining_ms should be 0 once interval elapsed");
    }

    // Clock step backwards: fail safe (accept, re-base).
    if (!gate.accept(5'000)) {
        return fail("backwards clock must not dead-lock the gate");
    }
    if (gate.last_ms() != 5'000) {
        return fail("re-base after backwards clock");
    }

    gate.reset();
    if (gate.has_last() || !gate.accept(1)) {
        return fail("reset must allow immediate accept");
    }

    // Gate disabled (lab) — every write accepted.
    SetpointSpacingGate off(0U);
    if (off.enabled() || !off.accept(100) || !off.accept(101) || !off.accept(102)) {
        return fail("disabled gate must accept everything");
    }

    // Pure rule.
    if (setpoint_spacing_ok(true, 0, 2'999, 3000) || !setpoint_spacing_ok(true, 0, 3'000, 3000) ||
        !setpoint_spacing_ok(false, 0, 0, 3000)) {
        return fail("setpoint_spacing_ok vectors");
    }

    // Master map row R07 must now be Pass with Eq (9).
    bool r07_ok = false;
    for (std::size_t i = 0; i < kRegulationMasterMapCount; ++i) {
        const RegulationMapEntry& e = kRegulationMasterMap[i];
        if (std::strcmp(e.id, "R07") == 0) {
            r07_ok = (e.default_p1 == RegulationP1Status::Pass) &&
                     (std::strcmp(e.equation, kEq9) == 0) &&
                     (std::strcmp(e.clause, kClauseO_7_3_3) == 0);
        }
    }
    if (!r07_ok) {
        return fail("regulation_map R07 must be Pass / Eq (9) / O.7.3.3");
    }

    std::cout << "setpoint_gate_test PASS (R07 O.7.3.3 Eq (9))\n";
    return EXIT_SUCCESS;
}
