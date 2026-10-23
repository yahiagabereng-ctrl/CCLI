#pragma once

#include "measurement/pdc_mmxu_phasors.hpp"
#include "modbus_adapter.hpp"

namespace cci::adapters {

void decode_pdc_mmxu_from_regs(const ModbusPollConfig& cfg, const int start,
                               const std::uint16_t* regs, const int reg_count,
                               cci::core::PdcMmxuPhasors& out);

}  // namespace cci::adapters
