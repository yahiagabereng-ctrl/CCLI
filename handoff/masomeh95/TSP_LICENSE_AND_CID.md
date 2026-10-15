# Test Suite Pro — CID + license (Masomeh handoff)

## CID (mandatory for this Connect profile)

| Item | Path |
|------|------|
| Repo canonical | `apps/ccli/config/icd/lab_tg544_eth_a.cid` |
| Handoff copy | `handoff/masomeh95/cid/lab_tg544_eth_a.cid` |
| Field pack | `cid\lab_tg544_eth_a.cid` after `scripts\setup-pc1-p3-07-connect.ps1` |

**TSP:** Import **`lab_tg544_eth_a.cid`** — not `product_tr400_eth_a_5ln.cid` for Oct-08 / `p3_07_connect`.

**DUT:** `model_cfg` → **`lab_tg544_eth_a.cfg`** (logger pushes `.cid` + `.cfg` via `scripts\push-lab-cid-to-dut.ps1`).

## License (not in git)

Triangle **61850 Test Suite Pro** uses **Sentinel** (`.v2c`). License files are **machine-bound** and are **not** stored in this repository.

| Status (2026-10-01) | Lab trial Key Id `1099954349370049388` — **expired** |
|---------------------|------------------------------------------------------|

**On a licensed PC:** copy the vendor `.v2c` to USB under `tsp_license\` only — do **not** commit `.v2c` to GitHub.

**Install (TMW / Sentinel):** TSP installer → license manager → import `.v2c` → reboot TSP if prompted.

**No license:** use **IEDExplorer** for manual MMS (see `lab/evidence/phase5/P5_VERIFICATION_CLIENT_STATUS_2026-10-01.md`).

## Git pull (Masomeh)

```powershell
git pull
powershell -File scripts\setup-pc1-p3-07-connect.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

CID is in git; license must come from Yahia USB or TMW support separately.
