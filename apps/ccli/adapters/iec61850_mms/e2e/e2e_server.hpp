#pragma once

#include "e2e_osi_bridge.h"
#include "e2e_crypto.hpp"
#include <functional>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace ccli::mms::e2e {
struct NativeServerFiles {
    bool allow_compatibility{false};
    std::string certificate, private_key, trust_anchors, crls, transport_certificate;
    std::vector<std::string> certificate_path;
    std::string ap_title;
    unsigned ae_qualifier{0};
    SignatureScheme signature_scheme{SignatureScheme::rsa_pkcs1_v15_sha256};
    unsigned rekey_seconds{900};
    std::size_t maximum_pdu{64000};
    std::size_t maximum_receive_pairs{65536};
};

class NativeServer {
public:
    // Called only after the native signature, certificate/path, identity,
    // timestamp and negotiation checks succeed. Authorization returns a token
    // owned by the application's normal connection lifecycle.
    using Authorize = std::function<bool(ByteView certificate, ByteView tls_certificate,
        const std::string& peer, void** token)>;
    using Release = std::function<void(void* token)>;
    using Audit = std::function<void(const std::string&)>;
    using MonotonicNow = std::function<std::chrono::steady_clock::time_point()>;
    // Dependency injection for deterministic epoch tests; no configuration or
    // wire option can replace the production steady clock.
    explicit NativeServer(MonotonicNow monotonic_now = {});
    ~NativeServer();
    NativeServer(const NativeServer&) = delete;
    NativeServer& operator=(const NativeServer&) = delete;
    bool configure(const NativeServerFiles& files, Authorize authorize,
                   Release release, Audit audit, std::string& error);
    // One adapter is registered at a time; ownership is matched to the stack's
    // authenticator parameter so other server instances cannot use its profile.
    bool install(void* authenticator_owner);
    void uninstall();
    int process(void** session, int event, const CcliE2eIo& io,
        ByteView input, std::vector<std::uint8_t>& output, CcliE2eReply& reply);
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    void* owner_{nullptr};
    static int handler(void*, void**, int, const CcliE2eIo*, const uint8_t*, int,
                       uint8_t*, int, CcliE2eReply*);
};
} // namespace ccli::mms::e2e
