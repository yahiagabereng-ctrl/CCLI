cmake_minimum_required(VERSION 3.16)
project(p4-cs104-client C)

set(LIB60870_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../apps/ccli/third_party/lib60870/lib60870-C")

add_subdirectory("${LIB60870_ROOT}" lib60870-build EXCLUDE_FROM_ALL)

include_directories(
    "${LIB60870_ROOT}/config"
    "${LIB60870_ROOT}/src/inc/api"
    "${LIB60870_ROOT}/src/inc/internal"
    "${LIB60870_ROOT}/src/common/inc"
    "${LIB60870_ROOT}/src/hal/inc"
)

add_executable(p4_cs104_client p4_cs104_client.c)
target_link_libraries(p4_cs104_client PRIVATE lib60870)

if(EXISTS "${LIB60870_ROOT}/dependencies/mbedtls-3.6" OR
   EXISTS "${LIB60870_ROOT}/dependencies/mbedtls-2.28")
  target_compile_definitions(p4_cs104_client PRIVATE CONFIG_CS104_SUPPORT_TLS=1)
  if(EXISTS "${LIB60870_ROOT}/dependencies/mbedtls-3.6")
    target_include_directories(p4_cs104_client PRIVATE
      "${LIB60870_ROOT}/src/hal/tls/mbedtls3"
      "${LIB60870_ROOT}/dependencies/mbedtls-3.6/include")
  else()
    target_include_directories(p4_cs104_client PRIVATE
      "${LIB60870_ROOT}/src/hal/tls/mbedtls"
      "${LIB60870_ROOT}/dependencies/mbedtls-2.28/include")
  endif()
endif()
