#pragma once

#include "config/ccli_config.hpp"

namespace cci::adapters {

/** Apply lab yaml modbus.commissioning to Chronos EMT432 (FC3/6/16). libmodbus ctx connected. */
bool emt432_apply_commissioning(void* libmodbus_ctx, const core::ModbusCommissioningConfig& cfg,
                                bool float_word_swap);

}  // namespace cci::adapters
