#ifndef P2A_H
#define P2A_H

#include <cassert>  // assert
#include <cstdint>  // uint64_t, uint32_t

// Expands the binary representation of input by a factor of scale.
// e.g., expand(0b1111ull, 3) == 0b001001001001

uint64_t expand(uint64_t input, uint32_t scale)
{
    assert(scale >= 1);

    uint64_t output = 0;

    for (uint32_t i = 0; i < 64; ++i)
    {
        uint64_t bit = (input >> i) & 1;
        uint64_t position = static_cast<uint64_t>(i) * scale;

        if (position < 64)
        {
            output |= bit << position;
        }
    }

    return output;
}

inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z){
    return  expand(x, 3) | (expand(y, 3) << 1) | (expand(z, 3) << 2);
}

#endif
