# Unified cert session pin — DUT deploy 2026-10-14

**Matrix:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](LAB_UNIFIED_CERT_TEST_MATRIX.md) rev 1.2  
**Deploy manifest:** [../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r52_2026-10-14_134735.md](../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r52_2026-10-14_134735.md)

## Session header (filled from live DUT)

| Field | Value |
|-------|--------|
| Date | 2026-10-14 |
| Specimen | **5-LN cert intent** · **interim MMS cfg** `lab_tg544_eth_a.cfg` (31-LN) until `product_tr400_eth_a_5ln.cid` |
| DUT `ccli --version` | **0.1.0-r52** (P3-07-AARE-SIGN-RAW-GT) |
| Binary SHA-256 | `1df82c26243021a86c533a179781c749fc852c2f724efa3b0aa4018b2969eca1` |
| Lab yaml | `lab_tr400_phase4_eth_b_ttyS1.yaml` → `/etc/ccli/lab.yaml` |
| MMS | `192.168.10.1:3782` TLS · `model_cfg` `/etc/ccli/icd/lab_tg544_eth_a.cfg` |
| IEC 104 | `192.168.1.130:2404` TLS |
| Modbus | RTU `/dev/ttyS1` (COM5 Sun mock) |
| Preflight script | `scripts/validate-dut-unified-matrix.ps1` → `PREFLIGHT_DUT_UNIFIED_2026-10-14.txt` |
| TSP version | *(fill on PC1)* |
| Operator | *(fill)* |

## Matrix rows ready on DUT (code + deploy)

| U-ID | Status | Notes |
|------|--------|--------|
| U-P0-07 | **Automated** | Single `ccli` · `iec61850service` disabled on deploy |
| U-P0-08 | **PASS** | Version + SHA pinned above |
| U-D7-01 | **PASS** | No `:102` listener (3782/2404 TLS only) |
| U-P0-06 | **PASS** | TLS under `/etc/ccli/tls/` |
| U-T1-01 … U-T3-05 | **Ready** | Run TestPro + `tsp-evidence-logger.ps1` |
| U-T1-02 / U-T1-03 | **INTERIM** | Browse/Compare vs **5-LN CID** blocked until product CID; use waivers or lab Compare for bench only |
| U-T1-04 PPV | **GAP→r53** | EMT432 PPV @980 in source; **r52** uses `plant.poc_ppv_kv` unless rebuilt |
| U-P0b-01 | **Bench** | Switch yaml to `lab_tr400_phase4_emt432_lan3_tcp.yaml` when meter probed |

## Next code / deploy actions

1. **`git pull`** · build **`ccli-bin`** (`lab/tg544-openwrt/wsl-build-ccli.sh`) for **PPV-from-meter** + bump **r53**.
2. Add **`product_tr400_eth_a_5ln.cid`** + cfg · `validate-5ln-cid.ps1` PASS · redeploy `model_cfg`.
3. PC1: **`tsp-evidence-logger.ps1 -Action Init`** → execute [LAB_UNIFIED_TSP_5LN_EXECUTION.md](LAB_UNIFIED_TSP_5LN_EXECUTION.md).

**RAG source_id:** `ccli-lab-unified-cert-test-matrix`
