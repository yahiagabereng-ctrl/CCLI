# Phase 5 — IEC 61850 lab gate (cleartext MMS)

**Programme:** HiTEKS CCLI · TesPro TG544 (`platform_tg500`)  
**Gate ID:** `P5_FULLCIRCLE`  
**Gate status:** **CLOSED** (lab) 2026-10-01 · build **0.1.0-r25**  
**Product build (lab):** `0.1.0-r25` (P5-COMPARE-FIX)  
**Lab profile:** cleartext MMS `192.168.10.1:102` · Modbus RTU COM5 → **A1/B1** (`/dev/ttyS1`)  
**Product profile:** TLS MMS `:3782` (separate evidence — not this gate)  
**DSO client:** TSP (historical PASS) · **IEDExplorer** if TSP license blocked — [P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md)

---

## Purpose

Phase 5 closes the **lab functional gate** for IEC 61850 observability, logical-node model, timing accuracy (Annex O/T), Wlim/WSd/VArSd control paths, and TesPro northbound — validated with **Test Suite Pro** (r25 session), **automated MMS clients**, and **TesPro MQTT** cross-check.

Annex O defines **what** must be measured and when (4 s, grid-aligned). Annex T defines **how** it maps to IEC 61850 LNs and MMS services.

---

## Document map

| Doc | Role |
|-----|------|
| **[P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md)** | **Canonical annex-traced test sequence** (O/T clauses → TSP steps) |
| **[P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md)** | Operator checklist (TSP or IEDExplorer / object / pass criteria) |
| **[P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md](P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md)** | TSP license block + IEDExplorer fallback |
| **[P5_TIMING_ANNEX_AUDIT_2026-10-01.md](P5_TIMING_ANNEX_AUDIT_2026-10-01.md)** | Timing + LN alignment audit vs Annex O/T |
| **[P5_LN_VALUE_MATRIX.md](P5_LN_VALUE_MATRIX.md)** | Every LN/DO — source, expected value, TSP pass |
| **[P5_00_LN_MATRIX_AUDIT.md](P5_00_LN_MATRIX_AUDIT.md)** | CID vs runtime LN audit + gap register |
| **[P5_LN_WIRE_STATUS.md](P5_LN_WIRE_STATUS.md)** | 31-LN wire tiers (W-L-C / W--S) |
| **[P5_TSP_CLEARTEXT_README.md](P5_TSP_CLEARTEXT_README.md)** | Deploy + TSP connection setup |
| **[P5_FINAL_CLOSEOUT.md](P5_FINAL_CLOSEOUT.md)** | Gate status + evidence chain |
| **[P5_MMS_TLS_README.md](P5_MMS_TLS_README.md)** | Product TLS path (parallel track) |
| **[P5_TESPRO_NORTHBOUND.md](P5_TESPRO_NORTHBOUND.md)** | TesPro MQTT collector + `channel_1` setup |
| **[P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md)** | **Frozen northbound config** + broker/verify scripts |
| **[P5_A1B1_RS485_HW_MAP.md](P5_A1B1_RS485_HW_MAP.md)** | **RS485 bench freeze** — A1/B1 → `/dev/ttyS1` |
| **[P5_GOOSE_SESSION_RECORD_2026-10-05.md](P5_GOOSE_SESSION_RECORD_2026-10-05.md)** | **START HERE** — GOOSE session handoff + resume checklist |
| **[P5_G02_CLOSEOUT_2026-10-05.md](P5_G02_CLOSEOUT_2026-10-05.md)** | **GOOSE publish (P5-G02)** — MMS/GoCB + LAN3 wire pcap |

**Compare / waivers (inbox):**

- `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md`
- `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_FIX_2026-10-01.md`

**Corpus:**

- `Architecture/_extracted_reg_analysis/annex_o.txt`
- `Architecture/_extracted_reg_analysis/annex_t.txt`
- `apps/ccli/config/icd/lab_tg544_eth_a.cid`

---

## Quick start

```powershell
$env:CCLI_TG544_PW = '000000'
python -u lab\modbus_rtu_slave.py --port COM5 --power-kw 450 --trace
.\lab\tg544-openwrt\run-p5-fullcircle-cleartext.ps1
```

**TSP** (if licensed): IED `CCI016_01` · `192.168.10.1:102` · TLS **off** · CID `lab_tg544_eth_a.cid`

**IEDExplorer** (if TSP blocked): same connection · follow sequencer IEDExplorer column.

Execute steps in **[P5_TSP_FULLCIRCLE_SEQUENCER.md](P5_TSP_FULLCIRCLE_SEQUENCER.md)** (order matters — Compare **before** URCB enable; Compare waivers apply if using IEDExplorer-only).

---

## Phase 5 requirement summary

| Area | Annex | Lab gate | Product track |
|------|-------|----------|---------------|
| 31 LN CID on wire | TR 57-126 | **PASS** | — |
| PdC TotW/TotVAr/PPV | T.3.1.3 | **PASS** | P5-06 (A, live V) |
| 4 s integrity period | T.3.2.1 / O.8.3 | **PASS** | — |
| :00/:04/:08 grid | O.8.3 | **GAP** | REQ-MET-002 |
| UTC ±100 ms | T.3.3.4.5 | **PASS** (chrony) | NTS (REQ-TIM-003) |
| GNSS sync flag / Good q | T.3.3.4.5 | **PASS** | 13 sats · `gnss_fix=1` |
| Intra-LN t coherence | O.8.3 | **PASS** | — |
| Intra-LN q coherence | O.8.3 | **PART** | LN-GAP-08 |
| Wlim / WSd / VArSd | O.9 / T.3.1.4 | **PASS** (lab) | PFSP/VArV/PFW deferred |
| 3 s set-point spacing | O.7.3.3 | **GAP** | REQ-CTL-004 |
| TsP 60 s / TsQ 10 s | O.7.3.1 | **DEFERRED** | regulation bench |
| TesPro MQTT northbound | — (P4-00) | **PASS** | [P5_TESPRO_NORTHBOUND_2026-10-01.md](P5_TESPRO_NORTHBOUND_2026-10-01.md) |

---

## Evidence exports

| Artifact | Path pattern |
|----------|--------------|
| Automated full circle | `P5_FULLCIRCLE_CLEARTEXT*.txt` |
| TSP manual log | `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_*.txt` |
| Compare export | `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_*.xlsx` |
| Deploy manifest | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r25_*.md` |

---

## Revert to product TLS

```powershell
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b.yaml `
  -Changes @("revert TLS :3782 product profile")
```
