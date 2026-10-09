/*=============================================================================
 * File       :  ccli_config.cpp
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  Minimal indentation-aware YAML reader for the CCLI lab config.
 *               Only the pf2 and io sections are consumed; everything else is
 *               ignored. Not a general YAML parser.
 *=============================================================================*/
#include "config/ccli_config.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>

namespace cci::core {

namespace {

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
            }
            continue;
        }

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
            } else if (key == "tls_chain_validation") {
                cfg.mms.tls.chain_validation =
                    parse_bool(value, cfg.mms.tls.chain_validation);
            } else if (key == "comms_loss_fallback_s") {
                cfg.mms.comms_loss_fallback_s = std::atoi(value.c_str());
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
            } else if (key == "syslog_host") {
                cfg.event_log.syslog_host = value;
            } else if (key == "syslog_port") {
                cfg.event_log.syslog_port = std::atoi(value.c_str());
            } else if (key == "watch_ifaces") {
                cfg.event_log.watch_ifaces = value;
            }
        }
    }

    cfg.loaded_from_file = true;
    return cfg;
}

}  // namespace cci::core
