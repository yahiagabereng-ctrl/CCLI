# Lab + Official Product workspace

**Document ID:** CCLI-LAB-WS-001  
**Revision:** 1.1  
**Date:** 2026-10-15

Use one Cursor window for **conformance/lab** and **product (SK0153 `program/`)** — **both inside this git repo**.

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

| Workspace folder | Path (in repo) | Use |
|------------------|----------------|-----|
| **CCLI Lab** | `.` (repo root) | r56+ pin, TSP/PICS, deploy, `knowledge-base/`, RAG sync |
| **CCLI Product** | `product/apps/ccli` | **Product intent** — `program/main.c`, `ccli_program`, product CMake |

Overview: [`product/README.md`](../product/README.md)

---

## RAG (Telematry)

RAG corpus is **not** duplicated under `product/`. Refresh from **repo root**:

```powershell
powershell -File scripts\sync-ccli-corpus-to-telematry.ps1
powershell -File scripts\ingest-product-deploy-rag.ps1
```

Index map: `lab/conformance/RAG_SOURCE_INDEX.md`

---

## Build targets

| Tree | Default `ccli` entry | OpenWrt override |
|------|----------------------|------------------|
| Lab | `services/ccli_main.cpp` | default `CCLI_SRC_DIR` → `apps/ccli` |
| Product | `program/main.c` (+ `ccli_program`) | `CCLI_SRC_DIR=.../product/apps/ccli` |

Do not change lab CMake to product entry; port shared fixes between trees or sync `adapters/` / `core/` as needed.

After clone:

```powershell
powershell -File scripts\link-product-third-party.ps1
```

---

## Legacy external folder

The old sibling directory `CCLI…Official Product` under `C:\Yahia\projects\` is **superseded** by `product/apps/ccli`. You may archive or delete it after verifying this tree builds.
