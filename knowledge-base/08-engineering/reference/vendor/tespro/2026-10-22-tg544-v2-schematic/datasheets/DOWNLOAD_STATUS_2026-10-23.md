# Datasheet download & ingest status — 2026-10-23 (updated)

**Ingest check:** `datasheets/_extracts/INGEST_CHECK.txt` — **OK=14 FAIL=0** (B0303S-1WR3 Mornsun DS validated 2026-10-23).

Re-run:

```powershell
python scripts/extract-tg544-v2-datasheets.py
powershell -File scripts/download-tg544-v2-datasheets.ps1   # optional re-fetch
powershell -File scripts/ingest-tespro-tg544-v2-schematic.ps1
```

## In repo — PDF + text extract (`_extracts/*.txt`)

| File | MPN | Extract | Key parameters (from DS text) |
|------|-----|---------|--------------------------------|
| `CA-IS3721HS_Chipanalog_CA-IS372x_V1.08_en.pdf` | CA-IS3721HS | OK | **3750 Vrms** isolation (SOIC8-NB) |
| `BL3085_I47_Belling_V2.3_cn.pdf` | BL3085(I47) | OK | RS485 transceiver, ±15 kV ESD (sheet ref) |
| `HK3FF-DC5V-SHG_HUIKE.pdf` | HK3FF-DC5V-SHG | OK | **10 A @ 277 VAC / 30 VDC** contact; coil–contact **1500 VAC** 1 min |
| `GD32F330xx_Datasheet_Rev_3.0.pdf` | GD32F330CBT6 | OK | DIO MCU GPIO / abs max |
| `DMP10H400SK3-13_DiodesInc.pdf` | DMP10H400SK3-13 | OK | Input reverse-polarity P-FET |
| `MT7986A_Datasheet_1.15_interim_for_MT7987.pdf` | MT7987 (interim) | OK | SoC until MT7987 DS from TesPro |
| `SLB9673_Infineon_DataSheet_Rev1.4.pdf` | SLB9673 | OK | TPM 2.0 electrical DS (not policy-only) |
| `TLP291-4_Toshiba.pdf` | TLP291(GB-TP,SE) | OK | Photocoupler — confirm **3750 Vrms** class in table |
| `SMBJ_Series_Littelfuse.pdf` | SMBJ** / SMBJ33CA | **PART** | Series DS — need line item **SMBJ33CA**, **SMBJ18CA** |
| `SMBJ_Series_DataSheet_N0193_RevD.pdf` | SMBJ series | **PART** | Duplicate series coverage |
| `SK56L_MCC_Schottky.pdf` | SK56L-TP + **SK510L-TP** | OK | MCC family **SK52L thru SK510L** — **SK510L** **100 V** / **5 A**; **SK56L** **60 V** / **5 A** |
| `PCAQ7060MW-301_Prodtech_Spec.pdf` | PCAQ7060MW-301 | OK | CM choke / PA spec (Prodtech/Würth class) |
| `WM1302_Seeed_LoRaWAN_Module.pdf` | WM1302 | **VERIFY** | Seeed **LoRaWAN** module DS — confirm schematic radio is WM1302 vs 4G MPN |
| `B0303S-1WR3_Mornsun.pdf` | B0303S-1WR3 | OK | **3.3 V → 3.3 V** iso DC-DC; **1500 VDC** 1 min / **3000 VDC** 1 s (5 V-input class note in series table) |

**Also in folder (no PDF extract):** `SLB9673_FIPS140_L2_Security_Policy.txt` — security policy supplement only.

## Quarantined (superseded)

| File | Issue |
|------|-------|
| `_bad/B0303S-1WR3_Mornsun.pdf` | Old HTML 404 fetch — replaced by valid DS in folder root |
| `_bad/B0303S-1WR3_Mornsun_try2.pdf` | Failed curl retry |

## Still missing (manual / TesPro)

| MPN | Reason |
|-----|--------|
| **SMDJ36CA** | Not in SMBJ series PDFs — separate **SMDJ** package DS |
| **SMBJ33CA**, **SMBJ18CA** | Prefer exact MPN pages (series OK for narrative only) |
| **SP3232** | RS232 — retry Analog SP3232E DS |
| **K0805L050BR**, **B3D090L-C**, **TR600-***, **MVR1812-470G** | Protection BOM |
| **918-518K2023D50012**, **PRTR5V0U2X** | USB-C / ESD |
| **SFP FX 1310 nm + copper 10/100** | **TesPro only** + port role map |
| Buck **U1** | Read marking on S01 POWER |

## Relay row 16 (HK3FF)

Contact rating **10 A @ 277 VAC** (HK3FF DS) — supports **250 V** supplier ask; plant **3 A curtail** still needs load study vs **10 A** contact max.
