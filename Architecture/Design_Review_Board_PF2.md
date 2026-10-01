# Design Review Board — PF2 / CEI 0-16 CCI

**Document ID:** CCLI-DRB-001  
**Revision:** 1.0  
**Date:** 2026-07-26  
**Role:** Senior Hardware Architect + Design Review Board  
**Corpus:** `Architecture/*`, `architecture.db`, `knowledge-base/08-engineering/*`, SK0146 Test Bench crosswalk  
**Verdict:** Functional architecture directionally correct. **NO-GO for Architecture Freeze / schematic capture.**

---

## Executive summary

| Layer | Status |
|-------|--------|
| SoM (Toradex Verdin **0063**) | Locked — lab path clear |
| Eval kit **99991105** | Lab only — must not be treated as product |
| Carrier C1–C10 | Blocks named; **MPNs TBD**; C1 topology unresolved |
| Pin budget D3 / Carrier freeze D4 | **BLOCKERS** |
| Knowledge (61850, Secure Boot, Ethernet, 62443) | Critical gaps — see Knowledge_Risks sheet |
| Overall readiness | **4/10** — Wave A lab GO; Architecture Freeze NO-GO |

### Top 5 challenges

1. Freeze **C1 topology** (managed switch vs dual PHY + 2× external PHY) — write ADR before BOM.  
2. Author **pin budget (D3)** before any carrier MPN selection.  
3. Split **IEC 61850 MMS vs GOOSE** in requirements and verification.  
4. Close **HAB + secure element + commercial 61850 license** as one security/legal package.  
5. Enforce **kit ≠ product** on every BOM line, test report, and EMC scope.

---

## 1. Architecture Validation

### 1.1 Missing blocks

| Missing | Impact | Gate |
|---------|--------|------|
| Pin budget (SoM ball → net → connector) | Cannot place PHY/UART/GPIO/PPS | D3 P0 |
| Altium-ready carrier freeze drawing | Schematic premature | D4 P0 |
| C1 topology ADR | Eth_A/Eth_B isolation ambiguous | Architecture Freeze |
| Independent HW failsafe / WDT for DO | PF2 DO hangs with FW crash | Reliability |
| Surge / EMC front-end class (RJ45, RS485, DI) | Certification / field failure | Pre-PCB |
| IEC 62443 zones & conduits diagram | Security compliance gap | KR-012 |
| Power load / thermal budget | Overheat / brownout | Pre-PCB |
| Toradex PCN longevity for 0063 | Lifecycle risk | D6 P0 |

### 1.2 Incorrect assumptions (challenged)

| Assumption | DRB challenge |
|------------|---------------|
| Kit 99991105 validates product interfaces | Kit has 2× GbE + non-opto DB9 RS485. **Does not** prove 4 Ethernet domains, opto RS485, DI 10–120 V, or product EMC. |
| VLAN alone = CEI outside-interface separation | Annex O/T expects separate outside interfaces. Shared L2 + VLAN may fail audit. |
| SK0146 DI/DO tests transfer to CCI | SK0146 = 3.3 V contact sim; CCI = 10–120 V opto / dry contact. Reuse craft, **not** limits. |
| GOOSE works on Wave A unmanaged switch | GOOSE is multicast / timing-sensitive; needs explicit L2 design and isolation from plant. |
| GPLv3 libiec61850 is shippable | Lab OK; product needs commercial license (R-LIC-001). |
| “Secure Boot checked” without key ceremony | HAB silicon ≠ product secure boot without fuses, signed CI, SE key storage. |

### 1.3 Over-engineering

- **LTE on rev A** — correctly Optional; do not delay C1/C2/C4/C10.  
- **SK0146 features on CCI** (keypad, IR, DLMS, valve) — out of PF2 scope.  
- **IEEE 1588 HW timestamping** — elevate only if utility requires peer sync; current REQ is GNSS NMEA/PPS-centric.  
- **4 Ethernet domains** — required by CEI, not over-engineering; but buying an industrial managed switch without ADR is premature.

### 1.4 Under-engineering

