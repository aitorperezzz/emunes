#ifndef CPU_OPCODE_H
#define CPU_OPCODE_H

#include <cstdint>
#include <map>

#include "AddressingMode.h"
#include "InstructionId.h"
#include "StatusRegisterBit.h"

namespace cpu
{

/// @brief An opcode after it has been parsed from raw
struct Opcode
{
    InstructionId instruction_id;   // The istruction in the opcode
    AddressingMode addressing_mode; // The addressing mode in the opcode
    std::size_t instruction_size;   // Number of bytes of the complete instruction, including the opcode
    std::size_t base_cycles;        // base number of cycles that this instruction consumes
};

/// @brief This model receives a raw byte and returns an opcode
class OpcodeParser
{
  public:
    OpcodeParser();

    /// @brief Return the opcode provided the waw byte
    Opcode parse(const uint8_t byte);

    /// @brief Internal mapping of raw bytes to opcodes
    std::map<uint8_t, Opcode> opcodes;
};
} // namespace cpu

#endif