# CCI Flowcharts — draw.io text extract (RAG corpus)

**RAG source_id:** `ccli-flowcharts-drawio-extract`  
**Date:** 2026-09-30  
**Purpose:** Searchable text from regulation flowcharts in `Flowcharts/*.drawio`.  
**Master equations:** `Flowcharts/PARAMETERS_AND_EQUATIONS.md` · `ccli-flowcharts-params-equations`

---

## CCI_Control_not final.drawio

1. CCI CONTROL SYSTEM | Module overview
2. One-page integration view · priority O.11 · fast ring O.7.3.1 · slow ring O.7.3.2 · measurements O.7.4
3. PLANT USER P or Q request
4. EXTERNAL COMMAND P or Q set-point
5. 3 s UPDATE CHECK from last processed set-point earlier update: reject
6. REJECT keep current command
7. ACTIVE-POWER LIMIT separate function module effective P ceiling
8. O.11 PRIORITY ANALYSIS select allowed P/Q request apply applicable P limits issue expected POC target
9. INTERNAL SET-POINT selected operating point at the POC
10. FAST CONTROL RING compare MC200 vs target respect capability / POC limits coordinate plant commands
11. PLANT ELEMENTS inverters / storage / units commands + telemetry
12. SLOW CONTROL RING selected Q(V) / cosφ(P) module dead band + ΔT scheduler eligible candidate to O.11
13. POC MCΔT fixed-block aggregation slow curve + thresholds
14. POC MC200 200 ms fixed-block data fast-loop feedback
15. POINT OF CONNECTION measured P / Q / V actual plant operating point
16. COMMAND SOURCES
17. SELECT EXPECTED OPERATING POINT
18. ACTUATE THE PLANT
19. POC FEEDBACK AND SLOW CURVE CALCULATION
20. optional unit feedback
21. MC200
22. Curve / function parameter updates: separate ΔT acceptance check inside slow ring.
23. O.7.3.3: the 3 s check applies to external set-point updates; reject an early update before O.11.
24. O.7.4: fast ring uses MC200; slow ring uses MCΔT. Detailed control logic is on the separate ring and priority pages.
25. CCI O.11 — Compatibility & Priority Arbitration
26. This page handles activation requests, plant-function availability, O.11 compatibility, configurable priorities, P-limit arbitration and capability conflicts. Communication, measurements, regulation-curve calculations and device allocation belong on separate CCI pages.
27. New regulation-function ACTIVATION request
28. Look up O.11 compatibility against the CURRENT ACTIVE function set P ↔ Q compatible · Q functions mutually incompatible · P target/modulation functions mutually incompatible · P-limit functions may coexist
29. Is any currently ACTIVE function incompatible with NEW?
30. NO — Activate NEW and retain all compatible active functions
31. Read CURRENT CONFIGURED executive-priority indices Do not assume the default order is permanently hard-coded
32. Is NEW priority index ≤ incompatible ACTIVE index?
33. YES — Activate NEW and simultaneously deactivate the previous incompatible function(s)
34. NO — Reject NEW activation request Existing higher-priority incompatible control remains active
35. REQUEST HANDLED — active control set unchanged
36. Accepted ACTIVE function set updated
37. Resolve ACTIVE-P functions • At most one incompatible P target/modulation function remains active. • Evaluate active P-limit functions O.9.2.1 / O.9.2.2 together with the P target, if any. • The effective P requirement/ceiling is the most restrictive applicable value.
38. For each quantity not controlled by any active regulation function, use the nominal/default operating condition defined for the specific plant
39. Can all simultaneously active compatible P/Q requests be satisfied within current plant and unit technical capability?
40. YES — Keep the resolved P/Q operating request
41. NO — Apply CEI 0-16 §8.8.6 capability constraints Move to the nearest feasible operating point while respecting required service priority and the capability limits of the plant elements. Example: reduce active power when necessary to satisfy a reactive-power request.
42. Apply higher-level override/interlock check before actuation Machine-local over/under-frequency transient controls and defense-plan actions have overriding priority. CCI must not implement the frequency-transient regulation itself or counteract it.
43. OUTPUT OF THIS PAGE: resolved internal P/Q request + active-function state → Plant Control / Command Allocation page
44. SPECIAL CASE — SAME DEFAULT PRIORITY = 6
45. Set-point cosφ O.9.1.1
46. Q = f(V) O.9.1.3
47. cosφ = f(P) O.9.1.2
48. All reactive-power regulation functions are mutually incompatible. Under the default Table 75 ordering, these three functions have index 6. Therefore a new priority-6 request satisfies NEW ≤ ACTIVE and replaces the previously active incompatible priority-6 function. This is a consequence of the general rule, not a separate arbitration algorithm.
49. Priority configuration requirement The default Table 75 order is the starting configuration, but the CCI priority order must be implemented as variable/configurable logic. Any change to the priority settings must be recorded in the apparatus log. The configuration and logging mechanisms can be detailed on separate pages.
50. Compatibility matrix used by the first decision • Any P-regulation function is compatible with Q-regulation functions. • Q-regulation functions are mutually incompatible. • DSO P modulation and external P set-point are mutually incompatible. • Active-power LIMIT functions may remain active together with other regulation functions; their effect is applied only when they actually constrain P.
51. YES — higher or equal priority
52. NO — lower priority
53. Check plant function availability / support Is the requested regulation function implemented, enabled and applicable for this specific plant configuration?
54. Is NEW function available / supported by this plant?
55. NO — Do not activate NEW Status: function unavailable / not supported for this plant Active control set unchanged
56. FAST CONTROL RING | Plant control at the POC
57. Figure 123 · CEI 0-16 Annex O, O.7.2 / O.7.3 / O.7.4 · detailed view connected to your O.11 priority page
58. Plant user P or Q setpoint
59. External P/Q command after 3 s update gate O.7.3.3 → O.11
60. SLOW CONTROL RING Aggregate POC measures (M CΔT) Q(V) or cosφ(P) + dead band candidate setpoint at POC
61. O.11 PRIORITY / COMPATIBILITY Select compatible P / Q functions and configured priorities
62. INTERNAL SETPOINT AT POC Expected P* and/or Q* (cosφ reference when selected)
63. POC / PdC METER Grid connection point Synchronously sampled V / I measured P, Q, V
64. PROCESS POC DATA Fixed 200 ms blocks (M C200) P_POC, Q_POC, V_POC with freshness / validity
65. COMPARE Expected vs measured POC operating point calculate P / Q deviation
66. CHECK POC LIMITS Feasible P / Q values and applicable gradients respect active priorities
67. COORDINATE ELEMENTS Allocate feasible P_i* / Q_i* using unit capability, state and measured response
68. PLANT ELEMENTS Inverters, generators, storage / controllable plant equipment
69. Element measurements Unit P_i / Q_i, status and available capability via internal communications
70. SETPOINT SOURCES
71. CONTROL SELECTION
72. FAST RING · CLOSED-LOOP CONTROL
73. INTERNAL COMMANDS
74. electrical response + network conditions → POC feedback
75. unit telemetry
76. Design view of Fig. 123 · POC means point of connection (PdC). Technical capability and applicable POC limits govern all allocated commands.
77. POC measurement stream → slow-ring aggregation over ΔT
78. Whole-plant settling (O.7.3.1): TsP ≤ 60 s · TsQ ≤ 10 s · from T0 until stable within ±5% at POC
79. Verification: Annex N.7.4.1 (P) and N.6.2.3 (Q). A new slow target may arrive before settling.
80. Measurement contract (O.7.4): IEC 61557-12 fixed blocks, synchronous V/I, POC accuracy O.13.2.1; element telemetry must preserve control accuracy (O.8.1).
81. SLOW CONTROL RING | Reusable curve scheduler
82. Annex O O.7.3.2 / O.7.4 · Q(V) and cosφ(P) are function modules · external P/Q and P limits enter O.11 separately
83. POC MEASUREMENT STREAM processed fixed-block data shared with fast ring
84. AGGREGATE OVER ΔT form MCΔT: averaged P, V for curves AND lock thresholds
85. ACTIVE CURVE MODE Q(V) or cosφ(P) mutually exclusive Q functions
86. FUNCTION MODULE PORT eligible Q(V) or cosφ(P) module returns POC candidate + dead band
87. SELECTED FUNCTION DEADBAND compare candidate vs last sent using that function’s δ
88. SLOW CANDIDATE → O.11 PRIORITY ANALYSISif accepted: fast internal setpoint may replace unfinished fast response
89. NO NEW SLOW CANDIDATE inactive or within dead band O.11 keeps other / current request
90. FUNCTION / CURVE CHANGE external activation or params distinct from external P/Q setpoint
91. ΔT COMMAND GATE interval shorter than ΔT: reject accepted curve applied by next ΔT
92. CONFIGURED PARAMETERS DSO-issued settings change only on new DSO instruction
93. ΔT adjustable 10–600 s; default 60 s unless DSO specifies otherwise (O.7.3.2).
94. Measurement basis (O.7.4): IEC 61557-12 fixed blocks, synchronous V/I; fast MC200, slow MCΔT; accuracy O.13.2.1.
95. Module interface: MCΔT + active mode + curve params → Qcalc or cosφcalc + δ. Active-P limitations and external P/Q commands bypass this ring to O.11.
96. NO UPDATE
97. COMMON PLANT CAPABILITY & NORMALIZATION | AI IMPLEMENTATION SPEC
98. [CEI] = explicit CEI 0-16 requirement/data | [DERIVED] = direct mathematical implementation | [SW ARCH] = chosen software architecture
99. [CEI] PLANT CONFIGURATION — PdC CAPABILITY Pimm = max active power injection [kW] Pass = max active power absorption [kW] Qcap = max capacitive reactive power [kvar] Qind = max inductive reactive power [kvar] Configured in the Operating Rule; generation/storage capability, loads excluded
100. [CEI] MAXIMUM APPARENT POWER REFERENCE Smax = √[ max(Pimm², Pass²) + max(Qind², Qcap²) ] Configured in the CCI from the Operating Rule. Smax remains the p.u. reference when available capability changes due to outage/maintenance.
101. [DERIVED] NORMALIZATION Ppu = P / Smax Qpu = Q / Smax Direct mathematical implementation of CEI requirement that Smax is the common p.u. reference
102. [SW ARCH] CONSUMERS INTERFACE O.11 priority / feasibility Q(V), cosφ(P), fixed-setpoint modules FastAllocator
103. [SW ARCH] DynamicAvailableCapability Compute / track the currently feasible plant or section capability from element capability and availability. [CEI RULE] update capability for outage / maintenance. [CEI RULE] do not change Smax because of these availability updates.
104. [SW ARCH] ElementCapabilityStatus Per-element inverter / storage / generator: • capability limits / curve • online / available status • current operating state
105. [SW ARCH] TO O.11 Expose: • configured Smax / plant references • current feasible capability Used for priority + feasibility resolution
106. [SW ARCH] TO FastAllocator Expose per-element capability / status Used to create feasible Pi* / Qi* commands
107. [CEI] Smax formula + Pimm/Pass/Qcap/Qind + Smax as p.u. reference + dynamic capability update for outage/maintenance.
108. [DERIVED] Ppu=P/Smax, Qpu=Q/Smax | [SW ARCH] CommonPlantCapability, DynamicAvailableCapability, Consumers, FastAllocator, ElementCapabilityStatus.

