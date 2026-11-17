/*=============================================================================
 * Eth_A MMS server — CEI 0-16 Allegato O + T / TR 57-126 Figura 2.
 *
 * Hierarchy (T.3.3.1 · signal_map.yaml):
 *   IED CCI016_01
 *     └─ LD LD_Plant
 *          ├─ LLN0
 *          ├─ WlimDWMX1.{Mod,WMaxSptPct}     O.9.2.2 lim W     — IMPL
 *          ├─ WSdDAGC1.{Mod,WSptPct}         O.9.2.3 s.p. W    — IMPL (Figura 2 green)
 *          ├─ WSaDAGC1 / VArSaDVAR1          Aggregator signed P/Q interface
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
#include "mms_rbac_security.hpp"
#include "mms_aare_auth.hpp"
#include "e2e/e2e_server.hpp"
#include "gnss_time.hpp"
#include "dso/setpoint_gate.hpp"
#include "measurement/measurement_store.hpp"
#include "event/time_sync_tracker.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <mutex>
#include <map>
#include <string>
#include <unordered_set>
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
#include "../security/iec62351_tls.h"
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
    struct SessionToken {
        MmsRbacIdentity identity;
        std::string peer;
        bool transport_bound{false};
        mutable std::time_t checked_second{0};
        mutable bool certificate_valid{false};
        mutable std::string certificate_reason;
    };
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
    DataObject* wsa_mod{nullptr};
    DataObject* wsa_wspt{nullptr};
    DataObject* varsa_mod{nullptr};
    DataObject* varsa_vartgt{nullptr};
    DataAttribute* wsa_mod_stval{nullptr};
    DataAttribute* wsa_mxval{nullptr};
    DataAttribute* varsa_mod_stval{nullptr};
    DataAttribute* varsa_mxval{nullptr};
    DataObject* pfsp_mod{nullptr};
    DataObject* pfsp_gn_tgt{nullptr};
    DataObject* pfsp_lod_tgt{nullptr};
    DataAttribute* agg_totw_mag{nullptr};
    DataAttribute* agg_totw_t{nullptr};
    DataAttribute* agg_totw_q{nullptr};
    DataAttribute* agg_totvar_mag{nullptr};
    DataAttribute* agg_totvar_t{nullptr};
    DataAttribute* agg_totvar_q{nullptr};
    DataAttribute* wsa_beh{nullptr};
    DataAttribute* wsa_fct{nullptr};
    DataAttribute* varsa_beh{nullptr};
    DataAttribute* varsa_fct{nullptr};
    DataAttribute* totw_mag{nullptr};
    DataAttribute* totw_t{nullptr};
    DataAttribute* totw_q{nullptr};
    DataAttribute* genpv_totw_mag{nullptr};
    DataAttribute* genpv_totw_t{nullptr};
    DataAttribute* genpv_totw_q{nullptr};
    DataAttribute* totvar_mag{nullptr};
    DataAttribute* totvar_t{nullptr};
    DataAttribute* totvar_q{nullptr};
    DataAttribute* ppv_phsab_mag{nullptr};
    DataAttribute* ppv_phsab_ang{nullptr};
    DataAttribute* ppv_phsab_t{nullptr};
    DataAttribute* ppv_phsab_q{nullptr};
    DataAttribute* ppv_phsbc_mag{nullptr};
    DataAttribute* ppv_phsbc_ang{nullptr};
    DataAttribute* ppv_phsbc_t{nullptr};
    DataAttribute* ppv_phsbc_q{nullptr};
    DataAttribute* ppv_phsca_mag{nullptr};
    DataAttribute* ppv_phsca_ang{nullptr};
    DataAttribute* ppv_phsca_t{nullptr};
    DataAttribute* ppv_phsca_q{nullptr};
    DataAttribute* aphsa_mag{nullptr};
    DataAttribute* aphsa_ang{nullptr};
    DataAttribute* aphsa_t{nullptr};
    DataAttribute* aphsa_q{nullptr};
    DataAttribute* aphsb_mag{nullptr};
    DataAttribute* aphsb_ang{nullptr};
    DataAttribute* aphsb_t{nullptr};
    DataAttribute* aphsb_q{nullptr};
    DataAttribute* aphsc_mag{nullptr};
    DataAttribute* aphsc_ang{nullptr};
    DataAttribute* aphsc_t{nullptr};
    DataAttribute* aphsc_q{nullptr};
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
    std::vector<uint8_t> dso_tls_der;
    std::vector<uint8_t> viewer_der;
    bool rbac_enabled{false};
    bool profile_a_enabled{false};
    std::unique_ptr<ccli::mms::e2e::NativeServer> native_e2e;
    bool native_required{false};
    MmsCommissioningPolicy commissioning_policy;
    MmsRbacSecurity rbac_security;
    std::unordered_set<SessionToken*> profile_tokens;
    struct TlsPeerIdentity { std::string certificate_pin; std::string key_pin; };
    std::map<std::string, TlsPeerIdentity> tls_peer_identities;
    std::vector<ReportControlBlock*> report_blocks;

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
    MmsTimeSyncAuditFn time_sync_audit{};
    core::TimeSyncEventTracker time_sync_events{};

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
    static MmsDataAccessError read_access_tramp(LogicalDevice* ld, LogicalNode* ln,
                                                 DataObject* data_object, FunctionalConstraint fc,
                                                 ClientConnection connection, void* parameter);
    static MmsDataAccessError write_access_tramp(DataAttribute* attribute, MmsValue* value,
                                                  ClientConnection connection, void* parameter);
    static bool dataset_access_tramp(void* parameter, ClientConnection connection,
                                     IedServer_DataSetOperation operation, const char* dataset_ref);
    static bool directory_access_tramp(void* parameter, ClientConnection connection,
                                       IedServer_DirectoryCategory category, LogicalDevice* ld);
    static bool list_access_tramp(void* parameter, ClientConnection connection, ACSIClass acsi_class,
                                  LogicalDevice* ld, LogicalNode* ln, const char* object_name,
                                  const char* sub_object_name, FunctionalConstraint fc);
    static bool control_block_access_tramp(void* parameter, ClientConnection connection,
                                           ACSIClass acsi_class, LogicalDevice* ld, LogicalNode* ln,
                                           const char* object_name, const char* sub_object_name,
                                           IedServer_ControlBlockAccessType access_type);
    static bool wire_cfg_runtime_nodes(Impl* impl);
    static bool create_mvp_model(Impl* impl);
    static bool create_mms_model(const core::MmsConfig& cfg, Impl* impl);
    ControlHandlerResult on_control(ControlAction action, MmsValue* value, bool test);

    bool policy_permits(void* token, MmsPermission permission,
                        const std::string& object_reference, bool auditable = true) const;
    bool issue_profile_a_token(MmsAcseMechanism mechanism, const uint8_t* certificate,
                               int certificate_length, void** security_token,
                               const IsoApplicationReference* application_reference);
    void release_profile_token(void* token);
    bool dataset_members_permitted(const SessionToken* token, const DataSet* dataset) const;
    DataSet* lookup_dataset(const std::string& reference) const;
    bool node_visible(ModelNode* node, void* token) const;
    void install_rbac_writes(ModelNode* node);
    bool session_valid(const SessionToken* token, std::string& reason) const;
    void* prepare_session(ClientConnection connection);
    void security_audit(const char* event, const char* severity, const SessionToken* token,
                        const char* service, const std::string& object, const std::string& reason) const;
#if defined(CONFIG_MMS_SUPPORT_TLS)
    static void tls_event_tramp(void* parameter, TLSEventLevel level, int code,
                                const char* message, TLSConnection connection);
#endif
};

static std::string mms_object_reference(LogicalDevice* ld, LogicalNode* ln,
                                        const char* object_name) {
    if (ld == nullptr || ln == nullptr || ld->name == nullptr || ln->name == nullptr) {
        return {};
    }
    std::string reference = std::string(ld->name) + "/" + ln->name;
    if (object_name != nullptr && object_name[0] != '\0') {
        reference += ".";
        reference += object_name;
    }
    return reference;
}

static std::string short_reference(ModelNode* node) {
    if (!node) return {};
    char reference[512]{};
    ModelNode_getObjectReferenceEx(node, reference, true);
    return reference;
}

DataSet* MmsAdapter::Impl::lookup_dataset(const std::string& supplied) const {
    auto reference = supplied;
    const auto slash = reference.find('/');
    if (slash == std::string::npos) return nullptr;
    if (model->name && reference.rfind(model->name, 0) != 0) reference.insert(0, model->name);
    const auto separator = reference.find('.', reference.find('/'));
    if (separator != std::string::npos) reference[separator] = '$';
    return IedModel_lookupDataSet(model, reference.c_str());
}

bool MmsAdapter::Impl::dataset_members_permitted(const SessionToken* token, const DataSet* dataset) const {
    if (!dataset || !dataset->fcdas || dataset->elementCount <= 0) return false;
    unsigned count = 0;
    for (const DataSetEntry* entry = dataset->fcdas; entry; entry = entry->sibling) {
        if (!entry->logicalDeviceName || !entry->variableName) return false;
        std::string domain = entry->logicalDeviceName;
        if (model->name && domain.rfind(model->name, 0) == 0) domain.erase(0, std::strlen(model->name));
        std::string variable = entry->variableName;
        const auto fc = variable.find('$');
        const auto object = fc == std::string::npos ? std::string::npos : variable.find('$', fc + 1);
        if (object == std::string::npos) return false; // Whole LN/FC could expose other objects.
        variable.erase(fc, object - fc);
        std::replace(variable.begin(), variable.end(), '$', '.');
        if (entry->componentName && *entry->componentName) {
            std::string component(entry->componentName);
            std::replace(component.begin(), component.end(), '$', '.');
            variable += "." + component;
        }
        if (!commissioning_policy.permits_roles(token->identity.roles, MmsPermission::ReadValues, domain + "/" + variable)) return false;
        if (++count > 4096) return false;
    }
    return count == static_cast<unsigned>(dataset->elementCount);
}

bool MmsAdapter::Impl::node_visible(ModelNode* node, void* token) const {
    if (!node) return false;
    if (node->modelType == DataObjectModelType) {
        const auto reference = short_reference(node);
        return policy_permits(token, MmsPermission::ListObjects, reference, false) &&
               policy_permits(token, MmsPermission::ReadValues, reference, false);
    }
    if (node->modelType == LogicalNodeModelType) {
        for (auto* block : report_blocks)
            if (reinterpret_cast<ModelNode*>(block->parent) == node &&
                policy_permits(token, MmsPermission::Reporting, short_reference(node) + "." + block->name, false)) return true;
        for (auto* dataset = model->dataSets; dataset; dataset = dataset->sibling) {
            if (!dataset->name || !dataset->logicalDeviceName) continue;
            const std::string prefix = std::string(node->name) + "$";
            if (std::string(dataset->name).rfind(prefix, 0) != 0) continue;
            std::string name = dataset->name;
            std::replace(name.begin(), name.end(), '$', '.');
            if (policy_permits(token, MmsPermission::DataSet, std::string(dataset->logicalDeviceName) + "/" + name, false)) return true;
        }
    }
    for (auto* child = node->firstChild; child; child = child->sibling)
        if (node_visible(child, token)) return true;
    return false;
}

void MmsAdapter::Impl::install_rbac_writes(ModelNode* node) {
    if (!node) return;
    if (node->modelType == DataAttributeModelType)
        IedServer_handleWriteAccess(server, reinterpret_cast<DataAttribute*>(node), Impl::write_access_tramp, this);
    if (node->modelType == DataObjectModelType && DataObject_hasFCData(reinterpret_cast<DataObject*>(node), IEC61850_FC_CO))
        IedServer_setPerformCheckHandler(server, reinterpret_cast<DataObject*>(node), Impl::check_tramp, this);
    for (auto* child = node->firstChild; child; child = child->sibling) install_rbac_writes(child);
}

MmsDataAccessError MmsAdapter::Impl::write_access_tramp(DataAttribute* attribute, MmsValue*,
                                                       ClientConnection connection, void* parameter) {
    auto* impl = static_cast<Impl*>(parameter);
    if (!impl || !impl->profile_a_enabled) return DATA_ACCESS_ERROR_SUCCESS;
    if (!attribute || !connection) return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
    // Process/status values and control structures must never be writable via CONFIG.
    if (attribute->fc != IEC61850_FC_CF && attribute->fc != IEC61850_FC_SP && attribute->fc != IEC61850_FC_DC)
        return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
    return impl->policy_permits(impl->prepare_session(connection), MmsPermission::Config,
                               short_reference(reinterpret_cast<ModelNode*>(attribute)))
        ? DATA_ACCESS_ERROR_SUCCESS : DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
}

CheckHandlerResult MmsAdapter::Impl::check_tramp(ControlAction action, void* parameter,
                                                 MmsValue* ctlVal, bool /*test*/,
                                                 bool /*interlockCheck*/) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr) {
        return CONTROL_OBJECT_ACCESS_DENIED;
    }
    if (!impl->rbac_enabled && !impl->profile_a_enabled) {
        return CONTROL_ACCEPTED;
    }
    ClientConnection cc = ControlAction_getClientConnection(action);
    void* tok = impl->prepare_session(cc);
    DataObject* object = ControlAction_getControlObject(action);
    const std::string object_ref =
        (object != nullptr && object->parent != nullptr && object->parent->parent != nullptr)
            ? std::string(object->parent->parent->name) + "/" +
                  object->parent->name + "." + object->name
            : std::string{};
    if (impl->policy_permits(tok, MmsPermission::Control, object_ref)) {
        if (object == impl->wsa_mod || object == impl->varsa_mod) {
            if (!ctlVal || MmsValue_getType(ctlVal) != MMS_INTEGER ||
                (MmsValue_toInt32(ctlVal) != -1 && MmsValue_toInt32(ctlVal) != -5))
                return CONTROL_VALUE_INVALID;
        } else if (object == impl->wsa_wspt || object == impl->varsa_vartgt) {
            auto* value = ctlVal;
            if (value && MmsValue_getType(value)==MMS_STRUCTURE && MmsValue_getArraySize(value)>0)
                value=MmsValue_getElement(value,0);
            if (!value || MmsValue_getType(value)!=MMS_FLOAT ||
                !std::isfinite(MmsValue_toFloat(value)) || MmsValue_toFloat(value)<-100 || MmsValue_toFloat(value)>100)
                return CONTROL_VALUE_INVALID;
        }
        return CONTROL_ACCEPTED;
    }
    std::fprintf(stderr,
                 "mms: RBAC DENY control %s — no CONTROL or allocated C3 CONFIG permission\n", object_ref.c_str());
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

