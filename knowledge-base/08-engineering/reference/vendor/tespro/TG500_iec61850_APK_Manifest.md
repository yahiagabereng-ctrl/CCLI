# TesPro TG500 — IEC 61850 opkg/APK bundle manifest

**Document ID:** CCLI-VENDOR-TESPRO-61850-APK-001  
**Date:** 2026-09-29  
**RAG source_id:** `ccli-tespro-61850-apk-manifest`  
**Parent:** `ccli-tespro-61850-manual-extract` · V-005  
**Supplier delivery folder (user PC):**  
`OneDrive\…\rmsg\file\2026-09\TG500_iec61850_apk_install\`

---

## Important naming

| Name | What it is |
|------|------------|
| **TesPro** (this document) | Gateway **vendor** — opkg/APK packages on TG544 OpenWrt |
| **Test Suite Pro (TMW)** | **Triangle MicroWorks** conformance tester on lab PC — **not** these APKs |
| **`apps/ccli`** | HiTEKS **DSO MMS server** (62351-3/4 on Eth_A :3782) — **separate** from TesPro packages |

These APKs are **not** the source of Test Suite Pro. They are **TesPro add-on firmware packages** for northbound IEC 61850 **data collection**.

---

## Package list (2026-09 delivery)

| File | Size (approx) | Role | Corpus |
|------|---------------|------|--------|
| `libiec61850-1.5.2-r1.apk` | 243 KB | IEC 61850 / MMS **library** (vendor build) | **HAVE** (manifest) |
| `libopen62541-1.3.6-r2.apk` | 594 KB | OPC-UA stack dependency | **HAVE** (manifest) |
| `iec61850-mmsd-1.0.0-r3.apk` | 31 KB | MMS **client/collector daemon** | **HAVE** (manifest) |
| `iec61850-proto-tespro-combined-1.0.0-r*.apk` | 136 KB | Web UI + collection + MQTT/TCP upload | **HAVE** (manifest) |

Install order on device (typical): `libopen62541` → `libiec61850` → `iec61850-mmsd` → `iec61850-proto-tespro-combined`.

---

## Architecture (from manual + package names)

```text
Remote IED(s)  --MMS/TCP:102-->  iec61850-mmsd (TesPro daemon)
                                      |
                                      v
                         iec61850-proto-tespro-combined (web UI)
                                      |
                                      v
                         MQTT / TCP northbound (optional TLS)
```

**Not in these packages:**

- DSO-facing MMS **server** on Eth_A
- Port **3782** / 62351-3 Transport TLS for DSO
- 62351-4 A-profile **AARQ/AARE** certificate authentication
- CEI Allegato T `Wlim` / operator control path

That remains **`apps/ccli`** on the product.

---

## Version note vs CCLI tree

| Component | TesPro APK | CCLI `apps/ccli` |
|-----------|------------|------------------|
| libiec61850 | **1.5.2** (vendor opkg) | **1.6** fork under `third_party/libiec61850` (P3-07 patches) |
| MMS role | **Client** (poll IEDs) | **Server** (DSO associate) |
| 62351-4 AARE signer | **Not present** | `mms_aare_auth.cpp` + patched `acse.c` |

Vendor libiec61850 **1.5.2** upstream also has **no** §11.2.2 AARE responder auth — same gap as mz-automation 1.6.

---

## P3-07 / Test Suite Pro relevance

| Question | Answer |
|----------|--------|
| Do these APKs fix TSP signature failure? | **No** — different stack, different role |
| Should TMW ticket mention these? | **Optional** — clarify DUT is **`ccli`**, not TesPro `iec61850-mmsd` |
| Can TesPro packages run alongside `ccli`? | **OPEN** — port/resource conflict on :102 vs :3782 unlikely; CPU/RAM coexistence **TBD** (V-005) |

---

## Recommended repo action

Copy APKs to (optional, binary):

`knowledge-base/08-engineering/reference/vendor/tespro/apk/TG500_iec61850_apk_install/`

Extract `.so` / binaries for version strings:

```bash
# On device after opkg install, or unpack APK (zip):
opkg info libiec61850
ls -la /usr/lib/libiec61850*
```

---

## Traceability

| ID | Requirement | These APKs | Owner |
|----|-------------|------------|-------|
| REQ-3514-APROF | 62351-4 A-profile server | **Not met** | `apps/ccli` |
| REQ-VEND-008 | Vendor 61850 documented | **PARTIAL** — collector only | TesPro manual + this manifest |
| V-005 | Coexistence vendor 61850 + ccli | **OPEN** | lab |
