#include "event/event_store.hpp"

#include <cerrno>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

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
    os << "{\"ts_ms\":" << ev.timestamp_ms << ",\"type\":\"" << json_escape(ev.type)
       << "\",\"detail\":\"" << json_escape(ev.detail) << "\"}";
    return os.str();
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

EventStore::EventStore(EventLogConfig cfg) : cfg_(std::move(cfg)) {
    if (cfg_.enabled && !cfg_.path.empty()) {
        (void)ensure_parent_dir(cfg_.path);
        load_from_file();
    }
}

void EventStore::append(EventRecord ev) {
    const bool at_cap = ring_.size() >= kMaxEvents;
    ring_.append(std::move(ev));
    if (!cfg_.enabled || cfg_.path.empty()) {
        return;
    }
    if (at_cap) {
        rewrite_file_from_ring();
        return;
    }
    persist_line(ring_.recent(1).back());
}

std::vector<EventRecord> EventStore::recent(const std::size_t max_count) const {
    return ring_.recent(max_count);
}

std::size_t EventStore::size() const {
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

bool EventStore::run_wrap_test(const std::string& path) {
    EventStore store(EventLogConfig{true, path});
    for (std::size_t i = 0; i < kMaxEvents + 1; ++i) {
        store.append({static_cast<std::int64_t>(1000 + i), "wrap_test",
                      "event_" + std::to_string(i)});
    }
    if (store.ring().size() != kMaxEvents) {
        return false;
    }
    const auto tail = store.recent(1);
    if (tail.empty() || tail.back().detail != "event_" + std::to_string(kMaxEvents)) {
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
    EventStore store(EventLogConfig{true, path});
    const auto rows = store.recent(max_count);
    if (json_out) {
        out << "[\n";
        for (std::size_t i = 0; i < rows.size(); ++i) {
            const auto& ev = rows[i];
            out << "  {\"ts_o14\":\"" << format_o14_timestamp(ev.timestamp_ms) << "\",\"type\":\""
                << json_escape(ev.type) << "\",\"detail\":\"" << json_escape(ev.detail) << "\"}";
            if (i + 1 < rows.size()) {
                out << ',';
            }
            out << '\n';
        }
        out << "]\n";
        return 0;
    }
    for (const auto& ev : rows) {
        out << format_o14_timestamp(ev.timestamp_ms) << ' ' << ev.type << ' ' << ev.detail
            << '\n';
    }
    return 0;
}

}  // namespace cci::core
