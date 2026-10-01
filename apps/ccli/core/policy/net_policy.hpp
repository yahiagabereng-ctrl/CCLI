#pragma once

#include <cstdint>

namespace cci::core {

enum class EthDomain : std::uint8_t {
    EthA,
    EthB,
    Plant,
    Wan,
};

struct NetPolicy {
    bool allow_eth_a_to_eth_b{false};
    bool allow_eth_b_to_eth_a{false};
    bool allow_wan_to_eth_a{false};
    bool allow_plant_to_eth_a{false};
};

inline NetPolicy lab_segmentation_policy() {
    NetPolicy p{};
    return p;
}

inline bool is_forward_allowed(const NetPolicy& policy, EthDomain from, EthDomain to) {
    if (from == to) {
        return true;
    }
    if (from == EthDomain::EthA && to == EthDomain::EthB) {
        return policy.allow_eth_a_to_eth_b;
    }
    if (from == EthDomain::EthB && to == EthDomain::EthA) {
        return policy.allow_eth_b_to_eth_a;
    }
    if (from == EthDomain::Wan && to == EthDomain::EthA) {
        return policy.allow_wan_to_eth_a;
    }
    if (from == EthDomain::Plant && to == EthDomain::EthA) {
        return policy.allow_plant_to_eth_a;
    }
    return false;
}

}  // namespace cci::core
