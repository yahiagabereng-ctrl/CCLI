# P6 — Phase closeout (Defence I/O / Annex M lab)

**Date:** 2026-10-02  
**Gate:** P6-03 software exit + documented gaps (P6-01, P6-06)  
**DUT:** TesPro TG544 @ `192.168.10.1`  
**Product build:** **0.1.0-r26** (P6-03-ANNEX-M)  
**Lab profile:** `lab_tr400_cleartext_tsp_ttyS1.yaml` + Annex M SMS poller  
**Normative refs:** CEI 0-16 Allegato **M** · **O.11** · **O.14**  
**Companion:** [P6_README.md](P6_README.md) · [CCI_Annex_M_Extract.md](../../../knowledge-base/08-engineering/CCI_Annex_M_Extract.md)

---

## Phase verdict

| Verdict | Meaning |
|---------|---------|
| **CLOSED (lab)** | P6-03 **PASS** — software inhibit path verified on DUT. P6-01 / P6-06 **documented GAP** acceptable per checklist exit rule. |

**Not in scope:** Field PI wiring photo (P6-02), accredited O.15 certification (Phase 7).

---

## Checklist register

| ID | Requirement | Status | Evidence |
|----|-------------|--------|----------|
| **P6-01** | M.1 telescatto path | **PART (lab)** | [P6-01_LAB_ARCHITECTURE.md](P6-01_LAB_ARCHITECTURE.md) — LTE→SMS→DIO3 mock; field = DSO modem→PI |
| **P6-02** | M.5.1 DO→PI wiring | **DEFERRED** | Lab relay on DIO3 not photographed; ubus/GD32 trip verified |
| **P6-03** | O.11 inhibit PF2 | **PASS** | [P6-03_ANNEX_M_INHIBIT.md](P6-03_ANNEX_M_INHIBIT.md), r26, log `annex_m_trip=ACTIVE` |
| **P6-04** | O.12 teledistacco sketch | **PART** | [P6-04_TELEDISTACCO_SKETCH.md](P6-04_TELEDISTACCO_SKETCH.md) |
| **P6-05** | O.14 log Annex M trip | **PASS (code)** | Event `annex_m` / `teledistacco_inhibit`; log `curtail_blocked_annex_m` |
| **P6-06** | REQ-IO-004 5 DI / 3 DO @ M class | **GAP** | [P6-06_IO_VOLTAGE_GAP.md](P6-06_IO_VOLTAGE_GAP.md) |
| **P6-07** | M.3.1 vs TR400 GPIO | **PART** | [P6-07_M_VOLTAGE_DELTA.md](P6-07_M_VOLTAGE_DELTA.md) |

---

## Lab architecture (validated)

```text
1NCE Portal MT-SMS (TRIP/CLEAR)
        │
        ▼
Quectel EC200A (usb0, ICCID 89882280666900497532)
        │
        ▼  cron ~30s
annex-m-sms-poller.sh → annex-m-trip.sh → ubus dido_v2 ch3
        │
        ▼
ccli r26 annex_m_trip_monitor (gpio 3) → inhibit DIO1 PF2 curtail [O.11]
```

| Layer | Artifact |
|-------|----------|
| SMS poller | `lab/tg544-openwrt/annex-m-sms-poller.sh` |
| Trip driver | `lab/tg544-openwrt/annex-m-trip.sh` |
| Device ack | `/tmp/annex-m-sms-last-reply.txt` |
| ccli config | `annex_m_trip_monitor.enabled: true`, `gpio: 3` |
| Deploy | `deploy-annex-m-sms.ps1`, `deploy-p6-03-config.ps1` |
| Evidence script | `run-p6-annex-m-inhibit.ps1` |

---

## Key validation snippets (2026-10-02)

```text
ccli 0.1.0-r26 (P6-03-ANNEX-M)
svc_io: annex_m_trip_monitor ch3 (O.11 inhibit DIO1) [P6-03]
io: curtail_do=OFF permissive=OK annex_m_trip=ACTIVE
SMS idx=0 body='TRIP' → trip on
2026-10-02T22:55:04+08:00 sms_ack: CCLI OK TRIP DIO3=1
```

---

## Deferred / product (non-blocking lab close)

| Item | Track |
|------|-------|
| DSO formal letter / site waiver for M.1 | P6-01 product |
| DIO3 relay wiring photo | P6-02 bench |
| MO-SMS reply via modem (CMS 520) | 1NCE portal MO optional; file ack reliable |
| G2RL / 10–120 V DI expansion | P6-06 BOM |
| Product TLS revert post-P5 | Phase 5 closeout note |

---

## PC commands (replay)

```powershell
$env:CCLI_TG544_PW = '<pw>'
# Binary once per build
pscp -scp lab\tg544-openwrt\ccli-bin root@192.168.10.1:/tmp/ccli.new
plink ... "sh /tmp/deploy-ccli-dut.sh"

.\lab\tg544-openwrt\deploy-p6-03-config.ps1
.\lab\tg544-openwrt\deploy-annex-m-sms.ps1
.\lab\tg544-openwrt\run-p6-annex-m-inhibit.ps1
```

---

## Sign-off

| Role | Item |
|------|------|
| Software gate | **P6-03 PASS** |
| Architecture | Lab mock documented; field PI path TBD |
| Hardware | REQ-IO-004 gap logged; TG544 DIO = 12 V lab class only |

**Phase 6 lab status:** **CLOSED** 2026-10-02.
