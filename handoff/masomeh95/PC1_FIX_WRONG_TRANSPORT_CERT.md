# FIX NOW — Transport certificate is wrong in Test Suite Pro

**DUT log (2026-10-15):** Connect fails with **Transport connection closed** because TSP sent the **MMS certificate at the Transport TLS layer**.

You must **edit TSP** — re-staging the pack alone is not enough if the UI still points at the old paths.

---

## What is WRONG (what the DUT saw)

| Layer | What TSP sent (wrong) | Serial (hex) | Subject hint |
|--------|------------------------|--------------|--------------|
| **Transport TLS cert** | **`client.pem`** | **`28:E5:3F:07:CF:E3:47:B0`** | OID **`2.5.4.106=…1.12`** (MMS / ACSE identity) |

That certificate belongs in **MMS Security only**, not in **Transport TLS**.

Typical mistake:

- Transport TLS certificate = **`client.pem`** ← **WRONG**
- Or both Transport and MMS set to the **same** `client.pem` file

---

## What is RIGHT (Oct-08 P3-07 PASS)

All files under **`D:\CCLI_TSP_FIELD_PACK\tls\`** after:

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

| TSP field | CORRECT file | How to recognize |
|-----------|--------------|------------------|
| Trust CA | **`root_CA.pem`** | Issuer `CN=CCLI_Lab_Root_CA` |
| **Transport TLS certificate** | **`client_tls.pem`** | Serial **`DC:9A:9E:26:D5:47:EF:38`** · **`CN=DSO_OPERATOR`** |
| Transport TLS private key | **`client_tsp.key`** | |
| MMS Security certificate | **`client.pem`** | Serial **`28:E5:3F:07:CF:E3:47:B0`** · OID **1.1.1.999.1.12** in subject |
| MMS Security private key | **`client_tsp.key`** | **Same key file** as Transport (Oct-08 profile) |
| CRL | **(empty / off)** | |
| Revoked | **(do not use)** | |

| Connection | Value |
|------------|--------|
| Server | **`192.168.10.1:3782`** · TLS **on** |
| IED | **`CCI016_01`** |
| CID | **`D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid`** |
| PC1 LAN1 | **`192.168.10.10/24`** |

---

## Edit checklist (do in order)

1. Run **`setup-pc1-p3-07-connect.ps1`** (above) so **`tls\`** is only **p3_07_connect** (no old **`client_acse.key`**).
2. Open **Test Suite Pro** → TLS / security settings for this IED.
3. **Browse** (do not type paths from memory):
   - Transport cert → **`D:\CCLI_TSP_FIELD_PACK\tls\client_tls.pem`**
   - Transport key → **`client_tsp.key`**
   - MMS cert → **`client.pem`**
   - MMS key → **`client_tsp.key`**
   - Trust → **`root_CA.pem`**
4. Clear CRL / revoked paths if anything is set.
5. **Save** project / connection profile in TSP.
6. On PC1, verify serials:

```powershell
cd D:\CCLI_TSP_FIELD_PACK\tls
openssl x509 -in client_tls.pem -noout -serial -subject
openssl x509 -in client.pem -noout -serial -subject
```

**Transport** must show **`DC:9A:9E…`** / **`DSO_OPERATOR`**.  
**MMS** must show **`28:E5:3F:07…`** / OID **1.12**.

7. Preflight:

```powershell
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

8. Tell Yahia **ready** → one **Connect** while he captures DUT log.

---

## Pass criteria

- TSP: **Initiate Response Success** / **Connected**
- DUT log (Yahia): **`ACSE auth ACCEPT`** and **`AARE responder auth [10]EXTERNAL`**

---

## Do NOT use for this session

- EJBCA USB zip / **`client_acse.key`** for MMS
- **`lab_tg544_eth_b.cid`** (5-LN) — use **`lab_tg544_eth_a.cid`**
- CRL / revoked TLS rows until Connect passes again

Reference: `lab/evidence/testsuite-pro/inbox/TSP_TRANSPORT_CLOSED_ROOT_CAUSE_2026-10-15.md`
