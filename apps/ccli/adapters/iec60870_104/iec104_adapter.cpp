/**

 * Phase 4 — IEC 60870-5-104 CS104 slave on Eth_B (operator conduit).

 *

 * Regulation traceability: see iec104_regulation.hpp

 *   P4-01 O.13.1.1.1 — bind Eth_B only (never 0.0.0.0 / Eth_A)

 *   P4-02 IEC 60870-5-104 — STARTDT, GI, periodic monitor ASDU

 *   P4-03 IEC 62351-3 — optional TLS via CS104_Slave_createSecure

 *   P4-04 O.13.1.3 — comms_loss_fallback_s when operator link lost

 *   P4-05 O.14 — operator command audit with parameters (accept/reject)

 */



#include "iec104_adapter.hpp"

#include "iec104_operator_rule.hpp"

#include "iec104_regulation.hpp"

#include "iec104_tls.hpp"



#include <algorithm>

#include <atomic>

#include <cerrno>

#include <cstdio>

#include <cstdint>

#include <cstring>

#include <iostream>

#include <string>



#if defined(CCLI_HAVE_LIB60870)

extern "C" {

#include "cs101_information_objects.h"

#include "cs104_slave.h"

#include "hal_time.h"

#if defined(CONFIG_CS104_SUPPORT_TLS)

#include "tls_config.h"

#endif

}

#endif

namespace cci::adapters {



namespace {



bool is_forbidden_bind(const std::string& addr) {

    return addr.empty() || addr == "0.0.0.0" || addr == "::" || addr == "192.168.10.1";

}



#if defined(CCLI_HAVE_LIB60870)



struct CommandParams {

    int    ioa{0};

    double value{0.0};

    bool   state{false};

    bool   select{false};

    bool   has_value{false};

    bool   has_state{false};

};



CommandParams extract_command_params(CS101_ASDU asdu, int type_id) {

    CommandParams p;

    if (CS101_ASDU_getNumberOfElements(asdu) < 1) {

        return p;

    }

    InformationObject io = CS101_ASDU_getElement(asdu, 0);

    if (io == nullptr) {

        return p;

    }

    p.ioa = InformationObject_getObjectAddress(io);

    switch (type_id) {

    case C_SC_NA_1: {

        const auto sc = reinterpret_cast<SingleCommand>(io);

        p.state = SingleCommand_getState(sc);

        p.select = SingleCommand_isSelect(sc);

        p.has_state = true;

        break;

    }

    case C_SE_NC_1: {

        const auto sp = reinterpret_cast<SetpointCommandShort>(io);

        p.value = static_cast<double>(SetpointCommandShort_getValue(sp));

        p.select = SetpointCommandShort_isSelect(sp);

        p.has_value = true;

        break;

    }

    default:

        break;

    }

    InformationObject_destroy(io);

    return p;

}



std::string format_o14_detail(int type_id, const CommandParams& p, bool rejected) {

    char buf[256];

    const char* result = rejected ? "reject" : "accept";

    if (p.has_value) {

        std::snprintf(buf, sizeof(buf),

                      "O.14 type=%d ioa=%d value=%.3f select=%d result=%s", type_id, p.ioa,

                      p.value, p.select ? 1 : 0, result);

    } else if (p.has_state) {

        std::snprintf(buf, sizeof(buf),

                      "O.14 type=%d ioa=%d state=%d select=%d result=%s", type_id, p.ioa,

                      p.state ? 1 : 0, p.select ? 1 : 0, result);

    } else if (p.ioa != 0) {

        std::snprintf(buf, sizeof(buf), "O.14 type=%d ioa=%d result=%s", type_id, p.ioa, result);

    } else {

        std::snprintf(buf, sizeof(buf), "O.14 type=%d result=%s", type_id, result);

    }

    return std::string(buf);

}



#endif



}  // namespace



#if defined(CCLI_HAVE_LIB60870)



struct Iec104Adapter::Impl {

    CS104_Slave slave{nullptr};

