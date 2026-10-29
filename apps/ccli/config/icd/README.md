# Allegato T ICD / logical node definitions (K4.3)

Place CID/ICD exports and mapping tables here.

## Normative sources

| Document | Status | Notes |
|----------|--------|-------|
| CEI 0-16 **Allegato T** | **HAVE** — `knowledge-base/08-engineering/CCI_Annex_T_Extract.md` | Data-model tables, MMS timing, cyber |
| CEI 0-16 consolidated PDF | **ON DISK** — `knowledge-base/07-protocols/0-16consolidata.pdf` | Italian; doc. pp. 534–653 |
| **CEI TR 57-126** — SCL example for CCI 61850 | **HAVE** — `knowledge-base/08-engineering/CCI_TR_57-126_Extract.md` | Referenced in Annex T §T.3 |
| IEC 61850-7-3 (CDC) | **HAVE** — `knowledge-base/08-engineering/CCI_61850-7-3_Extract.md` | DOType `@cdc` · libiec61850 `CDC_*` |
| IEC 61850-7-4 (LN/DO) | **HAVE** — `knowledge-base/08-engineering/CCI_61850-7-4_Extract.md` | MMXU TotW/TotVAr/PPV · 7-420 DER LNs |

## Artifacts

| File | Description |
|------|-------------|
| `cei-tr-57-126-example.cid` | Annex A reference CID from CEI TR 57-126 (PF1 + PF2 example plant) — **do not edit** |
| `lab_tg544_eth_a.cid` | **Lab** copy — Communication IP **`192.168.10.1/24`** (TG544 LAN1 Eth_A); 2026-09-25 |
| `lab_tg544_eth_a.cfg` | **Runtime model** (genconfig from CID) — **31 LNs**; deploy to `/etc/ccli/icd/` |
| `lab_tg544_aggregator.cid` | Additive CEI 57-142 Aggregator model, generated from the lab CID; preserves the DSO model |
| `lab_tg544_aggregator.cfg` | Runtime config generated from the Aggregator CID; select with `mms.model_cfg_path` |
| `signal_map.yaml` | **31-LN doc plane (rev 2, r29)** — equations + bindings; NOT loaded at runtime |
| `../plant/` | **CSV commissioning plane (2026-10-04)** — GOOSE data map + Modbus/Huawei + MMS points — see [`../plant/READ.md`](../plant/READ.md) |

## Usage

- Treat `cei-tr-57-126-example.cid` as a **reference baseline**, not a production freeze — override IED name, IP, POD, power limits, SGG count, and enabled PF2 functions per plant and DSO workbook.
- Regenerate CID from PDF: `python scripts/extract-cei-tr-57-126.py`
- Phase 3: load with libiec61850; validate reports (`intgPd=4000`) and SBO control paths.
- **r23 (2026-09-30):** full CID on wire via `mms.model_cfg` + `lab_tg544_eth_a.cfg`. Regenerate: `scripts/gen-mms-model-cfg.ps1`
- Legacy **9-LN MVP** if `model_cfg` unset in yaml.
- Aggregator variant (CEI 57-142:2026-07): regenerate the separate CID/runtime config with `python scripts/generate_aggregator_model.py --source apps/ccli/config/icd/lab_tg544_eth_a.cid --output apps/ccli/config/icd/lab_tg544_aggregator.cid --jar apps/ccli/third_party/libiec61850/tools/model_generator/genconfig.jar --cfg apps/ccli/config/icd/lab_tg544_aggregator.cfg`. Set `mms.model_cfg_path` to that CFG to use it. If it is unset, the hand-built model exposes the Aggregator interface too. Plant outputs remain disabled until a validated signed P/Q backend is implemented.

See also: `knowledge-base/08-engineering/CCI_CEI_0-16_Consolidated.md`
