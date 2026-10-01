# TSP SCL Verify triage — 2026-09-26 r2 (post-CID fix)

# CCLI TSP evidence
tsp_version: 4.7.4.5037
cid: apps/ccli/config/icd/lab_tg544_eth_a.cid (rev 3)
ied: CCI016_01
gate: OFF
tool: SCLVERIFY
source: apps/ccli/config/icd/Report 2.xlsx
evidence: lab/evidence/testsuite-pro/offline-scl/TSP_OFF_SCLVERIFY_2026-09-26_r2.xlsx
date: 2026-09-26

## Delta vs r1

| Severity | r1 | r2 | Δ |
|----------|---:|---:|--:|
| Error | 59 | **5** | −54 |
| Warning | 65 | 52 | −13 |
| Recommendation | 39 | 13 | −26 |
| Information | 5 | 5 | 0 |
| **Total** | **168** | **75** | **−93** |

**Verdict:** CtlModelKind / ctlModel Val / configRev fix **confirmed**. Cascade cleared.

## Remaining Errors (5) — disposition

| # | Message | Disposition |
|---|---------|-------------|
| 1–4 | Mod/Beh `on-blocked` ≠ master `blocked` | **ACCEPT (lab)** — CEI model naming |
| 5 | `SupSubscription` but no LGOS/LSVS | **DEFER** — trim Services or add LGOS when needed |

## Remaining Warnings / Recommendations — disposition

| Cluster | Count | Disposition |
|---------|------:|-------------|
| Custom LN (DPCC, DWMX, DAGC, …) | 15 | **ACCEPT** — Annex T / Figura 2 |
| `sboTimeout` / `operTimeout` unset | 26 | **DEFER** — set defaults when hardening SBO |
| `stSeld` not found (sbo-with-enhanced-security) | 13 | **DEFER** — add DA if strict SBO-enhanced required |
| Services GOOSE / Log / TimerActivated claims | 3 | **DEFER** — lab-honest Services trim |
| Duplicate Template pairing (TSP quirk) | 8 | **ACCEPT** — false adjacent-id pairing |

## Offline SCL gate status

| Gate | Status |
|------|--------|
| TSP_OFF SCL Verify (blocking Errors from ctlModel corruption) | **PASS** (cleared) |
| Residual Errors (Mod enum + SupSubscription) | **KNOWN / non-blocking for lab** |
| Live Connect :3782 TLS | Still Phase B — not started |

## Next (optional)

1. ~~Draft sequencer / TLS stage~~ → `lab/CCI_TestSuitePro_Sequencer_Draft.md` + `scripts/stage-tsp-tls.ps1` (**done**).  
2. In TSP: enter sequencer steps 1–6 offline; wire TLS profile to `…\CCLI_tls\` — keep Disconnected.  
3. When DUT up: Phase B Connect → export `TSP_P3_*` to `inbox/`.  
4. Optional CID harden: `sboTimeout` / `operTimeout` / `stSeld`; trim Services.
