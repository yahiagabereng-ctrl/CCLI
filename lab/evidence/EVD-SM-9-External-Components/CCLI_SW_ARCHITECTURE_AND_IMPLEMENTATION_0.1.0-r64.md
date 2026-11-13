# CCLI application — software architecture, implementation, and SBOM (0.1.0-r64)

**Document ID:** CCLI-SM9-APP-ARCH-001  
**Date:** 2026-11-13  
**Revision:** 1.0  
**Maps to:** IEC 62443-4-1 **SM-9** · **SVV-3** · CR **7.8** · `ZCR-1-SUC-TR400.md` §6  
**Pinned release:** **0.1.0-r64** (`P5-EMT432-CMV-ANG-URCB-PF1`)  
**Repo git (handoff):** see handoff commit SHA  
**Canonical narrative:** `knowledge-base/06-application/CCI_Application_Structure.md` (rev 1.3 — update when merging trees)

---

## Requirements

| ID | Requirement |
|----|-------------|
| **REQ-SM9-001** | Maintain **compile-time** SBOM for HiTEKS `ccli` including vendored stacks and platform-linked libs. |
| **REQ-SM9-002** | Maintain **runtime** platform inventory (TesproOS apk) separate from app SBOM. |
| **REQ-SW-001**…**005** | Version, build, deploy traceability — `lab/tg544-openwrt/CCLI_SW_VERSION_AND_BINARIES.md` |
| **REQ-ARCH-001** | Default product entry **`program/main.c`** (SK0153); legacy `ccli_main.cpp` only for bisect. |

---

## Architecture

### Functional (runtime)

```text
program/main.c  →  Boot / Startup / Main_TaskMain (cooperative tick)
       │
       ├── parser_modbus / regulation / f_pf2 / transmission_*  (C modules)
       └── ccli_bridge.cpp  →  svc_*  →  adapters  →  core
```

| Service | Adapter | Protocol / IO | Default lab config |
|---------|---------|---------------|-------------------|
| `svc_mms` | `iec61850_mms` | MMS **:3782** TLS + ACSE | `product_tr400_p3_07_connect_5ln.yaml` |
| `svc_modbus` | `modbus_master` | Modbus TCP → EMT432 (plant LAN) | Chronos map + commissioning |
| `svc_pf2` | — | CEI 0-16 PF2 FSM | `core/pf2` |
| `svc_goose` | `iec61850_goose` | L2 GOOSE (plant) | **OFF** (`CCLI_WITH_GOOSE=OFF`) |
| `transmission_104` | `iec60870_104` | CS104 **:2404** TLS | Eth_B operator |
| `svc_io` | `io/drv_gpio` | DI/DO via HAL | `platform_tg500` |

### Software layering (build)

| Layer | CMake target | Path |
|-------|--------------|------|
| **Program** | `ccli_program` | `apps/ccli/program/` |
| **Services** | `ccli_services` | `apps/ccli/services/` |
| **Adapters** | `ccli_adapters` | `apps/ccli/adapters/` |
| **Core** | `ccli_core` | `apps/ccli/core/` |
| **Platform HAL** | `ccli_platform` | `apps/ccli/platform/platform_tg500/` |
| **Executable** | `ccli` | links `ccli_program` + libs |
| **Product mirror** | same layout | `product/apps/ccli/` (promote / SK0153) |

**Lab vs product:** Single **build root** `apps/ccli/` (program merged 2026-10). Product tree is the **cert promotion copy**; sync via workspace scripts. Lab bisect: `-DCCLI_LEGACY_MAIN=ON` → `services/ccli_main.cpp`.

### Network / security (app scope)

- MMS and 104 **TLS required** for product yaml (`product_tr400.yaml` / P3-07 bench yaml).
- Zone isolation is **platform fw4** — see `lab/evidence/P2_fw4_NORMATIVA_COVERAGE_MATRIX.md`.
- Lab PKI under `config/tls/` — Trivy secret disposition in `lab/evidence/trivy/CCLI-TRIVY-FINDINGS-DETAIL-0.1.0-r64.json`.

### Power

N/A at application layer (hardware PSU in Architecture HW docs).

---

## Version pin (accurate as of 2026-11-13)

| Item | Value | Evidence |
|------|--------|----------|
| **App semver / release** | **0.1.0-r64** | `apps/ccli/VERSION` |
| **Codename** | P5-EMT432-CMV-ANG-URCB-PF1 | same |
| **Staging ELF SHA256** | `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` | `CCLI_DEPLOY_LATEST.md` |
| **APK (lab package)** | `ccli-0.1.0-r64.apk` | deploy manifest |
| **DUT running binary** | **0.1.0-r64** (same SHA) | `intake/20261112-104506/dut-platform-identity.txt` |
| **DUT apk metadata** | `ccli-0.1.0-r51` on image | `platform-apk-list-installed.txt` — **reconcile on next ipk bump** |
| **Platform OS** | TesproOS **32-2.1.0** `r601-af70fcaaa2` | dut-platform-identity |
| **Deploy manifest git** | `cfa34dd` (build tree pin in LATEST) | `CCLI_DEPLOY_LATEST.md` |

