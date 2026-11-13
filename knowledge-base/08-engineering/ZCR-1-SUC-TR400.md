# ZCR 1 — System Under Consideration (SUC)

**Document ID:** CCLI-ZCR-1-SUC-001  
**Revision:** 1.0  
**Date:** 2026-11-13  
**Status:** ENGINEERING BASELINE (`PART` — pending asset-owner inputs for tolerable risk)  
**Normative:** IEC 62443-3-2:2020 **§4.2 ZCR 1** · CRS precursor **§4.7.3** (ZCR 6.2)  
**Parent CRS:** `CCI_62443_Zones.md` (CCLI-SEC-62443-001 Rev 1.7)  
**Requirement:** **REQ-SEC-ZC-001** — Identify SUC perimeter and all access points  

**Product SKU:** TesPro **TG-524** · lab DUT **TR400** (TR500 SoC class)  
**Application:** HiTEKS **CCLI** (`apps/ccli/` → `/usr/sbin/ccli`) — PF2 / CEI 0-16 central plant controller  
**Lab DUT identity (LAN1, 2026-11-12):** TesproOS **32-2.1.0** `r601-af70fcaaa2` · `ccli` **0.1.0-r64** SHA256 `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7`

---

## 1. ZCR 1 workflow (Figure 1 alignment)

| Phase | Standard text | This document |
|-------|---------------|---------------|
| **Input** | Initial system architecture diagrams and inventory; company policies, regulations, tolerable risk guidelines | §2 |
| **Activity** | **ZCR 1 — Identify the System Under Consideration (SUC)** | §3–§7 |
| **Output** | Updated system architecture diagrams and inventory with **IACS external services and support** identified | §4–§8 · diagrams §9 |

**Next step in programme:** **ZCR 2** — [`ZCR-2-INITIAL-RISK-TR400.md`](ZCR-2-INITIAL-RISK-TR400.md) (`REQ-SEC-ZC-002`).

---

## 2. ZCR 1 inputs (corpus)

