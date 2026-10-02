# P5-00 — Logical node matrix audit (CID vs runtime)

**Date:** 2026-09-30  
**References:** `lab_tg544_eth_a.cid` · `signal_map.yaml` · `mms_adapter.cpp` · IEC 61850-7-4 extract  
**Tool:** `scripts/audit-ln-matrix.py`  
**Value matrix (TSP):** `P5_LN_VALUE_MATRIX.md`  
**Annex test sequence:** [P5_ANNEX_TEST_SEQUENCE.md](P5_ANNEX_TEST_SEQUENCE.md) (Part A)  
**Timing / LN alignment:** [P5_TIMING_ANNEX_AUDIT_2026-10-01.md](P5_TIMING_ANNEX_AUDIT_2026-10-01.md) (§2.5)

---

## Naming rule (Annex T / TR 57-126)

MMS object name = **`prefix` + `lnClass` + `inst`**

| Example | prefix | lnClass | inst |
|---------|--------|---------|------|
| `WlimDWMX1` | Wlim | DWMX | 1 |
| `PdCMMXU1` | PdC | MMXU | 1 |
| `VArSdDVAR1` | VArSd | DVAR | 1 |

Namespace: `LLN0.NamPlt.ldNs` = **IEC61850-7-4:2010**; DER control LNs use **61850-7-420** `lnNs` in CID.

---

## Runtime MVP (9 LNs) — `mms_adapter.cpp`

| MMS name | lnClass | CDC (7-3) | Status | Annex / clause |
|----------|---------|-----------|--------|----------------|
| `LLN0` | LLN0 | ENS Mod/Health | **IMPL** | Reports, DataSets |
| `WlimDWMX1` | DWMX | ENC Mod + APC WMaxSptPct | **IMPL** | O.9.2.2 / T Tab. 84 |
| `WSdDAGC1` | DAGC | ENC Mod + APC WSptPct | **IMPL** | O.9.2.3 / T Tab. 85 |
| `WSaDAGC1` | DAGC | ENC Mod (stub) | **STUB** | O.10.3.1 MSD — **runtime only; not in CID** |
| `VArSdDVAR1` | DVAR | ENC Mod (stub) | **STUB** | O.9.1.4 / P5-R01 |
| `PFSPDFPF1` | DFPF | ENC Mod (stub) | **STUB** | O.9.1.1 / P5-R02 |
| `VArVDVVR1` | DVVR | ENC Mod (stub) | **STUB** | O.9.1.3 / P5-R03 |
| `PFWDPFW1` | DPFW | ENC Mod (stub) | **STUB** | O.9.1.2 / P5-R04 |
| `PdCMMXU1` | MMXU | MV TotW/TotVAr + DEL PPV.phsAB | **PART** | T.3.1.3 — A/Beh deferred; PPV phsAB IMPL r22 |

### PdCMMXU1 data objects (vs CID MMXU1)

| DO | CID type | Runtime r22 | Notes |
|----|----------|-------------|-------|
| TotW | MV | **IMPL** | Modbus P → mag.f |
| TotVAr | MV | **IMPL** | Modbus Q → mag.f |
| PPV | DEL (phsAB/BC/CA) | **PART** | phsAB.cVal.mag.f only; lab yaml |
| A | WYE | **DEFERRED** | CID only |
| Beh | ENS | **DEFERRED** | CID only |

### Fixes applied 2026-09-30

1. Stub **Mod** changed from **ENS** → **ENC** (status-only) — matches TR `ENC_*_Mod` templates.
2. **PPV** added as **CDC DEL** (not flat MV) — matches 7-4 / TR `DEL_1_PPV`.

---

## Static-on-wire LNs (23) — r23 full cfg (not live-wired)

**On MMS wire** via `lab_tg544_eth_a.cfg`; ccli does not update from plant sim. See **`P5_LN_WIRE_STATUS.md`**.

Legacy label “CID-only / deferred” applied only to **MVP 9-LN path**. Groups:

| Group | LNs |
|-------|-----|
| Disconnection | `DisFRDECP1`, `DisFRDGEN1`, `DisFRDSTO1` |
| POC nameplate DPCC | `PdC_Wi/Wa/Qi/Qc/VA` DPCC1 |
| Plant MMXU2 | `GenPVMMXU1`, `GenWiMMXU1`, `GenTerMMXU1`, `GenIdrMMXU1`, `StMMXU1`, `SGGMMXU1/2` |
| SSGG | `SSGGDGEN1/2` |
| Breaker | `IDGXCBR1` |
| VArV curve helpers | `VArVDPMC1/2`, `VArVDECP1/2` |
| Physical | `LPHD1` |

---

## Discrepancies to track

| ID | Issue | Mitigation |
|----|-------|------------|
| LN-GAP-01 | `WSaDAGC1` in runtime, absent from TR example CID | Add to plant CID when MSD contract fixed |
| LN-GAP-02 | Hand-built model ≠ full CID (31 LNs) | genconfig / SCL loader (roadmap) |
| LN-GAP-03 | PPV from yaml, not analyzer | P5-06 register map + Modbus |
| LN-GAP-04 | Stub LNs lack APC set-point DOs | P5-R01… when plant Q path exists |
| LN-GAP-05 | MMS Mod/WSpt not seeded from yaml at boot | TSP: Operate before CID compare; see value matrix §B3 |
| LN-GAP-06 | Lab direct-enhanced ctlModel vs CID SBO | TSP Direct Operate |
| LN-GAP-07 | TotW/TotVAr quality questionable until GNSS | **CLOSED** 2026-10-01 — sky lock, gnss_fix=1 |
| LN-GAP-08 | PPV.phsAB.q not refreshed with time_quality | TotW/TotVAr q=Questionable, PPV q=Good stale; see timing audit §2.5.5 |

---

## Verification

```powershell
python scripts/audit-ln-matrix.py
# After LAN1 cable: mms_tls_client → READ OK TotW/TotVAr/PPV.phsAB
```

**Regenerate:** `python scripts/audit-ln-matrix.py`
