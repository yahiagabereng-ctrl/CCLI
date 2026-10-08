# P3-07 Test Suite Pro — live connect handoff (team)

**Status:** **PASS** on TG544 lab DUT · **2026-10-08**  
**Package:** `ccli-0.1.0-r51.apk` · tag **`P3-07-AARE-SIGN-RAW-GT`**

Triangle MicroWorks **61850 Test Suite Pro** reaches **Connected** against the CCLI MMS server on **Eth_A** (`192.168.10.1:3782`) with **TLS + MMS Security (62351-4 A-profile)** and **DSO_OPERATOR RBAC**.

Detailed evidence: [`lab/evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md`](evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md).

---

## What shipped in GitHub (this release)

| Area | Files / notes |
|------|----------------|
| **AARE ACSE** | `apps/ccli/third_party/libiec61850/src/mms/iso_acse/acse.c` — tags **0x88/0x89/0xAA**, 62351-4 OID, responding AP/AE |
| **Session LI** | `iso_session.c` — extended LI + PGI 194 for ~1.4 kB ACCEPT |
| **Presentation** | `iso_presentation.c` — tolerate extra mmsAuthenticationAS context |
| **ACSE RBAC** | `mms_acse_auth.cpp` — scan embedded X.509; **BER long-length fix** (r49) |
| **AARE auth body** | `mms_aare_auth.cpp` — **EXTERNAL a2 + ref 5** (r50); **sign raw GT** for TSP (r51) |
| **Version** | `apps/ccli/VERSION` + `package/ccli/Makefile` **`PKG_RELEASE:=51`** |

Build TG544 APK:

```bash
wsl bash lab/tg544-openwrt/build-ccli-ipk-sdk.sh
# → lab/tg544-openwrt/ccli-0.1.0-r51.apk
```

Deploy (lab password via env):

```powershell
$env:CCLI_TG544_PW = "..."
# upload APK, then on DUT:
# apk add --allow-untrusted --force-overwrite /tmp/ccli-r51.apk
# killall ccli; single instance on 3782
```

---

## TSP PC checklist (unchanged)

| Setting | Value |
|---------|--------|
| IED | `CCI016_01` @ `192.168.10.1:3782` |
| MMS Security | `client.pem` + `client_tsp.key` |
| Transport TLS | `client_tls.pem` + `client_tsp.key` |
| CA | `root_CA.pem` (stage with `scripts/stage-tsp-tls.ps1` → `C:\CCLI_product_tls\`) |

CRL warnings in TSP are OK for lab (`crl=off` on DUT).

---

## DUT checklist

| Item | Value |
|------|--------|
| Config | `/etc/ccli/lab.yaml` |
| Model | `/etc/ccli/icd/lab_tg544_eth_a.cfg` |
| TLS server | `server_tls.pem` + `server.key` |
| AARE auth (G.2) | `server.pem` + `server_acse.key` |
| ACSE client allow | `client.pem` (G.6.2) + `client_tls.pem` (TLS allowlist) |

**Operational:** only **one** `ccli` must listen on `:3782` (`killall ccli` before manual start if procd fights).

---

## Pass criteria (log + UI)

**TSP:** `Initiate Response Success`, status **Connected**, ReadResponse traffic.

**DUT (`logread` or `/tmp/ccli_r51.log`):**

```text
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=938 octets
mms: AARE responder auth [10]EXTERNAL ctx=5 inner=1215 octets
mms: iso session ACCEPT extended LI (spdu=1395 payload=1375)
```

---

## Failure ladder (debug order)

1. **Association timeout, no ACCEPT** — session extended LI / duplicate `ccli` / port not listening.  
2. **`ACSE auth REJECT` @ 938 B** — RBAC BER scan (need **r49+**).  
3. **Initiate timeout (no signature error)** — AARE EXTERNAL wrapper (need **r50+**).  
4. **`Unable to verify signature`** — raw GT signing (need **r51**); confirm embedded cert is **`server.pem`**, not TLS cert.  

Wire tools: `lab/evidence/testsuite-pro/pcap/wireshark_tls_decrypt/analyze_test11_r44.py`, `analyze_test7_r42.py`.

---

## Next tests (same session)

- **P3-03 / P3-04 / P3-05** — live MMS / operating rule / fallback with TSP connected.  
- Optional: capture + decrypt for regression archive (Npcap admin on PC).

---

## Contacts / traceability

| ID | Requirement |
|----|-------------|
| REQ-P3-07-001 | TSP mutual TLS + MMS Security association |
| REQ-P3-07-002 | DSO_OPERATOR RBAC on AARQ |
| REQ-P3-07-003 | Normative AARE + InitiateResponse accepted by TSP |

Prior triage notes remain under `lab/evidence/testsuite-pro/inbox/TSP_P3_07_*`.
