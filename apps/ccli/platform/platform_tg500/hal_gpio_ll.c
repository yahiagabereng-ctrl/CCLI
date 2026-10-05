/*=============================================================================
 * File       :  hal_gpio_ll.c  (platform_tg500)
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  TesPro TG544 / TR500 DIO via ubus dido_v2 (GD32 coprocessor).
 *
 * Logical GPIO line numbers map to ubus channel IDs (see lab_tr400.yaml io.*.gpio).
 *   gpio 1 → DIO1 / set_relay channel 1 (DO)
 *   gpio 2 → DIO2 / gd32.read_di di[1]     (DI)
 *
 * TesPro API notes (bench TG544, no public vendor doc — use `ubus -v list dido_v2`):
 *   - UCI `channel_N.mode=di` alone may leave `read_di` stale (`last_poll: 0`).
 *   - Always call `set_mode` + `gd32.set_mode` when configuring a DI channel.
 *   - Prefer `gd32.read_di` for hardware truth; fall back to `read_di`.
 *   - Bench: open -> state 0, DIO shorted to GND -> state 1 (matches smoke test).
 *=============================================================================*/
#include "cci/hal/gpio_ll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CCI_UBUS_CMD "ubus"
#define UBUS_BUF_SIZE 4096

static bool s_use_ubus = false;
static bool s_line_is_output[64];

static bool line_valid(int line) {
    return line >= 1 && line < 64;
}

static int ubus_relay_value_from_raw(int raw_level) {
    /* active-low sink: raw 0 (LOW) -> energise relay -> ubus value 1 */
    return raw_level == 0 ? 1 : 0;
}

static int raw_level_from_ubus_state(int ubus_state) {
    return ubus_state != 0 ? 0 : 1;
}

static bool ubus_run(const char *cmd, char *out, size_t out_size) {
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        return false;
    }

    if (out != NULL && out_size > 0) {
        size_t total = 0;
        out[0] = '\0';
        while (total + 1 < out_size) {
            char *dst = out + total;
            const size_t cap = out_size - total;
            if (fgets(dst, (int)cap, fp) == NULL) {
                break;
            }
            total += strlen(dst);
        }
    } else {
        char drain[256];
        while (fgets(drain, sizeof(drain), fp) != NULL) {
        }
    }

    const int status = pclose(fp);
    return status == 0;
}

static bool ubus_invoke(const char *cmd) {
    return ubus_run(cmd, NULL, 0);
}

static bool parse_int_after(const char *haystack, const char *needle, int *value) {
    const char *key = strstr(haystack, needle);
    if (key == NULL || value == NULL) {
        return false;
    }
    *value = atoi(key + strlen(needle));
    return true;
}

static bool parse_gd32_di_state(const char *json, int channel, int *ubus_state) {
    if (json == NULL || ubus_state == NULL || channel < 1 || channel > 4) {
        return false;
    }

    int di_mask = 0;
    if (parse_int_after(json, "\"di_mask\":", &di_mask)) {
        *ubus_state = (di_mask >> (channel - 1)) & 1;
        return true;
    }

    const char *di_key = strstr(json, "\"di\"");
    if (di_key == NULL) {
        return false;
    }
    const char *array = strchr(di_key, '[');
    if (array == NULL) {
        return false;
    }

    const int index = channel - 1;
    const char *cursor = array + 1;
    for (int i = 0; i <= index; ++i) {
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r') {
            ++cursor;
        }
        if (*cursor == '\0' || *cursor == ']') {
            return false;
        }
        if (i == index) {
            *ubus_state = (*cursor == '1') ? 1 : 0;
            return true;
        }
        cursor = strchr(cursor, ',');
        if (cursor == NULL) {
            return false;
        }
        ++cursor;
    }

    return false;
}

static bool ubus_read_gd32_di_state(int channel, int *ubus_state) {
    char buf[UBUS_BUF_SIZE];
    if (!ubus_run(CCI_UBUS_CMD " call dido_v2 gd32.read_di '{}' 2>/dev/null", buf,
                  sizeof(buf))) {
        return false;
    }
    if (strstr(buf, "\"success\": true") == NULL && strstr(buf, "\"success\":true") == NULL) {
        return false;
    }
    return parse_gd32_di_state(buf, channel, ubus_state);
}

static bool ubus_read_di_channel_state(int channel, int *ubus_state) {
    char cmd[160];
    char buf[UBUS_BUF_SIZE];

    snprintf(cmd, sizeof(cmd), "%s call dido_v2 read_di '{\"channel\":%d}' 2>/dev/null",
             CCI_UBUS_CMD, channel);
    if (!ubus_run(cmd, buf, sizeof(buf))) {
        return false;
    }
    if (!parse_int_after(buf, "\"state\":", ubus_state)) {
        return false;
    }
    return true;
}

