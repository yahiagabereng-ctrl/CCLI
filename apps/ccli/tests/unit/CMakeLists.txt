cmake_minimum_required(VERSION 3.16)

add_executable(tls_allowlist_config_test tls_allowlist_config_test.cpp)
target_link_libraries(tls_allowlist_config_test PRIVATE ccli_core)
add_test(NAME tls_allowlist_repeated_entries COMMAND tls_allowlist_config_test)

add_executable(aggregator_power_test aggregator_power_test.cpp)
target_include_directories(aggregator_power_test PRIVATE ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms)
target_link_libraries(aggregator_power_test PRIVATE ccli_core)
add_test(NAME aggregator_power_arbitration COMMAND aggregator_power_test)

add_executable(pf2_fsm_test pf2_fsm_test.cpp)
target_link_libraries(pf2_fsm_test PRIVATE ccli_core)

add_test(NAME pf2_fsm_normal COMMAND pf2_fsm_test)

add_executable(dso_phase1_test dso_phase1_test.cpp)
target_link_libraries(dso_phase1_test PRIVATE ccli_core)
add_test(NAME dso_phase1_regulation COMMAND dso_phase1_test)

add_executable(setpoint_gate_test setpoint_gate_test.cpp)
target_link_libraries(setpoint_gate_test PRIVATE ccli_core)
add_test(NAME dso_r07_setpoint_spacing COMMAND setpoint_gate_test)

add_executable(service_supervision_test service_supervision_test.cpp)
target_link_libraries(service_supervision_test PRIVATE ccli_core)
add_test(NAME service_supervision COMMAND service_supervision_test)

add_executable(event_store_test event_store_test.cpp)
target_link_libraries(event_store_test PRIVATE ccli_core)
add_test(NAME event_store_o14 COMMAND event_store_test)
add_executable(iec62351_logger_test iec62351_logger_test.cpp)
target_link_libraries(iec62351_logger_test PRIVATE ccli_core)
add_test(NAME iec62351_logger COMMAND iec62351_logger_test)
add_executable(platform_event_monitor_test platform_event_monitor_test.cpp)
target_link_libraries(platform_event_monitor_test PRIVATE ccli_core)
add_test(NAME platform_auth_account_events COMMAND platform_event_monitor_test)

find_package(Threads REQUIRED)
add_executable(event_store_rotation_test event_store_rotation_test.cpp)
target_link_libraries(event_store_rotation_test PRIVATE ccli_core Threads::Threads)
add_test(NAME event_store_rotation COMMAND event_store_rotation_test)

# DI/DO L1 test: compile the C driver + service against an inspectable mock HAL
# (no platform lib / no hardware) so we can inject inputs and read raw outputs.
add_executable(io_safe_state_test
    io_safe_state_test.c
    mock_gpio_ll.c
    ${CMAKE_SOURCE_DIR}/adapters/io/drv_gpio.c
    ${CMAKE_SOURCE_DIR}/services/svc_io.c
)
target_include_directories(io_safe_state_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/adapters/io
    ${CMAKE_SOURCE_DIR}/services
    ${CMAKE_SOURCE_DIR}/platform/include
)

add_test(NAME io_safe_state COMMAND io_safe_state_test)

# This test deliberately avoids the target-only adapter library. It verifies
# certificate-role parsing and commissioning policy decisions on host systems
# that do not provide the Linux/TG544 platform dependencies.
add_executable(mms_acse_auth_test
    mms_acse_auth_test.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/mms_acse_auth.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/mms_rbac.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/mms_xacml.cpp
    ${CMAKE_SOURCE_DIR}/third_party/pugixml/pugixml.cpp
)
target_include_directories(mms_acse_auth_test PRIVATE
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms
)
target_compile_definitions(mms_acse_auth_test PRIVATE CCLI_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME mms_acse_p307_acse_auth COMMAND mms_acse_auth_test)

# IEC 62351-4 host-only wire codec and transfer-state checks. Keep the sources
# direct so this target needs neither the platform adapter nor plant hardware.
add_executable(e2e_codec_test
    e2e_codec_test.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/ber_codec.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/e2e_codec.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/e2e_time.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/transfer_sequence.cpp
)
target_include_directories(e2e_codec_test PRIVATE
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e
)
add_test(NAME e2e_codec COMMAND e2e_codec_test)

add_executable(e2e_ber_field_test e2e_ber_field_test.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/ber_codec.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/e2e_codec.cpp
    ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e/e2e_time.cpp)
