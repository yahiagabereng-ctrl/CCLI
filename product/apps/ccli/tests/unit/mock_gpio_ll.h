/*=============================================================================
 * File       :  mock_gpio_ll.h
 *
 * Project    :  CCLI - CCI Central Plant Controller
 * Description:  Test-only inspection helpers for the mock low-level GPIO HAL.
 *=============================================================================*/
#ifndef CCI_MOCK_GPIO_LL_H
#define CCI_MOCK_GPIO_LL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Return the raw stored level of a line (0/1), or -1 if out of range. */
int mock_gpio_ll_get_level(int line);

/* Force the raw level of an input line, simulating field wiring. */
void mock_gpio_ll_force_level(int line, int level);

#ifdef __cplusplus
}
#endif

#endif /* CCI_MOCK_GPIO_LL_H */
