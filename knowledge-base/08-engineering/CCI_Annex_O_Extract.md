# CEI 0-16 Annex O — CCI Engineering Extract (English)

**Document ID:** CCLI-GRID-ANNEX-O-001  
**Revision:** 2.0  
**Date:** 2026-09-16  
**RAG source_id:** `cei-0-16-allegato-o`  
**Normative source (current law):** CEI 0-16 consolidated working edition **2025-12** (`knowledge-base/07-protocols/0-16consolidata.pdf`, 691 pp.) — **Allegato O** (normative), doc. pp. **473–513** / PDF pp. **480–520** (verified 2026-09-21; see `CCI_Annex_OTM_Verification.md`)  
**Official English working translation (2022-03 base only):** `Standards_pdf-20260918T124321Z-1-001/Standards_pdf/EstrattoAllegatoO.pdf` — CT 316, pub. **2024-12**, **50 pp.** — title *Annex O — DER Plant Controller* / *Controllore centrale di impianto*. **Does not include V5 (2025) / ARERA 385** — apply **§2.1 below** from consolidated Italian PDF.  
**Prior English baseline:** same CT 316 text as `EstrattoAllegatoO.pdf`; superseded for **Variant V5 (2025)** items in §2.1  
**Companion:** `CCI_Annex_T_Extract.md` · `CCI_CEI_0-16_Consolidated.md` · `CCI_ARERA_540_2021_Extract.md`  
**Programme link:** K7.1 · REQ-PF2-* · REQ-MET-* · REQ-IO-* (functional; no pin count)

---

## Summary

Annex O defines the **Central Plant Controller (CCI)** functional specification: PF1 observability, PF2 voltage/active-power control at the delivery point (POC/PdC), PF3 dispatch-market functions, control-loop timing, measurement accuracy, communication interfaces, data logger, and certification hooks.

**Corpus status:** **HAVE (English)** for core numerical requirements; **UPDATED** for **CEI 0-16 V5 (2025)** / **ARERA 385/2025/R/EEL** (see §2.1). Annex T covers IEC 61850 data model and cyber.

---

## 2.1 Variant V5 (2025) — ARERA 385/2025/R/EEL deltas *(English)*

Source: consolidated PDF preface (V5) + Allegato O clauses O.2, O.6, O.8, O.9, O.11, O.12 (Italian → English engineering summary).

| Topic | Prior baseline (2024 extract) | **V5 consolidated (2025-12)** |
|-------|------------------------------|-------------------------------|
| Regulatory driver | ARERA 36/2020/R/EEL, SOGL | **+ ARERA 385/2025/R/EEL** (Grid Code All. A.72 alignment, DER security) |
| PF2 active-power limitation on **DSO command** | PF2 class described as *optional* (ARERA timing TBD) | **Mandatory** for **wind and PV** plants with rated power **≥ 100 kW** (O.6 / O.9) |
| Aggregated P per source (O.8) | Required per generator thresholds | **Not mandatory** for wind/PV **100 kW ≤ P < 500 kW** where consumption is **auxiliaries only** |
| Measurement accuracy (O.13.2) | Class ≤ 0.2 % typical | For wind/PV **100–500 kW**, Grid Code **All. A.72** allows error **≤ 5 %** |
| Defence plan / Annex M | Referenced | **Annex M still applies ≥ 100 kW** until a later regulatory variant (385/2025 transitional note) |
| Priority O.11 | Teletrip + DSO limit mentioned | **Remote disconnect (teledistacco)** and **DSO active-power limitation** retain top priority below unit O/F regulation |
| Installation (O.12) | — | Example diagrams show **teledistacco predisposition** (CEI 0-16 §8.8.7.1) — physical connections to CCI |

**Applicability (O.2 — unchanged thresholds, clarified scope):**

- CCI mandatory for new MT connections when aggregated wind/PV per source **≥ 100 kW**, or dispatching participation, or **≥ 1000 kW** total (standard perimeter).
- When a CCI is installed, its functions apply to **all production units at the same delivery point (PdC)**, regardless of individual unit size.

