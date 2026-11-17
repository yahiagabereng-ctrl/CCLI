#include "mms_rbac_security.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace cci::adapters;
namespace {
void check(bool value, const std::string& message) { if (!value) throw std::runtime_error(message); }
std::string read(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    check(static_cast<bool>(file), "cannot read fixture: " + path.string());
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
void copy_fixture(const std::filesystem::path& from, const std::filesystem::path& to) {
    std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
}
}

int main(int argc, char** argv) {
    try {
        check(argc == 2, "fixture directory argument required");
        const std::filesystem::path base(argv[1]);
        const auto root = base / read(base / "fixture-directory.txt");
        const auto state = root / "activation.state";
        const auto ca = root / "ca.pem";
        const auto owner = root / "owner.pem";
        const auto crl = root / "active.crl.pem";
        MmsRbacSecurity security;
        MmsCommissioningPolicy policy;
        std::string error;
        const auto load = [&](const char* package) {
            return security.load((root / package / "commissioning-policy.xml").string(),
                                  owner.string(), state.string(), ca.string(), crl.string(), policy, error);
        };
        check(load("package1"), "signed package activation: " + error);
        check(security.sequence() == 1 && !policy.empty(), "initial policy sequence");
        const auto authenticate = [&](const char* name, MmsRbacIdentity& identity) {
            const auto bytes = read(root / name);
            return security.authenticate(reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size(), policy, identity, error);
        };
        MmsRbacIdentity identity;
        check(authenticate("dso.der", identity), "valid identity: " + error);
        check(identity.actor == "DSO" && identity.roles.size() == 1 && identity.roles[0].role_id == -1,
              "certificate identity/role binding");
        check(!identity.subject.empty() && !identity.issuer.empty() && !identity.serial.empty(), "audit identity metadata");
        const auto transport = read(root / "transport.der");
        check(identity.transport_fingerprint == MmsRbacSecurity::fingerprint(
            reinterpret_cast<const std::uint8_t*>(transport.data()), transport.size()), "transport certificate pin");
        check(identity.public_key_fingerprint == MmsRbacSecurity::public_key_fingerprint(
            reinterpret_cast<const std::uint8_t*>(transport.data()), transport.size()), "separate certificates share authenticated key");
        const auto good_identity = identity;
        check(!authenticate("dso_admin.der", identity), "DSO cannot assume SECADM from a trusted CA");
        check(!authenticate("wrong_issuer.der", identity), "wrong issuer rejected despite authorized pin");
        check(!authenticate("expired.der", identity), "expired identity rejected");
        check(!authenticate("revoked.der", identity), "revoked identity rejected");
        check(!authenticate("unconfigured.der", identity), "unconfigured certificate rejected");
        check(!authenticate("admin_operation.der", identity), "administrative token cannot authorize a user session");
        check(authenticate("owner_rev0.der", identity), "standard revision 0 accepted: " + error);
        check(identity.roles[0].revision == 0, "standard revision 0 remains distinct");
        check(authenticate("signed_auth.bin", identity) && identity.has_signature_proof,
              "fresh signed MMS proof accepted: " + error);
        check(!authenticate("signed_auth.bin", identity), "signed authentication replay rejected");
        check(!authenticate("rewrapped_auth.bin", identity), "BER re-encoding cannot bypass proof replay rejection");
        check(!authenticate("bad_auth.bin", identity), "bad MMS proof rejected");
        check(!authenticate("stale_auth.bin", identity), "stale MMS proof rejected");
        copy_fixture(root / "revoked_dso.crl.pem", crl);
        check(!security.valid_now(good_identity, error), "active role session revalidates a new CRL");
        copy_fixture(root / "base.crl.pem", crl);
        check(security.valid_now(good_identity, error), "valid role certificate remains usable");
        check(load("package2") && security.sequence() == 2, "new owner-approved policy activates: " + error);
        const auto checkpoint = read(state);
        check(load("package2"), "same package restart is permitted");
        for (const char* invalid : {"package1", "tampered", "wrong_owner", "same_sequence_changed", "bad_actor", "bad_policy"}) {
            check(!load(invalid), std::string("invalid package accepted: ") + invalid);
            check(policy.empty(), "failed activation must clear policy grants");
            check(read(state) == checkpoint, "failed activation must preserve rollback checkpoint");
        }
        check(load("package2"), "previous valid package can restart after failed candidate");
        const auto event = mms_rbac_audit("MMS_ACCESS_DENIED", "alarm", &good_identity,
                                         policy.policy_id(), security.sequence(), "CONTROL", "LD_Plant/WlimDWMX1.Mod", "test \"escape\"\n");
        check(event.find("\"rbac\":true") != std::string::npos && event.find("\\\"escape\\\"\\n") != std::string::npos &&
              event.find("\"serial\":") != std::string::npos && event.find("\"roles\":[") != std::string::npos,
              "structured audit metadata and escaping");
        std::cout << "mms_rbac_security_test: PASS (signed commissioning, actor binding, certificates, CRL, revisions, proof replay)\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "mms_rbac_security_test: FAIL: " << exception.what() << '\n';
        return 1;
    }
}
