# Session report — P3-11 chrony TimeQuality

**Date:** 2026-09-25 · **DUT:** TG544 Eth_A

| Gate | Result | Evidence |
|------|--------|----------|
| P3-11 T.3.3.4.5 TimeQuality | **PASS** | chrony ≤±100 ms + GNSS fix → `clockNotSynchronized=0` |
| Absolute OS clock | **PASS** | `chronyc tracking` System time ~ms vs NTP |
| ccli STEP path | **retired** | `gnss_discipline_clock: false` |

## Changes

- Sideload static `chronyd`/`chronyc` 4.8 (aarch64 musl)
- `gnss_chrony_sock` → chrony SOCK refclock
- ccli r15: `chrony_poll` observe-only; no `clock_settime`
- Disable BusyBox `sysntpd` while chrony runs

## Note

Ubus NMEA vs system measure remains noisy (~1 s); do **not** use it as ±100 ms oracle — use `chronyc tracking`.
