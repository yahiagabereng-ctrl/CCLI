# PC1 — pull frozen revision from GitHub (Test Suite Pro)

**When:** PC-B is on LAN1, DUT is deployed, ready for **Connect**.  
**Revision pin:** **`P3-07-TSP-CONNECT`** · firmware **`0.1.0-r55`** · TLS **`p3_07_connect`** · **crl=off**  
**Access freeze:** [`lab/LAB_ACCESS_FREEZE.md`](../../lab/LAB_ACCESS_FREEZE.md) (same SSH + TLS MD5 on logger and PC1)

---

## 1. Clone or pull (every session)

```powershell
cd D:\CCLI\CCLI\CCLI
# first time: git clone https://github.com/yahiagabereng-ctrl/CCLI.git D:\CCLI\CCLI\CCLI

git fetch origin
git pull origin main
git log -1 --oneline
```

---

## 2. Frozen credentials (same as logger PC)

```powershell
cd D:\CCLI\CCLI\CCLI
powershell -File scripts\ensure-lab-env.ps1
powershell -File scripts\source-lab-env.ps1
powershell -File scripts\verify-lab-access-freeze.ps1
```

If you use **field pack only** (no clone path):

```powershell
. D:\CCLI_TSP_FIELD_PACK\bench_scripts\lab-env.ps1
```

DUT SSH: **`root`** · password in **`lab-env.example.ps1`** freeze · host **`192.168.10.1`**.

---

## 3. Stage Test Suite Pro pack (TLS + CID + docs + scripts)

**P3-07 Connect (Oct-08 profile — use this for Connect now):**

```powershell
cd D:\CCLI\CCLI\CCLI
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
powershell -File scripts\verify-lab-access-freeze.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

Read **in order:**

1. `D:\CCLI_TSP_FIELD_PACK\00t_PC1_FIX_WRONG_TRANSPORT_CERT.md`
2. `D:\CCLI_TSP_FIELD_PACK\00r_PC1_P3_07_CONNECT_PROFILE_ACTIVE.md`
3. `D:\CCLI_TSP_FIELD_PACK\00q_LAB_ACCESS_FREEZE.md`

Repo copy of minimal TLS zip path: `handoff/masomeh95/CCLI_TSP_CONNECT_MIN/` (same PEMs as step 3).

---

## 4. Network (PC-B only — admin CMD)

```powershell
D:\CCLI_TSP_FIELD_PACK\bench_scripts\set_pc_lan1_dso.cmd
ping 192.168.10.1
```

PC1 must be **`192.168.10.10/24`**. Do **not** use `.10.20` (logger).

---

## 5. Test Suite Pro wiring (frozen)

| TSP slot | File under `D:\CCLI_TSP_FIELD_PACK\tls\` |
|----------|------------------------------------------|
| Trust | `root_CA.pem` |
| Transport TLS cert | **`client_tls.pem`** (CN=DSO_OPERATOR) |
| Transport TLS key | **`client_tsp.key`** |
| MMS cert | **`client.pem`** |
| MMS key | **`client_tsp.key`** (both key slots) |
| CRL / revoked | **off / empty** |

- **IED:** CCI016_01 · **192.168.10.1:3782** · TLS on  
- **CID:** `D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid`  
- Re-browse **every** path after `git pull` (do not keep old TSP browse paths).

---

## 6. Optional — confirm DUT matches PC1

From PC1 (with `CCLI_TG544_PW` set):

```powershell
powershell -File scripts\dut-mms-tsp-preflight.ps1 -HostAddr 192.168.10.1
powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1 -HostAddr 192.168.10.1
```

Pass log on DUT: `iso session CONNECT` → `ACSE auth ACCEPT` → `AARE responder auth [10]EXTERNAL`.

---

## 7. Revision reference (logger deploy record)

| Item | Location |
|------|----------|
| Pin JSON | `lab/tg544-openwrt/revision-pins/P3-07-TSP-CONNECT.json` |
| Recap | `lab/tg544-openwrt/DEPLOY_REVISION_RECAP.md` |
| Manifest (example) | `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r55_2026-10-15_181640.json` |
| Binary SHA (r55 on DUT) | `e192d3cce1a0bdff54bb712c6cefdbbd0ff7a2f5057f74c0bc0b4dac5a950b31` |

---

## 8. Evidence (after Connect)

```powershell
cd D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-16 -Operator Masomeh -CcliPkg 0.1.0-r55
```

See `00h_PC1_LOG_CAPTURE_REQUIREMENTS.md` in the staged pack.

**RAG:** `ccli-lab-access-freeze` · `ccli-tsp-two-pc-bench-runbook`
