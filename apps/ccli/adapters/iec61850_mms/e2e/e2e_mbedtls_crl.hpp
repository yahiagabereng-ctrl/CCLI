#pragma once
#include "mbedtls/x509_crl.h"
#if defined(CCLI_MBEDTLS_SCOPED_CRL)
extern "C" {
int ccli_mbedtls_x509_crl_parse_der_scoped(mbedtls_x509_crl*, const unsigned char*, size_t);
int ccli_mbedtls_x509_crl_parse_scoped(mbedtls_x509_crl*, const unsigned char*, size_t);
}
#endif
namespace ccli::mms::e2e {
inline int parse_native_crl_der(mbedtls_x509_crl* chain, const unsigned char* data, size_t size) {
#if defined(CCLI_MBEDTLS_SCOPED_CRL)
    return ccli_mbedtls_x509_crl_parse_der_scoped(chain, data, size);
#else
    return mbedtls_x509_crl_parse_der(chain, data, size);
#endif
}
inline int parse_native_crl(mbedtls_x509_crl* chain, const unsigned char* data, size_t size) {
#if defined(CCLI_MBEDTLS_SCOPED_CRL)
    return ccli_mbedtls_x509_crl_parse_scoped(chain, data, size);
#else
    return mbedtls_x509_crl_parse(chain, data, size);
#endif
}
}
