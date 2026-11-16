# EVD-SM-4 — Security training for SDL roles

**Document ID:** CCLI-EVD-SM-4-001  
**Revision:** 1.1  
**Date:** 2026-11-02  
**Status:** `STUB`  
**Phase:** 1 · SM Security management  
**Owner (HiTEKS):** HiTEKS HR / SDL lead  
**Consulted (TesPro):** C — platform team training evidence if requested under SM-10  
**Maps to:** **IEC 62443-4-1 SM-4** · **SDLA-SM-4** · Gap matrix §3  
**Company broadcast (essentials):** [`../62443/CCI_62443_ESSENTIAL_SECURITY_TRAINING.md`](../62443/CCI_62443_ESSENTIAL_SECURITY_TRAINING.md)  
**Gap cell today:** **GAP**  
**Evidence kind:** **ORG**  
**SDLA ID(s):** SDLA-SM-4  
**Depends on:** `EVD-SM-2-SDL-Roles`  
**Enrichment score:** **10/10** (playbook bar — not the same as Gap HAVE)  
**Playbook:** [`EVD-ENRICHMENT-PLAYBOOK.md`](EVD-ENRICHMENT-PLAYBOOK.md)

---

## 0. Enrichment scorecard

| Check | Result | Note |
|-------|--------|------|
| 1 Normative ask | PASS | one clear shall |
| 2 SDLA org+product | PASS | both auditor columns |
| 3 P pointer | PASS | SOP path or NONE — name |
| 4 R pointer | PASS | record path or NONE — name |
| 5 A pointer | PASS | repo/lab/vendor path |
| 6 Workstreams P/R/A | PASS | auto from §3 + §4.1–4.3 |
| 7 Ordered steps (Who/Output/Done) | PASS | table in §4.4 |
| 8 Dependencies | PASS | upstream packs |
| 9 TesPro vs HiTEKS | PASS | split duties |
| 10 Pass criteria | PASS | standard §7 checklist |

If score &lt; 10: deepen this pack (`Enrich EVD-SM-4-Training to 10/10`) before large execution effort.  
If score = 10 but Gap ≠ HAVE: **execute** §4 — enrichment is done; evidence is not.

---

## 1. Normative ask

People in SDL roles (SM-2) receive security training appropriate to their role.

## 2. What the auditor checks (SDLA-312)

| Column | Content |
|--------|---------|
| **Org / SDL validation** (SDLA cert) | Training process + curriculum mapped to SDL roles; records kept. |
| **Product / component validation** (SDA-C / product path) | Training completed for people who touched the CCI release under assessment. |

## 3. Evidence boxes (P / R / A)

| Box | Required | Pointer today | Box status |
|-----|----------|---------------|------------|
| **P** Procedure | Controlled SOP | [`../62443/templates/SM-4_Training.md`](../62443/templates/SM-4_Training.md) · Practice 1 SM-4 | **HAVE** |
| **R** Record | Proof on claimed release | NONE — attendance / completion records | MISS |
| **A** Artefact | Engineering / product output | N/A in repo — no code substitute | PART |

## 4. Steps to cover the GAP

### 4.1 Close **P** Procedure — today `MISS`

| Action | Detail |
|--------|--------|
| Target SOP | `62443/templates/SM-4_Training.md` — fill attendance |
| Owner | HiTEKS HR / SDL lead |
| Minimum content | Map every SDLA org-validation bullet in §2 into a numbered SOP clause |
| Done when | SOP has doc ID, revision, approver signature/date; linked from §3 |

### 4.2 Close **R** Record — today `MISS`

| Action | Detail |
|--------|--------|
| Target record | NONE — attendance / completion records |
| Store under | `lab/evidence/EVD-SM-4-Training/records/` |
| Every release / event | Complete §5 checklist; keep immutable copy |
| Done when | At least one filled record for a named CCI version/EUT |

### 4.3 Close **A** Artefact — today `PART`

| Action | Detail |
|--------|--------|
| Target artefact | N/A in repo — no code substitute |
| Kind hint | **ORG** — ORG≈docs · CODE≈repo/CI/test · MIX≈both |
| Done when | Auditor can open path and match §2 product-validation column |


### 4.4 Ordered execution (Who · Action · Output · Done when)

| # | Who | Action | Output path | Done when |
|---|-----|--------|-------------|-----------|
| 1 | HiTEKS SDL | From SM-2 RACI, list every named role. | `lab/evidence/EVD-SM-4-Training/` | Artefact exists and linked in §3 |
| 2 | HiTEKS SDL | Define minimum training (secure coding for SI roles; threat modeling for SR/SD; PSIRT for DM). | `lab/evidence/EVD-SM-4-Training/` | Artefact exists and linked in §3 |
| 3 | HiTEKS SDL | Deliver or record equivalent prior training; file certificates/attendance. | `lab/evidence/EVD-SM-4-Training/records/` | Artefact exists and linked in §3 |
| 4 | HiTEKS SDL | Re-train on major process change; keep annual refresher policy. | `lab/evidence/EVD-SM-4-Training/procedure.md (create)` | Artefact exists and linked in §3 |
| 5 | HiTEKS SDL | Gap → HAVE when records exist for current release team. | `lab/evidence/EVD-SM-4-Training/records/` | Gap matrix cell = HAVE/PART updated |

## 5. Record checklist

- [ ] Date / product version / EUT serial
- [ ] Executor / approver
- [ ] Attachments (logs, hashes, minutes)
- [ ] Residual risk decision if applicable
- [ ] Files stored under lab/evidence/EVD-SM-4-Training/

## 6. Artefacts

| Artefact | Location | Role |
|----------|----------|------|
| — | — | Fill when known |

## 7. Pass criteria (move Status → HAVE)

- [ ] Enrichment score 10/10 **or** residual FAIL accepted with note
- [ ] P approved (owner + date)
- [ ] R filed for named release / event under `lab/evidence/EVD-SM-4-Training/`
- [ ] A verifies (hash / test ID / CB excerpt)
- [ ] Gap matrix row updated
- [ ] This pack Status set to `HAVE` (or `PART` with residual in §9)

## 8. TesPro vs HiTEKS

| Party | Duty |
|-------|------|
| TesPro | May supply own training evidence for platform engineers. |
| HiTEKS | Train and record HiTEKS SDL roles (R). |

## 9. Open gaps / residual

| Missing | Blocks HAVE? | Owner |
|---------|--------------|-------|
| Any §3 box still MISS/GAP | Yes | HiTEKS HR / SDL lead |
| Enrichment score &lt; 10 | Blocks efficient execution | Docs / agent deepen pack |

---

*Generator: `scripts/enrich-evidence-packs.py`. Records live under `lab/evidence/` so re-runs do not wipe proof. Enrichment bar: `EVD-ENRICHMENT-PLAYBOOK.md`.*