---

## Cosfi(P).not confirmed.drawio

1. α adjustable
2. WRITTEN BY DSO / OPERATOR — configuration, not live values
3. P_A = 0.20×Pn
4. cosφ_A = 1.00
5. P_B = 0.50×Pn
6. cosφ_B = 1.00
7. P_C = 1.00×Pn
8. cosφ_C = 0.95
9. Voltage lock-in = 1.05×Vn
10. Voltage lock-out = 0.98×Vn
11. MEASURED LOCALLY BY CCI — not sent by DSO
12. P at POC 200ms cadence, MC200 (O.7.4)
13. V at PdC
14. FAST RING — applied to the plant (Figure 123)
15. Check gradient & limitation at POC ramp toward target if step is too large
16. Does inverter accept a native cosφ register?
17. Write cosφ setpoint to inverter's cosφ register (vendor-scaled, e.g. ±1000 = ±1.00)
18. Convert to Q: Q_target = P_measured × tan(arccos(cosφ_internal_sp)) sign = leading/lagging
19. Write Q_target to inverter's Q register
20. Modbus TCP/RTU write function code 0x06 / 0x10 to inverter (CCI = master)
21. V within lock-in/lock-out band? 0.98 ≤ V ≤ 1.05
22. STATEFUL VOLTAGE GATE OFF → ON if V ≥ V_in ON → OFF if V ≤ V_out
23. Function inactive — no action this cycle
24. Which zone is P in?
25. Zone 1: P ≤ P_A — saturated cosφ_calculated = cosφ_A
26. Zone 2: P_A < P < P_B interpolate A→B
27. Zone 2: P_A
28. Zone 3: P_B ≤ P < P_C interpolate B→C
29. Zone 3: P_B ≤ P
30. Zone 4: P ≥ P_C — saturated cosφ_calculated = cosφ_C
31. &|cosφ_calculated − cosφ_internal_sp&| ≥ δcosφ? δcosφ = α = 0.02
32. &|cosφ_calculated − cosφ_internal_sp&| ≥ δcosφ? δcosφ = α adjustable / default =0.02
33. Update cosφ_internal_sp
34. Hold previous setpoint
35. DEFINITIONS ──────────────── cosφ_calculated, piecewise: P≤P_A → cosφ_A P_A<P<P_B → cosφ_A + (P−P_A)/(P_B−P_A)×(cosφ_B−cosφ_A) P_B≤P<P_C → cosφ_B + (P−P_B)/(P_C−P_B)×(cosφ_C−cosφ_B) P≥P_C → cosφ_C ──────────────── cosφ_internal_sp: state variable = currently active setpoint; updated only when the trigger fires, otherwise retains its prior value
36. DEFINITIONS ──────────────── cosφ_calculated, piecewise: P≤P_A → cosφ_A P_A
37. config values
38. zone 1
39. zone 2
40. zone 3
41. zone 4
42. reference
43. Voltage Lock-In = 1.00 -- 1.10 / Default=1.05Voltage Lock-out 0.90 -- 1.00 / Default=0.98
44. O.11 / FAST RING INTERFACES — modules on separate pages
45. Send new cosφ(P) candidate to O.11 selection capability check
46. Is cosφ(P) the selected feasible Q function?
47. YES: update internal cosφ reference at POC
48. NO: preserve previously selected internal reference
49. Keep active target no cosφ(P) update
50. SEPARATE FAST RING: MC200 POC feedback, allocation and plant commands
51. Alireza Version

