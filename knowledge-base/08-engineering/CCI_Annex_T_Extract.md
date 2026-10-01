# CEI 0-16 Annex T — IEC 61850 / Cyber Engineering Extract (English)

**Document ID:** CCLI-GRID-ANNEX-T-001  
**Revision:** 2.0  
**Date:** 2026-09-16  
**RAG source_id:** `cei-0-16-allegato-t`  
**Normative source:** CEI 0-16 consolidated working edition **2025-12** (`knowledge-base/07-protocols/0-16consolidata.pdf`) — **Allegato T** (normative), doc. pp. 534–653 / PDF pp. 541–660  
**Prior English baseline:** CT 316 / CT 57 extract (2024-12)  
**Companion:** `CCI_Annex_O_Extract.md` · `CCI_CEI_0-16_Consolidated.md`  
**Programme link:** K7.2 · K4.3 ICD · REQ-61850-001

---

## Summary

Annex T specifies the **CCI–DSO (and Aggregator) IEC 61850 interface**: MMS timing (Type 3 ≈ 500 ms), logical device/node/data-object model, ACSI services, RBAC, PKI/TLS cyber profile, and logging. Plant-internal communication is **out of scope**.

**Corpus status:** **HAVE (English)** for normative body. **SCL/CID example** is in separate **CEI TR 57-126** — see §1.1.

---

## 1.1 SCL / ICD reference — **CEI TR 57-126** *(INGESTED)*

Annex T §T.3 states (consolidated PDF, doc. p. 535):

> For the concrete implementation of the CCI communication mode, reference may be made to **CEI TR 57-126** — *"Example of SCL file for IEC 61850 communication of the CCI"* (*Esempio di file SCL per la comunicazione IEC 61850 del CCI*).

| Item | Status |
|------|--------|
| `0-16consolidata.pdf` | **ON DISK** — contains Annex T tables and cyber spec |
| **CEI TR 57-126** (SCL/CID example) | **INGESTED** — `source_id`: `cei-tr-57-126` → `CCI_TR_57-126_Extract.md` |
| Example CID | **ON DISK** — `apps/ccli/config/icd/cei-tr-57-126-example.cid` |

Also introduced in **Variant V1 (2022-11)**: Allegato T explicitly references this TR (preface, consolidated doc. p. 3).

---

