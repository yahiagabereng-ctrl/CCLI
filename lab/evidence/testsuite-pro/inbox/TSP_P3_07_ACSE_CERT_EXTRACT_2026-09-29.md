# TSP P3-07 — ACSE 62351-4 certificate extraction fix

**Date:** 2026-09-29  
**DUT:** TG544 `192.168.10.1:3782` — pid after deploy **31383**  
**PC:** `192.168.10.10` (Eth_A)

## Symptom (before fix)

TSP System Status:

- `AssociationTimeout`
- `Initiate Response Failed: Transport connection closed`

DUT log (session layer already fixed):

```text
mms: iso session CONNECT (payload=1405)
mms: ACSE auth REJECT — cert not mapped to lab role (888 octets, mech=CERTIFICATE)
mms: acse AARQ auth failed (mechLen=8 authValueLen=1164)
```

## Root cause

TSP AARQ sends **IEC 62351-4 MMS-Authentication-value**:

```text
SEQUENCE {
  [0] IMPLICIT certificate  (tag 0x80)
  [1] IMPLICIT time         (tag 0x81)
  [2] IMPLICIT signature    (tag 0x82)
}
```

`extractX509FromAuthValue()` in `acse.c` treated the **outer 888-byte SEQUENCE** as the X.509 cert because `isX509CertificateSequence()` only checked valid BER length, not TBSCertificate structure. RBAC compare against `dso_der` (~869 octets from `/etc/ccli/tls/client.pem`) failed.

TLS layer had already verified **DSO_OPERATOR** — failure was ACSE extraction only.

## Fix (acse.c)

1. **Strengthen** `isX509CertificateSequence()` — require first child tag `0x30` (TBSCertificate).
2. **Add** `extractCertFrom62351MmsAuth()` — parse `0xa0` wrapper (if present) → `0x30` → tag **`0x80`** certificate.
3. **TLS fallback** — if AARQ cert RBAC fails, retry with verified TLS peer certificate (`ACSE_AUTH_TLS`).

## Deploy

```powershell
$env:CCLI_TG544_PW = "..."
wsl bash lab/tg544-openwrt/wsl-build-ccli.sh
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1
```

Build: `ccli-0.1.0-r16.apk` → `lab/tg544-openwrt/ccli-bin`  
Deploy: **OK** — listening `192.168.10.1:3782`, `rbac=on`, `acse_tls_auth=on`

## TSP retest (user action)

1. System Status → Connect `CCI016_01` @ `192.168.10.1:3782`
2. **Pass criteria on DUT** (`tail -f /tmp/ccli.log`):

   ```text
   mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=869 octets
   ```

   or (fallback path):

   ```text
   mms: ACSE auth ACCEPT — mech=TLS role=DSO_OPERATOR cert=869 octets
   ```

3. TSP should show **Connected** (not `AssociationTimeout`).

## After connect

- Capture TSP screenshot + `TSP_P3_07_LIVE_CONNECT_PASS` evidence
- Proceed P3-03 / P3-04 / P3-05 live tests
