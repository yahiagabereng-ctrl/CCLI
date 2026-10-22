#pragma once

#include "config/ccli_config.hpp"

#include <string>

namespace cci::adapters {

/**
 * Phase 4 Eth_B IEC 60870-5-104 slave (lib60870 CS104 threadless).
 *
 * Regulation: iec104_regulation.hpp
 *   P4-01 O.13.1.1.1 — Eth_B bind only
 *   P4-02 IEC 60870-5-104 — monitor + GI
 *   P4-03 IEC 62351-3 — optional TLS
 *   P4-04 O.13.1.3 — comms-loss fallback
 *   Operating Rule — annex monitor/command allow-list (operator_rule in yaml)
 */
class Iec104Adapter {
public:
    /** O.14 — operator command ASDU with parameters (accept or reject). */
    struct OperatorAuditEvent {
        bool        valid{false};
        int         type_id{0};
        bool        rejected{true};
        std::string detail; /**< e.g. O.14 type=50 ioa=3001 value=42.500 result=accept */
    };
    Iec104Adapter() = default;
    ~Iec104Adapter();

    Iec104Adapter(const Iec104Adapter&) = delete;
    Iec104Adapter& operator=(const Iec104Adapter&) = delete;

    bool start(const core::Iec104Config& cfg);
    void stop();
    void tick();
    bool is_running() const;

    void update_tot_w_kw(double p_kw);

    /** P4-04: true once when comms-loss fallback timer fires. */
    bool poll_comms_loss_fallback();

    /** O.14: one-shot operator ASDU audit (reject in monitor_only). */
    bool poll_operator_audit(OperatorAuditEvent& out);

    int client_count() const;

private:
    struct Impl;
    Impl* impl_{nullptr};
};

}  // namespace cci::adapters
