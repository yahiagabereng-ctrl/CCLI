# PC1 — Encrypted Alert / Initiate failed (P3-07 Connect)

Wireshark **Encrypted Alert** happens in **two different** failures. Read DUT log **during the same click**:

| DUT log during Connect | Meaning | Fix |
|------------------------|---------|-----|
| **`client CONNECT`** only — **no** `ACSE auth ACCEPT` | **Type A** — TLS OK, **no AARQ / MMS Security** (today’s bench) | **§1–§3 below** (TSP MMS key + lab CID) |
| **`ACSE auth ACCEPT`** + **`AARE responder auth [10]EXTERNAL`** then alert ~2 ms later | **Type B** — Sept/Oct **AARE signature** rejection | Firmware **r51+ AARE** path + wire capture; see `lab/evidence/TLS_WIRESHARK_HANDOFF_ALIREZA.md` |

**Current DUT (2026-10-16): Type A** — Transport **`DC:9A:9E…` / DSO_OPERATOR** OK, then  
`mms: client CONNECT count=1` **without** `mms: ACSE auth ACCEPT`.

TLS is **OK**. TSP aborts before the server sees MMS association (Encrypted Alert from **client**).

---

## Fix on Test Suite Pro (not another DUT deploy)

### 1. CID — must be **31-LN lab**, not 5-LN product

| Use | Do not use (unified cert session) |
|-----|-----------------------------------|
| **`D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid`** | `product_tr400_eth_a_5ln.cid` |

Re-import CID if you previously imported the 5-LN file (`00i_PC1_TSP_IMPORT_CHECKLIST.md` is for **EJBCA/5-LN**, not this Connect profile).

### 2. MMS private key — **`client_tsp.key`**, not `client_acse.key`

P3-07 pack has **no** `client_acse.key`. Both Transport and MMS use **`client_tsp.key`**.

| TSP slot | File |
|----------|------|
| Transport cert | **`client_tls.pem`** |
| Transport key | **`client_tsp.key`** |
| MMS cert | **`client.pem`** |
| MMS key | **`client_tsp.key`** ← **same file** |
| CRL / revoked | **empty** |

Wrong MMS key (old **`client_acse.key`** path from EJBCA docs) → **Encrypted Alert** after TLS.

```powershell
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

### 3. Stack tabs (Oct-08 PASS)

- **AP Title:** `1,1,1,999,1` on all relevant tabs  
- **Chaining depth:** `1`  
- **Sisco:** OFF  
- **Allow TLS 1.3:** OFF (if shown)  
- **IED:** `CCI016_01` · **`192.168.10.1:3782`** · TLS ON · local IP **`192.168.10.10`**

Save the connection profile after re-browse.

### 4. Verify on PC1 before Connect

```powershell
cd D:\CCLI_TSP_FIELD_PACK\tls
openssl x509 -in client_tls.pem -noout -serial -subject
openssl x509 -in client.pem -noout -serial -subject
# Transport serial must be DC9A9E... DSO_OPERATOR
# MMS serial must be 28E53F07... OID 1.12
```

---

## Pass criteria (DUT — Yahia can capture)

```text
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR …
mms: AARE responder auth [10]EXTERNAL …
```

TSP: **Initiate Response Success**.

See also: **`00t_PC1_FIX_WRONG_TRANSPORT_CERT.md`**, **`00r_PC1_P3_07_CONNECT_PROFILE_ACTIVE.md`**.
