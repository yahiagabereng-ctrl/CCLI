#include "tls_socket.h"
#include "hal_thread.h"
#include "../../adapters/security/iec62351_tls.h"
#include "iec104_tls.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef CCLI_TLS_TEST_CLOCK
#include <csignal>
#include <time.h>
static volatile std::sig_atomic_t advance_seconds = 0;
static void advance(int signal) { advance_seconds = advance_seconds + (signal == SIGUSR1 ? 601 : 31); }
extern "C" int __real_clock_gettime(clockid_t, struct timespec*);
extern "C" int __wrap_clock_gettime(clockid_t clock, struct timespec* now) {
    int result = __real_clock_gettime(clock, now);
    if (!result && clock == CLOCK_MONOTONIC) now->tv_sec += advance_seconds;
    return result;
}
#endif

static void event(void*, TLSEventLevel level, int code, const char* message, TLSConnection) {
    std::printf("EVENT %d %d %s\n", level, code, message ? message : "");
    std::fflush(stdout);
}

// Host-only loopback TLS echo server. Uses freshly generated test credentials.
int main(int argc, char** argv) {
    if (argc < 8 || argc > 11) return 2;
#ifdef CCLI_TLS_TEST_CLOCK
    std::signal(SIGUSR1, advance); std::signal(SIGUSR2, advance);
#endif
    TLSConfiguration cfg = nullptr;
    cci::adapters::Iec104TlsEvents events{
        [](void*, int level, int code, const char* message) {
            event(nullptr, static_cast<TLSEventLevel>(level), code, message, nullptr);
        }, nullptr};
    bool valid = false;
    if (argc >= 9 && std::strcmp(argv[8], "iec104") == 0) {
        cci::core::Iec104TlsConfig options;
        options.own_key = argv[2]; options.own_cert = argv[3]; options.ca_cert = argv[4];
        if (std::strcmp(argv[5], "-")) options.crl_path = argv[5];
        if (std::strcmp(argv[6], "-")) options.client_cert = argv[6];
        auto is_hash_policy = [](const char* value) {
            return std::strcmp(value, "sha384") == 0 || std::strcmp(value, "sha512") == 0;
        };
        if (argc >= 10 && !is_hash_policy(argv[9]))
            options.allowed_client_certs.emplace_back(argv[9]);
        for (int i = 9; i < argc; ++i) {
            if (std::strcmp(argv[i], "sha384") == 0) options.allow_sha384 = true;
            if (std::strcmp(argv[i], "sha512") == 0) options.allow_sha512 = true;
        }
        options.revocation_check_s = 1; options.renegotiation_s = std::atoi(argv[7]);
        std::string error;
        void* opaque = nullptr;
        valid = cci::adapters::iec104_create_tls_config(options, &opaque, error, &events);
        cfg = static_cast<TLSConfiguration>(opaque);
    } else {
        cfg = TLSConfiguration_create();
        TLSConfiguration_setEventHandler(cfg, event, nullptr);
        bool loaded = TLSConfiguration_setOwnKeyFromFile(cfg, argv[2], nullptr) &&
        TLSConfiguration_setOwnCertificateFromFile(cfg, argv[3]) &&
        TLSConfiguration_addCACertificateFromFile(cfg, argv[4]);
    if (std::strcmp(argv[5], "-")) loaded = loaded && TLSConfiguration_addCRLFromFile(cfg, argv[5]);
    if (std::strcmp(argv[6], "-")) {
        loaded = loaded && TLSConfiguration_addAllowedCertificateFromFile(cfg, argv[6]);
        TLSConfiguration_setAllowOnlyKnownCertificates(cfg, true);
    }
        const bool allow_sha384 = argc >= 9 && std::strcmp(argv[8], "sha384") == 0;
        const bool allow_sha512 = argc >= 9 && std::strcmp(argv[8], "sha512") == 0;
        valid = loaded && CCLI_TLS_setOptionalHashes(cfg, allow_sha384, allow_sha512) &&
                CCLI_TLS_configure(cfg, std::strcmp(argv[5], "-") ? argv[5] : "",
                                             1, std::atoi(argv[7]), 3600);
    }
    if (!valid) { if (cfg) TLSConfiguration_destroy(cfg); std::puts("REJECTED"); return 3; }
#ifdef CCLI_TLS_DEVICE_TEST
    // Explicit test build only; the installed application never uses this entry point.
    const char* bind = std::getenv("CCLI_TLS_TEST_BIND");
    if (!bind || std::strcmp(bind, "192.168.0.1")) return 6;
    if (std::atoi(argv[1]) != 3793) return 6;
#else
    const char* bind = "127.0.0.1";
#endif
    ServerSocket server = TcpServerSocket_create(bind, std::atoi(argv[1]));
    if (!server) return 4;
    ServerSocket_listen(server);
    std::puts("READY"); std::fflush(stdout);
    Socket socket = nullptr;
    for (int i = 0; i < 500 && !socket; ++i) { socket = ServerSocket_accept(server); Thread_sleep(10); }
    if (!socket) return 5;
    TLSSocket tls = TLSSocket_create(socket, cfg, true);
    if (tls) {
        uint8_t buffer[1024];
        for (int i = 0; i < 1500; ++i) {
            int n = TLSSocket_read(tls, buffer, sizeof(buffer));
            if (n < 0) break;
            if (n > 0 && TLSSocket_write(tls, buffer, n) < 0) break;
            Thread_sleep(10);
        }
        TLSSocket_close(tls);
    }
    Socket_destroy(socket);
    ServerSocket_destroy(server);
    TLSConfiguration_destroy(cfg);
    return 0;
}
