# CCI Application — `apps/ccli/`



User-space firmware for **PF2 / CEI 0-16 CCI** on **TesPro only**.



**OS (frozen):** **OpenWrt 25.12 / TesproOS** on **TG544 / TR500 / TG-524** (MT798X).  

**Active lab DUT:** **TG544** @ `192.168.1.130` — config `config/lab_tr400.yaml`.  

**Platform:** `platform_tg500` only (Raspberry Pi / `platform_pi` **out of scope**).



| Platform | OS | Role | Build |

|----------|-----|------|-------|

| `platform_tg500` | OpenWrt / TesproOS on TesPro | Lab + product | `lab/tg544-openwrt/` SDK → `.apk` |



**Corpus:** `knowledge-base/06-application/CCI_Application_Structure.md` · `CCI_TR500_IO_Peripheral.md` · `CCI_OpenWrt_Freeze.md`



## Build for TG544 (cross-compile)



```bash

cd lab/tg544-openwrt

./download-sdk.sh

./build-ccli-ipk-sdk.sh    # → ccli-*.apk

```



Deploy: `lab/tg544-openwrt/DEPLOY_CCLI.md`



## On-device bootstrap (interim)



If SDK parity issues appear, compile on TG544 with dev packages from TesPro — prefer host SDK when stable.



## Directory layout



```text

apps/ccli/

├── CMakeLists.txt                 # BUILD_PLATFORM=tg500

├── config/lab_tr400.yaml          # TG544 DIO / serial / PF2 bindings

├── core/                          # PF2, measurement, events — portable

├── adapters/                      # Modbus, MMS, IEC104

├── platform/platform_tg500/       # TesPro HAL (ubus DIO, ttyS*)

├── test_vectors/                  # Run on TG544

└── tests/unit/                    # Host unit tests (optional, not Pi-specific)

```



Portable code lives in `core/`, `adapters/`, `services/` — only `platform/` is target-specific.


