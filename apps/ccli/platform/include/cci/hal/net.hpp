#pragma once

#include <string>

namespace cci::hal {

struct NetBind {
    std::string device;
    std::string address;
    std::string role;  // dso | operator | plant
};

bool bind_interface(const NetBind& bind);
std::string platform_name();

}  // namespace cci::hal
