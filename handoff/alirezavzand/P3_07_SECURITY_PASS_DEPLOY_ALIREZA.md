# P3-07 SECURITY PASS — repeatable deploy (DUT + Test Suite Pro)

**Audience:** Alireza (@alirezavzand) · logger / DUT deploy · Masomeh / PC-B TestPro  
**Verified:** 2026-10-16 — **TSP Connected** after **r51 APK** + **p3_07_connect** PKI  
**Git pin:** `6c70f8a`+ · release tag **`p3-07-security-pass`**  
**Repo:** https://github.com/yahiagabereng-ctrl/CCLI

This profile is **not** the unified EJBCA / 5-LN matrix. Do **not** use `client_acse.key` or `product_tr400_eth_a_5ln.cid` here.

---

## 1. Download (once per bench PC)

### GitHub Release (binaries + zip pack)

| Asset | URL |
|-------|-----|
| **r51 APK (PASS binary)** | https://github.com/yahiagabereng-ctrl/CCLI/releases/download/p3-07-security-pass/ccli-0.1.0-r51.apk |
| **Full pack (zip)** | https://github.com/yahiagabereng-ctrl/CCLI/releases/download/p3-07-security-pass/P3-07-SECURITY-PASS.zip |

**APK SHA-256 (must match):**  
`0a2d2782d76e60dcbbafb851416ec24a0aa707b592c51cfa4545c08f1504419c`  
**Size:** 997 024 bytes

```powershell
git clone https://github.com/yahiagabereng-ctrl/CCLI.git
cd CCLI
git pull origin main

# Optional: save APK beside repo
curl.exe -L -o lab\tg544-openwrt\ccli-0.1.0-r51.apk `
  "https://github.com/yahiagabereng-ctrl/CCLI/releases/download/p3-07-security-pass/ccli-0.1.0-r51.apk"
certutil -hashfile lab\tg544-openwrt\ccli-0.1.0-r51.apk SHA256
```

In-repo copy (same bytes as release): `lab/releases/P3-07-SECURITY-PASS/` · see `MANIFEST.json`.

---

## 2. Certificate pins (DUT and TSP must match)

| File | MD5 | Serial (hex) | TSP use |
|------|-----|--------------|---------|
| `root_CA.pem` | `fa23eb4d198c74059cdf72d585eb50c6` | `4DAA40028901CFFB` | Trust CA (Transport + MMS) |
| `client_tls.pem` | `adda810ae47ced71f4880a18f93b7ddf` | **`DC9A9E26D547EF38`** | **Transport** public cert only |
| `client.pem` | `0b540059962d61cd897fe393074d6aac` | **`28E53F07CFE347B0`** | **MMS / ACSE** public cert only |
| `client_tsp.key` | `04a791a3e34ecf68521cfcb582867865` | — | **Both** Transport and MMS private key |
| `lab_crl.pem` | (see repo `apps/ccli/config/tls/lab_crl.pem`) | — | TSP CRL **both tabs** if UI requires a file; DUT yaml has **`crl=off`** |

Source tree on disk: `apps/ccli/config/tls/p3_07_connect/`

---

## 3. PC staging for Test Suite Pro (no spaces in paths)

Triangle MicroWorks often **fails** on paths with spaces (`Firmware Engineer`) or missing CA/key files.

**After every `setup-pc1-p3-07-connect.ps1` or full preflight**, refresh short paths:

```powershell
Remove-Item C:\CCLI_tls\* -Force -ErrorAction SilentlyContinue
Copy-Item -Force apps\ccli\config\tls\p3_07_connect\* C:\CCLI_tls\
Copy-Item -Force apps\ccli\config\tls\lab_crl.pem C:\CCLI_tls\

powershell -File scripts\verify-tsp-tls-key-match.ps1 -Profile P3-07 -TlsDir C:\CCLI_tls
# Expect: PASS Transport + PASS MMS (ignore revoked rows)
```

### TSP browse paths (copy exactly)

```
C:\CCLI_tls\root_CA.pem
C:\CCLI_tls\client_tls.pem
C:\CCLI_tls\client.pem
C:\CCLI_tls\client_tsp.key
C:\CCLI_tls\lab_crl.pem
```

Field pack CID (or zip):  
`CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid`  
after `powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest C:\...\CCLI_TSP_FIELD_PACK`

---

## 4. DUT deploy (TG544 @ 192.168.10.1 LAN1)

**Password:** `lab-env.ps1` → `CCLI_TG544_PW` (see `lab/tg544-openwrt/lab-env.example.ps1`).

### Order (important)

1. Push **yaml + model + p3_07_connect TLS** (config only).
2. Install **r51 APK** (overwrites `/usr/sbin/ccli` with PASS binary).
3. **Do not** run `deploy-ccli-session-fix.ps1` with `lab/tg544-openwrt/ccli-bin` **after** APK — that drops r55/r56 over r51.

### 4a. Config deploy (from logger PC on 192.168.10.x)

```powershell
cd CCLI
. .\lab\tg544-openwrt\lab-env.ps1

powershell -ExecutionPolicy Bypass -File lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -DeployProfile P3-07-SECURITY-PASS `
  -SkipVersionCheck `
  -Changes @('p3_07_connect yaml+tls for security PASS')
```

### 4b. r51 APK (DUT with wget, or upload APK)

```bash
# On DUT (SSH root@192.168.10.1)
wget -O /tmp/ccli-install.apk \
  'https://github.com/yahiagabereng-ctrl/CCLI/releases/download/p3-07-security-pass/ccli-0.1.0-r51.apk'
sha256sum /tmp/ccli-install.apk
# expect 0a2d2782d76e60dcbbafb851416ec24a0aa707b592c51cfa4545c08f1504419c

