/*=============================================================================
 * Eth_A MMS server — CEI 0-16 Allegato O + T / TR 57-126 Figura 2.
 *
 * Hierarchy (T.3.3.1 · signal_map.yaml):
 *   IED CCI016_01
 *     └─ LD LD_Plant
 *          ├─ LLN0
 *          ├─ WlimDWMX1.{Mod,WMaxSptPct}     O.9.2.2 lim W     — IMPL
 *          ├─ WSdDAGC1.{Mod,WSptPct}         O.9.2.3 s.p. W    — IMPL (Figura 2 green)
 *          ├─ WSaDAGC1.Mod                   O.10.3.1          — STUB Mod=5
 *          ├─ VArSdDVAR1.{Mod,VArTgtSptPct} O.9.1.4          — P5-R01 IMPL
 *          ├─ PFSPDFPF1.{Mod,PFGnTgtSpt,PFLodTgtSpt} O.9.1.1  — P5-R02 IMPL
 *          ├─ VArVDVVR1.Mod                  O.9.1.3           — STUB
 *          ├─ PFWDPFW1.Mod                   O.9.1.2           — STUB
 *          └─ PdCMMXU1.{TotW,TotVAr} + urcb_PdC_Mis4sec (4 s)  — TotW PART · TotVAr P5-M07
 *
 * Soft gates: TLS / RBAC DSO_OPERATOR / CRL / P3-05 Operating Rule fallback
 *=============================================================================*/
#include "mms_adapter.hpp"
#include "mms_acse_auth.hpp"
#include "mms_aare_auth.hpp"
#include "gnss_time.hpp"
#include "dso/setpoint_gate.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <time.h>

#if defined(CCLI_HAVE_LIBIEC61850)
extern "C" {
#include "iec61850_server.h"
#include "iec61850_cdc.h"
#include "iec61850_dynamic_model.h"
#include "iec61850_config_file_parser.h"
#include "iec61850_model.h"
#include "hal_time.h"
#if defined(CONFIG_MMS_SUPPORT_TLS)
#include "tls_config.h"
#endif
}
#endif

namespace cci::adapters {

namespace {

/* Opaque security tokens (stable addresses for ClientConnection_getSecurityToken). */
static int g_token_dso = static_cast<int>(MmsRole::DsoOperator);
static int g_token_viewer = static_cast<int>(MmsRole::Viewer);

static int b64_val(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    }
    if (c == '+') {
        return 62;
    }
    if (c == '/') {
        return 63;
    }
    return -1;
}

/** Extract DER from a PEM certificate file (first CERTIFICATE block). */
static bool load_pem_der(const std::string& path, std::vector<uint8_t>& out) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    std::string pem((std::istreambuf_iterator<char>(in)),
                    std::istreambuf_iterator<char>());
    const auto begin = pem.find("-----BEGIN CERTIFICATE-----");
    const auto end = pem.find("-----END CERTIFICATE-----");
    if (begin == std::string::npos || end == std::string::npos || end <= begin) {
        return false;
    }
    std::string b64;
    for (size_t i = begin + 27; i < end; ++i) {
        const char c = pem[i];
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
            continue;
        }
        b64.push_back(c);
    }
    out.clear();
    out.reserve(b64.size() * 3 / 4);
    int val = 0;
    int valb = -8;
    for (char c : b64) {
        const int d = b64_val(c);
        if (d < 0) {
            continue;
        }
        val = (val << 6) + d;
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return !out.empty();
}

static bool der_eq(const std::vector<uint8_t>& a, const uint8_t* buf, int len) {
    if (buf == nullptr || len <= 0) {
        return false;
    }
    if (static_cast<int>(a.size()) != len) {
        return false;
    }
    return std::memcmp(a.data(), buf, static_cast<size_t>(len)) == 0;
}

}  // namespace

#if defined(CCLI_HAVE_LIBIEC61850)

struct MmsAdapter::Impl {
    IedModel* model{nullptr};
    IedServer server{nullptr};
#if defined(CONFIG_MMS_SUPPORT_TLS)
    TLSConfiguration tls{nullptr};
#endif
    DataObject* wlim_mod{nullptr};
    DataObject* wlim_wmax{nullptr};
    DataObject* wsd_mod{nullptr};
    DataObject* wsd_wspt{nullptr};
    DataObject* varsd_mod{nullptr};
    DataObject* varsd_vartgt{nullptr};
    DataObject* pfsp_mod{nullptr};
    DataObject* pfsp_gn_tgt{nullptr};
    DataObject* pfsp_lod_tgt{nullptr};
    DataAttribute* totw_mag{nullptr};
    DataAttribute* totw_t{nullptr};
    DataAttribute* totw_q{nullptr};
    DataAttribute* totvar_mag{nullptr};
    DataAttribute* totvar_t{nullptr};
    DataAttribute* totvar_q{nullptr};
    DataAttribute* ppv_phsab_mag{nullptr};
    DataAttribute* ppv_phsab_t{nullptr};
    DataAttribute* ppv_phsab_q{nullptr};
    DataAttribute* wlim_mod_stval{nullptr};
    DataAttribute* wmax_mxval{nullptr};
    DataAttribute* wsd_mod_stval{nullptr};
    DataAttribute* wspt_mxval{nullptr};
    DataAttribute* varsd_mod_stval{nullptr};
    DataAttribute* vartgt_mxval{nullptr};
    DataAttribute* pfsp_mod_stval{nullptr};
    DataAttribute* pfsp_gn_mxval{nullptr};
    DataAttribute* pfsp_lod_mxval{nullptr};
    bool running{false};

    std::vector<uint8_t> dso_der;
    std::vector<uint8_t> viewer_der;
    bool rbac_enabled{false};

    int gnss_poll_s{15};
    bool gnss_discipline{false};
    bool chrony_poll{true};
    uint64_t last_gnss_poll_mono_ms{0}; /**< CLOCK_MONOTONIC — immune to STEPs */
    uint64_t last_gnss_step_mono_ms{0}; /**< rate-limit STEPs (modem NMEA is noisy) */
    bool gnss_fix{false};
    bool gnss_queried{false};
    std::int64_t last_offset_ms{0};

    mutable std::mutex mu;
    DsoLiveCommand live{};
    int client_count{0};
    int fallback_s{0};
    uint64_t last_zero_clients_ms{0};
    bool fallback_pending{false};
    bool fallback_latched{false};
    bool full_cid_model{false};
    MmsAuditFn audit{};

    /* O.7.3.3 / Eq (9) — R07: one gate across all DSO set-point objects (guarded by mu). */
    core::dso::SetpointSpacingGate spacing_gate{core::dso::kSetpointMinIntervalMsO733};

    void emit_audit(const char* detail) const {
        if (audit && detail != nullptr) {
            audit(detail);
        }
    }

    static uint64_t mono_now_ms() {
        timespec mono{};
        clock_gettime(CLOCK_MONOTONIC, &mono);
        return static_cast<uint64_t>(mono.tv_sec) * 1000ULL +
               static_cast<uint64_t>(mono.tv_nsec / 1000000L);
    }

    /**
     * Apply O.7.3.3 to one external set-point write. Returns false (and logs /
     * audits) when the write arrives sooner than the configured interval.
     * Caller must NOT hold mu.
     */
    bool setpoint_spacing_accept(const char* object_ref) {
        uint64_t wait_ms = 0;
        {
            std::lock_guard<std::mutex> lock(mu);
            const uint64_t now = mono_now_ms();
            if (spacing_gate.accept(now)) {
                return true;
            }
            wait_ms = spacing_gate.remaining_ms(now);
        }
        std::fprintf(stderr,
                     "mms: %s REJECT — set-point spacing < %u ms (wait %llu ms) [O.7.3.3 R07]\n",
                     object_ref, spacing_gate.min_interval_ms(),
                     static_cast<unsigned long long>(wait_ms));
        emit_audit("setpoint_reject_spacing_o733");
        return false;
    }

    static CheckHandlerResult check_tramp(ControlAction action, void* parameter,
                                          MmsValue* ctlVal, bool test,
                                          bool interlockCheck);
    static ControlHandlerResult control_tramp(ControlAction action, void* parameter,
                                              MmsValue* value, bool test);
    static bool auth_tramp(void* parameter, AcseAuthenticationParameter authParameter,
                           void** securityToken, IsoApplicationReference* appReference);
    static void conn_tramp(IedServer self, ClientConnection connection, bool connected,
                           void* parameter);
    static bool wire_cfg_runtime_nodes(Impl* impl);
    static bool create_mvp_model(Impl* impl);
    static bool create_mms_model(const core::MmsConfig& cfg, Impl* impl);
    ControlHandlerResult on_control(ControlAction action, MmsValue* value, bool test);
};

