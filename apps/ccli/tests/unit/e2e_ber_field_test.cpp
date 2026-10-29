#include "e2e_codec.hpp"
#include "e2e_wire.hpp"
#include "e2e_time.hpp"
#include "e2e_allocation_fault.hpp"
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace ccli::mms::e2e;
using namespace ccli::mms::e2e::wire;
namespace {
unsigned failures = 0;
std::size_t checks = 0, mutations = 0, accepted_mutations = 0;
std::vector<std::pair<std::string, bool>> field_assertions;
using Decoder = std::function<bool(const Bytes&, E2eCodecError&)>;
struct Field { std::string name; Bytes value; bool optional{false}; };
struct Seed { std::string name; Bytes encoded; Decoder decode; };
struct SeedMutationEvidence {
    std::string name;
    std::size_t mutations;
    std::size_t accepted;
    std::size_t rejected;
};
std::vector<Seed> seeds;
std::vector<SeedMutationEvidence> seed_mutation_evidence;
void check(bool ok, const std::string& name) {
    ++checks;
    // The structural matrix labels field-specific assertions as Type/field.
    // Keep these separate from the much larger byte-mutation counter so the
    // test run can be joined reproducibly to the expanded schema map.
    if (name.find('/') != std::string::npos)
        field_assertions.emplace_back(name, ok);
    if (!ok) { if (failures < 40) std::cerr << "FAIL " << name << '\n'; ++failures; }
}
Bytes text(unsigned tag_number, const std::string& value) { return tlv(tag_number, view(value)); }
Bytes sequence(const std::vector<Field>& fields, unsigned tag_number = 0x30) {
    Bytes body; for (const auto& field : fields) append(body, view(field.value));
    return tlv(tag_number, view(body));
}
Bytes fragment(const Bytes& encoded, bool indefinite, unsigned depth = 1) {
    const auto node = single(view(encoded));
    const bool bits = node.tag.tag_class == BerTagClass::universal && node.tag.number == 3;
    Bytes body;
    if (bits) {
        const auto n = node.value.size;
        const auto split = n > 2 ? (n - 1) / 2 : 0;
        Bytes left{0}; if (split) left.insert(left.end(), node.value.data + 1, node.value.data + 1 + split);
        Bytes right{node.value.data[0]}; right.insert(right.end(), node.value.data + 1 + split, node.value.data + n);
        append(body, view(tlv(3, view(left)))); append(body, view(tlv(3, view(right))));
    } else {
        const auto split = node.value.size / 2;
        append(body, view(tlv(4, {node.value.data, split})));
        append(body, view(tlv(4, {node.value.data + split, node.value.size - split})));
    }
    for (unsigned d = 1; d < depth; ++d) body = tlv(bits ? 0x23 : 0x24, view(body));
    if (!indefinite) return tlv(encoded[0] | 0x20, view(body));
    Bytes result{static_cast<std::uint8_t>(encoded[0] | 0x20), 0x80};
    append(result, view(body)); result.push_back(0); result.push_back(0); return result;
}
Bytes algorithm = tlv(0x30, view(tlv(6, view(oid_value("1.2.3")))));
Bytes certificate = tlv(4, view(Bytes{0x30, 0}));
Bytes timestamp = text(24, "20261023120000Z");
Bytes signature = tlv(0x30, view(join({view(algorithm), view(tlv(4, view(Bytes{1, 2, 3})))})));
std::vector<Field> token1_fields() {
    return {{"sigAlg", algorithm}, {"pkCert", certificate},
        {"certPath", tlv(0x30, view(certificate)), true}, {"version", tlv(3, view(Bytes{7, 0x80})), true},
        {"assoID", text(12, "123")},
        {"dHKey", tlv(0x30, view(join({view(integer(14)), view(tlv(4, view(Bytes{2})))})))},
        {"hmac", algorithm}, {"time", timestamp},
        {"encr-mode", tlv(0xa0, view(tlv(0xa1, view(algorithm))))},
        {"confParams", tlv(3, view(Bytes{7, 0x80}))}, {"attCert", certificate, true}};
}
std::vector<Field> token2_fields(bool iv = true) {
    auto result = std::vector<Field>{{"version", tlv(3, view(Bytes{7, 0x80})), true},
        {"assoID", text(12, "123:456")}, {"time", timestamp}, {"seq", integer(0)}};
    if (iv) result.push_back({"iv", tlv(0x80, view(Bytes(16, 0))), true});
    result.push_back({"rekey", tlv(0x81, view(Bytes{2})), true});
    result.push_back({"reqRekey", tlv(0x82, view(Bytes{1})), true});
    result.push_back({"changedKey", tlv(0x83, view(Bytes{0xff})), true}); return result;
}
std::vector<Field> token3_fields() {
    return {{"sigAlg", algorithm}, {"version", tlv(3, view(Bytes{7, 0x80})), true},
        {"assoID", text(12, "123:456"), true}, {"time", timestamp}, {"pkCert", certificate},
        {"certPath", tlv(0x30, view(certificate)), true}};
}
template<typename T, bool (*Decode)(ByteView, T&, E2eCodecError&, BerLimits) noexcept>
Decoder decoder() {
    return [](const Bytes& value, E2eCodecError& error) { T result; return Decode(view(value), result, error, {}); };
}
void matrix(const std::string& name, const std::vector<Field>& fields, Decoder decode,
            unsigned tag_number = 0x30, bool extensible_tail = false,
            int ambiguous_duplicate_index = -1, bool ambiguous_first_order = false) {
    E2eCodecError error{};
    const auto valid = sequence(fields, tag_number);
    check(decode(valid, error) && error == E2eCodecError::none, name + " valid");
    seeds.push_back({name, valid, decode});
    auto indefinite = valid; const auto node = single(view(valid));
    indefinite = {static_cast<std::uint8_t>(tag_number), 0x80}; append(indefinite, node.value);
    indefinite.push_back(0); indefinite.push_back(0);
    check(decode(indefinite, error), name + " indefinite container");
    for (std::size_t i = 0; i < fields.size(); ++i) {
        auto changed = fields; changed.erase(changed.begin() + i);
        check(decode(sequence(changed, tag_number), error) == fields[i].optional, name + "/" + fields[i].name + " omission");
        changed = fields; changed.insert(changed.begin() + i, fields[i]);
        const bool duplicate_is_extension = (extensible_tail && i + 1 == fields.size()) ||
                                            static_cast<int>(i) == ambiguous_duplicate_index;
        const bool duplicate_ok = decode(sequence(changed, tag_number), error);
        check(duplicate_ok == duplicate_is_extension &&
              (duplicate_ok ? error == E2eCodecError::none : error != E2eCodecError::none),
              name + "/" + fields[i].name + (duplicate_is_extension ? " extension-tag collision" : " duplicate"));
        if (!fields[i].optional) {
            changed = fields; changed[i].value = tlv(5, view(Bytes{}));
            check(!decode(sequence(changed, tag_number), error), name + "/" + fields[i].name + " wrong type");
        }
        if (i + 1 < fields.size() && fields[i].value != fields[i + 1].value) {
            changed = fields; std::swap(changed[i], changed[i + 1]);
            const bool order_ambiguous = ambiguous_first_order && i == 0;
            const bool reordered_ok = decode(sequence(changed, tag_number), error);
            check(reordered_ok == order_ambiguous &&
                  (reordered_ok ? error == E2eCodecError::none : error != E2eCodecError::none),
                  name + "/" + fields[i].name + (order_ambiguous ? " order/extension ambiguity" : " reordered"));
        }
    }
    for (std::size_t end = 0; end < valid.size(); ++end)
        check(!decode(Bytes(valid.begin(), valid.begin() + end), error), name + " truncation " + std::to_string(end));
    for (unsigned b = 0; b < 256; ++b) {
        auto trailing = valid; trailing.push_back(b);
        check(!decode(trailing, error), name + " trailing octet");
    }
}
void codec_matrix() {
    const auto one = decoder<ClearToken1View, decode_clear_token1>();
    const auto two = decoder<ClearToken2View, decode_clear_token2>();
    const auto three = decoder<ClearToken3View, decode_clear_token3>();
    const auto auth = decoder<AuthenticatorView, decode_authenticator>();
    matrix("ClearToken1", token1_fields(), one, 0x30, true, 9);
    matrix("ClearToken2", token2_fields(), two);
    matrix("ClearToken3", token3_fields(), three, 0x30, true, 4);
    matrix("Authenticator", {{"nonce", tlv(4, view(Bytes{1})), true},
           {"algo", algorithm}, {"icv", tlv(4, view(Bytes{2}))}}, auth, 0x30, true, -1, true);
    auto t1 = token1_fields(); auto t3 = token3_fields();
    for (auto kind : {SignedMessageKind::handshake_accept, SignedMessageKind::application_reject,
             SignedMessageKind::handshake_security_reject, SignedMessageKind::handshake_security_abort,
             SignedMessageKind::transfer_security_abort, SignedMessageKind::application_abort,
             SignedMessageKind::release_request, SignedMessageKind::release_response}) {
        const bool accept = kind == SignedMessageKind::handshake_accept;
        const bool app = accept || kind == SignedMessageKind::application_reject || kind == SignedMessageKind::application_abort;
        const bool diagnostic = kind == SignedMessageKind::handshake_security_reject ||
            kind == SignedMessageKind::handshake_security_abort || kind == SignedMessageKind::transfer_security_abort;
        std::vector<Field> body{{"token", sequence(accept ? t1 : t3)}};
        if (app) body.push_back({"applData", tlv(0xa8, view(Bytes{})), kind != SignedMessageKind::application_reject});
        if (diagnostic) body.push_back({"diag", integer(1, 10), true});
        unsigned tag_number = kind == SignedMessageKind::handshake_accept ? 0xa1 :
            kind == SignedMessageKind::application_reject ? 0xa2 : kind == SignedMessageKind::handshake_security_reject ? 0xa3 :
            kind == SignedMessageKind::handshake_security_abort ? 0xa0 : kind == SignedMessageKind::transfer_security_abort ? 0xa1 :
            kind == SignedMessageKind::application_abort ? 0xa2 : 0x30;
        Decoder signed_decode = [kind](const Bytes& b, E2eCodecError& e) { SignedMessageView result; return decode_signed_message(view(b), kind, result, e); };
        const auto signed_body = sequence(body);
        matrix("SignedMessage-" + std::to_string(static_cast<unsigned>(kind)), {{"tbs", signed_body}, {"sign", signature}}, signed_decode, tag_number);
        Decoder body_decode = [=](const Bytes& b, E2eCodecError& e) {
            return signed_decode(tlv(tag_number, view(join({view(b), view(signature)}))), e);
        };
        // Optional open-type applData is indistinguishable from an extension
        // of the same tag. Test required omission and diagnostic duplication
        // explicitly; don't invent a generic open-type duplicate rule.
        E2eCodecError error{};
        for (std::size_t i = 0; i < body.size(); ++i) {
            auto changed = body; changed.erase(changed.begin() + i);
            check(body_decode(sequence(changed), error) == body[i].optional, "signed body omission");
            if (!app && (i == 0 || diagnostic)) {
                changed = body; changed.insert(changed.begin() + i, body[i]);
                check(body_decode(sequence(changed), error), "signed body duplicate-tag extensibility ambiguity");
            }
        }
        auto extensible = body;
        extensible.push_back({"futureSequence", tlv(0x30, view(Bytes{}))});
        check(body_decode(sequence(extensible), error), "signed body extension reuses SEQUENCE tag");
        if (diagnostic) {
            extensible = body;
            extensible.push_back({"futureEnumerated", integer(2, 10)});
            check(body_decode(sequence(extensible), error), "signed body extension reuses ENUMERATED tag");
        }
    }
    const auto init = tlv(0xa8, view(Bytes{}));
    matrix("HandshakeReq", {{"pp", text(12, "IEC 61850-8-1")},
           {"tbs", tlv(0x30, view(join({view(sequence(t1)), view(init)})))}, {"sign", signature}},
           decoder<HandshakeRequestView, decode_handshake_request>());
    Decoder sig_decode = [=](const Bytes& b, E2eCodecError& e) {
        SignedMessageView result;
        return decode_signed_message(view(tlv(0x30, view(join({view(tlv(0x30, view(sequence(t3)))), view(b)})))),
            SignedMessageKind::release_request, result, e);
    };
    matrix("Signature", {{"algo", algorithm}, {"sign", tlv(4, view(Bytes{1}))}}, sig_decode, 0x30, true);
    Decoder dh_decode = [=](const Bytes& b, E2eCodecError& e) { auto fields = t1; fields[5].value = b; return one(sequence(fields), e); };
    matrix("DiffieHellmanSet", {{"groupId", integer(14)}, {"dhPublicKey", tlv(4, view(Bytes{2}))}}, dh_decode, 0x30, true);
    Decoder mode_decode = [=](const Bytes& b, E2eCodecError& e) {
        BerReader reader(view(b)); BerTlvView root; BerError reason{};
        if (!reader.next(root, reason) || !reader.finished()) { e = E2eCodecError::malformed_ber; return false; }
        auto fields = t1; fields[8].value = b; return one(sequence(fields), e);
    };
    matrix("non-aea", {{"encr", tlv(0xa0, view(algorithm)), true}, {"icvAlgID", tlv(0xa1, view(algorithm))}}, mode_decode, 0xa0);
    const auto token = sequence(token2_fields(false));
    const auto tbp = tlv(0x30, view(join({view(token), view(tlv(0xa0, view(Bytes{})))})));
    const auto authenticator = tlv(0x30, view(join({view(algorithm), view(tlv(4, view(Bytes{1})))})));
    matrix("ClearTransfer", {{"tbp", tbp}, {"auth", authenticator}}, decoder<ClearTransferView, decode_clear_transfer>());
    // Every OSI wrapper and its signed choice is a corpus seed too.
    E2eCodecError error{}; Bytes encoded;
    const auto request = sequence({{"pp", text(12, "IEC 61850-8-1")},
        {"tbs", tlv(0x30, view(join({view(sequence(t1)), view(init)})))}, {"sign", signature}});
    check(encode_osi_association_request(5, view(request), encoded, error), "OSI request encoder");
    seeds.push_back({"OSI-AssoReq", encoded, decoder<OsiAssociationRequestView, decode_osi_association_request>()});
    for (const auto& seed : std::vector<Seed>(seeds)) {
        if (seed.name.rfind("SignedMessage-", 0) != 0) continue;
        const unsigned kind = std::stoul(seed.name.substr(14));
        if (kind <= 2) {
            check(encode_osi_association_response(5, view(seed.encoded), encoded, error), "OSI response encoder");
            seeds.push_back({"OSI-AssoRsp", encoded, decoder<OsiAssociationResponseView, decode_osi_association_response>()});
        } else if (kind <= 5) {
            check(encode_osi_association_abort(5, view(seed.encoded), encoded, error), "OSI abort encoder");
            seeds.push_back({"OSI-AssoAbr", encoded, decoder<OsiAbortView, decode_osi_association_abort>()});
        } else if (kind == 6) {
            check(encode_osi_release_request(5, view(seed.encoded), encoded, error), "OSI release request encoder");
            seeds.push_back({"OSI-AssoRelReq", encoded, decoder<OsiReleaseView, decode_osi_release_request>()});
        } else {
            check(encode_osi_release_response(5, view(seed.encoded), encoded, error), "OSI release response encoder");
            seeds.push_back({"OSI-AssoRelRsp", encoded, decoder<OsiReleaseView, decode_osi_release_response>()});
        }
    }
    Bytes choice, presentation;
    check(encode_data_transfer_choice(DataTransferKind::clear, view(sequence({{"tbp", tbp}, {"auth", authenticator}})), choice, error), "DataTransfer choice");
    check(encode_presentation_user_data(5, view(choice), presentation, error), "presentation encoder");
    seeds.push_back({"Presentation", presentation, decoder<PresentationUserDataView, decode_presentation_user_data>()});
}
void string_matrix() {
    E2eCodecError error{};
    const auto algorithm_oid = tlv(6, view(oid_value("1.2.3")));
    for (const auto& parameters : {tlv(5, view(Bytes{})), integer(7), tlv(0x30, view(Bytes{}))}) {
        auto fields = token1_fields();
        fields[0].value = tlv(0x30, view(join({view(algorithm_oid), view(parameters)})));
        fields[6].value = fields[0].value;
        ClearToken1View parsed;
        check(decode_clear_token1(view(sequence(fields)), parsed, error), "AlgorithmIdentifier optional parameter accepted");
    }
    {
        auto fields = token1_fields();
        fields[0].value = tlv(0x30, view(join({view(algorithm_oid), view(tlv(5, view(Bytes{}))),
                                               view(tlv(5, view(Bytes{})))})));
        ClearToken1View parsed;
        check(decode_clear_token1(view(sequence(fields)), parsed, error),
              "AlgorithmIdentifier accepts extension additions");
    }
    for (bool allow_null : {false, true}) {
        const auto oid = oid_value("1.2.3");
        const auto base = tlv(6, view(oid));
        const auto null_value = tlv(5, view(Bytes{}));
        const auto extension = tlv(0x80, view(Bytes{1}));
        check(simple_algorithm_matches(view(tlv(0x30, view(base))), view(oid), allow_null), "simple algorithm absent parameters");
        check(simple_algorithm_matches(view(tlv(0x30, view(join({view(base), view(null_value)})))), view(oid), allow_null) == allow_null,
              "simple algorithm NULL parameter policy");
        check(simple_algorithm_matches(view(tlv(0x30, view(join({view(base), view(extension)})))), view(oid), allow_null),
              "simple algorithm distinguishable extension");
    }
    for (unsigned depth : {1U, 2U, 8U}) for (bool indefinite : {false, true}) {
        auto fields = token1_fields();
        for (unsigned index : {1U, 3U, 4U, 7U, 9U, 10U}) fields[index].value = fragment(fields[index].value, indefinite, depth);
        auto dh = ccli::mms::e2e::wire::fields(view(fields[5].value), 0x30);
        fields[5].value = tlv(0x30, view(join({dh[0].encoded, view(fragment(copy(dh[1].encoded), indefinite, depth))})));
        fields[2].value = tlv(0x30, view(fragment(certificate, indefinite, depth)));
        const auto encoded = sequence(fields); ClearToken1View result;
        check(decode_clear_token1(view(encoded), result, error), "all Token1 strings fragmented");
        check(result.association_id.size == 3 && result.public_key_certificate.size == 2 && result.tls_protected,
              "flattened Token1 logical values");
        auto copied = result; result = {};
        check(copied.association_id.data[0] == '1' && copied.dh.public_key.data[0] == 2, "copied fragmented-value lifetime");
        auto two = token2_fields();
        for (unsigned index : {0U, 1U, 2U, 4U, 5U}) {
            // iv/rekey have implicit OCTET STRING roots.
            two[index].value = fragment(two[index].value, indefinite, depth);
        }
        ClearToken2View result2; const auto encoded2 = sequence(two);
        check(decode_clear_token2(view(encoded2), result2, error) && result2.initialization_vector.size == 16,
              "all Token2 strings including implicit roots fragmented");
    }
    for (auto version : {Bytes{7, 0x80}, Bytes{0, 0x80}, Bytes{0, 0x80, 0}, Bytes{7, 0xff}}) {
        auto fields = token2_fields(); fields[0].value = tlv(3, view(version)); ClearToken2View result;
        check(decode_clear_token2(view(sequence(fields)), result, error), "BER named bits/trailing zero/padding equivalent v1");
    }
    for (unsigned tag_number : {3U, 4U, 12U, 24U}) {
        const auto primitive = tlv(tag_number, view(tag_number == 3 ? Bytes{7, 0x80} : Bytes{1, 2, 3}));
        const auto nested = fragment(primitive, true, 3); const auto node = single(view(nested));
        BerStringView value; BerError reason{}; BerLimits limits;
        check(ber_string_contents(node, tag_number, value, reason), "constructed string utility");
        limits.max_depth = 2;
        check(!ber_string_contents(node, tag_number, value, reason, limits), "string recursion cap");
        const auto wrong_segment = tlv(tag_number | 0x20, view(integer(1)));
        check(!ber_string_contents(single(view(wrong_segment)), tag_number, value, reason), "wrong string segment tag");
    }
    for (const auto& invalid : {Bytes{0x23, 3, 3, 1, 8}, Bytes{0x23, 8, 3, 2, 7, 0x80, 3, 2, 0, 0},
                                Bytes{0x23, 3, 3, 1, 1}, Bytes{0x23, 2, 3, 0}}) {
        BerStringView value; BerError reason{};
        check(!ber_string_contents(single(view(invalid)), 3, value, reason), "invalid BIT STRING segments/unused bits");
    }
}
void ber_boundaries() {
    for (std::size_t n : {0U, 1U, 2U, 126U, 127U, 128U, 129U, 254U, 255U, 256U, 1023U, 1024U, 64000U, 65535U, 65536U, 1048568U}) {
        const auto encoded = tlv(4, view(Bytes(n, 0xa5)));
        BerLimits limits; limits.max_encoded_bytes = encoded.size();
        BerReader reader(view(encoded), limits); BerTlvView value; BerError error{};
        check(reader.next(value, error) && reader.finished() && value.value.size == n, "BER length boundary " + std::to_string(n));
        --limits.max_encoded_bytes; BerReader small(view(encoded), limits);
        check(!small.next(value, error) && error == BerError::input_too_large, "encoded byte limit -1");
        for (unsigned octets = 1; octets <= sizeof(std::size_t); ++octets) {
            if (octets < sizeof(std::size_t) && n >> (8 * octets)) continue;
            Bytes long_form{4, static_cast<std::uint8_t>(0x80 | octets)};
            for (unsigned i = octets; i; --i) long_form.push_back((n >> ((i - 1) * 8)) & 255);
            long_form.resize(long_form.size() + n, 0xa5);
            BerLimits long_limits; long_limits.max_encoded_bytes = long_form.size();
            BerReader long_reader(view(long_form), long_limits);
            check(long_reader.next(value, error) && long_reader.finished() && value.value.size == n, "BER nonminimal length accepted");
        }
    }
    // The default budget counts the entire TLV, not only its contents.
    for (std::size_t size : {1048575U, 1048576U, 1048577U}) {
        const auto encoded = tlv(4, view(Bytes(size - 5, 0xa5)));
        check(encoded.size() == size, "default byte budget fixture size");
        BerReader reader(view(encoded)); BerTlvView value; BerError error{};
        const bool ok = reader.next(value, error);
        check(ok == (size <= 1048576U), "default encoded byte budget N-1/N/N+1");
        check(ok ? error == BerError::none : error == BerError::input_too_large,
              "default encoded byte budget error classification");
    }
    for (unsigned depth : {1U, 2U, 31U, 32U, 33U}) {
        Bytes nested{4, 0}; for (unsigned i = 1; i < depth; ++i) nested = tlv(0x30, view(nested));
        BerReader reader(view(nested)); BerTlvView value; BerError error{};
        check(reader.next(value, error) == (depth <= 32), "BER exact nesting boundary");
    }
    for (unsigned count : {1U, 2U, 4094U, 4095U, 4096U, 4097U}) {
        Bytes body; for (unsigned i = 0; i < count; ++i) append(body, view(Bytes{4, 0}));
        const auto encoded = tlv(0x30, view(body)); BerReader reader(view(encoded)); BerTlvView value; BerError error{};
        check(reader.next(value, error) == (count < 4096), "BER exact element boundary includes root");
    }
    for (auto invalid : {Bytes{0}, Bytes{0, 0}, Bytes{0x20, 0}, Bytes{4, 0x80, 0, 0},
                         Bytes{4, 0xff}, Bytes{4, 0x89}, Bytes{4, 0x82, 1}, Bytes{4, 1},
                         Bytes{0x1f}, Bytes{0x1f, 0x80, 0}, Bytes{0x1f, 0x1e, 0},
                         Bytes{0x1f, 0x90, 0x80, 0x80, 0x80, 0, 0},
                         Bytes{0x30, 0x80}, Bytes{0x30, 2, 0, 0}, Bytes{0x30, 0x80, 0}}) {
        BerReader reader(view(invalid)); BerTlvView value; BerError error{};
        check(!reader.next(value, error) && reader.offset() == 0, "malformed BER rejected without cursor advancement");
    }
    for (std::uint32_t tag_number : {31U, 127U, 128U, 16383U, 0xffffffffU}) {
        Bytes encoded{0x9f}; oid_arc(encoded, tag_number); encoded.push_back(0);
        BerReader reader(view(encoded)); BerTlvView value; BerError error{};
        check(reader.next(value, error) && value.tag.number == tag_number, "high tag boundary");
    }
    const Bytes bad_root{0x30, 0x80, 4, 0, 0, 0}; BerLimits no_indefinite; no_indefinite.allow_indefinite = false;
    BerReader reader(view(bad_root), no_indefinite); BerTlvView value; BerError error{};
    check(!reader.next(value, error) && error == BerError::indefinite_not_allowed, "DER-only policy remains strict");
}
void scalar_fields() {
    E2eCodecError error{};
    auto decode_two = decoder<ClearToken2View, decode_clear_token2>();
    for (const auto& value : {Bytes{}, Bytes{0xff}, Bytes{0x80}, Bytes{0, 1}, Bytes{0, 0},
                             Bytes(10, 0x7f), Bytes{1, 0, 0, 0, 0, 0, 0, 0, 0}}) {
        auto fields = token2_fields(); fields[3].value = tlv(2, view(value));
        check(!decode_two(sequence(fields), error), "sequence signed/minimal/overflow range");
    }
    for (auto value : {std::uint64_t{0}, std::uint64_t{127}, std::uint64_t{128}, ~std::uint64_t{0}}) {
        auto fields = token2_fields(); fields[3].value = integer(value); ClearToken2View result;
        const auto encoded = sequence(fields);
        check(decode_clear_token2(view(encoded), result, error) && result.sequence == value, "sequence uint64 boundary");
    }
    for (auto id : {"", ":", "1:", ":1", "1234", "1:1234", "1:2:3", "-1", " 1", "1 ", "a", "\xc0\xaf"}) {
        auto fields = token2_fields(); fields[1].value = text(12, id);
        check(!decode_two(sequence(fields), error), "invalid association ID grammar/UTF8");
    }
    for (auto id : {"0", "000", "999", "0:0", "123:456", "999:999"}) {
        auto fields = token2_fields(); fields[1].value = text(12, id);
        check(decode_two(sequence(fields), error), "association ID boundaries");
    }
    for (unsigned n : {0U, 1U, 15U, 16U, 17U, 256U}) {
        auto fields = token2_fields(); fields[4].value = tlv(0x80, view(Bytes(n, 0)));
        check(decode_two(sequence(fields), error) == (n == 16), "IV structural size");
    }
    for (auto value : {Bytes{}, Bytes{0}, Bytes{1}, Bytes{0x7f}, Bytes{0x80}, Bytes{0xff}, Bytes{0, 0}}) {
        auto fields = token2_fields(); fields[6].value = tlv(0x82, view(value)); ClearToken2View result;
        const auto encoded = sequence(fields); const bool ok = decode_clear_token2(view(encoded), result, error);
        check(ok == (value.size() == 1), "BOOLEAN exact size");
        if (ok) check(result.request_rekey == (value[0] != 0), "BER BOOLEAN any nonzero true");
    }
    for (auto encoded : {Bytes{0x82, 0x80, 0, 0}, Bytes{0xa2, 3, 1, 1, 0}, Bytes{0x81, 0}}) {
        auto fields = token2_fields(); fields[encoded[0] == 0x81 ? 5 : 6].value = encoded;
        check(!decode_two(sequence(fields), error), "invalid boolean form/empty rekey");
    }
    for (auto oid : {Bytes{}, Bytes{0x80, 0x2a}, Bytes{0x2a, 0x80}, Bytes{0xff}}) {
        auto fields = token1_fields(); fields[0].value = tlv(0x30, view(tlv(6, view(oid)))); ClearToken1View result;
        check(!decode_clear_token1(view(sequence(fields)), result, error), "invalid AlgorithmIdentifier OID");
    }
    auto request_decode = decoder<HandshakeRequestView, decode_handshake_request>();
    const auto tbs = tlv(0x30, view(sequence(token1_fields())));
    for (auto contents : {Bytes{0xc0, 0xaf}, Bytes{0xed, 0xa0, 0x80}, Bytes{0xf4, 0x90, 0x80, 0x80},
                          Bytes{0x80}, Bytes{0xe2, 0x82}, Bytes{0x1f}, Bytes{}}) {
        check(!request_decode(sequence({{"pp", tlv(12, view(contents))}, {"tbs", tbs}, {"sign", signature}}), error), "NMTOKEN invalid UTF8/alphabet");
    }
    check(request_decode(sequence({{"pp", text(12, "IEC-\xe2\x82\xac")}, {"tbs", tbs}, {"sign", signature}}), error), "NMTOKEN valid multibyte UTF8");
    for (auto input : {"20240229120000Z", "20260228120000Z", "2026102312Z", "202610231200Z",
                       "20261023120000.5Z", "20261023130000+0100", "20261023110000-0100"}) {
        std::chrono::system_clock::time_point parsed; GeneralizedTimeError reason{};
        check(parse_generalized_time(view(std::string(input)), parsed, reason), "valid time precision/calendar/offset");
    }
    for (auto input : {"20260229120000Z", "20261301120000Z", "20260100120000Z", "20261023240000Z",
                       "20261023126000Z", "20261023120060Z", "20261023120000", "20261023120000+2400",
                       "20261023120000+0060", "20261023120000.Z", "20261023120000..1Z", "20261023120000xZ"}) {
        std::chrono::system_clock::time_point parsed; GeneralizedTimeError reason{};
        check(!parse_generalized_time(view(std::string(input)), parsed, reason), "invalid time calendar/format/offset");
    }
    // Unknown context-tagged extension remains opaque. Root fields cannot
    // resume after the extension and closed outer containers reject additions.
    auto extension = Bytes{0xbf, 0x1f, 0x80, 4, 0, 0, 0};
    auto one = token1_fields(); one.push_back({"future", extension});
    check(decoder<ClearToken1View, decode_clear_token1>()(sequence(one), error), "Token1 unknown extension");
    auto two = token2_fields(); two.push_back({"future", extension});
    check(decode_two(sequence(two), error), "Token2 unknown extension");
    two.push_back({"lateRoot", tlv(0x82, view(Bytes{0}))});
    check(!decode_two(sequence(two), error), "root after extension rejected");
    auto three = token3_fields(); three.push_back({"future", extension});
    check(decoder<ClearToken3View, decode_clear_token3>()(sequence(three), error), "Token3 unknown extension");
    for (const auto& future : {tlv(3, view(Bytes{7, 0x80})), tlv(4, view(Bytes{1})),
                               tlv(12, view(Bytes{'x'})), tlv(0x30, view(Bytes{})),
                               tlv(24, view(Bytes{'2','0','2','6','1','0','2','3','1','2','0','0','0','0','Z'}))}) {
        auto one_with_collision = token1_fields(); one_with_collision.push_back({"future", future});
        check(decoder<ClearToken1View, decode_clear_token1>()(sequence(one_with_collision), error),
              "Token1 extension with familiar universal tag");
        auto three_with_collision = token3_fields(); three_with_collision.push_back({"future", future});
        check(decoder<ClearToken3View, decode_clear_token3>()(sequence(three_with_collision), error),
              "Token3 extension with familiar universal tag");
    }
}
void mutation_corpus() {
    for (const auto& seed : seeds) {
        const auto mutation_start = mutations;
        const auto accepted_start = accepted_mutations;
        E2eCodecError error{}; check(seed.decode(seed.encoded, error), seed.name + " corpus seed valid");
        for (std::size_t i = 0; i < seed.encoded.size(); ++i) {
            auto mutated = seed.encoded;
            for (unsigned octet = 0; octet < 256; ++octet) {
                mutated[i] = octet; error = E2eCodecError::none;
                const bool accepted = seed.decode(mutated, error); ++mutations; accepted_mutations += accepted;
                check(accepted == (error == E2eCodecError::none), seed.name + " mutation error consistency");
            }
        }
        seed_mutation_evidence.push_back({seed.name, mutations - mutation_start,
            accepted_mutations - accepted_start,
            (mutations - mutation_start) - (accepted_mutations - accepted_start)});
    }
}
void allocation_failures() {
    auto fields = token1_fields();
    for (unsigned index : {1U, 3U, 4U, 7U, 9U, 10U})
        fields[index].value = fragment(fields[index].value, true, 3);
    const auto encoded = sequence(fields);
    bool reached_success = false;
    unsigned injected = 0;
    for (long allocation = 0; allocation < 128; ++allocation) {
        ClearToken1View decoded; E2eCodecError error{};
        bool ok; unsigned failures;
        {
            e2e_test_alloc::Scope fault(allocation);
            ok = decode_clear_token1(view(encoded), decoded, error);
            failures = e2e_test_alloc::failures;
        }
        check(ok == (error == E2eCodecError::none), "allocation failure decoder error consistency");
        if (failures) {
            ++injected;
            check(!ok && error == E2eCodecError::resource_limit_exceeded,
                  "allocation failure maps to resource limit");
        } else { check(ok, "decode recovers after allocation campaign"); reached_success = true; break; }
        decoded = {};
        check(decode_clear_token1(view(encoded), decoded, error), "decode reuse after allocation failure");
    }
    check(injected > 0 && reached_success, "all constructed decode allocation positions exercised");
}
}
int main(int argc, char** argv) {
    codec_matrix(); string_matrix(); ber_boundaries(); scalar_fields(); allocation_failures(); mutation_corpus();
    if (argc > 2) return 2;
    if (argc == 2) {
        std::ofstream evidence(argv[1], std::ios::binary);
        if (!evidence) { std::cerr << "cannot write field assertion ledger\n"; return 2; }
        evidence << "kind\tassertion\tresult\tmutations\taccepted\trejected\n";
        for (const auto& item : field_assertions)
            evidence << "FIELD\t" << item.first << '\t' << (item.second ? "PASS" : "FAIL") << "\t0\t0\t0\n";
        std::map<std::string, unsigned> ordinals;
        for (const auto& seed : seed_mutation_evidence) {
            const auto ordinal = ++ordinals[seed.name];
            evidence << "SEED\t" << seed.name << '#' << ordinal << "\tPASS\t"
                     << seed.mutations << '\t' << seed.accepted << '\t' << seed.rejected << '\n';
        }
        if (!evidence) { std::cerr << "failed writing field assertion ledger\n"; return 2; }
    }
    std::cout << "BER/field checks=" << checks << " seeds=" << seeds.size() << " exhaustive single-octet mutations=" << mutations
              << " accepted=" << accepted_mutations << " rejected=" << mutations - accepted_mutations << " failures=" << failures << '\n';
    return failures ? 1 : 0;
}
