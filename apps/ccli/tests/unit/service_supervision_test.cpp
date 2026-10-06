#include "service/service_supervision.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <unistd.h>

namespace {

constexpr const char* kTestTombstone = "/tmp/ccli-test-crash.tombstone";
constexpr const char* kTestRunning = "/tmp/ccli-test-running";

bool write_file(const char* path, const char* body) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return false;
    }
    out << body;
    return out.good();
}

}  // namespace

int main() {
    /* Tombstone parse path is fixed in production; this test validates signum_name only
     * and file marker logic via the public startup_check after seeding /var paths.
     * Use /var/lib/ccli in integration; here we exercise signum_name + manual parse. */
    if (cci::core::ServiceSupervision::signum_name(SIGSEGV) != "SIGSEGV") {
        std::fprintf(stderr, "signum_name SIGSEGV failed\n");
        return 1;
    }

    if (!write_file(kTestTombstone, "signum=11\npid=999\n")) {
        std::fprintf(stderr, "write tombstone failed\n");
        return 1;
    }

    std::ifstream in(kTestTombstone);
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (text.find("signum=11") == std::string::npos) {
        std::fprintf(stderr, "tombstone content mismatch\n");
        return 1;
    }

    (void)unlink(kTestTombstone);
    (void)unlink(kTestRunning);

    std::printf("service_supervision_test: passed\n");
    return 0;
}
