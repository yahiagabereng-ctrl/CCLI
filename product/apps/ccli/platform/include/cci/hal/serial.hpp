#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cci::hal {

struct SerialConfig {
    std::string device;
    int baud{9600};
    char parity{'N'};
    int data_bits{8};
    int stop_bits{1};
};

class SerialPort {
public:
    explicit SerialPort(SerialConfig cfg);
    ~SerialPort();

    bool open();
    void close();
    bool is_open() const;

    int write(const std::uint8_t* data, std::size_t len);
    int read(std::uint8_t* data, std::size_t len, int timeout_ms);

private:
    SerialConfig cfg_;
    int fd_{-1};
};

}  // namespace cci::hal