    void* tls_config{nullptr};  /* P4-03 / 62351-3 — TLSConfiguration */

    CS101_AppLayerParameters alParams{nullptr};

    core::Iec104Config cfg{};

    bool running{false};

    double tot_w_kw{0.0};

    double msd_sp_kw{0.0};

    int msd_sp_ioa{0};

    std::atomic<int> active_clients{0};

    std::atomic<bool> fallback_pending{false};

    std::atomic<bool> audit_pending{false};

    int audit_type_id{0};

    bool audit_rejected{true};

    std::string audit_detail;

    std::uint64_t last_active_ms{0};

    std::uint64_t next_periodic_ms{0};

    sCS101_StaticASDU asdu{};

    std::uint8_t io_buf[250]{};



    void queue_audit(int type_id, bool rejected, const std::string& detail) {

        audit_type_id = type_id;

        audit_rejected = rejected;

        audit_detail = detail;

        audit_pending.store(true);

    }



    void send_tot_w(IMasterConnection connection, CS101_CauseOfTransmission cot) {

        if (alParams == nullptr || !cfg.operator_rule.monitor_tot_w) {

            return;

        }

        CS101_ASDU new_asdu = CS101_ASDU_initializeStatic(

            &asdu, alParams, false, cot, 0,

            static_cast<int>(cfg.common_address), false, false);



        const float value = static_cast<float>(tot_w_kw);

        CS101_ASDU_addInformationObject(

            new_asdu,

            reinterpret_cast<InformationObject>(MeasuredValueShort_create(

                reinterpret_cast<MeasuredValueShort>(&io_buf), cfg.ioa_tot_w, value,

                IEC60870_QUALITY_GOOD)));



        if (connection != nullptr) {

            IMasterConnection_sendASDU(connection, new_asdu);

        } else if (slave != nullptr) {

            CS104_Slave_enqueueASDU(slave, new_asdu);

        }

    }



    bool handle_command_accept(IMasterConnection connection, CS101_ASDU asdu, int type_id,

                               const CommandParams& params) {

        if (type_id == C_SE_NC_1) {

            if (params.ioa != cfg.operator_rule.ioa_msd_sp) {

                const auto detail = format_o14_detail(type_id, params, true);

                std::fprintf(stderr,

                             "iec104: MSD set-point IOA mismatch got=%d expected=%d [O.10.3.1]\n",

                             params.ioa, cfg.operator_rule.ioa_msd_sp);

                queue_audit(type_id, true, detail + " reason=ioa_mismatch");

                IMasterConnection_sendACT_CON(connection, asdu, true);

                return true;

            }

            if (params.select) {

                const auto detail = format_o14_detail(type_id, params, true);

                std::fprintf(stderr, "iec104: MSD select-only not applied [lab execute required]\n");

                queue_audit(type_id, true, detail + " reason=select_only");

                IMasterConnection_sendACT_CON(connection, asdu, true);

                return true;

            }

            msd_sp_kw = params.value;

            msd_sp_ioa = params.ioa;

            const auto detail = format_o14_detail(type_id, params, false);

            std::fprintf(stderr,

                         "iec104: O.10.3.1 MSD set-point accepted ioa=%d value=%.3f kW [lab stub]\n",

                         params.ioa, params.value);

            queue_audit(type_id, false, detail);

            IMasterConnection_sendACT_CON(connection, asdu, false);

            IMasterConnection_sendACT_TERM(connection, asdu);

            return true;

        }



        if (type_id == C_SC_NA_1) {

            const auto detail = format_o14_detail(type_id, params, false);

            std::fprintf(stderr, "iec104: operator single command accepted ioa=%d state=%d\n",

                         params.ioa, params.state ? 1 : 0);

            queue_audit(type_id, false, detail);

            IMasterConnection_sendACT_CON(connection, asdu, false);

            IMasterConnection_sendACT_TERM(connection, asdu);

            return true;

        }



        return false;

    }



