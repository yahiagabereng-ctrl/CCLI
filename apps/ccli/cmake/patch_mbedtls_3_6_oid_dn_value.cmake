# Mbed TLS 3.6's X.509 Name parser only accepts string AVAs, but IEC 62351-4
# Annex G requires an OBJECT IDENTIFIER-valued subject attribute. Apply this
# narrow, version-pinned source patch to the local Mbed TLS release tree.
function(ccli_patch_mbedtls_3_6_oid_dn_value source_file)
    if(NOT EXISTS "${source_file}")
        message(FATAL_ERROR "Mbed TLS 3.6 source is missing: ${source_file}")
    endif()

    file(READ "${source_file}" _mbedtls_x509_source)
    set(_patched_marker "if (**p != MBEDTLS_ASN1_OID && **p != MBEDTLS_ASN1_BMP_STRING &&")
    string(FIND "${_mbedtls_x509_source}" "${_patched_marker}" _already_patched)
    if(NOT _already_patched EQUAL -1)
        return()
    endif()
    # Recognize the equivalent spacing in a source tree patched manually
    # before this configure-time helper was added.
    set(_manual_patch_marker "if (**p != MBEDTLS_ASN1_OID            && **p != MBEDTLS_ASN1_BMP_STRING &&")
    string(FIND "${_mbedtls_x509_source}" "${_manual_patch_marker}" _manual_patch_found)
    if(NOT _manual_patch_found EQUAL -1)
        return()
    endif()

    set(_original_marker "if (**p != MBEDTLS_ASN1_BMP_STRING &&")
    string(FIND "${_mbedtls_x509_source}" "${_original_marker}" _original_found)
    if(_original_found EQUAL -1)
        message(FATAL_ERROR
            "Unsupported Mbed TLS 3.6 x509.c; cannot apply the IEC 62351-4 OID-valued subject patch")
    endif()

    string(REPLACE "${_original_marker}"
                   "${_patched_marker}"
                   _mbedtls_x509_source "${_mbedtls_x509_source}")
    file(WRITE "${source_file}" "${_mbedtls_x509_source}")
endfunction()
