# IEC 62351-9 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-004  
**Revision:** 1.4  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-9-capture-plan` → `ccli-62351-9-extract`  
**Normative basis:** IEC 62351-9:2023 — *Cyber security key management for power system equipment*  
**Capture status:** **HAVE** — P0 + **§8 (P2)** complete; pp. **82–90** partial only  
**Parent:** `ccli-62351-3-extract` · `ccli-62351-4-extract` · `ccli-62443-4-2-extract`  
**Programme link:** K6.3 · **CR 1.8 PKI** · K6.5 dual cert (Annex G)

---

## Summary

**62351-9:2023** defines **PKI lifecycle** for power-system equipment: RA/CA, **IDevID/LDevID** onboarding, enrolment protocols (**SCEP**, **EST**, **CMP**, **CMC**, **TAMP**), and revocation (**CRL**, **OCSP**, **SCVP**). Symmetric **group keys** (GDOI/GOOSE/SV) are **P2** for C1 MMS.

**TG-524:** Closes **K6.3 ceremony SOP** gap; backs **62351-3 §5.6** OCSP and **62351-4 Annex G** dual TLS + E2E certs.

**Document length:** ~140 pp. (Annex D ~p. 137).

**Edition note:** Corpus previously cited **62351-9:2017** — active capture is **2023**.

---

## ToC (confirmed from PDF pp. 2–7)

| § | Title | Page |
|---|-------|------|
| **1** | Scope | 10 |
| **2** | Normative references | 11 |
| **3** | Terms, definitions, abbreviations | 12–17 |
| **4** | Security concepts | 19–20 |
| **5** | Key establishment and management techniques | 21–48 |
| **5.7–5.9** | PKI, cert management, revocation | **30–47** |
| **6** | Key management (normative) | 49–50 |
| **7** | Asymmetric key management (normative) | 51–75 |
| **8** | Group-based key management (normative) | 75–102 |
| **9** | PICS | 102–104 |
| **Annex C** | Certificate enrolment (informative) | ~129–130 |
| **Annex D** | Security events → 62351-14 | ~131–137 |

---

## Capture batches

### Batch A — PKI architecture + revocation — **COMPLETE**

```
10–11   ✓ §1 Scope · §2 Normative references
30–34   ✓ §5.7 PKI/PMI · Fig 6–8 · §5.8.1–5.8.3 (IDevID/LDevID)
35      ✓ §5.8.3 ZTP (8572/8995/8520) · §5.8.4 Enrolment of an entity
36–41   ✓ §5.8.5–5.8.6 CSR · SCEP · EST · CMP · CMC (Figs 9–13)
42–47   ✓ §5.8.7 TAMP · §5.9 CRL/OCSP/SCVP · §5.9.4 recovery · §5.10 start
```

### Batch B — Normative asymmetric key mgmt — **COMPLETE**

```
51–58   ✓ §7.1–7.3 Tables 1–2 · key gen/protection · registration · EST/SCEP · TAMP
59–69   ✓ §7.4 cert verification · extensions · attribute certs · revocation status
70–72   ✓ §7.5–7.7 revocation · renewal · clock sync · §7.8 AVL start
103–104 ✓ §9 PICS · Tables 5–6 (Table 7–8 GDOI noted P2)
```

### Batch C — Ceremony SOP + security events — **COMPLETE**

```
49–50   ✓ §6.2 Handling of security events · §6.3–6.5 RNG/OIDs
129–130 ✓ Annex C.1–C.2 enrolment + renewal (70% lifetime)
131–137 ✓ Annex D Tables D.1–D.5 → 62351-14 event IDs
```

### Batch D — Lifecycle context *(optional P1)*

```
19–25   §4–§5.1–5.5 Key lifecycle policy
26–29   §5.6 DH/KDF (62351-4 §13 cross-ref)
48–49   §5.11 AVLs
105     Annex A relations
```

### Batch E — GDOI / GOOSE-SV / PTP — **COMPLETE** *(pp. 82–90 partial)*

```
35      ✓ §5.8.4 enrolment (Batch A supplement)
75      ✓ §7.8.6–8 AVL · §8.1 GDOI
76–81   ✓ §8.2 Table 3 · §8.3 IKEv1 main mode · §8.4.1
82–90   ~ §8.4.2–8.5.6 · Table 4 · ASN.1 selectors (corrupted/partial)
91–102  ✓ §8.5.7–8.7.4 TEK/PULL/PUSH/ops · §9 start
104     ✓ Tables 7–8 group PICS (in Batch B)
```

---

## CCLI mapping template

| 62351-9 concept | Conduit / artefact | 62443 / sibling |
|-----------------|---------------------|-----------------|
| §5.8 IDevID → LDevID onboarding | Factory + field commissioning | CR 1.8 |
| §5.8.6 **EST** / **SCEP** | OpenWrt cert enrolment | K6.3 SOP |
| §5.9 **OCSP** stapling / proxy | C1 substation controller pattern (Fig 16) | 62351-3 §5.6.4.4 |
| §5.9.4 revocation recovery | TPM re-enrolment after compromise | CR 1.8 |
| Dual cert (TLS + E2E) | Annex G.2 | 62351-4 extract |
| §7.7 clock sync | GNSS / NTP on TG-524 | 62351-4 10 min skew |
| Annex C state machines | K6.3 ceremony doc | **HAVE** — `ccli-k63-ceremony-sop` |
| Annex D events | **62351-14** | CR 6.2 |
| §8 GDOI / PTP | 61850-9-3 GNSS time | P2 · not C1 MMS |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| Batch B §7 + §9 | Normative + PICS captured | **PASS** |
| Batch C §6 + Annex C/D | K6.3 SOP + 62351-14 events | **PASS** |
| Batch E §8 GDOI | pp. 75–102 captured | **PASS** *(82–90 partial)* |
| K6.3 written SOP | Derived from Annex C + §7.3 | **PASS** | `ccli-k63-ceremony-sop` |
| 62351-3/4 cross-ref | OCSP + dual cert cite 62351-9 | **PASS** |
| Lab PICS | Batch B §9 Tables 5–6 | **PASS** |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-12 | **Batch A** pp. 10–11, 30–47 captured; **p. 35** missing |
| **1.1** | 2026-08-12 | **Batch B** pp. 51–72, 103–104 captured |
| **1.2** | 2026-08-12 | **Batch C** pp. 49–50, 129–137 captured |
| **1.3** | 2026-08-12 | **p. 35** + **Batch E** pp. 75–102 (§8 GDOI) captured |
| **1.4** | 2026-08-12 | **K6.3** formal SOP — `CCI_K6.3_Key_Ceremony_SOP.md` |
