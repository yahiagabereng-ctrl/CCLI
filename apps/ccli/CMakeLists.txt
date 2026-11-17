cmake_minimum_required(VERSION 3.16)
project(ccli LANGUAGES CXX C VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_PLATFORM "tg500" CACHE STRING "Target platform: tg500 (TesPro MT798X)")
set_property(CACHE BUILD_PLATFORM PROPERTY STRINGS tg500)
option(CCLI_BUILD_MMS_CLIENT "Build the commissioning client" OFF)
option(CCLI_MMS_TSP_CLIENT_PROFILE "Use the observed TSP client association encoding" OFF)
if(CCLI_MMS_TSP_CLIENT_PROFILE AND NOT CCLI_BUILD_MMS_CLIENT)
    message(FATAL_ERROR "The TSP client overlay requires CCLI_BUILD_MMS_CLIENT=ON")
endif()

include(cmake/platform.cmake)
include(cmake/ccli_version.cmake)
ccli_generate_version_header()

# --- libiec61850 (Phase 3 MMS) ------------------------------------------------
option(CCLI_WITH_LIBIEC61850 "Build IEC 61850 MMS with third_party/libiec61850" ON)
# Cert/product builds: OFF (5-LN CID has no GoCB). Lab GOOSE experiments: -DCCLI_WITH_GOOSE=ON
option(CCLI_WITH_GOOSE "Build IEC 61850 L2 GOOSE (plant LAN) with libiec61850" OFF)
set(CCLI_LIBIEC61850_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libiec61850")
set(CCLI_HAVE_LIBIEC61850 0)
if(CCLI_WITH_LIBIEC61850 AND EXISTS "${CCLI_LIBIEC61850_DIR}/CMakeLists.txt")
    set(BUILD_EXAMPLES OFF CACHE BOOL "libiec61850 examples" FORCE)
    set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "libiec61850 python" FORCE)
    if(CCLI_WITH_GOOSE)
        set(CONFIG_INCLUDE_GOOSE_SUPPORT ON CACHE BOOL "libiec61850 GOOSE" FORCE)
        set(CONFIG_IEC61850_L2_GOOSE ON CACHE BOOL "libiec61850 L2 GOOSE" FORCE)
    else()
        set(CONFIG_INCLUDE_GOOSE_SUPPORT OFF CACHE BOOL "libiec61850 GOOSE" FORCE)
        set(CONFIG_IEC61850_L2_GOOSE OFF CACHE BOOL "libiec61850 L2 GOOSE" FORCE)
    endif()
    set(CONFIG_IEC61850_L2_SMV OFF CACHE BOOL "libiec61850 L2 SMV" FORCE)
    set(CONFIG_IEC61850_R_GOOSE OFF CACHE BOOL "libiec61850 R-GOOSE" FORCE)
    set(CONFIG_IEC61850_R_SMV OFF CACHE BOOL "libiec61850 R-SMV" FORCE)
    add_subdirectory("${CCLI_LIBIEC61850_DIR}" EXCLUDE_FROM_ALL)
    include(cmake/rbac_reporting.cmake)
    include(cmake/tls_allowlist_validation.cmake)
    include(cmake/mms_certificate_auth.cmake)
    set(CCLI_HAVE_LIBIEC61850 1)
    message(STATUS "CCLI MMS: libiec61850 enabled (${CCLI_LIBIEC61850_DIR})")
else()
    message(STATUS "CCLI MMS: stub (third_party/libiec61850 missing or disabled)")
endif()

