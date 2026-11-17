#include "mms_rbac.hpp"
#include "mms_xacml.hpp"
#include <filesystem>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <stdexcept>
#include <set>

namespace cci::adapters {
namespace {

bool component_prefix(const std::string& value, const std::string& prefix) {
    return prefix == "*" || value == prefix || (value.size() > prefix.size() &&
        value.compare(0, prefix.size(), prefix) == 0 && (value[prefix.size()] == '.' || value[prefix.size()] == '/'));
}

// CEI 57-142:2026-07 Tables 22/23. These limits cannot be enlarged by a
// commissioned scope or by also granting CONFIG on a read-only object.
bool cei_private_allows(int role, MmsPermission permission, const std::string& reference) {
    if (role >= 0) return true;
    if (reference.rfind("LD_Plant/", 0) != 0) return false;
    if (permission == MmsPermission::DataSet || permission == MmsPermission::Reporting) return true;
    const auto object = reference.substr(9);
    const bool writing = permission == MmsPermission::Control || permission == MmsPermission::Config;
    static const std::vector<std::string> dso_rw = {
        "WlimDWMX1.Mod", "WlimDWMX1.WMaxSptPct", "WSdDAGC1.Mod", "WSdDAGC1.WSptPct",
        "VArSdDVAR1.Mod", "VArSdDVAR1.VArTgtSptPct", "PFSPDFPF1.Mod", "PFSPDFPF1.PFGnTgtSpt", "PFSPDFPF1.PFLodTgtSpt",
        "VArVDVVR1.Mod", "VArVDVVR1.K", "VArVDPMC1.WSpt1", "VArVDPMC2.WSpt1",
        "VArVDECP1.VMax", "VArVDECP1.Vmin", "VArVDECP2.VMax", "VArVDECP2.Vmin",
        "PFWDPFW1.Mod", "PFWDPFW1.WSetA", "PFWDPFW1.PFSetA", "PFWDPFW1.WSetB", "PFWDPFW1.PFSetB",
        "PFWDPFW1.WSetC", "PFWDPFW1.PFSetC", "PFWDPFW1.VLkIn", "PFWDPFW1.VLkOut"
    };
    static const std::vector<std::string> agg_rw = {"WSaDAGC1.Mod", "WSaDAGC1.WSptPct", "VArSaDVAR1.Mod", "VArSaDVAR1.VArTgtSptPct"};
    // Table 23 allocates the control value, not all configuration attributes of
    // these DOs. Even a broader signed scope must not expose ctlModel/metadata.
    if (role == -2 && permission == MmsPermission::Config)
        return std::any_of(agg_rw.begin(), agg_rw.end(), [&](const auto& item) {
            return component_prefix(object, item + ".Oper.ctlVal");
        });
    const auto& writable = role == -1 ? dso_rw : agg_rw;
    if (std::any_of(writable.begin(), writable.end(), [&](const auto& item) { return component_prefix(object, item); })) return true;
    if (writing) return false;
    static const std::vector<std::string> dso_ro = {
        "LPHD1.PhyNam", "PdC_WiDPCC1.WRtg", "PdC_WaDPCC1.WRtg", "PdC_QiDPCC1.VArRtg", "PdC_QcDPCC1.VArRtg", "PdC_VADPCC1.VARtg",
        "DisFRDECP1.Beh", "DisFRDGEN1.Beh", "DisFRDSTO1.Beh", "IDGXCBR1.Pos",
        "PdCMMXU1.TotW", "PdCMMXU1.TotVAr", "PdCMMXU1.PPV", "PdCMMXU1.A",
        "GenPVMMXU1.TotW", "GenWiMMXU1.TotW", "GenTerMMXU1.TotW", "GenIdrMMXU1.TotW", "StMMXU1.TotW"
    };
    if (role == -1 && std::any_of(dso_ro.begin(), dso_ro.end(), [&](const auto& item) { return component_prefix(object, item); })) return true;
    if (role == -2 && (component_prefix(object, "PdCiMMXU1.TotW") || component_prefix(object, "PdCiMMXU1.TotVAr"))) return true;
    const auto dot = object.find('.');
    const auto ln = object.substr(0, dot);
    const auto member = dot == std::string::npos ? std::string{} : object.substr(dot + 1);
    const auto numbered = [&](const char* prefix) {
        const std::string head(prefix);
        const auto suffix = ln.substr(std::min(ln.size(), head.size()));
        return ln.rfind(head, 0) == 0 && !suffix.empty() && suffix[0] != '0' && suffix.find_first_not_of("0123456789") == std::string::npos;
    };
    if (role == -1 && numbered("SGGMMXU") && component_prefix(member, "TotW")) return true;
    if (role == -1 && numbered("SSGGDGEN") && (component_prefix(member, "Health") || component_prefix(member, "GnGrId"))) return true;
    const std::vector<std::string> controlled = role == -1
        ? std::vector<std::string>{"WlimDWMX1", "WSdDAGC1", "VArSdDVAR1", "PFSPDFPF1", "VArVDVVR1", "PFWDPFW1"}
        : std::vector<std::string>{"WSaDAGC1", "VArSaDVAR1"};
    if (std::find(controlled.begin(), controlled.end(), ln) == controlled.end()) return false;
    return component_prefix(member, "Beh") || component_prefix(member, "Health") ||
        (ln == "WlimDWMX1" ? component_prefix(member, "FctOpStAuto") || component_prefix(member, "FctOpStEx") : component_prefix(member, "FctOpSt"));
}

constexpr std::uint8_t kSequence = 0x30;
constexpr std::uint8_t kInteger = 0x02;
constexpr std::uint8_t kUtf8String = 0x0c;
constexpr std::uint8_t kOctetString = 0x04;
constexpr std::uint8_t kExplicitExtensions = 0xa3;
constexpr std::uint8_t kBoolean = 0x01;
constexpr std::uint8_t kIecUserRolesOid[] = {0x28, 0x83, 0xe7, 0x0f, 0x08, 0x01};
constexpr std::uint8_t kLegacyUserRolesOid[] = {0x2a, 0x86, 0x48, 0xce, 0x56, 0x08, 0x01};

struct DerValue {
    std::uint8_t tag{0};
    const std::uint8_t* value{nullptr};
    std::size_t length{0};
};

class DerReader {
public:
    DerReader(const std::uint8_t* begin, std::size_t length) : pos_(begin), end_(begin + length) {}

