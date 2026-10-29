/* Independent CCLI watchdog for OpenWrt/procd on TesPro TG544.
 *
 * CCI writes /var/run/ccli-health after completed control-loop iterations.
 * This process uses CLOCK_MONOTONIC to detect a stale record, requests CCI's
 * SIGABRT safe-state path, then uses SIGKILL if it remains frozen so procd can
 * respawn it. Hardware watchdog ownership intentionally remains with platform
 * policy until the TG544 driver's nowayout/reset/safe-DO behaviour is proven.
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_HEALTH_FILE "/var/run/ccli-health"

static volatile sig_atomic_t running = 1;

static void stop_handler(int unused) {
    (void)unused;
    running = 0;
}

static long long monotonic_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return -1;
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

static int parse_long_field(const char* text, const char* key, long* value) {
    const size_t key_len = strlen(key);
    const char* p = text;
    while (p && *p) {
        if (strncmp(p, key, key_len) == 0) {
            char* end = NULL;
            errno = 0;
            const long parsed = strtol(p + key_len, &end, 10);
            if (errno == 0 && end != p + key_len) {
                *value = parsed;
                return 0;
            }
            return -1;
        }
        p = strchr(p, '\n');
        if (p)
            ++p;
    }
    return -1;
}

static int read_health(const char* path, long* pid, long long* heartbeat_ms) {
    char buf[256];
    const int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return -1;
    const ssize_t size = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    if (size <= 0)
        return -1;
    buf[size] = '\0';

    long parsed_ms = 0;
    if (parse_long_field(buf, "pid=", pid) != 0 ||
        parse_long_field(buf, "monotonic_ms=", &parsed_ms) != 0 || *pid <= 1 ||
        parsed_ms < 0)
        return -1;
    *heartbeat_ms = parsed_ms;
    return 0;
}

static int is_ccli_process(long pid) {
    char path[64];
    char name[32];
    snprintf(path, sizeof(path), "/proc/%ld/comm", pid);
    const int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return 0;
    const ssize_t size = read(fd, name, sizeof(name) - 1);
    close(fd);
    if (size <= 0)
        return 0;
    name[size] = '\0';
    return strcmp(name, "ccli\n") == 0 || strcmp(name, "ccli") == 0;
}

static void sleep_ms(unsigned int ms) {
    struct timespec delay;
    delay.tv_sec = ms / 1000U;
    delay.tv_nsec = (long)(ms % 1000U) * 1000000L;
    while (running && nanosleep(&delay, &delay) != 0 && errno == EINTR) {
    }
}

static void recover_stalled_ccli(long pid, unsigned int grace_s) {
    if (!is_ccli_process(pid)) {
        syslog(LOG_WARNING, "ccli watchdog: stale health PID %ld is not ccli; ignoring", pid);
        return;
    }

    syslog(LOG_ERR, "ccli watchdog: health stale; requesting safe abort for PID %ld", pid);
    (void)kill((pid_t)pid, SIGABRT);
    for (unsigned int waited = 0; running && waited < grace_s * 10U; ++waited) {
        sleep_ms(100);
        if (kill((pid_t)pid, 0) != 0 && errno == ESRCH)
            return;
    }
    if (is_ccli_process(pid)) {
        syslog(LOG_CRIT, "ccli watchdog: PID %ld did not exit; sending SIGKILL", pid);
        (void)kill((pid_t)pid, SIGKILL);
    }
}

static int parse_positive(const char* value, unsigned int* result) {
    char* end = NULL;
    errno = 0;
    const unsigned long parsed = strtoul(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed == 0 || parsed > 3600U)
        return -1;
    *result = (unsigned int)parsed;
    return 0;
}

int main(int argc, char** argv) {
    const char* health_file = DEFAULT_HEALTH_FILE;
    unsigned int stale_timeout_s = 8;
    unsigned int check_interval_s = 1;
    unsigned int kill_grace_s = 2;
    unsigned int startup_grace_s = 30;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--health-file") == 0 && i + 1 < argc) {
            health_file = argv[++i];
        } else if (strcmp(argv[i], "--stale-timeout") == 0 && i + 1 < argc) {
            if (parse_positive(argv[++i], &stale_timeout_s) != 0) return 2;
        } else if (strcmp(argv[i], "--check-interval") == 0 && i + 1 < argc) {
            if (parse_positive(argv[++i], &check_interval_s) != 0) return 2;
        } else if (strcmp(argv[i], "--kill-grace") == 0 && i + 1 < argc) {
            if (parse_positive(argv[++i], &kill_grace_s) != 0) return 2;
        } else if (strcmp(argv[i], "--startup-grace") == 0 && i + 1 < argc) {
            if (parse_positive(argv[++i], &startup_grace_s) != 0) return 2;
        } else {
            fprintf(stderr, "usage: %s [--health-file PATH] [--stale-timeout S] "
                            "[--check-interval S] [--kill-grace S] [--startup-grace S]\n", argv[0]);
            return 2;
        }
    }
    if (stale_timeout_s <= check_interval_s)
        return 2;

    openlog("ccli-watchdog", LOG_PID, LOG_DAEMON);
    signal(SIGTERM, stop_handler);
    signal(SIGINT, stop_handler);
    long long startup_deadline = monotonic_ms() + (long long)startup_grace_s * 1000LL;
    long acted_pid = -1;
    syslog(LOG_INFO, "started: stale=%us check=%us grace=%us", stale_timeout_s,
           check_interval_s, kill_grace_s);

    while (running) {
        long pid = -1;
        long long heartbeat_ms = -1;
        const long long now_ms = monotonic_ms();
        if (read_health(health_file, &pid, &heartbeat_ms) == 0) {
            const long long age_ms = now_ms - heartbeat_ms;
            if (age_ms >= 0 && age_ms > (long long)stale_timeout_s * 1000LL) {
                if (pid != acted_pid) {
                    recover_stalled_ccli(pid, kill_grace_s);
                    acted_pid = pid;
                }
            } else {
                acted_pid = -1;
            }
        } else if (now_ms >= startup_deadline) {
            /* CCLI may intentionally be stopped; do not restart without a PID record. */
            syslog(LOG_WARNING, "ccli watchdog: health record unavailable");
            startup_deadline = now_ms + 60000LL;
        }
        sleep_ms(check_interval_s * 1000U);
    }
    syslog(LOG_INFO, "stopped");
    closelog();
    return 0;
}
