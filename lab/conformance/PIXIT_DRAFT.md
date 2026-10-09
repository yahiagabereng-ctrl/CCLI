# PIXIT draft — server extra test information

**Document ID:** CCLI-LAB-PIXIT-001  
**Revision:** 1.0  
**Date:** 2026-10-08  
**RAG source_id:** `ccli-lab-pixit-draft`  
**Normative:** IEC 61850-10 Annex E style · copy into lab **TemplatePixit** Word when added to `lab/`  
**YAML source:** `apps/ccli/config/lab_tr400_phase1_regulation.yaml`  
**CID:** `apps/ccli/config/icd/lab_tg544_eth_a.cid`

Until the lab Word template is in git, this file **is** the HiTEKS PIXIT working copy.

---

## 1. Identification

| Item | Value |
|------|--------|
| IED | `CCI016_01` |
| Access point | `accessPoint1` · Eth_A (LAN1) |
| DUT IP | `192.168.10.1` |
| Product MMS port (TLS) | **3782** |
| Bench cleartext MMS (optional) | **102** — lab yaml `tls_enabled: false` only |
| Firmware ID | `ccli --version` + OpenWrt build / SHA-256 of `ccli` binary |

---

## 2. Association / server limits

| Item | Value | Source |
|------|--------|--------|
| Max simultaneous associations | **4** (engineering; not hard-capped in `mms_adapter.cpp`) | Questionnaire §10 |
| Max RCB clients per report | **2** | CID `RptEnabled max="2"` |
| Association timeout | Client / lab PIXIT; DUT follows TCP + TLS | |
| Abort / release | Supported | sAss |
| DynAssociation | Present in CID `<DynAssociation />` | CID Services |

**Lab day:** confirm whether 61850-10 runs on **102** or **3782** (questionnaire 3.6). Product DSO profile is **3782 + TLS**.

---

## 3. Reporting (URCB) — Table 14 parameters

| Item | Value |
|------|--------|
| Primary URCB | `CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec` |
| Dataset | `DS_R_PdC_Mis4sec` (TotW, TotVAr, PPV, A) |
| **IntgPd default** | **4000 ms** |
| IntgPd min / max (declared) | **1000–60000 ms** |
| TrgOps claimed | **period** (integrity) + **GI** |
| dchg / qchg / dupd | **Not enabled** on PdC URCB — do not test as claimed |
| OptFields | seqNum, timeStamp, dataSet, reasonCode, dataRef, entryID, configRef |
| Buffered reporting | **Not claimed** |
| Online dataset edit / ConfRev | **Static only** (`modify="false"`) |
| Segmentation | Small PdC dataset; no forced multi-segment claim |

Pass window used internally: integrity Δt **3900–4100 ms**.

---

## 4. Control — Table 26 parameters

| Object | ctlModel (CID) | First-pass PICS |
|--------|----------------|-----------------|
| `WlimDWMX1.Mod` | **direct-with-enhanced-security** | **M** |
| `WlimDWMX1.WMaxSptPct` | **direct-with-enhanced-security** | **M** |
| `WSdDAGC1.Mod` | **direct-with-enhanced-security** | **M** |
| `WSdDAGC1.WSptPct` | **direct-with-enhanced-security** | **M** |
| `VArSdDVAR1.*` | direct-with-enhanced-security | **O** (P5-R; optional quote) |
| `PFSP` / `VArV` / `PFW` | **sbo-with-enhanced-security** in CID | **N/A** — do not claim SBO |

Operate path: enhanced-security **Operate** (not direct-with-normal-security).  
Originator / test flag: test=`true` is **rejected** (`on_control`).  
RBAC: VIEWER associate OK; Operate **denied**.

---

## 5. Data / model

| Item | Value |
|------|--------|
| Logical device | `LD_Plant` |
| Name length | 32 (`Services nameLength="32"`) |
| Quality + timestamp | Published on PdC MX; TimeQuality via chrony/GNSS |
| Time accuracy (Annex T) | UTC **≤ ±100 ms** (`chronyc tracking`) |
| Time protocol | SNTP/NTP client (chrony); GNSS optional; **not** PTP 9-3 |

---

## 6. Security / TLS (also D7)

| Item | Value |
|------|--------|
| TLS | **1.2** minimum (`TLS_VERSION_TLS_1_2`) |
| Mutual auth | Required (client cert allow-list) |
| Cert | RSA **2048**; max **8192** octets |
| CA | Lab EJBCA root |
| Dual cert (Annex T G.2) | TLS = `server_tls.pem` · ACSE = `server.pem` |
| CRL | Loaded when `tls_crl` set (`lab_crl.pem`) |
| OCSP | **Not claimed** in first 62351-3 PID (optional) |
| Cipher intent | ECDHE / AES-GCM / SHA-256 (OpenWrt TLS stack) |

---

## 7. Application behaviour (CEI — declare so lab does not treat as 61850 fail)

| Item | Value |
|------|--------|
| DSO comms-loss fallback | **15 s** (`mms.comms_loss_fallback_s`) then clear live Wlim/WSd/VArSd |
| Eth_B 104 | **Out of 61850 PIXIT**; see `iec104_operator_rule.yaml` monitor_only |

---

## 8. Stimulus

| Item | Value |
|------|--------|
| Measurement source | Modbus RTU mock (`modbus_rtu_slave.py`) or plant meter |
| Physical I/O | Optional; Wlim actuation via `ubus call dido_v2 status` |

---

**RAG tags:** `lab`, `PIXIT`, `61850-10`, `ccli-lab-pixit-draft`
