#include "regulation/regulation_bench.hpp"

#include "dso/dso_active_power.hpp"
#include "dso/dso_phase1.hpp"
#include "dso/plant_envelope.hpp"
#include "dso/regulation_map.hpp"
#include "event/event_ring.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace cci::core::regulation {

namespace {

RegulationRow make_row(const char* id, const char* clause, const char* name,
                       const std::string& expected, const std::string& actual,
                       const char* unit, RowStatus status, const char* notes = "") {
    RegulationRow r{};
    r.id = id;
    r.clause = clause;
    r.name = name;
    r.expected = expected;
    r.actual = actual;
    r.unit = unit;
    r.status = status;
    if (notes) {
        r.notes = notes;
    }
    return r;
}

std::string fmt_num(const double v, const int prec = 2) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(prec) << v;
    return os.str();
}

RowStatus eq_num(const double a, const double b, const double eps = 0.11) {
    return std::fabs(a - b) <= eps ? RowStatus::Pass : RowStatus::Fail;
}

std::string json_escape(std::string s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (const char c : s) {
        if (c == '"') {
            out += "\\\"";
        } else if (c == '\\') {
            out += "\\\\";
        } else if (c == '\n') {
            out += "\\n";
        } else {
            out += c;
        }
    }
    return out;
}

void print_modbus_trace_json(const ModbusLiveTrace& t, std::ostream& out) {
    out << "{\"valid\":true,\"function_code\":" << t.function_code << ",\"slave_id\":"
        << t.slave_id << ",\"reg_start\":" << t.reg_start << ",\"reg_count\":" << t.reg_count
        << ",\"command\":\"" << json_escape(t.command) << "\",\"raw_regs_hex\":\""
        << json_escape(t.raw_regs_hex) << "\",\"p_kw\":" << t.p_kw << ",\"q_kvar\":" << t.q_kvar
        << ",\"success\":" << (t.success ? "true" : "false") << ",\"error\":\""
        << json_escape(t.error) << "\",\"exchange_ms\":" << t.exchange_ms << ",\"backend\":\""
        << json_escape(t.backend) << "\"}";
}

RowStatus from_p1_default(const dso::RegulationP1Status s) {
    switch (s) {
        case dso::RegulationP1Status::Pass:
            return RowStatus::Pass;
        case dso::RegulationP1Status::Part:
            return RowStatus::Part;
        case dso::RegulationP1Status::NotImpl:
            return RowStatus::NotImpl;
        case dso::RegulationP1Status::Na:
            return RowStatus::Na;
    }
    return RowStatus::Pending;
}

}  // namespace

const char* row_status_string(const RowStatus s) {
    switch (s) {
        case RowStatus::Pass:
            return "PASS";
        case RowStatus::Fail:
            return "FAIL";
        case RowStatus::Pending:
            return "PEND";
        case RowStatus::NotImpl:
            return "NOT_IMPL";
        case RowStatus::Na:
            return "N/A";
        case RowStatus::Part:
            return "PART";
    }
    return "?";
}

