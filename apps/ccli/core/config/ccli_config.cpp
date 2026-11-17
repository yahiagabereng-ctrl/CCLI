/*=============================================================================
 * File       :  ccli_config.cpp
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  Minimal indentation-aware YAML reader for the CCLI lab config.
 *               Only the pf2 and io sections are consumed; everything else is
 *               ignored. Not a general YAML parser.
 *=============================================================================*/
#include "config/ccli_config.hpp"
#include "config/modbus_pdc_config.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <fstream>
#include <string>

namespace cci::core {

namespace {

bool tls_timing(const std::string& key, const std::string& value, TlsTimingConfig& cfg) {
    int* field = nullptr;
    int minimum = 1;
    int maximum = 86400;
    if (key == "tls_revocation_check_s") field = &cfg.revocation_check_s;
    else if (key == "tls_renegotiation_s") { field = &cfg.renegotiation_s; minimum = 600; }
    else if (key == "tls_resumption_s") { field = &cfg.resumption_s; maximum = 7200; }
    else return false;
    int parsed = 0;
    const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
    if (result.ec != std::errc{} || result.ptr != value.data() + value.size() ||
        parsed < minimum || parsed > maximum) cfg.configuration_valid = false;
    else *field = parsed;
    return true;
}

std::size_t indent_of(const std::string& s) {
    std::size_t n = 0;
    while (n < s.size() && s[n] == ' ') {
        ++n;
    }
    return n;
}

std::string strip(std::string s) {
    // Drop inline comments.
    const auto hash = s.find('#');
    if (hash != std::string::npos) {
        s.erase(hash);
    }
    // Trim trailing whitespace.
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' ||
                          s.back() == '\n')) {
        s.pop_back();
    }
    return s;
}

// Split "key: value" (value optional). Returns key trimmed; value trimmed.
void split_kv(const std::string& line, std::string& key, std::string& value) {
    const auto colon = line.find(':');
    if (colon == std::string::npos) {
        key.clear();
        value.clear();
        return;
    }
    std::size_t ks = 0;
    while (ks < colon && line[ks] == ' ') {
        ++ks;
    }
    key = line.substr(ks, colon - ks);
    std::string v = line.substr(colon + 1);
    std::size_t vs = 0;
    while (vs < v.size() && v[vs] == ' ') {
        ++vs;
    }
    value = v.substr(vs);
}

bool parse_bool(const std::string& v, bool fallback) {
    if (v == "true" || v == "True" || v == "1" || v == "yes") {
        return true;
    }
    if (v == "false" || v == "False" || v == "0" || v == "no") {
        return false;
    }
    return fallback;
}
bool tls_optional_hash(const std::string& key, const std::string& value,
                       TlsTimingConfig& cfg) {
    bool* field = nullptr;
    if (key == "tls_allow_sha384") field = &cfg.allow_sha384;
    else if (key == "tls_allow_sha512") field = &cfg.allow_sha512;
    else return false;
    if (value == "true" || value == "True" || value == "1" || value == "yes") *field = true;
    else if (value == "false" || value == "False" || value == "0" || value == "no") *field = false;
    else cfg.configuration_valid = false;
    return true;
}
bool parse_e2e_bool(const std::string& value, bool& valid) {
    if (value != "true" && value != "True" && value != "1" && value != "yes" &&
        value != "false" && value != "False" && value != "0" && value != "no") {
        valid = false;
        return false;
    }
    return parse_bool(value, false);
}
unsigned parse_e2e_unsigned(const std::string& value, bool& valid) {
    unsigned result = 0;
    const auto parsed = std::from_chars(value.data(), value.data() + value.size(), result);
    if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size()) valid = false;
    return result;
}

}  // namespace

