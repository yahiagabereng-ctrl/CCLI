# Triangle MicroWorks 61850 Test Suite Pro — Engineering extract (CCLI)

**Document ID:** CCLI-LAB-TSP-EXT-001  
**Revision:** 1.0  
**Date:** 2026-09-26  
**RAG source_id:** `ccli-testsuite-pro-extract`  
**Upstream:** Triangle MicroWorks public product pages + What's New (harvest `testsuite-pro/raw/`)  
**CCLI role:** Eth_A **verification layer** (REQ-VER-001) — not the TesPro TG544 OEM  
**Procedure:** `lab/CCI_TestSuitePro_Verification_Layer.md`

---

## 1. Product identity

| Field | Value |
|-------|-------|
| Product | **61850 Test Suite Pro** (TSP) |
| Vendor | Triangle MicroWorks |
| Lab version observed | **v4.7.4** (Aug 2025) |
| IEC editions | Ed.1, Ed.2, Ed.2.1 |
| Host OS | Windows 10/Server (VM OK); Win11 via NDIS update (v4.7.3+) |
| Runtime deps | .NET **4.6.2** · ≥2 GB RAM (4 GB recommended) |

**Naming:** TSP ≠ **TesPro** (TG-524/TG544 hardware).

---

## 2. CCLI mapping — tool → path → gate

| TSP tool | CCLI path | Offline? | Gate |
|----------|-----------|----------|------|
| SCL Viewer | Load `lab_tg544_eth_a.cid` | **Yes** | OFF / model |
| SCL Verify | Same CID vs SCL rules | **Yes** | OFF |
| Compare Model | CID vs online discovery | Needs DUT | P3 model |
| Advanced Client / Test Client | MMS Connect · Read · Operate | Needs DUT | P3-01/04/06 |
| Report Viewer | `urcb_PdC_Mis4sec` TotW | Needs DUT | P3-03 · P5-M07 |
| Data Miner / Data Monitor | Browse DO/DA | Offline browse / live values | P3/P5 |
| Test Sequencer | Scripted Connect→Operate→Report | Draft offline | All P3/P5 |
| GOOSE Tracker | Plant GOOSE | N/A MVP | Deferred |
| Sniffer | Wire MMS/GOOSE | Needs NIC+DUT | P3-06 cleartext deny |
| System Status | Errors/Warnings | Both | Always |

Vendor methodology (aligns with CCLI): load SCL → clear System Status → observe → Sequencer automate.

---

## 3. CCLI connection profile (when DUT online)

| Parameter | Value | Source |
|-----------|-------|--------|
| IED | CCI016_01 | CID / `mms_adapter` |
| LD | LD_Plant | same |
| IP | 192.168.10.1 | LAN1 Eth_A |
| Port | **3782** TLS (lab yaml) | `lab_tr400_phase1_regulation.yaml` |
| CA | `apps/ccli/config/tls/root_CA.pem` | lab PKI |
| Client (write) | `client.pem` / `client.key` | DSO_OPERATOR |
| Client (read) | `viewer.pem` / `viewer.key` | VIEWER RBAC deny write |

---

## 4. Objects to verify (freeze)

From `apps/ccli/config/icd/signal_map.yaml` + runtime `mms_adapter.cpp`:

| Reference | Clause | Runtime |
|-----------|--------|---------|
| `LD_Plant/WlimDWMX1.{Mod,WMaxSptPct}` | O.9.2.2 | IMPL |
| `LD_Plant/WSdDAGC1.{Mod,WSptPct}` | O.9.2.3 | IMPL |
| `LD_Plant/PdCMMXU1.TotW` + URCB 4 s | T.3.1.3 | IMPL |
| VArSd / PFSP / VArV / PFW | O.9.1 | STUB Mod=5 |
| TotVAr / PPV | T.3.1.3 | DEFERRED P5-M |

**Offline check:** CID may lag dynamic model — SCL Verify + side-by-side with `signal_map.yaml` is mandatory before live Operate.

---

## 5. Log ingest contract

User drops files under `lab/evidence/testsuite-pro/inbox/` using names in that folder's README.  
Each text log should start with the metadata header (tsp_version, gate, tool, cid, tls).

Agent triage keywords:

| Log contains | Map to |
|--------------|--------|
| SCL Verify severity / OCL | OFF batch · CID quality |
| Connect fail / TLS | P3-06 |
| RptEna / TotW / intgPd | P3-03 |
| Operate Mod / WMaxSptPct / WSptPct | P3-04 |
| Access denied / VIEWER | P3-08 |
| Missing LN WSd / VArSd | CID↔runtime gap |

---

## 6. HAVE / MISSING (corpus)

| Item | Status |
|------|--------|
| Web overview / requirements / what's new raw | **HAVE** |
| Engineering extract (this file) | **HAVE** |
| Capture plan | **HAVE** |
| Lab procedure + evidence inbox | **HAVE** |
| Full Online Help HTML tree | **HAVE** — `testsuite-pro/help-offline/` from install v4.7.4.5037 |
| SCL Verify export for lab CID | **MISSING** — your offline run |
| Live Operate/Report logs | **WAIT** — DUT |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-26 | First extract from TMW public pages + CCLI gate map |
