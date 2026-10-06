cmake_minimum_required(VERSION 3.16)

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

add_executable(mms_acse_auth_test mms_acse_auth_test.cpp)
target_link_libraries(mms_acse_auth_test PRIVATE ccli_adapters)
add_test(NAME mms_acse_p307_acse_auth COMMAND mms_acse_auth_test)
