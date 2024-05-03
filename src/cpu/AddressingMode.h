#ifndef CPU_ADDRESSING_MODE_H
#define CPU_ADDRESSING_MODE_H

#include <cstdint>

namespace cpu
{
    enum class AddressingMode : uint8_t
    {
        IMP, // Implicit
        ACC, // Accumulator
        IMM, // Immediate
        ZP0, // Zero page
        ZPX, // Zero page X
        ZPY, // Zero page Y
        REL, // Relative
        ABS, // Absolute
        ABX, // Absolute X
        ABY, // Absolute Y
        IND, // Indirect
        IXI, // Indexed indirect
        IIX  // Indirect indexed
    };
}

#endif