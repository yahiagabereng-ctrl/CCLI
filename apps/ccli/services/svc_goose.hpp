#pragma once

#include "config/ccli_config.hpp"
#include "goose_adapter.hpp"

#include <functional>
#include <string>

namespace cci::services {

class GooseService {
public:
    bool start(const core::GooseConfig& cfg,
               std::function<void(const std::string&, const std::string&)> on_event = {}) {
        return adapter_.start(cfg, std::move(on_event));
    }
    void stop() { adapter_.stop(); }
    bool is_running() const { return adapter_.is_running(); }
    adapters::GooseRxSnapshot snapshot() const { return adapter_.snapshot(); }
    std::int64_t last_rx_ms() const { return adapter_.last_rx_ms(); }

private:
    adapters::GooseAdapter adapter_;
};

}  // namespace cci::services
