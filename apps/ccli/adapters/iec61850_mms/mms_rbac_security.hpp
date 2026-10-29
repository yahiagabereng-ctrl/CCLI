#pragma once

#include "mms_rbac.hpp"
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace cci::adapters {

struct MmsRbacIdentity {
    std::string fingerprint;
    std::string transport_fingerprint;
    std::string public_key_fingerprint;
    bool has_signature_proof{false};
    std::string actor;
    std::string subject;
    std::string issuer;
    std::string serial;
    std::string peer_address;
    std::vector<std::uint8_t> certificate;
    std::vector<MmsProfileARole> roles;
};

/** Additional RBAC verification. Never configures TLS or changes its trust. */
class MmsRbacSecurity {
public:
    bool load(const std::string& policy_path, const std::string& owner_certificate,
              const std::string& activation_state, const std::string& identity_ca,
              const std::string& identity_crl, MmsCommissioningPolicy& policy,
              std::string& error);
    bool authenticate(const std::uint8_t* auth, std::size_t length,
                      const MmsCommissioningPolicy& policy, MmsRbacIdentity& identity,
                      std::string& error) const;
    bool valid_now(const MmsRbacIdentity& identity, std::string& error) const;
    static bool matches_application_reference(const MmsRbacIdentity& identity,
                                             const std::string& oid, std::string& error);
    std::uint64_t sequence() const { return sequence_; }
    const std::string& owner_fingerprint() const { return owner_fingerprint_; }
    static std::string fingerprint(const std::uint8_t* data, std::size_t size);
    static std::string public_key_fingerprint(const std::uint8_t* data, std::size_t size);

private:
    struct Binding { std::string transport; std::string actor; std::vector<int> roles; };
    std::map<std::string, Binding> bindings_;
    std::string ca_path_, crl_path_;
    std::uint64_t sequence_{0};
    std::string owner_fingerprint_;
    mutable std::mutex proof_mu_;
    mutable std::map<std::string, std::int64_t> used_proofs_;
};

/** One-line JSON fields for the existing append-only security event store. */
std::string mms_rbac_audit(const std::string& event, const std::string& severity,
                           const MmsRbacIdentity* identity, const std::string& policy_id,
                           std::uint64_t sequence, const std::string& service,
                           const std::string& object, const std::string& reason);

} // namespace cci::adapters
