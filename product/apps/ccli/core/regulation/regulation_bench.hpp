#pragma once

/**
 * @file regulation_bench.hpp
 * @brief CEI / TR 57-126 reference values vs config and optional live plant readings.
 *
 * Maps rows to lab/PF2_REGULATION_PHASE1.md and lab/CCI_Figura2_Parameters.md.
 * Used by: ccli --regulation-check [--live] [--json]
 */

#include "config/ccli_config.hpp"
#include "measurement/measurement_store.hpp"
#include "pf2/pf2_fsm.hpp"

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace cci::core::regulation {

enum class RowStatus : std::uint8_t {
    Pass,
    Fail,
    Pending,
    NotImpl,
    Na,
    Part,
};

struct RegulationRow {
    std::string id;           /**< e.g. R01, L01, TR-T1 */
    std::string clause;       /**< O.8.2, TR 57-126 Table 1 */
    std::string name;
    std::string expected;     /**< normative / calculated reference */
    std::string actual;       /**< yaml, computed, or live */
    std::string unit;
    RowStatus status{RowStatus::Pending};
    std::string notes;
};

struct ModbusLiveTrace {
    bool        valid{false};
    int         function_code{3};
    int         slave_id{1};
    int         reg_start{0};
    int         reg_count{0};
    std::string command;       /**< human-readable master PDU summary */
    std::string raw_regs_hex;
    double      p_kw{0.0};
    double      q_kvar{0.0};
    bool        success{false};
    std::string error;
    std::int64_t exchange_ms{0};
    std::string backend;
};

struct LiveSnapshot {
    bool valid{false};
    double p_kw{0.0};
    DataQuality quality{DataQuality::Invalid};
    bool permissive_ok{false};
    bool permissive_bypass{false};
    Pf2State pf2_state{};
    bool curtailment_commanded{false};
    bool do_curtail_on{false};
    ModbusLiveTrace modbus_last{};
    std::vector<ModbusLiveTrace> modbus_log;
    double p_effective_export_kw{0.0};
    double pf2_enter_kw{0.0};
    double pf2_release_kw{0.0};
    int    modbus_poll_ms{1000};
    int    modbus_timeout_count{0};
};

struct RegulationReport {
    std::string config_path;
    std::vector<RegulationRow> rows;
    LiveSnapshot live{};
};

/** Build full report from config + DSO apply (no I/O). */
RegulationReport build_regulation_report(const CcliConfig& cfg, const std::string& config_path);

/** Merge live readings + Modbus master trace vs each regulation row. */
void apply_live_snapshot(RegulationReport& report, const LiveSnapshot& live,
                         const CcliConfig& cfg);

void print_regulation_report_text(const RegulationReport& report, std::ostream& out);
void print_regulation_report_json(const RegulationReport& report, std::ostream& out);

const char* row_status_string(RowStatus s);

}  // namespace cci::core::regulation
