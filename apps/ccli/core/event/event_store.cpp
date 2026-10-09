#include "event/event_store.hpp"
#include "event/o14_catalog.hpp"

#include <cerrno>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace cci::core {

namespace {

std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (const char c : s) {
        if (c == '"' || c == '\\') {
            out += '\\';
        }
        out += c;
    }
    return out;
}

bool parse_jsonl_record(const std::string& line, EventRecord& out) {
    if (line.empty() || line.front() != '{') {
        return false;
    }
    auto find_num = [&](const char* key, std::int64_t& val) -> bool {
        const std::string needle = std::string("\"") + key + "\":";
        const auto pos = line.find(needle);
        if (pos == std::string::npos) {
            return false;
        }
        val = std::strtoll(line.c_str() + pos + needle.size(), nullptr, 10);
        return true;
    };
    auto find_str = [&](const char* key, std::string& val) -> bool {
        const std::string needle = std::string("\"") + key + "\":\"";
        const auto pos = line.find(needle);
        if (pos == std::string::npos) {
            return false;
        }
        const auto start = pos + needle.size();
        const auto end = line.find('"', start);
        if (end == std::string::npos) {
            return false;
        }
        val = line.substr(start, end - start);
        return true;
    };
    if (!find_num("ts_ms", out.timestamp_ms)) {
        return false;
    }
    if (!find_str("type", out.type)) {
        return false;
    }
    if (!find_str("detail", out.detail)) {
        out.detail.clear();
    }
    if (!find_str("category", out.category)) {
        out.category = o14::category_for(out.type, out.detail);
    }
    return true;
}

bool ensure_parent_dir(const std::string& path) {
    const auto slash = path.rfind('/');
    if (slash == std::string::npos) {
        return true;
    }
    const std::string dir = path.substr(0, slash);
    struct stat st {};
    if (stat(dir.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    if (mkdir(dir.c_str(), 0750) == 0) {
        return true;
    }
    return errno == EEXIST;
}

std::string record_to_jsonl(const EventRecord& ev) {
    std::ostringstream os;
    os << "{\"ts_ms\":" << ev.timestamp_ms << ",\"ts_o14\":\""
       << format_o14_timestamp(ev.timestamp_ms) << "\",\"category\":\""
       << json_escape(ev.category) << "\",\"type\":\"" << json_escape(ev.type)
       << "\",\"detail\":\"" << json_escape(ev.detail) << "\"}";
    return os.str();
}

bool is_auth_warning(const EventRecord& ev) {
    return ev.detail.find("auth_reject") != std::string::npos ||
           ev.detail.find("rbac_deny") != std::string::npos ||
           ev.detail.find("reject") != std::string::npos;
}

}  // namespace

std::string format_o14_timestamp(const std::int64_t epoch_ms) {
    const std::time_t sec = static_cast<std::time_t>(epoch_ms / 1000);
    std::tm tm_buf{};
#if defined(_WIN32)
    gmtime_s(&tm_buf, &sec);
#else
    gmtime_r(&sec, &tm_buf);
#endif
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d", tm_buf.tm_year + 1900,
                  tm_buf.tm_mon + 1, tm_buf.tm_mday, tm_buf.tm_hour, tm_buf.tm_min,
                  tm_buf.tm_sec);
    return buf;
}

std::string format_rfc5424(const EventRecord& ev, const std::string& hostname,
                           const std::string& app_name) {
    /* facility local0=16; warning=4 / notice=5 */
    const int pri = 16 * 8 + (is_auth_warning(ev) ? 4 : 5);
    std::ostringstream os;
    os << '<' << pri << ">1 " << format_o14_timestamp(ev.timestamp_ms) << ' '
       << (hostname.empty() ? "-" : hostname) << ' ' << (app_name.empty() ? "ccli" : app_name)
       << " - " << (ev.category.empty() ? "-" : ev.category) << " - " << ev.type << ' '
       << ev.detail;
    return os.str();
}

std::string firmware_marker_path(const std::string& log_path) {
    const auto slash = log_path.rfind('/');
    if (slash == std::string::npos) {
        return "last_firmware.txt";
    }
    return log_path.substr(0, slash + 1) + "last_firmware.txt";
}

FirmwareBootNote EventStore::record_firmware_boot(const std::string& log_path,
                                                  const std::string& version) {
    FirmwareBootNote note;
    const std::string marker = firmware_marker_path(log_path);
    (void)ensure_parent_dir(marker);
    std::string prev;
    {
        std::ifstream in(marker);
        if (in) {
            std::getline(in, prev);
        }
    }
    if (prev.empty()) {
        note.kind = FirmwareBootKind::First;
        note.detail = "firmware_boot:" + version;
    } else if (prev == version) {
        note.kind = FirmwareBootKind::Unchanged;
        note.detail = "firmware_boot:" + version;
    } else {
        note.kind = FirmwareBootKind::Updated;
        note.detail = "firmware_update:" + prev + "->" + version;
    }
    std::ofstream out(marker, std::ios::trunc);
    if (out) {
        out << version << '\n';
    }
    return note;
}

EventStore::EventStore(EventLogConfig cfg) : cfg_(std::move(cfg)) {
    if (cfg_.enabled && !cfg_.path.empty()) {
        (void)ensure_parent_dir(cfg_.path);
        load_from_file();
        apply_file_mode();
    }
}

