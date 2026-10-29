#include "mms_acse_auth.hpp"
#include "mms_rbac.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

bool expect_result(const char* label, cci::adapters::MmsAcseAuthResult got,
                   cci::adapters::MmsAcseAuthResult want) {
    if (got != want) {
        std::cerr << label << ": expected " << static_cast<int>(want) << " got "
                  << static_cast<int>(got) << '\n';
        return false;
    }
    return true;
}

bool expect(bool condition, const char* label) {
    if (!condition) {
        std::cerr << label << " failed\n";
        return false;
    }
    return true;
}

std::vector<uint8_t> tlv(uint8_t tag, const std::vector<uint8_t>& value) {
    std::vector<uint8_t> result{tag};
    if (value.size() < 128) result.push_back(static_cast<uint8_t>(value.size()));
    else if (value.size() < 256) { result.push_back(0x81); result.push_back(static_cast<uint8_t>(value.size())); }
    else { result.push_back(0x82); result.push_back(static_cast<uint8_t>(value.size() >> 8)); result.push_back(static_cast<uint8_t>(value.size())); }
    result.insert(result.end(), value.begin(), value.end());
    return result;
}

std::vector<uint8_t> certificate_envelope(const std::vector<uint8_t>& extension_value,
                                          const std::vector<uint8_t>& oid,
                                          bool duplicate = false, bool critical = false) {
    auto extension = tlv(0x06, oid);
    if (critical) extension.insert(extension.end(), {0x01, 0x01, 0xff});
    const auto payload = tlv(0x04, extension_value);
    extension.insert(extension.end(), payload.begin(), payload.end());
    auto extensions = tlv(0x30, extension);
    if (duplicate) {
        const auto second = extensions;
        extensions.insert(extensions.end(), second.begin(), second.end());
    }
    // Structural parser fixture only: no signature/authentication assertion.
    auto tbs = tlv(0x30, tlv(0xa3, tlv(0x30, extensions)));
    tbs.insert(tbs.end(), {0x30, 0x00, 0x03, 0x01, 0x00});
    return tlv(0x30, tbs);
}

struct PolicyFixture {
    std::string roles, library, scopes;
    bool load(cci::adapters::MmsCommissioningPolicy& policy, std::string& error) const {
        return policy.load_xml(roles, library, scopes, error);
    }
};

void replace_once(std::string& value, const std::string& from, const std::string& to) {
    const auto pos = value.find(from);
    if (pos == std::string::npos) { std::cerr << "fixture text missing: " << from << '\n'; std::abort(); }
    value.replace(pos, from.size(), to);
}