---

## cosfi. not confirmed.drawio

1. Activation command Range: 5=Inactive / 1=Active Default: 5
2. Activation command Range:-5=Inactive / -1=Active Default: -5
3. Command = 1 (Active)?
4. Command = -1 (Active)?
5. Function idle - no cos phi set-point applied
6. Operating status Range: -1=Operating / -5=Inavtice / +5 = off Default: -5
7. Operating status Range: -1=Operating / -5=Inavtice/ +5 = off Default: -5
8. Status = 1 (Operating)?
9. Status = -1 (Operating)?
10. Function status Range: 0=Not available / 1=Autonomous / 2=Automatic Default: 1 (Autonomous)
11. Function status Range: 0=Not available / 1=Autonomous / 2=Enabled/Slaved (Priority) Default: 1 (Autonomous)
12. Plant flow direction at POC
13. cos phi setpoint, generation Range: -1.00..+1.00 (+capacitive / -inductive) Default: -0.95 (inductive)
14. cos phi setpoint, absorption Range: -1.00..+1.00 (+capacitive / -inductive) Default: +0.95 (capacitive)
15. Trigger equation: NONE - fixed setpoint, no formula (unlike Q(V), no ΔT recompute)
16. OUTPUT TO INVERTER: cos phi_sp applied directly sent via internal network (Modbus/61850)
17. accepted cosφ target to fast control ring
18. Settling check - inherited fast ring TsQ <= 10 s (O.7.3.1 - not explicitly restated in O.9.1.1)
19. Settled within TsQ <= 10 s ?
20. Set-point achieved - inverter holding fixed cos phi_sp
21. Flag deviation (see O.13.6 self-diagnostics)
22. Held until Activation command or Operating status changes (no periodic recompute - not curve-based)
23. No (default = 5, Inactive)
24. No (default = -5, Inactive)
25. Yes (1, Active)
26. Yes (-1, Active)
27. No (default = 5, Not-Operating)
28. No (default = -5, Not-Operating)
29. Generation (injecting P)
30. Absorption (load / charging)
31. No / anomaly

