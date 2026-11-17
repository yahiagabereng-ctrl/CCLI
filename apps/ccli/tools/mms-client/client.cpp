// MMS commissioning client. Authorization is enforced by the connected server.
extern "C" {
#include "iec61850_client.h"
#include "tls_config.h"
}
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <sstream>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <csignal>
#include <set>

static std::mutex output_mutex;
static volatile std::sig_atomic_t interrupted = 0;
static void stop_client(int) { interrupted = 1; }
#ifdef CCLI_MMS_TSP_CLIENT_PROFILE
extern "C" void CcliDualCert_setClientAareVerifier(int (*verifier)(const uint8_t*, int));
#endif

static std::string quote(const std::string& s) {
    std::string out = "\"";
    for (unsigned char c : s) {
        if (c == '\\' || c == '"') { out += '\\'; out += c; }
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if (c < 32) { const char* h = "0123456789abcdef"; out += "\\u00"; out += h[c >> 4]; out += h[c & 15]; }
        else out += c;
    }
    return out + '"';
}
static void event(const std::string& kind, const std::string& ref, int error, const std::string& value = "") {
    std::lock_guard<std::mutex> lock(output_mutex);
    std::cout << "{\"kind\":" << quote(kind) << ",\"reference\":" << quote(ref)
              << ",\"error\":" << error << ",\"error_text\":" << quote(error >= 10 ? IedClientError_toString(static_cast<IedClientError>(error)) : "")
              << ",\"value\":" << quote(value) << "}" << std::endl;
}
#ifdef CCLI_MMS_TSP_CLIENT_PROFILE
static int verify_server_aare(const uint8_t* bytes, int length) {
    if (length <= 0 || length > 16384) return 0;
    static const char hex[] = "0123456789abcdef";
    std::string encoded;
    for (int i = 0; i < length; ++i) { encoded += hex[bytes[i] >> 4]; encoded += hex[bytes[i] & 15]; }
    event("server_auth", "", 0, encoded);
    std::string verdict;
    return std::getline(std::cin, verdict) && verdict == "ACCEPT";
}
#endif
static bool read(IedConnection con, const std::string& ref, FunctionalConstraint fc, const MmsValue* expected = nullptr) {
    IedClientError err;
    auto value = IedConnection_readObject(con, &err, ref.c_str(), fc);
    bool ok = err == IED_ERROR_OK && value && MmsValue_getType(value) != MMS_DATA_ACCESS_ERROR;
    char text[8192] = {};
    if (value) MmsValue_printToBuffer(value, text, sizeof(text));
    const bool access_denied = value && MmsValue_getType(value) == MMS_DATA_ACCESS_ERROR &&
        MmsValue_getDataAccessError(value) == DATA_ACCESS_ERROR_OBJECT_ACCESS_DENIED;
    event("read", ref, ok ? 0 : (err ? int(err) : access_denied ? int(IED_ERROR_ACCESS_DENIED) : -1), text);
    if (ok && expected) {
        bool matches = false;
        const auto type = MmsValue_getType(value);
        if (type == MmsValue_getType(expected)) {
            if (type == MMS_INTEGER) matches = MmsValue_toInt64(value) == MmsValue_toInt64(expected);
            else if (type == MMS_FLOAT) {
                const auto a = MmsValue_toDouble(value), b = MmsValue_toDouble(expected);
                matches = std::isfinite(a) && std::abs(a-b) <= 1e-5 * std::max(1.0, std::abs(b));
            } else matches = MmsValue_equals(value, expected);
        }
        event("readback_match", ref, matches ? 0 : 1, matches ? "matches requested value" : "read succeeded but status differs from the requested value");
        ok = matches;
    }
    if (ok && MmsValue_getType(value) == MMS_UTC_TIME) {
        event("time_quality", ref, 0, "epoch_ms=" + std::to_string(MmsValue_getUtcTimeInMs(value)) +
              " clockNotSynchronized=" + std::to_string((MmsValue_getUtcTimeQuality(value) >> 5) & 1));
    }
    if (ok && ref.size() > 2 && ref.substr(ref.size() - 2) == ".q" && MmsValue_getType(value) == MMS_BIT_STRING) {
        Quality q = Quality_fromMmsValue(value);
        const auto validity = Quality_getValidity(&q);
        std::string label = validity == QUALITY_VALIDITY_GOOD ? "good" :
              validity == QUALITY_VALIDITY_INVALID ? "invalid" :
              validity == QUALITY_VALIDITY_QUESTIONABLE ? "questionable" : "reserved";
        event("quality", ref, 0, label + " raw=" + std::to_string(q) +
              " oldData=" + std::to_string(Quality_isFlagSet(&q, QUALITY_DETAIL_OLD_DATA)) +
              " inaccurate=" + std::to_string(Quality_isFlagSet(&q, QUALITY_DETAIL_INACCURATE)));
    }
    if (value) MmsValue_delete(value);
    return ok;
}
static bool directory(IedConnection con, const std::string& ref, const std::string& kind) {
    IedClientError err;
    LinkedList list = nullptr;
    if (kind == "devices") list = IedConnection_getServerDirectory(con, &err, false);
    else if (kind == "nodes") list = IedConnection_getLogicalDeviceDirectory(con, &err, ref.c_str());
    else if (kind == "attributes") list = IedConnection_getDataDirectoryFC(con, &err, ref.c_str());
    else list = IedConnection_getLogicalNodeDirectory(con, &err, ref.c_str(),
        kind == "datasets" ? ACSI_CLASS_DATA_SET : kind == "report-blocks" ? ACSI_CLASS_URCB : ACSI_CLASS_DATA_OBJECT);
    event("directory", ref, int(err), kind);
    for (auto item = list ? LinkedList_getNext(list) : nullptr; item; item = LinkedList_getNext(item))
        event("entry", ref, 0, static_cast<const char*>(item->data));
    if (list) LinkedList_destroy(list);
    return err == IED_ERROR_OK;
}

