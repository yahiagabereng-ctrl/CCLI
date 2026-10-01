# CCI Architecture Framework

**Document ID:** CCLI-ARCH-FW-001  
**Revision:** 1.3  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-architecture-framework`  
**Cursor rule:** `.cursor/rules/system-architect.mdc`  
**Purpose:** Repeatable methodology for PF2 / CEI 0-16 CCI hardware and system design — used by Cursor rules and BizChat RAG.

---

## Methodology chain

```
Requirements
  → Functional architecture
  → Hardware architecture
  → Network architecture
  → Security architecture
  → Power architecture
  → Interface matrix
  → BOM matrix
  → Knowledge gap analysis
  → Risk analysis
  → Verification plan
```

Do not skip steps. Part selection comes **after** interface and power budgets are defined.

**Visual one-board diagram (Mermaid):** `CCI_62443_Zones.md` § *Mermaid — one-board view* — zones, TR400 pin-out, conduits, regulations, phases, HW/SW requirements.

---

## Product context

| Layer | Role |
|-------|------|
| **Product platform (FROZEN)** | **TesPro TG-524 / TR400** — OpenWrt / TesproOS, **MediaTek MT798X**, integrated comm box |
| **Active lab DUT** | **TesPro TR400** (received 2026-09) — L2+ on-target validation |
| **Secondary mock** | **None** — TesPro TG544 only (Pi removed 2026-09-17) |
| **SoC (FROZEN — FINAL)** | **MT798X** — Cortex-A53 dual @ 1.6 GHz — see `CCI_SOC_Freeze.md` |
| **Archived only** | Toradex / NXP SoM + custom carrier — `_archive/som-study/` |

**CEI qualification:** TR400/TG-524 hardware capability ≠ CEI-qualified CCI from datasheet alone — Allegato O/T is **your software + evidence**.

Detail: `CCI_SOC_Freeze.md` · `CCI_TG500_Lab_Platform.md` · `CCI_CCLI_Product_Spec_and_Roadmap.md` · `CCI_TR400_Extract.md` · `CCI_Phase_Regulation_Checklists.md`

---

## Requirement ID prefixes

| Prefix | Domain |
|--------|--------|
| `REQ-NET-*` | Ethernet domains, VLANs, isolation |
| `REQ-SER-*` | RS485/RS232, Modbus, IEC 60870 |
| `REQ-IO-*` | DI 10–120 V, DO dry contact |
| `REQ-PWR-*` | 12–24 V input, sequencing, UPS context |
| `REQ-SEC-*` | Secure boot, HAB, secure element, IEC 62351 |
| `REQ-TIME-*` | GNSS, NTP, timestamping |
| `REQ-LTE-*` | Cellular backup / remote access |
| `REQ-PF2-*` | CEI 0-16 Annex O/T curtailment & observability |
| `REQ-REG-*` | CEI, IEC 61850, IEC 62443, ATEX reuse |
| `REQ-HW-*` | Platform binding — ports, devices, limits **fixed by TR400** |
| `REQ-BLD-*` | Build, deploy, SDK — **depends on TR400 OS/toolchain** |
| `REQ-SW-*` | Application profile on TesproOS — **depends on HW services present** |

---

## TR400 hardware-dependent requirements

Derived from **AiLux-class regulatory target** (`ccli-cci-module-regs-class`) mapped to **TR400 as-built** (`ccli-tr400-user-manual-extract`, `ccli-tg500-lab-platform` Rev 3.0).

**Legend — HW dependency:**

| Tag | Meaning |
|-----|---------|
| **BIND** | Requirement is **fixed or constrained** by TR400 physical interfaces / OS device names |
| **VERIFY** | TR400 may satisfy it — must be **proven on bench** (not datasheet alone) |
| **GAP** | AiLux-class needs more than TR400 provides — **expansion or Wave B** |
| **SW** | Same requirement on any platform — **implementation** differs on TR400 HAL |

**Config anchor:** `apps/ccli/config/lab_tr400.yaml`

### Platform & compute (C7 frozen)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-HW-001 | Product compute on **MT798X** integrated gateway — no discrete SoM | BIND | TR400 / TG-524 family | **HAVE** | `uname -a`; TesproOS 1.0.0 |
| REQ-HW-002 | **C/C++** user-space on **OpenWrt / TesproOS** — not vendor DLMS/OPC stack as CCI product | SW | Disable extras in product profile | **PART** | `REQ-SW-001` audit |
| REQ-BLD-001 | Cross-compile via **TesPro OpenWrt SDK** (K2.2) or on-device bootstrap | BIND | MT798X toolchain | **MISSING** | `.ipk` installs and runs on TR400 |
| REQ-BLD-002 | Deploy via **procd + `.ipk`** — same package model as Pi OpenWrt lab | SW | `package/ccli/` | **PART** | Service survives reboot |

### Ethernet & network (C1)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-HW-ETH-001 | **≥4** Ethernet roles: Eth_A (DSO), Eth_B (OA), 2× plant | BIND | **1 WAN + 4 LAN** GE — count **HAVE** | **PART** | Role map documented |
| REQ-NET-001 | **No L2 bridge** Eth_A ↔ Eth_B ↔ plant | VERIFY | OpenWrt **firewall zones** (LAN allow, WAN deny input) | **HAVE** | P2-01 matrix 2026-09-25 — role PC↔PC blocked |
| REQ-NET-002 | **WAN / LTE backup only** — no route to DSO MMS | VERIFY | WAN port + cellular WWAN | **HAVE** | P2-03 2026-09-25 — ping -I usb0 → DSO blocked |
| REQ-NET-003 | **Silkscreen ↔ `ip link` ↔ CCI role** documented | BIND | WAN, LAN1–4 labels on panel | **OPEN** | Table in `lab_tr400.yaml` + photo |
| REQ-NET-004 | Default lab access **192.168.0.0/24**, gateway **192.168.0.1** | BIND | Appendix B defaults | **HAVE** | SSH + Web UI |
| REQ-NET-005 | **Wi-Fi disabled** in CCI product profile | VERIFY | Wi-Fi LED on unit — must disable | **OPEN** | `wifi` down / UCI |

*AiLux gap:* reference module uses **4× 10/100** managed switch semantics; TR400 uses **5× GE** + **software segmentation** — function equivalent, architecture differs (ADR needed for GOOSE L2).

### Serial / Modbus (C2)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-HW-SER-001 | **2× RS485** plant serial ports | BIND | **A1/B1**, **A2/B2** on 7-pin block | **HAVE** | Physical wiring |
| REQ-SER-001 | Modbus RTU master on **RS485-A** | BIND | **`/dev/ttyS1`** (RS485-1) | **PART** | pymodbus slave on PC responds |
| REQ-SER-002 | Spare **RS485-B** for second plant bus | BIND | **`/dev/ttyS2`** (RS485-2) | **HAVE** | Loopback / second slave |
| REQ-HW-SER-002 | **RS232** service / debug only — out of cert path | BIND | **`/dev/ttyS0`** (RXD/TXD/GND) | **HAVE** | Not used in PF2 cert tests |
| REQ-HW-SER-003 | **Opto-isolated** RS485 per AiLux class | VERIFY | Isolation class **unconfirmed** | **OPEN** | TesPro written confirm + bench |
| REQ-SER-003 | A/B polarity + termination per manual | BIND | Manual p.48 termination note | **PART** | Stable RTU at 9600 8N1 |

*AiLux gap:* reference requires **RJ45 8/8 Modbus pinout** on serial; TR400 uses **terminal block A1/B1** — function OK, connector craft differs.

### Digital I/O & PF2 actuation (C4) — **largest HW gap**

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-HW-IO-001 | **4× configurable DIO** on 10-pin block (DIO1–DIO4) | BIND | Front panel **DIO1–DIO4** + AI/AO | **HAVE** | `gpiodetect` / `gpioinfo` |
| REQ-IO-001 | **DO curtailment** on lab binding | BIND | **DIO1** → relay module | **OPEN** | GPIO line in `lab_tr400.yaml` |
| REQ-IO-002 | **DI permissive** gates curtailment | BIND | **DIO2** ← field permissive | **OPEN** | Jumper test gates DO |
| REQ-IO-003 | **Boot / crash / comm fault → defined DO state** (62443 CR 3.6) | VERIFY | libgpiod default + FSM | **PART** | Reboot test VAL §7 |
| REQ-HW-IO-002 | **DIO voltage / current class** documented | VERIFY | Logic-level vs 10–120 V **unknown** | **OPEN** | Scope / TesPro DI spec |
| REQ-IO-004 | **Product class:** **5× DI 10–120 V** + **3× relay DO 250 V/3 A** | GAP | TR400: **4 DIO** only | **GAP** | Wave B fixture or expansion module |
| REQ-HW-IO-003 | **AI1/2, AO1/2** present — not AiLux PF2 core | BIND | Analog block on 10-pin | **DEFER** | Out of Phase 1 PF2 |

*Lab vs product:* Phases **0–1** may prove PF2 on **DIO1/DIO2 + relay**; Phase **6** must close **REQ-IO-004** or document approved expansion.

### Power (C10)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-PWR-001 | **12–36 V** class DC input (AiLux: 12–24 V ±20%) | BIND | Panel **12–30 V** terminal | **HAVE** | Within class |
| REQ-PWR-002 | Relay / DO hold during brief brownout | VERIFY | Gateway PSU + relay module | **OPEN** | Dip test on bench |

### Security (C6)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-SEC-001 | **Secure Boot** — only signed images | VERIFY | MT798X / TesproOS chain — **not NXP HAB** | **OPEN** | Boot unsigned image → fail |
| REQ-SEC-002 | **TPM 2.0** key storage | VERIFY | On-unit TPM (brochure) | **PASS** (presence 2026-09-25) | `tpm2_getcap` IFX SLB9673 — `lab/evidence/.../P0_TPM_VERIFY.txt`; key custody for TLS still TBD |
| REQ-SEC-003 | Default **`root` / `000000`** changed before any field use | BIND | Manual first-login warning | **OPEN** | Password policy enforced |

### Time & cellular (C5 / C7)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-TIME-001 | Event timestamps — **NTP** minimum | SW | **chrony** on TG544 (lab) · BusyBox ntpd disabled | **PASS** (lab) | chronyc ≤±100 ms · P3-11 |
| REQ-TIME-002 | **GNSS / PPS** for Annex O time quality | VERIFY | Modem GNSS + SOCK feeder; PPS = C5 gap | **PART** | TimeQuality from chrony+fix; PPS deferred |
| REQ-LTE-001 | **LTE / WAN** backup — isolated from Eth_A | VERIFY | WAN + WWAN zones | **PART** | Primary control unaffected when LTE lost |

### Commissioning (C8)

| ID | Requirement | Dep | TR400 binding | Status | Verification |
|----|-------------|-----|--------------|--------|--------------|
| REQ-HW-COM-001 | **USB 3.0** local commissioning — out of cert data path | BIND | Front USB | **HAVE** | USB not in MMS path |
| REQ-HW-COM-002 | **SSH + HTTPS Web** on LAN only for engineering | BIND | 192.168.0.1 TesproOS UI | **HAVE** | RBAC / firewall review |

### Software stacks bound to TR400 interfaces (C3)

| ID | Requirement | Dep | TR400 interface | Status | Verification |
|----|-------------|-----|-----------------|--------|--------------|
| REQ-61850-001 | MMS server on **Eth_A** per Allegato T | BIND | Mapped LAN → `eth_a` in YAML | **SW** | Client read on Eth_A |
| REQ-104-001 | IEC 60870-5-104 on **Eth_B** | BIND | Mapped LAN → `eth_b` | **SW** | 104 supervision tests |
| REQ-PF2-001 | Curtailment FSM — threshold, debounce, min on/off | SW | Uses **DIO1** DO | **PART** | L1-P-03 on TR400 |
| REQ-PF2-002 | Stale meter → safe state | SW | Modbus on **ttyS1** | **PART** | L1-C-01 |
| REQ-PF2-003 | Permissive before DO | BIND | **DIO2** DI | **PART** | Permissive open → no curtail |
| REQ-MET-007 | MC200 200 ms fixed blocks | SW | Same on TR400 | **SW** | Unit + L3 analyzer |

### Requirements **not** depending on TR400-specific HW (portable)

These stay valid on **Pi L1** and **TR400 L2+** with the same test vectors:

- REQ-PF2-004 (timestamped events), REQ-MET-008 (PF1 4 s blocks)
- REQ-SEC-004 (62443 CR set), REQ-SEC-005 (audit trail) — process + SW
- REQ-REG-* (CEI O/T, 61557-12, ARERA) — evidence on whichever DUT runs the stack

### Phase 0 closure — unblocks all BIND rows

| Capture on TR400 | Closes |
|------------------|--------|
| `ip link` + photo of silkscreen | REQ-NET-003, REQ-HW-ETH-001 |
| `gpiodetect` + `gpioinfo` → DIO1/DIO2 lines | REQ-IO-001, REQ-IO-002, REQ-HW-IO-001 |
| `tpm2_*` + secure boot probe | REQ-SEC-001, REQ-SEC-002 |
| Modbus RTU on A1/B1 | REQ-SER-001 |
| Relay click on DIO1 | REQ-PF2-001 end-to-end |

---

## C1–C10 functional blocks (BOM-driven)

| Block | Function | Phase 1 lab status (TR400) |
|-------|----------|----------------------------|
| C1 | Ethernet — Eth_A, Eth_B, plant ports | **Partial** — 1 WAN + 4 LAN on TR400; map **OPEN** |
| C2 | Serial — 2× isolated RS485 Modbus | **Partial/Full** — A1/B1, A2/B2; `ttyS1/S2` |
| C3 | Protocol stacks — 61850, 60870, Modbus | Software — Pi L1 + **TR400 L2** |
| C4 | Digital I/O — DI status, DO curtailment | **Partial** — **4× DIO** on TR400; AiLux 5/3 needs expansion |
| C5 | GNSS time sync | **Partial** — optional modem GNSS |
| C6 | Security — TPM, secure boot | TPM 2.0 + Secure Boot — **verify on TR400** |
| C7 | LTE modem | WAN/cellular — backup only |
| C8 | Local console / commissioning | USB 3.0 / SSH / TesproOS Web |
| C9 | Enclosure / mechanical | TR400 gateway — not final DIN CCI |
| C10 | Power — wide-range DC | **12–30 V** panel (within 12–36 V class) |

Detail: `CCI_Prototype_BOM.md`, `CCI_Vendor_Selection.md`.

---

## Interface matrix template

| Interface | Source | Destination | Protocol | Isolation | Notes |
|-----------|--------|-------------|----------|-----------|-------|
| Eth_A | CCI | DSO | IEC 61850 MMS / GOOSE | PHY/VLAN vs Eth_B | Annex O |
| Eth_B | CCI | Operatore Abilitato | IEC 61850 / 60870-104 | Separate from Eth_A | Annex T |
| Plant-1/2 | CCI | Plant LAN | Modbus TCP / raw | From DSO path | — |
| RS485-A/B | CCI | Energy analyzer | Modbus RTU | Opto-isolated | C2 |
| DI-1..n | Field 10–120 V | CCI GPIO | Digital in | Opto | C4 |
| DO-1..n | CCI relay | Curtailment | Dry contact | — | PF2 critical |
| GNSS | Antenna | SoM UART/USB | NMEA / PPS | — | C5 |
| LTE | Modem | Carrier network | PPP / QMI | Logical separation | C7 |

---

## BOM matrix template

| Block | Function | Candidate | PN | Est. EUR | Doc status |
|-------|----------|-----------|-----|----------|------------|
| LAB-GW | Product + lab DUT | **TesPro TR400 / TG-524** | TG-500 / MT798X | TBD quote | **FROZEN** · TR400 **ON BENCH** |
| LAB-MOCK | — | Raspberry Pi | — | — | **REMOVED** |
| SDK | Cross-compile | TesPro OpenWrt SDK | — | — | **MISSING** |

---

## Current knowledge (INGESTED / HAVE)

| Area | Corpus |
|------|--------|
| Programme & roadmap | `ccli-project-roadmap` |
| SoC freeze | `ccli-soc-freeze` |
| Prototype BOM & doc gate | `ccli-prototype-bom` |
| Validation L1–L4 / PF2 bench | `ccli-validation-strategy`, `ccli-mocking-bench-bom` |
| CEI 0-16 O/T extracts | `cei-0-16-allegato-o`, `cei-0-16-allegato-t` |
| TG-524 / TR400 lab platform | `ccli-tg500-lab-platform`, `ccli-tr400-user-manual-extract`, `ccli-product-spec-roadmap` |
| SoC freeze | `ccli-soc-freeze` |
| Protocol libraries | `ccli-github-protocol-libs` |
| ATEX QMS reuse | Org certification (not re-budgeted) |
| Regulations taxonomy | `ccli-cci-module-regs-class` |

---

## Knowledge gaps (Architecture Freeze blockers)

| Area | Current | Required | Gap |
|------|---------|----------|-----|
| OpenWrt SDK | None | TesPro toolchain | **MISSING** |
| Lab port map | Partial | Eth_A/B/plant labels | **PARTIAL** |
| RS485 isolation proof | Brochure | TesPro confirmation | **PARTIAL** |
| IEC 61850 commercial license | GPLv3 prototype | MZ Automation commercial quote | Budget item |
| Secure boot runbook | Git paths listed | Product key hierarchy doc | Planned |
| Accredited lab scope | ATEX NB relationship | 62443/61850 scoping quote | To do |

Clustered gate: `CCI_RAG_Knowledge_Base.md` · downloads: `DOWNLOADS.md`.

---

## Risk template

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-ISO-001 | Eth_A/Eth_B not physically isolated | Grid rejection, audit fail | Dual PHY or managed switch with strict VLAN; no shared MAC bridge |
| R-PF2-001 | Stale meter data drives curtailment | Wrong plant state | Timestamp + validity flags; timeout → safe state |
| R-SEC-001 | Keys in filesystem | IEC 62351 / 62443 fail | Secure element + signed images from day one |
| R-BOM-001 | Kit mistaken for field CCI | Wrong procurement | Label every doc: kit = lab, carrier = product |

---

## Verification template

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| REQ-NET-001 | L2 bench: Eth_A MMS server reachable | Client read/write on isolated port |
| REQ-SER-001 | Modbus RTU poll on RS485 | Register map matches analyzer DS |
| REQ-PF2-001 | Simulated DSO curtailment command | DO toggles within Annex O timing |
| REQ-TIME-001 | GNSS lock | PPS/NMEA timestamp on events |
| REQ-SEC-001 | Secure boot chain | Only signed image boots |

Bench levels: L1 software mock → L2 HIL → L3 real analyzer → L4 power HIL (`CCI_Validation_and_Mocking_Strategy.md`).

---

## Architecture deliverables checklist

1. Requirements list with IDs  
2. Block diagram (SoM + carrier + field interfaces) — `Architecture/Hardware_Architecture.drawio`  
3. Interface matrix — `Architecture/Interface_Matrix.xlsx`  
4. Power budget — `Architecture/Power_Tree.drawio`  
5. Pin budget / connector list  
6. BOM with MPN TBD → MPN locked — `Architecture/BOM_Matrix.xlsx`  
7. Risk register — `Architecture/Risk_Register.xlsx`  
8. Verification matrix — `Architecture/Verification_Matrix.xlsx`  
9. Document gate D0–D21 status — `Architecture/Knowledge_Matrix.xlsx`  

**Engineering workspace:** repo root `Architecture/` (draw.io + Excel). Regenerate stubs: `python scripts/init-architecture-workspace.py`.

---

## Keywords

`architecture framework`, `PF2`, `requirements`, `interface matrix`, `BOM matrix`, `verification`, `C1-C10`, `Eth_A`, `Eth_B`, `carrier`, `Architecture Freeze`, `ccli-architecture-framework`