CheckHandlerResult MmsAdapter::Impl::check_tramp(ControlAction action, void* parameter,
                                                 MmsValue* /*ctlVal*/, bool /*test*/,
                                                 bool /*interlockCheck*/) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr) {
        return CONTROL_OBJECT_ACCESS_DENIED;
    }
    if (!impl->rbac_enabled) {
        return CONTROL_ACCEPTED;
    }
    ClientConnection cc = ControlAction_getClientConnection(action);
    void* tok = (cc != nullptr) ? ClientConnection_getSecurityToken(cc) : nullptr;
    if (tok == &g_token_dso) {
        return CONTROL_ACCEPTED;
    }
    std::fprintf(stderr,
                 "mms: RBAC DENY Wlim/WSd — role is not DSO_OPERATOR (62351-8 / T.3.3.4)\n");
    impl->emit_audit("rbac_deny_control");
    return CONTROL_OBJECT_ACCESS_DENIED;
}

ControlHandlerResult MmsAdapter::Impl::control_tramp(ControlAction action,
                                                     void* parameter,
                                                     MmsValue* value, bool test) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr) {
        return CONTROL_RESULT_FAILED;
    }
    return impl->on_control(action, value, test);
}

bool MmsAdapter::Impl::auth_tramp(void* parameter,
                                  AcseAuthenticationParameter authParameter,
                                  void** securityToken,
                                  IsoApplicationReference* /*appReference*/) {
    auto* impl = static_cast<Impl*>(parameter);
    if (securityToken != nullptr) {
        *securityToken = nullptr;
    }
    if (authParameter == nullptr) {
        std::fprintf(stderr, "mms: ACSE auth REJECT — no auth parameter\n");
        if (impl != nullptr) {
            impl->emit_audit("auth_reject_no_parameter");
        }
        return false;
    }

    const MmsAcseMechanism mechanism =
        mms_acse_mechanism_from_libiec61850(static_cast<int>(authParameter->mechanism));
    const uint8_t* buf = authParameter->value.certificate.buf;
    const int len = authParameter->value.certificate.length;

    MmsAcseCredentialStore creds;
    if (impl != nullptr) {
        creds.dso_der = impl->dso_der;
        creds.viewer_der = impl->viewer_der;
        creds.rbac_enabled = impl->rbac_enabled;
    }

    const MmsAcseAuthResult verdict = evaluate_acse_authentication(creds, mechanism, buf, len);

    switch (verdict) {
    case MmsAcseAuthResult::AcceptDso:
    case MmsAcseAuthResult::AcceptGeneric:
        if (securityToken != nullptr) {
            *securityToken = &g_token_dso;
        }
        std::fprintf(stderr,
                     "mms: ACSE auth ACCEPT — mech=%s role=DSO_OPERATOR cert=%d octets\n",
                     mms_acse_mechanism_label(mechanism), len);
        if (impl != nullptr) {
            impl->emit_audit("auth_accept_dso_operator");
        }
        return true;
    case MmsAcseAuthResult::AcceptViewer:
        if (securityToken != nullptr) {
            *securityToken = &g_token_viewer;
        }
        std::fprintf(stderr,
                     "mms: ACSE auth ACCEPT — mech=%s role=VIEWER cert=%d octets\n",
                     mms_acse_mechanism_label(mechanism), len);
        if (impl != nullptr) {
            impl->emit_audit("auth_accept_viewer");
        }
        return true;
    case MmsAcseAuthResult::RejectMechanism:
        std::fprintf(stderr,
                     "mms: ACSE auth REJECT — mechanism=%s (%d); need TLS or CERTIFICATE\n",
                     mms_acse_mechanism_label(mechanism),
                     static_cast<int>(mechanism));
        if (impl != nullptr) {
            impl->emit_audit("auth_reject_mechanism");
        }
        return false;
    case MmsAcseAuthResult::RejectEmptyCert:
        std::fprintf(stderr,
                     "mms: ACSE auth REJECT — empty certificate (mech=%s)\n",
                     mms_acse_mechanism_label(mechanism));
        if (impl != nullptr) {
            impl->emit_audit("auth_reject_empty_cert");
        }
        return false;
    case MmsAcseAuthResult::RejectUnknownCert:
        std::fprintf(stderr,
                     "mms: ACSE auth REJECT — cert not mapped to lab role (%d octets, mech=%s)\n",
                     len, mms_acse_mechanism_label(mechanism));
        if (impl != nullptr) {
            impl->emit_audit("auth_reject_unknown_cert");
        }
        return false;
    }
    return false;
}

void MmsAdapter::Impl::conn_tramp(IedServer /*self*/, ClientConnection /*connection*/,
                                  bool connected, void* parameter) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(impl->mu);
    if (connected) {
        impl->client_count += 1;
        impl->fallback_latched = false;
        impl->fallback_pending = false;
        impl->last_zero_clients_ms = 0;
        std::fprintf(stderr, "mms: client CONNECT count=%d\n", impl->client_count);
        impl->emit_audit("client_connect");
    } else {
        if (impl->client_count > 0) {
            impl->client_count -= 1;
        }
        std::fprintf(stderr, "mms: client DISCONNECT count=%d\n", impl->client_count);
        impl->emit_audit("client_disconnect");
        if (impl->client_count == 0 && impl->fallback_s > 0) {
            impl->last_zero_clients_ms = Hal_getTimeInMs();
            std::fprintf(stderr,
                         "mms: no clients — Operating Rule fallback armed (%d s)\n",
                         impl->fallback_s);
        }
    }
}