| Input class | Artefact | Status |
|-------------|----------|--------|
| Architecture (logical) | `Architecture/mermaid/CCLI_Fig01_TR400_Zones_Pinout.mermaid` | **HAVE** |
| Architecture (port map) | `Architecture/Zones_62443_TG544_PortMap.drawio` | **HAVE** (P2-05) |
| Architecture (zones) | `knowledge-base/08-engineering/CCI_62443_Zones.md` | **HAVE** / **PART** |
| Runtime dataflow | `Architecture/CCI_Runtime_Dataflow_Architecture.md` | **HAVE** |
| Platform / bench | `lab/HW_CONFIGURATION_INDEX.md` · `CCI_TG500_Lab_Platform.md` | **HAVE** |
| Deploy pin (app) | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_LATEST.json` | **HAVE** |
| Platform inventory (runtime) | `lab/evidence/EVD-SM-9-External-Components/intake/20261112-104506/` | **HAVE** (302 apk; intake CDX) |
| App SCA sample | `lab/evidence/trivy/EVD-SCA-CCI-r64-Trivy.md` | **HAVE** |
| Corporate tolerable risk matrix | — | **MISS** (ZCR 6.8) |
| Asset-owner policy pack | — | **MISS** (feeds ZCR 7) |
| Regulations (product) | CEI 0-16 Allegati O/T, IEC 61850/62351, CRA/RED refs in `CYBER_NORMATIVA_SERIES_COVERAGE.md` | **PART** |

---

## 3. SUC identity (ZCR 6.2 — name, function, usage, equipment)

| Field | Value |
|-------|--------|
| **SUC name** | **PF2 Central Plant Controller (CPCI)** — HiTEKS CCLI on TesPro TR400/TG-524 |
| **Primary function** | Aggregate plant metering (Modbus), enforce power-factor / curtailment logic, expose **DSO** northbound (**IEC 61850 MMS** on Eth_A) and **operator** southbound (**IEC 60870-5-104** / client MMS on Eth_B), time-sync and evidence for CEI 0-16 conformance |
| **Intended usage** | Installed at point of connection / central plant; continuous operation; qualified operator + DSO remote access; maintenance via segregated temporary path (USB/console) |
| **Equipment under control (logical)** | POC measurement chain (analyzer, CT/VT class devices via Modbus), curtailment **DIO** outputs, MMS/104-visible plant model (ICD/CID), optional LTE backup path |
| **Equipment under control (not in Z0)** | Field inverters, external relays beyond DIO wiring, DSO head-end, operator SCADA — **connected via conduits**, not embedded in CCI firmware |
| **Security relevance** | IACS component at grid boundary; integrity of export measurements and curtailment commands; segmentation between DSO, operator, plant, maintenance, WAN |

---

## 4. SUC boundary — included vs excluded

### 4.1 Inside the SUC (assets + connectivity to be assessed)

| ID | Asset / path | Zone | Role |
|----|----------------|------|------|
| A-00 | **TR400/TG-524** hardware, TPM, PSU 12–36 V | Z0 | Platform |
| A-01 | **TesproOS** (OpenWrt-class) + kernel **6.12.68** | Z0 | OS / network stack |
| A-02 | **`ccli`** application **0.1.0-r64** | Z0 | Control logic, protocols, policy |
| A-03 | **DIO1–DIO4** (ubus `didoservice_v2`) | Z0 | Curtailment / permissive / optional trip |
| A-04 | **MMS server** path Eth_A (LAN1) | C1 ↔ Z1 | DSO exchange |
| A-05 | **IEC 104** / operator client path (LAN2/LAN3 lab binding) | C2 ↔ Z2 | Operator exchange |
| A-06 | **Modbus RTU** `/dev/ttyS1`, `/dev/ttyS2` · **Modbus TCP** plant LAN | C3 ↔ Z3 | Plant measurement |
| A-07 | **USB 3.0** console (SSH / LuCI / uHTTPd) | C4 ↔ Z4 | Temporary maintenance |
| A-08 | **WAN / LTE** (when fitted) | C5 ↔ Z5 | Backup IP |
| A-09 | **Bench peers** on VLANs (PC DSO/OA/plant simulators) | Z1–Z3 | *Lab only* — represent production peers inside SUC assessment |

### 4.2 Explicitly **outside** the SUC (no IACS assessment scope)

| Exclusion | Rationale |
|-----------|-----------|
| **Enterprise IT / office LAN** | No L2 bridge to Eth_A — `REQ-SEC-ZC-004` |
| **Vendor cloud / TesPro remote management** | Not contracted in baseline product profile |
| **Public Internet** (except defined WAN backup semantics) | Not a permanent control path to DSO zone |
| **Pi / dev mock platform** | Out of product scope (`CCI_SOC_Freeze.md`) |

---

## 5. Perimeter — access points (AP)

Physical labels from TR400 silkscreen; lab Linux binding from bench freeze (**`lab/HW_CONFIGURATION_INDEX.md`**).

| AP | Physical | Lab network / device | OpenWrt / Linux | Direction | Protocols (intended) | Conduit |
|----|----------|----------------------|-----------------|-----------|----------------------|---------|
| **AP-1** | **LAN1 (Eth_A)** | DUT `192.168.10.1` · PC `192.168.10.10` | `lan1` / `*_lan1_sec` | Bidirectional | IEC 61850 MMS (:3782 lab), IEC 62351-4 TLS (target) | **C1** |
| **AP-2** | **LAN2 / LAN3 (Eth_B)** | DUT `192.168.1.130` · PC `192.168.1.183` | `lan2` / `br-lan` (profile-dependent) | Bidirectional | IEC 60870-5-104 (:2404), 61850 client | **C2** |
| **AP-3a** | **A1/B1 RS485** | PC `COM5` ↔ **`/dev/ttyS1`** | `ttyS1` | CCI master → plant | Modbus RTU | **C3** |
| **AP-3b** | **A2/B2 RS485** | — | `ttyS2` | CCI master → plant | Modbus RTU | **C3** |
| **AP-3c** | **LAN3/LAN4 plant** | e.g. `192.168.178.0/24` EMT432 | plant iface | Bidirectional | Modbus TCP (:502) | **C3** |
| **AP-4** | **USB 3.0** | Maintainer laptop | `usb*` | Inbound maint. | SSH, HTTPS (LuCI) | **C4** |
| **AP-5** | **WAN** (+ LTE) | Backup only | `wan` | Inbound/outbound | IP / VPN (policy) | **C5** |
| **AP-6** | **Wi‑Fi** | — | **Disabled** | — | — | — |
| — | **RS232 service** | — | `/dev/ttyS0` | Service | Debug (out of cert path) | — |
| — | **DIO1–DIO4** | Relay / button | ubus GPIO | **Internal Z0** | Digital I/O | **Not** perimeter AP |

**Board rules (no extra APs):** No L2 bridge LAN1 ↔ LAN2 ↔ plant ↔ WAN; WAN must not route into Eth_A DSO (`CCLI_Fig01`).

---

## 6. Inventory — updated for ZCR 1 output

### 6.1 Core component (HiTEKS)

| Item | Version / ID | Evidence |
|------|----------------|----------|
| `ccli` binary | **0.1.0-r64** · `967df276…` | `CCLI_DEPLOY_LATEST.json` |
| Config pin (lab) | `product_tr400.yaml` → `/etc/ccli/lab.yaml` | `HW_CONFIGURATION_INDEX.md` §2 |
| ICD / MMS | `product_tr400_eth_a_5ln.cfg` | deploy manifest |
| Source tree | `apps/ccli/` · product `product/apps/ccli/` | repo |
| App SBOM + arch (r64) | `EVD-SM-9-External-Components/CCLI_APP_SBOM_0.1.0-r64.cdx.json` | **HAVE** |

### 6.2 Platform (TesPro) — runtime on bench DUT

| Item | Value | Evidence |
|------|--------|----------|
| OS | TesproOS **32-2.1.0** `r601-af70fcaaa2` | `intake/20261112-104506/dut-platform-identity.txt` |
| SoC / arch | MediaTek filogic · **aarch64_cortex-a53** | same |
| Installed packages | **302** (`apk list -I`) | `platform-apk-list-installed.txt` |
| SBOM proxy | CycloneDX intake (not vendor-signed) | `platform-runtime-inventory.cdx.json` |
| Formal vendor SBOM | Image-bundle `.bom.cdx.json` for **r601** | **MISS** — SM-10 / TesPro request |

### 6.3 Vendored libraries (HiTEKS build — linked into `ccli`)

| Component | Location | Notes |
|-----------|----------|--------|
| libiec61850 | `third_party/libiec61850` | MMS / 61850 stack |
| lib60870 | `third_party/lib60870` | 104 (if enabled) |
| mbedTLS | SDK / static link | SM-9 / SM-10 tracking |

### 6.4 Lab-only configuration assets (in SUC assessment, not production SM-8)

| Asset | Path | Scanner note |
|-------|------|--------------|
| EJBCA lab PKI | `apps/ccli/config/tls/` + on-DUT `etc/ccli/tls/` | Trivy **secret** HIGH — accepted lab |
| P3-07 Connect pack | `config/tls/p3_07_connect/` | Conformance TLS |

---

## 7. IACS external services and support (ZCR 1 output emphasis)

Entities and services that **support or border** the SUC but are not necessarily on the CCI image:

| ID | Service / party | Relationship | In SUC? | Record |
|----|-------------------|--------------|---------|--------|
| X-01 | **TesPro** — BSP, TesproOS build **r601**, hardware TR400 | Platform supplier | **Support** (OS/firmware) | SM-10 gap matrix |
| X-02 | **DSO** — MMS client / head-end | Consumes C1 | **Peer** (Z1) | CEI Allegato T tests |
| X-03 | **Qualified operator** — 104 / OA SCADA | Consumes C2 | **Peer** (Z2) | TSP / LAN2 evidence |
| X-04 | **Plant devices** — EMT432, Modbus slaves | Consumes C3 | **Peer** (Z3) | Phase 5 / HW map |
| X-05 | **Accredited test tools** — Test Suite Pro, ERDA | Conformance support | **Support** (lab) | `lab/evidence/testsuite-pro/` |
| X-06 | **EJBCA** (lab CA) | Issues bench TLS | **Support** (lab PKI) | `scripts/sync-ejbca-tls-to-product.ps1` |
| X-07 | **Accreditation / cert bodies** — TÜV, ERDA | Audit, not runtime | **Support** | `Certificate/` trees |
| X-08 | **GNSS / LTE modem** (when fitted) | Time / backup WAN | **Component** in C5 | TesPro feeds |

---

## 8. Architecture outputs (diagrams)

| Diagram | Path | ZCR role |
|---------|------|----------|
| Zones + conduits + pin-out | `Architecture/mermaid/CCLI_Fig01_TR400_Zones_Pinout.mermaid` | Updated architecture |
| Signed port map | `Architecture/Zones_62443_TG544_PortMap.drawio` | ZCR 3 / 6.3 precursor |
| Full zone CRS draft | `CCI_62443_Zones.md` | Parent; ZCR 1 §72–100 |

---

## 9. ZCR 1 verification

| Check | Pass criteria | Status |
|-------|---------------|--------|
| SUC named and bounded | §3 + §4 tables complete | **PASS** (engineering) |
| All APs listed | §5 no missing silkscreen AP | **PASS** (lab mapping) |
| Inventory traceable | §6 links to dated evidence | **PASS** (bench) |
| External services listed | §7 | **PASS** (draft) |
| Policies / tolerable risk attached | §2 corporate matrix | **FAIL** — input **MISSING** |
| Asset-owner acceptance | ZCR 7 signature | **MISS** (not ZCR 1) |

**Bench commands (repeat inventory):**

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
plink -ssh root@192.168.10.1 -pw $env:CCLI_TG544_PW -batch "cat /etc/openwrt_release; /usr/sbin/ccli --version; sha256sum /usr/sbin/ccli"
plink -ssh root@192.168.10.1 -pw $env:CCLI_TG544_PW -batch "apk list -I" > lab\evidence\EVD-SM-9-External-Components\intake\latest-apk.txt
```

