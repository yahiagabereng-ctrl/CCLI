# DUT deploy 2026-10-22 + TestPro certificates (P3-07 / 5-LN)

**DUT:** `root@192.168.1.130` (Eth_B) · MMS **`192.168.10.1:3782`** · 104 **`192.168.1.130:2404`**  
**Firmware:** `ccli 0.1.0-r57 (P5-5LN-ISO-PRES-AARE)`  
**ELF SHA256:** `51afd8a74ef3e604f90fd89dbb7f0fd8f7d08c75ce6eef7f0d1fa68b7d4e0533`  
**Manifest:** `lab/tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r57_2026-10-22_145239.md`  
**Yaml on DUT:** `product_tr400_p3_07_connect_5ln.yaml` · **crl=off**

## Files in this folder

| File | Test Suite Pro slot |
|------|---------------------|
| `root_CA.pem` | Trust CA |
| `client_tls.pem` | **Transport** TLS certificate (CN=DSO_OPERATOR, serial DC9A9E26…) |
| `client.pem` | **MMS / ACSE** certificate (OID 1.1.1.999.1.12, serial 28E53F07…) |
| `client_tsp.key` | Transport **and** MMS private key (both key slots) |
| `product_tr400_eth_a_5ln.cid` | IED model — **5 LNs** |
| `ccli-0.1.0-r57.apk` | OpenWrt package (same r57 as deployed ELF) |

**MD5 (must match DUT / LAB_ACCESS_FREEZE):**

| File | MD5 |
|------|-----|
| root_CA.pem | `7ada0cbe2be1bb4bddff7b6961461bc5` |
| client_tls.pem | `524fdd81b0d87abc9dc8c1cd2c490968` |
| client.pem | `fe374442fce9eea87f7a937bac1b3bea` |
| client_tsp.key | `9858beb9736ad5b32b72238d0cbfa27c` |

## TestPro connection

| Field | Value |
|-------|-------|
| IED | `CCI016_01` |
| Host | `192.168.10.1:3782` TLS on |
| PC LAN1 | `192.168.10.10/24` |
| CRL / revoked | **empty / off** |

Canonical repo TLS: `apps/ccli/config/tls/p3_07_connect/`
