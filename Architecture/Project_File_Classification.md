# Project File Classification & Cleanup

**Document ID:** CCLI-FILE-CLASS-001  
**Date:** 2026-07-26  
**Database:** `Architecture/architecture.db` → table `project_files`  

## Classification legend

| Class | Meaning |
|-------|---------|
| KEEP_SOURCE | Authoritative source (.md .tex .mermaid .csv intentional) |
| KEEP_DELIVERABLE | Published PDF/PNG |
| DISPOSE_BUILD | LaTeX `.aux .log .out .toc` — delete |
| DUPLICATE_OF_KB | Root copy also in knowledge-base — delete root |
| RELOCATE_CANDIDATE | Unique PDF — move into KB tree |
| SCRATCH | Review / optional remove |

## Screenshot set (from Explorer)

| File | Class | Action |
|------|-------|--------|
| `CCI_Architecture.mermaid` | KEEP_SOURCE | keep → kept |
| `CCI_Architecture.png` | KEEP_DELIVERABLE | keep → kept |
| `CCI_Clone_RE_Option.tex` | KEEP_SOURCE | keep → kept |
| `CCI_Clone_RE_Option.pdf` | KEEP_DELIVERABLE | keep → kept |
| `CCI_Clone_RE_Option.aux` | DISPOSE_BUILD | delete → deleted |
| `CCI_Clone_RE_Option.log` | DISPOSE_BUILD | delete → deleted |
| `CCI_Clone_RE_Option.out` | DISPOSE_BUILD | delete → deleted |
| `CCI_Clone_RE_Option.toc` | DISPOSE_BUILD | delete → deleted |
| `CCI_Project_Roadmap.md` | KEEP_SOURCE | keep → kept |
| `CCI_Project_Roadmap.tex` | KEEP_SOURCE | keep → kept |
| `CCI_Project_Roadmap.pdf` | KEEP_DELIVERABLE | keep → kept |
| `CCI_Project_Roadmap.aux` | DISPOSE_BUILD | delete → deleted |
| `CCI_Project_Roadmap.log` | DISPOSE_BUILD | delete → deleted |
| `CCI_Project_Roadmap.out` | DISPOSE_BUILD | delete → deleted |
| `CCI_Project_Roadmap.toc` | DISPOSE_BUILD | delete → deleted |
| `CCI_SoM_Comparison.tex` | KEEP_SOURCE | keep → kept |
| `CCI_SoM_Comparison.pdf` | KEEP_DELIVERABLE | keep → kept |
| `CCI_SoM_Comparison.aux` | DISPOSE_BUILD | delete → deleted |
| `CCI_SoM_Comparison.log` | DISPOSE_BUILD | delete → deleted |
| `CCI_SoM_Comparison.out` | DISPOSE_BUILD | delete → deleted |
| `CCI_SoM_Comparison.toc` | DISPOSE_BUILD | delete → deleted |
| `CCI_Knowledge_Base_Brief.md` | KEEP_SOURCE | keep → kept |
| `CCI_NXP_IMX8MDQLQCEC_Precision_Extract.md` | KEEP_SOURCE | keep → kept |
| `_pe_inventory.csv` | SCRATCH | review → kept |

## Cleanup summary

| Metric | Count |
|--------|------:|
| Classified | 38 |
| Deleted | 20 |
| Relocated | 3 |
| Kept | 12 |
| Review | 3 |

### Deleted

- `CCI_Clone_RE_Option.aux` (DISPOSE_BUILD)
- `CCI_Clone_RE_Option.log` (DISPOSE_BUILD)
- `CCI_Clone_RE_Option.out` (DISPOSE_BUILD)
- `CCI_Clone_RE_Option.toc` (DISPOSE_BUILD)
- `CCI_Project_Roadmap.aux` (DISPOSE_BUILD)
- `CCI_Project_Roadmap.log` (DISPOSE_BUILD)
- `CCI_Project_Roadmap.out` (DISPOSE_BUILD)
- `CCI_Project_Roadmap.toc` (DISPOSE_BUILD)
- `CCI_SoM_Comparison.aux` (DISPOSE_BUILD)
- `CCI_SoM_Comparison.log` (DISPOSE_BUILD)
- `CCI_SoM_Comparison.out` (DISPOSE_BUILD)
- `CCI_SoM_Comparison.toc` (DISPOSE_BUILD)
- `CELEX_32018L2001_EN_TXT.pdf` (DUPLICATE_OF_KB)
- `OJ_L_202402847_EN_TXT.pdf` (DUPLICATE_OF_KB)
- `EstrattoAllegatoO.pdf` (DUPLICATE_OF_KB)
- `EstrattoAllegatoT.pdf` (DUPLICATE_OF_KB)
- `iGate-850_V10_UM_EN.pdf` (DUPLICATE_OF_KB)
- `IMX8MDQLQCEC-1280316.pdf` (DUPLICATE_OF_KB)
- `moxa-mgate-5119-series-datasheet-v1.2.pdf` (DUPLICATE_OF_KB)
- `STM32L151RCTx.pdf` (DUPLICATE_OF_KB)

### Relocated

- `IMX8MMIEC.pdf` — moved → knowledge-base\08-engineering\reference\IMX8MMIEC.pdf
- `UG10164.pdf` — dest exists — removed root duplicate UG10164.pdf
- `Manuale CCI_I_24_R6_240610.pdf` — moved → knowledge-base\07-protocols\Manuale CCI_I_24_R6_240610.pdf

### Keep (essential)

| Path | Class | Role |
|------|-------|------|
| `CCI_Architecture.mermaid` | KEEP_SOURCE | Architecture diagram source |
| `CCI_Architecture.png` | KEEP_DELIVERABLE | Rendered architecture diagram |
| `CCI_Clone_RE_Option.tex` | KEEP_SOURCE | LaTeX source — clone/RE study |
| `CCI_Clone_RE_Option.pdf` | KEEP_DELIVERABLE | PDF deliverable |
| `CCI_Project_Roadmap.md` | KEEP_SOURCE | Roadmap markdown (RAG ccli-project-roadmap) |
| `CCI_Project_Roadmap.tex` | KEEP_SOURCE | Roadmap LaTeX source |
| `CCI_Project_Roadmap.pdf` | KEEP_DELIVERABLE | Roadmap PDF |
| `CCI_SoM_Comparison.tex` | KEEP_SOURCE | SoM comparison LaTeX |
| `CCI_SoM_Comparison.pdf` | KEEP_DELIVERABLE | SoM comparison PDF |
| `CCI_Knowledge_Base_Brief.md` | KEEP_SOURCE | KB brief |
| `CCI_NXP_IMX8MDQLQCEC_Precision_Extract.md` | KEEP_SOURCE | NXP DualLite precision extract |
| `CCI_Yocto_BSP_Notes.md` | DISPOSE | Removed — legacy BSP notes |

## SQL

```sql
SELECT path, class, action, notes FROM project_files ORDER BY class, path;
SELECT * FROM v_project_files_dispose;
SELECT * FROM v_project_files_keep;
```

Generated: 2026-07-26T02:40:26.288237+00:00
