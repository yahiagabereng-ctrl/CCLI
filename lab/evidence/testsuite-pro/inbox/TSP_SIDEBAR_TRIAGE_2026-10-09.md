# TSP sidebar triage — 2026-10-09 (post SCL Verify Error 0)

**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` rev **7**  
**Offline export:** `apps/ccli/config/icd/Report.xlsx` (SCL Verify)  
**Session:** Phase 2 — SCL clean; other modules still show **badges** (normal).

---

## SCL Verify — **PASS (Errors)**

| Severity | Count | Lab gate |
|----------|------:|----------|
| **Error** | **0** | **PASS** |
| Warning | 63 | Document + waive (below) |
| Recommendation | 13 | Informational (SBOw/Oper on enhanced-security) |
| Information | 5 | Stats (31 LN, 4 RCB, edition) — OK |

### Warning clusters (match TSP tree + Report.xlsx strings)

| Test (TSP) | ~Count | Cause | Fix now? | Disposition |
|------------|-------:|-------|----------|-------------|
| **CompareToStandardCustomElement** | 15 | Annex T **custom lnClass** (DWMX, DAGC, DVAR, DPCC, …) not in IEC master library | No | **ACCEPT** — CEI TR 57-126 Figura 2 specimen |
| **Controls** | 26 | **direct-with-enhanced-security** without `sboTimeout` / `operTimeout` / `stSeld` in CID | Optional | **DEFER** — lab uses **Direct Operate** (T1-07/08 PASS) |
| **DataTypeTemplatesIntegrityCheck** | 20 | TSP **duplicate template id** pairing (APC/ENC/Mod↔Beh adjacent types) | No | **ACCEPT** — known TSP quirk after enum fix (see r2 triage) |
| **Services** | 1 | **TimerActivatedControl** claimed, no `operTm` in model | Optional | **DEFER** — trim `<TimerActivatedControl />` in cert CID or add operTm stubs |
| **LogControls** | 1 | **ConfLogControl max=1** but no `LogControl` in IED | Optional | **DEFER** — remove ConfLogControl for PICS-honest profile |

### Recommendation (13)

| Test | Cause | Disposition |
|------|-------|-------------|
| **Controls** | SBOw / Cancel / Oper “unused” on **enhanced-security** APC/ENC | **OK** — you operate via **Oper**, not SBO select |

### Information (5)

| Test | Content |
|------|---------|
| **Stats** | Edition 2007B4, LN count, RCB count — use in PICS narrative |

**Evidence file:** update counts in `TSP_OFF_SCLVERIFY_2026-10-09.txt` after saving `offline-scl/TSP_OFF_SCLVERIFY_2026-10-09_r7.xlsx`.

---

## Other sidebar modules (badges in screenshot)

Badges are **not** all SCL errors. Triage per module:

### LGOS — green check

| Item | Meaning |
|------|---------|
| Status | **OK** for **MMS-only lab client** |
| Context | **LGOS** supervises **GOOSE subscription**. DUT **publishes** GOOSE (LAN3); TSP on LAN1 is not a GOOSE subscriber. **SupSubscription** removed from CID rev 6+. |

**Gate:** No action for Tier 1 / Phase 2 MMS path.

---

### System Status — badge **1**

| Typical cause | Check |
|---------------|--------|
| Not **Connected** to DUT | Connect TLS `:3782` — expect green **Associated** |
| One **Warning** row (association / certificate / directory) | Export list → `TSP_P3_01_CONNECT_*.txt` |
| Already passed T1-01 / T1-10 | Badge may persist until refresh; use **Output** + DUT log as truth |

**Pass criteria:** Connect + no Initiate/Associate **Error** (T1-01, T1-10 evidence).

---

### Compare Model — badge **1**

| Item | Detail |
|------|--------|
| Meaning | **Online model ≠ CID** on at least one **scope** (often **root / LLN0 / MMXU** rows) |
| Lab result | **PASS (scoped)** — see `TSP_P3_COMPARE_2026-10-09.txt` |
| Must-be-green | **WlimDWMX1**, **WSdDAGC1** |
| Waived red | PdCMMXU1 values, Gen\*, SGG\*, LLN0 after URCB enable — **PLANT-GAP-02**, **LN-GAP-01** |

**Gate:** Structure + control LNs; **not** full static value parity.

---

### GOOSE Tracker — badge **2**

| Item | Detail |
|------|--------|
| Meaning | TSP sees **2 GSEControl** in CID (`gcb_PdC_Mis4sec`, `gcb_Stato_Allarmi`) but **no live GOOSE** on the **PC capture NIC** |
| Topology | GOOSE egress **LAN3** (`192.168.30.x`); bench PC TSP usually on **LAN1** `192.168.10.10` |
| Product | **P5-G02 PASS** — Wireshark / DUT log on LAN3 (see `P5_GOOSE_SESSION_RECORD_2026-10-05.md`) |

**Gate:** **Exclude** from first UCA quote (`LAB_REQUIREMENT_TEST_MATRIX` — GOOSE Tier 4). Badge **expected** on two-PC LAN1-only session.

**Optional clear badge:** Sniffer on LAN3, or TSP GOOSE Tracker bound to adapter that sees `01-0C-CD-01-00-01` / `…-02`.

---

### Mod/Beh/Sim — badge **1**

TSP reads **Mod.stVal** and **Beh.stVal** per regulation LN and flags **Warning** when the pair is not “healthy” per its rules (not the same as Compare).

| LN | Screenshot | Meaning | Lab |
|----|------------|---------|-----|
| **WSdDAGC1** | Mod **ON**, Beh **ON** | Normal — matches CID (`Beh=on`, Mod active after cfg/Operate) | **OK** (T1-08) |
| **WlimDWMX1** | Mod **ON**, Beh **OFF** | **Mod ON + Beh OFF** — TSP expects Beh **on** when function is active | **LAB-BEH-01** — ccli updates **Mod** on Operate; **Beh** still **off** in CID/runtime (not wired to track Mod) |
| **VArSdDVAR1**, **PFSPDFPF1**, **VArVDVVR1**, **PFWDPFW1** | Mod **OFF**, Beh **OFF** | Annex T **deferred** / baseline **inactive** | **P5-DEFER-01** — not in Tier-1 path |

**CID reference:** `WlimDWMX1` has `Beh=off`, `Mod=off` in static CID; yaml boots **Mod=5** then Operate → **Mod=1** without flipping **Beh**.

**Gate:** **Does not block** Phase 2 if **Wlim/WSd Operate** evidence exists (T1-07/08). Product follow-up: set **Wlim.Beh=stVal on** when **Mod** transitions to active (or document PIXIT that Beh follows internal FctOpSt only).

**Filter note:** Toolbar **“Set Mod”** limits the list to LNs with controllable Mod — expected for O.9.2.x review.

---

## Quick matrix — what blocks Phase 2?

| Module | Blocks bench? | Your target |
|--------|---------------|-------------|
| SCL Verify Errors | Yes | **0** — done |
| SCL Warnings | No | Document 63 (this file) |
| Compare (scoped) | No | Wlim/WSd Match + waivers |
| System Status | Yes if Connect fails | Green when DUT up |
| GOOSE Tracker | No (MMS quote) | Tier 4 / LAN3 day |
| LGOS | No | Green |
| Mod/Beh/Sim | No | 1 waiver typical |

**Next Phase 2 step:** **5** — Read **PdCMMXU1** TotW / TotVAr / PPV @ TLS (or cite T1-04 + `Report4.csv`).
