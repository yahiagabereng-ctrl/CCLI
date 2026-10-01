# Original CCI-Class Controller — Project Roadmap

- **Target:** CEI 0-16 Annex O/T/M compliant Central Plant Controller, flexible 100 kW–6 MW MT plant range
- **Scope:** Original hardware/firmware design (not a clone of any existing commercial product)
- **Prototype budget:** ~€1,000 (single unit, BOM + fab + assembly)
- **Timeline stance:** Balanced — solid working prototype in ~10–12 weeks; grid/cyber certification is a separate, longer track
- **Existing certification:** organization already holds **ATEX** (EN/IEC 60079) — reuse QMS, Notified Body relationship, and technical-file practice for the CEI 0-16 track.
- **Alternate track:** see companion brief **CCLI-CLONE-RE-001** (`CCI_Clone_RE_Option.pdf`) for the clone / reverse-engineering option, risks, and decision guide.
- ✅ **Already in place:** your company is **ATEX-certified**. Standard MT CCI cabinets are usually *non-hazardous area* — confirm per project whether Ex marking is required on the CCI itself.
- ⚠️ **Set expectations up front:** the hardware prototype is the *cheap and fast* part. Remaining certification for this product (IEC 62443, IEC 61850, IEC 62351, CEI 0-16 demo) is realistically **€45,000–110,000+** and 9–18 months. ATEX is not budgeted again here.
- **Near-term execution plan (Italy only, no fabrication):** see **CCLI-PLAN-3M-001** (`CCI_3Month_Italy_NoFab_Plan.md`) — 13-week kit/software + certification *prep* (not full cert).

---

## 1. System architecture (original design)

This block diagram reflects the *generic, publicly-documented functional interfaces* that CEI 0-16 Annexes O/T require of any CCI (dual DSO/Operator Ethernet separation, plant-side ports, serial link to the energy analyzer, time sync, DI/DO for status/command) — not any specific vendor's schematic or component selection. See `CCI_Architecture.png` (primary visual) and `CCI_Architecture.mermaid` (editable companion).

**Core functional blocks:**

| Block | Function | Notes / track |
|---|---|---|
| Design approach | Option A (default): original SoM + carrier from public CEI 0-16 interfaces | Primary track — independent schematic, layout, firmware |
| Design approach | Option B: clone via reverse engineering (AiLux-class reference) | See **CCLI-CLONE-RE-001** (`CCI_Clone_RE_Option.pdf`) — higher IP/legal risk; gated research spike only |
| Compute module (SoM) | Runs embedded Linux, IEC 61850 server, Modbus master, PF2 control logic | Option A: Toradex/Variscite/Compulab-class. See §2 |
| Ethernet interfaces (≥4 port managed switch or SoM+switch IC) | Eth_A → DSO; Eth_B → Operator; 2× plant ports | DSO/plant isolation required — two isolated PHYs/VLANs minimum |
| RS-485/RS-232 serial (×2, isolated) | Modbus RTU master to energy analyzer; IEC 60870-5-101 optional | Opto-isolated; map registers from datasheet |
| GNSS module | Time sync for timestamped measurements/events | u-blox-class module + antenna |
| Cellular modem (LTE Cat-1/Cat-4) | Remote config/monitoring, backup comms | Optional on rev A if DSO link is wired |
| Digital I/O | DI: 10–120 Vdc status; DO: dry contact curtailment relay | Critical PF2 path — re-implement on Option B, do not copy reference network |
| Secure element + secure boot | Root of trust, key storage, signed firmware | Option A: your key hierarchy; Option B: never reuse reference keys/boot chain |
| Local console | USB service port for commissioning/diagnostics | Outside certified control path |
| Power input | 12–24 Vdc wide-range isolated DC-DC | Box CCI UPS/battery context |

---

## 2. Compute module: SoM + custom carrier

**Decision (from our discussion): off-the-shelf industrial SoM, original carrier board design.**

