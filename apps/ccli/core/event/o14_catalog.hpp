#pragma once

#include <string>

namespace cci::core::o14 {

/** Annex O.14 mandatory category ids (software logger). */
inline constexpr const char* kPsu = "o14.psu";
inline constexpr const char* kPower = "o14.power";
inline constexpr const char* kFirmware = "o14.firmware";
inline constexpr const char* kSwitchDi = "o14.switch_di";
inline constexpr const char* kCommsDso = "o14.comms_dso";
inline constexpr const char* kCommsOperator = "o14.comms_operator";
inline constexpr const char* kCommsPlant = "o14.comms_plant";
inline constexpr const char* kCommsGoose = "o14.comms_goose";
inline constexpr const char* kFunction = "o14.function";
inline constexpr const char* kMeter = "o14.meter";
inline constexpr const char* kNet = "o14.net";
inline constexpr const char* kAuth = "o14.auth";
inline constexpr const char* kControl = "o14.control";
inline constexpr const char* kDsoCommand = "o14.dso_command";
inline constexpr const char* kTripAnnexM = "o14.trip_annex_m";
inline constexpr const char* kPriority = "o14.priority";
inline constexpr const char* kPolygon = "o14.polygon";
inline constexpr const char* kSecurity = "o14.security";
inline constexpr const char* kLogger = "o14.logger";

inline const char* category_for(const std::string& type, const std::string& detail) {
    if (type == "system") {
        if (detail.find("psu") != std::string::npos) {
            return kPsu;
        }
        if (detail.find("firmware") != std::string::npos) {
            return kFirmware;
        }
        if (detail.find("power_") != std::string::npos) {
            return kPower;
        }
        if (detail.find("log_wrap") != std::string::npos) {
            return kLogger;
        }
        return kPower;
    }
    if (type == "di" || type == "do") {
        return kSwitchDi;
    }
    if (type == "mms") {
        if (detail.find("auth_") != std::string::npos ||
            detail.find("rbac_") != std::string::npos) {
            return kAuth;
        }
        if (detail.find("wlim") != std::string::npos ||
            detail.find("wsd") != std::string::npos ||
            detail.find("varsd") != std::string::npos ||
            detail.find("pfsp") != std::string::npos) {
            return kDsoCommand;
        }
        if (detail.find("client_") != std::string::npos ||
            detail.find("comms_loss") != std::string::npos) {
            return kCommsDso;
        }
        return kCommsDso;
    }
    if (type == "iec104") {
        if (detail.find("operator_asdu") != std::string::npos) {
            return kDsoCommand;
        }
        return kCommsOperator;
    }
    if (type == "modbus") {
        return kCommsPlant;
    }
    if (type == "goose") {
        return kCommsGoose;
    }
    if (type == "pf2") {
        return kFunction;
    }
    if (type == "meter") {
        return kMeter;
    }
    if (type == "net" || type == "iface") {
        return kNet;
    }
    if (type == "annex_m") {
        return kTripAnnexM;
    }
    if (type == "priority") {
        return kPriority;
    }
    if (type == "dso") {
        return kPolygon;
    }
    if (type == "security") {
        return kSecurity;
    }
    if (type == "wrap_test") {
        return kLogger;
    }
    return kLogger;
}

}  // namespace cci::core::o14
