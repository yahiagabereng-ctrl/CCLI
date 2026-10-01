#include "cci/hal/serial.hpp"

namespace cci::hal {

SerialPort::SerialPort(SerialConfig cfg) : cfg_(std::move(cfg)) {}

SerialPort::~SerialPort() { close(); }

bool SerialPort::open() {
    // Stub: real implementation uses termios on /dev/ttyUSB*
    return false;
}

void SerialPort::close() {
    fd_ = -1;
}

bool SerialPort::is_open() const { return fd_ >= 0; }

int SerialPort::write(const std::uint8_t* data, std::size_t len) {
    (void)data;
    (void)len;
    return -1;
}

int SerialPort::read(std::uint8_t* data, std::size_t len, int timeout_ms) {
    (void)data;
    (void)len;
    (void)timeout_ms;
    return -1;
}

}  // namespace cci::hal
