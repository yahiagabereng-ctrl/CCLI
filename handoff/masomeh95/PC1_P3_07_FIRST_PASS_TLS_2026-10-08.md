# PC1 — Test Suite Pro TLS (first documented PASS, 2026-10-08)

**For:** Masomeh · **PC-B** · LAN1 Test Suite Pro  
**Evidence:** [`lab/evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md`](../../lab/evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md)  
**Team summary:** [`lab/TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md`](../../lab/TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md)

This is the **first lab PASS** of mutual TLS + MMS Security (62351-4 A-profile) on **`192.168.10.1:3782`**. Use this **TSP wiring** whenever Yahia confirms the DUT still runs the **same PKI** as `apps/ccli/config/tls/` for that session (lab CA or matching EJBCA re-issue with **same slot names**).

If Yahia says **EJBCA 2026-10-14** redeploy only, read **`00k_PC1_EJBCA_TLS_TESTPRO.md`** instead — **file names are identical**; only PEM **bytes** must match DUT `/etc/ccli/tls/`.

---

## Step 1 — Get PEMs onto PC1

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

Or stage TLS only:

```powershell
powershell -File scripts\stage-tsp-tls.ps1 -ProductDest D:\CCLI_TSP_FIELD_PACK\tls
```

**Repo (all certs committed):** `apps/ccli/config/tls/` and `apps/ccli/config/tls/ejbca/` — see `lab/testpro/TSP_TLS_CERTIFICATE_PATHS.md` and `apps/ccli/config/tls/TESTPRO_TLS_FILE_MANIFEST.json`.

Required files under **`D:\CCLI_TSP_FIELD_PACK\tls\`** (or `C:\CCLI_product_tls\`):

```text
root_CA.pem
client.pem
client_tls.pem
client_tsp.key
```

Optional (Tier 3 later): `viewer.pem`, `viewer.key`, `revoked.pem`, `revoked_tls.pem`, `revoked_tsp.key`, `lab_crl.pem`

---

## Step 2 — OpenSSL preflight

```powershell
cd D:\CCLI_TSP_FIELD_PACK\tls
openssl verify -CAfile root_CA.pem client.pem client_tls.pem
openssl rsa -in client_tsp.key -check -noout
```

If TSP rejects the key: `openssl rsa -in client.key -out client_tsp.key`

---

## Step 3 — Test Suite Pro (exact PASS profile, 2026-10-08)

| TSP field | File |
|-----------|------|
| Trusted CA / root | **`root_CA.pem`** |
| **Transport TLS** certificate | **`client_tls.pem`** |
| **Transport TLS** private key | **`client_tsp.key`** |
| **MMS Security / ACSE** certificate | **`client.pem`** |
| **MMS Security** private key | **`client_tsp.key`** (same key, **both** slots) |

| Connection | Value |
|------------|--------|
| IED name | **`CCI016_01`** |
| Server | **`192.168.10.1:3782`** |
| TLS | **On** (not cleartext `:102`) |
| Local IP (LAN1) | **`192.168.10.10`** — run `bench_scripts\set_pc_lan1_dso.cmd` |
| TLS v1.3 | **OFF** (lab) |
| Sisco compatibility | **OFF** |
| GOOSE / SV | **OFF** |
| Connect from | **System Status** |

**CID at pass time:** `lab_tg544_eth_a.cid` / DUT model `lab_tg544_eth_a.cfg` (31-LN lab). Unified **5-LN** cert runs use `cid\product_tr400_eth_a_5ln.cid` when Yahia pins r54+ — **TLS paths unchanged**.

CRL warnings in TSP (“No valid Certificate Revocation files…”) were **OK** when DUT had **`crl=off`**.

---

## Step 4 — Pass criteria (same as evidence file)

1. TSP **Connected** (green).
2. Output: **`Initiate Response Success`**.
3. **ReadResponse** traffic (browse/read works).
4. No **Unable to verify signature**, no association timeout on this profile.

DUT log (successful path) should include lines like:

```text
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=… octets
mms: AARE responder auth [10]EXTERNAL ctx=5 inner=… octets
```

---

## DUT reference at first PASS

| Item | Value |
|------|--------|
| Package | **`ccli-0.1.0-r51.apk`** |
| Build tag | **`P3-07-AARE-SIGN-RAW-GT`** |
| Config | `/etc/ccli/lab.yaml` |
| Model | `/etc/ccli/icd/lab_tg544_eth_a.cfg` |
| TLS on DUT | `server_tls.pem` + `server.key` (transport); `server.pem` + `server_acse.key` (AARE G.2) |
| Operational | **One** `ccli` on **3782** — stop duplicate `iec61850service` / extra `ccli` if Yahia says so |

Ask Yahia for **current** `-CcliPkg` in evidence logger (may be r53/r54/r55); **TLS file wiring above stays the same**.

---

## Evidence logger (after Connect works)

```powershell
cd D:\CCLI_TSP_FIELD_PACK
$dt = Get-Date -Format yyyy-MM-dd
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate $dt -Operator Masomeh -CcliPkg "<Yahia pin>" -TspVersion "<TSP build>"
```

First row: **U-T1-01** / `-TestId T1-01` — see **`00g_TSP_5LN_EXECUTION_AND_LOGS.md`**.

---

**Keywords:** Masomeh, PC1, P3-07, 2026-10-08, TLS, client_tsp.key, first pass
