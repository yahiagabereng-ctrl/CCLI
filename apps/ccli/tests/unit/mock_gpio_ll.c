/*=============================================================================
 * File       :  mock_gpio_ll.c
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  Inspectable in-memory implementation of the low-level GPIO HAL
 *               for L1 unit tests. Lets the test inject inputs and read back
 *               the raw level driven onto output lines.
 *=============================================================================*/
#include "cci/hal/gpio_ll.h"
#include "mock_gpio_ll.h"

#define MOCK_MAX_LINES 64

static int s_level[MOCK_MAX_LINES];

static bool line_valid(int line) {
    return line >= 0 && line < MOCK_MAX_LINES;
}

bool cci_gpio_ll_init(void) {
    for (int i = 0; i < MOCK_MAX_LINES; ++i) {
        s_level[i] = 0;
    }
    return true;
}

bool cci_gpio_ll_config_output(int line, int initial_level) {
    if (!line_valid(line)) {
        return false;
    }
    s_level[line] = initial_level ? 1 : 0;
    return true;
}

bool cci_gpio_ll_config_input(int line) {
    return line_valid(line);
}

bool cci_gpio_ll_write(int line, int level) {
    if (!line_valid(line)) {
        return false;
    }
    s_level[line] = level ? 1 : 0;
    return true;
}

bool cci_gpio_ll_read(int line, int *level) {
    if (!line_valid(line) || level == 0) {
        return false;
    }
    *level = s_level[line];
    return true;
}

/*----------------------------- test helpers -------------------------------*/
int mock_gpio_ll_get_level(int line) {
    if (!line_valid(line)) {
        return -1;
    }
    return s_level[line];
}

void mock_gpio_ll_force_level(int line, int level) {
    if (line_valid(line)) {
        s_level[line] = level ? 1 : 0;
    }
}

void cci_gpio_ll_shutdown(void) {
    cci_gpio_ll_init();
}