static MmsValue* make_value(const std::string& type, const std::string& text) {
    size_t end = 0;
    if (type == "string") return MmsValue_newVisibleString(text.c_str());
    if (type == "bool") {
        if (text != "true" && text != "false" && text != "0" && text != "1") throw std::invalid_argument("bool must be true/false/0/1");
        return MmsValue_newBoolean(text == "true" || text == "1");
    }
    if (type == "int32" || type == "int64" || type == "uint32") {
        const auto n = std::stoll(text, &end);
        if (end != text.size()) throw std::invalid_argument("invalid integer");
        if (type == "int64") return MmsValue_newIntegerFromInt64(n);
        if (type == "uint32") {
            if (n < 0 || n > 4294967295LL) throw std::out_of_range("uint32");
            return MmsValue_newUnsignedFromUint32(uint32_t(n));
        }
        if (n < -2147483648LL || n > 2147483647LL) throw std::out_of_range("int32");
        return MmsValue_newIntegerFromInt32(int32_t(n));
    }
    if (type != "float" && type != "double") throw std::invalid_argument("unsupported value type");
    const double n = std::stod(text, &end);
    if (end != text.size() || !std::isfinite(n) || (type == "float" && std::abs(n) > std::numeric_limits<float>::max()))
        throw std::out_of_range("invalid/non-finite floating point value");
    return type == "float" ? MmsValue_newFloat(float(n)) : MmsValue_newDouble(n);
}

