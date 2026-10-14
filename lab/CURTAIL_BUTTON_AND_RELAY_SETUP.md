# Curtail relay (DIO1) + permissive button (DIO2) — install and verify

**Document ID:** CCLI-LAB-IO-CURTAIL-001  
**Platform:** TesPro TG544 / TR500 · `ubus dido_v2` (GD32)  
**Canonical map:** [TR400_HW_IO_CONFIG.md](TR400_HW_IO_CONFIG.md) · smoke: [TR400_DIO_SMOKE_TEST.md](TR400_DIO_SMOKE_TEST.md)

Use this checklist when you **physically add** the curtail relay and permissive key, then bring software in line before PF2 or **Wlim → DIO1** cert traces.

---

## 1. What goes where

| Item | Terminal | ubus | Role |
|------|----------|------|------|
| **Relay K1 (curtail)** | **DIO1** | ch **1** DO | PF2 / O.9.2.2 curtail output |
| **Button / key (NO)** | **DIO2** | ch **2** DI | Permissive gate — relay only energises when key is **closed** |

**Logic:** `DIO1 ON ⇔ PF2 commanded ∧ permissive OK ∧ ¬Annex M trip (DIO3)`.

---

## 2. Parts (lab bench)

| Part | Typical rating | Notes |
|------|----------------|-------|
| Relay coil | **SCH-32F-1A-12SQ** or equivalent | **12 V DC** coil, **~38 mA** |
| Flyback diode | 1N4007 (or relay module built-in) | Cathode toward **+12 V** side of coil |
| PSU | **12 V DC** | Common **GND** with TG544 |
| Permissive | **12 V NO pushbutton / key** | Not the Annex M trip relay |

**Do not** treat DIO1 as **250 V / 3 A** plant output — it is an **~12 V open-drain** driver. See [P6-06_IO_VOLTAGE_GAP.md](evidence/phase6/P6-06_IO_VOLTAGE_GAP.md).

---

## 3. Wiring

### 3.1 Relay on DIO1 (curtail)

```text
+12 V ──► coil (+) ──► coil (−) ──► DIO1 only
PSU (−) ──► TG544 GND
Flyback diode across coil (cathode at +12 V)
```

| ubus `set_relay` ch1 | DIO1 vs GND | Relay |
|----------------------|-------------|-------|
| `value: 0` | ~1.5 V | OFF (safe) |
| `value: 1` | ~0 V | ON (energised) |

### 3.2 Button on DIO2 (permissive)

```text
+12 V ──► [ key NO ] ──► DIO2
PSU (−) ──► TG544 GND
```

| Key | `gd32.read_di` → **`di[1]`** | CCLI |
|-----|------------------------------|------|
| Open | **0** | permissive BLOCKED |
| Closed | **1** | permissive OK |

Bench shortcut: jumper **DIO2 → GND** reads **1** (same polarity as closed key).

Leave **DIO3/DIO4** unconnected unless you run Annex M trip mock ([P6-03](evidence/phase6/P6-03_ANNEX_M_INHIBIT.md)).

---

## 4. One-time software (on TG544)

SSH to the DUT, then either run the bundled script via plink (§6) or:

```bash
# Persist DIO2 as DI (ch1 DO is usually default)
uci set didoservice_v2.channel_2.mode=di
uci commit didoservice_v2
/etc/init.d/didoservice_v2 restart

ubus call dido_v2 set_mode '{"channel":2,"mode":"DI"}'
ubus call dido_v2 gd32.set_mode '{"channel":2,"mode":"DI"}'

# Full lab channel map (ch3/ch4 DO for Annex M / spare): see tr400-dio-factory.cmds
```

**GD32 must be online** before I/O works:

```bash
ubus call dido_v2 gd32.status '{}'
# expect "online": true
```

If **false**, see §7 in [TR400_HW_IO_CONFIG.md](TR400_HW_IO_CONFIG.md).

Deploy CCLI I/O policy from repo:

```bash
# PC: copy apps/ccli/config/lab_tr400.yaml → /etc/ccli/lab.yaml
# (or your phase4/cleartext variant — io block must match below)
grep -A20 '^io:' /etc/ccli/lab.yaml
```

Required **`io:`** keys for production-style gating:

```yaml
io:
  backend: ubus_dido_v2
  permissive_bypass: false
  do_curtail:
    gpio: 1
    active_high: false
  di_permissive:
    gpio: 2
    active_low: true
```

Restart CCLI after yaml change:

```bash
/etc/init.d/ccli restart
logread | tail -30 | grep -iE 'svc_io|permissive'
# expect: do_curtail=GPIO1(active_low) di_permissive=GPIO2(active_low)
# must NOT show permissive_bypass=on for cert-style runs
```

---

## 5. Verification (order)

### 5.1 Relay only (ubus)

```bash
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
ubus call dido_v2 set_relay '{"channel":1,"value":1}'   # click
ubus call dido_v2 set_relay '{"channel":1,"value":0}'
```

Or wiring check via CCLI:

```bash
/usr/sbin/ccli --gpio-blink 3 --config /etc/ccli/lab.yaml
```

### 5.2 Button only (ubus)

```bash
ubus call dido_v2 gd32.read_di '{}'
# open key  → di[1]==0
# closed key → di[1]==1
```

### 5.3 CCLI mapping

```bash
/usr/sbin/ccli --gpio-test --config /etc/ccli/lab.yaml
```

### 5.4 End-to-end (PF2 or Wlim)

1. **Close** permissive key.  
2. Raise plant **P** above threshold (Modbus slave or DSO **Wlim** on cert cfg).  
3. Confirm **DIO1 ON** / relay energised; open key → **DIO1 must drop** even if PF2 still wants curtail.

Evidence line for cert pack: `ubus call dido_v2 status` or `get_relay` after **Wlim** operate — see [DUT_LAB_FINALIZE_AND_CERT_PATH.md](conformance/DUT_LAB_FINALIZE_AND_CERT_PATH.md) §2.3.

---

## 6. From Windows (PuTTY plink)

```powershell
$env:CCLI_TG544_PW = "your-root-password"
cd C:\Yahia\projects\CCLI\lab

# Channel modes + gd32 status + relay blink + DI read
& "C:\Program Files\PuTTY\plink.exe" -ssh root@192.168.1.130 -pw $env:CCLI_TG544_PW `
  -hostkey "SHA256:4N84xpdiJRUiipactNbXLdVa2+A4eCCrUurVDrz3qoE" `
  -m tr400-permissive-and-relay-setup.cmds

# Non-interactive map check (relay toggles; you watch/listen)
.\tr400-io-verify-remote.ps1
```

Interactive walkthrough on device: `sh tr400-io-verify.sh`.

---

## 7. Pass criteria (sign-off)

| □ | Check |
|---|--------|
| □ | `gd32.status` → **online: true** |
| □ | ubus ch **1** toggles relay; ch **2** mode **DI** |
| □ | Key open → `di[1]=0`; closed → `di[1]=1` |
| □ | `/etc/ccli/lab.yaml` gpio **1** / **2**, **`permissive_bypass: false`** |
| □ | CCLI log **permissive=OK** only when key closed |
| □ | PF2 or **Wlim** actuation energises relay **only** with key closed |

---

## 8. Document history

| Rev | Date | Change |
|-----|------|--------|
| 1.0 | 2026-10-14 | Initial install runbook (relay + permissive button) |
