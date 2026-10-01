# CCLI Product Specification & Roadmap — AiLux-class / CEI 0-16

**Document ID:** CCLI-SPEC-001  
**Revision:** 1.1  
**Date:** 2026-09-22  
**RAG source_id:** `ccli-product-spec-roadmap`  
**Platform:** TesPro **TR400** (lab DUT) · **TG-524 / TG-500** (frozen product line)  
**Companions:** `CCI_TG500_Lab_Platform.md` · `CCI_TR400_Extract.md` · `CCI_Module_Regulations_Classification.md` · `CCI_Validation_and_Mocking_Strategy.md` · **`CCI_Phase_Regulation_Checklists.md`**

---

## 1. Product intent

| Item | Definition |
|------|------------|
| **Product** | CEI 0-16 **CCI** (Controllore / Central Plant Controller) for Italy DER plants |
| **Behaviour reference** | AiLux-class module (`cei-0-16-cci-manual-ailux`) — **not** a firmware clone |
| **Frozen platform** | TesPro **TG-500 / TG-524** — OpenWrt / TesproOS, **MediaTek MT798X** |
| **Active lab DUT** | **TesPro TR400** (received 2026-09) — same software line as TG-524 |
| **Application** | New **C/C++** in `apps/ccli/` — 61850, Modbus, PF2 (`REQ-SW-001`) |

**CEI qualification** requires **your software + evidence** — not TesPro marketing claims alone.

---

## 2. Hardware specification

### 2.1 AiLux-class target (regulatory product class)

| Subsystem | AiLux-class spec | Corpus |
|-----------|------------------|--------|
| Ethernet | 4× 10/100 — **Eth_A**, **Eth_B**, 2× plant | `ccli-cci-module-regs-class` |
| Serial | 2× opto-isolated RS-232/485 | C2 |
| DI | **5×** opto **10–120 Vdc** (external field supply) | C4 |
| DO | **3×** relay dry contact **250 Vac / 3 A** | C4 |
| GNSS / LTE | GPS/GLONASS; LTE Cat.1 backup | C5 / C7 |
| Power | 12–24 Vdc ±20% | C10 |
| Security | Secure Boot, TPM 2.0, 62443-4-2 | C6 |

### 2.2 TR400 as-built (bench unit — photo + manual v1.0 EN)

| Subsystem | TR400 spec | vs AiLux | Status |
|-----------|------------|---------|--------|
| **Ethernet** | **1 WAN + 4 LAN** Gigabit; silkscreen **WAN, LAN2, LAN3** (+ LAN1 TBD on unit) | Meets **C1** count | **PART** — `ip link` map open |
| **RS485** | **A1/B1**, **A2/B2** + GND; **`/dev/ttyS1`**, **`/dev/ttyS2`** | Meets **C2** count | **PART** — isolation TBD |
| **RS232** | RXD/TXD/GND — **`/dev/ttyS0`** | Extra | Service |
| **Digital I/O** | **DIO1–DIO4** + **AI1/2**, **AO1/2** on 10-pin block | **4 DIO** vs 5 DI + 3 DO | **PART** — lab relay OK |
| **CAN** | H1/L1, H2/L2 | Not AiLux core | Out of scope P1 |
| **USB** | USB 3.0 | Commissioning | **C8 HAVE** |
| **Wi-Fi** | On unit — **disable** in product profile | N/A Italy CCI | **REQ-SW-001** |
| **Power** | **12–30 V** terminal | Within C10 | **HAVE** |
| **OS** | **TesproOS 1.0.0** (OpenWrt-based) | C7 frozen | **HAVE** |
| **Default access** | **192.168.0.1**, `root` / `000000` | Lab only | Change on first login |

### 2.3 Proposed lab bindings (`apps/ccli/config/lab_tr400.yaml`)

| Signal | Terminal / device | Notes |
|--------|-------------------|--------|
| RS485-A (meter) | **A1/B1** → **`/dev/ttyS1`** | USB-RS485 adapter on PC = Modbus slave |
| RS485-B | **A2/B2** → **`/dev/ttyS2`** | Spare plant bus |
| DO curtail | **DIO1** | External relay module; confirm line via `gpiodetect` |
| DI permissive | **DIO2** | Active-low; jumper to GND = permit |
| Engineering PC | **LAN2** (as wired) | Static 192.168.0.10/24 |

### 2.4 C1–C10 scorecard (TR400)