ControlHandlerResult MmsAdapter::Impl::on_control(ControlAction action, MmsValue* value,
                                                  bool test) {
    if (value == nullptr || test) {
        return CONTROL_RESULT_FAILED;
    }

    DataObject* ctl = ControlAction_getControlObject(action);
    if (ctl == nullptr) {
        return CONTROL_RESULT_FAILED;
    }

    const MmsType ty = MmsValue_getType(value);
    bool handled = false;

    /* ---- O.9.2.2 WlimDWMX1 (lim W) ---- */
    if (ctl == wlim_mod) {
        if (ty == MMS_INTEGER || ty == MMS_UNSIGNED || ty == MMS_BOOLEAN) {
            const int v = (ty == MMS_BOOLEAN) ? (MmsValue_getBoolean(value) ? 1 : 5)
                                              : MmsValue_toInt32(value);
            const bool active = (v == 1);
            {
                std::lock_guard<std::mutex> lock(mu);
                live.wlim_active = active;
                live.valid = true;
                live.dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && wlim_mod_stval != nullptr) {
                IedServer_updateInt32AttributeValue(server, wlim_mod_stval, v);
            }
            std::fprintf(stderr, "mms: WlimDWMX1.Mod ctlVal=%d → wlim_active=%s [O.9.2.2]\n",
                         v, active ? "true" : "false");
            handled = true;
        }
    } else if (ctl == wlim_wmax) {
        float pct = 0.0f;
        bool got = false;
        if (ty == MMS_FLOAT) {
            pct = MmsValue_toFloat(value);
            got = true;
        } else if (ty == MMS_STRUCTURE) {
            MmsValue* f = MmsValue_getElement(value, 0);
            if (f != nullptr && MmsValue_getType(f) == MMS_FLOAT) {
                pct = MmsValue_toFloat(f);
                got = true;
            }
        }
        if (got && !setpoint_spacing_accept("WlimDWMX1.WMaxSptPct")) {
            return CONTROL_RESULT_FAILED;
        }
        if (got) {
            {
                std::lock_guard<std::mutex> lock(mu);
                live.wmax_spt_pct = static_cast<double>(pct);
                live.valid = true;
                live.dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && wmax_mxval != nullptr) {
                IedServer_updateFloatAttributeValue(server, wmax_mxval, pct);
            }
            std::fprintf(stderr, "mms: WlimDWMX1.WMaxSptPct ctlVal=%.2f [O.9.2.2]\n",
                         static_cast<double>(pct));
            handled = true;
        }
    /* ---- O.9.2.3 WSdDAGC1 (Figura 2 s.p. W) ---- */
    } else if (ctl == wsd_mod) {
        if (ty == MMS_INTEGER || ty == MMS_UNSIGNED || ty == MMS_BOOLEAN) {
            const int v = (ty == MMS_BOOLEAN) ? (MmsValue_getBoolean(value) ? 1 : 5)
                                              : MmsValue_toInt32(value);
            const bool active = (v == 1);
            {
                std::lock_guard<std::mutex> lock(mu);
                live.wsd_active = active;
                live.valid = true;
                live.dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && wsd_mod_stval != nullptr) {
                IedServer_updateInt32AttributeValue(server, wsd_mod_stval, v);
            }
            std::fprintf(stderr, "mms: WSdDAGC1.Mod ctlVal=%d → wsd_active=%s [O.9.2.3]\n",
                         v, active ? "true" : "false");
            handled = true;
        }
    } else if (ctl == wsd_wspt) {
        float pct = 0.0f;
        bool got = false;
        if (ty == MMS_FLOAT) {
            pct = MmsValue_toFloat(value);
            got = true;
        } else if (ty == MMS_STRUCTURE) {
            MmsValue* f = MmsValue_getElement(value, 0);
            if (f != nullptr && MmsValue_getType(f) == MMS_FLOAT) {
                pct = MmsValue_toFloat(f);
                got = true;
            }
        }
        if (got && !setpoint_spacing_accept("WSdDAGC1.WSptPct")) {
            return CONTROL_RESULT_FAILED;
        }
        if (got) {
            {
                std::lock_guard<std::mutex> lock(mu);
                live.wspt_pct = static_cast<double>(pct);
                live.valid = true;
                live.dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && wspt_mxval != nullptr) {
                IedServer_updateFloatAttributeValue(server, wspt_mxval, pct);
            }
            std::fprintf(stderr, "mms: WSdDAGC1.WSptPct ctlVal=%.2f [O.9.2.3 Figura2 s.p.W]\n",
                         static_cast<double>(pct));
            handled = true;
        }
    /* ---- O.9.1.4 VArSdDVAR1 (s.p. Q) — P5-R01 ---- */
    } else if (ctl == varsd_mod) {
        if (ty == MMS_INTEGER || ty == MMS_UNSIGNED || ty == MMS_BOOLEAN) {
            const int v = (ty == MMS_BOOLEAN) ? (MmsValue_getBoolean(value) ? 1 : 5)
                                              : MmsValue_toInt32(value);
            const bool active = (v == 1);
            {
                std::lock_guard<std::mutex> lock(mu);
                live.varsd_active = active;
                live.valid = true;
                live.reactive_dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && varsd_mod_stval != nullptr) {
                IedServer_updateInt32AttributeValue(server, varsd_mod_stval, v);
            }
            std::fprintf(stderr, "mms: VArSdDVAR1.Mod ctlVal=%d → varsd_active=%s [O.9.1.4]\n",
                         v, active ? "true" : "false");
            handled = true;
        }
    } else if (ctl == varsd_vartgt) {
        float pct = 0.0f;
        bool got = false;
        if (ty == MMS_FLOAT) {
            pct = MmsValue_toFloat(value);
            got = true;
        } else if (ty == MMS_STRUCTURE) {
            MmsValue* f = MmsValue_getElement(value, 0);
            if (f != nullptr && MmsValue_getType(f) == MMS_FLOAT) {
                pct = MmsValue_toFloat(f);
                got = true;
            }
        }
        if (got && !setpoint_spacing_accept("VArSdDVAR1.VArTgtSptPct")) {
            return CONTROL_RESULT_FAILED;
        }
        if (got) {
            {
                std::lock_guard<std::mutex> lock(mu);
                live.vartgt_spt_pct = static_cast<double>(pct);
                live.valid = true;
                live.reactive_dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && vartgt_mxval != nullptr) {
                IedServer_updateFloatAttributeValue(server, vartgt_mxval, pct);
            }
            std::fprintf(stderr,
                         "mms: VArSdDVAR1.VArTgtSptPct ctlVal=%.2f [O.9.1.4 P5-R01]\n",
                         static_cast<double>(pct));
            handled = true;
        }
    /* ---- O.9.1.1 PFSPDFPF1 (cosφ) — P5-R02 ---- */
    } else if (ctl == pfsp_mod) {
        if (ty == MMS_INTEGER || ty == MMS_UNSIGNED || ty == MMS_BOOLEAN) {
            const int v = (ty == MMS_BOOLEAN) ? (MmsValue_getBoolean(value) ? 1 : 5)
                                              : MmsValue_toInt32(value);
            const bool active = (v == 1);
            {
                std::lock_guard<std::mutex> lock(mu);
                live.pfsp_active = active;
                live.valid = true;
                live.reactive_dirty = true;
                live.pfsp_dirty = true;
                fallback_latched = false;
            }
            if (server != nullptr && pfsp_mod_stval != nullptr) {
                IedServer_updateInt32AttributeValue(server, pfsp_mod_stval, v);
            }
            std::fprintf(stderr, "mms: PFSPDFPF1.Mod ctlVal=%d → pfsp_active=%s [O.9.1.1]\n",
                         v, active ? "true" : "false");
            handled = true;
        }
    } else if (ctl == pfsp_gn_tgt || ctl == pfsp_lod_tgt) {
        float cosphi = 0.0f;
        bool got = false;
        if (ty == MMS_FLOAT) {
            cosphi = MmsValue_toFloat(value);
            got = true;
        } else if (ty == MMS_STRUCTURE) {
            MmsValue* f = MmsValue_getElement(value, 0);
            if (f != nullptr && MmsValue_getType(f) == MMS_FLOAT) {
                cosphi = MmsValue_toFloat(f);
                got = true;
            }
        }
        const bool generation = (ctl == pfsp_gn_tgt);
        const char* ref =
            generation ? "PFSPDFPF1.PFGnTgtSpt" : "PFSPDFPF1.PFLodTgtSpt";
        if (got && !setpoint_spacing_accept(ref)) {
            return CONTROL_RESULT_FAILED;
        }
        if (got) {
            {
                std::lock_guard<std::mutex> lock(mu);
                live.pfsp_cosphi = static_cast<double>(cosphi);
                live.pfsp_generation = generation;
                live.valid = true;
                live.reactive_dirty = true;
                live.pfsp_dirty = true;
                fallback_latched = false;
            }
            DataAttribute* mx =
                generation ? pfsp_gn_mxval : pfsp_lod_mxval;
            if (server != nullptr && mx != nullptr) {
                IedServer_updateFloatAttributeValue(server, mx, cosphi);
            }
            std::fprintf(stderr,
                         "mms: PFSPDFPF1.%s ctlVal=%.4f [O.9.1.1 P5-R02]\n",
                         generation ? "PFGnTgtSpt" : "PFLodTgtSpt",
                         static_cast<double>(cosphi));
            handled = true;
        }
    }

    return handled ? CONTROL_RESULT_OK : CONTROL_RESULT_FAILED;
}

static DataObject* model_do(IedModel* model, const char* ref) {
    ModelNode* node = IedModel_getModelNodeByShortObjectReference(model, ref);
    if (node == nullptr || ModelNode_getType(node) != DataObjectModelType) {
        return nullptr;
    }
    return reinterpret_cast<DataObject*>(node);
}

static DataAttribute* model_da(IedModel* model, const char* ref) {
    ModelNode* node = IedModel_getModelNodeByShortObjectReference(model, ref);
    if (node == nullptr || ModelNode_getType(node) != DataAttributeModelType) {
        return nullptr;
    }
    return reinterpret_cast<DataAttribute*>(node);
}

