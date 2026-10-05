# Certificate OCR extracts

**Regenerate:** `python scripts/ocr-certificates.py`  
**Engine:** PyMuPDF (300 dpi) + Tesseract `ita+eng`  
**Tesseract:** `C:\Program Files\Tesseract-OCR\tesseract.exe`

| PDF | OCR output | Pages | Notes |
|-----|------------|-------|-------|
| `CEI 0-16.pdf` | `CEI_0-16_ocr.txt` · `CEI_0-16_ocr.md` | 1 | **TÜV Rheinland** CEI 0-16 Annex O/T conformity cert (Higeco CCI) — not the CEI standard text |
| `DI.CO. QUADRO_ACQUISIZIONE E MISURA.pdf` | `DI_CO_..._ocr.txt` · `.md` | 2 | **Dichiarazione di conformità** + rapporto di prova — CEI EN 61439-2 low-voltage switchboard for CCI cabinet |
| UCA / 62443 / 62351-3 PDFs | `*.txt` (text layer) | — | Native text — OCR skipped |

**RAG source_id (after ingest):** `ccli-cert-ocr-*` per file under `_extracts/`
