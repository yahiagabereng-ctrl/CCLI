#include "event/syslog_sender.hpp"
#include "event/event_store.hpp"
#include "event/iec62351_event.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>

#if defined(CCLI_HAVE_SYSLOG_TLS)
#include "iec62351_tls.h"
#include "mbedtls/sha256.h"
#include <arpa/inet.h>
#endif

namespace cci::core {
namespace {
bool fingerprint(const std::string& text, std::array<unsigned char, 32>& bytes) {
    // RFC 5425 uses algorithm name and colon-separated uppercase hex. Also
    // accept the common bare SHA-256 hex form used by commissioning tools.
    auto input = text;
    if (input.rfind("SHA-256:", 0) == 0) input.erase(0, 8);
    bool colon = input.size() == 95;
    if (!colon && input.size() != 64) return false;
    auto hex = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        const auto p = i * (colon ? 3 : 2);
        const int a = hex(input[p]), b = hex(input[p + 1]);
        if (a < 0 || b < 0 || (colon && i < 31 && input[p + 2] != ':')) return false;
        bytes[i] = static_cast<unsigned char>((a << 4) | b);
    }
    return true;
}
} // namespace

struct SyslogSender::Impl {
    EventLogConfig cfg;
    Failure failure;
    std::array<unsigned char, 32> pin{};
    std::mutex mutex;
    std::condition_variable ready;
    std::deque<std::string> pending;
    std::size_t queued_bytes{0};
    bool stopping{false};
    std::thread worker;
#if defined(CCLI_HAVE_SYSLOG_TLS)
    TLSConfiguration tls_config{nullptr};
    TLSSocket tls{nullptr};
    Socket socket{nullptr};
    std::string tls_error;

