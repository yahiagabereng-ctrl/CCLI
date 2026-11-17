#include "e2e_server.hpp"
#include "e2e_association.hpp"
#include "e2e_key_schedule.hpp"
#include "e2e_time.hpp"
#include "e2e_wire.hpp"
#include "transfer_sequence.hpp"

#include <array>
#include <chrono>
#include <fstream>
#include <limits>
#include <map>
#include <mutex>
#include <set>

#if defined(CONFIG_MMS_SUPPORT_TLS)
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_crl.h"
#include "e2e_mbedtls_crl.hpp"
#endif

namespace ccli::mms::e2e {
namespace {
using namespace wire;
using Clock = std::chrono::system_clock;
using Steady = std::chrono::steady_clock;
constexpr const char* kContext = "1.0.62351.4.2.1";
constexpr const char* kProtocol = "IEC 61850-8-1";
constexpr const char* kHmac = "1.2.840.113549.2.9";
constexpr const char* kRsa = "1.2.840.113549.1.1.11";
constexpr const char* kEcdsa = "1.2.840.10045.4.3.2";
constexpr const char* kGmac128 = "2.16.840.1.101.3.4.1.6";
constexpr const char* kGmac256 = "2.16.840.1.101.3.4.1.46";
NativeServer* installed_server = nullptr; // installed before listening, removed after stop

struct Failure : std::runtime_error {
    int diagnostic;
    Failure(const char* text, int diag = -1) : std::runtime_error(text), diagnostic(diag) {}
};
struct AcseFailure : std::runtime_error {
    int diagnostic; bool provider;
    AcseFailure(const char* text, int code = -1, bool from_provider = false)
        : std::runtime_error(text), diagnostic(code), provider(from_provider) {}
};
struct ApplicationAbort : std::runtime_error {
    Bytes diagnostic;
    explicit ApplicationAbort(Bytes pdu) : std::runtime_error("MMS requested abort"), diagnostic(std::move(pdu)) {}
};
void acse_require(bool condition, const char* message) {
    if (!condition) throw AcseFailure(message);
}
struct OsiFailure : std::runtime_error {
    unsigned diagnostic;
    OsiFailure(const char* text, unsigned code) : std::runtime_error(text), diagnostic(code) {}
};
void osi_require(bool condition, const char* message, unsigned code) {
    if (!condition) throw OsiFailure(message, code);
}
void check_mapping_reference(ByteView osi, std::uint32_t context) {
    try {
        const auto entries = fields(osi, 0x30);
        osi_require(entries.size() == 1, "invalid OSI association mapping", 1);
        const auto external = fields(entries[0].encoded, 0x28);
        osi_require(external.size() == 2 && tag(external[0], 2) && tag(external[1], 0xa0),
                    "invalid OSI EXTERNAL", 1);
        osi_require(number(external[0].value) == context, "invalid OSI indirect reference", 2);
    } catch (const OsiFailure&) { throw; }
      catch (const std::exception&) { throw OsiFailure("malformed OSI association mapping", 1); }
}
struct Secret : Bytes { ~Secret() { wire::erase(*this); } };
void require(bool condition, const char* message, int diagnostic = -1) {
    if (!condition) throw Failure(message, diagnostic);
}
void crypto_ok(CryptoError result) { require(result == CryptoError::none, "crypto operation failed"); }
std::string stamp(Clock::time_point now) {
    std::string value;
    require(format_generalized_time_utc(now, value), "invalid local UTC");
    return value;
}
Bytes acse_information(Bytes osi) {
    require(!osi.empty() && osi[0] == 0x30, "invalid OSI user information");
    osi[0] = 0xbe; return osi; // ACSE IMPLICIT Association-information
}
Bytes unwrap_information(const BerTlvView& information) {
    require(tag(information, 0xbe), "missing ACSE user information", 1);
    auto result = copy(information.encoded); result[0] = 0x30; return result;
}
bool simple_algorithm(ByteView encoded, const char* id) {
    return simple_algorithm_matches(encoded, view(oid_value(id)), std::string(id) != kEcdsa);
}

struct Icv {
    enum Kind { hmac, gmac128, gmac256 } kind{hmac};
    Bytes identifier;
    std::size_t key_size{32}, tag_size{32};
};
bool parse_icv(ByteView encoded, Icv& value) {
    const auto f = fields(encoded, 0x30);
    if (f.empty() || !tag(f[0], 6)) return false;
    if (simple_algorithm(encoded, kHmac)) {
        value.kind = Icv::hmac; value.key_size = value.tag_size = 32;
    } else {
        if (equal(f[0].value, view(oid_value(kGmac128)))) {
            value.kind = Icv::gmac128; value.key_size = 16;
        } else if (equal(f[0].value, view(oid_value(kGmac256)))) {
            value.kind = Icv::gmac256; value.key_size = 32;
        } else return false;
        // Handshake algorithm IDs must omit the per-message nonce. RFC 5084
        // GCMParameters can specify an ICV length; the default is 12 octets.
        value.tag_size = 12;
        if (f.size() > 1 && f[1].tag.tag_class != BerTagClass::context_specific) {
            if (!tag(f[1], 0x30)) return false;
            const auto parameters = fields(f[1].encoded, 0x30);
            if (parameters.size() != 1 || !tag(parameters[0], 2)) return false;
            value.tag_size = number(parameters[0].value);
        }
        if (value.tag_size < 12 || value.tag_size > 16) return false;
    }
    value.identifier = copy(encoded); return true;
}
Icv negotiate(const ClearToken1View& token) {
    require(simple_algorithm(token.hmac_algorithm, kHmac), "unsupported HKDF HMAC", 5);
    require(token.encryption_mode == EncryptionModeKind::separate_encryption_and_icv,
            "authenticated encryption is not enabled in this profile", 17);
    require(token.tls_protected && !token.application_protected && !token.vpn,
            "confidentiality profile mismatch", 6);
    require(!token.has_attribute_certificate, "attribute certificates are not enabled", 16);
    const auto mode = fields(token.encoded_encryption_mode, 0xa0);
    require(mode.size() == 1 && tag(mode[0], 0xa1), "application encryption not selected", 6);
    for (const auto& offer : fields(mode[0].encoded, 0xa1)) {
        Icv selected;
        if (parse_icv(offer.encoded, selected)) return selected;
    }
    throw Failure("no supported ICV algorithm", 13);
}

Bytes load_bytes(const std::string& path, bool secret = false) {
    std::ifstream stream(path, std::ios::binary);
    require(stream.good(), secret ? "unable to open private key" : "unable to open E2E public material");
    stream.seekg(0, std::ios::end); const auto size = stream.tellg();
    require(size > 0 && size <= 1024 * 1024, "invalid E2E material size");
    stream.seekg(0); Bytes bytes(static_cast<std::size_t>(size));
    stream.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
    require(stream.good(), "unable to read E2E material"); return bytes;
}
std::vector<Bytes> load_public(const std::string& path, bool crl) {
    auto input = load_bytes(path); input.push_back(0);
    const auto length = input[0] == '-' ? input.size() : input.size() - 1;
    std::vector<Bytes> result;
#if defined(CONFIG_MMS_SUPPORT_TLS)
    if (crl) {
        mbedtls_x509_crl parsed; mbedtls_x509_crl_init(&parsed);
        const int status = parse_native_crl(&parsed, input.data(), length);
        if (!status) for (auto* p = &parsed; p && p->raw.p; p = p->next)
            result.emplace_back(p->raw.p, p->raw.p + p->raw.len);
        mbedtls_x509_crl_free(&parsed);
        require(status == 0 && !result.empty(), "invalid E2E CRL file");
    } else {
        mbedtls_x509_crt parsed; mbedtls_x509_crt_init(&parsed);
        const int status = mbedtls_x509_crt_parse(&parsed, input.data(), length);
        if (!status) for (auto* p = &parsed; p && p->raw.p; p = p->next)
            result.emplace_back(p->raw.p, p->raw.p + p->raw.len);
        mbedtls_x509_crt_free(&parsed);
        require(status == 0 && !result.empty(), "invalid E2E certificate file");
    }
#else
    (void)crl; throw Failure("native E2E requires the Mbed TLS build");
#endif
    return result;
}
struct AddressedRequest {
    Bytes osi, identity_oid;
    std::vector<std::uint32_t> ap_arcs;
    std::uint32_t qualifier{0};
};
AddressedRequest parse_aarq(ByteView encoded, ByteView local_ap, unsigned local_ae) try {
    const auto components = fields(encoded, 0x60);
    std::set<unsigned> seen;
    Bytes peer_ap, qualifier, osi;
    std::uint32_t peer_qualifier = 0;
    bool context = false;
    unsigned previous_component = 0; bool first_component = true;
    for (const auto& component : components) {
        const auto t = component.encoded.data[0];
        acse_require(first_component || component.tag.number > previous_component, "out-of-order ACSE component");
        first_component = false; previous_component = component.tag.number;
        acse_require(seen.insert(t).second, "duplicate ACSE component");
        if (t == 0xa1) {
            const auto c = fields(component.encoded, t);
            acse_require(c.size() == 1 && tag(c[0], 6), "invalid application context field");
            if (!equal(c[0].value, view(oid_value(kContext))))
                throw AcseFailure("unsupported application context", 2);
            context = true;
        } else if (t == 0xa6 || t == 0xa2) {
            const auto c = fields(component.encoded, t);
            require(c.size() == 1 && tag(c[0], 6), "invalid AP title", 1);
            if (t == 0xa6) peer_ap = copy(c[0].value);
            else if (!equal(c[0].value, local_ap)) throw AcseFailure("called AP title mismatch", 7);
        } else if (t == 0xa7 || t == 0xa3) {
            const auto c = fields(component.encoded, t);
            require(c.size() == 1 && tag(c[0], 2), "invalid AE qualifier", 1);
            const auto value = number(c[0].value);
            require(value <= 0x7fffffffULL, "AE qualifier too large", 1);
            if (t == 0xa7) { oid_arc(qualifier, value); peer_qualifier = value; }
            else if (value != local_ae) throw AcseFailure("called AE qualifier mismatch", 9);
        } else if (t == 0xbe) osi = unwrap_information(component);
        else if (t == 0x80 || t == 0xa0) {
            const auto version = string_value(component, 3, true);
            acse_require(!version.empty(), "malformed ACSE version");
            if (version.size < 2 || version.data[1] != 0x80 ||
                !std::all_of(version.data + 2, version.data + version.size, [](std::uint8_t b) { return b == 0; }))
                throw AcseFailure("no common ACSE version", 2, true);
        } else if (t == 0xa4 || t == 0xa5 || t == 0xa8 || t == 0xa9) {
            auto value = fields(component.encoded, t);
            require(value.size() == 1 && tag(value[0], 2), "invalid invocation identifier", 1);
            (void)number(value[0].value);
        } else throw AcseFailure("forbidden or unsupported native ACSE component");
    }
    acse_require(context && !peer_ap.empty() && !qualifier.empty(), "incomplete native AARQ");
    osi_require(!osi.empty(), "missing OSI association mapping", 1);
    auto arcs = oid_arcs(view(peer_ap));
    append(peer_ap, view(qualifier));
    return {std::move(osi), std::move(peer_ap), std::move(arcs), peer_qualifier};
} catch (const AcseFailure&) { throw; }
  catch (const OsiFailure&) { throw; }
  catch (const std::exception&) { throw AcseFailure("malformed native AARQ"); }
bool native_context(ByteView encoded) try {
    for (const auto& f : fields(encoded, 0x60)) if (tag(f, 0xa1)) {
        const auto a = fields(f.encoded, 0xa1);
        acse_require(a.size() == 1 && tag(a[0], 6), "malformed application context field");
        (void)oid_arcs(a[0].value);
        return equal(a[0].value, view(oid_value(kContext)));
    }
    throw AcseFailure("missing application context");
} catch (const AcseFailure&) { throw; }
  catch (const std::exception&) { throw AcseFailure("malformed application context"); }
Bytes control_information(ByteView encoded, std::uint8_t expected) try {
    Bytes result; std::set<unsigned> seen;
    for (const auto& f : fields(encoded, expected)) {
        const auto tag_value = f.encoded.data[0];
        acse_require(seen.insert(tag_value).second, "duplicate ACSE control component");
        if (tag_value == 0xbe) result = unwrap_information(f);
        else acse_require(tag_value == 0x80 && f.value.size == 1 && f.value.data[0] == 0,
                          "invalid ACSE control source/reason");
    }
    osi_require(!result.empty(), "missing signed ACSE control message", 1);
    acse_require(expected != 0x64 || seen.count(0x80), "missing ACSE abort source");
    return result;
} catch (const AcseFailure&) { throw; }
  catch (const OsiFailure&) { throw; }
  catch (const std::exception&) { throw AcseFailure("malformed ACSE control envelope"); }
bool mms_pdu(ByteView encoded, int expected = -1) {
    const auto pdu = single(encoded);
    return pdu.tag.tag_class == BerTagClass::context_specific && pdu.tag.number <= 13 &&
        (expected < 0 || pdu.tag.number == static_cast<unsigned>(expected));
}
} // namespace

struct NativeServer::Impl {
    MonotonicNow monotonic_now;
    NativeServerFiles files;
    Authorize authorize;
    Release release;
    Audit audit;
    E2eCryptoProvider crypto;
    Bytes certificate, private_key, ap, own_identity, signature_algorithm, chain_encoded;
    std::vector<Bytes> anchors, crls, chain;
    std::mutex replay_mutex;
    std::map<std::string, Clock::time_point> replay;
    std::set<std::string> active_ids;
    unsigned next_id{0};
    Clock::time_point latest_clock{};
    bool ready{false};
    ~Impl() { erase(private_key); }

