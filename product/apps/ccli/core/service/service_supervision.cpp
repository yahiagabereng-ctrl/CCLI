#include "service/service_supervision.hpp"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <string>

namespace cci::core {

namespace {

constexpr const char* kCrashTombstonePath = "/var/lib/ccli/crash.tombstone";
constexpr const char* kRunningMarkerPath = "/var/run/ccli-running";

std::atomic<bool>* g_running_flag = nullptr;
ServiceSupervision::SafeStateHook g_safe_state_hook = nullptr;

void write_tombstone_async_safe(int signum) {
    const int fd = open(kCrashTombstonePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return;
    }
    char buf[64];
    const int len = snprintf(buf, sizeof(buf), "signum=%d\npid=%d\n", signum,
                             static_cast<int>(getpid()));
    if (len > 0) {
        (void)write(fd, buf, static_cast<size_t>(len));
    }
    close(fd);
}

void on_stop_signal(int /*signum*/) {
    if (g_running_flag != nullptr) {
        g_running_flag->store(false);
    }
}

void on_fatal_signal(int signum) {
    write_tombstone_async_safe(signum);
    /* Best-effort DO safe state (ubus path may be unsafe in-handler; crash is rare). */
    if (g_safe_state_hook != nullptr) {
        g_safe_state_hook();
    }
    _exit(128 + signum);
}

bool parse_tombstone_signum(const std::string& text, int* signum_out) {
    if (signum_out == nullptr) {
        return false;
    }
    const std::string key = "signum=";
    const auto pos = text.find(key);
    if (pos == std::string::npos) {
        return false;
    }
    const char* start = text.c_str() + pos + key.size();
    char* end = nullptr;
    const long val = strtol(start, &end, 10);
    if (end == start || val <= 0 || val > 127) {
        return false;
    }
    *signum_out = static_cast<int>(val);
    return true;
}

bool file_exists(const char* path) {
    return access(path, F_OK) == 0;
}

void remove_file_if_exists(const char* path) {
    if (file_exists(path)) {
        (void)unlink(path);
    }
}

}  // namespace

void ServiceSupervision::set_safe_state_hook(SafeStateHook hook) {
    g_safe_state_hook = hook;
}

void ServiceSupervision::install_handlers(std::atomic<bool>* running_flag) {
    g_running_flag = running_flag;

    struct sigaction sa_stop {};
    sa_stop.sa_handler = on_stop_signal;
    sigemptyset(&sa_stop.sa_mask);
    sa_stop.sa_flags = 0;
    sigaction(SIGTERM, &sa_stop, nullptr);
    sigaction(SIGINT, &sa_stop, nullptr);

    struct sigaction sa_fatal {};
    sa_fatal.sa_handler = on_fatal_signal;
    sigemptyset(&sa_fatal.sa_mask);
    sa_fatal.sa_flags = SA_RESETHAND;
    const int fatal_signals[] = {SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL};
    for (const int sig : fatal_signals) {
        sigaction(sig, &sa_fatal, nullptr);
    }
}

CrashRecoveryInfo ServiceSupervision::startup_check() {
    CrashRecoveryInfo info{};

    if (file_exists(kCrashTombstonePath)) {
        std::ifstream in(kCrashTombstonePath);
        std::ostringstream buf;
        buf << in.rdbuf();
        int signum = 0;
        if (parse_tombstone_signum(buf.str(), &signum)) {
            info.crash_recovered = true;
            info.crash_signum = signum;
        }
        remove_file_if_exists(kCrashTombstonePath);
    }

    if (file_exists(kRunningMarkerPath)) {
        info.unclean_restart = true;
    }

    return info;
}

void ServiceSupervision::mark_running() {
    const int fd = open(kRunningMarkerPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return;
    }
    char buf[32];
    const int len = snprintf(buf, sizeof(buf), "pid=%d\n", static_cast<int>(getpid()));
    if (len > 0) {
        (void)write(fd, buf, static_cast<size_t>(len));
    }
    close(fd);
}

void ServiceSupervision::mark_clean_shutdown() {
    remove_file_if_exists(kRunningMarkerPath);
    remove_file_if_exists(kCrashTombstonePath);
}

void ServiceSupervision::procd_watchdog_ping() {
    (void)system("ubus -t 1 call service event '{\"type\":\"watchdog\"}' "
                 ">/dev/null 2>&1");
}

std::string ServiceSupervision::signum_name(int signum) {
    switch (signum) {
        case SIGSEGV:
            return "SIGSEGV";
        case SIGABRT:
            return "SIGABRT";
        case SIGBUS:
            return "SIGBUS";
        case SIGFPE:
            return "SIGFPE";
        case SIGILL:
            return "SIGILL";
        case SIGTERM:
            return "SIGTERM";
        case SIGINT:
            return "SIGINT";
        default:
            return "SIG" + std::to_string(signum);
    }
}

}  // namespace cci::core