static bool ubus_set_channel_mode_di(int channel) {
    char cmd[192];
    bool ok = false;

    snprintf(cmd, sizeof(cmd),
             "%s call dido_v2 set_mode '{\"channel\":%d,\"mode\":\"DI\"}' 2>/dev/null",
             CCI_UBUS_CMD, channel);
    ok = ubus_invoke(cmd) || ok;

    snprintf(cmd, sizeof(cmd),
             "%s call dido_v2 gd32.set_mode '{\"channel\":%d,\"mode\":\"DI\"}' 2>/dev/null",
             CCI_UBUS_CMD, channel);
    ok = ubus_invoke(cmd) || ok;

    return ok;
}

static bool ubus_read_di_state(int channel, int *ubus_state) {
    if (ubus_read_gd32_di_state(channel, ubus_state)) {
        return true;
    }
    return ubus_read_di_channel_state(channel, ubus_state);
}

bool cci_gpio_ll_init(void) {
    memset(s_line_is_output, 0, sizeof(s_line_is_output));

    s_use_ubus = ubus_invoke(CCI_UBUS_CMD " call dido_v2 status '{}' >/dev/null 2>&1");
    if (!s_use_ubus) {
        fprintf(stderr, "hal_gpio_ll: ubus dido_v2 unavailable — DIO will not actuate\n");
    } else {
        fprintf(stderr, "hal_gpio_ll: TesPro dido_v2 via ubus (gd32.read_di + set_mode)\n");
    }

    return true;
}

bool cci_gpio_ll_config_output(int line, int initial_level) {
    if (!line_valid(line)) {
        return false;
    }

    s_line_is_output[line] = true;

    if (!s_use_ubus) {
        return true;
    }

    char cmd[160];
    const int ubus_val = ubus_relay_value_from_raw(initial_level);
    snprintf(cmd, sizeof(cmd), "%s call dido_v2 set_relay '{\"channel\":%d,\"value\":%d}'",
             CCI_UBUS_CMD, line, ubus_val);
    return ubus_invoke(cmd);
}

bool cci_gpio_ll_config_input(int line) {
    if (!line_valid(line)) {
        return false;
    }

    s_line_is_output[line] = false;

    if (!s_use_ubus) {
        return true;
    }

    if (!ubus_set_channel_mode_di(line)) {
        fprintf(stderr, "hal_gpio_ll: set_mode DI failed for channel %d\n", line);
        return false;
    }

    return true;
}

bool cci_gpio_ll_write(int line, int level) {
    if (!line_valid(line)) {
        return false;
    }

    if (!s_use_ubus) {
        return true;
    }

    if (!s_line_is_output[line]) {
        return false;
    }

    char cmd[160];
    const int ubus_val = ubus_relay_value_from_raw(level);
    snprintf(cmd, sizeof(cmd), "%s call dido_v2 set_relay '{\"channel\":%d,\"value\":%d}'",
             CCI_UBUS_CMD, line, ubus_val);
    return ubus_invoke(cmd);
}

bool cci_gpio_ll_read(int line, int *level) {
    if (!line_valid(line) || level == NULL) {
        return false;
    }

    if (!s_use_ubus) {
        *level = 0;
        return true;
    }

    int ubus_state = 0;
    if (!ubus_read_di_state(line, &ubus_state)) {
        return false;
    }

    *level = raw_level_from_ubus_state(ubus_state);
    return true;
}

static bool ubus_read_relay_state(int channel, int *ubus_state) {
    char buf[UBUS_BUF_SIZE];
    char needle_a[32];
    char needle_b[32];

    if (ubus_state == NULL || channel < 1 || channel > 4) {
        return false;
    }

    if (!ubus_run(CCI_UBUS_CMD " call dido_v2 status 2>/dev/null", buf,
                  sizeof(buf))) {
        return false;
    }

    snprintf(needle_a, sizeof(needle_a), "\"channel\": %d", channel);
    snprintf(needle_b, sizeof(needle_b), "\"channel\":%d", channel);
    const char *p = strstr(buf, needle_a);
    if (p == NULL) {
        p = strstr(buf, needle_b);
    }
    if (p == NULL) {
        return false;
    }

    const char *state_key = strstr(p, "\"state\"");
    if (state_key == NULL) {
        return false;
    }

    return parse_int_after(state_key, ":", ubus_state);
}

bool cci_gpio_ll_read_relay(int channel, int *energized) {
    if (energized == NULL) {
        return false;
    }

    if (!s_use_ubus) {
        *energized = 0;
        return true;
    }

    int ubus_state = 0;
    if (!ubus_read_relay_state(channel, &ubus_state)) {
        return false;
    }

    *energized = ubus_state != 0 ? 1 : 0;
    return true;
}

void cci_gpio_ll_shutdown(void) {
    for (int line = 1; line < 64; ++line) {
        if (s_line_is_output[line]) {
            (void)cci_gpio_ll_write(line, 1);
        }
    }
}