    static bool interrogationHandler(void* parameter, IMasterConnection connection,

                                     CS101_ASDU asdu, std::uint8_t qoi) {

        auto* self = static_cast<Impl*>(parameter);

        if (self == nullptr) {

            return false;

        }

        if (!self->cfg.operator_rule.allow_gi) {

            char detail[128];

            std::snprintf(detail, sizeof(detail), "O.14 type=100 qoi=%u result=reject", qoi);

            std::fprintf(stderr,

                         "iec104: GI rejected — operator_allow_gi=false [O.14 Operating Rule]\n");

            self->queue_audit(100, true, detail);

            IMasterConnection_sendACT_CON(connection, asdu, true);

            return true;

        }

        if (qoi != 20) {

            IMasterConnection_sendACT_CON(connection, asdu, true);

            return true;

        }



        IMasterConnection_sendACT_CON(connection, asdu, false);

        self->send_tot_w(connection, CS101_COT_INTERROGATED_BY_STATION);

        IMasterConnection_sendACT_TERM(connection, asdu);

        return true;

    }



    static bool asduHandler(void* parameter, IMasterConnection connection, CS101_ASDU asdu) {

        auto* self = static_cast<Impl*>(parameter);

        if (self == nullptr) {

            return false;

        }

        const int type_id = CS101_ASDU_getTypeID(asdu);

        const auto& rule = self->cfg.operator_rule;

        const bool cmds_ok = regulation::iec104::operator_rule::mode_allows_commands(

            rule.mode, rule.allow_commands);

        const CommandParams params = extract_command_params(asdu, type_id);



        if (type_id == C_CS_NA_1) {

            const auto detail = format_o14_detail(type_id, params, !rule.allow_clock_sync || !cmds_ok);

            if (rule.allow_clock_sync && cmds_ok) {

                self->queue_audit(type_id, false, detail);

                std::fprintf(stderr, "iec104: %s\n", detail.c_str());

                return false;

            }

            std::fprintf(stderr,

                         "iec104: clock sync rejected type=%d [Operating Rule monitor_only]\n",

                         type_id);

            self->queue_audit(type_id, true, detail);

            IMasterConnection_sendACT_CON(connection, asdu, true);

            return true;

        }



        if (!cmds_ok) {

            const auto detail = format_o14_detail(type_id, params, true);

            std::fprintf(stderr,

                         "iec104: operator command rejected type=%d mode=%s allow_commands=%d "

                         "[O.14 / Operating Rule]\n",

                         type_id, rule.mode.c_str(), rule.allow_commands ? 1 : 0);

            std::fprintf(stderr, "iec104: %s\n", detail.c_str());

            self->queue_audit(type_id, true, detail);

            IMasterConnection_sendACT_CON(connection, asdu, true);

            return true;

        }



        if (!regulation::iec104::operator_rule::is_allowed_command_type(type_id)) {

            const auto detail = format_o14_detail(type_id, params, true);

            std::fprintf(stderr, "iec104: operator command type=%d not in Operating Rule catalog\n",

                         type_id);

            self->queue_audit(type_id, true, detail + " reason=unknown_type");

            IMasterConnection_sendACT_CON(connection, asdu, true);

            return true;

        }



        if (self->handle_command_accept(connection, asdu, type_id, params)) {

            return true;

        }



        const auto detail = format_o14_detail(type_id, params, false);

        self->queue_audit(type_id, false, detail);

        return false;

    }



