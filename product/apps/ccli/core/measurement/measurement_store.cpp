#include "measurement/measurement_store.hpp"

namespace cci::core {

void MeasurementStore::update(const Measurement& m) { latest_ = m; }

Measurement MeasurementStore::snapshot() const { return latest_; }

}  // namespace cci::core
