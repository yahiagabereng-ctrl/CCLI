# Phase 4 close — Operator 104 (Eth_B)

**Date:** 2026-09-30  
**DUT:** TG544 · `ccli-0.1.0-r18` · `lab_tr400_phase4_eth_b.yaml`  
**Prior:** Phase 3 **CLOSED** (lab) 2026-09-25

---

## Verdict

**Phase 4 lab: CLOSED** (104 gates + documented operator role)

| ID | Result | Evidence |
|----|--------|----------|
| P4-00 | **PART** | Apk install PASS; LuCI/plant/coexistence deferred |
| P4-01 | **PASS** | `P4_01_ETH_B_ONLY.txt` |
| P4-02 | **PASS** | `P4_02_104_SESSION.txt` |
| P4-03 | **PASS** | `P4_03_104_TLS.pcapng` |
| P4-04 | **PASS** | `P4_04_104_FALLBACK.txt` |
| P4-05 | **WAIVED** | Monitor-only OA — `P4_05_OPERATOR_ROLE_WAIVE.md` |

**Exit rule:** P4-01…03 PASS (104 in scope) — **met**. P4-05 closed by waiver.

---

## Operatore Abilitato — role (summary)

See **`P4_OPERATOR_ROLE.md`**.

- **Eth_B:** 104 TLS **monitor/supervision** only.
- **Eth_A:** DSO MMS **commands** (`Wlim`, `WSd`) — Phase 3; unchanged.
- **Annex T:** DSO only. **Annex O §9–11:** operator interface + logging rules.

---

## Deploy snapshot

| Item | Value |
|------|--------|
| 104 bind | `192.168.1.130:2404` tls=on |
| Fallback | `iec104.comms_loss_fallback_s: 60` |
| PC lab | `192.168.1.183` LAN2 |
| Manifest | `CCLI_DEPLOY_0.1.0-r18_2026-09-30_142739.md` |

---

## Residuals (non-blocking)

| Item | Owner |
|------|--------|
| P4-00 LuCI Running / LAN3 plant / V-005 coexistence | Lab / vendor |
| Operator **command** ASDUs + O.14 audit on 104 | Future if Operating Rule requires |
| O.10.3.1 MSD on Eth_B | Commercial / programme |

---

## Next programme phase

**Phase 5** — TotVAr / O.9.1 reactive on **Eth_A MMS** (`P5-M` / `P5-R`).