    static void connectionEventHandler(void* parameter, IMasterConnection /*connection*/,

                                       CS104_PeerConnectionEvent event) {

        auto* self = static_cast<Impl*>(parameter);

        if (self == nullptr) {

            return;

        }

        const auto now = Hal_getTimeInMs();

        if (event == CS104_CON_EVENT_ACTIVATED) {

            self->active_clients.fetch_add(1);

            self->last_active_ms = now;

        } else if (event == CS104_CON_EVENT_DEACTIVATED || event == CS104_CON_EVENT_CONNECTION_CLOSED) {

            const int prev = self->active_clients.fetch_sub(1);

            if (prev <= 1) {

                self->active_clients.store(0);

            }

        } else if (event == CS104_CON_EVENT_CONNECTION_OPENED) {

            self->last_active_ms = now;

        }

    }

};



#else  /* !CCLI_HAVE_LIB60870 */



struct Iec104Adapter::Impl {};



#endif



Iec104Adapter::~Iec104Adapter() {

    stop();

}



bool Iec104Adapter::start(const core::Iec104Config& cfg) {

    stop();



#if !defined(CCLI_HAVE_LIB60870)

    (void)cfg;

    std::cerr << "iec104: lib60870 not linked (stub)\n";

    return false;

#else

    if (!cfg.enabled) {

        return false;

    }

    /* P4-01 / O.13.1.1.1 — operator traffic on Eth_B interface only */

    if (is_forbidden_bind(cfg.bind_address)) {

        std::cerr << "iec104: forbidden bind " << cfg.bind_address

                  << " (" << regulation::iec104::kAnnexO_EthB << " Eth_B only)\n";

        return false;

    }

    if (cfg.tcp_port <= 0 || cfg.tcp_port > 65535) {

        std::cerr << "iec104: invalid port " << cfg.tcp_port << "\n";

        return false;

    }



    auto* impl = new Impl{};

    impl->cfg = cfg;



    if (cfg.tls.enabled) {

        std::string tls_err;

        if (!iec104_create_tls_config(cfg.tls, &impl->tls_config, tls_err)) {

            delete impl;

            std::cerr << "iec104: TLS setup failed (" << regulation::iec104::kStd62351_3

                      << "): " << tls_err << "\n";

            return false;

        }

#if defined(CONFIG_CS104_SUPPORT_TLS)

        impl->slave = CS104_Slave_createSecure(100, 100,

                                               static_cast<TLSConfiguration>(impl->tls_config));

#else

        iec104_destroy_tls_config(impl->tls_config);

        delete impl;

        std::cerr << "iec104: TLS requested but CONFIG_CS104_SUPPORT_TLS=0\n";

        return false;

#endif

    } else {

        impl->slave = CS104_Slave_create(100, 100);

    }

    if (impl->slave == nullptr) {

        iec104_destroy_tls_config(impl->tls_config);

        delete impl;

        std::cerr << "iec104: CS104_Slave_create failed\n";

        return false;

    }



    CS104_Slave_setLocalAddress(impl->slave, cfg.bind_address.c_str());

    CS104_Slave_setLocalPort(impl->slave, cfg.tcp_port);

    CS104_Slave_setServerMode(impl->slave, CS104_MODE_SINGLE_REDUNDANCY_GROUP);

    CS104_Slave_setInterrogationHandler(impl->slave, Impl::interrogationHandler, impl);

    CS104_Slave_setASDUHandler(impl->slave, Impl::asduHandler, impl);

    CS104_Slave_setConnectionEventHandler(impl->slave, Impl::connectionEventHandler, impl);



    impl->alParams = CS104_Slave_getAppLayerParameters(impl->slave);



    CS104_Slave_startThreadless(impl->slave);

    if (!CS104_Slave_isRunning(impl->slave)) {

        CS104_Slave_destroy(impl->slave);

        delete impl;

        std::cerr << "iec104: startThreadless failed on " << cfg.bind_address << ":"

                  << cfg.tcp_port << " (" << std::strerror(errno) << ")\n";

        return false;

    }



    impl->running = true;

    impl->last_active_ms = Hal_getTimeInMs();

    impl->next_periodic_ms = impl->last_active_ms;

    impl_ = impl;



    const auto& orule = cfg.operator_rule;

    std::cerr << "iec104: listening " << cfg.bind_address << ":" << cfg.tcp_port

              << " CA=" << cfg.common_address << " tls=" << (cfg.tls.enabled ? "on" : "off")

              << " [" << regulation::iec104::kStd104 << " / "

              << regulation::iec104::kAnnexO_EthB << "]\n";

    std::cerr << "iec104: Operating Rule mode=" << orule.mode

              << " allow_commands=" << (orule.allow_commands ? "yes" : "no")

              << " gi=" << (orule.allow_gi ? "yes" : "no") << " monitor: TotW@"

              << cfg.ioa_tot_w << (orule.monitor_tot_w ? "=on" : "=off");

    if (orule.monitor_tot_var) {

        std::cerr << " TotVAr@" << orule.ioa_tot_var;

    }

    if (orule.monitor_ppv) {

        std::cerr << " PPV@" << orule.ioa_ppv;

    }

    if (orule.mode == regulation::iec104::operator_rule::kModeCommand) {

        std::cerr << " MSD_SP@" << orule.ioa_msd_sp << " [O.10.3.1]";

    }

    std::cerr << " [" << regulation::iec104::kAnnexO_Logging << "]\n";

    return true;

#endif

}



void Iec104Adapter::stop() {

#if defined(CCLI_HAVE_LIB60870)

    if (impl_ == nullptr) {

        return;

    }

    if (impl_->running && impl_->slave != nullptr) {

        CS104_Slave_stopThreadless(impl_->slave);

        CS104_Slave_destroy(impl_->slave);

    }

    iec104_destroy_tls_config(impl_->tls_config);

    delete impl_;

    impl_ = nullptr;

#endif

}



void Iec104Adapter::tick() {

#if defined(CCLI_HAVE_LIB60870)

    if (impl_ == nullptr || !impl_->running || impl_->slave == nullptr) {

        return;

    }



    CS104_Slave_tick(impl_->slave);



    const auto now = Hal_getTimeInMs();

    const int period_ms = std::max(1, impl_->cfg.periodic_s) * 1000;

    if (now >= impl_->next_periodic_ms && impl_->active_clients.load() > 0) {

        impl_->next_periodic_ms = now + static_cast<std::uint64_t>(period_ms);

        impl_->send_tot_w(nullptr, CS101_COT_PERIODIC);

    }



    if (impl_->cfg.comms_loss_fallback_s > 0 && impl_->active_clients.load() == 0 &&

        impl_->last_active_ms > 0) {

        const auto elapsed_s = (now - impl_->last_active_ms) / 1000U;

        if (elapsed_s >= static_cast<std::uint64_t>(impl_->cfg.comms_loss_fallback_s)) {

            impl_->fallback_pending.store(true);

            impl_->last_active_ms = now;

        }

    }

#endif

}



bool Iec104Adapter::is_running() const {

#if defined(CCLI_HAVE_LIB60870)

    return impl_ != nullptr && impl_->running;

#else

    return false;

#endif

}



void Iec104Adapter::update_tot_w_kw(const double p_kw) {

#if defined(CCLI_HAVE_LIB60870)

    if (impl_ != nullptr) {

        impl_->tot_w_kw = p_kw;

    }

#else

    (void)p_kw;

#endif

}



bool Iec104Adapter::poll_comms_loss_fallback() {

#if defined(CCLI_HAVE_LIB60870)

    if (impl_ == nullptr) {

        return false;

    }

    return impl_->fallback_pending.exchange(false);

#else

    return false;

#endif

}



bool Iec104Adapter::poll_operator_audit(OperatorAuditEvent& out) {

#if defined(CCLI_HAVE_LIB60870)

    if (impl_ == nullptr || !impl_->audit_pending.exchange(false)) {

        out.valid = false;

        return false;

    }

    out.valid = true;

    out.type_id = impl_->audit_type_id;

    out.rejected = impl_->audit_rejected;

    out.detail = impl_->audit_detail;

    return true;

#else

    (void)out;

    return false;

#endif

}



int Iec104Adapter::client_count() const {

#if defined(CCLI_HAVE_LIB60870)

    if (impl_ == nullptr) {

        return 0;

    }

    return impl_->active_clients.load();

#else

    return 0;

#endif

}



}  // namespace cci::adapters


