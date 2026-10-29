// On-demand check of a real issued PKI bundle through the product verifier.
// No keys, device connections or TLS configuration changes are required.
#include "mms_rbac_security.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace cci::adapters;
namespace fs = std::filesystem;
namespace {
void check(bool value, const std::string& message) {
    if (!value) throw std::runtime_error(message);
}
std::string read(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    check(static_cast<bool>(file), "cannot read public certificate " + path.string());
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
}

int main(int argc, char** argv) {
    try {
        check(argc == 3, "usage: mms_rbac_commissioned_pki_check BUNDLE NEW_STATE_PATH");
        const fs::path root(argv[1]), state(argv[2]);
        check(!fs::exists(state), "use a fresh isolated activation state path");
        MmsRbacSecurity security;
        MmsCommissioningPolicy policy;
        std::string error;
        const bool loaded = security.load((root / "verification-only/commissioning-policy.xml").string(),
                            (root / "owner-signer/owner-signer.pem").string(), state.string(),
                            (root / "rbac_ca.pem").string(), (root / "rbac_crl.pem").string(),
                            policy, error);
        check(loaded, "signed commissioning load failed: " + error);
        const auto authenticate = [&](const std::string& name, MmsRbacIdentity& identity) {
            const auto der = read(root / name / (name + ".der"));
            return security.authenticate(reinterpret_cast<const std::uint8_t*>(der.data()),
                                         der.size(), policy, identity, error);
        };
        const char* standard[] = {"viewer", "operator", "engineer", "installer", "secadm", "secaud", "rbacmnt"};
        unsigned positive = 0;
        for (int revision : {0, 1}) {
            for (int rid = 0; rid < 7; ++rid) {
                MmsRbacIdentity identity;
                const auto name = std::string(standard[rid]) + "-r" + std::to_string(revision);
                const bool authenticated = authenticate(name, identity);
                check(authenticated, name + " authentication failed: " + error);
                check(identity.actor == "OWNER" && identity.roles.size() == 1 &&
                      identity.roles[0].role_id == rid && identity.roles[0].revision == revision,
                      name + " role/revision/actor mismatch");
                check(security.valid_now(identity, error), name + " live validation failed: " + error);
                if (rid == 0) {
                    check(policy.permits_roles(identity.roles, MmsPermission::ReadValues, "LD_Plant/PdCMMXU1.TotW"),
                          name + " cannot read permitted measurement");
                    check(!policy.permits_roles(identity.roles, MmsPermission::Control, "LD_Plant/WlimDWMX1.Mod"),
                          name + " unexpectedly permits control");
                }
                ++positive;
            }
        }
        for (const auto* name : {"dso", "aggregator", "multi-viewer-engineer"}) {
            MmsRbacIdentity identity;
            const bool authenticated = authenticate(name, identity);
            check(authenticated, std::string(name) + " authentication failed: " + error);
            if (std::string(name) == "dso") {
                check(identity.actor == "DSO" && identity.roles.size() == 1 && identity.roles[0].role_id == -1,
                      "DSO binding mismatch");
                check(policy.permits_roles(identity.roles, MmsPermission::Control, "LD_Plant/WlimDWMX1.Mod"),
                      "DSO permitted control is missing");
                // CONFIG covers MMS writes: the agreed CEI RW objects permit
                // that path too. Measurement-only objects must remain RO.
                check(policy.permits_roles(identity.roles, MmsPermission::Config, "LD_Plant/WlimDWMX1.Mod"),
                      "DSO permitted RW configuration is missing");
                check(!policy.permits_roles(identity.roles, MmsPermission::Config, "LD_Plant/PdCMMXU1.TotW"),
                      "DSO unexpectedly permits writing a read-only measurement");
            } else if (std::string(name) == "aggregator") {
                check(identity.actor == "AGGREGATOR" && identity.roles.size() == 1 && identity.roles[0].role_id == -2,
                      "Aggregator binding mismatch");
            } else {
                check(identity.actor == "OWNER" && identity.roles.size() == 2 &&
                      identity.roles[0].role_id == 0 && identity.roles[1].role_id == 2,
                      "multi-role binding mismatch");
            }
            ++positive;
        }
        unsigned negative = 0;
        for (const auto* name : {"negative-unknown-role", "negative-wrong-aor", "negative-wrong-definition",
                                 "negative-wrong-revision", "negative-revoked", "negative-no-roles", "owner-signer"}) {
            MmsRbacIdentity identity;
            check(!authenticate(name, identity), std::string(name) + " was unexpectedly accepted");
            check(!error.empty(), std::string(name) + " has no rejection reason");
            std::cout << "Rejected " << name << ": " << error << '\n';
            ++negative;
        }
        std::cout << "PASS: product verifier accepted " << positive << " positive identities and rejected "
                  << negative << " negative/non-user identities; signed package, CA/CRL and access checks passed\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "FAIL: " << exception.what() << '\n';
        return 1;
    }
}
