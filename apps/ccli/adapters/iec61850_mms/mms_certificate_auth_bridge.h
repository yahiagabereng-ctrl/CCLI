#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void CcliDualCert_setAareAuthBuilder(int (*builder)(uint8_t*, int));
void CcliDualCert_setResponderAddress(const char* ap_title, int ae_qualifier);
#ifdef __cplusplus
}
#endif
