#pragma once

#include "config/ccli_config.hpp"
#include "iec104_adapter.hpp"

namespace cci::services {

class Iec104Service {
public:
    bool start(const core::Iec104Config& cfg) { return adapter_.start(cfg); }
    void stop() { adapter_.stop(); }
    void tick() { adapter_.tick(); }
    bool is_running() const { return adapter_.is_running(); }
    void update_tot_w_kw(double p_kw) { adapter_.update_tot_w_kw(p_kw); }
    bool poll_comms_loss_fallback() { return adapter_.poll_comms_loss_fallback(); }
    bool poll_operator_audit(adapters::Iec104Adapter::OperatorAuditEvent& out) {
        return adapter_.poll_operator_audit(out);
    }
    int client_count() const { return adapter_.client_count(); }

private:
    adapters::Iec104Adapter adapter_;
};

}  // namespace cci::services