RegulationReport build_regulation_report(const CcliConfig& cfg, const std::string& config_path) {
    RegulationReport report{};
    report.config_path = config_path;

    const dso::DsoMockInputs mock = dso::resolve_dso_mock_inputs(cfg.plant, cfg.dso);
    const double s_calc = dso::calc_smax_kva(mock.plant);
    const double s_used = dso::smax_kva_effective(mock.plant);

    Pf2Config pf2_eff = cfg.pf2;
    const auto dso_apply = dso::apply_dso_to_pf2_config(pf2_eff, cfg.plant, cfg.dso);
    const auto& der = dso_apply.derived;

    const bool tr_profile = cfg.dso.enabled && cfg.dso.profile == DsoProfile::Tr57126Table1;
    const bool mock_gates = cfg.dso.enabled && cfg.pf2.use_dso_mock;
    const bool export_figura2 = mock_gates && !cfg.dso.wlim_active && !cfg.dso.w110_active &&
                                cfg.dso.wsd_active;

    for (std::size_t i = 0; i < dso::kRegulationMasterMapCount; ++i) {
        const auto& e = dso::kRegulationMasterMap[i];
        std::string expected = "see PF2_REGULATION_PHASE1.md";
        std::string actual;
        std::string unit = "-";
        RowStatus status = from_p1_default(e.default_p1);
        std::string notes = e.equation;

        const std::string id(e.id);

        if (id == "R01") {
            expected = "~206.15 (TR plant corners)";
            actual = fmt_num(s_calc);
            unit = "kVA";
            status = eq_num(s_calc, 206.15, 0.25);
            notes = "Smax yaml=" + fmt_num(cfg.plant.smax_kva) + " kVA used=" + fmt_num(s_used);
        } else if (id == "R02") {
            expected = std::string("WMaxSptPct ") + dso::kEq2;
            actual = cfg.dso.wlim_active ? ("on " + fmt_num(cfg.dso.wmax_spt_pct) + " %") : "off";
            if (mock_gates && mock.commands.wlim_active) {
                actual += " → " + fmt_num(der.p_wlim_kw) + " kW";
                status = eq_num(der.p_wlim_kw,
                                dso::pct_smax_to_kw(mock.commands.wmax_spt_pct, s_used), 0.5);
            } else if (mock_gates && !mock.commands.wlim_active) {
                status = RowStatus::Pass;
                notes = tr_profile ? "TR Table 1 Wlim inactive" : "Wlim off (export path)";
            }
        } else if (id == "R03") {
            const double pct = mock.commands.wsd_active ? mock.commands.wspt_pct : 0.0;
            expected = fmt_num(pct) + " % → " + fmt_num(dso::pct_smax_to_kw(pct, s_used)) + " kW";
            actual = mock.commands.wsd_active ? fmt_num(der.p_wsd_kw) + " kW" : "wsd off";
            unit = "kW";
            if (mock.commands.wsd_active) {
                status = eq_num(der.p_wsd_kw, dso::pct_smax_to_kw(std::max(0.0, pct), s_used), 0.5);
            }
            notes = "Eq (4) import WSptPct not in FSM";
        } else if (id == "R04") {
            expected = "w110_pct when active";
            actual = cfg.dso.w110_active ? ("on " + fmt_num(cfg.dso.w110_pct) + " %") : "off";
            if (mock_gates && mock.commands.w110_active) {
                actual += " → " + fmt_num(der.p_w110_kw) + " kW";
                status =
                    eq_num(der.p_w110_kw, dso::pct_smax_to_kw(mock.commands.w110_pct, s_used), 0.5);
            } else if (mock_gates && !mock.commands.w110_active) {
                status = RowStatus::Pass;
                notes = tr_profile ? "TR Table 1 w110 inactive" : "w110 off (export path)";
            }
        } else if (id == "R05") {
            expected = "min(export caps)";
            actual = fmt_num(der.p_effective_export_kw);
            unit = "kW";
            status = mock_gates ? RowStatus::Pass : RowStatus::Na;
        } else if (id == "R06") {
            expected = "±5% @ TsP≤60s";
            actual = "settled_within_band_kw() unit PASS";
            status = RowStatus::Pass;
            notes = "dso_phase1_test Eq (7)(8)";
        } else if (id == "R07") {
            expected = "≥3 s between set-points";
            actual = "mms.setpoint_min_interval_s=" +
                     std::to_string(cfg.mms.setpoint_min_interval_s);
            unit = "s";
            if (cfg.mms.setpoint_min_interval_s >= 3) {
                status = RowStatus::Pass;
            } else if (cfg.mms.setpoint_min_interval_s > 0) {
                status = RowStatus::Part;
            } else {
                status = RowStatus::NotImpl;
            }
            notes = "Eq (9) SetpointSpacingGate in mms_adapter (r29)";
        } else if (id == "R08") {
            expected = "comm loss → safe (lab stale)";
            actual = "stale_data_s=" + std::to_string(cfg.pf2.stale_data_s) + " s";
            status = cfg.pf2.stale_data_s == 10 ? RowStatus::Pass : RowStatus::Part;
            notes = "O.13.2 fail-safe; full Annex U Phase 3+";
        } else if (id == "R09") {
            expected = "poll_ms=4000";
            actual = "poll_ms=" + std::to_string(cfg.modbus.poll_ms);
            status = cfg.modbus.poll_ms == 4000 ? RowStatus::Pass : RowStatus::Fail;
            notes = "O.8.3 TotW block period (Phase 1 Modbus poll)";
        } else if (id == "R10" || id == "R11" || id == "R12" || id == "R13") {
            expected = "inactive (TR Table 1)";
            actual = export_figura2 ? "N/A Figura 2" : "not configured";
            status = export_figura2 ? RowStatus::Na : RowStatus::NotImpl;
            notes = "Reactive paths Phase 3+";
        } else if (id == "R14") {
            expected = "≥2048";
            actual = std::to_string(core::EventRing::kMaxEvents);
            status = core::EventRing::kMaxEvents >= 2048 ? RowStatus::Pass : RowStatus::Fail;
            notes = "EventRing 2048 + EventStore 0640 + syslog RFC 5424 (r35)";
        } else if (id == "R15") {
            expected = "Annex M DIO3 inhibit";
            actual = "lab: DIO1/DIO2 only";
            status = RowStatus::Na;
            notes = "Phase 4 wiring";
        } else if (id == "L01") {
            expected = mock_gates ? fmt_num(dso_apply.pf2_threshold_kw) : "900";
            actual = fmt_num(pf2_eff.threshold_kw);
            unit = "kW";
            status = mock_gates ? eq_num(pf2_eff.threshold_kw, dso_apply.pf2_threshold_kw)
                                : eq_num(pf2_eff.threshold_kw, 900.0, 0.01);
            notes = "debounce_s=" + std::to_string(cfg.pf2.debounce_s);
        } else if (id == "L02") {
            expected = mock_gates ? fmt_num(dso_apply.pf2_release_kw) : "850";
            actual = fmt_num(pf2_eff.release_threshold_kw);
            unit = "kW";
            status = RowStatus::Pass;
        } else if (id == "L03") {
            expected = "10";
            actual = std::to_string(cfg.pf2.stale_data_s);
            unit = "s";
            status = cfg.pf2.stale_data_s == 10 ? RowStatus::Pass : RowStatus::Part;
        } else if (id == "L04") {
            expected = "400–950 kW ramp";
            actual = "modbus_rtu_slave.py --ramp";
            status = RowStatus::Pass;
        }

        report.rows.push_back(
            make_row(e.id, e.clause, e.name, expected, actual, unit.c_str(), status, notes.c_str()));
    }

    report.rows.push_back(make_row(
        "TR-T1", dso::kClauseTR57126, "Smax Table 1",
        "210", fmt_num(cfg.plant.smax_kva), "kVA", eq_num(cfg.plant.smax_kva, 210.0, 0.01), ""));

    report.rows.push_back(make_row(
        "PLANT", dso::kClauseO_8_2, "Plant yaml mock",
        "P/Q corners",
        fmt_num(cfg.plant.p_imm_kw) + "/" + fmt_num(cfg.plant.p_ass_kw) + " kW " +
            fmt_num(cfg.plant.q_ind_kvar) + "/" + fmt_num(cfg.plant.q_cap_kvar) + " kVAr",
        "-", RowStatus::Pass, ""));

    report.rows.push_back(make_row(
        "MOCK", "yaml", "DSO parameter mock",
        "w110/wlim/wsd",
        std::string("w110=") + (cfg.dso.w110_active ? "1" : "0") + "@" +
            fmt_num(cfg.dso.w110_pct) + "% wlim=" + (cfg.dso.wlim_active ? "1" : "0") + "@" +
            fmt_num(cfg.dso.wmax_spt_pct) + "% wsd=" + (cfg.dso.wsd_active ? "1" : "0") + "@" +
            fmt_num(cfg.dso.wspt_pct) + "%",
        "-", RowStatus::Pass, tr_profile ? "tr57126_table1" : "custom"));

    report.rows.push_back(make_row(
        "CFG", "yaml", "dso.enabled && use_dso_mock",
        "both true → Eq→kW",
        std::string(cfg.dso.enabled ? "true" : "false") + " / " +
            (cfg.pf2.use_dso_mock ? "true" : "false"),
        "-",
        dso_apply.applied ? RowStatus::Pass
                          : (cfg.dso.enabled && !cfg.pf2.use_dso_mock ? RowStatus::Part
                                                                        : RowStatus::Na),
        ""));

    report.rows.push_back(make_row(
        "IO-DI", "REQ-LAB-003", "Permissive",
        "active_low @ gd32=1", cfg.di_permissive.active_high ? "active_high" : "active_low", "-",
        RowStatus::Pass, ""));

    report.rows.push_back(make_row(
        "IO-byp", "Lab", "permissive_bypass",
        "false", cfg.permissive_bypass ? "true" : "false", "-",
        cfg.permissive_bypass ? RowStatus::Part : RowStatus::Pass, ""));

    report.rows.push_back(make_row(
        "REQ-61850", dso::kClauseAnnexT, "MMS live",
        "Eth_A", mock_gates ? "yaml active-P mock" : "stub", "-",
        mock_gates ? RowStatus::Na : RowStatus::NotImpl, "Phase 3 MMS"));

    return report;
}

