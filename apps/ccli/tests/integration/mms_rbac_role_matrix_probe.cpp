// Role matrix for the isolated on-device adapter; never sends writes to port 3782.
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
        check(argc == 8, "usage: probe HOST PORT SERVER_CA CERT KEY ROLE_ID LABEL");
        check(std::string(argv[2]) == "3783", "refusing operations outside isolated port 3783");
        const int role = std::stoi(argv[6]); // 20 = commissioned Viewer+Engineer union
        check((role >= -2 && role <= 6) || role == 20, "unsupported matrix role");
        const bool aggregator = role == -2;
        const bool can_config = role == -1 || role == 2 || role == 3 || role == 4 || role == 6 || role == 20;
        const bool can_control = role == -1 || role == 1;
        const bool can_dataset = role == -1 || role == 2 || role == 4 || role == 20;
        const bool can_report = role == -1 || role == 0 || role == 1 || role == 2 || role == 3 || role == 5 || role == 20;
        tls = TLSConfiguration_create();
        TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
        TLSConfiguration_setChainValidation(tls, true);
        check(TLSConfiguration_addCACertificateFromFile(tls, argv[3]), "server CA load failed");
        check(TLSConfiguration_setOwnCertificateFromFile(tls, argv[4]), "client certificate load failed");
        check(TLSConfiguration_setOwnKeyFromFile(tls, argv[5], nullptr), "client key load failed");
        connection = IedConnection_createWithTlsSupport(tls);
        IedConnection_setConnectTimeout(connection, 5000);
        IedConnection_setRequestTimeout(connection, 5000);
        IedClientError error;
        IedConnection_connect(connection, &error, argv[1], 3783);
        check(error == IED_ERROR_OK, "TLS/MMS association failed: " + std::to_string(error));
        std::cout << "PASS: " << argv[7] << " role-bearing TLS/MMS association\n";
        const auto readable = [&](const char* ref, FunctionalConstraint fc, bool expected) {
            auto* value = IedConnection_readObject(connection, &error, ref, fc);
            const bool allowed = error == IED_ERROR_OK && value && MmsValue_getType(value) != MMS_DATA_ACCESS_ERROR;
            const int access_error = value && MmsValue_getType(value) == MMS_DATA_ACCESS_ERROR
                ? static_cast<int>(MmsValue_getDataAccessError(value)) : -1;
            if (value) MmsValue_delete(value);
            check(allowed == expected, std::string("read permission mismatch: ") + ref + " error=" + std::to_string(error));
            if (!expected) check(error == IED_ERROR_ACCESS_DENIED || access_error == DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED,
                std::string("read did not fail with access denied: ") + ref + " client=" + std::to_string(error) + " mms=" + std::to_string(access_error));
            std::cout << "PASS: READ " << ref << ' ' << (expected ? "allowed" : "denied") << '\n';
        };
        readable("CCI016_01LD_Plant/PdCMMXU1.TotW.mag.f", IEC61850_FC_MX, !aggregator);
        if (aggregator) readable("CCI016_01LD_Plant/WSaDAGC1.Mod.stVal", IEC61850_FC_ST, true);
        // Same instantiated DSO object exercises private-role scope separation.
        readable("CCI016_01LD_Plant/WlimDWMX1.Mod.stVal", IEC61850_FC_ST, !aggregator);
        auto directory = IedConnection_getLogicalNodeDirectory(connection, &error,
            aggregator ? "CCI016_01LD_Plant/WSaDAGC1" : "CCI016_01LD_Plant/WlimDWMX1", ACSI_CLASS_DATA_OBJECT);
        const bool listed = error == IED_ERROR_OK && directory && LinkedList_size(directory) > 0;
        if (directory) LinkedList_destroy(directory);
        check(listed, "allowed object discovery returned no objects");
        std::cout << "PASS: LISTOBJECTS permitted scope\n";
        directory = IedConnection_getDataSetDirectory(connection, &error,
            "CCI016_01LD_Plant/LLN0.DS_R_PdC_Mis4sec", nullptr);
        const bool dataset_allowed = error == IED_ERROR_OK && directory;
        if (directory) LinkedList_destroy(directory);
        check(dataset_allowed == can_dataset, "DATASET directory permission mismatch: " + std::to_string(error));
        if (!can_dataset) check(error == IED_ERROR_ACCESS_DENIED, "dataset failure was not access denied");
        std::cout << "PASS: DATASET " << (can_dataset ? "allowed" : "denied") << '\n';

        ClientReportControlBlock rcb = nullptr;
        std::string reference;
        for (const char* suffix : {"", "01"}) {
            reference = std::string("CCI016_01LD_Plant/LLN0.RP.urcb_PdC_Mis4sec") + suffix;
            rcb = IedConnection_getRCBValues(connection, &error, reference.c_str(), nullptr);
            if (error == IED_ERROR_OK && rcb) break;
            if (rcb) ClientReportControlBlock_destroy(rcb);
            rcb = nullptr;
            if (error == IED_ERROR_ACCESS_DENIED) break;
        }
        const bool report_allowed = rcb != nullptr;
        if (report_allowed != can_report) {
            if (rcb) ClientReportControlBlock_destroy(rcb);
            throw std::runtime_error("REPORTING read permission mismatch: " + std::to_string(error));
        }
        if (can_report) {
            std::atomic<unsigned> count{0};
            IedConnection_installReportHandler(connection, reference.c_str(), ClientReportControlBlock_getRptId(rcb), report, &count);
            ClientReportControlBlock_setRptEna(rcb, true);
            IedConnection_setRCBValues(connection, &error, rcb, RCB_ELEMENT_RPT_ENA, true);
            const bool enabled = error == IED_ERROR_OK;
            for (unsigned i = 0; enabled && i < 65 && count.load() == 0; ++i)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            const bool received = count.load() > 0;
            ClientReportControlBlock_setRptEna(rcb, false);
            IedConnection_setRCBValues(connection, &error, rcb, RCB_ELEMENT_RPT_ENA, true);
            const bool disabled = error == IED_ERROR_OK;
            IedConnection_uninstallReportHandler(connection, reference.c_str());
            ClientReportControlBlock_destroy(rcb);
            check(enabled && received && disabled, "report enable/delivery/disable failed");
        } else check(error == IED_ERROR_ACCESS_DENIED, "report failure was not access denied");
        std::cout << "PASS: REPORTING " << (can_report ? "read/enable/delivery/disable allowed" : "denied") << '\n';

        IedConnection_writeInt32Value(connection, &error,
            "CCI016_01LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel", IEC61850_FC_CF, 3);
        check((error == IED_ERROR_OK) == can_config, "CONFIG permission mismatch: " + std::to_string(error));
        if (!can_config) check(error == IED_ERROR_ACCESS_DENIED, "config failure was not access denied");
        std::cout << "PASS: CONFIG " << (can_config ? "allowed (isolated memory)" : "denied") << '\n';
        if (!aggregator) {
            auto control = ControlObjectClient_create("CCI016_01LD_Plant/WlimDWMX1.Mod", connection);
            check(control != nullptr, "control client could not read permitted model");
            ControlObjectClient_setOrigin(control, "isolated-role-matrix", 1);
            auto* value = MmsValue_newIntegerFromInt32(5);
            const bool operated = ControlObjectClient_operate(control, value, 0);
            MmsValue_delete(value);
            ControlObjectClient_destroy(control);
            check(operated == can_control, "CONTROL permission mismatch");
            std::cout << "PASS: CONTROL " << (can_control ? "allowed (isolated memory)" : "denied") << '\n';
        } else std::cout << "LIMIT: Aggregator CONTROL target absent; DSO read/write scope denial tested\n";
        std::cout << "PASS: MATRIX " << argv[7] << '\n';
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
