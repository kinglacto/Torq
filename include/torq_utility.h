//
// Created by yashas on 6/26/25.
//

#pragma once

#include <utility>
#include <functional>

struct PairHash {
    size_t operator()(std::pair<int,int> const& p) const noexcept {
        // Pack two 32‑bit ints into one 64‑bit value, then hash
        uint64_t key = (uint64_t(uint32_t(p.first)) << 32) | uint32_t(p.second);
        return std::hash<uint64_t>()(key);
    }
};
