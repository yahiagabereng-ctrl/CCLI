/*=============================================================================
 * File       :  CCLI_TOOLS.HPP
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  PROGRAM - one-shot CLI tools and bench status file (C++),
 *               shared by program/ccli_bridge.cpp and services/ccli_main.cpp
 *
 * Copyright HiTEKS (c) 2026
 *=============================================================================*/
#pragma once

#include "config/ccli_config.hpp"
#include "dso/dso_phase1.hpp"
#include "measurement/measurement_store.hpp"
#include "mms_adapter.hpp"
#include "modbus_adapter.hpp"
#include "pf2/pf2_fsm.hpp"
#include "regulation/regulation_bench.hpp"

#include <atomic>
#include <cstddef>
#include <string>

namespace cci::program {

constexpr const char* kBenchStatusPath = "/var/run/ccli-status.json";

/** Read-only zone view for the lab dashboard (DSO Eth_A · operator Eth_B · plant). */
struct ZoneStatusSnapshot {
    bool                          mms_running{false};
    int                           mms_clients{0};
    bool                          gnss_fix{false};
    bool                          dso_cmd_seen{false};
    cci::adapters::DsoLiveCommand dso_cmd{};
    bool                          iec104_running{false};
    int                           iec104_clients{0};
    int                           modbus_link_state{0}; /* 0=unknown, 1=up, -1=down */
    cci::adapters::ModbusExchangeRecord modbus_last{};
};

void write_bench_status_file(const std::string& path, const cci::core::CcliConfig& cfg,
                             const cci::core::Measurement& m, const cci::core::Pf2State& pf2_st,
                             bool curtailment_commanded, bool permissive_ok,
                             bool annex_m_trip_active, bool do_curtail_on,
                             const cci::core::dso::DsoPhase1ApplyResult& dso_apply,
                             const ZoneStatusSnapshot& z);

int run_bench_status_json();
int run_bench_write_config(const std::string& path);

void print_usage();

int run_event_dump(const std::string& config_path, std::size_t count, bool json_out);
int run_event_wrap_test();

cci::core::regulation::ModbusLiveTrace modbus_trace_from(
    const cci::adapters::ModbusExchangeRecord& ex);

int run_regulation_check(const std::string& config_path, bool live, bool json_out,
                         int live_watch_sec);

void gpio_blink_curtail(int count, const std::atomic<bool>& running);
int  run_gpio_test(std::atomic<bool>* running);

}  // namespace cci::program