Candidate SoM families to evaluate (pick based on Linux BSP maturity, industrial temp range, and 10-year longevity commitment — you need this for a utility-facing product):

- Toradex Verdin iMX8M Mini/Plus
- Variscite DART-MX8M-Mini / DART-MX8M-Plus
- Compulab CL-SOM-iMX8
- NXP i.MX8M Mini EVK-class reference designs

All of these ship maintained **Linux BSPs** (OpenWrt on lab gateway, or vendor SDK). Your application stays C/C++ on **TG-524** or Pi mock.

---

## 3. Prototype BOM target (single unit, ~€1,000)

| Item | Est. cost (EUR) | Notes |
|---|---|---|
| Industrial SoM (qty 1) | €60–120 | Toradex/Variscite-class, i.MX8M Mini tier |
| Carrier PCB fab (4-layer, small batch 5–10 pcs) | €50–150 | JLCPCB/PCBWay prototype tier |
| Assembly (self or light-touch SMT service) | €100–250 | BGA stays on the SoM module — carrier assembly is hand/reflow-feasible |
| Ethernet PHYs/magnetics/RJ45 (×4) | €15–25 | |
| RS-485/RS-232 isolated transceivers (×2) | €10–15 | |
| GNSS module + antenna | €25–40 | u-blox-class |
| LTE module + antenna + SIM holder | €25–40 | Quectel-class; optional on rev A |
| Secure element | €2–5 | e.g. Microchip ATECC608B / STSAFE-A110 class part |
| DI/DO conditioning (opto-isolators, relays) | €15–25 | |
| Power stage (wide-range isolated DC-DC) | €15–25 | |
| DIN rail hardware, connectors, terminal blocks, enclosure | €40–60 | |
| Passives, stencil, shipping, contingency | €80–150 | |
| **Total (single prototype)** | **~€440–900** | Leaves headroom within €1,000 for one respin's worth of parts |

This is a *component-cost* estimate for one unit at prototype quantities — not a manufactured unit cost at volume, which would drop significantly (likely €150–300/unit at 100+ qty once NRE is amortized).

---

## 4. Path to Gerbers

| # | Step | Activity (Altium Designer) |
|---|---|---|
| 1 | Architecture freeze | Lock interfaces, connector count, power budget before schematic work |
| 2 | Schematic capture | **Altium Designer** — build from datasheets; link SoM/PHY footprints early |
| 3 | Design review | Power sequencing, ESD/surge on field I/O, isolation barriers |
| 4 | PCB layout | SoM first, then Ethernet PHY/magnetics, RS-485, power; keep analog away from switching/RF |
| 5 | DFM check | Altium DRC + fab DFM portal (JLCPCB/PCBWay) against stackup rules |
| 6 | Fab outputs | Altium **Output Job**: Gerber, drill, pick-and-place, BOM (ActiveBOM) |
| 7 | Fab + assembly | 1–3 week prototype turnaround |
| 8 | Bring-up | Power-on, boot SoM, verify each interface before firmware integration |

---

## 5. Firmware architecture & language choice

