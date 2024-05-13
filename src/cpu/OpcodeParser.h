#ifndef CPU_OPCODE_PARSER_H
#define CPU_OPCODE_PARSER_H

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
    uint8_t raw;                    // Raw value of the opcode
    InstructionId instruction_id;   // The instruction in the opcode
    AddressingMode addressing_mode; // The addressing mode in the opcode
    size_t instruction_size;        // Number of bytes of the complete instruction, including the opcode
    size_t base_cycles;             // Base number of cycles that this instruction consumes
};

/// @brief This model receives a raw byte and returns an opcode
class OpcodeParser
{
  public:
    OpcodeParser();

    /// @brief Return the opcode provided the raw byte
    Opcode parse(const uint8_t raw);

    /// @brief Internal mapping of raw bytes to opcodes
    std::map<uint8_t, Opcode> opcodes;

  private:
    void add(const uint8_t raw, const InstructionId instruction_id, const AddressingMode addressing_mode,
             const size_t instruction_size, const size_t base_cycles);
};
} // namespace cpu

#endif