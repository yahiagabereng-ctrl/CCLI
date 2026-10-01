# CCI GitHub protocol libraries

**RAG source_id:** `ccli-github-protocol-libs`  
**Platform:** OpenWrt 25.12 on **TesPro TG544 / TG-524** · cross-compile via `lab/tg544-openwrt/`

Lab and product application stacks for OpenWrt / native Linux builds.

| Library | URL | Role | License | Harvest |
|---------|-----|------|---------|---------|
| **libiec61850** | https://github.com/mz-automation/libiec61850 | MMS, GOOSE, SV — Eth_A | GPLv3 + commercial | `github-refs/libiec61850.md` |
| **lib60870** | https://github.com/mz-automation/lib60870 | IEC 60870-5-104 — Eth_B | GPLv3 + commercial | `github-refs/lib60870.md` |
| **libmodbus** | https://github.com/stephane/libmodbus | Modbus RTU/TCP master | LGPL | **LISTED** — pin at freeze |
| **pymodbus** | https://github.com/pymodbus-dev/pymodbus | Pi bench simulator | BSD | bench only |

**Ship path:** MZ commercial license for libiec61850 (and likely lib60870) before product release.

**Build:** `cmake` / `make` on Pi or OpenWrt SDK — see harvested READMEs.