bool MmsAdapter::Impl::wire_cfg_runtime_nodes(MmsAdapter::Impl* impl) {
    IedModel* model = impl->model;
    impl->wlim_mod = model_do(model, "LD_Plant/WlimDWMX1.Mod");
    impl->wlim_wmax = model_do(model, "LD_Plant/WlimDWMX1.WMaxSptPct");
    impl->wsd_mod = model_do(model, "LD_Plant/WSdDAGC1.Mod");
    impl->wsd_wspt = model_do(model, "LD_Plant/WSdDAGC1.WSptPct");

    impl->wlim_mod_stval = model_da(model, "LD_Plant/WlimDWMX1.Mod.stVal");
    impl->wmax_mxval = model_da(model, "LD_Plant/WlimDWMX1.WMaxSptPct.mxVal.f");
    impl->wsd_mod_stval = model_da(model, "LD_Plant/WSdDAGC1.Mod.stVal");
    impl->wspt_mxval = model_da(model, "LD_Plant/WSdDAGC1.WSptPct.mxVal.f");
    impl->varsd_mod = model_do(model, "LD_Plant/VArSdDVAR1.Mod");
    impl->varsd_vartgt = model_do(model, "LD_Plant/VArSdDVAR1.VArTgtSptPct");
    impl->varsd_mod_stval = model_da(model, "LD_Plant/VArSdDVAR1.Mod.stVal");
    impl->vartgt_mxval = model_da(model, "LD_Plant/VArSdDVAR1.VArTgtSptPct.mxVal.f");
    if (impl->vartgt_mxval == nullptr) {
        impl->vartgt_mxval =
            model_da(model, "LD_Plant/VArSdDVAR1.VArTgtSptPct.setMag.f");
    }

    impl->pfsp_mod = model_do(model, "LD_Plant/PFSPDFPF1.Mod");
    impl->pfsp_gn_tgt = model_do(model, "LD_Plant/PFSPDFPF1.PFGnTgtSpt");
    impl->pfsp_lod_tgt = model_do(model, "LD_Plant/PFSPDFPF1.PFLodTgtSpt");
    impl->pfsp_mod_stval = model_da(model, "LD_Plant/PFSPDFPF1.Mod.stVal");
    impl->pfsp_gn_mxval = model_da(model, "LD_Plant/PFSPDFPF1.PFGnTgtSpt.mxVal.f");
    if (impl->pfsp_gn_mxval == nullptr) {
        impl->pfsp_gn_mxval =
            model_da(model, "LD_Plant/PFSPDFPF1.PFGnTgtSpt.setMag.f");
    }
    impl->pfsp_lod_mxval = model_da(model, "LD_Plant/PFSPDFPF1.PFLodTgtSpt.mxVal.f");
    if (impl->pfsp_lod_mxval == nullptr) {
        impl->pfsp_lod_mxval =
            model_da(model, "LD_Plant/PFSPDFPF1.PFLodTgtSpt.setMag.f");
    }

    impl->totw_mag = model_da(model, "LD_Plant/PdCMMXU1.TotW.mag.f");
    impl->totw_t = model_da(model, "LD_Plant/PdCMMXU1.TotW.t");
    impl->totw_q = model_da(model, "LD_Plant/PdCMMXU1.TotW.q");
    impl->totvar_mag = model_da(model, "LD_Plant/PdCMMXU1.TotVAr.mag.f");
    impl->totvar_t = model_da(model, "LD_Plant/PdCMMXU1.TotVAr.t");
    impl->totvar_q = model_da(model, "LD_Plant/PdCMMXU1.TotVAr.q");
    impl->ppv_phsab_mag = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsAB.cVal.mag.f");
    impl->ppv_phsab_t = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsAB.t");
    impl->ppv_phsab_q = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsAB.q");

    if (impl->wlim_mod == nullptr || impl->wlim_wmax == nullptr || impl->wsd_mod == nullptr ||
        impl->wsd_wspt == nullptr || impl->totw_mag == nullptr || impl->totvar_mag == nullptr) {
        std::fprintf(stderr, "mms: cfg model missing required Wlim/WSd/PdC nodes\n");
        return false;
    }
    if (impl->varsd_mod == nullptr || impl->varsd_vartgt == nullptr) {
        std::fprintf(stderr, "mms: cfg model missing VArSdDVAR1 (P5-R01)\n");
        return false;
    }
    return true;
}

bool MmsAdapter::Impl::create_mvp_model(MmsAdapter::Impl* impl) {
    impl->model = IedModel_create("CCI016_01");
    LogicalDevice* ld = LogicalDevice_create("LD_Plant", impl->model);
    LogicalNode* lln0 = LogicalNode_create("LLN0", ld);
    (void)CDC_ENS_create("Mod", reinterpret_cast<ModelNode*>(lln0), 0);
    (void)CDC_ENS_create("Health", reinterpret_cast<ModelNode*>(lln0), 0);

    LogicalNode* wlim = LogicalNode_create("WlimDWMX1", ld);
    impl->wlim_mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(wlim), 0,
                                    CDC_CTL_MODEL_DIRECT_ENHANCED);
    impl->wlim_wmax =
        CDC_APC_create("WMaxSptPct", reinterpret_cast<ModelNode*>(wlim), 0,
                       CDC_CTL_MODEL_DIRECT_ENHANCED, false);

    impl->wlim_mod_stval = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->wlim_mod), "stVal"));
    impl->wmax_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
        reinterpret_cast<ModelNode*>(impl->wlim_wmax), "mxVal.f"));
    if (impl->wmax_mxval == nullptr) {
        impl->wmax_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
            reinterpret_cast<ModelNode*>(impl->wlim_wmax), "setMag.f"));
    }

    LogicalNode* wsd = LogicalNode_create("WSdDAGC1", ld);
    impl->wsd_mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(wsd), 0,
                                   CDC_CTL_MODEL_DIRECT_ENHANCED);
    impl->wsd_wspt =
        CDC_APC_create("WSptPct", reinterpret_cast<ModelNode*>(wsd), 0,
                       CDC_CTL_MODEL_DIRECT_ENHANCED, false);
    impl->wsd_mod_stval = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->wsd_mod), "stVal"));
    impl->wspt_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
        reinterpret_cast<ModelNode*>(impl->wsd_wspt), "mxVal.f"));
    if (impl->wspt_mxval == nullptr) {
        impl->wspt_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
            reinterpret_cast<ModelNode*>(impl->wsd_wspt), "setMag.f"));
    }

    auto stub_mod_inactive = [](LogicalDevice* device, const char* ln_name) {
        LogicalNode* ln = LogicalNode_create(ln_name, device);
        DataObject* mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(ln), 0,
                                         CONTROL_MODEL_STATUS_ONLY);
        return reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(mod), "stVal"));
    };
    LogicalNode* varsd = LogicalNode_create("VArSdDVAR1", ld);
    impl->varsd_mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(varsd), 0,
                                   CDC_CTL_MODEL_DIRECT_ENHANCED);
    impl->varsd_vartgt =
        CDC_APC_create("VArTgtSptPct", reinterpret_cast<ModelNode*>(varsd), 0,
                       CDC_CTL_MODEL_DIRECT_ENHANCED, false);
    impl->varsd_mod_stval = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->varsd_mod), "stVal"));
    impl->vartgt_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
        reinterpret_cast<ModelNode*>(impl->varsd_vartgt), "mxVal.f"));
    if (impl->vartgt_mxval == nullptr) {
        impl->vartgt_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
            reinterpret_cast<ModelNode*>(impl->varsd_vartgt), "setMag.f"));
    }

    DataAttribute* stub_wsa_mod = stub_mod_inactive(ld, "WSaDAGC1");
    DataAttribute* stub_varv = stub_mod_inactive(ld, "VArVDVVR1");
    DataAttribute* stub_pfw = stub_mod_inactive(ld, "PFWDPFW1");
    (void)stub_wsa_mod;
    (void)stub_varv;
    (void)stub_pfw;

    LogicalNode* pfsp = LogicalNode_create("PFSPDFPF1", ld);
    impl->pfsp_mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(pfsp), 0,
                                    CDC_CTL_MODEL_DIRECT_ENHANCED);
    impl->pfsp_gn_tgt =
        CDC_APC_create("PFGnTgtSpt", reinterpret_cast<ModelNode*>(pfsp), 0,
                       CDC_CTL_MODEL_DIRECT_ENHANCED, false);
    impl->pfsp_lod_tgt =
        CDC_APC_create("PFLodTgtSpt", reinterpret_cast<ModelNode*>(pfsp), 0,
                       CDC_CTL_MODEL_DIRECT_ENHANCED, false);
    impl->pfsp_mod_stval = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->pfsp_mod), "stVal"));
    impl->pfsp_gn_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
        reinterpret_cast<ModelNode*>(impl->pfsp_gn_tgt), "mxVal.f"));
    if (impl->pfsp_gn_mxval == nullptr) {
        impl->pfsp_gn_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
            reinterpret_cast<ModelNode*>(impl->pfsp_gn_tgt), "setMag.f"));
    }
    impl->pfsp_lod_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
        reinterpret_cast<ModelNode*>(impl->pfsp_lod_tgt), "mxVal.f"));
    if (impl->pfsp_lod_mxval == nullptr) {
        impl->pfsp_lod_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(
            reinterpret_cast<ModelNode*>(impl->pfsp_lod_tgt), "setMag.f"));
    }

    LogicalNode* pdc = LogicalNode_create("PdCMMXU1", ld);
    DataObject* totw =
        CDC_MV_create("TotW", reinterpret_cast<ModelNode*>(pdc), 0, false);
    impl->totw_mag = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(totw), "mag.f"));
    if (impl->totw_mag == nullptr) {
        impl->totw_mag = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(totw), "instMag.f"));
    }
    impl->totw_t = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(totw), "t"));
    impl->totw_q = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(totw), "q"));

    DataObject* totvar =
        CDC_MV_create("TotVAr", reinterpret_cast<ModelNode*>(pdc), 0, false);
    impl->totvar_mag = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(totvar), "mag.f"));
    if (impl->totvar_mag == nullptr) {
        impl->totvar_mag = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(totvar), "instMag.f"));
    }
    impl->totvar_t = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(totvar), "t"));
    impl->totvar_q = reinterpret_cast<DataAttribute*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(totvar), "q"));

    DataObject* ppv = CDC_DEL_create("PPV", reinterpret_cast<ModelNode*>(pdc), 0);
    DataObject* phsab = reinterpret_cast<DataObject*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(ppv), "phsAB"));
    if (phsab != nullptr) {
        DataAttribute* cval = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(phsab), "cVal"));
        if (cval != nullptr) {
            DataAttribute* mag = reinterpret_cast<DataAttribute*>(
                ModelNode_getChild(reinterpret_cast<ModelNode*>(cval), "mag"));
            if (mag != nullptr) {
                impl->ppv_phsab_mag = reinterpret_cast<DataAttribute*>(
                    ModelNode_getChild(reinterpret_cast<ModelNode*>(mag), "f"));
            }
        }
        impl->ppv_phsab_t = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(phsab), "t"));
        impl->ppv_phsab_q = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(phsab), "q"));
    }

    DataSet* ds = DataSet_create("DS_R_PdC_Mis4sec", lln0);
    DataSetEntry_create(ds, "PdCMMXU1$MX$TotW", -1, nullptr);
    DataSetEntry_create(ds, "PdCMMXU1$MX$TotVAr", -1, nullptr);
    DataSetEntry_create(ds, "PdCMMXU1$MX$PPV$phsAB$cVal$mag$f", -1, nullptr);

    const uint8_t trg =
        TRG_OPT_DATA_CHANGED | TRG_OPT_QUALITY_CHANGED | TRG_OPT_INTEGRITY;
    const uint8_t rpt_opts =
        RPT_OPT_SEQ_NUM | RPT_OPT_TIME_STAMP | RPT_OPT_REASON_FOR_INCLUSION |
        RPT_OPT_DATA_SET | RPT_OPT_DATA_REFERENCE;
    ReportControlBlock_create(
        "urcb_PdC_Mis4sec", lln0, "CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec",
        false, "DS_R_PdC_Mis4sec", 1, trg, rpt_opts, 0, 4000);
    return true;
}

