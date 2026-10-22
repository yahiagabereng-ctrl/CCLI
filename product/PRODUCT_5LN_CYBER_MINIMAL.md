# Product tree — 5-LN + full cyber (minimal surface)

**Document ID:** CCLI-PROD-5LN-CYBER-001  
**Date:** 2026-10-15  
**Tree:** `product/apps/ccli/`  
**Entry:** `program/main.c` (`ccli_program`)  
**Version:** `0.1.0-r55` (`P5-5LN-CERT-MODEL`) — aligned with lab freeze · [releases/product-cyber/0.1.0-r55/FREEZE.md](../releases/product-cyber/0.1.0-r55/FREEZE.md)  
**Specimen:** [`lab/conformance/PRODUCT_SPECIMEN_5LN.md`](../lab/conformance/PRODUCT_SPECIMEN_5LN.md)

---

## Verdict

**Minimal product surface is the 5-LN CID on the wire plus the full Eth_A/Eth_B cyber stack.**  
It is **not** a second slim binary: `program/` still ticks PF2, Modbus, I/O, O.14, MMS, and 104. Extra LNs (VArSd/PFSP) and GOOSE **must not appear** on the DSO MMS association.

| Surface | Product requirement | How it is enforced |
|---------|---------------------|--------------------|
| MMS LNs | Exactly **5** | `config/icd/product/product_tr400_eth_a_5ln.cid` + `.cfg` |
| Eth_A | **:3782 TLS 1.2** + **ACSE Cert B** + **CRL** + mutual auth | `config/product_tr400.yaml` · startup **REFUSE** if MMS without TLS or port **102** |
| Eth_B | **:2404 TLS** (operator) | Same yaml; startup **REFUSE** if 104 on without TLS |
| GOOSE | **Off** (no GoCB in 5-LN CID) | `CCLI_WITH_GOOSE=OFF` (CMake default) · yaml `goose.enabled: false` |
| Cleartext MMS | **Forbidden** | Startup gate + do not ship `lab_tr400_cleartext_tsp*.yaml` as DUT config |

---

## 5 LNs (certificate / DSO)

| LN | Code path (required) |
|----|----------------------|
| `LLN0` | Datasets / URCB in CID · loaded via `mms.model_cfg` |
| `LPHD1` | CID only |
| `PdCMMXU1` | `TransmissionMms_PublishTask` → TotW / TotVAr / PPV |
| `WlimDWMX1` | `mms_adapter` required nodes · `CcliBridge_Mms_PollAndApplyDsoCommand` → PF2 |
| `WSdDAGC1` | Same as Wlim |

**Not in CID (keep code, keep inactive):** `VArSdDVAR1`, `PFSPDFPF1` — `wire_cfg_runtime_nodes` logs *inactive* and all consumers are nullptr-guarded.

---

## Full cyber (product yaml)

| Item | Yaml keys | Standard |
|------|-----------|----------|
| TLS 1.2 MMS | `mms.tls_enabled` · `tcp_port: 3782` | 62351-3 · Allegato T.3.3.4 |
| Dual cert G.2 | `tls_own_cert` (Cert A) · `tls_acse_cert` (Cert B) | 62351-4 Annex G |
| Mutual + RBAC | `tls_client_cert` · `tls_viewer_cert` | 62351-8 roles |
| CRL | `tls_crl` · `tls_revoked_cert` | 62351-3 §5.6.4.4 |
| 104 TLS | `iec104.tls_enabled` · `:2404` | 62351-3 on Eth_B |

Ship file: **`product/apps/ccli/config/product_tr400.yaml`**  
Bench (same cyber + 5-LN, DSO mock on): **`config/lab_tr400_phase4_eth_b_ttyS1_5ln.yaml`**

---

## Minimal `program/` modules (keep)

Required for 5-LN + cyber + PF2 plant:

`main` · `boot` · `startup` · `fw_config` · `ccli_bridge` · `parser_modbus` · `f_pf2` · `regulation` · `outputs` · `drvgpio` · `transmission_mms` · `transmission_104` · `synchronize` · `alarm` · `alarm_input` · `queue_my` · `commands`

**Keep but idle unless yaml enables:** `transmission_goose.c` (yaml off + compile off).  
**Stub:** `program_flash.c`.

Do **not** delete `adapters/iec61850_mms/*` ACSE/AARE files — they **are** full cyber.

---

## Lab-only files in this tree (do not activate on product DUT)

| File | Why not product |
|------|-----------------|
| `config/lab_tr400_cleartext_tsp*.yaml` | :102 no TLS |
| `config/lab_tr400_goose_plant.yaml` | GOOSE plant path |
| `config/icd/lab_tg544_eth_a.cid` | **31-LN** lab model |
| `--lab-demo` | Simulator ramp |

---

## Build

```bash
cmake -S product/apps/ccli -B product/apps/ccli/build-product \
  -DBUILD_PLATFORM=tg500 -DCCLI_LEGACY_MAIN=OFF -DCCLI_WITH_GOOSE=OFF
```

OpenWrt: `CCLI_SRC_DIR=.../product/apps/ccli` (see `product/README.md`).

---

## Gaps still OPEN (not code-minimal)

| Gap | Note |
|-----|------|
| Accredited **62351-100-3** | O.15.4 — lab booking, not this tree |
| **62443-4-1/4-2** cert | O.15.4 mandatory for market DoC |
| Native 62351-4 §13 E2E | A-profile + TLS first |
| On-target `.apk` of **this** entry | Host SK0153 cutover PASS; TG544 product-entry still required |
