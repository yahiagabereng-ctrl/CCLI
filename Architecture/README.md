# Architecture workspace

Working package for PF2 / CEI 0-16 CCI design — **not** random clutter.

**Methodology:** `knowledge-base/08-engineering/CCI_Architecture_Framework.md`  
**Cursor rule:** `.cursor/rules/system-architect.mdc`

## Quick map (what you are looking at)

| Layer | What | Files |
|-------|------|-------|
| **Diagrams** | Pictures of the system | `*.drawio` |
| **Matrices** | Living Excel tables | `*.xlsx` |
| **Database** | Traceability master (query in Cursor) | `architecture.db`, `schema.sql`, `queries.sql` |
| **Reports** | Written reviews (regenerable) | `Design_Review_Board_*.md`, `Document_Coverage_*.md`, … |
| **Runtime dataflow** | LAN1/2 · MMS · Modbus · PF2 · GPIO · reactive gaps · **Test Suite Pro** | **`CCI_Runtime_Dataflow_Architecture.md`** |
| **Annex ↔ SW** | CEI O/T/M vs L0–L4 validation matrix | **`CCI_SW_Architecture_vs_Annex_Validation.md`** |
| **Ignore** | Generator logs / Excel locks / Draw.io temps | `_meta/`, `_generated/`, `~$*.xlsx`, `.$*.drawio*` |

Same data often appears as **xlsx + md + db rows** on purpose: Excel for humans, SQLite for Cursor SQL, Markdown for RAG/docs.

## Diagrams

| File | Purpose |
|------|---------|
| `System_Context.drawio` | DSO, Operator, plant, CCI boundaries |
| `Functional_Architecture.drawio` | C1–C10 functional blocks |
| `Hardware_Architecture.drawio` | **TG-524** final product gateway (MT798X) |
| `Power_Tree.drawio` | 12–24 V input / rails |
| `Network_Architecture.drawio` | Eth_A, Eth_B, plant (logical target) |
| **`Zones_62443_TG544_PortMap.drawio`** | **IEC 62443-3-2 zones ↔ TG544 ports** — lab as-built P2 (signed) + product target · `CCI_62443_Zones.md` Rev 1.5 |
| `TR400_Lab_Platform.drawio` | **TG544 bench** — 6 pages: system block, wiring, network zones, CCLI config, interface catalogue, **DRC-60A UPS + full DIO/AIO map** |
| **`Software_Architecture.drawio`** | **CCLI app SW** — 5 pages: layer model, Phase 1 runtime, CMake/deploy, roadmap/stubs, shared state · guide: `Software_Architecture.md` |
| **`CCI_SW_Architecture_vs_Annex_Validation.md`** | **Annex O/T/M ↔ L0–L4** validation matrix (SoT for compliance reviews) |
| **`CCI_Runtime_Dataflow_Architecture.md`** | **LAN1/LAN2 + MMS/Modbus/PF2/GPIO flow** · Mermaid · source-file map · reactive P5 gaps |
| `lab/tg544-openwrt/` | OpenWrt **25.12.5** SDK + CCLI `.apk` build (active) |
| `lab/pi4-openwrt/` | **ARCHIVED** — Pi 4 path out of scope |

## Matrices (Excel)

| File | Purpose |
|------|---------|
| `Interface_Matrix.xlsx` | Source → destination protocols |
| `BOM_Matrix.xlsx` | Block → candidate parts |
| `Knowledge_Matrix.xlsx` | KR-001…015 (MT798X/TG-524), learning plan, SK0146 leaks |
| `Risk_Register.xlsx` | Architecture risks |
| `Verification_Matrix.xlsx` | REQ → test → pass criteria |
| `Document_Coverage_Matrix.xlsx` | Docs HAVE/MISSING by subsystem |

## Reports (Markdown)

| File | Purpose |
|------|---------|
| `Design_Review_Board_PF2.md` | Full DRB review (CCLI-DRB-001) |
| `Document_Coverage_Matrix.md` | Coverage + missing docs + roadmap |
| `Knowledge_Matrix_SK0146.md` | SK0146 bench knowledge leaks |
| `Project_File_Classification.md` | Repo-root cleanup log |

## Database

```powershell
python scripts/init-architecture-db.py --dashboard
python scripts/init-architecture-db.py --query "SELECT * FROM v_open_knowledge_gaps"
```

- `architecture.db` — requirements, blocks, BOM, gaps, risks, verification, `project_files`
- `schema.sql` / `queries.sql` — DDL + curated Cursor queries  
- Meta timestamps: `_meta/`

## Regenerate

```powershell
python scripts/generate-architecture-from-rag.py
python scripts/build-document-coverage-matrix.py
python scripts/build-sk0146-knowledge-matrix.py
```

**Back up manual Excel edits first.** If Excel has a file open, output may go to `_generated/`.
