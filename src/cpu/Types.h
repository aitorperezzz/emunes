#ifndef CPU_TYPES_H
#define CPU_TYPES_H

#include <cstdint>

namespace cpu
{
    /// @brief The size of an instruction is between 1 and 3 bytes
    enum class InstructionSize : uint8_t
    {
        ONE_BYTE,
        TWO_BYTES,
        THREE_BYTES
    };
}

#endif