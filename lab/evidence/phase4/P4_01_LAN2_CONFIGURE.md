# P4-01 prep — Configure LAN2 (Operator Eth_B)

**Date:** 2026-09-30  
**Gate:** P4-01 — operator traffic on **Eth_B only** (`192.168.1.130`)  
**As-built:** Phase 2 `P2_ANNEX_PORT_MAP.txt`

---

## Cable map

| PC port | TG544 port | Role | DUT IP | PC IP (lab) |
|---------|------------|------|--------|-------------|
| Ethernet | **LAN2** | Operator Eth_B | **192.168.1.130** | **192.168.1.183** (static) or DHCP .150–.199 |
| — | LAN1 | DSO Eth_A | 192.168.10.1 | 192.168.10.10 — use when testing MMS/TSP |
| — | LAN3 | Plant | 192.168.30.1 | — |

**Important:** One cable, one role. After moving to LAN2, `192.168.10.1` will not respond until you move back to LAN1.

---

## Step 1 — PC IP (Administrator)

```cmd
REM Right-click → Run as administrator
lab\set_pc_lan2_oa.cmd
```

Or PowerShell (elevated):

```powershell
netsh interface ip set address name="Ethernet" static 192.168.1.183 255.255.255.0 192.168.1.130
netsh interface ip set dns name="Ethernet" static 192.168.1.130
ping 192.168.1.130
```

**Alternative:** leave PC on DHCP; TG544 `lan_sec` serves **192.168.1.150–199** if UCI unchanged from Phase 2.

---

## Step 2 — Verify link + DUT

```powershell
$env:CCLI_TG544_PW = "root-password"
.\lab\tg544-openwrt\verify-lan2-eth-b.ps1
```

**PASS criteria (Step 0):**

- [ ] PC IPv4 on `192.168.1.0/24`
- [ ] Ping `192.168.1.130` OK
- [ ] SSH `root@192.168.1.130` OK (hostkey `SHA256:2hiPouwqC1oxP…`)
- [ ] DUT `lan2` shows `192.168.1.130/24`
- [ ] LuCI: `http://192.168.1.130/cgi-bin/luci`
- [ ] **No** `:2404` on `192.168.10.1` (after 104 implemented: listen **only** on `.1.130`)

---

## Step 3 — What stays on LAN1

| Service | Interface | Port |
|---------|-----------|------|
| `ccli` MMS (Annex T) | Eth_A `192.168.10.1` | **3782** TLS |
| TSP / IEDScout | PC on LAN1 | client → :3782 |

104 and TesPro plant poll (**LAN3** `:102`) are independent of LAN2 operator work.

---

## Step 4 — Phase 4 yaml (ready for coding)

`apps/ccli/config/lab_tr400_phase4_eth_b.yaml` — adds `iec104:` with `bind_address: 192.168.1.130`, port **2404** (`enabled: false` until adapter ships).

---

## Switch back to DSO (LAN1)

```cmd
lab\set_pc_lan1_dso.cmd
```

---

## Evidence files

| File | Purpose |
|------|---------|
| `P4_01_LAN2_VERIFY_*.txt` | Auto log from verify script |
| `P4_01_ETH_B_ONLY.txt` | After 104: netstat proof no listener on Eth_A |
