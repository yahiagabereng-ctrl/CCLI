# Generate ccli_version_gen.h from apps/ccli/VERSION + git + build metadata.

function(ccli_generate_version_header)
    set(_version_file "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    if(NOT EXISTS "${_version_file}")
        message(FATAL_ERROR "Missing ${_version_file}")
    endif()

    file(READ "${_version_file}" _version_raw)
    string(REPLACE "\r\n" "\n" _version_raw "${_version_raw}")
    string(REPLACE "\r" "\n" _version_raw "${_version_raw}")
    string(REPLACE "\n" ";" _version_lines "${_version_raw}")

    list(LENGTH _version_lines _line_count)
    if(_line_count LESS 2)
        message(FATAL_ERROR "VERSION file must have semver + release lines")
    endif()

    list(GET _version_lines 0 CCLI_SEMVER)
    list(GET _version_lines 1 CCLI_RELEASE_STR)
    string(STRIP "${CCLI_SEMVER}" CCLI_SEMVER)
    string(STRIP "${CCLI_RELEASE_STR}" CCLI_RELEASE_STR)

    if(NOT CCLI_RELEASE_STR MATCHES "^[0-9]+$")
        message(FATAL_ERROR "VERSION release line must be integer, got '${CCLI_RELEASE_STR}'")
    endif()

    set(CCLI_CODENAME "")
    if(_line_count GREATER 2)
        list(SUBLIST _version_lines 2 -1 _codename_lines)
        list(JOIN _codename_lines " " CCLI_CODENAME)
        string(STRIP "${CCLI_CODENAME}" CCLI_CODENAME)
    endif()

    set(CCLI_GIT_SHA "unknown")
    find_program(GIT_EXECUTABLE git)
    if(GIT_EXECUTABLE)
        set(_git_dirs
            "${CMAKE_CURRENT_SOURCE_DIR}/../.."
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
            "${CMAKE_SOURCE_DIR}"
        )
        foreach(_git_dir IN LISTS _git_dirs)
            if(EXISTS "${_git_dir}/.git")
                execute_process(
                    COMMAND "${GIT_EXECUTABLE}" -C "${_git_dir}" rev-parse --short HEAD
                    OUTPUT_VARIABLE _git_out
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                )
                if(_git_out)
                    set(CCLI_GIT_SHA "${_git_out}")
                    break()
                endif()
            endif()
        endforeach()
    endif()

    string(TIMESTAMP CCLI_BUILD_TIMESTAMP UTC)

    set(CCLI_VERSION_FULL "${CCLI_SEMVER}-r${CCLI_RELEASE_STR}")
    set(_gen_dir "${CMAKE_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY "${_gen_dir}")

    set(_gen_h "${_gen_dir}/ccli_version_gen.h")
    file(WRITE "${_gen_h}" "/* Auto-generated — do not edit. */\n")
    file(APPEND "${_gen_h}" "#pragma once\n\n")
    file(APPEND "${_gen_h}" "#define CCLI_VERSION_STRING \"${CCLI_SEMVER}\"\n")
    file(APPEND "${_gen_h}" "#define CCLI_RELEASE ${CCLI_RELEASE_STR}\n")
    file(APPEND "${_gen_h}" "#define CCLI_VERSION_FULL \"${CCLI_VERSION_FULL}\"\n")
    file(APPEND "${_gen_h}" "#define CCLI_VERSION_CODENAME \"${CCLI_CODENAME}\"\n")
    file(APPEND "${_gen_h}" "#define CCLI_GIT_SHA \"${CCLI_GIT_SHA}\"\n")
    file(APPEND "${_gen_h}" "#define CCLI_BUILD_TIMESTAMP \"${CCLI_BUILD_TIMESTAMP}\"\n")
    file(APPEND "${_gen_h}" "#define CCLI_BUILD_PLATFORM \"${BUILD_PLATFORM}\"\n")

    set(CCLI_VERSION_GEN_H "${_gen_h}" PARENT_SCOPE)
    set(CCLI_VERSION_SEMVER "${CCLI_SEMVER}" PARENT_SCOPE)
    set(CCLI_VERSION_RELEASE "${CCLI_RELEASE_STR}" PARENT_SCOPE)
    set(CCLI_VERSION_FULL "${CCLI_VERSION_FULL}" PARENT_SCOPE)

    message(STATUS "CCLI version ${CCLI_VERSION_FULL} (${CCLI_GIT_SHA}) codename=${CCLI_CODENAME}")
endfunction()
