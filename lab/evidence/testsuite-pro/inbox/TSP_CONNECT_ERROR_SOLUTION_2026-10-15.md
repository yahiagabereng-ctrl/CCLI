# Connect error + solution (after `43e8e85` / Transport fix)

**Date:** 2026-10-15  
**Evidence:** `TSP_CONNECT_CAPTURE_2026-10-15_161653.md` · pcap `…161653.pcapng` · DUT log `…161653.txt`  
**TSP symptom:** `Initiate Response Failed: Transport connection closed`

---

## 1. Error (what failed)

| Layer | Result | Evidence |
|-------|--------|----------|
| TCP `192.168.10.10` → `192.168.10.1:3782` | PASS | pcap / Test-NetConnection |
| Mutual TLS 1.2 | PASS | Cipher **`0x003c`** = `TLS_RSA_WITH_AES_128_CBC_SHA256` |
| Transport client cert | PASS | Serial **`DC:9A:9E:26:D5:47:EF:38`** · `CN=DSO_OPERATOR` (`client_tls.pem`) |
| DUT `client CONNECT` | PASS | log |
| **ACSE / AARE** | **FAIL** | **No** `ACSE auth ACCEPT` · **no** server AppData AARE |
| Teardown | DUT TLS **Alert** then close | pcap frames 30 / 65 / 100 |

### Wire pattern (every retry)

```text
TLS OK
→ client AppData 80 + server 80
→ client AppData 1088 + 352   (AARQ / Initiate scale)
→ DUT TLS Alert (content_type 21)   ← NO server 1088+304 AARE
→ FIN
```

Oct-08 **PASS** needed DUT to continue with **AARE** and log:

```text
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR …
mms: AARE responder auth [10]EXTERNAL ctx=5 …
```

Those lines are **absent** on this capture. So the remaining error is **MMS Security / association after TLS**, not Transport PEMs anymore.

---

## 2. Already fixed (do not re-debug)

| Issue | Status |
|-------|--------|
| Transport slot = `client.pem` (wrong) | **FIXED** in `43e8e85` — wire shows `client_tls.pem` |
| Stale USB MD5 vs DUT | **OK** on Transport serial |
| Dual `ccli` / `iec61850d` bind fight | Must stay clean via `dut-mms-tsp-preflight.ps1` |

---

## 3. Solution (use Oct-08 ACTIVE profile only)

**Canonical guide:** `handoff/masomeh95/PC1_P3_07_CONNECT_PROFILE_ACTIVE.md`  
**DUT deploy:** `CCLI_DEPLOY_0.1.0-r55_2026-10-15_154538` → yaml `lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` · model **`lab_tg544_eth_a.cfg`** · PKI **`p3_07_connect/`** · **`crl=off`**

### Test Suite Pro — exact wiring

| Field | Must be |
|-------|---------|
| Trust CA | `client_tls` pack → **`root_CA.pem`** |
| Transport cert | **`client_tls.pem`** (serial `DC:9A:9E…`) |
| Transport key | **`client_tsp.key`** |
| MMS cert | **`client.pem`** (serial `28:E5:3F:07…`) |
| MMS key | **`client_tsp.key`** (same file — **not** `client_acse.key`) |
| CRL / revoked | **empty / off** |
| Server | **`192.168.10.1:3782`** TLS on |
| Local IP | **`192.168.10.10`** |
| CID | **`lab_tg544_eth_a.cid`** (31-LN — **not** `product_tr400_eth_a_5ln.cid`) |
| AP Title (all stack tabs) | **`1,1,1,999,1`** · AE **`12`** |
| Sisco | **OFF** |
| TLS v1.3 | **OFF** |

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
# Re-browse ALL six TLS paths in TSP (do not keep old EJBCA / 5-LN paths)
```

### Yahia DUT (before Connect)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\dut-mms-tsp-preflight.ps1
# Expect: one ccli · 3782 → ccli · no iec61850d · listening … crl=off … lab_tg544_eth_a
```

### Pass criteria

- TSP: **Initiate Response Success** / **Connected**
- DUT: **`ACSE auth ACCEPT`** + **`AARE responder auth [10]EXTERNAL`**
- Wire: after client 1088+352, **server** AppData ~1088+304 (AARE), not Alert

---

## 4. Do **not** use for this Connect recovery

Conflicting handoff `MASOMEH_TLS_HANDOFF_2026-10-15.md` / EJBCA USB path:

- MMS key **`client_acse.key`**
- CRL **`lab_crl.pem`**
- CID **`product_tr400_eth_a_5ln.cid`**

That is the **unified 5-LN / EJBCA** track. DUT is currently on **P3-07 Oct-08 connect profile** (31-LN, single `client_tsp.key`, crl=off). Mixing the two causes association abort after TLS.

---

## 5. If still FAIL after ACTIVE profile

1. Capture one Connect: DUT log + pcap.  
2. Confirm wire still Alert with **no** AARE → Yahia debug ISO session / AARQ parse (expect `iso session CONNECT` then ACCEPT or REJECT).  
3. Confirm no `SIGSEGV` / second `ccli` (seen in earlier `after_fix` note).

**Bottom line:** Transport error is solved. Remaining error = association aborted after client Initiate (no AARE). Solution = **strict Oct-08 ACTIVE TSP+CID+key profile** + clean single `ccli` on 3782.
