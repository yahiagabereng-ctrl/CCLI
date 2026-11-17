#include "event/platform_event_monitor.hpp"
#include "event/event_store.hpp"
#include "event/iec62351_event.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <cstdint>
#include <limits>

#if defined(__linux__)
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#if defined(CCLI_HAVE_SYSLOG_TLS)
#include <mbedtls/sha256.h>
#endif
#endif

namespace cci::core {
namespace {

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool contains(const std::string& value, const char* part) {
    return value.find(part) != std::string::npos;
}

bool account_name(const std::string& value) {
    return !value.empty() && value.size() <= 32 &&
        std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '@';
        });
}

std::string extract_user(const std::string& message) {
    static const char* markers[] = {"for '", "user '", "account '", "username '"};
    for (const char* marker : markers) {
        const auto at = message.find(marker);
        if (at == std::string::npos) continue;
        const auto first = at + std::strlen(marker);
        const auto last = message.find('\'', first);
        if (last == std::string::npos) continue;
        auto value = message.substr(first, last - first);
        if (account_name(value)) return value;
    }
    static const char* unquoted_markers[] = {"accepted login for ", "login succeeded for ", "authenticated user "};
    for (const char* marker : unquoted_markers) {
        const auto at = message.find(marker);
        if (at == std::string::npos) continue;
        const auto first = at + std::strlen(marker);
        auto last = message.find_first_of(" \t,;)", first);
        if (last == std::string::npos) last = message.size();
        auto value = message.substr(first, last - first);
        if (account_name(value)) return value;
    }
    const auto luci_login = message.find("luci: accepted login on ");
    const auto luci_failure = message.find("luci: failed login on ");
    const auto login_at = luci_login != std::string::npos ? luci_login : luci_failure;
    if (login_at != std::string::npos) {
        const auto for_at = message.find(" for ", login_at);
        if (for_at != std::string::npos) {
            const auto first = for_at + 5;
            auto last = message.find_first_of(" \t,;)", first);
            if (last == std::string::npos) last = message.size();
            auto value = message.substr(first, last - first);
            if (account_name(value)) return value;
        }
    }
    return {};
}

std::string extract_peer(const std::string& message) {
    const auto at = message.find(" from ");
    if (at == std::string::npos) return {};
    const auto first = at + 6;
    auto last = message.find_first_of(" \t\r,;)]", first);
    if (last == std::string::npos) last = message.size();
    const auto value = message.substr(first, last - first);
    if (value.empty() || value.size() > 64 ||
        !std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '.' || c == ':' || c == '-' || c == '_';
        })) return {};
    return value;
}

struct ServiceMessage {
    std::string service;
    std::string message;
};

ServiceMessage service_message(const std::string& line) {
    const auto luci_at = line.find("luci: ");
    if (luci_at != std::string::npos &&
        (line.find("accepted login on ", luci_at) != std::string::npos ||
         line.find("failed login on ", luci_at) != std::string::npos))
        return {"luci", line.substr(luci_at)};
    static const char* services[] = {"dropbear", "rpcd", "uhttpd", "luci", "nginx", "lighttpd", "passwd"};
    for (const char* service : services) {
        std::string token = std::string(service) + "[";
        auto at = line.find(token);
        std::size_t end = std::string::npos;
        if (at != std::string::npos) {
            end = line.find("]: ", at);
            if (end != std::string::npos) end += 3;
        } else {
            token = std::string(service) + ": ";
            at = line.find(token);
            if (at != std::string::npos) end = at + token.size();
        }
        if (at != std::string::npos && end != std::string::npos)
            return {service, line.substr(end)};
    }
    return {};
}

std::int64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

#if defined(__linux__)
bool load_import_cursor(const std::string& path, std::uint64_t& device,
                       std::uint64_t& inode, std::uint64_t& offset) {
    const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) return errno == ENOENT;
    struct stat st{};
    if (fstat(fd, &st) != 0 || !S_ISREG(st.st_mode) || st.st_uid != geteuid() ||
        (st.st_mode & 0022) != 0 || st.st_size < 0 || st.st_size > 256) {
        close(fd);
        return false;
    }
    std::array<char, 257> bytes{};
    std::size_t used = 0;
    while (used < bytes.size() - 1) {
        const auto n = read(fd, bytes.data() + used, bytes.size() - 1 - used);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) { close(fd); return false; }
        if (n == 0) break;
        used += static_cast<std::size_t>(n);
    }
    close(fd);
    std::istringstream input(std::string(bytes.data(), used));
    std::string version;
    if (!(input >> version >> device >> inode >> offset) || version != "v1") return false;
    std::string trailing;
    return !(input >> trailing);
}

