#include "e2e_codec.hpp"
#include "transfer_sequence.hpp"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <initializer_list>
#include <string>
#include <vector>

namespace {

using ccli::mms::e2e::ByteView;
using ccli::mms::e2e::ClearToken2Fields;
using ccli::mms::e2e::ClearToken2View;
using ccli::mms::e2e::ClearTransferView;
using ccli::mms::e2e::DataTransferKind;
using ccli::mms::e2e::E2eCodecError;
using ccli::mms::e2e::SignedMessageKind;

int failures = 0;

void expect(bool condition, const char* name) {
    if (!condition) {
        std::cerr << "FAIL: " << name << '\n';
        ++failures;
    }
}

ByteView view(const std::vector<std::uint8_t>& bytes) {
    return {bytes.data(), bytes.size()};
}

template <std::size_t N>
ByteView view(const std::array<std::uint8_t, N>& bytes) {
    return {bytes.data(), bytes.size()};
}

bool equal(ByteView left, ByteView right) {
    if (left.size != right.size || !left.valid() || !right.valid()) {
        return false;
    }
    for (std::size_t i = 0; i < left.size; ++i) {
        if (left.data[i] != right.data[i]) {
            return false;
        }
    }
    return true;
}

std::vector<std::uint8_t> concatenate(
    std::initializer_list<ByteView> parts) {
    std::vector<std::uint8_t> result;
    for (const auto part : parts) {
        if (part.valid()) {
            result.insert(result.end(), part.data, part.data + part.size);
        }
    }
    return result;
}

std::vector<std::uint8_t> tlv(std::uint8_t tag, ByteView contents) {
    std::vector<std::uint8_t> result;
    result.push_back(tag);
    if (contents.size < 128) {
        result.push_back(static_cast<std::uint8_t>(contents.size));
    } else {
        std::array<std::uint8_t, sizeof(std::size_t)> length{};
        std::size_t count = 0;
        for (std::size_t value = contents.size; value != 0; value >>= 8) {
            length[length.size() - 1 - count++] =
                static_cast<std::uint8_t>(value & 0xffu);
        }
        result.push_back(static_cast<std::uint8_t>(0x80u | count));
        result.insert(result.end(), length.end() - static_cast<std::ptrdiff_t>(count),
                      length.end());
    }
    if (contents.size != 0) {
        result.insert(result.end(), contents.data, contents.data + contents.size);
    }
    return result;
}

constexpr std::array<std::uint8_t, 27> kClearToken2 = {
    0x30, 0x19,
    0x0c, 0x03, '1', ':', '2',
    0x18, 0x0f, '2', '0', '2', '6', '1', '0', '2', '1', '0', '0', '0', '0', '0', '0', 'Z',
    0x02, 0x01, 0x00,
};

constexpr std::array<std::uint8_t, 13> kAuthenticator = {
    0x30, 0x0b,
    0x30, 0x04, 0x06, 0x02, 0x2a, 0x03,
    0x04, 0x03, 0xa1, 0xb2, 0xc3,
};

constexpr std::array<std::uint8_t, 6> kAlgorithmIdentifier = {
    0x30, 0x04, 0x06, 0x02, 0x2a, 0x03,
};

constexpr std::array<std::uint8_t, 15> kTimestamp = {
    '2', '0', '2', '6', '1', '0', '2', '1', '0', '0', '0', '0', '0', '0', 'Z',
};

void test_clear_token2_known_vector() {
    ClearToken2View decoded{};
    E2eCodecError error{};
    expect(ccli::mms::e2e::decode_clear_token2(view(kClearToken2), decoded, error),
           "decode independent ClearToken2 vector");
    expect(error == E2eCodecError::none && decoded.sequence == 0 &&
               decoded.association_id.size == 3 &&
               decoded.timestamp.size == kTimestamp.size(),
           "ClearToken2 fields");

    ClearToken2Fields fields{};
    fields.association_id = {kClearToken2.data() + 4, 3};
    fields.timestamp = view(kTimestamp);
    fields.sequence = 0;
    std::vector<std::uint8_t> encoded;
    expect(ccli::mms::e2e::encode_clear_token2(fields, encoded, error),
           "encode ClearToken2");
    expect(equal(view(encoded), view(kClearToken2)),
           "ClearToken2 canonical encoding matches independent vector");
}

void test_clear_token2_rejects_noncanonical_integer() {
    const std::array<std::uint8_t, 28> noncanonical = {
        0x30, 0x1a,
        0x0c, 0x03, '1', ':', '2',
        0x18, 0x0f, '2', '0', '2', '6', '1', '0', '2', '1', '0', '0', '0', '0', '0', '0', 'Z',
        0x02, 0x02, 0x00, 0x01,
    };
    ClearToken2View decoded{};
    E2eCodecError error{};
    expect(!ccli::mms::e2e::decode_clear_token2(view(noncanonical), decoded, error),
           "reject non-minimal non-negative INTEGER");
    expect(error == E2eCodecError::integer_out_of_range,
           "non-minimal INTEGER reports field error");
}

void test_authenticator_vector_and_clear_transfer_slices() {
    ccli::mms::e2e::AuthenticatorView authenticator{};
    E2eCodecError error{};
    expect(ccli::mms::e2e::decode_authenticator(view(kAuthenticator), authenticator, error),
           "decode independent Authenticator vector");
    expect(equal(authenticator.algorithm_identifier, view(kAlgorithmIdentifier)) &&
               authenticator.icv.size == 3 && authenticator.icv.data[0] == 0xa1,
           "Authenticator preserves algorithm and ICV bytes");

    ccli::mms::e2e::AuthenticatorFields auth_fields{};
    auth_fields.algorithm_identifier = view(kAlgorithmIdentifier);
    const std::array<std::uint8_t, 3> icv = {0xa1, 0xb2, 0xc3};
    auth_fields.icv = view(icv);
    std::vector<std::uint8_t> encoded_auth;
    expect(ccli::mms::e2e::encode_authenticator(auth_fields, encoded_auth, error),
           "encode Authenticator");
    expect(equal(view(encoded_auth), view(kAuthenticator)),
           "Authenticator canonical encoding matches independent vector");

    const std::array<std::uint8_t, 2> empty_appl_data = {0x30, 0x00};
    auto tbp_contents = concatenate({view(kClearToken2), view(empty_appl_data)});
    auto expected_tbp = tlv(0x30, view(tbp_contents));
    std::vector<std::uint8_t> encoded_tbp;
    expect(ccli::mms::e2e::encode_tbp(view(kClearToken2), view(empty_appl_data),
                                     encoded_tbp, error),
           "encode TBP");
    expect(equal(view(encoded_tbp), view(expected_tbp)),
           "TBP bytes match independently assembled BER vector");

    auto transfer_contents = concatenate({view(expected_tbp), view(kAuthenticator)});
    auto expected_transfer = tlv(0x30, view(transfer_contents));
    std::vector<std::uint8_t> encoded_transfer;
    expect(ccli::mms::e2e::encode_clear_transfer(view(expected_tbp), view(kAuthenticator),
                                                encoded_transfer, error),
           "encode ClearTransfer");
    expect(equal(view(encoded_transfer), view(expected_transfer)),
           "ClearTransfer bytes match independently assembled BER vector");

    ClearTransferView decoded_transfer{};
    expect(ccli::mms::e2e::decode_clear_transfer(view(expected_transfer),
                                                decoded_transfer, error),
           "decode ClearTransfer vector");
    expect(equal(decoded_transfer.protected_tbp, view(expected_tbp)) &&
               equal(decoded_transfer.icv_input, view(tbp_contents)) &&
               equal(decoded_transfer.application_data, view(empty_appl_data)),
           "ClearTransfer retains authenticated TBP and application bytes");
}

void test_presentation_wrapper_and_transfer_choice() {
    const std::array<std::uint8_t, 2> empty_appl_data = {0x30, 0x00};
    std::vector<std::uint8_t> tbp;
    E2eCodecError error{};
    expect(ccli::mms::e2e::encode_tbp(view(kClearToken2), view(empty_appl_data), tbp, error),
           "prepare presentation TBP");
    std::vector<std::uint8_t> transfer;
    expect(ccli::mms::e2e::encode_clear_transfer(view(tbp), view(kAuthenticator),
                                                transfer, error),
           "prepare presentation ClearTransfer");
    std::vector<std::uint8_t> choice;
    expect(ccli::mms::e2e::encode_data_transfer_choice(DataTransferKind::clear,
                                                       view(transfer), choice, error),
           "encode clear DataTransfer choice");
    std::vector<std::uint8_t> presentation;
    expect(ccli::mms::e2e::encode_presentation_user_data(3, view(choice),
                                                         presentation, error),
           "encode Presentation User-data");

    ccli::mms::e2e::PresentationUserDataView decoded{};
    expect(ccli::mms::e2e::decode_presentation_user_data(view(presentation), decoded, error),
           "decode Presentation User-data");
    expect(decoded.presentation_context_id == 3 &&
               decoded.kind == DataTransferKind::clear &&
               equal(decoded.encoded_transfer, view(choice)),
           "presentation context and transfer choice survive decoding");

    const std::array<std::uint8_t, 2> unsupported_choice = {0xa2, 0x00};
    DataTransferKind kind{};
    ByteView transfer_view{};
    expect(!ccli::mms::e2e::decode_data_transfer(view(unsupported_choice), kind,
                                                transfer_view, error) &&
               error == E2eCodecError::unsupported_transfer_choice,
           "reject unknown DataTransfer choice");
}

void test_signed_release_and_osi_wrapper() {
    const std::array<std::uint8_t, 3> cert = {0x01, 0x02, 0x03};
    const auto time_tlv = tlv(0x18, view(kTimestamp));
    const auto cert_tlv = tlv(0x04, view(cert));
    const auto token_contents = concatenate({view(kAlgorithmIdentifier),
                                              view(time_tlv), view(cert_tlv)});
    const auto token3 = tlv(0x30, view(token_contents));
    const auto tbs = tlv(0x30, view(token3));

    const std::array<std::uint8_t, 2> signature_value = {0xaa, 0xbb};
    const auto signature_octets = tlv(0x04, view(signature_value));
    const auto signature_contents = concatenate({view(kAlgorithmIdentifier),
                                                  view(signature_octets)});
    const auto signature = tlv(0x30, view(signature_contents));
    const auto release_contents = concatenate({view(tbs), view(signature)});
    const auto expected_release = tlv(0x30, view(release_contents));

    E2eCodecError error{};
    std::vector<std::uint8_t> encoded_release;
    expect(ccli::mms::e2e::encode_signed_message(SignedMessageKind::release_response,
                                                 view(tbs), view(kAlgorithmIdentifier),
                                                 view(signature_value), encoded_release,
                                                 error),
           "encode signed release response");
    expect(equal(view(encoded_release), view(expected_release)),
           "signed release response matches independently assembled bytes");

    ccli::mms::e2e::SignedMessageView signed_message{};
    expect(ccli::mms::e2e::decode_signed_message(view(expected_release),
                                                 SignedMessageKind::release_response,
                                                 signed_message, error),
           "decode signed release response vector");
    expect(equal(signed_message.protected_tbs, view(tbs)) &&
               equal(signed_message.signature.value, view(signature_value)),
           "signed TBS and signature bytes are preserved exactly");

    std::vector<std::uint8_t> osi_release;
    expect(ccli::mms::e2e::encode_osi_release_response(5, view(expected_release),
                                                      osi_release, error),
           "encode OSI release response wrapper");
    ccli::mms::e2e::OsiReleaseView decoded_osi{};
    expect(ccli::mms::e2e::decode_osi_release_response(view(osi_release),
                                                      decoded_osi, error),
           "decode OSI release response wrapper");
    expect(decoded_osi.indirect_reference == 5 &&
               equal(decoded_osi.message.encoded, view(expected_release)),
           "OSI release wrapper preserves indirect reference and message");
}

void test_ber_limits_and_transfer_sequence() {
    const std::array<std::uint8_t, 3> small_tlv = {0x04, 0x01, 0x00};
    ccli::mms::e2e::BerLimits limits{};
    limits.max_encoded_bytes = 2;
    ccli::mms::e2e::BerReader limited(view(small_tlv), limits);
    ccli::mms::e2e::BerTlvView parsed{};
    ccli::mms::e2e::BerError ber_error{};
    expect(!limited.next(parsed, ber_error) &&
               ber_error == ccli::mms::e2e::BerError::input_too_large,
           "BER reader enforces configured encoded-byte limit");

    using namespace std::chrono;
    using ccli::mms::e2e::TransferSequenceError;
    using ccli::mms::e2e::TransferSequenceWindow;
    const auto now = system_clock::time_point(seconds(1'800'000'000));
    TransferSequenceWindow sequence;
    TransferSequenceWindow::Candidate candidate{};
    expect(sequence.prepare_authenticated(now, 0, now, candidate) ==
               TransferSequenceError::none &&
               sequence.commit_authenticated(candidate) == TransferSequenceError::none,
           "first transfer starts sequence at zero");
    expect(sequence.prepare_authenticated(now, 1, now, candidate) ==
               TransferSequenceError::none &&
               sequence.commit_authenticated(candidate) == TransferSequenceError::none,
           "same-timestamp transfer increments sequence");
    expect(sequence.prepare_authenticated(now, 1, now, candidate) ==
               TransferSequenceError::sequence_not_incremented,
           "replayed transfer sequence is rejected");
    expect(sequence.prepare_authenticated(now + seconds(1), 0, now, candidate) ==
               TransferSequenceError::none &&
               sequence.commit_authenticated(candidate) == TransferSequenceError::none,
           "new timestamp resets sequence to zero");
    expect(sequence.prepare_authenticated(now, 2, now, candidate) ==
               TransferSequenceError::timestamp_regressed,
           "regressed transfer timestamp is rejected");
}

} // namespace

int main() {
    test_clear_token2_known_vector();
    test_clear_token2_rejects_noncanonical_integer();
    test_authenticator_vector_and_clear_transfer_slices();
    test_presentation_wrapper_and_transfer_choice();
    test_signed_release_and_osi_wrapper();
    test_ber_limits_and_transfer_sequence();
    if (failures != 0) {
        std::cerr << failures << " E2E codec check(s) failed\n";
        return 1;
    }
    std::cout << "E2E codec and transfer-sequence checks passed\n";
    return 0;
}
