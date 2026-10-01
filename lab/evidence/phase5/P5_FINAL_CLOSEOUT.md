# P5 — Phase closeout (lab evidence chain)

**Date:** 2026-09-30  
**Gate:** P5_FULLCIRCLE  
**DUT:** TesPro TG544 @ `192.168.10.1`  
**Product build:** **0.1.0-r24** (P5-R01)  
**Lab profile:** cleartext MMS `:102` · `lab_tr400_cleartext_tsp.yaml`  
**Operator UI:** Test Suite Pro (Advanced Client, Report Viewer, Compare Model)

---

## Requirements closed

| ID | Requirement | Evidence | Status |
|----|-------------|----------|--------|
| REQ-P5-001 | Full CID (31 LN) on MMS wire | `P5_LN_WIRE_STATUS.md`, ln browser | **PASS** |
| REQ-P5-002 | Live POC measurements (TotW, TotVAr, PPV) | TSP steps 2–4, automated client | **PASS** |
| REQ-P5-003 | URCB integrity ~4 s | Report Viewer / `mms_lab_client` | **PASS** |
| REQ-P5-004 | Wlim Direct Operate → PF2 + DIO | steps 8, Wlim client, DUT log | **PASS** |
| REQ-P5-005 | WSd structure + Operate path | step 9, cfg Mod=1 | **PASS** |
| REQ-P5-006 | VArSd reactive command (P5-R01) | step 11, `mms_varsd_client`, FC16 | **PASS** |
| REQ-P5-007 | Offline SCL Verify (blocking errors cleared) | `TSP_OFF_SCLVERIFY_*_TRIAGE.md` | **PASS** |
| REQ-P5-008 | Compare Model structure vs CID | waivers doc | **PASS** (waived values) |

## Deferred (non-blocking for P5 lab gate)

| Item | Reason | Track |
|------|--------|-------|
| PFSP / VArV / PFW reactive modes | Annex T Tab.88–91 out of P5-R01 scope | P6+ |
| GenPV / Sun2000 live values | Modbus plant master not wired | plant slice |
| GNSS-locked time quality | chrony lab; q=Questionable waived | LN-GAP-07 |
| Product TLS `:3782` live TSP session | cleartext lab bypass used | revert below |
| TSP steps 13–14 (P sweep, disconnect fallback) | manual — operator optional | sequencer |

---

## Automated evidence (r24 deploy)

| Artifact | Path |
|----------|------|
| Full circle log (canonical) | `lab/evidence/phase5/P5_FULLCIRCLE_CLEARTEXT.txt` |
| Deploy manifest | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r24_*.md` |
| TSP sequencer | `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_SEQUENCER.md` |
| TSP export (automated + manual summary) | `lab/evidence/testsuite-pro/inbox/TSP_P5_FULLCIRCLE_CLEARTEXT_2026-09-30_211654.txt` |
| Compare waivers | `lab/evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md` |
| LN matrix | `lab/evidence/phase5/P5_LN_VALUE_MATRIX.md` |
| Wire status | `lab/evidence/phase5/P5_LN_WIRE_STATUS.md` |

### Key r24 validation snippets

```text
ccli 0.1.0-r24 (P5-R01)
mms: listening on 192.168.10.1:102 tls=off model=full_cid_cfg
PdCMMXU1.TotW = 450 kW · TotVAr = 45 kvar · PPV.phsAB = 20 kV
OPERATE OK VArSdDVAR1.VArTgtSptPct = 10 · Mod = 1
modbus: FC16 write Q=21.00 kvar @ holding 40003
mms→plant: VArSd on @10% Smax → Q=21 kvar [O.9.1.4 P5-R01]
```

---

## Revert to product profile

When lab TSP session is complete:

```powershell
$env:CCLI_TG544_PW = '000000'
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b.yaml `
  -Changes @("revert TLS :3782 product profile")
```

Product MMS: `192.168.10.1:3782` TLS **ON** · client certs in `apps/ccli/config/tls/`.

---

## Verification summary

| Requirement | Test | Pass criteria | Result |
|-------------|------|---------------|--------|
| REQ-P5-001 | GetNameList / ln browser | 31 LNs | **PASS** |
| REQ-P5-002 | Read TotW/TotVAr/PPV | ≈450 / ≈45 / 20 kV | **PASS** |
| REQ-P5-003 | URCB 4 s | IntgPd=4000, ~4 reports | **PASS** |
| REQ-P5-004 | Wlim Operate 10% | DIO curtail, pf2 threshold | **PASS** |
| REQ-P5-006 | VArSd Operate 10% | Q=21 kvar FC16 + log | **PASS** |
| REQ-P5-007 | SCL Verify | 5 known errors waived | **PASS** |

**P5 lab gate: CLOSED** (2026-09-30, build 0.1.0-r24).
