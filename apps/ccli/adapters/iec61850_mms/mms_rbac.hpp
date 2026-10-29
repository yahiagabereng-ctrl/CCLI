#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cci::adapters {

/** IEC 62351-8 Table 3 and CEI 0-16 Annex T private role identifiers. */
enum class MmsRole : int {
    DsoOperator = -1,
    AggregatorOperator = -2,
    Viewer = 0,
    Operator = 1,
    Engineer = 2,
    Installer = 3,
    SecAdm = 4,
    SecAud = 5,
    RbacMnt = 6,
    Unknown = -99,
};

enum class MmsPermission : std::uint8_t {
    ListObjects,
    ReadValues,
    DataSet,
    Reporting,
    Control,
    Config,
};

struct MmsProfileARole {
    int role_id{static_cast<int>(MmsRole::Unknown)};
    std::string aor;
    std::uint8_t revision{0};
    std::string role_definition;
    int operation{1}; // Add=1, Delete=2, Change=3: administrative token operations
    bool has_operation{false};
    bool has_sequence{false};
    std::uint32_t status_sequence{0};
};

enum class MmsProfileAParseResult : std::uint8_t {
    Ok,
    Empty,
    Malformed,
    MissingExtension,
};

/** Decode the DER payload of X.509 extension OID 1.0.62351.8.1. */
MmsProfileAParseResult decode_iec_user_roles_extension(const std::uint8_t* der,
                                                        std::size_t length,
                                                        std::vector<MmsProfileARole>& roles);

/** Locate and decode the IECUserRoles extension in a DER X.509 certificate. */
MmsProfileAParseResult decode_profile_a_certificate(const std::uint8_t* der,
                                                     std::size_t length,
                                                     std::vector<MmsProfileARole>& roles);

const char* mms_role_name(MmsRole role);
const char* mms_permission_name(MmsPermission permission);

/**
 * IEC 62351-8 XACML Role/PermissionGroup/Permission PolicySet compiler.
 * AoR and plant object allocation are separate commissioning configuration.
 * Unsupported constructs and unresolved/cyclic references fail closed.
 */
class MmsCommissioningPolicy {
public:
    bool load_file(const std::string& path, std::string& error);
    bool load_xml(const std::string& roles_xml, const std::string& library_xml,
                  const std::string& scopes_xml, std::string& error);

    bool empty() const { return rules_.empty(); }
    const std::string& policy_id() const { return policy_id_; }
    const std::string& revision() const { return revision_; }

    bool accepts_role(const MmsProfileARole& role) const;
    bool permits_any(const MmsProfileARole& role, MmsPermission permission) const;
    bool permits(const MmsProfileARole& role, MmsPermission permission,
                 const std::string& object_reference) const;
    bool permits_roles(const std::vector<MmsProfileARole>& roles, MmsPermission permission,
                       const std::string& object_reference) const;
    /** CONTROL, or a private C3 CONFIG grant for this exact Oper.ctlVal payload.
     * Standard-role CONFIG never implicitly grants control operations. */
    bool permits_control_roles(const std::vector<MmsProfileARole>& roles,
                               const std::string& object_reference) const;
    bool permits_dataset(const MmsProfileARole& role, bool read_only,
                         const std::string& object_reference) const;
    bool permits_dataset_roles(const std::vector<MmsProfileARole>& roles, bool read_only,
                               const std::string& object_reference) const;
    bool permits_report(const MmsProfileARole& role, bool write,
                        const std::string& member, const std::string& object_reference) const;
    bool permits_report_roles(const std::vector<MmsProfileARole>& roles, bool write,
                              const std::string& member,
                              const std::string& object_reference) const;

private:
    struct Rule {
        int role_id{0};
        std::string aor;
        std::uint8_t token_revision{0};
        std::string role_definition;
        MmsPermission permission{MmsPermission::ReadValues};
        std::vector<std::string> object_prefixes;
    };

    std::string policy_id_;
    std::string revision_;
    std::vector<Rule> rules_;
};

}  // namespace cci::adapters
