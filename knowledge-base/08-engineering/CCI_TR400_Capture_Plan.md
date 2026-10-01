# TesPro TR-400 User Manual — Capture Plan (CCLI / product DUT)

**Document ID:** CCLI-HW-TR400-CP-001  
**Revision:** 1.0  
**Date:** 2026-09-15  
**RAG source_id:** `ccli-tr400-capture-plan` → `ccli-tr400-user-manual-extract`  
**PDF source_id:** `ccli-tr400-user-manual`  
**Source file:** `TR400_User_Manual_EN_v1.0.pdf` (OEM user manual, English v1.0)  
**Parent:** `ccli-tg500-lab-platform` · `ccli-tespro-supplier-correspondence` · `ccli-tg500-knowledge-tree` (K1, K3, K5)  
**Programme link:** Real hardware received — supersedes Pi-only lab for **port map**, **DI/DO**, **RS485**, **TesproOS/OpenWrt** bring-up

---

## Summary

The **TR-400 User Manual** is the **OEM user-facing document** shipped with the received TesPro gateway. TesPro marketing also uses **TR-424 / TR-425** (TR-400 series) and **TG-424 Pro / TG-524** names — treat this manual as **authoritative for the physical unit on the bench** until written SKU mapping is confirmed (K1.6).

**Ingest pipeline:**

| Step | Script / path |
|------|----------------|
| 1. Copy PDF locally (not OneDrive 0 B placeholder) | Desktop → `knowledge-base/08-engineering/reference/TR400_User_Manual_EN_v1.0.pdf` |
| 2. Auto-extract | `python scripts/extract-tr400-manual.py` |
| 3. Sync + RAG | `.\scripts\ingest-tr400-manual.ps1` |

**Status:** **HAVE** — PDF ingested from `C:\Yahia\projects\CCLI\TR400_User_Manual_EN_v1.0.pdf` (49 pp., 2026-09-15). Batches A–F content available in `ccli-tr400-user-manual-extract`.

---

## Why this document matters (vs TG-500 datasheet)

| Gap in corpus | TR-400 manual expected to close |
|---------------|----------------------------------|
| K3.1 Port map | Linux interface names ↔ silkscreen WAN/LAN1–3 |
| K5.1 RS485 | Terminal A/B/G, default `/dev/tty*` names, isolation note |
| K5 DI/DO | 8-P socket pinout, voltage/current, active level |
| K2.2 SDK | Factory OpenWrt/TesproOS access, opkg, SSH/Web UI defaults |
| K1.6 SKU | Label photo ↔ TR-424 vs TG-424 Pro vs TG-524 |
| K6.1 TPM | tpm device node, enablement, factory reset impact |

The **TG-500 Series Datasheet** (`ccli-tespro-tg500-ds`) remains **marketing/spec summary**; this manual is **operational + wiring**.

---

## Capture batches

### Batch A — Product identity & safety — **PENDING PDF**

| Topic | Expected section | Closes |
|-------|------------------|--------|
| Model variants (TR-424/425/444/445) | Cover / specifications | K1.6 |
| Regulatory / power ratings | Safety / specifications | K7 |
| Mechanical / dimensions | Hardware overview | C9 |

### Batch B — Power & connectors — **PENDING PDF**

| Topic | Expected section | Closes |
|-------|------------------|--------|
| 12–36 V DC terminal | Power / 2-P socket | C10 |
| Reverse polarity / surge claims | Specifications | — |
| Reset button behaviour | Hardware | Commissioning |

### Batch C — Ethernet & cellular — **PENDING PDF**

| Topic | Expected section | Closes |
|-------|------------------|--------|
| WAN + LAN port labels | Interfaces / LEDs | **K3.1 P0** |
| 4G / dual-SIM / antenna | Cellular chapter | K3.4 |
| GNSS antenna | Hardware | K3.5 |
| Wi-Fi disable for CCI profile | TesproOS network | K3.7 |

