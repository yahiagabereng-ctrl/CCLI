#include "measurement/measurement_store.hpp"

namespace cci::core {

void MeasurementStore::update(const Measurement& m) {
    std::lock_guard<std::mutex> lock(mutex_);
    latest_ = m;
}

Measurement MeasurementStore::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_;
}

}  // namespace cci::core
