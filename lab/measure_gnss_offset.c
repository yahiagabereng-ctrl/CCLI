/* Measure GNSS UTC vs CLOCK_REALTIME (Annex T T.3.3.4.5 ±100 ms).
 * Prefers latest of RMC/GGA; ms resolution via clock_gettime.
 * Build (OpenWrt SDK aarch64): see lab/tg544-openwrt/wsl-build-measure-gnss.sh
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        if (*k == '\\' && k[1]) {
            ++k;
        }
        out[i++] = *k++;
    }
    out[i] = 0;
    return i > 0;
}

int main(void) {
    long long t0 = realtime_ms();
    FILE* fp = popen("ubus call sim-manager gnss_get_position 2>/dev/null", "r");
    if (!fp) {
        fprintf(stderr, "VERDICT: FAIL — popen ubus\n");
        return 2;
    }
    char json[8192];
    size_t n = 0;
    json[0] = 0;
    while (n + 1 < sizeof(json)) {
        if (!fgets(json + n, (int)(sizeof(json) - n), fp)) break;
        n = strlen(json);
    }
    pclose(fp);
    long long t1 = realtime_ms();

    char raw[4096];
    if (!extract_raw(json, raw, sizeof(raw))) {
        printf("VERDICT: FAIL — no raw_nmea\n");
        return 2;
    }

    long long rmc = 0, gga = 0, gnss = 0;
    int yy = 0, mo = 0, dd = 0;
    if (!parse_rmc(raw, &rmc, &yy, &mo, &dd)) {
        printf("VERDICT: FAIL — no valid RMC\n");
        return 2;
    }
    gnss = rmc;
    if (parse_gga(raw, yy, mo, dd, &gga) && gga > rmc) {
        gnss = gga;
    }

    long long offset = t1 - gnss;
    long long abs_off = offset >= 0 ? offset : -offset;

    printf("sys_t0_ms=%lld\n", t0);
    printf("sys_t1_ms=%lld\n", t1);
    printf("ubus_rtt_ms=%lld\n", t1 - t0);
    printf("rmc_epoch_ms=%lld\n", rmc);
    printf("gga_epoch_ms=%lld\n", gga);
    printf("gnss_epoch_ms=%lld (latest)\n", gnss);
    printf("offset_ms=%lld  (sys_t1 - gnss; + means system ahead)\n", offset);
    printf("abs_offset_ms=%lld\n", abs_off);
    printf("limit_ms=100\n");

    if (abs_off <= 100) {
        printf("VERDICT: PASS — |offset|<=100 ms (T.3.3.4.5)\n");
        return 0;
    }
    printf("VERDICT: FAIL — |offset|=%lld ms > 100 ms\n", abs_off);
    return 1;
}