### Batch D — Serial field buses — **PENDING PDF**

| Topic | Expected section | Closes |
|-------|------------------|--------|
| RS485 ×2 pinout (A/B/G) | 7-P / 8-P connector | **K5.1 P0** |
| RS232 DB9 + terminals | Serial chapter | K5 |
| CAN ×2 (if present) | 4-P socket | Out of CCI scope — document only |
| Baud / parity defaults | Serial config | Modbus RTU |

### Batch E — DI/DO (PF2 critical) — **CLOSED (bench 2026-09-15)**

| Topic | Expected section | Closes | Status |
|-------|------------------|--------|--------|
| 4× DIO pinout (10-pin block) | I/O connector | **K5 P0** | **HAVE** — `ccli-tr500-io-peripheral` |
| Voltage / current / dry contact class | Specifications | C4 partial | **PART** — open-drain DO measured; plant class TBD |
| Active level / factory default state | I/O config | CR 3.6 policy input | **HAVE** — DIO1 sink, DIO2 active-low |
| Mapping to Linux API | Software | `platform_tg500` HAL | **HAVE** — **ubus `dido_v2`**, not libgpiod |

### Batch F — Software platform — **PENDING PDF**

| Topic | Expected section | Closes |
|-------|------------------|--------|
| TesproOS / OpenWrt Web UI | Getting started | K2.3 |
| SSH / default IP / password policy | Network | Lab bring-up |
| Firmware update / recovery | Maintenance | K2.7 |
| opkg / custom C++ app notes | Development | K2.2 partial |
| TPM 2.0 usage | Security | K6.1 |

### Batch G — Commissioning checklist (CCLI-authored after Batch A–F)

| Item | Source | Status |
|------|--------|--------|
| Photo: device label + SKU | Bench | **OPEN** |
| `ip link` / `uci show network` dump | SSH on DUT | **OPEN** |
| `ubus dido_v2` DIO smoke (DIO1/2) | SSH + DMM | **PASS** 2026-09-15 — see `ccli-tr500-io-peripheral` |
| `tpm2_getcap` or equivalent | SSH | **OPEN** |
| Update `CCI_TG500_Lab_Platform.md` port map table | This capture | **OPEN** |

---

## Cross-reference (existing corpus)

| Document | RAG id | Relationship |
|----------|--------|--------------|
| TG-500 Series Datasheet | `ccli-tespro-tg500-ds` | Spec baseline — 2 DI / 2 DO, 2 RS485, TPM 2.0 |
| Supplier correspondence | `ccli-tespro-supplier-correspondence` | TG-424 Pro, OpenWrt 24.10.5 |
| TG-500 lab platform freeze | `ccli-tg500-lab-platform` | CCLI port roles Eth_A/B/plant |
| MT7986A extract | `ccli-mt7986a-datasheet-extract` | SoC mux — **not** board map |
| OpenWrt procedural extract | `ccli-openwrt-procedural-extract` | UCI/procd after OS access confirmed |

---

## Verification

| Requirement | Test | Pass criteria |
|-------------|------|---------------|
| PDF ingested | `ingest-tr400-manual.ps1` | PDF > 1 KB; extract generated |
| RAG query | `source_id=ccli-tr400-user-manual-extract` | Returns DI/DO + RS485 pinout |
| K3.1 | Label WAN/LAN vs `eth*` | Table in extract matches `ip link` |
| K5 | RS485 Modbus poll | Analyzer responds on documented port |
| K1.6 | Label SKU photo | Written mapping TR-400 ↔ TG-424 Pro |

---

## Revision history

| Rev | Date | Notes |
|-----|------|-------|
| 1.0 | 2026-09-15 | Initial capture plan; PDF blocked on OneDrive placeholder |

## Keywords

`TR-400`, `TR400`, `TR-424`, `TG-424`, `TG-524`, `TesPro`, `user manual`, `ccli-tr400-capture-plan`, `ccli-tr400-user-manual`
