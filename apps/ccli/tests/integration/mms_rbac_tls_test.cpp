#include "mms_adapter.hpp"
#include "config/ccli_config.hpp"
extern "C" {
#include "iec61850_client.h"
#include "tls_config.h"
#include "tls_socket.h"
}
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {
void check(bool value, const std::string& message) { if (!value) throw std::runtime_error(message); }
std::string read(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    check(static_cast<bool>(file), "cannot read test fixture");
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
int free_port() {
    const int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    check(socket >= 0, "cannot allocate loopback socket");
    sockaddr_in address{}; address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    check(::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0, "loopback bind failed");
    socklen_t size = sizeof(address);
    check(::getsockname(socket, reinterpret_cast<sockaddr*>(&address), &size) == 0, "loopback port lookup failed");
    ::close(socket);
    return ntohs(address.sin_port);
}
struct Client {
    TLSConfiguration tls{TLSConfiguration_create()};
    IedConnection connection{nullptr};
    Client(const std::filesystem::path& root, const char* name, int port) {
        TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setChainValidation(tls, true);
        TLSConfiguration_setAllowOnlyKnownCertificates(tls, false);
        check(TLSConfiguration_addCACertificateFromFile(tls, (root / "ca.pem").c_str()), "client CA load failed");
        check(TLSConfiguration_setOwnCertificateFromFile(tls, (root / (std::string(name) + ".pem")).c_str()), "client certificate load failed");
        check(TLSConfiguration_setOwnKeyFromFile(tls, (root / (std::string(name) + ".key")).c_str(), nullptr), "client key load failed");
        connection = IedConnection_createWithTlsSupport(tls);
        IedClientError error;
        IedConnection_connect(connection, &error, "127.0.0.1", port);
        check(error == IED_ERROR_OK, "TLS/MMS connection failed: " + std::to_string(error));
    }
    ~Client() {
        if (connection) { IedConnection_close(connection); IedConnection_destroy(connection); }
        TLSConfiguration_destroy(tls);
    }
    bool readable(const char* ref, FunctionalConstraint fc) {
        IedClientError error;
        MmsValue* value = IedConnection_readObject(connection, &error, ref, fc);
        const bool allowed = error == IED_ERROR_OK && value && MmsValue_getType(value) != MMS_DATA_ACCESS_ERROR;
        if (!allowed)
            std::cerr << "read denied: " << ref << " error=" << error << " access="
                      << (value && MmsValue_getType(value) == MMS_DATA_ACCESS_ERROR ? MmsValue_getDataAccessError(value) : -1) << '\n';
        if (value) MmsValue_delete(value);
        return allowed;
    }
};
void report(void* parameter, ClientReport) { ++*static_cast<std::atomic<unsigned>*>(parameter); }
bool control_request(Client& client, const std::string& reference, float value, bool integer) {
    auto ctl = ControlObjectClient_create(reference.c_str(), client.connection);
    if (!ctl) return false;
    struct Completion { std::atomic<bool> done{false}, good{false}; } completion;
    ControlObjectClient_setCommandTerminationHandler(ctl, [](void* p, ControlObjectClient c) {
        auto* result = static_cast<Completion*>(p);
        const auto error = ControlObjectClient_getLastApplError(c);
        result->good = error.error == CONTROL_ERROR_NO_ERROR && error.addCause == ADD_CAUSE_UNKNOWN;
        result->done = true;
    }, &completion);
    auto* v = integer ? MmsValue_newIntegerFromInt32(static_cast<int>(value)) : MmsValue_newFloat(value);
    ControlObjectClient_setOrigin(ctl, "isolated-annex-t-rbac", 1);
    bool ok = ControlObjectClient_operate(ctl, v, 0);
    if (ok) {
        for (unsigned i = 0; i < 100 && !completion.done; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ok = completion.done && completion.good;
    }
    MmsValue_delete(v);
    ControlObjectClient_destroy(ctl);
    return ok;
}
bool contains(LinkedList list, const char* name) {
    for (auto item = LinkedList_getNext(list); item; item = LinkedList_getNext(item))
        if (std::string(static_cast<const char*>(LinkedList_getData(item))) == name) return true;
    return false;
}
bool tls_connects(const std::filesystem::path& root, const char* name, int port) {
    TLSConfiguration tls = TLSConfiguration_create();
    TLSConfiguration_setClientMode(tls);
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tls, true);
    check(TLSConfiguration_addCACertificateFromFile(tls, (root / "ca.pem").c_str()), "probe CA load");
    check(TLSConfiguration_setOwnCertificateFromFile(tls, (root / (std::string(name) + ".pem")).c_str()), "probe certificate load");
    check(TLSConfiguration_setOwnKeyFromFile(tls, (root / (std::string(name) + ".key")).c_str(), nullptr), "probe key load");
    Socket socket = TcpSocket_create();
    Socket_setConnectTimeout(socket, 3000);
    check(Socket_connect(socket, "127.0.0.1", port), "probe TCP connection failed");
    TLSSocket secure = TLSSocket_create(socket, tls, true);
    const bool accepted = secure != nullptr;
    if (secure) TLSSocket_close(secure);
    Socket_destroy(socket);
    TLSConfiguration_destroy(tls);
    return accepted;
}
cci::core::MmsConfig configuration(const std::filesystem::path& root, bool profile) {
    cci::core::MmsConfig cfg;
    cfg.enabled = true; cfg.bind_address = "127.0.0.1"; cfg.tcp_port = free_port();
    cfg.gnss_poll_s = 0; cfg.chrony_poll = false; cfg.comms_loss_fallback_s = 0;
    cfg.model_cfg_path = (root / "model.cfg").string();
    cfg.tls.enabled = true; cfg.tls.chain_validation = true;
    cfg.tls.own_cert = (root / "server.pem").string(); cfg.tls.own_key = (root / "server.key").string();
    cfg.tls.ca_cert = (root / "ca.pem").string();
    cfg.tls.crl_path = (root / "base.crl.pem").string();
    cfg.tls.client_cert = (root / (profile ? "unified_dso.pem" : "transport.pem")).string();
    cfg.tls.acse_client_cert = (root / (profile ? "owner_rev0.pem" : "dso.pem")).string();
    cfg.tls.viewer_cert = (root / "unified_viewer.pem").string();
    cfg.rbac_profile_a = profile;
    cfg.rbac_policy_path = (root / "package2/commissioning-policy.xml").string();
    cfg.rbac_owner_cert = (root / "owner.pem").string();
    cfg.rbac_activation_state = (root / "integration.state").string();
    cfg.rbac_identity_crl = (root / "integration.crl.pem").string();
    return cfg;
}
} // namespace

int main(int argc, char** argv) {
    try {
        check(argc == 2, "fixture directory argument required");
        const std::filesystem::path base(argv[1]);
        const auto root = base / read(base / "fixture-directory.txt");
        std::mutex mutex;
        std::vector<std::string> audit;
        const auto logger = [&](const std::string& event) {
            std::lock_guard<std::mutex> lock(mutex);
            audit.push_back(event);
            if (event.find("ASSOCIATION_") != std::string::npos || event.find("MMS_ACCESS_DENIED") != std::string::npos)
                std::cerr << event << '\n';
        };
        struct TlsCase { const char* client; const char* pin; const char* crl; bool chain; bool accepted; bool starts{true}; };
        const TlsCase cases[] = {
            {"transport", "transport", "base.crl.pem", true, true},
            {"revoked", "revoked", "base.crl.pem", true, false},
            {"expired", "expired", "base.crl.pem", true, false},
            {"wrong_issuer", "wrong_issuer", "base.crl.pem", true, false},
            {"unconfigured", "transport", "base.crl.pem", true, false},
            {"transport", "transport", "stale.crl.pem", true, false},
            {"transport", "transport", "bad_signature.crl.pem", true, false, false},
            {"wrong_issuer", "wrong_issuer", "base.crl.pem", false, false, false},
            {"revoked", "revoked", "base.crl.pem", false, false, false},
            {"expired", "expired", "base.crl.pem", false, false, false}
        };
        for (const auto& item : cases) {
            auto cfg = configuration(root, false);
            cfg.tls.chain_validation = item.chain;
            cfg.tls.client_cert = (root / (std::string(item.pin) + ".pem")).string();
            cfg.tls.crl_path = (root / item.crl).string();
            cci::adapters::MmsAdapter server;
            if (!item.starts) {
                check(!server.start(cfg, logger), "Invalid TLS policy must fail before opening listener");
                continue;
            }
            check(server.start(cfg, logger), "TLS verifier test server startup");
            check(tls_connects(root, item.client, cfg.tcp_port) == item.accepted,
                  std::string("TLS handshake result: ") + item.client + " / " + item.crl + (item.chain ? " chain" : " pinned-only"));
            server.stop();
        }
        {
            auto cfg = configuration(root, false);
            cfg.tls.crl_path = (root / "recover.crl.pem").string();
            std::filesystem::copy_file(root / "stale.crl.pem", cfg.tls.crl_path);
            const auto first_event = audit.size();
            cci::adapters::MmsAdapter server;
            check(server.start(cfg, logger), "Expired CRL must not block MMS listener startup");
            check(!tls_connects(root, "transport", cfg.tcp_port),
                  "New authentication must reject expired issuer CRL data");
            std::filesystem::copy_file(root / "base.crl.pem", root / "recover.next.pem");
            std::filesystem::rename(root / "recover.next.pem", cfg.tls.crl_path);
            check(tls_connects(root, "transport", cfg.tcp_port),
                  "Fresh CRL must restore authentication without restarting MMS");
            server.stop();
            check(std::any_of(audit.begin() + static_cast<std::ptrdiff_t>(first_event), audit.end(),
                [](const std::string& event) {
                    return event.find("warning: CRL expired") != std::string::npos &&
                           event.find("\"severity\":\"warning\"") != std::string::npos;
                }), "Expired startup CRL warning missing");
        }
        for (const bool missing_certificate : {true, false}) {
            auto cfg = configuration(root, false);
            if (missing_certificate)
                cfg.tls.own_cert = (root / "missing-endpoint.pem").string();
            else
                cfg.tls.own_key = (root / "missing-endpoint.key").string();
            const auto first_event = audit.size();
            cci::adapters::MmsAdapter server;
            check(!server.start(cfg, logger), "MMS must reject an unavailable local TLS identity");
            const bool alarm = std::any_of(audit.begin() + static_cast<std::ptrdiff_t>(first_event), audit.end(),
                [](const std::string& event) {
                    return event.find("TLS_SECURITY_EVENT") != std::string::npos &&
                           event.find("alarm: endpoint certificate unavailable") != std::string::npos;
                });
            check(alarm, "Unavailable local MMS TLS certificate/key alarm missing");
        }
        {
            auto cfg = configuration(root, false);
            cci::adapters::MmsAdapter server;
            check(server.start(cfg, logger), "legacy TLS server failed to start");
            {
                // A pre-RBAC-style transport certificate has no role extension.
                Client client(root, "transport", cfg.tcp_port);
                check(client.readable("CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX), "legacy TLS read regressed");
            }
            server.stop();
        }
        {
            auto cfg = configuration(root, true);
            cci::adapters::MmsAdapter server;
            check(server.start(cfg, logger), "Profile A TLS server failed to start");
            {
                Client dso(root, "unified_dso", cfg.tcp_port);
                check(dso.readable("CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX), "DSO read denied");
                check(!dso.readable("CCI016_01LD_Plant/LLN0.NamPlt.vendor", IEC61850_FC_DC), "DSO read escaped reserved objects");
                IedClientError error;
                IedConnection_writeInt32Value(dso.connection, &error, "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel", IEC61850_FC_CF, 3);
                check(error == IED_ERROR_OK, "DSO CONFIG on reserved RW object denied");
                // Annex T Table 101: exercise the discovery/definition and value
                // service paths over the actual MMS stack, including denials.
                auto names = IedConnection_getServerDirectory(dso.connection, &error, false);
                check(error == IED_ERROR_OK && names && contains(names, "CCI016_01LD_Plant"), "GetServerDirectory denied");
                LinkedList_destroy(names);
                names = IedConnection_getLogicalDeviceDirectory(dso.connection, &error, "CCI016_01LD_Plant");
                check(error == IED_ERROR_OK && names && contains(names, "WlimDWMX1") &&
                      !contains(names, "WSaDAGC1") && !contains(names, "PdCiMMXU1"), "LD directory leaked another actor's LN");
                LinkedList_destroy(names);
                names = IedConnection_getLogicalNodeDirectory(dso.connection, &error, "CCI016_01LD_Plant/WlimDWMX1", ACSI_CLASS_DATA_OBJECT);
                check(error == IED_ERROR_OK && names && contains(names, "Mod"), "GetLogicalNodeDirectory denied");
                LinkedList_destroy(names);
                names = IedConnection_getDataDirectory(dso.connection, &error, "CCI016_01LD_Plant/WlimDWMX1.Mod");
                check(error == IED_ERROR_OK && names, "GetDataDirectory denied");
                LinkedList_destroy(names);
                names = IedConnection_getDataDirectory(dso.connection, &error, "CCI016_01LD_Plant/LLN0.NamPlt");
                const bool hidden_directory = error != IED_ERROR_OK || !names || LinkedList_size(names) == 0;
                if (names) LinkedList_destroy(names);
                check(hidden_directory, "GetDataDirectory exposed an unauthorized object");
                auto specification = IedConnection_getVariableSpecification(dso.connection, &error,
                    "CCI016_01LD_Plant/WlimDWMX1.Mod.stVal", IEC61850_FC_ST);
                check(error == IED_ERROR_OK && specification, "GetDataDefinition denied");
                MmsVariableSpecification_destroy(specification);
                specification = IedConnection_getVariableSpecification(dso.connection, &error,
                    "CCI016_01LD_Plant/LLN0.NamPlt.vendor", IEC61850_FC_DC);
                if (specification) MmsVariableSpecification_destroy(specification);
                check(error != IED_ERROR_OK, "GetDataDefinition exposed an unauthorized object");
                check(!dso.readable("CCI016_01LD_Plant/LLN0", IEC61850_FC_DC), "GetAllDataValues bypassed per-object checks");
                check(dso.readable("CCI016_01LD_Plant/LPHD1.PhyNam.vendor", IEC61850_FC_DC), "reserved RO object not readable");
                char forbidden[] = "forbidden";
                IedConnection_writeVisibleStringValue(dso.connection, &error, "CCI016_01LD_Plant/LPHD1.PhyNam.vendor", IEC61850_FC_DC, forbidden);
                check(error != IED_ERROR_OK, "DSO wrote a reserved RO object");
                auto directory = IedConnection_getDataSetDirectory(dso.connection, &error, "CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec", nullptr);
                check(error == IED_ERROR_OK && directory, "safe dataset directory denied");
                LinkedList_destroy(directory);
                auto values = IedConnection_readDataSetValues(dso.connection, &error,
                    "CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec", nullptr);
                check(error == IED_ERROR_OK && values, "GetDataSetValues denied");
                ClientDataSet_destroy(values);
                values = IedConnection_readDataSetValues(dso.connection, &error,
                    "CCI016_01LD_Plant/LLN0.DS_R_Evil", nullptr);
                if (values) ClientDataSet_destroy(values);
                check(error != IED_ERROR_OK, "GetDataSetValues exposed an unauthorized member");
                directory = IedConnection_getDataSetDirectory(dso.connection, &error, "CCI016_01LD_Plant/LLN0.DS_R_Evil", nullptr);
                if (directory) LinkedList_destroy(directory);
                check(error != IED_ERROR_OK, "dataset exposed an unauthorized member");
                auto bad_rcb = IedConnection_getRCBValues(dso.connection, &error, "CCI016_01LD_Plant/LLN0.RP.urcb_Evil", nullptr);
                if (bad_rcb) ClientReportControlBlock_destroy(bad_rcb);
                check(error != IED_ERROR_OK, "report container exposed an unauthorized member");
                Client viewer(root, "unified_viewer", cfg.tcp_port);
                check(viewer.readable("CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX), "VIEWER process read denied");
                check(!viewer.readable("CCI016_01LD_Plant/LLN0.NamPlt.vendor", IEC61850_FC_DC), "VIEWER default scope exposes configuration metadata");
                IedConnection_writeInt32Value(viewer.connection, &error, "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel", IEC61850_FC_CF, 3);
                check(error != IED_ERROR_OK, "VIEWER CONFIG allowed");
                Client engineer(root, "owner_rev0", cfg.tcp_port);
                check(engineer.readable("CCI016_01LD_Plant/LLN0.NamPlt.vendor", IEC61850_FC_DC), "revision 0 ENGINEER read denied");
                {
                    Client release_client(root, "unified_viewer", cfg.tcp_port);
                    IedConnection_release(release_client.connection, &error);
                    check(error == IED_ERROR_OK, "authorized association Release failed");
                }
                {
                    Client abort_client(root, "unified_viewer", cfg.tcp_port);
                    IedConnection_abort(abort_client.connection, &error);
                    check(error == IED_ERROR_OK, "authorized association Abort failed");
                }
                const char* ref = "CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec01";
                auto rcb = IedConnection_getRCBValues(dso.connection, &error, ref, nullptr);
                check(error == IED_ERROR_OK && rcb, "DSO RCB read denied");
                std::atomic<unsigned> reports{0};
                IedConnection_installReportHandler(dso.connection, ref, ClientReportControlBlock_getRptId(rcb), report, &reports);
                ClientReportControlBlock_setRptEna(rcb, true);
                IedConnection_setRCBValues(dso.connection, &error, rcb, RCB_ELEMENT_RPT_ENA, true);
                check(error == IED_ERROR_OK, "DSO report enable denied");
                for (unsigned i = 0; i < 60 && reports.load() == 0; ++i) {
                    server.update_tot_w_kw(12.0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                check(reports.load() > 0, "authorized report did not arrive");
                std::filesystem::copy_file(root / "revoked_unified.crl.pem", root / "integration.crl.pem", std::filesystem::copy_options::overwrite_existing);
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                const auto before = reports.load();
                std::this_thread::sleep_for(std::chrono::milliseconds(4500));
                check(reports.load() == before, "report delivery continued after identity revocation");
                check(!dso.readable("CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX), "revoked live session still reads values");
                IedConnection_uninstallReportHandler(dso.connection, ref);
                ClientReportControlBlock_destroy(rcb);
            }
            server.stop();
        }

        for (const bool generated : {false, true}) {
            std::filesystem::copy_file(root / "base.crl.pem", root / "integration.crl.pem", std::filesystem::copy_options::overwrite_existing);
            auto cfg = configuration(root, true);
            cfg.model_cfg_path = generated ? (root / "aggregator-model.cfg").string() : "";
            cfg.rbac_policy_path = (root / "package_config_only/commissioning-policy.xml").string();
            cfg.rbac_activation_state = (root / "config-only.state").string();
            cfg.setpoint_min_interval_s = 0;
            cfg.tls.allowed_client_certs.push_back((root / "unified_aggregator.pem").string());
            cci::adapters::MmsAdapter server;
            const auto first_config_event = audit.size();
            check(server.start(cfg, logger), "C3 CONFIG-only model failed to start");
            {
                Client agg(root, "unified_aggregator", cfg.tcp_port);
                Client engineer(root, "owner_rev0", cfg.tcp_port);
                const std::string plant = "CCI016_01LD_Plant/";
                for (const char* name : {"WSaDAGC1.Mod", "VArSaDVAR1.Mod"}) {
                    check(control_request(agg, plant + name, -1, true) &&
                          control_request(agg, plant + name, -5, true), "C3 CONFIG mode operation denied");
                    check(!control_request(agg, plant + name, 1, true), "CONFIG bypassed mode validation");
                    check(!control_request(engineer, plant + name, -1, true), "standard CONFIG became CONTROL");
                }
                for (const char* name : {"WSaDAGC1.WSptPct", "VArSaDVAR1.VArTgtSptPct"}) {
                    check(control_request(agg, plant + name, -100, false) &&
                          control_request(agg, plant + name, 100, false), "C3 CONFIG setpoint operation denied");
                    check(!control_request(agg, plant + name, 101, false) &&
                          !control_request(agg, plant + name, std::numeric_limits<float>::quiet_NaN(), false),
                          "CONFIG bypassed signed setpoint validation");
                    IedClientError error;
                    IedConnection_writeInt32Value(agg.connection, &error, (plant + name + ".ctlModel").c_str(), IEC61850_FC_CF, 3);
                    check(error != IED_ERROR_OK, "C3 CONFIG exposed control-model configuration");
                }
                check(!control_request(agg, plant + "WlimDWMX1.Mod", 5, true), "C3 CONFIG crossed actor boundary");
            }
            server.stop();
            check(std::any_of(audit.begin() + static_cast<std::ptrdiff_t>(first_config_event), audit.end(),
                [](const std::string& event) {
                    return event.find("MMS_ACCESS_OK") != std::string::npos &&
                           event.find("authorized via allocated C3 CONFIG ctlVal") != std::string::npos;
                }), "C3 CONFIG authorization route missing from audit");
        }
        std::cout << "Annex T C3 CONFIG-only controls and validation on both models: PASS\n";
        for (const bool generated : {false, true}) {
            auto cfg = configuration(root, false);
            cfg.model_cfg_path = generated ? (root / "aggregator-model.cfg").string() : "";
            cci::adapters::MmsAdapter server;
            check(server.start(cfg, logger), "Aggregator 7-420 model completeness server failed to start");
            {
                Client client(root, "transport", cfg.tcp_port);
                IedClientError error;
                const std::string plant = "CCI016_01LD_Plant/";
                for (const auto* ln : {"WSaDAGC1", "VArSaDVAR1"}) {
                    check(client.readable((plant+ln+".NamPlt.vendor").c_str(), IEC61850_FC_DC),
                          "Aggregator LN nameplate is missing");
                    auto* namespace_value = IedConnection_readObject(
                        client.connection, &error, (plant+ln+".NamPlt.lnNs").c_str(), IEC61850_FC_EX);
                    check(error == IED_ERROR_OK && namespace_value &&
                          MmsValue_getType(namespace_value) == MMS_VISIBLE_STRING &&
                          std::string(MmsValue_toString(namespace_value)) ==
                              "(Tr)IEC 61850-CEI57-142:2026A",
                          "Aggregator LN namespace nameplate does not identify CEI 57-142");
                    MmsValue_delete(namespace_value);
                    check(client.readable((plant+ln+".RmpRteUse.setVal").c_str(), IEC61850_FC_SP),
                          "Aggregator 7-420 RmpRteUse object is missing");
                    check(client.readable((plant+ln+".InEcpRef.setSrcRef").c_str(), IEC61850_FC_SP),
                          "Aggregator 7-420 InEcpRef object is missing");
                }
                check(client.readable((plant+"WSaDAGC1.ReqW.mag.f").c_str(), IEC61850_FC_MX),
                      "DAGC1 ReqW object is missing");
                check(client.readable((plant+"VArSaDVAR1.ReqVAr.mag.f").c_str(), IEC61850_FC_MX),
                      "DVAR1 ReqVAr object is missing");
            }
            server.stop();
        }

        for (const bool generated : {false, true}) {
            // Real TLS/MMS on loopback, synthetic values, no controller or plant I/O.
            std::filesystem::copy_file(root / "base.crl.pem", root / "integration.crl.pem", std::filesystem::copy_options::overwrite_existing);
            auto cfg = configuration(root, true);
            cfg.model_cfg_path = generated ? (root / "aggregator-model.cfg").string() : "";
            cfg.setpoint_min_interval_s = 0;
            cfg.tls.allowed_client_certs.push_back((root / "unified_aggregator.pem").string());
            cci::adapters::MmsAdapter server;
            check(!cfg.aggregator_plant_outputs_enabled, "Aggregator outputs are not disabled by default");
            check(server.start(cfg, logger), "Aggregator model failed to start");
            {
                Client agg(root, "unified_aggregator", cfg.tcp_port);
                Client dso(root, "unified_dso", cfg.tcp_port);
                const std::string plant = "CCI016_01LD_Plant/";
                IedClientError error;
                server.update_tot_w_kw(-12.5);
                server.update_totvar_kvar(7.25);
                auto read_quality = [&](const char* name) {
                    auto* value = IedConnection_readObject(agg.connection, &error,
                        (plant+name).c_str(), IEC61850_FC_MX);
                    check(error == IED_ERROR_OK && value && MmsValue_getType(value) == MMS_BIT_STRING,
                          "Aggregator measurement quality is not readable");
                    const Quality result = Quality_fromMmsValue(value);
                    MmsValue_delete(value);
                    return result;
                };
                auto read_float = [&](const char* name) {
                    auto* value = IedConnection_readObject(agg.connection, &error, (plant+name).c_str(), IEC61850_FC_MX);
                    check(error == IED_ERROR_OK && value && MmsValue_getType(value)==MMS_FLOAT, "Aggregator measurement not readable");
                    const float result=MmsValue_toFloat(value); MmsValue_delete(value); return result;
                };
                check(read_float("PdCiMMXU1.TotW.mag.f")==-12.5f && read_float("PdCiMMXU1.TotVAr.mag.f")==7.25f,
                      "Aggregator values do not mirror signed POC measurements");
                for (const char* measurement : {"PdCiMMXU1.TotW.q", "PdCiMMXU1.TotVAr.q"}) {
                    Quality quality = read_quality(measurement);
                    check(Quality_getValidity(&quality) == QUALITY_VALIDITY_INVALID &&
                          Quality_isFlagSet(&quality, QUALITY_DETAIL_FAILURE),
                          "Aggregator started with invalid measurement quality unmarked");
                }
                server.update_aggregator_measurement_quality(cci::core::DataQuality::Good);
                for (const char* measurement : {"PdCiMMXU1.TotW.q", "PdCiMMXU1.TotVAr.q"}) {
                    Quality quality = read_quality(measurement);
                    check(Quality_getValidity(&quality) == QUALITY_VALIDITY_GOOD,
                          "good Aggregator measurements were not marked good");
                }
                server.update_aggregator_measurement_quality(cci::core::DataQuality::Stale);
                for (const char* measurement : {"PdCiMMXU1.TotW.q", "PdCiMMXU1.TotVAr.q"}) {
                    Quality quality = read_quality(measurement);
                    check(Quality_getValidity(&quality) == QUALITY_VALIDITY_QUESTIONABLE &&
                          Quality_isFlagSet(&quality, QUALITY_DETAIL_OLD_DATA),
                          "stale Aggregator measurements were not marked old/questionable");
                }
                server.update_aggregator_measurement_quality(cci::core::DataQuality::Invalid);
                for (const char* measurement : {"PdCiMMXU1.TotW.q", "PdCiMMXU1.TotVAr.q"}) {
                    Quality quality = read_quality(measurement);
                    check(Quality_getValidity(&quality) == QUALITY_VALIDITY_INVALID &&
                          Quality_isFlagSet(&quality, QUALITY_DETAIL_FAILURE),
                          "invalid Aggregator measurements were not marked failed");
                }
                server.update_aggregator_measurement_quality(cci::core::DataQuality::Good);
                check(!dso.readable((plant+"PdCiMMXU1.TotW.mag.f").c_str(), IEC61850_FC_MX), "DSO escaped into Aggregator measurements");
                check(!agg.readable((plant+"PdCMMXU1.TotW.mag.f").c_str(), IEC61850_FC_MX), "Aggregator escaped into DSO measurements");
                for (const auto* ln : {"WSaDAGC1", "VArSaDVAR1"}) {
                    for (const auto* status : {"Beh.stVal", "Health.stVal", "FctOpSt.stVal"})
                        check(agg.readable((plant+ln+"."+status).c_str(), IEC61850_FC_ST), "Aggregator status is absent");
                    IedConnection_writeInt32Value(agg.connection, &error, (plant+ln+".Beh.stVal").c_str(), IEC61850_FC_ST, -1);
                    check(error != IED_ERROR_OK, "Read-only Beh writable");
                }
                auto operate = [&](Client& client, const char* name, float value, bool integer) {
                    return control_request(client, plant + name, value, integer);
                };
                for (const char* mode : {"WSaDAGC1.Mod", "VArSaDVAR1.Mod"}) {
                    IedConnection_writeInt32Value(agg.connection,&error,(plant+mode+".ctlModel").c_str(),IEC61850_FC_CF,3);
                    check(error!=IED_ERROR_OK,"Aggregator changed a mode control model outside its allocated ctlVal access");
                    for (const int value : {-1,-5}) {
                        check(operate(agg,mode,value,true), "CEI negative activation value rejected");
                        const auto observed=IedConnection_readInt32Value(agg.connection,&error,(plant+mode+".stVal").c_str(),IEC61850_FC_ST);
                        check(error==IED_ERROR_OK && observed==value,"Aggregator mode readback differs from accepted request");
                    }
                    check(!operate(agg,mode,1,true) && !operate(agg,mode,5,true), "Old positive activation values silently accepted");
                    check(!operate(dso,mode,-1,true), "DSO controlled Aggregator mode");
                }
                for (const char* target : {"WSaDAGC1.WSptPct", "VArSaDVAR1.VArTgtSptPct"}) {
                    IedConnection_writeInt32Value(agg.connection,&error,(plant+target+".ctlModel").c_str(),IEC61850_FC_CF,3);
                    check(error!=IED_ERROR_OK,"Aggregator changed the control model outside its allocated ctlVal access");
                    for (float value : {-100.f,-25.f,0.f,25.f,100.f}) {
                        check(operate(agg,target,value,false), "Signed Aggregator target rejected");
                        check(read_float((std::string(target)+".mxVal.f").c_str())==value,"Signed Aggregator target readback changed sign/value");
                    }
                    check(!operate(agg,target,-101,false) && !operate(agg,target,101,false), "Out-of-range Aggregator target accepted");
                    check(!operate(agg,target,std::numeric_limits<float>::quiet_NaN(),false), "NaN Aggregator target accepted");
                    check(!operate(dso,target,10,false), "DSO controlled Aggregator target");
                }
                IedConnection_writeFloatValue(agg.connection,&error,(plant+"PdCiMMXU1.TotW.mag.f").c_str(),IEC61850_FC_MX,99);
                check(error!=IED_ERROR_OK,"Aggregator measurement is writable");
                cci::adapters::DsoLiveCommand request;
                check(server.poll_dso_live_command(request), "Aggregator request not delivered to memory");
                check(!request.for_plant(false).wsa_active && !request.for_plant(false).varsa_active,
                      "Aggregator request reaches plant while disabled");
                const char* dataset="CCI016_01LD_Plant/LLN0.DS_R_Aggregator_Mis4sec";
                auto directory=IedConnection_getDataSetDirectory(agg.connection,&error,dataset,nullptr);
                check(error==IED_ERROR_OK && directory && LinkedList_size(directory)==2, "Aggregator dataset absent/wrong members");
                LinkedList_destroy(directory);
                directory=IedConnection_getDataSetDirectory(dso.connection,&error,dataset,nullptr);
                check(error!=IED_ERROR_OK, "DSO read Aggregator dataset");
                if(directory) LinkedList_destroy(directory);
                IedConnection_deleteDataSet(agg.connection,&error,dataset);
                check(error!=IED_ERROR_OK, "Aggregator deleted read-only dataset");
                Client agg2(root, "unified_aggregator", cfg.tcp_port);
                for (const char* base : {"CCI016_01LD_Plant/LLN0.RP.urcb_Aggregator_Mis4sec",
                                         "CCI016_01LD_Plant/LLN0.BR.brcb_Aggregator_Mis4sec"}) {
                    const std::string ref01 = std::string(base) + "01";
                    const std::string ref02 = std::string(base) + "02";
                    auto rcb1=IedConnection_getRCBValues(agg.connection,&error,ref01.c_str(),nullptr);
                    check(error==IED_ERROR_OK && rcb1, "Aggregator report instance 01 missing or denied");
                    auto rcb2=IedConnection_getRCBValues(agg2.connection,&error,ref02.c_str(),nullptr);
                    check(error==IED_ERROR_OK && rcb2, "Aggregator report instance 02 missing or denied");
                    check(ClientReportControlBlock_getIntgPd(rcb1)==4000 &&
                          ClientReportControlBlock_getIntgPd(rcb2)==4000,
                          "Aggregator report integrity period is not four seconds");
                    std::atomic<unsigned> reports1{0}, reports2{0};
                    IedConnection_installReportHandler(agg.connection,ref01.c_str(),ClientReportControlBlock_getRptId(rcb1),report,&reports1);
                    IedConnection_installReportHandler(agg2.connection,ref02.c_str(),ClientReportControlBlock_getRptId(rcb2),report,&reports2);
                    ClientReportControlBlock_setRptEna(rcb1,true);
                    IedConnection_setRCBValues(agg.connection,&error,rcb1,RCB_ELEMENT_RPT_ENA,true);
                    check(error==IED_ERROR_OK, "Aggregator report instance 01 enable denied");
                    ClientReportControlBlock_setRptEna(rcb2,true);
                    IedConnection_setRCBValues(agg2.connection,&error,rcb2,RCB_ELEMENT_RPT_ENA,true);
                    check(error==IED_ERROR_OK, "second Aggregator client could not enable the same report");
                    for(unsigned i=0;i<60 && (reports1.load()==0 || reports2.load()==0);++i) {
                        server.update_tot_w_kw(10.+i); std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                    check(reports1.load()>0 && reports2.load()>0,
                          "one of the concurrent Aggregator report instances delivered no payload");
                    ClientReportControlBlock_setIntgPd(rcb1,100);
                    IedConnection_setRCBValues(agg.connection,&error,rcb1,RCB_ELEMENT_INTG_PD,true);
                    check(error!=IED_ERROR_OK, "Aggregator changed a report configuration field");
                    ClientReportControlBlock_setRptEna(rcb1,false);
                    IedConnection_setRCBValues(agg.connection,&error,rcb1,RCB_ELEMENT_RPT_ENA,true);
                    check(error==IED_ERROR_OK, "Aggregator report instance 01 disable denied");
                    ClientReportControlBlock_setRptEna(rcb2,false);
                    IedConnection_setRCBValues(agg2.connection,&error,rcb2,RCB_ELEMENT_RPT_ENA,true);
                    check(error==IED_ERROR_OK, "Aggregator report instance 02 disable denied");
                    IedConnection_uninstallReportHandler(agg.connection,ref01.c_str());
                    IedConnection_uninstallReportHandler(agg2.connection,ref02.c_str());
                    ClientReportControlBlock_destroy(rcb1);
                    ClientReportControlBlock_destroy(rcb2);
                    rcb1=IedConnection_getRCBValues(dso.connection,&error,ref01.c_str(),nullptr);
                    check(error!=IED_ERROR_OK, "DSO read Aggregator report instance 01");
                    if(rcb1) ClientReportControlBlock_destroy(rcb1);
                    rcb2=IedConnection_getRCBValues(dso.connection,&error,ref02.c_str(),nullptr);
                    check(error!=IED_ERROR_OK, "DSO read Aggregator report instance 02");
                    if(rcb2) ClientReportControlBlock_destroy(rcb2);
                }
            }
            server.stop();
            cfg.aggregator_plant_outputs_enabled=true;
            check(!server.start(cfg,logger), "Unvalidated Aggregator plant output backend enabled");
            std::cout << "Aggregator CEI model, signed controls, measurements, statuses, datasets, concurrent buffered/unbuffered reports and actor boundaries: PASS\n";
        }

        bool denied = false, authenticated = false;
        for (const auto& event : audit) {
            denied |= event.find("REPORT_ACCESS_DENIED") != std::string::npos;
            authenticated |= event.find("ASSOCIATION_AUTH_OK") != std::string::npos && event.find("\"serial\":") != std::string::npos;
        }
        check(denied && authenticated, "structured authentication/denial audit missing");
        std::cout << "mms_rbac_tls_test: PASS (legacy TLS, role TLS, reads/writes, dataset/report scope, revision 0, live revocation)\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "mms_rbac_tls_test: FAIL: " << exception.what() << '\n';
        return 1;
    }
}
