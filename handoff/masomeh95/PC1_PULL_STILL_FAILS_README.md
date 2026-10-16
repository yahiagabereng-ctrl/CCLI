# PC1 — `git pull` done but Encrypted Alert / no Connect (read this)

**GitHub gives correct PEMs.** Your symptom with **latest pull** usually means **Test Suite Pro still uses an old project** or the **wrong staging script** ran after pull.

---

## Do NOT run after pull (wrong TLS tree for Connect)

```powershell
# WRONG for P3-07 Connect — overwrites tls with EJBCA / dual-key docs
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

That pack tells TSP to use **`client_acse.key`** and **`product_tr400_eth_a_5ln.cid`** (`00i_PC1_TSP_IMPORT_CHECKLIST.md`).  
That matches **Encrypted Alert + no `ACSE auth ACCEPT` on DUT** even when transport TLS looks OK.

---

## DO run after pull (Connect profile only)

```powershell
cd D:\CCLI\CCLI\CCLI
git pull origin main
powershell -ExecutionPolicy Bypass -File scripts\pc1-p3-07-connect-preflight.ps1
```

Or use repo folder **`handoff/masomeh95/CCLI_TSP_CONNECT_MIN/`** (same MD5 as DUT when aligned).

---

## Reset Test Suite Pro (required if problem persists after pull)

Pull **does not** change paths saved inside your `.tsp` project.

1. **Close** Test Suite Pro.
2. **Rename** old project file (backup) or create **New Project**.
3. **Import CID only:** `D:\CCLI_TSP_FIELD_PACK\cid\lab_tg544_eth_a.cid` (**31-LN**, not 5-LN product).
4. **IED Connection** — browse **every** TLS field from `D:\CCLI_TSP_FIELD_PACK\tls\`:
   - Transport: **`client_tls.pem`** + **`client_tsp.key`**
   - MMS: **`client.pem`** + **`client_tsp.key`** (same key file)
   - Trust: **`root_CA.pem`**
   - CRL / revoked: **empty**
5. **Connect from System Status** (Oct-08 PASS), not a stale test cell with old paths.
6. AP Title **`1,1,1,999,1`**, Sisco **OFF**, TLS 1.3 **OFF**, local **`192.168.10.10`**, server **`192.168.10.1:3782`**.

---

## How to tell if it is PC1 vs DUT

During **one** Connect click, Yahia watches DUT:

| DUT log | Meaning |
|---------|---------|
| `client CONNECT` only | **PC1 / TSP MMS** (Type A) — fix § above |
| `ACSE auth ACCEPT` + `AARE …` then TSP alert | **Type B** — AARE verify (firmware/wire); rare until Type A fixed |

Oct-08 **PASS** used **`ccli-0.1.0-r51.apk`** (`P3-07-AARE-SIGN-RAW-GT`). Today DUT runs **`0.1.0-r55`** binary with same TLS yaml. If Type A is fixed and Type B appears, ask Yahia for **r51 rebuild + apk** on DUT.

---

## One-line proof TLS from PC1

```powershell
powershell -File scripts\pc1-test-mms-tls12.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

If this **PASS** but TSP still alerts → **100% TSP project paths / MMS key / CID**, not missing `git pull`.

See **`PC1_ENCRYPTED_ALERT_P3_07.md`** (Type A vs B).
