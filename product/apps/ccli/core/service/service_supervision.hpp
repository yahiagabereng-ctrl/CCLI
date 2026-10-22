#pragma once

#include <atomic>
#include <cstdint>
#include <string>

namespace cci::core {

/** Result of startup checks for crash / unclean restart (O.14 + 62443 SR 6.2). */
struct CrashRecoveryInfo {
    bool unclean_restart{false};
    bool crash_recovered{false};
    int  crash_signum{0};
};

/**
 * Process supervision: fatal-signal safe DO, crash tombstone, procd watchdog ping.
 * IEC 62443 SR 6.2 (service health) + CR 3.6 (safe outputs on fault).
 */
class ServiceSupervision {
public:
    static constexpr int kDefaultWatchdogPingS = 30;

    using SafeStateHook = void (*)();

    /** Register DO safe-state hook (call after svc_io_init). */
    static void set_safe_state_hook(SafeStateHook hook);

    /** Install SIGTERM/SIGINT + fatal crash handlers. */
    static void install_handlers(std::atomic<bool>* running_flag);

    /** Call after IO init, before main loop — reads tombstone / stale pid marker. */
    static CrashRecoveryInfo startup_check();

    /** Mark daemon running (removed on clean shutdown). */
    static void mark_running();

    /** Remove running marker and tombstone after graceful exit. */
    static void mark_clean_shutdown();

    /**
     * Ping procd watchdog (main loop only — uses ubus when present).
     * No-op on hosts without /sbin/ubus.
     */
    static void procd_watchdog_ping();

    static std::string signum_name(int signum);
};

}  // namespace cci::core
