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

## Root cause (Yahia check after `144deb9`)

| Layer | Finding |
|-------|---------|
| **PC1 PEMs vs DUT** | **MISMATCH.** PC1 bundle MD5s (`b28b0f41…` CA, `4490e817…` `client_tls`) ≠ DUT (`f265ae5c…`, `13c231ee…`). Field pack is an **older EJBCA tree**; `MASOMEH_CONNECT_FIX` spot-check table matches **current DUT**. |
| **OpenSSL “PASS”** | Handshake used **stale PC1** certs; may not match **today’s** DUT allowlist. Re-test after PEM refresh. |
| **DUT (2026-10-14 post-check)** | **`iec61850d` respawning**; at one point **3782 listened as `ubus`**, not `ccli`. Run **`scripts/dut-mms-tsp-preflight.ps1`** before next TSP attempt. |
| **TSP vs OpenSSL** | OpenSSL negotiated **TLS 1.3**; product claims **TLS 1.2 min** (62351-3). In TSP, keep **TLS 1.2** policy consistent with PIXIT; enable 1.3 only if TSP requires it for transport. |

## Fix order

1. **Logger PC:** `git pull` → `dut-mms-tsp-preflight.ps1` → confirm **`3782` → `ccli`** (not `ubus` / not `iec61850d`).
2. **PC1:** `git pull` → `setup-pc1-testpro-from-git.ps1` → MD5 **`client_tls.pem` = `13c231ee…`** (must match DUT).
3. **TSP:** Re-browse **all** paths under `C:\CCLI_TSP_FIELD_PACK\tls\`; MMS key = **`client_acse.key`**.
4. Retry Connect; if fail with SSL alert → paste TSP first red line + new `PC1_CONNECT_DIAG` run.

## Evidence files

- [TSP_P3_01_CONNECT_2026-10-14.txt](TSP_P3_01_CONNECT_2026-10-14.txt) — T1-01 stub, result FAIL
- [TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md](TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md) — team triage