bool write_import_cursor_atomic(const std::string& path, const std::string& contents) {
    std::string pattern = path + ".tmp.XXXXXX";
    std::vector<char> name(pattern.begin(), pattern.end());
    name.push_back('\0');
    const int fd = mkstemp(name.data());
    if (fd < 0) return false;
    bool ok = fchmod(fd, 0600) == 0;
    std::size_t written = 0;
    while (ok && written < contents.size()) {
        const auto n = write(fd, contents.data() + written, contents.size() - written);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { ok = false; break; }
        written += static_cast<std::size_t>(n);
    }
    if (ok) ok = fsync(fd) == 0;
    if (close(fd) != 0) ok = false;
    if (ok && rename(name.data(), path.c_str()) != 0) ok = false;
    if (!ok) unlink(name.data());
    if (!ok) return false;
    const auto slash = path.find_last_of('/');
    const auto parent = slash == std::string::npos ? std::string(".") :
                        slash == 0 ? std::string("/") : path.substr(0, slash);
    const int dir = open(parent.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (dir < 0) return false;
    const bool synced = fsync(dir) == 0;
    close(dir);
    return synced;
}
#endif

EventRecord make_event(std::int64_t timestamp, const std::string& id,
                       const std::string& detail, const std::string& app,
                       const std::string& user = {}, const std::string& peer = {}) {
    EventRecord event{timestamp, "security", detail, "security"};
    event.iec_id = id;
    event.app_name = app;
    event.user_id = user;
    event.peer_info = peer;
    return event;
}

std::vector<std::string> split_colons(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t begin = 0;
    for (;;) {
        const auto end = line.find(':', begin);
        fields.push_back(line.substr(begin, end == std::string::npos ? end : end - begin));
        if (end == std::string::npos) break;
        begin = end + 1;
    }
    return fields;
}

#if defined(__linux__)
std::string credential_fingerprint(const std::string& shadow_credential,
                                   const std::string& last_changed) {
    const std::string material = shadow_credential + '\0' + last_changed;
#if defined(CCLI_HAVE_SYSLOG_TLS)
    std::array<unsigned char, 32> digest{};
    if (mbedtls_sha256(reinterpret_cast<const unsigned char*>(material.data()),
                       material.size(), digest.data(), 0) == 0) {
        static constexpr char hex[] = "0123456789abcdef";
        std::string out;
        out.reserve(digest.size() * 2);
        for (const auto byte : digest) {
            out += hex[byte >> 4];
            out += hex[byte & 0x0f];
        }
        return out;
    }
#endif
    // A small non-cryptographic change fingerprint keeps password hashes out
    // of memory on feature-stub builds; it is never used as an authenticator.
    std::uint64_t hash = 14695981039346656037ULL;
    for (const unsigned char byte : material) {
        hash ^= byte;
        hash *= 1099511628211ULL;
    }
    std::ostringstream out;
    out << std::hex << std::setw(16) << std::setfill('0') << hash;
    return out.str();
}

bool read_account_snapshot(PlatformAccountSnapshot& snapshot) {
    std::ifstream passwd("/etc/passwd");
    if (!passwd) return false;
    std::map<std::string, std::string> attributes;
    std::string line;
    bool root_found = false;
    while (std::getline(passwd, line)) {
        const auto fields = split_colons(line);
        if (fields.size() < 7 || !account_name(fields[0])) continue;
        if (fields[0] == "root") root_found = true;
        attributes[fields[0]] = fields[2] + ":" + fields[3] + ":" + fields[6];
    }
    if (!root_found) return false; // ignore incomplete/partially replaced files

    std::ifstream groups("/etc/group");
    if (groups) {
        while (std::getline(groups, line)) {
            const auto fields = split_colons(line);
            if (fields.size() < 4 || !account_name(fields[0])) continue;
            std::istringstream members(fields[3]);
            std::string member;
            while (std::getline(members, member, ',')) {
                if (!account_name(member) || attributes.find(member) == attributes.end()) continue;
                attributes[member] += ":g=" + fields[0];
            }
        }
    }

    struct CredentialState { std::string fingerprint; bool locked{false}; };
    std::map<std::string, CredentialState> credentials;
    std::ifstream shadow("/etc/shadow");
    if (!shadow) return false; // do not claim password-change coverage without shadow access
    while (std::getline(shadow, line)) {
        const auto fields = split_colons(line);
        if (fields.size() < 2 || !account_name(fields[0])) continue;
        const auto last_changed = fields.size() > 2 ? fields[2] : std::string{};
        const bool locked = !fields[1].empty() && (fields[1].front() == '!' || fields[1].front() == '*');
        credentials[fields[0]] = {credential_fingerprint(fields[1], last_changed), locked};
    }

    snapshot.clear();
    for (const auto& account : attributes) {
        PlatformAccountState state;
        state.attributes = account.second;
        const auto credential = credentials.find(account.first);
        if (credential != credentials.end()) {
            state.credential_fingerprint = credential->second.fingerprint;
            state.credential_locked = credential->second.locked;
        }
        snapshot.emplace(account.first, std::move(state));
    }
    return true;
}

pid_t start_logread(int& output_fd) {
    const char* executable = access("/sbin/logread", X_OK) == 0 ? "/sbin/logread" :
                             access("/usr/bin/logread", X_OK) == 0 ? "/usr/bin/logread" : nullptr;
    if (!executable) return -1;
    int pipe_fds[2];
    if (pipe2(pipe_fds, O_CLOEXEC) != 0) return -1;
    const auto parent_pid = getpid();
    const long descriptor_limit = sysconf(_SC_OPEN_MAX);
    if (descriptor_limit < 0) { close(pipe_fds[0]); close(pipe_fds[1]); return -1; }
    const auto pid = fork();
    if (pid == 0) {
        // procd may kill CCI before its worker can join. Do not leave an
        // unowned collector (and its inherited descriptors) behind.
        if (prctl(PR_SET_PDEATHSIG, SIGTERM) != 0 || getppid() != parent_pid) _exit(127);
        close(pipe_fds[0]);
        if (dup2(pipe_fds[1], STDOUT_FILENO) < 0) _exit(127);
        const int null_fd = open("/dev/null", O_WRONLY | O_CLOEXEC);
        if (null_fd >= 0) dup2(null_fd, STDERR_FILENO);
        close(pipe_fds[1]);
        // The collector must not retain CCI's listening/client sockets or
        // open credential/log descriptors when its parent closes them.
        bool descriptors_closed = false;
#if defined(SYS_close_range)
        descriptors_closed = syscall(SYS_close_range, 3U, ~0U, 0U) == 0;
#endif
        if (!descriptors_closed)
            for (long fd = 3; fd < descriptor_limit; ++fd) close(static_cast<int>(fd));
        execl(executable, "logread", "-f", static_cast<char*>(nullptr));
        _exit(127);
    }
    close(pipe_fds[1]);
    if (pid < 0) { close(pipe_fds[0]); return -1; }
    const int flags = fcntl(pipe_fds[0], F_GETFL, 0);
    if (flags >= 0) fcntl(pipe_fds[0], F_SETFL, flags | O_NONBLOCK);
    output_fd = pipe_fds[0];
    return pid;
}

void stop_child(pid_t& child, int& output_fd) {
    if (output_fd >= 0) { close(output_fd); output_fd = -1; }
    if (child > 0) {
        kill(child, SIGTERM);
        int status = 0;
        for (int i = 0; i < 10; ++i) {
            if (waitpid(child, &status, WNOHANG) == child) { child = -1; return; }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        kill(child, SIGKILL);
        waitpid(child, &status, 0);
        child = -1;
    }
}

std::optional<EventRecord> process_log_line(std::string line) {
    if (line.empty() || line.size() > 4096) return std::nullopt;
    return parse_platform_auth_log_line(line, now_ms());
}

void monitor_loop(const PlatformEventMonitor::Append append, void* context,
                  const std::atomic<bool>* stopping,
                  const std::string& updater_audit_path,
                  const std::string& updater_cursor_path,
                  const std::vector<std::string>& credential_paths) {
    FirmwareUpdaterAuditImporter updater(updater_audit_path, updater_cursor_path);
    const std::string suffix = ".firmware-updater.cursor";
    const std::string log_path = updater_cursor_path.size() > suffix.size() ?
        updater_cursor_path.substr(0, updater_cursor_path.size() - suffix.size()) : std::string{};
    FirmwareUpdaterAuditImporter reads(log_path.empty() ? "" : log_path + ".read-audit",
                                      log_path.empty() ? "" : log_path + ".read-audit.cursor");
    const std::string platform_auth_path = log_path.empty() ? "" : log_path + ".platform-auth-audit";
    FirmwareUpdaterAuditImporter auth(platform_auth_path,
                                      platform_auth_path.empty() ? "" : platform_auth_path + ".cursor");
    PlatformAccountSnapshot accounts;
    (void)read_account_snapshot(accounts);
    struct FileState { bool exists{false}; dev_t device{}; ino_t inode{}; off_t size{}; timespec mtime{}; };
    auto file_state = [](const std::string& path) {
        FileState state;
        struct stat st{};
        if (::stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            state.exists = true; state.device = st.st_dev; state.inode = st.st_ino;
            state.size = st.st_size; state.mtime = st.st_mtim;
        }
        return state;
    };
    std::vector<FileState> credential_states;
    for (const auto& path : credential_paths) credential_states.push_back(file_state(path));
    const int inotify_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (inotify_fd >= 0) {
        (void)inotify_add_watch(inotify_fd, "/etc", IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_ATTRIB);
    }
    pid_t child = -1;
    int output_fd = -1;
    std::string pending;
    std::deque<EventRecord> auth_retry;
    constexpr std::size_t kMaxAuthRetryEvents = 2048;
    bool discard_line = false;
    std::array<char, 4096> buffer{};
    auto reconcile_at = std::chrono::steady_clock::time_point{};
    auto retry_logread_at = std::chrono::steady_clock::time_point{};
    while (!stopping->load()) {
        while (!auth_retry.empty()) {
            const bool saved = platform_auth_path.empty() ?
                (append && append(context, auth_retry.front())) :
                EventStore::audit_platform_auth(log_path, auth_retry.front());
            if (!saved) break;
            auth_retry.pop_front();
        }
        updater.poll(context, append);
        reads.poll(context, append, true);
        auth.poll(context, append, true);
        if (std::chrono::steady_clock::now() >= reconcile_at) {
            PlatformAccountSnapshot updated;
            if (read_account_snapshot(updated)) {
                bool saved = true;
                for (auto& event : diff_platform_account_snapshots(accounts, updated, now_ms()))
                    if (!append || !append(context, std::move(event))) saved = false;
                if (saved) accounts = std::move(updated);
            }
            for (std::size_t i = 0; i < credential_paths.size(); ++i) {
                const auto next = file_state(credential_paths[i]);
                const auto& previous = credential_states[i];
                if (next.exists != previous.exists || next.device != previous.device ||
                    next.inode != previous.inode || next.size != previous.size ||
                    next.mtime.tv_sec != previous.mtime.tv_sec ||
                    next.mtime.tv_nsec != previous.mtime.tv_nsec) {
                    const auto slash = credential_paths[i].find_last_of('/');
                    const auto basename = slash == std::string::npos ? credential_paths[i] :
                                          credential_paths[i].substr(slash + 1);
                    const auto normalized_name = lower(basename);
                    const auto operation = normalized_name.find(".crl") != std::string::npos ?
                        "revocation_list_file_changed" :
                        (normalized_name.find("rbac") != std::string::npos ||
                         normalized_name.find("role") != std::string::npos ||
                         normalized_name.find("policy") != std::string::npos ||
                         normalized_name.find("scope") != std::string::npos ||
                         normalized_name.find(".xml") != std::string::npos) ?
                        "authorization_policy_file_changed" : "credential_file_changed";
                    EventRecord event{now_ms(), "security", "security_configuration:changed", "security"};
                    event.iec_id = "IEC62351-14:6.10";
                    event.app_name = "credential-monitor";
                    event.parameters.emplace_back("P.Operation", operation);
                    event.parameters.emplace_back("P.File", basename.substr(0, 96));
                    event.parameters.emplace_back("P.State", next.exists ? "present" : "missing");
                    if (append && append(context, std::move(event))) credential_states[i] = next;
                }
            }
            reconcile_at = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        }
        if (child <= 0 && std::chrono::steady_clock::now() >= retry_logread_at) {
            child = start_logread(output_fd);
            retry_logread_at = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        }
        pollfd fds[2]{};
        nfds_t count = 0;
        if (output_fd >= 0) fds[count++] = {output_fd,
            static_cast<short>((auth_retry.size() < kMaxAuthRetryEvents ? POLLIN : 0) |
                POLLHUP | POLLERR), 0};
        if (inotify_fd >= 0) fds[count++] = {inotify_fd, POLLIN, 0};
        const int ready = poll(fds, count, child > 0 ? 1000 : 3000);
        if (ready > 0) {
            nfds_t index = 0;
            bool account_change = false;
            if (output_fd >= 0) {
                const auto revents = fds[index++].revents;
                if (revents & POLLIN) {
                    for (;;) {
                        // A byte can finish at most one event. Limit the read by
                        // remaining queue slots so this chunk cannot overflow it.
                        const auto capacity = kMaxAuthRetryEvents - auth_retry.size();
                        if (capacity == 0) break;
                        const auto size = read(output_fd, buffer.data(), std::min(buffer.size(), capacity));
                        if (size <= 0) break;
                        for (ssize_t i = 0; i < size; ++i) {
                            if (buffer[static_cast<std::size_t>(i)] == '\n') {
                                if (!discard_line) {
                                    auto event = process_log_line(std::move(pending));
                                    if (event) auth_retry.push_back(std::move(*event));
                                }
                                pending.clear();
                                discard_line = false;
                            } else if (pending.size() < 4096) {
                                if (!discard_line) pending += buffer[static_cast<std::size_t>(i)];
                            } else {
                                pending.clear();
                                discard_line = true; // drop oversized log record as a whole
                            }
                        }
                        if (auth_retry.size() >= kMaxAuthRetryEvents) break;
                    }
                }
                if (revents & (POLLHUP | POLLERR)) {
                    stop_child(child, output_fd);
                    pending.clear();
                    discard_line = false;
                }
            }
            if (inotify_fd >= 0 && index < count && (fds[index].revents & POLLIN)) {
                alignas(inotify_event) char events[4096];
                for (;;) {
                    const auto size = read(inotify_fd, events, sizeof(events));
                    if (size <= 0) break;
                    for (std::size_t offset = 0; offset + sizeof(inotify_event) <= static_cast<std::size_t>(size);) {
                        const auto* event = reinterpret_cast<const inotify_event*>(events + offset);
                        const std::string name = event->len ? event->name : "";
                        if ((event->mask & IN_Q_OVERFLOW) || name == "passwd" || name == "shadow" || name == "group") account_change = true;
                        offset += sizeof(inotify_event) + event->len;
                    }
                }
            }
            if (account_change) reconcile_at = std::chrono::steady_clock::time_point{};
        }
        if (child > 0) {
            int status = 0;
            if (waitpid(child, &status, WNOHANG) == child) {
                child = -1; // already reaped; do not signal a potentially reused PID
                pending.clear();
                discard_line = false;
                if (output_fd >= 0) { close(output_fd); output_fd = -1; }
                retry_logread_at = std::chrono::steady_clock::now() + std::chrono::seconds(5);
            }
        }
    }
    stop_child(child, output_fd);
    if (inotify_fd >= 0) close(inotify_fd);
}
#endif

} // namespace

std::optional<EventRecord> parse_platform_auth_log_line(const std::string& line,
                                                        const std::int64_t timestamp_ms) {
    if (line.size() > 4096) return std::nullopt;
    const auto parsed = service_message(line);
    if (parsed.service.empty()) return std::nullopt;
    const auto message = lower(parsed.message);
    auto user = extract_user(parsed.message);
    if (parsed.service == "dropbear" && user.empty() && contains(message, "exit (")) {
        const auto first = message.find("exit (") + 6;
        const auto last = message.find(')', first);
        if (last != std::string::npos) {
            const auto candidate = parsed.message.substr(first, last - first);
            if (account_name(candidate)) user = candidate;
        }
    }
    const auto peer = extract_peer(parsed.message);
    std::string id;
    if (parsed.service == "dropbear") {
        if ((contains(message, "too many authentication attempts") ||
             contains(message, "maximum authentication attempts") ||
             contains(message, "max auth tries"))) {
            id = "IEC62351-14:1.5";
        } else if (contains(message, "exit (") && contains(message, " from ") &&
                   !contains(message, "exit before auth")) {
            id = "IEC62351-14:1.8";
        } else if ((contains(message, "auth succeeded") || contains(message, "authentication succeeded") ||
                    contains(message, "authenticated successfully")) && !user.empty()) {
            id = "IEC62351-14:1.1";
        } else if ((contains(message, "auth failed") || contains(message, "authentication failed") ||
                    contains(message, "invalid password") || contains(message, "bad password") ||
                    contains(message, "nonexistent user")) && !contains(message, "exit before auth")) {
            id = "IEC62351-14:1.3";
        }
    } else if (parsed.service == "rpcd" || parsed.service == "uhttpd" || parsed.service == "luci" ||
               parsed.service == "nginx" || parsed.service == "lighttpd") {
        if (!user.empty() && (contains(message, "authentication successful") ||
                              contains(message, "login success") || contains(message, "login succeeded") ||
                              contains(message, "accepted login for ") ||
                              contains(message, "accepted login on ") ||
                              contains(message, "authenticated successfully") ||
                              (contains(message, "user '") && contains(message, " authenticated")))) {
            id = "IEC62351-14:1.1";
        } else if (!user.empty() && (contains(message, "authentication failed") ||
                                     contains(message, "authentication failure") ||
                                     contains(message, "login failed") || contains(message, "failed login on ") ||
                                     contains(message, "login failure"))) {
            id = "IEC62351-14:1.3";
        } else if (!user.empty() && (contains(message, "logged out") || contains(message, "logout") ||
                                     contains(message, "session closed") || contains(message, "session destroyed"))) {
            id = "IEC62351-14:1.8";
        }
    } else if (parsed.service == "passwd") {
        if (contains(message, "too short") || contains(message, "policy violation") ||
            contains(message, "does not meet")) {
            id = "IEC62351-14:2.11";
        } else if (contains(message, "failed") || contains(message, "failure") ||
                   contains(message, "token manipulation error")) {
            id = "IEC62351-14:2.10";
        }
    }
    if (id.empty()) return std::nullopt;
    const std::string detail = parsed.service == "passwd" ? "platform_account:credential_change_failed" :
        std::string("platform_auth:") + (id == "IEC62351-14:1.1" ? "success" :
                            id == "IEC62351-14:1.8" ? "logout" :
                            id == "IEC62351-14:1.5" ? "threshold" : "failure");
    auto event = make_event(timestamp_ms, id, detail, parsed.service, user, peer);
    if (parsed.service != "passwd") event.parameters.emplace_back("P.Service", parsed.service == "dropbear" ? "SSH" : "web");
    return event;
}

std::vector<EventRecord> diff_platform_account_snapshots(
    const PlatformAccountSnapshot& previous, const PlatformAccountSnapshot& current,
    const std::int64_t timestamp_ms) {
    std::vector<EventRecord> events;
    for (const auto& old_account : previous) {
        const auto next = current.find(old_account.first);
        if (next == current.end()) {
            events.push_back(make_event(timestamp_ms, "IEC62351-14:2.7", "platform_account:deleted",
                                        "account-monitor", old_account.first));
            continue;
        }
        const auto& before = old_account.second;
        const auto& after = next->second;
        if (before.credential_fingerprint != after.credential_fingerprint) {
            const auto lock_changed = before.credential_locked != after.credential_locked;
            const auto id = lock_changed ? (after.credential_locked ? "IEC62351-14:2.5" : "IEC62351-14:2.3") :
                            "IEC62351-14:2.9";
            auto event = make_event(timestamp_ms, id,
                lock_changed ? "platform_account:lock_state_changed" : "platform_account:credential_changed",
                "account-monitor", old_account.first);
            event.parameters.emplace_back("P.Change", lock_changed ? "lock_state" : "password");
            events.push_back(std::move(event));
        }
        if (before.attributes != after.attributes) {
            auto event = make_event(timestamp_ms, "IEC62351-14:6.10", "platform_account:attributes_changed",
                                    "account-monitor", old_account.first);
            event.parameters.emplace_back("P.Change", "account_or_group_membership");
            events.push_back(std::move(event));
        }
    }
    for (const auto& account : current) {
        if (previous.find(account.first) == previous.end()) {
            events.push_back(make_event(timestamp_ms, "IEC62351-14:2.1", "platform_account:created",
                                        "account-monitor", account.first));
        }
    }
    return events;
}

namespace {
int audit_hex_digit(const char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool audit_hex_decode(const std::string& input, const std::size_t max_bytes,
                      std::string& output) {
    if ((input.size() & 1U) != 0 || input.size() / 2 > max_bytes) return false;
    output.clear();
    output.reserve(input.size() / 2);
    for (std::size_t i = 0; i < input.size(); i += 2) {
        const int high = audit_hex_digit(input[i]);
        const int low = audit_hex_digit(input[i + 1]);
        if (high < 0 || low < 0) return false;
        output += static_cast<char>((high << 4) | low);
    }
    return true;
}
} // namespace

std::optional<EventRecord> parse_firmware_updater_audit_line(const std::string& line) {
    if (line.empty() || line.size() > 8192) return std::nullopt;
    const auto first = line.find('|');
    const auto second = first == std::string::npos ? first : line.find('|', first + 1);
    if (first == std::string::npos || second == std::string::npos || first == 0 ||
        second == first + 1) return std::nullopt;

    const auto name = line.substr(first + 1, second - first - 1);
    if (name == "PLATFORM_AUTH") {
        const auto raw = line.substr(second + 1);
        std::array<std::string, 4> fields;
        std::size_t begin = 0;
        for (std::size_t i = 0; i < fields.size(); ++i) {
            const auto delimiter = raw.find('|', begin);
            if (i + 1 < fields.size() && delimiter == std::string::npos) return std::nullopt;
            if (i + 1 == fields.size() && delimiter != std::string::npos) return std::nullopt;
            fields[i] = raw.substr(begin, delimiter == std::string::npos ? delimiter : delimiter - begin);
            begin = delimiter == std::string::npos ? raw.size() : delimiter + 1;
        }
        const auto timestamp_field = line.substr(0, first);
        if (!std::all_of(timestamp_field.begin(), timestamp_field.end(), [](unsigned char c) {
                return c >= '0' && c <= '9';
            })) return std::nullopt;
        errno = 0;
        char* end = nullptr;
        const auto timestamp = std::strtoll(timestamp_field.c_str(), &end, 10);
        if (errno != 0 || end == nullptr || *end != '\0' || timestamp < 0 ||
            timestamp > 253402300799999LL) return std::nullopt;
        static const std::set<std::string> allowed_ids{
            "IEC62351-14:1.1", "IEC62351-14:1.3", "IEC62351-14:1.5", "IEC62351-14:1.8",
            "IEC62351-14:2.10", "IEC62351-14:2.11"};
        if (allowed_ids.find(fields[0]) == allowed_ids.end() || !iec_event_definition(fields[0]))
            return std::nullopt;
        std::string app, user, peer;
        if (!audit_hex_decode(fields[1], 64, app) || !audit_hex_decode(fields[2], 128, user) ||
            !audit_hex_decode(fields[3], 128, peer)) return std::nullopt;
        static const std::set<std::string> allowed_services{
            "dropbear", "rpcd", "uhttpd", "luci", "nginx", "lighttpd", "passwd"};
        if (allowed_services.find(app) == allowed_services.end()) return std::nullopt;
        const bool account_event = fields[0].rfind("IEC62351-14:2.", 0) == 0;
        if ((account_event && app != "passwd") || (!account_event && app == "passwd")) return std::nullopt;
        EventRecord event{timestamp, "security", "platform_auth:replayed", "security"};
        event.iec_id = fields[0];
        event.app_name = app;
        event.user_id = user;
        event.peer_info = peer;
        if (!account_event) event.parameters.emplace_back("P.Service", app == "dropbear" ? "SSH" : "web");
        return event;
    }

    errno = 0;
    char* end = nullptr;
    const auto timestamp_field = line.substr(0, first);
    if (!std::all_of(timestamp_field.begin(), timestamp_field.end(), [](unsigned char c) {
            return c >= '0' && c <= '9';
        })) return std::nullopt;
    const auto seconds = std::strtoll(timestamp_field.c_str(), &end, 10);
    if (errno != 0 || end == nullptr || *end != '\0' || seconds < 0 ||
        seconds > 253402300799LL) return std::nullopt;

    if (name.empty() || name.size() > 64 || !std::all_of(name.begin(), name.end(), [](unsigned char c) {
            return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
        })) return std::nullopt;

    const auto raw_detail = line.substr(second + 1, 2048);
    std::string detail_text;
    detail_text.reserve(raw_detail.size());
    for (const unsigned char c : raw_detail)
        detail_text += (c >= 0x20 && c < 0x7f) ? static_cast<char>(c) : '?';

    EventRecord event{static_cast<std::int64_t>(seconds) * 1000,
                      "firmware_updater", "firmware_updater:" + name,
                      "security"};
    event.app_name = "firmware-updater";
    if (!detail_text.empty()) event.detail += " " + detail_text;
    if (name == "LOG_READ_SUCCESS" || name == "LOG_READ_FAIL") {
        event.iec_id = name == "LOG_READ_SUCCESS" ? "IEC62351-14:9.1" : "IEC62351-14:9.2";
        event.app_name = "event-reader";
        event.type = "security";
        event.detail = "security_log_read:" + name + " " + detail_text;
    } else if (name == "UPDATE_SUCCEEDED") {
        event.iec_id = "IEC62351-14:6.1";
    } else if (name == "UPDATE_FAILED" || name == "UPDATE_ABORTED" ||
               name == "CCLI_RESTORE_FAILED" || name == "CCLI_START_FAILED") {
        event.iec_id = "IEC62351-14:6.2";
    } else if (name == "CCLI_VERIFICATION_FAILED") {
        event.iec_id = "IEC62351-14:6.12";
    } else if (name == "AUTHORIZATION_REJECTED") {
        event.iec_id = "IEC62351-14:6.13";
    }

    const auto transaction = detail_text.find("transaction=");
    if (transaction != std::string::npos) {
        const auto begin = transaction + std::strlen("transaction=");
        auto finish = detail_text.find_first_of(" \t,;", begin);
        if (finish == std::string::npos) finish = detail_text.size();
        const auto value = detail_text.substr(begin, finish - begin);
        if (!value.empty() && value.size() <= 64 &&
            std::all_of(value.begin(), value.end(), [](unsigned char c) {
                return std::isalnum(c) || c == '-' || c == '_';
            })) event.parameters.emplace_back("P.Transaction", value);
    }
    if (name == "LOG_READ_SUCCESS" || name == "LOG_READ_FAIL") {
        if (detail_text.rfind("uid=", 0) == 0) {
            const auto uid = detail_text.substr(4);
            if (!uid.empty() && uid.size() <= 10 &&
                std::all_of(uid.begin(), uid.end(), [](unsigned char c) { return c >= '0' && c <= '9'; }))
                event.parameters.emplace_back("P.ProcessUid", uid);
        }
        event.parameters.emplace_back("P.Operation", "security_log_read");
    } else {
        event.parameters.emplace_back("P.UpdaterEvent", name);
    }
    return event;
}

struct FirmwareUpdaterAuditImporter::Impl {
    std::string audit_path;
    std::string cursor_path;
    Impl(std::string source, std::string cursor)
        : audit_path(std::move(source)), cursor_path(std::move(cursor)) {}
};

FirmwareUpdaterAuditImporter::FirmwareUpdaterAuditImporter(std::string audit_path,
                                                           std::string cursor_path)
    : impl_(new Impl(std::move(audit_path), std::move(cursor_path))) {}
FirmwareUpdaterAuditImporter::~FirmwareUpdaterAuditImporter() = default;

void FirmwareUpdaterAuditImporter::poll(void* context, const Append append,
                                       const bool compact_consumed) {
#if defined(__linux__)
    if (!impl_ || impl_->audit_path.empty() || impl_->cursor_path.empty() || !append) return;
    const int fd = open(impl_->audit_path.c_str(),
                        (compact_consumed ? O_RDWR : O_RDONLY) | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) return;
    if (flock(fd, LOCK_EX) != 0) { close(fd); return; }
    struct stat st{};
    if (fstat(fd, &st) != 0 || !S_ISREG(st.st_mode) || st.st_uid != geteuid() ||
        (st.st_mode & 0022) != 0) { (void)flock(fd, LOCK_UN); close(fd); return; }

    std::uint64_t cursor_device = 0, cursor_inode = 0, offset = 0;
    if (!load_import_cursor(impl_->cursor_path, cursor_device, cursor_inode, offset)) {
        (void)flock(fd, LOCK_UN);
        close(fd);
        return;
    }
    if (cursor_device != static_cast<std::uint64_t>(st.st_dev) ||
        cursor_inode != static_cast<std::uint64_t>(st.st_ino) ||
        offset > static_cast<std::uint64_t>(st.st_size)) offset = 0;
    if (offset > static_cast<std::uint64_t>(std::numeric_limits<off_t>::max())) {
        (void)flock(fd, LOCK_UN);
        close(fd);
        return;
    }

    std::array<char, 4096> buffer{};
    std::string pending;
    bool oversized = false;
    std::uint64_t scan = offset;
    std::uint64_t committed = offset;
    std::size_t total_read = 0;
    bool append_failed = false;
    while (total_read < 65536 || oversized) {
        const auto request = oversized ? buffer.size() : std::min(buffer.size(), 65536 - total_read);
        const auto count = pread(fd, buffer.data(), request, static_cast<off_t>(scan));
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) break;
        total_read += static_cast<std::size_t>(count);
        const auto chunk_start = scan;
        scan += static_cast<std::uint64_t>(count);
        for (ssize_t i = 0; i < count; ++i) {
            const char c = buffer[static_cast<std::size_t>(i)];
            if (c == '\n') {
                if (!oversized) {
                    const auto event = parse_firmware_updater_audit_line(pending);
                    if (event && !append(context, *event)) {
                        append_failed = true;
                        break;
                    }
                }
                pending.clear();
                oversized = false;
                committed = chunk_start + static_cast<std::uint64_t>(i) + 1;
            } else if (pending.size() < 8192) {
                if (!oversized) pending += c;
            } else {
                pending.clear();
                oversized = true;
            }
        }
        if (append_failed || static_cast<std::size_t>(count) < request) break;
    }
    if (committed <= offset) { (void)flock(fd, LOCK_UN); close(fd); return; }

    const std::string state = "v1 " + std::to_string(static_cast<std::uint64_t>(st.st_dev)) +
        " " + std::to_string(static_cast<std::uint64_t>(st.st_ino)) + " " +
        std::to_string(committed) + "\n";
    if (!write_import_cursor_atomic(impl_->cursor_path, state)) {
        (void)flock(fd, LOCK_UN);
        close(fd);
        return;
    }
    const auto slash = impl_->cursor_path.find_last_of('/');
    const auto parent = slash == std::string::npos ? std::string(".") :
                        slash == 0 ? std::string("/") : impl_->cursor_path.substr(0, slash);
    const int dfd = open(parent.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (dfd >= 0) { (void)fsync(dfd); close(dfd); }
    if (compact_consumed && committed == static_cast<std::uint64_t>(st.st_size) &&
        st.st_size >= 65536) {
        // Cursor already names the old end. If power fails after truncate, the
        // next poll detects offset > size and resumes at zero. If cursor reset
        // fails, the same recovery path applies without losing a new row.
        if (ftruncate(fd, 0) == 0 && fsync(fd) == 0) {
            const std::string reset = "v1 " + std::to_string(static_cast<std::uint64_t>(st.st_dev)) +
                " " + std::to_string(static_cast<std::uint64_t>(st.st_ino)) + " 0\n";
            (void)write_import_cursor_atomic(impl_->cursor_path, reset);
        }
    }
    (void)flock(fd, LOCK_UN);
    close(fd);
#else
    (void)context;
    (void)append;
#endif
}

struct PlatformEventMonitor::Impl {
#if defined(__linux__)
    std::atomic<bool> stopping{false};
    std::thread worker;
    Impl(void* context, const Append append, std::string updater_audit_path,
         std::string updater_cursor_path, std::vector<std::string> credential_paths) {
        worker = std::thread([this, context, append,
                              audit_path = std::move(updater_audit_path),
                              cursor_path = std::move(updater_cursor_path),
                              paths = std::move(credential_paths)] {
            monitor_loop(append, context, &stopping, audit_path, cursor_path, paths);
        });
    }
    ~Impl() {
        stopping.store(true);
        if (worker.joinable()) worker.join();
    }
#else
    Impl(void*, Append, std::string, std::string, std::vector<std::string>) {}
#endif
};

PlatformEventMonitor::PlatformEventMonitor(void* context, const Append append,
                                           std::string updater_audit_path,
                                           std::string updater_cursor_path,
                                           std::vector<std::string> credential_paths)
    : impl_(new Impl(context, append, std::move(updater_audit_path),
                     std::move(updater_cursor_path), std::move(credential_paths))) {}
PlatformEventMonitor::~PlatformEventMonitor() { delete impl_; }

bool platform_event_monitor_available() {
#if defined(__linux__)
    if (::access("/etc/openwrt_release", F_OK) != 0) return false;
    return true; // updater audit + account monitoring still work if logread is unavailable
#else
    return false;
#endif
}

} // namespace cci::core