target_include_directories(e2e_ber_field_test PRIVATE ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e)
option(CCLI_E2E_BER_SANITIZERS "Instrument the standalone BER/field corpus with ASan and UBSan" OFF)
if(CCLI_E2E_BER_SANITIZERS)
    target_compile_options(e2e_ber_field_test PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(e2e_ber_field_test PRIVATE -fsanitize=address,undefined)
endif()
add_test(NAME e2e_ber_fields COMMAND e2e_ber_field_test
    ${CMAKE_BINARY_DIR}/e2e-ber-field-assertions.tsv)
set_tests_properties(e2e_ber_fields PROPERTIES TIMEOUT 180)

if(CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc)
    add_executable(e2e_crypto_test e2e_crypto_test.cpp)
    target_include_directories(e2e_crypto_test PRIVATE
        ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e)
    target_link_libraries(e2e_crypto_test PRIVATE ccli_adapters)
    if(UNIX)
        find_package(Python3 COMPONENTS Interpreter REQUIRED)
        execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import cryptography"
                        RESULT_VARIABLE _e2e_crypto_available)
        if(NOT _e2e_crypto_available EQUAL 0)
            message(FATAL_ERROR "E2E crypto provider tests require Python cryptography")
        endif()
        set(_e2e_crypto_fixtures "${CMAKE_BINARY_DIR}/e2e-crypto-fixtures")
        add_test(NAME e2e_crypto_generate_fixtures
            COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/e2e_crypto_fixtures.py"
                    --out "${_e2e_crypto_fixtures}")
        set_tests_properties(e2e_crypto_generate_fixtures PROPERTIES
            FIXTURES_SETUP e2e_crypto_fixtures)
        add_test(NAME e2e_crypto_provider_checks
            COMMAND e2e_crypto_test "${_e2e_crypto_fixtures}")
        set_tests_properties(e2e_crypto_provider_checks PROPERTIES
            FIXTURES_REQUIRED e2e_crypto_fixtures)
    endif()
endif()

