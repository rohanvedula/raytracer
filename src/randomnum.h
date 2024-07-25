//
// Created by Rohan Vedula on 2024-07-25.
//

#ifndef RANDOMNUM_H
#define RANDOMNUM_H

// fast random number generator based pcg32_fast
#include <stdint.h>
namespace PCG32 {
    static uint64_t mcg_state = 0xcafef00dd15ea5e5u;	// must be odd
    static uint64_t const multiplier = 6364136223846793005u;
    uint32_t pcg32_fast(void) {
        uint64_t x = mcg_state;
        const unsigned count = (unsigned)(x >> 61);
        mcg_state = x * multiplier;
        x ^= x >> 22;
        return (uint32_t)(x >> (22 + count));
    }
    float rand() {
        return float(double(pcg32_fast()) / 4294967296.0);
    }
}

#endif //RANDOMNUM_H
