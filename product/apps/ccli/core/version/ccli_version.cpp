#include "version/ccli_version.hpp"

#include "ccli_version_gen.h"

namespace cci::core::version {

Info current() {
    Info v{};
    v.semver = CCLI_VERSION_STRING;
    v.release = CCLI_RELEASE;
    v.full = CCLI_VERSION_FULL;
    v.codename = CCLI_VERSION_CODENAME;
    v.git_sha = CCLI_GIT_SHA;
    v.build_utc = CCLI_BUILD_TIMESTAMP;
    v.platform = CCLI_BUILD_PLATFORM;
    return v;
}

void print_text(std::ostream& out) {
    const Info v = current();
    out << "ccli " << v.full;
    if (!v.codename.empty()) {
        out << " (" << v.codename << ")";
    }
    out << "\n"
        << "  semver:   " << v.semver << "\n"
        << "  release:  r" << v.release << "\n"
        << "  git:      " << v.git_sha << "\n"
        << "  built:    " << v.build_utc << " UTC\n"
        << "  platform: " << v.platform << "\n";
}

void print_json(std::ostream& out) {
    const Info v = current();
    out << "{\n"
        << "  \"name\": \"ccli\",\n"
        << "  \"semver\": \"" << v.semver << "\",\n"
        << "  \"release\": " << v.release << ",\n"
        << "  \"full\": \"" << v.full << "\",\n"
        << "  \"codename\": \"" << v.codename << "\",\n"
        << "  \"git_sha\": \"" << v.git_sha << "\",\n"
        << "  \"build_utc\": \"" << v.build_utc << "\",\n"
        << "  \"platform\": \"" << v.platform << "\"\n"
        << "}\n";
}

}  // namespace cci::core::version
