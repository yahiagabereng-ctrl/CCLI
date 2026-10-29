#pragma once
#include <functional>
#include <memory>
#include <string>

namespace cci::core {
struct EventLogConfig;
/** Bounded asynchronous RFC 5425 sender. No plaintext/UDP fallback. */
class SyslogSender {
public:
    using Failure = std::function<void(const std::string&)>;
    SyslogSender(const EventLogConfig&, Failure);
    ~SyslogSender();
    bool submit(std::string message);
    static bool available();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
} // namespace cci::core
