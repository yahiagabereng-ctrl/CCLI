#include "e2e_server.hpp"
#include "e2e_wire.hpp"
#include "mms_adapter.hpp"
extern "C" {
#include "iec61850_server.h"
#include "iec61850_dynamic_model.h"
#include "tls_config.h"
}
#include <atomic>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstring>
#include "e2e_allocation_fault.hpp"

using namespace ccli::mms::e2e;
namespace {
std::atomic<long> boundary_size{-1}, allocation_position{-1};
std::atomic<unsigned> allocation_hits{0};
std::atomic<unsigned>* test_reads = nullptr;
thread_local int (*original_dispatch)(void*, const uint8_t*, int, uint8_t*, int) = nullptr;
int boundary_dispatch(void* connection, const uint8_t* request, int size, uint8_t* response, int capacity) {
    static const std::string marker = "Boundary";
    const auto desired = boundary_size.load();
    if (desired < 0 || std::search(request, request + size, marker.begin(), marker.end()) == request + size)
        return original_dispatch(connection, request, size, response, capacity);
    ++*test_reads;
    if (desired > capacity) return capacity + 1;
    const auto input = wire::fields({request, static_cast<std::size_t>(size)}, 0xa0);
    wire::Bytes encoded;
    std::size_t payload = static_cast<std::size_t>(desired);
    for (unsigned attempt = 0; attempt < 8; ++attempt) {
        const auto data = wire::tlv(0x89, wire::view(wire::Bytes(payload, 0x5a)));
        const auto results = wire::tlv(0xa1, wire::view(data));
        const auto read_result = wire::tlv(0xa4, wire::view(results));
        encoded = wire::tlv(0xa1, wire::view(wire::join({input.at(0).encoded, wire::view(read_result)})));
        if (encoded.size() == static_cast<std::size_t>(desired)) break;
        const auto overhead = encoded.size() - payload;
        if (static_cast<std::size_t>(desired) < overhead) return -1;
        payload = static_cast<std::size_t>(desired) - overhead;
    }
    if (encoded.size() != static_cast<std::size_t>(desired)) return -1;
    std::memcpy(response, encoded.data(), encoded.size());
    return static_cast<int>(encoded.size());
}
// Test-only application callback and allocator scopes. Crypto and native
// dispatch/protection use the real engine; no fault option exists in firmware.
int campaign_handler(void* owner, void** session, int event, const CcliE2eIo* source,
    const uint8_t* input, int size, uint8_t* output, int capacity, CcliE2eReply* reply) {
    if (!source || !reply || size < 0 || capacity < 0) return -1;
    auto io = *source; original_dispatch = io.dispatch; io.dispatch = boundary_dispatch;
    const long fault = event == CCLI_E2E_RECEIVE || event == CCLI_E2E_PRESENTATION
        ? allocation_position.exchange(-1) : -1;
    e2e_test_alloc::Scope allocation(fault);
    int result = -1;
    try {
        wire::Bytes bytes;
        result = static_cast<NativeServer*>(owner)->process(session, event, io,
            {input, static_cast<std::size_t>(size)}, bytes, *reply);
        if (bytes.size() > static_cast<std::size_t>(capacity)) result = -1;
        else {
            if (!bytes.empty()) std::memcpy(output, bytes.data(), bytes.size());
            reply->size = static_cast<int>(bytes.size());
        }
    } catch (...) { result = -1; }
    if (fault >= 0) allocation_hits.store(e2e_test_alloc::failures);
    return result;
}
bool legacy_reject(void*, AcseAuthenticationParameter, void**, IsoApplicationReference*) { return false; }
MmsDataAccessError read_access(LogicalDevice*, LogicalNode*, DataObject* object, FunctionalConstraint,
                              ClientConnection connection, void* parameter) {
    ++*static_cast<std::atomic<unsigned>*>(parameter);
    if (std::strcmp(object->name, "Abort") == 0) {
        const std::uint8_t diagnostic[]{0xa4, 3, 0x85, 1, 1};
        if (!CcliE2e_requestApplicationAbort(ClientConnection_getSecurityToken(connection), diagnostic, sizeof(diagnostic)))
            return DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
    }
    return DATA_ACCESS_ERROR_SUCCESS;
}
wire::Bytes read(const std::string& file) {
    std::ifstream input(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
int product(const std::string& root, int port) {
    for (unsigned index = 0; index < 5; ++index)
        if (cci::core::load_config(root + "/invalid-" + std::to_string(index) + ".yaml").mms.e2e.configuration_valid)
            return 8;
    const auto parsed = cci::core::load_config(root + "/valid-e2e.yaml").mms.e2e;
    if (!parsed.configuration_valid || !parsed.enabled || parsed.allow_compatibility ||
        parsed.ae_qualifier != 7 || parsed.rekey_seconds != 900) return 8;
    cci::core::MmsConfig cfg;
    cfg.enabled = true; cfg.bind_address = "127.0.0.1"; cfg.tcp_port = port;
    cfg.gnss_poll_s = 0; cfg.chrony_poll = false;
    cfg.tls.enabled = true; cfg.tls.chain_validation = true;
    cfg.tls.own_cert = root + "/tls_server.pem"; cfg.tls.own_key = root + "/tls_server.key";
    cfg.tls.ca_cert = root + "/root.pem"; cfg.tls.client_cert = root + "/tls_client.pem";
    cfg.tls.crl_path = root + "/root.crl.der";
    cfg.rbac_profile_a = true; cfg.rbac_policy_path = root + "/policy/commissioning-policy.xml";
    cfg.rbac_owner_cert = root + "/owner.pem"; cfg.rbac_activation_state = root + "/product.state";
    cfg.rbac_identity_ca = root + "/root.pem"; cfg.rbac_identity_crl = root + "/root.crl.der";
    cfg.e2e.enabled = true; cfg.e2e.certificate = root + "/server.der";
    cfg.e2e.private_key = root + "/server.key"; cfg.e2e.trust_anchors = root + "/root.pem";
    cfg.e2e.crls = root + "/root.crl.der"; cfg.e2e.ap_title = "1.3.6.1.4.1.55555.62351.4.2";
    cfg.e2e.ae_qualifier = 1;
    cci::adapters::MmsAdapter server;
    if (!server.start(cfg, [](const std::string& value) { std::cerr << value << '\n'; })) return 7;
    std::cout << "READY\n" << std::flush;
    std::string command; std::getline(std::cin, command);
    server.stop(); std::cout << "PRODUCT_DONE\n"; return 0;
}
}
int main(int argc, char** argv) {
    if (argc < 3) return 2;
    const std::string root = argv[1];
    if (argc > 3 && std::string(argv[3]) == "product") return product(root, std::stoi(argv[2]));
    const bool ec = argc > 3 && std::string(argv[3]) == "ec";
    const bool resilience = argc > 3 && std::string(argv[argc - 1]) == "resilience";
    auto tls = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tls, true);
    if (!TLSConfiguration_addCACertificateFromFile(tls, (root + "/root.pem").c_str()) ||
        !TLSConfiguration_setOwnCertificateFromFile(tls, (root + "/tls_server.pem").c_str()) ||
        !TLSConfiguration_setOwnKeyFromFile(tls, (root + "/tls_server.key").c_str(), nullptr)) return 3;
    auto* model = IedModel_create("E2E");
    auto* ld = LogicalDevice_create("LD0", model);
    auto* ln = LogicalNode_create("LLN0", ld);
    auto* object = DataObject_create("Health", reinterpret_cast<ModelNode*>(ln), 0);
    auto* value = DataAttribute_create("stVal", reinterpret_cast<ModelNode*>(object),
        IEC61850_INT32, IEC61850_FC_ST, TRG_OPT_DATA_CHANGED, 0, 0);
    auto* abort_object = DataObject_create("Abort", reinterpret_cast<ModelNode*>(ln), 0);
    DataAttribute_create("stVal", reinterpret_cast<ModelNode*>(abort_object),
        IEC61850_INT32, IEC61850_FC_ST, 0, 0, 0);
    auto* dataset = DataSet_create("Events", ln);
    DataSetEntry_create(dataset, "LLN0$ST$Health$stVal", -1, nullptr);
    ReportControlBlock_create("Events", ln, "E2ELD0/LLN0.Events", false, "Events", 1,
        TRG_OPT_INTEGRITY, RPT_OPT_SEQ_NUM | RPT_OPT_TIME_STAMP | RPT_OPT_DATA_SET, 0, 100);
    auto config = IedServerConfig_create();
    auto server = IedServer_createWithConfig(model, tls, config);
    IedServerConfig_destroy(config);
    IedServer_setLocalIpAddress(server, "127.0.0.1");
    IedServer_updateInt32AttributeValue(server, value, 7);
    std::atomic<unsigned> reads{0};
    test_reads = &reads;
    IedServer_setReadAccessHandler(server, read_access, &reads);
    std::atomic<long long> monotonic_offset{0};
    NativeServer native([&] { return std::chrono::steady_clock::now() +
        std::chrono::seconds(monotonic_offset.load()); });
    NativeServerFiles files;
    if (resilience) files.maximum_receive_pairs = 64;
    files.certificate = root + (ec ? "/server_ec.der" : "/server.der");
    files.private_key = root + (ec ? "/server_ec.key" : "/server.key");
    files.transport_certificate = root + "/tls_server.pem";
    files.trust_anchors = root + "/root.pem"; files.crls = root + "/root.crl.der";
    files.ap_title = "1.3.6.1.4.1.55555.62351.4.2"; files.ae_qualifier = 1;
    files.signature_scheme = ec ? SignatureScheme::ecdsa_sha256 : SignatureScheme::rsa_pkcs1_v15_sha256;
    auto peer = read(root + "/peer.der");
    auto peer_ec = read(root + "/peer_ec.der");
    std::string error;
    int token = 7;
    if (CcliE2e_requestApplicationAbort(&token, nullptr, 0)) return 10;
    {
        auto duplicate = files;
        duplicate.transport_certificate = files.certificate;
        NativeServer rejected;
        if (rejected.configure(duplicate,
                [](ByteView, ByteView, const std::string&, void**) { return false; },
                {}, {}, error) || error.find("must be distinct") == std::string::npos) return 9;
    }
    if (!native.configure(files, [&](ByteView cert, ByteView transport, const std::string&, void** out) {
            if (transport.empty() || (!wire::equal(cert, wire::view(peer)) && !wire::equal(cert, wire::view(peer_ec))))
                return false;
            *out = &token; return true;
        }, {}, [](const std::string& event) { std::cerr << event << '\n'; }, error)) {
        std::cerr << "configure: " << error << '\n'; return 4;
    }
    IedServer_setAuthenticator(server, legacy_reject, &native);
    if (!native.install(&native)) return 5;
    if (!CcliE2e_install(&native, campaign_handler)) return 5;
    IedServer_start(server, std::stoi(argv[2]));
    if (!IedServer_isRunning(server)) return 6;
    std::cout << "READY\n" << std::flush;
    std::string command;
    while (std::getline(std::cin, command)) {
        if (command.rfind("advance ", 0) == 0) {
            monotonic_offset.fetch_add(std::stoll(command.substr(8)));
            std::cout << "ADVANCED\n" << std::flush;
        } else if (command.rfind("response-size ", 0) == 0) {
            boundary_size.store(std::stol(command.substr(14)));
            std::cout << "SIZE_SET\n" << std::flush;
        } else if (command.rfind("fail-alloc ", 0) == 0) {
            allocation_hits.store(0);
            allocation_position.store(std::stol(command.substr(11)));
            std::cout << "FAULT_SET\n" << std::flush;
        } else if (command == "metrics") {
            std::cout << "METRICS " << reads.load() << ' ' << allocation_hits.load() << '\n' << std::flush;
        } else if (command == "config-boundaries") {
            unsigned checks = 0;
            auto verify = [&](const NativeServerFiles& candidate, bool expected) {
                NativeServer isolated; std::string why;
                const bool accepted = isolated.configure(candidate,
                    [](ByteView, ByteView, const std::string&, void**) { return false; }, {}, {}, why);
                if (accepted != expected) return false;
                ++checks; return true;
            };
            for (unsigned value : {899U, 900U, 86400U, 86401U}) {
                auto candidate = files; candidate.rekey_seconds = value;
                if (!verify(candidate, value >= 900 && value <= 86400)) return 12;
            }
            for (std::size_t value : {1023U, 1024U, 64000U, 64001U}) {
                auto candidate = files; candidate.maximum_pdu = value;
                if (!verify(candidate, value >= 1024 && value <= 64000)) return 12;
            }
            for (std::size_t value : {0U, 1U, 65536U, 65537U}) {
                auto candidate = files; candidate.maximum_receive_pairs = value;
                if (!verify(candidate, value > 0 && value <= 65536)) return 12;
            }
            std::cout << "CONFIG_BOUNDARIES " << checks << '\n' << std::flush;
        } else break;
    }
    IedServer_stop(server);
    IedServer_destroy(server);
    native.uninstall();
    IedModel_destroy(model);
    TLSConfiguration_destroy(tls);
    std::cout << "READS " << reads.load() << '\n';
    return 0;
}
