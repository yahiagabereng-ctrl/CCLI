#include "goose_adapter.hpp"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)

extern "C" {
#include "goose_receiver.h"
#include "goose_subscriber.h"
}

namespace {

bool parse_mac(const std::string& text, uint8_t out[6]) {
    if (text.empty()) {
        return false;
    }
    unsigned a[6];
    if (std::sscanf(text.c_str(), "%x:%x:%x:%x:%x:%x", &a[0], &a[1], &a[2], &a[3], &a[4],
                    &a[5]) != 6) {
        return false;
    }
    for (int i = 0; i < 6; ++i) {
        out[i] = static_cast<uint8_t>(a[i]);
    }
    return true;
}

void goose_listener(GooseSubscriber subscriber, void* parameter) {
    auto* self = static_cast<cci::adapters::GooseAdapter*>(parameter);
    if (self != nullptr) {
        self->on_goose_received(subscriber);
    }
}

}  // namespace

/* Must be defined outside the anonymous namespace (member of cci::adapters::GooseAdapter). */
struct cci::adapters::GooseAdapter::Impl {
    GooseReceiver     receiver{nullptr};
    GooseSubscriber   subscriber{nullptr};
    std::vector<char> go_cb_ref_buf;
};

#endif  // CCLI_HAVE_LIBIEC61850 && CCLI_HAVE_GOOSE

namespace cci::adapters {

GooseAdapter::~GooseAdapter() { stop(); }

GooseRxSnapshot GooseAdapter::snapshot() const {
    GooseRxSnapshot snap;
#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
    if (impl_ == nullptr || impl_->subscriber == nullptr) {
        return snap;
    }
    snap.st_num = static_cast<uint32_t>(GooseSubscriber_getStNum(impl_->subscriber));
    snap.sq_num = static_cast<uint32_t>(GooseSubscriber_getSqNum(impl_->subscriber));
    snap.app_id = GooseSubscriber_getAppId(impl_->subscriber);
    snap.valid = GooseSubscriber_isValid(impl_->subscriber);
#else
    (void)snap;
#endif
    return snap;
}

void GooseAdapter::on_goose_received(void* subscriber) {
#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
    auto* sub = static_cast<GooseSubscriber>(subscriber);
    const uint32_t st = static_cast<uint32_t>(GooseSubscriber_getStNum(sub));
    const uint32_t sq = static_cast<uint32_t>(GooseSubscriber_getSqNum(sub));
    const int32_t app = GooseSubscriber_getAppId(sub);
    const bool ok = GooseSubscriber_isValid(sub);
    last_rx_ms_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count());
    std::cerr << "goose: rx appId=0x" << std::hex << app << std::dec << " stNum=" << st
              << " sqNum=" << sq << " valid=" << (ok ? "yes" : "no") << '\n';
    if (on_event_) {
        on_event_("goose",
                  "appId=0x" + std::to_string(app) + " stNum=" + std::to_string(st) +
                      " sqNum=" + std::to_string(sq) + " valid=" + (ok ? "1" : "0"));
    }
#else
    (void)subscriber;
#endif
}

bool GooseAdapter::start(const core::GooseConfig& cfg, EventCallback on_event) {
    stop();
    cfg_ = cfg;
    on_event_ = std::move(on_event);

    if (!cfg.enabled) {
        return false;
    }

#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
    if (cfg.subscribe_go_cb_ref.empty()) {
        std::cerr << "goose: subscribe_go_cb_ref required when goose.enabled=true\n";
        return false;
    }

    impl_ = new Impl();
    impl_->go_cb_ref_buf.assign(cfg.subscribe_go_cb_ref.begin(), cfg.subscribe_go_cb_ref.end());
    impl_->go_cb_ref_buf.push_back('\0');

    impl_->receiver = GooseReceiver_create();
    if (impl_->receiver == nullptr) {
        std::cerr << "goose: GooseReceiver_create failed\n";
        stop();
        return false;
    }
    GooseReceiver_setInterfaceId(impl_->receiver, cfg.interface.c_str());

    impl_->subscriber =
        GooseSubscriber_create(impl_->go_cb_ref_buf.data(), nullptr);
    if (impl_->subscriber == nullptr) {
        std::cerr << "goose: GooseSubscriber_create failed\n";
        stop();
        return false;
    }
    if (cfg.subscribe_app_id != 0) {
        GooseSubscriber_setAppId(impl_->subscriber, cfg.subscribe_app_id);
    }
    uint8_t mac[6];
    if (parse_mac(cfg.subscribe_dst_mac, mac)) {
        GooseSubscriber_setDstMac(impl_->subscriber, mac);
    }
    GooseSubscriber_setListener(impl_->subscriber, goose_listener, this);
    GooseReceiver_addSubscriber(impl_->receiver, impl_->subscriber);
    GooseReceiver_start(impl_->receiver);

    running_.store(true);
    std::cerr << "goose: listening on " << cfg.interface << " goCbRef="
              << cfg.subscribe_go_cb_ref << '\n';
    return true;
#else
    std::cerr << "goose: built without libiec61850 GOOSE support\n";
    (void)cfg;
    return false;
#endif
}

void GooseAdapter::stop() {
    running_.store(false);
#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
    if (impl_ == nullptr) {
        return;
    }
    if (impl_->receiver != nullptr) {
        GooseReceiver_stop(impl_->receiver);
    }
    if (impl_->receiver != nullptr) {
        GooseReceiver_destroy(impl_->receiver);
        impl_->receiver = nullptr;
    }
    impl_->subscriber = nullptr;
    delete impl_;
    impl_ = nullptr;
#endif
}

}  // namespace cci::adapters
