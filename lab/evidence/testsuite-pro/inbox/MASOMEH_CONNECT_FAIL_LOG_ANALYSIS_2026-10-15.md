# Connect FAIL — log analysis (2026-10-15)

**TSP:** `Initiate Response Failed: Transport connection closed` ([TSP_P3_01_CONNECT_2026-10-15.txt](TSP_P3_01_CONNECT_2026-10-15.txt))  
**PC1 TLS:** MD5 + `verify-tsp-tls-key-match.ps1` **PASS** on USB pack ([PC1_EVIDENCE_CAPTURE_2026-10-15_b.txt](PC1_EVIDENCE_CAPTURE_2026-10-15_b.txt))  
**Conclusion:** Not a wrong CA / wrong transport PEM on disk. Failure is **after transport TLS** or **wrong northbound listener / bench fight**.

---

## DUT log pattern (repeats on every attempt)

| Time / file | 3782 listener | TLS client cert | After TLS |
|-------------|---------------|-----------------|-----------|
| 143211 | **543/ccli** | DSO_OPERATOR transport OK | `client CONNECT count=1` → **no `ACSE auth ACCEPT`** → 15 s fallback |
| 143938 | **6268/ubus** (TesPro) | verify on **ccli** side too | `handshake failed -0x1` + bind-fail history |
| 144444 | ccli | — | **no TLS** in capture window |
| **Live 21:35** ([DUT_SNAPSHOT_LIVE_2026-10-15_now.txt](DUT_SNAPSHOT_LIVE_2026-10-15_now.txt)) | **543/ccli** | DSO_OPERATOR OK | `CONNECT count=1` → **`connection was closed gracefully`** (~1 s) → no ACSE |

**Missing line for PASS (Oct 8):** `mms: ACSE auth ACCEPT — mech=… role=DSO_OPERATOR`

So the client (TestPro) **opens TLS**, DUT **accepts transport cert**, then the **TCP/TLS session ends before ACSE/MMS Initiate completes**. TSP reports that as *Transport connection closed*.

---

## Bench issues seen in same logs (must stay fixed)

From POST tails in live captures:

```text
mms: FAILED to bind 192.168.10.1:3782 — disable TesPro IEC61850 LuCI
```

And **143938 PRE:** `3782` owned by **`ubus`**, not `ccli`.

**Now (21:35 UTC+8):** `3782` = **543/ccli**, `uci iec61850service.global.enabled=0` — bench OK for next try.

---

## Most likely cause (given PASS key-match on folder)

1. **TestPro UI still using wrong MMS private key** (`client_tsp.key` in MMS slot) even when files on disk are correct — re-browse **`client_acse.key`** for MMS Security.  
2. Less likely: CID/IED mismatch after TLS (would usually get further than graceful close).

---

## Masomeh — before next Connect

1. TSP → MMS Security private key = full path to **`client_acse.key`** (not `client_tsp.key`).  
2. Transport key = **`client_tsp.key`**. CRL = **`lab_crl.pem`**.  
3. Yahia runs **`capture-dut-tsp-connect-log.ps1 -DurationSec 120`** during one Connect.  
4. Pass criteria in DUT log: **`ACSE auth ACCEPT`** then AARE / Initiate success.

---

## Yahia — stabilize DUT (if 3782 ≠ ccli again)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\dut-mms-tsp-preflight.ps1
```

Verify: `netstat -tlnp | grep 3782` → **`ccli`** only.