---

## DSO_P_limit O_9_2_2.drawio

1. O.9.2.2 | Active-power injection limit commanded by the DSO Slaved CCI function at PdC · same ceiling interface as O.9.2.1 · offline User procedure on page 2
2. ACCEPTED DSO COMMAND / STORED STATE A: enabled; L: accepted ceiling in kW. Activate with valid L: A ← 1. Accepted limit update: L ← Lreq. Release: A ← 0; no new command: retain state.
3. PLANT CONFIGURATION Pn,plant, command scaling and unit capabilities: configurable from approved plant data. Received DSO threshold is an operating command.
4. DSO COMMAND VALIDATION Validate authority, operation, units and range. Δt = tnow − tlast Accept setpoint timing if no prior processed setpoint or Δt ≥ 3 s; otherwise reject (O.7.3.3).
5. VARIABLES / IMPLEMENTATION CONVENTION A ∈ {0,1}: active state; L: stored ceiling in kW. Positive P = injection at PdC. Superscript ⁺ = value after command processing. Pbase: requested power before active P ceilings.
6. ACCEPTED STATE UPDATE Value update: L⁺ = Lreq; otherwise L⁺ = L. Activate: A⁺ = 1; release: A⁺ = 0; otherwise A⁺ = A. Processed setpoint: tlast⁺ = tnow; otherwise retain tlast.
7. DSO LIMIT ENABLED? A = 1?
8. disabled: no ceiling
9. INACTIVE — A = 0 No ceiling from O.9.2.2. Other active control functions continue.
10. ACCEPTED CEILING USABLE? Valid stored L and DSO channel available?
11. CHANNEL LOSS / NO USABLE CEILING Channel loss: apply agreed delay and fallback; see page 2. Invalid command: reject, preserve valid state. No valid ceiling: do not activate this function.
12. OBTAIN THE DSO POWER CEILING L = accepted DSO power threshold. If command is s% of nominal plant power: L = (s / 100) × Pn,plant No voltage-dependent P(V) calculation.
13. DOES THE CEILING REQUIRE LIMITATION? Pbase > L? Pbase: otherwise requested feasible injection.
14. CEILING CURRENTLY NON-BINDING Pbase ≤ L: no additional reduction. Keep ceiling L active. Lower injection is allowed; do not force P up to L.
15. LIMITATION REQUIRED ΔPcurtail = max(0, Pbase − L) Pass ceiling L to the shared resolver. This quantity expresses the requested curtailment.
16. SHARED UNIT RAMP CONSTRAINT −Rdown,i Δtc ≤ Pi,cmd[k] − Pi,cmd[k−1] ≤ Rup,i Δtc Rates: configured applicable unit gradients (kW/s). Δtc: control-cycle duration (s).
17. FUNCTION OUTPUT TO O.11 A = 1 → PDSO,cap = L A = 0 → no ceiling from this function Active ceiling persists even when non-binding.
18. STATUS / DIAGNOSTICS / LOG Expose accepted ceiling and function state. O.14: record required communication and functional diagnostics. O.11: record priority changes.
19. SHARED O.11 PRIORITY / FEASIBILITY Pbounded = min(Pbase, L1, …, Lm) Lj: applicable active ceilings; none → Pbounded = Pbase. Resolve joint P/Q capability and priorities to obtain Pref. Default DSO limit priority: 2 when intervening.
20. SHARED FAST CONTROL RING Track resolved P at PdC; allocate curtailment to units. Storage absorption is possible if installed and SoC permits. Use configured plant capabilities.
21. SHARED FAST-RING FEEDBACK eP = Pref − PPdC,MC200 With active ceiling: excess = max(0, PPdC − L). Use valid feedback; monitor tracking and ceiling excess. The shared regulator determines unit-command corrections.
22. NO DSO CHANNEL — SEE PAGE 2 DSO may request reduction / disconnection through the Operating Rule procedure. The User implements it on site or by remote terminal. Automatic fallback is a separate path.
23. RESPONSE CRITERIA O.7.3.1 fast ring: TsP ≤ 60 s, ±5% settling band. 8.8.6.3.4 commanded reduction: within 1 min; ±2.5% Pn, with the stated special band for 10% / technical minimum. Apply the relevant test criterion.
24. scaling
25. accepted ceiling / channel state
26. continuous evaluation
27. state and events
28. agreed fallback (page 2)
29. O.9.2.2 — NO DSO COMMUNICATION CHANNEL Two independent paths: automatic CCI fallback and DSO request implemented by the User
30. DSO CHANNEL UNAVAILABLE Detect and log the communication state (O.14). The User procedure does not wait for the automatic fallback timer.
31. A. AUTOMATIC CCI BEHAVIOUR — O.13.1.2
32. B. DSO REQUEST IMPLEMENTED BY USER — O.9.2.2
33. LOSS TIMER / CONFIGURED FALLBACK On loss transition: tloss,start ← tnow. τloss = tnow − tloss,start Enter fallback if channel remains down and τloss ≥ Tloss. Tloss, waiting policy and fallback mode: agreed with DSO.
34. Channel still down AND τloss ≥ Tloss?
35. ENTER PRECONFIGURED AUTONOMOUS MODE Activate the agreed fallback functions through O.11. Continue shared control and communication monitoring. The actual mode is specified by the Operating Rule.
36. FALLBACK ≠ A NEW DSO REQUEST Loss of communication does not itself specify a new power threshold or order a disconnection. Which limits remain effective in fallback must follow the agreed configuration.
37. DSO ISSUES AN OFFLINE REQUEST Reduction or disconnection is requested through the predefined procedure in the Operating Rule (8.8.6.3.4). The exact alternative contact method is specified there.
38. USER RECEIVES AND CHECKS THE REQUEST Apply the Operating Rule procedure to confirm its origin, requested action and applicable conditions. This check is an implementation step of the agreed procedure.
39. USER CARRIES OUT THE REQUEST Either ON SITE or through a REMOTE CONTROL TERMINAL. This action is required even though the normal DSO–CCI channel is unavailable.
40. Requested action?
41. POWER REDUCTION User applies the requested reduction using the approved plant/terminal procedure. If the CCI implements it: submit the authorised ceiling through the approved User control path to O.11 and the shared fast ring (page 1).
42. DISCONNECTION User implements the requested disconnection through the authorised plant procedure/equipment. This diagram does not assign a protection or trip function to the CCI.
43. USER / PLANT CONFIRMS IMPLEMENTATION Verify the resulting power or switching state and perform any reporting required by the Operating Rule. CCI monitors available PdC, unit and switching feedback; avoids actions conflicting with disconnection (O.9.3).
44. CONTINUOUS CHANNEL MONITORING Restoration is monitored throughout both paths. No offline request: no action is taken under path B; path A continues according to its configured policy.
45. WHEN THE DSO CHANNEL IS RESTORED — O.13.1.2 Log restoration and return to implementing DSO requests through the normal interface (page 1). Synchronise command state using the agreed procedure; return of communication is not by itself a reconnection order.
46. DEFENCE-PLAN BOUNDARY — O.9.3 CCI can implement commanded power reduction. The separate Annex M device remains responsible for the required remote disconnection function. CCI must detect its action and avoid conflicting commands.
47. Source: supplied CEI 0-16:2025-12, O.9.2.2, 8.8.6.3.4, O.13.1.2, O.9.3 and O.14. Site procedure and fallback values: configurable / agreed in the Operating Rule.
48. DSO may still request action
49. not yet
50. reduce power
51. disconnect
52. on restoration
53. IMPLEMENTATION EQUATIONS — O.9.2.2 These equations specify the proposed software implementation. CEI clause references identify required behaviour; variable names, data structures and regulator design are implementation choices.
54. 1. NORMALISE AND VALIDATE THE REQUEST If nominal-power percentage s is received: L_req = (s / 100) × P_n,plant. If power is received directly: L_req = unit_conversion(received_power). Require a finite value, supported operation and admitted range according to the interface / Operating Rule. Reject invalid input; do not silently clamp a DSO command to a different value.
55. 2. RATE CHECK AND ATOMIC STATE UPDATE Use a monotonic clock: Δt = t_now − t_last. No previous processed setpoint → first update is eligible. For an external setpoint update: time_ok = (no_previous_setpoint) OR (Δt ≥ 3 s). Accepted value update: L⁺ = L_req; otherwise L⁺ = L. Accepted activation: A⁺ = 1 (valid ceiling required); release: A⁺ = 0; otherwise A⁺ = A. t_last⁺ = t_now only for a processed external setpoint; otherwise t_last⁺ = t_last. Apply a combined activation/value command atomically. Rejected commands change none of these variables.
56. 3. CEILING OUTPUT AND CURTAILMENT Output = {active: A, ceiling_kW: L}; ignore ceiling_kW when A = 0. For compatible active ceilings: P_bounded = min(P_base, L_1, …, L_m). No active ceilings → P_bounded = P_base. Contribution before other constraints: ΔP_curtail = max(0, P_base − L), when A = 1. Keep the ceiling active when ΔP_curtail = 0; it must constrain any later increase. The shared resolver determines P_ref from P_bounded, joint P/Q capability and applicable priorities.
57. 4. SHARED CONTROL — INTERFACE AND CONSTRAINTS e_P[k] = P_ref[k] − P_PdC,MC200[k]. Positive power means injection at PdC. Active-limit monitoring: P_excess[k] = max(0, P_PdC[k] − L). For each unit i: −R_down,i Δt_c ≤ P_i,cmd[k] − P_i,cmd[k−1] ≤ R_up,i Δt_c. Also enforce unit capability / technical minima. Ramp and capability constraints must be checked jointly. PI gains, allocation weights and saturation/anti-windup logic belong to the shared fast-ring design; O.9.2.2 does not prescribe their formula.
58. 5. COMMUNICATION LOSS AND RESTORATION On a detected up→down transition: t_loss,start = t_now. While down: τ_loss = t_now − t_loss,start. Enter the agreed fallback when τ_loss ≥ T_loss (O.13.1.2). Do not restart the timer every cycle. On restoration: reset the loss timer and resume DSO operation through the agreed synchronisation procedure. Offline DSO requests implemented by the User follow page 2 independently of this timer.
59. COMMISSIONING AND PERSISTENCE Configure plant ratings, command mapping, unit gradients, communication-loss delay and fallback policy from approved data. Restart state / recovery of stored commands must follow the agreed commissioning policy; do not assume automatic reactivation. The shared fast ring still requires its own complete implementation and validation.

