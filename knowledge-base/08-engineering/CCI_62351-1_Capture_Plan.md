# IEC TS 62351-1 — Capture Plan (CCLI / TG-524)

**Document ID:** CCLI-SEC-62351-001  
**Revision:** 1.2  
**Date:** 2026-08-11  
**RAG source_id:** `ccli-62351-1-capture-plan` · Batch A/B/C/D → `ccli-62351-1-extract`  
**Normative basis:** IEC TS 62351-1:2007(E) — *Communication network and system security — Introduction to security issues*  
**Capture status:** **HAVE** (intro) — pp. 4, 6–7, 9–23, 24–34 captured; bibliography p. 35 optional  
**Next track:** **62351-4:2018** — see `ccli-62351-4-capture-plan`  
**Parent:** `ccli-62443-zones-extract` · `ccli-62443-3-3-extract` · `ccli-62443-4-2-extract`  
**Programme link:** K6.5 · KR-001

---

## Summary

**62351-1** is the **series introduction** for IEC **62351** power-system communications security. It maps threats, countermeasures, and TC 57 protocol parts (**62351-3 … -7**) but does **not** define TLS/MMS profiles — those are in **62351-3/4/5/6**.

**TG-524 CCI scope:** unblock **CR 1.8 PKI**, **CR 3.1(1)**, **CR 4.1** via protocol-layer security on conduits **C1** (61850 MMS → **62351-4**) and **C2** (60870-5-104 → **62351-3/5**).

**Document length:** ~35 pages + bibliography (ToC).

---

## ToC (from screenshots)

| § | Title | Page |
|---|-------|------|
| — | Foreword | 4 |
| — | Committee draft / maintenance | 5 |
| **1** | Scope and object | 6 |
| **2** | Normative references | 7 |
| **3** | Terms (→ 62351-2) | 7 |
| **4** | Background | 7–8 |
| **5** | Security issues for 62351 series | 9–23 |
| **6** | Overview of 62351 series | 24–**34** |
| **7** | Conclusions | 34–35 *(ToC; p. 34 body = §6.11.3 tail)* |
| — | Bibliography | 35 |

**Figures:** 1–7 (pp. 14–18); **Fig 8** process cycle (p. 22); **Fig 9–12** (pp. 26–33).

---

## Capture batches — TG-524 (P0)

### Batch A — Foundation — **COMPLETE**

```
4, 6–7   ✓ Foreword · §1 Scope/Object · §2 refs · §3→62351-2 · §4.1 start
```

Optional: p. **5** (committee metadata).

### Batch B — §5 Security framework — **COMPLETE**

```
9–23   ✓ §5.1–5.8 threats · CIA+N · Figs 1–8 · risk · power-ops constraints · 5-step process
```

### Batch C — §6 Series map — **COMPLETE**

```
24–33   ✓ Fig 9 part correlation · 62351-3/4/5/6/7 summaries · Tables 1–2 · Figs 10–12
```

### Batch D — §6.11.3 NSM tail — **COMPLETE**

```
34   ✓ §6.11.3 a items 12–16 · b network control · c IED monitor · d IED control
```

### Batch E — Close *(optional)*

```
35   ← §7 Conclusions · Bibliography
```

---

## Next normative parts — **62351-4 ACTIVE**

| Part | Conduit | Priority |
|------|---------|----------|
| **62351-4** | C1 MMS / 61850 | **P0** |
| **62351-3** | TCP/TLS base | **P0** |
| **62351-5** | 60870-5 / 104 | **P0** |
| **62351-6** | 61850 profiles / GOOSE | P1 |
| **62351-7** | NSM monitoring | P1 |
| **62351-2** | Glossary | P2 |

---

## CCLI mapping template

| 62351-1 concept | 62443 / zones hook | CCLI control |
|-----------------|-------------------|--------------|
| Authentication > encryption (§5.8) | FR 1 IAC | MMS peer auth before confidentiality |
| VPN insufficient alone (§5.3.5) | CR 5.2 + 62351-4 | App-layer + firewall |
| Physical / electronic / security domain | 62443-3-2 zones | Z0–Z5, C1–C5 |
| Key mgmt / cert revocation (§5.6.2) | CR 1.8 PKI | TPM + offline ceremony (K6.3) |
| IEC 62351-3…-6 on wire | C1/C2 | K6.5 implementation |

---

## Verification

| Item | Pass criteria |
|------|---------------|
| K6.5 | Moves to **PART** when 62351-1 + 62351-4 SL rows captured |
| 62443 trace | Every C1/C2 62351 target in zones doc has corpus row |
| Lab quote | 62351-4 cited for MMS-TLS evidence |

---

## Revision history

| Rev | Date | Change |
|-----|------|--------|
| **1.0** | 2026-08-11 | Batch A pp. 4, 6–7 + Batch B pp. 9–23 |
| **1.1** | 2026-08-11 | **Batch C** pp. 24–33: §6 · Fig 9–12 · Tables 1–2 |
| **1.2** | 2026-08-11 | **Batch D** p. 34 · handoff to **62351-4** |
