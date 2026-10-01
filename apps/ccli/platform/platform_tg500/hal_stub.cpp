#include "cci/hal/gpio.hpp"
#include "cci/hal/net.hpp"

namespace cci::hal {

bool bind_interface(const NetBind& bind) {
    (void)bind;
    return false;
}

std::string platform_name() { return "platform_tg500"; }

bool gpio_init() { return true; }

bool gpio_set_output(int pin, GpioLevel level) {
    (void)pin;
    (void)level;
    return false;
}

bool gpio_read_input(int pin, GpioLevel& level) {
    (void)pin;
    level = GpioLevel::Low;
    return false;
}

}  // namespace cci::hal
