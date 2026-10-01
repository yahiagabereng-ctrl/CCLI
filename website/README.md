# CCLI Knowledge & Engineering Assistant (web)

Two-page site for the CCI knowledge base:

1. **Knowledge** (`/`) — structured explanation of the corpus + chatbot benefits  
2. **Assistant** (`/assistant`) — RAG chatbot against Telematry (`localhost:8001`)

## Run

```powershell
# Terminal 1 — Telematry RAG (if not already up)
cd "C:\Yahia\projects\Telematry System"
docker compose up -d

# Terminal 2 — website
cd c:\Yahia\projects\CCLI\website
npm install
npm run dev
```

Open **http://localhost:5177**

Vite proxies `/api/rag` → `http://localhost:8001`.

## Corpus analysis snapshot

Knowledge page section **Ingested documents & chunk scores** reads `public/corpus-stats.json`.

Rebuild after new ingest:

```powershell
cd c:\Yahia\projects\CCLI\website
.\scripts\build-corpus-stats.ps1
```

Shows CCLI sources, PDF/MD counts, chunk totals, quality mean/median/grade bands, and per-source table.
