#!/usr/bin/env python3
"""Add an explicit scope-aware CRL parser; preserve the default strict APIs."""
import hashlib
import sys
from pathlib import Path

path = Path(sys.argv[1])
text = path.read_text()
marker = "CCLI_SCOPE_AWARE_CRL_PARSER"
if marker in text:
    sys.exit(0)
if hashlib.sha256(text.encode()).hexdigest() != "e984b69328f499eaf9b354a4b8010713641aff05d16848ed519b55d3721e477b":
    raise RuntimeError("Review changed Mbed TLS 3.6 x509_crl.c before applying native scope support")


def once(old, new):
    global text
    if text.count(old) != 1:
        raise RuntimeError("CRL patch anchor changed: " + old[:90])
    text = text.replace(old, new, 1)


once("static int x509_get_crl_ext(unsigned char **p,\n"
     "                            const unsigned char *end,\n"
     "                            mbedtls_x509_buf *ext)\n",
     "static int x509_get_crl_ext(unsigned char **p,\n"
     "                            const unsigned char *end,\n"
     "                            mbedtls_x509_buf *ext, int ccli_allow_idp)\n")
once("        /* Get OID (currently ignored) */",
     "        /* Native E2E validates IssuingDistributionPoint scope separately. */\n"
     "        int ccli_is_idp = 0;\n        /* Get OID (currently ignored) */")
once("        *p += len;\n\n        /* Get optional critical */",
     '        ccli_is_idp = len == 3 && memcmp(*p, "\\x55\\x1d\\x1c", 3) == 0;\n'
     "        *p += len;\n\n        /* Get optional critical */")
once("        if (is_critical) {", "        if (is_critical && !(ccli_allow_idp && ccli_is_idp)) {")
once("int mbedtls_x509_crl_parse_der(mbedtls_x509_crl *chain,\n"
     "                               const unsigned char *buf, size_t buflen)",
     "static int ccli_x509_crl_parse_der(mbedtls_x509_crl *chain,\n"
     "                               const unsigned char *buf, size_t buflen, int ccli_allow_idp)")
once("x509_get_crl_ext(&p, end, &crl->crl_ext)", "x509_get_crl_ext(&p, end, &crl->crl_ext, ccli_allow_idp)")
once("int mbedtls_x509_crl_parse(mbedtls_x509_crl *chain, const unsigned char *buf, size_t buflen)",
     "static int ccli_x509_crl_parse(mbedtls_x509_crl *chain, const unsigned char *buf, size_t buflen, int ccli_allow_idp)")
once("mbedtls_x509_crl_parse_der(chain,\n                                                  pem.buf, pem.buflen)",
     "ccli_x509_crl_parse_der(chain,\n                                                  pem.buf, pem.buflen, ccli_allow_idp)")
once("return mbedtls_x509_crl_parse_der(chain, buf, buflen);",
     "return ccli_x509_crl_parse_der(chain, buf, buflen, ccli_allow_idp);")
start = text.index("static int ccli_x509_crl_parse(mbedtls")
end = text.index("\n#if defined(MBEDTLS_FS_IO)", start)
wrappers = r'''
/* CCLI_SCOPE_AWARE_CRL_PARSER
 * Only the native validator uses the explicit scoped entry points. They retain
 * the original raw TBS/signature. The caller must enforce IDP scope before
 * passing a parsed CRL to path verification. TLS/default callers stay strict.
 */
int mbedtls_x509_crl_parse_der(mbedtls_x509_crl *chain, const unsigned char *buf, size_t size)
{ return ccli_x509_crl_parse_der(chain, buf, size, 0); }
int mbedtls_x509_crl_parse(mbedtls_x509_crl *chain, const unsigned char *buf, size_t size)
{ return ccli_x509_crl_parse(chain, buf, size, 0); }
int ccli_mbedtls_x509_crl_parse_der_scoped(mbedtls_x509_crl *chain, const unsigned char *buf, size_t size)
{ return ccli_x509_crl_parse_der(chain, buf, size, 1); }
int ccli_mbedtls_x509_crl_parse_scoped(mbedtls_x509_crl *chain, const unsigned char *buf, size_t size)
{ return ccli_x509_crl_parse(chain, buf, size, 1); }
'''
text = text[:end] + wrappers + text[end:]
path.write_text(text)
