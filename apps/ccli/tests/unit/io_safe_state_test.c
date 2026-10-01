/*=============================================================================
 * File       :  io_safe_state_test.c
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  L1 unit tests for the DI/DO stack (svc_io + drv_gpio) against
 *               the inspectable mock GPIO HAL. No hardware required.
 *
 * Channels (config/lab_pi.yaml):
 *   DO curtail    -> line 5, active-low (OFF = HIGH = 1, typical relay module)
 *   DI permissive -> line 27, active-low   (ON  = LOW = 0)
 *=============================================================================*/
#include "svc_io.h"

#include "mock_gpio_ll.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DO_CURTAIL_LINE 5
#define DI_PERMISSIVE_LINE 27

static int check_level(const char *label, int line, int expected) {
    const int actual = mock_gpio_ll_get_level(line);
    if (actual != expected) {
        fprintf(stderr, "%s: line %d expected %d, got %d\n", label, line, expected, actual);
        return 0;
    }
    return 1;
}

int main(void) {
    /* L1-IO-01: init forces DO to safe (de-energized). Active-high -> LOW. */
    if (!svc_io_init()) {
        fprintf(stderr, "L1-IO-01: svc_io_init failed\n");
        return EXIT_FAILURE;
    }
    if (!check_level("L1-IO-01 boot safe", DO_CURTAIL_LINE, 1)) {
        return EXIT_FAILURE;
    }

    /* L1-IO-02: curtailment ON -> DO energized (active-high -> HIGH). */
    if (!svc_io_apply_curtailment(true)) {
        fprintf(stderr, "L1-IO-02: apply_curtailment(true) failed\n");
        return EXIT_FAILURE;
    }
    if (!check_level("L1-IO-02 curtail on", DO_CURTAIL_LINE, 0)) {
        return EXIT_FAILURE;
    }

    /* L1-IO-03: curtailment OFF -> DO de-energized. */
    if (!svc_io_apply_curtailment(false)) {
        fprintf(stderr, "L1-IO-03: apply_curtailment(false) failed\n");
        return EXIT_FAILURE;
    }
    if (!check_level("L1-IO-03 curtail off", DO_CURTAIL_LINE, 1)) {
        return EXIT_FAILURE;
    }

    /* L1-IO-04: safe_state overrides an active output (62443 CR 3.6). */
    (void)svc_io_apply_curtailment(true);
    svc_io_safe_state();
    if (!check_level("L1-IO-04 safe state", DO_CURTAIL_LINE, 1)) {
        return EXIT_FAILURE;
    }

    /* L1-IO-05: active-low DI. LOW = logical ON, HIGH = logical OFF. */
    bool ok = false;
    mock_gpio_ll_force_level(DI_PERMISSIVE_LINE, 0); /* wired closed */
    if (!svc_io_read_permissive(&ok) || ok != true) {
        fprintf(stderr, "L1-IO-05: permissive LOW expected ON\n");
        return EXIT_FAILURE;
    }
    mock_gpio_ll_force_level(DI_PERMISSIVE_LINE, 1); /* wired open */
    if (!svc_io_read_permissive(&ok) || ok != false) {
        fprintf(stderr, "L1-IO-05: permissive HIGH expected OFF\n");
        return EXIT_FAILURE;
    }

    printf("io_safe_state_test: all L1-IO cases passed\n");
    return EXIT_SUCCESS;
}