bool MmsAdapter::Impl::policy_permits(void* token, MmsPermission permission,
                                      const std::string& object_reference, bool auditable) const {
    if (!profile_a_enabled) {
        /* Legacy laboratory fallback: preserve the former DSO control gate. */
        return permission == MmsPermission::Control && token == &g_token_dso;
    }
    std::lock_guard<std::mutex> lock(mu);
    const auto token_it = profile_tokens.find(static_cast<SessionToken*>(token));
    if (token_it == profile_tokens.end()) {
        return false;
    }
    const auto* session = *token_it;
    std::string reason;
    const bool valid = session_valid(session, reason);
    const bool direct = valid && commissioning_policy.permits_roles(
        session->identity.roles, permission, object_reference);
    const bool config_control = valid && !direct && permission == MmsPermission::Control &&
        commissioning_policy.permits_control_roles(session->identity.roles, object_reference);
    const bool permitted = direct || config_control;
    if (auditable) security_audit(permitted ? "MMS_ACCESS_OK" : "MMS_ACCESS_DENIED",
                   permitted ? "notice" : "alarm", session,
                   mms_permission_name(permission), object_reference,
                   permitted ? (config_control
                       ? "authorized via allocated C3 CONFIG ctlVal" : "authorized")
                       : (reason.empty() ? "permission or object scope denied" : reason));
    return permitted;
}

void MmsAdapter::Impl::security_audit(const char* event, const char* severity,
                                      const SessionToken* token, const char* service,
                                      const std::string& object, const std::string& reason) const {
    const auto detail = mms_rbac_audit(event, severity, token ? &token->identity : nullptr,
        commissioning_policy.policy_id(), rbac_security.sequence(), service, object, reason);
    emit_audit(detail.c_str());
}

void* MmsAdapter::Impl::prepare_session(ClientConnection connection) {
    if (!connection) return nullptr;
    void* raw = ClientConnection_getSecurityToken(connection);
    if (!profile_a_enabled) return raw;
    std::lock_guard<std::mutex> lock(mu);
    const auto found = profile_tokens.find(static_cast<SessionToken*>(raw));
    if (found == profile_tokens.end()) return nullptr;
    auto* token = *found;
    // Upstream notifies TCP connection establishment before ACSE authentication.
    // Bind on the first service after authentication, using this actual connection.
    if (token->peer.empty()) {
        const auto address = ClientConnection_getPeerAddress(connection);
        token->peer = address ? address : "";
        token->identity.peer_address = token->peer;
        const auto peer = tls_peer_identities.find(token->peer);
        token->transport_bound = peer != tls_peer_identities.end() &&
            peer->second.certificate_pin == token->identity.transport_fingerprint &&
            (token->identity.has_signature_proof || peer->second.key_pin == token->identity.public_key_fingerprint);
        security_audit(token->transport_bound ? "ASSOCIATION_AUTH_OK" : "ASSOCIATION_BINDING_FAIL",
            token->transport_bound ? "notice" : "alarm", token, "ASSOCIATION", "",
            token->transport_bound ? "identity bound to authenticated transport" : "transport/identity pairing mismatch; all access denied");
    }
    return raw;
}

bool MmsAdapter::Impl::session_valid(const SessionToken* token, std::string& reason) const {
    // Caller holds mu. Transport binding is checked on every operation.
    const auto peer = tls_peer_identities.find(token->peer);
    if (!token->transport_bound || peer == tls_peer_identities.end() ||
        peer->second.certificate_pin != token->identity.transport_fingerprint ||
        (!token->identity.has_signature_proof && peer->second.key_pin != token->identity.public_key_fingerprint)) {
        reason = "authenticated TLS peer does not match commissioned identity binding";
        return false;
    }
    const auto second = std::time(nullptr);
    if (token->checked_second != second) {
        token->certificate_valid = rbac_security.valid_now(token->identity, token->certificate_reason);
        token->checked_second = second;
    }
    reason = token->certificate_reason;
    return token->certificate_valid;
}

#if defined(CONFIG_MMS_SUPPORT_TLS)
void MmsAdapter::Impl::tls_event_tramp(void* parameter, TLSEventLevel level, int code,
                                       const char* message, TLSConnection connection) {
    auto* impl = static_cast<Impl*>(parameter);
    if (!impl) return;
    // Passive observer: no TLS options, credentials, handshake or wire data are changed.
    if (impl->profile_a_enabled && connection && (code == TLS_EVENT_CODE_INF_SESSION_ESTABLISHED ||
                       code == TLS_EVENT_CODE_INF_SESSION_RENEGOTIATION)) {
        char address[80]{};
        int size = 0;
        const auto certificate = TLSConnection_getPeerCertificate(connection, &size);
        const char* peer = TLSConnection_getPeerAddress(connection, address);
        const auto pin = size > 0 ? MmsRbacSecurity::fingerprint(certificate, static_cast<std::size_t>(size)) : std::string{};
        const auto key_pin = size > 0 ? MmsRbacSecurity::public_key_fingerprint(certificate, static_cast<std::size_t>(size)) : std::string{};
        if (peer && !pin.empty()) {
            std::lock_guard<std::mutex> lock(impl->mu);
            // Failed ACSE handshakes have no disconnect token. Reclaim their
            // observer entries without disturbing an active bound association.
            if (impl->tls_peer_identities.size() >= 1024 && !impl->tls_peer_identities.count(peer)) {
                for (auto it = impl->tls_peer_identities.begin(); it != impl->tls_peer_identities.end(); ++it) {
                    const bool active = std::any_of(impl->profile_tokens.begin(), impl->profile_tokens.end(),
                        [&](const SessionToken* token) { return token->transport_bound && token->peer == it->first; });
                    if (!active) { impl->tls_peer_identities.erase(it); break; }
                }
            }
            if (impl->tls_peer_identities.size() < 1024 || impl->tls_peer_identities.count(peer))
                impl->tls_peer_identities[peer] = {pin, key_pin};
        }
    }
    char peer_address[61]{};
    const char* peer = connection ? TLSConnection_getPeerAddress(connection, peer_address) : nullptr;
    const std::string detail = std::string(message ? message : "TLS event") +
        (peer ? std::string(" peer=") + peer : "");
    if (!impl->audit)
        std::fprintf(stderr, "mms: TLS_SECURITY_EVENT severity=%d code=%d %s\n", level, code, detail.c_str());
    impl->security_audit("TLS_SECURITY_EVENT", level == TLS_SEC_EVT_INFO ? "notice" : level == TLS_SEC_EVT_WARNING ? "warning" : "alarm",
                          nullptr, "TLS", std::to_string(code), detail);
}
#endif

