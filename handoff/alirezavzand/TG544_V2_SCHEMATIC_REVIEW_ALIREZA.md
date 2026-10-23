# TG-544 V2 schematic — review handoff for Alireza

**Date:** 2026-10-23  
**Git commit:** **`eac5ec2`**  
**Scope:** HiTEKS **gateway requirements table** (PPT response form) × TesPro **TG-544 V2** schematic (supplier drop 2026-10-22). **Not** the cert workbook `TESPRO_VENDOR_ASK_CERT.xlsx`.

---

## Pull and open

```bash
git clone https://github.com/yahiagabereng-ctrl/CCLI.git
cd CCLI
git pull origin main
# Pin to handoff commit (replace with SHA from README):
# git checkout eac5ec2
```

**Start here (order):**

| Step | Document | Role |
|------|----------|------|
| 1 | **[README_FOR_ALIREZA.md](README_FOR_ALIREZA.md)** | Index + **commit SHA** |
| 2 | **`knowledge-base/…/TG544_V2_Schematic.pdf`** | Visual review (force-tracked in git) |
| 3 | [lab/evidence/hw/TG544_V2_SCHEMATIC_REVIEW_2026-10-23.md](../../lab/evidence/hw/TG544_V2_SCHEMATIC_REVIEW_2026-10-23.md) | Phase checklist — set PASS/PART/GAP |
| 4 | [lab/evidence/hw/TG544_V2_REQUIREMENT_SHEET_VERIFY_MATRIX.md](../../lab/evidence/hw/TG544_V2_REQUIREMENT_SHEET_VERIFY_MATRIX.md) | PPT rows 1–16 + Annex O × schematic sheets S01–S14 |
| 5 | [knowledge-base/08-engineering/CCI_TG544_V2_Schematic_Extract.md](../../knowledge-base/08-engineering/CCI_TG544_V2_Schematic_Extract.md) | Narrative extract + designators |
| 6 | [lab/evidence/hw/TG544_V2_DATASHEET_TRACKER.md](../../lab/evidence/hw/TG544_V2_DATASHEET_TRACKER.md) | MPN evidence HAVE/MISSING |
| 7 | [knowledge-base/…/datasheets/DOWNLOAD_STATUS_2026-10-23.md](../../knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/datasheets/DOWNLOAD_STATUS_2026-10-23.md) | Ingest check (**13** PDF text extracts) |

**Baseline supplier “Complied” column:**  
`knowledge-base/08-engineering/TesPro_Gateway_Specifications_Response_Form.pdf` (gitignored PDF — on repo PC / Telematry mirror).

**Superseded for I/O count:** `knowledge-base/08-engineering/reference/vendor/tespro/Schematic-GPIO.pdf` + [CCI_TR400_GPIO_Schematic_Extract.md](../../knowledge-base/08-engineering/CCI_TR400_GPIO_Schematic_Extract.md) (4 ch only).

---

## Canonical paths

| Item | Path |
|------|------|
| Schematic PDF | `knowledge-base/08-engineering/reference/vendor/tespro/2026-10-22-tg544-v2-schematic/TG544_V2_Schematic.pdf` |
| Text extract | `…/_extracts/TG544_V2_Schematic_full.txt` |
| Sheet blocks S01–S14 | `…/_extracts/TG544_V2_sheet_blocks.md` |
| Datasheet text (RAG) | `…/datasheets/_extracts/*.txt` |
| HW index | [lab/HW_CONFIGURATION_INDEX.md](../../lab/HW_CONFIGURATION_INDEX.md) |

---

## What we need from your review

1. **Visual pass** on ambiguous refs called **PART** in the checklist (BOM legibility).
2. **Ethernet:** confirm **U30 SFP** role; flag missing **100Base-FX 1310 nm** module PN (Eth_A gap).
3. **RS485:** **BL3085** + **CA-IS3721HS** — comment if isolation narrative is enough for row 8 (3750 Vrms in DS extract).
4. **DIO:** **6 DI + 6 DO** vs product **REQ-IO-004** (10–120 V DI / 250 V DO) — **SMBJ33CA** + dividers vs field class.
5. **Relay:** **HK3FF-DC5V-SHG** — DS says **10 A @ 277 VAC** (see `HK3FF…txt` extract); confirm vs **3 A** curtail story.
6. **Power:** locate **main DC input fuse** (USB **F1** only on sheet today).
7. **ubus ↔ J1/J2** — note any GPIO net names you can tie to **DIO 1–6** on DUT.

Return: update **Verdict** columns in the checklist/matrix, or a short markdown note under `lab/evidence/hw/` (e.g. `TG544_V2_ALIREZA_REVIEW_<date>.md`).

---

## Regenerate extracts (optional)

```powershell
python scripts/extract-tespro-tg544-v2-schematic.py
python scripts/extract-tg544-v2-datasheets.py
powershell -File scripts/ingest-tespro-tg544-v2-schematic.ps1 -ExtractOnly
```

---

## Known gaps (do not spend time re-proving)

| Gap | Owner |
|-----|--------|
| SFP FX + copper approved PNs | TesPro |
| Port map WAN/LAN/SFP ↔ Eth_A/Eth_B | TesPro + bench |
| **SMDJ36CA** / exact **SMBJ33/18** line items | Distributor DS |
| **MVR1812-470G** (sheet) vs **MVR0805** DS on disk | Get correct varistor DS |
| **TR600-150 / TR600-350** PTC | Distributor DS |
| WM1302 Seeed LoRaWAN PDF vs 4G block label | Confirm MPN on schematic |

---

## Related bench work (unchanged)

Connect-PASS **r57** / EMT432 **r64** handoffs remain in [README_FOR_ALIREZA.md](README_FOR_ALIREZA.md) — schematic review is **parallel** to cert MMS/Modbus work.