void EventStore::append(EventRecord ev) {
    if (ev.category.empty()) {
        ev.category = o14::category_for(ev.type, ev.detail);
    }
    EventRecord persisted;
    bool wrapped = false;
    {
        std::lock_guard<std::mutex> lock(mu_);
        const bool at_cap = ring_.size() >= kMaxEvents;
        ring_.append(ev);
        persisted = ring_.recent(1).empty() ? ev : ring_.recent(1).back();
        if (cfg_.enabled && !cfg_.path.empty()) {
            if (at_cap) {
                rewrite_file_from_ring();
                wrapped = true;
            } else {
                persist_line(persisted);
            }
            apply_file_mode();
        }
    }
    syslog_send(persisted);
    if (wrapped && persisted.type != "system") {
        EventRecord wrap{persisted.timestamp_ms, "system", "log_wrap:oldest_dropped",
                         o14::kLogger};
        syslog_send(wrap);
    }
}

std::vector<EventRecord> EventStore::recent(const std::size_t max_count) const {
    std::lock_guard<std::mutex> lock(mu_);
    return ring_.recent(max_count);
}

std::size_t EventStore::size() const {
    std::lock_guard<std::mutex> lock(mu_);
    return ring_.size();
}

void EventStore::load_from_file() {
    std::ifstream in(cfg_.path);
    if (!in) {
        return;
    }
    std::vector<EventRecord> loaded;
    std::string line;
    while (std::getline(in, line)) {
        EventRecord rec{};
        if (parse_jsonl_record(line, rec)) {
            loaded.push_back(std::move(rec));
        }
    }
    if (loaded.size() > kMaxEvents) {
        loaded.erase(loaded.begin(),
                     loaded.end() - static_cast<std::ptrdiff_t>(kMaxEvents));
    }
    for (const auto& rec : loaded) {
        ring_.append(rec);
    }
}

void EventStore::persist_line(const EventRecord& ev) {
    if (!ensure_parent_dir(cfg_.path)) {
        return;
    }
    std::ofstream out(cfg_.path, std::ios::app);
    if (!out) {
        return;
    }
    out << record_to_jsonl(ev) << '\n';
}

void EventStore::rewrite_file_from_ring() {
    const auto all = ring_.recent(kMaxEvents);
    std::ofstream out(cfg_.path, std::ios::trunc);
    if (!out) {
        return;
    }
    for (const auto& ev : all) {
        out << record_to_jsonl(ev) << '\n';
    }
}

void EventStore::apply_file_mode() const {
#if !defined(_WIN32)
    if (cfg_.path.empty()) {
        return;
    }
    (void)chmod(cfg_.path.c_str(), 0640);
#endif
}

void EventStore::syslog_send(const EventRecord& ev) const {
    if (!cfg_.syslog_enabled || cfg_.syslog_host.empty() || cfg_.syslog_port <= 0) {
        return;
    }
#if defined(_WIN32)
    (void)ev;
#else
    const std::string msg = format_rfc5424(ev, "-", "ccli");
    const int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return;
    }
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(cfg_.syslog_port));
    if (::inet_pton(AF_INET, cfg_.syslog_host.c_str(), &addr.sin_addr) != 1) {
        ::close(fd);
        return;
    }
    (void)::sendto(fd, msg.data(), msg.size(), 0, reinterpret_cast<sockaddr*>(&addr),
                   sizeof(addr));
    ::close(fd);
#endif
}

bool EventStore::run_wrap_test(const std::string& path) {
    EventStore store(EventLogConfig{true, path, false, "127.0.0.1", 514});
    for (std::size_t i = 0; i < kMaxEvents + 1; ++i) {
        store.append({static_cast<std::int64_t>(1000 + i), "wrap_test",
                      "event_" + std::to_string(i), ""});
    }
    if (store.size() != kMaxEvents) {
        return false;
    }
    const auto tail = store.recent(1);
    if (tail.empty() || tail.back().detail != "event_" + std::to_string(kMaxEvents)) {
        return false;
    }
    if (tail.back().category.empty()) {
        return false;
    }
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::size_t lines = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            ++lines;
        }
    }
    return lines == kMaxEvents;
}

int EventStore::dump_to_stream(std::ostream& out, const std::string& path,
                               const std::size_t max_count, const bool json_out) {
    EventStore store(EventLogConfig{true, path, false, "127.0.0.1", 514});
    const auto rows = store.recent(max_count);
    if (json_out) {
        out << "[\n";
        for (std::size_t i = 0; i < rows.size(); ++i) {
            const auto& ev = rows[i];
            out << "  {\"ts_o14\":\"" << format_o14_timestamp(ev.timestamp_ms)
                << "\",\"category\":\"" << json_escape(ev.category) << "\",\"type\":\""
                << json_escape(ev.type) << "\",\"detail\":\"" << json_escape(ev.detail)
                << "\"}";
            if (i + 1 < rows.size()) {
                out << ',';
            }
            out << '\n';
        }
        out << "]\n";
        return 0;
    }
    for (const auto& ev : rows) {
        out << format_o14_timestamp(ev.timestamp_ms) << ' ' << ev.category << ' ' << ev.type
            << ' ' << ev.detail << '\n';
    }
    return 0;
}

}  // namespace cci::core
