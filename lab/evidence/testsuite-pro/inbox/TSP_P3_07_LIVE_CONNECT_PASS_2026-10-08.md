# TSP P3-07 — Live association pass (2026-10-08)

**Result:** **PASS** — Test Suite Pro **Connected**, **Initiate Response Success**, ongoing **ReadResponse** traffic.

| Item | Value |
|------|--------|
| **Date / time (TSP log)** | 2026-10-08 ~16:41:30 local (PC `192.168.10.10`) |
| **IED (TSP workspace)** | `CCI016_01` (UI also showed `CC1016_01` variant spelling) |
| **DUT** | TG544 `192.168.10.1:3782` (Eth_A lab) |
| **Edition** | Edition_2 |
| **Phase** | P3-07 MMS Security + TLS mutual auth + RBAC |

## Deployed software

| Artifact | Value |
|----------|--------|
| **OpenWrt package** | `ccli-0.1.0-r51.apk` |
| **PKG_RELEASE** | `51` (`package/ccli/Makefile`) |
| **Build tag** | `P3-07-AARE-SIGN-RAW-GT` (`apps/ccli/VERSION` line 3) |
| **Semver** | `0.1.0` |
| **Platform** | `platform_tg500` (TesPro TG544) |
| **Config** | `/etc/ccli/lab.yaml` |
| **ICD / model** | `/etc/ccli/icd/lab_tg544_eth_a.cfg` (full CID) |

**Note:** At pass time, `ccli --version` on DUT reported `release: r48` because `apps/ccli/VERSION` line 2 lagged `PKG_RELEASE`; the running binary came from **`ccli-0.1.0-r51.apk`**. Line 2 synced to **51** in repo after this evidence file.

## TSP configuration (lab)

| Layer | Material |
|-------|----------|
| **MMS Security** | `client.pem` + `client_tsp.key` |
| **Transport TLS** | `client_tls.pem` + `client_tsp.key` |
| **CA** | `root_CA.pem`, depth 1, directory-to-CA empty |
| **Staging (PC)** | `C:\CCLI_product_tls\` (from `scripts/stage-tsp-tls.ps1`) |

CRL warnings in TSP output (“No valid Certificate Revocation files…”) are **informational** for this lab (`crl=off` on DUT).

## TSP pass criteria (observed)

1. System Status: **Connected** (green).
2. Output: `Initiate Response Success: SUCCESS`.
3. MMS reads succeed (multiple `ReadResponse` lines, e.g. `LLN0`, `GO`, `Phd` logical nodes).
4. No `Association Timeout`, no `Initiate Response Failed`, no `Unable to verify signature` on this session.

## DUT log correlate (`/tmp/ccli_r51.log`)

Association establishment (successful path):

```text
mms: listening on 192.168.10.1:3782 tls=on acse_tls_auth=on rbac=on crl=off ...
mms: client CONNECT count=1
mms: iso session CONNECT (payload=1456)
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=938 octets
mms: AARE responder auth [10]EXTERNAL ctx=5 inner=1215 octets
mms: iso session ACCEPT extended LI (spdu=1395 payload=1375)
```

No immediate `client DISCONNECT` after ACCEPT on this connect — session remained up for TSP polling.

## Fix stack in this build (r44 → r51 summary)

| Release | Change |
|---------|--------|
| r44+ | AARE ACSE tags **0x88 / 0x89 / 0xAA**; 62351-4 mechanism OID; responding AP/AE titles |
| Session | Inbound/outbound **extended LI** + PGI 194 for ~1.4 kB AARE |
| r49 | ACSE RBAC **BER long-length** fix (`mms_acse_auth.cpp`) — accept 938 B client auth blob |
| r50 | AARE auth **EXTERNAL [2] (a2)**, indirect-ref **5**, nested **0xa0** (mirror TSP AARQ) |
| **r51** | AARE signature over **raw GeneralizedTime** octets (TSP verify path); wire time still **0x81+GT** |

## Source touchpoints

- `apps/ccli/adapters/iec61850_mms/mms_acse_auth.cpp` — RBAC scan
- `apps/ccli/adapters/iec61850_mms/mms_aare_auth.cpp` — EXTERNAL wrapper + raw-GT sign
- `apps/ccli/third_party/libiec61850/src/mms/iso_acse/acse.c` — AARE encoder
- `apps/ccli/third_party/libiec61850/src/mms/iso_session/iso_session.c` — extended LI

## Verification / next

| Requirement | Test | Pass |
|-------------|------|------|
| REQ-P3-07-001 | TSP TLS + MMS Security connect to lab DUT | **PASS** (this session) |
| REQ-P3-07-002 | DUT RBAC DSO_OPERATOR on AARQ cert | **PASS** (log) |
| REQ-P3-07-003 | DUT emits parseable AARE + InitiateResponse | **PASS** (TSP initiate + reads) |

**Next:** P3-03 / P3-04 / P3-05 live tests on the same association; optional wire capture + decrypt for AARE gate archive.

## Evidence artifacts

- TSP screenshot: user capture 2026-10-08 ~16:41 (Connected + Initiate Response Success + ReadResponse).
- DUT log pull: `TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08_DUT_LOG.txt` (same inbox).
- Prior failures for traceability: `TSP_AssocTimeout_2026-10-08_DUT_LOG.txt`, `TSP_CONNECT_ATTEMPT_2026-10-08_DUT_LOG.txt`.
- APK on build host: `lab/tg544-openwrt/ccli-0.1.0-r51.apk`.