CcliConfig load_config(const std::string& path) {
    CcliConfig cfg{};

    std::ifstream in(path);
    if (!in.is_open()) {
        return cfg;  // defaults, loaded_from_file = false
    }

    std::string top;  // current indent-0 section (e.g. "pf2", "io")
    std::string sub;  // current indent-2 sub-section under io
    std::string raw;

    while (std::getline(in, raw)) {
        const std::string line = strip(raw);
        if (line.empty()) {
            continue;
        }
        const std::size_t ind = indent_of(line);

        std::string key;
        std::string value;
        split_kv(line, key, value);
        if (key.empty()) {
            continue;
        }

        if (ind == 0) {
            top = key;
            sub.clear();
            continue;
        }

        if (top == "pf2" && ind == 2 && !value.empty()) {
            if (key == "threshold_kw") {
                cfg.pf2.threshold_kw = std::strtod(value.c_str(), nullptr);
            } else if (key == "release_threshold_kw") {
                cfg.pf2.release_threshold_kw = std::strtod(value.c_str(), nullptr);
            } else if (key == "debounce_s") {
                cfg.pf2.debounce_s = std::atoi(value.c_str());
            } else if (key == "min_on_s") {
                cfg.pf2.min_on_s = std::atoi(value.c_str());
            } else if (key == "min_off_s") {
                cfg.pf2.min_off_s = std::atoi(value.c_str());
            } else if (key == "stale_data_s") {
                cfg.pf2.stale_data_s = std::atoi(value.c_str());
            } else if (key == "use_dso_mock") {
                cfg.pf2.use_dso_mock = parse_bool(value, cfg.pf2.use_dso_mock);
            }
            continue;
        }

        if (top == "plant" && ind == 2 && !value.empty()) {
            if (key == "p_imm_kw") {
                cfg.plant.p_imm_kw = std::strtod(value.c_str(), nullptr);
            } else if (key == "p_ass_kw") {
                cfg.plant.p_ass_kw = std::strtod(value.c_str(), nullptr);
            } else if (key == "q_ind_kvar") {
                cfg.plant.q_ind_kvar = std::strtod(value.c_str(), nullptr);
            } else if (key == "q_cap_kvar") {
                cfg.plant.q_cap_kvar = std::strtod(value.c_str(), nullptr);
            } else if (key == "smax_kva") {
                cfg.plant.smax_kva = std::strtod(value.c_str(), nullptr);
            } else if (key == "poc_ppv_kv") {
                cfg.plant.poc_ppv_kv = std::strtod(value.c_str(), nullptr);
            }
            continue;
        }

        if (top == "dso" && ind == 2 && !value.empty()) {
            if (key == "enabled") {
                cfg.dso.enabled = parse_bool(value, cfg.dso.enabled);
            } else if (key == "profile") {
                if (value == "tr57126_table1" || value == "TR57126") {
                    cfg.dso.profile = DsoProfile::Tr57126Table1;
                } else {
                    cfg.dso.profile = DsoProfile::Custom;
                }
            } else if (key == "w110_active") {
                cfg.dso.w110_active = parse_bool(value, cfg.dso.w110_active);
            } else if (key == "w110_pct") {
                cfg.dso.w110_pct = std::strtod(value.c_str(), nullptr);
            } else if (key == "wlim_active") {
                cfg.dso.wlim_active = parse_bool(value, cfg.dso.wlim_active);
            } else if (key == "wmax_spt_pct") {
                cfg.dso.wmax_spt_pct = std::strtod(value.c_str(), nullptr);
            } else if (key == "wsd_active") {
                cfg.dso.wsd_active = parse_bool(value, cfg.dso.wsd_active);
            } else if (key == "wspt_pct") {
                cfg.dso.wspt_pct = std::strtod(value.c_str(), nullptr);
            } else if (key == "release_delta_kw") {
                cfg.dso.release_delta_kw = std::strtod(value.c_str(), nullptr);
            }
            continue;
        }

        if (top == "modbus") {
            if (ind == 2 && value.empty()) {
                sub = key;
                continue;
            }
            if (ind == 2 && !value.empty()) {
                sub.clear();
            }
            if (ind == 4 && sub == "commissioning" && !value.empty()) {
                if (key == "revision") {
                    cfg.modbus.commissioning.revision = std::atoi(value.c_str());
                } else if (key == "enabled") {
                    cfg.modbus.commissioning.enabled =
                        parse_bool(value, cfg.modbus.commissioning.enabled);
                } else if (key == "apply_once") {
                    cfg.modbus.commissioning.apply_once =
                        parse_bool(value, cfg.modbus.commissioning.apply_once);
                } else if (key == "verify_mounted_version") {
                    cfg.modbus.commissioning.verify_mounted_version =
                        parse_bool(value, cfg.modbus.commissioning.verify_mounted_version);
                } else if (key == "write_meas_1p_integrator") {
                    cfg.modbus.commissioning.write_meas_1p_integrator =
                        parse_bool(value, cfg.modbus.commissioning.write_meas_1p_integrator);
                } else if (key == "write_filtered_measurement") {
                    cfg.modbus.commissioning.write_filtered_measurement =
                        parse_bool(value, cfg.modbus.commissioning.write_filtered_measurement);
                } else if (key == "write_rogowski_sensitivity") {
                    cfg.modbus.commissioning.write_rogowski_sensitivity =
                        parse_bool(value, cfg.modbus.commissioning.write_rogowski_sensitivity);
                } else if (key == "rogowski_sensitivity_v_per_a") {
                    cfg.modbus.commissioning.rogowski_sensitivity_v_per_a =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "verify_ta_chronos_register") {
                    cfg.modbus.commissioning.verify_ta_chronos_register =
                        parse_bool(value, cfg.modbus.commissioning.verify_ta_chronos_register);
                } else if (key == "write_ta_chronos_register") {
                    cfg.modbus.commissioning.write_ta_chronos_register =
                        parse_bool(value, cfg.modbus.commissioning.write_ta_chronos_register);
                } else if (key == "verify_transducer_bench_defaults") {
                    cfg.modbus.commissioning.verify_transducer_bench_defaults = parse_bool(
                        value, cfg.modbus.commissioning.verify_transducer_bench_defaults);
                } else if (key == "write_transducer_bench_defaults") {
                    cfg.modbus.commissioning.write_transducer_bench_defaults = parse_bool(
                        value, cfg.modbus.commissioning.write_transducer_bench_defaults);
                } else if (key == "ta_transducer_delay_deg") {
                    cfg.modbus.commissioning.ta_transducer_delay_deg =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "tv_transducer_ratio") {
                    cfg.modbus.commissioning.tv_transducer_ratio =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "tv_transducer_delay_deg") {
                    cfg.modbus.commissioning.tv_transducer_delay_deg =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "minimum_voltage_ripple") {
                    cfg.modbus.commissioning.minimum_voltage_ripple =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "minimum_current_ripple") {
                    cfg.modbus.commissioning.minimum_current_ripple =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "minimum_power_ripple") {
                    cfg.modbus.commissioning.minimum_power_ripple =
                        std::strtod(value.c_str(), nullptr);
                } else if (key == "verify_energy_unit_kwh") {
                    cfg.modbus.commissioning.verify_energy_unit_kwh =
                        parse_bool(value, cfg.modbus.commissioning.verify_energy_unit_kwh);
                } else if (key == "write_energy_unit_kwh") {
                    cfg.modbus.commissioning.write_energy_unit_kwh =
                        parse_bool(value, cfg.modbus.commissioning.write_energy_unit_kwh);
                } else if (key == "write_energy_saving") {
                    cfg.modbus.commissioning.write_energy_saving =
                        parse_bool(value, cfg.modbus.commissioning.write_energy_saving);
                } else if (key == "log_energy_readings") {
                    cfg.modbus.commissioning.log_energy_readings =
                        parse_bool(value, cfg.modbus.commissioning.log_energy_readings);
                }
                continue;
            }
        }

        if (top == "modbus" && ind == 2 && !value.empty()) {
            /* `backend` selects simulator vs libmodbus. `mode` (rtu/tcp) is transport
             * detail for RTU bench docs — do not treat it as an override of backend. */
            if (key == "backend") {
                if (value == "rtu" || value == "libmodbus") {
                    cfg.modbus.backend = ModbusBackendKind::Rtu;
                } else if (value == "tcp") {
                    cfg.modbus.backend = ModbusBackendKind::Tcp;
                } else if (value == "simulator" || value == "sim") {
                    cfg.modbus.backend = ModbusBackendKind::Simulator;
                }
            } else if (key == "mode") {
                if (value == "tcp") {
                    cfg.modbus.backend = ModbusBackendKind::Tcp;
                } else if (value == "rtu") {
                    cfg.modbus.backend = ModbusBackendKind::Rtu;
                }
            } else if (key == "host") {
                cfg.modbus.host = value;
            } else if (key == "tcp_port" || key == "port") {
                cfg.modbus.tcp_port = std::atoi(value.c_str());
            } else if (key == "device") {
                cfg.modbus.device = value;
            } else if (key == "baud") {
                cfg.modbus.baud = std::atoi(value.c_str());
            } else if (key == "parity") {
                cfg.modbus.parity = value.empty() ? 'N' : value[0];
            } else if (key == "slave_id") {
                cfg.modbus.slave_id = std::atoi(value.c_str());
            } else if (key == "poll_ms") {
                cfg.modbus.poll_ms = std::atoi(value.c_str());
            } else if (key == "timeout_ms") {
                cfg.modbus.timeout_ms = std::atoi(value.c_str());
            } else if (key == "retries") {
                cfg.modbus.retries = std::atoi(value.c_str());
            } else if (key == "reg_active_power") {
                cfg.modbus.reg_active_power = std::atoi(value.c_str());
            } else if (key == "reg_active_power_count") {
                cfg.modbus.reg_active_power_count = std::atoi(value.c_str());
            } else if (key == "reg_reactive_power") {
                cfg.modbus.reg_reactive_power = std::atoi(value.c_str());
            } else if (key == "reg_reactive_power_count") {
                cfg.modbus.reg_reactive_power_count = std::atoi(value.c_str());
            } else if (key == "reg_line_voltage") {
                cfg.modbus.reg_line_voltage = std::atoi(value.c_str());
            } else if (key == "reg_line_voltage_count") {
                cfg.modbus.reg_line_voltage_count = std::atoi(value.c_str());
            } else if (key == "line_voltage_scale") {
                cfg.modbus.line_voltage_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "reg_phase_a_current") {
                cfg.modbus.reg_phase_a_current = std::atoi(value.c_str());
            } else if (key == "reg_phase_a_current_count") {
                cfg.modbus.reg_phase_a_current_count = std::atoi(value.c_str());
            } else if (key == "phase_a_current_scale") {
                cfg.modbus.phase_a_current_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "poll_pdc_3ph") {
                cfg.modbus.poll_pdc_3ph = parse_bool(value, cfg.modbus.poll_pdc_3ph);
            } else if (key == "reg_phase_b_current") {
                cfg.modbus.reg_phase_b_current = std::atoi(value.c_str());
            } else if (key == "reg_phase_c_current") {
                cfg.modbus.reg_phase_c_current = std::atoi(value.c_str());
            } else if (key == "reg_ppv_phs_bc") {
                cfg.modbus.reg_ppv_phs_bc = std::atoi(value.c_str());
            } else if (key == "reg_ppv_phs_ca") {
                cfg.modbus.reg_ppv_phs_ca = std::atoi(value.c_str());
            } else if (key == "reg_active_power_l2") {
                cfg.modbus.reg_active_power_l2 = std::atoi(value.c_str());
            } else if (key == "reg_active_power_l3") {
                cfg.modbus.reg_active_power_l3 = std::atoi(value.c_str());
            } else if (key == "reg_reactive_power_l2") {
                cfg.modbus.reg_reactive_power_l2 = std::atoi(value.c_str());
            } else if (key == "reg_reactive_power_l3") {
                cfg.modbus.reg_reactive_power_l3 = std::atoi(value.c_str());
            } else if (key == "reg_power_factor") {
                cfg.modbus.reg_power_factor = std::atoi(value.c_str());
            } else if (key == "reg_power_factor_count") {
                cfg.modbus.reg_power_factor_count = std::atoi(value.c_str());
            } else if (key == "power_scale") {
                cfg.modbus.power_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "reactive_scale") {
                cfg.modbus.reactive_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "float_word_swap") {
                cfg.modbus.float_word_swap = parse_bool(value, cfg.modbus.float_word_swap);
            } else if (key == "trace") {
                cfg.modbus.trace = parse_bool(value, cfg.modbus.trace);
            }
            continue;
        }

        if (top == "inverter_modbus" && ind == 2 && !value.empty()) {
            auto& mb = cfg.inverter_modbus;
            if (key == "enabled") {
                mb.enabled = parse_bool(value, mb.enabled);
            } else if (key == "backend") {
                if (value == "rtu" || value == "libmodbus") {
                    mb.backend = ModbusBackendKind::Rtu;
                } else if (value == "tcp") {
                    mb.backend = ModbusBackendKind::Tcp;
                } else if (value == "simulator" || value == "sim") {
                    mb.backend = ModbusBackendKind::Simulator;
                }
            } else if (key == "host") {
                mb.host = value;
            } else if (key == "tcp_port" || key == "port") {
                mb.tcp_port = std::atoi(value.c_str());
            } else if (key == "device") {
                mb.device = value;
            } else if (key == "baud") {
                mb.baud = std::atoi(value.c_str());
            } else if (key == "parity") {
                mb.parity = value.empty() ? 'N' : value[0];
            } else if (key == "slave_id") {
                mb.slave_id = std::atoi(value.c_str());
            } else if (key == "poll_ms") {
                mb.poll_ms = std::atoi(value.c_str());
            } else if (key == "timeout_ms") {
                mb.timeout_ms = std::atoi(value.c_str());
            } else if (key == "retries") {
                mb.retries = std::atoi(value.c_str());
            } else if (key == "reg_active_power") {
                mb.reg_active_power = std::atoi(value.c_str());
            } else if (key == "reg_active_power_count") {
                mb.reg_active_power_count = std::atoi(value.c_str());
            } else if (key == "reg_reactive_power") {
                mb.reg_reactive_power = std::atoi(value.c_str());
            } else if (key == "reg_reactive_power_count") {
                mb.reg_reactive_power_count = std::atoi(value.c_str());
            } else if (key == "reg_line_voltage") {
                mb.reg_line_voltage = std::atoi(value.c_str());
            } else if (key == "reg_line_voltage_count") {
                mb.reg_line_voltage_count = std::atoi(value.c_str());
            } else if (key == "line_voltage_scale") {
                mb.line_voltage_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "power_scale") {
                mb.power_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "reactive_scale") {
                mb.reactive_scale = std::strtod(value.c_str(), nullptr);
            } else if (key == "float_word_swap") {
                mb.float_word_swap = parse_bool(value, mb.float_word_swap);
            } else if (key == "trace") {
                mb.trace = parse_bool(value, mb.trace);
            }
            continue;
        }

        if ((top == "iec104" || top == "mms") && ind == 2 && value.empty()) {
            tls_timing(key, value, top == "mms" ? static_cast<TlsTimingConfig&>(cfg.mms.tls)
                                               : static_cast<TlsTimingConfig&>(cfg.iec104.tls));
        }
        if (top == "iec104" && ind == 2 && !value.empty()) {
            if (key == "enabled") {
                cfg.iec104.enabled = parse_bool(value, cfg.iec104.enabled);
            } else if (key == "bind_address" || key == "bind") {
                cfg.iec104.bind_address = value;
            } else if (key == "tcp_port" || key == "port") {
                cfg.iec104.tcp_port = std::atoi(value.c_str());
            } else if (key == "comms_loss_fallback_s") {
                cfg.iec104.comms_loss_fallback_s = std::atoi(value.c_str());
            } else if (key == "common_address" || key == "ca") {
                cfg.iec104.common_address = std::atoi(value.c_str());
            } else if (key == "ioa_tot_w") {
                cfg.iec104.ioa_tot_w = std::atoi(value.c_str());
            } else if (key == "periodic_s") {
                cfg.iec104.periodic_s = std::atoi(value.c_str());
            } else if (key == "operator_rule_mode") {
                cfg.iec104.operator_rule.mode = value;
            } else if (key == "operator_allow_commands") {
                cfg.iec104.operator_rule.allow_commands =
                    parse_bool(value, cfg.iec104.operator_rule.allow_commands);
            } else if (key == "operator_allow_gi") {
                cfg.iec104.operator_rule.allow_gi =
                    parse_bool(value, cfg.iec104.operator_rule.allow_gi);
            } else if (key == "operator_allow_clock_sync") {
                cfg.iec104.operator_rule.allow_clock_sync =
                    parse_bool(value, cfg.iec104.operator_rule.allow_clock_sync);
            } else if (key == "operator_monitor_tot_w") {
                cfg.iec104.operator_rule.monitor_tot_w =
                    parse_bool(value, cfg.iec104.operator_rule.monitor_tot_w);
            } else if (key == "operator_monitor_totvar") {
                cfg.iec104.operator_rule.monitor_tot_var =
                    parse_bool(value, cfg.iec104.operator_rule.monitor_tot_var);
            } else if (key == "operator_monitor_ppv") {
                cfg.iec104.operator_rule.monitor_ppv =
                    parse_bool(value, cfg.iec104.operator_rule.monitor_ppv);
            } else if (key == "ioa_tot_var") {
                cfg.iec104.operator_rule.ioa_tot_var = std::atoi(value.c_str());
            } else if (key == "ioa_ppv") {
                cfg.iec104.operator_rule.ioa_ppv = std::atoi(value.c_str());
            } else if (key == "operator_ioa_msd_sp" || key == "ioa_msd_sp") {
                cfg.iec104.operator_rule.ioa_msd_sp = std::atoi(value.c_str());
            } else if (key == "tls_enabled") {
                cfg.iec104.tls.enabled = parse_bool(value, cfg.iec104.tls.enabled);
            } else if (key == "tls_own_key") {
                cfg.iec104.tls.own_key = value;
            } else if (key == "tls_own_cert") {
                cfg.iec104.tls.own_cert = value;
            } else if (key == "tls_ca_cert") {
                cfg.iec104.tls.ca_cert = value;
            } else if (key == "tls_client_cert") {
                cfg.iec104.tls.client_cert = value;
            } else if (key == "tls_allowed_client_cert") {
                cfg.iec104.tls.allowed_client_certs.push_back(value);
            } else if (key == "tls_crl") {
                cfg.iec104.tls.crl_path = value;
            } else if (tls_optional_hash(key, value, cfg.iec104.tls)) {
            } else if (tls_timing(key, value, cfg.iec104.tls)) {
            }
            continue;
        }

        if (top == "mms" && ind == 2 && value.empty() && key.rfind("e2e_", 0) == 0)
            cfg.mms.e2e.configuration_valid = false;
        if (top == "mms" && ind == 2 && !value.empty()) {
            if (key == "enabled") {
                cfg.mms.enabled = parse_bool(value, cfg.mms.enabled);
            } else if (key == "bind_address" || key == "bind") {
                cfg.mms.bind_address = value;
            } else if (key == "tcp_port" || key == "port") {
                cfg.mms.tcp_port = std::atoi(value.c_str());
            } else if (key == "tls_enabled") {
                cfg.mms.tls.enabled = parse_bool(value, cfg.mms.tls.enabled);
            } else if (key == "tls_own_key") {
                cfg.mms.tls.own_key = value;
            } else if (key == "tls_own_cert") {
                cfg.mms.tls.own_cert = value;
            } else if (key == "tls_additional_own_key") {
                cfg.mms.tls.additional_own_key = value;
            } else if (key == "tls_additional_own_cert") {
                cfg.mms.tls.additional_own_cert = value;
            } else if (key == "tls_acse_cert") {
                cfg.mms.tls.acse_cert = value;
            } else if (key == "tls_acse_key") {
                cfg.mms.tls.acse_key = value;
            } else if (key == "tls_ca_cert") {
                cfg.mms.tls.ca_cert = value;
            } else if (key == "tls_client_cert") {
                cfg.mms.tls.client_cert = value;
            } else if (key == "tls_acse_client_cert") {
                cfg.mms.tls.acse_client_cert = value;
            } else if (key == "tls_viewer_cert") {
                cfg.mms.tls.viewer_cert = value;
            } else if (key == "tls_acse_viewer_cert") {
                cfg.mms.tls.acse_viewer_cert = value;
            } else if (key == "tls_crl") {
                cfg.mms.tls.crl_path = value;
            } else if (key == "tls_revoked_cert") {
                cfg.mms.tls.revoked_cert = value;
            } else if (key == "tls_additional_client_cert") {
                cfg.mms.tls.additional_client_cert = value;
            } else if (key == "tls_allowed_client_cert") {
                cfg.mms.tls.allowed_client_certs.push_back(value);
            } else if (key == "tls_chain_validation") {
                cfg.mms.tls.chain_validation =
                    parse_bool(value, cfg.mms.tls.chain_validation);
            } else if (tls_optional_hash(key, value, cfg.mms.tls)) {
            } else if (tls_timing(key, value, cfg.mms.tls)) {
            } else if (key == "comms_loss_fallback_s") {
                cfg.mms.comms_loss_fallback_s = std::atoi(value.c_str());
            } else if (key == "aggregator_plant_outputs_enabled") {
                cfg.mms.aggregator_plant_outputs_enabled = parse_bool(value, false);
            } else if (key == "setpoint_min_interval_s") {
                cfg.mms.setpoint_min_interval_s = std::atoi(value.c_str());
            } else if (key == "gnss_poll_s") {
                cfg.mms.gnss_poll_s = std::atoi(value.c_str());
            } else if (key == "gnss_discipline_clock") {
                cfg.mms.gnss_discipline_clock =
                    parse_bool(value, cfg.mms.gnss_discipline_clock);
            } else if (key == "chrony_poll") {
                cfg.mms.chrony_poll = parse_bool(value, cfg.mms.chrony_poll);
            } else if (key == "model_cfg") {
                cfg.mms.model_cfg_path = value;
            } else if (key == "rbac_policy") {
                cfg.mms.rbac_policy_path = value;
            } else if (key == "rbac_profile_a") {
                cfg.mms.rbac_profile_a = parse_bool(value, cfg.mms.rbac_profile_a);
            } else if (key == "rbac_owner_cert") {
                cfg.mms.rbac_owner_cert = value;
            } else if (key == "rbac_activation_state") {
                cfg.mms.rbac_activation_state = value;
            } else if (key == "rbac_identity_ca") {
                cfg.mms.rbac_identity_ca = value;
            } else if (key == "rbac_identity_crl") {
                cfg.mms.rbac_identity_crl = value;
            } else if (key == "e2e_enabled") {
                cfg.mms.e2e.enabled = parse_e2e_bool(value, cfg.mms.e2e.configuration_valid);
            } else if (key == "e2e_allow_compatibility") {
                cfg.mms.e2e.allow_compatibility = parse_e2e_bool(value, cfg.mms.e2e.configuration_valid);
            } else if (key == "e2e_certificate") {
                cfg.mms.e2e.certificate = value;
            } else if (key == "e2e_private_key") {
                cfg.mms.e2e.private_key = value;
            } else if (key == "e2e_trust_anchors") {
                cfg.mms.e2e.trust_anchors = value;
            } else if (key == "e2e_crls") {
                cfg.mms.e2e.crls = value;
            } else if (key == "e2e_certificate_path") {
                cfg.mms.e2e.certificate_path.push_back(value);
            } else if (key == "e2e_ap_title") {
                cfg.mms.e2e.ap_title = value;
            } else if (key == "e2e_ae_qualifier") {
                cfg.mms.e2e.ae_qualifier = parse_e2e_unsigned(value, cfg.mms.e2e.configuration_valid);
            } else if (key == "e2e_signature_algorithm") {
                cfg.mms.e2e.signature_algorithm = value;
            } else if (key == "e2e_rekey_seconds") {
                cfg.mms.e2e.rekey_seconds = parse_e2e_unsigned(value, cfg.mms.e2e.configuration_valid);
            } else if (key.rfind("e2e_", 0) == 0) {
                cfg.mms.e2e.configuration_valid = false;
            }
            continue;
        }

        if (top == "goose" && ind == 2 && !value.empty()) {
            if (key == "enabled") {
                cfg.goose.enabled = parse_bool(value, cfg.goose.enabled);
            } else if (key == "interface") {
                cfg.goose.interface = value;
            } else if (key == "subscribe_go_cb_ref") {
                cfg.goose.subscribe_go_cb_ref = value;
            } else if (key == "subscribe_app_id") {
                cfg.goose.subscribe_app_id =
                    static_cast<uint16_t>(std::strtoul(value.c_str(), nullptr, 0));
            } else if (key == "subscribe_dst_mac") {
                cfg.goose.subscribe_dst_mac = value;
            } else if (key == "publish_enabled") {
                cfg.goose.publish_enabled = parse_bool(value, cfg.goose.publish_enabled);
            } else if (key == "publish_app_id") {
                cfg.goose.publish_app_id =
                    static_cast<uint16_t>(std::strtoul(value.c_str(), nullptr, 0));
            } else if (key == "publish_dst_mac") {
                cfg.goose.publish_dst_mac = value;
            } else if (key == "publish_vlan_id") {
                cfg.goose.publish_vlan_id =
                    static_cast<uint16_t>(std::strtoul(value.c_str(), nullptr, 10));
            } else if (key == "publish_interval_ms") {
                cfg.goose.publish_interval_ms = std::atoi(value.c_str());
            } else if (key == "timeout_s") {
                cfg.goose.timeout_s = std::atoi(value.c_str());
            }
            continue;
        }

        if (top == "io") {
            if (ind == 2 && !value.empty() && key == "permissive_bypass") {
                cfg.permissive_bypass = parse_bool(value, cfg.permissive_bypass);
                continue;
            }
            if (ind == 2 && value.empty()) {
                sub = key;
                continue;
            }
            if (ind == 4 && !value.empty()) {
                IoLineCfg* target = nullptr;
                if (sub == "do_curtail") {
                    target = &cfg.do_curtail;
                } else if (sub == "di_permissive") {
                    target = &cfg.di_permissive;
                }
                if (target != nullptr) {
                    if (key == "gpio") {
                        target->gpio = std::atoi(value.c_str());
                    } else if (key == "active_high") {
                        target->active_high = parse_bool(value, target->active_high);
                    } else if (key == "active_low") {
                        target->active_high = !parse_bool(value, !target->active_high);
                    }
                } else if (sub == "annex_m_trip_monitor") {
                    if (key == "enabled") {
                        cfg.annex_m_trip_monitor.enabled =
                            parse_bool(value, cfg.annex_m_trip_monitor.enabled);
                    } else if (key == "gpio") {
                        cfg.annex_m_trip_monitor.gpio = std::atoi(value.c_str());
                    }
                }
            }
            continue;
        }

        if (top == "event_log" && ind == 2 && !value.empty()) {
            if (key == "enabled") {
                cfg.event_log.enabled = parse_bool(value, cfg.event_log.enabled);
            } else if (key == "path") {
                cfg.event_log.path = value;
            } else if (key == "syslog_enabled") {
                cfg.event_log.syslog_enabled =
                    parse_bool(value, cfg.event_log.syslog_enabled);
            } else if (key == "platform_audit_enabled") {
                cfg.event_log.platform_audit_enabled =
                    parse_bool(value, cfg.event_log.platform_audit_enabled);
            } else if (key == "platform_updater_audit_path") {
                cfg.event_log.platform_updater_audit_path = value;
            } else if (key == "syslog_host") {
                cfg.event_log.syslog_host = value;
            } else if (key == "syslog_port") {
                int port = 0;
                const auto result = std::from_chars(value.data(), value.data() + value.size(), port);
                cfg.event_log.syslog_port = result.ec == std::errc{} && result.ptr == value.data() + value.size() ? port : 0;
            } else if (key == "syslog_source") {
                cfg.event_log.syslog_source = value;
            } else if (key == "syslog_app_name") {
                cfg.event_log.syslog_app_name = value;
            } else if (key == "syslog_tls_ca") {
                cfg.event_log.syslog_tls_ca = value;
            } else if (key == "syslog_tls_cert") {
                cfg.event_log.syslog_tls_cert = value;
            } else if (key == "syslog_tls_key") {
                cfg.event_log.syslog_tls_key = value;
            } else if (key == "syslog_tls_crl") {
                cfg.event_log.syslog_tls_crl = value;
            } else if (key == "syslog_tls_peer_sha256") {
                cfg.event_log.syslog_tls_peer_sha256 = value;
            } else if (key == "syslog_revocation_check_s" || key == "syslog_renegotiation_s" || key == "syslog_resumption_s") {
                int seconds = 0;
                const auto result = std::from_chars(value.data(), value.data() + value.size(), seconds);
                if (result.ec != std::errc{} || result.ptr != value.data() + value.size()) seconds = 0;
                if (key == "syslog_revocation_check_s") cfg.event_log.syslog_revocation_check_s = seconds;
                else if (key == "syslog_renegotiation_s") cfg.event_log.syslog_renegotiation_s = seconds;
                else cfg.event_log.syslog_resumption_s = seconds;
            } else if (key == "watch_ifaces") {
                cfg.event_log.watch_ifaces = value;
            }
        }
    }

    apply_modbus_pdc_defaults(cfg.modbus);
    apply_modbus_pdc_defaults(cfg.inverter_modbus);
    auto& watched = cfg.event_log.platform_credential_paths;
    auto add_path = [&watched](const std::string& path) {
        if (!path.empty() && std::find(watched.begin(), watched.end(), path) == watched.end())
            watched.push_back(path);
    };
    for (const auto& path : {cfg.mms.tls.own_key, cfg.mms.tls.own_cert,
         cfg.mms.tls.additional_own_key, cfg.mms.tls.additional_own_cert,
         cfg.mms.tls.acse_cert, cfg.mms.tls.acse_key, cfg.mms.tls.ca_cert, cfg.mms.tls.client_cert,
         cfg.mms.tls.acse_client_cert, cfg.mms.tls.viewer_cert, cfg.mms.tls.acse_viewer_cert,
         cfg.mms.tls.crl_path, cfg.mms.tls.revoked_cert, cfg.mms.tls.additional_client_cert,
         cfg.mms.rbac_owner_cert, cfg.mms.rbac_policy_path,
         cfg.iec104.tls.own_key, cfg.iec104.tls.own_cert, cfg.iec104.tls.ca_cert,
         cfg.iec104.tls.client_cert, cfg.iec104.tls.crl_path,
         cfg.event_log.syslog_tls_ca, cfg.event_log.syslog_tls_cert,
         cfg.event_log.syslog_tls_key, cfg.event_log.syslog_tls_crl}) add_path(path);
    for (const auto& path : cfg.mms.tls.allowed_client_certs) add_path(path);
    for (const auto& path : cfg.iec104.tls.allowed_client_certs) add_path(path);
    cfg.loaded_from_file = true;
    return cfg;
}

}  // namespace cci::core
