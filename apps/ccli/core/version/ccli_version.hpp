#pragma once

#include <iostream>
#include <string>

namespace cci::core::version {

struct Info {
    std::string semver;
    int         release{0};
    std::string full;
    std::string codename;
    std::string git_sha;
    std::string build_utc;
    std::string platform;
};

Info current();

void print_text(std::ostream& out);
void print_json(std::ostream& out);

}  // namespace cci::core::version
