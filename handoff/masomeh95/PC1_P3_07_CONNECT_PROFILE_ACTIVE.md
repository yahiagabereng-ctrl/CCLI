# ACTIVE — P3-07 Connect profile (Oct 2026-10-08 PASS wiring)

**Status:** Use this **instead of** EJBCA dual-key (`00k`) until Connect **PASS** again.  
**DUT:** `ccli 0.1.0-r55` (AARE raw-GT stack from r51 retained) · **`crl=off`** · **no revoke/CRL**  
**Model:** **`lab_tg544_eth_a.cfg`** (31-LN lab — same as first PASS)  
**PKI folder:** `apps/ccli/config/tls/p3_07_connect/` (single **`client_tsp.key`** for Transport **and** MMS)

---

## PC1 — one command

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\publish-p3-07-connect-profile.ps1   # if tls\p3_07_connect missing on PC
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

---

## Test Suite Pro (exact 2026-10-08)

| TSP field | File |
|-----------|------|
| Trust CA | **`root_CA.pem`** |
| Transport TLS cert | **`client_tls.pem`** |
| Transport TLS key | **`client_tsp.key`** |
| MMS Security cert | **`client.pem`** |
| MMS Security key | **`client_tsp.key`** ← **same file as Transport** |
| CRL | **(leave empty / disabled)** |
| Revoked | **(do not use)** |

| Connection | Value |
|------------|--------|
| IED | **`CCI016_01`** |
| Server | **`192.168.10.1:3782`** · TLS **on** |
| CID | **`cid\lab_tg544_eth_a.cid`** |
| LAN1 | **`192.168.10.10/24`** · `bench_scripts\set_pc_lan1_dso.cmd` |

**Re-browse every TLS path** after `git pull`. Do **not** use `Downloads\CCLI_TSP_TLS_COMPLETE` EJBCA zip for this session.

---

## Preflight

```powershell
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
openssl verify -CAfile D:\CCLI_TSP_FIELD_PACK\tls\root_CA.pem D:\CCLI_TSP_FIELD_PACK\tls\client.pem D:\CCLI_TSP_FIELD_PACK\tls\client_tls.pem
```

---

## Pass criteria (DUT log — Yahia capture)

```text
mms: listening on 192.168.10.1:3782 tls=on ... crl=off ...
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=… octets
mms: AARE responder auth [10]EXTERNAL ctx=5 inner=… octets
```

TSP: **Initiate Response Success** · **Connected**.

Evidence: `-CcliPkg 0.1.0-r55` · note **P3-07 connect profile** in session header.

---

## After Connect works

Yahia will re-enable EJBCA + CRL + 5-LN + cyber revoked rows (`00k` / unified matrix).