---

## DSO_P_modulation_O_9_2_3_.drawio

1. Command origin: Aggregator (BSP) via Eth_B interface (O.13.1.3) for MSD participation (O.10.3.1)
2. DSO ACTIVE-POWER MODULATION — O.9.2.3 Command origin: DSO via Eth_A (O.13.1.2). Same implementation as O.10.3.1.
3. Operating status Range: -1=Operating / -5=Not-Operating Default: -5
4. OPERATING STATUS — TABLE 85 −1 = Active / −5 = Inactive / +5 = Off Default: −5
5. Status = -1 (Operating)?
6. Status = −1 (Active)?
7. Function idle - no P set-point applied
8. Function idle: no P target from this mode.
9. Activation command Range: -5=Inactive / -1=Active Default: -5
10. ACTIVATION COMMAND — TABLE 85 −5 = Inactive / −1 = Active; default: −5. New activation requires the DSO channel and a valid target.
11. Command = -1 (Active)?
12. Command = −1 (Active)?
13. Function status Range: 0=Not available / 2=Automatic(Priority) Default: 2 - always externally driven, no Autonomous option
14. FUNCTION STATUS — TABLE 85 0 = Not available / 2 = Enabled (Priority); default: 2. Status 0 → unavailable: no activation. Status 2 → shared priority check. DSO channel loss → configured fallback (page 2).
15. Priority check - Table 1: this function = index 4 (beats index 5-7, loses to 1-3)
16. SHARED O.11 PRIORITY CHECK Default O.9.2.3 index = 3. Check conflicts with other P setpoint modes. New index ≤ existing index → replace conflicting mode. Otherwise reject activation. P ceilings remain compatible constraints.
17. Not activated - existing function stays in control
18. Activation refused: existing authorised mode stays in control.
19. Feed-in / feed-out active power setpoints Range: 0..100% of Smax (+feed-in / -feed-out) Default: 100 / 0 (full production baseline)
20. SIGNED TARGET — TABLE 85 Magnitude s: 0…100%; reference: plant Smax. Defaults: injection 100% / absorption 0%. P_req = σ × (s / 100) × Smax; σ = +1 injection, −1 absorption.
21. Trigger equation: NONE - direct target tracking, no formula
22. SETPOINT TIMING CHECK — O.7.3.3 First processed setpoint OR t_now − t_last ≥ 3 s? Reject faster updates; preserve accepted target and timestamp.
23. Distribute P target across generating units, and/or storage system modulation if compatible with state of charge (architecture left to User/plant designer)
24. ACCEPT TARGET / SHARED RESOLVER S⁺ = P_req; t_last⁺ = t_now for processed setpoint. P_bounded = min(S, applicable active P ceilings); no ceilings → S. Resolve P/Q capability and allocate generation / storage subject to SoC.
25. Can plant capability reach requested P?
26. Can plant reach P_bounded?
27. Bring plant to closest achievable P value
28. Resolve feasible P_ref subject to capability / priorities. Report limiting condition.
29. OUTPUT TO GENERATING UNITS: P_sp_new pushed across units via internal network (Modbus/61850)
30. SHARED FAST RING → UNIT COMMANDS Track feasible PdC target P_ref through allocated unit commands. Respect unit capability and configured gradients (equations on page 2).
31. Fast ring settling check (O.7.3.1): actual P at POC must settle within +/-5% of P_sp_new within TsP (time explicitly stated as 'within the time prescribed in O.7.3.1')
32. FAST-RING RESPONSE — O.7.3.1 TsP ≤ 60 s for any internal active-power setpoint change. Use the prescribed ±5% settling band and referenced test method. Retain requested S separately from feasible P_ref.
33. External Set-Point Dynamics (O.7.3.3): next update accepted only if >= 3 s since last processed set-point - faster updates REJECTED
34. CONTINUOUS SHARED FEEDBACK e_P[k] = P_ref[k] − P_PdC,MC200[k]. Positive error: increase net injection; negative error: reduce net injection.
35. P measurement fed back to Aggregator (instantaneous P at POC, per O.8.5)
36. Publish required PdC P and function status to DSO. Record diagnostics / changes as required by O.14 / O.11.
37. Hold current P_sp until next valid Aggregator command (>=3s later) or priority override
38. Hold accepted S between valid updates; continue regulation. Release → inactive; priority override → shared O.11. Channel loss → page 2; valid next update → timing check.
39. status 2
40. Higher-priority function (index 1-3) already active
41. Conflicting higher-priority P setpoint mode
42. No conflicting higher-priority function
43. Activation admitted
44. eligible
45. Yes: P_ref = P_bounded
46. REJECT UPDATE S⁺ = S; t_last⁺ = t_last. Continue prior authorised control.
47. too fast / invalid
48. status 0

