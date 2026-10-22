#pragma once

namespace cci::hal {

enum class GpioLevel { Low, High };

bool gpio_init();
bool gpio_set_output(int pin, GpioLevel level);
bool gpio_read_input(int pin, GpioLevel& level);

}  // namespace cci::hal
