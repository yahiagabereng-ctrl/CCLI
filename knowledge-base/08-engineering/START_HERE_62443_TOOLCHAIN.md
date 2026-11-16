# START HERE — CCLI 62443 toolchain (Jira + git register + Notion)

**As of finalize run:** register **20 rows OK** · Jira **CSEC** validate **31 PASS** · Notion **IEC 62443** database on private **Yahia** page (**20 rows synced**).

**All staff — mandatory SM-4 awareness:** [`62443/CCI_62443_ESSENTIAL_SECURITY_TRAINING.md`](62443/CCI_62443_ESSENTIAL_SECURITY_TRAINING.md) · broadcast text [`62443/company-broadcast/MESSAGE_62443_ESSENTIAL_TRAINING.txt`](62443/company-broadcast/MESSAGE_62443_ESSENTIAL_TRAINING.txt)

**No “Notion connection” in the repo:** that step lives only in the Notion app ([`scripts/notion/NOTION_CONNECT.md`](../../scripts/notion/NOTION_CONNECT.md)). You can also **skip Notion** and use CSV + Jira only.

## One command (after Notion **Connections**)

```powershell
cd c:\Yahia\projects\CCLI
.\scripts\finalize-62443-toolchain.ps1
```

## What is finalized

| Layer | Location |
|-------|----------|
| **Requirements register** | `CCI_62443_REGISTER.csv` + `.md` |
| **Jira (course style)** | Project **CSEC** — `scripts/jira/COURSE_STYLE_JIRA.md` |
| **Plant pattern reference** | `apps/ccli/config/plant/READ.md` |
| **Notion sync** | `scripts/notion/` |

## `.env` (repo root, gitignored)

```ini
ATLASSIAN_*          # Jira CSEC — HAVE
NOTION_TOKEN=        # HAVE
NOTION_DATABASE_ID=  # set after Connections (or create-register-database.py)
```

## Notion (one-time — required for sync)

1. Notion → page **CCLI 62443 Cyber** (or **Welcome** workspace page).
2. **⋯ → Connections →** add your integration (**CCLI Cursor**).
3. Run:
   ```powershell
   python scripts/notion/discover-databases.py
   python scripts/notion/create-register-database.py
   python scripts/notion/sync-register-to-notion.py
   ```

## Manual checklist (course)

- [ ] Jira automation: New Feature → Test Case → Done (`UDEMY_AUTOMATION.md`)
- [ ] Notion: Status **Complete** (not Done) on register
- [ ] Epic/story work in **CSEC**, not **KAN**

## Validate only

```powershell
.\scripts\validate-62443-toolchain.ps1
.\scripts\validate-zcr-tr400.ps1
```

Or: `finalize-62443-toolchain.ps1` (validate + Notion sync).

**Trivy (CCI firmware):** `scripts/trivy/scan-cci-firmware.ps1` · evidence `lab/evidence/trivy/` · narrative `lab/evidence/trivy/EVD-SCA-CCI-r64-Trivy.md` (**FINAL** r64)

## IEC 62443 package (single folder in this repo)

**Canonical package:** [`knowledge-base/08-engineering/62443/README.md`](knowledge-base/08-engineering/62443/README.md)

| Piece | Document |
|-------|----------|
| Map | `knowledge-base/08-engineering/62443/README.md` |
| 4-1 document library (Notion table) | `62443/SDL-4-1-Document-Library.md` · `62443/CCI_62443_4-1_LIBRARY.csv` · `python scripts/notion/sync-4-1-library-to-notion.py` |
| 4-1 lifecycle (waterfall + agile) | `62443/SDL-SM-Lifecycle.md` |
| 4-1 Practice 1 SM-1…13 | `62443/SDL-SM-Practice-1-Security-Management.md` · templates **1.1–1.3** |
| 3-2 ZCR 1–7 | `62443/ZCR-*.md` |
| Validation | `62443/VALIDATION.md` |
| Expanded CRS | `CCI_62443_Zones.md` |
| Lab sign-off archive | `lab/evidence/62443/` |
| **62443-4-2 TRF (SL-C 2)** | `62443/trf/README.md` · `62443/trf/READ.md` · `scripts/62443/export-trf-worksheet.py` |
| TesPro supplier slice | `62443/TesPro_IEC62443_Supplier_Clauses.xlsx` |
