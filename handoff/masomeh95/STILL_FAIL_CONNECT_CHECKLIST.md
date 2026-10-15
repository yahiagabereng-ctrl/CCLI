# Still "Initiate Response Failed" — checklist (DUT TLS OK)

If Yahia confirmed **MD5 pin PASS** on DUT and you still fail, the problem is almost always **one of these three** (not redeploy CA again).

## 1. TestPro UI paths (most common)

Disk MD5 can be correct while TSP still points at **old browsed files**.

| TSP slot | File | Serial check |
|----------|------|----------------|
| Transport TLS **cert** | **`client_tls.pem`** | `DC9A9E26D547EF38` · CN=**DSO_OPERATOR** |
| Transport TLS **key** | **`client_tsp.key`** | |
| MMS **cert** | **`client.pem`** | `28E53F07CFE347B0` · OID **1.12** |
| MMS **key** | **`client_tsp.key`** | **same file** as transport key |

**Wrong:** Transport = `client.pem` → DUT may still TLS-accept but TSP shows *Transport connection closed* before Initiate.

**Action:** Re-browse **all six** paths. Paste TSP **Output** window after Connect.

## 2. DUT firmware (logger)

Oct-08 PASS used **`ccli-0.1.0-r51.apk`**. Logger **binary-only** deploy is **not** APK install.

On DUT (Yahia):

```bash
/usr/sbin/ccli --version
sha256sum /usr/sbin/ccli
strings /usr/sbin/ccli | grep 'iso session CONNECT' | head -1
```

Need **`iso session CONNECT`** and **`ACSE auth ACCEPT`** in log during Connect — not only `client CONNECT count=1`.

Logger redeploy (LAN2): `scripts\deploy-p3-07-connect-dut.ps1 -HostAddr 192.168.1.130`

Optional APK: add **`-InstallApk`** (uses `ccli-0.1.0-r55.apk` from repo).

## 3. One synchronized capture

Yahia: `capture-dut-tsp-connect-log.ps1` **before** Connect (no preflight during click).

**Pass DUT lines:**

```text
mms: iso session CONNECT (payload=…)
mms: ACSE auth ACCEPT — … DSO_OPERATOR
mms: AARE responder auth [10]EXTERNAL …
```

If you only see `CONNECT count=1` and **no** `iso session` → fix **§1** first, then logger firmware **§2**.

## Connection constants

- **192.168.10.1:3782** · **CCI016_01** · **`lab_tg544_eth_a.cid`**
- PC LAN1 **192.168.10.10** · AP **1,1,1,999,1** · Sisco **OFF** · CRL **empty**
