#include "cci/hal/gpio.hpp"

namespace cci::hal {

bool gpio_init() { return true; }

bool gpio_set_output(int pin, GpioLevel level) {
    (void)pin;
    (void)level;
    return true;
}

bool gpio_read_input(int pin, GpioLevel& level) {
    (void)pin;
    level = GpioLevel::High;
    return true;
}

}  // namespace cci::hal
