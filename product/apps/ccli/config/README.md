# Product `config/`

| File | Role |
|------|------|
| **`product_tr400.yaml`** | **Ship** — 5-LN cyber · **EMT432 TCP LAN3** (ccli P/Q) · **Sun mock USB RS485** on PC (parallel) |
| **`product_tr400_emt432_5ln.yaml`** | Alias of `product_tr400.yaml` |
| **`product_tr400_rtu_a1b1.yaml`** | RTU meter on A1/B1 only (no TCP) |
| `lab_tr400_phase4_eth_b_ttyS1_5ln.yaml` | Cert bench (same cyber, DSO mock) |
| `icd/product/product_tr400_eth_a_5ln.cid` | Certificate CID (5 LNs) |
| `icd/product/product_tr400_eth_a_5ln.cfg` | Runtime model for `mms.model_cfg` |
| `lab_tr400_*.yaml` / `icd/lab_tg544_*` | Lab / 31-LN — **do not** set as DUT product config |
| `lab_tr400_cleartext_tsp*.yaml` | **Forbidden** on this binary (startup REFUSE) |

See [`../../PRODUCT_5LN_CYBER_MINIMAL.md`](../../PRODUCT_5LN_CYBER_MINIMAL.md).
