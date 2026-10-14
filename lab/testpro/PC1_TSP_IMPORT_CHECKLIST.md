# PC1 — import CID and certificates in Test Suite Pro

**Use this before first Connect (U-T1-01 / U-P0-01).**  
Masomeh pack copy: `00i_PC1_TSP_IMPORT_CHECKLIST.md` (from `stage-tsp-field-pack.ps1`).

---

## 0. One-time PC setup

1. **Admin:** `bench_scripts\set_pc_lan1_dso.cmd` → PC **`192.168.10.10/24`**
2. `ping 192.168.10.1` (DUT LAN1)
3. Install **OpenSSL 3.x Win64** and **Wireshark** (tshark) — see `00h_PC1_LOG_CAPTURE_REQUIREMENTS.md`

---

## 1. Stage TLS + CID from GitHub

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
   `D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid`
2. Confirm **IED name** = **`CCI016_01`**
3. Set **MMS server** address to match DUT (not only SCL static IP if TSP allows override):
   - Host **`192.168.10.1`**
   - Port **`3782`**
   - **TLS enabled** (not cleartext 102)

**5-LN certificate Compare (U-T1-02):** when `cid\product_tr400_eth_a_5ln.cid` exists in the pack, run `scripts\validate-5ln-cid.ps1` then switch Compare model to that CID. Until then, **do not sign off U-T1-02 / U-T1-03 (5 LNs)** — Connect and cyber rows still run on **lab** CID.

Specimen pin: `16_LAB_SPECIMEN_VERSION.md` in pack (CID History rev **9**, `configRev` **20261009**).

---

## 3. TLS in IED Connection (mutual TLS)

Use paths under `D:\CCLI_TSP_FIELD_PACK\tls\` (or `C:\CCLI_product_tls\` if you ran `stage-tsp-tls.ps1`).

| TSP field | File |
|-----------|------|
| Trusted CA / root | `root_CA.pem` |
| Transport TLS certificate | `client_tls.pem` |
| Transport TLS private key | **`client_tsp.key`** |
| MMS Security / ACSE certificate | `client.pem` |
| MMS Security private key | **`client_tsp.key`** (same key, both slots) |

Optional copies for negative tests:

| Test | Cert | Key |
|------|------|-----|
| T3-02 viewer read | `viewer.pem` | `viewer.key` |
| T3-04 CRL / revoked | `revoked.pem` | `revoked.key` + enable CRL `lab_crl.pem` per TSP UI |

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
