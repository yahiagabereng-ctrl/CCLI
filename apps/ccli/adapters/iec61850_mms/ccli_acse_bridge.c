/*
 * CCLI hooks registered from mms_aare_auth.cpp; consumed by patched libiec61850 acse.c.
 * If acse.c lacks CCLI encode hooks, link still succeeds but P3-07 AARE auth is inactive.
 */
#include "ccli_acse_bridge.h"

static CcliAareAuthBuildFn s_aare_builder;
static const char* s_resp_ap_title;
static int s_resp_ae_qual;

void AcseConnection_setCcliAareAuthBuilder(CcliAareAuthBuildFn fn) {
    s_aare_builder = fn;
}

void AcseConnection_setCcliRespondingApTitle(const char* apTitle, int aeQualifier) {
    s_resp_ap_title = apTitle;
    s_resp_ae_qual = aeQualifier;
}

CcliAareAuthBuildFn ccli_acse_get_aare_auth_builder(void) {
    return s_aare_builder;
}

const char* ccli_acse_get_responding_ap_title(void) {
    return s_resp_ap_title;
}

int ccli_acse_get_responding_ae_qualifier(void) {
    return s_resp_ae_qual;
}