struct Termination { std::atomic<bool> done{false}; std::atomic<bool> good{false}; };
static void termination(void* p, ControlObjectClient control) {
    const auto e = ControlObjectClient_getLastApplError(control);
    auto* t = static_cast<Termination*>(p);
    t->good = e.error == CONTROL_ERROR_NO_ERROR && e.addCause == ADD_CAUSE_UNKNOWN;
    event("termination", ControlObjectClient_getObjectReference(control), t->good ? 0 : 1,
          "error=" + std::to_string(e.error) + " addCause=" + std::to_string(e.addCause));
    t->done = true;
}
static bool control(IedConnection con, const std::string& cmd, const std::string& ref, MmsValue* value) {
    auto c = ControlObjectClient_create(ref.c_str(), con);
    if (!c) {
        // Preserve the server's access error; ControlObjectClient_create only returns null.
        read(con, ref + ".ctlModel", IEC61850_FC_CF);
        event(cmd, ref, 1, "cannot read control model; check object reference and access");
        return false;
    }
    const auto model = ControlObjectClient_getControlModel(c);
    const char* names[] = {"status-only", "direct-normal", "SBO-normal", "direct-enhanced", "SBO-enhanced"};
    event("control_model", ref, 0, std::to_string(model) + " " + (int(model) >= 0 && int(model) < 5 ? names[int(model)] : "unknown"));
    const auto value_type = ControlObjectClient_getCtlValType(c);
    event("control_type", ref, 0, value_type == MMS_BOOLEAN ? "bool" : value_type == MMS_INTEGER ? "int32" :
          value_type == MMS_UNSIGNED ? "uint32" : (value_type == MMS_FLOAT || value_type == MMS_STRUCTURE) ? "float" : "unsupported");
    if (cmd == "control-info") { ControlObjectClient_destroy(c); return true; }
    ControlObjectClient_setOrigin(c, "ccli-mms-client", 1);
    Termination t;
    ControlObjectClient_setCommandTerminationHandler(c, termination, &t);
    bool ok = model != CONTROL_MODEL_STATUS_ONLY;
    bool selected = false;
    if (ok && (model == CONTROL_MODEL_SBO_NORMAL || model == CONTROL_MODEL_SBO_ENHANCED)) {
        selected = model == CONTROL_MODEL_SBO_NORMAL ? ControlObjectClient_select(c) : ControlObjectClient_selectWithValue(c, value);
        ok = selected;
        event("select", ref, ok ? 0 : int(ControlObjectClient_getLastError(c)), ok ? "selected on this association" : "selection rejected");
    }
    if (cmd == "select-cancel") {
        if (!selected) { event(cmd, ref, 1, "no selection was established; inspect control model and selection error"); ok = false; }
        else { ok = ControlObjectClient_cancel(c); event("cancel", ref, ok ? 0 : 1); }
    } else if (ok) {
        ok = ControlObjectClient_operate(c, value, 0);
        event("operate", ref, ok ? 0 : 1, ok ? "request accepted" : "request rejected");
        if (ok && (model == CONTROL_MODEL_DIRECT_ENHANCED || model == CONTROL_MODEL_SBO_ENHANCED)) {
            for (int i = 0; i < 100 && !t.done; ++i) std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ok = t.done && t.good;
            if (!t.done) event("termination", ref, 1, "request accepted but command termination timed out; verify server state before retrying");
        }
    } else event("operate", ref, 1, "status-only object or selection failed");
    if (!ok) {
        const auto e = ControlObjectClient_getLastApplError(c);
        event("control_error", ref, int(ControlObjectClient_getLastError(c)),
              "error=" + std::to_string(e.error) + " addCause=" + std::to_string(e.addCause));
        if (selected) ControlObjectClient_cancel(c);
    }
    ControlObjectClient_destroy(c);
    if (ok && cmd == "operate") {
        event("readback", ref, 0, "reading status on the same MMS association");
        const bool analog = value_type == MMS_FLOAT || value_type == MMS_STRUCTURE;
        ok = read(con, ref + (analog ? ".mxVal.f" : ".stVal"), analog ? IEC61850_FC_MX : IEC61850_FC_ST, value) && ok;
    }
    return ok;
}

static bool dataset(IedConnection con, const std::string& cmd, const std::string& ref, const std::string& members) {
    IedClientError err = IED_ERROR_OK;
    if (cmd == "dataset-read") {
        auto ds = IedConnection_readDataSetValues(con, &err, ref.c_str(), nullptr);
        if (ds) { char text[16384]{}; MmsValue_printToBuffer(ClientDataSet_getValues(ds), text, sizeof(text)); event(cmd, ref, int(err), text); ClientDataSet_destroy(ds); }
        else event(cmd, ref, int(err) ? int(err) : -1, "no dataset returned");
        return ds && err == IED_ERROR_OK;
    }
    if (cmd == "dataset-members") {
        bool deletable = false;
        auto list = IedConnection_getDataSetDirectory(con, &err, ref.c_str(), &deletable);
        event(cmd, ref, int(err), deletable ? "deletable" : "static");
        for (auto item = list ? LinkedList_getNext(list) : nullptr; item; item = LinkedList_getNext(item)) event("entry", ref, 0, static_cast<const char*>(item->data));
        if (list) LinkedList_destroy(list);
    } else if (cmd == "dataset-create") {
        std::vector<std::string> refs;
        std::istringstream input(members); std::string item;
        while (std::getline(input, item, ';')) if (!item.empty()) refs.push_back(item);
        if (refs.empty()) { event(cmd, ref, 1, "provide at least one member"); return false; }
        auto list = LinkedList_create();
        for (auto& r : refs) LinkedList_add(list, const_cast<char*>(r.c_str()));
        IedConnection_createDataSet(con, &err, ref.c_str(), list);
        LinkedList_destroyStatic(list);
        event(cmd, ref, int(err), members);
    } else {
        const bool deleted = IedConnection_deleteDataSet(con, &err, ref.c_str());
        if (!deleted && err == IED_ERROR_OK) { event(cmd, ref, 1, "server did not delete the dataset"); return false; }
        event(cmd, ref, int(err));
    }
    return err == IED_ERROR_OK;
}

