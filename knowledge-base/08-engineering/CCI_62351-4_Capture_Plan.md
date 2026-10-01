# IEC 62351-4 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-003  
**Revision:** 1.7  
**Date:** 2026-08-12  
**RAG source_id:** `ccli-62351-4-capture-plan` → `ccli-62351-4-extract`  
**Normative basis:** IEC 62351-4:2018+AMD1:2020 — *Profiles including MMS and derivatives*  
**Capture status:** **HAVE (C1 P0)** — Batches A–G complete  
**Parent:** `ccli-62351-1-extract` · `ccli-62443-zones-extract` · `ccli-62443-4-2-extract`  
**Programme link:** K6.5 · KR-001 · **C1 Eth_A**

---

## Summary

**62351-4:2018** is the **normative MMS / application-layer security** profile for **IEC 61850** (and ICCP/TASE.2). It extends the 2007 TS with:

- **Transport security (T-security):** TLS — defers to **62351-3** (§6)
- **A-security-profile:** legacy OSI/MMS association auth (§11) — retained for compatibility
- **E2E application security:** handshake + data-transfer integrity/auth/encryption (§12–15) — **primary CCLI target**
- **Compatibility vs native modes** (§4.3–4.5)

**TG-524 C1 stack:** **62351-3 TLS** + **62351-4 E2E** on **61850 MMS** over TCP/IP.

**Regulatory hook:** CEI Allegato O/T — E2E security per **62351-4** (association + data-transfer).

**Document length:** ~102 pages + annexes (2018 ed.; consolidated CSV includes AMD1:2020).

---

## ToC (public preview — confirm from your PDF)

| § | Title | Page |
|---|-------|------|
| — | Foreword | 8 |
| **1** | Scope | 10 |
| **2** | Normative references | 11 |
| **3** | Terms, definitions, abbreviated terms | 12–16 |
| **4** | Security issues addressed | 17–21 |
| **5** | Specific requirements (ICCP, **61850**) | 21–22 |
| **6** | **Transport Security (TLS)** | 22–26 |
| **7** | Application layer security overview *(inf.)* | 26–28 |
| **8** | Cryptographic algorithms | 28–31 |
| **9** | Object identifier allocation | 32 |
| **10** | General OSI upper layer requirements | 32–36 |
| **11** | **A-security-profile** *(normative)* | 37–40 |
| **12** | E2E application security model *(inf.)* | 41–42 |
| **13** | **E2E application security** *(normative)* | 43–55 |
| **14** | E2E security error handling | 56–60 |
| **15** | E2E in **OSI** operational environment | 61–67 |
| **16** | E2E in XMPP environment | 67–71 |
| **17** | Conformance / PICS | 71–73 |
| **Annex A–E** | ASN.1 / XSD formal specs | 75+ |

**Key tables:** Table 1 security combinations (p. 19); Table 2–3 cipher suites (pp. 25–26); Table 4 SecPDU↔ACSE (p. 62).

**Figures:** Fig 1 application/transport profiles (p. 18).

---

## Capture batches — TG-524 C1 (P0)

Priority: **§5.2 (61850)** → **§6 TLS** → **§13 E2E** → **§17 conformance** → A-profile for legacy interop.

### Batch A — Foundation — **COMPLETE**

```
8–11, 18–21   ✓ Foreword · §1 Scope · §2 refs (partial) · §4 Fig 1 · Table 1 · modes/threats · §5.1 start
```

Gap: **pp. 12–16** (§3 terms) — optional add-on before Batch B.

### Batch B — 61850 + TLS transport — **COMPLETE** *(§6.2.1–7 gap pp. 22–23)*

```
22–27   ✓ §5.2 61850 contexts · §6.2.8–6.3.4 · Fig 2 ports · Tables 2–3 · §7.1 start
```

### Batch C — Crypto + A-profile — **COMPLETE** *(§10 pp. 33–36 gap)*

```
29–32, 37–41   ✓ §8 algorithms · §9 OID · §11 A-profile · MMS-Authentication-value · §12.1 preview
```

### Batch D — E2E normative core — **COMPLETE**

```
41–57   ✓ §12 Figs 5–8 · §13.1 handshake/abort/release · §13.3 ClearToken1/2/3 · §13.4 · §14.2 start
```

### Batch E — OSI mapping + conformance — **COMPLETE**

```
58–68, 75–78   ✓ §14 diagnostics/checks · §15 Table 4 ACSE mapping · §17 Tables 6–11 PICS
```

### Batch F — Annex G PKI — **COMPLETE**

```
103–106   ✓ Annex G end-entity cert · dual TLS/E2E certs · keyUsage · G.6.2 OSI subject OID
```

### Batch G — §13.2 data transfer — **COMPLETE**

```
49   ✓ §13.2 ClearTransfer · EncrTransfer · tbp/auth ICV rules
```

**C1 P0 capture track: COMPLETE.** Optional: §3 · §6.2.1–7 · §10 · §16 XMPP.

## CCLI mapping template

| 62351-4 concept | 62443 / zones | CCLI implementation |
|-----------------|---------------|---------------------|
| §6 TLS + 62351-3 | CR 3.1(1), 4.1 | OpenWrt TLS 1.2+ on **C1** |
| §13 E2E handshake | CR 1.8 PKI, 1.9 | libiec61850 + cert store (TPM) |
| §13 data-transfer auth | CR 3.1, 4.3 | MMS peer integrity on controls |
| Compatibility mode | Phased deploy | Non-secure MMS fallback policy |
| §17 PICS | Lab quote | Declare SL/security level |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| K6.5 | **PART→HAVE** when libiec61850 native E2E eval passes on C1 |
| 62443 CR 1.8 | PKI design cites 62351-4 §6 + §13 + **Annex G** |
| Allegato O/T | E2E association + data-transfer evidence documented |
| Lab | libiec61850 TLS + 62351-4 interop test plan |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-11 | Capture plan from public ToC; handoff from 62351-1 rev 1.2 |
| **1.1** | 2026-08-12 | **Batch A** pp. 8–11, 18–21 captured |
| **1.2** | 2026-08-12 | **Batch B** pp. 22–27: §5.2 · §6 · Tables 2–3 |
| **1.3** | 2026-08-12 | **Batch C** pp. 29–32, 37–41: §8 · §11 A-profile |
| **1.4** | 2026-08-12 | **Batch D** pp. 41–57: §12–13 E2E · ClearTokens |
| **1.5** | 2026-08-12 | **Batch E** pp. 58–68, 75–78: §14–15 · §17 PICS |
| **1.6** | 2026-08-12 | **Batch F** pp. 103–106: **Annex G** E2E cert profile |
| **1.7** | 2026-08-12 | **Batch G** p. 49: **§13.2** data transfer — **C1 P0 complete** |
