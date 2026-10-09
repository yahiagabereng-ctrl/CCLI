# CID / CFG / DUT alignment check — 2026-10-09 (evening)

**Trigger:** TSP Compare Model (~489 rows) + question whether workspace CID is latest.

---

## 1. Repo — canonical SCL (TSP / MICS)

| Item | Value |
|------|--------|
| File | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| Header `revision` | **7** (2026-10-09 — LLN0 repair) |
| SHA256 | `b028022b5a45b0942876aadf4ccc3597a974537be882080e7f11bec7927f7a32` |
| `LLN0.NamPlt.configRev` in CID | **`20261009`** |
| Other `.cid` in tree | `lab_tg544_tls_bench.cid` rev **3** (TLS fork — not compare reference) |

**Verdict:** TSP must use **`lab_tg544_eth_a.cid` rev 7** from this repo path. History pane showing rev 7 + LLN0 repair = correct file generation.

---

## 2. Repo — runtime model (DUT `model_cfg`)

| Item | Value |
|------|--------|
| File | `apps/ccli/config/icd/lab_tg544_eth_a.cfg` |
| SHA256 | `83c08352a29f4dfd543a1718ca21e90b9553b14691ac54c0c5e04056f924146c` |
| First `configRev` in cfg | **`20261002`** (21 LPL instances) |

**Verdict:** **MISMATCH vs CID rev 7** — `.cfg` was **not** regenerated after rev 6–7 CID edits. Online `NamPlt.configRev` comes from **cfg**, not from the `.cid` file on disk in TSP.

---

## 3. DUT live (192.168.10.1, SSH 2026-10-09)

| Check | Result |
|-------|--------|
| TCP **3782** from bench PC | **OPEN** |
| `ccli --version` | **0.1.0-r52** |
| `/etc/ccli/icd/` | **Only** `lab_tg544_eth_a.cfg` (no `.cid` on device — expected) |
| DUT cfg SHA256 | **`83c08352…`** — **matches repo cfg** |
| DUT `configRev` | **`20261002`** |

**Verdict:** DUT is **aligned with repo `.cfg`**, **not** with CID rev 7 **`configRev` 20261009**. MMS structure for controls (Wlim/WSd) still matches prior scoped Compare PASS; NamPlt / static CID-only fields will show **MisMatch**.

---

## 4. TSP Compare Model (~489 errors)

| Pattern | Meaning |
|---------|---------|
| **MissingFromDiscover** + empty Discovered column | **Stale / empty online discovery** — reconnect, full browse, run Compare **before** URCB enable (see annex runbook). |
| **~489 with Match/MisMatch filled** | Normal **scoped** compare: leaf DA / PPV / live RCB ≠ static CID (waivers in `TSP_P3_COMPARE_2026-10-09.txt`). |

**Verdict:** High error count **does not** mean “wrong CID revision” if discovery was not refreshed. Empty discover side **does** mean Compare was run too early or model tree not loaded.

---

## 5. Recommended actions (operator)

1. **TSP:** Confirm import path → `...\apps\ccli\config\icd\lab_tg544_eth_a.cid` (rev **7**).
2. **Compare:** Associate TLS **3782** → wait for full model browse → **Compare Model** (prefer **before** URCB).
3. **Align configRev (optional, for green NamPlt):**
   - Regenerate: `scripts/gen-mms-model-cfg.ps1` (from current CID).
   - Bump / verify all `configRev` = **`20261009`** in generated cfg.
   - Deploy cfg: `deploy-ccli-session-fix.ps1` (or pscp to `/etc/ccli/icd/`) + `/etc/init.d/ccli restart`.
4. **Re-run** Compare; expect **WlimDWMX1** / **WSdDAGC1** **Match** (gate); LLN0 **configRev** should Match after step 3.

---

## Cross-refs

- `TSP_P3_COMPARE_2026-10-09.txt` (CFG-REV-01 waiver)
- `TSP_SIDEBAR_TRIAGE_2026-10-09.md` (CID rev 7 SCL Verify PASS)
- `lab/conformance/LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md` (discovery before Compare)