static void report_callback(void* p, ClientReport report) {
    ++*static_cast<std::atomic<int>*>(p);
    char text[16384]{};
    const auto values = ClientReport_getDataSetValues(report);
    if (values) MmsValue_printToBuffer(values, text, sizeof(text));
    event("report", ClientReport_getRcbReference(report), 0, text);
}
static bool reports(IedConnection con, const std::string& cmd, const std::string& ref, int seconds) {
    // This process owns one association. Enabling a URCB can reserve it until disconnect.
    static std::set<std::string> owned_reservations;
    IedClientError err;
    auto rcb = IedConnection_getRCBValues(con, &err, ref.c_str(), nullptr);
    if (!rcb || err != IED_ERROR_OK) { if (rcb) ClientReportControlBlock_destroy(rcb); event(cmd, ref, int(err) ? int(err) : -1); return false; }
    const auto ds = ClientReportControlBlock_getDataSetReference(rcb);
    event("rcb", ref, 0, std::string("enabled=") + (ClientReportControlBlock_getRptEna(rcb) ? "true" : "false") + " dataset=" + (ds ? ds : ""));
    if (cmd == "rcb-read") { ClientReportControlBlock_destroy(rcb); return true; }
    if (ClientReportControlBlock_getRptEna(rcb) ||
        ((ClientReportControlBlock_getResv(rcb) || ClientReportControlBlock_getResvTms(rcb) > 0) &&
         owned_reservations.count(ref) == 0)) {
        event(cmd, ref, 1, "report block is already enabled/reserved; choose an unused block"); ClientReportControlBlock_destroy(rcb); return false;
    }
    std::atomic<int> received{0};
    IedConnection_installReportHandler(con, ref.c_str(), ClientReportControlBlock_getRptId(rcb), report_callback, &received);
    ClientReportControlBlock_setRptEna(rcb, true);
    IedConnection_setRCBValues(con, &err, rcb, RCB_ELEMENT_RPT_ENA, true);
    bool ok = err == IED_ERROR_OK;
    event("report-enable", ref, int(err));
    if (ok) owned_reservations.insert(ref);
    if (!ok) {
        IedConnection_uninstallReportHandler(con, ref.c_str());
        ClientReportControlBlock_destroy(rcb);
        return false;
    }
    if (ok) {
        if (cmd == "reports-gi") {
            ClientReportControlBlock_setGI(rcb, true);
            IedConnection_setRCBValues(con, &err, rcb, RCB_ELEMENT_GI, true);
            event("report-GI", ref, int(err));
            ok = err == IED_ERROR_OK;
        }
        if (ok) for (int i = 0; i < seconds * 10 && !interrupted; ++i) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ClientReportControlBlock_setRptEna(rcb, false);
    IedConnection_setRCBValues(con, &err, rcb, RCB_ELEMENT_RPT_ENA, true);
    event("report-disable", ref, int(err), "received=" + std::to_string(received.load()));
    ok = ok && err == IED_ERROR_OK && received > 0;
    IedConnection_uninstallReportHandler(con, ref.c_str());
    ClientReportControlBlock_destroy(rcb);
    return ok;
}
static int execute_command(IedConnection con, const std::string& cmd, const std::string& ref,
                           const std::string& fc_text, int count, const std::string& type, const std::string& text) {
    const auto fc = FunctionalConstraint_fromString(fc_text.c_str());
    const std::vector<std::string> commands = {"check", "browse", "datasets", "read", "watch", "write", "operate", "control-info", "select-cancel", "dataset-members", "dataset-read", "dataset-create", "dataset-delete", "report-blocks", "rcb-read", "reports", "reports-gi"};
    if (count < 1 || count > 3600 || std::find(commands.begin(), commands.end(), cmd) == commands.end() ||
        ((cmd == "read" || cmd == "watch" || cmd == "write") && fc == IEC61850_FC_NONE)) {
        event("input", ref, 2, "invalid command, count or functional constraint"); return 2;
    }
    if (IedConnection_getState(con) != IED_STATE_CONNECTED) {
        event("connection", ref, IED_ERROR_CONNECTION_LOST, "connection lost; reconnect explicitly; no operation was resent"); return 1;
    }
    MmsValue* input_value = nullptr;
    try { if (cmd == "write" || cmd == "operate" || cmd == "select-cancel") input_value = make_value(type, text); }
    catch (const std::exception& e) { event("input", ref, 2, e.what()); return 2; }
    bool ok = true;
    IedClientError err;
    if (cmd == "browse") ok = directory(con, ref, ref.empty() ? "devices" : (ref.find('/') == std::string::npos ? "nodes" : (ref.find('.') == std::string::npos ? "objects" : "attributes")));
    else if (cmd == "datasets") ok = directory(con, ref, "datasets");
    else if (cmd == "report-blocks") {
        ok = directory(con, ref, "report-blocks");
        IedClientError e; auto list = IedConnection_getLogicalNodeDirectory(con, &e, ref.c_str(), ACSI_CLASS_BRCB);
        event("directory", ref, int(e), "buffered-report-blocks");
        for (auto item = list ? LinkedList_getNext(list) : nullptr; item; item = LinkedList_getNext(item)) event("entry", ref, 0, static_cast<const char*>(item->data));
        if (list) LinkedList_destroy(list); ok = ok && e == IED_ERROR_OK;
        event("note", ref, 0, "directory visibility can be restricted; an authorized RCB may still be readable by its full reference");
    }
    else if (cmd == "write") {
        IedConnection_writeObject(con, &err, ref.c_str(), fc, input_value);
        ok = err == IED_ERROR_OK; event(cmd, ref, int(err), text);
        if (ok) read(con, ref, fc);
    }
    else if (cmd == "operate" || cmd == "control-info" || cmd == "select-cancel") ok = control(con, cmd, ref, input_value);
    else if (cmd.rfind("dataset-", 0) == 0) ok = dataset(con, cmd, ref, text);
    else if (cmd == "rcb-read" || cmd == "reports" || cmd == "reports-gi") ok = reports(con, cmd, ref, count);
    else if (cmd == "check") {
        ok = directory(con, "", "devices");
        for (const auto* suffix : {"TotW.mag.f", "TotVAr.mag.f", "TotW.q", "TotW.t"})
            ok = read(con, "CCI016_01LD_Plant/PdCMMXU1." + std::string(suffix), IEC61850_FC_MX) && ok;
    } else {
        for (int i = 0; i < (cmd == "watch" ? count : 1); ++i) {
            if (interrupted) break;
            if (i) std::this_thread::sleep_for(std::chrono::seconds(1));
            ok = read(con, ref, fc) && ok;
        }
    }
    if (input_value) MmsValue_delete(input_value);
    return interrupted ? 130 : ok ? 0 : 1;
}

// Length-prefixed UTF-8 fields allow spaces/newlines in values without shell parsing.
static bool read_field(std::string& field) {
    std::string header;
    if (!std::getline(std::cin, header)) return false;
    if (header.empty() || header.size() > 7 || header.find_first_not_of("0123456789") != std::string::npos) return false;
    const auto size = std::stoul(header);
    if (size > 65536) return false;
    field.resize(size);
    if (size && !std::cin.read(&field[0], size)) return false;
    return std::cin.get() == '\n';
}

static void session_state(void*, IedConnection, IedConnectionState state) {
    if (state == IED_STATE_CLOSED) event("session_state", "", IED_ERROR_CONNECTION_LOST, "disconnected");
}

int main(int argc, char** argv) {
    std::signal(SIGINT, stop_client);
    std::signal(SIGTERM, stop_client);
    std::signal(SIGUSR1, stop_client);
    if (argc != 15) {
        std::cerr << "Internal usage: client HOST PORT CA TLS_CERT TLS_KEY AUTH_BLOB AP AE COMMAND REF FC COUNT TYPE VALUE\n";
        return 2;
    }
    const int port = std::atoi(argv[2]);
    const std::string cmd = argv[9];
    if (port < 1 || port > 65535) return 2;
    const bool certificate_auth = std::string(argv[6]) != "-";
    std::vector<uint8_t> proof;
    if (certificate_auth) {
        std::ifstream file(argv[6], std::ios::binary);
        proof.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        if (proof.empty()) { event("credentials", "", 1, "missing authentication proof"); return 2; }
    }
    auto tls = TLSConfiguration_create();
    TLSConfiguration_setMinTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setMaxTlsVersion(tls, TLS_VERSION_TLS_1_2);
    TLSConfiguration_setChainValidation(tls, true);
#ifdef CCLI_MMS_TSP_CLIENT_PROFILE
    TLSConfiguration_clearCipherSuiteList(tls);
    TLSConfiguration_addCipherSuite(tls, 0xC02B); // TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256.
    TLSConfiguration_addCipherSuite(tls, 0x003c); // Observed TSP TLS_RSA_WITH_AES_128_CBC_SHA256.
    if (certificate_auth) CcliDualCert_setClientAareVerifier(verify_server_aare);
    event("wire_profile", "", 0, "TSP observed association: MMS/ACSE/auth contexts 3/1/5, COTP TPDU 1024");
#endif
    if (!TLSConfiguration_addCACertificateFromFile(tls, argv[3]) ||
        !TLSConfiguration_setOwnCertificateFromFile(tls, argv[4]) ||
        !TLSConfiguration_setOwnKeyFromFile(tls, argv[5], nullptr)) {
        event("credentials", "", 1, "cannot load CA/certificate/key"); TLSConfiguration_destroy(tls); return 2;
    }
    auto con = IedConnection_createWithTlsSupport(tls);
    auto params = MmsConnection_getIsoConnectionParameters(IedConnection_getMmsConnection(con));
#ifdef CCLI_MMS_TSP_CLIENT_PROFILE
    auto mms = IedConnection_getMmsConnection(con);
    MmsConnection_setLocalDetail(mms, 65000);
    MmsConnnection_setMaxOutstandingCalls(mms, 10, 10);
    PSelector psel = {4, {0, 0, 0, 1}};
    SSelector ssel = {2, {0, 1}};
    TSelector tsel = {2, {0, 1}};
    IsoConnectionParameters_setLocalAddresses(params, psel, ssel, tsel);
    IsoConnectionParameters_setRemoteAddresses(params, psel, ssel, tsel);
#endif
    if (certificate_auth) {
        auto auth = AcseAuthenticationParameter_create();
        auth->mechanism = ACSE_AUTH_CERTIFICATE;
        auth->value.certificate.buf = proof.data();
        auth->value.certificate.length = int(proof.size());
        IsoConnectionParameters_setAcseAuthenticationParameter(params, auth);
    }
    IsoConnectionParameters_setLocalApTitle(params, argv[7], std::atoi(argv[8]));
    IsoConnectionParameters_setRemoteApTitle(params, "1.1.1.999.1", 12);
    IedConnection_setConnectTimeout(con, 5000);
    IedConnection_setRequestTimeout(con, 5000);
    if (cmd == "session") IedConnection_installStateChangedHandler(con, session_state, nullptr);
    IedClientError err;
    IedConnection_connect(con, &err, argv[1], port);
    event("association", argv[1], int(err), certificate_auth ?
        "mutual TLS 1.2 + ACSE certificate authentication" : "mutual TLS 1.2 + TLS peer identity authentication");
    int result = err == IED_ERROR_OK ? 0 : 1;
    if (cmd == "session") {
        event("session_ready", argv[1], result, result ? "association failed" : "connection remains open until Exit or Reconnect");
        while (result == 0) {
            std::string fields[6];
            bool valid = true;
            for (auto& field : fields) if (!read_field(field)) { valid = false; break; }
            if (!valid || fields[0] == "quit") break;
            interrupted = 0;
            int code = 2;
            try { code = execute_command(con, fields[0], fields[1], fields[2], std::stoi(fields[3]), fields[4], fields[5]); }
            catch (const std::exception& e) { event("input", fields[1], 2, e.what()); }
            event("complete", fields[0], code);
            interrupted = 0;
        }
    } else if (result == 0) {
        result = execute_command(con, cmd, argv[10], argv[11], std::atoi(argv[12]), argv[13], argv[14]);
    }
    IedConnection_close(con);
    IedConnection_destroy(con);
    TLSConfiguration_destroy(tls);
    return result;
}
