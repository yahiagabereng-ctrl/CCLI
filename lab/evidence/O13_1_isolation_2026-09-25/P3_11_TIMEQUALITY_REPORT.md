# P3-11 TimeQuality / chrony — lab evidence

**Date:** 2026-09-25  
**DUT:** TG544 · `ccli-0.1.0-r15` · Eth_A `:3782`  
**Clause:** Annex T **T.3.3.4.5** · REQ-TIME-002 · K3.5

## Architecture

| Role | Component |
|------|-----------|
| Clock discipline | **chronyd** (NTP pool + GNSS SOCK feeder) |
| GNSS samples | `gnss_chrony_sock` ← ubus `sim-manager gnss_get_position` |
| TimeQuality / TotW.q | **ccli** observe-only (`chrony_poll=on`, `gnss_discipline_clock=off`) |

BusyBox `sysntpd` disabled while chrony runs.

## chronyc tracking (authoritative ±100 ms)

```
Reference ID    : … (NTP pool)
Stratum         : 3
System time     : ~0.000–0.008 s vs NTP
Leap status     : Normal
```

|offset| ≪ 100 ms → **PASS** for T.3.3.4.5 absolute UTC (NTP-backed).

GNSS SOCK source is present (`#? GNSS`) but variable (±0.5–1.3 s ubus NMEA age); chrony correctly prefers NTP until a PPS/low-jitter GNSS path exists (C5 gap).

## ccli log

```
mms: listening … chrony=on discipline=off …
mms: time_quality chrony offset_ms=0 within_pm100=yes leap=Normal
     gnss_fix=1 clockNotSynchronized=0 (T.3.3.4.5)
```

## Mapping

| Condition | `clockNotSynchronized` | `TotW.q` |
|-----------|------------------------|---------|
| chrony sync ≤100 ms + GNSS fix | 0 | GOOD |
| chrony unsync or no GNSS fix | 1 | QUESTIONABLE |

## Artifacts

- `P3_11_CHRONY_TRACKING.txt`
- Binaries: `lab/tg544-openwrt/{chronyd,chronyc,gnss_chrony_sock,chrony.conf,ccli-chrony.init}`
- Build: `wsl-build-chrony-static.sh`, `wsl-build-gnss-sock.sh`

## Product follow-up

- Prefer GNSS when PPS or low-latency NMEA is available (C5).
- Package chrony via TesPro LuCI / image; enable `/etc/init.d/ccli-chrony`.
- Optional: NTS (`chrony-nts`) for authenticated time.

**Verdict: PASS (lab)** — chrony holds UTC within ±100 ms; ccli reports TimeQuality from chrony + GNSS fix.
