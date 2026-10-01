# CCI Competitors — Hardware Validation Requirements

**Document ID:** CCLI-COMP-003  
**Revision:** 1.0  
**Date:** 2026-09-24  
**RAG source_id:** `ccli-competitors-hw-validation`  
**Platform under test:** TesPro **TG544 / TR500** (+ planned Eth_A media converter)  
**References:** `ccli-competitors-hw-extract` · `ccli-architecture-framework` · `cei-0-16-allegato-o` · `ccli-cci-module-regs-class`

---

## Purpose

Turn competitor HW claims into a **lab validation checklist** for CCLI. Pass = evidence on TG544 (or approved extension), not brochure text.

---

## Requirements (validation IDs)

Derived from competitor bar + Annex O/T + AiLux class.

### V-NET — Ethernet & isolation

| ID | Requirement | Competitor bar | TesPro today | Validation test | Pass criteria |
|----|-------------|----------------|--------------|-----------------|---------------|
| **V-NET-001** | No L2 bridge / no data exchange Eth_A ↔ Eth_B ↔ plant ↔ eng | Higeco **“No switch/bridge”**; Tesmec plant bus separated; O.13.1.1.1 | Shared MT7531 + zones | Flood/ping between role ports; capture | **0** frames / **blocked** ICMP |
| **V-NET-002** | Distinct L3 domains per role | All cabinets | `lan` / `lan_sec` / … | `ip addr` + firewall zones | Separate subnets; **no** inter-zone forward |
| **V-NET-003** | Eth_A optical **100BaseFX** (MM ~1310 nm) | Tesmec FX; Higeco SFP; MC Cu/fibre | Copper only | With **IMC-101-M-SC-T** (or equal) | Link up FX; MMS only on Eth_A |
| **V-NET-004** | Eth_B dedicated operator path | AiLux / Tesmec | Map one LAN | Role binding table + photo | Documented LAN ↔ role |
| **V-NET-005** | Plant Modbus on **non-DSO** path | Tesmec / MC Modbus TCP | RS485 and/or plant LAN | Poll meter; no path to Eth_A | Data OK; Eth_A quiet |
| **V-NET-006** | Port map silkscreen ↔ `ip link` | All | **OPEN** | Label photo + `ip link` | Table signed |

### V-IO — Digital / serial

| ID | Requirement | Competitor bar | TesPro today | Validation test | Pass criteria |
|----|-------------|----------------|--------------|-----------------|---------------|
| **V-IO-001** | DI field class for DG/DI status | AiLux **5× 10–120 V**; others 13–16 DI @ ~110 V | **4× DIO** ~24 V TVS | Scope / DMM; expansion if needed | Lab: 2 DI OK; **product:** plan expansion |
| **V-IO-002** | DO curtailment dry contact | AiLux **3× relay 250 Vac** | Opto+relay OEM | Curtail ON/OFF load | Relay actuates; safe state documented |
| **V-IO-003** | RS485 opto / surge evidence | AiLux opto RJ45; Tesmec field bus | Isolation **OPEN** | Ask TesPro + hipot or DS | Written isolation class |
| **V-IO-004** | Terminator / A-B polarity | All Modbus RTU users | Lab practice | Analyzer poll | Stable CRC; no bus errors |

### V-TIME — Clock

| ID | Requirement | Competitor bar | TesPro today | Validation test | Pass criteria |
|----|-------------|----------------|--------------|-----------------|---------------|
| **V-TIME-001** | GNSS (or NTS) — not plain NTP alone | Higeco FAQ Q05; all have GPS | Brochure GNSS | `gpsd` / NMEA / lock | Fix + ≤ ±100 ms vs O.13.5 |
| **V-TIME-002** | Holdover drift | Annex O | — | Disconnect GNSS | ≤ 1 s/day claim path |

### V-SEC — Cyber / HW root of trust

| ID | Requirement | Competitor bar | TesPro today | Validation test | Pass criteria |
|----|-------------|----------------|--------------|-----------------|---------------|
| **V-SEC-001** | Secure Boot | AiLux / Higeco | Brochure | Boot unsigned → fail | Policy evidence |
| **V-SEC-002** | TPM 2.0 | AiLux / FAQ FIPS L3 | Brochure | `tpm2_getcap` | Device present |
| **V-SEC-003** | 62443-4-1 / 4-2 process+component | Higeco cert claims | Programme PART | SDL artefacts | Gate before ship |
| **V-SEC-004** | Firewall default deny across roles | Higeco / O.13.1 | nftables | Zone matrix dump | No lan↔eth_a forward |

### V-PWR / ENV

