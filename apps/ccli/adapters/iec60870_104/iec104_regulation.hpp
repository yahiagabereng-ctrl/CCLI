#pragma once

/**
 * CEI 0-16 / IEC normative traceability — Phase 4 operator Eth_B (60870-5-104).
 *
 * Annex T applies to DSO MMS on Eth_A (:3782), not this conduit.
 *
 * | Gate   | Regulation              | Implementation anchor              |
 * |--------|-------------------------|------------------------------------|
 * | P4-01  | O.13.1.1.1 Eth_B only   | bind_address guard, no Eth_A :2404 |
 * | P4-02  | IEC 60870-5-104         | CS104 slave, STARTDT, GI, M_ME_NC_1|
 * | P4-03  | IEC 62351-3 (+62351-5)  | CS104_Slave_createSecure, TLS 1.2+ |
 * | P4-04  | O.13.1.2 / O.13.1.3     | comms_loss_fallback_s, Eth_B MSD   |
 * | P4-05  | O.14                    | param audit accept/reject + MSD stub |
 *
 * Operatore Abilitato (Eth_B): Operating Rule in yaml (monitor_only default).
 * Allowed monitor IOAs/clauses: iec104_operator_rule.hpp. Commands gated until
 * operator_rule_mode=command. DSO commands on Eth_A (Annex T).
 */

namespace cci::regulation::iec104 {

inline constexpr const char* kPhaseGate = "P4";
inline constexpr const char* kConduit = "Eth_B / LAN2 operator";

// Annex O — communication interfaces
inline constexpr const char* kAnnexO_EthB = "O.13.1.1.1";  // operator / aggregator port
inline constexpr const char* kAnnexO_Fallback = "O.13.1.2";  // DSO channel (Eth_A) — MMS
inline constexpr const char* kAnnexO_EthB_Fallback = "O.13.1.3";  // aggregator/BSP Eth_B lost
inline constexpr const char* kAnnexO_Logging = "O.14";  // operator command log

// Standards
inline constexpr const char* kStd104 = "IEC 60870-5-104";
inline constexpr const char* kStd62351_3 = "IEC 62351-3";  // TLS transport (P4-03)
inline constexpr const char* kStd62351_5 = "IEC TS 62351-5";  // STAS — optional / deferred

}  // namespace cci::regulation::iec104
