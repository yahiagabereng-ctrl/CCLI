# Product cyber + EMT432 meter map freeze — 0.1.0-r64

**Date:** 2026-10-23  
**Codename:** P5-EMT432-CMV-ANG-URCB-PF1  
**Parent:** `ccli-product-sw-freeze` · [EMT432_METER_CODE_FREEZE.md](../../../lab/conformance/EMT432_METER_CODE_FREEZE.md)

## Frozen pair

| Item | Value |
|------|--------|
| **Image** | `ccli-0.1.0-r64.apk` · ELF SHA-256 `967df2764819c3b616e5467a16a3f9a923b4002ad1ca4a73d156d16e91ab2df7` |
| **Yaml** | `product_tr400_p3_07_connect_5ln.yaml` SHA-256 `366a7d06100d13cff0d1f1c1c2ecdc171aa8e548c4dcccbf9e9f3860b04713a4` |
| **Model** | `product_tr400_eth_a_5ln.cfg` SHA-256 `68fc1de01c67037ca502945f0c7a94628b0847f7da7f342f5f4f7d102a0f5741` |
| **Meter map rev** | **1.0** · pin `lab/emt432-meter-code.freeze.json` |
| **LN verify log** | `lab/evidence/phase5/P5_EMT432_LN_VERIFY_2026-10-23_091200.txt` |

## Verify

```powershell
powershell -File scripts/verify-emt432-meter-code-freeze.ps1
```
