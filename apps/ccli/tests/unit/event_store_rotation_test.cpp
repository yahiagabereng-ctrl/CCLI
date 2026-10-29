#include "event/event_store.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#if !defined(_WIN32)
#include <sys/stat.h>
#endif

namespace {
namespace fs = std::filesystem;
using cci::core::EventStore;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
std::string bytes(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}
std::size_t lines(const fs::path& path) {
    std::ifstream in(path);
    std::size_t count = 0;
    std::string line;
    while (std::getline(in, line)) ++count;
    return count;
}
void seed(const fs::path& path, std::size_t count) {
    std::ofstream out(path);
    for (std::size_t i = 0; i < count; ++i)
        out << "{\"ts_ms\":" << i << ",\"type\":\"seed\",\"detail\":\"" << i << "\"}\n";
    require(out.good(), "seed failed");
}
void matches(const EventStore& a, const EventStore& b) {
    const auto expected = a.recent(EventStore::kMaxEvents);
    const auto actual = b.recent(EventStore::kMaxEvents);
    require(expected.size() == actual.size(), "retained count changed after reload");
    for (std::size_t i = 0; i < expected.size(); ++i)
        require(expected[i].timestamp_ms == actual[i].timestamp_ms &&
                expected[i].type == actual[i].type && expected[i].detail == actual[i].detail &&
                expected[i].category == actual[i].category, "retained event changed after reload");
}
void rotation(const fs::path& dir) {
    const auto path = dir / "events.jsonl";
    const auto previous = dir / "events.jsonl.1";
    seed(path, EventStore::kMaxEvents);
    const auto legacy = bytes(path);
#if !defined(_WIN32)
    struct stat before{};
    require(stat(path.c_str(), &before) == 0, "stat legacy failed");
#endif
    EventStore store({true, path.string()});
    require(store.append({2048, "mms", "client_connect", ""}), "first rotation failed");
    require(lines(path) == 1 && bytes(previous) == legacy, "legacy records were rewritten or lost");
#if !defined(_WIN32)
    struct stat after{};
    require(stat(previous.c_str(), &after) == 0 && before.st_ino == after.st_ino,
            "rotation copied records instead of renaming");
    require(stat(path.c_str(), &after) == 0 && (after.st_mode & 0777) == 0640,
            "new log permissions changed");
#endif
    // Cross two more boundaries: replacing the oldest segment must keep order.
    for (std::size_t i = 2049; i < 3 * EventStore::kMaxEvents + 31; ++i)
        require(store.append({static_cast<std::int64_t>(i), "mms", std::to_string(i), ""}), "append failed");
    require(lines(path) == 31 && lines(previous) == EventStore::kMaxEvents,
            "disk segments are not bounded");
    EventStore reloaded({true, path.string()});
    require(reloaded.size() == EventStore::kMaxEvents, "logical retention is not 2048");
    matches(store, reloaded);
    // Ordinary appends must not touch the retained segment.
    const auto retained = bytes(previous);
    require(store.append({99999, "mms", "next", ""}), "post-wrap append failed");
    require(bytes(previous) == retained && lines(path) == 32, "append rewrote retained records");
}
void recovery(const fs::path& dir) {
    const auto path = dir / "recover.jsonl";
    seed(path, EventStore::kMaxEvents);
    // Simulate power loss immediately after rotation, before creating current.
    fs::rename(path, path.string() + ".1");
    EventStore store({true, path.string()});
    require(store.size() == EventStore::kMaxEvents, "rotated-only log was not recovered");
    require(store.append({2048, "mms", "after-crash", ""}), "append after interrupted rotation failed");
    EventStore recovered({true, path.string()});
    matches(store, recovered);
    // A partial last record must not swallow the next valid append.
    {
        std::ofstream out(path, std::ios::app);
        out << "{\"ts_ms\":3,\"type\":\"mms\",\"detail\":\"interrupted";
    }
    EventStore torn({true, path.string()});
    require(torn.append({2049, "mms", "after-partial-write", ""}), "append after partial record failed");
    EventStore recovered_again({true, path.string()});
    matches(torn, recovered_again);
}
void strings(const fs::path& dir) {
    const auto path = dir / "strings.jsonl";
    const std::string detail = "{\"rbac\":true,\"reason\":\"quoted \\\"peer\\\"\"}\n\t\r\b\f" + std::string(1, '\x01');
    EventStore store({true, path.string()});
    require(store.append({1000, "mms", detail, "o14.security"}), "escaped audit append failed");
    require(lines(path) == 1, "control characters split a JSONL record");
    EventStore reloaded({true, path.string()});
    matches(store, reloaded);
    // External JSON writers may use escaped Unicode or omit the final newline.
    const auto unicode_path = dir / "unicode.jsonl";
    {
        std::ofstream out(unicode_path);
        out << "{\"ts_ms\":1,\"type\":\"mms\",\"detail\":\"\\u20ac\\ud83d\\ude00\"}";
    }
    EventStore unicode({true, unicode_path.string()});
    require(unicode.size() == 1 && unicode.recent(1)[0].detail == "\xe2\x82\xac\xf0\x9f\x98\x80",
            "Unicode was not decoded correctly");
    require(unicode.append({2, "mms", "separate-record", ""}), "append without trailing newline failed");
    EventStore unicode_reloaded({true, unicode_path.string()});
    matches(unicode, unicode_reloaded);
}
void failures(const fs::path& dir) {
    const auto path = dir / "failure.jsonl";
    seed(path, EventStore::kMaxEvents);
    const auto original = bytes(path);
    fs::create_directory(path.string() + ".1");
    EventStore store({true, path.string()});
    require(!store.append({9000, "mms", "failed-rotation", ""}) && !store.persistence_ok(),
            "rotation failure was silently accepted");
    require(bytes(path) == original, "failed rotation damaged the active log");
    require(store.recent(1)[0].detail == "failed-rotation", "failed persistence lost the in-memory event");
    fs::remove(path.string() + ".1");
    require(store.append({9001, "mms", "recovered", ""}) && store.persistence_ok(), "persistence did not recover");
}
void concurrent(const fs::path& dir) {
    const auto path = dir / "concurrent.jsonl";
    EventStore store({true, path.string()});
    std::vector<std::thread> writers;
    for (int thread = 0; thread < 4; ++thread)
        writers.emplace_back([&, thread] {
            for (int i = 0; i < 800; ++i)
                require(store.append({thread * 1000 + i, "mms", std::to_string(thread) + ":" + std::to_string(i), ""}),
                        "concurrent append failed");
        });
    for (auto& writer : writers) writer.join();
    EventStore reloaded({true, path.string()});
    require(store.size() == EventStore::kMaxEvents, "concurrent retention failed");
    matches(store, reloaded);
}
} // namespace

int main() {
    const auto dir = fs::temp_directory_path() / ("ccli-event-rotation-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if (!fs::create_directory(dir)) return 1;
    int result = 0;
    try {
        rotation(dir);
        recovery(dir);
        strings(dir);
        failures(dir);
        concurrent(dir);
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        result = 1;
    }
    fs::remove_all(dir);
    return result;
}
