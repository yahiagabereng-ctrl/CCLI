# Masomeh — fix TSP Connect (`Initiate Response Failed`)

**From:** Yahia · **Git:** `main` @ **`2f14e5b`** or later  
**DUT:** `ccli 0.1.0-r55` · **`192.168.10.1:3782`** · 5-LN model · IED **`CCI016_01`**

---

## A. On logger PC (optional — Yahia can run)

```powershell
cd D:\CCLI\CCLI
git pull origin main
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\dut-mms-tsp-preflight.ps1
```

**Pass:** one `ccli` · **3782** listen · no `iec61850d`.

---

## B. On your TestPro PC (PC1) — **required**

**Yahia (before USB) — full DUT TLS mirror:**

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\export-pc1-tls-for-masomeh.ps1 -Dest D:\handoff\PC1_tls_full
```

USB **entire folder** to Masomeh (all PEMs/keys from **live DUT**, including server + `root_CA.key`).

**Masomeh:** copy that folder to `D:\CCLI_TSP_FIELD_PACK\tls\` **or** from git (client subset only) then prefer USB:

```powershell
cd D:\CCLI\CCLI
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-dut-pc1-tls-alignment.ps1
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
powershell -File scripts\pc1-test-mms-tls12.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

**TSP:** set transport security to **TLS 1.2** (not 1.3-only / not “auto” if that picks 1.3). DUT **r56+** caps MMS at TLS 1.2 (62351-3 PIXIT).

**MD5 check** (must match **DUT** — if you see `b28b0f41…` / `4490e817…` you have a **stale pack**, re-run setup):

| File | MD5 (current DUT r55) |
|------|------------------------|
| `root_CA.pem` | `f265ae5ca881a5097bc74a543784b443` |
| `client_tls.pem` | `13c231ee6a2e782b3eb464ac2dee4c21` |
| `client.pem` | `7bb6b77cde4614ae68ad387e580e2696` |

Full list: `D:\CCLI_TSP_FIELD_PACK\tls\DUT_TLS_MANIFEST.json`  
Your **144deb9** capture had mismatches — see `TSP_P3_01_CONNECT_FAIL_SUMMARY_2026-10-14.md`.

---

## C. Test Suite Pro — **re-browse all cert paths**

Use **`D:\CCLI_TSP_FIELD_PACK\tls\`** only (not old `CCLI_product_tls` unless you just ran stage).

| TSP field | File |
|-----------|------|
| Trusted CA | `root_CA.pem` |
| Transport TLS cert | **`client_tls.pem`** |
| Transport TLS key | **`client_tsp.key`** |
| MMS Security cert | **`client.pem`** |
| MMS Security key | **`client_acse.key`** (not `client_tsp.key`) |

**IED Connection:** host **`192.168.10.1`**, port **`3782`**, TLS **on**.

**SCL:** import **`cid\product_tr400_eth_a_5ln.cid`** from the field pack.

---

## D. Pass criteria

- TSP: **`Initiate Response Success`** / **Connected**
- If still red: copy **first SSL error line** + your `client_tls.pem` MD5 to Yahia.

**Detail:** `lab/evidence/testsuite-pro/inbox/TSP_INITIATE_RESPONSE_TRIAGE_2026-10-14.md`