1. Purpose and Scope of This Document
This document consolidates every mandatory numerical and structural requirement stated in Annex T of CEI 0-16:2023-05 V2 ("Information exchange based on IEC 61850") that the CCI AiLux firmware must respect: communication timing and performance classes, measurement periodicity, IEC 61850 data-model structure (Logical Devices/Nodes/Data Objects), ACSI services, role-based access control, and the full cybersecurity profile (PKI, TLS, certificate management, logging, time sync, network segregation).
Annex T applies specifically to the CCI–DSO (and CCI–Aggregator) IEC 61850 interface; it does not define communication to the elements inside the plant. Unless noted otherwise, every value below is a hard normative requirement ("shall"), not a design suggestion.
The Annex T data model is split into three independent sections, each separately optional/mandatory:
Observability — mandatory (unless a specific Data Object is marked Optional in its own table).
Control — entirely Optional (implemented only if the CCI supports remote control by the DSO).
Participation in the Dispatching Services Market (Aggregator interface) — entirely Discretionary.
2. IEC 61850 Communication Timing (T.3.2.1)
The IEC 61850 server implemented by the CCI is a single logical access point (one IP address). All info below governs the client/server (MMS) exchange between CCI and DSO/Aggregator.
Data / Function
Sending mode
Performance class (IEC 61850-5)
Presence
Plant characteristics (static + dynamic config)
On request
Type 3 – low speed messages
Observability, Mandatory
Plant operating status
On request + on change
Type 3 – low speed messages
Observability, Mandatory
Plant measurements
Periodic, every 4 s
Type 3 – low speed messages
Observability, Mandatory
Operating parameter values (control setpoints)
On request + on change
Type 3 – low speed messages
Control, Optional
Performance Class Type 3 ≈ P5 ("low speed messages"); expected end-to-end transit time ≈ 500 ms inside the substation (IEC 61850-5, §11.2.2/11.2.3). Outside the substation these exchanges are not catalogued by the standard.
Power set-point (P/Q) latency is explicitly required as Performance Class Type 3.
The CCI is not required to publish GOOSE, but shall be able to subscribe GOOSE messages relating to control functions; any such high-speed exchange is required as Performance Class Type 1.
Mapping: IEC 61850-8-1 over MMS (two-party Client/Server application association), compatible with "Type 2/Type 3" typologies of §5.1 IEC 61850-8-1.
Source (Annex T): T.3.2.1 "Communication modes"; T.3.3.3 "Communication Protocol Mapping".
3. Measurement Data Model & Periodicity (T.3.1.3 / T.3.3.1.1.7)
Sampling/transmission period: every 4 s for all measurements below (MMXU logical node, DA "mag").
Measurement point
Quantity
Unit
IEC 61850 mapping
Presence
Point of Delivery (PdC)
Active power (signed)
kW
MMXU1 (prefix PdC) . TotW
Mandatory
Point of Delivery (PdC)
Reactive power (signed)
kVAr
MMXU1 (prefix PdC) . TotVAr
Mandatory
Point of Delivery (PdC)
Phase-to-phase voltages
kV
MMXU1 (prefix PdC) . PPV
Mandatory
Point of Delivery (PdC)
Phase currents
A
MMXU1 (prefix PdC) . A
Optional
Generation aggregate (PV/Wind/Thermal/Hydro)
Total active power per source
kW
MMXU2 (prefix GenPV/GenWi/GenTer/GenIdr) . TotW
Mandatory
Storage aggregate
Active power
kW
MMXU2 (prefix St) . TotW
Mandatory
Single Generating Group (SGG), N = 1..99
Active power
kW
MMXU2 (prefix SGG) . TotW
Mandatory
Source (Annex T): T.3.1.3 "Information on measurements of electrical quantities"; T.3.3.1.1.7 "Plant measurements".
4. Characteristic Power Vector (Table 80) & Nameplate Data Objects
Used both for static nameplate data at the PdC and as the p.u. reference base (Smax) for every control setpoint below.
Information
Unit
IEC 61850 mapping (DPCC, prefix)
Maximum active power input (generation)
kW
DPCC1 (PdC_Wi) . WRtg
Maximum active power in absorption
kW
DPCC1 (PdC_Wa) . WRtg
Maximum inductive reactive power
kVAr
DPCC2 (PdC_Qi) . VArRtg
Maximum capacitive reactive power
kVAr
DPCC2 (PdC_Qc) . VArRtg
Maximum apparent system power (Smax)
kVA
DPCC3 (PdC_VA) . VARtg
Source (Annex T): T.3.1 Table 80; T.3.3.1.1.3 "Operating Characteristics at the Connection Point".
5. Control-Function Parameters, Ranges and Defaults (T.3.1.4)
All Control, Optional unless marked Discretionary (MSD participation). Every percentage is referred to Smax unless stated otherwise.
5.1 Active Power Limitation (Table 85 — LN DWMX, prefix Wlim)
Parameter
Unit
Range
Default
Operating status
–
1 = Operating / 5 = Not-Operating
5
Active power limit in generation mode
%
0..100 (ref. Smax)
0
Activation command
–
5 = Inactive / 1 = Active
5
Setpoint function status (from DSO)
–
0 = Not available / 1 = Autonomous / 2 = Automatic (Priority)
1
Limit function status, P 110%
–
0 = Not available / 1 = Autonomous
2
5.2 Modulation of Active Power (Table 86 — LN DAGC, prefix WSd)
Parameter
Unit
Range
Default
Operating status
–
1 = Operating / 5 = Not-Operating
5
Feed-in / feed-out setpoint
%
0..100 (+feed-in / -feed-out, ref. Smax)
100 / 0
Activation command
–
5 = Inactive / 1 = Active
5
Function status
–
0 = Not available / 2 = Automatic (Priority)
2
5.3 Active Power Set-Point — MSD participation (Table 87 — LN DAGC, prefix WSa) — Discretionary
Parameter
Unit
Range
Default
Operating status
–
1 = Operating / 5 = Not-Operating
5
Feed-in / feed-out setpoint
%
0..100 (+feed-in / -feed-out, ref. Smax)
100 / 0
Activation command
–
5 = Inactive / 1 = Active
5
Function status
–
0 = Not available / 2 = Automatic (Priority)
2
5.4 Voltage Control with Reactive Power (Table 88 — LN DVAR, prefix VArSd)
Parameter
Unit
Range
Default
Operating status
–
1 = Operating / 5 = Not-Operating
5
Feed-in / feed-out reactive power
%
0..100 (+capacitive / -inductive, ref. Smax)
0 / 0
Activation command
–
5 = Inactive / 1 = Active
5
Function status
–
0 = Not available / 2 = Automatic (Priority)
2
5.5 Reactive Power Set-Point — MSD participation (Table 89 — LN DVAR, prefix VArSa) — Discretionary
Parameter
Unit
Range
Default
Operating status
–
1 = Operating / 5 = Not-Operating
5
Feed-in / feed-out reactive power
%
0..100 (+capacitive / -inductive, ref. Smax)
0 / 0
Activation command
–
5 = Inactive / 1 = Active
5
Function status
–
0 = Not available / 2 = Automatic (Priority)
2
5.6 Fixed Power Factor — cosφ (Table 90 — LN DFPF, prefix PFSP)
Parameter
Unit
Range
Default
Operating status
–
1 = Operating / 5 = Not-Operating
5
Cosφ setpoint, generation
P.U.
-1.00..+1.00 (+cap / -ind)
-0.95
Cosφ setpoint, absorption
P.U.
-1.00..1.00 (+cap / -ind)
0.95
Activation command
–
5 = Inactive / 1 = Active
5
Function status
–
0 = Not available / 1 = Autonomous / 2 = Automatic (Priority)
1
5.7 Reactive Power Control, curve Q = f(V) (Table 91 — LN DVVR, prefix VArV)
Parameter
Unit
Range
Reference
Default
Operating status
–
1 = Operating / 5 = Not-Operating
–
5
Activation command
–
5 = Inactive / 1 = Active
–
5
Function status
–
0 = Not avail. / 1 = Autonomous / 2 = Automatic (Priority)
–
1
K
–
-1.00..1.00
–
0
Active power lock-in
P.U.
0.00..max
Nominal active power
0.20
Active power lock-out
P.U.
0.00..max
Nominal active power
0.05
V threshold, higher 1
P.U.
0.00..max
Nominal voltage at PdC
1.08
V threshold, lower 1
P.U.
0.00..max
Nominal voltage at PdC
0.92
V threshold, higher 2
P.U.
0.00..max
Nominal voltage at PdC
1.10
V threshold, lower 2
P.U.
0.00..max
Nominal voltage at PdC
0.90
5.8 Reactive Power via cosφ(P) curve (Table 92 — LN DPFW, prefix PFW)
Parameter
Unit
Range
Reference
Default
Operating status
–
1 = Operating / 5 = Not-Operating
–
5
Activation command
–
5 = Inactive / 1 = Active
–
5
Function status
–
0 = Not avail. / 1 = Autonomous / 2 = Automatic (Priority)
–
1
P value (point A)
P.U.
0.00..max
Nominal active power
0.20
Cosφ value (point A)
P.U.
-1.00..-0.1 / +0.1..1.00 (+cap/-ind)
–
1.00
P value (point B)
P.U.
0.00..max
Nominal active power
0.50
Cosφ value (point B)
P.U.
-1.00..-0.1 / +0.1..1.00 (+cap/-ind)
–
1.00
P value (point C)
P.U.
0.00..max
Nominal active power
1.00
Cosφ value (point C)
P.U.
-1.00..-0.1 / +0.1..1.00 (+cap/-ind)
–
0.95
Voltage lock-in
P.U.
1.00..1.10
Nominal voltage at PdC
1.05
Voltage lock-out
P.U.
0.90..1.00
Nominal voltage at PdC
0.98
Source (Annex T): T.3.1.4.1 – T.3.1.4.8; T.3.3.1.2 "Control Data Models".
6. IEC 61850 Data Model Structure (T.3.3.1)
Element
Value
Namespace name
"(Tr) IEC 61850-CEI016:2022"
Namespace version / revision
2022 / 1
Namespace release date
1-04-2022
Logical Device
Single LD: LD_Plant (contains all Logical Nodes)
LN prefixes by plant section
Global (whole plant), St (storage), GenPV, GenWi, GenTer, GenIdr
Single Generating Unit (SGG) instances
Multi-instance, N = 1..99
Mandatory associated attributes
q (quality) and t (timestamp), wherever required by IEC 61850
Presence-condition attribute values used throughout the CCI data tables (extension to standard M/O/C):
M = Mandatory (prescribed by the standard).
O = Optional (provided by the standard).
C = Conditional (per standard-defined conditions).
R = Required (standardized as O/C by IEC 61850, but required to enable Annex O functions).
E = Extension (not present in the base standard, required for Annex O functions).
F = Forbidden (not applicable for this presence condition).
Source (Annex T): T.3.3.1 "IEC 61850 data model of information associated with the CCI"; Table 94, Table 95, Table 96.
6.1 Key Logical Nodes Reference
LN class
Typical prefix
Function
LPHD1
–
Physical device identity: PhyNam (vendor, swRev, location = POD identifier)
DPCC1/2/3
PdC_Wi, PdC_Wa, PdC_Qi, PdC_Qc, PdC_VA
Nameplate power vector at PdC (§4)
DECP1
DisFR
Overall plant readiness for control functions (Beh)
DGEN1 / DSTO1
DisFR
Generation / storage macrogroup readiness for control
MMXU1 / MMXU2
PdC, GenPV/Wi/Ter/Idr, St, SGG
Measurements (§3)
XCBR1
IDG
Main power plant breaker position (Pos, Open/Closed)
DGEN2
SSGG
Single Generating Unit operating status (Health) + ID (GnGrId), N=1..99
DWMX1 / DAGC1 / DVAR1 / DFPF1 / DVVR1 / DPMC1 / DECP2 / DPFW1
Wlim, WSd/WSa, VArSd/VArSa, PFSP, VArV, PFW
Control functions, §5
7. ACSI Services and Access Privileges (T.3.3.2 / T.3.3.4.3)
The IEC 61850 server shall implement, at minimum, the following ACSI service classes (IEC 61850-7-2), each mapped to a privilege:
ACSI class
Services
Privilege
Server / Association / LogicalDevice
GetServerDirectory, Release, Abort, GetLogicalDeviceDirectory
Listobjects
Logical Node
GetLogicalNodeDirectory, GetAllDataValues
Listobjects, Readvalues
Data Object
GetDataValues, SetDataValues, GetDataDirectory, GetDataDefinition
Readvalues, Control/config, Listobjects
DataSet
GetDataSetValues, GetDataSetDirectory (Set/Create/Delete NOT applicable)
Dataset
Buffered / Unbuffered Report Control Block
Report, GetBRCBValues/GetURCBValues (Set NOT applicable)
Reporting
Two custom mandatory roles (IEC 62351-8) must be implemented in addition to the seven standard roles:
Role
Role-ID
Revision
Privileges
DSO_OPERATOR
-1
1.0
LISTOBJECTS, READVALUES, DATASET (read-only), REPORTING (write only for report enable), CONTROL + CONFIG on DSO-reserved Data Objects (Table 98)
AGGREGATOR_OPERATOR
-2
1.0
Same structure, restricted to Aggregator-reserved Data Objects (Table 99)
Access token formats: profile A (X.509 public-key certificate extension) or profile B (X.509 attribute certificate) mandatory; profile C (JSON web token) and D (RADIUS) optional.
Attribute Certificates (profile B / PMI): recommended short validity, < 24 h, to limit replay risk.
Non-DSO/Aggregator authenticated subjects get privileges consistent only with their assigned role — no implicit escalation.
Source (Annex T): T.3.3.2 "ACSI services"; T.3.3.4.3 "Role Management"; Tables 97–102.
8. Cybersecurity Profile (T.3.3.4)
8.1 Time Synchronization (T.3.3.4.5)
Parameter
Requirement
Time reference
UTC (Coordinated Universal Time)
Reference-time uncertainty
≤ ± 100 ms
Max clock deviation between MMS/E2E association peers
10 minutes — beyond this, the association SHALL fail
Network sync protocol
NTP, client, UDP port 123
Secure variant
Mandatory: NTS (IETF RFC 8915), TLS-based authentication
Server redundancy
Redundant NTP architecture required, to protect against "fake ticker" servers (n false tickers tolerated ⇒ 2n+1 servers)
8.2 Transport Profile — TLS (IEC 62351-3)
Parameter
Requirement
Default TCP port (T-profile with TLS)
3782
Minimum TLS version
v1.2
Mandatory cipher suites
TLS_RSA_WITH_AES_128_CBC_SHA256; TLS_DHE_RSA_WITH_AES_128_GCM_SHA256; TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
Also required, disabled by default
TLS_RSA_WITH_NULL_SHA256 (no encryption; explicit enable procedure required)
TLS renegotiation interval
Aligned to CRL update period (≥ half of it); never < 10 minutes
TLS session resumption
At least every 2 hours; shorter than renegotiation interval; aligned to CRL update
Root certificates (CAs) supported
≥ 5 different
Max public-key certificate size
8192 octets (must be handled at least up to this size)
CRL update frequency
≤ 24 hours
OCSP response cache
≤ 24 hours (active session not terminated solely for exceeding this)
8.3 Cryptographic Algorithms and Key Lengths (T.3.3.4.1)
Element
Requirement
RSA key length
Minimum 2048 bit (recommended ≥ 3072 bit)
ECDSA key length
Minimum 256 bit (recommended ≥ 384 bit)
Hash (digital signature / ICV)
SHA-256
Digital signature algorithms
RSA-with-SHA256; ECDSA-with-SHA256
Integrity check (MAC)
hmacWithSHA256
Symmetric encryption
aes128-CBC / aes256-CBC; aes128-GCM / aes256-GCM (also give integrity+auth)
Diffie-Hellman groups
DH-14 (2048-bit, with RSA); ECDH: DH-23 (secp256r1) and DH-28 (brainpoolP256r1)
Application profile
End-to-End (E2E) security mandatory, per IEC 62351-4 (association + data-transfer phase)
8.4 GOOSE Communication Security (T.3.3.4.2, informative)
GOOSE confidentiality is not mandatory (latency constraints), but integrity/authentication is expected via extended PDU (Reserved1/Reserved2 fields), HMAC-SHA256 or AES-GMAC, truncatable to 128/256 bit.
Group Key Management Protocol distributes current + next key with validity time references.
StNum/SqNum of received PDUs should be checked to detect potential replay; a PDU with lower values than previously received should be discarded even if multipath communication could explain it.
8.5 PKI — Certificate & Key Lifecycle (T.3.3.4.9)
CCI shall generate/receive new key pairs when: no key pair present at start-up; change of controllership/ownership; command from an authorised entity; or suspected private-key compromise.
Enrolment protocol: at least one of SCEP (IETF RFC 8894, legacy pre-2015 versions excluded) or EST (IETF RFC 7030, over TLS 1.2+) shall be supported.
Certificate validity checking: both CRL and OCSP (RFC 6960) methods shall be supported; a nonce is mandatory in OCSP requests to prevent replay.
Trust Anchors required at minimum: DSO's CA, Administrative Domain's CA, Manufacturer's CA. Optional: firmware-update signing CA, Aggregator's CA, PKI-service TLS CA (e.g. for EST).
Certificate revocation grounds: suspected private-key compromise (device or CA), changed entity affiliation, replaced public-key certificate, CCI decommissioning, withdrawn role/privilege, suspected Attribute Authority key compromise.
8.6 Secure Boot & Firmware Update (T.3.3.4.8)
Trusted, phased secure boot sequence: each stage's validity verified before the next stage is installed/initialised.
Firmware update procedure mandatory steps: (1) verify updating user's credentials/authorisation; (2) verify full integrity and authenticity of new firmware via digital signature against the manufacturer's certificate public key; (3) controlled deactivation of CCI functions during update; (4) log the update activity — the security data logger itself must never be erased by this procedure.
8.7 Security Event Logging (T.3.3.4.6)
Parameter
Requirement
Storage model
Sequential circular buffer (FIFO), not deletable/modifiable
Minimum stored events before overwrite
2048 events (per IEEE 1686:2013)
Remote transmission
Syslog, RFC 5424/5425, over TLS
SNMP alternative
SNMPv3 with TSM profile (RFC 5591) over TLS (RFC 6353); Syslog→SNMP mapping per RFC 5676
Event severity categories
alarm (unauthorised activity) / error / notice (authorised activity) / warning (abnormal but not necessarily malicious)
Event categories logged (non-exhaustive; full mnemonic tables in Annex T §T.3.3.4.6.2): login/logout and authentication outcomes; user/role/permission management; file hash & digital-signature check failures; software update outcome; TLS session and certificate-validation events (including expired/revoked/untrusted certificate, CRL/OCSP unavailable or expired); MMS association handshake failures (E2E SecPDU reject/abort with diagnostic code); certificate lifecycle events (enrolment, renewal, revocation, CertAVL); RBAC/access-token validation failures.
8.8 Traffic Segregation and Local Interface Security (T.3.3.4.10 / T.3.3.4.11)
Remote-access segregation via router devices supporting NAT, VLAN, firewalling and VPN with channel encryption; any public-network connectivity must use a secure VPN, reserved exclusively for plant control/operation communications.
All local (non-network) commissioning/configuration communication must be protected by a user-authentication system under specific security policies.
User (non-DSO/Aggregator) remote access: standard protocols with confidentiality/authenticity/integrity (SSH or HTTPS), cryptographic credentials, mutual authentication; a segregation-of-duty role system limits access to CCI function/parameter subsets.
Source (Annex T): T.3.3.4.1 – T.3.3.4.11 in full; Tables accompanying each subclause.
9. Cross-Check Against Annex O Open Items
This section maps each open item listed in §13 of the companion Annex O document to where it is now resolved.
Annex O open item
Resolved in this document
Complete IEC 61850 logical-node / data-model mapping for all measurements, signals and commands
§3 (measurements), §4 (nameplate vector), §5 (control functions), §6 (LN/LD structure)
Full cybersecurity architecture: PKI, authentication, Asset Inventory API
§8.5 (PKI/certificates), §7 (roles/ACSI), §8.7 (logging); Asset Inventory identified via LPHD.PhyNam (T.3.3.4.7)
GOOSE/MMS specific timing performance classes beyond O.7.3
§2 (Type 3 = ~500 ms; Type 1 for GOOSE subscribe)
Detailed VPN/private-network requirements for Eth_A/Eth_B
§8.8 (traffic segregation: NAT/VLAN/firewall/VPN with channel encryption)
**SCL/CID baseline:** **CEI TR 57-126** ingested — `CCI_TR_57-126_Extract.md` + `apps/ccli/config/icd/cei-tr-57-126-example.cid`. DSO-specific workbook may still differ from the reference CID.
Together with the Annex O companion document, this file constitutes the complete mandatory-requirements baseline extracted from CEI 0-16 for the CCI AiLux firmware, as of the source publication date (2024-12).
