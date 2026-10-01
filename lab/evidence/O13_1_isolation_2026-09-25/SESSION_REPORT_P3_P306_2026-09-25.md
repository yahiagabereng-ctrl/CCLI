# Session report — Phase 3 P3-06 (MMS TLS / 62351-3)

**Date:** 2026-09-25  
**DUT:** TesPro TG544 / TR500 · Eth_A `192.168.10.1:3782`  
**Package:** `ccli-0.1.0-r7`  
**Clause:** Annex **T · T.3.3 / T.3.3.4** · **IEC 62351-3**

## Goal

No cleartext MMS on Eth_A — TLS 1.2+ T-profile (lab port **3782**).

## Results

| Check | Result |
|-------|--------|
| mbedtls 3.6.0 vendored | OK |
| Server `tls=on` listen | `192.168.10.1:3782` |
| TLS client browse | CONNECT OK · Mod.stVal · TotW |
| Cleartext `:102` | CONNECT FAIL |
| **P3-06** | **PASS** |

## Evidence

- `P3_06_MMS_TLS_ETH_A.txt`
- `P3_06_TLS_CLIENT.txt`
- `P3_06_CLEARTEXT_FAIL.txt`

## Phase 3 exit

| Gate | Status |
|------|--------|
| P3-01 | PASS |
| P3-04 | PASS |
| **P3-06** | **PASS** |

**Phase 3 exit criteria met** (soft: P3-07 62351-4, P3-08/09 RBAC/PKI, TPM-backed keys).

## Next

Phase 4 — IEC 60870-5-104 on Eth_B, or harden P3-07/P3-09.
