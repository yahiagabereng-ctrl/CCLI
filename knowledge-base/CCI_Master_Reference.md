# CCI Master Reference (living PDF)

**Document ID:** CCLI-MASTER-REF-001  
**Output PDF:** `CCI_Master_Reference.pdf` (same folder)  
**Build script:** `../scripts/build-master-reference.ps1`  
**Source manifest:** `../scripts/master-reference-manifest.json`

## Purpose

Single **always-updatable** reference PDF consolidating the CCI programme baseline: roadmap, knowledge-base structure, vendor selection, Wave 1 scorecard, prototype BOM, regulations map, RAG architecture, and download index.

Edit the **canonical Markdown sources** (not the generated PDF). Re-run the build script to refresh.

## Rebuild

```powershell
cd c:\Yahia\projects\CCLI
.\scripts\build-master-reference.ps1
```

Requires: **Python 3** + **MiKTeX** (`pdflatex` on PATH).

## What gets included

Sources are listed in `scripts/master-reference-manifest.json`:

| Part | Sources |
|------|---------|
| I — Programme overview | `CCI_Project_Roadmap.md`, `knowledge-base/README.md` |
| II — Vendor & BOM | Vendor selection, scorecard, kit catalog, prototype BOM |
| III — Regulations | `CCI_Module_Regulations_Classification.md` |
| IV — Engineering | RAG architecture, `DOWNLOADS.md` |
| V — Validation & test | PF2 validation strategy + mocking bench BOM |
| VI — RAG knowledge-base | Clustered document gate D0–D21 + ingest inventory |

To add a chapter: append an entry to the manifest, then rebuild.

## Generated artifacts (do not edit by hand)

- `knowledge-base/_build/CCI_Master_Reference.tex`
- `knowledge-base/_build/*.aux`, `.toc`, `.log`
- `knowledge-base/CCI_Master_Reference.pdf`

## RAG ingest (optional)

After rebuild, mirror to Telematry and ingest as `ccli-master-reference` if desired.
