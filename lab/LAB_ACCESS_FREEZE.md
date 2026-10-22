# Lab access freeze — logger PC and Test Suite Pro PC

**Document ID:** CCLI-LAB-ACCESS-FREEZE-001  
**Revision:** 1.0 · **2026-10-15**  
**RAG source_id:** `ccli-lab-access-freeze`  
**Pairs with:** [DEPLOY_REVISION_RECAP.md](tg544-openwrt/DEPLOY_REVISION_RECAP.md) · [revision-pins/](tg544-openwrt/revision-pins/)

Use **one frozen access profile** on **both** the deploy/logger PC and the **Test Suite Pro (PC-B)** PC. Do not invent per-machine SSH passwords or ad-hoc TSP TLS folders.

---

## Requirements

| ID | Requirement | Pass criteria |
|----|-------------|---------------|
| REQ-ACC-001 | Same DUT SSH secret on all bench PCs | `$env:CCLI_TG544_PW` from shared `lab-env` (see below) |
| REQ-ACC-002 | SSH user + host key unchanged | `root` · PuTTY host key **`SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE`** |
| REQ-ACC-003 | TSP PC uses frozen TLS pack | `D:\CCLI_TSP_FIELD_PACK\tls\` or **CCLI_TSP_CONNECT_MIN** (p3_07_connect bytes) |
| REQ-ACC-004 | Logger stages TSP TLS after LAN1 deploy | `C:\CCLI_tls` + `C:\CCLI_product_tls` (from deploy script) match DUT |
| REQ-ACC-005 | PC roles / IPs | PC-B **`192.168.10.10`** · logger LAN1 **`192.168.10.20`** · DUT **`192.168.10.1`** |

---

## 1. DUT SSH (both PCs)

| Field | Frozen value |
|-------|----------------|
| User | **`root`** |
| Password | **`000000`** (TG544 lab default — set in [`lab-env.example.ps1`](tg544-openwrt/lab-env.example.ps1)) |
| LAN1 | **`192.168.10.1`** (MMS deploy / TSP) |
| LAN2 | **`192.168.1.130`** (104 / plant SSH) |
| Env var | **`$env:CCLI_TG544_PW`** |

**On every PC before plink/pscp or evidence scripts:**

```powershell
powershell -File scripts\source-lab-env.ps1
# or, with repo clone:
. .\lab\tg544-openwrt\lab-env.ps1
```

**Setup (once per machine):**

```powershell
powershell -File scripts\ensure-lab-env.ps1
```

Copy **`lab\tg544-openwrt\lab-env.ps1`** from the logger to the TSP PC if you already customized it — it must **match** the example freeze unless the DUT password was changed in hardware.

**Security note:** Bench-only. Product/site controllers must not use this password.

**SSH keys (recommended):** Set `$env:CCLI_SSH_KEY` / `$env:CCLI_SSH_PUBKEY` in `lab-env.ps1`. Deploy scripts can use `scripts/Invoke-CcliPlink.ps1`. Harden DUT: [CCLI_DUT_ADMIN_HARDENING.md](CCLI_DUT_ADMIN_HARDENING.md) · `scripts/harden-dut-admin.ps1`.

---

## 2. Test Suite Pro “login” (certificates, not a password)

MMS/TLS auth is **certificate-based**. Frozen **P3-07 Connect** profile (matches DUT **p3_07_connect**, **crl=off**):

| TSP slot | File (under field pack `tls\`) |
|----------|-------------------------------|
| Trust | `root_CA.pem` |
| Transport TLS certificate | **`client_tls.pem`** (serial **`DC:9A:9E:26:D5:47:EF:38`**, CN=**DSO_OPERATOR**) |
| Transport TLS key | **`client_tsp.key`** |
| MMS Security certificate | **`client.pem`** |
| MMS Security key | **`client_tsp.key`** (same file in **both** key slots) |
| CRL / revoked | **Empty / off** |

**MD5 pin** (must match DUT `/etc/ccli/tls/` for this session):

| File | MD5 (live DUT 2026-10-16 Connect PASS) |
|------|-----|
| `root_CA.pem` | `fa23eb4d198c74059cdf72d585eb50c6` |
| `client_tls.pem` | `adda810ae47ced71f4880a18f93b7ddf` |
| `client.pem` | `0b540059962d61cd897fe393074d6aac` |
| `client_tsp.key` | `04a791a3e34ecf68521cfcb582867865` |

Repo `apps/ccli/config/tls/p3_07_connect` may show different **PEM** MD5s (line wrap) but **DER + serials** match this table. Serials: CA `4DAA40028901CFFB` · transport `DC9A9E26D547EF38` · MMS `28E53F07CFE347B0`.

**Connection:** IED **CCI016_01** · **`192.168.10.1:3782`** · TLS on · CID **`lab_tg544_eth_a.cid`**.

**Stage pack (either PC after `git pull`):**

```powershell
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1 -HostAddr 192.168.10.1
```

Minimal zip for handoff: [`handoff/masomeh95/CCLI_TSP_CONNECT_MIN/`](../handoff/masomeh95/CCLI_TSP_CONNECT_MIN/).

---

## 3. Frozen paths (both PCs)

| Purpose | Path |
|---------|------|
| TSP field pack root | **`D:\CCLI_TSP_FIELD_PACK`** (or set **`$env:CCLI_TSP_FIELD_PACK`**) |
| Repo env (logger) | **`<repo>\lab\tg544-openwrt\lab-env.ps1`** (gitignored; clone from example) |
| Env on TSP PC (no clone) | **`<pack>\bench_scripts\lab-env.ps1`** (staged from freeze) |
| PC-B LAN1 script | **`lab\set_pc_lan1_dso.cmd`** → **`192.168.10.10/24`** |
| Logger LAN1 script | **`lab\set_pc_lan1_logger.cmd`** → **`192.168.10.20/24`** |

---

## 4. Verification

```powershell
powershell -File scripts\source-lab-env.ps1
powershell -File scripts\verify-lab-access-freeze.ps1
plink -batch -pw $env:CCLI_TG544_PW -hostkey SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE root@192.168.10.1 "ccli --version | head -1"
```

---

## 5. Unified 5-LN cert sessions (different TLS tree)

When running **UNIFIED-5LN-CERT** (EJBCA, **crl=on**), use **`apps/ccli/config/tls/`** and matrix pin docs — not the p3_07 MD5 table above. SSH freeze (**REQ-ACC-001**) still applies.