---

## 10. Knowledge gaps

| Area | Current | Required for ZCR 1 close |
|------|---------|---------------------------|
| Tolerable risk / corporate matrix | **MISS** | Owner policy document |
| OpenWrt iface names on all SKUs | **PART** (`lan1` frozen in lab) | Product ifname freeze per SKU |
| Vendor CycloneDX for **r601** | **MISS** | SM-10 delivery |
| Production PKI (SM-8) vs lab TLS | Documented split | SM-8 EVD when in scope |

---

## 11. Traceability

| Link | ID |
|------|-----|
| Requirement | **REQ-SEC-ZC-001** |
| Zones / conduits (ZCR 3) | `CCI_62443_Zones.md` |
| Gap matrix | `CCI_62443_TesPro_HiTEKS_Gap_Matrix.md` §2 row ZCR 1 |
| Conformance matrix | `lab/conformance/Full Validate.md` (ZCR 1 → P0-01/04) |
| External components | `EVD-SM-9-External-Components.md` |
| SM-9 runtime intake | `lab/evidence/EVD-SM-9-External-Components/intake/20261112-104506/README.md` |

---

## 12. Document control

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-11-13 | Initial ZCR-1 SUC baseline extracted for TR400 lab; pins SM-9 intake 20261112-104506 |

**Approver (ZCR 7 — placeholder):** Asset owner / DSO integrator — *unsigned*.
