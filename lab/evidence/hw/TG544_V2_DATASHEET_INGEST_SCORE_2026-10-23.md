# TG-544 V2 — datasheet ingest score (2026-10-23)

**Manifest:** `knowledge-base/…/datasheets/_extracts/INGEST_CHECK.txt`  
**Score:** **20 / 20 PDFs OK** · **WARN=0** · **FAIL=0**

---

## Ingest score by schematic MPN (evidence vs BOM)

| Schematic MPN | PDF in repo | Ingest | MPN match | Notes |
|---------------|-------------|--------|-----------|--------|
| **B0303S-1WR3** | `B0303S-1WR3_Mornsun.pdf` | OK | **PASS** | 1500 VDC iso 1 min |
| **BL3085(I47)** | `BL3085_I47_Belling_V2.3_cn.pdf` | OK | **PASS** | |
| **CA-IS3721HS** | `CA-IS3721HS_…pdf` | OK | **PASS** | 3750 Vrms |
| **SLB9673** | `SLB9673_Infineon_…pdf` | OK | **PASS** | |
| **GD32F330** | `GD32F330xx_…pdf` | OK | **PASS** | |
| **HK3FF-DC5V-SHG** | `HK3FF-DC5V-SHG_HUIKE.pdf` | OK | **PASS** | 10 A @ 277 VAC |
| **TLP291** | `TLP291-4_Toshiba.pdf` | OK | **PASS** | |
| **PCAQ7060MW-301** / **C89** | `PCAQ7060MW-301_Prodtech_Spec.pdf` | OK | **PASS** | |
| **DMP10H400SK3-13** | `DMP10H400SK3-13_DiodesInc.pdf` | OK | **PASS** | |
| **SK56L-TP** | `SK56L_MCC_Schottky.pdf` | OK | **PASS** | Family SK52L–SK510L |
| **SK510L-TP** | `SK510AFL-TP_MCC_Schottky.pdf` + family | OK | **VERIFY** | Sheet **SK510L-TP**; DS is **SK510AFL-TP** (100 V / 5 A class) — confirm BOM |
| **SK52L** (if populated) | `SK52L_MCC_Schottky.pdf` | OK | **PASS** | Extra family doc |
| **B3D090L-C** | `B3D090L-C_GDT.pdf` | OK | **PASS** | 90 V GDT |
| **K0805L050BR** (F1) | `K0805L_Series_Littelfuse_PTC.pdf` | OK | **PASS** | LCSC **C2892412** = K0805L series incl. **050BR** |
| **SP3232** (S07) | `SP3222B_RS232_Anap.pdf` | OK | **PART** | DS covers **SP3222B/3232B** — not exact **SP3232** marking |
| **SMBJ33CA / SMBJ18CA** | `SMBJ_Series_*.pdf` | OK | **PART** | Series only |
| **SMDJ36CA** (S01) | — | — | **GAP** | Still missing |
| **MVR1812-470G** (RV1) | `MVR0805-470G_Varistor.pdf` | OK | **FAIL BOM** | Sheet **1812**; download is **0805-470G** — wrong package |
| **TR600-150 / TR600-350** | — | — | **GAP** | Still missing |
| **WM1302** / 4G block | `WM1302_Seeed_LoRaWAN_Module.pdf` | OK | **VERIFY** | LoRaWAN module vs 4G silk |
| **MT7987** | `MT7986A_…interim…pdf` | OK | **PART** | Interim SoC DS |
| **SFP FX + copper** | — | — | **GAP** | TesPro only |

**BOM-aligned DS coverage (P0 blocking MPNs):** **~12 PASS · 4 PART/VERIFY · 4 GAP** (SMDJ36, MVR1812, TR600, SFP×2).

---

## New files from `C:\Yahia\University` (2026-10-23)

| Source file | Canonical name in `datasheets/` |
|-------------|----------------------------------|
| `SK510AFL-TP.pdf` | `SK510AFL-TP_MCC_Schottky.pdf` |
| `SK52L.PDF` | `SK52L_MCC_Schottky.pdf` |
| `B3D090L-C.PDF` | `B3D090L-C_GDT.pdf` |
| `SP3222B.PDF` | `SP3222B_RS232_Anap.pdf` |
| `MVR0805-470G.PDF` | `MVR0805-470G_Varistor.pdf` (not **MVR1812**) |
| `C2892412.pdf` | `K0805L_Series_Littelfuse_PTC.pdf` |

Re-ingest: `python scripts/extract-tg544-v2-datasheets.py`
