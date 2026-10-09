# Phase 1 evidence index - 2026-10-09

Protocol: [EVIDENCE_PER_TEST_PROTOCOL.md](../EVIDENCE_PER_TEST_PROTOCOL.md)

| TSP ID | TSP log | DUT SSH | Wire summary | Pcap | Result |
|--------|---------|---------|--------------|------|--------|
| T1-01 | `TSP_P3_01_CONNECT_2026-10-09.txt` | `TSP_P3_01_CONNECT_2026-10-09_DUT_LOG.txt` | `TSP_P3_01_CONNECT_2026-10-09_WIRE.txt` | `TSP_P3_01_CONNECT_2026-10-09.pcapng` | PENDING |
| T1-02 | `TSP_P3_COMPARE_2026-10-09.txt` | `TSP_P3_COMPARE_2026-10-09_DUT_LOG.txt` | `TSP_P3_COMPARE_2026-10-09_WIRE.txt` | `TSP_P3_COMPARE_2026-10-09.pcapng` | PENDING |
| T1-03 | `TSP_P3_03_BROWSE_2026-10-09.txt` | `TSP_P3_03_BROWSE_2026-10-09_DUT_LOG.txt` | `TSP_P3_03_BROWSE_2026-10-09_WIRE.txt` | `TSP_P3_03_BROWSE_2026-10-09.pcapng` | PENDING |
| T1-04 | `TSP_P3_03_READ_PdC_2026-10-09.txt` | `TSP_P3_03_READ_PdC_2026-10-09_DUT_LOG.txt` | `TSP_P3_03_READ_PdC_2026-10-09_WIRE.txt` | `TSP_P3_03_READ_PdC_2026-10-09.pcapng` | PENDING |
| T1-05 | `TSP_P3_03_REPORT_URCB_2026-10-09.txt` | `TSP_P3_03_REPORT_URCB_2026-10-09_DUT_LOG.txt` | `TSP_P3_03_REPORT_URCB_2026-10-09_WIRE.txt` | `TSP_P3_03_REPORT_URCB_2026-10-09.pcapng` | PENDING |
| T1-06 | `TSP_P3_03_REPORT_TotW_2026-10-09.txt` | `TSP_P3_03_REPORT_TotW_2026-10-09_DUT_LOG.txt` | `TSP_P3_03_REPORT_TotW_2026-10-09_WIRE.txt` | `TSP_P3_03_REPORT_TotW_2026-10-09.pcapng` | PENDING |
| T1-07 | `TSP_P3_04_OPERATE_Wlim_2026-10-09.txt` | `TSP_P3_04_OPERATE_Wlim_2026-10-09_DUT_LOG.txt` | `TSP_P3_04_OPERATE_Wlim_2026-10-09_WIRE.txt` | `TSP_P3_04_OPERATE_Wlim_2026-10-09.pcapng` | PENDING |
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
