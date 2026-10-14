# Unified cert session pin — DUT deploy 2026-10-14

**Document ID:** CCLI-LAB-UNIFIED-PIN-001  
**RAG source_id:** `ccli-lab-unified-session-pin-2026-10-14`  
**Matrix:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) rev **1.4** · rows **U-P0-11**, **U-P0-07**, cyber **U-CYB-1003/1004-***  
**Repo version truth:** `apps/ccli/VERSION` → **0.1.0-r54** (`P5-5LN-CERT-MODEL`)

## Session header (current — r54 + 5-LN product model live)

| Field | Value |
|-------|--------|
| Date | 2026-10-14 |
| Specimen | **5-LN product model on DUT** — `/etc/ccli/icd/product_tr400_eth_a_5ln.cfg` (CID rev **1**, configRev **20261014**) |
| DUT `ccli --version` | **0.1.0-r54** (P5-5LN-CERT-MODEL) |
| Binary SHA-256 | `705752f8c0440a659e1726cba516753be42fff6d6297feba4a094db5cd43e885` |
| Lab yaml | **`lab_tr400_phase4_eth_b_ttyS1_5ln.yaml`** (cert run) · 31-LN fallback `lab_tr400_phase4_eth_b_ttyS1.yaml` |
| CID SHA-256 | `68C72E7F87D1C65B04A3E42925E727815EC8B1A421E0DC65296C7C65B3CF2582` |
| cfg SHA-256 (DUT verified = repo) | `7E13DE139559ADC68DA0BBAE0C8AB0BD20ABDED6AF3472A9974CD4A5BC6A3146` (6604 bytes) |
| MMS | `192.168.10.1:3782` TLS · crl=on · `model loaded from /etc/ccli/icd/product_tr400_eth_a_5ln.cfg` |
| IEC 104 | `192.168.1.130:2404` TLS |
| Preflight | `scripts/validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence` |
| Deploy manifest | [CCLI_DEPLOY_0.1.0-r54_2026-10-14_150409.md](../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r54_2026-10-14_150409.md) |
| Prior r53 manifest | [CCLI_DEPLOY_0.1.0-r53_2026-10-14_142430.md](../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r53_2026-10-14_142430.md) |

## r54 code delta (vs r53)

| Area | Change |
|------|--------|
| MMS | **VArSdDVAR1 / PFSPDFPF1 optional** in `wire_cfg_runtime_nodes` — 5-LN cert model boots; O.9.1.x logged **inactive** |
| Model | `product_tr400_eth_a_5ln.cid` + `.cfg` in git (specimen pin [PRODUCT_SPECIMEN_5LN.md](PRODUCT_SPECIMEN_5LN.md)) |
| Yaml | `lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` — `model_cfg` → product 5-LN |
| Deploy | `deploy-ccli-session-fix.ps1` uploads product cfg to `/etc/ccli/icd/` when present |

## r53 code delta (vs r52)

| Area | Change |
|------|--------|
| Modbus | EMT432 **V_L1_L2 @980** → `PdCMMXU1.PPV` when `reg_line_voltage` set in yaml |
| Config | `line_voltage_scale`, `reg_line_voltage_count` in `ccli_config` |
| Deploy | `deploy-ccli-dut.sh` disables **iec61850service** before **ccli** start |

## Matrix readiness

| U-ID | r54 |
|------|-----|
| U-P0-07 / U-D7-01 | Preflight script (reads **VERSION** from repo) |
| U-T1-04 PPV | **PASS** when EMT432 yaml + meter; else RTU bench uses yaml fallback |
| U-T1-02 / U-T1-03 5-LN | **UNBLOCKED** — CID in git + cfg live on DUT · `validate-5ln-cid.ps1` **PASS** 2026-10-14 · TSP import `cid\product_tr400_eth_a_5ln.cid` |

## Deploy

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
# After wsl-build-ccli.sh:
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 -HostAddr 192.168.1.130 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_5ln.yaml `
  -Changes @("5-LN cert session")
powershell -File scripts\validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence
```

**RAG:** `ccli-lab-unified-cert-test-matrix` · `ccli-product-sw-freeze` · `ccli-product-specimen-5ln`
