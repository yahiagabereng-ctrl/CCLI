# Document Coverage Matrix — PF2 CCI

**Document ID:** CCLI-DOC-COV-001  
**Revision:** 1.1  
**Date:** 2026-09-15  
**Role:** Chief Hardware Architect  
**Excel:** `C:\Yahia\projects\CCLI\Architecture\Document_Coverage_Matrix.xlsx`  
**Corpus:** DOWNLOADS.md · CCI_Prototype_BOM.md · Architecture/* · Design_Review_Board_PF2.md

---

## Document Coverage Matrix (by subsystem)

| Block | Subsystem | Required knowledge | Available documents | Missing documents | Priority | Impact if missing | Status |
|-------|-----------|--------------------|---------------------|-------------------|----------|-------------------|--------|
| LAB | TesPro **TR400 / TG-524** (OpenWrt lab DUT) | Port map, RS485 (`ttyS1/S2`), DIO1–4, OpenWrt SDK, Secure Boot / TPM | TG-500 DS; TR400 manual extract; lab platform Rev 3.0; product spec | OpenWrt SDK; DIO gpio map; SKU TR400 vs TG-424 Pro | P0 | L2 blocked until DIO map + optional SDK | **PARTIAL — TR400 ON BENCH** |
| MOCK | Raspberry Pi (L1 mock) | Same test vectors as TR400; pymodbus + libiec61850 native build | Validation strategy; mocking bench BOM; `apps/ccli` + `lab_pi.yaml` | TR400 `lab_tr400.yaml` GPIO lines; Pi validation runbook | P1 | L1 only — L2+ needs TR400 | PARTIAL |
| C1 | Ethernet — Eth_A / Eth_B / Plant×2 | 4-domain isolation; switch vs dual-PHY; GOOSE L2; magjacks; EMC Ethernet | CEI Allegato O/T; Network_Architecture.drawio; Interface_Matrix; Prototype BOM category; Validation strategy | C1 topology ADR (locked); Switch/PHY DS (MPN); Magjack DS; Pin budget for Ethernet pins | P0 | Wrong segmentation → grid rejection; GOOSE/MMS failure; carrier respin | MISSING |
| C2 | 2× Isolated RS485 Modbus RTU | Opto-isolated transceiver; RJ45 8/8 plant serial; EMC; termination | Vendor selection C2; Interface_Matrix; SK0146 RS485 sheet (on disk, NOT in RAG); ATEX reuse craft | Isolated RS485 transceiver DS (MPN); Connector pinout freeze; SK0146 RAG ingest | P1 | Schematic hold on C2; wrong EMC scope if kit DB9 used as product proof | MISSING |
| C3 | Protocols — IEC61850 / IEC104 / Modbus | MMS, GOOSE, 60870-5-104, Modbus TCP/RTU; commercial 61850 license | CEI O/T; Validation strategy; GitHub libiec61850/lib60870 refs; Mocking bench SW list; Regs classification | GOOSE lab runbook; IEC104 stack choice + test vectors; MZ commercial license quote; Analyzer Modbus map | P0 | Cannot prove DSO/operator path; legal risk shipping GPLv3; plant poll blocked | PARTIAL |
| C4 | DI 10–120 Vdc ×5 + DO dry contact ×3 | Opto DI conditioner; relay class; surge; PF2 failsafe; Wave B fixture | Vendor selection C4; Validation + Mocking Bench BOM Wave B; SK0146 DIN/DOUT (craft only) | Opto DI DS (MPN); Relay DS (MPN); Contact VA rating; HW WDT→DO failsafe note | P1 | False curtailment / unsafe DO; cannot freeze schematic C4 | MISSING |
| C5 | GNSS time sync (NMEA + PPS) | Module UART/I2C; PPS GPIO; antenna; industrial temp; optional PTP | Vendor selection C5; Prototype BOM category; Power_Tree stub; Validation L3 deferred note | GNSS module DS (MPN); Antenna DS; PPS pin in pin budget; PTP requirement decision | P1 | Event timestamp mismatch; L3 time tests blocked | MISSING |
| C6 | Secure Boot + Secure Element | HAB fuse policy; signed image CI; SE/TPM key storage; 62443 zones | TG-524 Secure Boot / TPM brochure; Architecture framework; Regs class; protocol libs | Product key hierarchy doc; SE/TPM DS (MPN or SoM SKU proof); IEC 62443 SL/zones diagram; 62351 test plan | P0 | Cyber assessment fail; keys in filesystem (R-SEC-001) | PARTIAL |
| C7 | LTE backup (optional rev A) | Cat.1 module; antenna; SIM; firewall vs Eth_A | Prototype BOM partial; Quectel-class refs in Telematry; Vendor selection optional | CCI-locked LTE module DS; RF/antenna layout guide for carrier | P3 | Rev A slip only — must not block Architecture Freeze | PARTIAL |
| C8 | Local console / commissioning USB | USB out of cert path; RBAC; physical access | Dev Board USB; Architecture framework; Interface_Matrix | Commissioning security procedure (62443) | P2 | Attack surface if USB unconstrained | PARTIAL |
| C9 | Mechanical / DIN-rail enclosure | Thermal envelope; connector placement; IP rating if required | Prototype BOM category; Architecture mech note | Enclosure drawing; Thermal budget; Connector family freeze | P2 | PCB/enclosure mismatch; thermal failure in cabinet | MISSING |
| C10 | Power — 12–24 Vdc isolated input | Isolated DC-DC; sequencing; eFuse; brownout; load budget; DI field supply independence | Power_Tree.drawio (conceptual); Prototype BOM C10 category; Kit 7–24 V note | PSU module DS (MPN); Current/load budget; Hold-up / brownout analysis | P1 | Field brownout; relay dropouts; SoM reset under load | MISSING |
| SYS | System architecture & CEI PF2 | Annex O/T PF2; Eth domains; curtailment safe-state; verification L1–L4 | Architecture Framework; DRB CCLI-DRB-001; draw.io set; Interface/BOM/Risk/Verification matrices; architecture.db; CEI O/T | Pin budget; Carrier freeze doc; C1 ADR locked | P0 | Architecture Freeze remains NO-GO | PARTIAL |
| VAL | Validation / mocking bench | Wave A–D fixtures; GOOSE/MMS/104/Modbus tests; evidence paths | Validation strategy (INGESTED); Mocking Bench BOM (INGESTED); Verification_Matrix | GOOSE test procedure; IEC104 client choice; Real analyzer register map; Evidence folder convention | P1 | False confidence from kit-only tests; delayed L2/L3 | PARTIAL |
| SEC | Cybersecurity / IEC 62443 / CRA | Zones/conduits; SL-T; CRA; secure boot ceremony | Regs classification; EU CRA PDF on disk; RED directive PDF; GitHub security layers | 62443 zones diagram; SL target; Licensed 62443/62351 extracts; Key ceremony SOP | P0 | Certification delay; weak product security story | PARTIAL |
| FIELD | Custom field CCI (deferred) | Future product carrier — not Phase 1 lab | Historical study in _archive/som-study/ | Field product architecture TBD | — | N/A for lab | DEFERRED |

---

## 1. Missing Documents Report

**Total missing/partial actions:** 25 · **P0:** 9 · **P1:** 11 · **P2/P3:** 5

### P0 — Architecture / Hardware Freeze blockers

| ID | Document | Subsystem | Impact | Action | Gate |
|----|----------|-----------|--------|--------|------|
| MD-001 | TesPro OpenWrt SDK | LAB | Cannot cross-compile for TG-524 | Request with TG-524 PO | Lab bring-up |
| MD-002 | TG-424 vs TG-524 SKU confirmation | LAB | Wrong device ordered | Confirm with TesPro FAE | PO |
| MD-003 | Lab port map (Eth_A/B/plant labels) | LAB | Blocks L2 HIL wiring | Label ports on receipt | L2 |
| MD-004 | CCI Carrier Block Diagram (frozen) | Carrier | No Altium-ready freeze; C1/C2/C4/C5 open | Export/freeze from Hardware_Architecture.drawio | D4 |
| MD-005 | C1 Ethernet topology ADR (locked) | C1 Ethernet | Wrong network segmentation; GOOSE failure | DRB decide switch vs dual PHY; update ADR-005 | Arch Freeze |
| MD-014 | Product key hierarchy / HAB ceremony SOP | C6 Security | Secure Boot theater without process | Author before Hardware Freeze | HW Freeze |
| MD-015 | IEC 62443 zones & conduits + SL target | Security | Security compliance gap | Architecture workshop; map Eth_A/B/plant/USB/LTE | D-CERT |
| MD-018 | GOOSE lab test procedure | C3 / Validation | Incorrect curtailment/event behavior | Author from validation strategy + Wave A | Arch Freeze |
| MD-022 | SK0146 schematic RAG ingest | Reuse / Bench | Knowledge leak during FCT/debug; lost reuse | Run ingest-sk0146-schematics.ps1 | D-SK0146 |

### P1 — Needed before schematic / firmware

| ID | Document | Subsystem | Impact | Action | Gate |
|----|----------|-----------|--------|--------|------|
| MD-006 | Ethernet switch / PHY datasheet | C1 Ethernet | Cannot complete C1 schematic | Select MPN after ADR-005 + pin budget | D12 |
| MD-007 | RJ45 magjack datasheet ×4 | C1 Ethernet | EMC/layout risk | Select with PHY/switch | D12 |
| MD-008 | Opto-isolated RS485 transceiver DS ×2 | C2 RS485 | C2 schematic hold | Select industrial isolated MPN | D13 |
| MD-009 | DI opto conditioner DS (10–120 V) | C4 Digital I/O | False PF2 / channel damage | Select after Wave B 24V learnings | D11 |
| MD-010 | DO relay datasheet (dry contact) | C4 Digital I/O | Curtailment contact unreliable | Spec VA + inductive load class | D11 |
| MD-011 | GNSS module + antenna DS | C5 GNSS | No PPS/timestamp path | Select u-blox-class with PPS | D9 |
| MD-012 | 12–24 V isolated PSU / DC-DC DS | C10 Power | Brownout / field failure | Size for SoM+PHY+relays+inrush | D14 |
| MD-013 | Secure element / TPM datasheet | C6 Security | Keys in filesystem; 62443 fail | Pick ATECC/STSAFE/SE050 or prove on-SoM TPM | D15 |
| MD-016 | MZ Automation libiec61850 commercial quote | C3 Protocols | Legal risk shipping GPLv3 | Request quote before product | D-LIC |
| MD-017 | Energy analyzer Modbus register map | C2/C3 | L3 plant serial software blocked | Pick analyzer model; extract map | D8 |
| MD-019 | IEC 60870-5-104 simulator choice + vectors | C3 Protocols | Operator Eth_B integration delay | Pick lib60870 or licensed client; write vectors | FW Dev |

### P2/P3 — PCB / product / optional

| ID | Document | Subsystem | Impact | Action | Gate |
|----|----------|-----------|--------|--------|------|
| MD-020 | Power load + thermal budget | C10 / C9 | Cabinet overheating; PSU undersize | Expand Power_Tree with currents | PCB |
| MD-021 | DIN-rail enclosure drawing | C9 Mechanical | Form-factor / connector clash | Select enclosure after thermal envelope | D18 |
| MD-023 | EMC connector family (RJ45 8/8) part PNs | C1/C2 EMC | Certification failures | Freeze pinout + magnetics family | D19 |
| MD-024 | Altium ActiveBOM export | Carrier | Fab BOM incomplete | Export after D3/D4 + MPN lock | D7 |
| MD-025 | Design Review Board ingest to RAG | SYS | DRB not queryable in BizChat/RAG | ingest-ccli.ps1 for DRB MD | — |

---

## 2. Knowledge Risk Matrix

| KR | Area | Current → Required | Risk | Impact | Priority | Mitigation | Target before |
|----|------|-------------------|------|--------|----------|------------|---------------|
| KR-001 | IEC 61850 MMS | Low → High | High | Communication failure with DSO | Critical | Study IEC61850 docs + lab validation | Architecture Freeze |
| KR-002 | IEC 61850 GOOSE | None → High | High | Incorrect event/curtailment behavior | Critical | Build GOOSE test environment | Architecture Freeze |
| KR-003 | Lab platform integration | Medium → High | Medium | Delayed on-target tests | Critical | Order TG-524; request OpenWrt SDK; label port map | Lab bring-up |
| KR-004 | Linux Boot Chain | Low → High | Medium | System startup failures | High | Learn U-Boot and Linux boot flow on TG-524 OpenWrt | Hardware Freeze |
| KR-005 | Secure Boot | None → High | High | Cybersecurity vulnerability | Critical | Study TG-524 Secure Boot / TPM path with TesPro SDK | Hardware Freeze |
| KR-006 | Secure Element | None → Medium | Medium | Weak key management | High | Prototype SE050 / ATECC608B / STSAFE | Hardware Freeze |
| KR-007 | Ethernet Switch Architecture | Low → High | High | Wrong network segmentation | Critical | Study managed switch design; write C1 ADR | Architecture Freeze |
| KR-008 | IEEE1588/PTP | None → Medium | Medium | Time synchronization errors | High | Confirm CEI need; GNSS/PTP test bench if required | System Integration |
| KR-009 | GNSS Timing | Low → Medium | Medium | Event timestamp mismatch | Medium | Prototype GNSS + PPS integration | System Integration |
| KR-010 | IEC 60870-5-104 | Low → Medium | Medium | Utility integration delay | High | Implement IEC104 simulator | Firmware Development |
| KR-011 | Linux Networking | Medium → High | Medium | Routing/firewall issues | High | Lab validation VLAN/firewall on kit | Architecture Freeze |
| KR-012 | IEC 62443 | None → Medium | High | Security compliance gap | Critical | Security architecture review; zones/conduits | Architecture Freeze |
| KR-013 | Thermal Design | Medium → High | Medium | Overheating in cabinet | Medium | Thermal simulations + power budget | PCB Layout |
| KR-014 | EMC for Ethernet | Medium → High | Medium | Certification failures | High | EMC design review on product carrier | PCB Layout |

### Heat map

| Band | Topics |
|------|--------|
| **Critical** | IEC61850 (MMS+GOOSE); Carrier Board Design; Secure Boot; Ethernet Architecture; IEC62443 |
| **High** | IEC104; Linux Networking; Secure Element; PTP (if required) |
| **Medium** | GNSS; Thermal Design; EMC Optimization |
| **Low** | LTE Integration; RS485; Modbus; Power Supply Basics |

### Reuse from ATEX / SK0146

| Area | Reuse | Caution |
|------|-------|---------|
| RS485 | High | Product needs opto RJ45 — not kit DB9 |
| LTE | High | Rev A optional |
| Digital Inputs | High | Craft only — CCI 10-120V ≠ SK0146 3.3V |
| Relay Outputs | High | Craft only — dry contact rating TBD |
| Power Protection | High | Expand for 12-24V carrier |
| EMC Basics | Medium | Re-qualify product connectors |
| Linux lab SoC | TG-524 ordered/mock | MT798X on OpenWrt |
| Carrier Board | None — New | Custom CCI carrier |
| IEC61850 | None — New | MMS + GOOSE |
| Secure Boot | None — New | HAB + signed images |
| Ethernet Segmentation | None — New | Eth_A / Eth_B / Plant |
| IEC62443 | None — New | SL / zones |

---

## 3. Learning Roadmap

| Order | Priority | Topic | Must complete before | KR |
|------:|----------|-------|---------------------|----|
| 1 | Critical | IEC 61850 MMS + GOOSE | Architecture Freeze | KR-001, KR-002 |
| 2 | Critical | Ethernet C1 topology (switch vs PHY) | Architecture Freeze | KR-007 |
| 3 | Critical | IEC 62443 zones / SL target | Architecture Freeze | KR-012 |
| 4 | Critical | Secure Boot + key ceremony | Hardware Freeze | KR-005, KR-004 |
| 5 | Critical | Carrier design + pin budget D3/D4 | Schematic Capture | KR-003 |
| 6 | High | Secure Element prototype | Hardware Freeze | KR-006 |
| 7 | High | IEC 60870-5-104 simulator | Firmware Development | KR-010 |
| 8 | High | Linux networking / firewall lab | Architecture Freeze | KR-011 |
| 9 | High | PTP (only if required) | System Integration | KR-008 |
| 10 | Medium | GNSS / PPS | System Integration | KR-009 |
| 11 | Medium | Thermal design | PCB Layout | KR-013 |
| 12 | Medium | EMC Ethernet optimization | PCB Layout | KR-014 |
| 13 | Low | LTE integration | Rev A | , Reuse ATEX |
| 14 | Low | RS485 / Modbus craft | Ongoing | , Reuse SK0146/ATEX |

### Roadmap phases

```text
NOW → Architecture Freeze
  ├─ IEC 61850 MMS + GOOSE lab (KR-001/002)
  ├─ C1 Ethernet ADR locked (KR-007)
  ├─ IEC 62443 zones (KR-012)
  └─ Linux networking lab (KR-011)

Architecture Freeze → Hardware Freeze
  ├─ Secure Boot + key ceremony (KR-005/004)
  └─ Secure Element prototype (KR-006)

Hardware Freeze → Schematic Capture
  └─ Carrier + pin budget D3/D4 (KR-003) + P0 docs MD-001…005

Schematic → PCB Layout
  ├─ Thermal (KR-013)
  └─ EMC Ethernet (KR-014)

Integration
  ├─ GNSS/PPS (KR-009)
  └─ PTP only if required (KR-008)
```

---

## Chief Architect summary

| Metric | Value |
|--------|------:|
| Subsystems reviewed | 16 |
| Missing document actions | 25 |
| P0 blockers | 9 |
| Knowledge risks (KR) | 14 |
| Critical KR | 6 |

**Immediate P0 pack:** MD-001 OpenWrt SDK · MD-002 SKU confirm · MD-003 Lab port map · MD-005 C1 ADR · MD-005 C1 ADR · MD-014 Key ceremony · MD-015 62443 zones · MD-018 GOOSE procedure · MD-022 SK0146 RAG ingest.

Generated: 2026-08-10T13:23:23.726502+00:00
