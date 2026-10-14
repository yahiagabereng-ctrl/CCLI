# T1-01 Connect — FAIL summary (PC1, 2026-10-14)

**Git at capture:** `2eba126`  
**Operator:** Masomeh (Test Suite Pro PC)  
**DUT target:** `192.168.10.1:3782` · IED `CCI016_01` · mutual TLS + MMS certificate

## TSP error

```text
CCI016_01: Initiate Response Failed: Transport connection closed
```

## PC1 checks (same session)

| Check | Result |
|-------|--------|
| TCP `192.168.10.1:3782` from `192.168.10.10` | OK |
| `verify-tsp-tls-key-match.ps1` on field pack | PASS |
| OpenSSL client auth (`client_tls.pem` + `client_tsp.key`) | **CONNECTION ESTABLISHED** (TLS 1.3) |

Full log: [PC1_CONNECT_DIAG_2026-10-14.txt](PC1_CONNECT_DIAG_2026-10-14.txt)

## MD5 note

PC1 bundle MD5s match `C:\CCLI_TSP_FIELD_PACK\tls\DUT_TLS_MANIFEST.json` (EJBCA sync tree).  
They **do not** match the older spot-check table in `MASOMEH_CONNECT_FIX_2026-10-14.md` until DUT TLS is redeployed to the same tree. OpenSSL accept implies DUT allowlist matches **current** `client_tls.pem` on PC1.

## Likely causes (ordered)

1. TSP still using **cached** cert paths — re-browse every TLS/MMS field under `C:\CCLI_TSP_FIELD_PACK\tls\`.
2. **MMS Security** wrong key (`client_tsp.key` on MMS tab) — must be `client_acse.key` + `client.pem`.
3. **DUT stack:** `iec61850d` or second listener on 3782 — needs `scripts/dut-mms-tsp-preflight.ps1` from logger PC (SSH).
4. Post-TLS **ACSE/CID** — capture TSP sniffer + DUT `/tmp/ccli.log` on next attempt.

## Evidence files

- [TSP_P3_01_CONNECT_2026-10-14.txt](TSP_P3_01_CONNECT_2026-10-14.txt) — T1-01 stub, result FAIL
- [TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md](TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md) — team triage
