# Product `config/`

Parent index: [`../../README.md`](../../README.md). Logical nodes and cyber: [`../../PRODUCT_17LN_CYBER.md`](../../PRODUCT_17LN_CYBER.md).

## Load on the DUT

| File | Role |
|------|------|
| **`product_tr400.yaml`** | Ship. 17-LN model. EMT432 Modbus **TCP** on LAN3. DSO mock off |
| **`product_tr400_emt432_17ln.yaml`** | Same plant and the same model |
| **`lab_tr400_phase4_eth_b_ttyS1_17ln.yaml`** | USB-RS485 meter on A1/B1 (`/dev/ttyS1`). DSO mock on |
| `icd/product/product_tr400_eth_a_17ln.cid` | Ship CID for the TSP client |
| `icd/product/product_tr400_eth_a_17ln.cfg` | Runtime model. DUT path is `/etc/ccli/icd/product_tr400_eth_a_17ln.cfg` |

There is no `product_tr400_rtu_a1b1.yaml`. The RTU meter profile is `lab_tr400_phase4_eth_b_ttyS1_17ln.yaml`.

## Leave on disk, do not select

| File | Why |
|------|-----|
| `icd/product/product_tr400_eth_a_5ln.*` | Previous A0 pin |
| `icd/lab_tg544_eth_a.*` | 31-LN lab model |
| `lab_tr400_cleartext_tsp*.yaml` | Port 102. This binary refuses it |
| `product_tr400_p3_07_connect.yaml` | Old TestPro profile. Loads the lab CID |
| other `lab_tr400_*.yaml` | Lab sessions, not the 17-LN ship |
