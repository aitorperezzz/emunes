#ifndef CPU_STATUS_REGISTER_BIT_H
#define CPU_STATUS_REGISTER_BIT_H

#include <cstdint>

namespace cpu
{
    /// @brief Enum for the relevant bits in the status register
    /// bit 7: N Negative
    /// bit 6: V overflow
    /// bit 5: [ignored]
    /// bit 4: B break
    /// bit 3: D Decimal
    /// bit 2: I Interrupt
    /// bit 1: Zero
    /// bit 0: Carry
    enum class StatusRegisterBit : uint8_t
    {
        CARRY,
        ZERO,
        INTERRUPT,
        DECIMAL,
        BREAK,
        IGNORED,
        OVERFLOW,
        NEGATIVE
    };
}

#endif