    bool read(DerValue& out) {
        if (pos_ == nullptr || pos_ >= end_) {
            return false;
        }
        out.tag = *pos_++;
        if (pos_ >= end_) {
            return false;
        }
        std::size_t length = 0;
        const std::uint8_t first = *pos_++;
        if ((first & 0x80U) == 0U) {
            length = first;
        } else {
            const std::size_t octets = first & 0x7fU;
            if (octets == 0 || octets > sizeof(std::size_t) ||
                static_cast<std::size_t>(end_ - pos_) < octets) {
                return false;
            }
            if (*pos_ == 0) return false; // DER forbids redundant length octets.
            for (std::size_t i = 0; i < octets; ++i) {
                if (length > (std::numeric_limits<std::size_t>::max() >> 8U)) {
                    return false;
                }
                length = (length << 8U) | *pos_++;
            }
            if (length < 128) return false;
        }
        if (static_cast<std::size_t>(end_ - pos_) < length) {
            return false;
        }
        out.value = pos_;
        out.length = length;
        pos_ += length;
        return true;
    }

    bool done() const { return pos_ == end_; }

private:
    const std::uint8_t* pos_;
    const std::uint8_t* end_;
};

bool oid_equal(const DerValue& value) {
    return value.tag == 0x06 &&
        ((value.length == sizeof(kIecUserRolesOid) &&
          std::equal(value.value, value.value + value.length, std::begin(kIecUserRolesOid))) ||
         (value.length == sizeof(kLegacyUserRolesOid) &&
          std::equal(value.value, value.value + value.length, std::begin(kLegacyUserRolesOid))));
}

bool parse_integer(const DerValue& value, int& result) {
    if (value.tag != kInteger || value.length == 0 || value.length > sizeof(int)) {
        return false;
    }
    if (value.length > 1 &&
        ((value.value[0] == 0 && (value.value[1] & 0x80U) == 0) ||
         (value.value[0] == 0xff && (value.value[1] & 0x80U) != 0))) return false;
    std::int64_t parsed = 0;
    for (std::size_t i = 0; i < value.length; ++i) {
        parsed = (parsed << 8U) | value.value[i];
    }
    if ((value.value[0] & 0x80U) != 0) {
        parsed -= (std::int64_t{1} << (value.length * 8U));
    }
    if (parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max()) {
        return false;
    }
    result = static_cast<int>(parsed);
    return true;
}

bool parse_utf8(const DerValue& value, std::string& result) {
    if (value.tag != kUtf8String) {
        return false;
    }
    // Validate Unicode scalar values, rejecting overlong forms, surrogates,
    // embedded NUL, truncated encodings and values beyond U+10FFFF.
    for (std::size_t i = 0; i < value.length;) {
        const auto first = value.value[i++];
        if (first == 0) return false;
        if (first < 0x80) continue;
        unsigned count = 0;
        std::uint32_t cp = 0, minimum = 0;
        if (first >= 0xc2 && first <= 0xdf) { count = 1; cp = first & 0x1f; minimum = 0x80; }
        else if (first >= 0xe0 && first <= 0xef) { count = 2; cp = first & 0x0f; minimum = 0x800; }
        else if (first >= 0xf0 && first <= 0xf4) { count = 3; cp = first & 0x07; minimum = 0x10000; }
        else return false;
        if (value.length - i < count) return false;
        while (count--) {
            const auto next = value.value[i++];
            if ((next & 0xc0) != 0x80) return false;
            cp = (cp << 6U) | (next & 0x3f);
        }
        if (cp < minimum || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) return false;
    }
    result.assign(reinterpret_cast<const char*>(value.value), value.length);
    return true;
}

bool parse_role_info(const DerValue& encoded, std::vector<MmsProfileARole>& roles) {
    if (encoded.tag != kSequence) {
        return false;
    }
    DerReader reader(encoded.value, encoded.length);
    DerValue role_ids;
    DerValue aor;
    DerValue revision;
    DerValue definition;
    if (!reader.read(role_ids) || role_ids.tag != kSequence || !reader.read(aor) ||
        !reader.read(revision) || !reader.read(definition)) {
        return false;
    }
    DerReader ids(role_ids.value, role_ids.length);
    MmsProfileARole role;
    if (!parse_utf8(aor, role.aor)) {
        return false;
    }
    int rev = 0;
    const auto characters = [](const std::string& value) {
        return std::count_if(value.begin(), value.end(), [](unsigned char c) { return (c & 0xc0) != 0x80; });
    };
    if (!parse_integer(revision, rev) || rev < 0 || rev > 255 ||
        !parse_utf8(definition, role.role_definition) || role.aor.empty() || characters(role.aor) > 64 ||
        characters(role.role_definition) > 23) {
        return false;
    }
    role.revision = static_cast<std::uint8_t>(rev);
    if (!reader.done()) {
        DerValue optional;
        if (!reader.read(optional)) return false;
        if (optional.tag == 0x0a) {
            role.has_operation = true;
            optional.tag = kInteger;
            if (!parse_integer(optional, role.operation) || role.operation < 1 || role.operation > 3) return false;
            if (reader.done()) optional.tag = 0;
            else if (!reader.read(optional)) return false;
        }
        if (optional.tag != 0) {
            if (optional.tag != kInteger || optional.length == 0 || optional.length > 5 || (optional.value[0] & 0x80)) return false;
            if (optional.length > 1 && optional.value[0] == 0 && !(optional.value[1] & 0x80)) return false;
            std::uint64_t sequence = 0;
            for (std::size_t i = 0; i < optional.length; ++i) sequence = sequence * 256 + optional.value[i];
            if (sequence > 0xffffffffULL) return false;
            role.has_sequence = true;
            role.status_sequence = static_cast<std::uint32_t>(sequence);
        }
    }
    if (!reader.done()) return false;
    const auto first_role = roles.size();
    if (std::any_of(roles.begin(), roles.end(), [&](const MmsProfileARole& existing) {
            return existing.aor == role.aor && existing.role_definition == role.role_definition;
        })) return false; // IEC 62351-8 10.5.1.3: one record per AoR/definition.
    bool has_role_id = false;
    while (!ids.done()) {
        DerValue role_id;
        MmsProfileARole expanded = role;
        if (!ids.read(role_id) || !parse_integer(role_id, expanded.role_id) ||
            expanded.role_id < -32768 || expanded.role_id > 32767) {
            return false;
        }
        if (std::any_of(roles.begin() + first_role, roles.end(), [&](const auto& existing) { return existing.role_id == expanded.role_id; })) return false;
        roles.push_back(std::move(expanded));
        if (roles.size() - first_role > 256 || roles.size() > 4096) return false;
        has_role_id = true;
    }
    return has_role_id;
}

}  // namespace

MmsProfileAParseResult decode_iec_user_roles_extension(const std::uint8_t* der,
                                                        std::size_t length,
                                                        std::vector<MmsProfileARole>& roles) {
    roles.clear();
    if (der == nullptr || length == 0) return MmsProfileAParseResult::Empty;
    DerReader extension_reader(der, length);
    DerValue extension;
    if (!extension_reader.read(extension) || !extension_reader.done() || extension.tag != kSequence) {
        return MmsProfileAParseResult::Malformed;
    }
    DerReader reader(extension.value, extension.length);
    while (!reader.done()) {
        DerValue item;
        if (!reader.read(item) || !parse_role_info(item, roles)) {
            roles.clear();
            return MmsProfileAParseResult::Malformed;
        }
    }
    return roles.empty() ? MmsProfileAParseResult::Empty : MmsProfileAParseResult::Ok;
}

MmsProfileAParseResult decode_profile_a_certificate(const std::uint8_t* der,
                                                     std::size_t length,
                                                     std::vector<MmsProfileARole>& roles) {
    roles.clear();
    if (der == nullptr || length == 0) return MmsProfileAParseResult::Empty;
    DerReader certificate_reader(der, length);
    DerValue certificate;
    DerValue tbs;
    if (!certificate_reader.read(certificate) || !certificate_reader.done() ||
        certificate.tag != kSequence) return MmsProfileAParseResult::Malformed;
    DerReader fields(certificate.value, certificate.length);
    if (!fields.read(tbs) || tbs.tag != kSequence) return MmsProfileAParseResult::Malformed;
    DerValue signature_algorithm, signature;
    if (!fields.read(signature_algorithm) || signature_algorithm.tag != kSequence ||
        !fields.read(signature) || signature.tag != 0x03 || !fields.done()) {
        return MmsProfileAParseResult::Malformed;
    }
    DerReader tbs_fields(tbs.value, tbs.length);
    DerValue field;
    bool found = false;
    while (!tbs_fields.done()) {
        if (!tbs_fields.read(field)) return MmsProfileAParseResult::Malformed;
        if (field.tag != kExplicitExtensions) continue;
        DerReader explicit_reader(field.value, field.length);
        DerValue extension_list;
        if (!explicit_reader.read(extension_list) || !explicit_reader.done() ||
            extension_list.tag != kSequence) return MmsProfileAParseResult::Malformed;
        DerReader extensions(extension_list.value, extension_list.length);
        while (!extensions.done()) {
            DerValue extension;
            if (!extensions.read(extension) || extension.tag != kSequence) return MmsProfileAParseResult::Malformed;
            DerReader entry(extension.value, extension.length);
            DerValue oid;
            DerValue next;
            if (!entry.read(oid) || !entry.read(next)) return MmsProfileAParseResult::Malformed;
            bool critical = false;
            if (next.tag == kBoolean) {
                if (next.length != 1 || (next.value[0] != 0 && next.value[0] != 0xff))
                    return MmsProfileAParseResult::Malformed;
                critical = next.value[0] != 0;
                if (!entry.read(next)) return MmsProfileAParseResult::Malformed;
            }
            if (next.tag != kOctetString || !entry.done()) return MmsProfileAParseResult::Malformed;
            if (oid_equal(oid)) {
                if (found || critical) { roles.clear(); return MmsProfileAParseResult::Malformed; }
                const auto result = decode_iec_user_roles_extension(next.value, next.length, roles);
                if (result != MmsProfileAParseResult::Ok) return result;
                found = true;
            }
        }
    }
    return found ? MmsProfileAParseResult::Ok : MmsProfileAParseResult::MissingExtension;
}

const char* mms_role_name(MmsRole role) {
    switch (role) {
    case MmsRole::DsoOperator: return "DSO_OPERATOR";
    case MmsRole::AggregatorOperator: return "AGGREGATOR_OPERATOR";
    case MmsRole::Viewer: return "VIEWER";
    case MmsRole::Operator: return "OPERATOR";
    case MmsRole::Engineer: return "ENGINEER";
    case MmsRole::Installer: return "INSTALLER";
    case MmsRole::SecAdm: return "SECADM";
    case MmsRole::SecAud: return "SECAUD";
    case MmsRole::RbacMnt: return "RBACMNT";
    default: return "UNKNOWN";
    }
}

const char* mms_permission_name(MmsPermission permission) {
    switch (permission) {
    case MmsPermission::ListObjects: return "LISTOBJECTS";
    case MmsPermission::ReadValues: return "READVALUES";
    case MmsPermission::DataSet: return "DATASET";
    case MmsPermission::Reporting: return "REPORTING";
    case MmsPermission::Control: return "CONTROL";
    case MmsPermission::Config: return "CONFIG";
    }
    return "UNKNOWN";
}

bool MmsCommissioningPolicy::load_file(const std::string& path, std::string& error) {
    rules_.clear(); policy_id_.clear(); revision_.clear(); error.clear();
    const auto directory = std::filesystem::path(path).parent_path();
    std::string documents[3];
    const std::filesystem::path paths[] = {path, directory / "permission-library.xml",
                                         directory / "commissioning-scopes.xml"};
    for (unsigned i = 0; i < 3; ++i) {
        std::ifstream in(paths[i], std::ios::binary | std::ios::ate);
        if (!in || in.tellg() < 0 || in.tellg() > 1024 * 1024) {
            error = "cannot read bounded commissioning policy package: " + paths[i].string();
            return false;
        }
        in.seekg(0);
        documents[i].assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }
    return load_xml(documents[0], documents[1], documents[2], error);
}

bool MmsCommissioningPolicy::load_xml(const std::string& roles_xml,
                                      const std::string& library_xml,
                                      const std::string& scopes_xml, std::string& error) {
    rules_.clear(); policy_id_.clear(); revision_.clear();
    std::vector<MmsCompiledGrant> grants;
    if (!compile_iec_xacml(roles_xml, library_xml, scopes_xml, grants,
                          policy_id_, revision_, error)) return false;
    for (auto& grant : grants) {
        Rule rule;
        rule.role_id = grant.role.role_id;
        rule.aor = std::move(grant.role.aor);
        rule.token_revision = grant.role.revision;
        rule.role_definition = std::move(grant.role.role_definition);
        rule.permission = grant.permission;
        rule.object_prefixes = std::move(grant.objects);
        rules_.push_back(std::move(rule));
    }
    return true;
}

bool MmsCommissioningPolicy::accepts_role(const MmsProfileARole& role) const {
    return std::any_of(rules_.begin(), rules_.end(), [&role](const Rule& rule) {
        return !role.has_operation && !role.has_sequence && rule.role_id == role.role_id && rule.aor == role.aor &&
               rule.token_revision == role.revision && rule.role_definition == role.role_definition;
    });
}

bool MmsCommissioningPolicy::permits_any(const MmsProfileARole& role,
                                         MmsPermission permission) const {
    return std::any_of(rules_.begin(), rules_.end(), [&](const Rule& rule) {
        return !role.has_operation && !role.has_sequence && rule.role_id == role.role_id && rule.aor == role.aor &&
               rule.token_revision == role.revision && rule.role_definition == role.role_definition &&
               rule.permission == permission;
    });
}

bool MmsCommissioningPolicy::permits(const MmsProfileARole& role, MmsPermission permission,
                                     const std::string& object_reference) const {
    return permits_roles({role}, permission, object_reference);
}

bool MmsCommissioningPolicy::permits_roles(const std::vector<MmsProfileARole>& roles,
                                           MmsPermission permission,
                                           const std::string& object_reference) const {
    bool permitted = false;
    for (const auto& rule : rules_) {
        if (rule.permission != permission || !cei_private_allows(rule.role_id, permission, object_reference) ||
            !std::any_of(roles.begin(), roles.end(), [&](const MmsProfileARole& role) {
                return !role.has_operation && !role.has_sequence && rule.role_id == role.role_id && rule.aor == role.aor &&
                    rule.token_revision == role.revision &&
                    rule.role_definition == role.role_definition;
            })) continue;
        const bool matches = std::any_of(rule.object_prefixes.begin(), rule.object_prefixes.end(),
                           [&](const std::string& prefix) {
                               return component_prefix(object_reference, prefix);
                           });
        if (matches) permitted = true;
    }
    return permitted;
}

bool MmsCommissioningPolicy::permits_control_roles(const std::vector<MmsProfileARole>& roles,
                                                   const std::string& object_reference) const {
    if (permits_roles(roles, MmsPermission::Control, object_reference)) return true;
    // The private-role tables map SetDataValues to CONTROL/CONFIG. An allocated
    // CONFIG ctlVal uses the same checked control service, never a raw cache write.
    for (const auto& role : roles)
        if ((role.role_id == -1 || role.role_id == -2) &&
            permits(role, MmsPermission::Config, object_reference + ".Oper.ctlVal")) return true;
    return false;
}

bool MmsCommissioningPolicy::permits_dataset(const MmsProfileARole& role, bool read_only,
                                             const std::string& object_reference) const {
    return permits_dataset_roles({role}, read_only, object_reference);
}

bool MmsCommissioningPolicy::permits_dataset_roles(const std::vector<MmsProfileARole>& roles,
                                                    bool read_only,
                                                    const std::string& object_reference) const {
    if (!read_only && std::any_of(roles.begin(), roles.end(), [](const MmsProfileARole& role) {
            return role.role_id == -1 || role.role_id == -2;
        })) return false;
    return permits_roles(roles, MmsPermission::DataSet, object_reference);
}

bool MmsCommissioningPolicy::permits_report(const MmsProfileARole& role, bool write,
                                            const std::string& member,
                                            const std::string& object_reference) const {
    return permits_report_roles({role}, write, member, object_reference);
}

bool MmsCommissioningPolicy::permits_report_roles(const std::vector<MmsProfileARole>& roles,
                                                   bool write, const std::string& member,
                                                   const std::string& object_reference) const {
    if (write && member != "RptEna" &&
        std::any_of(roles.begin(), roles.end(), [](const MmsProfileARole& role) {
            return role.role_id == -1 || role.role_id == -2;
        })) return false;
    return permits_roles(roles, MmsPermission::Reporting, object_reference);
}

}  // namespace cci::adapters
