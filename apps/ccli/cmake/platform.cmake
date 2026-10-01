if(BUILD_PLATFORM STREQUAL "tg500")
    set(CCLI_PLATFORM_SOURCES
        platform/platform_tg500/hal_stub.cpp
        platform/platform_tg500/hal_gpio_ll.c
        platform/platform_pi/hal_time.cpp
        platform/platform_tg500/platform_init.cpp
    )
    set(CCLI_PLATFORM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/platform/platform_tg500)
    add_compile_definitions(CCLI_PLATFORM_TG500=1)
else()
    message(FATAL_ERROR "Unknown BUILD_PLATFORM: ${BUILD_PLATFORM}. Use tg500 (TesPro MT798X).")
endif()

message(STATUS "CCLI BUILD_PLATFORM=${BUILD_PLATFORM}")

# --- libgpiod (platform_pi GPIO HAL) -----------------------------------------
function(ccli_setup_libgpiod target)
    option(CCLI_FORCE_LIBGPIOD "Link libgpiod even if find_library fails (OpenWrt SDK)" OFF)

    if(CCLI_GPIOD_INCLUDE_DIR)
        target_include_directories(${target} PRIVATE ${CCLI_GPIOD_INCLUDE_DIR})
    endif()
    if(CCLI_GPIOD_LINK_DIR)
        target_link_directories(${target} PUBLIC ${CCLI_GPIOD_LINK_DIR})
    endif()

    if(CCLI_GPIOD_LIBRARY)
        # Explicit path from OpenWrt SDK staging (preferred for cross-build).
    else()
        find_library(CCLI_GPIOD_LIBRARY NAMES gpiod
            PATHS ${CCLI_GPIOD_LINK_DIR} NO_DEFAULT_PATH)
        if(NOT CCLI_GPIOD_LIBRARY AND CCLI_GPIOD_LINK_DIR)
            find_library(CCLI_GPIOD_LIBRARY NAMES gpiod PATHS ${CCLI_GPIOD_LINK_DIR})
        endif()
    endif()

    if(CCLI_GPIOD_LIBRARY OR CCLI_FORCE_LIBGPIOD)
        target_compile_definitions(${target} PRIVATE CCLI_GPIO_LL_LIBGPIOD=1)
        if(CCLI_GPIOD_LIBRARY)
            target_link_libraries(${target} PUBLIC ${CCLI_GPIOD_LIBRARY})
            message(STATUS "CCLI GPIO backend: libgpiod (${CCLI_GPIOD_LIBRARY})")
        else()
            target_link_libraries(${target} PUBLIC gpiod)
            message(STATUS "CCLI GPIO backend: libgpiod (forced link)")
        endif()
    else()
        message(STATUS "CCLI GPIO backend: in-memory mock (libgpiod not found)")
    endif()
endfunction()

# --- libmodbus (Modbus master adapter) ---------------------------------------
function(ccli_setup_libmodbus target)
    option(CCLI_FORCE_LIBMODBUS "Link libmodbus even if find_library fails (OpenWrt SDK)" OFF)

    if(CCLI_MODBUS_INCLUDE_DIR)
        target_include_directories(${target} PRIVATE ${CCLI_MODBUS_INCLUDE_DIR})
    endif()
    if(CCLI_MODBUS_LINK_DIR)
        target_link_directories(${target} PUBLIC ${CCLI_MODBUS_LINK_DIR})
    endif()

    if(CCLI_MODBUS_LIBRARY)
        # Explicit path from OpenWrt SDK staging (preferred for cross-build).
    else()
        find_library(CCLI_MODBUS_LIBRARY NAMES modbus
            PATHS ${CCLI_MODBUS_LINK_DIR} NO_DEFAULT_PATH)
        if(NOT CCLI_MODBUS_LIBRARY AND CCLI_MODBUS_LINK_DIR)
            find_library(CCLI_MODBUS_LIBRARY NAMES modbus PATHS ${CCLI_MODBUS_LINK_DIR})
        endif()
    endif()

    if(CCLI_MODBUS_LIBRARY OR CCLI_FORCE_LIBMODBUS)
        target_compile_definitions(${target} PRIVATE CCLI_MODBUS_LIBMODBUS=1)
        if(CCLI_MODBUS_LIBRARY)
            target_link_libraries(${target} PUBLIC ${CCLI_MODBUS_LIBRARY})
            message(STATUS "CCLI Modbus backend: libmodbus (${CCLI_MODBUS_LIBRARY})")
        else()
            target_link_libraries(${target} PUBLIC modbus)
            message(STATUS "CCLI Modbus backend: libmodbus (forced link)")
        endif()
    else()
        message(STATUS "CCLI Modbus backend: simulator only (libmodbus not found)")
    endif()
endfunction()
