#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace cci::core {

/** Split space/comma-separated interface names from yaml `event_log.watch_ifaces`. */
inline std::vector<std::string> split_watch_ifaces(const std::string& raw) {
    std::vector<std::string> out;
    std::string cur;
    for (const char c : raw) {
        if (c == ',' || c == ' ' || c == '\t') {
            if (!cur.empty()) {
                out.push_back(cur);
                cur.clear();
            }
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) {
        out.push_back(cur);
    }
    return out;
}

/** Linux operstate: up / down / unknown / missing. Other OS: n/a. */
inline std::string read_iface_operstate(const std::string& iface) {
    if (iface.empty() || iface.find('/') != std::string::npos ||
        iface.find("..") != std::string::npos) {
        return "invalid";
    }
#if defined(_WIN32)
    (void)iface;
    return "n/a";
#else
    std::ifstream in("/sys/class/net/" + iface + "/operstate");
    if (!in) {
        return "missing";
    }
    std::string s;
    in >> s;
    return s.empty() ? "unknown" : s;
#endif
}

}  // namespace cci::core