---

## Interface matrix

| Interface | Source | Destination | Protocol / port |
|-----------|--------|-------------|---------------|
| Eth_A DSO | Test Suite Pro / SCADA | `ccli` MMS server | IEC 61850 MMS **3782/TCP** TLS |
| Eth_B operator | DCC / logger | `ccli` 104 | IEC 60870-5-104 **2404/TCP** TLS |
| Plant LAN3 | EMT432 meter | `ccli` Modbus master | Modbus TCP **502** |
| LTE WAN | Backup | `wan` (not ccli) | Policy routing — fw4 |
| DI/DO | Plant wiring | GPIO HAL | Digital I/O |
| Config | `/etc/ccli/lab.yaml` | `ccli_config` | YAML |

---

## BOM matrix (software)

| Block | Function | Version / ID | Status |
|-------|----------|--------------|--------|
| **ccli** (HiTEKS) | CCI firmware | **0.1.0-r64** | **HAVE** — deployed |
| **libiec61850** | MMS stack | **v1.6.0** (git tag) | **HAVE** static |
| **lib60870** | 104 stack | **v2.3.5** (git tag) | **HAVE** static |
| **mbedtls** | TLS for MMS/104 | **3.6.0** (vendored) | **HAVE** static |
| **libmodbus** | Modbus master | **3.1.12-r1** (DUT apk) | **HAVE** platform link |
| **TesproOS** | OS + 302 packages | 32-2.1.0 r601 | **HAVE** intake CDX |
| **Vendor signed SBOM** | Image bundle | — | **MISS** (SM-10) |

**Machine-readable app SBOM:** `CCLI_APP_SBOM_0.1.0-r64.cdx.json`  
**Regenerate:** `python scripts/sbom/generate-ccli-app-sbom.py`

**Platform runtime SBOM proxy:** `intake/20261112-104506/platform-runtime-inventory.cdx.json`

---

## Implemented features (r64 scope)

| ID / theme | Implementation | Path / notes |
|------------|----------------|--------------|
| **P3-07 Connect** | 5-LN CID, TLS MMS, TSP profile | `config/product_tr400_p3_07_connect_5ln.yaml` |
| **EMT432 / Chronos** | Modbus map + commissioning + ang URCB | `modbus_emt432_*`, `chronos_emt432_map.yaml` |
| **PF2** | FSM + regulation hooks | `core/pf2/`, `program/f_pf2.c` |
| **O.14 events** | Event store, syslog hooks | `core/event/` |
| **Cyber gate** | Refuse cleartext MMS/104 | `product_cyber_gate.hpp`, startup |
| **Zone dashboard** | Lab CGI (not product ship) | deploy scripts / lab only |
| **GOOSE plant** | Adapter present, **build OFF** for 5-LN cert | CMake `CCLI_WITH_GOOSE=OFF` |
| **GNSS time** | Adapter stub / integration point | `gnss_time.cpp` |
| **DSO mock** | Phase1 active/reactive | `core/dso/` |

**SCA evidence:** `lab/evidence/trivy/EVD-SCA-CCI-r64-Trivy.md` — **0 blocking CVE** on shipping ELF.

---

## Knowledge gaps

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| Vendor OS SBOM | apk intake CDX | TesPro signed `.bom.cdx.json` r601 | **MISS** |
| apk vs binary version | r51 apk name / r64 ELF | Align ipk version on deploy | **PART** |
| Formal SM-9 SOP | This pack + record | Controlled procedure doc | **PART** |
| `product/apps/ccli` drift | Mirror tree | Automated sync check in CI | **PART** |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| **SM9-R01** | GPL libs (61850/60870) linkage | License / export | MZ commercial license track |
| **SM9-R02** | Platform CVE outside app SBOM | Field exposure | SM-10 vendor SDL + Trivy DUT pull |
| **SM9-R03** | Lab keys on DUT | False positive secrets | ACCEPT policy + annex PKI verify |
| **SM9-R04** | Stale apk version field | Audit confusion | Bump ipk with each rN deploy |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-SM9-001 | Open `CCLI_APP_SBOM_0.1.0-r64.cdx.json` | Components match § BOM |
| REQ-SW-004 | `sha256sum /usr/sbin/ccli` on DUT | Matches staging SHA256 |
| REQ-SM9-002 | `platform-runtime-inventory.cdx.json` | 302 apk components |
| SVV-3 | Trivy r64 evidence | 0 blocking CVE on ELF |
| SM-9 record | `records/SM-9_r64_SBOM_signoff.md` | Filled for r64 |

---

## Cross-references

| Doc | Path |
|-----|------|
| Application structure (KB) | `knowledge-base/06-application/CCI_Application_Structure.md` |
| SW version policy | `lab/tg544-openwrt/CCLI_SW_VERSION_AND_BINARIES.md` |
| EVD-SM-9 pack | `knowledge-base/08-engineering/evidence-packs/EVD-SM-9-External-Components.md` |
| ZCR-1 SUC | `knowledge-base/08-engineering/ZCR-1-SUC-TR400.md` |
