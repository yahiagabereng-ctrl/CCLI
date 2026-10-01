# OpenWrt package — `ccli`



Builds `apps/ccli/` as an **`.apk`** for **OpenWrt 25.12.x** on **TesPro MT798X** (`mediatek/filogic`).



| Artifact | Installed path |

|----------|----------------|

| Binary | `/usr/sbin/ccli` |

| YAML config | `/etc/ccli/lab.yaml` (from `lab_tr400_phase1_regulation.yaml`) |

| UCI config | `/etc/config/ccli` |

| procd init | `/etc/init.d/ccli` |

| Modbus/ICD stubs | `/usr/share/ccli/config/` |



**Platform:** `platform_tg500` (TesPro TG544 / TR500 / TG-524).



## Link feed (SDK)



From OpenWrt 25.12 SDK (`lab/tg544-openwrt/`):



```bash

REPO=/mnt/c/Yahia/projects/CCLI



echo "src-link ccli ${REPO}/package" >> feeds.conf

./scripts/feeds update ccli

./scripts/feeds install ccli

make package/feeds/ccli/ccli/compile V=s

```



Output: `bin/packages/aarch64_cortex-a53/ccli/ccli-*.apk`



## Deploy to TG544



```bash

scp lab/tg544-openwrt/ccli-*.apk root@192.168.1.130:/tmp/

ssh root@192.168.1.130 'apk add --allow-untrusted /tmp/ccli-*.apk'

```



See `lab/tg544-openwrt/DEPLOY_CCLI.md`.



## References



- `knowledge-base/08-engineering/CCI_OpenWrt_Procedural_Extract.md` — UCI · procd · apk

- `apps/ccli/README.md` — application source layout


