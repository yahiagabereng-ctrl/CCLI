/*=============================================================================

 * File       :  hal_gpio_ll.c  (platform_pi)

 *

 * Project    :  CCLI - CCI Central Plant Controller

 * Description:  Raspberry Pi low-level GPIO HAL via libgpiod v2 (gpiochip).

 *

 * BCM pin numbers from config/lab_pi.yaml map 1:1 to offsets on

 * /dev/gpiochip0 (Pi 4 / OpenWrt bcm2711):

 *   do_curtail=5 (output), di_permissive=27 (input).

 *

 * Polarity is handled in drv_gpio.c; this layer drives raw HIGH/LOW using

 * GPIOD_LINE_VALUE_ACTIVE / INACTIVE (active_low not set on line settings).

 *

 * Build without libgpiod (dev PC): in-memory mock — see #else below.

 *=============================================================================*/

#include "cci/hal/gpio_ll.h"

#include <stdio.h>
#include <string.h>



#ifdef CCLI_GPIO_LL_LIBGPIOD



#include <errno.h>

#include <gpiod.h>



#ifndef CCLI_GPIO_CHIP_PATH

#define CCLI_GPIO_CHIP_PATH "/dev/gpiochip0"

#endif



#define CCLI_GPIOD_CONSUMER "ccli"



#define CCI_GPIO_LL_MAX_LINES 64



typedef struct {

    struct gpiod_line_request *req;

    bool                       configured;

    bool                       is_output;

} GpioLineSlot;



static GpioLineSlot s_lines[CCI_GPIO_LL_MAX_LINES];

static bool         s_ll_ready;



static bool line_valid(int line) {

    return line >= 0 && line < CCI_GPIO_LL_MAX_LINES;

}



static enum gpiod_line_value raw_to_gpiod(int level) {

    return level ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;

}



static int gpiod_to_raw(enum gpiod_line_value val) {

    if (val == GPIOD_LINE_VALUE_ACTIVE) {

        return 1;

    }

    if (val == GPIOD_LINE_VALUE_INACTIVE) {

        return 0;

    }

    return 0;

}



static bool request_line(int line, enum gpiod_line_direction dir, int initial_level) {

    struct gpiod_chip           *chip = NULL;

    struct gpiod_line_settings  *settings = NULL;

    struct gpiod_line_config    *line_cfg = NULL;

    struct gpiod_request_config *req_cfg = NULL;

    struct gpiod_line_request   *req = NULL;

    unsigned int                 offset = (unsigned int)line;

    bool                         ok = false;



    chip = gpiod_chip_open(CCLI_GPIO_CHIP_PATH);

    if (chip == NULL) {

        fprintf(stderr, "gpio_ll: gpiod_chip_open(%s) failed: %s\n", CCLI_GPIO_CHIP_PATH,

                strerror(errno));

        goto out;

    }



    settings = gpiod_line_settings_new();

    if (settings == NULL) {

        goto out;

    }



    gpiod_line_settings_set_direction(settings, dir);



    line_cfg = gpiod_line_config_new();

    if (line_cfg == NULL) {

        goto out;

    }



    if (gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings) < 0) {

        goto out;

    }



    if (dir == GPIOD_LINE_DIRECTION_OUTPUT) {

        enum gpiod_line_value out_val = raw_to_gpiod(initial_level);



        if (gpiod_line_config_set_output_values(line_cfg, &out_val, 1) < 0) {

            goto out;

        }

    }



    req_cfg = gpiod_request_config_new();

    if (req_cfg == NULL) {

        goto out;

    }

    gpiod_request_config_set_consumer(req_cfg, CCLI_GPIOD_CONSUMER);



    req = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    if (req == NULL) {

        fprintf(stderr, "gpio_ll: gpiod_chip_request_lines(line=%d, %s) failed: %s\n", line,

                dir == GPIOD_LINE_DIRECTION_OUTPUT ? "output" : "input", strerror(errno));

        goto out;

    }



    fprintf(stderr, "gpio_ll: line %d (%s) on %s\n", line,

            dir == GPIOD_LINE_DIRECTION_OUTPUT ? "output" : "input", CCLI_GPIO_CHIP_PATH);



    s_lines[line].req = req;

    s_lines[line].configured = true;

    s_lines[line].is_output = (dir == GPIOD_LINE_DIRECTION_OUTPUT);

    ok = true;



