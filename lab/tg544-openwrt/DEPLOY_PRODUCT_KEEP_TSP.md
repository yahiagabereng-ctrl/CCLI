# Deploy product firmware — keep TestPro (r51 P3-07) unchanged

**Rule:** Do **not** change TestPro TLS or cert slot wiring. DUT must use **`tls/p3_07_connect`** (same MD5 pins as [LAB_ACCESS_FREEZE.md](../LAB_ACCESS_FREEZE.md) §2).

| Item | Value |
|------|--------|
| **Script** | `deploy-product-fw-same-cyber.ps1` |
| **Binary** | **Keep DUT r51 ELF `7b05d7cc…`** (`ccli-bin.r51.dut-connect-pass`). `-ReplaceBinary` uploads r55 `ccli-bin` (breaks Connect until A/B). |
| **Yaml** | `apps/ccli/config/product_tr400_p3_07_connect.yaml` |
| **TLS dir** | `apps/ccli/config/tls/p3_07_connect` (**not** `tls/` EJBCA) |
| **MMS model** | `lab_tg544_eth_a.cfg` in `product_tr400_p3_07_connect.yaml` (5-LN: `_5ln.yaml` + TSP 5-LN CID) |
| **Runtime** | `crl=off` · allowlist **DSO_OPERATOR** client certs |

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File lab\tg544-openwrt\deploy-product-fw-same-cyber.ps1 -HostAddr 192.168.1.130
```

**Yaml/binary only (TLS already correct):**

```powershell
powershell -File lab\tg544-openwrt\deploy-product-fw-same-cyber.ps1 -TlsOnly
```

**Do not deploy** `product_tr400.yaml` with default `apps/ccli/config/tls` if TestPro stays on P3-07 — that replaces DUT CA and breaks Connect.

**Restore GitHub r51 (PASS) without changing TestPro certs:**

```powershell
powershell -File lab\tg544-openwrt\deploy-r51-github-p3-07-connect.ps1 -HostAddr 192.168.1.130
```

**Verify DUT PEMs match TestPro (no PC change):**

```powershell
powershell -File scripts\verify-dut-p3-07-connect-alignment.ps1 -HostAddr 192.168.1.130
```

Expect live DUT `root_CA.pem` MD5 **`fa23eb4d198c74059cdf72d585eb50c6`** (serial `4DAA40028901CFFB`).
