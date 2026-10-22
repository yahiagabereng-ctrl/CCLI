# DUT log error analysis — 2026-10-16 (TestPro Connect)

**Source:** `logread -e ccli` on DUT via LAN2 `192.168.1.130`  
**TSP symptom:** `Initiate Response Failed: Transport connection closed`

## Verdict

| Layer | Result |
|-------|--------|
| Transport TLS | **PASS** — allowlist match, client serial `DC:9A:9E:26:D5:47:EF:38` (`client_tls.pem`, CN=DSO_OPERATOR) |
| MMS / ACSE (AARQ) | **Never received** — no `iso session CONNECT`, no `ACSE auth ACCEPT` or `REJECT` in buffer |
| DUT AARE on wire | **N/A** — server only logs `AARE … ready` at **process start** (16:29:56, 16:39:43), not per client |
| DUT-reported error | **None** on latest attempts — session ends as `client DISCONNECT` immediately after `CONNECT` |

**Classification:** **Type A** — Test Suite Pro (or PC1 MMS cert/key/CID/project) aborts **after** TLS; DUT is not rejecting ACSE.

**Likely DUT-side fix (2026-10-16):** yaml had `model_cfg` → **5-LN** while frozen TSP uses **`lab_tg544_eth_a.cid`**. Align DUT to `/etc/ccli/icd/lab_tg544_eth_a.cfg` (`product_tr400_p3_07_connect.yaml`) and push yaml-only; use `product_tr400_p3_07_connect_5ln.yaml` only when TSP imports the 5-LN CID.

## Example attempts (latest)

```
16:42:17  TLS verify OK (DC:9A:9E:26:D5:47:EF:38)
16:42:17  mms: client CONNECT count=1
16:42:17  mms: client DISCONNECT count=0

16:44:10–16:44:17  (same pattern ×3)
```

## Not seen on successful-path attempts today

- `TLS: handshake failed` (on 16:42–16:44 series)
- `ACSE auth REJECT`
- `mms: iso session CONNECT`

## PC1 fix (matches log)

Re-browse TSP: MMS **`client.pem`** + **`client_tsp.key`** (not `client_acse.key`); CID **`lab_tg544_eth_a.cid`**; new project if paths cached.
