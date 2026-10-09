# Phase 2 — step by step (3782 TLS)

**Date:** 2026-10-09 · **Export target:** `TSP_P5_FULLCIRCLE_TLS_2026-10-09_*.txt`  
**Cross-ref Phase 1:** [TSP_PHASE1_SESSION_INDEX_2026-10-09.md](TSP_PHASE1_SESSION_INDEX_2026-10-09.md)

| Step | P5 # | Status | Evidence |
|------|------|--------|----------|
| 1 | P5-00 chrony | **PASS** (RMS offset ~5.9 ms, last ~17 µs) | `TSP_P5_FULLCIRCLE_TLS_2026-10-09.txt` § step 1 |
| 2 | Connect :3782 | **PASS** (T1-10 + session green) | `TSP_P5_FULLCIRCLE_TLS_2026-10-09.txt` § step 2 |
| 3 | Browse 31 LN | **PASS** (T1-03 cross-ref) | `TSP_P5_FULLCIRCLE_TLS_2026-10-09.txt` § step 3 |
| 4 | Compare vs CID | **PASS** (scoped; no TSP xlsx extract) | `TSP_P3_COMPARE_2026-10-09.txt` + `.png` |
| 4b | SCL Verify (offline) | **DONE** (fill counts) | `TSP_OFF_SCLVERIFY_2026-10-09.txt` → `offline-scl/` after triage |
| 5 | Read PdC TotW/VAr/PPV | **NOW** | `TSP_P5_FULLCIRCLE_TLS_*.txt` § step 5 |
| 6–7 | URCB + timing | PENDING | reuse T1-05/06 or re-verify |
| 8–10 | Wlim / WSd | PENDING | reuse T1-07/08 or re-operate |
| 11 | VArSd (optional) | SKIP | |
| 12 | P sweep PC-A | PENDING | |
| 13 | Fallback 15 s | PENDING | reuse T1-09 |
| 14 | Export sequencer log | PENDING | |
