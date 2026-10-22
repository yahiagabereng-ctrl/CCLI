/**
 * lib60870 CS104 uses Hal_getMonotonicTimeInMs; libiec61850 HAL omits it.
 * Single-function supplement when lib60870 is built WITHOUT_HAL.
 */
#include <stdint.h>
#include <time.h>

uint64_t
Hal_getMonotonicTimeInMs(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return ((uint64_t)ts.tv_sec * 1000ULL) + (uint64_t)(ts.tv_nsec / 1000000);
}

uint64_t
Hal_getMonotonicTimeInNs(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
}
