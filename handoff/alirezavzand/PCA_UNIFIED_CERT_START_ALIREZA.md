# PC-A unified cert run — Alireza start guide

**Matrix:** [LAB_UNIFIED_CERT_TEST_MATRIX.md](../../lab/conformance/LAB_UNIFIED_CERT_TEST_MATRIX.md) rev **1.3**  
**Pin:** [LAB_UNIFIED_SESSION_PIN_2026-10-14.md](../../lab/conformance/LAB_UNIFIED_SESSION_PIN_2026-10-14.md)  
**Runbook:** [TSP_TWO_PC_BENCH_RUNBOOK.md](../../lab/conformance/TSP_TWO_PC_BENCH_RUNBOOK.md)  
**Masomeh (PC-B TSP):** [../masomeh95/TSP_PC1_START_MASOMEH.md](../masomeh95/TSP_PC1_START_MASOMEH.md)

---

## 1. Repo sync

```powershell
git clone https://github.com/yahiagabereng-ctrl/CCLI.git
cd CCLI
git pull origin main
```

Optional field pack (shared docs on USB):

```powershell
powershell -File scripts\stage-tsp-field-pack.ps1 -Dest D:\CCLI_TSP_FIELD_PACK
```

---

## 2. DUT preflight (before Masomeh Connect)

```powershell
. .\lab\tg544-openwrt\lab-env.ps1
powershell -File scripts\validate-dut-unified-matrix.ps1 -FixServices -WriteEvidence
```

**Pass:** one `ccli` · **3782** + **2404** · no cleartext **:102** · evidence → `lab/evidence/testsuite-pro/inbox/PREFLIGHT_DUT_UNIFIED_*.txt`  
**Matrix:** **U-P0-07**, **U-P0-11**, **U-D7-01**

Target firmware: **`ccli 0.1.0-r53`** per session pin + deploy manifest under `lab/tg544-openwrt/deploy-manifests/`.

---

## 3. NIC scripts (Admin)

From repo or pack `bench_scripts\`:

| Script | PC IP | DUT |
|--------|-------|-----|
| `set_pc_lan2_oa.cmd` | `192.168.1.183` | `192.168.1.130:2404` |
| `set_pc_lan3_plant.cmd` or `set_pc_lan3_plant_178.cmd` | `192.168.30.10` | `192.168.30.1` + meter |

**Matrix:** **U-P0-03**, **U-P0-04**, **U-P0-05**

---

## 4. Phase 0b — plant (before U-T1-04 live reads)

```powershell
python lab\modbus_emt432_tcp_probe.py   # save stdout → modbus_emt432_<date>.txt
powershell -File lab\start-bench-plant.ps1 -SunOnly -Port COM5
```

**Yaml (DUT):** `lab_tr400_phase4_emt432_lan3_tcp.yaml` or `lab_tr400_phase4_eth_b_ttyS1.yaml` per pin.  
**Matrix:** **U-P0b-01 … U-P0b-05**

---

## 5. While Masomeh runs TSP (PC-B)

| When | Your action | U-ID |
|------|-------------|------|
| T1-05b | Change P on meter/Sun during report window | U-T1-05b |
| T1-07 | After Wlim: `ubus call dido_v2 status` → append `_DUT_LOG` | U-T1-11 |
| P5 step 13 | Vary inverter power if scripted | U-P5-02 |
| Any time | `chronyc tracking` on DUT → `P3_11_*.txt` | U-P3-11 |

**Do not** start TSP on this PC’s LAN1.

---

## 6. Phase 4 / 7 (same session or next)

| Phase | Action | Evidence |
|-------|--------|----------|
| 104 | Client → `:2404` TLS · monitor-only reject | `lab/evidence/phase4/P4_*` |
| O.14 | `ccli --event-wrap-test` · `--event-dump` · P7-11 script | `lab/evidence/phase7/P7_*` |

**Matrix:** **U-P4-01/02**, **U-P7-01 … U-P7-04**

---

## 7. TLS / Wireshark support (optional)

When Yahia sends **`TSP_*.pcapng`** or Step 7 fails:

- Read [TLS_WIRESHARK_HANDOFF_ALIREZA.md](../../lab/evidence/TLS_WIRESHARK_HANDOFF_ALIREZA.md)  
- Decrypt hint: `lab/evidence/testsuite-pro/pcap/README.md` (needs lab `server.key`)  
- PKI regen: [EJBCA_HANDOFF_ALIREZA.md](../../lab/evidence/EJBCA_HANDOFF_ALIREZA.md)

---

## 8. Closeout

1. Copy PC-A logs into repo `lab/evidence/`  
2. Tick your **U-*** rows in unified matrix (or send Yahia a list)  
3. Repo PC runs **U-CLO-03** ingest

**Keywords:** Alireza, PC-A, unified matrix, plant, TLS
