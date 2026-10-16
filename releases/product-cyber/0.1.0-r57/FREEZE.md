# Product cyber freeze — 0.1.0-r57 (Connect PASS)

**Date:** 2026-10-16  
**Codename:** P5-5LN-ISO-PRES-AARE  
**Result:** TestPro **Connected** — Initiate Response Success  
**Parent:** `ccli-product-sw-freeze` · r55 product 5-LN + r51 P3-07 cyber

## Frozen pair (do not split)

| Item | Value |
|------|--------|
| **Image** | `ccli-0.1.0-r57.apk` · ELF SHA-256 `96963619dbebb87e366510a555fdac729a4b948847b76f1f91d94b5b6cd7d697` |
| **Config** | `product_tr400_p3_07_connect_5ln.yaml` SHA-256 `ed7d8778996284ac3aa5407b9d9c70f5651ca0e5cc39fa146be0634a41ef5f86` |
| **Model** | `product_tr400_eth_a_5ln.cfg` SHA `7e13de139559adc68da0bbae0c8ab0bd20abded6af3472a9974cd4a5bc6a3146` |
| **CID (TestPro)** | `product_tr400_eth_a_5ln.cid` SHA `68c72e7f87d1c65b04a3e42925e727815ec8b1a421e0dc65296c7c65b3cf2582` |
| **TLS** | `apps/ccli/config/tls/p3_07_connect` · **crl=off** · `client_tsp.key` both TSP slots |
| **MMS** | `192.168.10.1:3782` TLS 1.2 · ACSE G.2 |
| **IEC 104** | `192.168.1.130:2404` TLS |

## Live DUT (2026-10-16 20:30 DUT local)

| Field | Value |
|-------|--------|
| Host | TG544 `192.168.1.130` · pid **20381** |
| Version | `ccli 0.1.0-r57 (P5-5LN-ISO-PRES-AARE)` |
| Yaml on DUT | `/etc/ccli/lab.yaml` (byte-identical to frozen yaml) |
| Model on DUT | `/etc/ccli/icd/product/product_tr400_eth_a_5ln.cfg` |
| Pin | `lab/tg544-openwrt/ccli-bin.r57.dut-connect-pass` |
| Extract | `lab/tg544-openwrt/dut-extract-connect-pass-r57-2026-10-16/` |

DUT log (PASS):

```text
mms: iso session CONNECT (payload=1332)
mms: iso presentation skip non-ACSE/MMS context id=5 (mmsAuthenticationAS)
mms: ACSE auth ACCEPT — mech=CERTIFICATE role=DSO_OPERATOR cert=1071 octets
mms: AARE responder auth [10]EXTERNAL ctx=5 inner=1075 octets
mms: iso session ACCEPT extended LI (spdu=1264 payload=1244)
```

## Restore

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
# Rebuild not required if ccli-bin.r57.dut-connect-pass is present:
Copy-Item lab\tg544-openwrt\ccli-bin.r57.dut-connect-pass lab\tg544-openwrt\ccli-bin -Force
powershell -File lab\tg544-openwrt\deploy-product-fw-same-cyber.ps1 -ReplaceBinary `
  -LabYaml apps\ccli\config\product_tr400_p3_07_connect_5ln.yaml
```

TestPro: keep P3-07 certs; import **5-LN CID** (`product_tr400_eth_a_5ln.cid`).

## Trees

| Tree | Role |
|------|------|
| `apps/ccli/` | Lab build · this r57 source |
| `releases/product-cyber/0.1.0-r57/` | Immutable Connect-PASS bundle |
