#include "config/ccli_config.hpp"
#include "event/event_store.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char** argv) {
    if (argc != 3) return 2;
    const auto config = cci::core::load_config(argv[1]);
    const bool expected_failure = std::string(argv[2]) == "fail";
    try {
        cci::core::EventStore store(config.event_log);
        const auto start = std::chrono::steady_clock::now();
        if (!store.append({std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count(), "TLS_HS_SUCCESS", "integration"})) return 3;
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count();
        if (elapsed > 500) { std::cerr << "TLS network operation blocked append\n"; return 4; }
        std::this_thread::sleep_for(std::chrono::seconds(4));
        if (expected_failure == store.syslog_ok()) return 5;
        if (expected_failure) {
            bool alarm = false;
            for (const auto& event : store.recent(2048))
                if (event.iec_id == "IEC62351-14:9.5") alarm = true;
            if (!alarm) return 6;
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 7;
    }
    return 0;
}
