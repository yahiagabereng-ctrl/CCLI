// Separate TLS/application identities. Control tests are restricted to isolated ports.
extern "C" {
#include "iec61850_client.h"
#include "tls_config.h"
#include "acse.h"
#include "iso_session.h"
}
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace { std::string response_file; }
namespace {
void save_chain(const std::string& suffix, BufferChain chain) {
    std::ofstream out(response_file + suffix, std::ios::binary);
    for (; chain; chain = chain->nextPart)
        out.write(reinterpret_cast<const char*>(chain->buffer), chain->partLength);
}
}
extern "C" void __real_AcseConnection_createAssociateRequestMessage(AcseConnection*, IsoConnectionParameters, BufferChain, BufferChain, AcseAuthenticationParameter);
extern "C" void __wrap_AcseConnection_createAssociateRequestMessage(AcseConnection* self, IsoConnectionParameters params,
    BufferChain out, BufferChain payload, AcseAuthenticationParameter auth) {
    __real_AcseConnection_createAssociateRequestMessage(self, params, out, payload, auth);
    save_chain(".aarq", out);
}
extern "C" void __real_IsoSession_createConnectSpdu(IsoSession*, IsoConnectionParameters, BufferChain, BufferChain);
extern "C" void __wrap_IsoSession_createConnectSpdu(IsoSession* self, IsoConnectionParameters params, BufferChain out, BufferChain payload) {
    __real_IsoSession_createConnectSpdu(self, params, out, payload);
    save_chain(".spdu", out);
}
extern "C" IsoSessionIndication __real_IsoSession_parseMessage(IsoSession*, ByteBuffer*);
extern "C" IsoSessionIndication __wrap_IsoSession_parseMessage(IsoSession* self, ByteBuffer* message) {
    if (message && message->size > 0 && message->buffer[0] == 14) {
        std::ofstream out(response_file + ".accept.spdu", std::ios::binary);
        out.write(reinterpret_cast<const char*>(message->buffer), message->size);
    }
    return __real_IsoSession_parseMessage(self, message);
}
extern "C" AcseIndication __real_AcseConnection_parseMessage(AcseConnection*, ByteBuffer*);
extern "C" AcseIndication __wrap_AcseConnection_parseMessage(AcseConnection* self, ByteBuffer* message) {
    // Test-only linker wrapper captures the actual received AARE for independent
    // Python certificate, timestamp and signature validation, including negatives.
    if (message && message->size > 0 && message->buffer[0] == 0x61) {
        std::ofstream out(response_file, std::ios::binary);
        out.write(reinterpret_cast<const char*>(message->buffer), message->size);
    }
    return __real_AcseConnection_parseMessage(self, message);
}
int main(int argc, char** argv) {
    if (argc != 12) {
        std::cerr << "usage: dual-probe HOST PORT SERVER_CA TLS_CERT TLS_KEY AUTH_BLOB AP AE AARE_OUT ALLOW|DENY LABEL\n";
        return 2;
    }
    const int port = std::atoi(argv[2]);
    const bool main_read_only = port == 3782 && std::string(argv[11]) == "main-readonly";
    if (!main_read_only && port != 3783 && !(port == 3784 && std::string(argv[1]) == "127.0.0.1")) {
        std::cerr << "refusing test outside isolated ports or explicit main-readonly mode\n"; return 2;
    }
    response_file = argv[9];
    std::ifstream input(argv[6], std::ios::binary);
    std::vector<uint8_t> auth{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    if (auth.empty()) return 2;
    auto tls = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tls, true);
    if (!TLSConfiguration_addCACertificateFromFile(tls, argv[3]) ||
        !TLSConfiguration_setOwnCertificateFromFile(tls, argv[4]) ||
        !TLSConfiguration_setOwnKeyFromFile(tls, argv[5], nullptr)) return 2;
    auto connection = IedConnection_createWithTlsSupport(tls);
    auto params = MmsConnection_getIsoConnectionParameters(IedConnection_getMmsConnection(connection));
    auto authentication = AcseAuthenticationParameter_create();
    authentication->mechanism = ACSE_AUTH_CERTIFICATE;
    authentication->value.certificate.buf = auth.data();
    authentication->value.certificate.length = static_cast<int>(auth.size());
    IsoConnectionParameters_setAcseAuthenticationParameter(params, authentication);
    IsoConnectionParameters_setLocalApTitle(params, argv[7], std::atoi(argv[8]));
    IsoConnectionParameters_setRemoteApTitle(params, "1.1.1.999.1", 12);
    IedConnection_setConnectTimeout(connection, 5000);
    IedConnection_setRequestTimeout(connection, 5000);
    IedClientError error;
    IedConnection_connect(connection, &error, argv[1], port);
    const bool associated = error == IED_ERROR_OK;
    bool read = false;
    if (associated) {
        auto value = IedConnection_readObject(connection, &error,
            "CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX);
        read = error == IED_ERROR_OK && value && MmsValue_getType(value) != MMS_DATA_ACCESS_ERROR;
        if (value) MmsValue_delete(value);
    }
    bool permissions = true;
    if (read && (std::string(argv[11]) == "dso-positive" || std::string(argv[11]) == "viewer-positive")) {
        const bool dso = std::string(argv[11]) == "dso-positive";
        IedConnection_writeInt32Value(connection, &error,
            "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel", IEC61850_FC_CF, 3);
        permissions = dso ? error == IED_ERROR_OK : error == IED_ERROR_ACCESS_DENIED;
        auto control = ControlObjectClient_create("CCI016_01LD_Plant/WlimDWMX1.Mod", connection);
        if (!control) permissions = false;
        else {
            auto value = MmsValue_newIntegerFromInt32(5);
            ControlObjectClient_setOrigin(control, "isolated-dual-certificate", 1);
            const bool operated = ControlObjectClient_operate(control, value, 0);
            permissions = permissions && operated == dso;
            MmsValue_delete(value); ControlObjectClient_destroy(control);
        }
        std::cout << (permissions ? "PASS: " : "FAIL: ") << argv[11]
                  << " CONFIG/CONTROL " << (dso ? "allowed in isolated memory" : "denied") << '\n';
    }
    const bool expected = std::string(argv[10]) == "ALLOW";
    const bool passed = read == expected && permissions;
    std::cout << (passed ? "PASS: " : "FAIL: ") << argv[11] << " associated=" << associated
              << " readable=" << read << " client_error=" << error << '\n';
    IedConnection_close(connection);
    IedConnection_destroy(connection);
    TLSConfiguration_destroy(tls);
    return passed ? 0 : 1;
}