if(CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc AND UNIX AND
   EXISTS "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0")
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import cryptography"
                    RESULT_VARIABLE _e2e_identity_crypto_available)
    if(NOT _e2e_identity_crypto_available EQUAL 0)
        message(FATAL_ERROR "E2E identity tests require Python cryptography")
    endif()
    set(_e2e_identity_fixtures "${CMAKE_BINARY_DIR}/e2e-identity-fixtures")
    add_test(NAME e2e_identity_generate_fixtures
        COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/e2e_identity_fixtures.py"
                --out "${_e2e_identity_fixtures}")
    set_tests_properties(e2e_identity_generate_fixtures PROPERTIES
        FIXTURES_SETUP e2e_identity_fixtures)
    add_executable(e2e_identity_test e2e_identity_test.cpp)
    target_include_directories(e2e_identity_test PRIVATE
        ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e
        "${_ccli_mbedtls_inc}" "${_ccli_mbedtls_config_inc}")
    target_compile_definitions(e2e_identity_test PRIVATE MBEDTLS_CONFIG_FILE=\"mbedtls_config.h\")
    target_link_libraries(e2e_identity_test PRIVATE ccli_adapters)
    add_test(NAME e2e_identity_profile_checks
        COMMAND e2e_identity_test "${_e2e_identity_fixtures}")
    set_tests_properties(e2e_identity_profile_checks PROPERTIES
        FIXTURES_REQUIRED e2e_identity_fixtures)

    add_executable(e2e_native_server_test e2e_native_server_test.cpp)
    target_include_directories(e2e_native_server_test PRIVATE
        ${CMAKE_SOURCE_DIR}/adapters/iec61850_mms/e2e
        "${CCLI_LIBIEC61850_DIR}/src/iec61850/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc"
        "${CCLI_LIBIEC61850_DIR}/src/logging"
        "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${_iec_bin}/config")
    target_compile_definitions(e2e_native_server_test PRIVATE CONFIG_MMS_SUPPORT_TLS=1)
    target_link_libraries(e2e_native_server_test PRIVATE ccli_adapters)
    add_test(NAME e2e_native_stack
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/e2e_native_stack_test.py"
            --server $<TARGET_FILE:e2e_native_server_test>
            --fixtures "${CMAKE_BINARY_DIR}/e2e-native-fixtures"
            --probe "${CMAKE_SOURCE_DIR}/tools/mms-client/e2e_probe.py")
    set_tests_properties(e2e_native_stack PROPERTIES TIMEOUT 60)
    add_test(NAME e2e_native_resilience
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/e2e_native_stack_test.py"
            --server $<TARGET_FILE:e2e_native_server_test>
            --fixtures "${CMAKE_BINARY_DIR}/e2e-resilience-fixtures"
            --probe "${CMAKE_SOURCE_DIR}/tools/mms-client/e2e_probe.py" --resilience)
    set_tests_properties(e2e_native_resilience PROPERTIES TIMEOUT 180)
    add_test(NAME e2e_native_field_campaign
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/e2e_native_stack_test.py"
            --server $<TARGET_FILE:e2e_native_server_test>
            --fixtures "${CMAKE_BINARY_DIR}/e2e-field-fixtures"
            --probe "${CMAKE_SOURCE_DIR}/tools/mms-client/e2e_probe.py" --field-campaign)
    set_tests_properties(e2e_native_field_campaign PROPERTIES TIMEOUT 600)
    add_custom_target(e2e_host_checks_build DEPENDS e2e_native_server_test
        e2e_codec_test e2e_ber_field_test e2e_identity_test e2e_crypto_test)
endif()

if(CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc AND UNIX)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import cryptography"
                    RESULT_VARIABLE _rbac_crypto_available)
    if(NOT _rbac_crypto_available EQUAL 0)
        message(FATAL_ERROR "RBAC integration tests require Python cryptography")
    endif()
    set(_rbac_fixtures "${CMAKE_BINARY_DIR}/rbac-security-fixtures")
    add_test(NAME mms_rbac_generate_certificates
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tests/integration/rbac_fixture.py"
                --source "${CMAKE_SOURCE_DIR}" --out "${_rbac_fixtures}")
    set_tests_properties(mms_rbac_generate_certificates PROPERTIES FIXTURES_SETUP rbac_certificates)
    add_executable(mms_rbac_security_test mms_rbac_security_test.cpp)
    target_link_libraries(mms_rbac_security_test PRIVATE ccli_adapters)
    # Explicitly run against a locally provisioned bundle; not an automatic
    # CTest dependency on private credentials or a running EJBCA service.
    add_executable(mms_rbac_commissioned_pki_check ../integration/mms_rbac_commissioned_pki_check.cpp)
    target_link_libraries(mms_rbac_commissioned_pki_check PRIVATE ccli_adapters)
    add_test(NAME mms_rbac_signed_security COMMAND mms_rbac_security_test "${_rbac_fixtures}")
    set_tests_properties(mms_rbac_signed_security PROPERTIES FIXTURES_REQUIRED rbac_certificates)
    add_executable(mms_rbac_tls_test ../integration/mms_rbac_tls_test.cpp)
    target_link_libraries(mms_rbac_tls_test PRIVATE ccli_adapters)
    target_compile_definitions(mms_rbac_tls_test PRIVATE CONFIG_MMS_SUPPORT_TLS=1)
    target_include_directories(mms_rbac_tls_test PRIVATE
        "${CCLI_LIBIEC61850_DIR}/src/iec61850/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc"
        "${CCLI_LIBIEC61850_DIR}/src/logging"
        "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${_iec_bin}/config")
    add_test(NAME mms_rbac_tls_authorization COMMAND mms_rbac_tls_test "${_rbac_fixtures}")
    add_executable(iec62351_tls_probe ../integration/iec62351_tls_probe.cpp
        "${CCLI_LIBIEC61850_DIR}/src/common/linked_list.c")
    target_link_libraries(iec62351_tls_probe PRIVATE ccli_adapters)
    target_include_directories(iec62351_tls_probe PRIVATE "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc" "${CCLI_LIBIEC61850_DIR}/src/mms/inc" "${_iec_bin}/config")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(iec62351_tls_probe PRIVATE CCLI_TLS_TEST_CLOCK=1)
        target_link_options(iec62351_tls_probe PRIVATE "-Wl,--wrap=clock_gettime")
    endif()
    add_test(NAME iec62351_tls_policy
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tests/integration/iec62351_tls_test.py"
            --server $<TARGET_FILE:iec62351_tls_probe>)
    set_tests_properties(iec62351_tls_policy PROPERTIES TIMEOUT 90)
    if(CCLI_HAVE_LIB60870)
        add_test(NAME iec62351_iec104_tls_policy
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tests/integration/iec62351_tls_test.py"
                --server $<TARGET_FILE:iec62351_tls_probe> --iec104)
        set_tests_properties(iec62351_iec104_tls_policy PROPERTIES TIMEOUT 90)
    endif()
    set_tests_properties(mms_rbac_tls_authorization PROPERTIES FIXTURES_REQUIRED rbac_certificates TIMEOUT 60)
    add_test(NAME mms_rbac_tls_source_baseline
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/../../scripts/check_rbac_tls_baseline.py")
    if(TARGET e2e_host_checks_build)
        add_dependencies(e2e_host_checks_build mms_acse_auth_test
            mms_rbac_security_test mms_rbac_tls_test)
    endif()
endif()
