#ifndef CPU_ADDRESSING_MODE_H
#define CPU_ADDRESSING_MODE_H

#include <cstdint>

namespace cpu
{
    enum class AddressingMode : uint8_t
    {
        IMPLICIT,
        ACCUMULATOR,
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        RELATIVE,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT,
        INDEXED_INDIRECT,
        INDIRECT_INDEXED
    };
}

#endif