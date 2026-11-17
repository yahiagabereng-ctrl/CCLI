#pragma once

#include "config/ccli_config.hpp"
#include "mms_rbac.hpp"
#include "measurement/pdc_mmxu_phasors.hpp"

#include <cstdint>
#include <functional>
#include <string>

namespace cci::core {
enum class DataQuality : std::uint8_t;
}

namespace cci::adapters {

/** O.14 audit hook — invoked from MMS connection / ACSE auth paths (P7-04). */
using MmsAuditFn = std::function<void(const std::string& detail)>;
using MmsTimeSyncAuditFn = std::function<bool(const std::string& iec_event_id,
                                              const std::string& source,
                                              std::int64_t offset_ms,
                                              bool gnss_fix,
                                              bool synchronized)>;

/**
 * Live DSO commands from Eth_A MMS.
 * O.9.2.2 Wlim · O.9.2.3 WSd · O.9.1.4 VArSd (P5-R01) · O.9.1.1 PFSP (P5-R02).
 */
struct DsoLiveCommand {
    bool valid{false};
    bool dirty{false};          /**< Active-power (Wlim/WSd) changed */
    bool reactive_dirty{false}; /**< Reactive (VArSd / PFSP) changed */
    bool pfsp_dirty{false};     /**< PFSP cosφ set-point changed — P5-R02 */
    bool wlim_active{false};    /**< WlimDWMX1.Mod 1=on, 5=off */
    double wmax_spt_pct{0.0};
    bool wsd_active{false}; /**< WSdDAGC1.Mod — Figura 2 s.p. W */
    double wspt_pct{0.0};   /**< WSdDAGC1.WSptPct % Smax */
    bool varsd_active{false};   /**< VArSdDVAR1.Mod — O.9.1.4 Q set-point */
    double vartgt_spt_pct{0.0}; /**< VArSdDVAR1.VArTgtSptPct % Smax (signed) */
    bool pfsp_active{false};    /**< PFSPDFPF1.Mod — O.9.1.1 cosφ */
    double pfsp_cosphi{1.0};    /**< PFGnTgtSpt or PFLodTgtSpt APC value */
    bool pfsp_generation{true}; /**< true = PFGnTgtSpt, false = PFLodTgtSpt */
    bool aggregator_dirty{false};
    bool aggregator_reactive_dirty{false};
    bool wsa_active{false};
    double wsa_spt_pct{100.0}; /**< Signed requested P target (% Smax); not an export ceiling. */
    bool varsa_active{false};
    double varsa_spt_pct{0.0}; /**< Signed Q target as percent Smax. */

    // Only this filtered snapshot may reach plant outputs. Preserve DSO state and deltas.
    DsoLiveCommand for_plant(bool aggregator_enabled) const {
        auto out = *this;
        if (!aggregator_enabled) {
            out.wsa_active = false; out.varsa_active = false;
            out.aggregator_dirty = false; out.aggregator_reactive_dirty = false;
        }
        return out;
    }

    // Preserve configured/previous DSO commands when only an Aggregator delta arrives.
    void merge_update(const DsoLiveCommand& u) {
        if (u.dirty) {
            wlim_active = u.wlim_active; wmax_spt_pct = u.wmax_spt_pct;
            wsd_active = u.wsd_active; wspt_pct = u.wspt_pct;
        }
        if (u.reactive_dirty) {
            varsd_active = u.varsd_active; vartgt_spt_pct = u.vartgt_spt_pct;
            pfsp_active = u.pfsp_active; pfsp_cosphi = u.pfsp_cosphi; pfsp_generation = u.pfsp_generation;
        }
        if (u.aggregator_dirty) { wsa_active = u.wsa_active; wsa_spt_pct = u.wsa_spt_pct; }
        if (u.aggregator_reactive_dirty) { varsa_active = u.varsa_active; varsa_spt_pct = u.varsa_spt_pct; }
        valid = u.valid; dirty = u.dirty; reactive_dirty = u.reactive_dirty; pfsp_dirty = u.pfsp_dirty;
        aggregator_dirty = u.aggregator_dirty; aggregator_reactive_dirty = u.aggregator_reactive_dirty;
    }
};

/** @deprecated alias — prefer DsoLiveCommand */
using WlimLiveCommand = DsoLiveCommand;

/**
 * Phase 3 Eth_A MMS server (libiec61850).
 * Hierarchy: CCI016_01 / LD_Plant / {WlimDWMX1, WSdDAGC1, stubs…, PdCMMXU1}.
 */
class MmsAdapter {
public:
    MmsAdapter() = default;
    ~MmsAdapter();

    MmsAdapter(const MmsAdapter&) = delete;
    MmsAdapter& operator=(const MmsAdapter&) = delete;

    bool start(const core::MmsConfig& cfg, MmsAuditFn audit = {},
               MmsTimeSyncAuditFn time_sync_audit = {});
    void stop();
    bool is_running() const;

    void update_tot_w_kw(double p_kw);
    /** Publish inverter active power when GenPVMMXU1 is present in the SCL model. */
    void update_gen_pv_active_power_kw(double p_kw, core::DataQuality quality);
    /** P5-M07 / T.3.1.3 — reactive power at POC (kVAr). */
    void update_totvar_kvar(double q_kvar);
    /** Publish source-data quality for the CEI Aggregator P/Q measurements. */
    void update_aggregator_measurement_quality(core::DataQuality quality);
    /** P5-M08 / T.3.1.3 — phase-phase V at POC (kV); PPV.phsAB lab path. */
    void update_ppv_kv(double ppv_kv);
    /** EMT432 I1 (A) → PdCMMXU1.A.phsA.cVal.mag.f */
    void update_phs_a_a(double amps);
    /** PPV DEL + A WYE CMV magnitudes and angles (1P and 3P). */
    void update_pdc_phasors(const core::PdcMmxuPhasors& pdc);

    /**
     * If a new Wlim/WSd control arrived, copy snapshot and clear dirty.
     */
    bool poll_dso_live_command(DsoLiveCommand& out);

    /** @deprecated — same as poll_dso_live_command */
    bool poll_wlim_command(WlimLiveCommand& out) { return poll_dso_live_command(out); }

    bool poll_comms_loss_fallback();
    /** True after P3-05 latch until next MMS client connects (O.13.1.2 autonomous). */
    bool operating_rule_fallback_latched() const;
    void refresh_time_quality();
    int client_count() const;
    bool gnss_fix() const;

    /**
     * P5-G02 — enable libiec61850 integrated GOOSE publisher for GoCBs in CID/.cfg.
     * Requires GSEControl + GSE in SCL; call after start() when goose.publish_enabled.
     */
    void enable_goose_publishing(const core::GooseConfig& cfg);

private:
    struct Impl;
    Impl* impl_{nullptr};
};

}  // namespace cci::adapters