| Block | TR400 score | Notes |
|-------|-------------|-------|
| C1 | **Partial → Full** | 5 GE ports; firewall + role binding |
| C2 | **Partial/Full** | 2× RS485; A1/B1 wired for L2 |
| C3 | **SW** | libiec61850 + lib60870 + libmodbus |
| C4 | **Partial** | 4 DIO + relay lab; AiLux 5/3 needs expansion |
| C5 | **Partial** | Modem GNSS optional |
| C6 | **Partial** | TPM/SB — verify on device |
| C7 | **Have** | LTE/WAN backup — isolated from Eth_A |
| C8 | **Have** | USB, SSH, TesproOS Web |
| C9 | **Partial** | Industrial enclosure — not final DIN CCI |
| C10 | **Have** | 12–30 V |

---

## 3. Software specification

| Layer | Spec | Status |
|-------|------|--------|
| OS | OpenWrt / TesproOS **24.10.x** | **FROZEN** |
| Language | C/C++17 user-space | **FROZEN** |
| Build | `BUILD_PLATFORM=pi` (Pi) \| `tg500` (TR400) | Pi **ACTIVE**; tg500 HAL **stub** |
| Deploy | `.ipk` + procd (`package/ccli/`) | Pi OpenWrt **HAVE**; TR400 **pending SDK** |
| Config | YAML → UCI (planned) | pf2+io **HAVE**; modbus **MISSING** |

### In-scope protocols (P0)

| Protocol | Interface | Role |
|----------|-----------|------|
| Modbus RTU/TCP | RS485 / plant LAN | Analyzer |
| IEC 61850 MMS | Eth_A | DSO / Annex T |
| IEC 60870-5-104 | Eth_B | Operatore Abilitato |
| PF2 FSM | Internal + DIO | Annex O curtailment |

### Out of scope (disable in TesproOS profile)

DLMS, OPC-UA, DNP3, S7, C37.118 — per `REQ-SW-001`.

---

## 4. Functional requirements

### 4.1 Grid / CEI (P0)

| ID | Requirement | Source |
|----|-------------|--------|
| REQ-PF2-001 | Curtailment FSM: threshold, hysteresis, debounce, min on/off | Annex O |
| REQ-PF2-002 | Stale/invalid meter → safe state; no stale-as-valid | Annex O, R-PF2-001 |
| REQ-PF2-003 | Permissive/interlock before DO | VAL §7 |
| REQ-PF2-004 | Timestamped curtailment events | Annex O |
| REQ-MET-007 | MC200: 200 ms fixed blocks | 61557-12 + Annex O |
| REQ-MET-008 | PF1 TX: 4 s aligned blocks | Annex O |
| REQ-61850-001 | MMS server on Eth_A per Allegato T ICD | Annex T |
| REQ-104-001 | 104 on Eth_B | AiLux manual |

### 4.2 Network (P0)

| ID | Requirement |
|----|-------------|
| REQ-NET-001 | No L2 bridge Eth_A ↔ Eth_B ↔ plant |
| REQ-NET-002 | WAN/LTE backup only — no path to DSO MMS |
| REQ-NET-003 | Silkscreen ↔ Linux ifname ↔ CCI role documented |

### 4.3 Field I/O & serial (P0)

| ID | Requirement |
|----|-------------|
| REQ-SER-001 | Modbus RTU on RS485-A (`ttyS1`) |
| REQ-IO-001 | DO curtailment on DIO1 (lab) |
| REQ-IO-002 | DI permissive on DIO2 (lab) |
| REQ-IO-003 | Boot/shutdown → defined DO state (CR 3.6) |
| REQ-IO-004 | Product: 5× DI 10–120 V, 3× relay DO (expansion if needed) |

### 4.4 Security (P0/P1)

| ID | Requirement |
|----|-------------|
| REQ-SEC-001 | Secure boot verified |
| REQ-SEC-002 | TPM 2.0 key storage |
| REQ-SEC-003 | 62351 on DSO/operator conduits |
| REQ-SEC-004 | 62443-4-2 CR set (SL-T 2 proposed) |
| REQ-SEC-005 | Protected audit trail for control actions |

---

## 5. Development roadmap

Phases are **regulation gates**. Full shall/pass tables: **`CCI_Phase_Regulation_Checklists.md`**.

