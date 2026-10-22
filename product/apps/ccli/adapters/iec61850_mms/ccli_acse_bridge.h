#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Build 62351-4 MMS-Authentication-value for AARE (responder). Returns length or -1. */
typedef int (*CcliAareAuthBuildFn)(uint8_t* out, int max_out);

void AcseConnection_setCcliAareAuthBuilder(CcliAareAuthBuildFn fn);

/** 62351-4 §10.5.3 responding-AP-title / responding-AE-qualifier in AARE. */
void AcseConnection_setCcliRespondingApTitle(const char* apTitle, int aeQualifier);

CcliAareAuthBuildFn ccli_acse_get_aare_auth_builder(void);
const char* ccli_acse_get_responding_ap_title(void);
int ccli_acse_get_responding_ae_qualifier(void);

#ifdef __cplusplus
}
#endif
