# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#

set(target ql_app_osi)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_app_lib_dir})
include_directories(${SOURCE_TOP_DIR}/components/newlib/include)
target_link_libraries(${target} PRIVATE ${libc_file_name} ${libm_file_name} ${libgcc_file_name})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_QUEC)
target_include_directories(${target} PUBLIC inc)
target_include_targets(${target} PRIVATE atr driver)
#target_link_libraries(${target} PRIVATE kernel driver ql_api_common)

target_sources(${target} PRIVATE
    adc.c
    alarm_input.c
    alarm_power.c
    alarm_tmax.c
    alarm_tmin.c
    alarm.c
    analog.c
    at_debug.c
    boot_flash.c
    boot.c
    calendar.c
    charger.c
    clock.c
    commands_ext.c
    commands.c
    counters.c
    credit.c
    debug_my.c
    default.c
    device_id.c
    dota_drvgpio.c
    dota_flash.c
    dota_ftp.c
    dota_gprs.c
    dota_main.c
    drvbattery.c
    drvgpio.c
    drvtemperature.c
    energy.c
    f_antifrost.c
    f_chrono.c
    f_regulation.c
    file_csv.c
    file_system.c
    forward.c
    fv_action.c
    fw_config.c
    fwd.c
    http_my.c
    id.c
    input_event.c
    language.c
    led.c
    log_status.c
    logs.c
    main.c
    messages.c
    mode.c
    myn.c
    names.c
    out_key.c
    outputs.c
    parser_rx.c
    parser_tx.c
    password.c
    phone.c
    phonebook.c
    pod.c
    program_flash.c
    queue_my.c
    regulation.c
    report.c
    reset.c
    rtc_alarm.c
    sms_answer_event.c
    smtp.c
    startup.c
    status.c
    strings_eng.c
    strings_fre.c
    strings_ger.c
    strings_ita.c
    strings_my.c
    strings_pol.c
    strings_spa.c
    strings_swe.c
    synchronize.c
    temperature.c
    test.c
    transmission_gprs.c
    transmission_sms.c
    ui_led.c
    user_gsm_retx.c
    user_gsm_sync.c
    utility.c
    variables.c
)

relative_glob(srcs include/*.h src/*.c inc/*.h)
#beautify_c_code(${target} ${srcs})