bool MmsAdapter::Impl::issue_profile_a_token(MmsAcseMechanism mechanism,
                                             const uint8_t* certificate,
                                             int certificate_length,
                                             void** security_token,
                                             const IsoApplicationReference* application_reference) {
    // Retain both authentication mechanisms already accepted by the working lab
    // flow. The role identity is individually pinned and subsequently bound to
    // the actual mutually authenticated transport certificate.
    if (!mms_acse_mechanism_supported(mechanism) || certificate == nullptr || certificate_length <= 0 ||
        security_token == nullptr) {
        return false;
    }
    MmsRbacIdentity identity;
    std::string reason;
    if (!rbac_security.authenticate(certificate, static_cast<std::size_t>(certificate_length),
                                    commissioning_policy, identity, reason)) {
        security_audit("ASSOCIATION_AUTH_FAIL", "alarm", nullptr,
                        mms_acse_mechanism_label(mechanism), "", reason);
        return false;
    }
    if (mechanism == MmsAcseMechanism::Certificate) {
        if (!identity.has_signature_proof) reason = "explicit MMS certificate authentication requires signature proof";
        else if (!application_reference || application_reference->apTitle.arcCount < 2 ||
                 application_reference->apTitle.arcCount > 10 || application_reference->aeQualifier < 0)
            reason = "missing or invalid calling application reference";
        else {
            std::string oid;
            for (int arc = 0; arc < application_reference->apTitle.arcCount; ++arc) {
                if (!oid.empty()) oid += '.';
                oid += std::to_string(application_reference->apTitle.arc[arc]);
            }
            oid += '.' + std::to_string(application_reference->aeQualifier);
            MmsRbacSecurity::matches_application_reference(identity, oid, reason);
        }
        if (!reason.empty()) {
            security_audit("ASSOCIATION_AUTH_FAIL", "alarm", nullptr, "CERTIFICATE", "", reason);
            return false;
        }
    }
    auto* token = new SessionToken{};
    token->identity = std::move(identity);
    {
        std::lock_guard<std::mutex> lock(mu);
        if (profile_tokens.size() >= 1024) { delete token; return false; }
        profile_tokens.insert(token);
    }
    *security_token = token;
    return true;
}

void MmsAdapter::Impl::release_profile_token(void* token) {
    std::lock_guard<std::mutex> lock(mu);
    const auto token_it = profile_tokens.find(static_cast<SessionToken*>(token));
    if (token_it != profile_tokens.end()) {
        delete *token_it;
        profile_tokens.erase(token_it);
    }
}

MmsDataAccessError MmsAdapter::Impl::read_access_tramp(LogicalDevice* ld, LogicalNode* ln,
                                                        DataObject* data_object,
                                                        FunctionalConstraint /*fc*/,
                                                        ClientConnection connection,
                                                        void* parameter) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr || !impl->profile_a_enabled) return DATA_ACCESS_ERROR_SUCCESS;
    if (!connection) return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
    if (!data_object) {
        // Bulk LN/FC reads are allowed only if every contained object is readable.
        if (!ln) return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
        for (auto* child = ln->firstChild; child; child = child->sibling)
            if (child->modelType == DataObjectModelType &&
                !impl->policy_permits(impl->prepare_session(connection), MmsPermission::ReadValues,
                                      short_reference(child))) return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
        return DATA_ACCESS_ERROR_SUCCESS;
    }
    const char* name = data_object->name;
    return impl->policy_permits(impl->prepare_session(connection), MmsPermission::ReadValues,
                                mms_object_reference(ld, ln, name))
        ? DATA_ACCESS_ERROR_SUCCESS : DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
}

bool MmsAdapter::Impl::dataset_access_tramp(void* parameter, ClientConnection connection,
                                            IedServer_DataSetOperation operation,
                                            const char* dataset_ref) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr || !impl->profile_a_enabled) return true;
    if (connection == nullptr || dataset_ref == nullptr) return false;
    const bool read_only = operation == DATASET_READ || operation == DATASET_GET_DIRECTORY;
    std::string reference(dataset_ref);
    if (impl->model->name != nullptr && reference.rfind(impl->model->name, 0) == 0)
        reference.erase(0, std::strlen(impl->model->name));
    std::replace(reference.begin(), reference.end(), '$', '.');
    void* raw_token = impl->prepare_session(connection);
    std::lock_guard<std::mutex> lock(impl->mu);
    const auto it = impl->profile_tokens.find(
        static_cast<SessionToken*>(raw_token));
    std::string reason;
    const bool permitted = it != impl->profile_tokens.end() && impl->session_valid(*it, reason) && read_only &&
        impl->commissioning_policy.permits_dataset_roles((*it)->identity.roles, true, reference) &&
        impl->dataset_members_permitted(*it, impl->lookup_dataset(reference));
    impl->security_audit(permitted ? "DATASET_ACCESS_OK" : "DATASET_ACCESS_DENIED", permitted ? "notice" : "alarm",
        it != impl->profile_tokens.end() ? *it : nullptr, "DATASET", reference,
        permitted ? "all members authorized" : "dataset action, session or member scope denied");
    return permitted;
}

bool MmsAdapter::Impl::directory_access_tramp(void* parameter, ClientConnection connection,
                                              IedServer_DirectoryCategory category, LogicalDevice* ld) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr || !impl->profile_a_enabled) return true;
    if (!connection || category == DIRECTORY_CAT_LOG_LIST) return false;
    void* token = impl->prepare_session(connection);
    if (ld) return impl->node_visible(reinterpret_cast<ModelNode*>(ld), token);
    bool visible = false;
    for (auto* device = impl->model->firstChild; device; device = reinterpret_cast<LogicalDevice*>(device->sibling)) {
        if (!impl->node_visible(reinterpret_cast<ModelNode*>(device), token)) return false;
        visible = true;
    }
    return visible;
}

bool MmsAdapter::Impl::list_access_tramp(void* parameter, ClientConnection connection,
                                         ACSIClass acsi_class, LogicalDevice* ld, LogicalNode* ln,
                                         const char* object_name, const char* /*sub_object_name*/,
                                         FunctionalConstraint /*fc*/) {
    auto* impl = static_cast<Impl*>(parameter);
    if (!impl || !impl->profile_a_enabled) return true;
    if (!connection) return false;
    void* token = impl->prepare_session(connection);
    if (!object_name || !*object_name)
        return impl->node_visible(ln ? reinterpret_cast<ModelNode*>(ln) : reinterpret_cast<ModelNode*>(ld), token);
    const auto reference = mms_object_reference(ld, ln, object_name);
    if (acsi_class == ACSI_CLASS_DATA_SET) return dataset_access_tramp(impl, connection, DATASET_GET_DIRECTORY, reference.c_str());
    if (acsi_class == ACSI_CLASS_BRCB || acsi_class == ACSI_CLASS_URCB)
        return control_block_access_tramp(impl, connection, acsi_class, ld, ln, object_name, "", IEC61850_CB_ACCESS_TYPE_READ);
    return impl->policy_permits(token, MmsPermission::ListObjects, reference, false) &&
           impl->policy_permits(token, MmsPermission::ReadValues, reference, false);
}

bool MmsAdapter::Impl::control_block_access_tramp(void* parameter, ClientConnection connection,
                                                  ACSIClass acsi_class, LogicalDevice* ld, LogicalNode* ln,
                                                  const char* object_name, const char* sub_object_name,
                                                  IedServer_ControlBlockAccessType access_type) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr || !impl->profile_a_enabled) return true;
    if (connection == nullptr) return false;
    const bool write = access_type == IEC61850_CB_ACCESS_TYPE_WRITE;
    const auto reference = mms_object_reference(ld, ln, object_name);
    if (acsi_class != ACSI_CLASS_BRCB && acsi_class != ACSI_CLASS_URCB) {
        // REPORTING never authorizes setting groups, logs, GOOSE or sampled values.
        return !write && impl->policy_permits(impl->prepare_session(connection),
                                              MmsPermission::ReadValues, reference);
    }
    void* raw_token = impl->prepare_session(connection);
    std::lock_guard<std::mutex> lock(impl->mu);
    const auto it = impl->profile_tokens.find(static_cast<SessionToken*>(raw_token));
    std::string reason;
    bool permitted = it != impl->profile_tokens.end() && impl->session_valid(*it, reason) &&
        impl->commissioning_policy.permits_report_roles((*it)->identity.roles, write,
            sub_object_name ? sub_object_name : "", reference);
    // Dataset membership is immutable for this commissioned MMS profile.
    if (write && (!sub_object_name || !*sub_object_name || std::strcmp(sub_object_name, "DatSet") == 0)) permitted = false;
    ReportControlBlock* rcb = nullptr;
    for (auto* candidate : impl->report_blocks)
        if (candidate->parent == ln && object_name && std::strcmp(candidate->name, object_name) == 0) { rcb = candidate; break; }
    if (permitted) {
        const std::string data_ref = rcb && rcb->dataSetName
            ? std::string(ld->name) + "/" + ln->name + "$" + rcb->dataSetName : std::string{};
        permitted = impl->dataset_members_permitted(*it, impl->lookup_dataset(data_ref));
    }
    // The generated reporting overlay invokes this callback before report delivery,
    // including buffered entries, so expiry/revocation cannot leave a live report.
    impl->security_audit(permitted ? "REPORT_ACCESS_OK" : "REPORT_ACCESS_DENIED", permitted ? "notice" : "alarm",
        it != impl->profile_tokens.end() ? *it : nullptr, "REPORTING", reference,
        permitted ? "report and payload members authorized" : "report action, session or member scope denied");
    return permitted;
}