- **OS/BSP:** **OpenWrt** on TesPro TG-524 (lab); cross-compile with vendor SDK. Raspberry Pi uses native Linux for mock.
- **Core control/protocol path (IEC 61850 server, Modbus master, PF2 curtailment logic, GOOSE handling):** C/C++. This is the code that will eventually face conformance and security audits — auditors and test labs are far more comfortable reviewing statically-typed, analyzable C/C++ than dynamic scripting languages, and real-time GOOSE timing requirements favor it anyway.
- **Config/commissioning tooling, local web console, diagnostics:** Python or a lightweight web stack is fine here — this code is outside the certified control-path boundary, so language choice is a productivity call, not a compliance one. Keep the boundary between "certified path" and "tooling" architecturally clean from day one — it makes scoping the IEC 62443-4-2 assessment much cheaper later.
- **IEC 61850 stack:** start on **[libiec61850](https://github.com/mz-automation/libiec61850)** (MZ Automation) for prototype/demo — **GPLv3** open-source core (MMS, GOOSE, SV, R-Session); **commercial licenses sold separately** by MZ Automation for shipping a closed product. Treat OSS as *free to prototype, pay to ship* — budget the commercial license from day one. Evaluate commercial stacks (SISCO, Triangle MicroWorks) before paid conformance testing — see §7.
- **Security baseline to build in from day one** (cheap now, expensive to retrofit later): secure boot chain, signed firmware images, TLS/DTLS for IEC 62351, key storage in the secure element, basic RBAC on the local console, audit logging. IEC 62443-4-1 specifically audits *how* you built and maintain the product (your SDLC), not just what features it has — so start documenting your development process now, not after the fact.

---

## 6. Timeline — Phase 1: Hardware + firmware prototype (~10–12 weeks, balanced pace)

| Weeks | Milestone |
|---|---|
| 1–2 | Architecture freeze, SoM selection finalized, schematic capture starts |
| 2–4 | Schematic complete + design review, long-lead parts ordered (SoM, LTE/GNSS modules) |
| 4–6 | PCB layout, DFM check, Gerbers to fab |
| 6–8 | Fab + assembly turnaround |
| 8–10 | Hardware bring-up: power, boot, verify each interface (Ethernet ×4, RS-485 ×2, GNSS, LTE, DI/DO) |
| 10–12 | Firmware integration: libiec61850 server running, Modbus master polling a real analyzer, DO-driven curtailment stub, local console reachable |

**Exit criteria for this phase:** a DIN-rail prototype that boots reliably, exposes an IEC 61850 MMS server over Eth_A, polls a Modbus energy analyzer over RS-485, timestamps data via GNSS, and can toggle a DO relay from a simulated DSO command. This is a credible demo/investor/pilot-partner artifact — it is *not* yet field-deployable at a real MT POC.

---

## 7. Phase 2: Certification path (separate track, 9–18 months)

**Scope note:** ATEX (EN/IEC 60079) is **already held** — not a new line item. Table covers remaining CEI/grid/cyber work.

| Workstream | What it involves | Rough cost | Duration |
|---|---|---|---|
| **ATEX (EN/IEC 60079)** | **Already certified** — reuse NB/QMS/technical-file process | Sunk / reuse | — |
| IEC 62443-4-1 | Secure SDLC process audit (partial overlap with ATEX QMS) | €10,000–30,000 | 2–5 months |
| IEC 62443-4-2 | Device security lab test (SL1 min, target SL2) | €15,000–40,000 | 2–4 months |
| IEC 61850 conformance | MMS/GOOSE/reporting lab test | €10,000–25,000 | 1–3 months |
| IEC 62351 | Cybersecurity protocol testing | Overlaps 4-2 | 3–6 months |
| CEI 0-16 Annex O/T + DSO demo | PF2/observability validation | Integrator time | 1–3 months |

**All-in for remaining CEI-class CCI certification: roughly €45,000–110,000 over 9–18 months** (ATEX excluded).

---

## 8. Team/skills needed

- Embedded hardware engineer (schematic + layout, comfortable with SoM carrier design)
- Embedded Linux/firmware engineer (OpenWrt, C/C++, protocol stacks)
- Someone who can own the IEC 62443-4-1 process documentation — leverage existing ATEX technical-file and NB workflow
- Accredited test lab relationship early — your ATEX Notified Body may partner on 62443/61850 scoping

---

## 9. To do

1. Confirm SoM family choice (Toradex vs Variscite vs Compulab) based on your BSP/tooling comfort and long-term sourcing preference.
2. I can draft the interface list and pin budget (connector-by-connector) as a starting schematic outline — original to your design, not copied from any existing product.
3. Get a scoping quote from one IEC 62443/61850 test lab now, even before hardware exists — their capacity and requirements should shape your firmware architecture decisions, not the other way around.

Happy to start on the schematic-level interface/pin-budget document next, or go deeper on any single phase above.
