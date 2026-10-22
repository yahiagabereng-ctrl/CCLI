/*=============================================================================
 * GNSS fix poll — TesPro sim-manager ubus (modem Quectel QGPS / LuCI GNSS).
 * Clause: Annex T T.3.3.4.5 · REQ-TIME-002 · K3.5
 *=============================================================================*/
#define _GNU_SOURCE
#include "gnss_time.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <sys/time.h>
#include <time.h>

namespace {

bool parse_bool_key(const char* json, const char* key, bool* value) {
    const char* p = std::strstr(json, key);
    if (p == nullptr || value == nullptr) {
        return false;
    }
    p += std::strlen(key);
    while (*p == ' ' || *p == '\t' || *p == ':') {
        ++p;
    }
    if (std::strncmp(p, "true", 4) == 0) {
        *value = true;
        return true;
    }
    if (std::strncmp(p, "false", 5) == 0) {
        *value = false;
        return true;
    }
    return false;
}

bool parse_int_key(const char* json, const char* key, int* value) {
    const char* p = std::strstr(json, key);
    if (p == nullptr || value == nullptr) {
        return false;
    }
    p += std::strlen(key);
    while (*p == ' ' || *p == '\t' || *p == ':' || *p == '"') {
        ++p;
    }
    *value = std::atoi(p);
    return true;
}

bool parse_string_key(const char* json, const char* key, char* out, size_t out_n) {
    const char* p = std::strstr(json, key);
    if (p == nullptr || out == nullptr || out_n == 0) {
        return false;
    }
    p += std::strlen(key);
    while (*p == ' ' || *p == '\t' || *p == ':') {
        ++p;
    }
    if (*p != '"') {
        return false;
    }
    ++p;
    size_t i = 0;
    while (*p != '\0' && *p != '"' && i + 1 < out_n) {
        out[i++] = *p++;
    }
    out[i] = '\0';
    return i > 0;
}

bool hhmmss_frac_ms(const char* tbuf, int* hh, int* mm, int* ss, int* ms) {
    if (tbuf == nullptr || std::strlen(tbuf) < 6 || hh == nullptr || mm == nullptr ||
        ss == nullptr || ms == nullptr) {
        return false;
    }
    *hh = (tbuf[0] - '0') * 10 + (tbuf[1] - '0');
    *mm = (tbuf[2] - '0') * 10 + (tbuf[3] - '0');
    *ss = (tbuf[4] - '0') * 10 + (tbuf[5] - '0');
    *ms = 0;
    if (tbuf[6] == '.' && tbuf[7]) {
        *ms = (tbuf[7] - '0') * 100;
        if (tbuf[8]) {
            *ms += (tbuf[8] - '0') * 10;
        }
        if (tbuf[9]) {
            *ms += (tbuf[9] - '0');
        }
    }
    return true;
}

bool ymd_hms_to_epoch_ms(int yy, int mo, int dd, int hh, int mm, int ss, int ms,
                         std::int64_t* epoch_ms) {
    if (epoch_ms == nullptr) {
        return false;
    }
    std::tm tm{};
    tm.tm_year = 2000 + yy - 1900;
    tm.tm_mon = mo - 1;
    tm.tm_mday = dd;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = ss;
    tm.tm_isdst = 0;
#if defined(_WIN32)
    (void)ms;
    return false;
#else
    const time_t sec = timegm(&tm);
    if (sec == static_cast<time_t>(-1)) {
        return false;
    }
    /* Sanity: 2020-01-01 .. 2040-01-01 UTC */
    if (sec < 1577836800 || sec > 2208988800) {
        return false;
    }
    *epoch_ms = static_cast<std::int64_t>(sec) * 1000 + ms;
    return true;
#endif
}

/** Parse $GNRMC/$GPRMC → UTC epoch ms (status A only). */
bool parse_rmc_epoch_ms(const char* raw, std::int64_t* epoch_ms) {
    if (raw == nullptr || epoch_ms == nullptr) {
        return false;
    }
    const char* p = std::strstr(raw, "$GNRMC,");
    if (p == nullptr) {
        p = std::strstr(raw, "$GPRMC,");
    }
    if (p == nullptr) {
        return false;
    }
    p = std::strchr(p, ',');
    if (p == nullptr) {
        return false;
    }
    ++p; /* hhmmss.ss */
    char tbuf[16]{};
    size_t i = 0;
    while (*p && *p != ',' && i + 1 < sizeof(tbuf)) {
        tbuf[i++] = *p++;
    }
    if (*p != ',') {
        return false;
    }
    ++p;
    if (*p != 'A') {
        return false; /* void */
    }
    /* RMC after status: lat,N,lon,E,spd,cog,date — 7 commas to date. */
    int commas = 0;
    while (*p && commas < 7) {
        if (*p == ',') {
            ++commas;
        }
        ++p;
    }
    if (commas < 7 || std::strlen(p) < 6) {
        return false;
    }
    char dbuf[8]{};
    std::memcpy(dbuf, p, 6);
    dbuf[6] = '\0';

    int hh = 0, mm = 0, ss = 0, ms = 0;
    if (!hhmmss_frac_ms(tbuf, &hh, &mm, &ss, &ms)) {
        return false;
    }
    const int dd = (dbuf[0] - '0') * 10 + (dbuf[1] - '0');
    const int mo = (dbuf[2] - '0') * 10 + (dbuf[3] - '0');
    const int yy = (dbuf[4] - '0') * 10 + (dbuf[5] - '0');
    return ymd_hms_to_epoch_ms(yy, mo, dd, hh, mm, ss, ms, epoch_ms);
}

/**
 * Parse $GNGGA/$GPGGA hhmmss — date taken from companion RMC (GGA has no date).
 * Modem ubus returns RMC then GGA; GGA is typically ~1–2 s newer.
 */
bool parse_gga_epoch_ms(const char* raw, std::int64_t rmc_epoch_ms,
                        std::int64_t* epoch_ms) {
    if (raw == nullptr || epoch_ms == nullptr || rmc_epoch_ms <= 0) {
        return false;
    }
    const char* p = std::strstr(raw, "$GNGGA,");
    if (p == nullptr) {
        p = std::strstr(raw, "$GPGGA,");
    }
    if (p == nullptr) {
        return false;
    }
    p = std::strchr(p, ',');
    if (p == nullptr) {
        return false;
    }
    ++p;
    char tbuf[16]{};
    size_t i = 0;
    while (*p && *p != ',' && i + 1 < sizeof(tbuf)) {
        tbuf[i++] = *p++;
    }
    int hh = 0, mm = 0, ss = 0, ms = 0;
    if (!hhmmss_frac_ms(tbuf, &hh, &mm, &ss, &ms)) {
        return false;
    }
    /* Reuse RMC calendar day; adjust if GGA time wrapped past midnight. */
    const time_t rmc_sec = static_cast<time_t>(rmc_epoch_ms / 1000);
    std::tm tm_rmc{};
#if defined(_WIN32)
    (void)rmc_sec;
    return false;
#else
    if (gmtime_r(&rmc_sec, &tm_rmc) == nullptr) {
        return false;
    }
    std::tm tm = tm_rmc;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = ss;
    tm.tm_isdst = 0;
    time_t sec = timegm(&tm);
    if (sec == static_cast<time_t>(-1)) {
        return false;
    }
    /* If GGA appears earlier than RMC by >12 h, assume next UTC day. */
    if (sec + 12 * 3600 < rmc_sec) {
        tm.tm_mday += 1;
        sec = timegm(&tm);
    }
    if (sec < 1577836800 || sec > 2208988800) {
        return false;
    }
    *epoch_ms = static_cast<std::int64_t>(sec) * 1000 + ms;
    return true;
#endif
}

/** Prefer latest among RMC/GGA (GGA is fetched later in sim-manager). */
bool parse_nmea_epoch_ms(const char* raw, std::int64_t* epoch_ms) {
    std::int64_t rmc = 0;
    if (!parse_rmc_epoch_ms(raw, &rmc)) {
        return false;
    }
    std::int64_t gga = 0;
    if (parse_gga_epoch_ms(raw, rmc, &gga) && gga > rmc) {
        *epoch_ms = gga;
    } else {
        *epoch_ms = rmc;
    }
    return true;
}

std::int64_t realtime_now_ms() {
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<std::int64_t>(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}

}  // namespace

bool gnss_query_fix(GnssFixStatus& out) {
    out = GnssFixStatus{};
    const std::int64_t t0 = realtime_now_ms();
    FILE* fp = popen("ubus call sim-manager gnss_get_position 2>/dev/null", "r");
    if (fp == nullptr) {
        return false;
    }
    char buf[8192];
    size_t total = 0;
    buf[0] = '\0';
    while (total + 1 < sizeof(buf)) {
        if (std::fgets(buf + total, static_cast<int>(sizeof(buf) - total), fp) ==
            nullptr) {
            break;
        }
        total = std::strlen(buf);
    }
    const int st = pclose(fp);
    const std::int64_t t1 = realtime_now_ms();
    if (st != 0 || total == 0) {
        return false;
    }

    bool fix = false;
    if (!parse_bool_key(buf, "\"fix\"", &fix)) {
        return false;
    }
    out.queried = true;
    out.fix = fix;
    parse_int_key(buf, "\"satellites\"", &out.satellites);
    if (out.satellites == 0) {
        char sat_s[16]{};
        if (parse_string_key(buf, "\"satellites\"", sat_s, sizeof(sat_s))) {
            out.satellites = std::atoi(sat_s);
        }
    }
    parse_string_key(buf, "\"time\"", out.time_utc, sizeof(out.time_utc));
    parse_string_key(buf, "\"date\"", out.date_utc, sizeof(out.date_utc));

    char raw[2048]{};
    if (parse_string_key(buf, "\"raw_nmea\"", raw, sizeof(raw))) {
        if (parse_nmea_epoch_ms(raw, &out.gnss_epoch_ms)) {
            out.has_epoch = true;
        }
    }
    /* Fallback: JSON date + time fields (second resolution). */
    if (!out.has_epoch && out.date_utc[0] && out.time_utc[0]) {
        std::tm tm{};
        int y = 0, mo = 0, d = 0, hh = 0, mi = 0, se = 0;
        if (std::sscanf(out.date_utc, "%d-%d-%d", &y, &mo, &d) == 3 &&
            std::sscanf(out.time_utc, "%d:%d:%d", &hh, &mi, &se) == 3) {
            tm.tm_year = y - 1900;
            tm.tm_mon = mo - 1;
            tm.tm_mday = d;
            tm.tm_hour = hh;
            tm.tm_min = mi;
            tm.tm_sec = se;
            tm.tm_isdst = 0;
            const time_t sec = timegm(&tm);
            if (sec != static_cast<time_t>(-1) && sec >= 1577836800 &&
                sec <= 2208988800) {
                out.gnss_epoch_ms = static_cast<std::int64_t>(sec) * 1000;
                out.has_epoch = true;
            }
        }
    }
    if (out.has_epoch) {
        out.offset_ms = t1 - out.gnss_epoch_ms;
        out.ubus_rtt_ms = t1 - t0;
    }
    return true;
}

bool chrony_query_status(ChronyStatus& out) {
    out = ChronyStatus{};
    FILE* fp = popen("chronyc tracking 2>/dev/null", "r");
    if (fp == nullptr) {
        return false;
    }
    char line[256];
    double sys_off = 0;
    bool got_off = false;
    bool got_leap = false;
    while (std::fgets(line, sizeof(line), fp) != nullptr) {
        if (std::strstr(line, "System time") != nullptr) {
            /* "System time     : 0.000012345 seconds slow of NTP time" */
            char* p = std::strchr(line, ':');
            if (p != nullptr) {
                sys_off = std::atof(p + 1);
                if (std::strstr(line, "slow") != nullptr) {
                    sys_off = -sys_off; /* system behind reference */
                }
                /* "fast" keeps positive: system ahead */
                got_off = true;
            }
        } else if (std::strstr(line, "Leap status") != nullptr) {
            char* p = std::strchr(line, ':');
            if (p != nullptr) {
                ++p;
                while (*p == ' ' || *p == '\t') {
                    ++p;
                }
                size_t i = 0;
                while (*p && *p != '\n' && *p != '\r' && i + 1 < sizeof(out.leap)) {
                    out.leap[i++] = *p++;
                }
                out.leap[i] = '\0';
                got_leap = true;
            }
        } else if (std::strstr(line, "Reference ID") != nullptr ||
                   std::strstr(line, "Ref time") != nullptr) {
            /* optional refid from "Reference ID    : GNSS" style */
            char* p = std::strchr(line, ':');
            if (p != nullptr && out.refid[0] == '\0') {
                ++p;
                while (*p == ' ' || *p == '\t') {
                    ++p;
                }
                size_t i = 0;
                while (*p && *p != '\n' && *p != '\r' && *p != ' ' &&
                       i + 1 < sizeof(out.refid)) {
                    out.refid[i++] = *p++;
                }
                out.refid[i] = '\0';
            }
        }
    }
    const int st = pclose(fp);
    out.queried = true;
    if (st != 0 || !got_off) {
        return false;
    }
    out.available = true;
    out.offset_s = sys_off;
    const double abs_off = sys_off >= 0 ? sys_off : -sys_off;
    const bool leap_ok =
        !got_leap || std::strstr(out.leap, "Normal") != nullptr ||
        std::strstr(out.leap, "Insert") != nullptr ||
        std::strstr(out.leap, "Delete") != nullptr;
    /* Not sync if chronyc says Not synchronised / Unsynchronised */
    if (got_leap && (std::strstr(out.leap, "not synchron") != nullptr ||
                     std::strstr(out.leap, "Not synchron") != nullptr ||
                     std::strstr(out.leap, "Unsync") != nullptr)) {
        out.synchronized = false;
    } else {
        out.synchronized = leap_ok && abs_off <= 0.100;
    }
    return true;
}

bool gnss_discipline_clock(const GnssFixStatus& st, std::int64_t* applied_offset_ms) {
    if (!st.fix || !st.has_epoch) {
        return false;
    }
    const std::int64_t abs_off =
        st.offset_ms >= 0 ? st.offset_ms : -st.offset_ms;
    if (abs_off <= 100) {
        if (applied_offset_ms != nullptr) {
            *applied_offset_ms = st.offset_ms;
        }
        return true; /* already within T.3.3.4.5 */
    }
    /* Step CLOCK_REALTIME to latest NMEA epoch (GGA preferred).
     * Lab only — stop ntpd first; product should use chrony/PPS. */
    const time_t sec = static_cast<time_t>(st.gnss_epoch_ms / 1000);
    const long nsec = static_cast<long>((st.gnss_epoch_ms % 1000) * 1000000L);
    timespec ts{};
    ts.tv_sec = sec;
    ts.tv_nsec = nsec;
    if (clock_settime(CLOCK_REALTIME, &ts) != 0) {
        return false;
    }
    if (applied_offset_ms != nullptr) {
        *applied_offset_ms = st.offset_ms;
    }
    return true;
}
