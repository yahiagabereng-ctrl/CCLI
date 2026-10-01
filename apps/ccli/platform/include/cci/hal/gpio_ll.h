/*=============================================================================
 * File       :  gpio_ll.h
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  Low-level, polarity-agnostic GPIO HAL (C API).
 *
 * This is the bottom layer of the DI/DO stack (SK0153 style: driver ->
 * low-level HAL). Levels are raw electrical levels: 0 = LOW, 1 = HIGH.
 * Polarity (active-high / active-low) is handled one layer up in drv_gpio.
 *
 * Implementations:
 *   platform_pi/hal_gpio_ll.c      - libgpiod v2 on /dev/gpiochip0 (Pi / OpenWrt)
 *   platform_tg500/hal_gpio_ll.c   - stub until TesPro V-003 line map arrives
 *   tests/unit/mock_gpio_ll.c      - inspectable mock for L1 unit tests
 *=============================================================================*/
#ifndef CCI_HAL_GPIO_LL_H
#define CCI_HAL_GPIO_LL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the low-level GPIO backend. Returns true on success. */
bool cci_gpio_ll_init(void);

/* Configure a line as an output and drive it to initial_level (0 or 1). */
bool cci_gpio_ll_config_output(int line, int initial_level);

/* Configure a line as an input. */
bool cci_gpio_ll_config_input(int line);

/* Write a raw level (0 = LOW, 1 = HIGH) to a configured output line. */
bool cci_gpio_ll_write(int line, int level);

/* Read a raw level (0 = LOW, 1 = HIGH) from a configured line. */
bool cci_gpio_ll_read(int line, int *level);

/* Release all line requests and shut down the backend. */
void cci_gpio_ll_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* CCI_HAL_GPIO_LL_H */
