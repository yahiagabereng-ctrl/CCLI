#include "cci/hal/gpio.hpp"
#include "cci/hal/net.hpp"

namespace cci::hal {

bool platform_init() { return gpio_init(); }

}  // namespace cci::hal
