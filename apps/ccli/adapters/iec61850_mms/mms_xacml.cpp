#include "mms_xacml.hpp"
#include "../../third_party/pugixml/pugixml.hpp"

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>

namespace cci::adapters {
namespace {
constexpr const char* ns = "urn:oasis:names:tc:xacml:3.0:core:schema:wd-17";
constexpr const char* subject = "urn:oasis:names:tc:xacml:1.0:subject-category:access-subject";
constexpr const char* resource = "urn:oasis:names:tc:xacml:3.0:attribute-category:resource";
constexpr const char* str_type = "http://www.w3.org/2001/XMLSchema#string";
constexpr const char* int_type = "http://www.w3.org/2001/XMLSchema#integer";
constexpr const char* role_attr = "urn:iec:62351:8:IS:1.0:attribute:role-id";
constexpr const char* definition_attr = "urn:iec:62351:8:IS:1.0:attribute:roleDefinition";
constexpr const char* revision_attr = "urn:iec:62351:8:IS:1.0:attribute:revision";
constexpr const char* object_attr = "urn:oasis:names:tc:xacml:1.0:resource:resource-id";
constexpr const char* action_attr = "urn:oasis:names:tc:xacml:1.0:action:action-id";
using Attributes = std::map<std::string, std::string>;

std::string local(const pugi::xml_node& node) {
    const std::string name = node.name();
    const auto colon = name.find(':');
    return name.substr(colon == std::string::npos ? 0 : colon + 1);
}

std::string namespace_uri(pugi::xml_node node, const std::string& name) {
    const auto colon = name.find(':');
    const std::string attr = colon == std::string::npos ? "xmlns" : "xmlns:" + name.substr(0, colon);
    for (; node; node = node.parent())
        if (node.attribute(attr.c_str())) return node.attribute(attr.c_str()).value();
    return {};
}

std::vector<pugi::xml_node> elements(const pugi::xml_node& node) {
    std::vector<pugi::xml_node> result;
    for (const auto child : node.children())
        if (child.type() == pugi::node_element) result.push_back(child);
        else if (child.type() != pugi::node_comment && child.type() != pugi::node_pcdata)
            throw std::runtime_error("unsupported XML node");
        else if (child.type() == pugi::node_pcdata && std::string(child.value()).find_first_not_of(" \r\n\t") != std::string::npos)
            throw std::runtime_error("unexpected XML text");
    return result;
}

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::size_t unicode_length(const std::string& value) {
    std::size_t length = 0;
    for (std::size_t i = 0; i < value.size(); ++length) {
        const auto first = static_cast<unsigned char>(value[i++]);
        require(first != 0, "embedded NUL in role attribute");
        if (first < 0x80) continue;
        unsigned count = 0;
        std::uint32_t cp = 0, minimum = 0;
        if (first >= 0xc2 && first <= 0xdf) { count = 1; cp = first & 0x1f; minimum = 0x80; }
        else if (first >= 0xe0 && first <= 0xef) { count = 2; cp = first & 0x0f; minimum = 0x800; }
        else if (first >= 0xf0 && first <= 0xf4) { count = 3; cp = first & 0x07; minimum = 0x10000; }
        else throw std::runtime_error("invalid UTF-8 role attribute");
        require(value.size() - i >= count, "truncated UTF-8 role attribute");
        while (count--) {
            const auto next = static_cast<unsigned char>(value[i++]);
            require((next & 0xc0) == 0x80, "invalid UTF-8 continuation");
            cp = (cp << 6U) | (next & 0x3f);
        }
        require(cp >= minimum && cp <= 0x10ffff && !(cp >= 0xd800 && cp <= 0xdfff),
                "invalid Unicode scalar in role attribute");
    }
    return length;
}

void attributes(const pugi::xml_node& node, std::initializer_list<const char*> allowed) {
    for (const auto attr : node.attributes()) {
        const std::string name = attr.name();
        if (name == "xmlns" || name.rfind("xmlns:", 0) == 0) continue;
        require(std::any_of(allowed.begin(), allowed.end(), [&](const char* item) { return name == item; }),
                "unsupported policy attribute");
    }
}

std::string text(const pugi::xml_node& node) {
    std::string result;
    for (const auto child : node.children()) {
        require(child.type() == pugi::node_pcdata || child.type() == pugi::node_cdata,
                "element must contain text only");
        result += child.value();
    }
    return result;
}

std::string reference(const pugi::xml_node& node) {
    const auto value = text(node);
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}

int integer(const std::string& value) {
    require(!value.empty() && value.size() <= 12, "invalid integer");
    std::size_t used = 0;
    const int result = std::stoi(value, &used);
    require(used == value.size() && std::to_string(result) == value, "noncanonical integer");
    return result;
}

MmsPermission permission(const std::string& name) {
    for (const auto item : {MmsPermission::ListObjects, MmsPermission::ReadValues,
                           MmsPermission::DataSet, MmsPermission::Reporting,
                           MmsPermission::Control, MmsPermission::Config})
        if (name == mms_permission_name(item)) return item;
    throw std::runtime_error("unsupported MMS permission");
}

void parse_document(const std::string& source, pugi::xml_document& doc, bool xacml) {
    require(source.size() <= 1024 * 1024, "policy document too large");
    const auto parsed = doc.load_buffer(source.data(), source.size(),
        pugi::parse_default | pugi::parse_comments | pugi::parse_doctype);
    require(static_cast<bool>(parsed), "invalid XML");
    for (const auto child : doc.children()) require(child.type() != pugi::node_doctype, "DTD prohibited");
    require(elements(doc).size() == 1, "exactly one document root required");
    if (xacml) {
        std::function<void(pugi::xml_node, unsigned)> validate = [&](pugi::xml_node node, unsigned depth) {
            require(depth <= 64, "XML nesting limit exceeded");
            require(namespace_uri(node, node.name()) == ns, "incorrect XACML namespace");
            for (const auto child : node.children())
                if (child.type() == pugi::node_element) validate(child, depth + 1);
        };
        validate(doc.document_element(), 0);
    }
}

// A target is an AND of AnyOf clauses; each AnyOf is an OR of AllOf clauses.
// Retain alternatives while compiling, instead of flattening away their logic.
std::vector<Attributes> target(const pugi::xml_node& node) {
    attributes(node, {});
    std::vector<Attributes> result(1);
    for (const auto any : elements(node)) {
        require(local(any) == "AnyOf", "unsupported Target child");
        attributes(any, {});
        std::vector<Attributes> alternatives;
        for (const auto all : elements(any)) {
            require(local(all) == "AllOf", "unsupported AnyOf child");
            attributes(all, {});
            Attributes values;
            for (const auto match : elements(all)) {
                require(local(match) == "Match", "unsupported AllOf child");
                attributes(match, {"MatchId"});
                const auto children = elements(match);
                require(children.size() == 2 && local(children[0]) == "AttributeValue" &&
                        local(children[1]) == "AttributeDesignator", "invalid Match structure");
                const auto value = children[0], designator = children[1];
                attributes(value, {"DataType"});
                attributes(designator, {"Category", "AttributeId", "DataType", "MustBePresent"});
                require(elements(designator).empty(), "nonempty designator");
                const std::string id = designator.attribute("AttributeId").value();
                const bool numeric = id == role_attr || id == revision_attr;
                require(numeric || id == definition_attr || id == object_attr || id == action_attr,
                        "unsupported IEC attribute identifier");
                require(std::string(designator.attribute("Category").value()) ==
                            (id == object_attr ? resource : subject), "incorrect IEC attribute category");
                const std::string type = numeric ? int_type : str_type;
                require(std::string(value.attribute("DataType").value()) == type &&
                        std::string(designator.attribute("DataType").value()) == type,
                        "incorrect IEC attribute type");
                require(std::string(match.attribute("MatchId").value()) ==
                            std::string("urn:oasis:names:tc:xacml:1.0:function:") +
                            (numeric ? "integer-equal" : "string-equal"), "unsupported Match function");
                const std::string present = designator.attribute("MustBePresent").value();
                require(present == "false" || present == "true", "invalid MustBePresent");
                const auto literal = text(value);
                if (numeric) integer(literal);
                require(values.emplace(id, literal).second, "duplicate target attribute");
            }
            require(!values.empty(), "empty AllOf");
            alternatives.push_back(std::move(values));
        }
        require(!alternatives.empty(), "empty AnyOf");
        std::vector<Attributes> combined;
        for (const auto& left : result) for (const auto& right : alternatives) {
            auto merged = left;
            bool consistent = true;
            for (const auto& item : right) {
                const auto found = merged.find(item.first);
                if (found != merged.end() && found->second != item.second) consistent = false;
                merged[item.first] = item.second;
            }
            if (consistent) combined.push_back(std::move(merged));
            require(combined.size() <= 256, "too many target alternatives");
        }
        result = std::move(combined);
    }
    return result;
}

std::vector<Attributes> intersect(const std::vector<Attributes>& left,
                                   const std::vector<Attributes>& right) {
    std::vector<Attributes> result;
    for (const auto& a : left) for (const auto& b : right) {
        auto merged = a;
        bool consistent = true;
        for (const auto& item : b) {
            const auto found = merged.find(item.first);
            if (found != merged.end() && found->second != item.second) consistent = false;
            merged[item.first] = item.second;
        }
        if (consistent) result.push_back(std::move(merged));
        require(result.size() <= 256, "too many inherited target alternatives");
    }
    return result;
}

} // namespace

bool compile_iec_xacml(const std::string& roles, const std::string& library,
                       const std::string& scopes, std::vector<MmsCompiledGrant>& grants,
                       std::string& policy_id, std::string& revision, std::string& error) {
    grants.clear();
    policy_id.clear();
    revision.clear();
    error.clear();
    try {
        pugi::xml_document role_doc, library_doc, scope_doc;
        parse_document(roles, role_doc, true);
        parse_document(library, library_doc, true);
        parse_document(scopes, scope_doc, false);
        const auto root = role_doc.document_element();
        require(local(root) == "PolicySet" && local(library_doc.document_element()) == "PolicySet",
                "IEC template requires PolicySet roots");
        policy_id = root.attribute("PolicySetId").value();
        revision = root.attribute("Version").value();
        std::map<std::string, pugi::xml_node> sets;
        std::set<std::string> identifiers;
        std::function<void(pugi::xml_node)> index = [&](pugi::xml_node node) {
            const auto kind = local(node);
            require(kind == "PolicySet" || kind == "Policy" || kind == "Rule", "invalid policy node");
            const char* id_attr = kind == "PolicySet" ? "PolicySetId" : kind == "Policy" ? "PolicyId" : "RuleId";
            const std::string id = node.attribute(id_attr).value();
            require(!id.empty() && identifiers.insert(id).second, "missing or duplicate policy identifier");
            if (kind == "Rule") {
                attributes(node, {"RuleId", "Effect"});
                require(std::string(node.attribute("Effect").value()) == "Permit", "IEC template accepts Permit rules only");
            } else {
                require(std::string(node.attribute("Version").value()) == "1.0", "unsupported policy Version");
                if (kind == "PolicySet") {
                    attributes(node, {"PolicySetId", "Version", "PolicyCombiningAlgId"});
                    require(std::string(node.attribute("PolicyCombiningAlgId").value()) ==
                        "urn:oasis:names:tc:xacml:3.0:policy-combining-algorithm:permit-overrides",
                        "IEC template requires PolicySet permit-overrides");
                    sets.emplace(id, node);
                } else {
                    attributes(node, {"PolicyId", "Version", "RuleCombiningAlgId"});
                    require(std::string(node.attribute("RuleCombiningAlgId").value()) ==
                        "urn:oasis:names:tc:xacml:1.0:rule-combining-algorithm:permit-overrides",
                        "IEC template requires Rule permit-overrides");
                }
            }
            unsigned targets = 0, descriptions = 0, bodies = 0;
            for (const auto child : elements(node)) {
                const auto name = local(child);
                if (name == "Description") { require(++descriptions == 1, "duplicate Description"); text(child); }
                else if (name == "Target") { require(++targets == 1, "duplicate Target"); target(child); }
                else if (name == "PolicySetIdReference") {
                    require(kind == "PolicySet", "reference outside PolicySet");
                    attributes(child, {});
                    require(!reference(child).empty(), "empty policy reference"); ++bodies;
                } else if ((kind == "PolicySet" && (name == "PolicySet" || name == "Policy")) ||
                           (kind == "Policy" && name == "Rule")) { index(child); ++bodies; }
                else throw std::runtime_error("unsupported IEC policy construct");
            }
            require(targets == 1 && (kind == "Rule" || bodies > 0), "incomplete policy node");
        };
        index(root);
        index(library_doc.document_element());
        // Validate the complete local reference graph, including unused entries.
        std::set<std::string> active, completed;
        std::function<void(pugi::xml_node, unsigned)> graph = [&](pugi::xml_node node, unsigned depth) {
            require(depth <= 32, "policy reference depth exceeded");
            const std::string id = node.attribute("PolicySetId").value();
            if (!id.empty()) {
                require(active.insert(id).second, "cyclic policy reference");
                if (completed.count(id)) { active.erase(id); return; }
            }
            for (const auto child : elements(node)) {
                if (local(child) == "PolicySetIdReference") {
                    const auto ref = sets.find(reference(child));
                    require(ref != sets.end(), "unresolved PolicySetIdReference");
                    graph(ref->second, depth + 1);
                } else if (local(child) == "PolicySet" || local(child) == "Policy") graph(child, depth + 1);
            }
            if (!id.empty()) { active.erase(id); completed.insert(id); }
        };
        graph(root, 0);
        graph(library_doc.document_element(), 0);
        std::vector<Attributes> semantics;
        std::size_t visits = 0;
        std::function<void(pugi::xml_node, const std::vector<Attributes>&)> compile =
            [&](pugi::xml_node node, const std::vector<Attributes>& inherited) {
                require(++visits <= 8192, "policy expansion limit exceeded");
                pugi::xml_node own_target;
                for (const auto child : elements(node)) if (local(child) == "Target") own_target = child;
                const auto context = intersect(inherited, target(own_target));
                if (local(node) == "Rule") {
                    for (const auto& item : context) {
                        require(item.size() == 5 && item.count(role_attr) && item.count(definition_attr) &&
                                item.count(revision_attr) && item.count(object_attr) && item.count(action_attr),
                                "incomplete role/permission path");
                        require(item.at(object_attr) == "IEC61850/MMS", "unsupported resource ObjectID");
                        permission(item.at(action_attr));
                        semantics.push_back(item);
                        require(semantics.size() <= 4096, "compiled permission limit exceeded");
                    }
                } else for (const auto child : elements(node)) {
                    const auto kind = local(child);
                    if (kind == "PolicySetIdReference") compile(sets.at(reference(child)), context);
                    else if (kind == "PolicySet" || kind == "Policy" || kind == "Rule") compile(child, context);
                }
            };
        compile(root, std::vector<Attributes>(1));
        require(!semantics.empty(), "no role permissions");
        const auto scope_root = scope_doc.document_element();
        require(std::string(scope_root.name()) == "CommissioningScopes", "invalid commissioning scopes root");
        attributes(scope_root, {"Version"});
        require(std::string(scope_root.attribute("Version").value()) == "1.0", "invalid scope version");
        std::set<std::string> bindings;
        for (const auto binding : elements(scope_root)) {
            require(std::string(binding.name()) == "Binding", "invalid scope binding");
            attributes(binding, {"RoleId", "AoR", "RoleDefinition", "Revision"});
            MmsProfileARole role;
            role.role_id = integer(binding.attribute("RoleId").value());
            const int rev = integer(binding.attribute("Revision").value());
            require(role.role_id >= -2 && role.role_id <= 6 && rev >= 0 && rev <= 255, "invalid scope role");
            role.revision = static_cast<std::uint8_t>(rev);
            role.aor = binding.attribute("AoR").value();
            role.role_definition = binding.attribute("RoleDefinition").value();
            require(!role.aor.empty() && unicode_length(role.aor) <= 64 && unicode_length(role.role_definition) <= 23,
                    "invalid scope identity");
            const std::string key = std::to_string(role.role_id) + "|" + std::to_string(rev) + "|" + role.aor + "|" + role.role_definition;
            require(bindings.insert(key).second, "duplicate scope binding");
            std::set<std::string> permissions;
            for (const auto scope : elements(binding)) {
                require(std::string(scope.name()) == "Scope", "invalid scope entry");
                attributes(scope, {"Permission"});
                const std::string action = scope.attribute("Permission").value();
                require(permissions.insert(action).second, "duplicate scope permission");
                MmsCompiledGrant grant{role, permission(action), {}};
                require(std::any_of(semantics.begin(), semantics.end(), [&](const Attributes& item) {
                    return integer(item.at(role_attr)) == role.role_id && integer(item.at(revision_attr)) == rev &&
                        item.at(definition_attr) == role.role_definition && item.at(action_attr) == action;
                }), "commissioning scope exceeds role permission");
                for (const auto object : elements(scope)) {
                    require(std::string(object.name()) == "Object", "invalid scope object");
                    attributes(object, {});
                    const auto value = text(object);
                    require(!value.empty() && value.size() <= 256 && (role.role_id >= 0 || value != "*"),
                            "invalid or overly broad scope object");
                    grant.objects.push_back(value);
                }
                require(!grant.objects.empty(), "empty object scope");
                grants.push_back(std::move(grant));
            }
        }
        require(!grants.empty(), "no commissioned grants");
        return true;
    } catch (const std::exception& exception) {
        grants.clear(); policy_id.clear(); revision.clear(); error = exception.what();
        return false;
    }
}

} // namespace cci::adapters
