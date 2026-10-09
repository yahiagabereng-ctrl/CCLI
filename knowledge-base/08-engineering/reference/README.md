# Reference PDFs — hardware & companion datasheets

| File | RAG `source_id` | Status |
|------|-----------------|--------|
| `MT7986A_Datasheet_1.15.pdf` | `ccli-mt7986a-datasheet` | HAVE — SoC reference (not board map) |
| `TR400_User_Manual_EN_v1.0.pdf` | `ccli-tr400-user-manual` | **HAVE** — 49 pp. v1.0 EN (ingested 2026-09-15) |
| `vendor/tespro/Schematic-GPIO.pdf` | `ccli-tr400-gpio-schematic` | **HAVE** — GD32 DIO/AI/AO sheet (2026-09-17) |
| `vendor/tespro/IEC61850-User-Manual.docx` | `ccli-tespro-61850-manual` | **HAVE** — IEC 61850 Protocol Service (northbound collector) |
| `pki/RFC7030_EST.txt` etc. | `ccli-rfc7030-est` | **HAVE** — IETF PKI RFCs (2026-10-06) |
| `vendor/ejbca/` | `ccli-ejbca-readme` | **HAVE** — EJBCA CE Docker + doc snapshots |
| `iec61850/IEC_61850-10-2025.pdf` | `ccli-61850-10-extract` | **HAVE** — OCR corpus + extract (2026-10-08) |
| `iec62351/IEC_TS_62351-100-3-2020.pdf` | `ccli-62351-100-3-extract` | **HAVE** — transport conformance tests (2026-10-08) |

**PKI ingest:** `powershell -File scripts\ingest-ejbca-pki.ps1`  
**61850-10 ingest:** `powershell -File scripts\ingest-61850-10.ps1`  
**62351-100-3 ingest:** `powershell -File scripts\ingest-62351-100-3.ps1`

Extract companion: `../CCI_TR400_Extract.md` (`ccli-tr400-user-manual-extract`).  
GPIO / regulation matrix: `../CCI_TR400_GPIO_Schematic_Extract.md` (`ccli-tr400-gpio-schematic-extract`).  
61850 service manual: `../CCI_TesPro_61850_Manual_Extract.md` (`ccli-tespro-61850-manual-extract`).