bool MmsAdapter::Impl::auth_tramp(void* parameter,
                                  AcseAuthenticationParameter authParameter,
                                  void** securityToken,
                                  IsoApplicationReference* appReference) {
    auto* impl = static_cast<Impl*>(parameter);
    if (securityToken != nullptr) {
        *securityToken = nullptr;
    }
    if (impl && impl->native_required) {
        impl->emit_audit("E2E:COMPATIBILITY_ASSOCIATION_REJECTED");
        return false;
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

    if (impl != nullptr && impl->profile_a_enabled) {
        if (impl->issue_profile_a_token(mechanism, buf, len, securityToken, appReference)) {
            std::fprintf(stderr, "mms: ACSE auth ACCEPT — Profile A role token\n");
            impl->emit_audit("rbac_profile_a_auth_success");
            return true;
        }
        std::fprintf(stderr, "mms: ACSE auth REJECT — Profile A role/policy verification failed\n");
        impl->emit_audit("rbac_profile_a_auth_reject");
        return false;
    }

    MmsAcseCredentialStore creds;
    if (impl != nullptr) {
        creds.dso_der = impl->dso_der;
        creds.dso_tls_der = impl->dso_tls_der;
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
                     "mms: ACSE auth REJECT — cert not mapped to lab role (%d octets, mech=%s, "
                     "dso=%zu tls=%zu, head=%02x%02x%02x)\n",
                     len, mms_acse_mechanism_label(mechanism), creds.dso_der.size(),
                     creds.dso_tls_der.size(),
                     len > 0 ? buf[0] : 0, len > 1 ? buf[1] : 0, len > 2 ? buf[2] : 0);
        if (impl != nullptr) {
            impl->emit_audit("auth_reject_unknown_cert");
        }
        return false;
    }
    return false;
}