PolicyFixture fixture(const std::string& permission, const std::string& object,
                      int role_id = -1, const std::string& aor = "DSO") {
    const std::string ns = "urn:oasis:names:tc:xacml:3.0:core:schema:wd-17";
    const std::string subject = "urn:oasis:names:tc:xacml:1.0:subject-category:access-subject";
    const std::string resource = "urn:oasis:names:tc:xacml:3.0:attribute-category:resource";
    const std::string iec = "urn:iec:62351:8:IS:1.0:attribute:";
    const auto match = [&](const std::string& attr, const std::string& value,
                            bool numeric, const std::string& category) {
        const std::string type = "http://www.w3.org/2001/XMLSchema#" +
                                std::string(numeric ? "integer" : "string");
        return std::string("<Match MatchId=\"urn:oasis:names:tc:xacml:1.0:function:") +
            (numeric ? "integer-equal" : "string-equal") + "\"><AttributeValue DataType=\"" +
            type + "\">" + value + "</AttributeValue><AttributeDesignator Category=\"" + category +
            "\" AttributeId=\"" + attr + "\" DataType=\"" + type + "\" MustBePresent=\"false\"/></Match>";
    };
    const auto set = [&](const std::string& id, bool root = false) {
        return std::string("<PolicySet ") + (root ? "xmlns=\"" + ns + "\" " : "") +
            "PolicySetId=\"" + id + "\" Version=\"1.0\" PolicyCombiningAlgId=\""
            "urn:oasis:names:tc:xacml:3.0:policy-combining-algorithm:permit-overrides\">";
    };
    PolicyFixture result;
    result.roles = set("urn:test:root", true) + "<Target/>" + set("urn:test:role") +
        "<Description>Private role " + std::to_string(role_id) + "</Description><Target><AnyOf><AllOf>" +
        match(iec + "role-id", std::to_string(role_id), true, subject) +
        match(iec + "roleDefinition", "IEC 62351-8-CEI016:2021", false, subject) +
        match(iec + "revision", "1", true, subject) +
        "</AllOf></AnyOf></Target><PolicySetIdReference>urn:test:group</PolicySetIdReference></PolicySet></PolicySet>";
    result.library = set("urn:test:library", true) + "<Target/>" + set("urn:test:group") +
        "<Target/><PolicySetIdReference>urn:test:permission</PolicySetIdReference></PolicySet>" +
        set("urn:test:permission") + "<Target/><Policy PolicyId=\"urn:test:policy\" Version=\"1.0\" "
        "RuleCombiningAlgId=\"urn:oasis:names:tc:xacml:1.0:rule-combining-algorithm:permit-overrides\">"
        "<Target/><Rule RuleId=\"urn:test:rule\" Effect=\"Permit\"><Target><AnyOf><AllOf>" +
        match("urn:oasis:names:tc:xacml:1.0:resource:resource-id", "IEC61850/MMS", false, resource) +
        "</AllOf></AnyOf><AnyOf><AllOf>" +
        match("urn:oasis:names:tc:xacml:1.0:action:action-id", permission, false, subject) +
        "</AllOf></AnyOf></Target></Rule></Policy></PolicySet></PolicySet>";
    result.scopes = "<CommissioningScopes Version=\"1.0\"><Binding RoleId=\"" + std::to_string(role_id) + "\" AoR=\"" + aor + "\" "
        "RoleDefinition=\"IEC 62351-8-CEI016:2021\" Revision=\"1\"><Scope Permission=\"" +
        permission + "\"><Object>" + object + "</Object></Scope></Binding></CommissioningScopes>";
    return result;
}

}  // namespace

