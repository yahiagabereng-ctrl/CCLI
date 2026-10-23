# P5 — EMT432 PdCMMXU1 3φ CMV (r62 deploy)

**Document ID:** CCLI-LAB-P5-EMT432-PDC-3PH-r62  
**Date:** 2026-10-22  
**Software:** `0.1.0-r62` (`P5-EMT432-PDC-3PH-CMV`)  
**Manifest:** [CCLI_DEPLOY_0.1.0-r62_2026-10-22_183656.md](../../tg544-openwrt/deploy-manifests/CCLI_DEPLOY_0.1.0-r62_2026-10-22_183656.md)

## What shipped

- **`modbus.poll_pdc_3ph: true`** on DUT yaml → single FC3 read **40907–40946** (offsets **906–945**).
- MMS updates **`PdCMMXU1.PPV.phsAB/BC/CA.cVal.{mag,ang}.f`** and **`A.phsA/B/C.cVal.{mag,ang}.f`**.
- **1P Rogowski:** I2/I3 and V_BC/V_CA stay **0**; angles **I** from **P1/Q1**, **PPV** ang **0°**.
- **3P:** when I2/I3 or V_BC active, PPV ang **0 / −120 / +120°** (nominal DEL).

## Binary identity

| Item | Value |
|------|--------|
| Staging / DUT ELF | SHA256 `20a974f14ddf73175a48d97105ccffca247d591b0f7a1efbecad873aa15baf3e` |
| Size | 7824040 B |
| Pin | `lab/tg544-openwrt/ccli-bin.r62.p5-emt432-pdc-3ph-cmv` |
| APK | `ccli-0.1.0-r62.apk` |

## Deploy

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
.\lab\tg544-openwrt\deploy-ccli-session-fix.ps1 `
  -HostAddr 192.168.1.130 `
  -LabYaml apps\ccli\config\product_tr400_p3_07_connect_5ln.yaml `
  -TlsDir apps\ccli\config\tls\p3_07_connect `
  -Changes @("r62 PdCMMXU1 PPV/A 3ph CMV")
```

**Note:** From this bench PC, **LAN1 `192.168.10.1` SSH timed out**; use **LAN2 `192.168.1.130`** for upload. MMS still listens on **`192.168.10.1:3782`** for TestPro.

## Post-deploy checks (PASS 2026-10-22)

```text
/usr/sbin/ccli --version  → 0.1.0-r62 (P5-EMT432-PDC-3PH-CMV)
sha256sum /usr/sbin/ccli  → 20a974f14ddf73175a48d97105ccffca247d591b0f7a1efbecad873aa15baf3e
ss -tlnp | grep 3782      → 192.168.10.1:3782 ccli
grep poll_pdc_3ph /etc/ccli/lab.yaml → true
```

**Modbus (logread):** `FC3 … addr=40907 qty=40` (extended block vs r61 qty=36).

## TestPro / MMS reads (manual)

| ObjectRef (under plant) | FC |
|-------------------------|-----|
| `PdCMMXU1.A.phsA.cVal.mag.f` / `ang.f` | MX |
| `PdCMMXU1.A.phsB.cVal.mag.f` / `ang.f` | MX |
| `PdCMMXU1.PPV.phsAB.cVal.mag.f` / `ang.f` | MX |
| `PdCMMXU1.PPV.phsBC.cVal.mag.f` | MX (≈0 on 1P) |

**4 s URCB dataset** unchanged (`DS_R_PdC_Mis4sec` — TotW, TotVAr, PPV AB mag, A phsA mag only).

## References

- [EMT432_CCLI_COMMISSIONING_MAP.md](../../conformance/EMT432_CCLI_COMMISSIONING_MAP.md) §2  
- [CCLI_SW_VERSION_AND_BINARIES.md](../../tg544-openwrt/CCLI_SW_VERSION_AND_BINARIES.md)  
- [apps/ccli/CHANGELOG.md](../../../apps/ccli/CHANGELOG.md) — **0.1.0-r62**
