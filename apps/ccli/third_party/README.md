# Third-party protocol libraries

Pin as git submodules when integrating:

| Library | URL | Role |
|---------|-----|------|
| libiec61850 | https://github.com/mz-automation/libiec61850 | MMS / GOOSE |
| lib60870 | https://github.com/mz-automation/lib60870 | IEC 60870-5-104 |
| libmodbus | https://github.com/stephane/libmodbus | Modbus RTU/TCP |

```bash
git submodule add https://github.com/mz-automation/libiec61850 third_party/libiec61850
git submodule add https://github.com/mz-automation/lib60870 third_party/lib60870
git submodule add https://github.com/stephane/libmodbus third_party/libmodbus
```

**Lab status (2026-09-25):** `libiec61850` **v1.6.0** present under `third_party/libiec61850` (CMake option `CCLI_WITH_LIBIEC61850`). Linked **static** into `ccli` for Eth_A MMS (Phase 3 Step 4). Ship requires **MZ commercial license**.

**Lab status (2026-09-30):** `lib60870` **v2.3.5** under `third_party/lib60870` (`CCLI_WITH_LIB60870`). CS104 slave on Eth_B via `iec104_adapter` (Phase 4 P4-02). GPL / commercial license as for libiec61850.

See `knowledge-base/07-protocols/CCI_GitHub_Protocol_Libraries.md`.