bool MmsAdapter::Impl::create_mms_model(const core::MmsConfig& cfg, MmsAdapter::Impl* impl) {
    if (!cfg.model_cfg_path.empty()) {
        impl->model =
            ConfigFileParser_createModelFromConfigFileEx(cfg.model_cfg_path.c_str());
        if (impl->model == nullptr) {
            std::fprintf(stderr, "mms: failed to parse model cfg %s\n",
                         cfg.model_cfg_path.c_str());
            return false;
        }
        impl->full_cid_model = true;
        if (!wire_cfg_runtime_nodes(impl)) {
            IedModel_destroy(impl->model);
            impl->model = nullptr;
            return false;
        }
        std::fprintf(stderr, "mms: full CID model loaded from %s\n",
                     cfg.model_cfg_path.c_str());
        return true;
    }
    impl->full_cid_model = false;
    return create_mvp_model(impl);
}

MmsAdapter::~MmsAdapter() { stop(); }

bool MmsAdapter::start(const core::MmsConfig& cfg, MmsAuditFn audit) {
    const char* bind_address = cfg.bind_address.c_str();
    const int tcp_port = cfg.tcp_port;

    if (impl_ != nullptr && impl_->running) {
        return true;
    }
    stop();

    auto* impl = new Impl();
    impl->audit = std::move(audit);
    impl->fallback_s = cfg.comms_loss_fallback_s;
    {
        const int spacing_s = cfg.setpoint_min_interval_s < 0 ? 0 : cfg.setpoint_min_interval_s;
        impl->spacing_gate =
            core::dso::SetpointSpacingGate(static_cast<uint32_t>(spacing_s) * 1000U);
        std::fprintf(stderr, "mms: O.7.3.3 set-point spacing gate %s (%d s)\n",
                     spacing_s > 0 ? "ON" : "OFF", spacing_s);
    }
    impl->gnss_poll_s = cfg.gnss_poll_s;
    impl->gnss_discipline = cfg.gnss_discipline_clock;
    impl->chrony_poll = cfg.chrony_poll;

    if (!Impl::create_mms_model(cfg, impl)) {
        delete impl;
        return false;
    }

#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (cfg.tls.enabled) {
        if (cfg.tls.own_key.empty() || cfg.tls.own_cert.empty() ||
            cfg.tls.ca_cert.empty()) {
            std::fprintf(stderr,
                         "mms: TLS enabled but tls_own_key/cert/ca_cert missing\n");
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        impl->tls = TLSConfiguration_create();
        TLSConfiguration_setMinTlsVersion(impl->tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setChainValidation(impl->tls, cfg.tls.chain_validation);
        const bool have_allow =
            !cfg.tls.client_cert.empty() || !cfg.tls.acse_client_cert.empty() ||
            !cfg.tls.viewer_cert.empty() || !cfg.tls.acse_viewer_cert.empty() ||
            !cfg.tls.revoked_cert.empty();
        TLSConfiguration_setAllowOnlyKnownCertificates(impl->tls, have_allow);

        if (!TLSConfiguration_setOwnKeyFromFile(impl->tls, cfg.tls.own_key.c_str(),
                                                nullptr)) {
            std::fprintf(stderr, "mms: TLS failed to load own key %s\n",
                         cfg.tls.own_key.c_str());
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        if (!TLSConfiguration_setOwnCertificateFromFile(impl->tls,
                                                        cfg.tls.own_cert.c_str())) {
            std::fprintf(stderr, "mms: TLS failed to load own cert %s\n",
                         cfg.tls.own_cert.c_str());
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        if (!TLSConfiguration_addCACertificateFromFile(impl->tls,
                                                       cfg.tls.ca_cert.c_str())) {
            std::fprintf(stderr, "mms: TLS failed to load CA %s\n",
                         cfg.tls.ca_cert.c_str());
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        if (!cfg.tls.crl_path.empty()) {
            if (!TLSConfiguration_addCRLFromFile(impl->tls, cfg.tls.crl_path.c_str())) {
                std::fprintf(stderr, "mms: TLS failed to load CRL %s\n",
                             cfg.tls.crl_path.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
            std::fprintf(stderr, "mms: TLS CRL loaded %s\n", cfg.tls.crl_path.c_str());
        }
        if (!cfg.tls.client_cert.empty()) {
            if (!TLSConfiguration_addAllowedCertificateFromFile(
                    impl->tls, cfg.tls.client_cert.c_str())) {
                std::fprintf(stderr, "mms: TLS failed to load DSO allowlist %s\n",
                             cfg.tls.client_cert.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
            const std::string& dso_acse =
                cfg.tls.acse_client_cert.empty() ? cfg.tls.client_cert
                                                 : cfg.tls.acse_client_cert;
            if (!load_pem_der(dso_acse, impl->dso_der)) {
                std::fprintf(stderr, "mms: failed to parse DSO ACSE cert DER %s\n",
                             dso_acse.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
            if (!cfg.tls.acse_client_cert.empty() &&
                cfg.tls.acse_client_cert != cfg.tls.client_cert) {
                if (!TLSConfiguration_addAllowedCertificateFromFile(
                        impl->tls, cfg.tls.acse_client_cert.c_str())) {
                    std::fprintf(stderr,
                                 "mms: TLS failed to load DSO ACSE allowlist %s\n",
                                 cfg.tls.acse_client_cert.c_str());
                    TLSConfiguration_destroy(impl->tls);
                    IedModel_destroy(impl->model);
                    delete impl;
                    return false;
                }
            }
        }
        if (!cfg.tls.viewer_cert.empty()) {
            if (!TLSConfiguration_addAllowedCertificateFromFile(
                    impl->tls, cfg.tls.viewer_cert.c_str())) {
                std::fprintf(stderr, "mms: TLS failed to load VIEWER allowlist %s\n",
                             cfg.tls.viewer_cert.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
            const std::string& viewer_acse =
                cfg.tls.acse_viewer_cert.empty() ? cfg.tls.viewer_cert
                                                   : cfg.tls.acse_viewer_cert;
            if (!load_pem_der(viewer_acse, impl->viewer_der)) {
                std::fprintf(stderr, "mms: failed to parse VIEWER ACSE cert DER %s\n",
                             viewer_acse.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
        }
        /* Allowlisted so CRL (not unknown-cert) is the deny path for P3-09. */
        if (!cfg.tls.revoked_cert.empty()) {
            if (!TLSConfiguration_addAllowedCertificateFromFile(
                    impl->tls, cfg.tls.revoked_cert.c_str())) {
                std::fprintf(stderr, "mms: TLS failed to load revoked allowlist %s\n",
                             cfg.tls.revoked_cert.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
        }
        impl->rbac_enabled = !impl->dso_der.empty();
        IedServerConfig server_cfg = IedServerConfig_create();
        IedServerConfig_enableResvTmsForBRCB(server_cfg, false);
#if defined(CCLI_HAVE_GOOSE)
        IedServerConfig_useIntegratedGoosePublisher(server_cfg, true);
#endif
        impl->server = IedServer_createWithConfig(impl->model, impl->tls, server_cfg);
        IedServerConfig_destroy(server_cfg);
    } else {
        IedServerConfig server_cfg = IedServerConfig_create();
        IedServerConfig_enableResvTmsForBRCB(server_cfg, false);
#if defined(CCLI_HAVE_GOOSE)
        IedServerConfig_useIntegratedGoosePublisher(server_cfg, true);
#endif
        impl->server = IedServer_createWithConfig(impl->model, nullptr, server_cfg);
        IedServerConfig_destroy(server_cfg);
    }
#else
    if (cfg.tls.enabled) {
        std::fprintf(stderr,
                     "mms: TLS requested but libiec61850 built without mbedtls "
                     "(CONFIG_MMS_SUPPORT_TLS)\n");
        IedModel_destroy(impl->model);
        delete impl;
        return false;
    }
    IedServerConfig server_cfg = IedServerConfig_create();
    IedServerConfig_enableResvTmsForBRCB(server_cfg, false);
#if defined(CCLI_HAVE_GOOSE)
    IedServerConfig_useIntegratedGoosePublisher(server_cfg, true);
#endif
    impl->server = IedServer_createWithConfig(impl->model, nullptr, server_cfg);
    IedServerConfig_destroy(server_cfg);
#endif

    if (impl->server == nullptr) {
        std::fprintf(stderr, "mms: IedServer_create failed\n");
#if defined(CONFIG_MMS_SUPPORT_TLS)
        if (impl->tls != nullptr) {
            TLSConfiguration_destroy(impl->tls);
        }
#endif
        IedModel_destroy(impl->model);
        delete impl;
        return false;
    }

#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (cfg.tls.enabled) {
        const std::string& acse_cert =
            cfg.tls.acse_cert.empty() ? cfg.tls.own_cert : cfg.tls.acse_cert;
        const std::string& acse_key =
            cfg.tls.acse_key.empty() ? cfg.tls.own_key : cfg.tls.acse_key;
        if (!mms_aare_auth_configure(acse_cert, acse_key)) {
            std::fprintf(stderr,
                         "mms: AARE 62351-4 responder auth disabled (server cert/key load failed)\n");
        } else {
            mms_aare_auth_register_acse_bridge();
        }
        IedServer_setAuthenticator(impl->server, Impl::auth_tramp, impl);
    }
#endif
    IedServer_setConnectionIndicationHandler(impl->server, Impl::conn_tramp, impl);

    /* ctlModel: CID + .cfg use direct-with-enhanced-security (TSP Compare + Direct Operate). */

    IedServer_setPerformCheckHandler(impl->server, impl->wlim_mod, Impl::check_tramp,
                                     impl);
    IedServer_setControlHandler(impl->server, impl->wlim_mod, Impl::control_tramp,
                                impl);
    IedServer_setPerformCheckHandler(impl->server, impl->wlim_wmax, Impl::check_tramp,
                                     impl);
    IedServer_setControlHandler(impl->server, impl->wlim_wmax, Impl::control_tramp,
                                impl);
    IedServer_setPerformCheckHandler(impl->server, impl->wsd_mod, Impl::check_tramp,
                                     impl);
    IedServer_setControlHandler(impl->server, impl->wsd_mod, Impl::control_tramp,
                                impl);
    IedServer_setPerformCheckHandler(impl->server, impl->wsd_wspt, Impl::check_tramp,
                                     impl);
    IedServer_setControlHandler(impl->server, impl->wsd_wspt, Impl::control_tramp,
                                impl);
    if (impl->varsd_mod != nullptr) {
        IedServer_setPerformCheckHandler(impl->server, impl->varsd_mod, Impl::check_tramp,
                                         impl);
        IedServer_setControlHandler(impl->server, impl->varsd_mod, Impl::control_tramp,
                                    impl);
    }
    if (impl->varsd_vartgt != nullptr) {
        IedServer_setPerformCheckHandler(impl->server, impl->varsd_vartgt,
                                         Impl::check_tramp, impl);
        IedServer_setControlHandler(impl->server, impl->varsd_vartgt, Impl::control_tramp,
                                    impl);
    }
    if (impl->pfsp_mod != nullptr) {
        IedServer_setPerformCheckHandler(impl->server, impl->pfsp_mod, Impl::check_tramp,
                                         impl);
        IedServer_setControlHandler(impl->server, impl->pfsp_mod, Impl::control_tramp,
                                    impl);
    }
    if (impl->pfsp_gn_tgt != nullptr) {
        IedServer_setPerformCheckHandler(impl->server, impl->pfsp_gn_tgt,
                                         Impl::check_tramp, impl);
        IedServer_setControlHandler(impl->server, impl->pfsp_gn_tgt, Impl::control_tramp,
                                    impl);
    }
    if (impl->pfsp_lod_tgt != nullptr) {
        IedServer_setPerformCheckHandler(impl->server, impl->pfsp_lod_tgt,
                                         Impl::check_tramp, impl);
        IedServer_setControlHandler(impl->server, impl->pfsp_lod_tgt, Impl::control_tramp,
                                    impl);
    }

    if (bind_address != nullptr && bind_address[0] != '\0' &&
        std::strcmp(bind_address, "0.0.0.0") != 0) {
        IedServer_setLocalIpAddress(impl->server, bind_address);
    }

    IedServer_start(impl->server, tcp_port);
    if (!IedServer_isRunning(impl->server)) {
        std::fprintf(stderr,
                     "mms: start failed bind=%s port=%d tls=%s (port busy or IP "
                     "missing)\n",
                     bind_address != nullptr ? bind_address : "(any)", tcp_port,
                     cfg.tls.enabled ? "on" : "off");
        IedServer_destroy(impl->server);
#if defined(CONFIG_MMS_SUPPORT_TLS)
        if (impl->tls != nullptr) {
            TLSConfiguration_destroy(impl->tls);
        }
#endif
        IedModel_destroy(impl->model);
        delete impl;
        return false;
    }

    if (!impl->full_cid_model) {
        if (impl->wlim_mod_stval != nullptr) {
            IedServer_updateInt32AttributeValue(impl->server, impl->wlim_mod_stval, 5);
        }
        if (impl->wsd_mod_stval != nullptr) {
            IedServer_updateInt32AttributeValue(impl->server, impl->wsd_mod_stval, 5);
        }
        if (impl->varsd_mod_stval != nullptr) {
            IedServer_updateInt32AttributeValue(impl->server, impl->varsd_mod_stval, 5);
        }
        if (impl->pfsp_mod_stval != nullptr) {
            IedServer_updateInt32AttributeValue(impl->server, impl->pfsp_mod_stval, 5);
        }
        static const char* k_stub_lns[] = {"WSaDAGC1", "VArVDVVR1", "PFWDPFW1"};
        for (const char* ln : k_stub_lns) {
            char path[96];
            std::snprintf(path, sizeof(path), "LD_Plant/%s.Mod.stVal", ln);
            DataAttribute* st = model_da(impl->model, path);
            if (st != nullptr) {
                IedServer_updateInt32AttributeValue(impl->server, st, 5);
            }
        }
    }

    /* Start assuming not synchronized until first GNSS poll. */
    IedServer_setTimeQuality(impl->server, true, false, true, 10);
    if (impl->totw_q != nullptr) {
        Quality q = QUALITY_VALIDITY_QUESTIONABLE | QUALITY_DETAIL_INACCURATE;
        IedServer_updateQuality(impl->server, impl->totw_q, q);
    }
    if (impl->totvar_q != nullptr) {
        Quality q = QUALITY_VALIDITY_QUESTIONABLE | QUALITY_DETAIL_INACCURATE;
        IedServer_updateQuality(impl->server, impl->totvar_q, q);
    }

    impl->running = true;
    impl_ = impl;
    std::fprintf(stderr,
                 "mms: listening on %s:%d tls=%s acse_tls_auth=%s rbac=%s "
                 "crl=%s fallback_s=%d gnss_poll_s=%d chrony=%s discipline=%s "
                 "model=%s\n",
                 (bind_address != nullptr && bind_address[0] != '\0')
                     ? bind_address
                     : "0.0.0.0",
                 tcp_port, cfg.tls.enabled ? "on" : "off",
                 cfg.tls.enabled ? "on" : "off", impl->rbac_enabled ? "on" : "off",
                 cfg.tls.crl_path.empty() ? "off" : "on", impl->fallback_s,
                 impl->gnss_poll_s, impl->chrony_poll ? "on" : "off",
                 impl->gnss_discipline ? "on" : "off",
                 impl->full_cid_model ? "full_cid_cfg" : "mvp_handbuilt");
    return true;
}

void MmsAdapter::stop() {
    if (impl_ == nullptr) {
        return;
    }
    if (impl_->server != nullptr) {
        if (impl_->running) {
            IedServer_stop(impl_->server);
        }
        IedServer_destroy(impl_->server);
        impl_->server = nullptr;
    }
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (impl_->tls != nullptr) {
        TLSConfiguration_destroy(impl_->tls);
        impl_->tls = nullptr;
    }
#endif
    if (impl_->model != nullptr) {
        IedModel_destroy(impl_->model);
        impl_->model = nullptr;
    }
    delete impl_;
    impl_ = nullptr;
}

bool MmsAdapter::is_running() const {
    return impl_ != nullptr && impl_->running && impl_->server != nullptr &&
           IedServer_isRunning(impl_->server);
}

#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
static void go_cb_event_handler(MmsGooseControlBlock go_cb, int event, void* parameter) {
    (void)parameter;
    const char* name =
        go_cb != nullptr ? MmsGooseControlBlock_getName(go_cb) : "(null)";
    const int ena = go_cb != nullptr ? MmsGooseControlBlock_getGoEna(go_cb) : -1;
    std::fprintf(stderr, "mms: GoCB %s event=%d GoEna=%d\n", name, event, ena);
}
#endif

void MmsAdapter::enable_goose_publishing(const core::GooseConfig& cfg) {
#if defined(CCLI_HAVE_LIBIEC61850) && defined(CCLI_HAVE_GOOSE)
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        std::fprintf(stderr, "mms: GOOSE publish skipped — MMS server not running\n");
        return;
    }
    if (!cfg.publish_enabled) {
        return;
    }
    if (!impl_->full_cid_model) {
        std::fprintf(stderr,
                     "mms: GOOSE publish requires full CID model (model_cfg in yaml)\n");
        return;
    }
    const char* iface = cfg.interface.empty() ? "br-lan" : cfg.interface.c_str();
    IedServer_setGooseInterfaceId(impl_->server, iface);
    IedServer_setGoCBHandler(impl_->server, go_cb_event_handler, impl_);
    IedServer_enableGoosePublishing(impl_->server);
    std::fprintf(stderr,
                 "mms: GOOSE publishing enabled on %s (GoEna=true on all GoCBs; "
                 "gcb_PdC_Mis4sec APPID=0x1000, gcb_Stato_Allarmi APPID=0x1001)\n",
                 iface);
#else
    (void)cfg;
    std::fprintf(stderr, "mms: GOOSE publish requested but built without CCLI_HAVE_GOOSE\n");
#endif
}

void MmsAdapter::update_tot_w_kw(double p_kw) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        return;
    }
    IedServer_lockDataModel(impl_->server);
    if (impl_->totw_t != nullptr) {
        IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->totw_t,
                                              Hal_getTimeInMs());
    }
    if (impl_->totw_mag != nullptr) {
        IedServer_updateFloatAttributeValue(impl_->server, impl_->totw_mag,
                                            static_cast<float>(p_kw));
    }
    IedServer_unlockDataModel(impl_->server);
}

void MmsAdapter::update_totvar_kvar(double q_kvar) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        return;
    }
    IedServer_lockDataModel(impl_->server);
    if (impl_->totvar_t != nullptr) {
        IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->totvar_t,
                                              Hal_getTimeInMs());
    }
    if (impl_->totvar_mag != nullptr) {
        IedServer_updateFloatAttributeValue(impl_->server, impl_->totvar_mag,
                                            static_cast<float>(q_kvar));
    }
    IedServer_unlockDataModel(impl_->server);
}

void MmsAdapter::update_ppv_kv(double ppv_kv) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        return;
    }
    IedServer_lockDataModel(impl_->server);
    if (impl_->ppv_phsab_t != nullptr) {
        IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->ppv_phsab_t,
                                            Hal_getTimeInMs());
    }
    if (impl_->ppv_phsab_mag != nullptr) {
        IedServer_updateFloatAttributeValue(impl_->server, impl_->ppv_phsab_mag,
                                            static_cast<float>(ppv_kv));
    }
    IedServer_unlockDataModel(impl_->server);
}

void MmsAdapter::refresh_time_quality() {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        return;
    }
    if (impl_->gnss_poll_s <= 0) {
        return;
    }
    /* Monotonic throttle so clock STEPs do not re-arm the poll timer. */
    timespec mono{};
    clock_gettime(CLOCK_MONOTONIC, &mono);
    const uint64_t now_mono = static_cast<uint64_t>(mono.tv_sec) * 1000ULL +
                              static_cast<uint64_t>(mono.tv_nsec / 1000000L);
    {
        std::lock_guard<std::mutex> lock(impl_->mu);
        if (impl_->last_gnss_poll_mono_ms != 0 &&
            now_mono < impl_->last_gnss_poll_mono_ms +
                           static_cast<uint64_t>(impl_->gnss_poll_s) * 1000ULL) {
            return;
        }
        impl_->last_gnss_poll_mono_ms = now_mono;
    }

    GnssFixStatus st{};
    const bool ok = gnss_query_fix(st);
    bool fix = false;
    bool state_changed = false;
    std::int64_t offset_ms = 0;
    bool within_100 = true;
    {
        std::lock_guard<std::mutex> lock(impl_->mu);
        const bool prev_fix = impl_->gnss_fix;
        const bool prev_q = impl_->gnss_queried;
        impl_->gnss_queried = ok && st.queried;
        impl_->gnss_fix = ok && st.fix;
        fix = impl_->gnss_fix;
        if (st.has_epoch) {
            impl_->last_offset_ms = st.offset_ms;
            offset_ms = st.offset_ms;
        }
        state_changed = (prev_fix != impl_->gnss_fix) ||
                        (prev_q != impl_->gnss_queried);
    }

    /* Absolute ±100 ms: chrony owns the clock. GNSS fix is lock evidence. */
    ChronyStatus ch{};
    bool chrony_ok = false;
    if (impl_->chrony_poll) {
        chrony_ok = chrony_query_status(ch) && ch.available;
        if (chrony_ok) {
            offset_ms = static_cast<std::int64_t>(ch.offset_s * 1000.0);
            within_100 = ch.synchronized;
            {
                std::lock_guard<std::mutex> lock(impl_->mu);
                impl_->last_offset_ms = offset_ms;
            }
        }
    }

    if (ok && st.fix && st.has_epoch && impl_->gnss_discipline) {
        /* Legacy STEP path — off when chrony is used. */
        const std::int64_t abs_off =
            st.offset_ms >= 0 ? st.offset_ms : -st.offset_ms;
        if (!chrony_ok) {
            within_100 = abs_off <= 100;
        }
        const bool force = abs_off > 2000;
        const bool due =
            impl_->last_gnss_step_mono_ms == 0 ||
            now_mono >= impl_->last_gnss_step_mono_ms + 120000ULL;
        if (abs_off > 100 && (force || due)) {
            std::int64_t applied = 0;
            if (gnss_discipline_clock(st, &applied)) {
                impl_->last_gnss_step_mono_ms = now_mono;
                std::fprintf(stderr,
                             "mms: time_quality GNSS clock STEP offset_was=%lld ms "
                             "(T.3.3.4.5 ±100 ms)\n",
                             static_cast<long long>(applied));
                if (!chrony_ok) {
                    within_100 = true;
                    offset_ms = 0;
                }
                {
                    std::lock_guard<std::mutex> lock(impl_->mu);
                    impl_->last_offset_ms = 0;
                }
            }
        }
    } else if (!chrony_ok && ok && st.has_epoch) {
        const std::int64_t abs_off =
            st.offset_ms >= 0 ? st.offset_ms : -st.offset_ms;
        within_100 = abs_off <= 100;
        offset_ms = st.offset_ms;
    } else if (!chrony_ok) {
        within_100 = false;
    }

    /* Sync requires GNSS fix + chrony (or NMEA) within ±100 ms. */
    const bool not_sync = !fix || !within_100;
    IedServer_setTimeQuality(impl_->server, true, false, not_sync, 10);
    const Quality poc_q =
        (!not_sync) ? static_cast<Quality>(QUALITY_VALIDITY_GOOD)
                    : static_cast<Quality>(QUALITY_VALIDITY_QUESTIONABLE |
                                           QUALITY_DETAIL_INACCURATE);
    if (impl_->totw_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->totw_q, poc_q);
    }
    if (impl_->totvar_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->totvar_q, poc_q);
    }

    if (!state_changed && ok && within_100 && fix && chrony_ok) {
        return;
    }
    if (chrony_ok) {
        std::fprintf(stderr,
                     "mms: time_quality chrony offset_ms=%lld within_pm100=%s "
                     "leap=%s ref=%s gnss_fix=%d clockNotSynchronized=%d "
                     "(T.3.3.4.5)\n",
                     static_cast<long long>(offset_ms), within_100 ? "yes" : "no",
                     ch.leap[0] ? ch.leap : "?", ch.refid[0] ? ch.refid : "?",
                     fix ? 1 : 0, not_sync ? 1 : 0);
    } else if (ok && st.fix && st.has_epoch) {
        std::fprintf(stderr,
                     "mms: time_quality GNSS fix sats=%d utc=%s %s "
                     "offset_ms=%lld within_pm100=%s clockNotSynchronized=%d "
                     "(no chrony; T.3.3.4.5)\n",
                     st.satellites, st.date_utc, st.time_utc,
                     static_cast<long long>(offset_ms), within_100 ? "yes" : "no",
                     not_sync ? 1 : 0);
    } else if (ok && st.fix) {
        std::fprintf(stderr,
                     "mms: time_quality GNSS fix sats=%d (no RMC epoch parse) "
                     "clockNotSynchronized=%d\n",
                     st.satellites, not_sync ? 1 : 0);
    } else if (ok) {
        std::fprintf(stderr,
                     "mms: time_quality GNSS searching/no-fix "
                     "clockNotSynchronized=1 (T.3.3.4.5)\n");
    } else {
        std::fprintf(stderr,
                     "mms: time_quality GNSS ubus unavailable — "
                     "clockNotSynchronized=1\n");
    }
}

bool MmsAdapter::gnss_fix() const {
    if (impl_ == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mu);
    return impl_->gnss_fix;
}

bool MmsAdapter::poll_dso_live_command(DsoLiveCommand& out) {
    if (impl_ == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mu);
    if (!impl_->live.dirty && !impl_->live.reactive_dirty && !impl_->live.pfsp_dirty) {
        return false;
    }
    out = impl_->live;
    impl_->live.dirty = false;
    impl_->live.reactive_dirty = false;
    impl_->live.pfsp_dirty = false;
    return true;
}

bool MmsAdapter::poll_comms_loss_fallback() {
    if (impl_ == nullptr || impl_->fallback_s <= 0) {
        return false;
    }
    DataAttribute* wlim_mod_stval = nullptr;
    DataAttribute* wsd_mod_stval = nullptr;
    DataAttribute* varsd_mod_stval = nullptr;
    DataAttribute* pfsp_mod_stval = nullptr;
    IedServer server = nullptr;
    int fallback_s = 0;
    {
        std::lock_guard<std::mutex> lock(impl_->mu);
        if (impl_->client_count > 0 || impl_->fallback_latched ||
            impl_->last_zero_clients_ms == 0) {
            return false;
        }
        const uint64_t now = Hal_getTimeInMs();
        const uint64_t due =
            impl_->last_zero_clients_ms +
            static_cast<uint64_t>(impl_->fallback_s) * 1000ULL;
        if (now < due) {
            return false;
        }
        /* Clear live Wlim+WSd+VArSd → Operating Rule autonomous (yaml) resumes. */
        impl_->live.wlim_active = false;
        impl_->live.wmax_spt_pct = 0.0;
        impl_->live.wsd_active = false;
        impl_->live.wspt_pct = 0.0;
        impl_->live.varsd_active = false;
        impl_->live.vartgt_spt_pct = 0.0;
        impl_->live.pfsp_active = false;
        impl_->live.pfsp_cosphi = 1.0;
        impl_->live.valid = true;
        impl_->live.dirty = true;
        impl_->live.reactive_dirty = true;
        impl_->live.pfsp_dirty = true;
        impl_->fallback_latched = true;
        impl_->fallback_pending = true;
        fallback_s = impl_->fallback_s;
        wlim_mod_stval = impl_->wlim_mod_stval;
        wsd_mod_stval = impl_->wsd_mod_stval;
        varsd_mod_stval = impl_->varsd_mod_stval;
        pfsp_mod_stval = impl_->pfsp_mod_stval;
        server = impl_->server;
    }
    std::fprintf(stderr,
                 "mms: P3-05 Operating Rule fallback — live Wlim/WSd/VArSd/PFSP cleared after %d s "
                 "no-comms\n",
                 fallback_s);
    if (server != nullptr && wlim_mod_stval != nullptr) {
        IedServer_updateInt32AttributeValue(server, wlim_mod_stval, 5);
    }
    if (server != nullptr && wsd_mod_stval != nullptr) {
        IedServer_updateInt32AttributeValue(server, wsd_mod_stval, 5);
    }
    if (server != nullptr && varsd_mod_stval != nullptr) {
        IedServer_updateInt32AttributeValue(server, varsd_mod_stval, 5);
    }
    if (server != nullptr && pfsp_mod_stval != nullptr) {
        IedServer_updateInt32AttributeValue(server, pfsp_mod_stval, 5);
    }
    return true;
}

int MmsAdapter::client_count() const {
    if (impl_ == nullptr) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(impl_->mu);
    return impl_->client_count;
}

#else /* !CCLI_HAVE_LIBIEC61850 */

struct MmsAdapter::Impl {};

MmsAdapter::~MmsAdapter() = default;

bool MmsAdapter::start(const core::MmsConfig& cfg, MmsAuditFn audit) {
    (void)cfg;
    (void)audit;
    std::fprintf(stderr,
                 "mms: stub (libiec61850 not linked) — rebuild with "
                 "CCLI_HAVE_LIBIEC61850\n");
    return false;
}

void MmsAdapter::stop() {}

bool MmsAdapter::is_running() const { return false; }

void MmsAdapter::update_tot_w_kw(double /*p_kw*/) {}

void MmsAdapter::update_totvar_kvar(double /*q_kvar*/) {}

void MmsAdapter::update_ppv_kv(double /*ppv_kv*/) {}

bool MmsAdapter::poll_dso_live_command(DsoLiveCommand& /*out*/) { return false; }

bool MmsAdapter::poll_comms_loss_fallback() { return false; }

void MmsAdapter::refresh_time_quality() {}

int MmsAdapter::client_count() const { return 0; }

bool MmsAdapter::gnss_fix() const { return false; }

#endif

}  // namespace cci::adapters
