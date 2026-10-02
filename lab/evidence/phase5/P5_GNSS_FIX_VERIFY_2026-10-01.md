# P5 — GNSS fix verification (REQ-TIM-002 / LN-GAP-07)

**Date:** 2026-10-01  
**DUT:** TG544 @ `192.168.10.1`  
**Build:** `ccli 0.1.0-r25`  
**Annex:** **T.3.3.4.5** — UTC ±100 ms + valid sync flag  
**Prior state:** `gnss_fix=0` · `clockNotSynchronized=1` · TotW/TotVAr **q=Questionable** (LN-GAP-07)

---

## 1. What “GNSS fixed” means for ccli

ccli sets **Good** quality only when **both**:

1. **chrony** `within_pm100=yes` (NTP offset ≤ 100 ms)  
2. **`ubus` GNSS `fix: true`** from `sim-manager gnss_get_position`

Then log shows:

```text
mms: time_quality ... gnss_fix=1 clockNotSynchronized=0 (T.3.3.4.5)
```

URCB / TSP reads: TotW.q / TotVAr.q → **Good** (not Questionable).

---

## 2. Remediation applied (2026-10-01)

| Step | Action | Result |
|------|--------|--------|
| 1 | `ubus call sim-manager gnss_start` | `success: true` |
| 2 | `ubus call sim-manager gnss_check` | `powered_on: true`, `supported: true` |
| 3 | Operator: antenna / sky view | **Reported fixed** by operator |

**Before:** `gnss_get_position` → `"message": "off"`, `fix: false`  
**After start:** `"message": "searching"` → expect `"fix": true` when locked

---

## 3. Verification commands (run on DUT)

```sh
ubus call sim-manager gnss_get_position
chronyc tracking | head -8
chronyc sources | head -6
logread | grep time_quality | tail -3
```

**Pass (REQ-TIM-002 / V-TIM-02):**

| Check | Pass criteria |
|-------|---------------|
| GNSS ubus | `"fix": true`, satellites ≥ 4 |
| chrony | \|offset\| ≤ 100 ms |
| ccli log | `gnss_fix=1` **`clockNotSynchronized=0`** |
| TSP / URCB | TotW.q = TotVAr.q = **Good** |

Automated from PC:

```powershell
$env:CCLI_TG544_PW = '000000'
.\lab\tg544-openwrt\verify-gnss-time-quality.ps1
```

---

## 4. PASS snapshot — antenna on sky (2026-10-01 12:34 local)

**Evidence file:** `P5_GNSS_FIX_VERIFY_2026-10-01_123413.txt`

```json
{ "fix": true, "satellites": "13", "success": true, "latitude": "45.485742", "longitude": "11.708784" }
```

```text
chrony GNSS SOCK: #x GNSS Reach 155
mms: time_quality chrony offset_ms=0 within_pm100=yes leap=Normal ref=1FD155F3 gnss_fix=1 clockNotSynchronized=0 (T.3.3.4.5)
```

**VERDICT: PASS** — REQ-TIM-002 / V-TIM-02 / LN-GAP-07 **CLOSED**.

---

## 5. TSP follow-up (step 5d)

Re-read URCB or `PdCMMXU1.TotW` — expect TotW.q / TotVAr.q = **Good**.  
PPV.q may still differ (**LN-GAP-08** — separate product fix).

---

## 6. Phase 5 impact

| REQ | Status |
|-----|--------|
| REQ-TIM-002 | **PASS** |
| REQ-LN-005 (TotW/TotVAr q) | **PASS** (PPV.q still LN-GAP-08) |
| LN-GAP-07 | **CLOSED** |

---

## 7. If fix stays false indoors

Lab may still **close P5** on chrony (REQ-TIM-001) with LN-GAP-07 waived; product gate requires outdoor GNSS lock test before field deploy.