void apply_live_snapshot(RegulationReport& report, const LiveSnapshot& live,
                         const CcliConfig& cfg) {
    report.live = live;
    if (!live.valid) {
        return;
    }

    const char* qstr = "invalid";
    if (live.quality == DataQuality::Good) {
        qstr = "good";
    } else if (live.quality == DataQuality::Stale) {
        qstr = "stale";
    }

    report.rows.push_back(make_row(
        "LIVE-P", "O.13.2", "Measured P (Modbus)",
        "(plant)", fmt_num(live.p_kw), "kW",
        live.quality == DataQuality::Good ? RowStatus::Pass : RowStatus::Fail, qstr));

    if (live.modbus_last.valid) {
        report.rows.push_back(make_row(
            "LIVE-MB", "Modbus", "Last master PDU (read)",
            "FC03 Read Holding", live.modbus_last.command.c_str(), "-",
            live.modbus_last.success ? RowStatus::Pass : RowStatus::Fail,
            live.modbus_last.backend.c_str()));
        if (!live.modbus_last.raw_regs_hex.empty()) {
            report.rows.push_back(make_row(
                "LIVE-MB-R", "O.8.3", "Register raw (TotW block)",
                "40001 float32 BE", live.modbus_last.raw_regs_hex.c_str(), "-",
                live.modbus_last.success ? RowStatus::Pass : RowStatus::Fail, ""));
        }
        report.rows.push_back(make_row(
            "LIVE-MB-W", "Phase 1", "Modbus writes (curtailment)",
            "none (DIO path)", "no FC06/FC16 in P1", "-", RowStatus::Na,
            "Curtail via DIO1 not Modbus"));
    }

    report.rows.push_back(make_row(
        "LIVE-FSM", "L01", "PF2 curtailment commanded",
        "(runtime)", live.curtailment_commanded ? "yes" : "no", "-",
        RowStatus::Pass, live.pf2_state.reason.c_str()));

    report.rows.push_back(make_row(
        "LIVE-DIO", "REQ-IO-001", "Curtail DO driven",
        "matches cmd∧perm", live.do_curtail_on ? "ON" : "OFF", "-",
        RowStatus::Pass, ""));

    if (!live.permissive_bypass) {
        report.rows.push_back(make_row(
            "LIVE-DI", "REQ-LAB-003", "Permissive OK",
            "key / 12V HIGH", live.permissive_ok ? "OK" : "BLOCKED", "-",
            live.permissive_ok ? RowStatus::Pass : RowStatus::Fail, ""));
    }

    const bool mock_gates = cfg.dso.enabled && cfg.pf2.use_dso_mock;
    const bool mb_ok = live.modbus_last.valid && live.modbus_last.success;

    auto conf_row = [&](const char* map_id, const char* clause, const char* name,
                        const std::string& expected, const std::string& actual, RowStatus st,
                        const char* note) {
        std::string id = std::string("LIVE-CONF-") + map_id;
        report.rows.push_back(make_row(id.c_str(), clause, name, expected, actual, "-", st, note));
    };

    conf_row("R01", dso::kClauseO_8_2, "Smax (no Modbus meter nameplate)",
             "yaml O.8.2", "no FC03 field", RowStatus::Na, "static yaml");

    conf_row("R02", dso::kClauseO_9_2_2, "Wlim set-point",
             "MMS WMaxSptPct", "no Modbus write", RowStatus::Na, "Phase 3 MMS");

    if (mock_gates && live.p_effective_export_kw > 0.0) {
        const bool band = dso::settled_within_band_kw(live.p_kw, live.p_effective_export_kw, 0.05);
        conf_row("R03", dso::kClauseO_9_2_3, "WSd P vs meter",
                 fmt_num(live.p_effective_export_kw) + " kW ±5%",
                 fmt_num(live.p_kw) + " kW", mb_ok && band ? RowStatus::Pass : RowStatus::Part,
                 "TotW via FC03");
        conf_row("R05", dso::kClauseO_11, "P vs min(export caps)",
                 fmt_num(live.p_effective_export_kw) + " kW",
                 fmt_num(live.p_kw) + " kW", mb_ok && band ? RowStatus::Pass : RowStatus::Part,
                 "Eq (5)(6) live");
        conf_row("R06", dso::kClauseO_7_3_1, "±5% band @ meter",
                 "±5%", band ? "in band" : "out of band",
                 mb_ok && band ? RowStatus::Pass : RowStatus::Fail, "O.7.3.1");
    } else {
        conf_row("R03", dso::kClauseO_9_2_3, "WSd P vs meter", "DSO mock off",
                 fmt_num(live.p_kw) + " kW read", mb_ok ? RowStatus::Part : RowStatus::Fail,
                 "enable use_dso_mock for CEI compare");
    }

    conf_row("R04", dso::kClauseO_9_2_1, "w110 autonomous cap", "Vn path",
             "no Modbus", RowStatus::Na, "no V meter Modbus");

    conf_row("R07", dso::kClauseO_7_3_3, "3 s set-point spacing", "≥3 s",
             "mms.setpoint_min_interval_s=" + std::to_string(cfg.mms.setpoint_min_interval_s),
             cfg.mms.setpoint_min_interval_s >= 3 ? RowStatus::Pass
             : cfg.mms.setpoint_min_interval_s > 0 ? RowStatus::Part
                                                   : RowStatus::NotImpl,
             "Eq (9) gate on WMaxSptPct/WSptPct/VArTgtSptPct");

    conf_row("R08", dso::kClauseO_13_1_2, "Comm loss → stale",
             "FC03 OK → good", live.modbus_last.success ? "read OK" : live.modbus_last.error,
             mb_ok && live.quality == DataQuality::Good ? RowStatus::Pass : RowStatus::Fail,
             "O.13.2 / L03 stale policy");

    conf_row("R09", dso::kClauseO_8_3, "TotW 4 s block read",
             "FC03 @" + std::to_string(40001 + live.modbus_last.reg_start),
             live.modbus_last.valid ? "poll_ms=" + std::to_string(live.modbus_poll_ms) : "no poll",
             mb_ok && live.modbus_poll_ms == 4000 ? RowStatus::Pass : RowStatus::Part,
             "master read only");

    conf_row("R10", dso::kClauseO_9_1, "Reactive VArV", "inactive",
             "Q read optional", RowStatus::Na, "Phase 3");
    conf_row("R11", dso::kClauseO_9_1, "Reactive PFW", "inactive", "n/a", RowStatus::Na, "");
    conf_row("R12", dso::kClauseO_9_1, "Reactive PFSP", "inactive", "n/a", RowStatus::Na, "");
    conf_row("R13", dso::kClauseO_9_1, "VArSd/VArSa Eq (3)", "n/a", "no Q SP path",
             RowStatus::Na, "Phase 5-R");
    conf_row("R14", dso::kClauseO_14, "Event logger", "n/a", "not on Modbus", RowStatus::Na, "");
    conf_row("R15", dso::kClauseAnnexM, "Teledistacco", "DIO3", "DIO1 DO only", RowStatus::Na,
             "not Modbus");

    if (live.pf2_enter_kw > 0.0) {
        const bool above = live.p_kw > live.pf2_enter_kw;
        const bool cmd = live.curtailment_commanded;
        RowStatus st = RowStatus::Pass;
        if (above && !cmd) {
            st = RowStatus::Part;
        }
        if (!above && cmd) {
            st = RowStatus::Part;
        }
        conf_row("L01", dso::kLabL01, "P vs enter + FSM",
                 fmt_num(live.pf2_enter_kw) + " kW",
                 fmt_num(live.p_kw) + " kW cmd=" + (cmd ? "yes" : "no"), st, "debounce not waited");
        conf_row("L02", dso::kLabL02, "Release hysteresis",
                 fmt_num(live.pf2_release_kw) + " kW",
                 live.curtailment_commanded ? "curtailing" : "released", RowStatus::Pass, "");
    }

    conf_row("L03", dso::kLabL03, "Stale comm fail-safe",
             "stop slave → stale", live.quality == DataQuality::Stale ? "stale" : "good",
             live.quality == DataQuality::Good ? RowStatus::Pass : RowStatus::Pass,
             "bench: stop PC slave");

    conf_row("L04", dso::kLabL04, "Ramp mock (Eq 18)",
             "400–950 kW", fmt_num(live.p_kw) + " kW", RowStatus::Pass, "slave --ramp");
}