**Not in this PDF:** product pin counts (5 DI / 3 DO) — those remain **AiLux product class**, not Annex O text.

---

1. Purpose and Scope of This Document
This document consolidates every mandatory numerical requirement stated in Annex O of CEI 0-16:2022-03 ("DER Plant Controller" / CCI) that the CCI AiLux firmware and hardware must respect: control-loop timing, measurement periodicity, measurement accuracy classes, control-function parameters/thresholds, communication interface specifications, data-logging minimums, and certification requirements.
Values are grouped by functional performance category as defined by Annex O:
PF1 — Mandatory: monitoring and data-exchange functions (always required).
PF2 — Voltage control and active power limitation at the POC: **mandatory PF2 sub-function** (DSO active-power limitation) for wind/PV **≥ 100 kW** per V5/ARERA 385/2025; other PF2 functions remain ARERA-scheduled optional unless mandated.
PF3 — Discretionary: plant management and Dispatching Services Market (MSD) participation.
Unless noted otherwise, every timing/accuracy value below is a hard normative requirement ("shall"), not a design suggestion.
2. Applicability Thresholds (Annex O, Clause O.2)
Parameter
Value / Condition
Minimum total rated power for mandatory CCI (new connections, "standard perimeter")
≥ 1000 kW (sum of production + storage units)
Plants participating in dispatching services
CCI required regardless of total power
Plants < 1 MW ("extended perimeter") / retrofits
Conditions to be established by ARERA (not yet fixed)
Source (Annex O): O.2 "Scope".
3. Control-Loop Timing Requirements
CCI implements two nested control rings: the fast ("plant control") ring and the slow ("control curve analysis") ring. Both have normatively fixed timing.
3.1 Fast Control Ring (Plant Control Ring)
Parameter
Requirement
Max settling time, active power (TsP)
60 s, for any size of internal active-power set-point change
Max settling time, reactive power (TsQ)
10 s, for any size of internal reactive-power set-point change
Settling-time definition
Time from instant of new set-point application to the instant the controlled quantity at POC (V, P, Q, etc.) stably enters a tolerance band of ±5% of the expected value
Measurement basis
MC200 — 200 ms fixed block interval measurements per IEC 61557-12 Annex B
Faster-than-required performance
Permitted (CCI may reach the expected value faster than the stated maxima)
MSD secondary power control (if participating)
Settling time must additionally allow the plant to follow the gradient prescribed by the TSO Grid Code
Source (Annex O): O.7.3.1 "Dynamics of the fast control ring"; O.7.4.
3.2 Slow Control Ring (Control Curve Analysis Ring)
Parameter
Requirement
Cycle time ΔT — adjustable range
10 s to 600 s (settable operating parameter)
Cycle time ΔT — default value
60 s, unless otherwise set by DSO in the "Operating Rule"
Reaction to external command changing control function/parameters
New parameters and control curves must be acquired and processed within ΔT of command reception
Minimum interval between change commands
Commands arriving at intervals shorter than ΔT are rejected
Measurement basis
MCΔT — measurements aggregated over the ΔT interval, per IEC 61557-12 Annex B
Voltage/current sampling
Voltage and current samples must be synchronous
Source (Annex O): O.7.3.2 "Dynamics of the slow control ring"; O.7.4.
3.3 External Set-Point Command Dynamics
Parameter
Requirement
Minimum interval between processed external set-point updates
3 s
Commands arriving faster than 3 s
Rejected
Source (Annex O): O.7.3.3 "External Set Point Dynamics".
4. Measurement & Data-Exchange Periodicity (Mandatory, PF1)
All periodic transmissions below must be synchronised to the 0/4/8/12/…/56-second marks of each minute (i.e., every 4 s, boundary-aligned).
Data / Function
Aggregation window
Transmission periodicity
Notes
P, Q, V at POC → DSO (power-flow monitoring)
4 s fixed block interval (IEC 61557-12)
Every 4 s, synchronous with :00,:04,:08…
Complete with timestamp and quality index (O.8.3)
P, Q at POC + aggregated/unit-level P → TSO via DSO (network observability)
4 s
Every 4 s, synchronous with :00,:04,:08…
Overwrites previous value; no buffering at the interface (O.8.4)
Instantaneous P (and Q) at POC → Aggregator/BSP (MSD participation, PF3)
Instantaneous
Every 4 s, synchronous with :00,:04,:08…
Real-time value, not averaged (O.8.5)
Status change of General Switch (DG) / Generating-Unit Switch (DDG)
Event-driven
Notified within max 4 s of event occurrence
O.8.6
4.1 Generating-Unit-Level Measurement Thresholds (for individually transmitted P measurements)
Unit type
Size threshold requiring individual measurement
Inverters of generating units (incl. wind via inverter)
Nominal power P ≥ 170 kW
Inverters of storage systems
Rated power P ≥ 50 kW
Rotating generators
Rated power P ≥ 250 kW
Source (Annex O): O.8.4, item c).
5. Measurement Accuracy Requirements
Element
Requirement
Measuring converter accuracy class
≤ 0.2
Current transformer (CT) accuracy class
≤ 0.5
Voltage transformer (VT) accuracy class
≤ 0.5
Rated burden (where applicable)
5 VA or 10 VA
Measurement standard / method
IEC 61557-12, fixed block interval method (Annex B); accuracy classes as above
Dedicated fiscal-metering CT/VT reuse
Excluded — fiscal CTs/VTs cannot be used for CCI measurements
Shared transducers (measurement/protection)
Allowed only if no interference between uses (dedicated windings/cores if necessary)
Compliance target
Must satisfy accuracy requirements of Tables 5 & 6, Annex A.6 of the TSO Grid Code
Source (Annex O): O.13.2.1 "Requirements for transducers and measuring converters".
6. Polygonal Characteristic & Power Reference Quantities (PF2/PF3)
Producer-declared quantities (stated in the "Operating Rule") that define the plant's operating envelope at the POC:
Symbol
Meaning
Pass
Maximum active power absorption from the grid
Pimm (Pfed)
Maximum active power fed into the grid
Qcap
Maximum capacitive reactive power (overexcitation)
Qind
Maximum inductive reactive power (underexcitation)
Smax
Maximum apparent power = √[max(Pimm², Pass²) + max(Qind², Qcap²)] — used as the p.u. reference base for all set-points exchanged with DSO/Aggregator
Source (Annex O): O.8.2 "Polygonal characteristic of the plant".
7. Control-Function Parameters and Thresholds (PF2 — Voltage Control & Active Power Limitation)
7.1 cosφ = f(P) Control (O.9.1.2)
Parameter
Value
Deadband δcosφ (α)
α = 0.02 (default; must be adjustable)
Trigger rule
New cosφ transmitted to fast control ring only if |cosφ_calculated − cosφ_internal_setpoint| ≥ α
Voltage lock-in / lock-out thresholds
User-settable; verified on ΔT-averaged voltage
Recalculation rate
Every ΔT (slow control ring)
7.2 Q = f(V) Control (O.9.1.3)
Parameter
Value
Deadband δQ (σ)
σ = 5% of Qmax (default; must be adjustable)
Trigger rule
New Q transmitted to fast control ring only if |Q_calculated − Q_internal_setpoint| ≥ σ
Active-power lock-in threshold
0.2 × Pfed
Active-power lock-out threshold
0.05 × Pfed
Lock-in/lock-out verification basis
ΔT-averaged active power values
Voltage breakpoints (settable)
V1i, V2i, V1s, V2s (curve shape per CEI 0-16 Fig. 67 / Clause I.3)
Slope parameter k
Adjustable, −1 ≤ k ≤ +1
Recalculation rate
Every ΔT (slow control ring)
7.3 Active Power Limitation (O.9.2)
Case
Requirement
Limitation for V ≈ 110% Un (O.9.2.1)
Autonomous function, user-activated only (local or remote terminal); every activation and every actual intervention must be logged in the data logger
Limitation on external DSO command (O.9.2.2)
Slaved function; if no comms channel, executed manually by user per the "Operating Rule" procedure
Modulation of active power on external DSO command (O.9.2.3)
Slaved function; identical implementation to the Active Power Set-Point function (O.10.3.1)
Max limitation step
Never larger than the steps stated in CEI 0-16 itself
7.4 Plant Start-Up / Reconnection Power Gradient (PF3, O.10.1–O.10.2)
Parameter
Value
Centralised start-up power growth gradient
Positive gradient not exceeding 20% Pn/min
Reconnection after interface-protection trip
Same gradient rule; reconnection enabled only once grid V/f meet CEI 0-16 clause 8.8.7.2 conditions
8. Priority Among Control Functions (Table 1, Annex O Clause O.11)
Lower index = higher execution priority. If a higher/equal-priority function is requested while an incompatible lower-priority one is active, the new one takes over immediately.
Priority index
Control function
1 (highest)
Active power limit intervention for V ≈ 110% VN (O.9.2.1)
2
Active power limitation on external DSO command (O.9.2.2)
3
Modulation of active power to POC on external command from DSO (O.9.2.3)
4
Active Power Set-Point function on external command (O.10.3.1)
5
Voltage regulation via reactive power supply on external command from DSO (O.9.1.4)
6
Set-point power factor (cosφ) (O.9.1.1)
6
Reactive power control Q = f(V) (O.9.1.3)
6
Reactive power control cosφ = f(P) (O.9.1.2)
7 (lowest)
Reactive Power Set-Point function on external command (O.10.3.2)
Above/outside this table, with absolute priority over everything else: (a) individual-machine over-/under-frequency active-power regulation (CEI 0-16 §8.8.6.3.2 / §8.8.6.3.3 — explicitly NOT implemented by CCI itself, must remain at unit level); (b) full/partial plant disconnection commanded by the "defence plan" teletripping device (Annex M) — CCI must not take any conflicting action when this occurs.
9. Communication Interface Technical Specifications
9.1 External Network Interfaces (O.13.1.1.1)
Interface
Purpose
Physical layer
Protocols
Eth_A
Communication to DSO
100BaseFX, optical, dual LC connector, 1310 nm multimode fibre
TCP/IPv4 (mandatory), IPv6 (optional); IEC 61850 family (MMS, GOOSE); SSL/TLS 1.2+ per IEC 62351-4/-8/-9; DHCP; DNS forwarding
Eth_B
Communication to other enabled remote operators (User remote / Aggregator)
10BaseT / 100BaseTX, auto-negotiation, auto MDI/MDIX (may use SFP)
Same protocol stack as Eth_A
Both interfaces: physical-link and data-link status must be exposed in the data logger. No internal switch/bridge function is permitted between CCI's own interfaces — no data exchange between Eth_A, Eth_B, the local configuration port, or the plant-element network is allowed inside CCI.
9.2 Local Configuration / Maintenance Interface (O.13.1.1.2)
Serial or USB technology only — IP-based communication explicitly excluded for security reasons.
Must implement user-recognition/authentication to prevent unauthorised access.
9.3 Loss-of-Communication Fallback Timing (O.13.1.2 / O.13.1.3)
Channel
Fallback behaviour
DSO channel (Eth_A) lost while CCI performs control
Automatic switch to pre-configured autonomous mode; switch delay must be long enough to ensure the loss is permanent (exact value set with DSO in "Operating Rule"); auto-restore to DSO-requested mode when comms return
Aggregator/BSP channel (Eth_B, MSD) lost
Same principle: delayed automatic switch to pre-configured mode, agreed with Aggregator after verification with DSO; auto-restore when comms return
10. Power Supply, Clock and Reliability
Parameter
Requirement
Backup power autonomy (UPS/battery)
≥ 1 hour for CCI circuits and associated data-communication equipment
Time reference
UTC (Coordinated Universal Time), via GPS receiver
Time-sync uncertainty
≤ ±100 ms
Clock drift during loss of external sync / power loss
≤ 1 s/day, across the full allowable operating temperature range
Reliability/availability target
Per TSO Grid Code Annex A.13, Article 6.2 (subject to be defined per ARERA Resolution 628/18/R/EEL)
Source (Annex O): O.13.3 "CCI Power supply"; O.13.5 "Internal clock and synchronisation"; O.13.10.
11. Data Logger Requirements (O.14)
Parameter
Requirement
Minimum stored events (rolling, non-overwritable by user)
2048 events
Timestamp format
yyyy/mm/dd hh:mm:ss
Access control
Username/password (or equivalent) required
Remote read-out
Standard protocol (e.g., syslog per RFC 5424, for SIEM integration)
Mandatory event categories to be logged
CCI power supply presence; power on/off and cause; firmware update (incl. version) and failed updates
Status (open/closed) of General Switch (DG) and Interface Switch(es) (DI)
Presence/absence of communication to DSO, to external operators, and to plant elements (physical-link and data-link status)
CCI functionality status; measurement-device functionality status
Irregular connections/disconnections to the communication network; connection to abnormal service endpoints or at inappropriate times
Anomalous network-traffic fingerprints (port scanning, deep scanning, etc.); malformed messages / message-authenticity verification errors
Authentication attempts (failed and successful); credential/key changes and failed change attempts; alarm/error-log resets
If bootloader lockout password is used: failed and successful boot authentications
Start/stop/activation status of each control function; events causing CCI to issue commands to plant elements (e.g. exceeding 110% Vn threshold, control-function intervention, set-point implementation)
Commands received from DSO and from any other authorised external operator, with their parameters
Tripping of General Protection (PG) and Interface Protection (PI)
Tripping of the defence-plan teletripping function (Annex M)
Tripping of over-/under-frequency regulation by controlled machines/units
Any change to control-function priority settings
Entry of new/changed characteristic electrical parameters of the installation (polygonal curve updates)
12. Testing, Accreditation & Certification Requirements
Test category
Requirement / Standard
Functional tests
Verify all applicable PF1/PF2/PF3 functions per Annex O Clause O.6; measurement accuracy, disturbance immunity, update-interval compliance; IEC 61850 comms correctness; self-diagnostics; data logging
General conformity (environmental/EMC/insulation)
CEI EN 61557-12 (recalls IEC 61010-1 and, notably, IEC 61010-2-201); reference temperature class ≥ K55; must run in a lab accredited per CEI UNI EN ISO/IEC 17025 (in Italy: ACCREDIA)
Hardware cybersecurity / tamper resistance
At least FIPS 140-2 Level 3, certified by an independent body
Product security (secure development lifecycle)
ISA Secure EDSA v3.0.0, per IEC 62443-4-1 and IEC 62443-4-2
IEC 61850 conformance
Certified by a laboratory accredited by the UCA User Group
Secure transport (IEC 62351-3)
Conformance per IEC 62351-100-3, certified by an accredited body
Quality system
ISO 9001:2000 (as amended) certification of the manufacturing process
CE marking
Mandatory
Declaration of Conformity
Manufacturer self-certification per Art. 47 of Italian Presidential Decree 445/2000; test reports retained ≥ 20 years from last production
13. Open Items — Pending Annex T
The following items are referenced by Annex O but fully specified only in Annex T ("Cybersecurity requirements relating to communication from the outside to CCI and from CCI to the outside"), which has not yet been uploaded/processed:
Complete IEC 61850 logical-node / data-model mapping for all measurements, signals and commands listed in this document (Appendix O-1 gives only illustrative examples, e.g. Q=f(V) status transitions OFF→ON→ACT).
Full cybersecurity architecture: certificate/PKI management, authentication mechanisms referenced in O.13.4 (firmware update) and O.13.7 (hardware tamper protection), Asset Inventory API details (O.13.7.2).
GOOSE/MMS specific timing performance classes, if any beyond what O.7.3 already fixes.
Detailed VPN/private-network requirements for Eth_A/Eth_B referenced in Annex O footnote 197.
Once Annex T is available, this document should be revised to add a dedicated "Cybersecurity & Data Model" section and cross-check every mandatory value above against Annex T for consistency.
