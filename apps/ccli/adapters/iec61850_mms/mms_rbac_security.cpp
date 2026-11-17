#include "mms_rbac_security.hpp"
#include "../../third_party/pugixml/pugixml.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <set>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <cstring>
#if defined(CONFIG_MMS_SUPPORT_TLS)
#include "mbedtls/pk.h"
#include "mbedtls/sha256.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_crl.h"
#include "mbedtls/version.h"
#endif
#if !defined(_WIN32)
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace cci::adapters {
namespace {
void require(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
void exact_attributes(const pugi::xml_node& node, std::initializer_list<const char*> names) {
    for (const auto name : names) require(static_cast<bool>(node.attribute(name)), "missing security configuration attribute");
    for (const auto attribute : node.attributes())
        require(std::any_of(names.begin(), names.end(), [&](const char* name) { return std::string(attribute.name()) == name; }),
                "unsupported security configuration attribute");
}
void single_root(const pugi::xml_document& document) {
    unsigned roots = 0;
    for (const auto child : document.children()) {
        require(child.type() != pugi::node_doctype, "security DTD prohibited");
        if (child.type() == pugi::node_element) ++roots;
    }
    require(roots == 1, "security document must have one root");
}
std::string read(const std::filesystem::path& path, std::size_t limit = 1024 * 1024) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    require(in && in.tellg() >= 0 && in.tellg() <= static_cast<std::streamoff>(limit), "cannot read bounded security file");
    in.seekg(0);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}
std::uint64_t number(const std::string& value) {
    require(!value.empty() && value.size() <= 20 && value.find_first_not_of("0123456789") == std::string::npos,
            "invalid commissioning sequence");
    const auto result = std::stoull(value);
    require(result > 0 && std::to_string(result) == value, "noncanonical commissioning sequence");
    return result;
}
bool hex_pin(const std::string& value) {
    return value.size() == 64 && value.find_first_not_of("0123456789abcdef") == std::string::npos;
}
std::vector<std::uint8_t> unhex(const std::string& value) {
    require(value.size() % 2 == 0 && value.find_first_not_of("0123456789abcdef") == std::string::npos,
            "invalid signature hex");
    std::vector<std::uint8_t> result;
    for (std::size_t i = 0; i < value.size(); i += 2)
        result.push_back(static_cast<std::uint8_t>(std::stoul(value.substr(i, 2), nullptr, 16)));
    return result;
}
std::string escape(const std::string& value) {
    std::ostringstream out;
    for (const unsigned char c : value) {
        if (c == '"' || c == '\\') out << '\\' << c;
        else if (c == '\n') out << "\\n";
        else if (c == '\r') out << "\\r";
        else if (c == '\t') out << "\\t";
        else if (c < 32) out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned>(c);
        else out << c;
    }
    return out.str();
}
#if defined(CONFIG_MMS_SUPPORT_TLS)
struct Certificate {
    mbedtls_x509_crt value;
    Certificate() { mbedtls_x509_crt_init(&value); }
    ~Certificate() { mbedtls_x509_crt_free(&value); }
};
struct Crl {
    mbedtls_x509_crl value;
    Crl() { mbedtls_x509_crl_init(&value); }
    ~Crl() { mbedtls_x509_crl_free(&value); }
};

std::string key_pin(mbedtls_pk_context* key) {
    unsigned char encoded[2048];
    const int size = mbedtls_pk_write_pubkey_der(key, encoded, sizeof(encoded));
    require(size > 0, "cannot encode identity public key");
    return MmsRbacSecurity::fingerprint(encoded + sizeof(encoded) - size, static_cast<std::size_t>(size));
}

struct AuthTlv { unsigned tag; const std::uint8_t* data; std::size_t size; };
bool auth_tlv(const std::uint8_t* bytes, std::size_t length, std::size_t& pos, AuthTlv& result) {
    if (pos + 2 > length) return false;
    result.tag = bytes[pos++];
    std::size_t size = bytes[pos++];
    if (size & 0x80) {
        const auto count = size & 0x7f;
        if (!count || count > 3 || pos + count > length) return false;
        size = 0;
        for (std::size_t i = 0; i < count; ++i) size = size * 256 + bytes[pos++];
    }
    if (size > length - pos) return false;
    result.data = bytes + pos; result.size = size; pos += size;
    return true;
}

bool verify_auth_proof(const std::uint8_t* bytes, std::size_t length, const MmsRbacIdentity& identity,
                       std::string& proof_pin) {
    for (std::size_t offset = 0; offset + 2 < length; ++offset) {
        if (bytes[offset] != 0xa0) continue;
        std::size_t pos = offset;
        AuthTlv outer{}, cert{}, stamp{}, signature{};
        if (!auth_tlv(bytes, length, pos, outer)) continue;
        pos = 0;
        if (!auth_tlv(outer.data, outer.size, pos, cert) || cert.tag != 0x80 ||
            cert.size != identity.certificate.size() ||
            !std::equal(identity.certificate.begin(), identity.certificate.end(), cert.data)) continue;
        require(auth_tlv(outer.data, outer.size, pos, stamp) && stamp.tag == 0x81 &&
                auth_tlv(outer.data, outer.size, pos, signature) && signature.tag == 0x82 && pos == outer.size,
                "invalid certificate-based authentication proof");
        const std::string time(reinterpret_cast<const char*>(stamp.data), stamp.size);
        require(time.size() >= 14 && time.size() <= 32 && time.substr(0, 14).find_first_not_of("0123456789") == std::string::npos,
                "invalid signed authentication time");
        auto tail = time.substr(14);
        if (!tail.empty() && tail.back() == 'Z') tail.pop_back();
        if (!tail.empty()) {
            require(tail.front() == '.' && tail.substr(1).find_first_not_of("0123456789") == std::string::npos,
                    "unsupported signed authentication time format");
        }
        std::tm utc{};
        utc.tm_year = std::stoi(time.substr(0, 4)) - 1900;
        utc.tm_mon = std::stoi(time.substr(4, 2)) - 1;
        utc.tm_mday = std::stoi(time.substr(6, 2));
        utc.tm_hour = std::stoi(time.substr(8, 2));
        utc.tm_min = std::stoi(time.substr(10, 2));
        utc.tm_sec = std::stoi(time.substr(12, 2));
        require(utc.tm_mon >= 0 && utc.tm_mon <= 11 && utc.tm_mday >= 1 && utc.tm_mday <= 31 &&
                utc.tm_hour <= 23 && utc.tm_min <= 59 && utc.tm_sec <= 59, "invalid signed calendar time");
        const auto original = utc;
#if defined(_WIN32)
        const auto signed_time = _mkgmtime(&utc);
#else
        const auto signed_time = timegm(&utc);
#endif
        require(utc.tm_year == original.tm_year && utc.tm_mon == original.tm_mon &&
                utc.tm_mday == original.tm_mday && utc.tm_hour == original.tm_hour &&
                utc.tm_min == original.tm_min && utc.tm_sec == original.tm_sec,
                "invalid signed calendar date");
        require(signed_time > 0 && std::difftime(std::time(nullptr), signed_time) <= 120 &&
                std::difftime(signed_time, std::time(nullptr)) <= 120, "stale or future authentication proof");
        Certificate leaf;
        require(mbedtls_x509_crt_parse_der(&leaf.value, identity.certificate.data(), identity.certificate.size()) == 0,
                "invalid proof certificate");
        unsigned char digest[32];
        require(mbedtls_sha256(stamp.data, stamp.size, digest, 0) == 0 &&
                mbedtls_pk_verify(&leaf.value.pk, MBEDTLS_MD_SHA256, digest, sizeof(digest), signature.data, signature.size) == 0,
                "MMS authentication signature invalid");
        // Replay identity follows the signed content, independent of BER wrapper
        // encoding or an alternative signature encoding of the same timestamp.
        proof_pin = identity.fingerprint + ":" + MmsRbacSecurity::fingerprint(stamp.data, stamp.size);
        return true;
    }
    return false;
}
void verify_certificate(const std::vector<std::uint8_t>& der, const std::string& ca_path,
                         const std::string& crl_path, MmsRbacIdentity* metadata) {
    Certificate leaf, ca;
    Crl crl;
    require(mbedtls_x509_crt_parse_der(&leaf.value, der.data(), der.size()) == 0, "invalid identity certificate");
#if MBEDTLS_VERSION_NUMBER >= 0x03060000
    require(!mbedtls_x509_crt_get_ca_istrue(&leaf.value), "CA certificate cannot assert session roles");
#elif MBEDTLS_VERSION_NUMBER >= 0x03000000
    require(!leaf.value.MBEDTLS_PRIVATE(ca_istrue), "CA certificate cannot assert session roles");
#else
    require(!leaf.value.ca_istrue, "CA certificate cannot assert session roles");
#endif
    require(mbedtls_x509_crt_check_key_usage(&leaf.value, MBEDTLS_X509_KU_DIGITAL_SIGNATURE) == 0,
            "identity certificate lacks digitalSignature usage");
    require(mbedtls_x509_crt_parse_file(&ca.value, ca_path.c_str()) == 0, "cannot load identity CA");
    if (!crl_path.empty()) {
        require(mbedtls_x509_crl_parse_file(&crl.value, crl_path.c_str()) == 0, "cannot load identity CRL");
        bool covered = false;
        for (auto* item = &crl.value; item; item = item->next)
            if (item->issuer_raw.len == leaf.value.issuer_raw.len &&
                std::equal(item->issuer_raw.p, item->issuer_raw.p + item->issuer_raw.len, leaf.value.issuer_raw.p)) covered = true;
        require(covered, "identity issuer has no configured CRL coverage");
    }
    std::uint32_t flags = 0;
    require(mbedtls_x509_crt_verify(&leaf.value, &ca.value, crl_path.empty() ? nullptr : &crl.value,
                                  nullptr, &flags, nullptr, nullptr) == 0 && flags == 0,
            "identity chain, lifetime or revocation verification failed");
    if (metadata) {
        char buffer[1024];
        require(mbedtls_x509_dn_gets(buffer, sizeof(buffer), &leaf.value.subject) >= 0, "subject name too long");
        metadata->subject = buffer;
        require(mbedtls_x509_dn_gets(buffer, sizeof(buffer), &leaf.value.issuer) >= 0, "issuer name too long");
        metadata->issuer = buffer;
        require(mbedtls_x509_serial_gets(buffer, sizeof(buffer), &leaf.value.serial) >= 0, "serial too long");
        metadata->serial = buffer;
        metadata->public_key_fingerprint = key_pin(&leaf.value.pk);
    }
}

void persist(const std::filesystem::path& path, const std::string& value) {
#if !defined(_WIN32)
    require(std::filesystem::is_directory(path.parent_path()), "activation state directory must be provisioned");
    const std::string temporary = path.string() + ".new";
    const int fd = ::open(temporary.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    require(fd >= 0, "cannot create activation state transaction");
    bool success = true;
    std::size_t written = 0;
    while (written < value.size()) {
        const auto count = ::write(fd, value.data() + written, value.size() - written);
        if (count <= 0) { success = false; break; }
        written += static_cast<std::size_t>(count);
    }
    if (::fsync(fd) != 0) success = false;
    if (::close(fd) != 0) success = false;
    if (!success || ::rename(temporary.c_str(), path.c_str()) != 0) {
        ::unlink(temporary.c_str());
        throw std::runtime_error("activation state commit failed");
    }
    const int directory = ::open(path.parent_path().c_str(), O_RDONLY | O_DIRECTORY);
    require(directory >= 0, "cannot open activation state directory");
    const int synced = ::fsync(directory);
    ::close(directory);
    require(synced == 0, "activation directory fsync failed");
#else
    (void) path; (void) value;
    throw std::runtime_error("production activation requires POSIX durable storage");
#endif
}
#endif
} // namespace

std::string MmsRbacSecurity::fingerprint(const std::uint8_t* data, std::size_t size) {
#if defined(CONFIG_MMS_SUPPORT_TLS)
    unsigned char digest[32];
    if (!data || !size || mbedtls_sha256(data, size, digest, 0) != 0) return {};
    std::ostringstream out;
    for (const auto byte : digest) out << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(byte);
    return out.str();
#else
    (void) data; (void) size; return {};
#endif
}

std::string MmsRbacSecurity::public_key_fingerprint(const std::uint8_t* data, std::size_t size) {
#if defined(CONFIG_MMS_SUPPORT_TLS)
    try {
        Certificate certificate;
        require(data && size && mbedtls_x509_crt_parse_der(&certificate.value, data, size) == 0,
                "invalid TLS peer certificate");
        return key_pin(&certificate.value.pk);
    } catch (...) { return {}; }
#else
    (void) data; (void) size; return {};
#endif
}

bool MmsRbacSecurity::load(const std::string& path, const std::string& owner_path,
                           const std::string& state_path, const std::string& ca,
                           const std::string& crl, MmsCommissioningPolicy& policy,
                           std::string& error) {
    bindings_.clear(); sequence_ = 0; owner_fingerprint_.clear(); error.clear();
    policy = MmsCommissioningPolicy{};
#if defined(CONFIG_MMS_SUPPORT_TLS)
    try {
        require(!owner_path.empty() && !state_path.empty() && !ca.empty() && !crl.empty(),
                "owner pin, activation state, identity CA and identity CRL required");
        const auto directory = std::filesystem::path(path).parent_path();
        pugi::xml_document manifest;
        const auto manifest_xml = read(directory / "commissioning-manifest.xml", 16384);
        require(static_cast<bool>(manifest.load_string(manifest_xml.c_str(), pugi::parse_default | pugi::parse_doctype)), "invalid signed manifest XML");
        single_root(manifest);
        const auto root = manifest.document_element();
        exact_attributes(root, {"Version", "Sequence", "Signature"});
        require(std::string(root.name()) == "CommissioningManifest" && std::string(root.attribute("Version").value()) == "1.0",
                "invalid signed manifest root");
        const auto sequence = number(root.attribute("Sequence").value());
        const std::string names[] = {"commissioning-policy.xml", "permission-library.xml", "commissioning-scopes.xml", "identity-bindings.xml"};
        require(std::filesystem::path(path).filename() == names[0], "signed package requires canonical role filename");
        std::string payload = "CCLI-RBAC-PACKAGE-1\n" + std::to_string(sequence) + "\n";
        std::string documents[4];
        std::set<std::string> found;
        for (const auto entry : root.children()) {
            if (entry.type() != pugi::node_element) continue;
            require(std::string(entry.name()) == "File", "unsupported manifest entry");
            exact_attributes(entry, {"Name", "Sha256"});
            require(!entry.first_child(), "manifest File must be empty");
            const std::string name = entry.attribute("Name").value();
            require(std::find(std::begin(names), std::end(names), name) != std::end(names) && found.insert(name).second,
                    "invalid or duplicate signed package filename");
        }
        require(found.size() == 4, "signed manifest must cover all four files");
        for (unsigned i = 0; i < 4; ++i) {
            documents[i] = read(directory / names[i]);
            const auto digest = fingerprint(reinterpret_cast<const std::uint8_t*>(documents[i].data()), documents[i].size());
            std::string expected;
            for (const auto entry : root.children("File")) if (names[i] == entry.attribute("Name").value()) expected = entry.attribute("Sha256").value();
            require(hex_pin(expected) && digest == expected, "signed package digest mismatch");
            payload += names[i] + " " + digest + "\n";
        }
        const auto signature = unhex(root.attribute("Signature").value());
        Certificate owner;
        require(mbedtls_x509_crt_parse_file(&owner.value, owner_path.c_str()) == 0, "cannot read pinned owner certificate");
        require(!mbedtls_x509_time_is_past(&owner.value.valid_to) && !mbedtls_x509_time_is_future(&owner.value.valid_from),
                "owner certificate outside validity period");
        require(mbedtls_x509_crt_check_key_usage(&owner.value, MBEDTLS_X509_KU_DIGITAL_SIGNATURE) == 0,
                "owner certificate lacks signing usage");
        require((mbedtls_pk_can_do(&owner.value.pk, MBEDTLS_PK_RSA) && mbedtls_pk_get_bitlen(&owner.value.pk) >= 2048) ||
                (mbedtls_pk_can_do(&owner.value.pk, MBEDTLS_PK_ECDSA) && mbedtls_pk_get_bitlen(&owner.value.pk) >= 256),
                "unsupported or weak owner signing key");
        unsigned char digest[32];
        require(mbedtls_sha256(reinterpret_cast<const unsigned char*>(payload.data()), payload.size(), digest, 0) == 0 &&
                mbedtls_pk_verify(&owner.value.pk, MBEDTLS_MD_SHA256, digest, sizeof(digest), signature.data(), signature.size()) == 0,
                "owner commissioning signature invalid");
        const auto package_hash = fingerprint(reinterpret_cast<const std::uint8_t*>(payload.data()), payload.size());
        const std::filesystem::path state(state_path);
        std::string previous;
        if (std::filesystem::exists(state)) {
            previous = read(state, 1024);
            const auto separator = previous.find(' ');
            require(separator != std::string::npos, "invalid activation state");
            const auto last = number(previous.substr(0, separator));
            require(sequence >= last, "commissioning rollback rejected");
            require(sequence != last || previous == std::to_string(sequence) + " " + package_hash + "\n",
                    "same sequence cannot activate different content");
        }
        MmsCommissioningPolicy candidate;
        require(candidate.load_xml(documents[0], documents[1], documents[2], error), error.empty() ? "policy compilation failed" : error.c_str());
        pugi::xml_document identities;
        require(static_cast<bool>(identities.load_string(documents[3].c_str(), pugi::parse_default | pugi::parse_doctype)), "invalid identity binding XML");
        single_root(identities);
        const auto identity_root = identities.document_element();
        exact_attributes(identity_root, {"Version"});
        require(std::string(identity_root.name()) == "IdentityBindings" && std::string(identity_root.attribute("Version").value()) == "1.0",
                "invalid identity binding root");
        for (const auto entry : identity_root.children()) {
            if (entry.type() != pugi::node_element) continue;
            require(std::string(entry.name()) == "Identity", "unsupported identity binding");
            exact_attributes(entry, {"Sha256", "TransportSha256", "Actor", "Roles"});
            require(!entry.first_child(), "Identity binding must be empty");
            const std::string identity = entry.attribute("Sha256").value();
            Binding binding{entry.attribute("TransportSha256").value(), entry.attribute("Actor").value(), {}};
            require(hex_pin(identity) && hex_pin(binding.transport), "invalid identity/transport fingerprint");
            require(binding.actor == "DSO" || binding.actor == "AGGREGATOR" || binding.actor == "OWNER", "invalid identity actor");
            std::string roles = entry.attribute("Roles").value();
            require(!roles.empty() && roles.back() != ',' && roles.size() <= 64, "invalid allowed roles list");
            std::istringstream list(roles);
            std::string item;
            while (std::getline(list, item, ',')) {
                std::size_t used = 0;
                const int role = std::stoi(item, &used);
                require(used == item.size() && std::to_string(role) == item && role >= -2 && role <= 6, "invalid allowed role");
                require((binding.actor == "OWNER" && role >= 0) || (binding.actor == "DSO" && (role == -1 || role == 0)) ||
                        (binding.actor == "AGGREGATOR" && (role == -2 || role == 0)), "actor cannot assume configured role");
                require(std::find(binding.roles.begin(), binding.roles.end(), role) == binding.roles.end(), "duplicate allowed role");
                binding.roles.push_back(role);
            }
            require(!binding.roles.empty() && bindings_.emplace(identity, std::move(binding)).second, "empty or duplicate identity binding");
            require(bindings_.size() <= 1024, "identity binding count limit exceeded");
        }
        require(!bindings_.empty(), "no role identities commissioned");
        // Read and validate trust material before committing activation state.
        Certificate trust;
        require(mbedtls_x509_crt_parse_file(&trust.value, ca.c_str()) == 0, "identity CA unavailable");
        if (!crl.empty()) { Crl test; require(mbedtls_x509_crl_parse_file(&test.value, crl.c_str()) == 0, "identity CRL unavailable"); }
        const std::string next = std::to_string(sequence) + " " + package_hash + "\n";
        if (previous != next) persist(state, next);
        ca_path_ = ca; crl_path_ = crl; sequence_ = sequence;
        owner_fingerprint_ = fingerprint(owner.value.raw.p, owner.value.raw.len);
        policy = std::move(candidate);
        return true;
    } catch (const std::exception& exception) { error = exception.what(); bindings_.clear(); return false; }
#else
    (void) path; (void) owner_path; (void) state_path; (void) ca; (void) crl;
    error = "RBAC cryptographic verification unavailable"; return false;
#endif
}

bool MmsRbacSecurity::authenticate(const std::uint8_t* auth, std::size_t length,
                                   const MmsCommissioningPolicy& policy,
                                   MmsRbacIdentity& identity, std::string& error) const {
    identity = {}; error.clear();
#if defined(CONFIG_MMS_SUPPORT_TLS)
    try {
        require(auth && length > 0 && length <= 65536, "invalid authentication payload");
        // Preserve the established TSP embedded-certificate convention, but
        // select only an exact owner-authorized certificate fingerprint.
        unsigned matches = 0;
        for (std::size_t start = 0; start + 2 < length; ++start) {
            if (auth[start] != 0x30) continue;
            std::size_t offset = start + 2, size = auth[start + 1];
            if (size & 0x80) {
                const auto octets = size & 0x7f;
                if (!octets || octets > 3 || offset + octets > length) continue;
                size = 0;
                for (std::size_t i = 0; i < octets; ++i) size = size * 256 + auth[offset++];
            }
            if (size > length - offset) continue;
            const auto total = offset + size - start;
            const auto pin = fingerprint(auth + start, total);
            const auto binding = bindings_.find(pin);
            if (binding == bindings_.end()) continue;
            require(++matches == 1, "ambiguous identity certificate payload");
            identity.certificate.assign(auth + start, auth + start + total);
            identity.fingerprint = pin;
            identity.transport_fingerprint = binding->second.transport;
            identity.actor = binding->second.actor;
            verify_certificate(identity.certificate, ca_path_, crl_path_, &identity);
            require(decode_profile_a_certificate(identity.certificate.data(), identity.certificate.size(), identity.roles) == MmsProfileAParseResult::Ok,
                    "invalid or missing Profile A role extension");
            for (const auto& role : identity.roles) {
                require(!role.has_operation && !role.has_sequence,
                        "administrative update token cannot authorize an MMS user session");
                require(std::find(binding->second.roles.begin(), binding->second.roles.end(), role.role_id) != binding->second.roles.end() &&
                        policy.accepts_role(role), "asserted role not authorized for this identity");
            }
            require(!identity.roles.empty(), "certificate has no active role assertions");
        }
        require(matches == 1, "identity certificate not individually authorized");
        std::string proof_pin;
        identity.has_signature_proof = verify_auth_proof(auth, length, identity, proof_pin);
        if (identity.has_signature_proof) {
            std::lock_guard<std::mutex> lock(proof_mu_);
            const auto now = static_cast<std::int64_t>(std::time(nullptr));
            for (auto it = used_proofs_.begin(); it != used_proofs_.end(); )
                if (it->second < now - 240) it = used_proofs_.erase(it); else ++it;
            require(used_proofs_.size() < 4096 && used_proofs_.emplace(proof_pin, now).second,
                    "replayed authentication proof");
        }
        return true;
    } catch (const std::exception& exception) { identity = {}; error = exception.what(); return false; }
#else
    (void) auth; (void) length; (void) policy; error = "RBAC cryptographic verification unavailable"; return false;
#endif
}

bool MmsRbacSecurity::matches_application_reference(const MmsRbacIdentity& identity,
                                                    const std::string& oid, std::string& error) {
#if defined(CONFIG_MMS_SUPPORT_TLS)
    try {
        Certificate leaf;
        require(mbedtls_x509_crt_parse_der(&leaf.value, identity.certificate.data(), identity.certificate.size()) == 0,
                "invalid application identity certificate");
        const unsigned char attribute[] = {0x55, 0x04, 0x6a}; // id-at-objectIdentifier
        unsigned matches = 0;
        for (auto* name = &leaf.value.subject; name; name = name->next) {
            if (name->oid.len != sizeof(attribute) || std::memcmp(name->oid.p, attribute, sizeof(attribute))) continue;
            ++matches;
            // Preserve the existing TSP profile's textual AP/AE representation.
            const std::string value(reinterpret_cast<const char*>(name->val.p), name->val.len);
            require(value == oid, "certificate AP/AE does not match calling application reference");
        }
        require(matches == 1, "application certificate needs one AP/AE objectIdentifier attribute");
        return true;
    } catch (const std::exception& exception) { error = exception.what(); return false; }
#else
    (void)identity; (void)oid; error = "application certificate validation unavailable"; return false;
#endif
}

bool MmsRbacSecurity::valid_now(const MmsRbacIdentity& identity, std::string& error) const {
#if defined(CONFIG_MMS_SUPPORT_TLS)
    try {
        require(bindings_.count(identity.fingerprint) != 0, "identity no longer authorized");
        verify_certificate(identity.certificate, ca_path_, crl_path_, nullptr);
        return true;
    } catch (const std::exception& exception) { error = exception.what(); return false; }
#else
    (void) identity; error = "RBAC cryptographic verification unavailable"; return false;
#endif
}

std::string mms_rbac_audit(const std::string& event, const std::string& severity,
                           const MmsRbacIdentity* identity, const std::string& policy_id,
                           std::uint64_t sequence, const std::string& service,
                           const std::string& object, const std::string& reason) {
    std::ostringstream out;
    out << "{\"rbac\":true,\"event\":\"" << escape(event) << "\",\"severity\":\"" << escape(severity)
        << "\",\"policy\":\"" << escape(policy_id) << "\",\"sequence\":" << sequence
        << ",\"service\":\"" << escape(service) << "\",\"object\":\"" << escape(object)
        << "\",\"reason\":\"" << escape(reason) << '"';
    if (identity) {
        out << ",\"subject\":\"" << escape(identity->subject) << "\",\"issuer\":\"" << escape(identity->issuer)
            << "\",\"serial\":\"" << escape(identity->serial) << "\",\"sha256\":\"" << identity->fingerprint
            << "\",\"peer\":\"" << escape(identity->peer_address)
            << "\",\"transport_sha256\":\"" << identity->transport_fingerprint
            << "\",\"actor\":\"" << identity->actor << "\",\"roles\":[";
        bool first = true;
        for (const auto& role : identity->roles) {
            if (!first) out << ',';
            first = false;
            out << "{\"id\":" << role.role_id << ",\"aor\":\"" << escape(role.aor)
                << "\",\"revision\":" << static_cast<unsigned>(role.revision)
                << ",\"definition\":\"" << escape(role.role_definition) << '"';
            if (role.has_operation) out << ",\"operation\":" << role.operation;
            if (role.has_sequence) out << ",\"status_sequence\":" << role.status_sequence;
            out << '}';
        }
        out << ']';
    }
    out << '}';
    return out.str();
}

} // namespace cci::adapters