- C1 still “switch **or** PHY” while Eth_A/Eth_B are Critical.  
- Secure element MPN / SoM TPM SKU path unresolved with REQ-SEC-002 Critical.  
- Power tree conceptual — no inrush, hold-up, brownout, relay coil budget.  
- No REQ for 62443 SL target or GOOSE-specific verification case.  
- No independent DO failsafe path for curtailment.

---

## 2. Interface Validation

### 2.1 Present (Interface_Matrix / architecture.db)

Eth_A · Eth_B · Plant-1 · Plant-2 · RS485-A/B · DI-1..5 · DO-1..3 · GNSS · LTE · USB · CAN (kit only, not product).

### 2.2 Missing / weak interfaces

| Interface | Gap |
|-----------|-----|
| GNSS **PPS** to SoM GPIO | Not in pin budget |
| Eth_A/Eth_B **physical** segregation | Silicon path not locked |
| DI field supply / return / surge | Polarity, common-mode, clamp class open |
| DO contact rating | VA / inductive load for contactors unspecified |
| IEC 60870-5-104 role | On Eth_B protocol list; no dedicated REQ-ID |
| Service USB attack surface | Out of cert path but needs 62443 controls |

### 2.3 Isolation requirements (freeze)

1. Eth_A ↛ Eth_B ↛ Plant — no L2 bridge.  
2. RS485 galvanic isolation on **carrier** (not kit DB9).  
3. DI opto + external loop supply.  
4. DO relay isolation from SoM.  
5. LTE logical separation from DSO path (firewall / policy).

### 2.4 Critical communication paths

```
DSO Eth_A (MMS/GOOSE) → PF2 logic → DO curtailment     [safety-critical]
Analyzer RS485 Modbus → observability / PF2 inputs     [stale-data risk]
Operator Eth_B (61850/104)                             [must not bleed to Eth_A]
GNSS → PPS/NMEA → event timestamps                     [stub on kit]
```

---

## 3. BOM Review

**Rule:** Purchasing links are meaningless until MPN is locked. Sources below are **search channels**, not part URLs.

| Block | Recommended | Alternative | Lifecycle | Supply risk | Est. EUR | Sources | Recommendation |
|-------|-------------|-------------|-----------|-------------|----------|---------|----------------|
| SoM | Toradex Verdin Plus IT **0063** | Variscite DART-MX8M-PLUS (compare only) | IT SKU; **PCN letter MISSING** | Single-vendor SoM | 150–350 | Toradex FAE / webshop | **KEEP** — obtain 10y PCN (D6) |
| Lab DUT | Dev Board **99991105** | — | Lab only | — | ~300 | Toradex | **LAB ONLY** — never field BOM |
| C1 Ethernet | Decide ADR first: industrial 4-port managed switch **or** SoM 2× GbE + 2× PHY | Microchip/Marvell/Realtek industrial classes | Prefer >10y industrial | **High** until chosen | Switch often 15–80+ (BOM 8–20 underestimates) | Mouser, DigiKey, Farnell, Arrow | **STOP** — topology ADR before MPN |
| C1 magjacks | Industrial RJ45 magjack ×4 | Integrated magnetics PHY | Watch EOL | Medium | 4–10 ×4 | DigiKey, Mouser | After C1 ADR |
| C2 RS485 | Isolated transceiver ×2 (ISO14xx / ADM2795E-class / similar) | Discrete opto + RS485 | Prefer industrial temp | Medium | 10–25 ×2 | DigiKey, Mouser | Product must be opto-isolated RJ45 — not kit DB9 |
| C4 DI | 10–120 Vdc opto conditioner ×5 | Discrete opto + clamp | — | Medium | 8–15 ×5 | DigiKey, Mouser | Validate 24 V first (Wave B) |
| C4 DO | Electromechanical relay dry contact ×3 (250 Vac class) | SSR (leakage caveat) | Contactor load class | Medium | 5–15 ×3 | DigiKey, Mouser | Spec inductive surge / VA |
| C5 GNSS | u-blox-class + PPS | Quectel GNSS | Module EOL common | Medium–High | 20–35 + antenna 5–15 | DigiKey, Mouser | Need industrial temp + PPS |
| C6 SE | Pick one: ATECC608B **or** STSAFE **or** SE050 **or** on-SoM TPM | — | — | Medium | 2–8 | DigiKey, Mouser | Confirm Verdin BSP support before schematic |
| C6 HAB | i.MX8M Plus HAB (on SoM) | — | Silicon feature | — | 0 | NXP / Toradex docs | Pair with signed-image CI |
| C7 LTE | Quectel Cat.1 class (rev A) | — | Regional SKU risk | High | 25–40 | DigiKey, Mouser | Optional — do not gate freeze |
| C10 PSU | Isolated 12–24 Vdc industrial module | Discrete buck + isolator | Critical path | High | 15–40 (BOM 10–25 tight) | DigiKey, Mouser, Farnell | Size for SoM + PHY + relays + inrush |
| C3 stack | libiec61850 lab | MZ Automation commercial | License risk | **Legal High** | 0 + quote | MZ Automation | Quote before product |
| Mech | DIN-rail enclosure TBD | — | — | Medium | 15–40 | RS, Farnell | After thermal envelope |