    static void tls_event(void* context, TLSEventLevel level, int,
                          const char* message, TLSConnection) {
        auto* self = static_cast<Impl*>(context);
        if (level != TLS_SEC_EVT_INFO) self->tls_error = message ? message : "TLS policy failure";
    }
    void close() {
        if (tls) { TLSSocket_close(tls); tls = nullptr; }
        if (socket) { Socket_destroy(socket); socket = nullptr; }
        if (tls_config) { TLSConfiguration_destroy(tls_config); tls_config = nullptr; }
    }
    bool connect() {
        close();
        tls_error.clear();
        tls_config = TLSConfiguration_create();
        if (!tls_config) return false;
        TLSConfiguration_setClientMode(tls_config);
        TLSConfiguration_setEventHandler(tls_config, tls_event, this);
        TLSConfiguration_setMinTlsVersion(tls_config, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setMaxTlsVersion(tls_config, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setChainValidation(tls_config, true);
        TLSConfiguration_setAllowOnlyKnownCertificates(tls_config, false);
        if (!TLSConfiguration_setOwnKeyFromFile(tls_config, cfg.syslog_tls_key.c_str(), nullptr) ||
            !TLSConfiguration_setOwnCertificateFromFile(tls_config, cfg.syslog_tls_cert.c_str()) ||
            !TLSConfiguration_addCACertificateFromFile(tls_config, cfg.syslog_tls_ca.c_str()) ||
            !TLSConfiguration_addCRLFromFile(tls_config, cfg.syslog_tls_crl.c_str()) ||
            !CCLI_TLS_configure(tls_config, cfg.syslog_tls_crl.c_str(), cfg.syslog_revocation_check_s,
                                cfg.syslog_renegotiation_s, cfg.syslog_resumption_s)) return false;
        CCLI_TLS_setIoTimeout(tls_config, 2000);
        socket = TcpSocket_create();
        if (!socket) return false;
        Socket_setConnectTimeout(socket, 2000);
        if (!Socket_connect(socket, cfg.syslog_host.c_str(), cfg.syslog_port)) return false;
        tls = TLSSocket_create(socket, tls_config, true);
        if (!tls) return false;
        return authorized_peer();
    }
    bool authorized_peer() {
        int size = 0;
        const auto* cert = TLSSocket_getPeerCertificate(tls, &size);
        std::array<unsigned char, 32> actual{};
        if (!cert || size <= 0 || mbedtls_sha256(cert, static_cast<std::size_t>(size), actual.data(), 0) != 0 || actual != pin) {
            tls_error = "collector leaf SHA-256 fingerprint does not match authorized identity";
            return false;
        }
        return true;
    }
    bool healthy() {
        if (!tls || !CCLI_TLS_tick(tls)) return false;
        unsigned char ignored[64];
        // Drive alerts and peer-initiated TLS 1.2 renegotiation while idle.
        return TLSSocket_read(tls, ignored, sizeof(ignored)) >= 0 && authorized_peer();
    }
    bool send(const std::string& message) {
        auto frame = frame_rfc5425(message);
        return tls && TLSSocket_write(tls, reinterpret_cast<uint8_t*>(frame.data()),
                                      static_cast<int>(frame.size())) == static_cast<int>(frame.size());
    }
    void run() {
        for (;;) {
            std::string message;
            {
                std::unique_lock<std::mutex> lock(mutex);
                ready.wait_for(lock, std::chrono::milliseconds(100), [&] { return stopping || !pending.empty(); });
                if (stopping) break;
                if (!pending.empty()) message = pending.front();
            }
            if (message.empty()) {
                if (tls && !healthy()) { close(); failure("TLS syslog session lost"); }
                continue;
            }
            if ((!tls && !connect()) || !healthy() || !send(message)) {
                const auto reason = tls_error.empty() ? "collector TLS connection/send failed" : tls_error;
                close();
                failure(reason);
                std::unique_lock<std::mutex> lock(mutex);
                ready.wait_for(lock, std::chrono::seconds(1), [&] { return stopping; });
                if (stopping) break;
                continue;
            }
            std::lock_guard<std::mutex> lock(mutex);
            queued_bytes -= pending.front().size();
            pending.pop_front();
        }
        close();
    }
#endif

    Impl(const EventLogConfig& config, Failure handler) : cfg(config), failure(std::move(handler)) {
        if (!SyslogSender::available()) throw std::invalid_argument("IEC 62351-14 syslog requires the reviewed TLS build");
        EventRecord probe{0, "TLS_HS_SUCCESS", "", ""};
        if (!prepare_iec_event(probe, cfg.syslog_source, cfg.syslog_app_name) ||
            cfg.syslog_port < 1 || cfg.syslog_port > 65535 || cfg.syslog_tls_ca.empty() ||
            cfg.syslog_tls_cert.empty() || cfg.syslog_tls_key.empty() || cfg.syslog_tls_crl.empty() ||
            !fingerprint(cfg.syslog_tls_peer_sha256, pin) || cfg.syslog_revocation_check_s < 1 ||
            cfg.syslog_revocation_check_s > 86400 || cfg.syslog_renegotiation_s < 600 || cfg.syslog_renegotiation_s > 86400 ||
            cfg.syslog_resumption_s < 1 || cfg.syslog_resumption_s > 7200)
            throw std::invalid_argument("IEC 62351-14 syslog requires source/app identity, valid timers, CA, client certificate/key, CRL and collector SHA-256 pin");
#if defined(CCLI_HAVE_SYSLOG_TLS)
        in_addr address{};
        if (inet_pton(AF_INET, cfg.syslog_host.c_str(), &address) != 1)
            throw std::invalid_argument("TLS syslog collector must be a numeric IPv4 address");
        worker = std::thread([this] { run(); });
#endif
    }
    ~Impl() {
        { std::lock_guard<std::mutex> lock(mutex); stopping = true; }
        ready.notify_all();
        if (worker.joinable()) worker.join();
    }
};

SyslogSender::SyslogSender(const EventLogConfig& cfg, Failure failure)
    : impl_(std::make_unique<Impl>(cfg, std::move(failure))) {}
SyslogSender::~SyslogSender() = default;
bool SyslogSender::submit(std::string message) {
    if (message.empty() || message.size() > 8192) return false;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->stopping || impl_->pending.size() >= 2048 || impl_->queued_bytes + message.size() > 2 * 1024 * 1024) return false;
        impl_->queued_bytes += message.size();
        impl_->pending.push_back(std::move(message));
    }
    impl_->ready.notify_one();
    return true;
}
bool SyslogSender::available() {
#if defined(CCLI_HAVE_SYSLOG_TLS)
    return true;
#else
    return false;
#endif
}
} // namespace cci::core