    E2ePeerCertificatePath path(ByteView identity) {
        E2ePeerCertificatePath p; p.expected_identity_oid_value = identity;
        for (const auto& a : anchors) p.trust_anchors.push_back(view(a));
        for (const auto& c : crls) p.issuer_crls.push_back(view(c));
        return p;
    }
    void event(const std::string& text) { if (audit) audit("E2E:" + text); }
    std::string reserve(const ClearToken1View& token, Clock::time_point now) {
        Clock::time_point peer_time; GeneralizedTimeError time_error{};
        require(parse_generalized_time(token.timestamp, peer_time, time_error), "invalid timestamp", 10);
        std::array<std::uint8_t, 32> certificate_hash{};
        crypto_ok(crypto.sha256(token.public_key_certificate, certificate_hash));
        const std::string peer(reinterpret_cast<const char*>(certificate_hash.data()), certificate_hash.size());
        const std::string client(reinterpret_cast<const char*>(token.association_id.data), token.association_id.size);
        require(client.size() <= 3 && client.find_first_not_of("0123456789") == std::string::npos,
                "client association identifier must be one to three decimal digits", 1);
        const std::string key = peer + ":" + client + ":" + stamp(peer_time);
        std::lock_guard<std::mutex> lock(replay_mutex);
        require(now + std::chrono::seconds(1) >= latest_clock, "UTC clock moved backwards");
        latest_clock = std::max(now, latest_clock);
        for (auto it = replay.begin(); it != replay.end();) {
            if (it->second < latest_clock - std::chrono::seconds(1)) it = replay.erase(it); else ++it;
        }
        require(replay.size() < 4096 && !replay.count(key), "replayed handshake");
        std::string selected;
        for (unsigned i = 0; i < 1000; ++i) {
            selected = client + ":" + std::to_string(next_id++ % 1000);
            if (active_ids.insert(peer + selected).second) {
                replay.emplace(key, peer_time + std::chrono::minutes(10));
                return peer + selected; // prefix is retained internally, never sent
            }
        }
        throw Failure("association identifier space exhausted");
    }
};

namespace {
struct Session {
    std::mutex mutex;
    NativeServer::Release release;
    Bytes peer_certificate, initial_salt, identity_oid, association, client_association;
    std::vector<Bytes> peer_chain;
    std::string reserved_id;
    std::uint32_t context{0};
    Icv icv;
    KeyAgreementGroup group{};
    std::unique_ptr<E2eEphemeralKey> dh;
    E2eDirectionalKeyMaterial keys, pending;
    bool accepted{false}, pending_send{false}, closed{false};
    TransferSequenceWindow receive_window;
    std::string send_time;
    std::uint64_t send_sequence{0}, nonce_sequence{0};
    std::array<std::uint8_t, 4> nonce_prefix{};
    std::set<Bytes> received_nonces;
    std::set<std::pair<Clock::time_point, std::uint64_t>> received_pairs;
    Steady::time_point last_rekey{Steady::now()};
    Steady::time_point server_epoch_started{last_rekey};
    void* token{nullptr};
    ~Session() { if (token && release) release(token); }
};

template<typename Server>
Bytes token3(Server& server, const Session& session, Clock::time_point now) {
    auto time = tlv(24, view(stamp(now)));
    auto cert = tlv(4, view(server.certificate));
    Bytes content = server.signature_algorithm;
    const auto& id = session.accepted ? session.association : session.client_association;
    if (!id.empty()) append(content, view(tlv(12, view(id))));
    append(content, view(time)); append(content, view(cert)); append(content, view(server.chain_encoded));
    return tlv(0x30, view(content));
}
template<typename Server>
Bytes signed_message(Server& server, SignedMessageKind kind, ByteView tbs) {
    E2eCodecError error{}; Bytes signature, encoded; ByteView signed_contents;
    require(ber_authentication_contents(tbs, signed_contents, error), "invalid signed body");
    crypto_ok(server.crypto.sign_sha256(server.files.signature_scheme, view(server.private_key), {},
        signed_contents, signature));
    require(encode_signed_message(kind, tbs, view(server.signature_algorithm), view(signature), encoded, error),
            "unable to encode signed message");
    return encoded;
}
template<typename Server>
Bytes aare(Server& server, ByteView osi, bool accept, unsigned cause = 1, bool provider = false) {
    const auto context = tlv(0xa1, view(tlv(6, view(oid_value(kContext)))));
    const auto result = tlv(0xa2, view(integer(accept ? 0 : 1)));
    const auto diagnostic = tlv(0xa3, view(tlv(provider ? 0xa2 : 0xa1, view(integer(accept ? 0 : cause)))));
    const auto ap = tlv(0xa4, view(tlv(6, view(server.ap))));
    const auto ae = tlv(0xa5, view(integer(server.files.ae_qualifier)));
    const auto information = osi.empty() ? Bytes{} : acse_information(copy(osi));
    const auto content = join({view(context),view(result),view(diagnostic),view(ap),view(ae),view(information)});
    return tlv(0x61, view(content));
}
template<typename Server>
Bytes signed_failure(Server& server, Session& session, const Failure& failure,
                     bool handshake, Clock::time_point now) {
    auto content = token3(server, session, now);
    if (failure.diagnostic >= 0) append(content, view(integer(failure.diagnostic, 10)));
    auto tbs = tlv(0x30, view(content));
    const auto kind = handshake ? SignedMessageKind::handshake_security_reject : SignedMessageKind::transfer_security_abort;
    const auto signed_body = signed_message(server, kind, view(tbs));
    Bytes osi; E2eCodecError error{};
    if (handshake) {
        require(encode_osi_association_response(session.context, view(signed_body), osi, error), "reject wrapper failed");
        return aare(server, view(osi), false);
    }
    require(encode_osi_association_abort(session.context, view(signed_body), osi, error), "abort wrapper failed");
    const auto source = integer(0, 0x80);
    const auto information = acse_information(std::move(osi));
    return tlv(0x64, view(join({view(source), view(information)})));
}
template<typename Server>
Bytes dispatch(Server& server, const CcliE2eIo& io, ByteView pdu) {
    require(io.dispatch && mms_pdu(pdu), "invalid MMS application PDU", 1);
    require(pdu.size <= server.files.maximum_pdu, "MMS PDU exceeds profile limit", 1);
    // The selected MMS stack may serialize up to its compile-time PDU cap,
    // even when the native profile has a smaller limit. The C bridge checks
    // this backing capacity before calling the legacy serializer.
    Bytes response(std::max<std::size_t>(server.files.maximum_pdu, 65535));
    int count;
    try {
        count = io.dispatch(io.connection, pdu.data, static_cast<int>(pdu.size),
            response.data(), static_cast<int>(response.size()));
    } catch (...) {
#if defined(CCLI_HAVE_LIBIEC61850)
        CcliE2e_clearDispatchContext(io.connection);
#endif
        throw;
    }
    if (count <= -2 && count >= -static_cast<int>(response.size()) - 2) {
        response.resize(static_cast<std::size_t>(-count - 2));
        throw ApplicationAbort(std::move(response));
    }
    require(count >= 0 && static_cast<std::size_t>(count) <= response.size(), "MMS dispatch failed");
    if (static_cast<std::size_t>(count) > server.files.maximum_pdu)
        throw ApplicationAbort({});
    response.resize(count); return response;
}
template<typename Server>
Bytes protect(Server& server, Session& session, ByteView pdu, Clock::time_point now) {
    require(session.accepted && !session.closed && mms_pdu(pdu) && pdu.size <= server.files.maximum_pdu,
            "outgoing MMS outside native association or profile limit");
    const auto time = stamp(now);
    require(session.send_time.empty() || now >= session.receive_window.last_timestamp().value_or(Clock::time_point{}) -
        std::chrono::minutes(10), "invalid local UTC");
    if (time == session.send_time) {
        require(session.send_sequence != std::numeric_limits<std::uint64_t>::max(), "sequence exhausted");
        ++session.send_sequence;
    } else {
        if (!session.send_time.empty()) {
            Clock::time_point previous; GeneralizedTimeError e{};
            require(parse_generalized_time(view(session.send_time), previous, e) && now >= previous, "UTC clock regressed");
        }
        session.send_time = time; session.send_sequence = 0;
    }
    const auto elapsed = server.monotonic_now() - session.server_epoch_started;
    require(elapsed < std::chrono::seconds(server.files.rekey_seconds), "application rekey overdue");
    ClearToken2Fields fields;
    fields.association_id = view(session.association); fields.timestamp = view(time);
    fields.sequence = session.send_sequence; fields.changed_key = session.pending_send;
    fields.request_rekey = !session.pending_send &&
        (elapsed >= std::chrono::seconds(server.files.rekey_seconds - 60) ||
         session.received_nonces.size() >= 60000);
    Bytes token, tbp, tag_value, auth, transfer, choice, nonce;
    E2eCodecError error{};
    require(encode_clear_token2(fields, token, error) && encode_tbp(view(token), pdu, tbp, error), "transfer encoding failed");
    ByteView protected_bytes;
    require(ber_authentication_contents(view(tbp), protected_bytes, error), "invalid TBP");
    if (session.icv.kind == Icv::hmac) {
        std::array<std::uint8_t,32> mac{};
        crypto_ok(server.crypto.hmac_sha256(view(session.keys.server_directional_key), protected_bytes, mac));
        tag_value.assign(mac.begin(), mac.end());
    } else {
        require(session.nonce_sequence != std::numeric_limits<std::uint64_t>::max(), "GMAC nonce exhausted");
        nonce.assign(session.nonce_prefix.begin(), session.nonce_prefix.end());
        const auto value = session.nonce_sequence++;
        for (int i=7; i>=0; --i) nonce.push_back((value >> (i * 8)) & 255);
        tag_value.resize(session.icv.tag_size);
        crypto_ok(server.crypto.aes_gmac(view(session.keys.server_directional_key), view(nonce),
                                        protected_bytes, {tag_value.data(), tag_value.size()}));
    }
    AuthenticatorFields authenticator;
    authenticator.has_nonce = !nonce.empty(); authenticator.nonce = view(nonce);
    authenticator.algorithm_identifier = view(session.icv.identifier); authenticator.icv = view(tag_value);
    require(encode_authenticator(authenticator, auth, error) &&
        encode_clear_transfer(view(tbp), view(auth), transfer, error) &&
        encode_data_transfer_choice(DataTransferKind::clear, view(transfer), choice, error), "transfer envelope failed");
    if (session.pending_send) {
        erase(session.keys.server_directional_key);
        session.keys.server_directional_key.swap(session.pending.server_directional_key);
        session.pending.clear(); session.pending_send = false;
        session.server_epoch_started = session.last_rekey;
        session.nonce_sequence = 0;
        crypto_ok(server.crypto.random({session.nonce_prefix.data(), session.nonce_prefix.size()}));
    }
    return choice;
}
template<typename Server>
Bytes receive(Server& server, Session& session, ByteView input, Clock::time_point now) {
    require(session.accepted && !session.closed, "incoming MMS outside native association", 1);
    require(server.monotonic_now() - session.last_rekey < std::chrono::seconds(server.files.rekey_seconds),
            "application rekey overdue");
    require(input.size <= server.files.maximum_pdu + 4096, "incoming transfer exceeds profile limit", 1);
    DataTransferKind kind; ByteView encoded; E2eCodecError error{};
    require(decode_data_transfer(input, kind, encoded, error), "invalid DataTransfer", 1);
    require(kind == DataTransferKind::clear, "encrypted transfer not negotiated", 3);
    const auto parts = fields(encoded, 0xa0);
    require(parts.size() >= 2 && tag(parts[0], 0x30) && tag(parts[1], 0x30), "invalid ClearTransfer root", 1);
    AuthenticatorView authenticator;
    require(decode_authenticator(parts[1].encoded, authenticator, error), "invalid Authenticator", 1);
    ByteView icv_input;
    require(ber_authentication_contents(parts[0].encoded, icv_input, error), "invalid TBP", 1);
    Icv received;
    require(parse_icv(authenticator.algorithm_identifier, received) &&
        received.kind == session.icv.kind && received.tag_size == session.icv.tag_size,
        "invalid ICV algorithm");
    require(authenticator.icv.size == session.icv.tag_size, "invalid ICV length");
    Bytes nonce;
    if (session.icv.kind == Icv::hmac) {
        require(!authenticator.has_nonce, "unexpected HMAC nonce", 1);
        crypto_ok(server.crypto.verify_hmac_sha256(view(session.keys.client_directional_key),
            icv_input, authenticator.icv));
    } else {
        require(authenticator.has_nonce && !authenticator.nonce.empty() &&
                authenticator.nonce.size <= 256,
                "GMAC nonce required", 6);
        crypto_ok(server.crypto.verify_aes_gmac(view(session.keys.client_directional_key),
            authenticator.nonce, icv_input, authenticator.icv));
        nonce = copy(authenticator.nonce);
        require(session.received_nonces.size() < 65536 && !session.received_nonces.count(nonce),
                "reused GMAC nonce");
    }
    ClearTransferView transfer;
    const bool decoded_transfer = decode_clear_transfer(encoded, transfer, error);
    require(decoded_transfer, "invalid ClearTransfer", error == E2eCodecError::unsupported_version ? 2 : 1);
    const auto& token = transfer.token;
    require(equal(token.association_id, view(session.association)), "association identifier mismatch");
    require(!token.request_rekey && !token.changed_key, "client sent server-only rekey flags", 1);
    require(!token.has_initialization_vector, "unexpected encryption IV", 1);
    Clock::time_point peer_time; GeneralizedTimeError time_error{};
    require(parse_generalized_time(token.timestamp, peer_time, time_error), "invalid transfer timestamp", 7);
    const auto pair = std::make_pair(peer_time, token.sequence);
    while (!session.received_pairs.empty() && session.received_pairs.begin()->first < now - std::chrono::minutes(10))
        session.received_pairs.erase(session.received_pairs.begin());
    require(!session.received_pairs.count(pair), "replayed transfer");
    require(session.received_pairs.size() < server.files.maximum_receive_pairs, "transfer replay window exhausted");
    TransferSequenceWindow::Candidate candidate;
    const auto replay = session.receive_window.prepare_authenticated(token.timestamp, token.sequence, now, candidate);
    require(replay == TransferSequenceError::none, "transfer timestamp or sequence rejected",
        replay == TransferSequenceError::sequence_not_incremented ||
        replay == TransferSequenceError::sequence_not_zero_at_new_timestamp ? 8 : 7);
    require(mms_pdu(transfer.application_data) && transfer.application_data.size <= server.files.maximum_pdu,
            "invalid or oversized protected MMS PDU", 1);
    if (token.has_rekey_public_key) {
        require(!session.pending_send, "overlapping application rekey", 10);
        Secret secret; Bytes salt;
        const auto derived = session.dh->derive_shared_secret(token.rekey_public_key, secret,
            KeyAgreementUse::retained_server_rekey);
        if (derived != CryptoError::none) { erase(secret); throw Failure("invalid rekey public key", 10); }
        auto result = derive_rekey_transcript_salt_sha256(server.crypto, transfer.icv_input, view(session.initial_salt), salt);
        if (result == CryptoError::none)
            result = derive_directional_key_material_sha256(server.crypto, view(secret), view(salt),
                {session.icv.key_size,session.icv.key_size,0,0}, session.pending);
        erase(secret); crypto_ok(result);
    }
    require(session.receive_window.commit_authenticated(candidate) == TransferSequenceError::none,
            "receive sequence commit failed");
    session.received_pairs.insert(pair);
    if (!nonce.empty()) session.received_nonces.insert(std::move(nonce));
    if (token.has_rekey_public_key) {
        erase(session.keys.client_directional_key);
        session.keys.client_directional_key.swap(session.pending.client_directional_key);
        session.received_nonces.clear(); session.pending_send = true;
        session.last_rekey = server.monotonic_now();
        server.event("REKEY_RECEIVED");
    }
    return copy(transfer.application_data);
}
template<typename Server>
void verify_control_envelope(Server& server, const Session& session, ByteView osi) {
    const auto entries = fields(osi, 0x30);
    require(entries.size() == 1, "invalid control envelope", 1);
    const auto external = fields(entries[0].encoded, 0x28);
    require(external.size() == 2 && tag(external[1], 0xa0), "invalid control EXTERNAL", 1);
    const auto encoded = single(external[1].value);
    const auto body = fields(encoded.encoded, encoded.encoded.data[0]);
    require(body.size() == 2 && tag(body[0], 0x30), "invalid signed control body", 1);
    const auto signature = fields(body[1].encoded, 0x30);
    require(signature.size() >= 2, "invalid control signature", 1);
    const auto signature_value = string_value(signature[1], 4);
    const bool rsa = simple_algorithm(signature[0].encoded, kRsa);
    require(rsa || simple_algorithm(signature[0].encoded, kEcdsa), "unsupported control signature");
    ByteView signed_contents; E2eCodecError error{};
    require(ber_authentication_contents(body[0].encoded, signed_contents, error), "invalid signed control TBS", 1);
    crypto_ok(server.crypto.verify_sha256(rsa ? SignatureScheme::rsa_pkcs1_v15_sha256 : SignatureScheme::ecdsa_sha256,
        view(session.peer_certificate), signed_contents, signature_value));
}
template<typename Server>
void validate_control(Server& server, Session& session, const SignedMessageView& message) {
    const auto& token = message.token3;
    require(equal(token.public_key_certificate, view(session.peer_certificate)) &&
        token.has_association_id && equal(token.association_id, view(session.association)) &&
        ((simple_algorithm(token.signature_algorithm, kRsa) &&
          simple_algorithm(message.signature.algorithm_identifier, kRsa)) ||
         (simple_algorithm(token.signature_algorithm, kEcdsa) &&
          simple_algorithm(message.signature.algorithm_identifier, kEcdsa))),
        "control identity/algorithm mismatch");
    Clock::time_point peer_time; GeneralizedTimeError time_error{};
    const auto current = Clock::now();
    require(parse_generalized_time(token.timestamp, peer_time, time_error) &&
        peer_time >= current - std::chrono::minutes(10) &&
        peer_time <= current + std::chrono::minutes(10), "invalid control timestamp", 7);
    auto path = server.path(view(session.identity_oid));
    std::vector<BerStringView> certificate_values;
    if (token.has_certificate_path) {
        for (const auto& certificate : fields(token.certificate_path, 0x30))
            certificate_values.push_back(string_value(certificate, 4));
        for (const auto& certificate : certificate_values) path.intermediates.push_back(certificate);
    } else {
        for (const auto& certificate : session.peer_chain) path.intermediates.push_back(view(certificate));
    }
    require(validate_peer_certificate(view(session.peer_certificate), path) == IdentityError::none,
            "control certificate is no longer valid");
}
} // namespace

NativeServer::NativeServer(MonotonicNow monotonic_now) : impl_(new Impl) {
    impl_->monotonic_now = monotonic_now ? std::move(monotonic_now) : [] { return Steady::now(); };
}
NativeServer::~NativeServer() { uninstall(); }

bool NativeServer::configure(const NativeServerFiles& files, Authorize authorize,
                            Release release, Audit audit, std::string& error) {
    error.clear();
    try {
        require(!impl_->ready && !owner_, "native server already configured");
        require(authorize && files.rekey_seconds >= 900 && files.rekey_seconds <= 86400 &&
            files.maximum_pdu >= 1024 && files.maximum_pdu <= 64000 &&
            files.maximum_receive_pairs > 0 && files.maximum_receive_pairs <= 65536, "invalid native E2E profile");
        auto& s = *impl_;
        s.files = files; s.authorize = std::move(authorize);
        s.release = std::move(release); s.audit = std::move(audit);
        crypto_ok(s.crypto.status());
        s.ap = oid_value(files.ap_title); s.own_identity = s.ap; oid_arc(s.own_identity, files.ae_qualifier);
        (void)oid_arcs(view(s.ap));
        require(files.ae_qualifier <= 0x7fffffffU, "AE qualifier outside stack limits");
        auto certificates = load_public(files.certificate, false);
        require(certificates.size() == 1, "native leaf file must contain exactly one certificate");
        s.certificate = std::move(certificates.front());
        const auto transport_certificates = load_public(files.transport_certificate, false);
        require(!equal(view(s.certificate), view(transport_certificates.front())),
                "TLS and application end-entity certificates must be distinct");
        s.private_key = load_bytes(files.private_key, true);
        s.anchors = load_public(files.trust_anchors, false); s.crls = load_public(files.crls, true);
        for (const auto& path : files.certificate_path) {
            auto values = load_public(path, false);
            for (auto& certificate : values) s.chain.push_back(std::move(certificate));
        }
        auto own_path = s.path(view(s.own_identity));
        Bytes chain_values;
        for (const auto& certificate : s.chain) {
            own_path.intermediates.push_back(view(certificate));
            append(chain_values, view(tlv(4, view(certificate))));
        }
        if (!chain_values.empty()) s.chain_encoded = tlv(0x30, view(chain_values));
        require(validate_peer_certificate(view(s.certificate), own_path) == IdentityError::none,
                "server application certificate does not satisfy Annex G/trust/CRL/identity policy");
        s.signature_algorithm = algorithm(files.signature_scheme == SignatureScheme::ecdsa_sha256 ? kEcdsa : kRsa);
        Bytes proof; const std::string challenge = "CCLI native E2E configuration";
        crypto_ok(s.crypto.sign_sha256(files.signature_scheme, view(s.private_key), {}, view(challenge), proof));
        crypto_ok(s.crypto.verify_sha256(files.signature_scheme, view(s.certificate), view(challenge), view(proof)));
        s.ready = true; return true;
    } catch (const std::exception& exception) { error = exception.what(); auto now = impl_->monotonic_now; impl_.reset(new Impl);
        impl_->monotonic_now = std::move(now); return false; }
}

bool NativeServer::install(void* owner) {
#if defined(CCLI_HAVE_LIBIEC61850)
    if (!impl_->ready || !owner || owner_ || installed_server) return false;
    installed_server = this;
    if (!CcliE2e_install(owner, &NativeServer::handler)) { installed_server = nullptr; return false; }
    owner_ = owner; return true;
#else
    (void)owner; return false;
#endif
}
void NativeServer::uninstall() {
#if defined(CCLI_HAVE_LIBIEC61850)
    if (owner_) { CcliE2e_install(owner_, nullptr); owner_ = nullptr; installed_server = nullptr; }
#endif
}
int NativeServer::handler(void*, void** session, int event, const CcliE2eIo* io,
    const uint8_t* input, int size, uint8_t* output, int capacity, CcliE2eReply* reply) {
    if (!installed_server || !io || !reply || size < 0 || capacity < 0) return -1;
    try {
        Bytes bytes; const auto result = installed_server->process(session, event, *io,
            {input, static_cast<std::size_t>(size)}, bytes, *reply);
        if (bytes.size() > static_cast<std::size_t>(capacity)) return -1;
        if (!bytes.empty()) std::memcpy(output, bytes.data(), bytes.size());
        reply->size = static_cast<int>(bytes.size()); return result;
    } catch (...) { return -1; }
}

int NativeServer::process(void** storage, int event, const CcliE2eIo& transport_io,
                         ByteView input, Bytes& output, CcliE2eReply& reply) {
    output.clear(); reply = {};
    if (!storage || !impl_->ready) return -1;
    auto& server = *impl_;
    auto io = transport_io;
    auto* session = static_cast<Session*>(*storage);
    if (event == CCLI_E2E_CLOSE) {
        if (session) {
            { std::lock_guard<std::mutex> lock(server.replay_mutex); server.active_ids.erase(session->reserved_id); }
            delete session; *storage = nullptr;
        }
        return 1;
    }
    std::unique_lock<std::mutex> session_lock;
    if (session) session_lock = std::unique_lock<std::mutex>(session->mutex);
    try {
        if (event == CCLI_E2E_PRESENTATION || event == CCLI_E2E_PRESENTATION_RELEASE ||
            event == CCLI_E2E_PRESENTATION_ABORT) {
            osi_require(input.size <= server.files.maximum_pdu + 4096, "oversized native presentation envelope", 1);
            try {
                const auto entries = fields(input, 0x61);
                osi_require(entries.size() == 1, "native presentation requires one PDV", 1);
                const auto pdv = fields(entries[0].encoded, 0x30);
                osi_require(pdv.size() == 2 && tag(pdv[0], 2) && tag(pdv[1], 0xa0), "invalid native PDV", 1);
                const auto pci = number(pdv[0].value);
                osi_require(pci <= 0x7fffffffU, "invalid native PCI", 3);
                io.input_context = static_cast<int>(pci);
                (void)single(pdv[1].value);
                input = pdv[1].value;
                event = event == CCLI_E2E_PRESENTATION_RELEASE ? CCLI_E2E_RELEASE :
                    event == CCLI_E2E_PRESENTATION_ABORT || io.input_context == io.acse_context
                        ? CCLI_E2E_CONTROL : CCLI_E2E_RECEIVE;
            } catch (const OsiFailure&) { throw; }
              catch (const std::exception&) { throw OsiFailure("malformed native presentation envelope", 1); }
        }
        if (event == CCLI_E2E_ASSOCIATE) {
            acse_require(!session, "duplicate association on native connection");
            acse_require(input.size <= 10240 && input.size <= server.files.maximum_pdu, "association too large");
            if (!native_context(input)) {
                if (!server.files.allow_compatibility || io.e2e_context != 0)
                    throw AcseFailure("unsupported application context", 2);
                return 0;
            }
            session = new Session;
            session->release = server.release; *storage = session;
            session_lock = std::unique_lock<std::mutex>(session->mutex);
            session->context = io.e2e_context;
            require(io.input_context == io.acse_context, "AARQ received on wrong presentation context", 1);
            require(io.e2e_context > 0 && io.e2e_context != io.mms_context &&
                    io.e2e_context != io.acse_context, "missing native presentation context", 1);
            require(io.tls_certificate && io.tls_certificate_size > 0, "native profile requires mutual TLS");
            const auto request = parse_aarq(input, view(server.ap), server.files.ae_qualifier);
            check_mapping_reference(view(request.osi), session->context);
            // Safe partial ID for a rejection, including failures before the
            // full typed token can be validated. Never append a server ID here.
            try {
                const auto external = fields(fields(view(request.osi), 0x30)[0].encoded, 0x28);
                const auto handshake = fields(single(external[1].value).encoded, 0x30);
                const auto tbs = fields(handshake.at(1).encoded, 0x30);
                for (const auto& field : fields(tbs.at(0).encoded, 0x30)) if (
                    field.tag.tag_class == BerTagClass::universal && field.tag.number == 12) {
                    const auto id = copy(string_value(field, 12));
                    if (!id.empty() && id.size() <= 7 &&
                        std::string(id.begin(), id.end()).find_first_not_of("0123456789:") == std::string::npos)
                        session->client_association = id;
                    break;
                }
            } catch (const std::exception&) { /* the security decoder handles malformed token fields */ }
            HandshakeValidationPolicy policy;
            const std::string protected_protocol = kProtocol;
            policy.expected_protected_protocol = view(protected_protocol);
            policy.peer_certificate_path = server.path(view(request.identity_oid));
            policy.current_time = Clock::now();
            ValidatedHandshakePeer peer;
            IdentityError identity_error{}; E2eCodecError codec_error{};
            const auto validation = validate_handshake_request(view(request.osi), policy, server.crypto, peer,
                &identity_error, &codec_error);
            int diagnostic = -1;
            if (validation == HandshakeValidationError::protected_protocol_mismatch) diagnostic = 2;
            if (validation == HandshakeValidationError::unsupported_signature_algorithm) diagnostic = 3;
            if (validation == HandshakeValidationError::invalid_timestamp ||
                validation == HandshakeValidationError::timestamp_outside_window) diagnostic = 10;
            if (validation == HandshakeValidationError::invalid_certificate_path_encoding) diagnostic = 9;
            if (validation == HandshakeValidationError::peer_identity_rejected) diagnostic = 8;
            if (identity_error == IdentityError::association_identity_mismatch) diagnostic = -1;
            if (identity_error == IdentityError::revoked_or_untrusted ||
                identity_error == IdentityError::invalid_intermediate_chain) diagnostic = 9;
            if (validation == HandshakeValidationError::malformed_association_request) diagnostic = 1;
            if (codec_error == E2eCodecError::unsupported_version) diagnostic = 4;
            require(validation == HandshakeValidationError::none, "native handshake preflight rejected", diagnostic);
            require(!equal(peer.certificate_der, {io.tls_certificate,
                    static_cast<std::size_t>(io.tls_certificate_size)}),
                    "peer reused its TLS end-entity certificate for E2E");
            OsiAssociationRequestView decoded; E2eCodecError error{};
            require(decode_osi_association_request(view(request.osi), decoded, error) &&
                decoded.indirect_reference == session->context, "invalid handshake indirect reference", 1);
            const auto& token = decoded.handshake.token1_fields;
            require(decoded.handshake.has_application_data && mms_pdu(decoded.handshake.application_data, 8),
                    "MMS InitiateRequest required", 1);
            session->icv = negotiate(token);
            require(token.dh.group_id == 14 || token.dh.group_id == 23 || token.dh.group_id == 28,
                    "unsupported DH group", 11);
            session->group = static_cast<KeyAgreementGroup>(token.dh.group_id);
            session->reserved_id = server.reserve(token, policy.current_time);
            const auto public_id = session->reserved_id.substr(32);
            session->association.assign(public_id.begin(), public_id.end());
            session->peer_certificate = copy(peer.certificate_der);
            session->identity_oid = request.identity_oid;
            if (token.has_certificate_path)
                for (const auto& certificate : fields(token.certificate_path, 0x30))
                    session->peer_chain.push_back(copy(string_value(certificate, 4)));
            crypto_ok(server.crypto.generate_ephemeral_key(session->group, session->dh));
            Secret secret;
            const auto agreement = session->dh->derive_shared_secret(peer.ephemeral_public_key, secret);
            if (agreement != CryptoError::none) { erase(secret); throw Failure("invalid DH public key", 11); }
            require(server.authorize(peer.certificate_der,
                {io.tls_certificate, static_cast<std::size_t>(io.tls_certificate_size)},
                io.peer_address ? io.peer_address : "", &session->token), "native identity authorization rejected");
            require(io.security_token && session->token, "missing stack security token");
            *io.security_token = session->token;
            require(io.set_reference && io.set_reference(io.connection, request.ap_arcs.data(),
                static_cast<int>(request.ap_arcs.size()), request.qualifier), "unable to set calling application reference");
            require(io.set_maximum_pdu && io.set_maximum_pdu(io.connection,
                static_cast<int>(server.files.maximum_pdu)), "unable to set native MMS PDU limit");
            auto response = dispatch(server, io, decoded.handshake.application_data);
            if (mms_pdu(view(response), 10)) {
                const auto content = join({view(token3(server, *session, Clock::now())), view(response)});
                const auto tbs = tlv(0x30, view(content));
                const auto rejected = signed_message(server, SignedMessageKind::application_reject, view(tbs));
                Bytes wrapper;
                require(encode_osi_association_response(session->context, view(rejected), wrapper, error),
                        "application reject wrapper failed");
                output = aare(server, view(wrapper), false); reply.kind = CCLI_E2E_ACCEPT;
                session->closed = true; reply.close_after_send = 1;
                server.event("APPLICATION_REJECT"); return 1;
            }
            require(mms_pdu(view(response), 9), "invalid MMS initiation response");
            const auto now = stamp(Clock::now());
            auto cert = tlv(4, view(server.certificate));
            auto id = tlv(12, view(session->association));
            auto dh_group = integer(token.dh.group_id);
            auto dh_public = tlv(4, session->dh->public_key());
            auto dh = tlv(0x30, view(join({view(dh_group),view(dh_public)})));
            auto hmac = algorithm(kHmac); auto time = tlv(24, view(now));
            auto icv_list = tlv(0xa1, view(session->icv.identifier));
            auto mode = tlv(0xa0, view(icv_list));
            const Bytes confidentiality{7,0x80};
            auto conf = tlv(3, view(confidentiality));
            auto token1 = tlv(0x30, view(join({view(server.signature_algorithm),view(cert),
                view(server.chain_encoded),view(id),view(dh),view(hmac),view(time),view(mode),view(conf)})));
            auto tbs = tlv(0x30, view(join({view(token1),view(response)})));
            auto accept = signed_message(server, SignedMessageKind::handshake_accept, view(tbs));
            Bytes osi; require(encode_osi_association_response(session->context, view(accept), osi, error),
                               "association response encoding failed");
            ByteView accept_contents;
            require(ber_authentication_contents(view(tbs), accept_contents, error), "invalid accept TBS");
            auto derived = derive_initial_transcript_salt_sha256(server.crypto, decoded.handshake.signed_tbs_contents,
                accept_contents, session->initial_salt);
            if (derived == CryptoError::none)
                derived = derive_directional_key_material_sha256(server.crypto, view(secret), view(session->initial_salt),
                    {session->icv.key_size,session->icv.key_size,0,0}, session->keys);
            erase(secret); crypto_ok(derived);
            crypto_ok(server.crypto.random({session->nonce_prefix.data(),session->nonce_prefix.size()}));
            session->accepted = true; session->last_rekey = server.monotonic_now();
            session->server_epoch_started = session->last_rekey;
            output = aare(server, view(osi), true); reply.kind = CCLI_E2E_ACCEPT;
            server.event("ASSOCIATION_ACCEPT"); return 1;
        }
        if (!session || !session->accepted || session->closed) { reply.close_after_send = 1; return -1; }
        if (event == CCLI_E2E_TICK) {
            require(server.monotonic_now() - session->last_rekey <
                std::chrono::seconds(server.files.rekey_seconds) &&
                server.monotonic_now() - session->server_epoch_started <
                std::chrono::seconds(server.files.rekey_seconds), "application rekey overdue");
            return 1;
        }
        if (event == CCLI_E2E_SEND) {
            output = protect(server, *session, input, Clock::now()); reply.kind = CCLI_E2E_DATA; return 1;
        }
        if (event == CCLI_E2E_RECEIVE) {
            osi_require(io.input_context == io.e2e_context, "transfer received on wrong presentation context", 3);
            auto pdu = receive(server, *session, input, Clock::now());
            auto response = dispatch(server, io, view(pdu));
            if (!response.empty()) { output = protect(server, *session, view(response), Clock::now()); reply.kind = CCLI_E2E_DATA; }
            return 1;
        }
        require(event == CCLI_E2E_RELEASE || event == CCLI_E2E_CONTROL, "unexpected native event");
        osi_require(io.input_context == io.acse_context, "ACSE control on wrong presentation context", 3);
        if (event == CCLI_E2E_CONTROL) {
            std::vector<BerTlvView> components;
            try { components = fields(input, 0x64); }
            catch (const std::exception&) { throw AcseFailure("malformed ACSE abort"); }
            if (components.size() == 1 && tag(components[0], 0x80) &&
                components[0].value.size == 1 && components[0].value.data[0] == 1) {
                session->closed = true; reply.close_after_send = 1;
                server.event("PEER_PROVIDER_ABORT"); return 1;
            }
            const auto osi = control_information(input, 0x64);
            check_mapping_reference(view(osi), session->context);
            const auto mapping = fields(fields(view(osi), 0x30)[0].encoded, 0x28);
            if (tag(single(mapping[1].value), 0x83)) {
                OsiAbortView abort; E2eCodecError error{};
                osi_require(decode_osi_association_abort(view(osi), abort, error) && abort.osi_diagnostic <= 3,
                            "invalid peer OSI abort", 1);
                session->closed = true; reply.close_after_send = 1;
                server.event("PEER_OSI_ABORT"); return 1;
            }
            verify_control_envelope(server, *session, view(osi));
            OsiAbortView abort; E2eCodecError error{};
            const bool decoded_abort = decode_osi_association_abort(view(osi), abort, error);
            require(decoded_abort && abort.indirect_reference == session->context && !abort.has_osi_diagnostic,
                    "invalid signed abort", error == E2eCodecError::unsupported_version ? 2 : 1);
            validate_control(server, *session, abort.message);
            session->closed = true; reply.close_after_send = 1; server.event("PEER_ABORT"); return 1;
        }
        const auto osi = control_information(input, 0x62);
        check_mapping_reference(view(osi), session->context);
        verify_control_envelope(server, *session, view(osi));
        OsiReleaseView release; E2eCodecError error{};
        const bool decoded_release = decode_osi_release_request(view(osi), release, error);
        require(decoded_release && release.indirect_reference == session->context, "invalid signed release",
                error == E2eCodecError::unsupported_version ? 2 : 1);
        const auto& message = release.message;
        validate_control(server, *session, message);
        auto body = token3(server, *session, Clock::now());
        auto tbs = tlv(0x30, view(body));
        auto signed_body = signed_message(server, SignedMessageKind::release_response, view(tbs));
        Bytes wrapper;
        require(encode_osi_release_response(session->context, view(signed_body), wrapper, error), "release wrapper failed");
        auto information = acse_information(std::move(wrapper));
        output = tlv(0x63, view(information)); reply.kind = CCLI_E2E_RELEASE_RESPONSE;
        reply.close_after_send = 1; session->closed = true; server.event("ASSOCIATION_RELEASE"); return 1;
    } catch (const ApplicationAbort& abort) {
        if (!session || !session->accepted) { reply.close_after_send = 1; return -1; }
        auto content = token3(server, *session, Clock::now());
        require(abort.diagnostic.empty() || mms_pdu(view(abort.diagnostic)), "invalid MMS abort diagnostic");
        append(content, view(abort.diagnostic));
        const auto tbs = tlv(0x30, view(content));
        const auto signed_body = signed_message(server, SignedMessageKind::application_abort, view(tbs));
        Bytes wrapper; E2eCodecError error{};
        require(encode_osi_association_abort(session->context, view(signed_body), wrapper, error), "ApplAbort wrapper failed");
        const auto source = integer(0, 0x80);
        const auto information = acse_information(std::move(wrapper));
        output = tlv(0x64, view(join({view(source), view(information)})));
        reply.kind = CCLI_E2E_ABORT; reply.close_after_send = 1; session->closed = true;
        server.event("APPLICATION_ABORT"); return 1;
    } catch (const AcseFailure& failure) {
        server.event(std::string("ACSE_REJECT:") + failure.what());
        if (failure.diagnostic >= 0) {
            output = aare(server, {}, false, static_cast<unsigned>(failure.diagnostic), failure.provider);
            reply.kind = CCLI_E2E_ACCEPT;
        } else {
            output = tlv(0x64, view(integer(1, 0x80)));
            reply.kind = CCLI_E2E_ABORT;
        }
        reply.close_after_send = 1; if (session) session->closed = true; return 1;
    } catch (const OsiFailure& failure) {
        // IEC 62351-4 15.6.3: service-user ABRT, OSI diagnostic in
        // user-information, no abort-diagnostic and no security signature.
        server.event(std::string("OSI_REJECT:") + failure.what());
        reply.close_after_send = 1;
        if (!session || !session->context) return -1;
        const auto diagnostic = integer(failure.diagnostic, 0x83);
        Bytes wrapper; E2eCodecError error{};
        require(encode_osi_association_abort(session->context, view(diagnostic), wrapper, error),
                "unable to encode OSI diagnostic abort");
        const auto source = integer(0, 0x80);
        const auto information = acse_information(std::move(wrapper));
        output = tlv(0x64, view(join({view(source), view(information)})));
        reply.kind = CCLI_E2E_ABORT; session->closed = true; return 1;
    } catch (const Failure& failure) {
        server.event(std::string("REJECT:") + failure.what());
        if (failure.diagnostic < 0) server.event(std::string("ALARM:") + failure.what());
        reply.close_after_send = 1;
        if (session && session->context) {
            output = signed_failure(server, *session, failure, !session->accepted, Clock::now());
            reply.kind = session->accepted ? CCLI_E2E_ABORT : CCLI_E2E_ACCEPT;
            session->closed = true; return 1;
        }
        return -1;
    } catch (const std::exception& exception) {
        server.event(std::string("FAILURE:") + exception.what()); reply.close_after_send = 1;
        if (session && session->accepted && !session->closed) {
            output = signed_failure(server, *session, Failure("malformed security data", 1), false, Clock::now());
            reply.kind = CCLI_E2E_ABORT; session->closed = true; return 1;
        }
        return -1;
    }
}
} // namespace ccli::mms::e2e
