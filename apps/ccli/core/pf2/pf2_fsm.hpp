#pragma once

#include "measurement/measurement_store.hpp"

#include <cstdint>
#include <string>

namespace cci::core {

enum class Pf2StateId : std::uint8_t {
    Normal,
    CurtailPending,
    CurtailActive,
    SafeState,
};

struct Pf2State {
    Pf2StateId state{Pf2StateId::SafeState};
    std::string reason;
    bool curtailment_active{false};
};

struct Pf2Config {
    double threshold_kw{900.0};          /**< L01 — enter curtail when P > this (kW) */
    double release_threshold_kw{850.0};    /**< L02 — hysteresis release when P < this (kW) */
    int debounce_s{30};
    int min_on_s{60};
    int min_off_s{30};
    int stale_data_s{10};                /**< L03 — meter age / bad quality → SafeState */
    /**
     * When true AND dso.enabled: threshold/release overwritten by core/dso/dso_phase1.cpp
     * from CEI % Smax (O.8.2, O.11). When false: use threshold_kw / release_threshold_kw as-is.
     */
    bool use_dso_mock{false};
};

/** Yaml `plant:` section → O.8.2 inputs (see core/dso/plant_envelope.hpp). */
struct PlantConfig {
    double p_imm_kw{200.0};
    double p_ass_kw{200.0};
    double q_ind_kvar{50.0};
    double q_cap_kvar{50.0};
    double smax_kva{210.0};  /**< Authoritative Smax (kVA); Eq (1) may differ slightly */
    /** Lab POC line-line V (kV) until analyzer PPV register mapped (P5-M08). */
    double poc_ppv_kv{20.0};
};

enum class DsoProfile : std::uint8_t {
    Custom,          /**< Use plant: + dso: numeric fields from yaml */
    Tr57126Table1,   /**< CEI TR 57-126 Table 1 / Figura 2 preset (WSd 20 %, Wlim off) */
};

/** Yaml `dso:` section → mock of Wlim/WSd until Phase 3 MMS (core/dso/dso_active_power.hpp). */
struct DsoConfig {
    bool enabled{false};   /**< Master switch for equation layer */
    DsoProfile profile{DsoProfile::Custom};
    bool w110_active{false};
    double w110_pct{100.0};
    bool wlim_active{false};
    double wmax_spt_pct{0.0};
    bool wsd_active{false};
    double wspt_pct{20.0};
    double release_delta_kw{50.0};  /**< Hysteresis span when DSO mock rewrites thresholds */
};

class Pf2Fsm {
public:
    explicit Pf2Fsm(Pf2Config cfg);

    Pf2State step(const Measurement& m, std::int64_t now_ms);

    /** Hot-update enter/release thresholds (P3-04 live Wlim). */
    void set_thresholds(double enter_kw, double release_kw);

    const Pf2Config& config() const { return cfg_; }

private:
    bool is_stale(const Measurement& m, std::int64_t now_ms) const;
    void enter_safe_state(const std::string& reason);

    Pf2Config cfg_;
    Pf2State state_{};
    std::int64_t over_threshold_since_ms_{-1};
    std::int64_t under_release_since_ms_{-1};
    std::int64_t curtailment_started_ms_{-1};
    std::int64_t curtailment_cleared_ms_{-1};
};

}  // namespace cci::core