**BOM readiness:** Lab ~4/10 · Production ~2/10.

---

## 4. Security Review

| Topic | Assessment | Action |
|-------|------------|--------|
| Secure Boot | Right silicon (HAB). Knowledge KR-005 Critical. No fuse/key-ceremony doc. | Study HAB + Toradex signed-boot; write key hierarchy before HW freeze |
| Key storage | Filesystem keys = R-SEC-001 High | SE or TPM path mandatory for product |
| Secure Element | ATECC / STSAFE / SE050 / on-SoM TPM unresolved | Prototype one path on Wave A/B carrier stub |
| IEC 62443 | Taxonomy HAVE; **no SL-T / zones-conduits** for Eth_A/B/plant/USB/LTE | Architecture review + KR-012 learning |
| IEC 62351 | Mentioned; not in verification cases | Add TLS/auth test when stack chosen |
| USB console | Correctly out of cert path | Physical access + RBAC still required |

**Challenge:** “Secure Boot + SE” checkboxes without signed CI and key ceremony are theater.

---

## 5. Reliability Review

| Area | Finding | Mitigation |
|------|---------|------------|
| SPOF | Single SoM, single PSU, single DO path, possible single switch | Document degraded modes; consider HW WDT→DO safe state |
| Thermal | No cabinet thermal model | KR-013 before PCB; measure kit + estimate carrier dissipation |
| Watchdog | SoM WDT assumed only | Require HW WDT feeding safe-state for PF2 DO |
| Power integrity | Incomplete: brownout, sequencing, eFuse, relay inrush | Expand Power_Tree with currents and hold-up |
| Time | No GNSS on kit | Mark L2 timestamp tests stub (R-TIME-001) |
| EMC | Kit serial ≠ product C2 | Re-test on carrier (R-EMC-001) |

---

## 6. Knowledge Gap Review

### 6.1 Heat map

**Critical (delay whole project):** IEC 61850 MMS · IEC 61850 GOOSE · Carrier board · Secure Boot · Ethernet architecture · IEC 62443  

**High (before architecture freeze):** IEC 104 · Linux networking · Secure element · PTP (if required) · Linux boot chain  

**Medium (before PCB):** GNSS · Thermal · EMC Ethernet  

**Low (reuse from ATEX/SK0146):** LTE · RS485 · Modbus · Power protection basics  

### 6.2 Reuse matrix (ATEX / SK0146 → CCI)

| Area | Reuse | Caution |
|------|-------|---------|
| RS485 / LTE / DI / DO / power protection | High | Voltage/isolation classes differ |
| EMC basics | Medium | Re-qualify product connectors |
| Linux SoM / Carrier / 61850 / Secure Boot / Eth segmentation / 62443 | **New** | Primary learning load |

### 6.3 Required documents (P0)

