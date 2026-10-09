# P7 — Evidence pack / O.14 data logger (Phase 7)

**Date opened:** 2026-10-02  
**DUT:** TesPro TG544 @ `192.168.10.1`  
**Product build (target):** **0.1.0-r35** (P7-O14-LOGGER)  
**Normative refs:** CEI 0-16 Allegato **O.14** · **O.15** (cert path) · **K7.5** DSO demo  
**Companion:** [CCI_Phase_Regulation_Checklists.md](../../../knowledge-base/08-engineering/CCI_Phase_Regulation_Checklists.md)

---

## Phase exit gate

| ID | Requirement | Status | Evidence |
|----|-------------|--------|----------|
| **P7-01** | ≥2048 events, user cannot overwrite | **HAVE (software r35)** | wrap test + `--event-clear` denied + 0640 |
| **P7-02** | Timestamp `yyyy/mm/dd hh:mm:ss` | **HAVE** | `--event-dump` UTC |
| **P7-03** | Remote syslog RFC 5424 | **HAVE (UDP client r35)** | `event_log.syslog_*` — SIEM receive still lab wiring |
| **P7-04** | Mandatory O.14 category matrix | **HAVE (software)** · HW PART | [P7-04_EVENT_COVERAGE_MATRIX.md](P7-04_EVENT_COVERAGE_MATRIX.md) |
| **P7-11** | K7.5 DSO demo script | **PART** | [P7-11_DSO_DEMO_SCRIPT.md](P7-11_DSO_DEMO_SCRIPT.md) |

**Exit rule:** P7-01, P7-04, P7-11 **PASS** (lab). Accredited O.15 certification is **after** this pack.

---

## Software artifacts (r35)

| Component | Path |
|-----------|------|
| Event ring + persistence | `apps/ccli/core/event/event_store.{hpp,cpp}` |
| Config | `event_log.enabled`, `event_log.path` in lab yaml |
| CLI dump | `ccli --event-dump [--count N] [--json]` |
| CLI wrap test | `ccli --event-wrap-test` |
| Lab script | `lab/tg544-openwrt/run-p7-event-wrap-test.ps1` |

**Default store:** `/var/lib/ccli/events.jsonl` (root-owned, append-only from daemon).

---

## Lab procedure (quick)

```powershell
# On lab PC — after deploy r27
.\lab\tg544-openwrt\run-p7-event-wrap-test.ps1

# On DUT (via plink)
mkdir -p /var/lib/ccli
ccli --event-wrap-test
ccli --config /etc/ccli/lab.yaml --event-dump --count 20
```

---

## Open / deferred (not blocking Phase 7 software exit)

| ID | Item |
|----|------|
| P7-03 | SIEM **receive** of RFC 5424 (client is r35) |
| P7-05…P7-10 | SDLC, 62351-100-3 cert plan, FIPS claim, EMC, ARERA memo |
| P7-12 | `--lab-demo` off in product image audit |

---

## Related phases

- P5 MMS full-circle: [../phase5/P5_FINAL_CLOSEOUT.md](../phase5/P5_FINAL_CLOSEOUT.md)
- P6 Annex M inhibit: [../phase6/P6_FINAL_CLOSEOUT.md](../phase6/P6_FINAL_CLOSEOUT.md)
