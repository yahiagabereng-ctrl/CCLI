# P4-00 — TesPro IEC 61850 supplier software (install + verify)

**Date:** 2026-09-30  
**DUT:** TG544 / TR500 · `192.168.10.1` (LAN1 deploy)  
**Gate:** **P4-00** — supplier add-on installed before Eth_B 104 work  
**Manual:** `knowledge-base/08-engineering/reference/vendor/tespro/IEC61850-User-Manual.docx`  
**Manifest:** `knowledge-base/08-engineering/reference/vendor/tespro/TG500_iec61850_APK_Manifest.md`

---

## Scope

| In scope | Out of scope |
|----------|--------------|
| TesproOS **apk** install (4 packages) | DSO MMS server Eth_A `:3782` |
| `iec61850-mmsd` + `iec61850d` **running** | CEI Annex T / 62351-4 A-profile |
| LuCI **Services → IEC 61850 Protocol** | Operator 104 Eth_B `:2404` (P4-01…) |
| SSH health script | Northbound MQTT/TCP config (optional) |

**Programme rule:** DSO path remains **`apps/ccli`**. TesPro stack = **MMS client/collector** (poll plant IEDs on `:102`).

---

## Install procedure (LuCI offline upload)

**Path:** System → Device Online Management → **Software Packages** → Local Package Upload

Install **one `.apk` at a time**, wait for success, **dependency order**:

| Step | Package |
|------|---------|
| 1 | `libopen62541-1.3.6-r2.apk` |
| 2 | `libiec61850-1.6.2-r1.apk` (lab DUT; manifest listed 1.5.2-r1) |
| 3 | `iec61850-mmsd-1.0.0-r3.apk` |
| 4 | `iec61850-proto-tespro-combined-1.0.0-r6.apk` |

**Common errors:**

- `iec61850-mmsd (no such package)` → install steps 1–3 before step 4.
- `Unable to lock database` → wait 60 s; remove stale lock via SSH; one upload at a time.

**Supplier folder (PC):** WeChat export `TG500_iec61850_apk_install` (see manifest).

---

## SSH verification (2026-09-30) — **PASS**

```text
apk list -I | grep -E '61850|open62541'
  libopen62541-1.3.6-r2 [installed]
  libiec61850-1.6.2-r1 [installed]
  iec61850-mmsd-1.0.0-r3 [installed]
  iec61850-proto-tespro-combined-1.0.0-r6 [installed]

Processes:
  /usr/sbin/iec61850-mmsd
  /usr/sbin/iec61850d

Init:
  /etc/init.d/iec61850-mmsd  → running
  /etc/init.d/iec61850service → running
```

**Repeat check:**

```powershell
$env:CCLI_TG544_PW = "<set locally>"
.\lab\tg544-openwrt\check-tespro-61850-remote.ps1
```

---

## Gate status

| ID | Check | Result |
|----|-------|--------|
| P4-00a | Four apk packages installed | **PASS** 2026-09-30 |
| P4-00b | `iec61850-mmsd` + `iec61850d` running | **PASS** 2026-09-30 |
| P4-00c | LuCI service page loads + **Running** | **OPEN** (manual confirm) |
| P4-00d | Plant device configured (LAN3 IED `:102`) | **OPEN** |
| P4-00e | `ccli` + TesPro coexistence (V-005) | **OPEN** (`ccli` not running at verify) |

---

## Next (optional before 104)

1. LuCI → **Services → IEC 61850 Protocol** → confirm **Running**.
2. **Add Device** — IED Host on plant LAN (`192.168.30.x`), MMS port **102**, upload **plant ICD** (not `lab_tg544_eth_a.cid`).
3. When resuming DSO work: deploy/start **`ccli`** on `192.168.10.1:3782`; re-run coexistence check.

---

## Traceability

| REQ / ID | Link |
|----------|------|
| REQ-VEND-008 | Vendor 61850 documented — collector role |
| V-005 | Coexistence vendor 61850 + `ccli` — partial (install only) |
| `ccli-mms-server-vs-tespro-collector` | Architecture split |
