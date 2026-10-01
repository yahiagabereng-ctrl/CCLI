# MMS multi-tool verification matrix — session record

**Date:** YYYY-MM-DD  
**Operator:**  
**DUT:** TG544 `192.168.10.1:3782` · yaml: `lab_tr400_phase1_regulation.yaml`  
**PC:** 192.168.10.10  
**Procedure:** `lab/CCI_MMS_MultiTool_Verification.md`

---

## Results summary

| Tool | Result | Evidence file |
|------|--------|---------------|
| libIEC61850 `mms_tls_client` | PASS / FAIL | |
| libIEC61850 `mms_tls_wlim_client` | PASS / FAIL | |
| Test Suite Pro Connect | PASS / FAIL | |
| TMW IED Simulator (Sample Security) | PASS / FAIL / N/A | |
| Omicron IEDScout | PASS / FAIL / N/A | |
| Wireshark TLS + frame notes | PASS / FAIL | |
| `locate_p3_07_failure.py` | DUT OK / DUT FAIL | |

---

## Phase 0 — DUT

```
(paste netstat + ccli log excerpt)
```

---

## Phase A — libIEC61850

```
(paste client output)
```

---

## Phase B — Test Suite Pro

**TSP version:**  
**Error text (if any):**

```
(paste System Status / Execution log)
```

**pcap:** `lab/evidence/testsuite-pro/pcap/...`

---

## Phase C — IED Simulator

**SCL used:**  
**Simulator IP:port:**  
**Use TMW Sample Security:** yes/no  
**TSP Connect result:**

---

## Phase D — IEDScout

**Licence / version:**  
**TLS config:**  
**Result:**

---

## Phase E — Wireshark

| Frame # | Description |
|---------|-------------|
| | Client Hello |
| | Server Hello |
| | AARQ (app data) |
| | AARE (app data) |
| | Alert (if fail) |

**TLS decrypt:** yes/no  
**Initiate Request seen:** yes/no

---

## Decision (see matrix in procedure doc)

**Verdict:** DUT A-profile delta / TSP issue / DUT down / GREEN

**Next action:**
