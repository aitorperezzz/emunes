#include <stdexcept>

#include "AddressingMode.h"
#include "common/Logging.h"

namespace cpu
{
std::string print_addressing_mode(const AddressingMode addressing_mode)
{
    switch (addressing_mode)
    {
    case AddressingMode::IMP:
        // Implicit
        return "IMP (implicit)";
    case AddressingMode::ACC:
        // Accumulator
        return "ACC (accumulator)";
    case AddressingMode::IMM:
        // Immediate
        return "IMM (immediate)";
    case AddressingMode::ZP0:
        // Zero page
        return "ZP0 (zero page)";
    case AddressingMode::ZPX:
        // Zero page X
        return "ZPX (zero page X)";
    case AddressingMode::ZPY:
        // Zero page Y
        return "ZPY (zero page Y)";
    case AddressingMode::REL:
        // Relative
        return "REL (relative)";
    case AddressingMode::ABS:
        // Absolute
        return "ABS (absolute)";
    case AddressingMode::ABX:
        // Absolute X
        return "ABX (absolute X)";
    case AddressingMode::ABY:
        // Absolute Y
        return "ABY (absolute Y)";
    case AddressingMode::IND:
        // Indirect
        return "IND (indirect)";
    case AddressingMode::IXI:
        // Indexed indirect
        return "IXI (indexed indirect)";
    case AddressingMode::IIX:
        // Indirect indexed
        return "IIX (indirect indexed)";
    default:
        uint8_t addressing_mode_raw = (uint8_t)addressing_mode;
        throw std::out_of_range("Addressing mode not understood: " +
                                common::print_hex(addressing_mode_raw, sizeof(addressing_mode_raw)));
    }
}
} // namespace cpu