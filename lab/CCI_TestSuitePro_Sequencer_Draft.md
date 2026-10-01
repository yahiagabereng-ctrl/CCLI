# CCLI — TSP Test Sequencer draft (offline, no Connect)

**Document ID:** CCLI-LAB-TSP-SEQ-001  
**Revision:** 1.0  
**Date:** 2026-09-26  
**Workspace (suggested):** `CCLI_OFF_SCL` → save as / clone to `CCLI_P3_SEQ` when ready  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid` (rev 3) · IED `CCI016_01`  
**Procedure:** `lab/CCI_TestSuitePro_Verification_Layer.md` §A.3–A.4  
**Do not Connect** until DUT + PC NIC are up — build steps offline only.

---

## 1. Connection profile (configure now, connect later)

| Field | Value |
|-------|--------|
| IED | `CCI016_01` |
| Access Point | `accessPoint1` |
| IP | `192.168.10.1` |
| Port | **3782** (lab TLS — not CID default 102) |
| Transport | **TLS** (62351-3 T-profile) |
| Trust / CA | `apps/ccli/config/tls/root_CA.pem` |
| Client cert (DSO write) | `client.pem` + `client.key` |
| Viewer-only (negative / read) | `viewer.pem` + `viewer.key` |
| Auth | Mutual TLS (lab P3-07) |

Stage PEMs on PC:

```powershell
cd c:\Yahia\projects\CCLI
powershell -File scripts\stage-tsp-tls.ps1
```

Default stage dir: `C:\ProgramData\Triangle MicroWorks\61850 Test Suite Pro\CCLI_tls\`

In TSP: System Status / IED Connection Configuration → point TLS trust + client identity at that folder. Leave **Disconnected** until Phase B.

---

## 2. Sequencer steps (build in Test Sequencer tool)

Vendor help: `help-offline/Content/Use test sequencer to test.htm` · sidebar **Test Sequencer**.

Use SBO if TSP requires Select-before-Operate for `sbo-with-enhanced-security` (ctlModel after r2 fix). Lab `permissive_bypass` may still accept direct Oper — prefer **SBO** for product-honest evidence.

| # | Gate | Action | Object / params | Pass criteria (when live) | Evidence name |
|---|------|--------|-----------------|---------------------------|---------------|
| 0 | — | Comment | `CCLI P3 min sequence · Wlim+WSd · fallback 15 s` | — | — |
| 1 | P3-01 | **Connect** | `192.168.10.1:3782` TLS · `client.pem` | Assoc OK · LD `LD_Plant` visible | `TSP_P3_01_CONNECT_*.log` |
| 2 | P3-03 | **EnableReport** | `CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec` · RptEna=true · IntgPd=4000 | Reports ~every 4 s · TotW present | `TSP_P3_03_REPORT_TotW_*.csv` |
| 3a | P3-04 | **Operate** (SBO) | `…/WlimDWMX1.Mod` · ctlVal=**1** (Active) | Mod.stVal=1 | `TSP_P3_04_OPERATE_Wlim_*.txt` |
| 3b | P3-04 | **Operate** (SBO) | `…/WlimDWMX1.WMaxSptPct` · ctlVal=**70** (% Smax) | Server `mms→pf2` Wlim@70% | same |
| 4a | P3-04 | **Operate** (SBO) | `…/WSdDAGC1.Mod` · ctlVal=**1** | Mod.stVal=1 | `TSP_P3_04_OPERATE_WSd_*.txt` |
| 4b | P3-04 | **Operate** (SBO) | `…/WSdDAGC1.WSptPct` · ctlVal=**20** | WSd@20% · Figura 2 | same |
| 5 | P3-05 | **Disconnect** | — | Assoc closed | `TSP_P3_05_FALLBACK_*.txt` |
| 6 | P3-05 | **Wait** | **15 s** (`comms_loss_fallback_s`) | After wait: DSO inactive / PF2 fallback (DUT log + `ubus call dido_v2 status`) | same |

### MMS references (copy into TSP)

```text
IED:     CCI016_01
LD:      LD_Plant
URCB:    CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec
Wlim:    CCI016_01LD_Plant/WlimDWMX1.Mod
         CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct
WSd:     CCI016_01LD_Plant/WSdDAGC1.Mod
         CCI016_01LD_Plant/WSdDAGC1.WSptPct
```

`signal_map.yaml`: Mod **1=Active, 5=Inactive**; WMaxSptPct / WSptPct = % of plant **Smax** (lab 210 kVA → WSd 20% = 42 kW).

### Outside TSP (actuation)

After steps 3–4, on DUT:

```sh
ubus call dido_v2 status
logread | grep -E 'mms→pf2|Wlim|WSd|DO curtail'
```

---

## 3. Optional negative steps (same workspace, separate run)

| # | Gate | Action | Expect |
|---|------|--------|--------|
| N1 | P3-06 | Connect with **no** client cert | Fail handshake |
| N2 | P3-08 | Connect with **viewer** cert · try Operate Wlim | Assoc OK · Operate **denied** |
| N3 | P3-09 | Connect with **revoked** cert | Fail (CRL) |

---

## 4. Save / export

1. Save sequencer in workspace `CCLI_P3_SEQ` (ProgramData Workspaces).  
2. Optional export / screenshot → `lab/evidence/testsuite-pro/inbox/TSP_OFF_SEQ_2026-09-26.*`  
3. Do **not** hit Run/Connect until Phase B checklist below is green.

---

## 5. Phase B go / no-go (before first Connect)

| Check | Ready when |
|-------|------------|
| CID rev 3 in workspace | SCL Verify r2 disposition accepted |
| PC NIC | `192.168.10.x/24` |
| DUT `ccli` | Listening `192.168.10.1:3782` TLS |
| PEMs on DUT | `/etc/ccli/tls/` (server + CA + client allowlist) |
| PEMs on PC | `stage-tsp-tls.ps1` run · TSP profile set |
| Sequencer steps 1–6 | Built offline |

---

## Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-09-26 | Initial offline sequencer + TLS stage after SCL Verify r2 |
