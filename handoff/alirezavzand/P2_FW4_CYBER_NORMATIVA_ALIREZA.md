# P2 / fw4 vs cyber normativa — handoff for Alireza

**Date:** 2026-11-12  
**Repo:** https://github.com/yahiagabereng-ctrl/CCLI  
**Git commit:** *(pinned below after push — run `git pull origin main`)*

---

## Why this pack

Nicola mapped **TesproOS fw4** (TR500/TG544, **r601**) against **CEI O.13 / Annex T / 62443 / REQ-V** and the full **62351 + O.15 cyber** corpus. Use this when reviewing **zone firewall**, **P2-03 LTE isolation**, or **CSA / vendor cyber** questions.

---

## Start here (read order)

| # | Document | Role |
|---|----------|------|
| 1 | [`lab/evidence/CYBER_NORMATIVA_SERIES_COVERAGE.md`](../../lab/evidence/CYBER_NORMATIVA_SERIES_COVERAGE.md) | **Master index** — all cyber series & annexes (O, T, M, 62351, 62443, O.15 #3–#6) |
| 2 | [`lab/evidence/P2_fw4_NORMATIVA_COVERAGE_MATRIX.md`](../../lab/evidence/P2_fw4_NORMATIVA_COVERAGE_MATRIX.md) | **62-row** fw4 ↔ normativa table (Layer 1 only) |
| 3 | [`knowledge-base/08-engineering/CCI_62443_Zones.md`](../../knowledge-base/08-engineering/CCI_62443_Zones.md) | Zones Z0–Z5 · conduits · P2 as-built |
| 4 | [`lab/evidence/O13_1_isolation_2026-09-25/P2_01_CROSS_PING_MATRIX.txt`](../../lab/evidence/O13_1_isolation_2026-09-25/P2_01_CROSS_PING_MATRIX.txt) | Hard-gate evidence (2025-09-25 — **re-run on r601**) |

Clause-level rows: `lab/conformance/Full Validate.md` · `knowledge-base/08-engineering/CCI_62351_Clause_Verification_Matrix.md`

---

## Verdict (short)

| Area | Status |
|------|--------|
| **Inter-zone forward** (Eth_A / Eth_B / plant) | **PASS** pattern (same as P2-01/02 design) — **re-verify on r601** |
| **O.13.1.1.2 / T.3.3.4.11** (no IP maint on role ports) | **GAP** — INPUT accept-all; LuCI/SSH bind |
| **REQ-V-19/36** fw/UCI export on file | **GAP** — capture `nft` + `uci export firewall` on DUT |
| **Full cyber stack** (TLS, PKI, boot, 100-3 ACC) | **Not fw4** — see CYBER index §8 layer model |

---

## Suggested bench actions (Alireza / TLS / zone role)

1. On DUT **r601**: export firewall → `lab/evidence/P2_fw4_export_r601.txt` (see matrix §6 P0).  
2. Re-run **P2-01** cross-ping + **P2-03** `ping -I usb0` to DSO subnet.  
3. `ss -tlnp` on DUT — confirm **:3782** / **:2404** bind; no LuCI on **192.168.10.1**.

---

## Chat blurb

See [`MESSAGE_TO_SEND.txt`](MESSAGE_TO_SEND.txt) (section **fw4 cyber normativa**).