---

## Lim W110 O.9.2.1.drawio

1. O.9.2.1 | Active-power injection limit near 110% Un Autonomous CCI function at PdC · plant-specific parameters are configurable · output is a P ceiling
2. USER ACTIVATION — FIXED REQUIREMENT The User enables/disables this autonomous function via the User terminal, on site or remotely. Define retained state after restart in the implementation.
3. PLANT CONFIGURATION — ENTER APPROVED VALUES Un at PdC: configurable from plant data. Pn of each generating unit: configurable from unit data. Overvoltage protection settings: reference for coordination.
4. PdC MEASUREMENT CONFIGURATION Voltage source and evaluated voltage quantity: configurable for the plant. Measurement validity/freshness criteria: define for the device.
5. PARAMETER SOURCES Plant design / as-built data: Un, unit Pn, voltage meter and protection settings. Commissioning approval: intervention rule, P(V) curve and Q(V) coordination.
6. [CEI] SCOPE Autonomous limitation near 110% Un. Independent interface/unit overvoltage protection still acts separately.
7. ENABLED AND AVAILABLE? User enable state and device availability.
8. disabled: no ceiling
9. INACTIVE Ceiling from this function: none. Signal/log activation state.
10. VOLTAGE MEASUREMENT VALID? Apply configured source and defined freshness/quality checks.
11. INVALID OR STALE VOLTAGE Fallback action, alarm and resumption rule: TO DEFINE in the implementation and commissioning procedure.
12. INTERVENTION — COMMISSIONING CONFIGURATION Vstart and Vrelease: configurable; coordinate with actual overvoltage protection. Hysteresis / dwell: configurable if used. O.9.2.1 gives no universal numerical thresholds.
13. IS LIMITATION INTERVENING? Evaluate the configured intervention rule.
14. NO INTERVENTION / RELEASE Use the configured release rule; remove this function’s ceiling smoothly. Other active-power requests remain applicable.
15. CALCULATE P CEILING — COMMISSIONING CONFIGURATION P110,cap(V): configurable continuous law for this plant. Configure breakpoints, power base and minimum after engineering approval.
16. ENFORCE SMOOTH REDUCTION — FIXED REQUIREMENT No power steps; reduction no faster than 0.33 Pn/s. Apply against the relevant unit rating; define recovery behavior.
17. FUNCTION OUTPUT TO O.11 If intervening: configured P110,cap(V) ceiling. Otherwise: no ceiling from this function; expose intervention state.
18. ACTIVATION / INTERVENTION — FIXED REQUIREMENT Signal and store activation and intervention separately in the CCI data logger (O.14). Map fields and event timestamps in implementation.
19. O.11 PRIORITY / FEASIBILITY Table 75 default: index 1 when intervening; use configurable priorities. Combine with DSO P limit and P setpoint; apply the feasible ceiling. Q functions may coexist.
20. SHARED FAST CONTROL RING Track resolved P at PdC and command plant elements. Unit allocation uses configured capabilities; storage use depends on plant equipment and SoC.
21. PLANT + PdC FEEDBACK Issue unit commands; use fresh PdC P and V for feedback and monitoring. Check the applicable settling and gradient requirements at the PdC.
22. Q(V) COORDINATION — COMMISSIONING CONFIGURATION V1s, V2s, V1i, V2i: configure for the plant’s Q(V) curve. Review interaction with P limiting and validate stability.
23. WHAT THE CCI IMPLEMENTS Store and validate approved plant configuration. This function proposes a P ceiling; O.11 resolves concurrent requests; the shared fast ring commands units. Record the approved values at commissioning.
24. next measurement
25. state and events
26. approved fault policy

