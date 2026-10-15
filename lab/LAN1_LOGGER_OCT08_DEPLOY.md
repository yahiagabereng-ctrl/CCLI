# Logger PC on LAN1 — Oct-08 Connect deploy (r51 + p3_07_connect)

Use when the logger is cabled to **TG544 LAN1 (Eth_A)** for deploy, ping, and Wireshark on `:3782`.

## 1. IP (do not use 192.168.10.10 — reserved for PC-B / TSP)

**Admin:** `lab\set_pc_lan1_logger.cmd` → **`192.168.10.20/24`**, gateway **`192.168.10.1`**.

If the adapter is not named `Ethernet`, edit `ADAPTER=` in the script (Device Manager).

Pass: `ping 192.168.10.1` · `plink root@192.168.10.1` (same host key as LAN2).

**Note:** One physical NIC cannot be on **both** LAN1 and LAN2 at once. If you moved the cable from LAN2 to LAN1, run this script before SSH. Use USB GbE if you need LAN2 and LAN1 simultaneously.

## 2. Firmware + config (Oct-08 PASS profile)

| Item | Value |
|------|--------|
| Binary | **`ccli 0.1.0-r51`** · codename **`P3-07-AARE-SIGN-RAW-GT`** |
| Yaml | `apps/ccli/config/lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml` |
| TLS | `apps/ccli/config/tls/p3_07_connect/` |
| Model | `apps/ccli/config/icd/lab_tg544_eth_a.cfg` |

Build r51 (when repo is on r55):

```powershell
# Set apps/ccli/VERSION to 0.1.0 / 51 / P3-07-AARE-SIGN-RAW-GT and PKG_RELEASE:=51, then:
wsl bash lab/tg544-openwrt/build-ccli-ipk-sdk.sh
```

Reference SHA (r51 manifest): `2b166430fdb5851a46bad31c1741f11f7c553161fe98bce8b6bbc6558cbdf7ae` (7805952 bytes).

## 3. Deploy from logger (LAN1 up)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.10.1 `
  -LabYaml apps\ccli\config\lab_tr400_phase4_eth_b_ttyS1_p3_07_connect.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -Changes @('Oct-08 repro','ccli r51','p3_07_connect PKI')
```

## 4. Verify

```powershell
powershell -File scripts\dut-mms-tsp-preflight.ps1 -HostAddr 192.168.10.1
powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1
```

Pass log pattern: `iso session CONNECT` → `ACSE auth ACCEPT` → `AARE responder auth [10]EXTERNAL`.

## 5. PC-B (Masomeh)

Unchanged: `scripts\setup-pc1-p3-07-connect.ps1` · TSP **`192.168.10.10`** · same TLS MD5 pin as DUT.
