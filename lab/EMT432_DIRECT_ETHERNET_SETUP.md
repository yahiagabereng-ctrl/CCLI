# EMT432 — PC direct Ethernet + web portal + Rogowski registers

**Model:** `EMT432.V.FULL` (Rogowski) · bench IP **192.168.178.102**  
**Secrets:** PW Web / PW AP / PSW on device label — **do not commit to git**

---

## 1. PC network (Administrator)

Direct cable **PC ↔ meter RJ45** (or via LAN3 switch with DUT):

```cmd
lab\set_pc_lan3_plant_178.cmd
```

Sets plant NIC to **192.168.178.10 / 255.255.255.0** (edit adapter name in `.cmd` if not `Ethernet 2`).

Find meter if IP unknown:

```powershell
python lab\emt432_rogowski_config.py --scan --subnet 192.168.178
python lab\modbus_emt432_tcp_probe.py --scan --subnet 192.168.178
```

Set meter IP on display or web: **192.168.178.102**, mask **/24**, GW **192.168.178.1** (when DUT on LAN3).

**Ethernet @ 192.168.178.250 (factory-style RJ45):** static IP, **DHCP off** is OK. **Never set gateway = meter IP** (e.g. GW **192.168.178.250** breaks comms on many units).

| Path | Meter Ethernet IP | Gateway on meter |
|------|-------------------|------------------|
| PC direct cable (no DUT) | **192.168.178.250** | **empty / 0.0.0.0** |
| DUT LAN3 (TG-544 **192.168.178.1**) | **192.168.178.250** or **.102** | **192.168.178.1** |

Fix bad GW via **Wi‑Fi AP** → **`http://192.168.4.1`** if **`http://192.168.178.250`** does not open. PC plant NIC: **192.168.178.10/24**, **no gateway**.

---

## 2. Availability check

```powershell
ping 192.168.178.250
ping 192.168.178.102
python lab\modbus_emt432_tcp_probe.py --host 192.168.178.250
python lab\emt432_rogowski_config.py --host 192.168.178.250 --read
```

(Use **`.102`** if that is the address configured on Ethernet or Wi‑Fi STA.)

| Step | Pass |
|------|------|
| ping | Replies from `.102` |
| Modbus | `connect: OK`, `mounted_version` → **333mV/Rog** |
| Web | Browser opens meter UI (see §3) |

---

## 3. Web portal (label **PW Web**)

1. PC on **178.10**, meter at **178.102**.
2. Browser: **`http://192.168.178.102`** (try **`https://`** if HTTP redirects — see MI-ENG).
3. Login with **PW Web** from nameplate.
4. Set: **monofase**, Rogowski **I1**, associate **FRC-350** if offered (preferred vs raw Modbus).

Alternative: phone → meter **Wi‑Fi AP** → password **PW AP** → same configuration.

Touchscreen: menu PIN **PSW** on label.

---

## 4. Modbus register setup (if web/menu insufficient)

**EMT430_MR Rev2:**

| Register | Action |
|----------|--------|
| **40232** | Single phase + **bit 13 integrator ON** |
| **40235** | Rogowski sensitivity **[V/A]** — **0.0001** for FRC-350 **100 mV/kA**; **0.000085** for **85 mV/kA** |
| **40234** | Chronos TA profile — use **app/NFC** when possible |

Bench script (from repo root, pymodbus installed):

```powershell
python lab\emt432_rogowski_config.py --host 192.168.178.102 --read
python lab\emt432_rogowski_config.py --host 192.168.178.102 --apply-1p-rogowski --set-sensitivity-mvka 100
```

Re-read; with heater on, **P1** should be **kW** scale. Then deploy `lab_tr400_emt432_lan3_tcp_102.yaml` to DUT.

---

## 5. DUT path (after direct setup)

Reconnect plant LAN3: **DUT 178.1** · **meter .102** · **PC-A 178.10** · Modbus from DUT `lab.yaml`.
