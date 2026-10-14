# Lab + Official Product workspace

**Document ID:** CCLI-LAB-WS-001  
**Date:** 2026-10-14

Use one Cursor window for **conformance/lab** and **SK0153 product** trees.

---

## Open the workspace

**Option A — Explorer**

Double-click:

`CCLI-Lab-And-Product.code-workspace`

(repo root: `C:\Yahia\projects\CCLI\`)

**Option B — script**

```powershell
powershell -File scripts\open-lab-product-workspace.ps1
```

**Option C — Cursor CLI**

```powershell
cursor "C:\Yahia\projects\CCLI\CCLI-Lab-And-Product.code-workspace"
```

---

## Folder roles

| Workspace folder | Path | Use |
|------------------|------|-----|
| **CCLI Lab** | `C:\Yahia\projects\CCLI` | r52 pin, TSP/PICS, deploy, `knowledge-base/`, RAG sync scripts |
| **CCLI Product (apps/ccli)** | `…\CCLI` + Official Product suffix `\apps\ccli` | **Product intent** — `program/main.c`, `ccli_program`, product CMake |

---

## RAG (Telematry)

RAG corpus is **not** duplicated in the product folder. Refresh from **CCLI Lab** root:

```powershell
powershell -File scripts\sync-ccli-corpus-to-telematry.ps1
powershell -File scripts\ingest-product-deploy-rag.ps1
```

Index map: `lab/conformance/RAG_SOURCE_INDEX.md`

---

## Build targets

| Tree | Default `ccli` entry |
|------|----------------------|
| Lab | `services/ccli_main.cpp` |
| Official Product | `program/main.c` (+ `ccli_program` static lib) |

Do not change lab CMake to product entry; merge shared logic in `core/` / `adapters/` via git or manual port between trees.
