#include "event/event_store.hpp"
#include "event/o14_catalog.hpp"
#include "event/iec62351_event.hpp"
#include "event/platform_event_monitor.hpp"
#include "event/syslog_sender.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>
#include <limits>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#if !defined(_WIN32)
#include <fcntl.h>
#if defined(__linux__)
#include <sys/file.h>
#endif
#include <unistd.h>
#endif

namespace cci::core {

namespace {

bool append_platform_event(void* context, EventRecord event) {
    return static_cast<EventStore*>(context)->append(std::move(event));
}

std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    constexpr char hex[] = "0123456789abcdef";
    for (const unsigned char c : s) {
        if (c == '"' || c == '\\') {
            out += '\\';
        }
        if (c < 32) {
            out += "\\u00";
            out += hex[c >> 4];
            out += hex[c & 15];
        } else {
            out += static_cast<char>(c);
        }
    }
    return out;
}

std::string audit_hex(const std::string& value) {
    constexpr char digits[] = "0123456789abcdef";
    std::string encoded;
    encoded.reserve(value.size() * 2);
    for (const unsigned char c : value) {
        encoded += digits[c >> 4];
        encoded += digits[c & 0x0f];
    }
    return encoded;
}

bool ensure_parent_dir(const std::string& path);

#if defined(__linux__)
bool append_restricted_sidecar(const std::string& sidecar, const std::string& line) {
    if (sidecar.empty() || line.empty() || !ensure_parent_dir(sidecar)) return false;
    const int fd = open(sidecar.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC | O_NOFOLLOW, 0600);
    if (fd < 0) return false;
    struct stat st{};
    const bool safe = fstat(fd, &st) == 0 && S_ISREG(st.st_mode) &&
                      st.st_uid == getuid() && (st.st_mode & 0077) == 0;
    bool ok = safe && flock(fd, LOCK_EX) == 0;
    std::size_t written = 0;
    while (ok && written < line.size()) {
        const auto count = write(fd, line.data() + written, line.size() - written);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) { ok = false; break; }
        written += static_cast<std::size_t>(count);
    }
    if (ok) ok = fsync(fd) == 0;
    (void)flock(fd, LOCK_UN);
    close(fd);
    if (!ok) return false;
    const auto slash = sidecar.find_last_of('/');
    const auto parent = slash == std::string::npos ? std::string(".") :
                        slash == 0 ? std::string("/") : sidecar.substr(0, slash);
    const int dir = open(parent.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (dir < 0) return false;
    const bool synced = fsync(dir) == 0;
    close(dir);
    return synced;
}
#endif

bool json_string(const std::string& text, std::size_t& pos, std::string& value) {
    if (pos >= text.size() || text[pos++] != '"') return false;
    value.clear();
    auto hex4 = [&](unsigned& result) {
        result = 0;
        for (int i = 0; i < 4; ++i) {
            if (pos >= text.size()) return false;
            const char c = text[pos++];
            const int digit = c >= '0' && c <= '9' ? c - '0' :
                              c >= 'a' && c <= 'f' ? c - 'a' + 10 :
                              c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
            if (digit < 0) return false;
            result = (result << 4) | static_cast<unsigned>(digit);
        }
        return true;
    };
    while (pos < text.size()) {
        unsigned char c = text[pos++];
        if (c == '"') return true;
        if (c < 32) return false;
        if (c != '\\') { value += static_cast<char>(c); continue; }
        if (pos == text.size()) return false;
        switch (text[pos++]) {
        case '"': value += '"'; break;
        case '\\': value += '\\'; break;
        case '/': value += '/'; break;
        case 'b': value += '\b'; break;
        case 'f': value += '\f'; break;
        case 'n': value += '\n'; break;
        case 'r': value += '\r'; break;
        case 't': value += '\t'; break;
        case 'u': {
            unsigned cp;
            if (!hex4(cp)) return false;
            if (cp >= 0xd800 && cp <= 0xdbff) {
                if (pos + 2 > text.size() || text.compare(pos, 2, "\\u") != 0) return false;
                pos += 2;
                unsigned low;
                if (!hex4(low) || low < 0xdc00 || low > 0xdfff) return false;
                cp = 0x10000 + ((cp - 0xd800) << 10) + low - 0xdc00;
            } else if (cp >= 0xdc00 && cp <= 0xdfff) return false;
            if (cp < 0x80) value += static_cast<char>(cp);
            else {
                if (cp >= 0x10000) value += static_cast<char>(0xf0 | (cp >> 18));
                if (cp >= 0x800) value += static_cast<char>((cp >= 0x10000 ? 0x80 : 0xe0) | ((cp >> 12) & 0x3f));
                value += static_cast<char>((cp >= 0x800 ? 0x80 : 0xc0) | ((cp >> 6) & 0x3f));
                value += static_cast<char>(0x80 | (cp & 0x3f));
            }
            break;
        }
        default: return false;
        }
    }
    return false;
}

bool parse_jsonl_record(const std::string& line, EventRecord& out) {
    std::size_t pos = 0;
    const auto space = [&] { while (pos < line.size() && line[pos] != '\0' && std::strchr(" \t\r\n", line[pos])) ++pos; };
    space();
    if (pos == line.size() || line[pos++] != '{') return false;
    bool timestamp = false, type = false;
    out = {};
    for (;;) {
        space();
        std::string key, value;
        if (!json_string(line, pos, key)) return false;
        space();
        if (pos == line.size() || line[pos++] != ':') return false;
        space();
        if (pos < line.size() && line[pos] == '"') {
            if (!json_string(line, pos, value)) return false;
            if (key == "type") { out.type = value; type = true; }
            else if (key == "detail") out.detail = value;
            else if (key == "category") out.category = value;
            else if (key == "iec_id") out.iec_id = value;
            else if (key == "mnemonic") out.mnemonic = value;
            else if (key == "severity") out.severity = value;
            else if (key == "text") out.text = value;
            else if (key == "source") out.source = value;
            else if (key == "app_name") out.app_name = value;
            else if (key == "peer_info") out.peer_info = value;
            else if (key == "user_id") out.user_id = value;
            else if (key == "role") out.role = value;
            else if (key.rfind("P.", 0) == 0) out.parameters.emplace_back(key, value);
        } else {
            if (pos == line.size() || (line[pos] != '-' && (line[pos] < '0' || line[pos] > '9'))) return false;
            char* end = nullptr;
            errno = 0;
            const auto number = std::strtoll(line.c_str() + pos, &end, 10);
            if (errno || end == line.c_str() + pos) return false;
            pos = static_cast<std::size_t>(end - line.c_str());
            if (key == "ts_ms") { out.timestamp_ms = number; timestamp = true; }
            else if (key == "sequence") {
                if (number < 0 || number > std::numeric_limits<std::uint32_t>::max()) return false;
                out.sequence = static_cast<std::uint32_t>(number);
            }
        }
        space();
        if (pos == line.size()) return false;
        const char delimiter = line[pos++];
        if (delimiter == '}') break;
        if (delimiter != ',') return false;
    }
    space();
    if (pos != line.size() || !timestamp || !type) return false;
    if (out.category.empty()) out.category = o14::category_for(out.type, out.detail);
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
       << "\",\"detail\":\"" << json_escape(ev.detail) << "\",\"sequence\":" << ev.sequence;
    const auto field = [&](const char* key, const std::string& value) {
        os << ",\"" << key << "\":\"" << json_escape(value) << '"';
    };
    if (!ev.iec_id.empty()) {
        field("iec_id", ev.iec_id);
        field("mnemonic", ev.mnemonic);
        field("severity", ev.severity);
        field("text", ev.text);
        field("source", ev.source);
        field("app_name", ev.app_name);
        field("peer_info", ev.peer_info);
        field("user_id", ev.user_id);
        field("role", ev.role);
        for (const auto& p : ev.parameters) field(p.first.c_str(), p.second);
    }
    os << '}';
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
    char buf[80];
    std::snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d", tm_buf.tm_year + 1900,
                  tm_buf.tm_mon + 1, tm_buf.tm_mday, tm_buf.tm_hour, tm_buf.tm_min,
                  tm_buf.tm_sec);
    return buf;
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

EventStore::EventStore(EventLogConfig cfg, bool read_only)
    : cfg_(std::move(cfg)), read_only_(read_only) {
    if (cfg_.syslog_source.empty()) {
        char hostname[256]{};
#if !defined(_WIN32)
        if (::gethostname(hostname, sizeof(hostname) - 1) == 0) cfg_.syslog_source = hostname;
#else
        const char* computer = std::getenv("COMPUTERNAME");
        if (computer) cfg_.syslog_source = computer;
#endif
        if (cfg_.syslog_source.empty()) cfg_.syslog_source = "localhost";
    }
    if (cfg_.enabled && !cfg_.path.empty()) {
        if (!read_only_ && !ensure_parent_dir(cfg_.path)) persistence_error("create log directory");
        load_from_file();
    }
    if (!read_only_ && cfg_.syslog_enabled)
        sender_ = std::make_unique<SyslogSender>(cfg_, [this](const std::string& reason) { syslog_failure(reason); });
    if (!read_only_ && cfg_.enabled && cfg_.platform_audit_enabled && platform_event_monitor_available()) {
        const auto cursor_path = cfg_.path.empty() ? std::string{} :
            cfg_.path + ".firmware-updater.cursor";
        platform_monitor_ = std::make_unique<PlatformEventMonitor>(
            this, append_platform_event, cfg_.platform_updater_audit_path, cursor_path,
            cfg_.platform_credential_paths);
    }
}

EventStore::~EventStore() {
    platform_monitor_.reset(); // join the producer before destroying the store
    sender_.reset();
}

bool EventStore::audit_log_read(const std::string& path, bool success) {
#if defined(__linux__)
    const std::string line = std::to_string(std::time(nullptr)) + "|" +
        (success ? "LOG_READ_SUCCESS" : "LOG_READ_FAIL") + "|uid=" +
        std::to_string(getuid()) + "\n";
    return append_restricted_sidecar(path + ".read-audit", line);
#else
    (void)path; (void)success; return false;
#endif
}

bool EventStore::audit_platform_auth(const std::string& path, const EventRecord& event) {
#if defined(__linux__)
    if (path.empty() || !iec_event_definition(event.iec_id) ||
        event.timestamp_ms < 0 || event.timestamp_ms > 253402300799999LL ||
        event.app_name.size() > 64 || event.user_id.size() > 128 || event.peer_info.size() > 128)
        return false;
    const std::string line = std::to_string(event.timestamp_ms) + "|PLATFORM_AUTH|" + event.iec_id +
        "|" + audit_hex(event.app_name) + "|" + audit_hex(event.user_id) + "|" +
        audit_hex(event.peer_info) + "\n";
    return append_restricted_sidecar(path + ".platform-auth-audit", line);
#else
    (void)path; (void)event; return false;
#endif
}

bool EventStore::append(EventRecord ev) { return append_impl(std::move(ev), true); }

bool EventStore::append_impl(EventRecord ev, bool forward) {
    if (read_only_) return false;
    if (ev.category.empty()) {
        ev.category = o14::category_for(ev.type, ev.detail);
    }
    const bool prepared = prepare_iec_event(ev, cfg_.syslog_source, cfg_.syslog_app_name);
    bool ok = ev.iec_id.empty() || prepared;
    // Retain malformed producer input as a raw local audit entry, never as a
    // purportedly valid IEC record or a remotely transmitted message.
    if (!ok) ev.iec_id.clear();
    bool queue_failed = false;
    {
        std::lock_guard<std::mutex> lock(mu_);
        last_sequence_ = last_sequence_ == std::numeric_limits<std::uint32_t>::max() ? 1 : last_sequence_ + 1;
        ev.sequence = last_sequence_;
        ring_.append(ev);
        if (cfg_.enabled && !cfg_.path.empty()) {
            persistence_ok_ = persist_line(ev);
            ok = persistence_ok_ && ok;
        }
        // Enqueue under the same lock as numbering/persistence: concurrent
        // producers must reach the collector in the assigned sequence order.
        if (forward && prepared && sender_) {
            queue_failed = !sender_->submit(format_rfc5424(ev, ev.source, ev.app_name));
        }
    }
    if (queue_failed) syslog_failure("bounded TLS syslog queue full; event retained locally");
    return ok;
}

bool EventStore::syslog_ok() const {
    std::lock_guard<std::mutex> lock(mu_);
    return syslog_ok_;
}

void EventStore::syslog_failure(const std::string& reason) {
    bool report;
    {
        std::lock_guard<std::mutex> lock(mu_);
        report = syslog_ok_;
        syslog_ok_ = false;
    }
    if (!report) return; // Latch one local alarm; never recursively send it.
    std::fprintf(stderr, "event_store: TLS syslog unavailable: %s\n", reason.c_str());
    EventRecord event{std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count(), "security", reason, o14::kSecurity};
    event.iec_id = "IEC62351-14:9.5";
    append_impl(std::move(event), false);
}

bool EventStore::persistence_ok() const {
    std::lock_guard<std::mutex> lock(mu_);
    return persistence_ok_;
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
    // A crash between rename and creation of the next segment leaves .1 intact.
    // Stream both segments into the bounded ring, oldest first.
    load_segment(cfg_.path + ".1", false);
    load_segment(cfg_.path, true);
}

void EventStore::load_segment(const std::string& path, bool current) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        if (errno != ENOENT) persistence_error("read log segment");
        return;
    }