| Phase | Weeks | Regulation cluster | Goal | Exit (one sentence) |
|-------|-------|--------------------|------|---------------------|
| **0** Platform bind | 1 | Traceability only | Ports, devices, YAML | Silkscreen ↔ Linux ↔ role photographed |
| **1** Local PF2 | 1–3 | **O.9.2** path, **O.13** stale, **CR 3.6** | Modbus → FSM → DIO1 | Local curtail + stale-safe; **not** DSO `Wlim` |
| **2** Isolation | 3–4 | **O.13.1.1.1**, **62443-3-2** | No L2 bridge | Packet test: plant ↛ Eth_A |
| **3** DSO MMS | 4–8 | **T**, **O.9.2.2**, **62351-3/4/8/9** | MMS + `Wlim` on Eth_A | DSO client write `Wlim` over TLS |
| **4** Operator 104 | 6–10 | **O.13 Eth_B**, **60870-5-104** | lib60870 on Eth_B | Session + reconnect; **or** phase waived |
| **5** Observability + Q | 8–12 | **O.8**, **T.3.1.3 P/Q/V**, **O.9.1**, **61557-12** | PF1 MMXU + reactive DSO | 4 s TotW+**TotVAr**; VArSd or plant-Q GAP |
| **6** Defence I/O | 10–14 | **M**, **O.11**, **REQ-IO-004** | Teletrip inhibit + C4 | FSM blocks on M trip; product I/O or GAP |
| **7** Evidence | 12–16+ | **O.14**, **O.15**, **ARERA** | Logger + cert pack | 2048 events + DSO demo script |

**Critical path:** 0 → 1 → 2 → 3. Phase 4 optional vs CEI core. Phase 1 **must not** be signed as Annex T PF2.  
**Phase 5** = **P5-M** (TotW/TotVAr/PPV) + **P5-R** (O.9.1 reactive). Gates: `CCI_Phase_Regulation_Checklists.md` · gap report: `lab/CCI_Reactive_Roadmap_Gap_Report.md`.

---

## 6. Regulatory validation matrix

| Regulation | What to validate | Level | Pass criteria |
|------------|------------------|-------|---------------|
| **CEI 0-16 Annex O** | PF2, observability, timing | L1→L3 | Debounce, safe on stale; MC200/PF1 |
| **CEI 0-16 Annex T** | 61850 ICD/MMS | L2→L3 | Client read; time quality |
| **AiLux manual** | End-to-end behaviour | L2→L3 | Req ↔ test traceability |
| **61557-12** | Fixed blocks, EPMF | L1→L3 | Non-sliding 4 s blocks |
| **60870-5-104** | Operator telecontrol | L2 | Supervision, reconnect |
| **62351-3/4/5** | TLS on 104/61850 | L2 | No cleartext on DSO/OA |
| **62351-14** | Audit syslog | L2 | DO/modbus events persisted |
| **62443-3-3 / 4-2** | CR 3.6 DO, zones | L2 | Predetermined DO; no WAN→Eth_A |
| **62443-4-1** | SDLC evidence | Process | Threat model + test records |

**Golden rule (VAL §9):** Comm fault must **not** cause uncontrolled curtailment or silent stale data.

### Mandatory PF2 tests

| ID | Stimulus |
|----|----------|
| L1-P-03 | P > threshold → curtail after debounce |
| L1-C-01 | Meter timeout → SafeState, DO off |
| L1-C-03 | Bad CRC → stale, not Good |
| VAL §7 | Reboot → defined DO; permissive gates actuation |
| VAL §9 | Eth/GNSS/LTE loss → bounded behaviour |

---

## 7. Traceability

| Topic | Document |
|-------|----------|
| TR400 OEM manual | `CCI_TR400_Extract.md` · `ccli-tr400-user-manual` |
| Platform freeze | `CCI_TG500_Lab_Platform.md` |
| Validation ladder | `CCI_Validation_and_Mocking_Strategy.md` |
| AiLux HW/protocol class | `CCI_Module_Regulations_Classification.md` |
| Application layout | `CCI_Application_Structure.md` |
| Cyber zones | `CCI_62443_Zones.md` |
| **Phase × regulation checklists** | **`CCI_Phase_Regulation_Checklists.md`** |

---

## Revision history

| Rev | Date | Notes |
|-----|------|-------|
| 1.2 | 2026-09-26 | Phase 5 owns TotVAr + O.9.1 reactive (P5-M / P5-R); was unassigned |
| 1.1 | 2026-09-22 | Phases renamed to regulation clusters; checklists in `CCI_Phase_Regulation_Checklists.md` |
| 1.0 | 2026-09-15 | Initial spec — TR400 on bench; AiLux-class target |

## Keywords

`CCLI`, `TR400`, `TG-524`, `AiLux`, `CEI 0-16`, `PF2`, `roadmap`, `ccli-product-spec-roadmap`