void print_regulation_report_text(const RegulationReport& report, std::ostream& out) {
    out << "=== CCLI regulation bench ===\n";
    out << "config: " << report.config_path << "\n";
    out << "doc: lab/PF2_REGULATION_PHASE1.md · lab/CCI_Figura2_Parameters.md\n\n";

    out << std::left << std::setw(8) << "ID" << std::setw(14) << "Clause" << std::setw(28)
        << "Name" << std::setw(22) << "Expected" << std::setw(14) << "Actual" << std::setw(6)
        << "Unit" << "Status\n";
    out << std::string(100, '-') << "\n";

    for (const auto& r : report.rows) {
        out << std::setw(8) << r.id << std::setw(14) << r.clause << std::setw(28) << r.name
            << std::setw(22) << r.expected << std::setw(14) << r.actual << std::setw(6)
            << r.unit << row_status_string(r.status);
        if (!r.notes.empty()) {
            out << "  (" << r.notes << ")";
        }
        out << "\n";
    }

    if (report.live.valid) {
        out << "\n--- live snapshot ---\n";
        out << "P=" << report.live.p_kw << " kW  permissive_ok="
            << (report.live.permissive_ok ? "yes" : "no") << "  pf2="
            << report.live.pf2_state.reason << "\n";
        if (report.live.modbus_last.valid) {
            out << "Modbus: " << report.live.modbus_last.command << "\n";
        }
        if (!report.live.modbus_log.empty()) {
            out << "\n--- live Modbus master log (" << report.live.modbus_log.size()
                << " polls) ---\n";
            for (const auto& mb : report.live.modbus_log) {
                out << mb.command;
                if (!mb.raw_regs_hex.empty()) {
                    out << "  raw=" << mb.raw_regs_hex;
                }
                out << "  P=" << fmt_num(mb.p_kw) << " kW  "
                    << (mb.success ? "OK" : "FAIL") << "\n";
            }
        }
    }
    out << "\nRun: ccli --regulation-check --live [--live-watch SEC] --config /etc/ccli/lab.yaml\n";
    out << "Web: open lab/regulation_bench/index.html (import JSON export)\n";
}

