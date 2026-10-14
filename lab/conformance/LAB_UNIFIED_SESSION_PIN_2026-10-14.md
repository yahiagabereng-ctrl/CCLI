# Unified cert session pin — DUT deploy 2026-10-14

**Document ID:** CCLI-LAB-UNIFIED-PIN-001  
**RAG source_id:** `ccli-lab-unified-session-pin-2026-10-14`  
**Matrix:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) rev **1.3** · rows **U-P0-11**, **U-P0-07**  
**Repo version truth:** `apps/ccli/VERSION` → **0.1.0-r53** (`P5-PPV-EMT432-UNIFIED-CERT`)

## Session header (target after r53 deploy)

| Field | Value |
|-------|--------|
| Date | 2026-10-14 |
| Specimen | **5-LN cert intent** · interim MMS **`lab_tg544_eth_a.cfg`** until `product_tr400_eth_a_5ln.cid` |
| DUT `ccli --version` | **0.1.0-r53** (P5-PPV-EMT432-UNIFIED-CERT) |
| Binary SHA-256 | `11a9a4f0fe70bb066e31d9cdca38321e7c900cf7914a20fd0c534d011b3e14da` |
| Lab yaml | `lab_tr400_phase4_eth_b_ttyS1.yaml` or `lab_tr400_phase4_emt432_lan3_tcp.yaml` when meter live |
| MMS | `192.168.10.1:3782` TLS |
| IEC 104 | `192.168.1.130:2404` TLS |
| Preflight | `scripts/validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence` |
| Deploy manifest | [CCLI_DEPLOY_0.1.0-r53_2026-10-14_142430.md](../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r53_2026-10-14_142430.md) |

## r53 code delta (vs r52)

| Area | Change |
|------|--------|
| Modbus | EMT432 **V_L1_L2 @980** → `PdCMMXU1.PPV` when `reg_line_voltage` set in yaml |
| Config | `line_voltage_scale`, `reg_line_voltage_count` in `ccli_config` |
| Deploy | `deploy-ccli-dut.sh` disables **iec61850service** before **ccli** start |

## Matrix readiness

| U-ID | r53 |
|------|-----|
| U-P0-07 / U-D7-01 | Preflight script (reads **VERSION** from repo) |
| U-T1-04 PPV | **PASS** when EMT432 yaml + meter; else RTU bench uses yaml fallback |
| U-T1-02 / U-T1-03 5-LN | **HAVE** — `apps/ccli/config/icd/product/product_tr400_eth_a_5ln.cid` in repo · run `validate-5ln-cid.ps1` before Compare |

## Deploy

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
# After wsl-build-ccli.sh:
.\lab\tg544-openwrt\deploy-ccli-bench-lan2.ps1
powershell -File scripts\validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence
```

**RAG:** `ccli-lab-unified-cert-test-matrix` · `ccli-product-sw-freeze`
