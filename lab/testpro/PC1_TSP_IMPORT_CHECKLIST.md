# PC1 — import CID and certificates in Test Suite Pro

**Use this before first Connect (U-T1-01 / U-P0-01).**  
Masomeh pack copy: `00i_PC1_TSP_IMPORT_CHECKLIST.md` (from `stage-tsp-field-pack.ps1`).

**Connect failing (`Initiate Response Failed`)?** Start with **`MASOMEH_CONNECT_FIX_2026-10-14.md`** (pack: `00k_…`) and **`scripts/dut-mms-tsp-preflight.ps1`** on logger PC.

---

## 0. One-time PC setup

1. **Admin:** `bench_scripts\set_pc_lan1_dso.cmd` → PC **`192.168.10.10/24`**
2. `ping 192.168.10.1` (DUT LAN1)
3. Install **OpenSSL 3.x Win64** and **Wireshark** (tshark) — see `00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`

---

## 1. Stage TLS + CID from GitHub

Yahia may refresh PEMs from **EJBCA** (`lab/EJBCA_LAB_SETUP.md` §12.4) before push — always **`git pull`** so PC1 matches DUT **`/etc/ccli/tls/`**. After a CA sync, re-run **`stage-tsp-tls.ps1`** even if `root_CA.pem` looks updated: a **stale `client_tls.pem`** causes TSP **`tlsv1 alert access denied`** ([`TSP_TLS_ACCESS_DENIED_2026-10-14.md`](../evidence/testsuite-pro/inbox/TSP_TLS_ACCESS_DENIED_2026-10-14.md)).

```powershell
cd D:\CCLI\CCLI   # or your clone path
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

Verify folder:

```text
D:\CCLI_TSP_FIELD_PACK\tls\root_CA.pem
D:\CCLI_TSP_FIELD_PACK\tls\client.pem
D:\CCLI_TSP_FIELD_PACK\tls\client_tls.pem
D:\CCLI_TSP_FIELD_PACK\tls\client_tsp.key
D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid
```

OpenSSL preflight (from `tls\`):

```powershell
cd D:\CCLI_TSP_FIELD_PACK\tls
openssl verify -CAfile root_CA.pem client.pem client_tls.pem
openssl rsa -in client_tsp.key -check -noout
```

If TSP rejects the key: `openssl rsa -in client.key -out client_tsp.key`

---

## 2. Import CID in Test Suite Pro

1. **File → Import** (or project **SCL / IED**) → select  
   **`D:\CCLI_TSP_FIELD_PACK\cid\product_tr400_eth_a_5ln.cid`** (cert model — matches DUT since **r54**)
2. Confirm **IED name** = **`CCI016_01`**
3. Set **MMS server** address to match DUT (not only SCL static IP if TSP allows override):
   - Host **`192.168.10.1`**
   - Port **`3782`**
   - **TLS enabled** (not cleartext 102)

**Compare (U-T1-02) / browse (U-T1-03):** DUT runs `product_tr400_eth_a_5ln.cfg` — browse must show exactly **5 LNs** (`LLN0, LPHD1, PdCMMXU1, WlimDWMX1, WSdDAGC1`). Gate before Compare: `powershell -File scripts\validate-5ln-cid.ps1` → **PASS**.

**31-LN lab CID** (`cid\lab_tg544_eth_a.cid`) only if Yahia reverts DUT to `lab_tr400_phase4_eth_b_ttyS1.yaml` — confirm with session header first.

Specimen pin: pack `00c_PRODUCT_SPECIMEN_5LN.md` (5-LN CID rev **1**, `configRev` **20261014**) · 31-LN history in `16_LAB_SPECIMEN_VERSION.md`.

---

## 3. TLS in IED Connection (mutual TLS)

Use paths under `D:\CCLI_TSP_FIELD_PACK\tls\` (or `C:\CCLI_product_tls\` if you ran `stage-tsp-tls.ps1`).

| TSP field | File |
|-----------|------|
| Trusted CA / root | `root_CA.pem` |
| Transport TLS certificate | `client_tls.pem` |
| Transport TLS private key | **`client_tsp.key`** (Cert A) |
| MMS Security / ACSE certificate | `client.pem` |
| MMS Security private key | **`client_acse.key`** (Cert B — dual key; verify with `scripts/verify-tsp-tls-key-match.ps1`) |

Optional copies for negative tests:

| Test | Transport TLS | MMS Security | Private key | CRL |
|------|---------------|--------------|-------------|-----|
| T3-02 viewer read | — | `viewer.pem` | `viewer.key` | — |
| **T3-04 revoked** | **`revoked_tls.pem`** | **`revoked.pem`** | **`revoked_tsp.key`** (both slots) | **`lab_crl.pem`** |

After stage, confirm pack contains:

```text
tls\revoked.pem
tls\revoked_tls.pem
tls\revoked.key
tls\revoked_tsp.key
tls\lab_crl.pem
```

OpenSSL (optional): `openssl verify -CAfile root_CA.pem -crl_check -CRLfile lab_crl.pem revoked.pem` → must **fail** (revoked).

Connection summary:

| Parameter | Value |
|-----------|--------|
| Role | Client (DSO) |
| Server | `192.168.10.1:3782` |
| Security | TLS + MMS Security (62351-4 mutual auth) |
| AP-title / AE-qualifier | Match SCL / DUT (lab: AP `1.1.1.999.1`, AE **12** — confirm in TSP after Connect trace) |

Stay **Disconnected** until LAN1 + TLS paths are set; then **Connect** for **U-T1-01**.

---

## 4. Evidence logger (same folder)

```powershell
cd D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-14 -Operator Masomeh -CcliPkg 0.1.0-r53
```

See `00g_TSP_5LN_EXECUTION_AND_LOGS.md` for `-TestId` per matrix row.

---

**Keywords:** TestPro, PC1, TLS, CID, lab_tg544_eth_a, 3782
