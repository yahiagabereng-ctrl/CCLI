#pragma once

#include "config/ccli_config.hpp"

#include <functional>
#include <string>

namespace cci::adapters {

/** O.14 audit hook — invoked from MMS connection / ACSE auth paths (P7-04). */
using MmsAuditFn = std::function<void(const std::string& detail)>;

/** Annex T / 62351-8 lab roles (private DSO_OPERATOR = −1). */
enum class MmsRole : int {
    Viewer = 0,
    DsoOperator = -1,
    Unknown = -99,
};

/**
 * Live DSO commands from Eth_A MMS.
 * O.9.2.2 Wlim · O.9.2.3 WSd · O.9.1.4 VArSd (P5-R01).
 */
struct DsoLiveCommand {
    bool valid{false};
    bool dirty{false};          /**< Active-power (Wlim/WSd) changed */
    bool reactive_dirty{false}; /**< Reactive (VArSd) changed — P5-R01 */
    bool wlim_active{false};    /**< WlimDWMX1.Mod 1=on, 5=off */
    double wmax_spt_pct{0.0};
    bool wsd_active{false}; /**< WSdDAGC1.Mod — Figura 2 s.p. W */
    double wspt_pct{0.0};   /**< WSdDAGC1.WSptPct % Smax */
    bool varsd_active{false};   /**< VArSdDVAR1.Mod — O.9.1.4 Q set-point */
    double vartgt_spt_pct{0.0}; /**< VArSdDVAR1.VArTgtSptPct % Smax (signed) */
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

    bool start(const core::MmsConfig& cfg, MmsAuditFn audit = {});
    void stop();
    bool is_running() const;

    void update_tot_w_kw(double p_kw);
    /** P5-M07 / T.3.1.3 — reactive power at POC (kVAr). */
    void update_totvar_kvar(double q_kvar);
    /** P5-M08 / T.3.1.3 — phase-phase V at POC (kV); PPV.phsAB lab path. */
    void update_ppv_kv(double ppv_kv);

    /**
     * If a new Wlim/WSd control arrived, copy snapshot and clear dirty.
     */
    bool poll_dso_live_command(DsoLiveCommand& out);

    /** @deprecated — same as poll_dso_live_command */
    bool poll_wlim_command(WlimLiveCommand& out) { return poll_dso_live_command(out); }

    bool poll_comms_loss_fallback();
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
