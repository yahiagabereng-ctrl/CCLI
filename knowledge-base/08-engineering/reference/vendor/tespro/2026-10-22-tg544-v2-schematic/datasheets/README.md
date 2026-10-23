# TG-544 V2 — downloaded component datasheets

Place vendor PDFs here as `<MPN>.pdf`.

| Doc | Purpose |
|-----|---------|
| [DOWNLOAD_STATUS_2026-10-23.md](DOWNLOAD_STATUS_2026-10-23.md) | **HAVE / MISSING** after automated pull |
| `lab/evidence/hw/TG544_V2_DATASHEET_TRACKER.md` (repo root) | Full P0/P1 list |

```powershell
python scripts/extract-tg544-v2-datasheets.py          # validate PDFs + _extracts/
powershell -File scripts/ingest-tespro-tg544-v2-schematic.ps1
powershell -File scripts/download-tg544-v2-datasheets.ps1   # optional re-fetch
```

Ingest result: `_extracts/INGEST_CHECK.txt` · status table: [DOWNLOAD_STATUS_2026-10-23.md](DOWNLOAD_STATUS_2026-10-23.md)
