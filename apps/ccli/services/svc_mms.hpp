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
    void update_phs_a_a(double amps) { adapter_.update_phs_a_a(amps); }
    void update_pdc_phasors(const core::PdcMmxuPhasors& pdc) {
        adapter_.update_pdc_phasors(pdc);
    }
    bool poll_dso_live_command(adapters::DsoLiveCommand& out) {
        return adapter_.poll_dso_live_command(out);
    }
    bool poll_wlim_command(adapters::WlimLiveCommand& out) {
        return poll_dso_live_command(out);
    }
    bool poll_comms_loss_fallback() { return adapter_.poll_comms_loss_fallback(); }
    bool operating_rule_fallback_latched() const {
        return adapter_.operating_rule_fallback_latched();
    }
    void refresh_time_quality() { adapter_.refresh_time_quality(); }
    bool gnss_fix() const { return adapter_.gnss_fix(); }
    int client_count() const { return adapter_.client_count(); }
    void enable_goose_publishing(const core::GooseConfig& cfg) {
        adapter_.enable_goose_publishing(cfg);
    }

private:
    adapters::MmsAdapter adapter_;
};

}  // namespace cci::services
