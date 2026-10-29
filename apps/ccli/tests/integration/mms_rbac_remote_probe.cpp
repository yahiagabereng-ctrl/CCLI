// Network test client for the isolated device harness, never the plant port.
extern "C" {
#include "iec61850_client.h"
#include "tls_config.h"
}
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace {
void check(bool value, const std::string& message) { if (!value) throw std::runtime_error(message); }
void report(void* parameter, ClientReport) { ++*static_cast<std::atomic<unsigned>*>(parameter); }
}
int main(int argc, char** argv) {
    TLSConfiguration tls = nullptr;
    IedConnection connection = nullptr;
    try {
        check(argc == 7, "usage: probe HOST SERVER_CA CERT KEY ROLE PORT");
        check(std::string(argv[6]) == "3783", "refusing any port except isolated test port 3783");
        const std::string role(argv[5]);
        tls = TLSConfiguration_create();
        TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setChainValidation(tls, true);
        check(TLSConfiguration_addCACertificateFromFile(tls, argv[2]), "server CA load failed");
        check(TLSConfiguration_setOwnCertificateFromFile(tls, argv[3]), "client certificate load failed");
        check(TLSConfiguration_setOwnKeyFromFile(tls, argv[4], nullptr), "client key load failed");
        connection = IedConnection_createWithTlsSupport(tls);
        IedConnection_setConnectTimeout(connection, 5000);
        IedConnection_setRequestTimeout(connection, 5000);
        IedClientError error;
        IedConnection_connect(connection, &error, argv[1], std::atoi(argv[6]));
        if (role == "UNAUTHORIZED" && error != IED_ERROR_OK) {
            // Caller independently proves this certificate's TLS handshake.
            // The adapter can reject an unbound identity at MMS association.
            std::cout << "PASS: uncommissioned identity rejected at MMS association\n";
            IedConnection_close(connection);
            IedConnection_destroy(connection);
            TLSConfiguration_destroy(tls);
            return 0;
        }
        check(error == IED_ERROR_OK, "TLS/MMS association failed: " + std::to_string(error));
        auto* value = IedConnection_readObject(connection, &error, "CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX);
        const bool readable = error == IED_ERROR_OK && value && MmsValue_getType(value) != MMS_DATA_ACCESS_ERROR;
        if (value) MmsValue_delete(value);
        if (role == "UNAUTHORIZED") {
            check(!readable, "uncommissioned identity read process values");
            std::cout << "PASS: pinned CA-valid but uncommissioned identity denied MMS access\n";
        } else {
            check(readable, "measurement read denied: " + std::to_string(error));
            std::cout << "PASS: " << role << " mutual TLS/MMS and measurement read\n";
            if (role == "VIEWER" || role == "DSO" || role == "ENGINEER") {
                IedConnection_writeInt32Value(connection, &error, "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel", IEC61850_FC_CF, 3);
                check((error == IED_ERROR_OK) == (role != "VIEWER"), "RW configuration permission mismatch: " + std::to_string(error));
                std::cout << "PASS: " << role << " RW CONFIG " << (role == "VIEWER" ? "denied" : "allowed") << '\n';
                if (role != "ENGINEER") {
                    IedConnection_writeFloatValue(connection, &error, "CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX, 99.0f);
                    check(error != IED_ERROR_OK, "read-only measurement accepted a write");
                    std::cout << "PASS: read-only measurement write denied\n";
                }
            }
            if (role == "VIEWER" || role == "DSO") {
                auto control = ControlObjectClient_create("CCI016_01LD_Plant/WlimDWMX1.Mod", connection);
                check(control != nullptr, "control client creation failed");
                ControlObjectClient_setOrigin(control, "isolated-rbac-test", 1);
                auto ctl = MmsValue_newIntegerFromInt32(5);
                const bool operated = ControlObjectClient_operate(control, ctl, 0);
                MmsValue_delete(ctl);
                ControlObjectClient_destroy(control);
                check(operated == (role == "DSO"), "CONTROL permission mismatch");
                std::cout << "PASS: " << role << " CONTROL " << (operated ? "allowed (isolated memory only)" : "denied") << '\n';
            }
            if (role == "DSO") {
                auto directory = IedConnection_getDataSetDirectory(connection, &error, "CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec", nullptr);
                check(error == IED_ERROR_OK && directory, "static dataset directory denied");
                LinkedList_destroy(directory);
                ClientReportControlBlock rcb = nullptr;
                std::string reference;
                for (const char* suffix : {"", "01"}) {
                    reference = std::string("CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec") + suffix;
                    rcb = IedConnection_getRCBValues(connection, &error, reference.c_str(), nullptr);
                    if (error == IED_ERROR_OK && rcb) break;
                    if (rcb) ClientReportControlBlock_destroy(rcb);
                    rcb = nullptr;
                }
                check(rcb != nullptr, "report control block read failed");
                std::atomic<unsigned> count{0};
                IedConnection_installReportHandler(connection, reference.c_str(), ClientReportControlBlock_getRptId(rcb), report, &count);
                ClientReportControlBlock_setRptEna(rcb, true);
                IedConnection_setRCBValues(connection, &error, rcb, RCB_ELEMENT_RPT_ENA, true);
                check(error == IED_ERROR_OK, "report enable denied");
                for (unsigned i = 0; i < 60 && count.load() == 0; ++i) std::this_thread::sleep_for(std::chrono::milliseconds(100));
                const bool received = count.load() > 0;
                ClientReportControlBlock_setRptEna(rcb, false);
                IedConnection_setRCBValues(connection, &error, rcb, RCB_ELEMENT_RPT_ENA, true);
                IedConnection_uninstallReportHandler(connection, reference.c_str());
                ClientReportControlBlock_destroy(rcb);
                check(received, "no authorized report received");
                std::cout << "PASS: static dataset membership, report enable and delivery\n";
            }
        }
        IedConnection_close(connection);
        IedConnection_destroy(connection);
        TLSConfiguration_destroy(tls);
        return 0;
    } catch (const std::exception& e) {
        if (connection) { IedConnection_close(connection); IedConnection_destroy(connection); }
        if (tls) TLSConfiguration_destroy(tls);
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
