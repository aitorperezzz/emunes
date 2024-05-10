#ifndef CPU_STATUS_REGISTER_BIT_H
#define CPU_STATUS_REGISTER_BIT_H

#include <cstdint>

namespace cpu
{
/// @brief Enum for the relevant bits in the status register
enum class StatusRegisterBit : uint8_t
{
    CARRY,     // bit 0: Carry
    ZERO,      // bit 1: Zero
    INTERRUPT, // bit 2: I Interrupt
    DECIMAL,   // bit 3: D Decimal
    BREAK,     // bit 4: B break
    IGNORED,   // bit 5: [ignored]
    OVERFLOW,  // bit 6: V overflow
    NEGATIVE   // bit 7: N Negative
};
} // namespace cpu

#endif
