# TSP — `Initiate Response Failed` triage (2026-10-14)

**Symptom:** `CCI016_01: Initiate Response Failed: Transport connection closed` (often after SSL lines in TSP log).

**DUT checked:** `ccli 0.1.0-r55` · 5-LN yaml · TLS MD5 OK · **`iec61850d` was running** (must stop).

---

## Step 0 — DUT preflight (do first)

On DUT (SSH LAN2 `192.168.1.130`):

```bash
/etc/init.d/iec61850service stop
/etc/init.d/iec61850service disable
killall iec61850d
killall ccli
/usr/sbin/ccli --config /etc/ccli/lab.yaml >>/tmp/ccli.log 2>&1 &
sleep 2
ps w | grep ccli | grep -v grep    # exactly ONE
netstat -tlnp | grep 3782          # 192.168.10.1:3782 -> ccli
```

Or redeploy from logger PC: `lab/tg544-openwrt/deploy-ccli-bench-lan2.ps1` (runs `deploy-ccli-dut.sh` disable TesPro stack).

**Fail if:** `iec61850d` still present · two `ccli` · nothing on **3782**.

---

## Step 1 — Which layer failed?

| TSP log | DUT `/tmp/ccli.log` or `logread` | Meaning |
|---------|----------------------------------|---------|
| `tlsv1 alert access denied` | `TLS: handshake failed` | **Transport TLS** — wrong/missing **client_tls.pem** on TSP or MD5 ≠ DUT |
| `unable to get local issuer certificate` | (none) | TSP **CA** wrong — restage `root_CA.pem` |
| No SSL alert, then Initiate failed | `ACSE auth` / `AARE` lines | **MMS Security** — wrong **client.pem** / **client_acse.key** pairing |
| `Association Timeout` | silence | Often TLS never completed or wrong IP/port **102** vs **3782** |

**Live DUT evidence (2026-10-14 23:16):** `TLS: handshake failed - mbedtls_ssl_handshake returned -0x1` → treat as **Step 2** (PC1 certs / TSP paths), not MMS AARE yet.

---

## Step 2 — PC1 TLS (Masomeh)

```powershell
git pull origin main
powershell -File scripts\setup-pc1-testpro-from-git.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
powershell -File scripts\verify-tsp-tls-key-match.ps1 -TlsDir D:\CCLI_TSP_FIELD_PACK\tls
```

**MD5 must match DUT:**

| File | MD5 |
|------|-----|
| `root_CA.pem` | `f265ae5ca881a5097bc74a543784b443` |
| `client_tls.pem` | `13c231ee6a2e782b3eb464ac2dee4c21` |
| `client.pem` | `7bb6b77cde4614ae68ad387e580e2696` |

**TSP IED Connection — re-browse every path** (cached paths keep old PEMs):

| Slot | File |
|------|------|
| CA | `D:\CCLI_TSP_FIELD_PACK\tls\root_CA.pem` |
| Transport TLS | **`client_tls.pem`** + **`client_tsp.key`** |
| MMS Security | **`client.pem`** + **`client_acse.key`** (not `client_tsp.key`) |

Connection: **`192.168.10.1:3782`** TLS · IED **`CCI016_01`** · CID **`product_tr400_eth_a_5ln.cid`** (matches DUT 5-LN cfg).

---

## Step 3 — Pass criteria

**TSP:** `Initiate Response Success` · **Connected**

**DUT tail during connect:**

```text
mms: ACSE auth ACCEPT — … role=DSO_OPERATOR …
mms: AARE responder auth …
```

Refs: [TSP_TLS_ACCESS_DENIED_2026-10-14.md](TSP_TLS_ACCESS_DENIED_2026-10-14.md) · [TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md](../../../TSP_P3_07_LIVE_CONNECT_TEAM_HANDOFF.md)
