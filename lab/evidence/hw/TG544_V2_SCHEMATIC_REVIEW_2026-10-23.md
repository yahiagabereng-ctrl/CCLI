# TG-544 V2 schematic review — checklist (vs HiTEKS supplier table)

**Document ID:** CCLI-LAB-HW-SCH-REV-001  
**Date:** 2026-10-23  
**PDF:** `knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/TG544_V2_Schematic.pdf`  
**Extract:** `…/_extracts/TG544_V2_Schematic_full.txt`  
**Analysis:** `knowledge-base/08-engineering/CCI_TG544_V2_Schematic_Extract.md`  
**RAG source_id:** `ccli-tg544-v2-schematic-review`

**How to use:** Work top to bottom. Set **Verdict** to PASS / PART / GAP / N/A. Add **Sheet/ref** (PDF block or designator). **Initial review** column filled from text extract 2026-10-23.

---

## Phase 0 — Package

| # | Check | Verdict (initial) | Sheet / ref | Notes |
|---|--------|-------------------|-------------|-------|
| 0.1 | SKU = **TG-544 V2** | **PASS** | Title area | Supplier drop 2026-10-22 |
| 0.2 | Composite includes POWER, ETH, RS485, DIO, TPM | **PASS** | Block labels | Single-page PDF |
| 0.3 | Supersedes **Schematic-GPIO.pdf** for I/O count | **PASS** | 6 vs 4 ch | Keep old sheet for diff |
| 0.4 | BOM/MPN legible on extract | **PART** | Many refs | Re-read PDF visually for ambiguous refs |

---

## Phase 1 — PPT rows 1–11

| # | PPT | Requirement | Verdict (initial) | Key evidence (designators) | Follow-up |
|---|-----|-------------|-------------------|----------------------------|-----------|
| 1.1 | 1 | Dual-core ≥1 GHz | **PASS** | MT7987, 2.5G ETH | Match freeze |
| 1.2 | 2 | 2 GB RAM | **PASS** | EMMC / DDR (SoC block) | Confirm on BOM |
| 1.3 | 3 | ≥32 GB eMMC | **PASS** | EMMC nets | `df` on DUT |
| 1.4 | 4 | GPS/GNSS | **PASS** | WM1302, SIM GNSS | Bench GNSS tests |
| 1.5 | 5 | 4G LTE | **PASS** | 3.8V_4G_5G, dual SIM | Optional CAT4 |
| 1.6 | 6 | 4× isolated Eth; **100BaseFX** + **10/100 SFP** | **PART** | WAN, LAN0–2, **U30 SFP**, 2P5GBT_* | Request **FX SFP PN** + which port = Eth_A |
| 1.7 | 7 | >2× USB | **PART** | USB Type-C, USB-A paths | 1× Type-C on label |
| 1.8 | 8 | 2× RS485 **isolated** | **PART** | **U8/U10 BL3085**, **U13–U16 CA-IS3721HS**, GND_RS232-485 | Written **kV** / UL1577 |
| 1.9 | 9 | TPM 2.0 | **PASS** | **SLB9673** | `tpm2_getcap` |
| 1.10 | 10 | 12 V; **input fuse** | **PART** | **DC_IN+**, **SMDJ36CA**, **SK510L/SK56L**, **DMP10H400SK3**; **F1** only on USB | Ask **main DC fuse** location/rating |
| 1.11 | 11 | OpenWrt / C++ | **N/A** | — | Software |

---

## Phase 2 — PPT rows 12–15 (evidence, not PCB)

| # | PPT | Ask | Verdict (initial) | Follow-up |
|---|-----|-----|-------------------|-----------|
| 2.1 | 12 | IEC 61850 SCD/CID for DSO | **N/A** HW | HiTEKS **`ccli` CID**; TesPro = collector |
| 2.2 | 13 | IEC 62351 parts/certs | **N/A** HW | PKI / firmware pack |
| 2.3 | 14 | 62443-4-1 SDLA | **N/A** HW | Programme docs |
| 2.4 | 15 | 62443-4-2 SL1 / secure boot | **PART** | SoC boot + TesPro TG500 guides |

---

## Phase 3 — PPT row 16 (6 DIO)

| # | Check | Verdict (initial) | Evidence | Follow-up |
|---|--------|-------------------|----------|-----------|
| 3.1 | **6× DI** terminals | **PASS** | **J2**, DI1+…DI6- | Label photo |
| 3.2 | **6× DO** terminals | **PASS** | **J1**, DO1+…DO6- | Label photo |
| 3.3 | **10–120 V DI** class | **GAP** | **SMBJ33CA**, 47k dividers | Same class as old 4-ch sheet |
| 3.4 | **250 V / 3 A DO** | **OPEN** | **K1–K6 HK3FF-DC5V-SHG** | Request relay **contact** datasheet |
| 3.5 | Opto isolation per channel | **PASS** | **TLP291** U20–U29 | Per-channel |
| 3.6 | PTC field | **PASS** | **TR600-350**, note 350 mA / 700 mA | Chinese note on sheet |
| 3.7 | **ubus** map DI/DO 1–6 | **GAP** | GPIO*_IF nets | Request from TesPro |
| 3.8 | Safe state power-down | **OPEN** | HK3FF NO/NC usage | Wiring + FSM doc |

---

## Phase 4 — Annex O / lab cross-checks

| # | ID | Check | Verdict (initial) | Evidence |
|---|-----|--------|-------------------|----------|
| 4.1 | O.13.1.1.1 | Eth_A fibre 1310 nm LC | **GAP** | SFP cage only |
| 4.2 | O.13.1.1.1 | Eth_B 10/100 | **OPEN** | SFP or LAN port assignment |
| 4.3 | O.13.1.4 | Plant on RS485 not Eth_A/B | **PART** | A1/A2 + LAN roles — policy doc |
| 4.4 | V-IO-003 | RS485 surge | **PART** | SMBJ18CA, B3D090L-C |
| 4.5 | REQ-IO-004 | Product 5 DI + 3 DO @ field class | **GAP** | 6+6 present; voltage class not met |
| 4.6 | Hall tamper | Anti-tamper | **PART** | Hall + GPIO/HALL | Row 5 cert ask |

---

## Summary scorecard (initial)

| Area | PASS | PART | GAP | OPEN |
|------|------|------|-----|------|
| Compute / storage / radio | 4 | 0 | 0 | 0 |
| Ethernet / SFP | 0 | 1 | 1 | 1 |
| RS485 | 0 | 1 | 0 | 1 |
| Power / fuse | 0 | 1 | 0 | 1 |
| 6× DIO | 3 | 0 | 2 | 2 |
| Cyber rows 12–15 | 0 | 1 | 0 | 0 |

**Top supplier questions (from this review):**

1. **SFP module PN** for **100BaseFX LC 1310 nm** on which port = **Eth_A (DSO)**?  
2. **Port map:** WAN vs LAN0/1/2 vs LAN4 ↔ CEI roles + `ip link` names.  
3. **Main DC input fuse** (if any) — rating and location on **DC_IN+**.  
4. **HK3FF-DC5V-SHG** contact rating (Vac/A) for **DO1–DO6**.  
5. **DI1–6** guaranteed input voltage range (min/max V).  
6. **ubus `didoservice_v2` channel ↔ DI/DO terminal** table.  
7. **CA-IS3721HS** / **BL3085** isolation working voltage certificate.

---

## Regenerate extract

```powershell
python scripts/extract-tespro-tg544-v2-schematic.py
```
