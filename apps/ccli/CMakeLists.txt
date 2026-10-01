cmake_minimum_required(VERSION 3.16)
project(ccli LANGUAGES CXX C VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_PLATFORM "tg500" CACHE STRING "Target platform: tg500 (TesPro MT798X)")
set_property(CACHE BUILD_PLATFORM PROPERTY STRINGS tg500)

include(cmake/platform.cmake)
include(cmake/ccli_version.cmake)
ccli_generate_version_header()

# --- libiec61850 (Phase 3 MMS) ------------------------------------------------
option(CCLI_WITH_LIBIEC61850 "Build IEC 61850 MMS with third_party/libiec61850" ON)
set(CCLI_LIBIEC61850_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libiec61850")
set(CCLI_HAVE_LIBIEC61850 0)
if(CCLI_WITH_LIBIEC61850 AND EXISTS "${CCLI_LIBIEC61850_DIR}/CMakeLists.txt")
    set(BUILD_EXAMPLES OFF CACHE BOOL "libiec61850 examples" FORCE)
    set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "libiec61850 python" FORCE)
    set(CONFIG_INCLUDE_GOOSE_SUPPORT OFF CACHE BOOL "libiec61850 GOOSE" FORCE)
    set(CONFIG_IEC61850_L2_GOOSE OFF CACHE BOOL "libiec61850 L2 GOOSE" FORCE)
    set(CONFIG_IEC61850_L2_SMV OFF CACHE BOOL "libiec61850 L2 SMV" FORCE)
    set(CONFIG_IEC61850_R_GOOSE OFF CACHE BOOL "libiec61850 R-GOOSE" FORCE)
    set(CONFIG_IEC61850_R_SMV OFF CACHE BOOL "libiec61850 R-SMV" FORCE)
    add_subdirectory("${CCLI_LIBIEC61850_DIR}" EXCLUDE_FROM_ALL)
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
    # libiec61850 HAL lacks Hal_getMonotonicTimeInMs; embed supplement in lib60870.
    target_sources(lib60870 PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec60870_104/hal60870_monotonic.c")
    set(CCLI_HAVE_LIB60870 1)
    message(STATUS "CCLI 104: lib60870 enabled (${CCLI_LIB60870_DIR})")
else()
    message(STATUS "CCLI 104: stub (third_party/lib60870 missing or disabled)")
endif()

add_library(ccli_core STATIC
    core/pf2/pf2_fsm.cpp
    core/dso/dso_active_power.cpp
    core/dso/dso_reactive_power.cpp
    core/dso/dso_phase1.cpp
    core/measurement/measurement_store.cpp
    core/event/event_ring.cpp
    core/policy/net_policy.cpp
    core/config/ccli_config.cpp
    core/regulation/regulation_bench.cpp
    core/version/ccli_version.cpp
)
target_include_directories(ccli_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
    ${CMAKE_BINARY_DIR}/generated
)

set(CCLI_ADAPTERS_SOURCES
    adapters/modbus_master/modbus_adapter.cpp
    adapters/iec61850_mms/mms_adapter.cpp
    adapters/iec61850_mms/mms_acse_auth.cpp
    adapters/iec61850_mms/mms_aare_auth.cpp
    adapters/iec61850_mms/gnss_time.cpp
    adapters/iec60870_104/iec104_adapter.cpp
    adapters/iec60870_104/iec104_tls.cpp
    adapters/io/drv_gpio.c
)
add_library(ccli_adapters STATIC ${CCLI_ADAPTERS_SOURCES})
target_include_directories(ccli_adapters PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/modbus_master
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec61850_mms
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/iec60870_104
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/io
)

add_library(ccli_services STATIC
    services/svc_pf2.cpp
    services/svc_modbus.cpp
    services/svc_mms.cpp
    services/svc_io.c
)
target_link_libraries(ccli_adapters PUBLIC ccli_core)
ccli_setup_libmodbus(ccli_adapters)
if(CCLI_HAVE_LIBIEC61850)
    target_compile_definitions(ccli_adapters PRIVATE CCLI_HAVE_LIBIEC61850=1)
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
        else()
            set(_ccli_mbedtls_inc "${CCLI_LIBIEC61850_DIR}/third_party/mbedtls/mbedtls-2.28/include")
        endif()
        target_include_directories(ccli_adapters PRIVATE "${_ccli_mbedtls_inc}")
    endif()
    target_include_directories(ccli_adapters PRIVATE
        "${CCLI_LIBIEC61850_DIR}/src/iec61850/inc"
        "${CCLI_LIBIEC61850_DIR}/src/common/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc"
        "${CCLI_LIBIEC61850_DIR}/src/mms/inc_private"
        "${CCLI_LIBIEC61850_DIR}/src/logging"
        "${CCLI_LIBIEC61850_DIR}/hal/inc"
        "${_iec_bin}/config"
    )
    # Prefer static link so OpenWrt apk needs no extra iec61850 package.
    target_link_libraries(ccli_adapters PUBLIC iec61850)
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

add_executable(ccli services/ccli_main.cpp)
target_link_libraries(ccli PRIVATE ccli_services ccli_platform)
if(CCLI_HAVE_LIB60870)
    target_link_libraries(ccli PRIVATE lib60870)
endif()
if(BUILD_PLATFORM STREQUAL "tg500")
    # TesproOS image may ship without libstdc++.so.6; static link for lab deploy.
    target_link_options(ccli PRIVATE -static-libstdc++)
endif()

option(CCLI_BUILD_TESTS "Build unit tests" ON)
if(CCLI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/unit OPTIONAL)
endif()

install(TARGETS ccli RUNTIME DESTINATION bin)
install(DIRECTORY config/ DESTINATION share/ccli/config)
