#include "transfer_sequence.hpp"

#include "e2e_time.hpp"

#include <limits>

namespace ccli::mms::e2e {
namespace {

long double seconds_since_epoch(std::chrono::system_clock::time_point value) noexcept {
    return std::chrono::duration<long double>(value.time_since_epoch()).count();
}

} // namespace

TransferSequenceWindow::TransferSequenceWindow(TransferSequencePolicy policy) noexcept
    : policy_(policy) {}

TransferSequenceError TransferSequenceWindow::prepare_authenticated(
    std::chrono::system_clock::time_point message_time,
    std::uint64_t sequence,
    std::chrono::system_clock::time_point current_time,
    Candidate& candidate) const noexcept {
    candidate = {};
    if (policy_.maximum_clock_skew.count() < 0) {
        return TransferSequenceError::invalid_policy;
    }

    const long double delta = seconds_since_epoch(message_time) - seconds_since_epoch(current_time);
    const long double magnitude = delta < 0 ? -delta : delta;
    if (magnitude > static_cast<long double>(policy_.maximum_clock_skew.count())) {
        return TransferSequenceError::timestamp_outside_window;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!last_timestamp_) {
        if (sequence != 0) {
            return TransferSequenceError::sequence_not_zero_at_new_timestamp;
        }
    } else if (message_time < *last_timestamp_) {
        return TransferSequenceError::timestamp_regressed;
    } else if (message_time == *last_timestamp_) {
        if (!last_sequence_ || *last_sequence_ == std::numeric_limits<std::uint64_t>::max() ||
            sequence != *last_sequence_ + 1) {
            return TransferSequenceError::sequence_not_incremented;
        }
    } else if (sequence != 0) {
        return TransferSequenceError::sequence_not_zero_at_new_timestamp;
    }

    candidate.owner_ = this;
    candidate.timestamp_ = message_time;
    candidate.sequence_ = sequence;
    candidate.generation_ = generation_;
    return TransferSequenceError::none;
}

TransferSequenceError TransferSequenceWindow::prepare_authenticated(
    ByteView encoded_timestamp,
    std::uint64_t sequence,
    std::chrono::system_clock::time_point current_time,
    Candidate& candidate) const noexcept {
    std::chrono::system_clock::time_point message_time{};
    GeneralizedTimeError time_error = GeneralizedTimeError::none;
    if (!parse_generalized_time(encoded_timestamp, message_time, time_error)) {
        candidate = {};
        return TransferSequenceError::invalid_timestamp;
    }
    return prepare_authenticated(message_time, sequence, current_time, candidate);
}

TransferSequenceError TransferSequenceWindow::commit_authenticated(
    const Candidate& candidate) noexcept {
    if (candidate.owner_ != this) {
        return TransferSequenceError::foreign_candidate;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (candidate.generation_ != generation_) {
        return TransferSequenceError::stale_candidate;
    }
    if (generation_ == std::numeric_limits<std::uint64_t>::max()) {
        return TransferSequenceError::generation_exhausted;
    }

    last_timestamp_ = candidate.timestamp_;
    last_sequence_ = candidate.sequence_;
    ++generation_;
    return TransferSequenceError::none;
}

TransferSequenceError TransferSequenceWindow::reset() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (generation_ == std::numeric_limits<std::uint64_t>::max()) {
        return TransferSequenceError::generation_exhausted;
    }
    ++generation_;
    last_timestamp_.reset();
    last_sequence_.reset();
    return TransferSequenceError::none;
}

bool TransferSequenceWindow::has_state() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return last_timestamp_.has_value();
}

std::optional<std::chrono::system_clock::time_point>
TransferSequenceWindow::last_timestamp() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return last_timestamp_;
}

std::optional<std::uint64_t> TransferSequenceWindow::last_sequence() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return last_sequence_;
}

} // namespace ccli::mms::e2e
