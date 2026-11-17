#pragma once
#include "event/event_ring.hpp"

namespace cci::core {
struct IecEventDefinition {
    const char* id;
    const char* mnemonic;
    const char* severity;
    const char* text;
};
const IecEventDefinition* iec_event_definition(const std::string& id);
/** Classify existing CCI audit stimuli or resolve an explicitly supplied IEC ID.
 * No invented IEC IDs are assigned to unrelated O.14 process records. */
bool prepare_iec_event(EventRecord& event, const std::string& source,
                       const std::string& app_name);
bool valid_iec_event(const EventRecord& event);
std::string format_rfc5424(const EventRecord& event, const std::string& hostname,
                           const std::string& app_name);
/** RFC 5425 octet counting (length excludes the length prefix and space). */
std::string frame_rfc5425(const std::string& message);
} // namespace cci::core
