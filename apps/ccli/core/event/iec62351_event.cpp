#include "event/iec62351_event.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <map>
#include <sstream>
#include <limits>

namespace cci::core {
namespace {
const IecEventDefinition definitions[] = {
#include "event/iec62351_catalog.inc"
};

// Read JSON scalars without matching keys embedded inside escaped audit text.
// The existing RBAC audit contains a bounded array of role objects; preserve
// the first asserted role triplet. The full assertion stays in local detail.
std::map<std::string, std::string> audit_fields(const std::string& input) {
    std::map<std::string, std::string> fields;
    std::size_t p = 0;
    auto string = [&](std::string& value) {
        if (p >= input.size() || input[p++] != '"') return false;
        value.clear();
        while (p < input.size()) {
            char c = input[p++];
            if (c == '"') return true;
            if (c == '\\') {
                if (p == input.size()) return false;
                c = input[p++];
                if (c == 'n') c = '\n';
                else if (c == 'r') c = '\r';
                else if (c == 't') c = '\t';
                else if (c == 'u') {
                    auto codepoint = [&](unsigned& code) {
                        code = 0;
                        for (int i = 0; i < 4; ++i) {
                            if (p == input.size()) return false;
                            const char h = input[p++];
                            const int n = h >= '0' && h <= '9' ? h - '0' : h >= 'a' && h <= 'f' ? h - 'a' + 10 : h >= 'A' && h <= 'F' ? h - 'A' + 10 : -1;
                            if (n < 0) return false;
                            code = (code << 4) | static_cast<unsigned>(n);
                        }
                        return true;
                    };
                    unsigned cp;
                    if (!codepoint(cp)) return false;
                    if (cp >= 0xd800 && cp <= 0xdbff) {
                        if (input.compare(p, 2, "\\u") != 0) return false;
                        p += 2;
                        unsigned low;
                        if (!codepoint(low) || low < 0xdc00 || low > 0xdfff) return false;
                        cp = 0x10000 + ((cp - 0xd800) << 10) + low - 0xdc00;
                    } else if (cp >= 0xdc00 && cp <= 0xdfff) return false;
                    if (cp < 0x80) value += static_cast<char>(cp);
                    else {
                        if (cp >= 0x10000) value += static_cast<char>(0xf0 | (cp >> 18));
                        if (cp >= 0x800) value += static_cast<char>((cp >= 0x10000 ? 0x80 : 0xe0) | ((cp >> 12) & 0x3f));
                        value += static_cast<char>((cp >= 0x800 ? 0x80 : 0xc0) | ((cp >> 6) & 0x3f));
                        value += static_cast<char>(0x80 | (cp & 0x3f));
                    }
                    continue;
                } else if (c != '"' && c != '\\' && c != '/') return false;
            }
            value += c;
        }
        return false;
    };
    while (p < input.size()) {
        if (input[p] != '"') { ++p; continue; }
        std::string key, value;
        if (!string(key)) break;
        while (p < input.size() && std::isspace(static_cast<unsigned char>(input[p]))) ++p;
        if (p == input.size() || input[p++] != ':') continue;
        while (p < input.size() && std::isspace(static_cast<unsigned char>(input[p]))) ++p;
        if (p < input.size() && input[p] == '"') {
            if (!string(value)) break;
            fields.emplace(key, value);
        } else if (p < input.size() && (input[p] == '-' || std::isdigit(static_cast<unsigned char>(input[p])))) {
            const auto start = p++;
            while (p < input.size() && std::isdigit(static_cast<unsigned char>(input[p]))) ++p;
            fields.emplace(key, input.substr(start, p - start));
        }
    }
    return fields;
}

bool contains(const std::string& value, const char* part) { return value.find(part) != std::string::npos; }

const char* tls_id(int code, const std::string& detail) {
    if (contains(detail, "RSA key") || contains(detail, "key length"))
        return contains(detail, "less than 1024") || contains(detail, "< 1024") ? "IEC62351-3:2.13" : "IEC62351-3:2.11";
    if (contains(detail, "signature_algorithms extension unavailable")) return "IEC62351-3:1.15";
    switch (code) {
    case 16: return "IEC62351-3:1.1";
    case 3: return contains(detail, "peer certificate") ? "IEC62351-3:1.6" : "IEC62351-3:1.7";
    case 5: return "IEC62351-3:2.1";
    case 7: return "IEC62351-3:1.6";
    case 9: return "IEC62351-3:1.3";
    case 11: return "IEC62351-3:2.8";
    case 12: return "IEC62351-3:2.4";
    case 13: return "IEC62351-3:2.2";
    case 14: return "IEC62351-3:2.3";
    case 15: return "IEC62351-3:1.9";
    case 1003: return "IEC62351-3:2.14";
    case 1004: return "IEC62351-3:1.8";
    case 1005: return "IEC62351-3:1.5";
    case 1001:
        if (contains(detail, "signature")) return "IEC62351-9:4.4";
        return contains(detail, "expired") || contains(detail, "not yet valid") ?
            "IEC62351-3:2.6" : "IEC62351-3:2.5";
    default: break;
    }
    if (contains(detail, "renegotiation within") || contains(detail, "renegotiation interval")) return "IEC62351-3:1.11";
    if (code == 6) return "IEC62351-3:2.10";
    if (contains(detail, "signature algorithm") || contains(detail, "cryptographic policy")) return "IEC62351-3:2.9";
    if (contains(detail, "signature could not")) return "IEC62351-9:2.2";
    return "IEC62351-14:4.3";
}

std::string inferred_id(const EventRecord& event, const std::map<std::string, std::string>& fields) {
    auto value = [&](const char* key) { const auto it = fields.find(key); return it == fields.end() ? std::string{} : it->second; };
    const auto name = value("event");
    const auto& detail = event.detail;
    if (name == "TLS_SECURITY_EVENT" || contains(detail, "TLS_SECURITY_EVENT severity=")) {
        std::string code = value("object");
        const auto pos = detail.find(" code=");
        if (code.empty() && pos != std::string::npos) code = detail.substr(pos + 6);
        int number = 0;
        try { number = std::stoi(code); } catch (...) {}
        return tls_id(number, detail);
    }
    // Producers can supply a catalogue mnemonic without inventing a new ID.
    for (const auto& def : definitions)
        if (event.type == def.mnemonic || name == def.mnemonic) return def.id;
    if (name == "ASSOCIATION_AUTH_OK" || name == "E2E_AUTHORIZATION_OK") return "IEC62351-14:1.1";
    if (detail.rfind("auth_accept_", 0) == 0 || detail == "rbac_profile_a_auth_success") return "IEC62351-14:1.1";
    if (name == "ASSOCIATION_RELEASE" || detail == "E2E:ASSOCIATION_RELEASE") return "IEC62351-14:1.8";
    if (name == "POLICY_ACTIVATED") return "IEC62351-14:6.10";
    if (name == "POLICY_ACTIVATION_FAIL" || name == "TLS_CONFIGURATION_REJECTED") return "IEC62351-14:6.12";
    if (name == "ASSOCIATION_AUTH_FAIL" || name == "ASSOCIATION_BINDING_FAIL" || name == "E2E_AUTHORIZATION_FAIL" ||
        contains(detail, "auth_reject") || contains(detail, "rbac_deny")) return "IEC62351-14:4.4";
    if (name == "MMS_ACCESS_DENIED" || name == "DATASET_ACCESS_DENIED" || name == "REPORT_ACCESS_DENIED")
        return "IEC62351-14:5.4";
    // Version changes alone do not prove a successful firmware update.
    if (detail.rfind("power_on:", 0) == 0) return "IEC62351-14:8.4";
    // Reading existing TLS files is not certificate installation.
    if (detail.rfind("crash_recovered:", 0) == 0 || detail.rfind("unclean_restart:", 0) == 0)
        return "IEC62351-14:8.4";
    if (contains(detail, "client_connect") || detail == "E2E:ASSOCIATION_ACCEPT" ||
        detail == "link_up" || detail == "link_recovered") return "IEC62351-14:4.1";
    if (contains(detail, "comms_loss") || detail.rfind("link_down:", 0) == 0) return "IEC62351-14:4.2";
    if (detail == "E2E:APPLICATION_REJECT" || detail == "E2E:APPLICATION_ABORT") return "IEC62351-14:4.3";
    if (detail.rfind("E2E:ACSE_REJECT:", 0) == 0) {
        if (contains(detail, "calling AP-title")) return "IEC62351-4:15.7";
        if (contains(detail, "calling AE-qualifier")) return "IEC62351-4:15.9";
        if (contains(detail, "called AP-title")) return "IEC62351-4:15.11";
        if (contains(detail, "called AE-qualifier")) return "IEC62351-4:15.13";
        if (contains(detail, "application context")) return "IEC62351-4:15.5";
        if (contains(detail, "mechanism")) return "IEC62351-4:15.15";
        return "IEC62351-4:15.4";
    }
    if (detail.rfind("E2E:OSI_REJECT:", 0) == 0) return "IEC62351-4:15.1";
    if (detail.rfind("E2E:REJECT:", 0) == 0) return "IEC62351-14:4.3";
    if (detail == "E2E:COMPATIBILITY_ASSOCIATION_REJECTED") return "IEC62351-14:4.3";
    if (detail.rfind("E2E:ALARM:", 0) == 0) return "IEC62351-14:4.4";
    if (detail.rfind("E2E:FAILURE:", 0) == 0) return "IEC62351-14:5.4";
    return {};
}

bool printable(const std::string& value, std::size_t max, bool space = false) {
    return !value.empty() && value.size() <= max && std::all_of(value.begin(), value.end(), [space](unsigned char c) {
        return c >= (space ? 32 : 33) && c <= 126;
    });
}
bool utf8(const std::string& value, std::size_t max) {
    if (value.size() > max) return false;
    for (std::size_t p = 0; p < value.size();) {
        const auto c = static_cast<unsigned char>(value[p++]);
        if (c < 0x80) { if (c < 32 || c == 127) return false; continue; }
        const int more = c >= 0xc2 && c <= 0xdf ? 1 : c >= 0xe0 && c <= 0xef ? 2 : c >= 0xf0 && c <= 0xf4 ? 3 : -1;
        if (more < 0 || p + more > value.size()) return false;
        const auto next = static_cast<unsigned char>(value[p]);
        if ((c == 0xe0 && next < 0xa0) || (c == 0xed && next >= 0xa0) ||
            (c == 0xf0 && next < 0x90) || (c == 0xf4 && next >= 0x90)) return false;
        for (int i = 0; i < more; ++i) if ((static_cast<unsigned char>(value[p++]) & 0xc0) != 0x80) return false;
    }
    return true;
}
std::string escape_sd(const std::string& value) {
    std::string out;
    for (char c : value) {
        if (c == '"' || c == '\\' || c == ']') out += '\\';
        out += c;
    }
    return out;
}
} // namespace

const IecEventDefinition* iec_event_definition(const std::string& id) {
    for (const auto& def : definitions) if (id == def.id) return &def;
    return nullptr;
}

bool prepare_iec_event(EventRecord& event, const std::string& source, const std::string& app_name) {
    const auto fields = audit_fields(event.detail);
    if (event.iec_id.empty()) event.iec_id = inferred_id(event, fields);
    const auto* def = iec_event_definition(event.iec_id);
    if (!def) return false;
    event.mnemonic = def->mnemonic;
    event.severity = def->severity;
    event.text = def->text;
    if (event.source.empty()) event.source = source;
    if (event.app_name.empty()) event.app_name = app_name;
    auto field = [&](const char* key) { const auto it = fields.find(key); return it == fields.end() ? std::string{} : it->second; };
    if (event.peer_info.empty()) event.peer_info = field("peer");
    if (event.peer_info.empty()) {
        const auto reason = field("reason").empty() ? event.detail : field("reason");
        const auto pos = reason.find(" peer=");
        if (pos != std::string::npos) event.peer_info = reason.substr(pos + 6, reason.find(' ', pos + 6) - pos - 6);
    }
    if (event.user_id.empty()) {
        event.user_id = field("subject");
        // A full DN may exceed UsrID's limit. Use a stable 128-bit certificate
        // identifier; the complete subject/fingerprint remain in local detail.
        if (event.user_id.size() > 32) event.user_id = field("sha256").substr(0, 32);
    }
    if (event.role.empty() && !field("id").empty())
        event.role = field("id") + ":" + field("revision") + ":" + field("definition");
    return valid_iec_event(event);
}

bool valid_iec_event(const EventRecord& event) {
    const auto* def = iec_event_definition(event.iec_id);
    if (!def || event.mnemonic != def->mnemonic || event.severity != def->severity || event.text != def->text ||
        !printable(event.source, 255) || !printable(event.app_name, 48) ||
        !utf8(event.text, 128) || event.timestamp_ms < 0 || event.timestamp_ms > 253402300799999LL ||
        event.source.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.:-/") != std::string::npos ||
        (!event.peer_info.empty() && !printable(event.peer_info, 64, true)) ||
        !utf8(event.user_id, 32) || !utf8(event.role, 29)) return false;
    std::map<std::string, bool> used;
    for (const auto& param : event.parameters) {
        if (param.first.rfind("P.", 0) != 0 || !printable(param.first, 32) ||
            param.first.find_first_of("=\" ]") != std::string::npos || !utf8(param.second, 2048) ||
            !used.emplace(param.first, true).second) return false;
    }
    return true;
}

std::string format_rfc5424(const EventRecord& original, const std::string& hostname, const std::string& app_name) {
    auto event = original;
    if (!prepare_iec_event(event, hostname, app_name)) return {};
    const int severity = event.severity == "alarm" ? 1 : event.severity == "error" ? 3 : event.severity == "warning" ? 4 : 5;
    const auto seconds = static_cast<std::time_t>(event.timestamp_ms / 1000);
    std::tm utc{};
#if defined(_WIN32)
    if (gmtime_s(&utc, &seconds) != 0) return {};
#else
    if (!gmtime_r(&seconds, &utc)) return {};
#endif
    char timestamp[80];
    std::snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
        utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday, utc.tm_hour, utc.tm_min, utc.tm_sec,
        static_cast<int>(event.timestamp_ms % 1000));
    std::ostringstream out;
    out << '<' << 104 + severity << ">1 " << timestamp << ' ' << event.source << ' ' << event.app_name
        << " - IEC62351-14:1 [62351-14@41912";
    auto param = [&](const char* name, const std::string& value) { out << ' ' << name << "=\"" << escape_sd(value) << '"'; };
    param("ID", event.iec_id);
    param("MNEMONIC", event.mnemonic);
    param("Text", event.text);
    param("SqNum", std::to_string(event.sequence));
    if (!event.peer_info.empty()) param("PeerInfo", event.peer_info);
    if (!event.user_id.empty()) param("UsrID", event.user_id);
    if (!event.role.empty()) param("Role", event.role);
    for (const auto& p : event.parameters) param(p.first.c_str(), p.second);
    out << ']';
    return out.str();
}

std::string frame_rfc5425(const std::string& message) {
    if (message.empty()) return {};
    return std::to_string(message.size()) + ' ' + message;
}
} // namespace cci::core
