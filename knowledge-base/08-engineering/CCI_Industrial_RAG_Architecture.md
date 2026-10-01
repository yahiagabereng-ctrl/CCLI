# CCI Industrial RAG Architecture (three indexes)

**Doc ID:** CCLI-RAG-ARCH-001  
**Revision:** 1.0  
**Date:** 2026-07-25  
**Target score:** ~9.8/10 industrial engineering RAG (from ~8.5/10 vector-first baseline)

---

## 1. Three indexes

| Index | Role | Where |
|-------|------|--------|
| **Vector** | Semantic similarity over chunk embeddings | Telematry SQLite `chunks.embedding_json` (+ optional Qdrant) |
| **Keyword** | Exact token / acronym retrieval (HAB, TPM, IEC62351, 99991105…) | Token overlap + exact-term boost; optional Elasticsearch BM25 |
| **Knowledge graph** | Typed reasoning paths | Seed JSON + SQLite `graph_edges` |

**Hybrid fusion (query time):**

```
score = 0.50 × vector + 0.50 × keyword
      + chunk_type_boost + context_boost + entity_overlap_boost
      + exact_industrial_term_boost (capped)
```

Then attach **seed graph paths** and **missing_sources** (from regulatory map) into query traces / citation `graph_paths`.

---

## 2. Artifacts

| Artifact | Path |
|----------|------|
| Seed knowledge graph | `08-engineering/cci-knowledge-graph.json` |
| Entity lexicon (CCI) | Telematry `rag-python/car_manual_pipeline/dictionaries/ccli_entities.yaml` |
| Regulatory routing | `07-protocols/CCI_Module_Regulations_Classification.md` (+ `.json`) |
| Eval set | `08-engineering/cci-hybrid-rag-eval.json` |
| Entity extract | `entity_extraction.py` (loads car + CCI dictionaries) |
| Graph expand | `graph_rag.py` (`seed_graph_paths_for_query`, `missing_sources_for_query`) |
| Hybrid retrieve | `retrieval.py` (`POST /v1/query`) |

Env override for graph path: `CCI_KNOWLEDGE_GRAPH_PATH`.

---

## 3. Edge types (seed graph)

`ENABLES` · `IMPLEMENTS` · `REQUIRES` · `MAPS_TO` · `COVERED_BY` · `PARTIAL` · `MISSING` · `CERT_CLAIM` · `RELATED`

Example spine:

```
i.MX 8 → TrustZone → Secure Boot → HAB
Eth_A → IEC 61850 → IEC 62351
Verdin 99991105 -[PARTIAL]-> C1 Ethernet
Verdin 99991105 -[MISSING]-> C2 RS-485 / C4 DI/DO / C5 GNSS
```

---

## 4. Query contract (already on `/v1/query`)

Request: `tenant_id`, `bot_id`, `query`, `source_ids[]`, `top_k`, `grounding_prompt`.

Response traces now include:

- `hybrid_mode`
- `query_entities`
- `seed_graph_paths`
- `missing_sources` (MISSING/PARTIAL norms from seed `rag_status_refs`)
- Citations carry `graph_paths` + `chunk_metadata.entity_names` (after re-ingest)

---

## 5. Apply after code deploy

1. Restart RAG container / process so new dictionaries and scoring load.
2. Re-ingest CCLI sources so chunk metadata gets CCI entities:

```powershell
cd "C:\Yahia\projects\Telematry System"
.\scripts\ingest-ccli.ps1
```

3. Smoke-test:

```powershell
$body = @{
  tenant_id = "telematry"; bot_id = "default"
  query = "What CCI controls does Toradex Verdin 99991105 not cover regarding HAB and RS-485?"
  source_ids = @("ccli-vendor-selection","ccli-toradex-verdin-devboard","ccli-cci-module-regs-class")
  top_k = 8; answer_language = "en"; clarify = $false
} | ConvertTo-Json
Invoke-RestMethod http://localhost:8001/v1/query -Method POST -Body $body -ContentType "application/json" |
  Select-Object -ExpandProperty traces | ConvertTo-Json -Depth 5
```

Expect: `query_entities` containing `hab` / `verdin` / `rs485`, `seed_graph_paths` with PARTIAL/MISSING edges, `missing_sources` empty or limited unless norms are asked.

---

## 6. Still out of MVP (next)

- Full Okapi BM25 / SQLite FTS5 inverted index (today: overlap + exact boost + optional ES)
- Semantic re-chunking pass (heading/table-aware) beyond current classifiers
- LLM entity extraction second pass
- Graph DB (Neo4j) — seed JSON is enough until scale

---

## 7. Keywords

`hybrid rag`, `bm25`, `knowledge graph`, `entity extraction`, `HAB`, `TrustZone`, `IEC 62351`, `CEI 0-16`, `ccli-knowledge-graph`, `industrial rag`
