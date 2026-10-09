# Lab specimen version pin — `lab_tg544_eth_a`

**Document ID:** CCLI-LAB-SPEC-VER-001  
**Revision:** 1.0  
**Date:** 2026-10-09  
**RAG source_id:** `ccli-lab-specimen-version`  
**Git (current pin):** commit `354843782c2899232e97f289f7a3b8318eb5d792` (2026-10-09)

Use this file as the **single reference** for which CID/cfg revision TSP, MICS, PICS, and Compare Model must load. Do not use ad‑hoc “LLN0 repair” CID edits that are not in git.

---

## Current pin (2026-10-09)

| Item | Value |
|------|--------|
| IED name | `CCI016_01` |
| Access point | `accessPoint1` · MMS bind **192.168.10.1:3782** (TLS lab) |
| CID path | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| **CID History revision** | **9** (`when=2026-10-09`) — **authoritative lab rev** |
| SCL `<Header revision="…">` | **3** (legacy export header; trace changes via `<History><Hitem revision="…">`) |
| `LLN0.NamPlt.configRev` | **20261009** (rev 9 Compare / OptFlds alignment) |
| Model cfg path | `apps/ccli/config/icd/lab_tg544_eth_a.cfg` |
| cfg generated from | CID History **rev 9** · `scripts/gen-mms-model-cfg.ps1` (+ RC options patch) |
| cfg size (bytes) | **33362** |
| cfg SHA-256 | `E3285E8F9B692C52DBFE6B141B308513ABCACA00F56F8702F0EC757478D77E49` |
| DUT deploy path | `/etc/ccli/icd/lab_tg544_eth_a.cfg` |
| Runtime mode | `model=full_cid_cfg` · yaml `model_cfg: /etc/ccli/icd/lab_tg544_eth_a.cfg` |
| LN count (Compare) | **31** |
| Lab firmware (validated) | `ccli 0.1.0-r52` (record actual `ccli --version` on each session) |

### Rev 6–7 model delta (Compare fix)

- **`MMXU2` LNType:** `TotW`, `TotVAr` only — **no** `PPV` / `A` on Gen* / St / SGG.
- **`MMXU1` (`PdCMMXU1`):** keeps `PPV` / `A` (POC measurement); **~60** TSP SDO/DO dual-Missing rows — **waive** (browse proves PPV).
- **URCB `OptFields`:** `entryID="false"` → cfg **`options=159`** (TSP `[0111110011]`).
- **BRCB `OptFields`:** `entryID="false"` → cfg **`options=191`** (TSP `[0111111010]`).
- **`configRev`:** **20261009** (CID + cfg LLN0 NamPlt).

Evidence: `lab/evidence/testsuite-pro/inbox/TSP_P3_COMPARE_FIX_2026-10-09.md` · waivers `TSP_P5_COMPARE_WAIVERS.md`.

---

## CID History (lab file)

| Hitem rev | Date | Summary |
|-----------|------|---------|
| 1 | 2022-08-05 | TR 57-126 CEI 0-16 example base |
| 2 | 2026-09-25 | Eth_A lab IP 192.168.10.1/24 |
| 3 | 2026-09-26 | SCL Verify enum / LPL configRev fixes |
| 4 | 2026-10-02 | LN/DO audit (MMXU2 + PPV/A added; DPCC, DGEN, DVVR, …) |
| 5 | 2026-10-02 | GOOSE GSEControl gcb_PdC_Mis4sec + gcb_Stato_Allarmi |
| 6 | 2026-10-09 | SCL Verify: Mod/Beh enum; PdC PPV/A SDI nesting |
| 7 | 2026-10-09 | LLN0 repair: DataSet–Report–DOI–GSE order |
| 8 | 2026-10-09 | MMXU2 drop PPV/A; URCB entryID=false (489→69 Compare) |
| **9** | **2026-10-09** | **configRev 20261009; BRCB entryID=false; cfg RC 159/191** |

---

## Regenerate and deploy cfg

```powershell
# From repo root
.\scripts\gen-mms-model-cfg.ps1
# Deploy (example): lab\tg544-openwrt\deploy-ccli-session-fix.ps1 or pscp to /etc/ccli/icd/
```

After regen: update **cfg SHA-256** and **git commit** in this document.

---

## Related documents (align to CID History rev 9)

| Document | ID | Doc rev | Specimen field |
|----------|-----|---------|----------------|
| [PIXIT_DRAFT.md](PIXIT_DRAFT.md) | CCLI-LAB-PIXIT-001 | 1.1 | OptFields / entryID |
| [SCL_DATASET_PLACEMENT.md](SCL_DATASET_PLACEMENT.md) | CCLI-LAB-SCL-DS-001 | 1.0 | CID rev 9 |
| [MICS_DRAFT.md](MICS_DRAFT.md) | CCLI-LAB-MICS-001 | 1.1 | Compare source CID |
| [PICS_FILL.md](PICS_FILL.md) | CCLI-LAB-PICS-FILL-001 | 1.1 | Cover CID rev |
| [CID_PICS_ALIGNMENT.md](CID_PICS_ALIGNMENT.md) | CCLI-LAB-CID-PICS-001 | 1.1 | Services vs rev 9 |
| [LAB_CONFIG_GUIDE.md](LAB_CONFIG_GUIDE.md) | CCLI-LAB-D6-001 | 1.1 | DUT paths |
| [TSP_P5_COMPARE_WAIVERS.md](../evidence/testsuite-pro/inbox/TSP_P5_COMPARE_WAIVERS.md) | — | — | 2026-10-09 section |
| [TSP_P3_COMPARE_FIX_2026-10-09.md](../evidence/testsuite-pro/inbox/TSP_P3_COMPARE_FIX_2026-10-09.md) | — | — | Fix pack |

---

## TSP / evidence log header (add to every export)

```text
cid: apps/ccli/config/icd/lab_tg544_eth_a.cid
cid_history_rev: 9
configRev: 20261009
model_cfg: apps/ccli/config/icd/lab_tg544_eth_a.cfg
model_cfg_sha256: E3285E8F9B692C52DBFE6B141B308513ABCACA00F56F8702F0EC757478D77E49
ccli_pkg: 0.1.0-rNN
git: <short commit>
```
