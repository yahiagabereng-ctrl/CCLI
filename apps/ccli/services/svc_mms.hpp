#pragma once

#include "config/ccli_config.hpp"
#include "mms_adapter.hpp"

namespace cci::services {

class MmsService {
public:
    bool start(const core::MmsConfig& cfg, adapters::MmsAuditFn audit = {}) {
        return adapter_.start(cfg, std::move(audit));
    }
    void stop() { adapter_.stop(); }
    bool is_running() const { return adapter_.is_running(); }
    void update_tot_w_kw(double p_kw) { adapter_.update_tot_w_kw(p_kw); }
    void update_totvar_kvar(double q_kvar) { adapter_.update_totvar_kvar(q_kvar); }
    void update_ppv_kv(double ppv_kv) { adapter_.update_ppv_kv(ppv_kv); }
    bool poll_dso_live_command(adapters::DsoLiveCommand& out) {
        return adapter_.poll_dso_live_command(out);
    }
    bool poll_wlim_command(adapters::WlimLiveCommand& out) {
        return poll_dso_live_command(out);
    }
    bool poll_comms_loss_fallback() { return adapter_.poll_comms_loss_fallback(); }
    void refresh_time_quality() { adapter_.refresh_time_quality(); }
    bool gnss_fix() const { return adapter_.gnss_fix(); }
    void enable_goose_publishing(const core::GooseConfig& cfg) {
        adapter_.enable_goose_publishing(cfg);
    }

private:
    adapters::MmsAdapter adapter_;
};

}  // namespace cci::services