out:

    gpiod_request_config_free(req_cfg);

    gpiod_line_config_free(line_cfg);

    gpiod_line_settings_free(settings);

    gpiod_chip_close(chip);

    return ok;

}



bool cci_gpio_ll_init(void) {

    memset(s_lines, 0, sizeof(s_lines));

    s_ll_ready = true;

    return true;

}



bool cci_gpio_ll_config_output(int line, int initial_level) {

    if (!s_ll_ready || !line_valid(line)) {

        return false;

    }

    if (s_lines[line].configured) {

        if (!s_lines[line].is_output) {

            return false;

        }

        return cci_gpio_ll_write(line, initial_level);

    }

    return request_line(line, GPIOD_LINE_DIRECTION_OUTPUT, initial_level);

}



bool cci_gpio_ll_config_input(int line) {

    if (!s_ll_ready || !line_valid(line)) {

        return false;

    }

    if (s_lines[line].configured) {

        return !s_lines[line].is_output;

    }

    return request_line(line, GPIOD_LINE_DIRECTION_INPUT, 0);

}



bool cci_gpio_ll_write(int line, int level) {

    if (!s_ll_ready || !line_valid(line) || !s_lines[line].configured ||

        !s_lines[line].is_output || s_lines[line].req == NULL) {

        return false;

    }

    if (gpiod_line_request_set_value(s_lines[line].req, (unsigned int)line,

                                     raw_to_gpiod(level)) < 0) {

        return false;

    }

    return true;

}



bool cci_gpio_ll_read(int line, int *level) {

    enum gpiod_line_value val;



    if (!s_ll_ready || !line_valid(line) || level == NULL ||

        !s_lines[line].configured || s_lines[line].req == NULL) {

        return false;

    }



    val = gpiod_line_request_get_value(s_lines[line].req, (unsigned int)line);

    if (val == GPIOD_LINE_VALUE_ERROR) {

        return false;

    }



    *level = gpiod_to_raw(val);

    return true;

}



bool cci_gpio_ll_read_relay(int channel, int *energized) {

    int level = 0;

    if (!cci_gpio_ll_read(channel, &level)) {

        return false;

    }

    if (energized != NULL) {

        *energized = level == 0 ? 1 : 0;

    }

    return true;

}



void cci_gpio_ll_shutdown(void) {

    for (int i = 0; i < CCI_GPIO_LL_MAX_LINES; ++i) {

        if (s_lines[i].req != NULL) {

            gpiod_line_request_release(s_lines[i].req);

            s_lines[i].req = NULL;

        }

        s_lines[i].configured = false;

        s_lines[i].is_output = false;

    }

    s_ll_ready = false;

}



#else /* !CCLI_GPIO_LL_LIBGPIOD — in-memory mock for host builds without hardware */



#define CCI_GPIO_LL_MAX_LINES 64



static int  s_level[CCI_GPIO_LL_MAX_LINES];

static bool s_configured[CCI_GPIO_LL_MAX_LINES];



static bool line_valid(int line) {

    return line >= 0 && line < CCI_GPIO_LL_MAX_LINES;

}



bool cci_gpio_ll_init(void) {

    for (int i = 0; i < CCI_GPIO_LL_MAX_LINES; ++i) {

        s_level[i] = 0;

        s_configured[i] = false;

    }

    return true;

}



bool cci_gpio_ll_config_output(int line, int initial_level) {

    if (!line_valid(line)) {

        return false;

    }

    s_configured[line] = true;

    s_level[line] = initial_level ? 1 : 0;

    return true;

}



bool cci_gpio_ll_config_input(int line) {

    if (!line_valid(line)) {

        return false;

    }

    s_configured[line] = true;

    return true;

}



bool cci_gpio_ll_write(int line, int level) {

    if (!line_valid(line)) {

        return false;

    }

    s_level[line] = level ? 1 : 0;

    return true;

}



bool cci_gpio_ll_read(int line, int *level) {

    if (!line_valid(line) || level == NULL) {

        return false;

    }

    *level = s_level[line];

    return true;

}



bool cci_gpio_ll_read_relay(int channel, int *energized) {

    if (!line_valid(channel) || energized == NULL) {

        return false;

    }

    *energized = s_level[channel] == 0 ? 1 : 0;

    return true;

}



void cci_gpio_ll_shutdown(void) {

    for (int i = 0; i < CCI_GPIO_LL_MAX_LINES; ++i) {

        s_configured[i] = false;

        s_level[i] = 0;

    }

}



#endif /* CCLI_GPIO_LL_LIBGPIOD */