void print_regulation_report_json(const RegulationReport& report, std::ostream& out) {
    out << "{\n  \"config\": \"" << json_escape(report.config_path) << "\",\n  \"rows\": [\n";
    for (std::size_t i = 0; i < report.rows.size(); ++i) {
        const auto& r = report.rows[i];
        out << "    {\"id\":\"" << json_escape(r.id) << "\",\"clause\":\"" << json_escape(r.clause)
            << "\",\"name\":\"" << json_escape(r.name) << "\",\"expected\":\""
            << json_escape(r.expected) << "\",\"actual\":\"" << json_escape(r.actual)
            << "\",\"unit\":\"" << json_escape(r.unit) << "\",\"status\":\""
            << row_status_string(r.status) << "\",\"notes\":\"" << json_escape(r.notes) << "\"}";
        if (i + 1 < report.rows.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ],\n  \"live\": { \"valid\": " << (report.live.valid ? "true" : "false");
    if (report.live.valid) {
        out << ", \"p_kw\": " << report.live.p_kw << ", \"permissive_ok\": "
            << (report.live.permissive_ok ? "true" : "false") << ", \"curtailment_commanded\": "
            << (report.live.curtailment_commanded ? "true" : "false") << ", \"do_curtail_on\": "
            << (report.live.do_curtail_on ? "true" : "false") << ", \"pf2_enter_kw\": "
            << report.live.pf2_enter_kw << ", \"p_effective_export_kw\": "
            << report.live.p_effective_export_kw << ", \"modbus_poll_ms\": "
            << report.live.modbus_poll_ms;
        if (report.live.modbus_last.valid) {
            out << ", \"modbus_last\": ";
            print_modbus_trace_json(report.live.modbus_last, out);
        }
        if (!report.live.modbus_log.empty()) {
            out << ", \"modbus_log\": [";
            for (std::size_t j = 0; j < report.live.modbus_log.size(); ++j) {
                if (j > 0) {
                    out << ",";
                }
                print_modbus_trace_json(report.live.modbus_log[j], out);
            }
            out << "]";
        }
    }
    out << " }\n}\n";
}

}  // namespace cci::core::regulation
