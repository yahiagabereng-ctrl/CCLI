#include "cci/hal/net.hpp"

namespace cci::hal {

bool bind_interface(const NetBind& bind) {
    (void)bind;
    // TODO: set address on device; lab script may configure host networking
    return true;
}

std::string platform_name() { return "platform_pi"; }

}  // namespace cci::hal
