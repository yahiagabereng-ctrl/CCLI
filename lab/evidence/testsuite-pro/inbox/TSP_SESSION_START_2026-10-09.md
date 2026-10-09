# TSP session — START (2026-10-09)

**Status:** **GO** on **PC-B / LAN1** · MMS **3782** verified  
**Deferred:** LAN2/104 (PC-A), GNSS fix (note for P3-11 / URCB timing)

## Session record (fill as you go)

| Field | Value |
|-------|--------|
| Date | 2026-10-09 |
| Git | `5939fb8` (merge origin/main + r51) |
| DUT | `0.1.0-r48` · `/etc/ccli/lab.yaml` = phase4 ttyS1 |
| PC | `192.168.10.10` → `192.168.10.1:3782` |
| IED | `CCI016_01` |
| CID | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |

## Test Suite Pro — connection (T1-01)

| Setting | Value |
|---------|--------|
| IP / port | **`192.168.10.1:3782`** |
| Local IP | **`192.168.10.10`** |
| MMS Security | `C:\CCLI_product_tls\client.pem` + `client_tsp.key` |
| Transport TLS | `C:\CCLI_product_tls\client_tls.pem` + `client_tsp.key` |
| CA | `C:\CCLI_product_tls\root_CA.pem` |
| CRL in TSP | **Leave empty** (CRL warnings OK) |

**Connect** → expect **Initiate Response Success** → save screenshot/log as `TSP_P3_01_CONNECT_2026-10-09.*`

## Run order (Phase 1 — do not skip)

1. **T1-01** Connect (confirm green)  
2. **T1-03** Advanced Client: browse **`LD_Plant`** (~31 LNs)  
3. **T1-02** **Compare Model** vs CID (**before** enabling URCB)  
4. **T1-04** Read `PdCMMXU1` TotW, TotVAr, PPV  
5. **T1-05–06** Enable `urcb_PdC_Mis4sec` IntgPd=**4000** · check Δt  
6. **T1-07–08** Operate Wlim / WSd (enhanced security)  
7. **T1-09** Release · wait **15 s**  
8. **T1-10** Disconnect · reconnect  

Annex map: [LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md](../../conformance/LAB_SESSION_2026-10-09_ANNEX_RUNBOOK.md)

## Waivers / notes today

| Item | Note |
|------|------|
| TotW live values | Without PC-A Modbus slave, reads may be mock/stale — document in log |
| GNSS | NTP chrony OK; `clockNotSynchronized=1` — waiver for strict T.3.3.4.5 until GNSS on |
| Compare Model | Run **after** full browse; export `TSP_P3_COMPARE_*.xlsx` |
| LAN2 / 104 | **Not in this session block** — see [LAN2_PC-A_SETUP.md](../../LAN2_PC-A_SETUP.md) |

## Evidence drop

`lab/evidence/testsuite-pro/inbox/TSP_*` · `pcap/TSP_*`

**Per test:** always run the logger before/after each T1 step:

```powershell
powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-xx
powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
```

See [EVIDENCE_PER_TEST_PROTOCOL.md](../EVIDENCE_PER_TEST_PROTOCOL.md).
