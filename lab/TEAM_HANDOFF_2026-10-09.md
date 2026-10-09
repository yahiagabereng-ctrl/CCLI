# CCLI lab — team handoff (2026-10-09)

**Audience:** HiTEKS CCLI / lab bench  
**Repo:** `https://github.com/yahiagabereng-ctrl/CCLI.git`  
**Branch:** `master`  
**Pin commits:** `a1dee46` (evidence archive) · **`a4b8d1c`** (Compare fix + specimen CID/cfg)

---

## Executive summary

Two-PC formal **Test Suite Pro** session on **TG544** (Eth_A **3782 TLS**, Eth_B **104**, LAN3 **EMT432** meter). **Phase 1 Tier-1** controls and reporting largely **PASS**; **Phase 2** full-circle through Compare/SCL; **DUT redeployed** with Compare-fix CID/cfg.

| Area | Status |
|------|--------|
| MMS TLS Connect / reconnect | **PASS** (T1-01, T1-10) |
| Browse 31 LN | **PASS** (T1-03) |
| URCB 4 s PdC | **PASS** (T1-05/06) |
| Wlim / WSd Operate | **PASS** (T1-07/08) |
| Comms fallback 15 s | **PASS** (T1-09) |
| Compare Model (scoped) | **PASS** — waivers; **489 → ~69** after `a4b8d1c` + DUT deploy |
| SCL Verify (offline) | **Error 0** after CID rev 6–8 hygiene |
| DUT build | **0.1.0-r52** · `lab_tr400_phase4_emt432_lan3_tcp.yaml` |

---

## DUT state (after evening deploy)

| Item | Value |
|------|--------|
| **Host** | `192.168.10.1` (LAN1 MMS) |
| **Version** | **0.1.0-r52** (P3-07-AARE-SIGN-RAW-GT) |
| **Deploy manifest** | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r52_2026-10-09_192326.md` |
| **CID/cfg on DUT** | `/etc/ccli/icd/lab_tg544_eth_a.{cid,cfg}` · **configRev `20261009`** |
| **Specimen doc** | `lab/conformance/LAB_SPECIMEN_VERSION.md` |

**Deploy command (repeat):**

```powershell
cd D:\CCLI\CCLI\CCLI
. .\lab\tg544-openwrt\lab-env.ps1
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -HostAddr 192.168.10.1 `
  -LabYaml "apps\ccli\config\lab_tr400_phase4_emt432_lan3_tcp.yaml" `
  -Changes @("Describe change here")
```

Stop **`iec61850service`** on DUT before deploy if **3782** bind fails.

---

## Evidence index (start here)

| Doc | Purpose |
|-----|---------|
| [`lab/evidence/testsuite-pro/inbox/TSP_PHASE1_SESSION_INDEX_2026-10-09.md`](evidence/testsuite-pro/inbox/TSP_PHASE1_SESSION_INDEX_2026-10-09.md) | Tier-1 T1-01…T1-10 map |
| [`lab/evidence/testsuite-pro/inbox/PHASE2_STEP_BY_STEP_2026-10-09.md`](evidence/testsuite-pro/inbox/PHASE2_STEP_BY_STEP_2026-10-09.md) | Phase 2 tracker (step 5+ open) |
| [`lab/evidence/testsuite-pro/inbox/LAB_SESSION_START_2026-10-09_FINAL.md`](evidence/testsuite-pro/inbox/LAB_SESSION_START_2026-10-09_FINAL.md) | Bench topology GO/NO-GO |
| [`lab/evidence/testsuite-pro/inbox/TSP_SIDEBAR_TRIAGE_2026-10-09.md`](evidence/testsuite-pro/inbox/TSP_SIDEBAR_TRIAGE_2026-10-09.md) | SCL Warnings + Compare/GOOSE/Mod badges |
| [`lab/evidence/testsuite-pro/inbox/TSP_P3_COMPARE_FIX_2026-10-09.md`](evidence/testsuite-pro/inbox/TSP_P3_COMPARE_FIX_2026-10-09.md) | **489 → 69** Compare root cause + re-test |
| [`lab/evidence/testsuite-pro/inbox/TSP_OFF_SCLVERIFY_2026-10-09.txt`](evidence/testsuite-pro/inbox/TSP_OFF_SCLVERIFY_2026-10-09.txt) | Offline SCL counts |
| [`lab/conformance/LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md`](conformance/LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md) | Annex session runbook |

**Tooling:** `scripts/tsp-evidence-logger.ps1` · [`EVIDENCE_PER_TEST_PROTOCOL.md`](evidence/testsuite-pro/EVIDENCE_PER_TEST_PROTOCOL.md)

---

## Compare fix (`a4b8d1c`) — what changed

1. **MMXU2** (Gen*, St, SGG): removed **PPV/A** from LNodeType — datasets already **TotW-only**; removes TSP dual Missing (~420 rows).
2. **URCB/BRCB** `OptFields`: **entryID=false**; regen **`.cfg`** RC options (URCB 159, BRCB 191).
3. **NamPlt configRev** **`20261009`** aligned repo ↔ DUT ↔ TSP CID import.

**TSP re-test:** Disconnect → Connect → **URCB off** → Compare → expect **~69** rows (PdC PPV/A SDO artefact may remain — waived).

---

## Plant / measures note

**PdC TotW/TotVAr ≈ 0** is **expected** with **EMT432** on LAN3 (no CT), not the old **450 kW RS485 mock**. See [`P5_EMT432_METER_VALUES_2026-10-09.txt`](evidence/phase5/P5_EMT432_METER_VALUES_2026-10-09.txt).

---

## Open work (next session)

| Step | Owner | Action |
|------|-------|--------|
| Phase 2 step **5–14** | PC-B TSP | PdC reads (document EMT432 baseline), URCB cite/re-run, export full-circle txt |
| Compare **AFTER** xlsx | PC-B | Save to inbox after post-deploy Compare |
| Phase 3 security negatives | Lab | P3-07 matrix |
| Phase 4 **104** | PC-A | When matrix row scheduled |
| Product | Eng | **Wlim Beh** vs **Mod ON** (LAB-BEH-01) — optional PIXIT |

---

## Pull for team

```bash
git pull origin master
# Expect: a1dee46, a4b8d1c, compare fix docs, LAB_SPECIMEN_VERSION.md
```

**PC-B TSP:** Re-import `apps/ccli/config/icd/lab_tg544_eth_a.cid` from pulled repo (Header **revision 9**).

---

## Contact / session

**Operator:** Federico · **Date:** 2026-10-09 · **Platform:** TesPro TG544 · **OpenWrt** lab profile
