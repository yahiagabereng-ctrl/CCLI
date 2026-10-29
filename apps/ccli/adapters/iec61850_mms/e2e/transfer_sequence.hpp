#pragma once

#include "ber_codec.hpp"

#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>

namespace ccli::mms::e2e {

enum class TransferSequenceError : std::uint8_t {
    none,
    invalid_policy,
    invalid_timestamp,
    timestamp_outside_window,
    timestamp_regressed,
    sequence_not_zero_at_new_timestamp,
    sequence_not_incremented,
    foreign_candidate,
    stale_candidate,
    generation_exhausted,
};

struct TransferSequencePolicy {
    // IEC profile baseline: reject timestamps more than ten minutes from UTC.
    std::chrono::seconds maximum_clock_skew{std::chrono::minutes(10)};
};

// Per-direction receive state for one MMS association. Authentication must be
// complete before prepare_authenticated(); commit_authenticated() is the only
// operation that advances replay state.
class TransferSequenceWindow {
public:
    class Candidate {
    public:
        Candidate() = default;

    private:
        friend class TransferSequenceWindow;
        const TransferSequenceWindow* owner_{nullptr};
        std::chrono::system_clock::time_point timestamp_{};
        std::uint64_t sequence_{0};
        std::uint64_t generation_{0};
    };

    explicit TransferSequenceWindow(TransferSequencePolicy policy = {}) noexcept;
    TransferSequenceWindow(const TransferSequenceWindow&) = delete;
    TransferSequenceWindow& operator=(const TransferSequenceWindow&) = delete;
    TransferSequenceWindow(TransferSequenceWindow&&) = delete;
    TransferSequenceWindow& operator=(TransferSequenceWindow&&) = delete;

    TransferSequenceError prepare_authenticated(
        std::chrono::system_clock::time_point message_time,
        std::uint64_t sequence,
        std::chrono::system_clock::time_point current_time,
        Candidate& candidate) const noexcept;

    // Parse the wire GeneralizedTime and apply the same skew/replay policy.
    // Call only after the message authenticator has been verified.
    TransferSequenceError prepare_authenticated(
        ByteView encoded_timestamp,
        std::uint64_t sequence,
        std::chrono::system_clock::time_point current_time,
        Candidate& candidate) const noexcept;

    TransferSequenceError commit_authenticated(const Candidate& candidate) noexcept;

    // Reset for a new association. Existing prepared candidates become stale.
    TransferSequenceError reset() noexcept;

    bool has_state() const noexcept;
    std::optional<std::chrono::system_clock::time_point> last_timestamp() const noexcept;
    std::optional<std::uint64_t> last_sequence() const noexcept;

private:
    TransferSequencePolicy policy_;
    mutable std::mutex mutex_;
    std::optional<std::chrono::system_clock::time_point> last_timestamp_;
    std::optional<std::uint64_t> last_sequence_;
    std::uint64_t generation_{0};
};

} // namespace ccli::mms::e2e
