#pragma once

/**
 * Operatore Abilitato — Operating Rule parameter catalog (Annex O / T alignment).
 *
 * Eth_B 104 exposes a subset of POC quantities. DSO mandatory set (T.3.1.3) is
 * published on Eth_A MMS; operator 104 mapping follows the same engineering names
 * where the Operating Rule enables them.
 *
 * mode monitor_only: GI + configured monitor IOAs only; command ASDUs rejected + logged (O.14).
 * mode command: allow listed command type IDs (future MSD / arming profile).
 */

#include <string>

namespace cci::regulation::iec104::operator_rule {

inline constexpr const char* kModeMonitorOnly = "monitor_only";
inline constexpr const char* kModeCommand = "command";

/** Annex-aligned monitor points (configure IOA per site Operating Rule). */
struct MonitorPointDef {
    const char* id;
    const char* asdu_name;
    int         default_type_id;
    const char* unit;
    const char* clause;
};

inline constexpr MonitorPointDef kAnnexMonitorCatalog[] = {
    {"TotW", "M_ME_NC_1", 13, "kW", "O.8.3 — P,Q at POC (4 s block)"},
    {"TotVAr", "M_ME_NC_1", 13, "kVAr", "O.8.3 — Q at POC"},
    {"PPV", "M_ME_NC_1", 13, "kV", "T.3.1.3 — V at POC"},
    {"CommsStatus", "M_SP_NA_1", 1, "-", "O.14 — Eth_B physical/data-link status"},
};

/** Command types — disabled unless operator_rule_mode=command and allow_commands. */
struct CommandTypeDef {
    const char* id;
    int         type_id;
    const char* clause;
};

inline constexpr CommandTypeDef kAnnexCommandCatalog[] = {
    {"GI", 100, "C_IC_NA_1 — supervision (O.13 Eth_B)"},
    {"Setpoint", 50, "C_SE_NC_1 — O.10.3.1 active power set-point (MSD)"},
    {"SingleCommand", 45, "C_SC_NA_1 — O.10.3.1 / plant command class"},
    {"ClockSync", 103, "C_CS_NA_1 — O.13.5 time sync (yaml-gated)"},
};

inline bool mode_allows_commands(const std::string& mode, bool allow_commands_flag) {
    return allow_commands_flag && mode == kModeCommand;
}

/** Command ASDU types allowed when command mode is on (O.10.3.1 / supervision). */
inline bool is_allowed_command_type(int type_id) {
    switch (type_id) {
    case 45:   /* C_SC_NA_1 */
    case 50:   /* C_SE_NC_1 — O.10.3.1 MSD active-power SP */
    case 103:  /* C_CS_NA_1 */
        return true;
    default:
        return false;
    }
}

}  // namespace cci::regulation::iec104::operator_rule
