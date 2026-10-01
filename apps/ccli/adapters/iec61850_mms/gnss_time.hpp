#pragma once

#include <cstdint>

/** TesPro TG544 GNSS lock via ubus sim-manager (T.3.3.4.5 / P3-11). */
struct GnssFixStatus {
    bool queried{false};   /**< true if ubus returned parseable JSON */
    bool fix{false};       /**< Position Fixed */
    int  satellites{0};
    char time_utc[16]{};   /**< "HH:MM:SS" from modem, if present */
    char date_utc[16]{};   /**< "YYYY-MM-DD" if present */
    bool has_epoch{false};
    std::int64_t gnss_epoch_ms{0}; /**< from NMEA RMC UTC */
    std::int64_t offset_ms{0};     /**< system_now - gnss_epoch (at receive) */
    std::int64_t ubus_rtt_ms{0};
};

/**
 * Call `ubus call sim-manager gnss_get_position`.
 * Slow (modem AT) — throttle callers (e.g. every 10–15 s).
 * @return true when ubus ran and JSON was parsed (fix may still be false).
 */
bool gnss_query_fix(GnssFixStatus& out);

/** chronyd tracking snapshot for T.3.3.4.5 (±100 ms). */
struct ChronyStatus {
    bool queried{false};
    bool available{false};
    bool synchronized{false}; /**< Leap Normal and |offset| <= 100 ms */
    double offset_s{0};       /**< system − NTP/ref (seconds); sign from chronyc */
    char leap[32]{};
    char refid[32]{};
};

/** Parse `chronyc tracking` (no clock step). */
bool chrony_query_status(ChronyStatus& out);

/**
 * Legacy lab helper: step CLOCK_REALTIME from NMEA. Prefer chrony.
 * @return true if within ±100 ms after check/step (or already was).
 */
bool gnss_discipline_clock(const GnssFixStatus& st, std::int64_t* applied_offset_ms);
