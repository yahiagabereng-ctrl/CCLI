/* Feed chrony SOCK refclock from TesPro ubus GNSS (Quectel NMEA).
 * Clause: Annex T T.3.3.4.5 / REQ-TIME-002 — OS discipline via chrony, not ccli STEP.
 *
 * chrony.conf:
 *   refclock SOCK /var/run/chrony.gnss.sock poll 4 filter 2
 */
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define SOCK_MAGIC 0x534f434b
#define DEFAULT_SOCK "/var/run/chrony.gnss.sock"
#define DEFAULT_PERIOD_S 16

struct sock_sample {
    struct timeval tv;
    double offset;
    int pulse;
    int leap;
    int _pad;
    int magic;
};

static long long realtime_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000L;
}

static int hhmmss_ms(const char* t, int* hh, int* mm, int* ss, int* ms) {
    if (!t || strlen(t) < 6) return 0;
    *hh = (t[0] - '0') * 10 + (t[1] - '0');
    *mm = (t[2] - '0') * 10 + (t[3] - '0');
    *ss = (t[4] - '0') * 10 + (t[5] - '0');
    *ms = 0;
    if (t[6] == '.' && t[7]) {
        *ms = (t[7] - '0') * 100;
        if (t[8]) *ms += (t[8] - '0') * 10;
        if (t[9]) *ms += (t[9] - '0');
    }
    return 1;
}

static int ymd_to_epoch_ms(int yy, int mo, int dd, int hh, int mm, int ss, int ms,
                           long long* out) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_year = 2000 + yy - 1900;
    tm.tm_mon = mo - 1;
    tm.tm_mday = dd;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = ss;
    time_t sec = timegm(&tm);
    if (sec == (time_t)-1 || sec < 1577836800 || sec > 2208988800) return 0;
    *out = (long long)sec * 1000LL + ms;
    return 1;
}

static int parse_rmc(const char* raw, long long* epoch_ms, int* yy, int* mo, int* dd) {
    const char* p = strstr(raw, "$GNRMC,");
    if (!p) p = strstr(raw, "$GPRMC,");
    if (!p) return 0;
    p = strchr(p, ',');
    if (!p) return 0;
    ++p;
    char tbuf[16];
    size_t i = 0;
    while (*p && *p != ',' && i + 1 < sizeof(tbuf)) tbuf[i++] = *p++;
    tbuf[i] = 0;
    if (*p != ',') return 0;
    ++p;
    if (*p != 'A') return 0;
    int commas = 0;
    while (*p && commas < 7) {
        if (*p == ',') ++commas;
        ++p;
    }
    if (commas < 7 || strlen(p) < 6) return 0;
    *dd = (p[0] - '0') * 10 + (p[1] - '0');
    *mo = (p[2] - '0') * 10 + (p[3] - '0');
    *yy = (p[4] - '0') * 10 + (p[5] - '0');
    int hh, mm, ss, ms;
    if (!hhmmss_ms(tbuf, &hh, &mm, &ss, &ms)) return 0;
    return ymd_to_epoch_ms(*yy, *mo, *dd, hh, mm, ss, ms, epoch_ms);
}

static int parse_gga(const char* raw, int yy, int mo, int dd, long long* epoch_ms) {
    const char* p = strstr(raw, "$GNGGA,");
    if (!p) p = strstr(raw, "$GPGGA,");
    if (!p) return 0;
    p = strchr(p, ',');
    if (!p) return 0;
    ++p;
    char tbuf[16];
    size_t i = 0;
    while (*p && *p != ',' && i + 1 < sizeof(tbuf)) tbuf[i++] = *p++;
    tbuf[i] = 0;
    int hh, mm, ss, ms;
    if (!hhmmss_ms(tbuf, &hh, &mm, &ss, &ms)) return 0;
    return ymd_to_epoch_ms(yy, mo, dd, hh, mm, ss, ms, epoch_ms);
}

static int extract_raw(const char* json, char* out, size_t out_n) {
    const char* k = strstr(json, "\"raw_nmea\"");
    if (!k) return 0;
    k = strchr(k, ':');
    if (!k) return 0;
    ++k;
    while (*k == ' ' || *k == '\t') ++k;
    if (*k != '"') return 0;
    ++k;
    size_t i = 0;
    while (*k && *k != '"' && i + 1 < out_n) {
        if (*k == '\\' && k[1]) ++k;
        out[i++] = *k++;
    }
    out[i] = 0;
    return (int)i > 0;
}

static int query_gnss_epoch_ms(long long* epoch_ms, long long* t1_ms) {
    long long t0 = realtime_ms();
    (void)t0;
    FILE* fp = popen("ubus call sim-manager gnss_get_position 2>/dev/null", "r");
    if (!fp) return 0;
    char json[8192];
    size_t n = 0;
    json[0] = 0;
    while (n + 1 < sizeof(json)) {
        if (!fgets(json + n, (int)(sizeof(json) - n), fp)) break;
        n = strlen(json);
    }
    pclose(fp);
    *t1_ms = realtime_ms();

    if (!strstr(json, "\"fix\": true") && !strstr(json, "\"fix\":true")) {
        return 0;
    }
    char raw[4096];
    if (!extract_raw(json, raw, sizeof(raw))) return 0;
    long long rmc = 0, gga = 0;
    int yy = 0, mo = 0, dd = 0;
    if (!parse_rmc(raw, &rmc, &yy, &mo, &dd)) return 0;
    *epoch_ms = rmc;
    if (parse_gga(raw, yy, mo, dd, &gga) && gga > rmc) {
        *epoch_ms = gga;
    }
    return 1;
}

static int send_sample(const char* sock_path, long long sys_ms, long long gnss_ms) {
    struct sock_sample s;
    memset(&s, 0, sizeof(s));
    s.tv.tv_sec = (time_t)(sys_ms / 1000);
    s.tv.tv_usec = (suseconds_t)((sys_ms % 1000) * 1000);
    /* offset = true - system (seconds) */
    s.offset = ((double)(gnss_ms - sys_ms)) / 1000.0;
    s.pulse = 0;
    s.leap = 0;
    s.magic = SOCK_MAGIC;

    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd < 0) return -1;
    struct sockaddr_un sa;
    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_UNIX;
    snprintf(sa.sun_path, sizeof(sa.sun_path), "%s", sock_path);
    if (connect(fd, (struct sockaddr*)&sa, sizeof(sa)) != 0) {
        close(fd);
        return -1;
    }
    ssize_t n = send(fd, &s, sizeof(s), 0);
    close(fd);
    return (n == (ssize_t)sizeof(s)) ? 0 : -1;
}

int main(int argc, char** argv) {
    const char* sock = DEFAULT_SOCK;
    int period = DEFAULT_PERIOD_S;
    if (argc > 1) sock = argv[1];
    if (argc > 2) period = atoi(argv[2]);
    if (period < 8) period = 8;

    fprintf(stderr, "gnss_chrony_sock: sock=%s period=%ds\n", sock, period);
    for (;;) {
        long long gnss = 0, t1 = 0;
        if (query_gnss_epoch_ms(&gnss, &t1)) {
            double off_ms = (double)(gnss - t1);
            if (send_sample(sock, t1, gnss) == 0) {
                fprintf(stderr, "gnss_chrony_sock: sample offset_ms=%.0f ok\n", off_ms);
            } else {
                fprintf(stderr, "gnss_chrony_sock: send failed errno=%d (%s)\n",
                        errno, strerror(errno));
            }
        } else {
            fprintf(stderr, "gnss_chrony_sock: no GNSS fix\n");
        }
        sleep((unsigned)period);
    }
    return 0;
}
