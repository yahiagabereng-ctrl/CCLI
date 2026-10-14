# TSP Connect — `tlsv1 alert access denied` (2026-10-14)

**Symptom (PC-B / Masomeh TestPro):**

```text
OpenSSL … 0A000419 … tlsv1 alert access denied
CCI016_01: Initiate Response Failed: Transport connection closed
Transport disconnect … 192.168.10.10 → 192.168.10.1
```

**Layer:** **Transport TLS** fails before MMS Initiate. DUT **`ccli`** uses **allow-only-known-certificates**; the client cert TSP presents must **byte-match** `/etc/ccli/tls/client_tls.pem` on the DUT (see `mms_adapter.cpp` → `TLSConfiguration_addAllowedCertificateFromFile`).

---

## Root cause found (dev PC preflight)

After **EJBCA → product TLS sync** (`apps/ccli/config/tls/PKI_SYNC_MANIFEST.json`, 2026-10-14), **`C:\CCLI_product_tls\root_CA.pem`** was refreshed but **`client_tls.pem` was stale**:

| File | Repo / DUT target (MD5) | Stale `CCLI_product_tls` (before fix) |
|------|-------------------------|----------------------------------------|
| `root_CA.pem` | `F265AE5C…` (`CN=CCLI Lab CA`) | `F265AE5C…` OK |
| `client_tls.pem` | **`13C231EE…`** | **`35A09C9F…`** WRONG |

TSP offered the old transport cert → DUT allowlist rejected → **access denied**.

Older note (legacy vs EJBCA CA only): [TSP_TLS_CA_MISMATCH_2026-10-09.md](TSP_TLS_CA_MISMATCH_2026-10-09.md).

---

## Fix — PC1 / Test Suite Pro (Masomeh)

1. **Git pull** on the TestPro PC clone.

2. **Restage TLS** (all three destinations):

   ```powershell
   cd C:\Yahia\projects\CCLI   # or D:\CCLI\CCLI
   git pull origin main
   powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
   powershell -File scripts\stage-tsp-tls.ps1
   powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir C:\CCLI_product_tls
   ```

   **Pass criteria:** verify script **PASS**; MD5 `client_tls.pem` = **`13C231EE6A2E782B3EB464AC2DEE4C21`** (must match repo `apps/ccli/config/tls/client_tls.pem`).

3. **TSP security** ([TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md](../../TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md)):

   | Slot | Path |
   |------|------|
   | CA | `C:\CCLI_product_tls\root_CA.pem` (or `D:\CCLI_TSP_FIELD_PACK\tls\`) |
   | Transport TLS | **`client_tls.pem`** + **`client_tsp.key`** |
   | MMS Security | **`client.pem`** + **`client_acse.key`** |

4. **Re-browse** cert paths in TSP (cached paths keep old PEMs). **Connect** `CCI016_01` @ `192.168.10.1:3782`.

---

## DUT verify (2026-10-14, LAN2 SSH)

| File | DUT MD5 | Repo MD5 | Match |
|------|---------|----------|-------|
| `root_CA.pem` | `f265ae5c…` | `F265AE5C…` | yes |
| `client_tls.pem` | **`13c231ee…`** | **`13C231EE…`** | yes |
| `server_tls.pem` | `40af60a3…` | `40AF60A3…` | yes |

Firmware **`ccli 0.1.0-r55`**, single **`ccli`** process. **No DUT TLS redeploy needed** if MD5s match; fix is on **PC1 staging** only.

---

## Fix — DUT (if Connect still fails after PC restage)

Deploy TLS from the **same** tree as git (`apps/ccli/config/tls` after sync):

```powershell
$env:CCLI_TG544_PW = '…'
powershell -File lab\tg544-openwrt\deploy-ccli-bench-lan2.ps1
# or deploy-ccli-session-fix.ps1 with current bench yaml
```

On DUT:

```bash
md5sum /etc/ccli/tls/root_CA.pem /etc/ccli/tls/client_tls.pem
ss -tlnp | grep 3782    # single ccli
/etc/init.d/iec61850service stop 2>/dev/null
```

Expect DUT `client_tls.pem` MD5 **`13C231EE…`**. Mismatch → redeploy TLS before another TSP attempt.

---

## Pass

- TSP: **Initiate Response Success** / **Connected**
- DUT log: `mms: ACSE auth ACCEPT … DSO_OPERATOR`

**Negative test (T3-01):** same alert is **expected** when connecting **without** client cert — do not use that profile for T1-01.