void MmsAdapter::Impl::conn_tramp(IedServer /*self*/, ClientConnection connection,
                                  bool connected, void* parameter) {
    auto* impl = static_cast<Impl*>(parameter);
    if (impl == nullptr) {
        return;
    }
    if (impl->profile_a_enabled && connection != nullptr) {
        const auto raw_token = static_cast<SessionToken*>(ClientConnection_getSecurityToken(connection));
        {
            std::lock_guard<std::mutex> lock(impl->mu);
            const auto found = impl->profile_tokens.find(raw_token);
            if (found != impl->profile_tokens.end()) {
                auto* token = *found;
                const auto address = ClientConnection_getPeerAddress(connection);
                token->peer = address ? address : "";
                token->identity.peer_address = token->peer;
                const auto transport = impl->tls_peer_identities.find(token->peer);
                token->transport_bound = connected && transport != impl->tls_peer_identities.end() &&
                    transport->second.certificate_pin == token->identity.transport_fingerprint &&
                    (token->identity.has_signature_proof || transport->second.key_pin == token->identity.public_key_fingerprint);
                impl->security_audit(connected ? (token->transport_bound ? "ASSOCIATION_AUTH_OK" : "ASSOCIATION_BINDING_FAIL") : "ASSOCIATION_RELEASE",
                    connected && !token->transport_bound ? "alarm" : "notice", token, "ASSOCIATION", "",
                    connected ? (token->transport_bound ? "identity bound to authenticated transport" : "transport/identity pairing mismatch; all access denied") : "disconnected");
                if (!connected) impl->tls_peer_identities.erase(token->peer);
            }
        }
        if (!connected) impl->release_profile_token(raw_token);
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

    const auto aggregator_failure = [&]() {
        ControlAction_setError(action, CONTROL_ERROR_UNKNOWN);
        ControlAction_setAddCause(action, ADD_CAUSE_INCONSISTENT_PARAMETERS);
        return CONTROL_RESULT_FAILED;
    };
    // Aggregator controls use the same checked control service, with distinct state.
    if (ctl == wsa_mod || ctl == varsa_mod) {
        if (ty != MMS_INTEGER) return aggregator_failure();
        const int v = MmsValue_toInt32(value);
        if (v != -1 && v != -5) return aggregator_failure();
        const bool active_power = ctl == wsa_mod;
        {
            std::lock_guard<std::mutex> lock(mu);
            if (active_power) { live.wsa_active = v == -1; live.aggregator_dirty = true; }
            else { live.varsa_active = v == -1; live.aggregator_reactive_dirty = true; }
            live.valid = true;
            fallback_latched = false;
        }
        IedServer_updateInt32AttributeValue(server, active_power ? wsa_mod_stval : varsa_mod_stval, v);
        // Beh/FctOpSt describe availability of the plant function, not just accepted requests.
        // The transport remains testable while physical Aggregator actuation is unavailable.
        emit_audit(active_power ? "aggregator_wsa_mode_update_outputs_disabled" : "aggregator_varsa_mode_update_outputs_disabled");
        return CONTROL_RESULT_OK;
    }
    if (ctl == wsa_wspt || ctl == varsa_vartgt) {
        MmsValue* scalar = value;
        if (ty == MMS_STRUCTURE && MmsValue_getArraySize(value) > 0) scalar = MmsValue_getElement(value, 0);
        if (!scalar || MmsValue_getType(scalar) != MMS_FLOAT) return aggregator_failure();
        const float pct = MmsValue_toFloat(scalar);
        const bool active_power = ctl == wsa_wspt;
        // CEI 57-142:2026-07 4.3.1.3.2/.3: signed P and Q, percent Smax.
        if (!std::isfinite(pct) || pct < -100.0f || pct > 100.0f) {
            emit_audit("aggregator_setpoint_rejected_range");
            return aggregator_failure();
        }
        if (!setpoint_spacing_accept(active_power ? "WSaDAGC1.WSptPct" : "VArSaDVAR1.VArTgtSptPct"))
            return aggregator_failure();
        {
            std::lock_guard<std::mutex> lock(mu);
            if (active_power) { live.wsa_spt_pct = pct; live.aggregator_dirty = true; }
            else { live.varsa_spt_pct = pct; live.aggregator_reactive_dirty = true; }
            live.valid = true;
            fallback_latched = false;
        }
        IedServer_updateFloatAttributeValue(server, active_power ? wsa_mxval : varsa_mxval, pct);
        emit_audit(active_power ? "aggregator_wsa_setpoint_update" : "aggregator_varsa_setpoint_update");
        return CONTROL_RESULT_OK;
    }

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

/** MX leaf float (e.g. CMV cVal.mag.f) — reject constructed `mag` nodes. */
static DataAttribute* model_mx_float_f(IedModel* model, const char* ln_ref,
                                       const char* do_rel_path) {
    char ref[160];
    std::snprintf(ref, sizeof(ref), "%s.%s", ln_ref, do_rel_path);
    DataAttribute* da = model_da(model, ref);
    if (da == nullptr) {
        ModelNode* ln = IedModel_getModelNodeByShortObjectReference(model, ln_ref);
        if (ln != nullptr) {
            ModelNode* node =
                ModelNode_getChildWithFc(ln, do_rel_path, IEC61850_FC_MX);
            if (node != nullptr && ModelNode_getType(node) == DataAttributeModelType) {
                da = reinterpret_cast<DataAttribute*>(node);
            }
        }
    }
    if (da == nullptr) {
        return nullptr;
    }
    if (da->mmsValue != nullptr && MmsValue_getType(da->mmsValue) != MMS_FLOAT) {
        return nullptr;
    }
    return da;
}

static DataAttribute* model_mx_float_f_ln(IedModel* model, const char* ln_ref,
                                          const char* do_rel_path) {
    DataAttribute* da = model_mx_float_f(model, ln_ref, do_rel_path);
    if (da != nullptr) {
        return da;
    }
    ModelNode* ln = IedModel_getModelNodeByShortObjectReference(model, ln_ref);
    if (ln == nullptr) {
        return nullptr;
    }
    ModelNode* node = ModelNode_getChildWithFc(ln, do_rel_path, IEC61850_FC_MX);
    if (node != nullptr && ModelNode_getType(node) == DataAttributeModelType) {
        return reinterpret_cast<DataAttribute*>(node);
    }
    return nullptr;
}

bool MmsAdapter::Impl::wire_cfg_runtime_nodes(MmsAdapter::Impl* impl) {
    IedModel* model = impl->model;
    impl->wlim_mod = model_do(model, "LD_Plant/WlimDWMX1.Mod");
    impl->wlim_wmax = model_do(model, "LD_Plant/WlimDWMX1.WMaxSptPct");
    impl->wsd_mod = model_do(model, "LD_Plant/WSdDAGC1.Mod");
    impl->wsd_wspt = model_do(model, "LD_Plant/WSdDAGC1.WSptPct");
    impl->wsa_mod = model_do(model, "LD_Plant/WSaDAGC1.Mod");
    impl->wsa_wspt = model_do(model, "LD_Plant/WSaDAGC1.WSptPct");
    impl->varsa_mod = model_do(model, "LD_Plant/VArSaDVAR1.Mod");
    impl->varsa_vartgt = model_do(model, "LD_Plant/VArSaDVAR1.VArTgtSptPct");
    impl->wsa_mod_stval = model_da(model, "LD_Plant/WSaDAGC1.Mod.stVal");
    impl->wsa_mxval = model_da(model, "LD_Plant/WSaDAGC1.WSptPct.mxVal.f");
    impl->varsa_mod_stval = model_da(model, "LD_Plant/VArSaDVAR1.Mod.stVal");
    impl->varsa_mxval = model_da(model, "LD_Plant/VArSaDVAR1.VArTgtSptPct.mxVal.f");

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

    impl->agg_totw_mag = model_da(model, "LD_Plant/PdCiMMXU1.TotW.mag.f");
    impl->agg_totw_t = model_da(model, "LD_Plant/PdCiMMXU1.TotW.t");
    impl->agg_totw_q = model_da(model, "LD_Plant/PdCiMMXU1.TotW.q");
    impl->agg_totvar_mag = model_da(model, "LD_Plant/PdCiMMXU1.TotVAr.mag.f");
    impl->agg_totvar_t = model_da(model, "LD_Plant/PdCiMMXU1.TotVAr.t");
    impl->agg_totvar_q = model_da(model, "LD_Plant/PdCiMMXU1.TotVAr.q");
    impl->wsa_beh = model_da(model, "LD_Plant/WSaDAGC1.Beh.stVal");
    impl->wsa_fct = model_da(model, "LD_Plant/WSaDAGC1.FctOpSt.stVal");
    impl->varsa_beh = model_da(model, "LD_Plant/VArSaDVAR1.Beh.stVal");
    impl->varsa_fct = model_da(model, "LD_Plant/VArSaDVAR1.FctOpSt.stVal");
    impl->totw_mag = model_da(model, "LD_Plant/PdCMMXU1.TotW.mag.f");
    impl->totw_t = model_da(model, "LD_Plant/PdCMMXU1.TotW.t");
    impl->totw_q = model_da(model, "LD_Plant/PdCMMXU1.TotW.q");
    impl->genpv_totw_mag = model_da(model, "LD_Plant/GenPVMMXU1.TotW.mag.f");
    impl->genpv_totw_t = model_da(model, "LD_Plant/GenPVMMXU1.TotW.t");
    impl->genpv_totw_q = model_da(model, "LD_Plant/GenPVMMXU1.TotW.q");
    impl->totvar_mag = model_da(model, "LD_Plant/PdCMMXU1.TotVAr.mag.f");
    impl->totvar_t = model_da(model, "LD_Plant/PdCMMXU1.TotVAr.t");
    impl->totvar_q = model_da(model, "LD_Plant/PdCMMXU1.TotVAr.q");
    impl->ppv_phsab_mag =
        model_mx_float_f(model, "LD_Plant/PdCMMXU1", "PPV.phsAB.cVal.mag.f");
    impl->ppv_phsab_ang =
        model_mx_float_f_ln(model, "LD_Plant/PdCMMXU1", "PPV.phsAB.cVal.ang.f");
    impl->ppv_phsab_t = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsAB.t");
    impl->ppv_phsab_q = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsAB.q");
    impl->ppv_phsbc_mag =
        model_mx_float_f(model, "LD_Plant/PdCMMXU1", "PPV.phsBC.cVal.mag.f");
    impl->ppv_phsbc_ang =
        model_mx_float_f_ln(model, "LD_Plant/PdCMMXU1", "PPV.phsBC.cVal.ang.f");
    impl->ppv_phsbc_t = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsBC.t");
    impl->ppv_phsbc_q = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsBC.q");
    impl->ppv_phsca_mag =
        model_mx_float_f(model, "LD_Plant/PdCMMXU1", "PPV.phsCA.cVal.mag.f");
    impl->ppv_phsca_ang =
        model_mx_float_f_ln(model, "LD_Plant/PdCMMXU1", "PPV.phsCA.cVal.ang.f");
    impl->ppv_phsca_t = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsCA.t");
    impl->ppv_phsca_q = model_da(model, "LD_Plant/PdCMMXU1.PPV.phsCA.q");
    impl->aphsa_mag = model_mx_float_f(model, "LD_Plant/PdCMMXU1", "A.phsA.cVal.mag.f");
    impl->aphsa_ang = model_mx_float_f_ln(model, "LD_Plant/PdCMMXU1", "A.phsA.cVal.ang.f");
    impl->aphsa_t = model_da(model, "LD_Plant/PdCMMXU1.A.phsA.t");
    impl->aphsa_q = model_da(model, "LD_Plant/PdCMMXU1.A.phsA.q");
    impl->aphsb_mag = model_mx_float_f(model, "LD_Plant/PdCMMXU1", "A.phsB.cVal.mag.f");
    impl->aphsb_ang = model_mx_float_f_ln(model, "LD_Plant/PdCMMXU1", "A.phsB.cVal.ang.f");
    impl->aphsb_t = model_da(model, "LD_Plant/PdCMMXU1.A.phsB.t");
    impl->aphsb_q = model_da(model, "LD_Plant/PdCMMXU1.A.phsB.q");
    impl->aphsc_mag = model_mx_float_f(model, "LD_Plant/PdCMMXU1", "A.phsC.cVal.mag.f");
    impl->aphsc_ang = model_mx_float_f_ln(model, "LD_Plant/PdCMMXU1", "A.phsC.cVal.ang.f");
    impl->aphsc_t = model_da(model, "LD_Plant/PdCMMXU1.A.phsC.t");
    impl->aphsc_q = model_da(model, "LD_Plant/PdCMMXU1.A.phsC.q");
    if (impl->aphsa_t == nullptr || impl->aphsa_q == nullptr) {
        ModelNode* ln = IedModel_getModelNodeByShortObjectReference(model, "LD_Plant/PdCMMXU1");
        if (ln != nullptr) {
            ModelNode* t = ModelNode_getChildWithFc(ln, "A.phsA.t", IEC61850_FC_MX);
            if (t != nullptr && ModelNode_getType(t) == DataAttributeModelType) {
                impl->aphsa_t = reinterpret_cast<DataAttribute*>(t);
            }
            ModelNode* q = ModelNode_getChildWithFc(ln, "A.phsA.q", IEC61850_FC_MX);
            if (q != nullptr && ModelNode_getType(q) == DataAttributeModelType) {
                impl->aphsa_q = reinterpret_cast<DataAttribute*>(q);
            }
        }
    }

    if (impl->wlim_mod == nullptr || impl->wlim_wmax == nullptr || impl->wsd_mod == nullptr ||
        impl->wsd_wspt == nullptr || impl->totw_mag == nullptr || impl->totvar_mag == nullptr) {
        std::fprintf(stderr, "mms: cfg model missing required Wlim/WSd/PdC nodes\n");
        return false;
    }
    /* Reactive LNs are optional: absent in the product 5-LN cert model
     * (PRODUCT_SPECIMEN_5LN — LLN0+LPHD+PdCMMXU+WlimDWMX+WSdDAGC only).
     * All varsd/pfsp consumers are nullptr-guarded; O.9.1.x stays inactive. */
    if (impl->varsd_mod == nullptr || impl->varsd_vartgt == nullptr) {
        std::fprintf(stderr,
                     "mms: cfg model without VArSdDVAR1 — O.9.1.4 P5-R01 inactive "
                     "(5-LN cert model)\n");
    }
    if (impl->pfsp_mod == nullptr) {
        std::fprintf(stderr,
                     "mms: cfg model without PFSPDFPF1 — O.9.1.1 P5-R02 inactive "
                     "(5-LN cert model)\n");
    }
    if (impl->aphsa_mag == nullptr) {
        std::fprintf(stderr,
                     "mms: PdCMMXU1.A.phsA.cVal.mag.f not wired — EMT432 I1 will not "
                     "appear on MMS (check .cfg / FC=MX / MMS_FLOAT leaf)\n");
    } else {
        // Dynamic-model attributes acquire their MMS values at server creation,
        // after this wiring pass. Do not dereference an uninitialized value.
        const float init = impl->aphsa_mag->mmsValue != nullptr
                               ? MmsValue_toFloat(impl->aphsa_mag->mmsValue)
                               : 0.0f;
        std::fprintf(stderr,
                     "mms: PdCMMXU1.A.phsA.cVal.mag.f wired (EMT432 I1 @922) init=%.3fA\n",
                     static_cast<double>(init));
    }
    if (impl->aphsa_ang == nullptr) {
        std::fprintf(stderr,
                     "mms: PdCMMXU1.A.phsA.cVal.ang.f NOT wired — phase angle will stay 0 "
                     "on MMS\n");
    } else {
        std::fprintf(stderr, "mms: PdCMMXU1.A.phsA.cVal.ang.f wired (atan2 Q1,P1 deg)\n");
    }
    if (impl->ppv_phsab_ang == nullptr) {
        std::fprintf(stderr, "mms: PdCMMXU1.PPV.phsAB.cVal.ang.f NOT wired\n");
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

    LogicalNode* wsa = LogicalNode_create("WSaDAGC1", ld);
    auto add_aggregator_ln_support = [](LogicalNode* ln, const char* request_name) {
        auto* parent = reinterpret_cast<ModelNode*>(ln);
        CDC_LPL_create("NamPlt", parent,
                       CDC_OPTION_AC_LN0_M | CDC_OPTION_AC_LN0_EX | CDC_OPTION_AC_DLD_M);
        CDC_MV_create(request_name, parent, 0, false);
        CDC_SPG_create("RmpRteUse", parent, 0);
        auto* ecp_ref = DataObject_create("InEcpRef", parent, 0);
        DataAttribute_create("setSrcRef", reinterpret_cast<ModelNode*>(ecp_ref),
                             IEC61850_VISIBLE_STRING_129, IEC61850_FC_SP,
                             TRG_OPT_DATA_CHANGED, 0, 0);
    };
    add_aggregator_ln_support(wsa, "ReqW");
    impl->wsa_mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(wsa), 0, CDC_CTL_MODEL_DIRECT_ENHANCED);
    impl->wsa_wspt = CDC_APC_create("WSptPct", reinterpret_cast<ModelNode*>(wsa), 0, CDC_CTL_MODEL_DIRECT_ENHANCED, false);
    impl->wsa_mod_stval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->wsa_mod), "stVal"));
    impl->wsa_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->wsa_wspt), "mxVal.f"));
    LogicalNode* varsa = LogicalNode_create("VArSaDVAR1", ld);
    add_aggregator_ln_support(varsa, "ReqVAr");
    impl->varsa_mod = CDC_ENC_create("Mod", reinterpret_cast<ModelNode*>(varsa), 0, CDC_CTL_MODEL_DIRECT_ENHANCED);
    impl->varsa_vartgt = CDC_APC_create("VArTgtSptPct", reinterpret_cast<ModelNode*>(varsa), 0, CDC_CTL_MODEL_DIRECT_ENHANCED, false);
    impl->varsa_mod_stval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->varsa_mod), "stVal"));
    impl->varsa_mxval = reinterpret_cast<DataAttribute*>(ModelNode_getChild(reinterpret_cast<ModelNode*>(impl->varsa_vartgt), "mxVal.f"));
    DataAttribute* stub_varv = stub_mod_inactive(ld, "VArVDVVR1");
    DataAttribute* stub_pfw = stub_mod_inactive(ld, "PFWDPFW1");
    (void)stub_varv;
    (void)stub_pfw;

    for (auto* node : {wsa, varsa}) {
        for (const char* name : {"Beh", "Health", "FctOpSt"})
            CDC_ENS_create(name, reinterpret_cast<ModelNode*>(node), 0);
    }
    LogicalNode* agg_meter = LogicalNode_create("PdCiMMXU1", ld);
    CDC_MV_create("TotW", reinterpret_cast<ModelNode*>(agg_meter), 0, false);
    CDC_MV_create("TotVAr", reinterpret_cast<ModelNode*>(agg_meter), 0, false);

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

    DataObject* a_wye = CDC_WYE_create("A", reinterpret_cast<ModelNode*>(pdc), 0);
    DataObject* phsa = reinterpret_cast<DataObject*>(
        ModelNode_getChild(reinterpret_cast<ModelNode*>(a_wye), "phsA"));
    if (phsa != nullptr) {
        DataAttribute* cval = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(phsa), "cVal"));
        if (cval != nullptr) {
            DataAttribute* mag = reinterpret_cast<DataAttribute*>(
                ModelNode_getChild(reinterpret_cast<ModelNode*>(cval), "mag"));
            if (mag != nullptr) {
                impl->aphsa_mag = reinterpret_cast<DataAttribute*>(
                    ModelNode_getChild(reinterpret_cast<ModelNode*>(mag), "f"));
            }
        }
        impl->aphsa_t = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(phsa), "t"));
        impl->aphsa_q = reinterpret_cast<DataAttribute*>(
            ModelNode_getChild(reinterpret_cast<ModelNode*>(phsa), "q"));
    }

    DataSet* ds = DataSet_create("DS_R_PdC_Mis4sec", lln0);
    DataSetEntry_create(ds, "PdCMMXU1$MX$TotW", -1, nullptr);
    DataSetEntry_create(ds, "PdCMMXU1$MX$TotVAr", -1, nullptr);
    DataSetEntry_create(ds, "PdCMMXU1$MX$PPV$phsAB$cVal$mag$f", -1, nullptr);
    DataSetEntry_create(ds, "PdCMMXU1$MX$A$phsA$cVal$mag$f", -1, nullptr);

    const uint8_t trg =
        TRG_OPT_DATA_CHANGED | TRG_OPT_QUALITY_CHANGED | TRG_OPT_INTEGRITY;
    const uint8_t rpt_opts =
        RPT_OPT_SEQ_NUM | RPT_OPT_TIME_STAMP | RPT_OPT_REASON_FOR_INCLUSION |
        RPT_OPT_DATA_SET | RPT_OPT_DATA_REFERENCE;
    ReportControlBlock_create(
        "urcb_PdC_Mis4sec", lln0, "CCI016_01LD_Plant/LLN0.urcb_PdC_Mis4sec",
        false, "DS_R_PdC_Mis4sec", 1, trg, rpt_opts, 0, 4000);
    DataSet* agg_ds = DataSet_create("DS_R_Aggregator_Mis4sec", lln0);
    DataSetEntry_create(agg_ds, "PdCiMMXU1$MX$TotW", -1, nullptr);
    DataSetEntry_create(agg_ds, "PdCiMMXU1$MX$TotVAr", -1, nullptr);
    for (const bool buffered : {false, true}) {
        const char* prefix = buffered ? "brcb" : "urcb";
        for (const char* instance : {"01", "02"}) {
            const std::string name = std::string(prefix) + "_Aggregator_Mis4sec" + instance;
            const std::string id = std::string("CCI016_01LD_Plant/LLN0.") + name;
            ReportControlBlock_create(name.c_str(), lln0, id.c_str(), buffered,
                "DS_R_Aggregator_Mis4sec", 1, trg, rpt_opts, 0, 4000);
        }
    }
    // Use the same runtime wiring for both generated and handbuilt models.
    return wire_cfg_runtime_nodes(impl);
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

