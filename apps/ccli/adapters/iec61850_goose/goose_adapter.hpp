#pragma once

#include "config/ccli_config.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>

namespace cci::adapters {

/** Last received GOOSE metadata for lab / O.14 logging. */
struct GooseRxSnapshot {
    uint32_t st_num{0};
    uint32_t sq_num{0};
    int32_t  app_id{0};
    bool     valid{false};
};

class GooseAdapter {
public:
    using EventCallback = std::function<void(const std::string& category, const std::string& detail)>;

    GooseAdapter() = default;
    ~GooseAdapter();

    GooseAdapter(const GooseAdapter&) = delete;
    GooseAdapter& operator=(const GooseAdapter&) = delete;

    bool start(const core::GooseConfig& cfg, EventCallback on_event = {});
    void stop();
    bool is_running() const { return running_.load(); }
    GooseRxSnapshot snapshot() const;

    /** Called from libiec61850 GooseListener (C callback). */
    void on_goose_received(void* subscriber);

private:

    core::GooseConfig cfg_{};
    EventCallback     on_event_{};
    std::atomic<bool> running_{false};

#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
    struct Impl;
    Impl* impl_{nullptr};
#endif
};

}  // namespace cci::adapters