| ID | Requirement | Competitor bar | TesPro today | Validation test | Pass criteria |
|----|-------------|----------------|--------------|-----------------|---------------|
| **V-PWR-001** | DC input class 12–36 V (AiLux 12–24) | Cabinets often 230 Vac / 110 Vdc | **12–30 V** | Brown-out / range | Operates in range |
| **V-ENV-001** | EMC / insulation lab | Higeco FAQ ISO **17025** | — | Plan CE file | Lab booked |
| **V-ENV-002** | Functional conformity body | ISO **17065** | — | Plan | Body identified |

### V-MET — Measurement chain (system, not only gateway)

| ID | Requirement | Competitor bar | TesPro today | Validation test | Pass criteria |
|----|-------------|----------------|--------------|-----------------|---------------|
| **V-MET-001** | POC P/Q/V via dedicated MU | Higeco Class 0.2S PA; FAQ no PI/PG CTs | External analyzer Modbus | Register map + accuracy | Error within Annex O / site class |
| **V-MET-002** | 4 s / timing observability | Annex O / T | `ccli` PF1 path | Timestamped logs | Cadence met |

---

## Architecture (validation topology)

```
DSO fibre ── IMC-101 FX ── TG544 Eth_A zone
OA / eng  ─────────────── TG544 Eth_B / eng zones
Plant     ── RS485 / plant LAN ── TG544 plant zone
Meter     ── Modbus ───────────── plant only
GNSS      ── TG544 or external NTS
```

Competitor cabinets often bundle switch + converter + analyzer; CCLI may keep analyzer external — **V-MET** still mandatory.

---

## BOM matrix (validation assets)

| Block | Function | Candidate | Status |
|-------|----------|-----------|--------|
| DUT | CCI compute | TG544 | **ACTIVE** |
| Eth_A FX | Annex O media | **Moxa IMC-101-M-SC-T** | **SELECT** |
| Plant sim | Modbus slave | PC / analyzer | Lab |
| Capture | Isolation proof | Wireshark + 2 NICs | Lab |
| GNSS | Time | Active antenna or NTS | **OPEN** |
| I/O expand | AiLux DI/DO class | Wave B / TesPro expand | **GAP** |

---

## Knowledge gaps (block validation)

| Gap | Blocks | Action |
|-----|--------|--------|
| Shared MT7531 vs Higeco “no bridge” narrative | V-NET-001 | Packet evidence + architecture note |
| No FX on DUT | V-NET-003 | Order IMC-101 |
| 4 DIO ≠ 5×10–120 V / 3×250 Vac | V-IO-001/002 | Product expansion plan |
| RS485 isolation unproven | V-IO-003 | TesPro V-007 |
| GNSS not verified on unit | V-TIME-001 | Lab bring-up |
| UCA 61850 cert | Competitor Higeco bar | Phase 3 cert plan |

---

## Risks

| ID | Description | Impact | Mitigation |
|----|-------------|--------|------------|
| R-COMP-001 | Auditor expects cabinet FX + multi-MAC like Tesmec | Cert delay | IMC-101 + V-NET-001 evidence pack |
| R-COMP-002 | DI voltage class fail field | Incomplete PF2 status | Expansion module |
| R-COMP-003 | NTP-only time sync | O.13.5 fail | GNSS/NTS before cert |
| R-COMP-004 | Using plant switch that bridges to Eth_A | O.13.1 fail | Never shared L2 with DSO |

---

## Verification campaign order (lab)

1. **V-NET-006** port map  
2. **V-NET-002** zones + **V-NET-001** isolation packet test  
3. **V-NET-003** FX converter (when available)  
4. **V-IO-002** curtail DO + **V-IO-004** Modbus RTU  
5. **V-TIME-001** GNSS  
6. **V-SEC-002/004** TPM + firewall dump  
7. **V-MET-001** analyzer accuracy (with Class CT/VT)  
8. Document gaps V-IO-001 product path  

---

## Verdict for CCLI HW

| Area | vs competitors | Decision |
|------|----------------|----------|
| Compute / RAM | TG544 **≥** AiLux | OK |
| Logical isolation | Must match Higeco/Tesmec **intent** | Validate V-NET-001 |
| Eth_A FX | Behind Tesmec/Higeco/MC | **Extension IMC-101** — do not redesign TesPro |
| DI/DO class | Behind AiLux/cabinets | **Expansion** — not LAN redesign |
| Cyber claims | Behind Higeco cert marketing | SDL + TPM evidence |

**Do not change TesPro LAN silicon for competitor parity.** Extend for **FX (V-NET-003)** and **I/O class (V-IO-001/002)**; prove **no bridge (V-NET-001)**.

---

`HW validation`, `V-NET-001`, `competitors`, `Higeco`, `Tesmec`, `IMC-101`, `ccli-competitors-hw-validation`