int main() {
    const std::vector<uint8_t> lab_cert{0x30, 0x01, 0x02};
    const uint8_t* buf = lab_cert.data();
    const int len = static_cast<int>(lab_cert.size());

    cci::adapters::MmsAcseCredentialStore open{};
    open.rbac_enabled = false;

    if (!expect_result("TLS open RBAC",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Tls, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptGeneric)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("CERTIFICATE open RBAC",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Certificate, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptGeneric)) {
        return EXIT_FAILURE;
    }

    cci::adapters::MmsAcseCredentialStore rbac{};
    rbac.rbac_enabled = true;
    rbac.dso_der = lab_cert;
    rbac.viewer_der = {0x99};

    if (!expect_result("CERTIFICATE DSO allowlist",
                       cci::adapters::evaluate_acse_authentication(
                           rbac, cci::adapters::MmsAcseMechanism::Certificate, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptDso)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("TLS DSO allowlist",
                       cci::adapters::evaluate_acse_authentication(
                           rbac, cci::adapters::MmsAcseMechanism::Tls, buf, len),
                       cci::adapters::MmsAcseAuthResult::AcceptDso)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("viewer cert",
                       cci::adapters::evaluate_acse_authentication(
                           rbac, cci::adapters::MmsAcseMechanism::Certificate,
                           rbac.viewer_der.data(),
                           static_cast<int>(rbac.viewer_der.size())),
                       cci::adapters::MmsAcseAuthResult::AcceptViewer)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("password rejected",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Password, buf, len),
                       cci::adapters::MmsAcseAuthResult::RejectMechanism)) {
        return EXIT_FAILURE;
    }
    if (!expect_result("empty cert",
                       cci::adapters::evaluate_acse_authentication(
                           open, cci::adapters::MmsAcseMechanism::Tls, nullptr, 0),
                       cci::adapters::MmsAcseAuthResult::RejectEmptyCert)) {
        return EXIT_FAILURE;
    }

    if (!cci::adapters::mms_acse_mechanism_supported(cci::adapters::MmsAcseMechanism::Certificate) ||
        !cci::adapters::mms_acse_mechanism_supported(cci::adapters::MmsAcseMechanism::Tls)) {
        std::cerr << "supported mechanism check failed\n";
        return EXIT_FAILURE;
    }

    // IEC 62351-8 Profile A extension: one DSO_OPERATOR, AoR DSO, revision 1.
    const std::vector<uint8_t> profile_a_extension{
        0x30, 0x28, 0x30, 0x26, 0x30, 0x03, 0x02, 0x01, 0xff,
        0x0c, 0x03, 'D', 'S', 'O', 0x02, 0x01, 0x01,
        0x0c, 0x17, 'I', 'E', 'C', ' ', '6', '2', '3', '5', '1', '-', '8', '-',
        'C', 'E', 'I', '0', '1', '6', ':', '2', '0', '2', '1'};
    std::vector<cci::adapters::MmsProfileARole> roles;
    if (!expect(cci::adapters::decode_iec_user_roles_extension(
                    profile_a_extension.data(), profile_a_extension.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Ok &&
                    roles.size() == 1 && roles[0].role_id == -1 &&
                    roles[0].aor == "DSO" && roles[0].revision == 1 &&
                    roles[0].role_definition == "IEC 62351-8-CEI016:2021",
                "Profile A DSO extension")) {
        return EXIT_FAILURE;
    }
    const auto commissioning_policy = fixture("CONTROL", "LD_Plant/WlimDWMX1");
    cci::adapters::MmsCommissioningPolicy policy;
    std::string policy_error;
    if (!expect(commissioning_policy.load(policy, policy_error), "commissioning policy load") ||
        !expect(policy.accepts_role(roles[0]), "commissioning policy role match") ||
        !expect(policy.permits(roles[0], cci::adapters::MmsPermission::Control,
                               "LD_Plant/WlimDWMX1.Mod"),
                "DSO control permit") ||
        !expect(!policy.permits(roles[0], cci::adapters::MmsPermission::Control,
                                "LD_Plant/AggregatorOnly.Mod"),
                "cross-scope control deny")) {
        return EXIT_FAILURE;
    }
    cci::adapters::MmsCommissioningPolicy packaged_policy;
    if (!expect(packaged_policy.load_file(
                    std::string(CCLI_SOURCE_DIR) + "/config/rbac/commissioning-policy.xml",
                    policy_error),
                "packaged commissioning policy load") ||
        !expect(packaged_policy.permits(roles[0], cci::adapters::MmsPermission::Control,
                                        "LD_Plant/WlimDWMX1.Mod"),
                "packaged DSO control permit")) {
        return EXIT_FAILURE;
    }
    const cci::adapters::MmsProfileARole aggregator{-2, "AGGREGATOR", 1, "IEC 62351-8-CEI016:2021"};
    const cci::adapters::MmsProfileARole viewer{0, "CCI", 1, "IEC62351-8"};
    if (!expect(packaged_policy.permits(aggregator, cci::adapters::MmsPermission::Control,
                                        "LD_Plant/WSaDAGC1.WSptPct"), "referenced Aggregator control") ||
        !expect(!packaged_policy.permits(aggregator, cci::adapters::MmsPermission::Control,
                                         "LD_Plant/WSdDAGC1.WSptPct"), "Aggregator cross-scope denial") ||
        !expect(packaged_policy.permits(viewer, cci::adapters::MmsPermission::ReadValues,
                                        "LD_Plant/PdCMMXU1.TotW"), "shared referenced READVALUES") ||
        !expect(!packaged_policy.permits(viewer, cci::adapters::MmsPermission::Control,
                                         "LD_Plant/WSdDAGC1.WSptPct"),
                "unreferenced library CONTROL cannot grant Viewer access")) return EXIT_FAILURE;
    for (int id = 0; id <= 6; ++id) {
        for (unsigned revision = 0; revision <= 1; ++revision) {
            if (!expect(packaged_policy.accepts_role({id, "CCI", static_cast<uint8_t>(revision), "IEC62351-8"}),
                        "all standard roles accept revisions 0 and 1")) return EXIT_FAILURE;
        }
    }
    // All standard-role grants at both supported revisions, across the six
    // ACSI permission classes used by the selected CCI profile.
    const bool rights[7][6] = {
        {true,true,false,true,false,false}, {true,true,false,true,true,false},
        {true,true,true,true,false,true}, {true,true,false,true,false,true},
        {true,true,true,false,false,true}, {true,true,false,true,false,false},
        {true,true,false,false,false,true}
    };
    const cci::adapters::MmsPermission permissions[] = {
        cci::adapters::MmsPermission::ListObjects, cci::adapters::MmsPermission::ReadValues,
        cci::adapters::MmsPermission::DataSet, cci::adapters::MmsPermission::Reporting,
        cci::adapters::MmsPermission::Control, cci::adapters::MmsPermission::Config
    };
    const char* objects[] = {"LD_Plant/PdCMMXU1.TotW", "LD_Plant/PdCMMXU1.TotW",
        "LD_Plant/LLN0.DS_R_PdC_Mis4sec", "LD_Plant/LLN0.urcb_PdC_Mis4sec01",
        "LD_Plant/WlimDWMX1.Mod", "LD_Plant/WlimDWMX1.WMaxSptPct.ctlModel"};
    for (int id = 0; id <= 6; ++id) for (unsigned rev = 0; rev <= 1; ++rev) {
        const cci::adapters::MmsProfileARole role{id, "CCI", static_cast<uint8_t>(rev), "IEC62351-8"};
        for (unsigned p = 0; p < 6; ++p)
            if (!expect(packaged_policy.permits(role, permissions[p], objects[p]) == rights[id][p],
                        "standard role/revision privilege matrix")) return EXIT_FAILURE;
        if (!expect(packaged_policy.permits_control_roles({role}, objects[4]) == rights[id][4],
                    "standard CONFIG must not become CONTROL")) return EXIT_FAILURE;
    }
    for (const char* object : {"WSaDAGC1.Mod", "WSaDAGC1.WSptPct", "VArSaDVAR1.Mod", "VArSaDVAR1.VArTgtSptPct"}) {
        const std::string reference = "LD_Plant/" + std::string(object);
        if (!expect(packaged_policy.permits(aggregator, cci::adapters::MmsPermission::Config,
                                           reference + ".Oper.ctlVal"), "Aggregator C3 CONFIG payload granted") ||
            !expect(!packaged_policy.permits(aggregator, cci::adapters::MmsPermission::Config,
                                            reference + ".ctlModel"), "Aggregator control model protected")) return EXIT_FAILURE;
    }
    auto config_only = fixture("CONFIG", "LD_Plant/WSaDAGC1.WSptPct", -2, "AGGREGATOR");
    cci::adapters::MmsCommissioningPolicy config_policy;
    if (!expect(config_only.load(config_policy, policy_error), "CONFIG-only private-role fixture loads") ||
        !expect(!config_policy.permits(aggregator, cci::adapters::MmsPermission::Control,
                                       "LD_Plant/WSaDAGC1.WSptPct"), "fixture has no CONTROL grant") ||
        !expect(config_policy.permits_control_roles({aggregator}, "LD_Plant/WSaDAGC1.WSptPct"),
                "C3 CONFIG takes the validated control path") ||
        !expect(!config_policy.permits(aggregator, cci::adapters::MmsPermission::Config,
                                       "LD_Plant/WSaDAGC1.WSptPct.ctlModel"), "broad scope cannot expand C3") ||
        !expect(!config_policy.permits_control_roles({aggregator}, "LD_Plant/WSaDAGC10.WSptPct"),
                "C3 prefix collision denied") ||
        !expect(!config_policy.permits_control_roles({aggregator}, "LD_Plant/WSdDAGC1.WSptPct"),
                "C3 cross-actor control denied")) return EXIT_FAILURE;
    for (const char* suffix : {"", ".mxVal.f", ".Oper.origin", ".SBOw.ctlVal"})
        if (!expect(!config_policy.permits(aggregator, cci::adapters::MmsPermission::Config,
                       std::string("LD_Plant/WSaDAGC1.WSptPct") + suffix),
                    "C3 CONFIG grants only the allocated control value")) return EXIT_FAILURE;
    const cci::adapters::MmsProfileARole config_dso{-1, "DSO", 1, "IEC 62351-8-CEI016:2021"};
    if (!expect(fixture("CONFIG", "LD_Plant/WlimDWMX1.Mod").load(config_policy, policy_error) &&
                config_policy.permits_control_roles({config_dso}, "LD_Plant/WlimDWMX1.Mod"),
                "DSO C3 CONFIG uses checked controls too")) return EXIT_FAILURE;
    if (!expect(!packaged_policy.permits({5, "CCI", 1, "IEC62351-8"},
                                         cci::adapters::MmsPermission::DataSet, "LD_Plant/LLN0.DS_R_PdC_Mis4sec"),
                "SECAUD cannot create or use datasets through DATASET permission")) return EXIT_FAILURE;
    if (!expect(cci::adapters::decode_iec_user_roles_extension(
                    profile_a_extension.data(), profile_a_extension.size() - 1, roles) ==
                    cci::adapters::MmsProfileAParseResult::Malformed,
                "Profile A malformed extension")) {
        return EXIT_FAILURE;
    }

    // Regression checks against IEC 62351-8 10.5.1.3 and Annex T C1/C2.
    const cci::adapters::MmsProfileARole dso{-1, "DSO", 1, "IEC 62351-8-CEI016:2021"};
    if (!expect(!policy.permits(dso, cci::adapters::MmsPermission::Control,
                                "LD_Plant/WlimDWMX10.Mod"), "LN prefix collision denied")) return EXIT_FAILURE;
    auto wrong_revision = dso;
    wrong_revision.revision = 0;
    if (!expect(!policy.accepts_role(wrong_revision), "custom role wrong revision denied")) return EXIT_FAILURE;
    auto invalid_utf8 = profile_a_extension;
    invalid_utf8[11] = 0xff;
    if (!expect(cci::adapters::decode_iec_user_roles_extension(invalid_utf8.data(), invalid_utf8.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Malformed, "invalid UTF8 denied")) return EXIT_FAILURE;
    auto optional_delete = profile_a_extension;
    optional_delete[1] += 3;
    optional_delete[3] += 3;
    optional_delete.insert(optional_delete.end(), {0x0a, 0x01, 0x02});
    if (!expect(cci::adapters::decode_iec_user_roles_extension(optional_delete.data(), optional_delete.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Ok && roles.size() == 1 &&
                    roles.front().has_operation && roles.front().operation == 2 &&
                    !policy.accepts_role(roles.front()), "administrative Delete parsed but denied for session access")) return EXIT_FAILURE;
    auto optional_sequence = optional_delete;
    optional_sequence[1] += 7;
    optional_sequence[3] += 7;
    optional_sequence.insert(optional_sequence.end(), {0x02, 0x05, 0, 0xff, 0xff, 0xff, 0xff});
    if (!expect(cci::adapters::decode_iec_user_roles_extension(optional_sequence.data(), optional_sequence.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Ok && roles.front().has_sequence &&
                    roles.front().status_sequence == 0xffffffffU && !policy.accepts_role(roles.front()),
                "maximum administrative sequence parsed without session grants")) return EXIT_FAILURE;
    optional_sequence[optional_sequence.size() - 4] = 0x01;
    optional_sequence[optional_sequence.size() - 5] = 0x01;
    if (!expect(cci::adapters::decode_iec_user_roles_extension(optional_sequence.data(), optional_sequence.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Malformed, "administrative sequence overflow denied")) return EXIT_FAILURE;
    const auto unicode_token = [&](unsigned characters) {
        std::vector<uint8_t> aor;
        for (unsigned i = 0; i < characters; ++i) aor.insert(aor.end(), {0xc3, 0xa9});
        auto body = tlv(0x30, tlv(2, {0xff}));
        const auto area = tlv(12, aor);
        body.insert(body.end(), area.begin(), area.end());
        body.insert(body.end(), {2, 1, 1, 12, 0});
        return tlv(0x30, tlv(0x30, body));
    };
    const auto unicode64 = unicode_token(64), unicode65 = unicode_token(65);
    if (!expect(cci::adapters::decode_iec_user_roles_extension(unicode64.data(), unicode64.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Ok && roles.front().aor.size() == 128,
                "AoR limit counts Unicode characters") ||
        !expect(cci::adapters::decode_iec_user_roles_extension(unicode65.data(), unicode65.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Malformed, "AoR character overflow denied")) return EXIT_FAILURE;
    auto out_of_range = profile_a_extension;
    out_of_range[1] += 2; out_of_range[3] += 2; out_of_range[5] += 2;
    out_of_range[7] = 3;
    out_of_range.erase(out_of_range.begin() + 8);
    out_of_range.insert(out_of_range.begin() + 8, {0, 0x80, 0}); // +32768
    if (!expect(cci::adapters::decode_iec_user_roles_extension(out_of_range.data(), out_of_range.size(), roles) ==
                    cci::adapters::MmsProfileAParseResult::Malformed, "RoleID range enforced")) return EXIT_FAILURE;
    for (const auto& oid : std::vector<std::vector<uint8_t>>{
             {0x28, 0x83, 0xe7, 0x0f, 0x08, 0x01},
             {0x2a, 0x86, 0x48, 0xce, 0x56, 0x08, 0x01}}) {
        const auto cert = certificate_envelope(profile_a_extension, oid);
        const auto duplicate = certificate_envelope(profile_a_extension, oid, true);
        const auto critical = certificate_envelope(profile_a_extension, oid, false, true);
        if (!expect(cci::adapters::decode_profile_a_certificate(cert.data(), cert.size(), roles) ==
                        cci::adapters::MmsProfileAParseResult::Ok && roles[0].role_id == -1,
                    "current and legacy OID certificate decoding") ||
            !expect(cci::adapters::decode_profile_a_certificate(duplicate.data(), duplicate.size(), roles) ==
                        cci::adapters::MmsProfileAParseResult::Malformed, "duplicate role extension denied") ||
            !expect(cci::adapters::decode_profile_a_certificate(critical.data(), critical.size(), roles) ==
                        cci::adapters::MmsProfileAParseResult::Malformed, "critical role extension denied")) return EXIT_FAILURE;
    }

    if (!expect(fixture("DATASET", "LD_Plant/LLN0").load(policy, policy_error), "dataset fixture loads") ||
        !expect(policy.permits_dataset(dso, true, "LD_Plant/LLN0.DS"), "custom dataset read allowed") ||
        !expect(!policy.permits_dataset(dso, false, "LD_Plant/LLN0.DS"), "custom dataset mutation denied") ||
        !expect(fixture("REPORTING", "LD_Plant/LLN0").load(policy, policy_error), "report fixture loads") ||
        !expect(policy.permits_report(dso, false, "", "LD_Plant/LLN0.urcb"), "report read allowed") ||
        !expect(policy.permits_report(dso, true, "RptEna", "LD_Plant/LLN0.urcb"), "RptEna write allowed") ||
        !expect(!policy.permits_report(dso, true, "DatSet", "LD_Plant/LLN0.urcb"), "DatSet write denied") ||
        !expect(!policy.permits_report(dso, true, "", "LD_Plant/LLN0.urcb"), "whole RCB write denied")) return EXIT_FAILURE;

    if (!expect(!policy.load_file(std::string(CCLI_SOURCE_DIR) + "/no-such-policy.xml", policy_error) &&
                policy.empty(), "failed reload clears grants")) return EXIT_FAILURE;
    const auto reject = [&](const PolicyFixture& candidate, const char* label) {
        return expect(!candidate.load(policy, policy_error) && policy.empty(), label);
    };
    auto candidate = fixture("CONTROL", "LD_Plant/LLN0");
    auto broad_scope = fixture("CONFIG", "LD_Plant/LPHD1.PhyNam");
    if (!expect(broad_scope.load(policy, policy_error), "owner scope for reserved RO object loads") ||
        !expect(!policy.permits(dso, cci::adapters::MmsPermission::Config, "LD_Plant/LPHD1.PhyNam.vendor"),
                "private reserved RO bound survives broader commissioning")) return EXIT_FAILURE;
    replace_once(candidate.roles, ">urn:test:group<", ">\n  urn:test:group\n<");
    if (!expect(candidate.load(policy, policy_error), "wrapped template reference resolves")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.roles, ">1</AttributeValue>", ">1bad</AttributeValue>");
    if (!reject(candidate, "invalid integer revision rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.library, "</Rule>", "<Condition/></Rule>");
    if (!reject(candidate, "unsupported Condition rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.roles, "permit-overrides", "deny-overrides");
    if (!reject(candidate, "template requires permit-overrides")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    candidate.roles.insert(0, "<!DOCTYPE PolicySet [<!ENTITY x 'Permit'>]>");
    if (!reject(candidate, "DTD prohibited")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.roles, ">urn:test:group<", ">urn:test:missing<");
    if (!reject(candidate, "missing policy reference rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.library, ">urn:test:permission<", ">urn:test:group<");
    if (!reject(candidate, "cyclic policy reference rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.library, "PolicySetId=\"urn:test:permission\"", "PolicySetId=\"urn:test:group\"");
    if (!reject(candidate, "duplicate policy identifier rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.roles, "#integer", "#string");
    if (!reject(candidate, "wrong RoleID datatype rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.scopes, "Permission=\"CONTROL\"", "Permission=\"CONFIG\"");
    if (!reject(candidate, "scope cannot add a role permission")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.scopes, ">LD_Plant/LLN0<", ">*<");
    if (!reject(candidate, "private role wildcard scope rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.scopes, "AoR=\"DSO\"", "AoR=\"OTHER\"");
    if (!expect(candidate.load(policy, policy_error), "separate commissioning AoR loads") ||
        !expect(!policy.accepts_role(dso), "uncommissioned AoR denied") ||
        !expect(policy.accepts_role({-1, "OTHER", 1, "IEC 62351-8-CEI016:2021"}),
                "AoR binding changes without editing role definition")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.library, ">CONTROL<", ">CONFIG<");
    if (!reject(candidate, "action target is enforced")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.library, ">IEC61850/MMS<", ">OTHER<");
    if (!reject(candidate, "resource target is enforced")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    replace_once(candidate.roles, "core:schema:wd-17", "core:schema:wrong");
    if (!reject(candidate, "wrong namespace rejected")) return EXIT_FAILURE;
    candidate = fixture("CONTROL", "LD_Plant/LLN0");
    for (unsigned i = 0; i < 65; ++i) {
        replace_once(candidate.roles, "<Target/>", "<X><Target/></X>");
    }
    if (!reject(candidate, "excessive XML nesting rejected")) return EXIT_FAILURE;

    std::cout << "mms_acse_auth_test: PASS (ACSE + Profile A + commissioning policy)\n";
    return EXIT_SUCCESS;
}
