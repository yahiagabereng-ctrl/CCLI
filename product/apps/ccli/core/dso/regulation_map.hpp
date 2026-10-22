#pragma once



/**

 * @file regulation_map.hpp

 * @brief Phase 1 master regulation map — clause IDs, equation refs, traceability rows.

 *

 * Runtime: regulation_bench uses kRegulationMasterMap for row skeletons.

 * Docs: lab/PF2_REGULATION_PHASE1.md · lab/CCI_Figura2_Parameters.md

 */



#include <cstddef>

#include <cstdint>



namespace cci::core::dso {



/* --- Normative clause strings (Annex O / TR) --- */

constexpr const char* kClauseO_8_2 = "O.8.2";

constexpr const char* kClauseO_8_3 = "O.8.3";

constexpr const char* kClauseO_9_1 = "O.9.1";

constexpr const char* kClauseO_9_2_1 = "O.9.2.1";

constexpr const char* kClauseO_9_2_2 = "O.9.2.2";

constexpr const char* kClauseO_9_2_3 = "O.9.2.3";

constexpr const char* kClauseO_7_3_1 = "O.7.3.1";

constexpr const char* kClauseO_7_3_3 = "O.7.3.3";

constexpr const char* kClauseO_11 = "O.11";

constexpr const char* kClauseO_13_1_2 = "O.13.1.2";

constexpr const char* kClauseO_14 = "O.14";

constexpr const char* kClauseAnnexM = "Annex M";

constexpr const char* kClauseTR57126 = "TR 57-126 Table 1";

constexpr const char* kClauseAnnexT = "Annex T";



/* --- Lab FSM / bench (L01–L04) --- */

constexpr const char* kLabL01 = "L01";

constexpr const char* kLabL02 = "L02";

constexpr const char* kLabL03 = "L03";

constexpr const char* kLabL04 = "L04";



/** Equation labels in PF2_REGULATION_PHASE1.md */

constexpr const char* kEq1 = "Eq (1)";

constexpr const char* kEq2 = "Eq (2)";

constexpr const char* kEq3 = "Eq (3)";

constexpr const char* kEq4 = "Eq (4)";

constexpr const char* kEq5_6 = "Eq (5)(6)";

constexpr const char* kEq7_8 = "Eq (7)(8)";

constexpr const char* kEq9 = "Eq (9)";

constexpr const char* kEq13_17 = "Eq (13)–(17)";

constexpr const char* kEq18 = "Eq (18)";



/** Phase 1 implementation status for master-map rows (not runtime FSM state). */

enum class RegulationP1Status : std::uint8_t {

    Pass,

    Part,

    NotImpl,

    Na,

};



struct RegulationMapEntry {

    const char* id;

    const char* clause;

    const char* name;

    const char* equation;

    RegulationP1Status default_p1;

};



/** Master map R01–R15 + L01–L04 — see lab/PF2_REGULATION_PHASE1.md table */

constexpr RegulationMapEntry kRegulationMasterMap[] = {

    {"R01", kClauseO_8_2, "Smax polygon nameplate", kEq1, RegulationP1Status::Pass},

    {"R02", kClauseO_9_2_2, "Wlim WMaxSptPct mock", kEq2, RegulationP1Status::Pass},

    {"R03", kClauseO_9_2_3, "WSd WSptPct mock", kEq2, RegulationP1Status::Pass},

    {"R04", kClauseO_9_2_1, "110% Vn autonomous P cap", kEq2, RegulationP1Status::Pass},

    {"R05", kClauseO_11, "Active P min() combination", kEq5_6, RegulationP1Status::Pass},

    {"R06", kClauseO_7_3_1, "TsP ±5% settling band", kEq7_8, RegulationP1Status::Pass},

    {"R07", kClauseO_7_3_3, "3 s min between set-points", kEq9, RegulationP1Status::Pass},

    {"R08", kClauseO_13_1_2, "DSO Eth_A loss / Operating Rule", "-", RegulationP1Status::Pass},

    {"R09", kClauseO_8_3, "TotW 4 s measurement blocks", "-", RegulationP1Status::Pass},

    {"R10", kClauseO_9_1, "Reactive VArV", "Eq (10)", RegulationP1Status::Na},

    {"R11", kClauseO_9_1, "Reactive PFW", "Eq (11)", RegulationP1Status::Na},

    {"R12", kClauseO_9_1, "Reactive PFSP", "Eq (12)", RegulationP1Status::Na},

    {"R13", kClauseO_9_1, "VArSd/VArSa direct Q SP", kEq3, RegulationP1Status::Na},

    {"R14", kClauseO_14, "Data logger capacity", "-", RegulationP1Status::Pass},

    {"R15", kClauseAnnexM, "Teledistacco DIO inhibit", "-", RegulationP1Status::Na},

    {"L01", kLabL01, "PF2 enter threshold + debounce", kEq13_17, RegulationP1Status::Pass},

    {"L02", kLabL02, "PF2 release hysteresis", kEq13_17, RegulationP1Status::Pass},

    {"L03", kLabL03, "Stale meter fail-safe", kEq13_17, RegulationP1Status::Pass},

    {"L04", kLabL04, "Modbus slave P ramp mock", kEq18, RegulationP1Status::Pass},

};



constexpr std::size_t kRegulationMasterMapCount =

    sizeof(kRegulationMasterMap) / sizeof(kRegulationMasterMap[0]);



}  // namespace cci::core::dso

