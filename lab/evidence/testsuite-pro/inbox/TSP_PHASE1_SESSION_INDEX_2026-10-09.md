# Phase 1 evidence index - 2026-10-09

**Operator sign-off:** T1–T10 executed in Test Suite Pro (2026-10-09). DUT finalize: [DUT_LAB_FINALIZE_AND_CERT_PATH.md](../../../conformance/DUT_LAB_FINALIZE_AND_CERT_PATH.md).

Protocol: [EVIDENCE_PER_TEST_PROTOCOL.md](../EVIDENCE_PER_TEST_PROTOCOL.md)

| TSP ID | TSP log | DUT SSH | Wire summary | Pcap | Result |
|--------|---------|---------|--------------|------|--------|
| T1-01 | `TSP_P3_01_CONNECT_2026-10-09.txt` | `TSP_P3_01_CONNECT_2026-10-09_DUT_LOG.txt` | `TSP_P3_01_CONNECT_2026-10-09_WIRE.txt` | `TSP_P3_01_CONNECT_2026-10-09.pcapng` | **PASS** (session) |
| T1-02 | `TSP_P3_COMPARE_2026-10-09.txt` | — | — | optional | **PASS (scoped)** + waivers |
| T1-03 | `TSP_P3_03_BROWSE_2026-10-09.txt` | optional | optional | optional | **PASS** |
| T1-04 | `TSP_P3_03_READ_PdC_2026-10-09.txt` | optional | optional | optional | **PASS** |
| T1-05 | `TSP_P3_03_REPORT_URCB_2026-10-09.txt` | optional | optional | optional | **PASS** |
| T1-06 | `TSP_P3_03_REPORT_TotW_2026-10-09.txt` | optional | optional | optional | **PASS** |
| T1-07 | `TSP_P3_04_OPERATE_Wlim_2026-10-09.txt` | optional | optional | optional | **PASS** |
| T1-08 | `TSP_P3_04_OPERATE_WSd_2026-10-09.txt` | `TSP_P3_04_OPERATE_WSd_2026-10-09_DUT_LOG.txt` | `TSP_P3_04_OPERATE_WSd_2026-10-09_WIRE.txt` | `TSP_P3_04_OPERATE_WSd_2026-10-09.pcapng` | **PASS** (wspt20; pcap optional) |
| T1-09 | `TSP_P3_05_FALLBACK_2026-10-09.txt` | `TSP_P3_05_FALLBACK_2026-10-09_DUT_LOG.txt` | `TSP_P3_05_FALLBACK_2026-10-09_WIRE.txt` | `TSP_P3_05_FALLBACK_2026-10-09.pcapng` | **PASS** |
| T1-10 | `TSP_P3_01_RECONNECT_2026-10-09.txt` | `TSP_P3_01_RECONNECT_2026-10-09_DUT_LOG.txt` | `TSP_P3_01_RECONNECT_2026-10-09_WIRE.txt` | `TSP_P3_01_RECONNECT_2026-10-09.pcapng` | **PASS** |

Before each test:

```powershell
. D:\CCLI\CCLI\CCLI\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-xx -Operator "<name>"
```

After each test (copy TSP Output first):

```powershell
powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
```
