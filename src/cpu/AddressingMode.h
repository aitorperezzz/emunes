#ifndef CPU_ADDRESSING_MODE_H
#define CPU_ADDRESSING_MODE_H

#include <cstdint>

namespace cpu
{

/// @brief Possible values of addressing modes of MOS6502
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

/// @brief Return a string with a description of the addressing mode
std::string print_addressing_mode(const AddressingMode addressing_mode);
} // namespace cpu

#endif