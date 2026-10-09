# Evidence per test (mandatory on PC-B)

**Applies to:** Phase 1 matrix **T1-01 … T1-10** on Test Suite Pro (LAN1 `192.168.10.10` → DUT `192.168.10.1:3782`).

Every matrix step gets **four artifacts** (one bundle per T1-xx):

| Artifact | Location | Naming |
|----------|----------|--------|
| **TSP log** | `inbox/` | `TSP_<gate>_<tool>_YYYY-MM-DD.txt` |
| **DUT SSH log** | `inbox/` | same basename + `_DUT_LOG.txt` (PRE at Start, POST at Stop) |
| **Wireshark summary** | `inbox/` | same basename + `_WIRE.txt` (frame counts; auto at Stop) |
| **Wireshark pcap** | `pcap/` | same basename + `.pcapng` |

Optional: screenshots (`.png`) with the same basename.

**SSH:** PC-B must reach DUT **`192.168.10.1`** (LAN1). Source secrets once:

```powershell
. D:\CCLI\CCLI\CCLI\lab\tg544-openwrt\lab-env.ps1
```

Requires PuTTY **plink** + **pscp**. Skip SSH with `-NoSsh` if offline.

---

## Workflow (repeat for each T1-xx)

1. **Start logger** (opens capture + refreshes log header):

   ```powershell
   cd D:\CCLI\CCLI\CCLI
   powershell -File scripts\tsp-evidence-logger.ps1 -Action Start -TestId T1-07 -Operator "YourName"
   ```

2. **Run exactly one TSP step** (e.g. Operate Wlim).

3. In TSP: select **Output** / relevant lines → **Copy**.

4. **Stop logger** (stops tshark, appends clipboard into the log):

   ```powershell
   powershell -File scripts\tsp-evidence-logger.ps1 -Action Stop -PasteClipboard
   ```

5. Set `result: PASS` or `FAIL` at the top of the `.txt` if not obvious from pasted output.

6. Tick the row in [LAB_REQUIREMENT_TEST_MATRIX.md](../../conformance/LAB_REQUIREMENT_TEST_MATRIX.md).

**One test = one Start/Stop pair.** Do not leave capture running across unrelated steps (keeps pcaps aligned with matrix rows).

---

## First session of the day

Create all stubs + session index:

```powershell
powershell -File scripts\tsp-evidence-logger.ps1 -Action Init -SessionDate 2026-10-09
```

Index file: `inbox/TSP_PHASE1_SESSION_INDEX_YYYY-MM-DD.md`.

---

## Metadata header (auto-written by script)

The logger writes the standard header from [README.md](README.md) plus `tsp_id`, `result`, `hint`, and capture timestamps.

---

## PC-A appendix (after T1-07 / T1-08)

Append to the same `.txt` or a sibling file:

```text
# DUT appendix (SSH)
ubus call dido_v2 status
logread | grep -iE 'Wlim|WSd|RBAC' | tail -20
```

---

## Troubleshooting

| Issue | Action |
|-------|--------|
| `Capture already active` | `-Action Stop` then Start again |
| Empty / tiny pcap | Confirm LAN1, filter `host 192.168.10.1 and port 3782`, TSP traffic during window |
| No Wireshark | `-NoCapture` on Start; still save TSP log |
| Compare step | Export `.xlsx` manually; log stub is for scope notes |

**Field pack:** `scripts/stage-tsp-field-pack.ps1` copies `tsp-evidence-logger.ps1` to the TSP PC USB pack.