bool MmsAdapter::start(const core::MmsConfig& cfg, MmsAuditFn audit,
                       MmsTimeSyncAuditFn time_sync_audit) {
    if (!cfg.e2e.configuration_valid) {
        std::fprintf(stderr, "mms: invalid native E2E configuration; startup refused\n");
        return false;
    }
    if (cfg.e2e.enabled && (!cfg.tls.enabled || !cfg.tls.chain_validation || !cfg.rbac_profile_a)) {
        std::fprintf(stderr, "mms: native E2E requires mutual TLS, chain validation and commissioned Profile A RBAC\n");
        return false;
    }
    // No signed active-power tracking backend exists yet. Never reinterpret a signed
    // Aggregator request as a PF2 export ceiling or enable partial plant actuation.
    if (cfg.aggregator_plant_outputs_enabled) {
        std::fprintf(stderr, "mms: Aggregator plant outputs require a validated signed P/Q backend; startup refused\n");
        return false;
    }
    const char* bind_address = cfg.bind_address.c_str();
    const int tcp_port = cfg.tcp_port;

    if (impl_ != nullptr && impl_->running) {
        return true;
    }
    stop();

    auto* impl = new Impl();
    impl->native_required = cfg.e2e.enabled && !cfg.e2e.allow_compatibility;
    impl->audit = std::move(audit);
    impl->time_sync_audit = std::move(time_sync_audit);
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

    if (cfg.rbac_profile_a) {
        for (auto* block = impl->model->rcbs; block; block = block->sibling)
            impl->report_blocks.push_back(block);
        std::string policy_error;
        if (!impl->audit) policy_error = "security event handler required";
        if (!impl->audit || !cfg.tls.enabled || !cfg.tls.chain_validation || cfg.rbac_policy_path.empty() ||
            !impl->rbac_security.load(cfg.rbac_policy_path, cfg.rbac_owner_cert,
                cfg.rbac_activation_state,
                cfg.rbac_identity_ca.empty() ? cfg.tls.ca_cert : cfg.rbac_identity_ca,
                cfg.rbac_identity_crl.empty() ? cfg.tls.crl_path : cfg.rbac_identity_crl,
                impl->commissioning_policy, policy_error)) {
            std::fprintf(stderr,
                         "mms: Profile A requires TLS chain validation and a valid commissioning policy (%s)\n",
                         policy_error.empty() ? "missing TLS/configuration" : policy_error.c_str());
            impl->security_audit("POLICY_ACTIVATION_FAIL", "alarm", nullptr, "COMMISSIONING", cfg.rbac_policy_path, policy_error);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        impl->profile_a_enabled = true;
        impl->security_audit("POLICY_ACTIVATED", "notice", nullptr, "COMMISSIONING", cfg.rbac_policy_path,
                              "owner signature verified: " + impl->rbac_security.owner_fingerprint());
        std::fprintf(stderr, "mms: Profile A commissioning policy %s revision %s loaded\n",
                     impl->commissioning_policy.policy_id().c_str(),
                     impl->commissioning_policy.revision().c_str());
    }

#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (cfg.tls.enabled) {
        if (cfg.tls.own_key.empty() || cfg.tls.own_cert.empty() ||
            cfg.tls.ca_cert.empty() || cfg.tls.crl_path.empty() ||
            !cfg.tls.chain_validation || !cfg.tls.configuration_valid) {
            const bool endpoint_identity_missing =
                cfg.tls.own_key.empty() || cfg.tls.own_cert.empty();
            const bool crl_missing = cfg.tls.crl_path.empty();
            impl->security_audit(endpoint_identity_missing ? "TLS_SECURITY_EVENT" : "TLS_CONFIGURATION_REJECTED",
                "alarm", nullptr, "TLS",
                endpoint_identity_missing ? std::to_string(TLS_EVENT_CODE_ALM_CERT_UNAVAILABLE) :
                    crl_missing ? std::to_string(CCLI_TLS_EVENT_CODE_CRL) : "",
                endpoint_identity_missing ? "alarm: endpoint certificate unavailable" :
                    crl_missing ? "alarm: TLS requires a configured CRL; peer revocation cannot be checked" :
                    "Missing TLS CA, disabled chain validation or invalid timers");
            std::fprintf(stderr,
                         "mms: TLS enabled but identity, CA, CRL or TLS policy is incomplete\n");
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        impl->tls = TLSConfiguration_create();
        if (!impl->tls) {
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        TLSConfiguration_setMinTlsVersion(impl->tls, TLS_VERSION_TLS_1_2);
        /* 62351-3 / PIXIT: TLS 1.2 only (no TLS 1.3 claim) */
        TLSConfiguration_setMaxTlsVersion(impl->tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setChainValidation(impl->tls, cfg.tls.chain_validation);
        const bool have_allow =
            !cfg.tls.client_cert.empty() || !cfg.tls.acse_client_cert.empty() ||
            !cfg.tls.viewer_cert.empty() || !cfg.tls.acse_viewer_cert.empty() ||
            !cfg.tls.revoked_cert.empty() || !cfg.tls.additional_client_cert.empty() ||
            !cfg.tls.allowed_client_certs.empty();
        TLSConfiguration_setAllowOnlyKnownCertificates(impl->tls, have_allow);
        TLSConfiguration_setEventHandler(impl->tls, Impl::tls_event_tramp, impl);

        if (!TLSConfiguration_setOwnKeyFromFile(impl->tls, cfg.tls.own_key.c_str(),
                                                nullptr)) {
            impl->security_audit("TLS_SECURITY_EVENT", "alarm", nullptr, "TLS",
                std::to_string(TLS_EVENT_CODE_ALM_CERT_UNAVAILABLE),
                "alarm: endpoint certificate unavailable");
            std::fprintf(stderr, "mms: TLS failed to load own key %s\n",
                         cfg.tls.own_key.c_str());
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        if (!TLSConfiguration_setOwnCertificateFromFile(impl->tls,
                                                        cfg.tls.own_cert.c_str())) {
            impl->security_audit("TLS_SECURITY_EVENT", "alarm", nullptr, "TLS",
                std::to_string(TLS_EVENT_CODE_ALM_CERT_UNAVAILABLE),
                "alarm: endpoint certificate unavailable");
            std::fprintf(stderr, "mms: TLS failed to load own cert %s\n",
                         cfg.tls.own_cert.c_str());
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        if (cfg.tls.additional_own_cert.empty() != cfg.tls.additional_own_key.empty()) {
            impl->security_audit("TLS_CONFIGURATION_REJECTED", "alarm", nullptr, "TLS", "",
                "MMS additional server certificate and key must be configured together");
            std::fprintf(stderr, "mms: additional TLS server identity requires both certificate and key\n");
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
        }
        if (!cfg.tls.additional_own_cert.empty() &&
            !TLSConfiguration_addOwnCertificateAndKeyFromFiles(
                impl->tls, cfg.tls.additional_own_cert.c_str(),
                cfg.tls.additional_own_key.c_str(), nullptr)) {
            impl->security_audit("TLS_SECURITY_EVENT", "alarm", nullptr, "TLS",
                std::to_string(TLS_EVENT_CODE_ALM_CERT_UNAVAILABLE),
                "alarm: additional endpoint certificate unavailable or key mismatch");
            std::fprintf(stderr, "mms: failed to load additional TLS server identity\n");
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
                impl->security_audit("TLS_SECURITY_EVENT", "alarm", nullptr, "TLS",
                    std::to_string(CCLI_TLS_EVENT_CODE_CRL),
                    "alarm: configured TLS CRL could not be loaded");
                std::fprintf(stderr, "mms: TLS failed to load CRL %s\n",
                             cfg.tls.crl_path.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
            std::fprintf(stderr, "mms: TLS CRL loaded %s\n", cfg.tls.crl_path.c_str());
        }
        if (!CCLI_TLS_setOptionalHashes(impl->tls, cfg.tls.allow_sha384, cfg.tls.allow_sha512)) {
            impl->security_audit("TLS_CONFIGURATION_REJECTED", "alarm", nullptr, "TLS", "",
                "Optional TLS signature hash policy is unsupported by this build");
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false;
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
            if (!cfg.tls.client_cert.empty() &&
                cfg.tls.client_cert != dso_acse &&
                !load_pem_der(cfg.tls.client_cert, impl->dso_tls_der)) {
                std::fprintf(stderr, "mms: failed to parse DSO TLS cert DER %s\n",
                             cfg.tls.client_cert.c_str());
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
        if (!cfg.tls.additional_client_cert.empty()) {
            if (!TLSConfiguration_addAllowedCertificateFromFile(
                    impl->tls, cfg.tls.additional_client_cert.c_str())) {
                std::fprintf(stderr, "mms: TLS failed to load additional client allowlist %s\n",
                             cfg.tls.additional_client_cert.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
        }
        for (const auto& cert : cfg.tls.allowed_client_certs) {
            if (!TLSConfiguration_addAllowedCertificateFromFile(impl->tls, cert.c_str())) {
                std::fprintf(stderr, "mms: TLS failed to load allowed client certificate %s\n", cert.c_str());
                TLSConfiguration_destroy(impl->tls);
                IedModel_destroy(impl->model);
                delete impl;
                return false;
            }
        }
        /* Install every peer pin before CCLI_TLS_configure finalizes the TLS policy. */
        if (!CCLI_TLS_configure(impl->tls, cfg.tls.crl_path.c_str(),
                cfg.tls.revocation_check_s, cfg.tls.renegotiation_s, cfg.tls.resumption_s)) {
            TLSConfiguration_destroy(impl->tls);
            IedModel_destroy(impl->model);
            delete impl;
            return false; /* The registered HAL event handler persists the rejection. */
        }
        impl->rbac_enabled = !impl->dso_der.empty();
        IedServerConfig server_cfg = IedServerConfig_create();
        if (impl->profile_a_enabled) {
            // These services are outside the declared first-pass MMS PICS.
            // Keep the static datasets/RCBs; avoid unguarded file or dynamic lists.
            IedServerConfig_enableFileService(server_cfg, false);
            IedServerConfig_enableDynamicDataSetService(server_cfg, false);
            IedServerConfig_enableLogService(server_cfg, false);
        }
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
    if (cfg.e2e.enabled) {
        using namespace ccli::mms::e2e;
        NativeServerFiles files;
        files.allow_compatibility = cfg.e2e.allow_compatibility;
        files.certificate = cfg.e2e.certificate; files.private_key = cfg.e2e.private_key;
        files.transport_certificate = cfg.tls.own_cert;
        files.trust_anchors = cfg.e2e.trust_anchors; files.crls = cfg.e2e.crls;
        files.certificate_path = cfg.e2e.certificate_path;
        files.ap_title = cfg.e2e.ap_title; files.ae_qualifier = cfg.e2e.ae_qualifier;
        files.rekey_seconds = cfg.e2e.rekey_seconds;
        files.signature_scheme = cfg.e2e.signature_algorithm == "ecdsa-sha256"
            ? SignatureScheme::ecdsa_sha256 : SignatureScheme::rsa_pkcs1_v15_sha256;
        impl->native_e2e.reset(new NativeServer);
        std::string error;
        const bool configured = (cfg.e2e.signature_algorithm == "rsa-sha256" ||
            cfg.e2e.signature_algorithm == "ecdsa-sha256") && impl->native_e2e->configure(files,
            [impl](ByteView cert, ByteView transport, const std::string& peer, void** out) {
                MmsRbacIdentity identity; std::string reason;
                if (!out || !impl->rbac_security.authenticate(cert.data, cert.size,
                    impl->commissioning_policy, identity, reason) ||
                    identity.transport_fingerprint != MmsRbacSecurity::fingerprint(transport.data, transport.size)) {
                    impl->security_audit("E2E_AUTHORIZATION_FAIL", "alarm", nullptr, "ASSOCIATION", "", reason);
                    return false;
                }
                // Proof has already been checked over the native HandshakeReq TBS.
                identity.has_signature_proof = true; identity.peer_address = peer;
                auto token = std::make_unique<Impl::SessionToken>();
                token->identity = std::move(identity); token->peer = peer; token->transport_bound = true;
                std::lock_guard<std::mutex> lock(impl->mu);
                if (impl->profile_tokens.size() >= 1024) return false;
                impl->tls_peer_identities[peer] = Impl::TlsPeerIdentity{token->identity.transport_fingerprint,
                    MmsRbacSecurity::public_key_fingerprint(transport.data, transport.size)};
                impl->profile_tokens.insert(token.get());
                impl->security_audit("E2E_AUTHORIZATION_OK", "notice", token.get(), "ASSOCIATION", "", "native signature and commissioned transport/role binding verified");
                *out = token.release(); return true;
            }, {}, [impl](const std::string& event) { impl->emit_audit(event.c_str()); }, error);
        if (!configured || !impl->native_e2e->install(impl)) {
            std::fprintf(stderr, "mms: native E2E startup refused: %s\n", error.c_str());
            IedServer_destroy(impl->server);
#if defined(CONFIG_MMS_SUPPORT_TLS)
            if (impl->tls) TLSConfiguration_destroy(impl->tls);
#endif
            IedModel_destroy(impl->model); delete impl; return false;
        }
        std::fprintf(stderr, "mms: native E2E enabled; compatibility=%s\n",
            cfg.e2e.allow_compatibility ? "explicitly allowed" : "rejected");
    }
    IedServer_setConnectionIndicationHandler(impl->server, Impl::conn_tramp, impl);
    if (impl->profile_a_enabled) {
        for (auto* device = impl->model->firstChild; device; device = reinterpret_cast<LogicalDevice*>(device->sibling))
            impl->install_rbac_writes(reinterpret_cast<ModelNode*>(device));
        IedServer_setReadAccessHandler(impl->server, Impl::read_access_tramp, impl);
        IedServer_setDataSetAccessHandler(impl->server, Impl::dataset_access_tramp, impl);
        IedServer_setDirectoryAccessHandler(impl->server, Impl::directory_access_tramp, impl);
        IedServer_setListObjectsAccessHandler(impl->server, Impl::list_access_tramp, impl);
        IedServer_setControlBlockAccessHandler(impl->server,
                                               Impl::control_block_access_tramp, impl);
    }

    /* ctlModel: CID + .cfg use direct-with-enhanced-security (TSP Compare + Direct Operate). */
    for (auto* object : {impl->wsa_mod, impl->wsa_wspt, impl->varsa_mod, impl->varsa_vartgt}) {
        if (!object) continue;
        IedServer_setPerformCheckHandler(impl->server, object, Impl::check_tramp, impl);
        IedServer_setControlHandler(impl->server, object, Impl::control_tramp, impl);
    }

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

    const std::string aggregator_namespace = "(Tr)IEC 61850-CEI57-142:2026A";
    auto set_aggregator_nameplate = [&](const char* ln) {
        const std::string base = std::string("LD_Plant/") + ln + ".NamPlt.";
        const std::pair<const char*, const char*> values[] = {
            {"vendor", "CCLI"}, {"swRev", "0.1.0"}, {"configRev", "1"},
            {"ldNs", aggregator_namespace.c_str()}, {"lnNs", aggregator_namespace.c_str()},
        };
        for (const auto& [attribute, value] : values) {
            if (auto* target = model_da(impl->model, (base + attribute).c_str())) {
                std::string mutable_value(value);
                IedServer_updateVisibleStringAttributeValue(impl->server, target,
                                                              mutable_value.data());
            }
        }
    };
    set_aggregator_nameplate("WSaDAGC1");
    set_aggregator_nameplate("VArSaDVAR1");

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
        if (impl->wsa_mod_stval) IedServer_updateInt32AttributeValue(impl->server, impl->wsa_mod_stval, -5);
        if (impl->varsa_mod_stval) IedServer_updateInt32AttributeValue(impl->server, impl->varsa_mod_stval, -5);
        if (impl->wsa_mxval) IedServer_updateFloatAttributeValue(impl->server, impl->wsa_mxval, 100.0f);
        static const char* k_stub_lns[] = {"VArVDVVR1", "PFWDPFW1"};
        for (const char* ln : k_stub_lns) {
            char path[96];
            std::snprintf(path, sizeof(path), "LD_Plant/%s.Mod.stVal", ln);
            DataAttribute* st = model_da(impl->model, path);
            if (st != nullptr) {
                IedServer_updateInt32AttributeValue(impl->server, st, 5);
            }
        }
    }

    // Plant output capability is deliberately unavailable. Do not advertise accepted
    // test requests as a running physical regulation function.
    for (auto* attr : {impl->wsa_beh, impl->varsa_beh})
        if (attr) IedServer_updateInt32AttributeValue(impl->server, attr, 5);
    for (const char* ref : {"LD_Plant/WSaDAGC1.Health.stVal", "LD_Plant/VArSaDVAR1.Health.stVal"})
        if (auto* attr=model_da(impl->model,ref)) IedServer_updateInt32AttributeValue(impl->server,attr,1);
    for (auto* attr : {impl->wsa_fct, impl->varsa_fct})
        if (attr) IedServer_updateInt32AttributeValue(impl->server, attr, 0);

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

    for (auto* attr : {impl->agg_totw_q, impl->agg_totvar_q})
        if (attr) IedServer_updateQuality(impl->server, attr,
            static_cast<Quality>(QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE));
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
    for (auto* token : impl_->profile_tokens) {
        delete token;
    }
    impl_->profile_tokens.clear();
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
    if (impl_->agg_totw_t) IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->agg_totw_t, Hal_getTimeInMs());
    if (impl_->agg_totw_mag) IedServer_updateFloatAttributeValue(impl_->server, impl_->agg_totw_mag, static_cast<float>(p_kw));
    IedServer_unlockDataModel(impl_->server);
}

void MmsAdapter::update_gen_pv_active_power_kw(double p_kw, core::DataQuality quality) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr ||
        (impl_->genpv_totw_mag == nullptr && impl_->genpv_totw_q == nullptr)) {
        return;
    }
    Quality protocol_quality = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
    switch (quality) {
    case core::DataQuality::Good:
        protocol_quality = QUALITY_VALIDITY_GOOD;
        break;
    case core::DataQuality::Stale:
        protocol_quality = QUALITY_VALIDITY_QUESTIONABLE | QUALITY_DETAIL_OLD_DATA;
        break;
    case core::DataQuality::Invalid:
        break;
    }
    IedServer_lockDataModel(impl_->server);
    if (impl_->genpv_totw_q != nullptr)
        IedServer_updateQuality(impl_->server, impl_->genpv_totw_q, protocol_quality);
    if (quality == core::DataQuality::Good) {
        if (impl_->genpv_totw_t != nullptr)
            IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->genpv_totw_t,
                                                  Hal_getTimeInMs());
        if (impl_->genpv_totw_mag != nullptr)
            IedServer_updateFloatAttributeValue(impl_->server, impl_->genpv_totw_mag,
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
    if (impl_->agg_totvar_t) IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->agg_totvar_t, Hal_getTimeInMs());
    if (impl_->agg_totvar_mag) IedServer_updateFloatAttributeValue(impl_->server, impl_->agg_totvar_mag, static_cast<float>(q_kvar));
    IedServer_unlockDataModel(impl_->server);
}

void MmsAdapter::update_aggregator_measurement_quality(core::DataQuality quality) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        return;
    }

    Quality protocol_quality = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
    switch (quality) {
    case core::DataQuality::Good:
        protocol_quality = QUALITY_VALIDITY_GOOD;
        break;
    case core::DataQuality::Stale:
        protocol_quality = QUALITY_VALIDITY_QUESTIONABLE | QUALITY_DETAIL_OLD_DATA;
        break;
    case core::DataQuality::Invalid:
        break;
    }

    IedServer_lockDataModel(impl_->server);
    for (auto* attr : {impl_->agg_totw_q, impl_->agg_totvar_q}) {
        if (attr != nullptr) {
            IedServer_updateQuality(impl_->server, attr, protocol_quality);
        }
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

void MmsAdapter::update_phs_a_a(double amps) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr) {
        return;
    }
    IedServer_lockDataModel(impl_->server);
    if (impl_->aphsa_t != nullptr) {
        IedServer_updateUTCTimeAttributeValue(impl_->server, impl_->aphsa_t,
                                              Hal_getTimeInMs());
    }
    if (impl_->aphsa_mag != nullptr) {
        IedServer_updateFloatAttributeValue(impl_->server, impl_->aphsa_mag,
                                            static_cast<float>(amps));
        static std::int64_t last_log_ms = 0;
        const std::int64_t now = Hal_getTimeInMs();
        if (amps > 0.5 && (last_log_ms == 0 || (now - last_log_ms) > 30000)) {
            last_log_ms = now;
            std::fprintf(stderr, "mms: PdCMMXU1.A.phsA.cVal.mag.f publish %.3fA\n", amps);
        }
    }
    IedServer_unlockDataModel(impl_->server);
}

namespace {

void update_cmv_leaf(IedServer server, DataAttribute* mag, DataAttribute* ang,
                     DataAttribute* t, const double mag_val, const double ang_deg) {
    const std::int64_t now = Hal_getTimeInMs();
    if (t != nullptr) {
        IedServer_updateUTCTimeAttributeValue(server, t, now);
    }
    if (mag != nullptr) {
        IedServer_updateFloatAttributeValue(server, mag, static_cast<float>(mag_val));
    }
    if (ang != nullptr) {
        IedServer_updateFloatAttributeValue(server, ang, static_cast<float>(ang_deg));
    }
}

}  // namespace

void MmsAdapter::update_pdc_phasors(const core::PdcMmxuPhasors& pdc) {
    if (impl_ == nullptr || !impl_->running || impl_->server == nullptr || !pdc.valid) {
        return;
    }
    IedServer_lockDataModel(impl_->server);
    IedServer srv = impl_->server;
    update_cmv_leaf(srv, impl_->ppv_phsab_mag, impl_->ppv_phsab_ang, impl_->ppv_phsab_t,
                    pdc.ppv_ab_kv, pdc.ppv_ab_ang_deg);
    update_cmv_leaf(srv, impl_->ppv_phsbc_mag, impl_->ppv_phsbc_ang, impl_->ppv_phsbc_t,
                    pdc.ppv_bc_kv, pdc.ppv_bc_ang_deg);
    update_cmv_leaf(srv, impl_->ppv_phsca_mag, impl_->ppv_phsca_ang, impl_->ppv_phsca_t,
                    pdc.ppv_ca_kv, pdc.ppv_ca_ang_deg);
    update_cmv_leaf(srv, impl_->aphsa_mag, impl_->aphsa_ang, impl_->aphsa_t, pdc.a_phs_a,
                    pdc.a_phs_a_ang_deg);
    update_cmv_leaf(srv, impl_->aphsb_mag, impl_->aphsb_ang, impl_->aphsb_t, pdc.a_phs_b,
                    pdc.a_phs_b_ang_deg);
    update_cmv_leaf(srv, impl_->aphsc_mag, impl_->aphsc_ang, impl_->aphsc_t, pdc.a_phs_c,
                    pdc.a_phs_c_ang_deg);
    static std::int64_t last_ang_log_ms = 0;
    const std::int64_t now = Hal_getTimeInMs();
    if (impl_->aphsa_ang != nullptr &&
        (std::fabs(pdc.a_phs_a_ang_deg) > 0.05 ||
         std::fabs(pdc.ppv_ab_ang_deg) > 0.05) &&
        (last_ang_log_ms == 0 || (now - last_ang_log_ms) > 30000)) {
        last_ang_log_ms = now;
        std::fprintf(stderr,
                     "mms: PdCMMXU1 angles publish A.phsA.ang=%.2fdeg PPV.phsAB.ang=%.2fdeg\n",
                     pdc.a_phs_a_ang_deg, pdc.ppv_ab_ang_deg);
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
    const std::string time_source = chrony_ok ? "chrony" :
                                    ok ? "gnss" : "unavailable";
    std::string time_sync_event;
    core::TimeSyncEventTracker candidate;
    {
        std::lock_guard<std::mutex> lock(impl_->mu);
        candidate = impl_->time_sync_events;
        time_sync_event = candidate.update(!not_sync, time_source);
    }
    if (!time_sync_event.empty() && impl_->time_sync_audit) {
        if (impl_->time_sync_audit(time_sync_event, time_source, offset_ms, fix, !not_sync)) {
            std::lock_guard<std::mutex> lock(impl_->mu);
            impl_->time_sync_events = candidate;
        }
    }
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
    if (impl_->ppv_phsab_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->ppv_phsab_q, poc_q);
    }
    if (impl_->aphsa_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->aphsa_q, poc_q);
    }
    if (impl_->ppv_phsbc_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->ppv_phsbc_q, poc_q);
    }
    if (impl_->ppv_phsca_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->ppv_phsca_q, poc_q);
    }
    if (impl_->aphsb_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->aphsb_q, poc_q);
    }
    if (impl_->aphsc_q != nullptr) {
        IedServer_updateQuality(impl_->server, impl_->aphsc_q, poc_q);
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
    if (!impl_->live.dirty && !impl_->live.reactive_dirty && !impl_->live.pfsp_dirty &&
        !impl_->live.aggregator_dirty && !impl_->live.aggregator_reactive_dirty) {
        return false;
    }
    out = impl_->live;
    impl_->live.dirty = false;
    impl_->live.reactive_dirty = false;
    impl_->live.pfsp_dirty = false;
    impl_->live.aggregator_dirty = false;
    impl_->live.aggregator_reactive_dirty = false;
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
        impl_->live.wsa_active = false;
        impl_->live.wsa_spt_pct = 100.0;
        impl_->live.varsa_active = false;
        impl_->live.varsa_spt_pct = 0.0;
        impl_->live.aggregator_dirty = true;
        impl_->live.aggregator_reactive_dirty = true;
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
    if (server && impl_->wsa_mod_stval) IedServer_updateInt32AttributeValue(server, impl_->wsa_mod_stval, -5);
    if (server && impl_->varsa_mod_stval) IedServer_updateInt32AttributeValue(server, impl_->varsa_mod_stval, -5);
    if (server && impl_->wsa_mxval) IedServer_updateFloatAttributeValue(server, impl_->wsa_mxval, 100.0f);
    if (server && impl_->varsa_mxval) IedServer_updateFloatAttributeValue(server, impl_->varsa_mxval, 0.0f);
    return true;
}

int MmsAdapter::client_count() const {
    if (impl_ == nullptr) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(impl_->mu);
    return impl_->client_count;
}

bool MmsAdapter::operating_rule_fallback_latched() const {
    if (impl_ == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(impl_->mu);
    return impl_->fallback_latched;
}

#else /* !CCLI_HAVE_LIBIEC61850 */

struct MmsAdapter::Impl {};

MmsAdapter::~MmsAdapter() = default;

bool MmsAdapter::start(const core::MmsConfig& cfg, MmsAuditFn audit,
                       MmsTimeSyncAuditFn time_sync_audit) {
    (void)cfg;
    (void)audit;
    (void)time_sync_audit;
    std::fprintf(stderr,
                 "mms: stub (libiec61850 not linked) — rebuild with "
                 "CCLI_HAVE_LIBIEC61850\n");
    return false;
}

void MmsAdapter::stop() {}

bool MmsAdapter::is_running() const { return false; }

void MmsAdapter::update_tot_w_kw(double /*p_kw*/) {}

void MmsAdapter::update_gen_pv_active_power_kw(double /*p_kw*/, core::DataQuality /*quality*/) {}

void MmsAdapter::update_totvar_kvar(double /*q_kvar*/) {}

void MmsAdapter::update_aggregator_measurement_quality(core::DataQuality /*quality*/) {}

void MmsAdapter::update_ppv_kv(double /*ppv_kv*/) {}

void MmsAdapter::update_phs_a_a(double /*amps*/) {}

void MmsAdapter::update_pdc_phasors(const core::PdcMmxuPhasors& /*pdc*/) {}

bool MmsAdapter::poll_dso_live_command(DsoLiveCommand& /*out*/) { return false; }

bool MmsAdapter::poll_comms_loss_fallback() { return false; }

bool MmsAdapter::operating_rule_fallback_latched() const { return false; }

void MmsAdapter::refresh_time_quality() {}

int MmsAdapter::client_count() const { return 0; }

bool MmsAdapter::gnss_fix() const { return false; }

void MmsAdapter::enable_goose_publishing(const core::GooseConfig& /*cfg*/) {}

#endif

}  // namespace cci::adapters