killall ccli 2>/dev/null
apk add --allow-untrusted --force-overwrite /tmp/ccli-install.apk
/usr/sbin/ccli --version
# expect: ccli 0.1.0-r51 (P3-07-AARE-SIGN-RAW-GT)

/etc/init.d/ccli restart
```

**Post-install binary SHA-256 (reference):**  
`7b05d7ccbbecd7c041d30cc6a362e1e8d1d12dea0e39792f37159d8cc5a6fabe`

### 4c. Preflight (before every TSP Connect session)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\dut-mms-tsp-preflight.ps1 -HostAddr 192.168.10.1
powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1 -HostAddr 192.168.10.1
```

Expect:

- `PREFLIGHT_PASS_3782_CCLI`
- MD5 lines match §2
- `ccli 0.1.0-r51 (P3-07-AARE-SIGN-RAW-GT)`

---

## 5. Test Suite Pro — exact settings

### Workspace / connection

| Setting | Value |
|---------|--------|
| **IED name** | `CCI016_01` |
| **CID** | `lab_tg544_eth_a.cid` only (31-LN lab) |
| **Server IP** | `192.168.10.1` |
| **Port** | **`3782`** (TLS MMS lab — not 102 alone) |
| **Local IP (PC-B)** | **`192.168.10.10`** /24 |
| **Client Connection IP (workspace)** | **`192.168.10.10`** |
| **Allow MMS** | On |
| **Connect from** | **System Status** |

### Stack (Show Advanced)

| Field | Value |
|-------|--------|
| Session selector (HEX) | `0001` |
| Presentation selector (HEX) | `00000001` |
| Transport selector (HEX) | `0001` |
| AP Title | `1,1,1,999,1` |
| AE Qualifier | `12` |
| AE / AP Invoke ID | `10` / `10` (lab default) |
| **Listening port** | **`3782`** |

### Transport Layer Security

| Field | Value |
|-------|--------|
| TLS enabled | **Checked** |
| Allow TLS 1.3 | **Unchecked** |
| Chaining depth | **`1`** |
| Certificate Authority | `C:\CCLI_tls\root_CA.pem` |
| Revocation list | `C:\CCLI_tls\lab_crl.pem` (if empty CRL errors in TSP) |
| Directory to CA | empty |
| Private key | `C:\CCLI_tls\client_tsp.key` |
| Public certificate | **`C:\CCLI_tls\client_tls.pem`** |

### MMS Security (General)

| Field | Value |
|-------|--------|
| Mechanism | **Certificate** |
| Chaining depth | **`1`** |
| Certificate Authority | `C:\CCLI_tls\root_CA.pem` |
| Revocation list | `C:\CCLI_tls\lab_crl.pem` |
| **Sisco compatibility** | **Unchecked** |
| Private key | `C:\CCLI_tls\client_tsp.key` |
| Public certificate | **`C:\CCLI_tls\client.pem`** |

**Never swap** Transport `client_tls.pem` ↔ MMS `client.pem`.

---

## 6. PC-B preflight (before Connect)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -ExecutionPolicy Bypass -File scripts\pc1-p3-07-connect-preflight.ps1 `
  -Dest "C:\Users\...\CCLI_TSP_FIELD_PACK" `
  -DutHost 192.168.10.1 `
  -PcIp 192.168.10.10
```

Then **re-copy `C:\CCLI_tls`** as in §3 (preflight may stage EJBCA files into `C:\CCLI_tls`).

OpenSSL smoke:

```powershell
powershell -File scripts\pc1-test-mms-tls12.ps1 -TlsDir C:\CCLI_tls -DutHost 192.168.10.1
```

Expect: `Verification: OK` · TLS 1.2.

---

## 7. Pass criteria

### Test Suite Pro

- System Status: **Connected**
- Output: **Initiate Response Success**

### DUT (`logread -e ccli` during Connect)

```text
mms: iso session CONNECT (payload=…)
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR …
mms: AARE responder auth [10]EXTERNAL …
```

Startup line:

```text
mms: listening on 192.168.10.1:3782 tls=on … crl=off … model=full_cid_cfg
```

---

## 8. Do not use for this session

| Wrong | Why |
|-------|-----|
| `client_acse.key` | EJBCA / unified matrix |
| `product_tr400_eth_a_5ln.cid` | Wrong LN model |
| Transport slot = `client.pem` | TLS fails or MMS abort |
| Long paths under `CCLI_TSP_FIELD_PACK\...` only | Key/CA load failures; use `C:\CCLI_tls\` |
| `lab/tg544-openwrt/ccli-bin` after r51 APK | Overwrites PASS binary with r55/r56 |
| Empty CRL when TSP shows error 20 / key errors | Use `lab_crl.pem` on PC; DUT stays `crl=off` |

---

## 9. Evidence / references

| Doc | Path |
|-----|------|
| Oct-08 PASS | `lab/evidence/testsuite-pro/inbox/TSP_P3_07_LIVE_CONNECT_PASS_2026-10-08.md` |
| Release pack | `lab/releases/P3-07-SECURITY-PASS/` |
| Masomeh PC1 | `handoff/masomeh95/00_START_HERE_P3_07_CONNECT.md` |
| Wire / TLS (Alireza) | `lab/evidence/TLS_WIRESHARK_HANDOFF_ALIREZA.md` |

---

## 10. One-page checklist

- [ ] Release APK SHA-256 verified  
- [ ] DUT: yaml `p3_07_connect` · TLS MD5 §2 · **r51** `ccli --version`  
- [ ] DUT: preflight **3782 → ccli**  
- [ ] PC: `C:\CCLI_tls\` refreshed · key-match **PASS**  
- [ ] TSP: CID + §5 certs + port **3782** + local **192.168.10.10**  
- [ ] Connect → DUT **ACSE auth ACCEPT**
