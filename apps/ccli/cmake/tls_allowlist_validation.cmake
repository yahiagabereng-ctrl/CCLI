# An individual certificate pin is an additional constraint, not permission to
# erase revocation, expiry, usage or chain errors. Keep vendored wire sources
# intact and generate the narrowly corrected verifier for reproducible builds.
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# The pinned libIEC61850 v1.6.0 HAL does not expose the additional server
# identity API used by CCLI. Generate a narrow header overlay instead of
# modifying the submodule checkout, so a clean clone builds from its gitlink.
# The HAL also uses the parent repo's adapters/security/mbedtls_config.h,
# which carries the required ECDHE/ECDSA feature modules.
set(CCLI_LIBIEC61850_TLS_OVERLAY_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/libiec61850/include")
file(MAKE_DIRECTORY "${CCLI_LIBIEC61850_TLS_OVERLAY_INCLUDE_DIR}")
set(_tls_api_header "${CCLI_LIBIEC61850_DIR}/hal/inc/tls_config.h")
file(READ "${_tls_api_header}" _tls_api_header_text)
string(FIND "${_tls_api_header_text}" "TLSConfiguration_addOwnCertificateAndKeyFromFiles" _tls_api_position)
if(_tls_api_position EQUAL -1)
    string(REGEX MATCH "PAL_API bool[ \t\r\n]+TLSConfiguration_setOwnCertificateFromFile\\(TLSConfiguration self, const char\\* filename\\);" _tls_api_anchor "${_tls_api_header_text}")
    if(NOT _tls_api_anchor)
        message(FATAL_ERROR "Review additional TLS identity API overlay for this libIEC61850 revision")
    endif()
    set(_tls_api_declaration [=[

/**
 * Add an additional server certificate/private-key identity.
 * The certificate and key must match; this API is server-only.
 */
PAL_API bool
TLSConfiguration_addOwnCertificateAndKeyFromFiles(TLSConfiguration self,
                                                    const char* certificateFilename,
                                                    const char* keyFilename,
                                                    const char* keyPassword);]=])
    string(REPLACE "${_tls_api_anchor}" "${_tls_api_anchor}${_tls_api_declaration}" _tls_api_header_text "${_tls_api_header_text}")
endif()
set(_tls_api_header_overlay "${CCLI_LIBIEC61850_TLS_OVERLAY_INCLUDE_DIR}/tls_config.h")
set(_tls_api_header_existing "")
if(EXISTS "${_tls_api_header_overlay}")
    file(READ "${_tls_api_header_overlay}" _tls_api_header_existing)
endif()
if(NOT _tls_api_header_existing STREQUAL _tls_api_header_text)
    file(WRITE "${_tls_api_header_overlay}" "${_tls_api_header_text}")
endif()

foreach(_tls_target hal hal-shared)
    if(NOT TARGET ${_tls_target})
        continue()
    endif()
    get_target_property(_tls_sources ${_tls_target} SOURCES)
    get_target_property(_tls_source_dir ${_tls_target} SOURCE_DIR)
    set(_tls_corrected_sources "")
    foreach(_tls_source IN LISTS _tls_sources)
        if(_tls_source MATCHES "[/\\]tls[/\\](mbedtls3|mbedtls)[/\\]tls_mbedtls\\.c$")
            set(_tls_backend "${CMAKE_MATCH_1}")
            if(IS_ABSOLUTE "${_tls_source}")
                set(_tls_original "${_tls_source}")
            else()
                set(_tls_original "${_tls_source_dir}/${_tls_source}")
            endif()
            file(READ "${_tls_original}" _tls_text)
            set(_tls_pattern "if \\(certMatches\\)[ \t\r\n]+\\*flags = 0;[ \t\r\n]+else")
            # Count without a semicolon: CMake list separators would otherwise
            # split a single C statement into multiple list elements.
            string(REGEX MATCHALL "if \\(certMatches\\)[ \t\r\n]+\\*flags = 0" _tls_matches "${_tls_text}")
            list(LENGTH _tls_matches _tls_match_count)
            if(NOT _tls_match_count EQUAL 1)
                message(FATAL_ERROR "Review TLS allowlist correction for this libIEC61850 revision")
            endif()
            set(_tls_replacement "if (certMatches) {\n                /* CCLI: pins must preserve certificate/CRL validation errors. */\n                if (self->tlsConfig->chainValidation == false)\n                    *flags &= ~MBEDTLS_X509_BADCERT_NOT_TRUSTED;\n            }\n            else")
            string(REGEX REPLACE "${_tls_pattern}" "${_tls_replacement}" _tls_text "${_tls_text}")
            set(_tls_generated "${CMAKE_BINARY_DIR}/generated/tls/${_tls_backend}/tls_mbedtls.c")
            file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated/tls/${_tls_backend}")
            if(NOT _tls_backend STREQUAL "mbedtls3")
                message(FATAL_ERROR "The reviewed IEC 62351-3 profile requires the pinned mbedTLS 3.6 backend")
            endif()
            set(_tls_intermediate "${_tls_generated}.input")
            file(WRITE "${_tls_intermediate}" "${_tls_text}")
            execute_process(COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/iec62351_tls_overlay.py" "${_tls_intermediate}"
                RESULT_VARIABLE _tls_patch_result)
            if(NOT _tls_patch_result EQUAL 0)
                message(FATAL_ERROR "IEC 62351-3 TLS overlay failed")
            endif()
            file(READ "${_tls_intermediate}" _tls_text)
            set(_tls_existing "")
            if(EXISTS "${_tls_generated}")
                file(READ "${_tls_generated}" _tls_existing)
            endif()
            if(NOT _tls_existing STREQUAL _tls_text)
                file(WRITE "${_tls_generated}" "${_tls_text}")
            endif()
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_tls_original}")
            list(APPEND _tls_corrected_sources "${_tls_generated}")
        elseif(IS_ABSOLUTE "${_tls_source}" OR _tls_source MATCHES "^\\$<")
            list(APPEND _tls_corrected_sources "${_tls_source}")
        else()
            list(APPEND _tls_corrected_sources "${_tls_source_dir}/${_tls_source}")
        endif()
    endforeach()
    set_property(TARGET ${_tls_target} PROPERTY SOURCES "${_tls_corrected_sources}")
    target_include_directories(${_tls_target} BEFORE PRIVATE "${CCLI_LIBIEC61850_TLS_OVERLAY_INCLUDE_DIR}")
    target_include_directories(${_tls_target} BEFORE PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/security")
    # The strict ClientHello callback reads the pinned 3.6 parser's signature
    # list before the server can use its legacy SHA-1 fallback.
    target_include_directories(${_tls_target} PRIVATE
        "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0/library")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${_tls_api_header}"
        "${CMAKE_CURRENT_LIST_DIR}/iec62351_tls_overlay.py"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/security/iec62351_backend.inc")
endforeach()