---

## Q(V).not confirmed.drawio

1. Plant rated active power: Pn
2. lock-in = 0.20 × Pn& lock-out = 0.05 × Pn
3. lock-in = 0.20 × Pn lock-out = 0.05 × Pn
4. Measured P vs thresholds
5. Function ON
6. Function OFF
7. Vn (nominal voltage), V_measured
8. Vpu = V_measured / Vn
9. Compare Vpu to V2i, V1i, V1s, V2s
10. K = 1& (saturated injection)
11. K = 1 (saturated injection)
12. K = (V1i &− Vpu) / (V1i &− V2i)
13. K = (V1i − Vpu) / (V1i − V2i)
14. K = 0 (deadband)
15. K = (Vpu &− V1s) / (V2s &− V1s)
16. K = (Vpu &− V1s) / (V2s − V1s)
17. K = 1& (saturated absorption)
18. K = 1 (saturated absorption)
19. Q_calculated = 0 + K × Qmax& (lower zone: injection, +Q)
20. Q_calculated = 0
21. Q_calculated = 0 &− K × Qmax& (upper zone: absorption, &−Q)
22. Q_calculated = 0 − K × Qmax (upper zone: absorption, −Q)
23. Trigger check (slow ring, O.7.3.2):& |Q_calculated &− Q_sp| ≥ &δQ ?& evaluated every &ΔT = 10&–600s, default 60s
24. Trigger check (slow ring, O.7.3.2): |Q_calculated &− Q_sp| ≥ δQ evaluated every ΔT = 10–600s, default 60s
25. No update &— Q_sp unchanged& wait for next &ΔT cycle
26. OUTPUT TO INVERTER:& Q_sp_new = Q_calculated& sent via internal network (Modbus/61850)
27. FAST RING CHECK (O.7.3.1):& inverter's actual Q at POC must settle& within &±5% of Q_sp_new
28. Settled within& TsQ ≤ 10 s ?& (verified per N.7.4.1 / N.6.2.3)
29. Set-point achieved &—& inverter now tracking Q_sp_new
30. Flag deviation& (see O.13.6 self-diagnostics)
31. P > lock-in
32. P < lock-out
33. Vpu ≤ V2i
34. V2i < Vpu ≤ V1i
35. V1i < Vpu < V1s
36. V1s ≤ Vpu < V2s
37. Vpu ≥ V2s
38. No / anomaly

---

## S.P.Q(MSD).Not. confirmed.drawio

