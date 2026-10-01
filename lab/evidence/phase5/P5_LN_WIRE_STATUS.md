# P5 — Logical node wire status (accurate for Test Suite Pro)

**Document ID:** P5-LN-WIRE-STATUS-001  
**Date:** 2026-09-30  
**Build:** 0.1.0-r24 · `model_cfg: /etc/ccli/icd/lab_tg544_eth_a.cfg`  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` (31 LNs)

---

## Why docs said “DEFERRED” (and why that was misleading)

Three different meanings were mixed:

| Term (old docs) | What it actually meant | r23 truth |
|-----------------|------------------------|-----------|
| **DEFERRED** in `audit-ln-matrix.py` | Not in the **9-LN hand-built** list in `mms_adapter.cpp` | **Wrong label** — those LNs **are on MMS** via `.cfg` |
| **§D “CID-only — not on wire”** | Pre-r23 MVP path only | **Outdated** — r23 loads all **31** from genconfig |
| **DEFERRED (plant wiring)** | No live plant source (Modbus/DIO) | **Still correct** — static cfg defaults, not simulated |

**Rule for TSP:** distinguish **on MMS wire** (browse/read) vs **live-updated** (ccli refreshes) vs **control-wired** (Operate reaches PF2/DIO).

Verify DUT mode before TSP:

```text
grep model /tmp/ccli.log | tail -1
# expect: model=full_cid_cfg
# if model=mvp_handbuilt → only 9 LNs; Compare Model will fail
```

Regenerate this table: `python scripts/audit-ln-matrix.py --r23`

---

## Status legend (use in TSP planning)

| Code | On MMS wire (r23) | Live update | Control handler | TSP action |
|------|-------------------|-------------|-----------------|------------|
| **W-L-C** | ✓ | ✓ | ✓ Wlim/WSd | Read + Operate + expect PF2/DIO |
| **W-L-** | ✓ | ✓ | — | Read; values track Modbus/yaml |
| **W--S** | ✓ | — | — | Read static cfg; structure compare OK |
| **W--C** | ✓ | — | struct only | Operate may fail / no plant effect (reactive) |
| **MVP** | ✓ (9 LN path only) | partial | partial | Legacy deploy without `.cfg` |
| **—** | ✗ | — | — | Not in model (`WSaDAGC1` on r23) |

---

## All 31 logical nodes — r23 full CID model

| # | MMS name | lnClass | On wire | Live update source | Control | Boot Mod (cfg) | TSP notes |
|---|----------|---------|---------|-------------------|---------|----------------|-----------|
| 1 | `LLN0` | LLN0 | **W-L-** | URCB integrity 4 s | — | Mod=1 | Enable `urcb_PdC_Mis4sec`; BRCB datasets exist but not driven |
| 2 | `LPHD1` | LPHD | **W--S** | — | — | PhyHealth=Ok | Read-only nameplate |
| 3 | `PdC_WiDPCC1` | DPCC | **W--S** | — | — | WRtg=200 kW | Static plant nameplate |
| 4 | `PdC_WaDPCC1` | DPCC | **W--S** | — | — | WRtg=200 kW | Static |
| 5 | `PdC_QiDPCC1` | DPCC | **W--S** | — | — | VArRtg=50 | Static |
| 6 | `PdC_QcDPCC1` | DPCC | **W--S** | — | — | VArRtg=50 | Static |
| 7 | `PdC_VADPCC1` | DPCC | **W--S** | — | — | VARtg=210 kVA | Matches `plant.smax_kva` |
| 8 | `DisFRDECP1` | DECP | **W--S** | — | — | Beh=on | DG / disconnection — no DI feed yet |
| 9 | `DisFRDGEN1` | DGEN | **W--S** | — | — | Beh=on | Static |
| 10 | `DisFRDSTO1` | DSTO | **W--S** | — | — | Beh=on | Static |
| 11 | `PdCMMXU1` | MMXU | **W-L-** | Modbus 40001/40003 + yaml PPV | — | Beh=on | **Primary TSP reads**; A/BC/CA not updated |
| 12 | `GenPVMMXU1` | MMXU | **W--S** | — | — | Beh=on | TotW=0 static; field PV #### |
| 13 | `GenWiMMXU1` | MMXU | **W--S** | — | — | Beh=on | Static |
| 14 | `GenTerMMXU1` | MMXU | **W--S** | — | — | Beh=on | Static |
| 15 | `GenIdrMMXU1` | MMXU | **W--S** | — | — | Beh=on | Static |
| 16 | `StMMXU1` | MMXU | **W--S** | — | — | Beh=on | Static |
| 17 | `SGGMMXU1` | MMXU | **W--S** | — | — | Beh=on | GenPV(1) group P |
| 18 | `SGGMMXU2` | MMXU | **W--S** | — | — | Beh=on | GenTer(1) group P |
| 19 | `IDGXCBR1` | XCBR | **W--S** | — | — | Pos status-only | DDG breaker — no live Pos feed |
| 20 | `SSGGDGEN1` | DGEN | **W--S** | — | — | GnGrId=1, Health=Ok | Static |
| 21 | `SSGGDGEN2` | DGEN | **W--S** | — | — | GnGrId=2, Health=Ok | Static |
| 22 | `WlimDWMX1` | DWMX | **W-L-C** | Operate + PF2 arbiter | ✓ direct-enhanced | **Mod=5**, Beh=5 | Use **Direct Operate** (lab); CID says SBO |
| 23 | `WSdDAGC1` | DAGC | **W-L-C** | Operate + yaml/PF2 | ✓ direct-enhanced | **Mod=1**, WSptPct=**20** | r23: Mod≠5 at boot (unlike MVP) |
| 24 | `VArSdDVAR1` | DVAR | **W-L-C** | Mod+APC | ✓ P5-R01 | Mod=5 @ baseline | Operate → Modbus Q write (lab FC16) |
| 25 | `PFSPDFPF1` | DFPF | **W--C** | — | ✗ | Mod=5 | Same |
| 26 | `VArVDVVR1` | DVVR | **W--C** | — | ✗ | Mod=5 | Same |
| 27 | `VArVDPMC1` | DPMC | **W--S** | — | ✗ | Beh=off | Q(V) lock-in params |
| 28 | `VArVDPMC2` | DPMC | **W--S** | — | ✗ | Beh=off | Q(V) lock-out |
| 29 | `VArVDECP1` | DECP | **W--S** | — | — | V band 1 | Static |
| 30 | `VArVDECP2` | DECP | **W--S** | — | — | V band 2 | Static |
| 31 | `PFWDPFW1` | DPFW | **W--C** | — | ✗ | Mod=5 | cosφ=f(P) — stub |

### Not in r23 model

| MMS name | lnClass | On wire r23 | On wire MVP only | Notes |
|----------|---------|-------------|------------------|-------|
| `WSaDAGC1` | DAGC | **—** | Mod=5 stub | O.10.3.1 MSD; in CID workbook, not in TR CID |

---

## Summary counts (r23 + model_cfg)

| Category | Count | LNs |
|----------|-------|-----|
| **On MMS wire (browse/read)** | **31** | All CID LNs |
| **Live-updated by ccli** | **4** | LLN0 reports, `PdCMMXU1` (3 DOs), `Wlim`/`WSd` after Operate |
| **Control handlers registered** | **3** | `WlimDWMX1`, `WSdDAGC1`, `VArSdDVAR1` (P5-R01) |
| **Static structure (cfg defaults)** | **23** | All W--S rows in audit table |
| **Reactive — live command (P5-R01)** | **1** | VArSd |
| **Reactive — structure on wire, no plant path** | **3** | PFSP, VArV, PFW |
| **Absent from r23** | **1** | WSa (MVP only) |

---

## TSP Compare Model — what to expect

| Check | r23 full_cid_cfg | mvp_handbuilt |
|-------|------------------|---------------|
| LN count GetNameList | **31 PASS** | **9 PART** |
| LN names vs CID | **MATCH** | 22 MISSING |
| DO tree vs CID | **MATCH** (genconfig from CID) | many MISSING |
| **Values** vs CID `<Val>` | **DIFFER** on live DOs | DIFFER |
| `PdCMMXU1.TotW` | live Modbus | live |
| `WSdDAGC1.Mod` | cfg **1** (r23) vs CID **on** | forced **5** |
| `WlimDWMX1.Mod` | cfg **5** vs CID **off** | **5** |
| Reactive Operate (VArSd) | **OK** — Modbus Q write (P5-R01) | N/A or Mod-only |

**Waive:** value deltas on live DOs; ctlModel SBO in CID vs direct-enhanced in lab.  
**Fail:** LN or DO **missing** from browse (wrong deploy / MVP path).

---

## Pre-TSP verification checklist

```powershell
# 1. Audit table (this doc source)
python scripts/audit-ln-matrix.py --r23

