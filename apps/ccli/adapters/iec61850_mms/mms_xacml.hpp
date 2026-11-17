#pragma once

#include "mms_rbac.hpp"

namespace cci::adapters {

struct MmsCompiledGrant {
    MmsProfileARole role;
    MmsPermission permission;
    std::vector<std::string> objects;
};

// Compile the IEC role PolicySet and its local permission library, then
// intersect role semantics with separately commissioned AoR/object scopes.
bool compile_iec_xacml(const std::string& roles, const std::string& library,
                       const std::string& scopes, std::vector<MmsCompiledGrant>& grants,
                       std::string& policy_id, std::string& revision, std::string& error);

} // namespace cci::adapters