1. Command origin: Aggregator (BSP) via Eth_B interface (O.13.1.3) for MSD participation (O.10.3 / O.10.4)
2. Operating status Range: -1=Operating / -5=Not-Operating Default: -5
3. Status = -1 (Operating)?
4. Function idle - no Q set-point applied
5. Activation command Range: -5=Inactive / -1=Active Default: -5
6. Command = -1 (Active)?
7. Function status Range: 0=Not available / 2=Automatic(Priority) Default: 2 - note: no Autonomous option, always externally driven
8. Priority check - Table 1: this function = index 7 (LOWEST priority) any other active function wins
9. Not activated - existing function stays in control
10. Feed-in / feed-out reactive power target Range: 0..100% of Smax (+capacitive / -inductive) Default: 0 / 0
11. Trigger equation: NONE - direct target tracking, no formula (same as O.9.1.4)
12. Distribute Q target across plant elements (method left to User / plant designer, per O.9.1.4 implementation)
13. Can plant capability reach requested Q?
14. Bring plant to closest achievable Q value
15. OUTPUT TO INVERTERS: Q_sp_new pushed across generating units via internal network (Modbus/61850)
16. Fast ring settling check (O.7.3.1): actual Q at POC must settle within +/-5% of Q_sp_new within TsQ <= 10 s
17. External Set-Point Dynamics (O.7.3.3): next update accepted only if >= 3 s since last processed set-point - faster updates REJECTED
18. Q measurement fed back to Aggregator (instantaneous Q, per O.8.5)
19. Hold current Q_sp until next valid Aggregator command (>= 3 s later) or priority override
20. Higher-priority function already active
21. No conflicting higher-priority function

---

## S.P.W.(MSD).not.Confirmed.drawio

1. Command origin: Aggregator (BSP) via Eth_B interface (O.13.1.3) for MSD participation (O.10.3.1)
2. Operating status Range: -1=Operating / -5=Not-Operating Default: -5
3. Status = -1 (Operating)?
4. Function idle - no P set-point applied
5. Activation command Range: -5=Inactive / -1=Active Default: -5
6. Command = -1 (Active)?
7. Function status Range: 0=Not available / 2=Automatic(Priority) Default: 2 - always externally driven, no Autonomous option
8. Priority check - Table 1: this function = index 4 (beats index 5-7, loses to 1-3)
9. Not activated - existing function stays in control
10. Feed-in / feed-out active power setpoints Range: 0..100% of Smax (+feed-in / -feed-out) Default: 100 / 0 (full production baseline)
11. Trigger equation: NONE - direct target tracking, no formula
12. Distribute P target across generating units, and/or storage system modulation if compatible with state of charge (architecture left to User/plant designer)
13. Can plant capability reach requested P?
14. Bring plant to closest achievable P value
15. OUTPUT TO GENERATING UNITS: P_sp_new pushed across units via internal network (Modbus/61850)
16. Fast ring settling check (O.7.3.1): actual P at POC must settle within +/-5% of P_sp_new within TsP (time explicitly stated as 'within the time prescribed in O.7.3.1')
17. External Set-Point Dynamics (O.7.3.3): next update accepted only if >= 3 s since last processed set-point - faster updates REJECTED
18. P measurement fed back to Aggregator (instantaneous P at POC, per O.8.5)
19. Hold current P_sp until next valid Aggregator command (>=3s later) or priority override
20. Higher-priority function (index 1-3) already active
21. No conflicting higher-priority function

---

## V.ctrl.Q.on.DSO.NotConfirmed.drawio

1. Command origin: DSO via Eth_A interface (O.13.1.2) Voltage control mode iv (O.9.1.4)
2. Operating status Range: -1=Operating / -5=Not-Operating Default: -5
3. Status = -1 (Operating)?
4. Function idle - no Q set-point applied
5. Activation command Range: -5=Inactive / -1=Active Default: -5
6. Command = -1 (Active)?
7. Function status Range: 0=Not available / 2=Automatic(Priority) Default: 2 - always externally driven, no Autonomous option
8. Priority check - Table 1: this function = index 5 (mid priority - beats index 6 defaults and index 7 Aggregator Q, loses to 1-4)
9. Not activated - existing function stays in control
10. Feed-in / feed-out reactive power target Range: 0..100% of Smax (+capacitive / -inductive) Default: 0 / 0
11. Trigger equation: NONE - direct target tracking, no formula (identical to O.10.3.2, DSO-triggered instead)
12. Distribute Q target across plant elements (method left to User / plant designer)
13. Can plant capability reach requested Q?
14. Bring plant to closest achievable Q value
15. OUTPUT TO INVERTERS: Q_sp_new pushed across generating units via internal network (Modbus/61850)
16. Fast ring settling check (O.7.3.1): actual Q at POC must settle within +/-5% of Q_sp_new within TsQ <= 10 s
17. External Set-Point Dynamics (O.7.3.3): next update accepted only if >= 3 s since last processed set-point - faster updates REJECTED
18. Q measurement fed back to DSO (instantaneous Q, per O.8.4/61850)
19. Hold current Q_sp until next valid DSO command (>=3s later) or priority override
20. Higher-priority function (index 1-4) already active
21. No conflicting higher-priority function

---

## Keywords

flowchart, Wlim, WSd, W110, O.9.2.1, O.9.2.2, O.9.2.3, O.11, VArV, PFW, PFSP, VArSd, MSD, Q(V), cosphi, Smax, PF2, CEI 0-16, ccli-flowcharts-drawio-extract