| Doc | source_id / path | Status |
|-----|------------------|--------|
| Pin budget | `ccli-pin-budget` | MISSING |
| Carrier block freeze | `ccli-carrier-block` | MISSING |
| IMX8MPIEC | `ccli-nxp-imx8m-plus-iec` | MISSING |
| Toradex PCN 0063 | vendor PDF | MISSING |
| C1 switch/PHY DS | after MPN | MISSING |
| SK0146 schematics in RAG | `sk0146-*` | NOT INGESTED |
| Verdin Carrier Guide | `ccli-toradex-carrier-guide` | HAVE |
| CEI O/T | `cei-0-16-allegato-o/t` | HAVE |
| Validation strategy | `ccli-validation-strategy` | HAVE |

### 6.4 Learning plan

| Priority | Topic | Target before |
|----------|-------|---------------|
| Critical | IEC 61850 MMS + GOOSE lab | Architecture Freeze |
| Critical | Ethernet C1 topology | Architecture Freeze |
| Critical | Secure Boot + key ceremony | Hardware Freeze |
| Critical | Carrier + pin budget | Schematic Capture |
| Critical | IEC 62443 zones | Architecture Freeze |
| High | IEC 104 simulator | Firmware Development |
| High | Secure element prototype | Hardware Freeze |
| Medium | Thermal / EMC | PCB Layout |
| Medium | GNSS/PPS (PTP if required) | System Integration |

Living Excel: `Architecture/Knowledge_Matrix.xlsx` sheets **Knowledge_Matrix**, **Knowledge_Risks**, **Learning_Plan**, **Document_Mapping**.

---

## 7. Verification Plan

### 7.1 Required tests (by layer)

| Layer | Tests | Pass criteria |
|-------|-------|---------------|
| **Hardware** | 12–24 V sweep; brownout; DI 24 V then range; DO load; RS485 isolation (product) | No rail collapse; no false PF2; isolation rating met |
| **Communication** | Eth_A MMS; Eth_B 104/61850; Plant Modbus TCP; RS485 Modbus; **GOOSE**; Eth_A↔Eth_B crosstalk | No L2 bleed; GOOSE within timing budget |
| **PF2** | Meter/Eth/GNSS stale matrix | Defined safe state; documented DO behavior |
| **Security** | Unsigned image reject; SE ops; USB lockdown | Only signed boots; no plaintext private keys |
| **Time** | GNSS PPS event stamp (carrier) | Sub-second ordering — kit = stub |

### 7.2 Bench waves (existing)

- **Wave A (BUY NOW):** L1 software mock + L2 Eth/RS485 HIL on kit  
- **Wave B:** DI box 24 V + DO lamps  
- **Wave C:** Real analyzer  
- **Wave D:** Power HIL — defer  

Do **not** claim CEI readiness from Wave A unmanaged switch + kit DB9.

---

## 8. Final Design Readiness

| Dimension | Score (0–10) | Rationale |
|-----------|-------------|-----------|
| Requirements | **7** | CEI domains covered; GOOSE/104/62443 SL thin |
| Architecture | **5** | C1–C10 clear; C1 + D3/D4 not frozen |
| BOM | **2** | SoM/kit only; carrier TBD |
| Verification | **5** | Strategy good; GOOSE/SE/EMC incomplete |
| Production readiness | **1** | No pin budget, locked MPNs, PCN, or enclosure |

**Overall: 4/10**

### Go / No-Go

| Decision | Status |
|----------|--------|
| Buy Verdin kit + SoM 0063 | **GO** |
| Buy Wave A mocking bench | **GO** |
| Architecture Freeze | **NO-GO** |
| Schematic / Gerbers | **NO-GO** |
| Carrier MPN campaign | **NO-GO** until pin budget + C1 ADR |

---

## Document control

| Artifact | Path |
|----------|------|
| This review | `Architecture/Design_Review_Board_PF2.md` |
| Knowledge risks Excel | `Architecture/Knowledge_Matrix.xlsx` |
| Traceability DB | `Architecture/architecture.db` |
| Framework | `knowledge-base/08-engineering/CCI_Architecture_Framework.md` |

**RAG source_id (when ingested):** `ccli-design-review-board-pf2`