#if !defined(_WIN32)
    if (!read_only_ && chmod(path.c_str(), 0640) != 0) persistence_error("set log permissions");
#endif
    std::string line;
    while (std::getline(in, line)) {
        if (current) {
            ++current_lines_;
            needs_separator_ = in.eof();
        }
        EventRecord rec{};
        if (parse_jsonl_record(line, rec)) {
            if (rec.sequence) last_sequence_ = rec.sequence;
            ring_.append(std::move(rec));
        }
    }
    if (in.bad()) persistence_error("read log segment");
}

bool EventStore::persistence_error(const char* operation) {
    const int error = errno ? errno : EIO;
    persistence_ok_ = false;
    std::fprintf(stderr, "event_store: %s failed for %s: %s; audit persistence degraded\n",
                 operation, cfg_.path.c_str(), std::strerror(error));
    return false;
}

bool EventStore::persist_line(const EventRecord& ev) {
    if (!ensure_parent_dir(cfg_.path)) return persistence_error("create log directory");
    if (current_lines_ >= kMaxEvents) {
        const std::string previous = cfg_.path + ".1";
#if defined(_WIN32)
        // Windows rename cannot replace an existing file. The full current
        // segment still retains the latest 2048 records if interrupted here.
        if (std::remove(previous.c_str()) != 0 && errno != ENOENT)
            return persistence_error("remove expired log segment");
#endif
        // POSIX rename atomically replaces the expired previous segment. No
        // retained records are copied, serialized again, or truncated.
        if (std::rename(cfg_.path.c_str(), previous.c_str()) != 0)
            return persistence_error("rotate log segment");
        current_lines_ = 0;
        needs_separator_ = false;
    }

    const std::string line = (needs_separator_ ? "\n" : "") + record_to_jsonl(ev) + '\n';
#if !defined(_WIN32)
    const int fd = ::open(cfg_.path.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0640);
    if (fd < 0) return persistence_error("open log segment");
    if (::fchmod(fd, 0640) != 0) {
        const int error = errno;
        ::close(fd);
        errno = error;
        return persistence_error("set log permissions");
    }
    std::size_t offset = 0;
    while (offset < line.size()) {
        const auto written = ::write(fd, line.data() + offset, line.size() - offset);
        if (written < 0 && errno == EINTR) continue;
        if (written <= 0) {
            const int error = written < 0 ? errno : EIO;
            ::close(fd);
            needs_separator_ = true;
            errno = error;
            return persistence_error("append log record");
        }
        offset += static_cast<std::size_t>(written);
    }
    if (::fsync(fd) != 0) {
        const int error = errno;
        ::close(fd);
        errno = error;
        needs_separator_ = true;
        return persistence_error("sync log record");
    }
    if (::close(fd) != 0) {
        needs_separator_ = true;
        return persistence_error("close log segment");
    }
    if (current_lines_ == 0) {
        const auto slash = cfg_.path.rfind('/');
        const std::string directory = slash == std::string::npos ? "." :
            slash == 0 ? "/" : cfg_.path.substr(0, slash);
        const int parent = ::open(directory.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
        if (parent < 0) return persistence_error("open log directory for sync");
        const int result = ::fsync(parent);
        const int error = errno;
        ::close(parent);
        if (result != 0) { errno = error; return persistence_error("sync log directory"); }
    }
#else
    std::ofstream out(cfg_.path, std::ios::binary | std::ios::app);
    out << line;
    out.close();
    if (!out) { needs_separator_ = true; return persistence_error("append log record"); }
#endif
    ++current_lines_;
    needs_separator_ = false;
    return true;
}

bool EventStore::run_wrap_test(const std::string& path) {
    EventLogConfig isolated;
    isolated.path = path;
    isolated.platform_audit_enabled = false;
    EventStore store(isolated);
    for (std::size_t i = 0; i < kMaxEvents + 1; ++i) {
        if (!store.append({static_cast<std::int64_t>(1000 + i), "wrap_test",
                           "event_" + std::to_string(i), ""})) return false;
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
    EventStore reloaded(isolated, true);
    const auto all = reloaded.recent(kMaxEvents);
    return reloaded.persistence_ok() && all.size() == kMaxEvents &&
           all.front().detail == "event_1" && all.back().detail == "event_" + std::to_string(kMaxEvents);
}

int EventStore::dump_to_stream(std::ostream& out, const std::string& path,
                               const std::size_t max_count, const bool json_out) {
    EventLogConfig reader;
    reader.path = path;
    EventStore store(reader, true);
    if (!store.persistence_ok()) return 1;
    const auto rows = store.recent(max_count);
    if (json_out) {
        out << "[\n";
        for (std::size_t i = 0; i < rows.size(); ++i) {
            const auto& ev = rows[i];
            out << "  " << record_to_jsonl(ev);
            if (i + 1 < rows.size()) {
                out << ',';
            }
            out << '\n';
        }
        out << "]\n";
        return out.good() ? 0 : 1;
    }
    for (const auto& ev : rows) {
        out << format_o14_timestamp(ev.timestamp_ms) << ' ' << ev.category << ' ' << ev.type
            << ' ' << ev.detail << '\n';
    }
    return out.good() ? 0 : 1;
}

}  // namespace cci::core