# --- lib60870 (Phase 4 CS104) -------------------------------------------------
option(CCLI_WITH_LIB60870 "Build IEC 60870-5-104 with third_party/lib60870" ON)
set(CCLI_LIB60870_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib60870/lib60870-C")
set(CCLI_HAVE_LIB60870 0)
if(CCLI_WITH_LIB60870 AND EXISTS "${CCLI_LIB60870_DIR}/CMakeLists.txt")
    set(BUILD_EXAMPLES OFF CACHE BOOL "lib60870 examples" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "lib60870 tests" FORCE)
    # Reuse libiec61850 HAL; supplement Hal_getMonotonicTimeInMs in hal60870_monotonic.c
    set(BUILD_HAL OFF CACHE BOOL "lib60870 HAL" FORCE)
    set(BUILD_COMMON OFF CACHE BOOL "lib60870 common" FORCE)
    add_subdirectory("${CCLI_LIB60870_DIR}" EXCLUDE_FROM_ALL)
    # CCLI's IEC 104 TLS calls are served by the patched libIEC61850 HAL.
    # lib60870 must compile protocol code only; a bundled HAL would introduce
    # an unpatched TLS backend (and duplicate HAL symbols) into the executable.
    foreach(_ccli_lib60870_target lib60870 lib60870-shared)
        if(TARGET ${_ccli_lib60870_target})
            get_target_property(_ccli_lib60870_sources ${_ccli_lib60870_target} SOURCES)
            foreach(_ccli_lib60870_source IN LISTS _ccli_lib60870_sources)
                if(_ccli_lib60870_source MATCHES "[/\\\\]hal[/\\\\]tls[/\\\\]mbedtls(3)?[/\\\\]tls_mbedtls\\.c$")
                    message(FATAL_ERROR
                        "${_ccli_lib60870_target} compiles its own TLS HAL. "
                        "CCLI IEC 104 TLS must use the reviewed libIEC61850 HAL.")
                endif()
            endforeach()
        endif()
    endforeach()
    # libiec61850 HAL lacks Hal_getMonotonicTimeInMs; embed supplement in lib60870.
    target_sources(lib60870 PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec60870_104/hal60870_monotonic.c")
    set(CCLI_HAVE_LIB60870 1)
    message(STATUS "CCLI 104: lib60870 enabled (${CCLI_LIB60870_DIR})")
else()
    message(STATUS "CCLI 104: stub (third_party/lib60870 missing or disabled)")
endif()

option(CCLI_REQUIRE_PROTOCOL_DEPENDENCIES "Fail rather than stub requested MMS/IEC104 protocols" OFF)
if(CCLI_HAVE_LIBIEC61850)
    include(cmake/tls_idle_tick.cmake)
    ccli_tls_idle_tick(iec61850 iso_connection.c mms)
    ccli_tls_idle_tick(iec61850-shared iso_connection.c mms)
    ccli_tls_idle_tick(lib60870 cs104_slave.c iec104)
endif()
if(CCLI_REQUIRE_PROTOCOL_DEPENDENCIES)
    if((CCLI_WITH_LIBIEC61850 AND NOT CCLI_HAVE_LIBIEC61850) OR
       (CCLI_WITH_LIB60870 AND NOT CCLI_HAVE_LIB60870))
        message(FATAL_ERROR "A requested protocol dependency is missing. Initialize the pinned submodules before building a deployment candidate.")
    endif()
endif()

add_library(ccli_core STATIC
    core/pf2/pf2_fsm.cpp
    core/dso/dso_active_power.cpp
    core/dso/dso_reactive_power.cpp
    core/dso/dso_phase1.cpp
    core/measurement/measurement_store.cpp
    core/event/event_ring.cpp
    core/event/event_store.cpp
    core/event/iec62351_event.cpp
    core/event/platform_event_monitor.cpp
    core/event/time_sync_tracker.cpp
    adapters/security/syslog_sender.cpp
    core/policy/net_policy.cpp
    core/config/ccli_config.cpp
    core/regulation/regulation_bench.cpp
    core/service/service_supervision.cpp
    core/version/ccli_version.cpp
)
target_include_directories(ccli_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
    ${CMAKE_BINARY_DIR}/generated
)
find_package(Threads REQUIRED)
target_link_libraries(ccli_core PUBLIC Threads::Threads)

set(CCLI_ADAPTERS_SOURCES
    adapters/modbus_master/modbus_adapter.cpp
    adapters/modbus_master/modbus_pdc_decode.cpp
    adapters/modbus_master/modbus_emt432_commissioning.cpp
    adapters/iec61850_goose/goose_adapter.cpp
    adapters/iec61850_mms/mms_adapter.cpp
    adapters/iec61850_mms/mms_acse_auth.cpp
    adapters/iec61850_mms/e2e/ber_codec.cpp
    adapters/iec61850_mms/e2e/e2e_codec.cpp
    adapters/iec61850_mms/e2e/e2e_association.cpp
    adapters/iec61850_mms/e2e/e2e_server.cpp
    adapters/iec61850_mms/e2e/e2e_crypto.cpp
    adapters/iec61850_mms/e2e/e2e_identity.cpp
    adapters/iec61850_mms/e2e/e2e_time.cpp
    adapters/iec61850_mms/e2e/e2e_key_schedule.cpp
    adapters/iec61850_mms/e2e/transfer_sequence.cpp
    adapters/iec61850_mms/mms_rbac.cpp
    adapters/iec61850_mms/mms_xacml.cpp
    adapters/iec61850_mms/mms_rbac_security.cpp
    third_party/pugixml/pugixml.cpp
    adapters/iec61850_mms/mms_aare_auth.cpp
    adapters/iec61850_mms/ccli_acse_bridge.c
    adapters/iec61850_mms/gnss_time.cpp
    adapters/iec60870_104/iec104_adapter.cpp
    adapters/iec60870_104/iec104_tls.cpp
    adapters/io/drv_gpio.c
)
add_library(ccli_adapters STATIC ${CCLI_ADAPTERS_SOURCES})
if(CCLI_HAVE_LIBIEC61850)
    target_include_directories(ccli_adapters BEFORE PRIVATE
        "${CCLI_LIBIEC61850_TLS_OVERLAY_INCLUDE_DIR}")
endif()
target_include_directories(ccli_adapters PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/modbus_master
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec61850_mms
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec61850_goose
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec60870_104
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/io
)

add_library(ccli_services STATIC
    services/svc_pf2.cpp
    services/svc_modbus.cpp
    services/svc_mms.cpp
    services/svc_goose.cpp
    services/svc_io.c
)
target_link_libraries(ccli_adapters PUBLIC ccli_core)
ccli_setup_libmodbus(ccli_adapters)
if(CCLI_HAVE_LIBIEC61850)
    target_compile_definitions(ccli_adapters PRIVATE CCLI_HAVE_LIBIEC61850=1)
    if(CCLI_WITH_GOOSE)
        target_compile_definitions(ccli_adapters PRIVATE CCLI_HAVE_GOOSE=1)
    endif()
    # Binary dir name follows source path under apps/ccli (third_party/libiec61850).
    set(_iec_bin "${CMAKE_BINARY_DIR}/third_party/libiec61850")
    if(NOT EXISTS "${_iec_bin}/config/stack_config.h")
        set(_iec_bin "${CMAKE_BINARY_DIR}/libiec61850")
    endif()
    if(EXISTS "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0" OR
       EXISTS "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-2.28")
        target_compile_definitions(ccli_adapters PRIVATE CONFIG_MMS_SUPPORT_TLS=1)
        message(STATUS "CCLI MMS: TLS (62351-3) enabled via mbedtls")
        if(EXISTS "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0")
            set(_ccli_mbedtls_inc "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0/include")
            set(_ccli_mbedtls_config_inc "${CMAKE_CURRENT_SOURCE_DIR}/adapters/security")
            include(cmake/patch_mbedtls_3_6_oid_dn_value.cmake)
            ccli_patch_mbedtls_3_6_oid_dn_value(
                "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0/library/x509.c")
            set(_crl_patch "${CMAKE_CURRENT_SOURCE_DIR}/cmake/patch_mbedtls_3_6_crl_scope.py")
            execute_process(COMMAND "${Python3_EXECUTABLE}" "${_crl_patch}"
                "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0/library/x509_crl.c"
                RESULT_VARIABLE _crl_patch_result ERROR_VARIABLE _crl_patch_error)
            if(NOT _crl_patch_result EQUAL 0)
                message(FATAL_ERROR "Native E2E CRL parser patch: ${_crl_patch_error}")
            endif()
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_crl_patch}")
            target_compile_definitions(ccli_adapters PRIVATE CCLI_MBEDTLS_SCOPED_CRL=1)
        else()
            set(_ccli_mbedtls_inc "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-2.28/include")
            set(_ccli_mbedtls_config_inc "${CCLI_LIBIEC61850_DIR}/hal/tls/mbedtls")
        endif()
        target_include_directories(ccli_adapters PRIVATE
            "${_ccli_mbedtls_inc}"
            "${_ccli_mbedtls_config_inc}")
        target_compile_definitions(ccli_adapters PRIVATE
            MBEDTLS_CONFIG_FILE=\"mbedtls_config.h\")
    endif()
    target_include_directories(ccli_adapters PRIVATE
        "${CCLI_LIBIEC61850_DIR}/src/iec61850/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc_private"
        "${CCLI_LIBIEC61850_DIR}/src/logging"
        "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${CCLI_LIBIEC61850_DIR}/src/goose"
        # goose_receiver.h includes r_session.h (R-GOOSE session layer) unconditionally.
        "${CCLI_LIBIEC61850_DIR}/src/r_session"
        "${_iec_bin}/config"
    )
    # Prefer static link so OpenWrt apk needs no extra iec61850 package.
    target_link_libraries(ccli_adapters PUBLIC iec61850)
endif()

if(CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(ccli_core PRIVATE CCLI_HAVE_SYSLOG_TLS=1 MBEDTLS_CONFIG_FILE="mbedtls_config.h")
    target_include_directories(ccli_core PRIVATE
        "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${CMAKE_CURRENT_SOURCE_DIR}/adapters/security"
        "${_ccli_mbedtls_inc}" "${_ccli_mbedtls_config_inc}")
    # The vendor HAL uses LinkedList from iec61850, while iec61850 uses HAL.
    # Group the static archives for core-only logger/CLI consumers as well.
    target_link_libraries(ccli_core PUBLIC -Wl,--start-group iec61850 hal -Wl,--end-group)
endif()

if(CCLI_HAVE_LIB60870)
    target_compile_definitions(ccli_adapters PRIVATE CCLI_HAVE_LIB60870=1)
    set(_60870_bin "${CMAKE_BINARY_DIR}/third_party/lib60870/lib60870-C")
    if(NOT EXISTS "${_60870_bin}/config")
        set(_60870_bin "${CMAKE_BINARY_DIR}/lib60870-C")
    endif()
    target_include_directories(ccli_adapters PRIVATE
        "${CCLI_LIB60870_DIR}/src/inc/api"
        "${CCLI_LIB60870_DIR}/src/inc/internal"
        "${CCLI_LIB60870_DIR}/src/common/inc"
        "${CCLI_LIB60870_DIR}/src/hal/inc"
        "${CCLI_LIB60870_DIR}/config"
    )
    target_link_libraries(ccli_adapters PUBLIC lib60870)
    if(EXISTS "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-3.6.0" OR
       EXISTS "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-2.28")
        target_compile_definitions(lib60870 PRIVATE CONFIG_CS104_SUPPORT_TLS=1)
        target_compile_definitions(ccli_adapters PRIVATE CONFIG_CS104_SUPPORT_TLS=1)
        message(STATUS "CCLI 104: TLS (62351-3) enabled via shared mbedtls / iec61850 HAL")
    endif()
endif()

target_link_libraries(ccli_services PUBLIC ccli_adapters ccli_platform)
target_include_directories(ccli_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/services)

add_library(ccli_platform STATIC ${CCLI_PLATFORM_SOURCES})
target_include_directories(ccli_platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
    ${CCLI_PLATFORM_INCLUDE_DIRS}
)
target_link_libraries(ccli_platform PUBLIC ccli_core)

# drv_gpio.c (in ccli_adapters) calls the low-level GPIO HAL (in ccli_platform).
target_link_libraries(ccli_adapters PUBLIC ccli_platform)

# SK0153 product program (merged from product/apps/ccli) — default entry for cert builds.
add_library(ccli_program STATIC
    program/alarm.c
    program/alarm_input.c
    program/boot.c
    program/commands.c
    program/drvgpio.c
    program/f_pf2.c
    program/fw_config.c
    program/outputs.c
    program/parser_modbus.c
    program/program_flash.c
    program/queue_my.c
    program/regulation.c
    program/startup.c
    program/synchronize.c
    program/transmission_104.c
    program/transmission_goose.c
    program/transmission_mms.c
    program/ccli_bridge.cpp
    program/ccli_tools.cpp
    services/svc_io.c
)
target_include_directories(ccli_program PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/program
    ${CMAKE_CURRENT_SOURCE_DIR}/program/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/io
    ${CMAKE_CURRENT_SOURCE_DIR}/services
)
target_link_libraries(ccli_program PUBLIC ccli_services ccli_platform)

option(CCLI_LEGACY_MAIN "Build validated personal services/ccli_main.cpp entry" ON)
if(CCLI_LEGACY_MAIN)
    add_executable(ccli services/ccli_main.cpp)
    target_link_libraries(ccli PRIVATE ccli_services ccli_platform)
    message(STATUS "CCLI entry: legacy ccli_main.cpp")
else()
    add_executable(ccli program/main.c)
    set_target_properties(ccli PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(ccli PRIVATE ccli_program ccli_services ccli_platform)
    message(STATUS "CCLI entry: program/main.c (product + frozen cyber)")
endif()
if(CCLI_HAVE_LIB60870)
    target_link_libraries(ccli PRIVATE lib60870)
endif()

# Deliberately separate process: detects a whole-process CCLI freeze and lets
# procd respawn it. It does not own /dev/watchdog; that needs TG544 driver and
# safe-output validation before production enablement.
add_executable(ccli-watchdog watchdog/ccli_watchdog.c)
if(BUILD_PLATFORM STREQUAL "tg500")
    target_link_options(ccli PRIVATE -static-libstdc++)
endif()

option(CCLI_BUILD_TESTS "Build unit tests" ON)
option(CCLI_BUILD_MMS_DEVICE_TEST "Build isolated on-device MMS adapter test harness (no plant I/O)" OFF)
option(CCLI_BUILD_TLS_DEVICE_TEST "Build isolated TLS device echo probe (disposable test credentials, no plant I/O)" OFF)
if(CCLI_BUILD_TLS_DEVICE_TEST)
    if(NOT CCLI_HAVE_LIBIEC61850 OR NOT CCLI_HAVE_LIB60870 OR NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(FATAL_ERROR "The TLS device probe requires both protocol libraries and Linux")
    endif()
    add_executable(iec62351-tls-device-probe tests/integration/iec62351_tls_probe.cpp
        "${CCLI_LIBIEC61850_DIR}/src/common/linked_list.c")
    target_link_libraries(iec62351-tls-device-probe PRIVATE ccli_adapters)
    target_include_directories(iec62351-tls-device-probe PRIVATE
        "${CCLI_LIBIEC61850_DIR}/hal/inc" "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc" "${_iec_bin}/config")
    target_compile_definitions(iec62351-tls-device-probe PRIVATE CCLI_TLS_TEST_CLOCK=1 CCLI_TLS_DEVICE_TEST=1)
    target_link_options(iec62351-tls-device-probe PRIVATE "-Wl,--wrap=clock_gettime" -static-libstdc++)
endif()
if(CCLI_BUILD_MMS_DEVICE_TEST)
    add_executable(mms-rbac-device-server tests/integration/mms_rbac_device_server.cpp)
    target_link_libraries(mms-rbac-device-server PRIVATE ccli_adapters)
    if(BUILD_PLATFORM STREQUAL "tg500")
        target_link_options(mms-rbac-device-server PRIVATE -static-libstdc++)
    endif()
endif()
if(CCLI_BUILD_TESTS AND UNIX)
    add_executable(mms-rbac-dual-host-server tests/integration/mms_rbac_device_server.cpp)
    target_compile_definitions(mms-rbac-dual-host-server PRIVATE CCLI_HOST_DUAL_TEST=1)
    target_link_libraries(mms-rbac-dual-host-server PRIVATE ccli_adapters)
endif()
if(CCLI_BUILD_TESTS AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc)
    # Explicit lab probe; not an automatic test requiring private PKI material.
    add_executable(mms-rbac-dual-probe tests/integration/mms_rbac_dual_probe.cpp)
    target_compile_definitions(mms-rbac-dual-probe PRIVATE CONFIG_MMS_SUPPORT_TLS=1)
    target_include_directories(mms-rbac-dual-probe PRIVATE
        "${CCLI_LIBIEC61850_DIR}/src/iec61850/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc_private"
        "${CCLI_LIBIEC61850_DIR}/src/logging"
        "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${_iec_bin}/config")
    target_link_libraries(mms-rbac-dual-probe PRIVATE iec61850 pthread m rt)
    target_link_options(mms-rbac-dual-probe PRIVATE
        -Wl,--wrap=AcseConnection_parseMessage
        -Wl,--wrap=AcseConnection_createAssociateRequestMessage
        -Wl,--wrap=IsoSession_createConnectSpdu
        -Wl,--wrap=IsoSession_parseMessage)
endif()
if(CCLI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/unit OPTIONAL)
    if(CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_executable(syslog-tls-probe tests/integration/syslog_tls_probe.cpp)
        target_link_libraries(syslog-tls-probe PRIVATE ccli_core)
        find_program(CCLI_OPENSSL openssl)
        if(CCLI_OPENSSL)
            add_test(NAME iec62351_syslog_tls COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/syslog_tls_test.py" $<TARGET_FILE:syslog-tls-probe>)
            set_tests_properties(iec62351_syslog_tls PROPERTIES TIMEOUT 100)
        endif()
    endif()
endif()

install(TARGETS ccli ccli-watchdog RUNTIME DESTINATION bin)
if(CCLI_BUILD_MMS_CLIENT AND CCLI_HAVE_LIBIEC61850 AND DEFINED _ccli_mbedtls_inc)
    add_executable(ccli-mms-client tools/mms-client/client.cpp)
    target_compile_definitions(ccli-mms-client PRIVATE CONFIG_MMS_SUPPORT_TLS=1)
    if(CCLI_MMS_TSP_CLIENT_PROFILE)
        target_compile_definitions(ccli-mms-client PRIVATE CCLI_MMS_TSP_CLIENT_PROFILE=1)
    endif()
    target_include_directories(ccli-mms-client PRIVATE
        "${CCLI_LIBIEC61850_DIR}/src/iec61850/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc_private"
        "${CCLI_LIBIEC61850_DIR}/src/logging"
        "${CCLI_LIBIEC61850_DIR}/hal/inc" "${_iec_bin}/config")
    target_link_libraries(ccli-mms-client PRIVATE iec61850 pthread m rt)
endif()
install(DIRECTORY config/ DESTINATION share/ccli/config)
