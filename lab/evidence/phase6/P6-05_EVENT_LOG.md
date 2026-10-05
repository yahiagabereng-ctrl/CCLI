# P6-05 — Annex M trip event logging (O.14)

**Status:** **PASS (implementation)**

## Code paths

| Sink | Category | Detail |
|------|----------|--------|
| stderr / `/tmp/ccli.log` | `io` | `curtail_blocked_annex_m` |
| stderr / `/tmp/ccli.log` | `io` | `annex_m_trip=ACTIVE` in status line |
| Event ring | `annex_m` | `teledistacco_inhibit` (when PF2 commanded + trip active + DO transition) |
| Bench JSON | field | `annex_m_trip_active: true` |

## Trigger condition

```text
annex_m_trip && pf2.curtailment_active() → block DIO1 + log + event
```

## Lab observation (2026-10-02)

With Modbus P below PF2 threshold during trip test, status line confirmed:

```text
io: curtail_do=OFF permissive=OK annex_m_trip=ACTIVE
```

Event `teledistacco_inhibit` fires on **DO state transition** while curtail commanded — reproduce with P > threshold + DIO3 ON.

**Source:** `apps/ccli/services/ccli_main.cpp` (annex_m event append).