# 2. Deploy + confirm model mode
$env:CCLI_TG544_PW = "<root>"
.\lab\tg544-openwrt\run-p5-fullcircle-cleartext.ps1

# 3. LN browser (must ln_count=31)
.\lab\tg544-openwrt\run-p5-cleartext-ln-capture.ps1
```

Pass before opening Test Suite Pro:

- [ ] DUT log: `model=full_cid_cfg`
- [ ] `ln_count=31` · `VERDICT: PASS`
- [ ] Modbus slave running (P/Q per test plan)
- [ ] TSP: TLS **OFF**, port **102**, CID loaded

---

## Code references

| Mechanism | File | Function |
|-----------|------|----------|
| Load 31 LN `.cfg` | `mms_adapter.cpp` | `create_mms_model()` → `ConfigFileParser_createModelFromConfigFileEx` |
| Live node pointers | `mms_adapter.cpp` | `wire_cfg_runtime_nodes()` — Wlim, WSd, PdC only |
| Control handlers | `mms_adapter.cpp` | `IedServer_setControlHandler` — Wlim + WSd + VArSd (P5-R01) |
| Modbus → MMS | `mms_adapter.cpp` | `update_tot_w_kw`, `update_totvar_kvar`, `update_ppv_kv` |
| MVP 9-LN fallback | `mms_adapter.cpp` | `create_mvp_model()` if `model_cfg` empty |
